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

#ifndef _U2_RAW_DATA_UDR_SCHEMA_UNIT_TESTS_H_
#define _U2_RAW_DATA_UDR_SCHEMA_UNIT_TESTS_H_

#include <unittest.h>

#include "core/dbi/DbiTest.h"

namespace U2 {

class RawDataUdrSchemaTestData {
public:
    static void shutdown();
    static U2DbiRef getDbiRef();
    static U2EntityRef getObjRef();

private:
    static TestDbiProvider dbiProvider;
    static const QString UDR_DB_URL;
    static bool inited;
    static U2EntityRef objRef;

private:
    static void init();
    static void initData();
};

/* getObject 1 */
DECLARE_TEST(RawDataUdrSchemaUnitTests, getObject);
/* getObject 2 */
DECLARE_TEST(RawDataUdrSchemaUnitTests, getObject_Null);
/* readAllContent 1 */
DECLARE_TEST(RawDataUdrSchemaUnitTests, readAllContent);
/* readAllContent 2 */
DECLARE_TEST(RawDataUdrSchemaUnitTests, readAllContent_Null);
/* createObject */
DECLARE_TEST(RawDataUdrSchemaUnitTests, createObject);
/* writeContent 1 */
DECLARE_TEST(RawDataUdrSchemaUnitTests, writeContent);
/* writeContent 2 */
DECLARE_TEST(RawDataUdrSchemaUnitTests, writeContent_Null);

} // U2

DECLARE_METATYPE(RawDataUdrSchemaUnitTests, getObject);
DECLARE_METATYPE(RawDataUdrSchemaUnitTests, getObject_Null);
DECLARE_METATYPE(RawDataUdrSchemaUnitTests, readAllContent);
DECLARE_METATYPE(RawDataUdrSchemaUnitTests, readAllContent_Null);
DECLARE_METATYPE(RawDataUdrSchemaUnitTests, createObject);
DECLARE_METATYPE(RawDataUdrSchemaUnitTests, writeContent);
DECLARE_METATYPE(RawDataUdrSchemaUnitTests, writeContent_Null);

#endif // _U2_RAW_DATA_UDR_SCHEMA_UNIT_TESTS_H_
