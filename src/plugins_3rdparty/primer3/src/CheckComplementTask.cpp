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
    Task(tr("CHeck complement task"), TaskFlags_FOSCOE),
    settings(_settings),
    results(_results),
    seqObj(_seqObj) {}

void CheckComplementTask::run() {
    SAFE_POINT(settings.enabled, "Check complement is run, but not enabled", );

    for (const auto& pair : qAsConst(results)) {
        auto leftPrimerSequence = getPrimerSequence(pair->getLeftPrimer());
        auto rightPrimerSequence = DNASequenceUtils::reverseComplement(getPrimerSequence(pair->getRightPrimer()));

        QMap<PrimersInDimer, DimerFinderResult> dimers;
        dimers.insert(PrimersInDimer::Left, SelfDimersFinder(leftPrimerSequence).getResult());
        dimers.insert(PrimersInDimer::Right, SelfDimersFinder(rightPrimerSequence).getResult());
        dimers.insert(PrimersInDimer::Both, HeteroDimersFinder(leftPrimerSequence, rightPrimerSequence).getResult());
        const auto& dimersKeys = dimers.keys();
        for (const auto& primersInDimer : qAsConst(dimersKeys)) {
            const auto& dimer = dimers.value(primersInDimer);
            if (dimer.baseCounts >= settings.maxComplementPairs) {
                addFilterdPrimer(pair, primersInDimer, dimer);
            } else if (getGAndCNumber(dimer.dimer) >= settings.maxGcPair) {
                addFilterdPrimer(pair, primersInDimer, dimer);
            }
        }
        //auto leftSelfDimerResult = ;
        /*if (leftSelfDimerResult.baseCounts >= settings.maxComplementPairs) {
            addFilterdPrimer(pair, leftSelfDimerResult);
        } else if (getGAndCNumber(leftSelfDimerResult.dimer) >= settings.maxGcPair) {
            addFilterdPrimer(pair, leftSelfDimerResult);
        }
        auto rightSelfDimerResult = SelfDimersFinder(rightPrimerSequence).getResult();
        auto heteroDimerResult = HeteroDimersFinder(leftPrimerSequence, rightPrimerSequence).getResult();*/

        int i = 0;
    }

}

QString CheckComplementTask::generateReport() const {
    return "TEST";
}

QList<QSharedPointer<PrimerPair>> CheckComplementTask::getFilteredPrimers() const {
    /*QList<PrimerPair> result;
    for (const auto& filteredPrimerData : qAsConst(filteredPrimers)) {
        result << filteredPrimerData.pair;
    }*/
    return filteredPrimers.keys();
}

QByteArray CheckComplementTask::getPrimerSequence(QSharedPointer<PrimerSingle> primer) const {
    QByteArray primerSequence;
    auto regions = primer->getSequenceRegions(seqObj->getSequenceLength());
    for (const auto& r : qAsConst(regions)) {
        primerSequence += seqObj->getSequenceData(r);
    }

    return primerSequence;
}

void CheckComplementTask::addFilterdPrimer(const QSharedPointer<PrimerPair>& pair, PrimersInDimer primersInDimer, const DimerFinderResult& dimer) {
    auto dimerFinderResultList = filteredPrimers.value(pair);
    dimerFinderResultList.insert(primersInDimer, dimer);
    filteredPrimers.insert(pair, dimerFinderResultList);
}

int CheckComplementTask::getGAndCNumber(const QString& dimer) {
    return dimer.count('G') + dimer.count('C');
}


}
