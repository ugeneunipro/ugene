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

#include "RealignSequencesInAlignmentTaskTest.h"

#include <U2Algorithm/BaseAlignmentAlgorithmsIds.h>

#include <U2Core/DocumentModel.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>

#include <U2View/RealignSequencesInAlignmentTask.h>

namespace U2 {

#define IN_OBJECT_NAME_ATTR "in"
#define ROWS_LIST_ATTR "rows"

void GTest_Realign::init(XMLTestFormat*, const QDomElement& el) {
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
    for (const QString& str : qAsConst(rowsIndexesToAlignStringList)) {
        int rowIndex = str.toInt(&conversionIsOk);
        if (!conversionIsOk) {
            wrongValue(ROWS_LIST_ATTR);
            return;
        }
        rowsIndexesToAlign.append(rowIndex);
    }
}

void GTest_Realign::prepare() {
    doc = getContext<Document>(this, inputObjectName);
    if (doc == nullptr) {
        stateInfo.setError(QString("context not found %1").arg(inputObjectName));
        return;
    }

    QList<GObject*> list = doc->findGObjectByType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);
    if (list.isEmpty()) {
        stateInfo.setError(QString("container of object with type \"%1\" is empty").arg(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT));
        return;
    }

    GObject* obj = list.first();
    if (obj == nullptr) {
        stateInfo.setError(QString("object with type \"%1\" not found").arg(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT));
        return;
    }
    msaObj = qobject_cast<MultipleSequenceAlignmentObject*>(obj);
    if (msaObj == nullptr) {
        stateInfo.setError(QString("error can't cast to multiple alignment from GObject"));
        return;
    }
    QList<qint64> rowIds = msaObj->getMultipleAlignment()->getRowsIds();
    QSet<qint64> rowIdsToRealign;
    for (int index : qAsConst(rowsIndexesToAlign)) {
        rowIdsToRealign.insert(rowIds.at(index));
    }
    realignTask = new RealignSequencesInAlignmentTask(msaObj, rowIdsToRealign, BaseAlignmentAlgorithmsIds::ALIGN_SEQUENCES_TO_ALIGNMENT_BY_MAFFT);
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

}  // namespace U2