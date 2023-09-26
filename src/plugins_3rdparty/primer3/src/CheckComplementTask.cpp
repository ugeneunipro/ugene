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
    CHECK_EXT(!seqObj.isNull(), setError(tr("Sequence object has been closed, abort")), );

    for (const auto& pair : qAsConst(results)) {
        auto leftPrimerSequence = getPrimerSequence(pair->getLeftPrimer());
        auto rightPrimerSequence = DNASequenceUtils::reverseComplement(getPrimerSequence(pair->getRightPrimer()));

        PrimerPairData primerPairData;
        primerPairData.primerPair = pair;
        primerPairData.leftPrimerSequence = leftPrimerSequence;
        primerPairData.rightPrimerSequence = rightPrimerSequence;

        primerPairData.leftPrimerSelfDimer = HeteroDimersFinder(leftPrimerSequence, leftPrimerSequence).getResult();
        bool leftDimerFiltered = isBasePairNumberBad(primerPairData.leftPrimerSelfDimer) || isGcContentBad(primerPairData.leftPrimerSelfDimer);
        primerPairData.rightPrimerSelfDimer = HeteroDimersFinder(rightPrimerSequence, rightPrimerSequence).getResult();
        bool rightDimerFiltered = isBasePairNumberBad(primerPairData.rightPrimerSelfDimer) || isGcContentBad(primerPairData.rightPrimerSelfDimer);
        primerPairData.heteroDimer = HeteroDimersFinder(leftPrimerSequence, rightPrimerSequence).getResult();
        bool heteroDimerFiltered = isBasePairNumberBad(primerPairData.heteroDimer) || isGcContentBad(primerPairData.heteroDimer);
        primerPairData.filtered = leftDimerFiltered || rightDimerFiltered || heteroDimerFiltered;

        primers << primerPairData;
    }
}

