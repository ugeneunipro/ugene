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

#include <U2Core/DNAChromatogramObject.h>
#include <U2Core/DatatypeSerializeUtils.h>
#include <U2Core/RawDataUdrSchema.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/UdrDbi.h>

#include "../util/DatatypeSerializeUtilsUnitTest.h"

#include "DNAChromatogramObjectUnitTests.h"

namespace U2 {

bool DNAChromatogramObjectTestData::inited = false;
const QString DNAChromatogramObjectTestData::UDR_DB_URL = "DNAChromatogramObjectUnitTests.ugenedb";
TestDbiProvider DNAChromatogramObjectTestData::dbiProvider = TestDbiProvider();
U2EntityRef DNAChromatogramObjectTestData::objRef = U2EntityRef();
DNAChromatogram DNAChromatogramObjectTestData::chroma = DNAChromatogram();

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

U2ObjectDbi * DNAChromatogramObjectTestData::getObjDbi() {
    if (!inited) {
        init();
    }
    return dbiProvider.getDbi()->getObjectDbi();
}

UdrDbi * DNAChromatogramObjectTestData::getUdrDbi() {
    if (!inited) {
        init();
    }
    return dbiProvider.getDbi()->getUdrDbi();
}

const DNAChromatogram & DNAChromatogramObjectTestData::getChromatogram() {
    if (!inited) {
        init();
    }
    return chroma;
}

void DNAChromatogramObjectTestData::init() {
    bool ok = dbiProvider.init(UDR_DB_URL, true);
    SAFE_POINT(ok, "dbi provider failed to initialize",);

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

    chroma = DNAChromatogram();
    chroma.seqLength = 102;
    chroma.G << 51;
    chroma.prob_A << '1';
    chroma.prob_C << '2';
    RawDataUdrSchema::writeContent(DNAChromatogramSerializer::serialize(chroma), objRef, os);
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
    DNAChromatogram src;
    src.baseCalls << 20;
    U2OpStatusImpl os;
    QScopedPointer<DNAChromatogramObject> object(DNAChromatogramObject::createInstance(src, "object", DNAChromatogramObjectTestData::getDbiRef(), os));
    CHECK_NO_ERROR(os);

    CompareUtils::checkEqual(src, object->getChromatogram(), os);
    CHECK_NO_ERROR(os);
}

IMPLEMENT_TEST(DNAChromatogramObjectUnitTests, createInstance_WrongDbi) {
    DNAChromatogram src;
    src.A << 30;

    U2OpStatusImpl os;
    QScopedPointer<DNAChromatogramObject> object(DNAChromatogramObject::createInstance(src, "object", U2DbiRef(), os));
    CHECK_TRUE(os.hasError(), "no error");
}

IMPLEMENT_TEST(DNAChromatogramObjectUnitTests, getChromatogram) {
    DNAChromatogramObject object("object", DNAChromatogramObjectTestData::getObjRef());

    U2OpStatusImpl os;
    CompareUtils::checkEqual(DNAChromatogramObjectTestData::getChromatogram(), object.getChromatogram(), os);
    CHECK_NO_ERROR(os);
}

IMPLEMENT_TEST(DNAChromatogramObjectUnitTests, getChromatogram_Null) {
    U2EntityRef objRef = DNAChromatogramObjectTestData::getObjRef();
    objRef.entityId = "some id";

    DNAChromatogramObject object("object", objRef);
    U2OpStatusImpl os;
    CompareUtils::checkEqual(DNAChromatogram(), object.getChromatogram(), os);
    CHECK_NO_ERROR(os);
}

IMPLEMENT_TEST(DNAChromatogramObjectUnitTests, clone) {
    DNAChromatogramObject object("object", DNAChromatogramObjectTestData::getObjRef());

    U2OpStatusImpl os;
    GObject *clonedGObj = object.clone(DNAChromatogramObjectTestData::getDbiRef(), os);
    QScopedPointer<DNAChromatogramObject> cloned(dynamic_cast<DNAChromatogramObject*>(clonedGObj));
    CHECK_NO_ERROR(os);

    CompareUtils::checkEqual(object.getChromatogram(), cloned->getChromatogram(), os);
    CHECK_NO_ERROR(os);
}

IMPLEMENT_TEST(DNAChromatogramObjectUnitTests, clone_NullDbi) {
    DNAChromatogramObject object("object", DNAChromatogramObjectTestData::getObjRef());

    U2OpStatusImpl os;
    GObject *clonedGObj = object.clone(U2DbiRef(), os);
    Q_UNUSED(clonedGObj);
    CHECK_TRUE(os.hasError(), "no error");
}

IMPLEMENT_TEST(DNAChromatogramObjectUnitTests, clone_NullObj) {
    U2EntityRef objRef = DNAChromatogramObjectTestData::getObjRef();
    objRef.entityId = "some id";
    DNAChromatogramObject object("object", objRef);

    U2OpStatusImpl os;
    GObject *clonedGObj = object.clone(DNAChromatogramObjectTestData::getDbiRef(), os);
    Q_UNUSED(clonedGObj);
    CHECK_TRUE(os.hasError(), "no error");
}

IMPLEMENT_TEST(DNAChromatogramObjectUnitTests, remove) {
    DNAChromatogram src;
    src.C << 50;
    src.hasQV = true;

    U2OpStatusImpl os;
    QScopedPointer<DNAChromatogramObject> object(DNAChromatogramObject::createInstance(src, "object", DNAChromatogramObjectTestData::getDbiRef(), os));
    CHECK_NO_ERROR(os);
    U2DataId objId = object->getEntityRef().entityId;

    DNAChromatogramObjectTestData::getObjDbi()->removeObject(objId, os);
    CHECK_NO_ERROR(os);

    QList<UdrRecord> records = DNAChromatogramObjectTestData::getUdrDbi()->getObjectRecords(RawDataUdrSchema::ID, objId, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(records.isEmpty(), "records");
}

} // U2
