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

#include "RealignSequencesInAlignmentTaskTest.h"

#include <U2Core/DocumentModel.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>

#include <U2View/RealignSequencesInAlignmentTask.h>

namespace U2 {

#define IN_OBJECT_NAME_ATTR "in"
#define ROWS_LIST_ATTR "rows"
#define FORCE_USE_UGENE_ALIGNER_ATTR "useUgeneAligner"

void GTest_Realign::init(XMLTestFormat* tf, const QDomElement& el) {
    Q_UNUSED(tf);

    forceUseUgeneAligner = false;

    inputObjectName = el.attribute(IN_OBJECT_NAME_ATTR);
    if (inputObjectName.isEmpty()) {
        failMissingValue(IN_OBJECT_NAME_ATTR);
        return;
    }

    QString rows = el.attribute(ROWS_LIST_ATTR);
    if (rows.isEmpty()) {
        failMissingValue(ROWS_LIST_ATTR);
        return;
    }

    QStringList rowsIndexesToAlignStringList = rows.split(",");
    bool conversionIsOk = false;
    foreach(const QString & str, rowsIndexesToAlignStringList) {
        qint64 rowIndex = str.toUInt(&conversionIsOk);
        if (!conversionIsOk) {
            wrongValue(ROWS_LIST_ATTR);
            return;
        }
        rowsIndexesToAlign.append(rowIndex);
    }

    QString forceUseUgeneAlignerStr = el.attribute(FORCE_USE_UGENE_ALIGNER_ATTR);
    if (!forceUseUgeneAlignerStr.isEmpty() && forceUseUgeneAlignerStr == "true") {
        forceUseUgeneAligner = true;
    }
}

void GTest_Realign::prepare() {
    doc = getContext<Document>(this, inputObjectName);
    if (doc == NULL) {
        stateInfo.setError(QString("context not found %1").arg(inputObjectName));
        return;
    }

    QList<GObject*> list = doc->findGObjectByType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);
    if (list.size() == 0) {
        stateInfo.setError(QString("container of object with type \"%1\" is empty").arg(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT));
        return;
    }

    GObject* obj = list.first();
    if (obj == NULL) {
        stateInfo.setError(QString("object with type \"%1\" not found").arg(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT));
        return;
    }
    assert(obj != NULL);
    msaObj = qobject_cast<MultipleSequenceAlignmentObject*>(obj);
    if (msaObj == NULL) {
        stateInfo.setError(QString("error can't cast to multiple alignment from GObject"));
        return;
    }
    QSet<qint64> rowIdsToRealign;
    foreach(const qint64 index, rowsIndexesToAlign) {
        rowIdsToRealign.insert(msaObj->getMultipleAlignment()->getRowsIds().at(index));
    }
    realignTask = new RealignSequencesInAlignmentTask(msaObj, rowIdsToRealign);
    addSubTask(realignTask);
}

Task::ReportResult GTest_Realign::report() {
    if (!hasError()) {
        if (realignTask->hasError()) {
            stateInfo.setError(realignTask->getError());
            return ReportResult_Finished;
        }
    }
    return ReportResult_Finished;
}

void GTest_Realign::cleanup() {
    XmlTest::cleanup();
}

QList<XMLTestFactory*> RealignTests::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(GTest_Realign::createFactory());
    return res;
}

}