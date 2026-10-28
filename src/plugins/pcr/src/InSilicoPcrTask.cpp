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

#include "InSilicoPcrTask.h"

#include <algorithm>

#include <QDataStream>
#include <QIODevice>

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
    : Task(tr("In Silico PCR"), TaskFlags(TaskFlag_ReportingIsSupported) | TaskFlag_ReportingIsEnabled | TaskFlag_FailOnSubtaskError | TaskFlag_FailOnSubtaskCancel),
      settings(_settings), forwardSearch(nullptr), reverseSearch(nullptr), minProductSize(0) {
    GCOUNTER(cvar, "InSilicoPcrTask");
    minProductSize = qMax(settings->forwardPrimer.length(), settings->reversePrimer.length());
    SAFE_POINT_EXT(settings->temperatureCalculator != nullptr, setError(L10N::nullPointerError("TmCalculator")), );
}

namespace {
int getMaxError(const InSilicoPcrTaskSettings* settings, U2Strand::Direction direction) {
    int res = 0;
    if (direction == U2Strand::Direct) {
        res = qMin((qsizetype)settings->forwardMismatches, settings->forwardPrimer.length() - settings->perfectMatch);
        res = qMin(res, settings->forwardPrimer.length() - 1);
    } else {
        res = qMin((qsizetype)settings->reverseMismatches, settings->reversePrimer.length() - settings->perfectMatch);
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

    result.pattern = direction == U2Strand::Direct ? settings->forwardPrimer : settings->reversePrimer;
    result.maxErr = getMaxError(settings, direction);
    const int ledgeSize = qMax(0, int(result.pattern.size()) - int(settings->perfectMatch) - 1);
    if (!result.searchIsCircular && ledgeSize > 0) {
        // Both primer orientations may have a 5' overhang at a template end.
        // Hits with a 3' end outside the template are rejected during validation.
        const QByteArray ledge(ledgeSize, 'N');
        result.sequence.prepend(ledge);
        result.sequence.append(ledge);
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
    forwardSettings.maxResult2Find = FindAlgorithmSettings::MAX_RESULT_TO_FIND_UNLIMITED;
    reverseSettings.maxResult2Find = FindAlgorithmSettings::MAX_RESULT_TO_FIND_UNLIMITED;
    forwardSearch = new FindAlgorithmTask(forwardSettings);
    reverseSearch = new FindAlgorithmTask(reverseSettings);
    addSubTask(forwardSearch);
    addSubTask(reverseSearch);
}

void InSilicoPcrTask::run() {
    CHECK(!isCanceled(), );
    QList<PrimerBind> forwardDirect, forwardComplementary, reverseDirect, reverseComplementary;
    collectPrimerBinds(forwardSearch, true, forwardDirect, forwardComplementary);
    CHECK(!isCanceled(), );
    collectPrimerBinds(reverseSearch, false, reverseDirect, reverseComplementary);
    CHECK(!isCanceled(), );

    pairPrimerBinds(forwardDirect, reverseComplementary, U2Strand::Direct);
    CHECK(!isCanceled(), );
    pairPrimerBinds(reverseDirect, forwardComplementary, U2Strand::Complementary);
    productKeys.clear();
}

void InSilicoPcrTask::collectPrimerBinds(FindAlgorithmTask* search, bool isForwardPrimer, QList<PrimerBind>& direct, QList<PrimerBind>& complementary) {
    const auto matches = search->popResults();
    algoLog.details(tr("Primer sites found: %1").arg(matches.size()));
    for (const auto& match : matches) {
        CHECK(!isCanceled(), );
        const PrimerBind bind = getPrimerBind(match, isForwardPrimer);
        if (bind.region.isEmpty()) {
            continue;
        }
        if (settings->perfectMatch > 0 && !checkPerfectMatch(bind, match.strand.getDirection())) {
            continue;
        }
        (match.strand.isDirect() ? direct : complementary).append(bind);
    }
}

void InSilicoPcrTask::pairPrimerBinds(QList<PrimerBind>& left, QList<PrimerBind>& right, U2Strand::Direction direction) {
    CHECK(!left.isEmpty() && !right.isEmpty() && !isCanceled(), );
    // For linear templates product length = rightEnd - leftStart, including
    // 5' overhangs. Sorting by these adjusted coordinates allows range queries.
    auto rightEnd = [](const PrimerBind& bind) { return bind.region.endPos() + bind.ledge; };
    std::sort(right.begin(), right.end(), [&](const PrimerBind& a, const PrimerBind& b) {
        return rightEnd(a) < rightEnd(b);
    });
    std::sort(left.begin(), left.end(), [](const PrimerBind& a, const PrimerBind& b) {
        return a.region.startPos - a.ledge < b.region.startPos - b.ledge;
    });
    CHECK(!isCanceled(), );
    for (const auto& leftBind : left) {
        CHECK(!isCanceled(), );
        const qint64 start = leftBind.region.startPos - leftBind.ledge;
        auto visitRange = [&](qint64 low, qint64 high) {
            if (low > high) {
                return;
            }
            auto it = std::lower_bound(right.cbegin(), right.cend(), low, [&](const PrimerBind& bind, qint64 value) {
                return rightEnd(bind) < value;
            });
            for (; it != right.cend() && rightEnd(*it) <= high; ++it) {
                CHECK(!isCanceled(), );
                const qint64 length = getProductSize(leftBind, *it);
                if (isCorrectProductSize(length, minProductSize)) {
                    createAndAddResult(leftBind, U2Region(leftBind.region.startPos, length), *it, direction);
                }
            }
        };
        visitRange(start + minProductSize, start + settings->maxProductSize);
        if (settings->isCircular) {
            // getProductSize wraps negative distances once. Do not revisit
            // nonnegative distances or generate multiple laps of the template.
            const qint64 size = settings->sequence.size();
            visitRange(start + minProductSize - size, qMin(start - 1, start + settings->maxProductSize - size));
        }
    }
}

InSilicoPcrTask::PrimerBind InSilicoPcrTask::getPrimerBind(const FindAlgorithmResult& match, bool isForwardPrimer) const {
    PrimerBind bind;
    bind.primer = isForwardPrimer ? settings->forwardPrimer : settings->reversePrimer;
    bind.region = match.region;
    if (settings->isCircular) {
        return bind;
    }
    const int padding = qMax(0, int(bind.primer.size()) - int(settings->perfectMatch) - 1);
    bind.region.startPos -= padding;
    const qint64 end = bind.region.endPos();
    if (bind.region.startPos < 0) {
        if (!match.strand.isDirect() || end > settings->sequence.size()) {
            bind.region = U2Region();
            return bind;
        }
        bind.ledge = -bind.region.startPos;
        bind.region = U2Region(0, end);
    } else if (end > settings->sequence.size()) {
        if (!match.strand.isComplementary()) {
            bind.region = U2Region();
            return bind;
        }
        bind.ledge = end - settings->sequence.size();
        bind.region.length -= bind.ledge;
    }
    return bind;
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
    if (isCanceled()) {
        return tr("PCR search was canceled. No complete results are available.");
    }
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
    InSilicoPcrProduct result;
    result.region = product;
    result.forwardPrimerMatchLength = leftPrimer.region.length + leftPrimer.ledge;
    result.reversePrimerMatchLength = rightPrimer.region.length + rightPrimer.ledge;
    result.forwardPrimer = settings->forwardPrimer;
    result.reversePrimer = settings->reversePrimer;
    result.forwardPrimerLedge = leftPrimer.primer.left(leftPrimer.ledge);
    result.reversePrimerLedge = rightPrimer.primer.left(rightPrimer.ledge);
    if (U2Strand::Complementary == direction) {
        qSwap(result.forwardPrimer, result.reversePrimer);
    }

    // All fields used by equality except temperature, which is deterministic
    // for the same template, primers and calculator settings within this task.
    QByteArray key;
    QDataStream stream(&key, QIODevice::WriteOnly);
    stream << result.region.startPos << result.region.length
           << result.forwardPrimer << result.reversePrimer
           << result.forwardPrimerMatchLength << result.reversePrimerMatchLength
           << result.forwardPrimerLedge << result.reversePrimerLedge;
    CHECK(!productKeys.contains(key), );

    const U2Region templateRegion(product.startPos, product.length - leftPrimer.ledge - rightPrimer.ledge);
    QByteArray productSequence = getSequence(templateRegion, U2Strand::Direct);
    productSequence.prepend(result.forwardPrimerLedge);
    productSequence.append(DNASequenceUtils::reverseComplement(result.reversePrimerLedge));

    result.ta = settings->temperatureCalculator->getAnnealingTemperature(productSequence,
                                                            direction == U2Strand::Direct ? settings->forwardPrimer : settings->reversePrimer,
                                                            direction == U2Strand::Direct ? settings->reversePrimer : settings->forwardPrimer);
    CHECK(result.isValid(), );
    productKeys.insert(key);
    results.append(result);
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
    // Canceled tasks must never expose a partial list as a complete result.
    static const QList<InSilicoPcrProduct> emptyResults;
    return isCanceled() || hasError() ? emptyResults : results;
}

const InSilicoPcrTaskSettings* InSilicoPcrTask::getSettings() const {
    return settings;
}

InSilicoPcrTask::PrimerBind::PrimerBind() {
    ledge = 0;
}

}  // namespace U2
