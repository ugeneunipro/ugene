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

#include "SQLiteObjectDbiUnitTests.h"
#include "core/util/MsaDbiUtilsUnitTests.h"

#include <U2Core/DNAAlphabet.h>
#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2MsaDbi.h>
#include <U2Core/U2SequenceDbi.h>
#include <U2Core/U2SequenceUtils.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Formats/SQLiteDbi.h>
#include <U2Formats/SQLiteObjectDbi.h>
#include <U2Formats/SQLiteSequenceDbi.h>


namespace U2 {

TestDbiProvider SQLiteObjectDbiTestData::dbiProvider = TestDbiProvider();
const QString& SQLiteObjectDbiTestData::SQLITE_OBJ_DB_URL("sqlite-obj-dbi.ugenedb");
U2AttributeDbi* SQLiteObjectDbiTestData::attributeDbi = NULL;
U2MsaDbi* SQLiteObjectDbiTestData::msaDbi = NULL;
U2SequenceDbi* SQLiteObjectDbiTestData::sequenceDbi = NULL;
SQLiteDbi* SQLiteObjectDbiTestData::sqliteDbi = NULL;
SQLiteObjectDbi* SQLiteObjectDbiTestData::sqliteObjectDbi = NULL;

void SQLiteObjectDbiTestData::init() {
    SAFE_POINT(NULL == sqliteDbi, "sqliteDbi has already been initialized!", );

    // Get URL
    bool ok = dbiProvider.init(SQLITE_OBJ_DB_URL, false);
    SAFE_POINT(ok, "Dbi provider failed to initialize in MsaTestData::init()!",);

    U2Dbi* dbi = dbiProvider.getDbi();
    QString url = dbi->getDbiRef().dbiId;
    dbiProvider.close();

    // Init DBI
    sqliteDbi = new SQLiteDbi();
    QHash<QString, QString> initProperties;
    initProperties[U2DbiOptions::U2_DBI_OPTION_URL] = url;
    U2OpStatusImpl os;
    sqliteDbi->init(initProperties, QVariantMap(), os);
    SAFE_POINT_OP(os, );

    sqliteObjectDbi = sqliteDbi->getSQLiteObjectDbi();
    SAFE_POINT(NULL != sqliteObjectDbi, "Failed to get sqliteObjectDbi!",);

    attributeDbi = sqliteDbi->getAttributeDbi();
    SAFE_POINT(NULL != attributeDbi, "Failed to get attributeDbi!",);

    msaDbi = sqliteDbi->getMsaDbi();
    SAFE_POINT(NULL != msaDbi, "Failed to get msaDbi!",);

    sequenceDbi = sqliteDbi->getSequenceDbi();
    SAFE_POINT(NULL != sequenceDbi, "Failed to get sequenceDbi!",);
}

void SQLiteObjectDbiTestData::shutdown() {
    if (NULL != sqliteDbi) {

        delete sqliteDbi;

        sqliteDbi = NULL;
        sqliteObjectDbi = NULL;
        attributeDbi = NULL;
        msaDbi = NULL;
        sequenceDbi = NULL;
    }
}

SQLiteDbi* SQLiteObjectDbiTestData::getSQLiteDbi() {
    if (NULL == sqliteDbi) {
        init();
    }
    return sqliteDbi;
}

SQLiteObjectDbi* SQLiteObjectDbiTestData::getSQLiteObjectDbi() {
    if (NULL == sqliteObjectDbi) {
        init();
    }
    return sqliteObjectDbi;
}

U2AttributeDbi* SQLiteObjectDbiTestData::getAttributeDbi() {
    if (NULL == attributeDbi) {
        init();
    }
    return attributeDbi;
}

U2MsaDbi* SQLiteObjectDbiTestData::getMsaDbi() {
    if (NULL == msaDbi) {
        init();
    }
    return msaDbi;
}

U2SequenceDbi* SQLiteObjectDbiTestData::getSequenceDbi() {
    if (NULL == sequenceDbi) {
        init();
    }
    return sequenceDbi;
}

U2DataId SQLiteObjectDbiTestData::createTestMsa(bool enableModTracking, U2OpStatus& os) {
    // Create an alignment
    U2AlphabetId alphabet = BaseDNAAlphabetIds::NUCL_DNA_DEFAULT();
    U2DataId msaId = sqliteDbi->getMsaDbi()->createMsaObject("", "Test alignment", alphabet, os);
    CHECK_OP(os, U2DataId());

    if (enableModTracking) {
        sqliteDbi->getObjectDbi()->setTrackModType(msaId, TrackOnUpdate, os);
        CHECK_OP(os, U2DataId());
    }

    return msaId;
}

void SQLiteObjectDbiTestData::addTestRow(const U2DataId& msaId, U2OpStatus& os) {
    U2Sequence seq;
    seq.alphabet = BaseDNAAlphabetIds::NUCL_DNA_DEFAULT();
    seq.circular = false;
    seq.trackModType = NoTrack;
    seq.visualName = "Test sequence";
    sqliteDbi->getSQLiteSequenceDbi()->createSequenceObject(seq, "", os, U2DbiObjectRank_TopLevel);
    SAFE_POINT_OP(os, );

    U2MsaRow row;
    row.sequenceId = seq.id;
    row.gstart = 0;
    row.gend = 0;
    row.length = 0;

    sqliteDbi->getMsaDbi()->addRow(msaId, -1, row, os);
    SAFE_POINT_OP(os, );
}



IMPLEMENT_TEST(SQLiteObjectDbiUnitTests, removeMsaObject) {
    U2OpStatusImpl os;
    U2MsaDbi* msaDbi = SQLiteObjectDbiTestData::getMsaDbi();

    // FIRST ALIGNMENT
    // Create an alignment
    U2DataId msaId = msaDbi->createMsaObject("", "Test name", BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), os);
    CHECK_NO_ERROR(os);

