/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
 * http://ugene.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include "InSilicoPcrTests.h"
#include <algorithm>
#include <random>

#include <QApplication>
#include <QDomElement>
#include <QScopeGuard>
#include <QSignalSpy>

#include <U2Algorithm/RoughTmCalculatorCmdFactory.h>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/DNASequenceUtils.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/ProjectService.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SequenceUtils.h>

#include <U2Gui/U2WidgetStateStorage.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>

#include "ExtractProductTask.h"
#include "InSilicoPcrOptionPanelWidget.h"
#include "InSilicoPcrProductsTable.h"
#include "InSilicoPcrTask.h"

namespace U2 {
namespace {

InSilicoPcrTaskSettings settingsForTest() {
    InSilicoPcrTaskSettings s;
    s.forwardPrimer = "ACGTCAGTACGATCGTACGA";
    s.reversePrimer = "TGCACTGACTGACGTAGCTA";
    s.perfectMatch = 15;
    s.maxProductSize = 5000;
    s.temperatureCalculator = RoughTmCalculatorCmdFactory().createCalculator({});
    return s;
}

QList<InSilicoPcrProduct> runPcr(const InSilicoPcrTaskSettings& settings) {
    InSilicoPcrTask task(new InSilicoPcrTaskSettings(settings));
    task.prepare();
    for (auto subtask : task.getSubtasks()) {
        subtask->run();
    }
    task.run();
    return task.getResults();
}

QString checkExtractedProduct(const InSilicoPcrTaskSettings& settings, const U2Region& region, const QByteArray& expected, ExtractProductSettings::AnnotationsExtraction mode = ExtractProductSettings::None, const QMap<QString, U2Region>& sourceAnnotations = {}, const QMap<QString, U2Region>& expectedAnnotations = {}) {
    const auto products = runPcr(settings);
    const auto product = std::find_if(products.cbegin(), products.cend(), [&](const InSilicoPcrProduct& p) {
        return p.region == region;
    });
    if (product == products.cend()) {
        return "Missing product for extraction";
    }

    U2OpStatusImpl os;
    const auto dbiRef = AppContext::getDbiRegistry()->getSessionTmpDbiRef(os);
    CHECK_OP(os, os.getError());
    DNASequence sequence("extraction-template", settings.sequence, AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT()));
    sequence.circular = settings.isCircular;
    const auto sequenceRef = U2SequenceUtils::import(os, dbiRef, sequence);
    CHECK_OP(os, os.getError());
    U2EntityRef annotationsRef;
    auto cleanup = qScopeGuard([&] {
        U2OpStatus2Log cleanupStatus;
        DbiConnection connection(dbiRef, cleanupStatus);
        if (!cleanupStatus.hasError()) {
            connection.dbi->getObjectDbi()->removeObject(sequenceRef.entityId, cleanupStatus);
            if (annotationsRef.isValid()) {
                connection.dbi->getObjectDbi()->removeObject(annotationsRef.entityId, cleanupStatus);
            }
        }
    });

