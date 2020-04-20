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

#ifndef _U2_UDR_SCHEMA_UNIT_TESTS_H_
#define _U2_UDR_SCHEMA_UNIT_TESTS_H_

#include <unittest.h>

namespace U2 {

/* Can not add field with the name == record_id */
DECLARE_TEST(UdrSchemaUnitTests, addField_RecordId);
/* Can not add field with the name == object to the schema with object reference */
DECLARE_TEST(UdrSchemaUnitTests, addField_Object);
/* Can not add fields with similar names */
DECLARE_TEST(UdrSchemaUnitTests, addField_DuplicateName);
/* Can not add a field incorrect name */
DECLARE_TEST(UdrSchemaUnitTests, addField_IncorrectName);
/* Add BLOB 1 */
DECLARE_TEST(UdrSchemaUnitTests, addField_BLOB_1_not_indexed);
/* Add BLOB 2 */
DECLARE_TEST(UdrSchemaUnitTests, addField_BLOB_2_indexed);
/* Get field arguments 1 */
DECLARE_TEST(UdrSchemaUnitTests, getField_OutOfRange_1_negative);
/* Get field arguments 2 */
DECLARE_TEST(UdrSchemaUnitTests, getField_OutOfRange_2_greater);
/* Get field arguments 3 */
DECLARE_TEST(UdrSchemaUnitTests, getField_Range);
/* Get field data */
DECLARE_TEST(UdrSchemaUnitTests, getField_Data);
/* Multi index content 1 */
DECLARE_TEST(UdrSchemaUnitTests, addMultiIndex_DuplicateField_1);
/* Multi index content 2 */
DECLARE_TEST(UdrSchemaUnitTests, addMultiIndex_DuplicateField_2);
/* Multi index content 3 */
DECLARE_TEST(UdrSchemaUnitTests, addMultiIndex_OutOfRange_1);
/* Multi index content 4 */
DECLARE_TEST(UdrSchemaUnitTests, addMultiIndex_OutOfRange_2);
/* Multi index content 5 */
DECLARE_TEST(UdrSchemaUnitTests, addMultiIndex_OutOfRange_3_negative);
/* Multi index content 6 */
DECLARE_TEST(UdrSchemaUnitTests, addMultiIndex_SingleColumn);
/* Multi index content 7 */
DECLARE_TEST(UdrSchemaUnitTests, addMultiIndex_AllColumns);
/* Multi index content 8 */
DECLARE_TEST(UdrSchemaUnitTests, addMultiIndex_BackOrder);
/* Multi index content 9 */
DECLARE_TEST(UdrSchemaUnitTests, addMultiIndex_DuplicateIndex);
/* Multi index content 10 */
DECLARE_TEST(UdrSchemaUnitTests, addMultiIndex_BLOB);
/* UdrValue types mismatches 1 */
DECLARE_TEST(UdrSchemaUnitTests, UdrValue_Double_getInt);
/* UdrValue types mismatches 2 */
DECLARE_TEST(UdrSchemaUnitTests, UdrValue_String_getDouble);
/* UdrValue types mismatches 3 */
DECLARE_TEST(UdrSchemaUnitTests, UdrValue_Null_getInt);
/* UdrValue types mismatches 4 */
DECLARE_TEST(UdrSchemaUnitTests, UdrValue_Null_getString);
/* UdrValue types mismatches 5 */
DECLARE_TEST(UdrSchemaUnitTests, UdrValue_Double_getDouble);
/* UdrValue types mismatches 6 */
DECLARE_TEST(UdrSchemaUnitTests, UdrValue_String_getDataId);
/* Schema and field name checker 1 */
DECLARE_TEST(UdrSchemaUnitTests, isCorrectName_CorrectName);
/* Schema and field name checker 2 */
DECLARE_TEST(UdrSchemaUnitTests, isCorrectName_FirstDigit);
/* Schema and field name checker 3 */
DECLARE_TEST(UdrSchemaUnitTests, isCorrectName_FirstUnderscore);
/* Schema and field name checker 4 */
DECLARE_TEST(UdrSchemaUnitTests, isCorrectName_HasSpaces);
/* Schema and field name checker 5 */
DECLARE_TEST(UdrSchemaUnitTests, registerSchema_IncorrectName);
/* Schema with object reference */
DECLARE_TEST(UdrSchemaUnitTests, UdrSchema_ObjectReference);
/* Schema without object reference */
DECLARE_TEST(UdrSchemaUnitTests, UdrSchema_NoObjectReference);

} // U2