QString CheckComplementTask::generateReport() const {
    QString res;

    res += "<br><br>" + QString("<strong>%1</strong>").arg(tr("Check complement")) + "<br><br>";
    res += QString("%1:").arg(tr("The following filtering settings have been used"));
    if (settings.enableMaxComplementPairs) {
        res += "<br>" + QString("<strong>%1</strong> = %2 %3").arg(tr("Max base pairs in dimers")).arg(settings.maxComplementPairs).arg(tr("bp"));
    }
    if (settings.enableMaxGcContent) {
        res += "<br>" + QString("<strong>%1</strong> = %2 %").arg(tr("Max dimer GC-content")).arg(settings.maxGcContent);
    }
    res += "<br><br>" + QString("%1:").arg(tr("The following primers have been found and processed (red - filtered, green - passed)")) + "<br><br>";

    res += "<table style=\"border-collapse: collapse; vertical-align: middle;\" border=\"1\">";

    res += "<tr>";
    res += QString("<td><pre>%1</pre></td>").arg(tr("No."));
    res += QString("<td><pre>%1</pre></td>").arg(tr("Strand"));
    res += QString("<td><pre>%1</pre></td>").arg(tr("Primer"));
    res += QString("<td><pre>%1</pre></td>").arg(tr("Self-dimer"));
    res += QString("<td><pre>%1</pre></td>").arg(tr("Delta G (kcal/mol)"));
    res += QString("<td><pre>%1</pre></td>").arg(tr("Base Pairs (bp)"));
    res += QString("<td><pre>%1</pre></td>").arg(tr("G/C pairs (bp)"));
    res += QString("<td><pre>%1</pre></td>").arg(tr("G/C-content (%)"));
    res += QString("<td><pre>%1</pre></td>").arg(tr("Hetero-dimer"));
    res += QString("<td><pre>%1</pre></td>").arg(tr("Delta G (kcal/mol)"));
    res += QString("<td><pre>%1</pre></td>").arg(tr("Base Pairs (bp)"));
    res += QString("<td><pre>%1</pre></td>").arg(tr("G/C pairs (bp)"));
    res += QString("<td><pre>%1</pre></td>").arg(tr("G/C-content (%)"));
    res += "</tr>";

    for (int i = 0; i < primers.size(); i++) {
        const auto& primerData = primers[i];
        res += QString("<tr bgcolor=\"%1\">").arg(primerData.filtered ? FILTERED_PRIMER_PAIR_COLOR : CORRECT_PRIMER_PAIR_COLOR);
        res += QString("<td rowspan=\"2\"><pre>%1</pre></td>").arg(i + 1);
        res += QString("<td><pre>%1</pre></td>").arg(tr("Forward"));
        res += QString("<td><pre> %1 </pre></td>").arg(primerData.leftPrimerSequence);
        res += QString("<td><pre>%1</pre></td>").arg(primerData.leftPrimerSelfDimer.dimersOverlap);
        res += QString("<td>%1</td>").arg(primerData.leftPrimerSelfDimer.deltaG);
        res += QString("<td>%1</td>").arg(getGcCountString(primerData.leftPrimerSelfDimer));
        res += QString("<td>%1</td>").arg(getGcCount(primerData.leftPrimerSelfDimer.dimer));
        res += QString("<td>%1</td>").arg(getGcContentString(primerData.leftPrimerSelfDimer));

        res += QString("<td rowspan=\"2\"><pre>%1</pre></td>").arg(primerData.heteroDimer.dimersOverlap);
        res += QString("<td rowspan=\"2\">%1</td>").arg(primerData.heteroDimer.deltaG);
        res += QString("<td rowspan=\"2\">%1</td>").arg(getGcCountString(primerData.heteroDimer));
        res += QString("<td rowspan=\"2\">%1</td>").arg(getGcCount(primerData.heteroDimer.dimer));
        res += QString("<td rowspan=\"2\">%1</td>").arg(getGcContentString(primerData.heteroDimer));
        res += "</tr>";

        res += QString("<tr bgcolor=\"%1\">").arg(primerData.filtered ? FILTERED_PRIMER_PAIR_COLOR : CORRECT_PRIMER_PAIR_COLOR);
        res += QString("<td><pre>%1</pre></td>").arg(tr("Reverse"));
        res += QString("<td><pre> %1 </pre></td>").arg(primerData.rightPrimerSequence);
        res += QString("<td><pre>%1</pre></td>").arg(primerData.rightPrimerSelfDimer.dimersOverlap);
        res += QString("<td>%1</td>").arg(primerData.rightPrimerSelfDimer.deltaG);
        res += QString("<td>%1</td>").arg(getGcCountString(primerData.rightPrimerSelfDimer));
        res += QString("<td>%1</td>").arg(getGcCount(primerData.rightPrimerSelfDimer.dimer));
        res += QString("<td>%1</td>").arg(getGcContentString(primerData.rightPrimerSelfDimer));
        res += "</tr>";
    }

    res += "</table>";

    return res;
}

QList<QSharedPointer<PrimerPair>> CheckComplementTask::getFilteredPrimers() const {
    QList<QSharedPointer<PrimerPair>> result;
    for (const auto& primerPairData : qAsConst(primers)) {
        CHECK_CONTINUE(primerPairData.filtered);

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
    CHECK(settings.enableMaxComplementPairs, false);

    return dimer.baseCounts > settings.maxComplementPairs;
}

bool CheckComplementTask::isGcContentBad(const DimerFinderResult& dimer) const {
    CHECK(settings.enableMaxGcContent, false);

    return getGcCount(dimer.dimer) > MINIMUN_G_AND_C_NUMBER_FOR_BAD_DIMER && getGcContent(dimer.dimer) > ((double)settings.maxGcContent / 100);
}

QString CheckComplementTask::getGcCountString(const DimerFinderResult& dimer) const {
    QString res = QString::number(dimer.baseCounts);
    if (isBasePairNumberBad(dimer)) {
        res = "<strong>" + res + "</strong>";
    }

    return res;
}

QString CheckComplementTask::getGcContentString(const DimerFinderResult& dimer) const {
    int gcContent = getGcContent(dimer.dimer) * 100;
    QString res = QString::number(gcContent);
    if (isGcContentBad(dimer)) {
        res = "<strong>" + res + "</strong>";
    }

    return res;
}

int CheckComplementTask::getGcCount(const QString& dimer) {
    return dimer.count('G') + dimer.count('C');
}

double CheckComplementTask::getGcContent(const QString& dimer) {
    return (double)getGcCount(dimer) / dimer.size();
}


}
