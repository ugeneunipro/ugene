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

#ifndef _U2_SQLITE_OBJECT_DBI_UNIT_TESTS_H_
#define _U2_SQLITE_OBJECT_DBI_UNIT_TESTS_H_

#include "core/dbi/DbiTest.h"

#include <unittest.h>


namespace U2 {

class SQLiteDbi;
class SQLiteObjectDbi;
class U2AttributeDbi;
class U2MsaDbi;
class U2SequenceDbi;

class SQLiteObjectDbiTestData {
public:
    static void init();

    static void shutdown();

    static SQLiteDbi* getSQLiteDbi();
    static SQLiteObjectDbi* getSQLiteObjectDbi();
    static U2AttributeDbi* getAttributeDbi();
    static U2MsaDbi* getMsaDbi();
    static U2SequenceDbi* getSequenceDbi();

    static U2DataId createTestMsa(bool enableModTracking, U2OpStatus& os);
    static void addTestRow(const U2DataId& msaId, U2OpStatus& os);

private:
    static TestDbiProvider dbiProvider;
    static const QString& SQLITE_OBJ_DB_URL;
    static U2AttributeDbi* attributeDbi;
    static U2MsaDbi* msaDbi;
    static U2SequenceDbi* sequenceDbi;
    static SQLiteObjectDbi* sqliteObjectDbi;
    static SQLiteDbi* sqliteDbi;
};

/**
 * Remove a MSA object from the database, the following must be removed:
 *   1) Records in "Attribute" table (MSA info)
 *   2) Records in "StringAttribute"
 *   3) Records in "MsaRow"
 *   4) Records in "MsaRowGap"
 *   5) Records in "Sequence" (not used by other objects)
 *   6) The record in "Msa"
 *   7) The record in "Object"
 * Records of another MSA object are not removed.
 */
DECLARE_TEST(SQLiteObjectDbiUnitTests, removeMsaObject);
DECLARE_TEST(SQLiteObjectDbiUnitTests, setTrackModType);

/**
 * canUndoRedo: check canUndo() and canRedo() functions.
 *  ^ noTrack                   - modification tracking is turned off: no undo, no redo.
 *  ^ noAction                  - no action happened with object after creation: no undo, no redo.
 *  ^ lastState                 - object is in the last state: undo, no redo.
 *  ^ firstState                - object is in the first state: no undo, redo.
 *  ^ midState                  - object is in the middle state: undo, redo.
 *  ^ oneUserStep               - object gets two actions with one user step, then undo: no undo, redo.
 */
DECLARE_TEST(SQLiteObjectDbiUnitTests, canUndoRedo_noTrack);
DECLARE_TEST(SQLiteObjectDbiUnitTests, canUndoRedo_noAction);
DECLARE_TEST(SQLiteObjectDbiUnitTests, canUndoRedo_lastState);
DECLARE_TEST(SQLiteObjectDbiUnitTests, canUndoRedo_firstState);
DECLARE_TEST(SQLiteObjectDbiUnitTests, canUndoRedo_midState);
DECLARE_TEST(SQLiteObjectDbiUnitTests, canUndoRedo_oneUserStep);

/**
 * Common tests for undo/redo and modification steps.
 *   ^ user2MultiVersions    - verify the master object version, version in the userModStep and
 *                             canUndo/canRedo when there is one user mod step with 3 multi steps.
 *   ^ actionAfterUndo       - verify that modification steps are removed properly when an action is
 *                             done after undo. Verify undo/redo after this.
 *   ^ actionUndoActionUndo1 - do an action, undo it, do another action, undo it. User steps are created manually.
 *   ^ actionUndoActionUndo2 - do an action, undo it, do another action, undo it. User steps are created automatically.
 *   ^ actionUndoActionUndo3 - do an action (userModStep is created automatically), undo, do an action (userModStep
 *                             is created manually), undo
 *   ^ actionUndoActionUndo4 - do an action (userModStep is created manually), undo, do an action (userModStep
 *                             is created automatically), undo
 *   ^ user3Single6          - create a user step manually (with 2 multi/single steps), create a user step
 *                             automatically (with 1 multi/step step), create 3rd user step manually
 *                             with 2 multi steps and 3 single steps (i.e. add row + update row content).
 *                             Do undo/redo. Verify versions and canUndo/canRedo.
 */
DECLARE_TEST(SQLiteObjectDbiUnitTests, commonUndoRedo_user3Multi);
DECLARE_TEST(SQLiteObjectDbiUnitTests, commonUndoRedo_actionAfterUndo);
DECLARE_TEST(SQLiteObjectDbiUnitTests, commonUndoRedo_actionUndoActionUndo1);
DECLARE_TEST(SQLiteObjectDbiUnitTests, commonUndoRedo_actionUndoActionUndo2);
DECLARE_TEST(SQLiteObjectDbiUnitTests, commonUndoRedo_actionUndoActionUndo3);
DECLARE_TEST(SQLiteObjectDbiUnitTests, commonUndoRedo_actionUndoActionUndo4);
DECLARE_TEST(SQLiteObjectDbiUnitTests, commonUndoRedo_user3Single6);


} // namespace

DECLARE_METATYPE(SQLiteObjectDbiUnitTests, removeMsaObject);
DECLARE_METATYPE(SQLiteObjectDbiUnitTests, setTrackModType);

DECLARE_METATYPE(SQLiteObjectDbiUnitTests, canUndoRedo_noTrack);
DECLARE_METATYPE(SQLiteObjectDbiUnitTests, canUndoRedo_noAction);
DECLARE_METATYPE(SQLiteObjectDbiUnitTests, canUndoRedo_lastState);
DECLARE_METATYPE(SQLiteObjectDbiUnitTests, canUndoRedo_firstState);
DECLARE_METATYPE(SQLiteObjectDbiUnitTests, canUndoRedo_midState);
DECLARE_METATYPE(SQLiteObjectDbiUnitTests, canUndoRedo_oneUserStep);

DECLARE_METATYPE(SQLiteObjectDbiUnitTests, commonUndoRedo_user3Multi);
DECLARE_METATYPE(SQLiteObjectDbiUnitTests, commonUndoRedo_actionAfterUndo);
DECLARE_METATYPE(SQLiteObjectDbiUnitTests, commonUndoRedo_actionUndoActionUndo1);
DECLARE_METATYPE(SQLiteObjectDbiUnitTests, commonUndoRedo_actionUndoActionUndo2);
DECLARE_METATYPE(SQLiteObjectDbiUnitTests, commonUndoRedo_actionUndoActionUndo3);
DECLARE_METATYPE(SQLiteObjectDbiUnitTests, commonUndoRedo_actionUndoActionUndo4);
DECLARE_METATYPE(SQLiteObjectDbiUnitTests, commonUndoRedo_user3Single6);

#endif