DECLARE_METATYPE(UdrSchemaUnitTests, addField_RecordId);
DECLARE_METATYPE(UdrSchemaUnitTests, addField_Object);
DECLARE_METATYPE(UdrSchemaUnitTests, addField_DuplicateName);
DECLARE_METATYPE(UdrSchemaUnitTests, addField_IncorrectName);
DECLARE_METATYPE(UdrSchemaUnitTests, addField_BLOB_1_not_indexed);
DECLARE_METATYPE(UdrSchemaUnitTests, addField_BLOB_2_indexed);
DECLARE_METATYPE(UdrSchemaUnitTests, getField_OutOfRange_1_negative);
DECLARE_METATYPE(UdrSchemaUnitTests, getField_OutOfRange_2_greater);
DECLARE_METATYPE(UdrSchemaUnitTests, getField_Range);
DECLARE_METATYPE(UdrSchemaUnitTests, getField_Data);
DECLARE_METATYPE(UdrSchemaUnitTests, addMultiIndex_DuplicateField_1);
DECLARE_METATYPE(UdrSchemaUnitTests, addMultiIndex_DuplicateField_2);
DECLARE_METATYPE(UdrSchemaUnitTests, addMultiIndex_OutOfRange_1);
DECLARE_METATYPE(UdrSchemaUnitTests, addMultiIndex_OutOfRange_2);
DECLARE_METATYPE(UdrSchemaUnitTests, addMultiIndex_OutOfRange_3_negative);
DECLARE_METATYPE(UdrSchemaUnitTests, addMultiIndex_SingleColumn);
DECLARE_METATYPE(UdrSchemaUnitTests, addMultiIndex_AllColumns);
DECLARE_METATYPE(UdrSchemaUnitTests, addMultiIndex_BackOrder);
DECLARE_METATYPE(UdrSchemaUnitTests, addMultiIndex_DuplicateIndex);
DECLARE_METATYPE(UdrSchemaUnitTests, addMultiIndex_BLOB);
DECLARE_METATYPE(UdrSchemaUnitTests, UdrValue_Double_getInt);
DECLARE_METATYPE(UdrSchemaUnitTests, UdrValue_String_getDouble);
DECLARE_METATYPE(UdrSchemaUnitTests, UdrValue_Null_getInt);
DECLARE_METATYPE(UdrSchemaUnitTests, UdrValue_Null_getString);
DECLARE_METATYPE(UdrSchemaUnitTests, UdrValue_Double_getDouble);
DECLARE_METATYPE(UdrSchemaUnitTests, UdrValue_String_getDataId);
DECLARE_METATYPE(UdrSchemaUnitTests, isCorrectName_CorrectName);
DECLARE_METATYPE(UdrSchemaUnitTests, isCorrectName_FirstDigit);
DECLARE_METATYPE(UdrSchemaUnitTests, isCorrectName_FirstUnderscore);
DECLARE_METATYPE(UdrSchemaUnitTests, isCorrectName_HasSpaces);
DECLARE_METATYPE(UdrSchemaUnitTests, registerSchema_IncorrectName);
DECLARE_METATYPE(UdrSchemaUnitTests, UdrSchema_ObjectReference);
DECLARE_METATYPE(UdrSchemaUnitTests, UdrSchema_NoObjectReference);

#endif // _U2_UDR_SCHEMA_UNIT_TESTS_H_
