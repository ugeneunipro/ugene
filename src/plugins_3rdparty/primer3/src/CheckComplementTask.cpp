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

    /*res += QString("Structural alignment") + "<br><br>";
    res += QString("<b>RMSD</b> = %1").arg("result.rmsd");*/
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
    res += "<td><pre>No.</pre></td>";
    res += "<td><pre>Strand</pre></td>";
    res += "<td><pre>Primer</pre></td>";
    res += "<td><pre>Self-dimer</pre></td>";
    res += "<td><pre>Delta G (kcal/mol)</pre></td>";
    res += "<td><pre>Base Pairs (bp)</pre></td>";
    res += "<td><pre>G/C pairs (bp)</pre></td>";
    res += "<td><pre>G/C pairs (%)</pre></td>";
    res += "<td><pre>Hetero-dimer</pre></td>";
    res += "<td><pre>Delta G (kcal/mol)</pre></td>";
    res += "<td><pre>Base Pairs (bp)</pre></td>";
    res += "<td><pre>G/C pairs (bp)</pre></td>";
    res += "<td><pre>G/C pairs (%)</pre></td>";
    res += "</tr>";

    // row 1
    res += "<tr bgcolor=\"Salmon\">";
    res += "<td rowspan=\"2\"><pre>1</pre></td>";
    res += "<td><pre>Forward</pre></td>";
    res += "<td><pre> TGCAACTGGCTCTAGGGACTTGCAACTGGCTCTAGGGACT </pre></td>";
    res += "<td><pre>    TGCAACTGGCTCTAGGGACTTGCAACTGGCTCTAGGGACT\n<span style=\"color: red; \">         ||||       </span>\nCGTGCTTCAGACCTCTGGAG</pre></td>";
    res += "<td>-6.7</td>";
    res += "<td>4</td>";
    res += "<td>3</td>";
    res += "<td>75</td>";
    res += "<td rowspan=\"2\"><pre>    TGCAACTGGTGCAACTGGCTCTAGGGACTCTCTAGGGACT\n<span style=\"color: red; \">         ||||       </span>\nCGTGCTTCAGACCTCTGGAG</pre></td>";
    res += "<td rowspan=\"2\">-6.7</td>";
    res += "<td rowspan=\"2\">4</td>";
    res += "<td rowspan=\"2\">3</td>";
    res += "<td rowspan=\"2\">75</td>";
    res += "</tr>";

    res += "<tr bgcolor=\"Salmon\">";
    res += "<td><pre>Reverse</pre></td>";
    res += "<td><pre> TGCAACTGGCTCTAGGGACTTGCAACTGGCTCTAGGGACT </pre></td>";
    res += "<td><pre>    AAAATGCAACTGGCTCTGCAACTGGCTCTAGGGACTTAGGGACT\n<span style=\"color: red; \">             ||||       </span>\nAAAACGTGCTTCAGACCTCTGGAG</pre></td>";
    res += "<td>-6.7</td>";
    res += "<td><strong>3</strong></td>";
    res += "<td>3</td>";
    res += "<td>75</td>";
    res += "</tr>";

    // row 2
    res += "<tr bgcolor=\"LightGreen\">";
    res += "<td rowspan=\"2\"><pre>2</pre></td>";
    res += "<td><pre>Forward</pre></td>";
    res += "<td><pre> TGCAACTGGCTCGCTCTAGGGACT </pre></td>";
    res += "<td><pre>    TGCAACTGGGGACTTGCAACTGT\n<span style=\"color: red; \">         ||||       </span>\nCGTGCTTCAGACCTCTGGAG</pre></td>";
    res += "<td>-6.7</td>";
    res += "<td>4</td>";
    res += "<td>3</td>";
    res += "<td>75</td>";
    res += "<td rowspan=\"2\"><pre>    TGCAACTGGTGCAACTGGCTCTAGGGACTCTCTAGGGACT\n<span style=\"color: red; \">         ||||       </span>\nCGTGCTTCACTGGAG</pre></td>";
    res += "<td rowspan=\"2\">-6.7</td>";
    res += "<td rowspan=\"2\">4</td>";
    res += "<td rowspan=\"2\">3</td>";
    res += "<td rowspan=\"2\">75</td>";
    res += "</tr>";

    res += "<tr bgcolor=\"LightGreen\">";
    res += "<td><pre>Reverse</pre></td>";
    res += "<td><pre> TGCAACTGGCTCTGGCTCTAGGGACT </pre></td>";
    res += "<td><pre>    AAAATGCAACTGGCTCTAGGGACTTAGGGACT\n<span style=\"color: red; \">             ||||       </span>\nAAAACGGACCTCTGGAG</pre></td>";
    res += "<td>-6.7</td>";
    res += "<td>4</td>";
    res += "<td>3</td>";
    res += "<td>75</td>";
    res += "</tr>";

    res += "</table>";

    return res;


    res += QString("<p><strong>%1</strong></p>").arg(tr("Check complement"));
    
    //  style=\"border-collapse: collapse; width: 100%; text-align: center; vertical-align: middle;\" border=\"1\"

    res += "<table style=\"border-collapse: collapse; vertical-align: middle;\" border=\"1\">";
    //res += "<tbody>";

    res += "<tr>";
    res += "<td text-align: center;>No.</td>";
    res += "<td text-align: center;>F/R</td>";
    res += "<td>Self-dimer</td>";
    res += "</tr>";

    res += "<tr>";
    res += "<td text-align: center;>1</td>";
    //res += "<td rowspan=\"2\">1</td>";
    res += "<td text-align: center;>F</td>";
    res += "<td text-align: center;><pre>    TGCAACTGGCTCTAGGGACT\n<span style=\"color: red; \">         ||||       </span>\nCGTGCTTCAGACCTCTGGAG</pre></td>";

    res += "</tr>";

    //res += "</tbody>";
    res += "</table>";

    return res;

    res += "<table style='border-collapse: collapse; width: 100%; text-align: center; vertical-align: middle;' border='1'><tbody><tr><td>No.</td><td>F/R</td><td>Self-dimer</td><td>Hetero-dimer</td><td>Delta G (kcal/mol)</td><td>Base Pairs (bp)</td><td><p>G/C pairs (bp)</p></td><td><p><span style='font-weight: 400;'>G/C pairs (%)</span></p></td></tr><tr style='height: 18px;'><td style='width:14.2857%; height: 36px;' rowspan='2'>1</td><td style='width: 16.5584%; height: 18px;'>F</td>";
    res += "<td style='width: 12.013%; height: 18px;'><pre>    TGCAACTGGCTCTAGGGACT<br>";
    res += "<span style='color: red;'>         ||||       </span><br>";
    res += "CGTGCTTCAGACCTCTGGAG</pre></td><td style='width: 4.6875%; height: 18px;' rowspan='2'><pre>    TGCAACTGGCTCTAGGGACT";
    res += "<span style='color: red;'>         ||||       </span>";
    res += "CGTGCTTCAGACCTCTGGAG</pre></td><td style='width: 23.8839%; height: 18px;'>&nbsp;</td><td style='width: 7.10225%; height: 18px;'>&nbsp;</td><td style='width: 3.55112%; height:18px;'>&nbsp;</td><td style='width: 5.25568%; height: 18px;'>&nbsp;</td></tr><tr style='height: 18px;'><td style='width: 16.5584%; height: 18px;'>R</td><td style='width: 12.013%; height: 18px;'><pre>    TGCAACTGGCTCTAGGGACT";
    res += "<span style='color: red;'>         ||||       </span>";
    res += "CGTGCTTCAGACCTCTGGAG</pre></td><td style='width: 23.8839%; height: 18px;'>&nbsp;</td><td style='width: 7.10225%; height: 18px;'>&nbsp;</td><td style='width: 3.55112%; height:18px;'>&nbsp;</td><td style='width: 5.25568%; height: 18px;'>&nbsp;</td></tr></tbody></table>";

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
    CHECK(settings.enableMaxComplementPairs, false);

    return dimer.baseCounts > settings.maxComplementPairs;
}

bool CheckComplementTask::isGcContentBad(const DimerFinderResult& dimer) const {
    CHECK(settings.enableMaxGcContent, false);

    return getGAndCNumber(dimer.dimer) > MINIMUN_G_AND_C_NUMBER_FOR_BAD_DIMER && getGAndCProportion(dimer.dimer) > ((double)settings.maxGcContent / 100);
}

int CheckComplementTask::getGAndCNumber(const QString& dimer) {
    return dimer.count('G') + dimer.count('C');
}

double CheckComplementTask::getGAndCProportion(const QString& dimer) {
    return (double)getGAndCNumber(dimer) / dimer.size();
}


}
