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

#ifndef _U2_UDR_DBI_UNIT_TESTS_H_
#define _U2_UDR_DBI_UNIT_TESTS_H_

#include "core/dbi/DbiTest.h"

#include <unittest.h>

#include <U2Core/UdrSchema.h>

namespace U2 {

class UdrDbi;

class UdrTestData {
public:
    static void shutdown();

    static UdrDbi * getUdrDbi();

    static U2DataId id1;
    static U2DataId id2;
    static U2DataId id_2;
    static QByteArray dataSchema2;
    static U2DataId obj1Schema3;
    static U2DataId obj2Schema3;

private:
    static TestDbiProvider dbiProvider;
    static const QString &UDR_DB_URL;
    static UdrDbi *udrDbi;

private:
    static void init();
    static void initTestUdr();
    static void initTestData();
};

/* Getting records */
DECLARE_TEST(UdrDbiUnitTests, getRecord);
/* Adding one record */
DECLARE_TEST(UdrDbiUnitTests, addRecord_1);
/* Adding two records */
DECLARE_TEST(UdrDbiUnitTests, addRecord_2);
/* Adding a record with adapter */
DECLARE_TEST(UdrDbiUnitTests, addRecord_with_adapter);
/* Removing a record */
DECLARE_TEST(UdrDbiUnitTests, removeRecord);
/* Writing data to BLOB with adapter 1 */
DECLARE_TEST(UdrDbiUnitTests, OutputStream_write);
/* Writing data to BLOB with adapter 2 */
DECLARE_TEST(UdrDbiUnitTests, OutputStream_write_2);
/* Reading data partially from BLOB with adapter 1 */
DECLARE_TEST(UdrDbiUnitTests, InputStream_read);
/* Reading data partially from BLOB with adapter 2 */
DECLARE_TEST(UdrDbiUnitTests, InputStream_skip_read);
/* Skipping data from BLOB with adapter 1 */
DECLARE_TEST(UdrDbiUnitTests, InputStream_skip_OutOfRange_1);
/* Skipping data from BLOB with adapter 2 */
DECLARE_TEST(UdrDbiUnitTests, InputStream_skip_OutOfRange_2_negative);
/* Skipping data from BLOB with adapter 3 */
DECLARE_TEST(UdrDbiUnitTests, InputStream_skip_Range_1);
/* Skipping data from BLOB with adapter 4 */
DECLARE_TEST(UdrDbiUnitTests, InputStream_skip_Range_2_negative);
/* createObject 1 */
DECLARE_TEST(UdrDbiUnitTests, createObject);
/* createObject 2 */
DECLARE_TEST(UdrDbiUnitTests, createObject_removeObject);
/* getObjectRecords 1 */
DECLARE_TEST(UdrDbiUnitTests, getObjectRecords_1);
/* getObjectRecords 2 */
DECLARE_TEST(UdrDbiUnitTests, getObjectRecords_2);

} // U2

DECLARE_METATYPE(UdrDbiUnitTests, getRecord);
DECLARE_METATYPE(UdrDbiUnitTests, addRecord_1);
DECLARE_METATYPE(UdrDbiUnitTests, addRecord_2);
DECLARE_METATYPE(UdrDbiUnitTests, addRecord_with_adapter);
DECLARE_METATYPE(UdrDbiUnitTests, removeRecord);
DECLARE_METATYPE(UdrDbiUnitTests, OutputStream_write);
DECLARE_METATYPE(UdrDbiUnitTests, OutputStream_write_2);
DECLARE_METATYPE(UdrDbiUnitTests, InputStream_read);
DECLARE_METATYPE(UdrDbiUnitTests, InputStream_skip_read);
DECLARE_METATYPE(UdrDbiUnitTests, InputStream_skip_OutOfRange_1);
DECLARE_METATYPE(UdrDbiUnitTests, InputStream_skip_OutOfRange_2_negative);
DECLARE_METATYPE(UdrDbiUnitTests, InputStream_skip_Range_1);
DECLARE_METATYPE(UdrDbiUnitTests, InputStream_skip_Range_2_negative);
DECLARE_METATYPE(UdrDbiUnitTests, createObject);
DECLARE_METATYPE(UdrDbiUnitTests, createObject_removeObject);
DECLARE_METATYPE(UdrDbiUnitTests, getObjectRecords_1);
DECLARE_METATYPE(UdrDbiUnitTests, getObjectRecords_2);

#endif // _U2_UDR_DBI_UNIT_TESTS_H_
