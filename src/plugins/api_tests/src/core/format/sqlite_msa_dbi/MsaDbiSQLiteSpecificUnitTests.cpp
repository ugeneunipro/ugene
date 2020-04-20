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

#include "MsaDbiSQLiteSpecificUnitTests.h"
#include "core/util/MsaDbiUtilsUnitTests.h"

#include <U2Core/DNAAlphabet.h>
#include <U2Core/U2DbiPackUtils.h>
#include <U2Core/U2MsaDbi.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Formats/SQLiteDbi.h>
#include <U2Formats/SQLiteObjectDbi.h>
#include <U2Formats/SQLiteModDbi.h>


namespace U2 {

TestDbiProvider MsaSQLiteSpecificTestData::dbiProvider = TestDbiProvider();
const QString& MsaSQLiteSpecificTestData::SQLITE_MSA_DB_URL("sqlite-msa-dbi.ugenedb");
SQLiteDbi* MsaSQLiteSpecificTestData::sqliteDbi = NULL;

const QString MsaSQLiteSpecificTestData::TEST_MSA_NAME = "Test alignment";

void MsaSQLiteSpecificTestData::init() {
    SAFE_POINT(NULL == sqliteDbi, "sqliteDbi has already been initialized!", );

    // Get URL
    bool ok = dbiProvider.init(SQLITE_MSA_DB_URL, false);
    SAFE_POINT(ok, "Dbi provider failed to initialize!",);

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

    // Get msa IDs
    QList<U2DataId> ids = sqliteDbi->getObjectDbi()->getObjects(U2Type::Msa, 0,
        U2DbiOptions::U2_DBI_NO_LIMIT, os);
    SAFE_POINT_OP(os,);
}

void MsaSQLiteSpecificTestData::shutdown() {
    if (NULL != sqliteDbi) {
        U2OpStatusImpl os;
        sqliteDbi->shutdown(os);
        SAFE_POINT_OP(os, );
        delete sqliteDbi;
        sqliteDbi = NULL;
    }
}

SQLiteDbi* MsaSQLiteSpecificTestData::getSQLiteDbi() {
    if (NULL == sqliteDbi) {
        init();
    }
    return sqliteDbi;
}

qint64 MsaSQLiteSpecificTestData::getModStepsNum(const U2DataId& objId, U2OpStatus& os) {
    SQLiteReadQuery qModSteps("SELECT COUNT(*) FROM SingleModStep WHERE object = ?1", sqliteDbi->getDbRef(), os);
    qModSteps.bindDataId(1, objId);
    return qModSteps.selectInt64();
}

U2MsaRow MsaSQLiteSpecificTestData::addRow(const U2DataId &msaId, const QByteArray &name, const QByteArray &seq, const QList<U2MsaGap> &gaps, U2OpStatus &os) {
    U2Sequence sequence;
    sequence.alphabet = BaseDNAAlphabetIds::NUCL_DNA_DEFAULT();
    sequence.visualName = name;
    sqliteDbi->getSequenceDbi()->createSequenceObject(sequence, "", os);
    CHECK_OP(os, U2MsaRow());

    U2Region reg(0, 0);
    sqliteDbi->getSequenceDbi()->updateSequenceData(sequence.id, reg, seq, QVariantMap(), os);
    CHECK_OP(os, U2MsaRow());

    U2MsaRow row;
    row.sequenceId = sequence.id;
    row.gstart = 0;
    row.gend = seq.length();
    row.gaps = gaps;
    row.length = MsaRowUtils::getRowLengthWithoutTrailing(seq, gaps);

    sqliteDbi->getMsaDbi()->addRow(msaId, -1, row, os);
    CHECK_OP(os, U2MsaRow());
    return row;
}

U2MsaRow MsaSQLiteSpecificTestData::createRow(qint64 seqLength, U2OpStatus &os) {
    U2DataId seqId = MsaSQLiteSpecificTestData::createTestSequence(false, seqLength, os);
    CHECK_OP(os, U2MsaRow());

    QList<U2MsaGap> gaps;
    qint64 gapLength = 2;
    gaps << U2MsaGap(1, gapLength);

    U2MsaRow row;
    row.sequenceId = seqId;
    row.gstart = 1;
    row.gend = 20;
    row.gaps = gaps;
    row.length = row.gend - row.gstart + gapLength;
    return row;
}

U2DataId MsaSQLiteSpecificTestData::createTestMsa(bool enableModTracking, U2OpStatus& os) {
    // Create an alignment
    U2AlphabetId alphabet = BaseDNAAlphabetIds::NUCL_DNA_DEFAULT();
    U2DataId msaId = sqliteDbi->getMsaDbi()->createMsaObject("", TEST_MSA_NAME, alphabet, os);
    CHECK_OP(os, U2DataId());

    // The alignment has the following rows:
    // T-AAGAC-TTCTA
    // TAAGC--TACTA
    addRow(msaId, "1", "TAAGACTTCTA", QList<U2MsaGap>() << U2MsaGap(1, 1) << U2MsaGap(7, 1), os);
    addRow(msaId, "2", "TAAGCTACTA", QList<U2MsaGap>() << U2MsaGap(5, 2), os);

    if (enableModTracking) {
        sqliteDbi->getObjectDbi()->setTrackModType(msaId, TrackOnUpdate, os);
        CHECK_OP(os, U2DataId());
    }

    return msaId;
}

U2DataId MsaSQLiteSpecificTestData::createTestSequence(bool enableModTracking, qint64 seqLength, U2OpStatus& os){
    U2Sequence sequence;
    sequence.alphabet = BaseDNAAlphabetIds::NUCL_DNA_DEFAULT();
    sequence.visualName = "Test sequence";
    if (false == enableModTracking) {
        sequence.trackModType = NoTrack;
    } else {
        sequence.trackModType = TrackOnUpdate;
    }
    sqliteDbi->getSequenceDbi()->createSequenceObject(sequence, "", os);
    CHECK_OP(os, "");

    QByteArray seq;
    seq.fill('A', seqLength);

    sqliteDbi->getSequenceDbi()->updateSequenceData(sequence.id, U2_REGION_MAX, seq, QVariantMap(), os);
    CHECK_OP(os, "");

    return sequence.id;
}

U2DataId MsaSQLiteSpecificTestData::createNotSoSmallTestMsa(bool enableModTracking, U2OpStatus& os) {
    // Create an alignment
    U2AlphabetId alphabet = BaseDNAAlphabetIds::NUCL_DNA_DEFAULT();
    U2DataId msaId = sqliteDbi->getMsaDbi()->createMsaObject("", TEST_MSA_NAME, alphabet, os);
    CHECK_OP(os, U2DataId());

    // The alignment has the following rows:
    // T-AAGAC-TTCTA--ATTTATTCGGGG-CTAGCTAGCA--TTTCAGCTAGCT---ACGGG
    // TAAGC--TACTAGGGA-CTGAG-CTAG---CATTCAGCTACGAGGGGGC--AAACTACTG
    // TTAGCTAGCTTAGCATCT--TAGCTATG--ACGTACAGCGATT-CGAC-GTA-TCAG-CT
    // ACTTTGAC---TCGAGCGAGC---TTATTTTACGATCAC--GACCCATAAGAATA--GAGCTAT
    // ACTGGC-TACAGCGA-TCTAGG-GGAAATC-TACTATC-T-ACGAT-CTAGTATCACGAT
    // AC--TAGGTTCAGCTAG--CGGATCGACATTTTGTAGGACGACGT--ACTTTCCCA--CGCTA
    // TTT--C-G-T-GTGTCACATA-C----GTCAGT----------TAACGATCGATCACACAC
    addRow(msaId, "1", "TAAGACTTCTAATTTATTCGGGGCTAGCTAGCATTTCAGCTAGCTACGGG", QList<U2MsaGap>() << U2MsaGap(1, 1) << U2MsaGap(7, 1) << U2MsaGap(13, 2) << U2MsaGap(27, 1) << U2MsaGap(38, 2) << U2MsaGap(52, 3), os);
    addRow(msaId, "2", "TAAGCTACTAGGGACTGAGCTAGCATTCAGCTACGAGGGGGCAAACTACTG", QList<U2MsaGap>() << U2MsaGap(5, 2) << U2MsaGap(16, 1) << U2MsaGap(22, 1) << U2MsaGap(27, 1) << U2MsaGap(49, 2), os);
    addRow(msaId, "3", "TTAGCTAGCTTAGCATCTTAGCTATGACGTACAGCGATTCGACGTATCAGCT", QList<U2MsaGap>() << U2MsaGap(18, 2) << U2MsaGap(28, 2) << U2MsaGap(43, 1) << U2MsaGap(48, 1) << U2MsaGap(52, 1) << U2MsaGap(57, 1), os);
    addRow(msaId, "4", "ACTTTGACTCGAGCGAGCTTATTTTACGATCACGACCCATAAGAATAGAGCTAT", QList<U2MsaGap>() << U2MsaGap(8, 3) << U2MsaGap(21, 3) << U2MsaGap(39, 2) << U2MsaGap(55, 2), os);
    addRow(msaId, "5", "ACTGGCTACAGCGATCTAGGGGAAATCTACTATCTACGATCTAGTATCACGAT", QList<U2MsaGap>() << U2MsaGap(6, 1) << U2MsaGap(15, 1) << U2MsaGap(22, 1) << U2MsaGap(30, 1) << U2MsaGap(38, 1) << U2MsaGap(40, 1) << U2MsaGap(46, 1), os);
    addRow(msaId, "6", "ACTAGGTTCAGCTAGCGGATCGACATTTTGTAGGACGACGTACTTTCCCACGCTA", QList<U2MsaGap>() << U2MsaGap(2, 2) << U2MsaGap(17, 2) << U2MsaGap(45, 2) << U2MsaGap(56, 2), os);
    addRow(msaId, "7", "TTTCGTGTGTCACATACGTCAGTTAACGATCGATCACACAC", QList<U2MsaGap>() << U2MsaGap(3, 2) << U2MsaGap(6, 1) << U2MsaGap(8, 1) << U2MsaGap(10, 1) << U2MsaGap(21, 1) << U2MsaGap(23, 4) << U2MsaGap(33, 10), os);

    if (enableModTracking) {
        sqliteDbi->getObjectDbi()->setTrackModType(msaId, TrackOnUpdate, os);
        CHECK_OP(os, U2DataId());
    }

    return msaId;
}


IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, updateMsaName_noModTrack) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(false, os);
    CHECK_NO_ERROR(os);

    // Get current version
    int objVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Rename the msa
    QString newName = "Renamed alignment";
    sqliteDbi->getMsaDbi()->updateMsaName(msaId, newName, os);
    CHECK_NO_ERROR(os);

    // Verify the msa has been renamed
    U2Msa msaObj = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(newName, msaObj.visualName, "name");

    // Verify the version has been incremented
    int versionAfterUpdate = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(objVersion + 1, versionAfterUpdate, "version");

    // Verify that there are no modification steps
    qint64 actualModStepsNum = MsaSQLiteSpecificTestData::getModStepsNum(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(0, actualModStepsNum, "mod steps num");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, updateMsaName_undo) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    QString msaName = MsaSQLiteSpecificTestData::TEST_MSA_NAME;
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get current version
    int objVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Rename the msa
    QString newName = "Renamed alignment";
    sqliteDbi->getMsaDbi()->updateMsaName(msaId, newName, os);
    CHECK_NO_ERROR(os);

    // Verify the msa has been renamed
    U2Msa msaObj = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(msaObj.visualName, newName, "name");

    // Verify the version has been incremented
    int versionAfterUpdate = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(objVersion + 1, versionAfterUpdate, "version");

    // Verify the modification step
    U2SingleModStep modStep = sqliteDbi->getModDbi()->getModStep(msaId, objVersion, os);
    QString expectedModDetails = "0\t" + msaName + "\t" + newName;
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(QString(msaId), QString(modStep.objectId), "object id");
    CHECK_EQUAL(objVersion, modStep.version, "version in mod step");
    CHECK_EQUAL(U2ModType::objUpdatedName, modStep.modType, "mod step type");
    CHECK_EQUAL(expectedModDetails, QString(modStep.details), "mod step details");

    // Undo msa renaming
    sqliteDbi->getSQLiteObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify the object name and version have been restored
    U2Msa msaObjAfterUndo = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(msaName, msaObjAfterUndo.visualName, "name after undo");
    CHECK_EQUAL(objVersion, msaObjAfterUndo.version, "version after undo");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, updateMsaName_redo) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    QString msaName = MsaSQLiteSpecificTestData::TEST_MSA_NAME;
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get current version
    int objVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Rename the msa
    QString newName = "Renamed alignment";
    sqliteDbi->getMsaDbi()->updateMsaName(msaId, newName, os);
    CHECK_NO_ERROR(os);

    // Undo msa renaming
    sqliteDbi->getSQLiteObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Redo msa renaming
    sqliteDbi->getSQLiteObjectDbi()->redo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify the object name and version
    U2Msa msaObjAfterUndo = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(newName, msaObjAfterUndo.visualName, "name after undo/redo");
    CHECK_EQUAL(objVersion + 1, msaObjAfterUndo.version, "version after undo/redo");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, updateMsaName_severalSteps) {
    //5 changes, 4 undo steps, 2 redo steps, 1 undo step, 1 redo step
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    QString msaName = MsaSQLiteSpecificTestData::TEST_MSA_NAME;
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get current version
    int objVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Prepare value list
    QStringList newNames(sqliteDbi->getMsaDbi()->getMsaObject(msaId, os).visualName);
    CHECK_NO_ERROR(os);
    for (int i = 1; i < 6; ++i) {
        newNames << "Renamed alignment" + QString::number(i);
    }

    // Steps count
    int valuesCount = newNames.length();    // changes = valuesCount - 1;
    QList<int> steps;                       // negative - undo steps, positive - redo steps;
    steps << -4 << 2 << -1 << 1;
    int expectedIndex = valuesCount - 1;
    for (int i = 0; i < steps.length(); ++i) {
        expectedIndex += steps[i];
    }

    // Rename the msa (changesCount times)
    for (int i = 1; i < newNames.length(); ++i) {
        sqliteDbi->getMsaDbi()->updateMsaName(msaId, newNames[i], os);
        CHECK_NO_ERROR(os);
    }

    // Undo and redo msa renaming
    int totalUndo = 0;
    int totalRedo = 0;
    for (int i  = 0; i < steps.length(); ++i) {
        if (steps[i] < 0) {
            for (int j = 0; j < -steps[i]; ++j) {
                sqliteDbi->getSQLiteObjectDbi()->undo(msaId, os);
                CHECK_NO_ERROR(os);
                totalUndo++;
            }
        } else {
            for (int j = 0; j < steps[i]; ++j) {
                sqliteDbi->getSQLiteObjectDbi()->redo(msaId, os);
                CHECK_NO_ERROR(os);
                totalRedo++;
            }
        }
    }

    // Verify the object name and version
    U2Msa msaObjAfterUndo = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(newNames[expectedIndex], msaObjAfterUndo.visualName, QString("name after %1 changes, %2 undo steps, %3 redo steps").arg(valuesCount - 1, totalUndo, totalRedo));
    CHECK_EQUAL(objVersion + expectedIndex, msaObjAfterUndo.version, "version after undo/redo");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, updateMsaAlphabet_noModTrack) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(false, os);
    CHECK_NO_ERROR(os);

    // Get current version
    int objVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Update the msa alphabet
    U2AlphabetId oldAlphabet = BaseDNAAlphabetIds::NUCL_DNA_DEFAULT();
    U2AlphabetId newAlphabet = BaseDNAAlphabetIds::NUCL_DNA_EXTENDED();
    sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId, newAlphabet, os);
    CHECK_NO_ERROR(os);

    // Verify the alphabet has been changed
    U2Msa msaObj = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(newAlphabet.id, msaObj.alphabet.id, "alphabet");

    // Verify the version has been incremented
    int versionAfterUpdate = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(objVersion + 1, versionAfterUpdate, "version");

    // Verify that there are no modification steps
    qint64 actualModStepsNum = MsaSQLiteSpecificTestData::getModStepsNum(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(0, actualModStepsNum, "mod steps num");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, updateMsaAlphabet_undo) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get current version
    int objVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Update the msa alphabet
    U2AlphabetId oldAlphabet = BaseDNAAlphabetIds::NUCL_DNA_DEFAULT();
    U2AlphabetId newAlphabet = BaseDNAAlphabetIds::NUCL_DNA_EXTENDED();
    sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId, newAlphabet, os);
    CHECK_NO_ERROR(os);

    // Verify the alphabet has been changed
    U2Msa msaObj = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(newAlphabet.id, msaObj.alphabet.id, "alphabet");

    // Verify the version has been incremented
    int versionAfterUpdate = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(objVersion + 1, versionAfterUpdate, "version");

    // Verify the modification step
    U2SingleModStep modStep = sqliteDbi->getModDbi()->getModStep(msaId, objVersion, os);
    QString expectedModDetails = "0\t" + oldAlphabet.id + "\t" + newAlphabet.id;
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(QString(msaId), QString(modStep.objectId), "object id");
    CHECK_EQUAL(objVersion, modStep.version, "version in mod step");
    CHECK_EQUAL(U2ModType::msaUpdatedAlphabet, modStep.modType, "mod step type");
    CHECK_EQUAL(expectedModDetails, QString(modStep.details), "mod step details");

    // Undo alphabet updating
    sqliteDbi->getSQLiteObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify the object alphabet and version have been restored
    U2Msa msaObjAfterUndo = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldAlphabet.id, msaObjAfterUndo.alphabet.id, "name after undo");
    CHECK_EQUAL(objVersion, msaObjAfterUndo.version, "version after undo");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, updateMsaAlphabet_redo) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    QString msaName = MsaSQLiteSpecificTestData::TEST_MSA_NAME;
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get current version
    int objVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Update the msa alphabet
    U2AlphabetId oldAlphabet = BaseDNAAlphabetIds::NUCL_DNA_DEFAULT();
    U2AlphabetId newAlphabet = BaseDNAAlphabetIds::NUCL_DNA_EXTENDED();
    sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId, newAlphabet, os);
    CHECK_NO_ERROR(os);

    // Undo alphabet updating
    sqliteDbi->getSQLiteObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Redo alphabet updating
    sqliteDbi->getSQLiteObjectDbi()->redo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify the object alphabet and version
    U2Msa msaObjAfterUndo = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(newAlphabet.id, msaObjAfterUndo.alphabet.id, "alphabet after undo/redo");
    CHECK_EQUAL(objVersion + 1, msaObjAfterUndo.version, "version after undo/redo");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, updateMsaAlphabet_severalSteps) {
    //3 changes, 3 undo steps, 2 redo steps, 2 undo steps, 1 redo step
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    QString msaName = MsaSQLiteSpecificTestData::TEST_MSA_NAME;
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get current version
    int objVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Prepare value list
    QList<U2AlphabetId> newAlphabets;
    newAlphabets << sqliteDbi->getMsaDbi()->getMsaObject(msaId, os).alphabet;
    CHECK_NO_ERROR(os);
    newAlphabets << BaseDNAAlphabetIds::NUCL_DNA_EXTENDED();
    newAlphabets << BaseDNAAlphabetIds::NUCL_RNA_DEFAULT();
    newAlphabets << BaseDNAAlphabetIds::NUCL_RNA_EXTENDED();

    // Steps count
    int valuesCount = newAlphabets.length();    // changes = valuesCount - 1;
    QList<int> steps;                           // negative - undo steps, positive - redo steps;
    steps << -3 << 2 << -2 << 1;
    int expectedIndex = valuesCount - 1;
    for (int i = 0; i < steps.length(); ++i) {
        expectedIndex += steps[i];
    }

    // Update the msa alphabet
    for (int i = 1; i < newAlphabets.length(); ++i) {
        sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId, newAlphabets[i], os);
        CHECK_NO_ERROR(os);
    }

    // Undo and redo alphabet updating
    int totalUndo = 0;
    int totalRedo = 0;
    for (int i  = 0; i < steps.length(); ++i) {
        if (steps[i] < 0) {
            for (int j = 0; j < -steps[i]; ++j) {
                sqliteDbi->getSQLiteObjectDbi()->undo(msaId, os);
                CHECK_NO_ERROR(os);
                totalUndo++;
            }
        } else {
            for (int j = 0; j < steps[i]; ++j) {
                sqliteDbi->getSQLiteObjectDbi()->redo(msaId, os);
                CHECK_NO_ERROR(os);
                totalRedo++;
            }
        }
    }

    // Verify the object alphabet and version
    U2Msa msaObjAfterUndo = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(newAlphabets[expectedIndex].id, msaObjAfterUndo.alphabet.id, QString("alphabet after %1 changes, %2 undo steps, %3 redo steps").arg(valuesCount - 1, totalUndo, totalRedo));
    CHECK_EQUAL(objVersion + expectedIndex, msaObjAfterUndo.version, "version after undo/redo");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, updateGapModel_noModTrack) {
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(false, os);
    CHECK_NO_ERROR(os);
    QList<qint64> rows = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    qint64 rowId = rows.first();

    // Get current version
    int objVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Update gaps
    QList<U2MsaGap> newGaps;
    newGaps << U2MsaGap(4, 3) << U2MsaGap(11, 3); // TAAG---ACTT---CTA
    CHECK_NO_ERROR(os);
    sqliteDbi->getMsaDbi()->updateGapModel(msaId, rowId, newGaps, os);
    CHECK_NO_ERROR(os);

    // Verify gaps
    U2MsaRow row = sqliteDbi->getMsaDbi()->getRow(msaId, rowId, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(newGaps == row.gaps, "gaps");

    // Verify msa length
    U2Msa msaAfterUpdate = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_EQUAL(17, msaAfterUpdate.length, "length");

    // Verify version
    int versionAfterUpdate = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(objVersion + 1, versionAfterUpdate, "version");

    // Verify no modification steps
    qint64 actualModStepsNum = MsaSQLiteSpecificTestData::getModStepsNum(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(0, actualModStepsNum, "mod steps num");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, updateGapModel_undo) {
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    QList<qint64> rows = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    qint64 rowId = rows[1];

    // Get current version
    int objVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Update gaps
    QList<U2MsaGap> oldGaps = sqliteDbi->getMsaDbi()->getRow(msaId, rowId, os).gaps;
    QList<U2MsaGap> newGaps;
    newGaps << U2MsaGap(1, 4) << U2MsaGap(14, 11); // T----AAGCTACTA-----------
    CHECK_NO_ERROR(os);
    sqliteDbi->getMsaDbi()->updateGapModel(msaId, rowId, newGaps, os);
    CHECK_NO_ERROR(os);

    // Verify gaps
    U2MsaRow rowAfterUpdate = sqliteDbi->getMsaDbi()->getRow(msaId, rowId, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(newGaps == rowAfterUpdate.gaps, "gaps"); // ! cthis  should not be like that !

    // Verify msa length
    U2Msa msaAfterUpdate = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_EQUAL(25, msaAfterUpdate.length, "length");

    // Verify version
    int versionAfterUpdate = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(objVersion + 1, versionAfterUpdate, "version");

    // Verify the modification step
    QString expectedModDetails = "0\t" + QByteArray::number(rowAfterUpdate.rowId) + "\t\"5,2\"\t\"1,4;14,11\"";
    QList< QList<U2SingleModStep> > modSteps = sqliteDbi->getSQLiteModDbi()->getModSteps(msaId, objVersion, os);
    CHECK_EQUAL(1, modSteps.count(), "mod steps count");
    CHECK_EQUAL(2, modSteps.first().size(), "mod single steps count");
    U2SingleModStep modStep = modSteps.first().first();
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(QString(msaId), QString(modStep.objectId), "object id");
    CHECK_EQUAL(objVersion, modStep.version, "version in mod step");
    CHECK_EQUAL(U2ModType::msaLengthChanged, modStep.modType, "mod step type");
    CHECK_EQUAL("13\t25", QString(modStep.details), "mod step details");

    modStep = modSteps.first().last();
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(QString(msaId), QString(modStep.objectId), "object id");
    CHECK_EQUAL(objVersion, modStep.version, "version in mod step");
    CHECK_EQUAL(U2ModType::msaUpdatedGapModel, modStep.modType, "mod step type");
    CHECK_EQUAL(expectedModDetails, QString(modStep.details), "mod step details");

    // Undo
    sqliteDbi->getSQLiteObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify gaps
    U2MsaRow rowAfterUndo = sqliteDbi->getMsaDbi()->getRow(msaId, rowId, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(oldGaps == rowAfterUndo.gaps, "gaps after undo");

    // Verify msa length
    U2Msa msaAfterUndo = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_EQUAL(13, msaAfterUndo.length, "length");

    // Verify version
    int versionAfterUndo = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(objVersion, versionAfterUndo, "version after undo");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, updateGapModel_redo) {
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    QList<qint64> rows = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    qint64 rowId = rows.first();

    // Get current version
    int objVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Update gaps
    QList<U2MsaGap> newGaps; // TAAGACTTCTA
    CHECK_NO_ERROR(os);
    sqliteDbi->getMsaDbi()->updateGapModel(msaId, rowId, newGaps, os);
    CHECK_NO_ERROR(os);

    // Undo
    sqliteDbi->getSQLiteObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Redo
    sqliteDbi->getSQLiteObjectDbi()->redo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify gaps
    U2MsaRow rowAfterRedo = sqliteDbi->getMsaDbi()->getRow(msaId, rowId, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(newGaps == rowAfterRedo.gaps, "gaps after undo");

    // Verify msa length
    U2Msa msaAfterRedo = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_EQUAL(13, msaAfterRedo.length, "length");

    // Verify version
    int versionAfterRedo = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(objVersion + 1, versionAfterRedo, "version after undo");

    // Verify the modification step
    QString expectedModDetails = "0\t" + QByteArray::number(rowAfterRedo.rowId) + "\t\"1,1;7,1\"\t\"\"";
    U2SingleModStep modStep = sqliteDbi->getModDbi()->getModStep(msaId, objVersion, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(QString(msaId), QString(modStep.objectId), "object id");
    CHECK_EQUAL(objVersion, modStep.version, "version in mod step");
    CHECK_EQUAL(U2ModType::msaUpdatedGapModel, modStep.modType, "mod step type");
    CHECK_EQUAL(expectedModDetails, QString(modStep.details), "mod step details");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, updateGapModel_severalSteps) {
    //6 changes, 4 undo steps, 2 redo steps, 1 undo step, 3 redo steps
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get current version
    int objVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Get rows
    QList<U2MsaRow> oldRows = sqliteDbi->getMsaDbi()->getRows(msaId, os);
    CHECK_NO_ERROR(os);

    // Prepare value list
    QList<QList<U2MsaGap> > gapModels;
    gapModels << oldRows[0].gaps;  // base value

    QList<U2MsaGap> gapModel;
    for (int i = 0; i < 6; ++i) {
        gapModel.clear();
        gapModel << U2MsaGap(0, i + 1) << U2MsaGap(i + 3, 2);
        gapModels << gapModel;
    }

    // Steps count
    int valuesCount = gapModels.length();    // changes = valuesCount - 1;
    QList<int> steps;                        // negative - undo steps, positive - redo steps;
    steps << -4 << 2 << -1 << 3;
    int expectedIndex = valuesCount - 1;
    for (int i = 0; i < steps.length(); ++i) {
        expectedIndex += steps[i];
    }

    //Expected msa length
    qint64 expectedMsaLength = oldRows[0].gend - oldRows[1].gstart;
    CHECK_NO_ERROR(os);
    foreach (U2MsaGap gap, gapModels[expectedIndex]) {
        expectedMsaLength += gap.gap;
    }
    expectedMsaLength = qMax(expectedMsaLength, oldRows[1].length);

    // Update the msa gap model
    for (int i = 1; i < gapModels.length(); ++i) {
        sqliteDbi->getMsaDbi()->updateGapModel(msaId, oldRows[0].rowId, gapModels[i], os);
        CHECK_NO_ERROR(os);
    }

    // Undo and redo gap model updating
    int totalUndo = 0;
    int totalRedo = 0;
    for (int i  = 0; i < steps.length(); ++i) {
        if (steps[i] < 0) {
            for (int j = 0; j < -steps[i]; ++j) {
                sqliteDbi->getSQLiteObjectDbi()->undo(msaId, os);
                CHECK_NO_ERROR(os);
                totalUndo++;
            }
        } else {
            for (int j = 0; j < steps[i]; ++j) {
                sqliteDbi->getSQLiteObjectDbi()->redo(msaId, os);
                CHECK_NO_ERROR(os);
                totalRedo++;
            }
        }
    }

    // Verify gaps
    U2MsaRow finalRow = sqliteDbi->getMsaDbi()->getRow(msaId, oldRows[0].rowId, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(gapModels[expectedIndex] == finalRow.gaps, "final gaps");

    // Verify msa length
    U2Msa finalMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_EQUAL(expectedMsaLength, finalMsa.length, "length");

    // Verify version
    int finalVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(objVersion + expectedIndex, finalVersion, "final version");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, updateRowContent_noModTrack) {
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(false, os);
    CHECK_NO_ERROR(os);
    QList<qint64> rows = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    qint64 rowId = rows.first();

    // Get current version
    int oldVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Update row content
    CHECK_NO_ERROR(os);
    QByteArray newSeq = "AAAAGGGGCCCCTTTT";
    QList<U2MsaGap> newGaps; newGaps << U2MsaGap(4, 4) << U2MsaGap(20, 4); // AAAA----GGGGCCCCTTTT----
    sqliteDbi->getMsaDbi()->updateRowContent(msaId, rowId, newSeq, newGaps, os);
    CHECK_NO_ERROR(os);

    // Verify gaps
    U2MsaRow newRow = sqliteDbi->getMsaDbi()->getRow(msaId, rowId, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(newGaps == newRow.gaps, "gaps");

    // Verify seq
    QByteArray newestSeq = sqliteDbi->getSequenceDbi()->getSequenceData(newRow.sequenceId, U2_REGION_MAX, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(newSeq, newestSeq, "sequence");

    // Verify msa length
    U2Msa newMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_EQUAL(24, newMsa.length, "length");

    // Verify version
    int newVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldVersion + 1, newVersion, "version");

    // Verify no modification steps
    qint64 modStepsNum = MsaSQLiteSpecificTestData::getModStepsNum(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(0, modStepsNum, "mod steps num");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, updateRowContent_undo) {
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    QList<qint64> rows = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    qint64 rowId = rows[1];

    // Update row content
    U2MsaRow oldRow = sqliteDbi->getMsaDbi()->getRow(msaId, rowId, os);
    CHECK_NO_ERROR(os);
    QByteArray oldSeq = sqliteDbi->getSequenceDbi()->getSequenceData(oldRow.sequenceId, U2_REGION_MAX, os);
    CHECK_NO_ERROR(os);
    QByteArray newSeq = "GGGGGTTTTTCCCCCAAAAA";
    QList<U2MsaGap> newGaps; newGaps << U2MsaGap(0, 1) << U2MsaGap(2, 1); // -G-GGGGTTTTTCCCCCAAAAA

    // Get current version
    int oldMsaVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    int oldSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(oldRow.sequenceId, os);
    CHECK_NO_ERROR(os);
    qint64 msaModStepsNum = MsaSQLiteSpecificTestData::getModStepsNum(msaId, os);
    CHECK_NO_ERROR(os);

    // Call test function
    sqliteDbi->getMsaDbi()->updateRowContent(msaId, rowId, newSeq, newGaps, os);
    CHECK_NO_ERROR(os);

    // Verify gaps
    U2MsaRow newRow = sqliteDbi->getMsaDbi()->getRow(msaId, rowId, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(newGaps == newRow.gaps, "gaps");

    // Verify seq
    QByteArray newestSeq = sqliteDbi->getSequenceDbi()->getSequenceData(newRow.sequenceId, U2_REGION_MAX, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(newSeq, newestSeq, "sequence");

    // Verify msa length
    U2Msa newMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_EQUAL(22, newMsa.length, "length");

    // Verify version
    int newVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldMsaVersion + 1, newVersion, "version");

    // Verify single modification steps
    QString expectedSeqModDetails = U2DbiPackUtils::packSequenceDataDetails(U2_REGION_MAX, oldSeq, newSeq, QVariantMap());
    QString expectedRowModDetails = U2DbiPackUtils::packRowInfoDetails(oldRow, newRow);
    QString expectedGapModDetails = U2DbiPackUtils::packGapDetails(oldRow.rowId, oldRow.gaps, newRow.gaps);
    QString expectedLenModDetails = U2DbiPackUtils::packAlignmentLength(13, 22);

    QList< QList<U2SingleModStep> > modSteps = sqliteDbi->getSQLiteModDbi()->getModSteps(msaId, oldMsaVersion, os);
    QList<U2SingleModStep> msaSingleModSteps;
    foreach (QList<U2SingleModStep> multiStep, modSteps) {
        foreach (U2SingleModStep singleStep, multiStep) {
            msaSingleModSteps.append(singleStep);
        }
    }
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(msaModStepsNum + 4, msaSingleModSteps.count(), "msa single mod steps count");

    CHECK_EQUAL(QString(oldRow.sequenceId), QString(msaSingleModSteps[0].objectId), "seq object id");
    CHECK_EQUAL(oldSeqVersion, msaSingleModSteps[0].version, "seq version in mod step");
    CHECK_EQUAL(U2ModType::sequenceUpdatedData, msaSingleModSteps[0].modType, "seq mod step type");
    CHECK_EQUAL(expectedSeqModDetails, QString(msaSingleModSteps[0].details), "seq mod step details");

    CHECK_EQUAL(QString(msaId), QString(msaSingleModSteps[1].objectId), "msa object id");
    CHECK_EQUAL(oldMsaVersion, msaSingleModSteps[1].version, "msa version in mod step");
    CHECK_EQUAL(U2ModType::msaUpdatedRowInfo, msaSingleModSteps[1].modType, "msa row mod step type");
    CHECK_EQUAL(expectedRowModDetails, QString(msaSingleModSteps[1].details), "msa row mod step details");

    CHECK_EQUAL(QString(msaId), QString(msaSingleModSteps[2].objectId), "msa object id");
    CHECK_EQUAL(oldMsaVersion, msaSingleModSteps[2].version, "msa version in mod step");
    CHECK_EQUAL(U2ModType::msaLengthChanged, msaSingleModSteps[2].modType, "msa gaps mod step type");
    CHECK_EQUAL(expectedLenModDetails, QString(msaSingleModSteps[2].details), "msa gaps mod step details");

    CHECK_EQUAL(QString(msaId), QString(msaSingleModSteps[3].objectId), "msa object id");
    CHECK_EQUAL(oldMsaVersion, msaSingleModSteps[3].version, "msa version in mod step");
    CHECK_EQUAL(U2ModType::msaUpdatedGapModel, msaSingleModSteps[3].modType, "msa gaps mod step type");
    CHECK_EQUAL(expectedGapModDetails, QString(msaSingleModSteps[3].details), "msa gaps mod step details");


    // Undo
    sqliteDbi->getSQLiteObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify gaps
    U2MsaRow undoRow = sqliteDbi->getMsaDbi()->getRow(msaId, rowId, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(oldRow.gaps == undoRow.gaps, "gaps after undo");

    // Verify seq
    QByteArray undoSeq = sqliteDbi->getSequenceDbi()->getSequenceData(undoRow.sequenceId, U2_REGION_MAX, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(QString(oldSeq), QString(undoSeq), "sequence");

    // Verify msa length
    U2Msa undoMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_EQUAL(13, undoMsa.length, "length");

    // Verify version
    int undoVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldMsaVersion, undoVersion, "version after undo");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, updateRowContent_redo) {
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    QList<qint64> rows = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    qint64 rowId = rows.first();

    // Get current version
    int oldMsaVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Update row content
    U2MsaRow oldRow = sqliteDbi->getMsaDbi()->getRow(msaId, rowId, os);
    CHECK_NO_ERROR(os);
    QByteArray oldSeq = sqliteDbi->getSequenceDbi()->getSequenceData(oldRow.sequenceId, U2_REGION_MAX, os);
    int oldSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(oldRow.sequenceId, os);
    CHECK_NO_ERROR(os);
    CHECK_NO_ERROR(os);
    QByteArray newSeq = "";
    U2MsaRow newRow;
    newRow.gstart = 0;
    newRow.gend = 0;
    newRow.length = 0;
    newRow.rowId = rowId;
    newRow.sequenceId = oldRow.sequenceId;
    qint64 msaModStepsNum = MsaSQLiteSpecificTestData::getModStepsNum(msaId, os);
    CHECK_NO_ERROR(os);

    // Call test function
    sqliteDbi->getMsaDbi()->updateRowContent(msaId, rowId, newSeq, newRow.gaps, os);
    CHECK_NO_ERROR(os);

    // Undo
    sqliteDbi->getSQLiteObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Redo
    sqliteDbi->getSQLiteObjectDbi()->redo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify gaps
    U2MsaRow redoRow = sqliteDbi->getMsaDbi()->getRow(msaId, rowId, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(newRow.gaps == redoRow.gaps, "gaps after undo");

    // Verify seq
    QByteArray redoSeq = sqliteDbi->getSequenceDbi()->getSequenceData(redoRow.sequenceId, U2_REGION_MAX, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(newSeq, redoSeq, "sequence");

    // Verify msa length
    U2Msa redoMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_EQUAL(13, redoMsa.length, "length");

    // Verify version
    int redoVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldMsaVersion + 1, redoVersion, "version after undo");

    // Verify single modification steps
    QString expectedSeqModDetails = U2DbiPackUtils::packSequenceDataDetails(U2_REGION_MAX, oldSeq, newSeq, QVariantMap());
    QString expectedRowModDetails = U2DbiPackUtils::packRowInfoDetails(oldRow, newRow);
    QString expectedGapModDetails = U2DbiPackUtils::packGapDetails(oldRow.rowId, oldRow.gaps, newRow.gaps);

    QList< QList<U2SingleModStep> > modSteps = sqliteDbi->getSQLiteModDbi()->getModSteps(msaId, oldMsaVersion, os);
    QList<U2SingleModStep> msaSingleModSteps;
    foreach (QList<U2SingleModStep> multiStep, modSteps) {
        foreach (U2SingleModStep singleStep, multiStep) {
            msaSingleModSteps.append(singleStep);
        }
    }

    CHECK_NO_ERROR(os);
    CHECK_EQUAL(msaModStepsNum + 3, msaSingleModSteps.count(), "msa single mod steps count");

    CHECK_EQUAL(QString(oldRow.sequenceId), QString(msaSingleModSteps[0].objectId), "seq object id");
    CHECK_EQUAL(oldSeqVersion, msaSingleModSteps[0].version, "seq version in mod step");
    CHECK_EQUAL(U2ModType::sequenceUpdatedData, msaSingleModSteps[0].modType, "seq mod step type");
    CHECK_EQUAL(expectedSeqModDetails, QString(msaSingleModSteps[0].details), "seq mod step details");

    CHECK_EQUAL(QString(msaId), QString(msaSingleModSteps[1].objectId), "msa object id");
    CHECK_EQUAL(oldMsaVersion, msaSingleModSteps[1].version, "msa version in mod step");
    CHECK_EQUAL(U2ModType::msaUpdatedRowInfo, msaSingleModSteps[1].modType, "msa row mod step type");
    CHECK_EQUAL(expectedRowModDetails, QString(msaSingleModSteps[1].details), "msa row mod step details");

    CHECK_EQUAL(QString(msaId), QString(msaSingleModSteps[2].objectId), "msa object id");
    CHECK_EQUAL(oldMsaVersion, msaSingleModSteps[2].version, "msa version in mod step");
    CHECK_EQUAL(U2ModType::msaUpdatedGapModel, msaSingleModSteps[2].modType, "msa gaps mod step type");
    CHECK_EQUAL(expectedGapModDetails, QString(msaSingleModSteps[2].details), "msa gaps mod step details");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, updateRowContent_severalSteps) {
    //6 changes, 6 undo steps, 4 redo steps, 3 undo step, 2 redo steps
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get current version
    int oldVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Get rows
    QList<U2MsaRow> oldRows = sqliteDbi->getMsaDbi()->getRows(msaId, os);
    CHECK_NO_ERROR(os);

    // Prepare value list
    QList<QPair<QByteArray, QList<U2MsaGap> > > rowContents;
    rowContents << QPair<QByteArray, QList<U2MsaGap> >(sqliteDbi->getSequenceDbi()->getSequenceData(oldRows[0].sequenceId, U2_REGION_MAX, os), oldRows[0].gaps);
    CHECK_NO_ERROR(os);
    for (int i = 0; i < 6; ++i) {
        QByteArray firstPart((i + 1) * 2, 'A');
        QByteArray secondPart((i + 3) * 2, 'C');
        QList<U2MsaGap> gapModel = QList<U2MsaGap>() << U2MsaGap((i + 2) * 2, 5);
        rowContents << QPair<QByteArray, QList<U2MsaGap> >(firstPart + secondPart, gapModel);
    }

    // Steps count
    int valuesCount = rowContents.length();    // changes = valuesCount - 1;
    QList<int> steps;                          // negative - undo steps, positive - redo steps;
    steps << -6 << 4 << -3 << 2;
    int expectedIndex = valuesCount - 1;
    for (int i = 0; i < steps.length(); ++i) {
        expectedIndex += steps[i];
    }

    //Expected msa length
    qint64 expectedMsaLength = rowContents[expectedIndex].first.length();
    CHECK_NO_ERROR(os);
    foreach (U2MsaGap gap, rowContents[expectedIndex].second) {
        expectedMsaLength += gap.gap;
    }
    expectedMsaLength = qMax(expectedMsaLength, oldRows[1].length);

    // Update row content
    for (int i = 1; i < rowContents.length(); ++i) {
        sqliteDbi->getMsaDbi()->updateRowContent(msaId, oldRows[0].rowId, rowContents[i].first, rowContents[i].second, os);
        CHECK_NO_ERROR(os);
    }

    // Undo and redo gap model updating
    int totalUndo = 0;
    int totalRedo = 0;
    for (int i  = 0; i < steps.length(); ++i) {
        if (steps[i] < 0) {
            for (int j = 0; j < -steps[i]; ++j) {
                sqliteDbi->getSQLiteObjectDbi()->undo(msaId, os);
                CHECK_NO_ERROR(os);
                totalUndo++;
            }
        } else {
            for (int j = 0; j < steps[i]; ++j) {
                sqliteDbi->getSQLiteObjectDbi()->redo(msaId, os);
                CHECK_NO_ERROR(os);
                totalRedo++;
            }
        }
    }

    // Verify gaps
    U2MsaRow finalRow = sqliteDbi->getMsaDbi()->getRow(msaId, oldRows[0].rowId, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(rowContents[expectedIndex].second == finalRow.gaps, "final gaps");

    // Verify seq
    QByteArray finalSeq = sqliteDbi->getSequenceDbi()->getSequenceData(finalRow.sequenceId, U2_REGION_MAX, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(QString(rowContents[expectedIndex].first), QString(finalSeq), "sequence");

    // Verify msa length
    U2Msa finalMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_EQUAL(expectedMsaLength, finalMsa.length, "length");

    // Verify version
    int finalVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldVersion + expectedIndex, finalVersion, "final version");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, setNewRowsOrder_noModTrack) {
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createNotSoSmallTestMsa(false, os);
    CHECK_NO_ERROR(os);

    // Get current version
    int oldVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Get current row order
    QList<qint64> oldOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_NOT_EQUAL(0, oldOrder.length(), "incorrect oreder list length");

    // Get current rows
    QList<U2MsaRow> oldRows = sqliteDbi->getMsaDbi()->getRows(msaId, os);
    CHECK_NO_ERROR(os);

    // Get current sequences
    QList<QByteArray> oldSequences;
    foreach(U2MsaRow row, oldRows) {
        oldSequences << sqliteDbi->getSequenceDbi()->getSequenceData(row.sequenceId, U2_REGION_MAX, os);
        CHECK_NO_ERROR(os);
    }

    // Get current U2Ma
    U2Msa oldMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);

    // Set new row order
    //Expected order (indexes): 4, 5, 1, 6, 0, 2, 3
    QList<qint64> newOrder = oldOrder;
    newOrder.swap(0, 4);
    newOrder.swap(3, 6);
    newOrder.swap(2, 5);
    newOrder.swap(1, 2);

    sqliteDbi->getMsaDbi()->setNewRowsOrder(msaId, newOrder, os);
    CHECK_NO_ERROR(os);

    // Verify order
    QList<qint64> newestOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(newOrder.length(), newestOrder.length(), "row order length");
    for (int i = 0; i < newOrder.length(); ++i) {
        CHECK_EQUAL(newOrder[i], newestOrder[i],  "row order");
    }

    // Verify gaps and IDs
    // Order of the list has changed
    QList<U2MsaRow> newRows = sqliteDbi->getMsaDbi()->getRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldRows.length(), newRows.length(), "rows length");
    foreach (U2MsaRow newRow, newRows) {
        bool ok = false;
        foreach (U2MsaRow oldRow, oldRows) {
            if (newRow.gaps == oldRow.gaps && newRow.gend == oldRow.gend &&
                    newRow.gstart == oldRow.gstart && newRow.length == oldRow.length &&
                    newRow.rowId == oldRow.rowId && newRow.sequenceId == oldRow.sequenceId) {
                ok = true;
                break;
            }
        }
        CHECK_TRUE(ok, "rows");
    }

    // Verify seq
    // Order of the list has changed
    foreach (U2MsaRow row, newRows) {
        QByteArray sequence = sqliteDbi->getSequenceDbi()->getSequenceData(row.sequenceId, U2_REGION_MAX, os);
        CHECK_NO_ERROR(os);
        CHECK_TRUE(oldSequences.contains(sequence), "sequence");
    }

    // Verify U2Ma
    U2Msa newMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(oldMsa.alphabet == newMsa.alphabet, "msa");
    CHECK_TRUE(oldMsa.length == newMsa.length, "msa");

    // Verify version
    int newVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldVersion + 1, newVersion, "version");

    // Verify no modification steps
    qint64 modStepsNum = MsaSQLiteSpecificTestData::getModStepsNum(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(0, modStepsNum, "mod steps num");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, setNewRowsOrder_undo) {
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createNotSoSmallTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get current version
    int oldVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Get current row order
    QList<qint64> oldOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_NOT_EQUAL(0, oldOrder.length(), "incorrect oreder list length");

    // Get current rows
    QList<U2MsaRow> oldRows = sqliteDbi->getMsaDbi()->getRows(msaId, os);
    CHECK_NO_ERROR(os);

    // Get current sequences
    QList<QByteArray> oldSequences;
    foreach(U2MsaRow row, oldRows) {
        oldSequences << sqliteDbi->getSequenceDbi()->getSequenceData(row.sequenceId, U2_REGION_MAX, os);
        CHECK_NO_ERROR(os);
    }

    // Get current U2Ma
    U2Msa oldMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);

    // Set new row order
    //Expected order (indexes): 4, 5, 1, 6, 0, 2, 3
    QList<qint64> newOrder = oldOrder;
    newOrder.swap(0, 4);
    newOrder.swap(3, 6);
    newOrder.swap(2, 5);
    newOrder.swap(1, 2);

    sqliteDbi->getMsaDbi()->setNewRowsOrder(msaId, newOrder, os);
    CHECK_NO_ERROR(os);

    // Verify order
    QList<qint64> newestOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(newOrder.length(), newestOrder.length(), "row order length");
    for (int i = 0; i < newOrder.length(); ++i) {
        CHECK_EQUAL(newOrder[i], newestOrder[i],  "row order");
    }

    // Verify gaps and IDs
    // Order of the list has changed
    QList<U2MsaRow> newRows = sqliteDbi->getMsaDbi()->getRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldRows.length(), newRows.length(), "rows length");
    foreach (U2MsaRow newRow, newRows) {
        bool ok = false;
        foreach (U2MsaRow oldRow, oldRows) {
            if (newRow.gaps == oldRow.gaps && newRow.gend == oldRow.gend &&
                    newRow.gstart == oldRow.gstart && newRow.length == oldRow.length &&
                    newRow.rowId == oldRow.rowId && newRow.sequenceId == oldRow.sequenceId) {
                ok = true;
                break;
            }
        }
        CHECK_TRUE(ok, "rows");
    }

    // Verify seq
    // Order of the list has changed
    foreach (U2MsaRow row, newRows) {
        QByteArray sequence = sqliteDbi->getSequenceDbi()->getSequenceData(row.sequenceId, U2_REGION_MAX, os);
        CHECK_NO_ERROR(os);
        CHECK_TRUE(oldSequences.contains(sequence), "sequence");
    }

    // Verify U2Ma
    U2Msa newMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(oldMsa.alphabet == newMsa.alphabet, "msa");
    CHECK_TRUE(oldMsa.length == newMsa.length, "msa");

    // Verify version
    int newVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldVersion + 1, newVersion, "version");

    // Verify modification steps
    qint64 modStepsNum = MsaSQLiteSpecificTestData::getModStepsNum(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(1, modStepsNum, "mod steps num");

    // Undo
    sqliteDbi->getObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify order
    newestOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldOrder.length(), newestOrder.length(), "row order length");
    for (int i = 0; i < oldOrder.length(); ++i) {
        CHECK_EQUAL(oldOrder[i], newestOrder[i],  "row order");
    }

    // Verify gaps and IDs
    // Order of the list has changed
    newRows = sqliteDbi->getMsaDbi()->getRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldRows.length(), newRows.length(), "rows length");
    foreach (U2MsaRow newRow, newRows) {
        bool ok = false;
        foreach (U2MsaRow oldRow, oldRows) {
            if (newRow.gaps == oldRow.gaps && newRow.gend == oldRow.gend &&
                    newRow.gstart == oldRow.gstart && newRow.length == oldRow.length &&
                    newRow.rowId == oldRow.rowId && newRow.sequenceId == oldRow.sequenceId) {
                ok = true;
                break;
            }
        }
        CHECK_TRUE(ok, "rows");
    }

    // Verify seq
    // Order of the list has changed
    foreach (U2MsaRow row, newRows) {
        QByteArray sequence = sqliteDbi->getSequenceDbi()->getSequenceData(row.sequenceId, U2_REGION_MAX, os);
        CHECK_TRUE(oldSequences.contains(sequence), "sequence");
    }

    // Verify U2Ma
    newMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(oldMsa.alphabet == newMsa.alphabet, "msa");
    CHECK_TRUE(oldMsa.length == newMsa.length, "msa");

    // Verify version
    newVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldVersion, newVersion, "version");

    // Verify modification steps
    modStepsNum = MsaSQLiteSpecificTestData::getModStepsNum(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(1, modStepsNum, "mod steps num");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, setNewRowsOrder_redo) {
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createNotSoSmallTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get current version
    int oldVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Get current row order
    QList<qint64> oldOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_NOT_EQUAL(0, oldOrder.length(), "incorrect oreder list length");

    // Get current rows
    QList<U2MsaRow> oldRows = sqliteDbi->getMsaDbi()->getRows(msaId, os);
    CHECK_NO_ERROR(os);

    // Get current sequences
    QList<QByteArray> oldSequences;
    foreach(U2MsaRow row, oldRows) {
        oldSequences << sqliteDbi->getSequenceDbi()->getSequenceData(row.sequenceId, U2_REGION_MAX, os);
        CHECK_NO_ERROR(os);
    }

    // Get current U2Ma
    U2Msa oldMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);

    // Set new row order
    //Expected order (indexes): 4, 5, 1, 6, 0, 2, 3
    QList<qint64> newOrder = oldOrder;
    newOrder.swap(0, 4);
    newOrder.swap(3, 6);
    newOrder.swap(2, 5);
    newOrder.swap(1, 2);

    sqliteDbi->getMsaDbi()->setNewRowsOrder(msaId, newOrder, os);
    CHECK_NO_ERROR(os);

    // Verify order
    QList<qint64> newestOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(newOrder.length(), newestOrder.length(), "row order length");
    for (int i = 0; i < newOrder.length(); ++i) {
        CHECK_EQUAL(newOrder[i], newestOrder[i],  "row order");
    }

    // Verify gaps and IDs
    // Order of the list has changed
    QList<U2MsaRow> newRows = sqliteDbi->getMsaDbi()->getRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldRows.length(), newRows.length(), "rows length");
    foreach (U2MsaRow newRow, newRows) {
        bool ok = false;
        foreach (U2MsaRow oldRow, oldRows) {
            if (newRow.gaps == oldRow.gaps && newRow.gend == oldRow.gend &&
                    newRow.gstart == oldRow.gstart && newRow.length == oldRow.length &&
                    newRow.rowId == oldRow.rowId && newRow.sequenceId == oldRow.sequenceId) {
                ok = true;
                break;
            }
        }
        CHECK_TRUE(ok, "rows");
    }

    // Verify seq
    // Order of the list has changed
    foreach (U2MsaRow row, newRows) {
        QByteArray sequence = sqliteDbi->getSequenceDbi()->getSequenceData(row.sequenceId, U2_REGION_MAX, os);
        CHECK_TRUE(oldSequences.contains(sequence), "sequence");
    }

    // Verify U2Ma
    U2Msa newMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(oldMsa.alphabet == newMsa.alphabet, "msa");
    CHECK_TRUE(oldMsa.length == newMsa.length, "msa");

    // Verify version
    int newVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldVersion + 1, newVersion, "version");

    // Verify modification steps
    qint64 modStepsNum = MsaSQLiteSpecificTestData::getModStepsNum(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(1, modStepsNum, "mod steps num");

    // Undo
    sqliteDbi->getObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify order
    newestOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldOrder.length(), newestOrder.length(), "row order length");
    for (int i = 0; i < oldOrder.length(); ++i) {
        CHECK_EQUAL(oldOrder[i], newestOrder[i],  "row order");
    }

    // Verify gaps and IDs
    // Order of the list has changed
    newRows = sqliteDbi->getMsaDbi()->getRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldRows.length(), newRows.length(), "rows length");
    foreach (U2MsaRow newRow, newRows) {
        bool ok = false;
        foreach (U2MsaRow oldRow, oldRows) {
            if (newRow.gaps == oldRow.gaps && newRow.gend == oldRow.gend &&
                    newRow.gstart == oldRow.gstart && newRow.length == oldRow.length &&
                    newRow.rowId == oldRow.rowId && newRow.sequenceId == oldRow.sequenceId) {
                ok = true;
                break;
            }
        }
        CHECK_TRUE(ok, "rows");
    }

    // Verify seq
    // Order of the list has changed
    foreach (U2MsaRow row, newRows) {
        QByteArray sequence = sqliteDbi->getSequenceDbi()->getSequenceData(row.sequenceId, U2_REGION_MAX, os);
        CHECK_TRUE(oldSequences.contains(sequence), "sequence");
    }

    // Verify U2Ma
    newMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(oldMsa.alphabet == newMsa.alphabet, "msa");
    CHECK_TRUE(oldMsa.length == newMsa.length, "msa");

    // Verify version
    newVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldVersion, newVersion, "version");

    // Verify modification steps
    modStepsNum = MsaSQLiteSpecificTestData::getModStepsNum(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(1, modStepsNum, "mod steps num");

    // Redo
    sqliteDbi->getObjectDbi()->redo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify order
    newestOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(newOrder.length(), newestOrder.length(), "row order length");
    for (int i = 0; i < newOrder.length(); ++i) {
        CHECK_EQUAL(newOrder[i], newestOrder[i],  "row order");
    }

    // Verify gaps and IDs
    // Order of the list has changed
    newRows = sqliteDbi->getMsaDbi()->getRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldRows.length(), newRows.length(), "rows length");
    foreach (U2MsaRow newRow, newRows) {
        bool ok = false;
        foreach (U2MsaRow oldRow, oldRows) {
            if (newRow.gaps == oldRow.gaps && newRow.gend == oldRow.gend &&
                    newRow.gstart == oldRow.gstart && newRow.length == oldRow.length &&
                    newRow.rowId == oldRow.rowId && newRow.sequenceId == oldRow.sequenceId) {
                ok = true;
                break;
            }
        }
        CHECK_TRUE(ok, "rows");
    }

    // Verify seq
    // Order of the list has changed
    foreach (U2MsaRow row, newRows) {
        QByteArray sequence = sqliteDbi->getSequenceDbi()->getSequenceData(row.sequenceId, U2_REGION_MAX, os);
        CHECK_NO_ERROR(os);
        CHECK_TRUE(oldSequences.contains(sequence), "sequence");
    }

    // Verify U2Ma
    newMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(oldMsa.alphabet == newMsa.alphabet, "msa");
    CHECK_TRUE(oldMsa.length == newMsa.length, "msa");

    // Verify version
    newVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldVersion + 1, newVersion, "version");

    // Verify modification steps
    modStepsNum = MsaSQLiteSpecificTestData::getModStepsNum(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(1, modStepsNum, "mod steps num");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, setNewRowsOrder_severalSteps) {
    //6 changes, 5 undo steps, 3 redo steps, 4 undo step, 3 redo steps
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get current version
    int objVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Get rows
    QList<U2MsaRow> oldRows = sqliteDbi->getMsaDbi()->getRows(msaId, os);
    CHECK_NO_ERROR(os);

    // Get current sequences
    QList<QByteArray> oldSequences;
    foreach(U2MsaRow row, oldRows) {
        oldSequences << sqliteDbi->getSequenceDbi()->getSequenceData(row.sequenceId, U2_REGION_MAX, os);
        CHECK_NO_ERROR(os);
    }

    // Get current U2Ma
    U2Msa oldMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);

    // Prepare value list
    QList<QList<qint64> > rowOrders;
    rowOrders << sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);  // base value
    CHECK_NO_ERROR(os);

    QList<qint64> rowOrder;
    for (int i = 0; i < 6; ++i) {
        rowOrder.clear();
        for (int j = 1; j <= oldRows.length(); ++j) {
            // On every step:
            // 0 1 2 3 4 5 6
            // 4 1 5 2 6 3 0
            rowOrder << rowOrders[i][(j * 11) % oldRows.length()];
        }
        rowOrders << rowOrder;
    }

    // Steps count
    int valuesCount = rowOrders.length();    // changes = valuesCount - 1;
    QList<int> steps;                        // negative - undo steps, positive - redo steps;
    steps << -5 << 3 << -4 << 3;
    int expectedIndex = valuesCount - 1;
    for (int i = 0; i < steps.length(); ++i) {
        expectedIndex += steps[i];
    }

    // Update the row order
    for (int i = 1; i < rowOrders.length(); ++i) {
        sqliteDbi->getMsaDbi()->setNewRowsOrder(msaId, rowOrders[i], os);
        CHECK_NO_ERROR(os);
    }

    // Undo and redo row order updating
    int totalUndo = 0;
    int totalRedo = 0;
    for (int i  = 0; i < steps.length(); ++i) {
        if (steps[i] < 0) {
            for (int j = 0; j < -steps[i]; ++j) {
                sqliteDbi->getSQLiteObjectDbi()->undo(msaId, os);
                CHECK_NO_ERROR(os);
                totalUndo++;
            }
        } else {
            for (int j = 0; j < steps[i]; ++j) {
                sqliteDbi->getSQLiteObjectDbi()->redo(msaId, os);
                CHECK_NO_ERROR(os);
                totalRedo++;
            }
        }
    }

    // Verify order
    QList<qint64> finalOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(rowOrders[expectedIndex].length(), finalOrder.length(), "row order length");
    for (int i = 0; i < finalOrder.length(); ++i) {
        CHECK_EQUAL(rowOrders[expectedIndex][i], finalOrder[i],  "row order");
    }

    // Verify gaps and IDs
    // Order of the list has changed
    QList<U2MsaRow> newRows = sqliteDbi->getMsaDbi()->getRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldRows.length(), newRows.length(), "rows length");
    foreach (U2MsaRow newRow, newRows) {
        bool ok = false;
        foreach (U2MsaRow oldRow, oldRows) {
            if (newRow.gaps == oldRow.gaps && newRow.gend == oldRow.gend &&
                    newRow.gstart == oldRow.gstart && newRow.length == oldRow.length &&
                    newRow.rowId == oldRow.rowId && newRow.sequenceId == oldRow.sequenceId) {
                ok = true;
                break;
            }
        }
        CHECK_TRUE(ok, "rows");
    }

    // Verify seq
    // Order of the list has changed
    foreach (U2MsaRow row, newRows) {
        QByteArray sequence = sqliteDbi->getSequenceDbi()->getSequenceData(row.sequenceId, U2_REGION_MAX, os);
        CHECK_NO_ERROR(os);
        CHECK_TRUE(oldSequences.contains(sequence), "sequence");
    }

    // Verify U2Ma
    U2Msa newMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(oldMsa.alphabet == newMsa.alphabet, "msa");
    CHECK_TRUE(oldMsa.length == newMsa.length, "msa");

    // Verify version
    qint64 newVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(objVersion + expectedIndex, newVersion, "version");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, updateRowName_noModTrack) {
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(false, os);
    CHECK_NO_ERROR(os);
    QList<qint64> rows = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    qint64 rowId = rows.first();

    // Get current msa version
    int oldMsaVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    U2MsaRow oldRow = sqliteDbi->getMsaDbi()->getRow(msaId, rowId, os);
    CHECK_NO_ERROR(os);
    U2Sequence oldSeq = sqliteDbi->getSequenceDbi()->getSequenceObject(oldRow.sequenceId, os);
    CHECK_NO_ERROR(os);

    // Get current seq version
    int oldSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(oldSeq.id, os);
    CHECK_NO_ERROR(os);

    // Update row name
    QString oldName = oldSeq.visualName;
    QString newName = oldName + "_new";
    sqliteDbi->getMsaDbi()->updateRowName(msaId, rowId, newName, os);
    CHECK_NO_ERROR(os);

    // Verify name
    U2MsaRow newRow = sqliteDbi->getMsaDbi()->getRow(msaId, rowId, os);
    CHECK_NO_ERROR(os);
    QString newestName = sqliteDbi->getSequenceDbi()->getSequenceObject(newRow.sequenceId, os).visualName;
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(newName, newestName, "name");

    // Verify msa version
    int newMsaVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldMsaVersion + 1, newMsaVersion, "msa version");

    // Verify seq version
    int newSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(oldSeq.id, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldSeqVersion + 1, newSeqVersion, "seq version");

    // Verify no modification steps
    qint64 modStepsNum = MsaSQLiteSpecificTestData::getModStepsNum(oldSeq.id, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(0, modStepsNum, "mod steps num");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, updateRowName_undo) {
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    QList<qint64> rows = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    qint64 rowId = rows[1];

    // Get current msa version
    int oldMsaVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    U2MsaRow oldRow = sqliteDbi->getMsaDbi()->getRow(msaId, rowId, os);
    CHECK_NO_ERROR(os);
    U2Sequence oldSeq = sqliteDbi->getSequenceDbi()->getSequenceObject(oldRow.sequenceId, os);
    CHECK_NO_ERROR(os);

    // Get current seq version
    int oldSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(oldSeq.id, os);
    CHECK_NO_ERROR(os);

    // Update row name
    QString oldName = oldSeq.visualName;
    QString newName = oldName + "_new";
    sqliteDbi->getMsaDbi()->updateRowName(msaId, rowId, newName, os);
    CHECK_NO_ERROR(os);

    // Verify name
    U2MsaRow newRow = sqliteDbi->getMsaDbi()->getRow(msaId, rowId, os);
    CHECK_NO_ERROR(os);
    QString newestName = sqliteDbi->getSequenceDbi()->getSequenceObject(newRow.sequenceId, os).visualName;
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(newName, newestName, "name");

    // Verify msa version
    int newMsaVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldMsaVersion + 1, newMsaVersion, "msa version");

    // Verify seq version
    int newSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(oldSeq.id, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldSeqVersion + 1, newSeqVersion, "seq version");

    // Verify the modification step
    QString expectedModDetails = "0\t2\t2_new";
    U2SingleModStep modStep = sqliteDbi->getModDbi()->getModStep(oldSeq.id, oldSeqVersion, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(QString(oldSeq.id), QString(modStep.objectId), "object id");
    CHECK_EQUAL(oldSeqVersion, modStep.version, "version in mod step");
    CHECK_EQUAL(U2ModType::objUpdatedName, modStep.modType, "mod step type");
    CHECK_EQUAL(expectedModDetails, QString(modStep.details), "mod step details");

    // Undo
    sqliteDbi->getSQLiteObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify name
    U2MsaRow undoRow = sqliteDbi->getMsaDbi()->getRow(msaId, rowId, os);
    CHECK_NO_ERROR(os);
    QString undoName = sqliteDbi->getSequenceDbi()->getSequenceObject(undoRow.sequenceId, os).visualName;
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldName, undoName, "name after undo");

    // Verify msa version
    int undoMsaVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldMsaVersion, undoMsaVersion, "msa version after undo");

    // Verify seq version
    int undoSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(oldSeq.id, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldSeqVersion, undoSeqVersion, "seq version after undo");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, updateRowName_redo) {
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    QList<qint64> rows = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    qint64 rowId = rows[1];

    // Get current msa version
    int oldMsaVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    U2MsaRow oldRow = sqliteDbi->getMsaDbi()->getRow(msaId, rowId, os);
    CHECK_NO_ERROR(os);
    U2Sequence oldSeq = sqliteDbi->getSequenceDbi()->getSequenceObject(oldRow.sequenceId, os);
    CHECK_NO_ERROR(os);

    // Get current seq version
    int oldSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(oldSeq.id, os);
    CHECK_NO_ERROR(os);

    // Update row name
    QString oldName = oldSeq.visualName;
    QString newName = oldName + "_new_new";
    sqliteDbi->getMsaDbi()->updateRowName(msaId, rowId, newName, os);
    CHECK_NO_ERROR(os);

    // Undo
    sqliteDbi->getSQLiteObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Redo
    sqliteDbi->getSQLiteObjectDbi()->redo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify name
    U2MsaRow redoRow = sqliteDbi->getMsaDbi()->getRow(msaId, rowId, os);
    CHECK_NO_ERROR(os);
    QString redoName = sqliteDbi->getSequenceDbi()->getSequenceObject(redoRow.sequenceId, os).visualName;
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(newName, redoName, "name after undo");

    // Verify msa version
    int redoMsaVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldMsaVersion + 1, redoMsaVersion, "msa version after redo");

    // Verify seq version
    int redoSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(oldSeq.id, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldSeqVersion + 1, redoSeqVersion, "seq version after redo");

    // Verify the modification step
    QString expectedModDetails = "0\t2\t2_new_new";
    U2SingleModStep modStep = sqliteDbi->getModDbi()->getModStep(oldSeq.id, oldSeqVersion, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(QString(oldSeq.id), QString(modStep.objectId), "object id");
    CHECK_EQUAL(oldSeqVersion, modStep.version, "version in mod step");
    CHECK_EQUAL(U2ModType::objUpdatedName, modStep.modType, "mod step type");
    CHECK_EQUAL(expectedModDetails, QString(modStep.details), "mod step details");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, updateRowName_severalSteps) {
    //6 changes, 3 undo steps, 1 redo steps, 4 undo step, 3 redo steps
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get rows
    QList<U2MsaRow> oldRows = sqliteDbi->getMsaDbi()->getRows(msaId, os);
    CHECK_NO_ERROR(os);

    // Get current seq version
    int oldSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(oldRows[1].sequenceId, os);
    CHECK_NO_ERROR(os);

    // Get row ids
    QList<qint64> rowIds = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);

    // Prepare value list
    QStringList rowNames;
    U2MsaRow oldRow = sqliteDbi->getMsaDbi()->getRow(msaId, oldRows[1].rowId, os);
    CHECK_NO_ERROR(os);
    rowNames << sqliteDbi->getSequenceDbi()->getSequenceObject(oldRow.sequenceId, os).visualName;
    CHECK_NO_ERROR(os);

    for (int i = 0; i < 6; ++i) {
        rowNames << rowNames[i] + "test";
    }

    // Steps count
    int valuesCount = rowNames.length();     // changes = valuesCount - 1;
    QList<int> steps;                        // negative - undo steps, positive - redo steps;
    steps << -3 << 1 << -4 << 3;
    int expectedIndex = valuesCount - 1;
    for (int i = 0; i < steps.length(); ++i) {
        expectedIndex += steps[i];
    }

    // Update row name
    for (int i = 1; i < rowNames.length(); ++i) {
        sqliteDbi->getMsaDbi()->updateRowName(msaId, oldRows[1].rowId, rowNames[i], os);
        CHECK_NO_ERROR(os);
    }

    // Undo and redo row order updating
    int totalUndo = 0;
    int totalRedo = 0;
    for (int i  = 0; i < steps.length(); ++i) {
        if (steps[i] < 0) {
            for (int j = 0; j < -steps[i]; ++j) {
                sqliteDbi->getSQLiteObjectDbi()->undo(msaId, os);
                CHECK_NO_ERROR(os);
                totalUndo++;
            }
        } else {
            for (int j = 0; j < steps[i]; ++j) {
                sqliteDbi->getSQLiteObjectDbi()->redo(msaId, os);
                CHECK_NO_ERROR(os);
                totalRedo++;
            }
        }
    }

    // Verify name
    U2MsaRow finalRow = sqliteDbi->getMsaDbi()->getRow(msaId, oldRows[1].rowId, os);
    CHECK_NO_ERROR(os);
    QString finalName = sqliteDbi->getSequenceDbi()->getSequenceObject(finalRow.sequenceId, os).visualName;
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(rowNames[expectedIndex], finalName, "name after all");

    // Verify version
    int finalVersion = sqliteDbi->getObjectDbi()->getObjectVersion(oldRows[1].sequenceId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldSeqVersion + expectedIndex, finalVersion, "version after all");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, addRow_append_noModTrack) {
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(false, os);
    CHECK_NO_ERROR(os);

    // Get current version
    int oldVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Add a row
    qint64 seqLength = 100;
    U2DataId seqId = MsaSQLiteSpecificTestData::createTestSequence(false, seqLength, os);
    CHECK_NO_ERROR(os);

    QList<U2MsaGap> gaps;
    qint64 gapLength = 2;
    gaps << U2MsaGap(1, gapLength);

    U2MsaRow row;
    row.sequenceId = seqId;
    row.gstart = 1;
    row.gend = 20;
    row.gaps = gaps;
    qint64 rowLength = row.gend - row.gstart + gapLength;
    row.length = rowLength;

    sqliteDbi->getMsaDbi()->addRow(msaId, -1, row, os);
    CHECK_NO_ERROR(os);

    // Verify the row
    U2MsaRow newRow = sqliteDbi->getMsaDbi()->getRow(msaId, row.rowId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(seqId, newRow.sequenceId, "sequence id");
    CHECK_EQUAL(1, newRow.gstart, "gstart");
    CHECK_EQUAL(20, newRow.gend, "gend");
    CHECK_TRUE(gaps == newRow.gaps, "gaps");
    CHECK_EQUAL(rowLength, newRow.length, "row length");

    // Verify MSA length
    U2Msa msaObj = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(rowLength, msaObj.length, "msa length");

    // Verify number of rows
    qint64 numOfRows = sqliteDbi->getMsaDbi()->getNumOfRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(3, numOfRows, "number of rows");

    // Verify rows order
    QList<qint64> rowsOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(3, rowsOrder.count(), "rows order items number");
    CHECK_EQUAL(row.rowId, rowsOrder.last(), "last row id");

    // Verify version
    int newVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldVersion + 1, newVersion, "version");

    // Verify no modification steps
    qint64 modStepsNum = MsaSQLiteSpecificTestData::getModStepsNum(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(0, modStepsNum, "mod steps num");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, addRow_append_undo) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get current version
    int objVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Get msa length, number of rows and rows order
    U2Msa oldMsaObj = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    qint64 oldMsaLength = oldMsaObj.length;
    qint64 oldNumOfRows = sqliteDbi->getMsaDbi()->getNumOfRows(msaId, os);
    CHECK_NO_ERROR(os);
    QList<qint64> oldRowsOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);

    // Add a row
    qint64 seqLength = 100;
    U2DataId seqId = MsaSQLiteSpecificTestData::createTestSequence(false, seqLength, os);
    CHECK_NO_ERROR(os);

    QList<U2MsaGap> gaps;
    qint64 gapLength = 2;
    gaps << U2MsaGap(1, gapLength);

    U2MsaRow row;
    row.sequenceId = seqId;
    row.gstart = 1;
    row.gend = 20;
    row.gaps = gaps;
    qint64 rowLength = row.gend - row.gstart + gapLength;
    row.length = rowLength;

    sqliteDbi->getMsaDbi()->addRow(msaId, -1, row, os);
    CHECK_NO_ERROR(os);

    // Verify the row
    U2MsaRow newRow = sqliteDbi->getMsaDbi()->getRow(msaId, row.rowId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(seqId, newRow.sequenceId, "sequence id");
    CHECK_EQUAL(1, newRow.gstart, "gstart");
    CHECK_EQUAL(20, newRow.gend, "gend");
    CHECK_TRUE(gaps == newRow.gaps, "gaps");
    CHECK_EQUAL(rowLength, newRow.length, "row length");

    // Verify MSA length
    U2Msa msaObj = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(rowLength, msaObj.length, "msa length");

    // Verify number of rows
    qint64 numOfRows = sqliteDbi->getMsaDbi()->getNumOfRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(3, numOfRows, "number of rows");

    // Verify rows order
    QList<qint64> rowsOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(3, rowsOrder.count(), "rows order items number");
    CHECK_EQUAL(row.rowId, rowsOrder.last(), "last row id");

    // Verify the version has been incremented
    int versionAfterUpdate = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(objVersion + 1, versionAfterUpdate, "version");

    // Verify the modification step
    QList< QList<U2SingleModStep> > modSteps = sqliteDbi->getSQLiteModDbi()->getModSteps(msaId, objVersion, os);
    CHECK_EQUAL(1, modSteps.count(), "mod steps count");
    CHECK_EQUAL(2, modSteps.first().count(), "mod single steps count");
    U2SingleModStep modStep = modSteps.first().last();
    QString expectedModDetails = "0\t-1\t" +
        QByteArray::number(row.rowId) + "\t" +
        row.sequenceId.toHex() + "\t" +
        "1\t20\t\"1,2\"";
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(QString(msaId), QString(modStep.objectId), "object id");
    CHECK_EQUAL(objVersion, modStep.version, "version in mod step");
    CHECK_EQUAL(U2ModType::msaAddedRow, modStep.modType, "mod step type");
    CHECK_EQUAL(expectedModDetails, QString(modStep.details), "mod step details");

    modStep = modSteps.first().first();
    CHECK_EQUAL(QString(msaId), QString(modStep.objectId), "object id");
    CHECK_EQUAL(objVersion, modStep.version, "version in mod step");
    CHECK_EQUAL(U2ModType::msaLengthChanged, modStep.modType, "mod step type");

    // Undo adding the row
    sqliteDbi->getSQLiteObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify msa length, number of rows and rows order
    U2Msa msaObjAfterUndo = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    qint64 msaLengthAfterUndo = msaObjAfterUndo.length;
    qint64 numOfRowsAfterUndo = sqliteDbi->getMsaDbi()->getNumOfRows(msaId, os);
    CHECK_NO_ERROR(os);
    QList<qint64> rowsOrderAfterUndo = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldMsaLength, msaLengthAfterUndo, "msa length after undo");
    CHECK_EQUAL(oldNumOfRows, numOfRowsAfterUndo, "number of rows after undo");
    CHECK_TRUE(oldRowsOrder == rowsOrderAfterUndo, "rows ids after undo");

    // Verify the version
    CHECK_EQUAL(objVersion, msaObjAfterUndo.version, "version after undo");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, addRow_append_redo) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get current version
    int objVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Add a row
    qint64 seqLength = 100;
    U2DataId seqId = MsaSQLiteSpecificTestData::createTestSequence(false, seqLength, os);
    CHECK_NO_ERROR(os);

    QList<U2MsaGap> gaps;
    qint64 gapLength = 2;
    gaps << U2MsaGap(1, gapLength);

    U2MsaRow row;
    row.sequenceId = seqId;
    row.gstart = 1;
    row.gend = 20;
    row.gaps = gaps;
    qint64 rowLength = row.gend - row.gstart + gapLength;
    row.length = rowLength;

    sqliteDbi->getMsaDbi()->addRow(msaId, -1, row, os);
    CHECK_NO_ERROR(os);

    // Undo row addition
    sqliteDbi->getSQLiteObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Redo row addition
    sqliteDbi->getSQLiteObjectDbi()->redo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify the row
    U2MsaRow newRow = sqliteDbi->getMsaDbi()->getRow(msaId, row.rowId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(QString(seqId), QString(newRow.sequenceId), "sequence id");
    CHECK_EQUAL(1, newRow.gstart, "gstart");
    CHECK_EQUAL(20, newRow.gend, "gend");
    CHECK_TRUE(gaps == newRow.gaps, "gaps");
    CHECK_EQUAL(rowLength, newRow.length, "row length");

    // Verify MSA length
    U2Msa msaObj = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(rowLength, msaObj.length, "msa length");

    // Verify number of rows
    qint64 numOfRows = sqliteDbi->getMsaDbi()->getNumOfRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(3, numOfRows, "number of rows");

    // Verify rows order
    QList<qint64> rowsOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(3, rowsOrder.count(), "rows order items number");
    CHECK_EQUAL(row.rowId, rowsOrder.last(), "last row id");

    // Verify the version
    CHECK_EQUAL(objVersion + 1, msaObj.version, "version after undo");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, addRow_zeroPos_undo) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createNotSoSmallTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get current values
    int baseVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    U2Msa baseMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    qint64 baseNumOfRows = sqliteDbi->getMsaDbi()->getNumOfRows(msaId, os);
    QList<qint64> baseRowsOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);

    // Prepare test values
    qint64 seqLength = 100;
    U2DataId seqId = MsaSQLiteSpecificTestData::createTestSequence(false, seqLength, os);
    CHECK_NO_ERROR(os);

    QList<U2MsaGap> gaps;
    qint64 gapLength = 2;
    gaps << U2MsaGap(1, gapLength);

    U2MsaRow row;
    row.sequenceId = seqId;
    row.gstart = 1;
    row.gend = 20;
    row.gaps = gaps;
    qint64 rowLength = row.gend - row.gstart + gapLength;
    row.length = rowLength;

    qint64 posInMsa = 0;

    // Prepare expected values
    qint64 expectedMsaLength = qMax(baseMsa.length, rowLength);

    // Call test function
    sqliteDbi->getMsaDbi()->addRow(msaId, posInMsa, row, os);
    CHECK_NO_ERROR(os);

    // Verify the row
    U2MsaRow newRow = sqliteDbi->getMsaDbi()->getRow(msaId, row.rowId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(QString(row.sequenceId), QString(newRow.sequenceId), "sequence id");
    CHECK_EQUAL(row.gstart, newRow.gstart, "gstart");
    CHECK_EQUAL(row.gend, newRow.gend, "gend");
    CHECK_TRUE(row.gaps == newRow.gaps, "gaps");
    CHECK_EQUAL(rowLength, newRow.length, "row length");

    // Verify MSA length
    U2Msa newMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(expectedMsaLength, newMsa.length, "msa length");

    // Verify number of rows
    qint64 numOfRows = sqliteDbi->getMsaDbi()->getNumOfRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseNumOfRows + 1, numOfRows, "number of rows");

    // Verify rows order
    QList<qint64> rowsOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseRowsOrder.count() + 1, rowsOrder.count(), "rows order items number");
    CHECK_EQUAL(row.rowId, rowsOrder.first(), "first row id");
    rowsOrder.removeFirst();
    CHECK_TRUE(baseRowsOrder == rowsOrder, "other row ids");

    // Verify the version has been incremented
    int versionAfterUpdate = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseVersion + 1, versionAfterUpdate, "version");

    // Verify the modification step
    U2SingleModStep modStep = sqliteDbi->getModDbi()->getModStep(msaId, baseVersion, os);
    QString expectedModDetails = "0\t" +
            QByteArray::number(posInMsa) + "\t" +
            QByteArray::number(row.rowId) + "\t" +
            row.sequenceId.toHex() + "\t" +
            "1\t20\t\"1,2\"";
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(QString(msaId), QString(modStep.objectId), "object id");
    CHECK_EQUAL(baseVersion, modStep.version, "version in mod step");
    CHECK_EQUAL(U2ModType::msaAddedRow, modStep.modType, "mod step type");
    CHECK_EQUAL(expectedModDetails, QString(modStep.details), "mod step details");

    // Undo adding the row
    sqliteDbi->getSQLiteObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify msa length, number of rows and rows order
    U2Msa msaAfterUndo = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    qint64 msaLengthAfterUndo = msaAfterUndo.length;
    qint64 numOfRowsAfterUndo = sqliteDbi->getMsaDbi()->getNumOfRows(msaId, os);
    CHECK_NO_ERROR(os);
    QList<qint64> rowsOrderAfterUndo = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseMsa.length, msaLengthAfterUndo, "msa length after undo");
    CHECK_EQUAL(baseNumOfRows, numOfRowsAfterUndo, "number of rows after undo");
    CHECK_TRUE(baseRowsOrder == rowsOrderAfterUndo, "rows ids after undo");

    // Verify the version
    CHECK_EQUAL(baseVersion, msaAfterUndo.version, "version after undo");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, addRow_zeroPos_redo) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createNotSoSmallTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get current values
    int baseVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    U2Msa baseMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    qint64 baseNumOfRows = sqliteDbi->getMsaDbi()->getNumOfRows(msaId, os);
    QList<qint64> baseRowsOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);

    // Prepare test values
    qint64 seqLength = 100;
    U2DataId seqId = MsaSQLiteSpecificTestData::createTestSequence(false, seqLength, os);
    CHECK_NO_ERROR(os);

    QList<U2MsaGap> gaps;
    qint64 gapLength = 2;
    gaps << U2MsaGap(1, gapLength);

    U2MsaRow row;
    row.sequenceId = seqId;
    row.gstart = 1;
    row.gend = 20;
    row.gaps = gaps;
    qint64 rowLength = row.gend - row.gstart + gapLength;
    row.length = rowLength;

    qint64 posInMsa = 0;

    // Prepare expected values
    qint64 expectedMsaLength = qMax(baseMsa.length, rowLength);

    // Call test function
    sqliteDbi->getMsaDbi()->addRow(msaId, posInMsa, row, os);
    CHECK_NO_ERROR(os);

    // Undo adding the row
    sqliteDbi->getSQLiteObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Redo row addition
    sqliteDbi->getSQLiteObjectDbi()->redo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify the row
    U2MsaRow rowAfterRedo = sqliteDbi->getMsaDbi()->getRow(msaId, row.rowId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(QString(row.sequenceId), QString(rowAfterRedo.sequenceId), "sequence id");
    CHECK_EQUAL(row.gstart, rowAfterRedo.gstart, "gstart");
    CHECK_EQUAL(row.gend, rowAfterRedo.gend, "gend");
    CHECK_TRUE(row.gaps == rowAfterRedo.gaps, "gaps");
    CHECK_EQUAL(rowLength, rowAfterRedo.length, "row length");

    // Verify MSA length
    U2Msa msaAfterRedo = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(expectedMsaLength, msaAfterRedo.length, "msa length");

    // Verify number of rows
    qint64 numOfRowsAfterRedo = sqliteDbi->getMsaDbi()->getNumOfRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseNumOfRows + 1, numOfRowsAfterRedo, "number of rows");

    // Verify rows order
    QList<qint64> rowsOrderAfterRedo = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseRowsOrder.count() + 1, rowsOrderAfterRedo.count(), "rows order items number");
    CHECK_EQUAL(row.rowId, rowsOrderAfterRedo.first(), "first row id");
    rowsOrderAfterRedo.removeFirst();
    CHECK_TRUE(baseRowsOrder == rowsOrderAfterRedo, "other row ids");

    // Verify the version has been incremented
    int versionAfterRedo = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseVersion + 1, versionAfterRedo, "version");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, addRow_middlePos_undo) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createNotSoSmallTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get current values
    int baseVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    U2Msa baseMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    qint64 baseNumOfRows = sqliteDbi->getMsaDbi()->getNumOfRows(msaId, os);
    QList<qint64> baseRowsOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);

    // Prepare test values
    qint64 seqLength = 100;
    U2DataId seqId = MsaSQLiteSpecificTestData::createTestSequence(false, seqLength, os);
    CHECK_NO_ERROR(os);

    QList<U2MsaGap> gaps;
    qint64 gapLength = 2;
    gaps << U2MsaGap(1, gapLength);

    U2MsaRow row;
    row.sequenceId = seqId;
    row.gstart = 1;
    row.gend = 20;
    row.gaps = gaps;
    qint64 posInMsa = 3;
    row.length = row.gend - row.gstart + gapLength;

    // Call test function
    sqliteDbi->getMsaDbi()->addRow(msaId, posInMsa, row, os);
    CHECK_NO_ERROR(os);

    // Undo adding the row
    sqliteDbi->getSQLiteObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify MSA length
    U2Msa msaAfterRedo = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseMsa.length, msaAfterRedo.length, "msa length");

    // Verify number of rows
    qint64 numOfRowsAfterRedo = sqliteDbi->getMsaDbi()->getNumOfRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseNumOfRows, numOfRowsAfterRedo, "number of rows");

    // Verify rows order
    QList<qint64> rowsOrderAfterRedo = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseRowsOrder.count(), rowsOrderAfterRedo.count(), "rows order items number");
    CHECK_TRUE(baseRowsOrder == rowsOrderAfterRedo, "other row ids");

    // Verify the version has been incremented
    int versionAfterRedo = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseVersion, versionAfterRedo, "version");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, addRow_middlePos_redo) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createNotSoSmallTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get current values
    int baseVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    U2Msa baseMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    qint64 baseNumOfRows = sqliteDbi->getMsaDbi()->getNumOfRows(msaId, os);
    QList<qint64> baseRowsOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);

    // Prepare test values
    qint64 seqLength = 100;
    U2DataId seqId = MsaSQLiteSpecificTestData::createTestSequence(false, seqLength, os);
    CHECK_NO_ERROR(os);

    QList<U2MsaGap> gaps;
    qint64 gapLength = 2;
    gaps << U2MsaGap(1, gapLength);

    U2MsaRow row;
    row.sequenceId = seqId;
    row.gstart = 1;
    row.gend = 20;
    row.gaps = gaps;
    qint64 rowLength = row.gend - row.gstart + gapLength;
    row.length = rowLength;

    qint64 posInMsa = 3;

    // Prepare expected values
    qint64 expectedMsaLength = qMax(baseMsa.length, rowLength);

    // Call test function
    sqliteDbi->getMsaDbi()->addRow(msaId, posInMsa, row, os);
    CHECK_NO_ERROR(os);

    // Undo adding the row
    sqliteDbi->getSQLiteObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Redo row addition
    sqliteDbi->getSQLiteObjectDbi()->redo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify the row
    U2MsaRow rowAfterRedo = sqliteDbi->getMsaDbi()->getRow(msaId, row.rowId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(QString(row.sequenceId), QString(rowAfterRedo.sequenceId), "sequence id");
    CHECK_EQUAL(row.gstart, rowAfterRedo.gstart, "gstart");
    CHECK_EQUAL(row.gend, rowAfterRedo.gend, "gend");
    CHECK_TRUE(row.gaps == rowAfterRedo.gaps, "gaps");
    CHECK_EQUAL(rowLength, rowAfterRedo.length, "row length");

    // Verify MSA length
    U2Msa msaAfterRedo = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(expectedMsaLength, msaAfterRedo.length, "msa length");

    // Verify number of rows
    qint64 numOfRowsAfterRedo = sqliteDbi->getMsaDbi()->getNumOfRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseNumOfRows + 1, numOfRowsAfterRedo, "number of rows");

    // Verify rows order
    QList<qint64> rowsOrderAfterRedo = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseRowsOrder.count() + 1, rowsOrderAfterRedo.count(), "rows order items number");
    CHECK_EQUAL(row.rowId, rowsOrderAfterRedo[posInMsa], "new row id");
    rowsOrderAfterRedo.removeAt(posInMsa);
    CHECK_TRUE(baseRowsOrder == rowsOrderAfterRedo, "other row ids");

    // Verify the version has been incremented
    int versionAfterRedo = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseVersion + 1, versionAfterRedo, "version");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, addRow_lastRowPos_undo) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createNotSoSmallTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get current values
    int baseVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    U2Msa baseMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    qint64 baseNumOfRows = sqliteDbi->getMsaDbi()->getNumOfRows(msaId, os);
    QList<qint64> baseRowsOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);

    // Prepare test values
    qint64 seqLength = 100;
    U2DataId seqId = MsaSQLiteSpecificTestData::createTestSequence(false, seqLength, os);
    CHECK_NO_ERROR(os);

    QList<U2MsaGap> gaps;
    qint64 gapLength = 2;
    gaps << U2MsaGap(1, gapLength);

    U2MsaRow row;
    row.sequenceId = seqId;
    row.gstart = 1;
    row.gend = 20;
    row.gaps = gaps;
    qint64 posInMsa = baseNumOfRows;
    row.length = row.gend - row.gstart + gapLength;

    // Call test function
    sqliteDbi->getMsaDbi()->addRow(msaId, posInMsa, row, os);
    CHECK_NO_ERROR(os);

    // Undo adding the row
    sqliteDbi->getSQLiteObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify MSA length
    U2Msa msaAfterRedo = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseMsa.length, msaAfterRedo.length, "msa length");

    // Verify number of rows
    qint64 numOfRowsAfterRedo = sqliteDbi->getMsaDbi()->getNumOfRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseNumOfRows, numOfRowsAfterRedo, "number of rows");

    // Verify rows order
    QList<qint64> rowsOrderAfterRedo = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseRowsOrder.count(), rowsOrderAfterRedo.count(), "rows order items number");
    CHECK_TRUE(baseRowsOrder == rowsOrderAfterRedo, "other row ids");

    // Verify the version has been incremented
    int versionAfterRedo = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseVersion, versionAfterRedo, "version");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, addRow_lastRowPos_redo) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createNotSoSmallTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get current values
    int baseVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    U2Msa baseMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    qint64 baseNumOfRows = sqliteDbi->getMsaDbi()->getNumOfRows(msaId, os);
    QList<qint64> baseRowsOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);

    // Prepare test values
    qint64 seqLength = 100;
    U2DataId seqId = MsaSQLiteSpecificTestData::createTestSequence(false, seqLength, os);
    CHECK_NO_ERROR(os);

    QList<U2MsaGap> gaps;
    qint64 gapLength = 2;
    gaps << U2MsaGap(1, gapLength);

    U2MsaRow row;
    row.sequenceId = seqId;
    row.gstart = 1;
    row.gend = 20;
    row.gaps = gaps;
    qint64 rowLength = row.gend - row.gstart + gapLength;
    row.length = rowLength;

    qint64 posInMsa = baseNumOfRows;

    // Prepare expected values
    qint64 expectedMsaLength = qMax(baseMsa.length, rowLength);

    // Call test function
    sqliteDbi->getMsaDbi()->addRow(msaId, posInMsa, row, os);
    CHECK_NO_ERROR(os);

    // Undo adding the row
    sqliteDbi->getSQLiteObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Redo row addition
    sqliteDbi->getSQLiteObjectDbi()->redo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify the row
    U2MsaRow rowAfterRedo = sqliteDbi->getMsaDbi()->getRow(msaId, row.rowId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(QString(row.sequenceId), QString(rowAfterRedo.sequenceId), "sequence id");
    CHECK_EQUAL(row.gstart, rowAfterRedo.gstart, "gstart");
    CHECK_EQUAL(row.gend, rowAfterRedo.gend, "gend");
    CHECK_TRUE(row.gaps == rowAfterRedo.gaps, "gaps");
    CHECK_EQUAL(rowLength, rowAfterRedo.length, "row length");

    // Verify MSA length
    U2Msa msaAfterRedo = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(expectedMsaLength, msaAfterRedo.length, "msa length");

    // Verify number of rows
    qint64 numOfRowsAfterRedo = sqliteDbi->getMsaDbi()->getNumOfRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseNumOfRows + 1, numOfRowsAfterRedo, "number of rows");

    // Verify rows order
    QList<qint64> rowsOrderAfterRedo = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseRowsOrder.count() + 1, rowsOrderAfterRedo.count(), "rows order items number");
    CHECK_EQUAL(row.rowId, rowsOrderAfterRedo[posInMsa], "new row id");
    rowsOrderAfterRedo.removeAt(posInMsa);
    CHECK_TRUE(baseRowsOrder == rowsOrderAfterRedo, "other row ids");

    // Verify the version has been incremented
    int versionAfterRedo = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseVersion + 1, versionAfterRedo, "version");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, addRow_child_noModTrack) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createNotSoSmallTestMsa(false, os);
    CHECK_NO_ERROR(os);

    // Prepare test values
    qint64 seqLength = 100;
    U2DataId seqId = MsaSQLiteSpecificTestData::createTestSequence(false, seqLength, os);
    CHECK_NO_ERROR(os);
    U2TrackModType seqTrackModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(NoTrack, seqTrackModType, "sequence base track mod type");

    QList<U2MsaGap> gaps;
    qint64 gapLength = 2;
    gaps << U2MsaGap(1, gapLength);

    U2MsaRow row;
    row.sequenceId = seqId;
    row.gstart = 1;
    row.gend = 20;
    row.gaps = gaps;
    qint64 posInMsa = 5;
    row.length = row.gend - row.gstart + gapLength;

    // Prepare expected values
    U2TrackModType expectedChildModTrackType = seqTrackModType;
    CHECK_NO_ERROR(os);

    // Call test function
    sqliteDbi->getMsaDbi()->addRow(msaId, posInMsa, row, os);
    CHECK_NO_ERROR(os);

    U2TrackModType finalChildModTrackType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(expectedChildModTrackType, finalChildModTrackType, "sequence track mod type");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, addRow_child_Track) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createNotSoSmallTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Prepare test values
    qint64 seqLength = 100;
    U2DataId seqId = MsaSQLiteSpecificTestData::createTestSequence(false, seqLength, os);
    CHECK_NO_ERROR(os);
    U2TrackModType seqTrackModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(NoTrack, seqTrackModType, "sequence base track mod type");

    QList<U2MsaGap> gaps;
    qint64 gapLength = 2;
    gaps << U2MsaGap(1, gapLength);

    U2MsaRow row;
    row.sequenceId = seqId;
    row.gstart = 1;
    row.gend = 20;
    row.gaps = gaps;
    qint64 posInMsa = 5;
    row.length = row.gend - row.gstart + gapLength;

    // Prepare expected values
    U2TrackModType expectedChildModTrackType = sqliteDbi->getObjectDbi()->getTrackModType(msaId, os);
    CHECK_NO_ERROR(os);

    // Call test function
    sqliteDbi->getMsaDbi()->addRow(msaId, posInMsa, row, os);
    CHECK_NO_ERROR(os);

    U2TrackModType finalChildModTrackType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(expectedChildModTrackType, finalChildModTrackType, "sequence track mod type");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, addRow_several_Steps) {
    //6 changes, 5 undo steps, 1 redo steps, 2 undo step, 5 redo steps
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Prepare some base values
    QList<U2MsaRow> baseRows = sqliteDbi->getMsaDbi()->getRows(msaId, os);
    CHECK_NO_ERROR(os);
    int baseMsaVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Prepare value list
    QList<qint64> posInMsa;
    QList<QByteArray> newSequences;
    QList<QList<U2MsaRow> > rows;
    rows << baseRows;

    for (int i = 0; i < 6; ++i) {
        posInMsa << i + 2;
        rows << rows[i];
        rows[i + 1].insert(posInMsa[i], MsaSQLiteSpecificTestData::createRow((i + 1) * 10, os));
        CHECK_NO_ERROR(os);
        newSequences << sqliteDbi->getSequenceDbi()->getSequenceData(rows[i + 1][posInMsa[i]].sequenceId, U2_REGION_MAX, os);
        CHECK_NO_ERROR(os);
    }

    // Steps count
    int valuesCount = rows.count();     // changes = valuesCount - 1;
    QList<int> steps;                   // negative - undo steps, positive - redo steps;
    steps << -5 << 1 << -2 << 5;
    int expectedIndex = valuesCount - 1;
    for (int i = 0; i < steps.length(); ++i) {
        expectedIndex += steps[i];
    }

    // Call test function
    for (int i = 1; i < valuesCount; ++i) {
        sqliteDbi->getMsaDbi()->addRow(msaId, posInMsa[i - 1], rows[i][posInMsa[i - 1]], os);
        CHECK_NO_ERROR(os);
    }

    // Undo and redo row order updating
    int totalUndo = 0;
    int totalRedo = 0;
    for (int i  = 0; i < steps.length(); ++i) {
        if (steps[i] < 0) {
            for (int j = 0; j < -steps[i]; ++j) {
                sqliteDbi->getSQLiteObjectDbi()->undo(msaId, os);
                CHECK_NO_ERROR(os);
                totalUndo++;
            }
        } else {
            for (int j = 0; j < steps[i]; ++j) {
                sqliteDbi->getSQLiteObjectDbi()->redo(msaId, os);
                CHECK_NO_ERROR(os);
                totalRedo++;
            }
        }
    }

    // Verify version
    int finalVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseMsaVersion + expectedIndex, finalVersion, "version");

    // Verify rows
    QList<U2MsaRow> finalRows = sqliteDbi->getMsaDbi()->getRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(rows[expectedIndex].count(), finalRows.count(), "rows count");
    for (int i = 0; i < finalRows.count(); ++i) {
        CHECK_EQUAL(rows[expectedIndex][i].gstart, finalRows[i].gstart, QString("%1 row gstart").arg(i));
        CHECK_EQUAL(rows[expectedIndex][i].gend, finalRows[i].gend, QString("%1 row gend").arg(i));
        CHECK_EQUAL(rows[expectedIndex][i].length, finalRows[i].length, QString("%1 row length").arg(i));
        CHECK_EQUAL(QString(rows[expectedIndex][i].sequenceId), QString(finalRows[i].sequenceId), QString("%1 row sequenceId").arg(i));
        CHECK_TRUE(rows[expectedIndex][i].gaps == finalRows[i].gaps, QString("%1 row gaps").arg(i));
    }

    // Verify added sequences data
    for (int i = 0; i < expectedIndex; ++i) {
        QByteArray finalSequence = sqliteDbi->getSequenceDbi()->getSequenceData(finalRows[posInMsa[i]].sequenceId, U2_REGION_MAX, os);
        CHECK_NO_ERROR(os);
        CHECK_EQUAL(QString(newSequences[i]), QString(finalSequence), QString("sequence data of %1 step").arg(i));
    }
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, removeRow_noModTrack) {
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(false, os);
    CHECK_NO_ERROR(os);
    QList<qint64> rows = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    qint64 rowId = rows.first();
    CHECK_NO_ERROR(os);

    // Get current version
    int oldVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Remove row
    sqliteDbi->getMsaDbi()->removeRow(msaId, rowId, os);
    CHECK_NO_ERROR(os);

    // Verify msa length
    U2Msa newMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(13, newMsa.length, "msa length");

    // Verify number of rows
    qint64 numOfRows = sqliteDbi->getMsaDbi()->getNumOfRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(1, numOfRows, "number of rows");

    // Verify version
    int newVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldVersion + 1, newVersion, "version");

    // Verify no modification steps
    qint64 modStepsNum = MsaSQLiteSpecificTestData::getModStepsNum(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(0, modStepsNum, "mod steps num");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, removeRow_undo) {
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    QList<qint64> rows = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    qint64 rowId = rows.last();
    U2MsaRow oldRow = sqliteDbi->getMsaDbi()->getRow(msaId, rowId, os);
    CHECK_NO_ERROR(os);

    // Get current version
    int oldVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Remove row
    sqliteDbi->getMsaDbi()->removeRow(msaId, rowId, os);
    CHECK_NO_ERROR(os);

    // Verify msa length
    U2Msa newMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(13, newMsa.length, "msa length");

    // Verify number of rows
    qint64 numOfRows = sqliteDbi->getMsaDbi()->getNumOfRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(1, numOfRows, "number of rows");

    // Verify version
    int newVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldVersion + 1, newVersion, "version");

    // Verify the modification step
    QString expectedModDetails = "0\t1\t" +
        QByteArray::number(oldRow.rowId) + "\t" +
        oldRow.sequenceId.toHex() + "\t0\t10\t\"5,2\"";
    U2SingleModStep modStep = sqliteDbi->getModDbi()->getModStep(msaId, oldVersion, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(QString(msaId), QString(modStep.objectId), "object id");
    CHECK_EQUAL(oldVersion, modStep.version, "version in mod step");
    CHECK_EQUAL(U2ModType::msaRemovedRow, modStep.modType, "mod step type");
    CHECK_EQUAL(expectedModDetails, QString(modStep.details), "mod step details");

    // Undo
    sqliteDbi->getObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify undo
    U2Msa undoMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(13, undoMsa.length, "msa length after undo");
    CHECK_EQUAL(oldVersion, undoMsa.version, "version after undo");

    qint64 undoNumOfRows = sqliteDbi->getMsaDbi()->getNumOfRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(2, undoNumOfRows, "number of rows after undo");
    QList<qint64> undoRowsOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(rows == undoRowsOrder, "rows ids after undo");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, removeRow_redo) {
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    QList<qint64> rows = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    qint64 rowId = rows.first();
    U2MsaRow oldRow = sqliteDbi->getMsaDbi()->getRow(msaId, rowId, os);
    CHECK_NO_ERROR(os);

    // Get current version
    int oldVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Remove row
    sqliteDbi->getMsaDbi()->removeRow(msaId, rowId, os);
    CHECK_NO_ERROR(os);

    // Undo
    sqliteDbi->getObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Redo
    sqliteDbi->getObjectDbi()->redo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify msa length
    U2Msa redoMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(13, redoMsa.length, "msa length");

    // Verify number of rows
    qint64 redoNumOfRows = sqliteDbi->getMsaDbi()->getNumOfRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(1, redoNumOfRows, "number of rows");

    // Verify version
    int redoVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldVersion + 1, redoVersion, "version");

    // Verify the modification step
    QString expectedModDetails = "0\t0\t" +
        QByteArray::number(oldRow.rowId) + "\t" +
        oldRow.sequenceId.toHex() + "\t0\t11\t\"1,1;7,1\"";
    U2SingleModStep modStep = sqliteDbi->getModDbi()->getModStep(msaId, oldVersion, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(QString(msaId), QString(modStep.objectId), "object id");
    CHECK_EQUAL(oldVersion, modStep.version, "version in mod step");
    CHECK_EQUAL(U2ModType::msaRemovedRow, modStep.modType, "mod step type");
    CHECK_EQUAL(expectedModDetails, QString(modStep.details), "mod step details");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, removeRows_noModTrack) {
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(false, os);
    CHECK_NO_ERROR(os);
    QList<qint64> rows = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);

    // Get current version
    int oldVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Remove rows
    QList<qint64> rowsIds;
    rowsIds << rows.first();
    sqliteDbi->getMsaDbi()->removeRows(msaId, rowsIds, os);
    CHECK_NO_ERROR(os);

    // Verify msa length
    U2Msa newMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(13, newMsa.length, "msa length");

    // Verify number of rows
    qint64 numOfRows = sqliteDbi->getMsaDbi()->getNumOfRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(1, numOfRows, "number of rows");

    // Verify version
    int newVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldVersion + 1, newVersion, "version");

    // Verify no modification steps
    qint64 modStepsNum = MsaSQLiteSpecificTestData::getModStepsNum(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(0, modStepsNum, "mod steps num");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, removeRows_undo) {
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    QList<U2MsaRow> rows = sqliteDbi->getMsaDbi()->getRows(msaId, os);
    CHECK_NO_ERROR(os);
    QList<qint64> rowsIds = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);

    // Get current version
    int oldVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Remove rows
    sqliteDbi->getMsaDbi()->removeRows(msaId, rowsIds, os);
    CHECK_NO_ERROR(os);

    // Verify msa length
    U2Msa newMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(0, newMsa.length, "msa length");

    // Verify number of rows
    qint64 numOfRows = sqliteDbi->getMsaDbi()->getNumOfRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(0, numOfRows, "number of rows");

    // Verify version
    int newVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldVersion + 1, newVersion, "version");

    // Verify the modification step
    QByteArray sep(1, char(11));
    QString expectedModDetails = "0" + sep +
        "0\t0\t" + QByteArray::number(rowsIds[0]) + "\t" +rows[0].sequenceId.toHex() + "\t0\t11\t\"1,1;7,1\"" +
        sep +
        "0\t1\t" + QByteArray::number(rowsIds[1]) + "\t" +rows[1].sequenceId.toHex() + "\t0\t10\t\"5,2\"";
    QList< QList<U2SingleModStep> > modSteps = sqliteDbi->getSQLiteModDbi()->getModSteps(msaId, oldVersion, os);
    CHECK_EQUAL(1, modSteps.count(), "mod steps count");
    CHECK_EQUAL(2, modSteps.first().count(), "mod single steps");
    U2SingleModStep modStep = modSteps.first().last();
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(QString(msaId), QString(modStep.objectId), "object id");
    CHECK_EQUAL(oldVersion, modStep.version, "version in mod step");
    CHECK_EQUAL(U2ModType::msaRemovedRows, modStep.modType, "mod step type");
    CHECK_EQUAL(expectedModDetails, QString(modStep.details), "mod step details");

    modStep = modSteps.first().first();
    CHECK_EQUAL(QString(msaId), QString(modStep.objectId), "object id");
    CHECK_EQUAL(oldVersion, modStep.version, "version in mod step");
    CHECK_EQUAL(U2ModType::msaLengthChanged, modStep.modType, "mod step type");

    // Undo
    sqliteDbi->getObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify undo
    U2Msa undoMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(13, undoMsa.length, "msa length after undo");
    CHECK_EQUAL(oldVersion, undoMsa.version, "version after undo");

    qint64 undoNumOfRows = sqliteDbi->getMsaDbi()->getNumOfRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(2, undoNumOfRows, "number of rows after undo");
    QList<qint64> undoRowsOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(rowsIds == undoRowsOrder, "rows ids after undo");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, removeRows_redo) {
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    QList<U2MsaRow> rows = sqliteDbi->getMsaDbi()->getRows(msaId, os);
    CHECK_NO_ERROR(os);
    QList<qint64> rowsIds = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);

    // Get current version
    int oldVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Remove rows
    sqliteDbi->getMsaDbi()->removeRows(msaId, QList<qint64>() << rowsIds.last(), os);
    CHECK_NO_ERROR(os);

    // Undo
    sqliteDbi->getObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Redo
    sqliteDbi->getObjectDbi()->redo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify msa length
    U2Msa redoMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(13, redoMsa.length, "msa length");

    // Verify number of rows
    qint64 redoNumOfRows = sqliteDbi->getMsaDbi()->getNumOfRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(1, redoNumOfRows, "number of rows");

    // Verify version
    int redoVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldVersion + 1, redoVersion, "version");

    // Verify the modification step
    QByteArray sep(1, char(11));
    QString expectedModDetails = "0" + sep +
        "0\t1\t" + QByteArray::number(rowsIds[1]) + "\t" +rows[1].sequenceId.toHex() + "\t0\t10\t\"5,2\"";
    U2SingleModStep modStep = sqliteDbi->getModDbi()->getModStep(msaId, oldVersion, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(QString(msaId), QString(modStep.objectId), "object id");
    CHECK_EQUAL(oldVersion, modStep.version, "version in mod step");
    CHECK_EQUAL(U2ModType::msaRemovedRows, modStep.modType, "mod step type");
    CHECK_EQUAL(expectedModDetails, QString(modStep.details), "mod step details");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, addRows_noModTrack) {
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(false, os);
    CHECK_NO_ERROR(os);

    // Get current version
    int oldVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Add rows
    U2MsaRow row = MsaSQLiteSpecificTestData::createRow(100, os);
    CHECK_NO_ERROR(os);
    QList<U2MsaRow> rows; rows << row;

    sqliteDbi->getMsaDbi()->addRows(msaId, rows, os);
    CHECK_NO_ERROR(os);

    // Verify row
    U2MsaRow newRow = sqliteDbi->getMsaDbi()->getRow(msaId, rows[0].rowId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(QString(row.sequenceId), QString(newRow.sequenceId), "sequence id");
    CHECK_EQUAL(1, newRow.gstart, "gstart");
    CHECK_EQUAL(20, newRow.gend, "gend");
    CHECK_TRUE(row.gaps == newRow.gaps, "gaps");
    CHECK_EQUAL(row.length, newRow.length, "row length");

    // Verify msa length
    U2Msa msaObj = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(row.length, msaObj.length, "msa length");

    // Verify number of rows
    qint64 numOfRows = sqliteDbi->getMsaDbi()->getNumOfRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(3, numOfRows, "number of rows");

    // Verify rows order
    QList<qint64> rowsOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(3, rowsOrder.count(), "rows order items number");
    CHECK_EQUAL(rows[0].rowId, rowsOrder.last(), "last row id");

    // Verify version
    int newVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldVersion + 1, newVersion, "version");

    // Verify no modification steps
    qint64 modStepsNum = MsaSQLiteSpecificTestData::getModStepsNum(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(0, modStepsNum, "mod steps num");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, addRows_undo) {
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    QList<qint64> order = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);

    // Get current version
    int oldVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Add rows
    U2MsaRow row1 = MsaSQLiteSpecificTestData::createRow(100, os);
    CHECK_NO_ERROR(os);
    U2MsaRow row2 = MsaSQLiteSpecificTestData::createRow(200, os);
    CHECK_NO_ERROR(os);
    QList<U2MsaRow> rows; rows << row1 << row2;

    sqliteDbi->getMsaDbi()->addRows(msaId, rows, os);
    CHECK_NO_ERROR(os);
    order << rows[0].rowId << rows[1].rowId;

    { // Verify rows
        U2MsaRow newRow1 = sqliteDbi->getMsaDbi()->getRow(msaId, rows[0].rowId, os);
        CHECK_NO_ERROR(os);
        CHECK_EQUAL(QString(row1.sequenceId), QString(newRow1.sequenceId), "sequence id 1");
        CHECK_EQUAL(1, newRow1.gstart, "gstart 1");
        CHECK_EQUAL(20, newRow1.gend, "gend 1");
        CHECK_TRUE(row1.gaps == newRow1.gaps, "gaps 1");
        CHECK_EQUAL(row1.length, newRow1.length, "row length 1");

        U2MsaRow newRow2 = sqliteDbi->getMsaDbi()->getRow(msaId, rows[1].rowId, os);
        CHECK_NO_ERROR(os);
        CHECK_EQUAL(QString(row2.sequenceId), QString(newRow2.sequenceId), "sequence id 2");
        CHECK_EQUAL(1, newRow2.gstart, "gstart 2");
        CHECK_EQUAL(20, newRow2.gend, "gend 2");
        CHECK_TRUE(row2.gaps == newRow2.gaps, "gaps 2");
        CHECK_EQUAL(row2.length, newRow2.length, "row length 2");
    }

    // Verify msa length
    U2Msa msaObj = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(row1.length, msaObj.length, "msa length");

    // Verify number of rows
    qint64 numOfRows = sqliteDbi->getMsaDbi()->getNumOfRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(4, numOfRows, "number of rows");

    // Verify rows order
    QList<qint64> rowsOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(order == rowsOrder, "row order");

    // Verify version
    int newVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldVersion + 1, newVersion, "version");

    // Verify no modification steps
    QByteArray sep(1, char(11));
    QString expectedModDetails = "0" + sep +
        "0\t2\t" + QByteArray::number(rows[0].rowId) + "\t" +rows[0].sequenceId.toHex() + "\t1\t20\t\"1,2\"" +
        sep +
        "0\t3\t" + QByteArray::number(rows[1].rowId) + "\t" +rows[1].sequenceId.toHex() + "\t1\t20\t\"1,2\"";
    QList< QList<U2SingleModStep> > modSteps = sqliteDbi->getSQLiteModDbi()->getModSteps(msaId, oldVersion, os);
    CHECK_EQUAL(1, modSteps.count(), "mod steps count");
    CHECK_EQUAL(2, modSteps.first().count(), "mod single step count");
    U2SingleModStep modStep = modSteps.first().last();
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(QString(msaId), QString(modStep.objectId), "object id");
    CHECK_EQUAL(oldVersion, modStep.version, "version in mod step");
    CHECK_EQUAL(U2ModType::msaAddedRows, modStep.modType, "mod step type");
    CHECK_EQUAL(expectedModDetails, QString(modStep.details), "mod step details");

    modStep = modSteps.first().first();
    CHECK_EQUAL(QString(msaId), QString(modStep.objectId), "object id");
    CHECK_EQUAL(oldVersion, modStep.version, "version in mod step");
    CHECK_EQUAL(U2ModType::msaLengthChanged, modStep.modType, "mod step type");

    // Undo
    sqliteDbi->getObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Verify undo
    U2Msa undoMsa = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(13, undoMsa.length, "msa length after undo");
    CHECK_EQUAL(oldVersion, undoMsa.version, "version after undo");

    qint64 undoNumOfRows = sqliteDbi->getMsaDbi()->getNumOfRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(2, undoNumOfRows, "number of rows after undo");
    QList<qint64> undoRowsOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(order.mid(0, 2) == undoRowsOrder, "rows ids after undo");
}

