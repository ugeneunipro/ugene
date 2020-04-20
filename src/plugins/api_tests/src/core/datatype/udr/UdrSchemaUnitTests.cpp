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

#include <U2Core/AppContext.h>
#include <U2Core/UdrSchemaRegistry.h>
#include <U2Core/UdrRecord.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "UdrSchemaUnitTests.h"

namespace U2 {

namespace {
    UdrSchema getTestSchema() {
        U2OpStatusImpl os;
        UdrSchema schema("id");
        schema.addField(UdrSchema::FieldDesc("name1", UdrSchema::INTEGER, UdrSchema::INDEXED), os);
        schema.addField(UdrSchema::FieldDesc("name2", UdrSchema::BLOB, UdrSchema::NOT_INDEXED), os);
        schema.addField(UdrSchema::FieldDesc("name3", UdrSchema::STRING), os);
        schema.addField(UdrSchema::FieldDesc("name4", UdrSchema::DOUBLE), os);
        SAFE_POINT_OP(os, schema);
        return schema;
    }
    UdrSchema getTestSchema2() {
        U2OpStatusImpl os;
        UdrSchema schema("id");
        schema.addField(UdrSchema::FieldDesc("name1", UdrSchema::INTEGER, UdrSchema::INDEXED), os);
        schema.addField(UdrSchema::FieldDesc("name2", UdrSchema::INTEGER, UdrSchema::NOT_INDEXED), os);
        schema.addField(UdrSchema::FieldDesc("name3", UdrSchema::STRING), os);
        schema.addField(UdrSchema::FieldDesc("name4", UdrSchema::DOUBLE), os);
        SAFE_POINT_OP(os, schema);
        return schema;
    }
}

IMPLEMENT_TEST(UdrSchemaUnitTests, addField_RecordId) {
    U2OpStatusImpl os;
    UdrSchema schema("id");
    schema.addField(UdrSchema::FieldDesc(UdrSchema::RECORD_ID_FIELD_NAME, UdrSchema::INTEGER), os);
    CHECK_TRUE(os.hasError(), "record_id field");
}

IMPLEMENT_TEST(UdrSchemaUnitTests, addField_Object) {
    U2OpStatusImpl os;
    UdrSchema schema("id", true);
    schema.addField(UdrSchema::FieldDesc(UdrSchema::OBJECT_FIELD_NAME, UdrSchema::INTEGER), os);
    CHECK_TRUE(os.hasError(), "duplicate name");
}

IMPLEMENT_TEST(UdrSchemaUnitTests, addField_DuplicateName) {
    U2OpStatusImpl os;
    UdrSchema schema("id");
    schema.addField(UdrSchema::FieldDesc("name", UdrSchema::INTEGER), os);
    CHECK_NO_ERROR(os);
    schema.addField(UdrSchema::FieldDesc("name", UdrSchema::INTEGER), os);
    CHECK_TRUE(os.hasError(), "duplicate name");
}

IMPLEMENT_TEST(UdrSchemaUnitTests, addField_IncorrectName) {
    U2OpStatusImpl os;
    UdrSchema schema("id");
    schema.addField(UdrSchema::FieldDesc("field 1", UdrSchema::INTEGER), os);
    CHECK_TRUE(os.hasError(), "incorrect name");
}

IMPLEMENT_TEST(UdrSchemaUnitTests, addField_BLOB_1_not_indexed) {
    U2OpStatusImpl os;
    UdrSchema schema("id");

    schema.addField(UdrSchema::FieldDesc("name", UdrSchema::BLOB, UdrSchema::NOT_INDEXED), os);
    CHECK_NO_ERROR(os);
}

IMPLEMENT_TEST(UdrSchemaUnitTests, addField_BLOB_2_indexed) {
    U2OpStatusImpl os;
    UdrSchema schema("id");

    schema.addField(UdrSchema::FieldDesc("name", UdrSchema::BLOB, UdrSchema::INDEXED), os);
    CHECK_TRUE(os.hasError(), "indexed BLOB");
}

IMPLEMENT_TEST(UdrSchemaUnitTests, getField_OutOfRange_1_negative) {
    U2OpStatusImpl os;
    UdrSchema schema = getTestSchema();

    schema.getField(-10, os);
    CHECK_TRUE(os.hasError(), "out of range");
}

IMPLEMENT_TEST(UdrSchemaUnitTests, getField_OutOfRange_2_greater) {
    U2OpStatusImpl os;
    UdrSchema schema = getTestSchema();

    schema.getField(10, os);
    CHECK_TRUE(os.hasError(), "out of range");
}

IMPLEMENT_TEST(UdrSchemaUnitTests, getField_Range) {
    U2OpStatusImpl os;
    UdrSchema schema = getTestSchema();

    schema.getField(1, os);
    CHECK_NO_ERROR(os);
}

IMPLEMENT_TEST(UdrSchemaUnitTests, getField_Data) {
    U2OpStatusImpl os;
    UdrSchema schema = getTestSchema();

    UdrSchema::FieldDesc field1 = schema.getField(0, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE("name1" == field1.getName(), "name 1");
    CHECK_TRUE(UdrSchema::INTEGER == field1.getDataType(), "dataType 1");
    CHECK_TRUE(UdrSchema::INDEXED == field1.getIndexType(), "indexType 1");

    UdrSchema::FieldDesc field2 = schema.getField(1, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE("name2" == field2.getName(), "name 2");
    CHECK_TRUE(UdrSchema::BLOB == field2.getDataType(), "dataType 2");
    CHECK_TRUE(UdrSchema::NOT_INDEXED == field2.getIndexType(), "indexType 2");
}

IMPLEMENT_TEST(UdrSchemaUnitTests, addMultiIndex_DuplicateField_1) {
    U2OpStatusImpl os;
    UdrSchema schema = getTestSchema();

    QList<int> index; index << 0 << 0;
    schema.addMultiIndex(index, os);
    CHECK_TRUE(os.hasError(), "duplicate fields");
}

IMPLEMENT_TEST(UdrSchemaUnitTests, addMultiIndex_DuplicateField_2) {
    U2OpStatusImpl os;
    UdrSchema schema = getTestSchema();

    QList<int> index; index << 0 << 2 << 0;
    schema.addMultiIndex(index, os);
    CHECK_TRUE(os.hasError(), "duplicate fields");
}

IMPLEMENT_TEST(UdrSchemaUnitTests, addMultiIndex_OutOfRange_1) {
    U2OpStatusImpl os;
    UdrSchema schema = getTestSchema();

    QList<int> index; index << 3 << 4;
    schema.addMultiIndex(index, os);
    CHECK_TRUE(os.hasError(), "out of range");
}

IMPLEMENT_TEST(UdrSchemaUnitTests, addMultiIndex_OutOfRange_2) {
    U2OpStatusImpl os;
    UdrSchema schema = getTestSchema();

    QList<int> index; index << 0 << 2 << 3 << 4;
    schema.addMultiIndex(index, os);
    CHECK_TRUE(os.hasError(), "out of range");
}

IMPLEMENT_TEST(UdrSchemaUnitTests, addMultiIndex_OutOfRange_3_negative) {
    U2OpStatusImpl os;
    UdrSchema schema = getTestSchema();

    QList<int> index; index << 2 << -1;
    schema.addMultiIndex(index, os);
    CHECK_TRUE(os.hasError(), "out of range");
}

IMPLEMENT_TEST(UdrSchemaUnitTests, addMultiIndex_SingleColumn) {
    U2OpStatusImpl os;
    UdrSchema schema = getTestSchema();

    QList<int> index; index << 0;
    schema.addMultiIndex(index, os);
    CHECK_NO_ERROR(os);
}

IMPLEMENT_TEST(UdrSchemaUnitTests, addMultiIndex_AllColumns) {
    U2OpStatusImpl os;
    UdrSchema schema = getTestSchema2();

    QList<int> index; index << 0 << 1 << 2 << 3;
    schema.addMultiIndex(index, os);
    CHECK_NO_ERROR(os);
}

IMPLEMENT_TEST(UdrSchemaUnitTests, addMultiIndex_BackOrder) {
    U2OpStatusImpl os;
    UdrSchema schema = getTestSchema();

    QList<int> index; index << 3 << 0;
    schema.addMultiIndex(index, os);
    CHECK_NO_ERROR(os);
}

IMPLEMENT_TEST(UdrSchemaUnitTests, addMultiIndex_DuplicateIndex) {
    U2OpStatusImpl os;
    UdrSchema schema = getTestSchema();

    QList<int> index; index << 3 << 0;
    schema.addMultiIndex(index, os);
    CHECK_NO_ERROR(os);
    schema.addMultiIndex(index, os);
    CHECK_TRUE(os.hasError(), "duplicate index");
}

IMPLEMENT_TEST(UdrSchemaUnitTests, addMultiIndex_BLOB) {
    U2OpStatusImpl os;
    UdrSchema schema = getTestSchema();

    QList<int> index; index << 0 << 1;
    schema.addMultiIndex(index, os);
    CHECK_TRUE(os.hasError(), "indexed BLOB");
}

IMPLEMENT_TEST(UdrSchemaUnitTests, UdrValue_Double_getInt) {
    UdrValue v(51.0);
    U2OpStatusImpl os;
    v.getInt(os);
    CHECK_TRUE(os.hasError(), "no error");
}

IMPLEMENT_TEST(UdrSchemaUnitTests, UdrValue_String_getDouble) {
    UdrValue v(QString("test"));
    U2OpStatusImpl os;
    v.getDouble(os);
    CHECK_TRUE(os.hasError(), "no error");
}

IMPLEMENT_TEST(UdrSchemaUnitTests, UdrValue_Null_getInt) {
    UdrValue v;
    U2OpStatusImpl os;
    v.getInt(os);
    CHECK_TRUE(os.hasError(), "no error");
}

IMPLEMENT_TEST(UdrSchemaUnitTests, UdrValue_Null_getString) {
    UdrValue v;
    U2OpStatusImpl os;
    v.getString(os);
    CHECK_TRUE(os.hasError(), "no error");
}

IMPLEMENT_TEST(UdrSchemaUnitTests, UdrValue_Double_getDouble) {
    UdrValue v(102.0);
    U2OpStatusImpl os;
    CHECK_TRUE(102.0 == v.getDouble(os), "data mismatch");
    CHECK_NO_ERROR(os);
}

IMPLEMENT_TEST(UdrSchemaUnitTests, UdrValue_String_getDataId) {
    UdrValue v(QString("id"));
    U2OpStatusImpl os;
    v.getDataId(os);
    CHECK_TRUE(os.hasError(), "no error");
}

IMPLEMENT_TEST(UdrSchemaUnitTests, isCorrectName_CorrectName) {
    CHECK_TRUE(UdrSchemaRegistry::isCorrectName("name_Of_the_id_is_Correct_0001"), "name is correct");
}

IMPLEMENT_TEST(UdrSchemaUnitTests, isCorrectName_FirstDigit) {
    CHECK_FALSE(UdrSchemaRegistry::isCorrectName("51"), "first digit");
}

IMPLEMENT_TEST(UdrSchemaUnitTests, isCorrectName_FirstUnderscore) {
    CHECK_TRUE(UdrSchemaRegistry::isCorrectName("_Name_51"), "first underscore is correct");
}

IMPLEMENT_TEST(UdrSchemaUnitTests, isCorrectName_HasSpaces) {
    CHECK_FALSE(UdrSchemaRegistry::isCorrectName("name 1"), "has spaces");
}

IMPLEMENT_TEST(UdrSchemaUnitTests, registerSchema_IncorrectName) {
    UdrSchema schema("schema 1");
    U2OpStatusImpl os;
    AppContext::getUdrSchemaRegistry()->registerSchema(&schema, os);
    CHECK_TRUE(os.hasError(), "registered");
}

IMPLEMENT_TEST(UdrSchemaUnitTests, UdrSchema_ObjectReference) {
    UdrSchema schema("id", true);
    CHECK_TRUE(1 == schema.size(), "wrong size");
    CHECK_TRUE(schema.hasObjectReference(), "object reference");

    U2OpStatusImpl os;
    UdrSchema::FieldDesc field = schema.getField(0, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(UdrSchema::OBJECT_FIELD_NAME == field.getName(), "name");
    CHECK_TRUE(UdrSchema::ID == field.getDataType(), "dataType");
    CHECK_TRUE(UdrSchema::INDEXED == field.getIndexType(), "indexType");
}

IMPLEMENT_TEST(UdrSchemaUnitTests, UdrSchema_NoObjectReference) {
    UdrSchema schema("id");
    CHECK_TRUE(0 == schema.size(), "wrong size");
    CHECK_FALSE(schema.hasObjectReference(), "no object reference");
}

} // U2