    // Add alignment info
    U2StringAttribute attr(msaId, "MSA1 info key", "MSA1 info value");
    U2AttributeDbi* attrDbi = SQLiteObjectDbiTestData::getAttributeDbi();
    attrDbi->createStringAttribute(attr, os);
    CHECK_NO_ERROR(os);

    // Create sequences
    U2SequenceDbi* sequenceDbi = SQLiteObjectDbiTestData::getSequenceDbi();
    U2Sequence seq1;
    U2Sequence seq2;
    sequenceDbi->createSequenceObject(seq1, "", os);
    CHECK_NO_ERROR(os);
    sequenceDbi->createSequenceObject(seq2, "", os);
    CHECK_NO_ERROR(os);

    // Add rows
    U2MsaRow row1;
    row1.rowId = 0;
    row1.sequenceId = seq1.id;
    row1.gstart = 0;
    row1.gend = 5;

    U2MsaGap row1gap1(0, 2);
    U2MsaGap row1gap2(3, 1);
    QList<U2MsaGap> row1gaps;
    row1gaps << row1gap1 << row1gap2;

    row1.gaps = row1gaps;

    U2MsaRow row2;
    row2.rowId = 1;
    row2.sequenceId = seq2.id;
    row2.gstart = 2;
    row2.gend = 4;

    U2MsaGap row2gap(1, 2);
    QList<U2MsaGap> row2gaps;
    row2gaps << row2gap;

    row2.gaps = row2gaps;

    QList<U2MsaRow> rows;
    rows << row1 << row2;

    msaDbi->addRows(msaId, rows, os);
    CHECK_NO_ERROR(os);

    // SECOND ALIGNMENT
    // Create an alignment
    U2DataId msaId2 = msaDbi->createMsaObject("", "Test name 2", BaseDNAAlphabetIds::AMINO_DEFAULT(), os);
    CHECK_NO_ERROR(os);

    // Add alignment info
    U2StringAttribute attr2(msaId2, "MSA2 info key", "MSA2 info value");
    attrDbi->createStringAttribute(attr2, os);
    CHECK_NO_ERROR(os);

    // Create sequences
    U2Sequence al2Seq;
    sequenceDbi->createSequenceObject(al2Seq, "", os);
    CHECK_NO_ERROR(os);

    // Add rows
    U2MsaRow al2Row;
    al2Row.rowId = 0;
    al2Row.sequenceId = al2Seq.id;
    al2Row.gstart = 0;
    al2Row.gend = 15;

    U2MsaGap al2RowGap(1, 12);
    QList<U2MsaGap> al2RowGaps;
    al2RowGaps << al2RowGap;

    al2Row.gaps = al2RowGaps;

    QList<U2MsaRow> al2Rows;
    al2Rows << al2Row;

    msaDbi->addRows(msaId2, al2Rows, os);
    CHECK_NO_ERROR(os);

    // REMOVE THE FIRST ALIGNMENT OBJECT
    SQLiteObjectDbi* sqliteObjectDbi = SQLiteObjectDbiTestData::getSQLiteObjectDbi();
    sqliteObjectDbi->removeObject(msaId, os);

    // VERIFY THAT THERE IS ONLY THE SECOND ALIGNMENT'S RECORDS LEFT IN TABLES
    SQLiteDbi* sqliteDbi = SQLiteObjectDbiTestData::getSQLiteDbi();

    // "Attribute"
    SQLiteReadQuery qAttr("SELECT COUNT(*) FROM Attribute WHERE name = ?1", sqliteDbi->getDbRef(), os);
    qAttr.bindString(1, "MSA1 info key");
    qint64 msa1AttrNum = qAttr.selectInt64();
    CHECK_EQUAL(0, msa1AttrNum, "MSA1 attributes number");

    qAttr.reset(true);
    qAttr.bindString(1, "MSA2 info key");
    qint64 msa2AttrNum = qAttr.selectInt64();
    CHECK_EQUAL(1, msa2AttrNum, "MSA2 attributes number");

    // "StringAttribute"
    SQLiteReadQuery qStringAttr("SELECT COUNT(*) FROM StringAttribute WHERE value = ?1", sqliteDbi->getDbRef(), os);
    qStringAttr.bindString(1, "MSA1 info value");
    qint64 msa1StrAttrNum = qStringAttr.selectInt64();
    CHECK_EQUAL(0, msa1StrAttrNum, "MSA1 string attributes number");

    qStringAttr.reset(true);
    qStringAttr.bindString(1, "MSA2 info value");
    qint64 msa2StrAttrNum = qStringAttr.selectInt64();
    CHECK_EQUAL(1, msa2StrAttrNum, "MSA2 string attributes number");

    // "MsaRow"
    SQLiteReadQuery qMsaRow("SELECT COUNT(*) FROM MsaRow WHERE msa = ?1", sqliteDbi->getDbRef(), os);
    qMsaRow.bindDataId(1, msaId);
    qint64 msa1Rows = qMsaRow.selectInt64();
    CHECK_EQUAL(0, msa1Rows, "number of rows in MSA1");

    qMsaRow.reset(true);
    qMsaRow.bindDataId(1, msaId2);
    qint64 msa2Rows = qMsaRow.selectInt64();
    CHECK_EQUAL(1, msa2Rows, "number of rows in MSA2");

