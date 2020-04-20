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

#include <U2Core/UdrDbi.h>
#include <U2Core/RawDataUdrSchema.h>
#include <U2Core/TextObject.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "TextObjectUnitTests.h"

namespace U2 {

bool TextObjectTestData::inited = false;
const QString TextObjectTestData::UDR_DB_URL = "TextObjectUnitTests.ugenedb";
TestDbiProvider TextObjectTestData::dbiProvider = TestDbiProvider();
U2EntityRef TextObjectTestData::objRef = U2EntityRef();

U2DbiRef TextObjectTestData::getDbiRef() {
    if (!inited) {
        init();
    }
    return dbiProvider.getDbi()->getDbiRef();
}

U2EntityRef TextObjectTestData::getObjRef() {
    if (!inited) {
        init();
    }
    return objRef;
}

U2ObjectDbi * TextObjectTestData::getObjDbi() {
    if (!inited) {
        init();
    }
    return dbiProvider.getDbi()->getObjectDbi();
}

UdrDbi * TextObjectTestData::getUdrDbi() {
    if (!inited) {
        init();
    }
    return dbiProvider.getDbi()->getUdrDbi();
}

void TextObjectTestData::init() {
    bool ok = dbiProvider.init(UDR_DB_URL, true);
    SAFE_POINT(ok, "dbi provider failed to initialize",);

    inited = true;

    initData();
}

void TextObjectTestData::initData() {
    U2DbiRef dbiRef = TextObjectTestData::getDbiRef();
    U2RawData object(dbiRef);
    object.visualName = "Text";

    U2OpStatusImpl os;
    RawDataUdrSchema::createObject(dbiRef, object, os);
    SAFE_POINT_OP(os, );

    objRef = U2EntityRef(dbiRef, object.id);

    RawDataUdrSchema::writeContent("text", objRef, os);
    SAFE_POINT_OP(os, );
}

void TextObjectTestData::shutdown() {
    if (inited) {
        inited = false;
        dbiProvider.close();
    }
}

IMPLEMENT_TEST(TextObjectUnitTests, createInstance) {
    U2OpStatusImpl os;
    QScopedPointer<TextObject> object(TextObject::createInstance("some text", "object", TextObjectTestData::getDbiRef(), os));
    CHECK_NO_ERROR(os);

    CHECK_TRUE("some text" == object->getText(), "text");
}

IMPLEMENT_TEST(TextObjectUnitTests, createInstance_WrongDbi) {
    U2OpStatusImpl os;
    QScopedPointer<TextObject> object(TextObject::createInstance("some text", "object", U2DbiRef(), os));
    CHECK_TRUE(os.hasError(), "no error");
}

IMPLEMENT_TEST(TextObjectUnitTests, getText) {
    TextObject object("object", TextObjectTestData::getObjRef());
    CHECK_TRUE("text" == object.getText(), "text");
}

IMPLEMENT_TEST(TextObjectUnitTests, getText_Null) {
    U2EntityRef objRef = TextObjectTestData::getObjRef();
    objRef.entityId = "some id";

    TextObject object("object", objRef);
    CHECK_TRUE("" == object.getText(), "text");
}

IMPLEMENT_TEST(TextObjectUnitTests, setText) {
    U2OpStatusImpl os;
    QScopedPointer<TextObject> object(TextObject::createInstance("", "object", TextObjectTestData::getDbiRef(), os));
    CHECK_NO_ERROR(os);

    object->setText("the very text");
    CHECK_TRUE("the very text" == object->getText(), "text");
}

IMPLEMENT_TEST(TextObjectUnitTests, clone) {
    TextObject object("object", TextObjectTestData::getObjRef());

    U2OpStatusImpl os;
    GObject *clonedGObj = object.clone(TextObjectTestData::getDbiRef(), os);
    QScopedPointer<TextObject> cloned(dynamic_cast<TextObject*>(clonedGObj));
    CHECK_NO_ERROR(os);

    cloned->setText("cloned text");

    CHECK_TRUE("text" == object.getText(), "text");
    CHECK_TRUE("cloned text" == cloned->getText(), "cloned text");
}

IMPLEMENT_TEST(TextObjectUnitTests, clone_NullDbi) {
    TextObject object("object", TextObjectTestData::getObjRef());

    U2OpStatusImpl os;
    GObject *clonedGObj = object.clone(U2DbiRef(), os);
    Q_UNUSED(clonedGObj);
    CHECK_TRUE(os.hasError(), "no error");
}

IMPLEMENT_TEST(TextObjectUnitTests, clone_NullObj) {
    U2EntityRef objRef = TextObjectTestData::getObjRef();
    objRef.entityId = "some id";
    TextObject object("object", objRef);

    U2OpStatusImpl os;
    GObject *clonedGObj = object.clone(TextObjectTestData::getDbiRef(), os);
    Q_UNUSED(clonedGObj);
    CHECK_TRUE(os.hasError(), "no error");
}

IMPLEMENT_TEST(TextObjectUnitTests, remove) {
    U2OpStatusImpl os;
    QScopedPointer<TextObject> object(TextObject::createInstance("some text", "object", TextObjectTestData::getDbiRef(), os));
    CHECK_NO_ERROR(os);
    U2DataId objId = object->getEntityRef().entityId;

    TextObjectTestData::getObjDbi()->removeObject(objId, os);
    CHECK_NO_ERROR(os);

    QList<UdrRecord> records = TextObjectTestData::getUdrDbi()->getObjectRecords(RawDataUdrSchema::ID, objId, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(records.isEmpty(), "records");
}

} // U2
