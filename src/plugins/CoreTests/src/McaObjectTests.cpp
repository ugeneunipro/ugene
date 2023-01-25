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

#include "McaObjectTests.h"

#include <QDomElement>

#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceUtils.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/MultipleChromatogramAlignmentObject.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

const QString GTest_CompareTwoMca::DOC1_ATTR = "doc1";
const QString GTest_CompareTwoMca::DOC2_ATTR = "doc2";

void GTest_CompareTwoMca::init(XMLTestFormat*, const QDomElement& element) {
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

Task::ReportResult GTest_CompareTwoMca::report() {
    Document* doc1 = getContext<Document>(this, docContextName);
    CHECK_EXT(nullptr != doc1, setError(QString("document not found: %1").arg(docContextName)), ReportResult_Finished);
    const QList<GObject*> objs1 = doc1->getObjects();
    CHECK_EXT(1 == objs1.size(), setError(QString("document '%1' contains several objects: the comparison not implemented").arg(docContextName)), ReportResult_Finished);
    MultipleChromatogramAlignmentObject* mca1 = qobject_cast<MultipleChromatogramAlignmentObject*>(objs1.first());
    CHECK_EXT(nullptr != mca1, setError(QString("document '%1' contains an incorrect object: expected '%2', got '%3'").arg(docContextName).arg(GObjectTypes::MULTIPLE_CHROMATOGRAM_ALIGNMENT).arg(objs1.first()->getGObjectType())), ReportResult_Finished);

    Document* doc2 = getContext<Document>(this, secondDocContextName);
    CHECK_EXT(nullptr != doc2, setError(QString("document not found: %1").arg(secondDocContextName)), ReportResult_Finished);
    const QList<GObject*> objs2 = doc2->getObjects();
    CHECK_EXT(1 == objs2.size(), setError(QString("document '%1' contains several objects: the comparison not implemented").arg(secondDocContextName)), ReportResult_Finished);
    MultipleChromatogramAlignmentObject* mca2 = qobject_cast<MultipleChromatogramAlignmentObject*>(objs2.first());
    CHECK_EXT(nullptr != mca2, setError(QString("document '%1' contains an incorrect object: expected '%2', got '%3'").arg(secondDocContextName).arg(GObjectTypes::MULTIPLE_CHROMATOGRAM_ALIGNMENT).arg(objs2.first()->getGObjectType())), ReportResult_Finished);

    const qint64 rowsNumber1 = mca1->getRowCount();
    const qint64 rowsNumber2 = mca2->getRowCount();
    CHECK_EXT(rowsNumber1 == rowsNumber2,
              setError(QString("The rows numbers differ: the object '%1' from the document '%2' contains %3 rows, the object '%4' from the document '%5' contains %6 rows")
                           .arg(mca1->getGObjectName())
                           .arg(docContextName)
                           .arg(rowsNumber1)
                           .arg(mca2->getGObjectName())
                           .arg(secondDocContextName)
                           .arg(rowsNumber2)),
              ReportResult_Finished);

    for (int i = 0; i < rowsNumber1; i++) {
        const MultipleChromatogramAlignmentRow row1 = mca1->getMcaRow(i);
        const MultipleChromatogramAlignmentRow row2 = mca2->getMcaRow(i);
        const bool areEqual = row1->isEqualCore(*row2);
        CHECK_EXT(areEqual, setError(QString("The rows with number %1 differ from each other").arg(i)), ReportResult_Finished);
    }

    U2SequenceObject* reference1 = mca1->getReferenceObj();
    U2SequenceObject* reference2 = mca2->getReferenceObj();
    CHECK_EXT((reference1 == nullptr) == (reference2 == nullptr),
              setError("One object has a reference, but another one doesn't"),
              ReportResult_Finished);

    CHECK(reference1 != nullptr && reference2 != nullptr, ReportResult_Finished);

    QByteArray reference1Sequence = reference1->getWholeSequenceData(stateInfo);
    CHECK_OP(stateInfo, ReportResult_Finished);
    QByteArray reference2Sequence = reference2->getWholeSequenceData(stateInfo);
    CHECK_OP(stateInfo, ReportResult_Finished);
    CHECK_EXT(reference1Sequence == reference2Sequence, setError("References are not equal"), ReportResult_Finished);

    return ReportResult_Finished;
}

QList<XMLTestFactory*> McaObjectTests::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(GTest_CompareTwoMca::createFactory());
    return res;
}

}  // namespace U2
