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

#include <U2Core/ChromatogramObject.h>
#include <U2Core/DatatypeSerializeUtils.h>
#include <U2Core/RawDataUdrSchema.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/UdrDbi.h>

#include "../util/DatatypeSerializeUtilsUnitTest.h"
#include "ChromatogramObjectUnitTests.h"

namespace U2 {

bool DNAChromatogramObjectTestData::inited = false;
const QString DNAChromatogramObjectTestData::UDR_DB_URL = "DNAChromatogramObjectUnitTests.ugenedb";
TestDbiProvider DNAChromatogramObjectTestData::dbiProvider = TestDbiProvider();
U2EntityRef DNAChromatogramObjectTestData::objRef = U2EntityRef();
Chromatogram DNAChromatogramObjectTestData::chromatogram = Chromatogram();

U2DbiRef DNAChromatogramObjectTestData::getDbiRef() {
    if (!inited) {
        init();
    }
    return dbiProvider.getDbi()->getDbiRef();
}

U2EntityRef DNAChromatogramObjectTestData::getObjRef() {
    if (!inited) {
        init();
    }
    return objRef;
}

U2ObjectDbi* DNAChromatogramObjectTestData::getObjDbi() {
    if (!inited) {
        init();
    }
    return dbiProvider.getDbi()->getObjectDbi();
}

UdrDbi* DNAChromatogramObjectTestData::getUdrDbi() {
    if (!inited) {
        init();
    }
    return dbiProvider.getDbi()->getUdrDbi();
}

const Chromatogram& DNAChromatogramObjectTestData::getChromatogram() {
    if (!inited) {
        init();
    }
    return chromatogram;
}

void DNAChromatogramObjectTestData::init() {
    bool ok = dbiProvider.init(UDR_DB_URL, true);
    SAFE_POINT(ok, "dbi provider failed to initialize", );

    initData();

    inited = true;
}

void DNAChromatogramObjectTestData::initData() {
    U2DbiRef dbiRef = dbiProvider.getDbi()->getDbiRef();
    U2RawData object(dbiRef);
    object.serializer = DNAChromatogramSerializer::ID;

    U2OpStatusImpl os;
    RawDataUdrSchema::createObject(dbiRef, object, os);
    SAFE_POINT_OP(os, );

    objRef = U2EntityRef(dbiRef, object.id);

    chromatogram = Chromatogram();
    chromatogram->seqLength = 102;
    chromatogram->G << 51;
    chromatogram->prob_A << '1';
    chromatogram->prob_C << '2';
    RawDataUdrSchema::writeContent(DNAChromatogramSerializer::serialize(chromatogram), objRef, os);
    SAFE_POINT_OP(os, );
}

void DNAChromatogramObjectTestData::shutdown() {
    if (inited) {
        inited = false;
        U2OpStatusImpl os;
        dbiProvider.close();
        SAFE_POINT_OP(os, );
    }
}

IMPLEMENT_TEST(DNAChromatogramObjectUnitTests, createInstance) {
    Chromatogram chromatogram;
    chromatogram->baseCalls << 20;
    U2OpStatusImpl os;
    QScopedPointer<ChromatogramObject> object(ChromatogramObject::createInstance(chromatogram, "object", DNAChromatogramObjectTestData::getDbiRef(), os));
    CHECK_NO_ERROR(os);

    CompareUtils::checkEqual(chromatogram, object->getChromatogram(), os);
    CHECK_NO_ERROR(os);
}

IMPLEMENT_TEST(DNAChromatogramObjectUnitTests, createInstance_WrongDbi) {
    Chromatogram chromatogram;
    chromatogram->A << 30;

    U2OpStatusImpl os;
    QScopedPointer<ChromatogramObject> object(ChromatogramObject::createInstance(chromatogram, "object", U2DbiRef(), os));
    CHECK_TRUE(os.hasError(), "no error");
}

IMPLEMENT_TEST(DNAChromatogramObjectUnitTests, getChromatogram) {
    ChromatogramObject object("object", DNAChromatogramObjectTestData::getObjRef());

    U2OpStatusImpl os;
    CompareUtils::checkEqual(DNAChromatogramObjectTestData::getChromatogram(), object.getChromatogram(), os);
    CHECK_NO_ERROR(os);
}

IMPLEMENT_TEST(DNAChromatogramObjectUnitTests, getChromatogram_Null) {
    U2EntityRef objRef = DNAChromatogramObjectTestData::getObjRef();
    objRef.entityId = "some id";

    ChromatogramObject object("object", objRef);
    U2OpStatusImpl os;
    CompareUtils::checkEqual(Chromatogram(), object.getChromatogram(), os);
    CHECK_NO_ERROR(os);
}

IMPLEMENT_TEST(DNAChromatogramObjectUnitTests, clone) {
    ChromatogramObject object("object", DNAChromatogramObjectTestData::getObjRef());

    U2OpStatusImpl os;
    GObject* clonedGObj = object.clone(DNAChromatogramObjectTestData::getDbiRef(), os);
    QScopedPointer<ChromatogramObject> cloned(dynamic_cast<ChromatogramObject*>(clonedGObj));
    CHECK_NO_ERROR(os);

    CompareUtils::checkEqual(object.getChromatogram(), cloned->getChromatogram(), os);
    CHECK_NO_ERROR(os);
}

IMPLEMENT_TEST(DNAChromatogramObjectUnitTests, clone_NullDbi) {
    ChromatogramObject object("object", DNAChromatogramObjectTestData::getObjRef());

    U2OpStatusImpl os;
    GObject* clonedGObj = object.clone(U2DbiRef(), os);
    Q_UNUSED(clonedGObj);
    CHECK_TRUE(os.hasError(), "no error");
}

IMPLEMENT_TEST(DNAChromatogramObjectUnitTests, clone_NullObj) {
    U2EntityRef objRef = DNAChromatogramObjectTestData::getObjRef();
    objRef.entityId = "some id";
    ChromatogramObject object("object", objRef);

    U2OpStatusImpl os;
    GObject* clonedGObj = object.clone(DNAChromatogramObjectTestData::getDbiRef(), os);
    Q_UNUSED(clonedGObj);
    CHECK_TRUE(os.hasError(), "no error");
}

IMPLEMENT_TEST(DNAChromatogramObjectUnitTests, remove) {
    Chromatogram chromatogram;
    chromatogram->C << 50;
    chromatogram->hasQV = true;

    U2OpStatusImpl os;
    QScopedPointer<ChromatogramObject> object(ChromatogramObject::createInstance(chromatogram, "object", DNAChromatogramObjectTestData::getDbiRef(), os));
    CHECK_NO_ERROR(os);
    U2DataId objId = object->getEntityRef().entityId;

    DNAChromatogramObjectTestData::getObjDbi()->removeObject(objId, os);
    CHECK_NO_ERROR(os);

    QList<UdrRecord> records = DNAChromatogramObjectTestData::getUdrDbi()->getObjectRecords(RawDataUdrSchema::ID, objId, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(records.isEmpty(), "records");
}

}  // namespace U2