    // "MsaRowGap"
    SQLiteReadQuery qMsaRowGap("SELECT COUNT(*) FROM MsaRowGap WHERE msa = ?1", sqliteDbi->getDbRef(), os);
    qMsaRowGap.bindDataId(1, msaId);
    qint64 msa1Gaps = qMsaRowGap.selectInt64();
    CHECK_EQUAL(0, msa1Gaps, "number of gaps in MSA1 rows");

    qMsaRowGap.reset(true);
    qMsaRowGap.bindDataId(1, msaId2);
    qint64 msa2Gaps = qMsaRowGap.selectInt64();
    CHECK_EQUAL(1, msa2Gaps, "number of gaps in MSA2 rows");

    // "Sequence"
    SQLiteReadQuery qSeq("SELECT COUNT(*) FROM Sequence WHERE object = ?1", sqliteDbi->getDbRef(), os);
    qSeq.bindDataId(1, seq1.id);
    qint64 msa1seq1 = qSeq.selectInt64();
    CHECK_EQUAL(0, msa1seq1, "seq1 of msa1");

    qSeq.reset(true);
    qSeq.bindDataId(1, seq2.id);
    qint64 msa1seq2 = qSeq.selectInt64();
    CHECK_EQUAL(0, msa1seq2, "seq2 of msa1");

    qSeq.reset(true);
    qSeq.bindDataId(1, al2Seq.id);
    qint64 msa2seq = qSeq.selectInt64();
    CHECK_EQUAL(1, msa2seq, "seq of msa2");

    // "Msa"
    SQLiteReadQuery qMsa("SELECT COUNT(*) FROM Msa WHERE object = ?1", sqliteDbi->getDbRef(), os);
    qMsa.bindDataId(1, msaId);
    qint64 msa1records = qMsa.selectInt64();
    CHECK_EQUAL(0, msa1records, "number of MSA1 records");

    qMsa.reset(true);
    qMsa.bindDataId(1, msaId2);
    qint64 msa2records = qMsa.selectInt64();
    CHECK_EQUAL(1, msa2records, "number of MSA2 records");

    // "Object"
    SQLiteReadQuery qObj("SELECT COUNT(*) FROM Object WHERE id = ?1", sqliteDbi->getDbRef(), os);
    qObj.bindDataId(1, msaId);
    qint64 msa1objects = qObj.selectInt64();
    CHECK_EQUAL(0, msa1objects, "number of MSA1 objects");

    qObj.reset(true);
    qObj.bindDataId(1, msaId2);
    qint64 msa2objects = qObj.selectInt64();
    CHECK_EQUAL(1, msa2objects, "number of MSA2 objects");

    // Remove the second alignment
    sqliteObjectDbi->removeObject(msaId2, os);
}

IMPLEMENT_TEST(SQLiteObjectDbiUnitTests, setTrackModType) {
    U2OpStatusImpl os;
    U2MsaDbi *msaDbi = SQLiteObjectDbiTestData::getMsaDbi();
    SQLiteObjectDbi *objectDbi = SQLiteObjectDbiTestData::getSQLiteObjectDbi();

    // Create alignment 1
    U2DataId msaId1 = msaDbi->createMsaObject("", "Test name 1", BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), os);
    CHECK_NO_ERROR(os);
    Utils::addRow(msaDbi->getRootDbi(), msaId1, "1", "ACGTACGT", QList<U2MsaGap>(), os);
    CHECK_NO_ERROR(os);
    QList<U2MsaRow> rows1 = msaDbi->getRows(msaId1, os);
    CHECK_NO_ERROR(os);

    // Create alignment 2
    U2DataId msaId2 = msaDbi->createMsaObject("", "Test name 2", BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), os);
    CHECK_NO_ERROR(os);
    Utils::addRow(msaDbi->getRootDbi(), msaId2, "2", "CCCCCCC", QList<U2MsaGap>(), os);
    CHECK_NO_ERROR(os);
    QList<U2MsaRow> rows2 = msaDbi->getRows(msaId2, os);
    CHECK_NO_ERROR(os);

    // Change mod track 1
    objectDbi->setTrackModType(msaId1, TrackOnUpdate, os);
    CHECK_NO_ERROR(os);

    U2TrackModType newType1_1 = objectDbi->getTrackModType(rows1[0].sequenceId, os);
    CHECK_EQUAL(TrackOnUpdate, newType1_1, "new mod track type 1_1");

    U2TrackModType newType1_2 = objectDbi->getTrackModType(rows2[0].sequenceId, os);
    CHECK_EQUAL(NoTrack, newType1_2, "new mod track type 1_2");

    // Change mod track 2
    objectDbi->setTrackModType(msaId1, NoTrack, os);
    CHECK_NO_ERROR(os);
    objectDbi->setTrackModType(msaId2, TrackOnUpdate, os);
    CHECK_NO_ERROR(os);

    U2TrackModType newType2_1 = objectDbi->getTrackModType(rows1[0].sequenceId, os);
    CHECK_EQUAL(NoTrack, newType2_1, "new mod track type 2_1");

    U2TrackModType newType2_2 = objectDbi->getTrackModType(rows2[0].sequenceId, os);
    CHECK_EQUAL(TrackOnUpdate, newType2_2, "new mod track type 2_2");
}

