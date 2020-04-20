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

#ifndef _U2_SEQUENCE_DBI_SQLITE_SPECIFIC_UNIT_TESTS_H_
#define _U2_SEQUENCE_DBI_SQLITE_SPECIFIC_UNIT_TESTS_H_

#include "core/dbi/DbiTest.h"

#include <unittest.h>


namespace U2 {

class SQLiteDbi;
class U2SequenceDbi;

class SequenceSQLiteSpecificTestData {
public:
    static void init();
    static void shutdown();

    static SQLiteDbi* getSQLiteDbi();

    static U2DataId createTestSequence(bool enableModTracking, qint64 seqLength, U2OpStatus& os);
    static U2DataId createTestSequence(bool enableModTracking, const QByteArray& seqData, U2OpStatus& os);

    static qint64 getModStepsNum(const U2DataId& objId, U2OpStatus& os);

    static const QString TEST_SEQUENCE_NAME;

private:

    static TestDbiProvider dbiProvider;
    static const QString& SQLITE_SEQUENCE_DB_URL;
    static SQLiteDbi* sqliteDbi;
};

///////////////////////////////////////////////////////////////
// Tests for U2SequenceDbi API that assume that SQLiteDbi was used.
// The tests verify the correct structure of the SQLiteDbi
// after some operations.
///////////////////////////////////////////////////////////////

/**
  * Update a sequence data:
  *     ^ noModTrack      - update the sequence with disabled modification tracking,
  *                         there are no modSteps.
  *     ^ empty           - the sequence is empty.
  *     ^ clearHint       - the sequence is not empty, all data is removed, an appropriate hint is used.
  *     ^ clearNoHint     - the sequence is not empty, all data is removed, there are no hints.
  *     ^ begin           - the part at the beginning of the sequence is updated.
  *     ^ middle          - the part in the middle of the sequence is updated.
  *     ^ middleNoLength  - the part in the middle of the sequence is updated,
  *                         length is not changed, an appropriate hint is used.
  *     ^ end             - the part at the end of the sequence is updated.
  */
DECLARE_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_noModTrack);
DECLARE_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_emptyHint);
DECLARE_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_emptyNoHint);
DECLARE_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_clear);
DECLARE_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_begin);
DECLARE_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_middle);
DECLARE_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_middle_middleNoLength);
DECLARE_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_end);

DECLARE_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_noModTrack_undo);
DECLARE_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_emptyHint_undo);
DECLARE_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_emptyNoHint_undo);
DECLARE_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_clear_undo);
DECLARE_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_begin_undo);
DECLARE_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_middle_undo);
DECLARE_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_middle_middleNoLength_undo);
DECLARE_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_end_undo);

DECLARE_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_noModTrack_redo);
DECLARE_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_emptyHint_redo);
DECLARE_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_emptyNoHint_redo);
DECLARE_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_clear_redo);
DECLARE_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_begin_redo);
DECLARE_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_middle_redo);
DECLARE_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_middle_middleNoLength_redo);
DECLARE_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_end_redo);

} // namespace


DECLARE_METATYPE(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_noModTrack);
DECLARE_METATYPE(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_emptyHint);
DECLARE_METATYPE(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_emptyNoHint);
DECLARE_METATYPE(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_clear);
DECLARE_METATYPE(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_begin);
DECLARE_METATYPE(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_middle);
DECLARE_METATYPE(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_middle_middleNoLength);
DECLARE_METATYPE(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_end);

DECLARE_METATYPE(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_noModTrack_undo);
DECLARE_METATYPE(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_emptyHint_undo);
DECLARE_METATYPE(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_emptyNoHint_undo);
DECLARE_METATYPE(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_clear_undo);
DECLARE_METATYPE(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_begin_undo);
DECLARE_METATYPE(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_middle_undo);
DECLARE_METATYPE(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_middle_middleNoLength_undo);
DECLARE_METATYPE(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_end_undo);

DECLARE_METATYPE(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_noModTrack_redo);
DECLARE_METATYPE(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_emptyHint_redo);
DECLARE_METATYPE(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_emptyNoHint_redo);
DECLARE_METATYPE(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_clear_redo);
DECLARE_METATYPE(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_begin_redo);
DECLARE_METATYPE(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_middle_redo);
DECLARE_METATYPE(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_middle_middleNoLength_redo);
DECLARE_METATYPE(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_end_redo);

#endif  // _U2_SEQUENCE_DBI_SQLITE_SPECIFIC_UNIT_TESTS_H_