IMPLEMENT_TEST(MsaDbiSQLiteSpecificUnitTests, addRows_redo) {
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = MsaSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = MsaSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    QList<qint64> order = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);

    // Get current version
    int oldVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Add rows
    U2MsaRow row1 = MsaSQLiteSpecificTestData::createRow(100, os);
    CHECK_NO_ERROR(os);
    U2MsaRow row2 = MsaSQLiteSpecificTestData::createRow(200, os);
    CHECK_NO_ERROR(os);
    QList<U2MsaRow> rows; rows << row1 << row2;

    sqliteDbi->getMsaDbi()->addRows(msaId, rows, os);
    CHECK_NO_ERROR(os);
    order << rows[0].rowId << rows[1].rowId;

    // Undo
    sqliteDbi->getObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Redo
    sqliteDbi->getObjectDbi()->redo(msaId, os);
    CHECK_NO_ERROR(os);

    { // Verify rows
        U2MsaRow newRow1 = sqliteDbi->getMsaDbi()->getRow(msaId, rows[0].rowId, os);
        CHECK_NO_ERROR(os);
        CHECK_EQUAL(QString(row1.sequenceId), QString(newRow1.sequenceId), "sequence id 1");
        CHECK_EQUAL(1, newRow1.gstart, "gstart 1");
        CHECK_EQUAL(20, newRow1.gend, "gend 1");
        CHECK_TRUE(row1.gaps == newRow1.gaps, "gaps 1");
        CHECK_EQUAL(row1.length, newRow1.length, "row length 1");

        U2MsaRow newRow2 = sqliteDbi->getMsaDbi()->getRow(msaId, rows[1].rowId, os);
        CHECK_NO_ERROR(os);
        CHECK_EQUAL(QString(row2.sequenceId), QString(newRow2.sequenceId), "sequence id 2");
        CHECK_EQUAL(1, newRow2.gstart, "gstart 2");
        CHECK_EQUAL(20, newRow2.gend, "gend 2");
        CHECK_TRUE(row2.gaps == newRow2.gaps, "gaps 2");
        CHECK_EQUAL(row2.length, newRow2.length, "row length 2");
    }

    // Verify msa length
    U2Msa msaObj = sqliteDbi->getMsaDbi()->getMsaObject(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(row1.length, msaObj.length, "msa length");

    // Verify number of rows
    qint64 numOfRows = sqliteDbi->getMsaDbi()->getNumOfRows(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(4, numOfRows, "number of rows");

    // Verify rows order
    QList<qint64> rowsOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(order == rowsOrder, "row order");

    // Verify version
    int newVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(oldVersion + 1, newVersion, "version");

    // Verify no modification steps
    QByteArray sep(1, char(11));
    QString expectedModDetails = "0" + sep +
        "0\t2\t" + QByteArray::number(rows[0].rowId) + "\t" +rows[0].sequenceId.toHex() + "\t1\t20\t\"1,2\"" +
        sep +
        "0\t3\t" + QByteArray::number(rows[1].rowId) + "\t" +rows[1].sequenceId.toHex() + "\t1\t20\t\"1,2\"";
    QList< QList<U2SingleModStep> > modSteps = sqliteDbi->getSQLiteModDbi()->getModSteps(msaId, oldVersion, os);
    CHECK_EQUAL(1, modSteps.count(), "mod steps count");
    CHECK_EQUAL(2, modSteps.first().count(), "single mod steps count");
    U2SingleModStep modStep = modSteps.first().last();
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(QString(msaId), QString(modStep.objectId), "object id");
    CHECK_EQUAL(oldVersion, modStep.version, "version in mod step");
    CHECK_EQUAL(U2ModType::msaAddedRows, modStep.modType, "mod step type");
    CHECK_EQUAL(expectedModDetails, QString(modStep.details), "mod step details");

    modStep = modSteps.first().first();
    CHECK_EQUAL(QString(msaId), QString(modStep.objectId), "object id");
    CHECK_EQUAL(oldVersion, modStep.version, "version in mod step");
    CHECK_EQUAL(U2ModType::msaLengthChanged, modStep.modType, "mod step type");
}

} // namespace
