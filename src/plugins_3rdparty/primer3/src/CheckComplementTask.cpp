/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "CheckComplementTask.h"

#include "Primer3Task.h"

#include <U2Core/DNASequenceUtils.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

CheckComplementTask::CheckComplementTask(const CheckComplementSettings& _settings, const QList<QSharedPointer<PrimerPair>>& _results, U2SequenceObject* _seqObj) :
    Task(tr("Check complement task"), TaskFlags_FOSCOE),
    settings(_settings),
    results(_results),
    seqObj(_seqObj) {}

void CheckComplementTask::run() {
    SAFE_POINT(settings.enabled, "Check complement is run, but not enabled", );
    CHECK(!seqObj.isNull(), setError(tr("Sequence object has been closed, abort")), );

    for (const auto& pair : qAsConst(results)) {
        auto leftPrimerSequence = getPrimerSequence(pair->getLeftPrimer());
        auto rightPrimerSequence = DNASequenceUtils::reverseComplement(getPrimerSequence(pair->getRightPrimer()));

        QMap<PrimersInDimer, DimerFinderResult> dimers;
        dimers.insert(PrimersInDimer::Left, HeteroDimersFinder(leftPrimerSequence, leftPrimerSequence).getResult());
        dimers.insert(PrimersInDimer::Right, HeteroDimersFinder(rightPrimerSequence, rightPrimerSequence).getResult());
        dimers.insert(PrimersInDimer::Both, HeteroDimersFinder(leftPrimerSequence, rightPrimerSequence).getResult());
        const auto& dimersKeys = dimers.keys();
        QMap<PrimersInDimer, DimerFinderResult> badDimerPrimers;
        for (const auto& primersInDimer : qAsConst(dimersKeys)) {
            const auto& dimer = dimers.value(primersInDimer);
            if (isBasePairNumberBad(dimer) || isGcContentBad(dimer)) {
                badDimerPrimers.insert(primersInDimer, dimer);
            }
        }
        CHECK_CONTINUE(!badDimerPrimers.isEmpty());

        PrimerPairData primerPairData;
        primerPairData.primerPair = pair;
        primerPairData.leftPrimerSequence = leftPrimerSequence;
        primerPairData.rightPrimerSequence = rightPrimerSequence;
        primerPairData.badDimerPrimers = badDimerPrimers;
        filteredPrimers << primerPairData;
    }
}

QString CheckComplementTask::generateReport() const {
    QString res;
    res += QString("<p><strong>%1</strong></p>").arg(tr("Check complement"));
    CHECK_EXT(!filteredPrimers.isEmpty(), res += tr("No primers have been filtered"), res);

    res += QString("<p>%1:</p>").arg(tr("The following primers have been filtered"));

    for (int i = 0; i < filteredPrimers.size(); i++) {
        const auto& primerPairData = filteredPrimers[i];
        res += "<strong>";
        res += QString::number(i + 1);
        res += "</strong>";
        res += "<pre>";
        res += QString("%1:&nbsp;&nbsp;<strong>5'</strong> %2 <strong>3'</strong>").arg(tr("Left")).arg(primerPairData.leftPrimerSequence);
        res += "<br />";
        res += QString("%1:&nbsp;<strong>3'</strong> %2 <strong>5'</strong>").arg(tr("Right")).arg(primerPairData.rightPrimerSequence);
        res += "</pre>";

        auto primersInDimer = primerPairData.badDimerPrimers.keys();
        for (const auto& pid : qAsConst(primersInDimer)) {
            res += "<p>";
            const auto& dimerFinderResult = primerPairData.badDimerPrimers.value(pid);
            switch (pid) {
            case PrimersInDimer::Left:
                res += tr("Left primer self-dimer:");
                break;
            case PrimersInDimer::Right:
                res += tr("Right primer self-dimer:");
                break;
            case PrimersInDimer::Both:
                res += tr("Hetero-dimer:");
                break;
            }

            res += "<br />";
            res += dimerFinderResult.getShortBoldReport();
            if (isBasePairNumberBad(dimerFinderResult)) {
                res += tr(" (max %1 bp)").arg(settings.maxComplementPairs);
            } else if (isGcContentBad(dimerFinderResult)) {
                res += tr(" <b>G/C pairs:</b> %1 bp or %2 % (max %3 %)")
                    .arg(getGAndCNumber(dimerFinderResult.dimer))
                    .arg(getGAndCProportion(dimerFinderResult.dimer) * 100)
                    .arg(settings.maxGcContent);
            }
            res += "<pre>" + dimerFinderResult.dimersOverlap + "</pre>";
            res += "</p>";
        }
    }

    return res;
}

QList<QSharedPointer<PrimerPair>> CheckComplementTask::getFilteredPrimers() const {
    QList<QSharedPointer<PrimerPair>> result;
    for (const auto& primerPairData : qAsConst(filteredPrimers)) {
        result << primerPairData.primerPair;
    }
    return result;
}

QByteArray CheckComplementTask::getPrimerSequence(QSharedPointer<PrimerSingle> primer) const {
    QByteArray primerSequence;
    auto regions = primer->getSequenceRegions(seqObj->getSequenceLength());
    for (const auto& r : qAsConst(regions)) {
        primerSequence += seqObj->getSequenceData(r);
    }

    return primerSequence;
}

bool CheckComplementTask::isBasePairNumberBad(const DimerFinderResult& dimer) const {
    return dimer.baseCounts > settings.maxComplementPairs;
}

bool CheckComplementTask::isGcContentBad(const DimerFinderResult& dimer) const {
    return getGAndCNumber(dimer.dimer) > MINIMUN_G_AND_C_NUMBER_FOR_BAD_DIMER && getGAndCProportion(dimer.dimer) > ((double)settings.maxGcContent / 100);
}

int CheckComplementTask::getGAndCNumber(const QString& dimer) {
    return dimer.count('G') + dimer.count('C');
}

double CheckComplementTask::getGAndCProportion(const QString& dimer) {
    return (double)getGAndCNumber(dimer) / dimer.size();
}


}
