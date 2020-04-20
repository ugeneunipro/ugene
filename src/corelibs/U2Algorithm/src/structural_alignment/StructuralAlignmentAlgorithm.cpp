/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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

#include "StructuralAlignmentAlgorithm.h"

namespace U2 {

/* class U2ALGORITHM_EXPORT BioStruct3DReference */
QString BioStruct3DReference::print() const {
    QString s = obj->getGObjectName();

    if (chains.size() == 1) {
        int chainId = chains.first();
        s += QString(" chain %1").arg(chainId);
        s += QString(" region %1..%2").arg(chainRegion.startPos + 1).arg(chainRegion.endPos());
    }
    else {
        s += " chains [";
        foreach (int chain, chains) {
            s += QString::number(chain) + ",";
        }
        s.chop(1);
        s += "]";
    }

    s += QString(" model %3").arg(modelId);
    return s;
}

/* class U2ALGORITHM_EXPORT StructuralAlignmentTask : public Task */
StructuralAlignmentTask::StructuralAlignmentTask(StructuralAlignmentAlgorithm *_algorithm, const StructuralAlignmentTaskSettings &_settings)
        : Task("StructuralAlignmentTask", TaskFlag_ReportingIsSupported | TaskFlag_ReportingIsEnabled), algorithm(_algorithm), settings(_settings), result()
{}

void StructuralAlignmentTask::run() {
    result = algorithm->align(settings, stateInfo);
}

Task::ReportResult StructuralAlignmentTask::report() {
    return ReportResult_Finished;
}

QString StructuralAlignmentTask::generateReport() const {
    QString res;

    if (!hasError()) {
        res += QString("Structural alignment finished on <b>%1</b> (reference) vs <b>%2</b>").arg(settings.ref.print(), settings.alt.print()) + "<br><br>";
        res += QString("<b>RMSD</b> = %1").arg(result.rmsd);

        res += "<table><tr><td>";
            res += "<b>Transform</b> = ";
        res += "</td><td>";
            res += "<table>";
                res += "<tr>";
                for (int i = 0; i < 16; ++i) {
                    res += "<td>" + QString::number(result.transform[i]) + "</td>";
                    if ((i+1) % 4 == 0 && i < 15) {
                        res += "<\tr><tr>";
                    }
                }
                res += "</tr>";
            res += "</table>";
        res += "</td></tr></table>";
    }
    else {
        res += QString("Structural alignment on <b>%1</b> (reference) vs <b>%2</b> failed").arg(settings.ref.print(), settings.alt.print());
    }

    return res;
}

}   // namespace U2
