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

#include <U2Core/RawDataUdrSchema.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "RawDataUdrSchemaUnitTests.h"

namespace U2 {

bool RawDataUdrSchemaTestData::inited = false;
const QString RawDataUdrSchemaTestData::UDR_DB_URL = "RawDataUdrSchemaUnitTests.ugenedb";
TestDbiProvider RawDataUdrSchemaTestData::dbiProvider = TestDbiProvider();
U2EntityRef RawDataUdrSchemaTestData::objRef = U2EntityRef();

U2DbiRef RawDataUdrSchemaTestData::getDbiRef() {
    if (!inited) {
        init();
    }
    return dbiProvider.getDbi()->getDbiRef();
}

U2EntityRef RawDataUdrSchemaTestData::getObjRef() {
    if (!inited) {
        init();
    }
    return objRef;
}

void RawDataUdrSchemaTestData::init() {
    bool ok = dbiProvider.init(UDR_DB_URL, true);
    SAFE_POINT(ok, "dbi provider failed to initialize",);

    initData();

    inited = true;
}

void RawDataUdrSchemaTestData::initData() {
    U2DbiRef dbiRef = dbiProvider.getDbi()->getDbiRef();
    U2RawData object(dbiRef);
    object.visualName = "test url";
    object.serializer = "test serializer";

    U2OpStatusImpl os;
    RawDataUdrSchema::createObject(dbiRef, object, os);
    SAFE_POINT_OP(os, );

    objRef = U2EntityRef(dbiRef, object.id);

    RawDataUdrSchema::writeContent("test data", objRef, os);
    SAFE_POINT_OP(os, );
}

void RawDataUdrSchemaTestData::shutdown() {
    if (inited) {
        inited = false;
        U2OpStatusImpl os;
        dbiProvider.close();
        SAFE_POINT_OP(os, );
    }
}

IMPLEMENT_TEST(RawDataUdrSchemaUnitTests, getObject) {
    U2OpStatusImpl os;
    U2RawData object = RawDataUdrSchema::getObject(RawDataUdrSchemaTestData::getObjRef(), os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE("test url" == object.visualName, "url");
    CHECK_TRUE("test serializer" == object.serializer, "serializer");
}

IMPLEMENT_TEST(RawDataUdrSchemaUnitTests, getObject_Null) {
    U2EntityRef objRef = RawDataUdrSchemaTestData::getObjRef();
    objRef.entityId = "some id";
    U2OpStatusImpl os;
    U2RawData object = RawDataUdrSchema::getObject(objRef, os);
    CHECK_TRUE(os.hasError(), "no error");
}

IMPLEMENT_TEST(RawDataUdrSchemaUnitTests, readAllContent) {
    U2OpStatusImpl os;
    QByteArray data = RawDataUdrSchema::readAllContent(RawDataUdrSchemaTestData::getObjRef(), os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE("test data" == data, "data");
}

IMPLEMENT_TEST(RawDataUdrSchemaUnitTests, readAllContent_Null) {
    U2EntityRef objRef = RawDataUdrSchemaTestData::getObjRef();
    objRef.entityId = "some id";

    U2OpStatusImpl os;
    QByteArray data = RawDataUdrSchema::readAllContent(objRef, os);
    CHECK_TRUE(os.hasError(), "no error");
}

IMPLEMENT_TEST(RawDataUdrSchemaUnitTests, createObject) {
    U2RawData object(RawDataUdrSchemaTestData::getDbiRef());
    object.visualName = "test url 1";
    object.serializer = "test serializer 1";

    U2OpStatusImpl os;
    RawDataUdrSchema::createObject(RawDataUdrSchemaTestData::getDbiRef(), object, os);
    CHECK_NO_ERROR(os);
    U2EntityRef entRef(RawDataUdrSchemaTestData::getDbiRef(), object.id);

    U2RawData objectDb = RawDataUdrSchema::getObject(entRef, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(objectDb.visualName == object.visualName, "name");
    CHECK_TRUE(objectDb.serializer == object.serializer, "serializer");
}

IMPLEMENT_TEST(RawDataUdrSchemaUnitTests, writeContent) {
    U2OpStatusImpl os;
    RawDataUdrSchema::writeContent("content", RawDataUdrSchemaTestData::getObjRef(), os);
    CHECK_NO_ERROR(os);
    QByteArray data = RawDataUdrSchema::readAllContent(RawDataUdrSchemaTestData::getObjRef(), os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE("content" == data, "data");
}

IMPLEMENT_TEST(RawDataUdrSchemaUnitTests, writeContent_Null) {
    U2EntityRef objRef = RawDataUdrSchemaTestData::getObjRef();
    objRef.entityId = "some id";

    U2OpStatusImpl os;
    RawDataUdrSchema::writeContent("content", objRef, os);
    CHECK_TRUE(os.hasError(), "no error");
}

} // U2
