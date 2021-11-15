/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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
 
#include "PCRPrimerDesignForDNAAssemblyTaskTest.h"

#include <QFileInfo>

#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

#define DOC_NAME_ATTR "docName"
#define SEQ_NAME_ATTR "seqName"
#define ENERGY_ATTR "gibbsFreeEnergy"
#define MELTING_POINT_ATTR "meltingPoint"
#define OVERLAP_ATTR "overlapLength"
#define ENERGY_EXCLUDE_ATTR "gibbsFreeEnergyExclude"
#define MELTING_POINT_EXCLUDE_ATTR "meltingPointExclude"
#define COMPL_LENGTH_EXCLUDE_ATTR "complementLengthExclude"
#define INSERT_TO_ATTR "insertTo"
#define LEFT_AREA_ATTR "leftArea"
#define RIGHT_AREA_ATTR "rightArea"
#define RESULT_ATTR "expectedResultsList"
#define BACKBONE_URL_ATTR "backboneSequenceUrl"
#define OTHER_SEQ_PCR_URL_ATTR "otherSequencesInPcrUrl"

void GTest_PCRPrimerDesignForDNAAssemblyTaskTest::init(XMLTestFormat *, const QDomElement &el) {
    docName = el.attribute(DOC_NAME_ATTR);
    seqName = el.attribute(SEQ_NAME_ATTR);
    settings.gibbsFreeEnergy = getU2RangeInt(el, ENERGY_ATTR, ";");
    settings.meltingPoint = getU2RangeInt(el, MELTING_POINT_ATTR, ";");
    settings.overlapLength = getU2RangeInt(el, OVERLAP_ATTR, ";");
    settings.gibbsFreeEnergyExclude = getInt(el, ENERGY_EXCLUDE_ATTR);
    settings.meltingPointExclude = getInt(el, MELTING_POINT_EXCLUDE_ATTR);
    settings.complementLengthExclude = getInt(el, COMPL_LENGTH_EXCLUDE_ATTR);
    QString buf = el.attribute(INSERT_TO_ATTR);
    if (buf == "5") {
        settings.insertTo = PCRPrimerDesignForDNAAssemblyTaskSettings::BackboneBearings::Backbone5;
    } else if (buf == "3") {
        settings.insertTo = PCRPrimerDesignForDNAAssemblyTaskSettings::BackboneBearings::Backbone3;
    } else {
        wrongValue(INSERT_TO_ATTR);
        return;
    }
    settings.leftArea = getU2Region(el, LEFT_AREA_ATTR);
    settings.rightArea = getU2Region(el, RIGHT_AREA_ATTR);
    result = getU2RegionList(el, RESULT_ATTR, ";", 8);
    buf = el.attribute(BACKBONE_URL_ATTR);
    if (!buf.isEmpty()) {
        QFileInfo refFile(env->getVar("COMMON_DATA_DIR") + "/" + buf);
        if (!refFile.exists()) {
            stateInfo.setError(QString("file not exist %1").arg(refFile.absoluteFilePath()));
            return;
        }
        settings.backboneSequenceUrl = env->getVar("COMMON_DATA_DIR") + "/" + buf;
    }
    buf = el.attribute(OTHER_SEQ_PCR_URL_ATTR);
    if (!buf.isEmpty()) {
        QFileInfo refFile(env->getVar("COMMON_DATA_DIR") + "/" + buf);
        if (!refFile.exists()) {
            stateInfo.setError(QString("file not exist %1").arg(refFile.absoluteFilePath()));
            return;
        }
        settings.otherSequencesInPcrUrl = env->getVar("COMMON_DATA_DIR") + "/" + buf;
    }
}

void GTest_PCRPrimerDesignForDNAAssemblyTaskTest::prepare() {
    Document *loadedDocument = getContext<Document>(this, docName);
    if (loadedDocument == nullptr) {
        stateInfo.setError(GTest::tr("context not found %1").arg(docName));
        return;
    }
    auto dnaso = qobject_cast<U2SequenceObject *>(loadedDocument->findGObjectByName(seqName));
    if (dnaso == nullptr) {
        stateInfo.setError(GTest::tr("Sequence %1 not found").arg(seqName));
        return;
    }
    task = new PCRPrimerDesignForDNAAssemblyTask(settings, dnaso->getWholeSequenceData(stateInfo));
    CHECK_OP(stateInfo, );
    addSubTask(task);
}

Task::ReportResult GTest_PCRPrimerDesignForDNAAssemblyTaskTest::report() {
    const QList<U2Region> actualResults = task->getResults();
    if (actualResults != result) {
        CHECK_EXT(actualResults.size() == 8, setError(QString("Actual results size %1, expected 8.")
            .arg(QString::number(actualResults.size()))), ReportResult_Finished);
        QString actualResultsStr;
        for (const U2Region &region : qAsConst(actualResults)) {
            actualResultsStr.append(QString::number(region.startPos) + ".." + QString::number(region.length));
            if (region != actualResults.last()) {
                actualResultsStr.append(";");
            }
        }
        setError(QString("Expected results not match with actual. Actual results'%1'").arg(actualResultsStr));
    }
    return ReportResult_Finished;
}

QList<XMLTestFactory *> PCRPrimerDesignForDNAAssemblyTaskTest::createTestFactories() {
    QList<XMLTestFactory *> res;
    res.append(GTest_PCRPrimerDesignForDNAAssemblyTaskTest::createFactory());
    return res;
}

}  // namespace U2