IMPLEMENT_TEST(SQLiteObjectDbiUnitTests, canUndoRedo_noTrack) {
    U2OpStatusImpl os;
    U2ObjectDbi* objDbi = SQLiteObjectDbiTestData::getSQLiteObjectDbi();

    // Create test msa
    U2DataId msaId = SQLiteObjectDbiTestData::createTestMsa(false, os);
    CHECK_NO_ERROR(os);

    // Do action
    SQLiteObjectDbiTestData::addTestRow(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify canUndo/canRedo
    bool undoState = objDbi->canUndo(msaId, os); CHECK_NO_ERROR(os);
    bool redoState = objDbi->canRedo(msaId, os); CHECK_NO_ERROR(os);
    CHECK_FALSE(undoState, "undo state");
    CHECK_FALSE(redoState, "redo state");
}

IMPLEMENT_TEST(SQLiteObjectDbiUnitTests, canUndoRedo_noAction) {
    U2OpStatusImpl os;
    U2ObjectDbi* objDbi = SQLiteObjectDbiTestData::getSQLiteObjectDbi();

    // Create test msa
    U2DataId msaId = SQLiteObjectDbiTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Verify canUndo/canRedo
    bool undoState = objDbi->canUndo(msaId, os); CHECK_NO_ERROR(os);
    bool redoState = objDbi->canRedo(msaId, os); CHECK_NO_ERROR(os);
    CHECK_FALSE(undoState, "undo state");
    CHECK_FALSE(redoState, "redo state");
}

IMPLEMENT_TEST(SQLiteObjectDbiUnitTests, canUndoRedo_lastState) {
    U2OpStatusImpl os;
    U2ObjectDbi* objDbi = SQLiteObjectDbiTestData::getSQLiteObjectDbi();

    // Create test msa
    U2DataId msaId = SQLiteObjectDbiTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Do action
    SQLiteObjectDbiTestData::addTestRow(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify canUndo/canRedo
    bool undoState = objDbi->canUndo(msaId, os); CHECK_NO_ERROR(os);
    bool redoState = objDbi->canRedo(msaId, os); CHECK_NO_ERROR(os);
    CHECK_TRUE(undoState, "undo state");
    CHECK_FALSE(redoState, "redo state");
}

IMPLEMENT_TEST(SQLiteObjectDbiUnitTests, canUndoRedo_firstState) {
    U2OpStatusImpl os;
    U2ObjectDbi* objDbi = SQLiteObjectDbiTestData::getSQLiteObjectDbi();

    // Create test msa
    U2DataId msaId = SQLiteObjectDbiTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Do action, undo
    SQLiteObjectDbiTestData::addTestRow(msaId, os);
    CHECK_NO_ERROR(os);
    objDbi->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify canUndo/canRedo
    bool undoState = objDbi->canUndo(msaId, os); CHECK_NO_ERROR(os);
    bool redoState = objDbi->canRedo(msaId, os); CHECK_NO_ERROR(os);
    CHECK_FALSE(undoState, "undo state");
    CHECK_TRUE(redoState, "redo state");
}

IMPLEMENT_TEST(SQLiteObjectDbiUnitTests, canUndoRedo_midState) {
    U2OpStatusImpl os;
    U2ObjectDbi* objDbi = SQLiteObjectDbiTestData::getSQLiteObjectDbi();

    // Create test msa
    U2DataId msaId = SQLiteObjectDbiTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Do action twice, undo
    SQLiteObjectDbiTestData::addTestRow(msaId, os);
    CHECK_NO_ERROR(os);
    SQLiteObjectDbiTestData::addTestRow(msaId, os);
    CHECK_NO_ERROR(os);

    objDbi->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify canUndo/canRedo
    bool undoState = objDbi->canUndo(msaId, os); CHECK_NO_ERROR(os);
    bool redoState = objDbi->canRedo(msaId, os); CHECK_NO_ERROR(os);
    CHECK_TRUE(undoState, "undo state");
    CHECK_TRUE(redoState, "redo state");
}

IMPLEMENT_TEST(SQLiteObjectDbiUnitTests, canUndoRedo_oneUserStep) {
    U2OpStatusImpl os;
    U2ObjectDbi* objDbi = SQLiteObjectDbiTestData::getSQLiteObjectDbi();

    // Create test msa
    U2DataId msaId = SQLiteObjectDbiTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Do action twice inside one UserStep
    {
        U2UseCommonUserModStep userStep(objDbi->getRootDbi(), msaId, os);
        CHECK_NO_ERROR(os);
        Q_UNUSED(userStep);

        SQLiteObjectDbiTestData::addTestRow(msaId, os);
        CHECK_NO_ERROR(os);
        SQLiteObjectDbiTestData::addTestRow(msaId, os);
        CHECK_NO_ERROR(os);
    }

    // Verify canUndo/canRedo
    bool undoState = objDbi->canUndo(msaId, os); CHECK_NO_ERROR(os);
    bool redoState = objDbi->canRedo(msaId, os); CHECK_NO_ERROR(os);
    CHECK_TRUE(undoState, "undo state before undo");
    CHECK_FALSE(redoState, "redo state before undo");

    // Undo UserStep
    objDbi->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify canUndo/canRedo
    undoState = objDbi->canUndo(msaId, os); CHECK_NO_ERROR(os);
    redoState = objDbi->canRedo(msaId, os); CHECK_NO_ERROR(os);
    CHECK_FALSE(undoState, "undo state after undo");
    CHECK_TRUE(redoState, "redo state after undo");
}

IMPLEMENT_TEST(SQLiteObjectDbiUnitTests, commonUndoRedo_user3Multi) {
    U2OpStatusImpl os;
    U2ObjectDbi *objDbi = SQLiteObjectDbiTestData::getSQLiteObjectDbi();
    SQLiteDbi *sqliteDbi = SQLiteObjectDbiTestData::getSQLiteDbi();

    // Create test msa
    U2DataId msaId = SQLiteObjectDbiTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get msa version
    qint64 msaVersion = objDbi->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Do 3 actions
    {
        U2UseCommonUserModStep userStep(objDbi->getRootDbi(), msaId, os);
        CHECK_NO_ERROR(os);
        Q_UNUSED(userStep);

        SQLiteObjectDbiTestData::addTestRow(msaId, os);
        CHECK_NO_ERROR(os);

        SQLiteObjectDbiTestData::addTestRow(msaId, os);
        CHECK_NO_ERROR(os);

        SQLiteObjectDbiTestData::addTestRow(msaId, os);
        CHECK_NO_ERROR(os);
    }

    // Verify version in the userModStep
    SQLiteReadQuery qVersion("SELECT version FROM UserModStep WHERE object = ?1", sqliteDbi->getDbRef(), os);
    qVersion.bindDataId(1, msaId);
    if (qVersion.step()) {
        qint64 userStepVersion = qVersion.getInt64(0);
        CHECK_EQUAL(msaVersion, userStepVersion, "version in user step");
    }
    else {
        CHECK_TRUE(false, "Failed to get userModStep version!");
    }
    CHECK_NO_ERROR(os);

    // Verify msa version
    qint64 msaVersionAftAct = objDbi->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(msaVersion + 3, msaVersionAftAct, "msa version after 3 actions");

    // Verify canUndo/canRedo
    bool undoState = objDbi->canUndo(msaId, os); CHECK_NO_ERROR(os);
    bool redoState = objDbi->canRedo(msaId, os); CHECK_NO_ERROR(os);
    CHECK_TRUE(undoState, "undo state before undo");
    CHECK_FALSE(redoState, "redo state before undo");

    // Undo
    objDbi->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify msa version
    qint64 msaVersionAftUndo = objDbi->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(msaVersion, msaVersionAftUndo, "msa version after undo");

    // Verify canUndo/canRedo
    undoState = objDbi->canUndo(msaId, os);
    CHECK_NO_ERROR(os);
    redoState = objDbi->canRedo(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_FALSE(undoState, "undo state after undo");
    CHECK_TRUE(redoState, "redo state after undo");

    // Redo
    objDbi->redo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify msa version
    qint64 msaVersionAftRedo = objDbi->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(msaVersion + 3, msaVersionAftRedo, "msa version after redo");

    // Verify canUndo/canRedo
    undoState = objDbi->canUndo(msaId, os); CHECK_NO_ERROR(os);
    redoState = objDbi->canRedo(msaId, os); CHECK_NO_ERROR(os);
    CHECK_TRUE(undoState, "undo state after redo");
    CHECK_FALSE(redoState, "redo state after redo");
}

IMPLEMENT_TEST(SQLiteObjectDbiUnitTests, commonUndoRedo_actionAfterUndo) {
    U2OpStatusImpl os;
    U2ObjectDbi *objDbi = SQLiteObjectDbiTestData::getSQLiteObjectDbi();
    SQLiteDbi *sqliteDbi = SQLiteObjectDbiTestData::getSQLiteDbi();

    // Create test msa
    U2DataId msaId = SQLiteObjectDbiTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get msa version
    qint64 msaVersion = objDbi->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Do 2 actions
    {
        U2UseCommonUserModStep userStep(objDbi->getRootDbi(), msaId, os);
        CHECK_NO_ERROR(os);
        Q_UNUSED(userStep);

        SQLiteObjectDbiTestData::addTestRow(msaId, os);
        CHECK_NO_ERROR(os);

        SQLiteObjectDbiTestData::addTestRow(msaId, os);
        CHECK_NO_ERROR(os);
    }

    // Undo
    objDbi->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Do action
    SQLiteObjectDbiTestData::addTestRow(msaId, os);

    // Check there is no obsolete steps
    SQLiteReadQuery qUser("SELECT COUNT(*) FROM UserModStep WHERE object = ?1", sqliteDbi->getDbRef(), os);
    qUser.bindDataId(1, msaId);
    if (qUser.step()) {
        CHECK_EQUAL(1, qUser.getInt64(0), "number of user steps");
    }
    else {
        CHECK_TRUE(false, "Unexpected error!");
    }
    CHECK_NO_ERROR(os);

    SQLiteReadQuery qSingle("SELECT COUNT(*) FROM SingleModStep WHERE object = ?1", sqliteDbi->getDbRef(), os);
    qSingle.bindDataId(1, msaId);
    if (qSingle.step()) {
        CHECK_EQUAL(1, qSingle.getInt64(0), "number of single steps");
    }
    else {
        CHECK_TRUE(false, "Unexpected error!");
    }
    CHECK_NO_ERROR(os);

    // Undo
    objDbi->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify msa version
    qint64 msaVersionAfterActUndo = objDbi->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(msaVersion, msaVersionAfterActUndo, "msa version after undo, action and undo");

    // Verify canUndo/canRedo
    bool undoState = objDbi->canUndo(msaId, os); CHECK_NO_ERROR(os);
    bool redoState = objDbi->canRedo(msaId, os); CHECK_NO_ERROR(os);
    CHECK_FALSE(undoState, "undo state after undo, action and undo");
    CHECK_TRUE(redoState, "redo state after undo, action and undo");

    // Redo
    objDbi->redo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify msa version
    qint64 msaVersionAfterActRedo = objDbi->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(msaVersion + 1, msaVersionAfterActRedo, "msa version after undo, action and undo/redo");

    // Verify canUndo/canRedo
    undoState = objDbi->canUndo(msaId, os); CHECK_NO_ERROR(os);
    redoState = objDbi->canRedo(msaId, os); CHECK_NO_ERROR(os);
    CHECK_TRUE(undoState, "undo state after undo, action and undo/redo");
    CHECK_FALSE(redoState, "redo state after undo, action and undo/redo");
}

IMPLEMENT_TEST(SQLiteObjectDbiUnitTests, commonUndoRedo_actionUndoActionUndo1) {
    U2OpStatusImpl os;
    U2ObjectDbi *objDbi = SQLiteObjectDbiTestData::getSQLiteObjectDbi();
    SQLiteDbi *sqliteDbi = SQLiteObjectDbiTestData::getSQLiteDbi();

    // Create test msa
    U2DataId msaId = SQLiteObjectDbiTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get msa version
    qint64 msaVersion = objDbi->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Do an action
    {
        U2UseCommonUserModStep userStep(objDbi->getRootDbi(), msaId, os);
        CHECK_NO_ERROR(os);
        Q_UNUSED(userStep);

        SQLiteObjectDbiTestData::addTestRow(msaId, os);
        CHECK_NO_ERROR(os);
    }

    // Undo
    objDbi->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Do an action
    {
        U2UseCommonUserModStep userStep(objDbi->getRootDbi(), msaId, os);
        CHECK_NO_ERROR(os);
        Q_UNUSED(userStep);

        SQLiteObjectDbiTestData::addTestRow(msaId, os);
        CHECK_NO_ERROR(os);
    }

    // Check there is no obsolete steps
    SQLiteReadQuery qUser("SELECT COUNT(*) FROM UserModStep WHERE object = ?1", sqliteDbi->getDbRef(), os);
    qUser.bindDataId(1, msaId);
    if (qUser.step()) {
        CHECK_EQUAL(1, qUser.getInt64(0), "number of user steps");
    }
    else {
        CHECK_TRUE(false, "Unexpected error!");
    }
    CHECK_NO_ERROR(os);

    SQLiteReadQuery qSingle("SELECT COUNT(*) FROM SingleModStep WHERE object = ?1", sqliteDbi->getDbRef(), os);
    qSingle.bindDataId(1, msaId);
    if (qSingle.step()) {
        CHECK_EQUAL(1, qSingle.getInt64(0), "number of single steps");
    }
    else {
        CHECK_TRUE(false, "Unexpected error!");
    }
    CHECK_NO_ERROR(os);

    // Undo
    objDbi->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify msa version
    qint64 msaVersionAfter = objDbi->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(msaVersion, msaVersionAfter, "msa version after action, undo, action, undo");

    // Verify canUndo/canRedo
    bool undoState = objDbi->canUndo(msaId, os); CHECK_NO_ERROR(os);
    bool redoState = objDbi->canRedo(msaId, os); CHECK_NO_ERROR(os);
    CHECK_FALSE(undoState, "undo state after undo, action and undo/redo");
    CHECK_TRUE(redoState, "redo state after undo, action and undo/redo");
}

IMPLEMENT_TEST(SQLiteObjectDbiUnitTests, commonUndoRedo_actionUndoActionUndo2) {
    U2OpStatusImpl os;
    U2ObjectDbi *objDbi = SQLiteObjectDbiTestData::getSQLiteObjectDbi();
    SQLiteDbi *sqliteDbi = SQLiteObjectDbiTestData::getSQLiteDbi();

    // Create test msa
    U2DataId msaId = SQLiteObjectDbiTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get msa version
    qint64 msaVersion = objDbi->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Do an action
    SQLiteObjectDbiTestData::addTestRow(msaId, os);

    // Undo
    objDbi->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Do an action
    SQLiteObjectDbiTestData::addTestRow(msaId, os);

    // Check there is no obsolete steps
    SQLiteReadQuery qUser("SELECT COUNT(*) FROM UserModStep WHERE object = ?1", sqliteDbi->getDbRef(), os);
    qUser.bindDataId(1, msaId);
    if (qUser.step()) {
        CHECK_EQUAL(1, qUser.getInt64(0), "number of user steps");
    }
    else {
        CHECK_TRUE(false, "Unexpected error!");
    }
    CHECK_NO_ERROR(os);

    SQLiteReadQuery qSingle("SELECT COUNT(*) FROM SingleModStep WHERE object = ?1", sqliteDbi->getDbRef(), os);
    qSingle.bindDataId(1, msaId);
    if (qSingle.step()) {
        CHECK_EQUAL(1, qSingle.getInt64(0), "number of single steps");
    }
    else {
        CHECK_TRUE(false, "Unexpected error!");
    }
    CHECK_NO_ERROR(os);

    // Undo
    objDbi->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify msa version
    qint64 msaVersionAfter = objDbi->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(msaVersion, msaVersionAfter, "msa version after action, undo, action, undo");

    // Verify canUndo/canRedo
    bool undoState = objDbi->canUndo(msaId, os); CHECK_NO_ERROR(os);
    bool redoState = objDbi->canRedo(msaId, os); CHECK_NO_ERROR(os);
    CHECK_FALSE(undoState, "undo state after undo, action and undo/redo");
    CHECK_TRUE(redoState, "redo state after undo, action and undo/redo");
}

IMPLEMENT_TEST(SQLiteObjectDbiUnitTests, commonUndoRedo_actionUndoActionUndo3) {
    U2OpStatusImpl os;
    U2ObjectDbi *objDbi = SQLiteObjectDbiTestData::getSQLiteObjectDbi();
    SQLiteDbi *sqliteDbi = SQLiteObjectDbiTestData::getSQLiteDbi();

    // Create test msa
    U2DataId msaId = SQLiteObjectDbiTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get msa version
    qint64 msaVersion = objDbi->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Do an action
    SQLiteObjectDbiTestData::addTestRow(msaId, os);

    // Undo
    objDbi->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Do an action
    {
        U2UseCommonUserModStep userStep(objDbi->getRootDbi(), msaId, os);
        CHECK_NO_ERROR(os);
        Q_UNUSED(userStep);

        SQLiteObjectDbiTestData::addTestRow(msaId, os);
        CHECK_NO_ERROR(os);
    }

    // Check there is no obsolete steps
    SQLiteReadQuery qUser("SELECT COUNT(*) FROM UserModStep WHERE object = ?1", sqliteDbi->getDbRef(), os);
    qUser.bindDataId(1, msaId);
    if (qUser.step()) {
        CHECK_EQUAL(1, qUser.getInt64(0), "number of user steps");
    }
    else {
        CHECK_TRUE(false, "Unexpected error!");
    }
    CHECK_NO_ERROR(os);

    SQLiteReadQuery qSingle("SELECT COUNT(*) FROM SingleModStep WHERE object = ?1", sqliteDbi->getDbRef(), os);
    qSingle.bindDataId(1, msaId);
    if (qSingle.step()) {
        CHECK_EQUAL(1, qSingle.getInt64(0), "number of single steps");
    }
    else {
        CHECK_TRUE(false, "Unexpected error!");
    }
    CHECK_NO_ERROR(os);

    // Undo
    objDbi->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify msa version
    qint64 msaVersionAfter = objDbi->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(msaVersion, msaVersionAfter, "msa version after action, undo, action, undo");

    // Verify canUndo/canRedo
    bool undoState = objDbi->canUndo(msaId, os); CHECK_NO_ERROR(os);
    bool redoState = objDbi->canRedo(msaId, os); CHECK_NO_ERROR(os);
    CHECK_FALSE(undoState, "undo state after undo, action and undo/redo");
    CHECK_TRUE(redoState, "redo state after undo, action and undo/redo");
}

IMPLEMENT_TEST(SQLiteObjectDbiUnitTests, commonUndoRedo_actionUndoActionUndo4) {
    U2OpStatusImpl os;
    U2ObjectDbi *objDbi = SQLiteObjectDbiTestData::getSQLiteObjectDbi();
    SQLiteDbi *sqliteDbi = SQLiteObjectDbiTestData::getSQLiteDbi();

    // Create test msa
    U2DataId msaId = SQLiteObjectDbiTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get msa version
    qint64 msaVersion = objDbi->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Do an action
    {
        U2UseCommonUserModStep userStep(objDbi->getRootDbi(), msaId, os);
        CHECK_NO_ERROR(os);
        Q_UNUSED(userStep);

        SQLiteObjectDbiTestData::addTestRow(msaId, os);
        CHECK_NO_ERROR(os);
    }

    // Undo
    objDbi->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Do an action
    SQLiteObjectDbiTestData::addTestRow(msaId, os);

    // Check there is no obsolete steps
    SQLiteReadQuery qUser("SELECT COUNT(*) FROM UserModStep WHERE object = ?1", sqliteDbi->getDbRef(), os);
    qUser.bindDataId(1, msaId);
    if (qUser.step()) {
        CHECK_EQUAL(1, qUser.getInt64(0), "number of user steps");
    }
    else {
        CHECK_TRUE(false, "Unexpected error!");
    }
    CHECK_NO_ERROR(os);

    SQLiteReadQuery qSingle("SELECT COUNT(*) FROM SingleModStep WHERE object = ?1", sqliteDbi->getDbRef(), os);
    qSingle.bindDataId(1, msaId);
    if (qSingle.step()) {
        CHECK_EQUAL(1, qSingle.getInt64(0), "number of single steps");
    }
    else {
        CHECK_TRUE(false, "Unexpected error!");
    }
    CHECK_NO_ERROR(os);

    // Undo
    objDbi->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify msa version
    qint64 msaVersionAfter = objDbi->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(msaVersion, msaVersionAfter, "msa version after action, undo, action, undo");

    // Verify canUndo/canRedo
    bool undoState = objDbi->canUndo(msaId, os); CHECK_NO_ERROR(os);
    bool redoState = objDbi->canRedo(msaId, os); CHECK_NO_ERROR(os);
    CHECK_FALSE(undoState, "undo state after undo, action and undo/redo");
    CHECK_TRUE(redoState, "redo state after undo, action and undo/redo");
}

IMPLEMENT_TEST(SQLiteObjectDbiUnitTests, commonUndoRedo_user3Single6) {
    U2OpStatusImpl os;
    U2ObjectDbi *objDbi = SQLiteObjectDbiTestData::getSQLiteObjectDbi();
    SQLiteDbi *sqliteDbi = SQLiteObjectDbiTestData::getSQLiteDbi();

    // Create test msa
    U2DataId msaId = SQLiteObjectDbiTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get msa version
    qint64 msaVersion = objDbi->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // User step 1
    {
        U2UseCommonUserModStep userStep(objDbi->getRootDbi(), msaId, os);
        CHECK_NO_ERROR(os);
        Q_UNUSED(userStep);

        SQLiteObjectDbiTestData::addTestRow(msaId, os); // multi/single step 1
        CHECK_NO_ERROR(os);

        SQLiteObjectDbiTestData::addTestRow(msaId, os); // multi/single step 2
        CHECK_NO_ERROR(os);
    }

    // User step 2
    SQLiteObjectDbiTestData::addTestRow(msaId, os); // multi/single step 3

    // Verify version
    qint64 msaVersionAfterUser1 = msaVersion + 2; // verified in another test
    qint64 msaVersionAfterUser2 = objDbi->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(msaVersionAfterUser1 + 1, msaVersionAfterUser2, "msa version after user step 2");

    // Verify canUndo/canRedo
    bool undoState = objDbi->canUndo(msaId, os); CHECK_NO_ERROR(os);
    bool redoState = objDbi->canRedo(msaId, os); CHECK_NO_ERROR(os);
    CHECK_TRUE(undoState, "undo state after user step 2");
    CHECK_FALSE(redoState, "redo state after user step 2");

    // User step 3
    U2Sequence seq;
    seq.alphabet = BaseDNAAlphabetIds::NUCL_DNA_DEFAULT();
    seq.circular = false;
    seq.trackModType = NoTrack;
    seq.visualName = "Test sequence";
    sqliteDbi->getSQLiteSequenceDbi()->createSequenceObject(seq, "", os, U2DbiObjectRank_TopLevel);
    CHECK_NO_ERROR(os);

    U2MsaRow row;
    row.sequenceId = seq.id;
    row.gstart = 0;
    row.gend = 0;
    row.length = 0;

    {
        U2UseCommonUserModStep userStep(objDbi->getRootDbi(), msaId, os);
        CHECK_NO_ERROR(os);
        Q_UNUSED(userStep);

        sqliteDbi->getMsaDbi()->addRow(msaId, -1, row, os); // multi/single step 4
        CHECK_NO_ERROR(os);

        sqliteDbi->getMsaDbi()->updateRowContent(msaId, row.rowId, "ACGT", QList<U2MsaGap>(), os); // multi step 5, single steps 5-6
        CHECK_NO_ERROR(os);
    }

    // Verify version
    qint64 msaVersionAfterUser3 = objDbi->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(msaVersionAfterUser2 + 2, msaVersionAfterUser3, "msa version after user step 3");

    // Verify canUndo/canRedo
    undoState = objDbi->canUndo(msaId, os); CHECK_NO_ERROR(os);
    redoState = objDbi->canRedo(msaId, os); CHECK_NO_ERROR(os);
    CHECK_TRUE(undoState, "undo state after user step 3");
    CHECK_FALSE(redoState, "redo state after user step 3");

    // Undo 1 (to user step 2)
    objDbi->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify version
    qint64 msaVersionAfterUndo1 = objDbi->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(msaVersionAfterUser2, msaVersionAfterUndo1, "msa version after undo 1");

    // Verify canUndo/canRedo
    undoState = objDbi->canUndo(msaId, os); CHECK_NO_ERROR(os);
    redoState = objDbi->canRedo(msaId, os); CHECK_NO_ERROR(os);
    CHECK_TRUE(undoState, "undo state after undo 1");
    CHECK_TRUE(redoState, "redo state after undo 1");

    // Undo 2 (to user step 1)
    objDbi->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify version
    qint64 msaVersionAfterUndo2 = objDbi->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(msaVersionAfterUser1, msaVersionAfterUndo2, "msa version after undo 2");

    // Verify canUndo/canRedo
    undoState = objDbi->canUndo(msaId, os); CHECK_NO_ERROR(os);
    redoState = objDbi->canRedo(msaId, os); CHECK_NO_ERROR(os);
    CHECK_TRUE(undoState, "undo state after undo 2");
    CHECK_TRUE(redoState, "redo state after undo 2");

    // Undo 3 (to original)
    objDbi->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify version
    qint64 msaVersionAfterUndo3 = objDbi->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(msaVersion, msaVersionAfterUndo3, "msa version after undo 3");

    // Verify canUndo/canRedo
    undoState = objDbi->canUndo(msaId, os); CHECK_NO_ERROR(os);
    redoState = objDbi->canRedo(msaId, os); CHECK_NO_ERROR(os);
    CHECK_FALSE(undoState, "undo state after undo 3");
    CHECK_TRUE(redoState, "redo state after undo 3");

    // Redo 1 (to user step 1)
    objDbi->redo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify version
    qint64 msaVersionAfterRedo1 = objDbi->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(msaVersionAfterUser1, msaVersionAfterRedo1, "msa version after redo 1");

    // Verify canUndo/canRedo
    undoState = objDbi->canUndo(msaId, os); CHECK_NO_ERROR(os);
    redoState = objDbi->canRedo(msaId, os); CHECK_NO_ERROR(os);
    CHECK_TRUE(undoState, "undo state after redo 1");
    CHECK_TRUE(redoState, "redo state after redo 1");

    // Redo 2 (to user step 2)
    objDbi->redo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify version
    qint64 msaVersionAfterRedo2 = objDbi->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(msaVersionAfterUser2, msaVersionAfterRedo2, "msa version after redo 2");

    // Verify canUndo/canRedo
    undoState = objDbi->canUndo(msaId, os); CHECK_NO_ERROR(os);
    redoState = objDbi->canRedo(msaId, os); CHECK_NO_ERROR(os);
    CHECK_TRUE(undoState, "undo state after redo 2");
    CHECK_TRUE(redoState, "redo state after redo 2");

    // Redo 3 (to user step 3)
    objDbi->redo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify version
    qint64 msaVersionAfterRedo3 = objDbi->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(msaVersionAfterUser3, msaVersionAfterRedo3, "msa version after redo 3");

    // Verify canUndo/canRedo
    undoState = objDbi->canUndo(msaId, os); CHECK_NO_ERROR(os);
    redoState = objDbi->canRedo(msaId, os); CHECK_NO_ERROR(os);
    CHECK_TRUE(undoState, "undo state after redo 3");
    CHECK_FALSE(redoState, "redo state after redo 3");
}

} // namespace
