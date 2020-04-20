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

#include "MsaObjectUnitTests.h"

#include <U2Core/MultipleSequenceAlignmentExporter.h>
#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/U2MsaDbi.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>

namespace U2 {

TestDbiProvider MsaObjectTestData::dbiProvider = TestDbiProvider();
const QString& MsaObjectTestData::MAL_OBJ_DB_URL("malignment-object-dbi.ugenedb");
U2DbiRef MsaObjectTestData::dbiRef =  U2DbiRef();

void MsaObjectTestData::init() {
    bool ok = dbiProvider.init(MAL_OBJ_DB_URL, false);
    SAFE_POINT(ok, "Dbi provider failed to initialize in MsaObjectTestData::init()!",);

    U2Dbi* dbi = dbiProvider.getDbi();
    dbiRef = dbi->getDbiRef();
    dbiProvider.close();
}

void MsaObjectTestData::shutdown() {
    if (dbiRef != U2DbiRef()) {
        U2OpStatusImpl opStatus;
        dbiRef = U2DbiRef();
        dbiProvider.close();
        SAFE_POINT_OP(opStatus, );
    }
}

U2DbiRef MsaObjectTestData::getDbiRef() {
    if (dbiRef == U2DbiRef()) {
        init();
    }
    return dbiRef;
}

MultipleSequenceAlignmentObject *MsaObjectTestData::getTestAlignmentObject(const U2DbiRef &dbiRef, const QString &name, U2OpStatus &os) {
    const U2EntityRef entityRef = getTestAlignmentRef(dbiRef, name, os);
    CHECK_OP(os, NULL);

    return new MultipleSequenceAlignmentObject(name, entityRef);
}

U2EntityRef MsaObjectTestData::getTestAlignmentRef(const U2DbiRef &dbiRef, const QString &name, U2OpStatus &os) {
    DbiConnection con(dbiRef, os);
    CHECK_OP(os, U2EntityRef());

    QScopedPointer<U2DbiIterator<U2DataId> > it(con.dbi->getObjectDbi()->getObjectsByVisualName(name, U2Type::Msa, os));
    CHECK_OP(os, U2EntityRef());

    CHECK_EXT(it->hasNext(), os.setError(QString("Malignment object '%1' wasn't found in the database").arg(name)), U2EntityRef());
    const U2DataId msaId = it->next();
    CHECK_EXT(!msaId.isEmpty(), os.setError(QString("Malignment object '%1' wasn't found in the database").arg(name)), U2EntityRef());

    return U2EntityRef(dbiRef, msaId);
}

MultipleSequenceAlignment MsaObjectTestData::getTestAlignment(const U2DbiRef &dbiRef, const QString &name, U2OpStatus &os) {
    U2EntityRef malignmentRef = getTestAlignmentRef(dbiRef, name, os);
    CHECK_OP(os, MultipleSequenceAlignment());

    MultipleSequenceAlignmentExporter exporter;
    return exporter.getAlignment(dbiRef, malignmentRef.entityId, os);
}

IMPLEMENT_TEST(MsaObjectUnitTests, getMAlignment) {
//  Test data:
//  ---AG-T
//  AG-CT-TAA

    const QString alName = "Test alignment";
    const U2DbiRef dbiRef = MsaObjectTestData::getDbiRef();
    U2OpStatusImpl os;

    QScopedPointer<MultipleSequenceAlignmentObject> alObj(MsaObjectTestData::getTestAlignmentObject(dbiRef, alName, os));
    CHECK_NO_ERROR(os);

    const MultipleSequenceAlignment alActual = alObj->getMultipleAlignment();

    const bool alsEqual = (*alActual == *MsaObjectTestData::getTestAlignment(dbiRef, alName, os));
    CHECK_TRUE(alsEqual, "Actual alignment doesn't equal to the original!");
    CHECK_EQUAL(alName, alActual->getName(), "alignment name");
}

IMPLEMENT_TEST(MsaObjectUnitTests, setMAlignment) {
//  Test data, alignment 1:
//  ---AG-T
//  AG-CT-TAA

//  alignment 2:
//  AC-GT--AAA
//  -ACACA-GT

    const QString firstAlignmentName = "Test alignment";
    const QString secondAlignmentName = "Test alignment 2";
    const U2DbiRef dbiRef = MsaObjectTestData::getDbiRef();
    U2OpStatusImpl os;

    QScopedPointer<MultipleSequenceAlignmentObject> alObj(MsaObjectTestData::getTestAlignmentObject(dbiRef, firstAlignmentName, os));
    CHECK_NO_ERROR(os);

    const MultipleSequenceAlignment secondAlignment = MsaObjectTestData::getTestAlignment(dbiRef, secondAlignmentName, os);
    alObj->setMultipleAlignment(secondAlignment);
    const MultipleSequenceAlignment actualAlignment = alObj->getMultipleAlignment();

    bool alsEqual = (*secondAlignment == *actualAlignment);
    CHECK_TRUE(alsEqual, "Actual alignment doesn't equal to the original!");
    CHECK_EQUAL(secondAlignmentName, actualAlignment->getName(), "alignment name");
}

IMPLEMENT_TEST( MsaObjectUnitTests, deleteGap_trailingGaps ) {
//  Test data:
//  AC-GT--AAA----
//  -ACA---GTT----
//  -ACACA-G------

//  Expected result: the same

    const QString malignment = "Alignment with trailing gaps";
    const U2DbiRef dbiRef = MsaObjectTestData::getDbiRef();
    U2OpStatusImpl os;

    QScopedPointer<MultipleSequenceAlignmentObject> alnObj(MsaObjectTestData::getTestAlignmentObject(dbiRef, malignment, os));
    CHECK_NO_ERROR(os);

    alnObj->deleteGap(os, U2Region(0, alnObj->getNumRows()), 10, 3);

    const MultipleSequenceAlignment resultAlignment = alnObj->getMultipleAlignment();
    CHECK_TRUE(resultAlignment->getMsaRow(0)->getData() == "AC-GT--AAA-", "First row content is unexpected!");
    CHECK_TRUE(resultAlignment->getMsaRow(1)->getData() == "-ACA---GTT-", "Second row content is unexpected!");
    CHECK_TRUE(resultAlignment->getMsaRow(2)->getData() == "-ACACA-G---", "Third row content is unexpected!");
}

IMPLEMENT_TEST( MsaObjectUnitTests, deleteGap_regionWithNonGapSymbols ) {
//  Test data:
//  AC-GT--AAA----
//  -ACA---GTT----
//  -ACACA-G------

//  Expected result: the same

    const QString alignmentName = "Alignment with trailing gaps";
    const U2DbiRef dbiRef = MsaObjectTestData::getDbiRef();
    U2OpStatusImpl os;

    QScopedPointer<MultipleSequenceAlignmentObject> alnObj(MsaObjectTestData::getTestAlignmentObject(dbiRef, alignmentName, os));
    CHECK_NO_ERROR( os );

    const int countOfDeleted = alnObj->deleteGap(os, U2Region(1, alnObj->getNumRows() - 1), 6, 2);
    SAFE_POINT_OP(os, );

    CHECK_TRUE(0 == countOfDeleted, "Unexpected count of removed symbols!");
    const MultipleSequenceAlignment resultAlignment = alnObj->getMultipleAlignment();
    CHECK_TRUE(resultAlignment->getMsaRow(0)->getData() == "AC-GT--AAA----", "First row content is unexpected!");
    CHECK_TRUE(resultAlignment->getMsaRow(1)->getData() == "-ACA---GTT----", "Second row content is unexpected!");
    CHECK_TRUE(resultAlignment->getMsaRow(2)->getData() == "-ACACA-G------", "Third row content is unexpected!");
}

IMPLEMENT_TEST( MsaObjectUnitTests, deleteGap_gapRegion ) {
//  Test data:
//  AC-GT--AAA----
//  -ACA---GTT----
//  -ACACA-G------

//  Expected result:
//  AC-GTAAA----
//  -ACA-GTT----
//  -ACACA-G------

    const QString alignmentName = "Alignment with trailing gaps";
    const U2DbiRef dbiRef = MsaObjectTestData::getDbiRef();
    U2OpStatusImpl os;

    QScopedPointer<MultipleSequenceAlignmentObject> alnObj(MsaObjectTestData::getTestAlignmentObject(dbiRef, alignmentName, os));
    CHECK_NO_ERROR(os);

    const int countOfDeleted = alnObj->deleteGap(os, U2Region(0, alnObj->getNumRows() - 1), 5, 2);
    SAFE_POINT_OP(os, );

    CHECK_TRUE(2 == countOfDeleted, "Unexpected count of removed symbols!");
    const MultipleSequenceAlignment resultAlignment = alnObj->getMultipleAlignment();
    CHECK_TRUE(resultAlignment->getMsaRow(0)->getData() == "AC-GTAAA---", "First row content is unexpected!");
    CHECK_TRUE(resultAlignment->getMsaRow(1)->getData() == "-ACA-GTT---", "Second row content is unexpected!");
    CHECK_TRUE(resultAlignment->getMsaRow(2)->getData() == "-ACACA-G---", "Third row content is unexpected!");
}

} // namespace
