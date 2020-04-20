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

#include <cmath>

#include <QDomElement>

#include <U2Core/BioStruct3DObject.h>

#include "PToolsTests.h"

namespace U2 {

/* class Gtest_PToolsAlignerTask : public XmlTest */

const QString Gtest_PToolsAlignerTask::REFO_ATTR("refo");
const QString Gtest_PToolsAlignerTask::MOBO_ATTR("mobo");
const QString Gtest_PToolsAlignerTask::RMSD_ATTR("rmsd");
const QString Gtest_PToolsAlignerTask::ACCURACY_ATTR("accuracy");
const QString Gtest_PToolsAlignerTask::TRANSFORM_ATTR("transform");

void Gtest_PToolsAlignerTask::init(XMLTestFormat*, const QDomElement& el) {
    {
        refName = el.attribute(REFO_ATTR);
        if (refName.isEmpty()) {
            failMissingValue(REFO_ATTR);
            return;
        }
    }

    {
        mobName = el.attribute(MOBO_ATTR);
        if (mobName.isEmpty()) {
            failMissingValue(MOBO_ATTR);
            return;
        }
    }

    {
        QString str_expectedRMSD = el.attribute(RMSD_ATTR);
        if (refName.isEmpty()) {
            failMissingValue(RMSD_ATTR);
            return;
        }
        bool ok;
        expected.rmsd = str_expectedRMSD.toDouble(&ok);
        if (!ok) {
            setError(QString("Error: bad rmsd value %1").arg(str_expectedRMSD));
            return;
        }
    }

    {
        QString str_accuracy = el.attribute(ACCURACY_ATTR, "0");

        bool ok;
        accuracy = str_accuracy.toDouble(&ok);
        if (!ok) {
            setError(QString("Error: bad rmsd value %1").arg(str_accuracy));
            return;
        }
    }

    {
        QString str_expectedTransform = el.attribute(TRANSFORM_ATTR);
        if (refName.isEmpty()) {
            failMissingValue(TRANSFORM_ATTR);
            return;
        }

        QStringList str_telements = str_expectedTransform.split(",");
        if (str_telements.size() != 16) {
            setError(QString("Error: bad transform matrix value %1").arg(str_expectedTransform));
            return;
        }
        else {
            int i = 0;
            foreach (const QString &str_elem, str_telements) {
                bool ok;
                expected.transform[i++] = str_elem.trimmed().toDouble(&ok);
                if (!ok) {
                    setError(QString("Error: bad transform matrix element value %1").arg(str_elem));
                    return;
                }
            }
        }
    }
}

void Gtest_PToolsAlignerTask::run() {
    BioStruct3DObject   *refo = getContext<BioStruct3DObject>(this, refName),
                        *mobo = getContext<BioStruct3DObject>(this, mobName);

    if (refo == 0 || mobo == 0) {
        if (refo == 0) {
            stateInfo.setError(QString("Error: can't cast to BioSrtuct3D from GObject named: %1").arg(refName));
        }
        if (mobo == 0) {
            stateInfo.setError(QString("Error: can't cast to BioSrtuct3D from GObject named: %1").arg(mobName));
        }

        return;
    }

    StructuralAlignmentTaskSettings settings( BioStruct3DReference(refo, refo->getBioStruct3D().moleculeMap.keys(), refo->getBioStruct3D().modelMap.keys().first()),
                                              BioStruct3DReference(mobo, mobo->getBioStruct3D().moleculeMap.keys(), mobo->getBioStruct3D().modelMap.keys().first()) );

    result = PToolsAligner().align(settings, stateInfo);
}

/** Compare alignment results with given accuracy  */
static bool isEqual(const StructuralAlignment &first, const StructuralAlignment &second, double accuracy) {
    if (fabs(first.rmsd - second.rmsd) > accuracy) {
        return false;
    }

    for (int i = 0; i < 16; ++i) {
        if (fabs(first.transform[i] - second.transform[i]) > accuracy) {
            return false;
        }
    }

    return true;
}

/** Pretty print the alignment result with given accuracy
  * so you can see the missmatches
  */
static QString alignmentToStr(const StructuralAlignment &al, double acc = 0) {
    int prec = 6;
    if (acc != 0.0) {
        prec = (int) log10(1.0/acc);
    }
    prec = (prec < 1) ? 1 : prec;

    QString res = QString("RMSD: ") + QString::number(al.rmsd, 'g', prec);

    res += "; Transform: [ ";
    for (int i = 0; i < 16; ++i) {
        res += QString::number(al.transform[i], 'g', prec) + ", ";
    }
    res.chop(2);
    res += " ]";

    return res;
}

Task::ReportResult Gtest_PToolsAlignerTask::report() {
    if (!isEqual(result, expected, accuracy)) {
        QString msg = QString("Alignment results are not equal with accuracy %1 \n").arg(accuracy);
        msg += " Calculated - \n" + alignmentToStr(result, accuracy);
        msg += " Expected   - \n" + alignmentToStr(expected, accuracy);

        stateInfo.setError(msg);
    }
    return ReportResult_Finished;
}

}   // namespace U2
