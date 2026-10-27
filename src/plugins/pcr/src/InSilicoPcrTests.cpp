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

#include <random>

#include <QDomElement>

#include <U2Algorithm/RoughTmCalculatorCmdFactory.h>

#include <U2Core/DNASequenceUtils.h>

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
    CountingTmCalculator() : TmCalculator({}), calculator(RoughTmCalculatorCmdFactory().createCalculator({})) {
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

}  // namespace U2
