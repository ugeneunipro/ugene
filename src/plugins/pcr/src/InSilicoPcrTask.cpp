/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "InSilicoPcrTask.h"

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceUtils.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/L10n.h>
#include <U2Core/Primer.h>
#include <U2Core/PrimerStatistics.h>
#include <U2Core/U2Msa.h>

namespace U2 {

InSilicoPcrProduct::InSilicoPcrProduct()
    : ta(Primer::INVALID_TM),
      forwardPrimerMatchLength(0),
      reversePrimerMatchLength(0) {
}

bool InSilicoPcrProduct::operator==(const InSilicoPcrProduct& anotherProduct) const {
    return region == anotherProduct.region &&
           qFuzzyCompare(ta, anotherProduct.ta) &&
           forwardPrimer == anotherProduct.forwardPrimer &&
           reversePrimer == anotherProduct.reversePrimer &&
           forwardPrimerMatchLength == anotherProduct.forwardPrimerMatchLength &&
           reversePrimerMatchLength == anotherProduct.reversePrimerMatchLength &&
           forwardPrimerLedge == anotherProduct.forwardPrimerLedge &&
           reversePrimerLedge == anotherProduct.reversePrimerLedge;
}

bool InSilicoPcrProduct::isValid() const {
    return ta != Primer::INVALID_TM;
}

InSilicoPcrTask::InSilicoPcrTask(InSilicoPcrTaskSettings* _settings)
    : Task(tr("In Silico PCR"), TaskFlags(TaskFlag_ReportingIsSupported) | TaskFlag_ReportingIsEnabled | TaskFlag_FailOnSubtaskError),
      settings(_settings), forwardSearch(nullptr), reverseSearch(nullptr), minProductSize(0) {
    GCOUNTER(cvar, "InSilicoPcrTask");
    minProductSize = qMax(settings->forwardPrimer.length(), settings->reversePrimer.length());
    SAFE_POINT_EXT(settings->temperatureCalculator != nullptr, setError(L10N::nullPointerError("TmCalculator")), );
}

namespace {
int getMaxError(const InSilicoPcrTaskSettings* settings, U2Strand::Direction direction) {
    int res = 0;
    if (direction == U2Strand::Direct) {
        res = qMin(settings->forwardMismatches, settings->forwardPrimer.length() - settings->perfectMatch);
        res = qMin(res, settings->forwardPrimer.length() - 1);
    } else {
        res = qMin(settings->reverseMismatches, settings->reversePrimer.length() - settings->perfectMatch);
        res = qMin(res, settings->reversePrimer.length() - 1);
    }
    return qMax(0, res);
}

}  // namespace

FindAlgorithmTaskSettings InSilicoPcrTask::getFindPatternSettings(U2Strand::Direction direction) {
    FindAlgorithmTaskSettings result;
    const DNAAlphabet* alphabet = AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
    SAFE_POINT_EXT(alphabet != nullptr, setError(L10N::nullPointerError("DNA Alphabet")), result);
    DNATranslation* translator = AppContext::getDNATranslationRegistry()->lookupComplementTranslation(alphabet);
    SAFE_POINT_EXT(translator != nullptr, setError(L10N::nullPointerError("DNA Translator")), result);

    result.sequence = settings->sequence;
    result.searchIsCircular = settings->isCircular;
    result.searchRegion.length = settings->sequence.length();
    result.patternSettings = FindAlgorithmPatternSettings_Subst;
    result.strand = FindAlgorithmStrand_Both;
    result.useAmbiguousBases = settings->useAmbiguousBases;

    int pos = 0;
    if (U2Strand::Direct == direction) {
        result.pattern = settings->forwardPrimer;
        pos = 0;
    } else {
        result.pattern = settings->reversePrimer;
        pos = result.sequence.size();
    }

    result.maxErr = getMaxError(settings, direction);
    int ledgeSize = result.pattern.size() - settings->perfectMatch - 1;
    if (!result.searchIsCircular && ledgeSize > 0) {
        QByteArray ledge(ledgeSize, 'N');
        result.sequence.insert(pos, ledge);
    }
    result.searchRegion.length = result.sequence.length();
    result.complementTT = translator;

    return result;
}

InSilicoPcrTask::~InSilicoPcrTask() {
    delete settings;
}

void InSilicoPcrTask::prepare() {
    if (!PrimerStatistics::validatePrimerLength(settings->forwardPrimer) || !PrimerStatistics::validatePrimerLength(settings->reversePrimer)) {
        algoLog.details(tr("One of the given do not fits acceptable length. Task cancelled."));
        stateInfo.setCanceled(true);
        return;
    }
    FindAlgorithmTaskSettings forwardSettings = getFindPatternSettings(U2Strand::Direct);
    CHECK_OP(stateInfo, );
    FindAlgorithmTaskSettings reverseSettings = getFindPatternSettings(U2Strand::Complementary);
    CHECK_OP(stateInfo, );
    forwardSettings.maxResult2Find = MAX_RESULTS_FOR_PRIMERS_PER_STRAND;
    reverseSettings.maxResult2Find = MAX_RESULTS_FOR_PRIMERS_PER_STRAND;
    forwardSearch = new FindAlgorithmTask(forwardSettings);
    reverseSearch = new FindAlgorithmTask(reverseSettings);
    addSubTask(forwardSearch);
    addSubTask(reverseSearch);
}

void InSilicoPcrTask::run() {
    QList<FindAlgorithmResult> forwardResults = forwardSearch->popResults();
    QList<FindAlgorithmResult> reverseResults = reverseSearch->popResults();
    algoLog.details(tr("Forward primers found: %1").arg(forwardResults.size()));
    algoLog.details(tr("Reverse primers found: %1").arg(reverseResults.size()));

    for (const FindAlgorithmResult& forward : qAsConst(forwardResults)) {
        for (const FindAlgorithmResult& reverse : qAsConst(reverseResults)) {
            CHECK(!isCanceled(), );
            if (forward.strand == reverse.strand) {
                continue;
            }
            const PrimerBind leftBind = getPrimerBind(forward, reverse, U2Strand::Direct);
            const PrimerBind rightBind = getPrimerBind(forward, reverse, U2Strand::Complementary);

            const qint64 productSize = getProductSize(leftBind, rightBind);
            U2Region productRegion(leftBind.region.startPos, productSize);
            if (isProductAcceptable(leftBind, rightBind, productRegion)) {
                createAndAddResult(leftBind, productRegion, rightBind, forward.strand.getDirection());
            }
        }
    }
}

InSilicoPcrTask::PrimerBind InSilicoPcrTask::getPrimerBind(const FindAlgorithmResult& forward, const FindAlgorithmResult& reverse, U2Strand::Direction direction) const {
    PrimerBind result;
    bool switched = forward.strand.isComplementary();
    if ((U2Strand::Direct == direction && switched) ||
        (U2Strand::Complementary == direction && !switched)) {
        result.primer = settings->reversePrimer;
        result.mismatches = settings->reverseMismatches;
        result.region = reverse.region;
        const qint64 reverseRegionEndPos = reverse.region.endPos();
        const qint64 sequenceSize = settings->sequence.size();
        if (reverseRegionEndPos > sequenceSize) {
            result.region = U2Region(reverse.region.startPos, sequenceSize - reverse.region.startPos);
            result.ledge = reverseRegionEndPos - sequenceSize;
        } else {
            result.region = reverse.region;
            result.ledge = 0;
        }
    } else {
        result.primer = settings->forwardPrimer;
        result.mismatches = settings->forwardMismatches;
        const int ledge = result.primer.size() - settings->perfectMatch - 1;
        if (forward.region.startPos < ledge) {
            result.region = U2Region(0, forward.region.length - forward.region.startPos);
            result.ledge = forward.region.startPos;
        } else {
            result.region = U2Region(forward.region.startPos, forward.region.length);
            if (!settings->isCircular && ledge > 0) {
                result.region.startPos -= (ledge);
            }
            result.ledge = 0;
        }
        result.mismatches += result.ledge;
    }
    return result;
}

bool InSilicoPcrTask::isProductAcceptable(const PrimerBind& leftBind, const PrimerBind& rightBind, const U2Region& productRegion) const {
    CHECK(isCorrectProductSize(productRegion.length, minProductSize), false);

    if (settings->perfectMatch > 0) {
        if (leftBind.mismatches > 0) {
            CHECK(checkPerfectMatch(leftBind, U2Strand::Direct), false);
        }
        if (rightBind.mismatches > 0) {
            CHECK(checkPerfectMatch(rightBind, U2Strand::Complementary), false);
        }
    }
    return true;
}

bool InSilicoPcrTask::isCorrectProductSize(qint64 productSize, qint64 minPrimerSize) const {
    return (productSize >= minPrimerSize) && (productSize <= qint64(settings->maxProductSize));
}

bool InSilicoPcrTask::checkPerfectMatch(const PrimerBind& bind, U2Strand::Direction direction) const {
    const QByteArray sequence = getSequence(bind.region, direction);
    QByteArray croppedPrimer = bind.primer;
    if (bind.ledge > 0) {
        croppedPrimer = bind.primer.right(bind.region.length);
    }
    const int sequenceLength = sequence.length();
    const int primerLength = croppedPrimer.length();
    SAFE_POINT(sequenceLength == primerLength, L10N::internalError("Wrong match length"), false);

    int perfectMatch = qMin(sequence.length(), int(settings->perfectMatch));
    for (int i = 0; i < perfectMatch; i++) {
        char a = sequence.at(sequenceLength - 1 - i);
        char b = croppedPrimer.at(primerLength - 1 - i);
        if (!FindAlgorithm::cmpAmbiguousDna(a, b)) {
            return false;
        }
    }
    return true;
}

QByteArray InSilicoPcrTask::getSequence(const U2Region& region, U2Strand::Direction direction) const {
    QByteArray sequence;
    if (settings->isCircular && region.endPos() > settings->sequence.size()) {
        sequence = settings->sequence.mid(region.startPos, settings->sequence.size() - region.startPos);
        sequence += settings->sequence.mid(0, region.endPos() - settings->sequence.size());
    } else {
        sequence = settings->sequence.mid(region.startPos, region.length);
    }
    if (U2Strand::Complementary == direction) {
        return DNASequenceUtils::reverseComplement(sequence);
    }
    return sequence;
}

QString InSilicoPcrTask::generateReport() const {
    QString spaces;
    for (int i = 0; i < 150; i++) {
        spaces += "&nbsp;";
    }
    if (PrimerStatistics::validate(settings->forwardPrimer) && PrimerStatistics::validate(settings->reversePrimer)) {
        PrimersPairStatistics calc(settings->forwardPrimer, settings->reversePrimer, settings->temperatureCalculator);
        return tr("Products found: %1").arg(results.size()) +
               "<br>" +
               spaces +
               "<br>" +
               tr("Primers details:") +
               calc.generateReport();
    }

    return tr("Products found: %1. <br><br>"
              "The detailed information about primers is not available as primers or sequence contain a character from the Extended DNA alphabet.")
        .arg(results.size());
}

void InSilicoPcrTask::createAndAddResult(const PrimerBind& leftPrimer, const U2Region& product, const PrimerBind& rightPrimer, U2Strand::Direction direction) {
    QByteArray productSequence = settings->sequence.mid(product.startPos, product.length);
    if (productSequence.length() < product.length) {
        if (settings->isCircular) {
            productSequence += settings->sequence.left(product.endPos() - settings->sequence.length());
        } else {
            CHECK(updateSequenceByPrimers(leftPrimer, rightPrimer, productSequence), );
        }
    }

    InSilicoPcrProduct result;
    result.region = product;
    result.ta = settings->temperatureCalculator->getAnnealingTemperature(productSequence,
                                                          direction == U2Strand::Direct ? settings->forwardPrimer : settings->reversePrimer,
                                                          direction == U2Strand::Direct ? settings->reversePrimer : settings->forwardPrimer);
    result.forwardPrimerMatchLength = leftPrimer.region.length + leftPrimer.ledge;
    result.reversePrimerMatchLength = rightPrimer.region.length + rightPrimer.ledge;
    result.forwardPrimer = settings->forwardPrimer;
    result.reversePrimer = settings->reversePrimer;
    result.forwardPrimerLedge = settings->forwardPrimer.left(leftPrimer.ledge);
    result.reversePrimerLedge = settings->reversePrimer.left(rightPrimer.ledge);
    if (U2Strand::Complementary == direction) {
        qSwap(result.forwardPrimer, result.reversePrimer);
    }

    CHECK(result.isValid(), );

    for (const InSilicoPcrProduct& existingProduct : qAsConst(results)) {
        if (existingProduct == result) {
            return;
        }
    }
    results.append(result);
}

bool InSilicoPcrTask::updateSequenceByPrimers(const PrimerBind& leftPrimer, const PrimerBind& rightPrimer, QByteArray& productSequence) const {
    SAFE_POINT((leftPrimer.ledge > 0 || rightPrimer.ledge > 0),
               "Error: at least one primer should has a ledge on one side",
               false);

    updateSequenceByPrimer(leftPrimer, productSequence);
    updateSequenceByPrimer(rightPrimer, productSequence);

    return true;
}

void InSilicoPcrTask::updateSequenceByPrimer(const PrimerBind& primer, QByteArray& productSequence) const {
    if (primer.region.startPos == 0) {
        QByteArray primerPart = primer.primer.left(primer.ledge);
        productSequence.insert(0, primerPart);
    } else {
        QByteArray primerPart = DNASequenceUtils::reverseComplement(primer.primer.left(primer.ledge));
        productSequence.insert(productSequence.size(), primerPart);
    }
}

qint64 InSilicoPcrTask::getProductSize(const PrimerBind& leftBind, const PrimerBind& rightBind) const {
    const qint64 ledge = rightBind.ledge + leftBind.ledge;
    qint64 result = rightBind.region.endPos() - leftBind.region.startPos + ledge;
    if (result < 0 && settings->isCircular) {
        result += settings->sequence.length();
    }
    return result;
}

const QList<InSilicoPcrProduct>& InSilicoPcrTask::getResults() const {
    return results;
}

const InSilicoPcrTaskSettings* InSilicoPcrTask::getSettings() const {
    return settings;
}

InSilicoPcrTask::PrimerBind::PrimerBind() {
    mismatches = 0;
    ledge = 0;
}

}  // namespace U2