    QScopedPointer<AnnotationTableObject> sourceFeatures;
    if (!sourceAnnotations.isEmpty()) {
        sourceFeatures.reset(new AnnotationTableObject("template-features", dbiRef));
        annotationsRef = sourceFeatures->getEntityRef();
        for (auto it = sourceAnnotations.cbegin(); it != sourceAnnotations.cend(); ++it) {
            SharedAnnotationData data(new AnnotationData);
            data->name = it.key();
            data->location->regions << it.value();
            sourceFeatures->addAnnotations({data});
        }
    }
    ExtractProductSettings extractionSettings;
    extractionSettings.sequenceRef = sequenceRef;
    extractionSettings.targetDbiRef = dbiRef;
    extractionSettings.annotationsExtraction = mode;
    if (annotationsRef.isValid()) {
        extractionSettings.annotationRefs << annotationsRef;
    }
    ExtractProductTask extraction(*product, extractionSettings, {{DocumentRemovalMode_Synchronous, true}});
    extraction.run();
    if (extraction.hasError()) {
        return extraction.getError();
    }
    QScopedPointer<Document, QScopedPointerDeleteLater> document(extraction.takeResult());
    if (document.isNull()) {
        return "No extracted document";
    }
    const auto sequences = document->findGObjectByType(GObjectTypes::SEQUENCE);
    if (sequences.size() != 1) {
        return "Expected one exported sequence";
    }
    const auto exported = qobject_cast<U2SequenceObject*>(sequences.first());
    const QByteArray actual = exported->getWholeSequenceData(os);
    CHECK_OP(os, os.getError());
    if (actual != expected) {
        return QString("Incorrect extracted DNA: expected %1, got %2").arg(QString(expected), QString(actual));
    }
    QMap<QString, U2Region> actualAnnotations;
    const auto annotationTables = document->findGObjectByType(GObjectTypes::ANNOTATION_TABLE);
    for (auto object : annotationTables) {
        const auto table = qobject_cast<AnnotationTableObject*>(object);
        for (auto annotation : table->getAnnotations()) {
            if (sourceAnnotations.contains(annotation->getName())) {
                const auto regions = annotation->getRegions();
                if (regions.size() != 1) {
                    return "Expected one region per exported annotation";
                }
                actualAnnotations[annotation->getName()] = regions.first();
            }
        }
    }
    if (actualAnnotations != expectedAnnotations) {
        auto describe = [](const QMap<QString, U2Region>& annotations) {
            QStringList descriptions;
            for (auto it = annotations.cbegin(); it != annotations.cend(); ++it) {
                descriptions << QString("%1 [%2,%3)").arg(it.key()).arg(it.value().startPos).arg(it.value().endPos());
            }
            return descriptions.join(", ");
        };
        return QString("Incorrect extracted annotations: expected %1; got %2").arg(describe(expectedAnnotations), describe(actualAnnotations));
    }
    return {};
}

bool containsAll(const QList<InSilicoPcrProduct>& actual, const QList<InSilicoPcrProduct>& expected) {
    for (const auto& product : expected) {
        if (!actual.contains(product)) {
            return false;
        }
    }
    return true;
}

struct ReferenceSite {
    QByteArray primer;
    U2Region region;
    int overhang = 0;
    bool direct = true;
};

// Deliberately simple independent oracle: scan every position on both strands,
// count substitutions explicitly, then enumerate every pair. No production
// search, binding normalization, range index or deduplication is used here.
QList<ReferenceSite> referenceSites(const InSilicoPcrTaskSettings& s, const QByteArray& primer, int allowed) {
    QList<ReferenceSite> sites;
    const int n = s.sequence.size();
    const int length = primer.size();
    const int padding = qMax(0, length - int(s.perfectMatch) - 1);
    const int maxErrors = qMax(0, qMin(allowed, qMin(length - int(s.perfectMatch), length - 1)));
    for (bool direct : {true, false}) {
        for (int start = s.isCircular ? 0 : -padding; start <= (s.isCircular ? n - 1 : n + padding - length); ++start) {
            const int end = start + length;
            if (!s.isCircular && ((direct && end > n) || (!direct && start < 0))) {
                continue;
            }
            QByteArray sequence;
            for (int i = start; i < end; ++i) {
                sequence += s.isCircular ? s.sequence[(i + n) % n] : (i < 0 || i >= n ? 'N' : s.sequence[i]);
            }
            if (!direct) {
                sequence = DNASequenceUtils::reverseComplement(sequence);
            }
            int errors = 0;
            bool perfect = true;
            for (int i = 0; i < length; ++i) {
                const bool equal = s.useAmbiguousBases ? FindAlgorithm::cmpAmbiguousDna(sequence[i], primer[i]) : sequence[i] == primer[i];
                errors += !equal;
                if (i >= length - int(s.perfectMatch) && !equal) {
                    perfect = false;
                }
            }
            if (errors > maxErrors || !perfect) {
                continue;
            }
            ReferenceSite site;
            site.primer = primer;
            site.direct = direct;
            const int begin = s.isCircular ? start : qMax(0, start);
            const int stop = s.isCircular ? end : qMin(n, end);
            site.region = U2Region(begin, stop - begin);
            site.overhang = length - site.region.length;
            sites.append(site);
        }
    }
    return sites;
}

QList<InSilicoPcrProduct> exhaustiveProducts(const InSilicoPcrTaskSettings& s) {
    auto forward = referenceSites(s, s.forwardPrimer, s.forwardMismatches);
    auto reverse = referenceSites(s, s.reversePrimer, s.reverseMismatches);
    QList<InSilicoPcrProduct> expected;
    for (const auto& f : forward) {
        for (const auto& r : reverse) {
            if (f.direct == r.direct) {
                continue;
            }
            const auto& left = f.direct ? f : r;
            const auto& right = f.direct ? r : f;
            qint64 length = right.region.endPos() - left.region.startPos + left.overhang + right.overhang;
            if (length < 0 && s.isCircular) {
                length += s.sequence.size();
            }
            if (length < qMax(s.forwardPrimer.size(), s.reversePrimer.size()) || length > s.maxProductSize) {
                continue;
            }
            InSilicoPcrProduct p;
            p.region = U2Region(left.region.startPos, length);
            p.forwardPrimer = left.primer;
            p.reversePrimer = right.primer;
            p.forwardPrimerMatchLength = left.region.length + left.overhang;
            p.reversePrimerMatchLength = right.region.length + right.overhang;
            p.forwardPrimerLedge = left.primer.left(left.overhang);
            p.reversePrimerLedge = right.primer.left(right.overhang);
            QByteArray product;
            const int templateLength = length - left.overhang - right.overhang;
            for (int i = 0; i < templateLength; ++i) {
                const int position = left.region.startPos + i;
                product += s.sequence[s.isCircular ? position % s.sequence.size() : position];
            }
            product = p.forwardPrimerLedge + product + DNASequenceUtils::reverseComplement(p.reversePrimerLedge);
            p.ta = s.temperatureCalculator->getAnnealingTemperature(product, p.forwardPrimer, p.reversePrimer);
            if (p.isValid() && !expected.contains(p)) {
                expected.append(p);
            }
        }
    }
    return expected;
}

class CountingTmCalculator : public TmCalculator {
public:
    CountingTmCalculator()
        : TmCalculator({}), calculator(RoughTmCalculatorCmdFactory().createCalculator({})) {
    }
    double calculateMeltingTemperature(const QByteArray& sequence) override {
        ++calls;
        if (cancelTask != nullptr) {
            cancelTask->cancel();
        }
        return calculator->getMeltingTemperature(sequence);
    }
    QSharedPointer<TmCalculator> calculator;
    int calls = 0;
    Task* cancelTask = nullptr;
};

QString compareExhaustive(const InSilicoPcrTaskSettings& settings) {
    auto actual = runPcr(settings);
    auto expected = exhaustiveProducts(settings);
    if (actual.size() != expected.size() || !containsAll(actual, expected)) {
        return QString("Indexed/exhaustive results differ: %1 / %2").arg(actual.size()).arg(expected.size());
    }
    return {};
}

QString runInSilicoPcrTests(const QString& scenario) {
    auto s = settingsForTest();
    const QByteArray reverseSite = DNASequenceUtils::reverseComplement(s.reversePrimer);
    const QByteArray amplicon = s.forwardPrimer + QByteArray(40, 'C') + reverseSite;
    if (scenario == "mismatch_limit") {
        for (int count : {49, 50, 51, 100}) {
            s.sequence = QByteArray();
            for (int i = 0; i < count; ++i) {
                s.sequence += "ACGTCAGTACGATCGTACGTCCCCC";
            }
            s.sequence += amplicon + QByteArray(30, 'C');
            s.forwardMismatches = s.reverseMismatches = 0;
            auto exact = runPcr(s);
            if (exact.size() != 1 || exact.first().region != U2Region(count * 25, 80)) {
                return "Missing exact baseline product";
            }
            for (int errors : {1, 2, 3}) {
                s.forwardMismatches = s.reverseMismatches = errors;
                if (!containsAll(runPcr(s), exact)) {
                    return QString("Exact product lost with %1 preceding sites and %2 mismatches").arg(count).arg(errors);
                }
            }
        }
        s.sequence = s.forwardPrimer + QByteArray(40, 'C');
        QByteArray nearReverse = s.reversePrimer;
        nearReverse[nearReverse.size() - 1] = 'T';
        for (int i = 0; i < 100; ++i) {
            s.sequence += DNASequenceUtils::reverseComplement(nearReverse) + QByteArray(5, 'C');
        }
        s.sequence += reverseSite + QByteArray(30, 'C');
        s.forwardMismatches = s.reverseMismatches = 0;
        auto exact = runPcr(s);
        s.reverseMismatches = 1;
        if (exact.size() != 1 || !containsAll(runPcr(s), exact)) {
            return "Exact product lost behind inexact reverse-primer sites";
        }
    } else if (scenario == "many_sites") {
        s.maxProductSize = 80;
        for (int i = 0; i < 80; ++i) {
            s.sequence += amplicon + QByteArray(30, 'C');
        }
        auto products = runPcr(s);
        if (products.size() != 80) {
            return QString("Expected 80 products, got %1").arg(products.size());
        }
        for (int i = 0; i < 80; ++i) {
            bool found = false;
            for (const auto& p : products) {
                found |= p.region == U2Region(i * 110, 80);
            }
            if (!found) {
                return "Missing product after the former candidate limit";
            }
        }
    } else if (scenario == "exhaustive") {
        // Fixed seed, both primer orientations, exact and inexact sites,
        // both topology types and product lengths at/beyond the boundaries.
        std::mt19937 random(1969);
        for (int sample = 0; sample < 40; ++sample) {
            s.sequence = QByteArray(20, 'C');
            for (int site = 0; site < 6; ++site) {
                QByteArray primer = (random() % 2) ? s.forwardPrimer : s.reversePrimer;
                if (random() % 3 == 0) {
                    primer[0] = primer[0] == 'A' ? 'C' : 'A';
                }
                s.sequence += (random() % 2) ? primer : DNASequenceUtils::reverseComplement(primer);
                s.sequence += QByteArray(random() % 30 + 1, 'C');
            }
            s.forwardMismatches = s.reverseMismatches = sample % 3;
            s.perfectMatch = (sample % 3 == 0) ? 0 : 15;
            s.isCircular = sample % 2;
            for (uint size : {20u, 21u, 79u, 80u, 81u, 200u, 500u}) {
                s.maxProductSize = size;
                QString error = compareExhaustive(s);
                if (!error.isEmpty()) {
                    return QString("Sample %1, size %2: %3").arg(sample).arg(size).arg(error);
                }
            }
        }
    } else if (scenario == "orientations") {
        for (bool reverse : {false, true}) {
            s.sequence = QByteArray(30, 'C') + (reverse ? DNASequenceUtils::reverseComplement(amplicon) : amplicon) + QByteArray(30, 'C');
            auto products = runPcr(s);
            if (products.size() != 1 || products.first().region != U2Region(30, 80) ||
                products.first().forwardPrimer != (reverse ? s.reversePrimer : s.forwardPrimer)) {
                return "Incorrect product orientation";
            }
            QString error = compareExhaustive(s);
            if (!error.isEmpty()) {
                return error;
            }
        }
    } else if (scenario == "circular") {
        s.isCircular = true;
        const QByteArray circle = amplicon + QByteArray(60, 'C');
        // Move the origin through the product, including both binding sites.
        for (int origin : {0, 1, 10, 25, 60, 70, 79, 100, 137, 138, 139}) {
            s.sequence = circle.mid(origin) + circle.left(origin);
            s.forwardMismatches = s.reverseMismatches = 1;
            auto products = runPcr(s);
            if (products.size() != 1 || products.first().region != U2Region((circle.size() - origin) % circle.size(), 80)) {
                return QString("Lost circular product at rotation %1").arg(origin);
            }
            QString error = compareExhaustive(s);
            if (!error.isEmpty()) {
                return error;
            }
        }
    } else if (scenario == "ambiguous") {
        s.sequence = QByteArray(30, 'C') + amplicon + QByteArray(30, 'C');
        s.forwardPrimer[0] = 'N';
        s.reversePrimer[0] = 'N';
        s.forwardMismatches = s.reverseMismatches = 1;
        for (bool ambiguous : {false, true}) {
            s.useAmbiguousBases = ambiguous;
            auto products = runPcr(s);
            if (products.isEmpty()) {
                return "Missing product with degenerate primers";
            }
            QString error = compareExhaustive(s);
            if (!error.isEmpty()) {
                return error;
            }
        }
    } else if (scenario == "cancellation") {
        s.sequence = amplicon + QByteArray(30, 'C') + amplicon;
        auto calculator = new CountingTmCalculator;
        s.temperatureCalculator.reset(calculator);
        InSilicoPcrTask task(new InSilicoPcrTaskSettings(s));
        task.prepare();
        for (auto subtask : task.getSubtasks()) {
            subtask->run();
        }
        calculator->cancelTask = &task;
        task.run();
        if (!task.isCanceled() || !task.getResults().isEmpty() || task.generateReport() != InSilicoPcrTask::tr("PCR search was canceled. No complete results are available.")) {
            return "Canceled PCR exposes incomplete products";
        }
    } else if (scenario == "duplicates_and_temperature") {
        s.reversePrimer = s.forwardPrimer;
        s.sequence = QByteArray(30, 'C') + s.forwardPrimer + QByteArray(40, 'C') + DNASequenceUtils::reverseComplement(s.forwardPrimer) + QByteArray(30, 'C');
        auto calculator = new CountingTmCalculator;
        s.temperatureCalculator.reset(calculator);
        auto products = runPcr(s);
        if (products.size() != 1 || calculator->calls != 2) {
            return QString("Expected one product and two temperature calculations, got %1 / %2").arg(products.size()).arg(calculator->calls);
        }
        return compareExhaustive(s);
    } else if (scenario == "annotation_extraction") {
        const QByteArray product = s.forwardPrimer + QByteArray("CG").repeated(20) + reverseSite;
        for (int overhang : {0, 1}) {
            const QByteArray prefix(overhang == 0 ? 30 : 0, 'C');
            s.sequence = prefix + product.mid(overhang) + QByteArray(30, 'C');
            const int templateLength = product.size() - overhang;
            const QMap<QString, U2Region> annotations {{"inside", U2Region(prefix.size() + 30, 5)},
                                                       {"crossing", U2Region(prefix.size() + templateLength - 9, 10)},
                                                       {"outside", U2Region(prefix.size() + templateLength, 1)}};
            for (auto mode : {ExtractProductSettings::All, ExtractProductSettings::Inner}) {
                QMap<QString, U2Region> expected {{"inside", U2Region(30 + overhang, 5)}};
                if (mode == ExtractProductSettings::All) {
                    expected["crossing"] = U2Region(71, 9);
                }
                const auto error = checkExtractedProduct(s, U2Region(prefix.size(), 80), product, mode, annotations, expected);
                if (!error.isEmpty()) {
                    return QString("Overhang %1, mode %2: %3").arg(overhang).arg(mode).arg(error);
                }
            }
        }
        s.isCircular = true;
        const QByteArray circle = product + QByteArray(60, 'C');
        s.sequence = circle.mid(40) + circle.left(40);
        const QMap<QString, U2Region> annotations {{"head", U2Region(10, 5)}, {"tail", U2Region(120, 5)}, {"crossing", U2Region(35, 10)}, {"outside", U2Region(60, 5)}};
        for (auto mode : {ExtractProductSettings::All, ExtractProductSettings::Inner}) {
            QMap<QString, U2Region> expected {{"head", U2Region(50, 5)}, {"tail", U2Region(20, 5)}};
            if (mode == ExtractProductSettings::All) {
                expected["crossing"] = U2Region(75, 5);
            }
            const auto error = checkExtractedProduct(s, U2Region(100, 80), product, mode, annotations, expected);
            if (!error.isEmpty()) {
                return QString("Circular annotations, mode %1: %2").arg(mode).arg(error);
            }
        }
    } else if (scenario == "product_extraction") {
        const QByteArray product = s.forwardPrimer + QByteArray("CG").repeated(20) + reverseSite;
        for (bool reverse : {false, true}) {
            const QByteArray oriented = reverse ? DNASequenceUtils::reverseComplement(product) : product;
            for (int leftOverhang : {0, 1, 2, 3}) {
                for (int rightOverhang : {0, 1, 2, 3}) {
                    const QByteArray prefix(leftOverhang == 0 ? 30 : 0, 'C');
                    const QByteArray suffix(rightOverhang == 0 ? 30 : 0, 'C');
                    s.sequence = prefix + oriented.mid(leftOverhang, oriented.size() - leftOverhang - rightOverhang) + suffix;
                    const QString error = checkExtractedProduct(s, U2Region(prefix.size(), oriented.size()), oriented);
                    if (!error.isEmpty()) {
                        return QString("Overhangs %1 / %2, reverse=%3: %4").arg(leftOverhang).arg(rightOverhang).arg(reverse).arg(error);
                    }
                }
            }
            s.isCircular = true;
            const QByteArray circle = oriented + QByteArray(60, 'C');
            for (int origin : {0, 1, 10, 25, 60, 70, 79, 100, 137, 138, 139}) {
                s.sequence = circle.mid(origin) + circle.left(origin);
                const QString error = checkExtractedProduct(s, U2Region((circle.size() - origin) % circle.size(), oriented.size()), oriented);
                if (!error.isEmpty()) {
                    return QString("Circular origin %1, reverse=%2: %3").arg(origin).arg(reverse).arg(error);
                }
            }
            s.isCircular = false;
        }
    } else if (scenario == "overhangs") {
        for (bool reverse : {false, true}) {
            const QByteArray oriented = reverse ? DNASequenceUtils::reverseComplement(amplicon) : amplicon;
            for (int leftOverhang : {0, 1, 2, 3}) {
                for (int rightOverhang : {0, 1, 2, 3}) {
                    s.sequence = oriented.mid(leftOverhang, oriented.size() - leftOverhang - rightOverhang);
                    auto products = runPcr(s);
                    bool found = false;
                    for (const auto& p : products) {
                        found |= p.region == U2Region(0, 80) &&
                                 p.forwardPrimerLedge == (reverse ? s.reversePrimer : s.forwardPrimer).left(leftOverhang) &&
                                 p.reversePrimerLedge == (reverse ? s.forwardPrimer : s.reversePrimer).left(rightOverhang);
                    }
                    if (!found) {
                        return QString("Missing product with %1 / %2-base overhangs, reverse=%3").arg(leftOverhang).arg(rightOverhang).arg(reverse);
                    }
                    QString error = compareExhaustive(s);
                    if (!error.isEmpty()) {
                        return error;
                    }
                }
            }
        }
        // A left overhang with the right binding site inside the template:
        // the product must include the primer overhang, not trailing template bases.
        s.sequence = amplicon.mid(1) + QByteArray(30, 'C');
        return compareExhaustive(s);
    } else {
        return "Unknown PCR test scenario: " + scenario;
    }
    return {};
}

QString runProductsTableTest(const QString& scenario) {
    U2OpStatusImpl os;
    const auto dbiRef = AppContext::getDbiRegistry()->getSessionTmpDbiRef(os);
    CHECK_OP(os, os.getError());
    DNASequence sequence("table-template", QByteArray(scenario == "large" ? 100100 : 100, 'A'), AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT()));
    const auto sequenceRef = U2SequenceUtils::import(os, dbiRef, sequence);
    CHECK_OP(os, os.getError());
    auto cleanup = qScopeGuard([&] {
        U2OpStatus2Log cleanupStatus;
        DbiConnection connection(dbiRef, cleanupStatus);
        if (!cleanupStatus.hasError()) {
            connection.dbi->getObjectDbi()->removeObject(sequenceRef.entityId, cleanupStatus);
        }
    });
    U2SequenceObject sequenceObject("table-template", sequenceRef);
    ADVSequenceObjectContext context(nullptr, &sequenceObject);
    ADVSequenceObjectContext otherContext(nullptr, &sequenceObject);
    InSilicoPcrProductsTable table(nullptr);
    InSilicoPcrProduct product;
    product.region = U2Region(10, 30);
    product.ta = 46.5;
    product.forwardPrimer = QByteArray(20, 'A');
    product.reversePrimer = QByteArray(20, 'T');
    product.forwardPrimerMatchLength = product.reversePrimerMatchLength = 20;
    auto wrapped = product;
    wrapped.region = U2Region(90, 20);
    wrapped.ta = TmCalculator::INVALID_TM;
    auto overhang = product;
    overhang.region = U2Region(0, 105);
    overhang.forwardPrimerLedge = "AAA";
    overhang.reversePrimerLedge = "TT";
    const QList<InSilicoPcrProduct> products {product, wrapped, overhang};
    QSignalSpy changes(&table, &InSilicoPcrProductsTable::si_productsChanged);
    table.showProducts(products, &context);
    auto model = table.model();
    if (scenario == "lifecycle") {
        if (table.onSequenceChanged(&otherContext) || model->rowCount() != 3) {
            return "An unrelated sequence change cleared PCR products";
        }
        if (!table.onSequenceChanged(&context) || model->rowCount() != 0 ||
            !table.getAllProducts().isEmpty() || table.productsContext() != nullptr ||
            table.selectedProductsCount() != 0 || changes.size() != 2) {
            return "Clearing the PCR table retained product data or sequence context";
        }
        {
            auto temporary = products;
            table.showProducts(temporary, &otherContext);
            temporary.clear();
        }
        if (model->rowCount() != 3 || table.getAllProducts() != products ||
            table.getSelectedProducts() != QList<InSilicoPcrProduct> {product} || table.productsContext() != &otherContext) {
            return "Replacing PCR results lost data or the initial selection";
        }
        table.showProducts({}, &otherContext);
        if (model->rowCount() != 0 || !table.getAllProducts().isEmpty() || table.selectedProductsCount() != 0 || changes.size() != 4) {
            return "Empty PCR results retained old rows or selection";
        }
    } else if (scenario == "display") {
        const QList<QStringList> expected {{"11 - 40", "30", "46.5"},
                                           {"91 - 10", "20", InSilicoPcrProductsTable::tr("N/A")},
                                           {"1 - 105", "105", "46.5"}};
        if (model->rowCount() != 3 || model->columnCount() != 3 || table.getAllProducts() != products) {
            return "Incorrect PCR table dimensions or products";
        }
        for (int row = 0; row < 3; ++row) {
            for (int column = 0; column < 3; ++column) {
                const auto index = model->index(row, column);
                if (model->data(index).toString() != expected[row][column] || model->flags(index).testFlag(Qt::ItemIsEditable)) {
                    return QString("Incorrect PCR cell %1 / %2: %3").arg(row).arg(column).arg(model->data(index).toString());
                }
            }
            if (model->headerData(row, Qt::Horizontal).toString().isEmpty()) {
                return "Missing PCR column header";
            }
        }
        if (model->data({}).isValid() || model->data(model->index(0, 0), Qt::EditRole).isValid() ||
            model->rowCount(model->index(0, 0)) != 0) {
            return "PCR table exposed invalid cells or child rows";
        }
    } else if (scenario == "selection") {
        if (table.selectedProductsCount() != 1 || table.getSelectedProducts() != QList<InSilicoPcrProduct> {product} ||
            context.getSequenceSelection()->getSelectedRegions() != QVector<U2Region> {U2Region(10, 30)}) {
            return "Incorrect initial PCR selection";
        }
        auto selection = table.selectionModel();
        selection->select(model->index(1, 0), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
        if (table.getSelectedProducts() != QList<InSilicoPcrProduct> {wrapped} ||
            context.getSequenceSelection()->getSelectedRegions() != QVector<U2Region> {U2Region(90, 10), U2Region(0, 10)}) {
            return "Circular PCR selection did not wrap at the template boundary";
        }
        selection->select(model->index(2, 0), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
        if (context.getSequenceSelection()->getSelectedRegions() != QVector<U2Region> {U2Region(0, 100)}) {
            return "PCR overhangs were included in the template selection";
        }
        selection->select(model->index(0, 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
        const auto selected = table.getSelectedProducts();
        if (table.selectedProductsCount() != 2 || selected.size() != 2 || !selected.contains(product) || !selected.contains(overhang)) {
            return "PCR export selection lost or duplicated products";
        }
        table.clearSelection();
        if (table.selectedProductsCount() != 0 || !table.getSelectedProducts().isEmpty()) {
            return "Clearing the selection retained selected PCR products";
        }
    } else if (scenario == "state") {
        auto clearState = qScopeGuard([] { U2WidgetStateStorage::onWindowClose(nullptr); });
        AnnotatedDNAView view("PCR table state test", {});
        InSilicoPcrOptionPanelWidget panel(&view);
        PcrOptionsPanelSavableTab saved(&panel, nullptr);
        panel.productsTable->showProducts(products, &context);
        if (!saved.getChildIds().contains("productsTable")) {
            return "PCR table is excluded from saved panel state";
        }
        const auto snapshot = saved.getChildValue("productsTable");
        panel.productsTable->clearSelection();
        if (panel.extractProductButton->isEnabled()) {
            return "PCR export button remained enabled without a selection";
        }
        panel.productsTable->showProducts({}, &context);
        saved.setChildValue("productsTable", snapshot);
        if (panel.productsTable->getAllProducts() != products || panel.productsTable->productsContext() != &context ||
            panel.productsTable->selectedProductsCount() != 1 || !panel.extractProductButton->isEnabled()) {
            return "Restoring PCR panel state lost products, context or selection";
        }
        panel.productsTable->selectAll();
        if (!panel.extractProductButton->isEnabled()) {
            return "PCR export button is disabled for multiple selected products";
        }
        panel.productsTable->showProducts({}, &context);
        if (panel.extractProductButton->isEnabled()) {
            return "Empty PCR results left the export button enabled";
        }
    } else if (scenario == "large") {
        QList<InSilicoPcrProduct> manyProducts;
        manyProducts.reserve(100000);
        for (int row = 0; row < 100000; ++row) {
            auto next = product;
            next.region = U2Region(row, 30);
            manyProducts.append(next);
        }
        table.showProducts(manyProducts, &context);
        manyProducts.clear();
        if (model->rowCount() != 100000 || table.getAllProducts().size() != 100000 ||
            model->data(model->index(99999, 0)).toString() != "100000 - 100029") {
            return "Large PCR results were truncated or lost their last row";
        }
        table.selectAll();
        if (table.selectedProductsCount() != 100000) {
            return "Select all did not include all PCR products";
        }
        table.selectionModel()->select(model->index(99999, 0), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
        const auto selected = table.getSelectedProducts();
        if (selected.size() != 1 || selected.first().region != U2Region(99999, 30) ||
            context.getSequenceSelection()->getSelectedRegions() != QVector<U2Region> {U2Region(99999, 30)}) {
            return "Selecting the last PCR row returned another product";
        }
    } else {
        return "Unknown PCR table test scenario: " + scenario;
    }
    return {};
}

}  // namespace

void GTest_InSilicoPcr::init(XMLTestFormat*, const QDomElement& el) {
    scenario = el.attribute("scenario");
}

void GTest_InSilicoPcr::run() {
    QString error = runInSilicoPcrTests(scenario);
    if (!error.isEmpty()) {
        setError(error);
    }
}

void GTest_InSilicoPcrProductsTable::init(XMLTestFormat*, const QDomElement& el) {
    scenario = el.attribute("scenario");
}

void GTest_InSilicoPcrProductsTable::prepare() {
    if (qobject_cast<QApplication*>(QCoreApplication::instance()) == nullptr) {
        setError("PCR table tests require a GUI application");
        return;
    }
    if (scenario == "state" && AppContext::getProject() == nullptr) {
        createdProject = true;
        addSubTask(AppContext::getProjectLoader()->createNewProjectTask());
    }
}

QList<Task*> GTest_InSilicoPcrProductsTable::onSubTaskFinished(Task* subTask) {
    if (subTask->hasError() || subTask->isCanceled()) {
        setError(subTask->hasError() ? subTask->getError() : "PCR table test setup was canceled");
        return {};
    }
    if (createdProject && !checkedState) {
        checkedState = true;
        tableError = runProductsTableTest(scenario);
        return {AppContext::getProjectService()->closeProjectTask()};
    }
    return {};
}

void GTest_InSilicoPcrProductsTable::run() {
    const auto error = checkedState ? tableError : runProductsTableTest(scenario);
    if (!error.isEmpty()) {
        setError(error);
    }
}

}  // namespace U2
