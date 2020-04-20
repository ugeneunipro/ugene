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

#include <QDomElement>

#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceUtils.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/U2SafePoints.h>

#include "MsaObjectTests.h"

namespace U2 {

const QString GTest_CompareTwoMsa::DOC1_ATTR = "doc1";
const QString GTest_CompareTwoMsa::DOC2_ATTR = "doc2";

void GTest_CompareTwoMsa::init(XMLTestFormat *, const QDomElement& element) {
    docContextName = element.attribute(DOC1_ATTR);
    if (docContextName.isEmpty()) {
        failMissingValue(DOC1_ATTR);
        return;
    }

    secondDocContextName = element.attribute(DOC2_ATTR);
    if (secondDocContextName.isEmpty()) {
        failMissingValue(DOC2_ATTR);
        return;
    }
}

Task::ReportResult GTest_CompareTwoMsa::report() {
    Document *doc1 = getContext<Document>(this, docContextName);
    CHECK_EXT(NULL != doc1, setError(QString("document not found: %1").arg(docContextName)), ReportResult_Finished);

    const QList<GObject *> objs1 = doc1->getObjects();
    CHECK_EXT(1 == objs1.size(), setError(QString("document '%1' contains several objects: the comparison not implemented").arg(docContextName)), ReportResult_Finished);

    MultipleSequenceAlignmentObject *msa1 = qobject_cast<MultipleSequenceAlignmentObject *>(objs1.first());
    CHECK_EXT(NULL != msa1, setError(QString("document '%1' contains an incorrect object: expected '%2', got '%3'")
                                     .arg(docContextName)
                                     .arg(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT)
                                     .arg(objs1.first()->getGObjectType())), ReportResult_Finished);

    Document *doc2 = getContext<Document>(this, secondDocContextName);
    CHECK_EXT(NULL != doc2, setError(QString("document not found: %1").arg(secondDocContextName)), ReportResult_Finished);

    const QList<GObject *> objs2 = doc2->getObjects();
    CHECK_EXT(1 == objs2.size(), setError(QString("document '%1' contains several objects: the comparison not implemented").arg(secondDocContextName)), ReportResult_Finished);

    MultipleSequenceAlignmentObject *msa2 = qobject_cast<MultipleSequenceAlignmentObject *>(objs2.first());
    CHECK_EXT(NULL != msa2, setError(QString("document '%1' contains an incorrect object: expected '%2', got '%3'")
                                     .arg(secondDocContextName)
                                     .arg(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT)
                                     .arg(objs2.first()->getGObjectType())), ReportResult_Finished);

    const qint64 rowsNumber1 = msa1->getNumRows();
    const qint64 rowsNumber2 = msa2->getNumRows();
    CHECK_EXT(rowsNumber1 == rowsNumber2,
              setError(QString("The rows numbers differ: the object '%1' from the document '%2' contains %3 rows, the object '%4' from the document '%5' contains %6 rows")
              .arg(msa1->getGObjectName())
              .arg(docContextName)
              .arg(rowsNumber1)
              .arg(msa2->getGObjectName())
              .arg(secondDocContextName)
              .arg(rowsNumber2)), ReportResult_Finished);

    for (int i = 0; i < rowsNumber1; i++) {
        const MultipleSequenceAlignmentRow row1 = msa1->getMsaRow(i);
        const MultipleSequenceAlignmentRow row2 = msa2->getMsaRow(i);
        const bool areEqual = row1->isRowContentEqual(row2);
        CHECK_EXT(areEqual, setError(QString("The rows with number %1 differ from each other").arg(i)), ReportResult_Finished);
    }

    return ReportResult_Finished;
}

QList<XMLTestFactory *> MsaObjectTests::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(GTest_CompareTwoMsa::createFactory());
    return res;
}

}   // namespace U2
