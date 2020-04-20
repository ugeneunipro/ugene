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

#include "ModDbiSQLiteSpecificUnitTests.h"
#include "core/util/MsaDbiUtilsUnitTests.h"

#include <U2Core/DNAAlphabet.h>
#include <U2Core/U2AbstractDbi.h>
#include <U2Core/U2DbiPackUtils.h>
#include <U2Core/U2MsaDbi.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Formats/SQLiteDbi.h>
#include <U2Formats/SQLiteModDbi.h>
#include <U2Formats/SQLiteObjectDbi.h>


#define IMPLEMENT_MOD_TEST(suite, name) \
    static const int _##suite##_##name##_type ATTR_UNUSED = qRegisterMetaType<U2::TEST_CLASS(suite, name)>(TEST_CLASS_STR(suite, name)); \
    void TEST_CLASS(suite, name)::SetUp() { \
        ModSQLiteSpecificTestData::cleanUpAllModSteps(); \
    } \
    void TEST_CLASS(suite, name)::Test()

namespace U2 {

TestDbiProvider ModSQLiteSpecificTestData::dbiProvider = TestDbiProvider();
const QString& ModSQLiteSpecificTestData::SQLITE_MSA_DB_URL("sqlite-mod-dbi.ugenedb");
SQLiteDbi* ModSQLiteSpecificTestData::sqliteDbi = NULL;

const QString ModSQLiteSpecificTestData::TEST_MSA_NAME = "Test alignment";

void ModSQLiteSpecificTestData::init() {
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

void ModSQLiteSpecificTestData::shutdown() {
    if (NULL != sqliteDbi) {
        U2OpStatusImpl os;
        sqliteDbi->shutdown(os);
        SAFE_POINT_OP(os, );
        delete sqliteDbi;
        sqliteDbi = NULL;
    }
}

void ModSQLiteSpecificTestData::cleanUpAllModSteps() {
    if (NULL != sqliteDbi) {
        U2OpStatusImpl os;
        SQLiteWriteQuery qSingle("DELETE FROM SingleModStep", sqliteDbi->getDbRef(), os);
        SQLiteWriteQuery qMulti("DELETE FROM MultiModStep", sqliteDbi->getDbRef(), os);
        SQLiteWriteQuery qUser("DELETE FROM UserModStep", sqliteDbi->getDbRef(), os);

        qSingle.execute();
        qMulti.execute();
        qUser.execute();
    }
}

SQLiteDbi* ModSQLiteSpecificTestData::getSQLiteDbi() {
    if (NULL == sqliteDbi) {
        init();
    }
    return sqliteDbi;
}

void ModSQLiteSpecificTestData::getAllSteps(QList<U2SingleModStep>& singleSteps, QList<U2MultiModStep4Test>& multiSteps, QList<U2UserModStep4Test>& userSteps, U2OpStatus& os) {
    singleSteps.clear();
    multiSteps.clear();
    userSteps.clear();

    SQLiteReadQuery qSingle("SELECT id, object, otype, oextra, version, modType, details, multiStepId FROM SingleModStep", sqliteDbi->getDbRef(), os);
    SAFE_POINT_OP(os, );
    while (qSingle.step()) {
        U2SingleModStep singleStep;
        singleStep.id = qSingle.getInt64(0);
        singleStep.objectId = qSingle.getDataIdExt(1);
        singleStep.version = qSingle.getInt64(4);
        singleStep.modType = qSingle.getInt64(5);
        singleStep.details = qSingle.getBlob(6);
        singleStep.multiStepId = qSingle.getInt64(7);
        singleSteps.append(singleStep);
    }

    SQLiteReadQuery qMulti("SELECT id, userStepId FROM MultiModStep", sqliteDbi->getDbRef(), os);
    SAFE_POINT_OP(os, );
    while (qMulti.step()) {
        U2MultiModStep4Test multiStep;
        multiStep.id = qMulti.getInt64(0);
        multiStep.userStepId = qMulti.getInt64(1);
        multiSteps.append(multiStep);
    }

    SQLiteReadQuery qUser("SELECT id, object, otype, oextra, version FROM UserModStep", sqliteDbi->getDbRef(), os);
    SAFE_POINT_OP(os, );
    while (qUser.step()) {
        U2UserModStep4Test userStep;
        userStep.id = qUser.getInt64(0);
        userStep.masterObjId = qUser.getDataIdExt(1);
        userStep.version = qUser.getInt64(4);
        userSteps.append(userStep);
    }
}

U2SingleModStep ModSQLiteSpecificTestData::prepareSingleStep(qint64 modVersion, U2OpStatus& os) {
    U2SingleModStep step;

    // Create an object
    U2DataId objId = createObject(os);
    SAFE_POINT_OP(os, step);

    // Fill in the step
    step.objectId = objId;
    step.version = modVersion;
    step.modType = U2ModType::objUpdatedName;
    step.details = U2DbiPackUtils::packObjectNameDetails("Test object", "Test object");

    return step;
}

U2DataId ModSQLiteSpecificTestData::createObject(U2OpStatus& os) {
    // Create an object
    U2Sequence obj; // creates a sequence object to make type U2Type::Sequence
    obj.dbiId = sqliteDbi->getDbiId();
    obj.visualName = "Test object";

    sqliteDbi->getSQLiteObjectDbi()->createObject(obj, "", U2DbiObjectRank_TopLevel, os);
    SAFE_POINT_OP(os, U2DataId());

    return obj.id;
}

qint64 ModSQLiteSpecificTestData::getModStepsNum(const U2DataId& objId, U2OpStatus& os) {
    SQLiteReadQuery qModSteps("SELECT COUNT(*) FROM SingleModStep WHERE object = ?1", sqliteDbi->getDbRef(), os);
    qModSteps.bindDataId(1, objId);
    return qModSteps.selectInt64();
}

U2SingleModStep ModSQLiteSpecificTestData::getLastModStep(const U2DataId& objId, U2OpStatus& os) {
    U2SingleModStep res;
    SQLiteReadQuery qModStep("SELECT id, object, otype, oextra, version, modType, details FROM SingleModStep WHERE object = ?1 ORDER BY version DESC LIMIT 1", sqliteDbi->getDbRef(), os);
    CHECK_OP(os, res);

    qModStep.bindDataId(1, objId);
    qModStep.update(1);

    res.id = qModStep.getInt32(0);
    res.objectId = qModStep.getDataIdExt(1);
    res.version = qModStep.getInt64(4);
    res.modType = qModStep.getInt64(5);
    res.details = qModStep.getBlob(6);
    return res;
}

QList<U2SingleModStep> ModSQLiteSpecificTestData::getAllModSteps(const U2DataId& objId, U2OpStatus& os) {
    QList<U2SingleModStep> res;
    SQLiteReadQuery qModStep("SELECT id, object, otype, oextra, version, modType, details"
        " FROM SingleModStep WHERE object = ?1 ORDER BY version", sqliteDbi->getDbRef(), os);
    CHECK_OP(os, res);

    qModStep.bindDataId(1, objId);
    while(qModStep.step()) {
        U2SingleModStep modStep;
        modStep.id = qModStep.getInt32(0);
        modStep.objectId = qModStep.getDataIdExt(1);
        modStep.version = qModStep.getInt64(4);
        modStep.modType = qModStep.getInt64(5);
        modStep.details = qModStep.getBlob(6);
        res << modStep;
    }
    return res;
}

U2MsaRow ModSQLiteSpecificTestData::addRow(const U2DataId &msaId, const QByteArray &name, const QByteArray &seq, const QList<U2MsaGap> &gaps, U2OpStatus &os) {
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

U2DataId ModSQLiteSpecificTestData::createTestMsa(bool enableModTracking, U2OpStatus& os) {
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


IMPLEMENT_TEST(ModDbiSQLiteSpecificUnitTests, updateMsaName_noModTrack) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = ModSQLiteSpecificTestData::createTestMsa(false, os);
    CHECK_NO_ERROR(os);

    // Rename the msa
    QString newName = "Renamed alignment";
    sqliteDbi->getMsaDbi()->updateMsaName(msaId, newName, os);
    CHECK_NO_ERROR(os);

    // Verify that there are no modification steps
    qint64 actualModStepsNum = ModSQLiteSpecificTestData::getModStepsNum(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(0, actualModStepsNum, "mod steps num");
}

IMPLEMENT_TEST(ModDbiSQLiteSpecificUnitTests, updateMsaName_severalSteps) {
    //5 changes, 4 undo steps, 2 redo steps, 1 undo step, 1 redo step
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get some base data
    qint64 baseObjVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    QList<U2SingleModStep> baseModStepList = ModSQLiteSpecificTestData::getAllModSteps(msaId, os);
    CHECK_NO_ERROR(os);

    // Prepare value list
    QStringList names(sqliteDbi->getMsaDbi()->getMsaObject(msaId, os).visualName);
    CHECK_NO_ERROR(os);
    for (int i = 1; i < 6; ++i) {
        names << "Renamed alignment" + QString::number(i);
    }

    // Steps count
    int valuesCount = names.length();    // changes = valuesCount - 1;
    QList<int> steps;                       // negative - undo steps, positive - redo steps;
    steps << -4 << 2 << -1 << 1;
    int expectedIndex = valuesCount - 1;
    for (int i = 0; i < steps.length(); ++i) {
        expectedIndex += steps[i];
    }

    // Prepare modStep list
    QList<U2SingleModStep> modSteps;
    for (int i = 0; i < valuesCount - 1; ++i) {
        U2SingleModStep modStep;
        modStep.modType = U2ModType::objUpdatedName;
        modStep.objectId = msaId;
        modStep.version = baseObjVersion + i;
        //PackUtils::VERSION hardcoded, correct after test failure.
        modStep.details = "0\t" + names[i].toLatin1() + "\t" + names[i + 1].toLatin1();
        modSteps << modStep;
    }
    QList<U2SingleModStep> expectedModStepList = baseModStepList + modSteps;

    // Rename the msa (changesCount times)
    for (int i = 1; i < names.length(); ++i) {
        sqliteDbi->getMsaDbi()->updateMsaName(msaId, names[i], os);
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

    // Check modSteps
    QList<U2SingleModStep> finalModStepList = ModSQLiteSpecificTestData::getAllModSteps(msaId, os);
    CHECK_EQUAL(expectedModStepList.length(), finalModStepList.length(), "mod steps table size");
    for (int i = 0; i < expectedModStepList.length(); ++i) {
        CHECK_EQUAL(expectedModStepList[i].modType, finalModStepList[i].modType, "mod type");
        CHECK_EQUAL(QString(expectedModStepList[i].objectId), QString(finalModStepList[i].objectId), "object id");
        CHECK_EQUAL(expectedModStepList[i].version, finalModStepList[i].version, "version");
        CHECK_EQUAL(QString(expectedModStepList[i].details), QString(finalModStepList[i].details), "details");
    }
}

IMPLEMENT_TEST(ModDbiSQLiteSpecificUnitTests, updateMsaName_severalUndoThenAction) {
    //5 changes, 4 undo steps, 1 action
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get some base data
    qint64 baseObjVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    QList<U2SingleModStep> baseModStepList = ModSQLiteSpecificTestData::getAllModSteps(msaId, os);
    CHECK_NO_ERROR(os);

    // Prepare value list
    QStringList names(sqliteDbi->getMsaDbi()->getMsaObject(msaId, os).visualName);
    CHECK_NO_ERROR(os);
    for (int i = 1; i < 6; ++i) {
        names << "Renamed alignment" + QString::number(i);
    }
    QString newName("Action occurred");

    // Steps count
    int valuesCount = names.length();       // changes = valuesCount - 1;
    QList<int> steps;                       // negative - undo steps, positive - redo steps;
    steps << -4;
    int expectedIndex = valuesCount - 1;
    for (int i = 0; i < steps.length(); ++i) {
        expectedIndex += steps[i];
    }

    // Prepare modStep list
    QList<U2SingleModStep> modSteps;
    for (int i = 0; i < valuesCount - 1; ++i) {
        U2SingleModStep modStep;
        modStep.modType = U2ModType::objUpdatedName;
        modStep.objectId = msaId;
        modStep.version = baseObjVersion + i;
        //PackUtils::VERSION hardcoded, correct after test failure.
        modStep.details = "0\t" + names[i].toLatin1() + "\t" + names[i + 1].toLatin1();
        modSteps << modStep;
    }
    QList<U2SingleModStep> expectedModStepList = baseModStepList;
    for (int i = 0; i < expectedIndex; ++i) {
        expectedModStepList << modSteps[i];
    }
    U2SingleModStep actionModStep;
    actionModStep.modType = U2ModType::objUpdatedName;
    actionModStep.objectId = msaId;
    actionModStep.version = baseObjVersion + expectedIndex;
    //PackUtils::VERSION hardcoded, correct after test failure.
    actionModStep.details = "0\t" + names[expectedIndex].toLatin1() + "\t" + newName.toLatin1();
    expectedModStepList << actionModStep;

    // Rename the msa (changesCount times)
    for (int i = 1; i < names.length(); ++i) {
        sqliteDbi->getMsaDbi()->updateMsaName(msaId, names[i], os);
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

    // Additional action
    sqliteDbi->getMsaDbi()->updateMsaName(msaId, newName, os);
    CHECK_NO_ERROR(os);

    // Check modSteps
    QList<U2SingleModStep> finalModStepList = ModSQLiteSpecificTestData::getAllModSteps(msaId, os);
    CHECK_EQUAL(expectedModStepList.length(), finalModStepList.length(), "mod steps table size");
    for (int i = 0; i < expectedModStepList.length(); ++i) {
        CHECK_EQUAL(expectedModStepList[i].modType, finalModStepList[i].modType, "mod type");
        CHECK_EQUAL(QString(expectedModStepList[i].objectId), QString(finalModStepList[i].objectId), "object id");
        CHECK_EQUAL(expectedModStepList[i].version, finalModStepList[i].version, "version");
        CHECK_EQUAL(QString(expectedModStepList[i].details), QString(finalModStepList[i].details), "details");
    }
}

IMPLEMENT_TEST(ModDbiSQLiteSpecificUnitTests, updateMsaAlphabet_noModTrack) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = ModSQLiteSpecificTestData::createTestMsa(false, os);
    CHECK_NO_ERROR(os);

    // Update the msa alphabet
    U2AlphabetId newAlphabet = BaseDNAAlphabetIds::NUCL_DNA_EXTENDED();
    sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId, newAlphabet, os);
    CHECK_NO_ERROR(os);

    // Verify that there are no modification steps
    qint64 actualModStepsNum = ModSQLiteSpecificTestData::getModStepsNum(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(0, actualModStepsNum, "mod steps num");
}

IMPLEMENT_TEST(ModDbiSQLiteSpecificUnitTests, updateMsaAlphabet_severalSteps) {
    //3 changes, 3 undo steps, 2 redo steps, 2 undo steps, 1 redo step
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get some base data
    qint64 baseObjVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    QList<U2SingleModStep> baseModStepList = ModSQLiteSpecificTestData::getAllModSteps(msaId, os);
    CHECK_NO_ERROR(os);

    // Prepare value list
    QList<U2AlphabetId> alphabets;
    alphabets << sqliteDbi->getMsaDbi()->getMsaObject(msaId, os).alphabet;
    CHECK_NO_ERROR(os);
    alphabets << BaseDNAAlphabetIds::NUCL_DNA_EXTENDED();
    alphabets << BaseDNAAlphabetIds::NUCL_RNA_DEFAULT();
    alphabets << BaseDNAAlphabetIds::NUCL_RNA_EXTENDED();

    // Steps count
    int valuesCount = alphabets.length();    // changes = valuesCount - 1;
    QList<int> steps;                           // negative - undo steps, positive - redo steps;
    steps << -3 << 2 << -2 << 1;
    int expectedIndex = valuesCount - 1;
    for (int i = 0; i < steps.length(); ++i) {
        expectedIndex += steps[i];
    }

    // Prepare modStep list
    QList<U2SingleModStep> modSteps;
    for (int i = 0; i < valuesCount - 1; ++i) {
        U2SingleModStep modStep;
        modStep.modType = U2ModType::msaUpdatedAlphabet;
        modStep.objectId = msaId;
        modStep.version = baseObjVersion + i;
        //PackUtils::VERSION hardcoded, correct after test failure.
        modStep.details = "0\t" + alphabets[i].id.toLatin1() + "\t" + alphabets[i + 1].id.toLatin1();
        modSteps << modStep;
    }
    QList<U2SingleModStep> expectedModStepList = baseModStepList + modSteps;

    // Update the msa alphabet
    for (int i = 1; i < alphabets.length(); ++i) {
        sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId, alphabets[i], os);
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

    // Check modSteps
    QList<U2SingleModStep> finalModStepList = ModSQLiteSpecificTestData::getAllModSteps(msaId, os);
    CHECK_EQUAL(expectedModStepList.length(), finalModStepList.length(), "mod steps table size");
    for (int i = 0; i < expectedModStepList.length(); ++i) {
        CHECK_EQUAL(expectedModStepList[i].modType, finalModStepList[i].modType, "mod type");
        CHECK_EQUAL(QString(expectedModStepList[i].objectId), QString(finalModStepList[i].objectId), "object id");
        CHECK_EQUAL(expectedModStepList[i].version, finalModStepList[i].version, "version");
        CHECK_EQUAL(QString(expectedModStepList[i].details), QString(finalModStepList[i].details), "details");
    }
}

IMPLEMENT_TEST(ModDbiSQLiteSpecificUnitTests, updateMsaAlphabet_severalUndoThenAction) {
    //3 changes, 2 undo steps, 1 action
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get some base data
    qint64 baseObjVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    QList<U2SingleModStep> baseModStepList = ModSQLiteSpecificTestData::getAllModSteps(msaId, os);
    CHECK_NO_ERROR(os);

    // Prepare value list
    QList<U2AlphabetId> alphabets;
    alphabets << sqliteDbi->getMsaDbi()->getMsaObject(msaId, os).alphabet;
    CHECK_NO_ERROR(os);
    alphabets << BaseDNAAlphabetIds::NUCL_DNA_EXTENDED();
    alphabets << BaseDNAAlphabetIds::NUCL_RNA_DEFAULT();
    alphabets << BaseDNAAlphabetIds::NUCL_RNA_EXTENDED();
    U2AlphabetId newAlphabet = BaseDNAAlphabetIds::AMINO_DEFAULT();

    // Steps count
    int valuesCount = alphabets.length();    // changes = valuesCount - 1;
    QList<int> steps;                           // negative - undo steps, positive - redo steps;
    steps << -2;
    int expectedIndex = valuesCount - 1;
    for (int i = 0; i < steps.length(); ++i) {
        expectedIndex += steps[i];
    }

    // Prepare modStep list
    QList<U2SingleModStep> modSteps;
    for (int i = 0; i < valuesCount - 1; ++i) {
        U2SingleModStep modStep;
        modStep.modType = U2ModType::msaUpdatedAlphabet;
        modStep.objectId = msaId;
        modStep.version = baseObjVersion + i;
        //PackUtils::VERSION hardcoded, correct after test failure.
        modStep.details = "0\t" + alphabets[i].id.toLatin1() + "\t" + alphabets[i + 1].id.toLatin1();
        modSteps << modStep;
    }

    QList<U2SingleModStep> expectedModStepList = baseModStepList;
    for (int i = 0; i < expectedIndex; ++i) {
        expectedModStepList << modSteps[i];
    }

    U2SingleModStep actionModStep;
    actionModStep.modType = U2ModType::msaUpdatedAlphabet;
    actionModStep.objectId = msaId;
    actionModStep.version = baseObjVersion + expectedIndex;
    //PackUtils::VERSION hardcoded, correct after test failure.
    actionModStep.details = "0\t" + alphabets[expectedIndex].id.toLatin1() + "\t" + newAlphabet.id.toLatin1();
    expectedModStepList << actionModStep;

    // Update the msa alphabet
    for (int i = 1; i < alphabets.length(); ++i) {
        sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId, alphabets[i], os);
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

    // Additional action
    sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId, newAlphabet, os);
    CHECK_NO_ERROR(os);

    // Check modSteps
    QList<U2SingleModStep> finalModStepList = ModSQLiteSpecificTestData::getAllModSteps(msaId, os);
    CHECK_EQUAL(expectedModStepList.length(), finalModStepList.length(), "mod steps table size");
    for (int i = 0; i < expectedModStepList.length(); ++i) {
        CHECK_EQUAL(expectedModStepList[i].modType, finalModStepList[i].modType, "mod type");
        CHECK_EQUAL(QString(expectedModStepList[i].objectId), QString(finalModStepList[i].objectId), "object id");
        CHECK_EQUAL(expectedModStepList[i].version, finalModStepList[i].version, "version");
        CHECK_EQUAL(QString(expectedModStepList[i].details), QString(finalModStepList[i].details), "details");
    }
}

IMPLEMENT_TEST(ModDbiSQLiteSpecificUnitTests, updateGapModel_noModTrack) {
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = ModSQLiteSpecificTestData::createTestMsa(false, os);
    CHECK_NO_ERROR(os);

    // Get some base data
    QList<U2MsaRow> baseRows = sqliteDbi->getMsaDbi()->getRows(msaId, os);
    CHECK_NO_ERROR(os);

    // Update gaps
    QList<U2MsaGap> newGaps; newGaps << U2MsaGap(4, 3) << U2MsaGap(11, 3); // TAAG---ACTT---CTA
    CHECK_NO_ERROR(os);
    sqliteDbi->getMsaDbi()->updateGapModel(msaId, baseRows[0].rowId, newGaps, os);
    CHECK_NO_ERROR(os);

    // Verify no modification steps
    qint64 actualModStepsNum = ModSQLiteSpecificTestData::getModStepsNum(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(0, actualModStepsNum, "mod steps num");
}

IMPLEMENT_TEST(ModDbiSQLiteSpecificUnitTests, updateGapModel_severalSteps) {
    //6 changes, 4 undo steps, 2 redo steps, 1 undo step, 3 redo steps
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get some base data
    qint64 baseObjVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    QList<U2SingleModStep> baseModStepList = ModSQLiteSpecificTestData::getAllModSteps(msaId, os);
    CHECK_NO_ERROR(os);
    QList<U2MsaRow> baseRows = sqliteDbi->getMsaDbi()->getRows(msaId, os);
    CHECK_NO_ERROR(os);

    // Prepare value list
    QList<QList<U2MsaGap> > gapModels;
    gapModels << baseRows[0].gaps;  // base value

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

    // Prepare modStep list
    QList<U2SingleModStep> modSteps;
    for (int i = 0; i < valuesCount - 1; ++i) {
        U2SingleModStep modStep;
        modStep.modType = U2ModType::msaUpdatedGapModel;
        modStep.objectId = msaId;
        modStep.version = baseObjVersion + i;
        //PackUtils::VERSION hardcoded, correct after test failure.
        QByteArray gapsToByteArrayFirst;
        QByteArray gapsToByteArraySecond;
        gapsToByteArrayFirst += "\"";
        foreach(U2MsaGap gap, gapModels[i]) {
            if (gapsToByteArrayFirst.length() > 1) {
                gapsToByteArrayFirst += ";";
            }
            gapsToByteArrayFirst += QByteArray::number(gap.offset) + "," + QByteArray::number(gap.gap);
        }
        gapsToByteArrayFirst += "\"";
        gapsToByteArraySecond += "\"";
        foreach(U2MsaGap gap, gapModels[i + 1]) {
            if (gapsToByteArraySecond.length() > 1) {
                gapsToByteArraySecond += ";";
            }
            gapsToByteArraySecond += QByteArray::number(gap.offset) + "," + QByteArray::number(gap.gap);
        }
        gapsToByteArraySecond += "\"";

        modStep.details = "0\t" + QByteArray::number(baseRows[0].rowId) + "\t" + gapsToByteArrayFirst + "\t" + gapsToByteArraySecond;
        modSteps << modStep;
    }
    QList<U2SingleModStep> expectedModStepList = baseModStepList + modSteps;

    // Update the msa gap model
    for (int i = 1; i < gapModels.length(); ++i) {
        sqliteDbi->getMsaDbi()->updateGapModel(msaId, baseRows[0].rowId, gapModels[i], os);
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

    // Check modSteps
    QList<U2SingleModStep> finalModStepList = ModSQLiteSpecificTestData::getAllModSteps(msaId, os);
    CHECK_EQUAL(expectedModStepList.length(), finalModStepList.length(), "mod steps table size");
    for (int i = 0; i < expectedModStepList.length(); ++i) {
        CHECK_EQUAL(expectedModStepList[i].modType, finalModStepList[i].modType, "mod type");
        CHECK_EQUAL(QString(expectedModStepList[i].objectId), QString(finalModStepList[i].objectId), "object id");
        CHECK_EQUAL(expectedModStepList[i].version, finalModStepList[i].version, "version");
        CHECK_EQUAL(QString(expectedModStepList[i].details), QString(finalModStepList[i].details), "details");
    }
}

IMPLEMENT_TEST(ModDbiSQLiteSpecificUnitTests, updateGapModel_severalUndoThenAction) {
    //6 changes, 4 undo steps, 1 action
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get some base data
    qint64 baseObjVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    QList<U2SingleModStep> baseModStepList = ModSQLiteSpecificTestData::getAllModSteps(msaId, os);
    CHECK_NO_ERROR(os);
    QList<U2MsaRow> baseRows = sqliteDbi->getMsaDbi()->getRows(msaId, os);
    CHECK_NO_ERROR(os);

    // Prepare value list
    QList<QList<U2MsaGap> > gapModels;
    gapModels << baseRows[0].gaps;  // base value

    QList<U2MsaGap> gapModel;
    for (int i = 0; i < 6; ++i) {
        gapModel.clear();
        gapModel << U2MsaGap(0, i + 1) << U2MsaGap(i + 3, 2);
        gapModels << gapModel;
    }
    QList<U2MsaGap> newGapModel = QList<U2MsaGap>() << U2MsaGap(1, 1) << U2MsaGap(2, 1) << U2MsaGap(3, 1);

    // Steps count
    int valuesCount = gapModels.length();    // changes = valuesCount - 1;
    QList<int> steps;                        // negative - undo steps, positive - redo steps;
    steps << -4 << 2 << -1 << 3;
    int expectedIndex = valuesCount - 1;
    for (int i = 0; i < steps.length(); ++i) {
        expectedIndex += steps[i];
    }

    // Prepare modStep list
    QList<U2SingleModStep> modSteps;
    for (int i = 0; i < valuesCount - 1; ++i) {
        U2SingleModStep modStep;
        modStep.modType = U2ModType::msaUpdatedGapModel;
        modStep.objectId = msaId;
        modStep.version = baseObjVersion + i;
        //PackUtils::VERSION hardcoded, correct after test failure.
        QByteArray gapsToByteArrayFirst;
        QByteArray gapsToByteArraySecond;
        gapsToByteArrayFirst += "\"";
        foreach(U2MsaGap gap, gapModels[i]) {
            if (gapsToByteArrayFirst.length() > 1) {
                gapsToByteArrayFirst += ";";
            }
            gapsToByteArrayFirst += QByteArray::number(gap.offset) + "," + QByteArray::number(gap.gap);
        }
        gapsToByteArrayFirst += "\"";
        gapsToByteArraySecond += "\"";
        foreach(U2MsaGap gap, gapModels[i + 1]) {
            if (gapsToByteArraySecond.length() > 1) {
                gapsToByteArraySecond += ";";
            }
            gapsToByteArraySecond += QByteArray::number(gap.offset) + "," + QByteArray::number(gap.gap);
        }
        gapsToByteArraySecond += "\"";

        modStep.details = "0\t" + QByteArray::number(baseRows[0].rowId) + "\t" + gapsToByteArrayFirst + "\t" + gapsToByteArraySecond;
        modSteps << modStep;
    }

    QList<U2SingleModStep> expectedModStepList = baseModStepList;
    for (int i = 0; i < expectedIndex; ++i) {
        expectedModStepList << modSteps[i];
    }

    U2SingleModStep actionModStep;
    actionModStep.modType = U2ModType::msaUpdatedGapModel;
    actionModStep.objectId = msaId;
    actionModStep.version = baseObjVersion + expectedIndex;
    //PackUtils::VERSION hardcoded, correct after test failure.
    QByteArray gapsToByteArrayFirst;
    QByteArray gapsToByteArraySecond;
    gapsToByteArrayFirst += "\"";
    foreach(U2MsaGap gap, gapModels[expectedIndex]) {
        if (gapsToByteArrayFirst.length() > 1) {
            gapsToByteArrayFirst += ";";
        }
        gapsToByteArrayFirst += QByteArray::number(gap.offset) + "," + QByteArray::number(gap.gap);
    }
    gapsToByteArrayFirst += "\"";
    gapsToByteArraySecond += "\"";
    foreach(U2MsaGap gap, newGapModel) {
        if (gapsToByteArraySecond.length() > 1) {
            gapsToByteArraySecond += ";";
        }
        gapsToByteArraySecond += QByteArray::number(gap.offset) + "," + QByteArray::number(gap.gap);
    }
    gapsToByteArraySecond += "\"";

    actionModStep.details = "0\t" + QByteArray::number(baseRows[0].rowId) + "\t" + gapsToByteArrayFirst + "\t" + gapsToByteArraySecond;
    expectedModStepList << actionModStep;

    // Update the msa gap model
    for (int i = 1; i < gapModels.length(); ++i) {
        sqliteDbi->getMsaDbi()->updateGapModel(msaId, baseRows[0].rowId, gapModels[i], os);
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

    // Additional action
    sqliteDbi->getMsaDbi()->updateGapModel(msaId, baseRows[0].rowId, newGapModel, os);
    CHECK_NO_ERROR(os);

    // Check modSteps
    QList<U2SingleModStep> finalModStepList = ModSQLiteSpecificTestData::getAllModSteps(msaId, os);
    CHECK_EQUAL(expectedModStepList.length(), finalModStepList.length(), "mod steps table size");
    for (int i = 0; i < expectedModStepList.length(); ++i) {
        CHECK_EQUAL(expectedModStepList[i].modType, finalModStepList[i].modType, "mod type");
        CHECK_EQUAL(QString(expectedModStepList[i].objectId), QString(finalModStepList[i].objectId), "object id");
        CHECK_EQUAL(expectedModStepList[i].version, finalModStepList[i].version, "version");
        CHECK_EQUAL(QString(expectedModStepList[i].details), QString(finalModStepList[i].details), "details");
    }
}

IMPLEMENT_TEST(ModDbiSQLiteSpecificUnitTests, updateRowContent_noModTrack) {
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = ModSQLiteSpecificTestData::createTestMsa(false, os);
    CHECK_NO_ERROR(os);

    // Get some base data
    QList<U2MsaRow> baseRows = sqliteDbi->getMsaDbi()->getRows(msaId, os);
    CHECK_NO_ERROR(os);

    // Update row content
    QByteArray newSeq = "AAAAGGGGCCCCTTTT";
    QList<U2MsaGap> newGaps; newGaps << U2MsaGap(4, 4) << U2MsaGap(20, 4); // AAAA----GGGGCCCCTTTT----
    sqliteDbi->getMsaDbi()->updateRowContent(msaId, baseRows[0].rowId, newSeq, newGaps, os);
    CHECK_NO_ERROR(os);

    // Verify no modification steps
    qint64 modStepsNum = ModSQLiteSpecificTestData::getModStepsNum(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(0, modStepsNum, "mod steps num");
}

IMPLEMENT_TEST(ModDbiSQLiteSpecificUnitTests, updateRowContent_severalSteps) {
    //6 changes, 6 undo steps, 4 redo steps, 3 undo step, 2 redo steps
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get some base data
    qint64 rowNumber = 0;
    qint64 baseMsaVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    QList<U2SingleModStep> baseMsaModStepList = ModSQLiteSpecificTestData::getAllModSteps(msaId, os);
    CHECK_NO_ERROR(os);
    QList<U2MsaRow> baseRows = sqliteDbi->getMsaDbi()->getRows(msaId, os);
    CHECK_NO_ERROR(os);
    qint64 baseSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(baseRows[rowNumber].sequenceId, os);
    CHECK_NO_ERROR(os);
    QList<U2SingleModStep> baseSeqModStepList = ModSQLiteSpecificTestData::getAllModSteps(baseRows[rowNumber].sequenceId, os);
    CHECK_NO_ERROR(os);

    // Prepare value list
    QList<QByteArray> seqDataList;
    QList<U2MsaRow> rowInfoList;
    seqDataList << sqliteDbi->getSequenceDbi()->getSequenceData(baseRows[rowNumber].sequenceId, U2_REGION_MAX, os);
    CHECK_NO_ERROR(os);
    rowInfoList << baseRows[rowNumber];
    for (int i = 0; i < 6; ++i) {
        QByteArray firstPart((i + 1) * 2, 'A');
        QByteArray secondPart((i + 3) * 2, 'C');
        QList<U2MsaGap> gapModel = QList<U2MsaGap>() << U2MsaGap((i + 2) * 2, 5);

        U2MsaRow row;
        row.gaps = gapModel;
        row.gstart = 0;
        row.gend = firstPart.length() + secondPart.length();
        row.length = row.gend;
        foreach (const U2MsaGap& gap, gapModel) {
            if (gap.offset < row.length) { // ignore trailing gaps
                row.length += gap.gap;
            }
        }
        row.rowId = baseRows[rowNumber].rowId;
        row.sequenceId = baseRows[rowNumber].sequenceId;

        seqDataList << firstPart + secondPart;
        rowInfoList << row;
    }

    // Steps count
    int valuesCount = seqDataList.length();    // changes = valuesCount - 1;
    QList<int> steps;                          // negative - undo steps, positive - redo steps;
    steps << -6 << 4 << -3 << 2;
    int expectedIndex = valuesCount - 1;
    for (int i = 0; i < steps.length(); ++i) {
        expectedIndex += steps[i];
    }

    // Prepare modStep list
    // msa singleModSteps
    QList<U2SingleModStep> msaModSteps;
    for (int i = 0; i < valuesCount - 1; ++i) {
        U2SingleModStep rowModStep;
        rowModStep.modType = U2ModType::msaUpdatedRowInfo;
        rowModStep.objectId = msaId;
        rowModStep.version = baseMsaVersion + i;
        rowModStep.details = U2DbiPackUtils::packRowInfoDetails(rowInfoList[i], rowInfoList[i + 1]);
        msaModSteps << rowModStep;

        U2SingleModStep gapModStep;
        gapModStep.modType = U2ModType::msaUpdatedGapModel;
        gapModStep.objectId = msaId;
        gapModStep.version = baseMsaVersion + i;
        gapModStep.details = U2DbiPackUtils::packGapDetails(baseRows[rowNumber].rowId,
                                                               rowInfoList[i].gaps,
                                                               rowInfoList[i + 1].gaps);
        msaModSteps << gapModStep;
    }
    QList<U2SingleModStep> expectedMsaModStepList = baseMsaModStepList + msaModSteps;

    // sequence singleModSteps
    QList<U2SingleModStep> seqModSteps;
    for (int i = 0; i < valuesCount - 1; ++i) {
        U2SingleModStep modStep;
        modStep.modType = U2ModType::sequenceUpdatedData;
        modStep.objectId = baseRows[rowNumber].sequenceId;
        modStep.version = baseSeqVersion + i;
        modStep.details = U2DbiPackUtils::packSequenceDataDetails(U2_REGION_MAX,
                                                                     seqDataList[i],
                                                                     seqDataList[i + 1],
                                                                     QVariantMap());
        seqModSteps << modStep;
    }
    QList<U2SingleModStep> expectedSeqModStepList = baseSeqModStepList + seqModSteps;

    // Update row content
    for (int i = 1; i < seqDataList.length(); ++i) {
        sqliteDbi->getMsaDbi()->updateRowContent(msaId, baseRows[0].rowId, seqDataList[i], rowInfoList[i].gaps, os);
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

    // Check msaModSteps
    QList<U2SingleModStep> finalMsaModStepList = ModSQLiteSpecificTestData::getAllModSteps(msaId, os);
    CHECK_EQUAL(expectedMsaModStepList.length(), finalMsaModStepList.length(), "msa mod steps table size");
    for (int i = 0; i < expectedMsaModStepList.length(); ++i) {
        CHECK_EQUAL(expectedMsaModStepList[i].modType, finalMsaModStepList[i].modType, "msa mod type");
        CHECK_EQUAL(QString(expectedMsaModStepList[i].objectId), QString(finalMsaModStepList[i].objectId), "msa object id");
        CHECK_EQUAL(expectedMsaModStepList[i].version, finalMsaModStepList[i].version, "msa version");
        CHECK_EQUAL(QString(expectedMsaModStepList[i].details), QString(finalMsaModStepList[i].details), "msa mod details");
    }

    // Check seqModSteps
    QList<U2SingleModStep> finalSeqModStepList = ModSQLiteSpecificTestData::getAllModSteps(baseRows[rowNumber].sequenceId, os);
    CHECK_EQUAL(expectedSeqModStepList.length(), finalSeqModStepList.length(), "seq mod steps table size");
    for (int i = 0; i < expectedSeqModStepList.length(); ++i) {
        CHECK_EQUAL(expectedSeqModStepList[i].modType, finalSeqModStepList[i].modType, "seq mod type");
        CHECK_EQUAL(QString(expectedSeqModStepList[i].objectId), QString(finalSeqModStepList[i].objectId), "seq object id");
        CHECK_EQUAL(expectedSeqModStepList[i].version, finalSeqModStepList[i].version, "seq version");
        CHECK_EQUAL(QString(expectedSeqModStepList[i].details), QString(finalSeqModStepList[i].details), "seq mod details");
    }
}

IMPLEMENT_TEST(ModDbiSQLiteSpecificUnitTests, updateRowContent_severalUndoThenAction) {
    //6 changes, 6 undo steps, 1 action
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get some base data
    qint64 rowNumber = 0;
    qint64 baseMsaVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    QList<U2SingleModStep> baseMsaModStepList = ModSQLiteSpecificTestData::getAllModSteps(msaId, os);
    CHECK_NO_ERROR(os);
    QList<U2MsaRow> baseRows = sqliteDbi->getMsaDbi()->getRows(msaId, os);
    CHECK_NO_ERROR(os);
    qint64 baseSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(baseRows[rowNumber].sequenceId, os);
    CHECK_NO_ERROR(os);
    QList<U2SingleModStep> baseSeqModStepList = ModSQLiteSpecificTestData::getAllModSteps(baseRows[rowNumber].sequenceId, os);
    CHECK_NO_ERROR(os);

    // Prepare value list
    QList<QByteArray> seqDataList;
    QList<U2MsaRow> rowInfoList;
    seqDataList << sqliteDbi->getSequenceDbi()->getSequenceData(baseRows[rowNumber].sequenceId, U2_REGION_MAX, os);
    CHECK_NO_ERROR(os);
    rowInfoList << baseRows[rowNumber];
    for (int i = 0; i < 6; ++i) {
        QByteArray firstPart((i + 1) * 2, 'A');
        QByteArray secondPart((i + 3) * 2, 'C');
        QList<U2MsaGap> gapModel = QList<U2MsaGap>() << U2MsaGap((i + 2) * 2, 5);

        U2MsaRow row;
        row.gaps = gapModel;
        row.gstart = 0;
        row.gend = firstPart.length() + secondPart.length();
        row.length = row.gend;
        foreach (const U2MsaGap& gap, gapModel) {
            if (gap.offset < row.length) { // ignore trailing gaps
                row.length += gap.gap;
            }
        }
        row.rowId = baseRows[rowNumber].rowId;
        row.sequenceId = baseRows[rowNumber].sequenceId;

        seqDataList << firstPart + secondPart;
        rowInfoList << row;
    }
    QByteArray newSeqData = "AA";
    U2MsaRow newRow;
    newRow.gaps = QList<U2MsaGap>() << U2MsaGap(1, 1);
    newRow.gstart = 0;
    newRow.gend = newSeqData.length();
    newRow.length = newRow.gend + 1;
    newRow.rowId = baseRows[rowNumber].rowId;
    newRow.sequenceId = baseRows[rowNumber].sequenceId;

    // Steps count
    int valuesCount = seqDataList.length();    // changes = valuesCount - 1;
    QList<int> steps;                          // negative - undo steps, positive - redo steps;
    steps << -6 << 4 << -3 << 2;
    int expectedIndex = valuesCount - 1;
    for (int i = 0; i < steps.length(); ++i) {
        expectedIndex += steps[i];
    }

    // Prepare modStep list
    // msa singleModSteps
    QList<U2SingleModStep> msaModSteps;
    for (int i = 0; i < valuesCount - 1; ++i) {
        U2SingleModStep rowModStep;
        rowModStep.modType = U2ModType::msaUpdatedRowInfo;
        rowModStep.objectId = msaId;
        rowModStep.version = baseMsaVersion + i;
        rowModStep.details = U2DbiPackUtils::packRowInfoDetails(rowInfoList[i], rowInfoList[i + 1]);
        msaModSteps << rowModStep;

        U2SingleModStep gapModStep;
        gapModStep.modType = U2ModType::msaUpdatedGapModel;
        gapModStep.objectId = msaId;
        gapModStep.version = baseMsaVersion + i;
        gapModStep.details = U2DbiPackUtils::packGapDetails(baseRows[rowNumber].rowId,
                                                               rowInfoList[i].gaps,
                                                               rowInfoList[i + 1].gaps);
        msaModSteps << gapModStep;
    }

    // sequence singleModSteps
    QList<U2SingleModStep> seqModSteps;
    for (int i = 0; i < valuesCount - 1; ++i) {
        U2SingleModStep modStep;
        modStep.modType = U2ModType::sequenceUpdatedData;
        modStep.objectId = baseRows[rowNumber].sequenceId;
        modStep.version = baseSeqVersion + i;
        modStep.details = U2DbiPackUtils::packSequenceDataDetails(U2_REGION_MAX,
                                                                     seqDataList[i],
                                                                     seqDataList[i + 1],
                                                                     QVariantMap());
        seqModSteps << modStep;
    }

    QList<U2SingleModStep> expectedSeqModStepList = baseSeqModStepList;
    QList<U2SingleModStep> expectedMsaModStepList = baseMsaModStepList;
    for (int i = 0; i < expectedIndex; ++i) {
        expectedMsaModStepList << msaModSteps[i * 2];
        expectedMsaModStepList << msaModSteps[(i * 2) + 1];
        expectedSeqModStepList << seqModSteps[i];
    }

    U2SingleModStep actionSeqModStep;
    actionSeqModStep.modType = U2ModType::sequenceUpdatedData;
    actionSeqModStep.objectId = baseRows[rowNumber].sequenceId;
    actionSeqModStep.version = baseSeqVersion + expectedIndex;
    actionSeqModStep.details = U2DbiPackUtils::packSequenceDataDetails(U2_REGION_MAX,
                                                                          seqDataList[expectedIndex],
                                                                          newSeqData,
                                                                          QVariantMap());
    U2SingleModStep actionRowModStep;
    actionRowModStep.modType = U2ModType::msaUpdatedRowInfo;
    actionRowModStep.objectId = msaId;
    actionRowModStep.version = baseMsaVersion + expectedIndex;
    actionRowModStep.details = U2DbiPackUtils::packRowInfoDetails(rowInfoList[expectedIndex], newRow);

    U2SingleModStep actionGapModStep;
    actionGapModStep.modType = U2ModType::msaUpdatedGapModel;
    actionGapModStep.objectId = msaId;
    actionGapModStep.version = baseMsaVersion + expectedIndex;
    actionGapModStep.details = U2DbiPackUtils::packGapDetails(baseRows[rowNumber].rowId,
                                                                 rowInfoList[expectedIndex].gaps,
                                                                 newRow.gaps);

    expectedSeqModStepList << actionSeqModStep;
    expectedMsaModStepList << actionRowModStep;
    expectedMsaModStepList << actionGapModStep;

    // Update row content
    for (int i = 1; i < seqDataList.length(); ++i) {
        sqliteDbi->getMsaDbi()->updateRowContent(msaId, baseRows[rowNumber].rowId, seqDataList[i], rowInfoList[i].gaps, os);
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

    // Additional action
    sqliteDbi->getMsaDbi()->updateRowContent(msaId, baseRows[rowNumber].rowId, newSeqData, newRow.gaps, os);
    CHECK_NO_ERROR(os);

    // Check msaModSteps
    QList<U2SingleModStep> finalMsaModStepList = ModSQLiteSpecificTestData::getAllModSteps(msaId, os);
    CHECK_EQUAL(expectedMsaModStepList.length(), finalMsaModStepList.length(), "msa mod steps table size");
    for (int i = 0; i < expectedMsaModStepList.length(); ++i) {
        CHECK_EQUAL(expectedMsaModStepList[i].modType, finalMsaModStepList[i].modType, "msa mod type");
        CHECK_EQUAL(QString(expectedMsaModStepList[i].objectId), QString(finalMsaModStepList[i].objectId), "msa object id");
        CHECK_EQUAL(expectedMsaModStepList[i].version, finalMsaModStepList[i].version, "msa version");
        CHECK_EQUAL(QString(expectedMsaModStepList[i].details), QString(finalMsaModStepList[i].details), "msa mod details");
    }

    // Check seqModSteps
    QList<U2SingleModStep> finalSeqModStepList = ModSQLiteSpecificTestData::getAllModSteps(baseRows[rowNumber].sequenceId, os);
    CHECK_EQUAL(expectedSeqModStepList.length(), finalSeqModStepList.length(), "seq mod steps table size");
    for (int i = 0; i < expectedSeqModStepList.length(); ++i) {
        CHECK_EQUAL(expectedSeqModStepList[i].modType, finalSeqModStepList[i].modType, "seq mod type");
        CHECK_EQUAL(QString(expectedSeqModStepList[i].objectId), QString(finalSeqModStepList[i].objectId), "seq object id");
        CHECK_EQUAL(expectedSeqModStepList[i].version, finalSeqModStepList[i].version, "seq version");
        CHECK_EQUAL(QString(expectedSeqModStepList[i].details), QString(finalSeqModStepList[i].details), "seq mod details");
    }
}

IMPLEMENT_TEST(ModDbiSQLiteSpecificUnitTests, setNewRowsOrder_noModTrack) {
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = ModSQLiteSpecificTestData::createTestMsa(false, os);
    CHECK_NO_ERROR(os);

    // Get current row order
    QList<qint64> oldOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_NOT_EQUAL(0, oldOrder.length(), "incorrect oreder list length");

    // Set new row order
    //Expected order (indexes): 4, 5, 1, 6, 0, 2, 3
    QList<qint64> newOrder = oldOrder;
    newOrder.swap(0, 4);
    newOrder.swap(3, 6);
    newOrder.swap(2, 5);
    newOrder.swap(1, 2);

    sqliteDbi->getMsaDbi()->setNewRowsOrder(msaId, newOrder, os);
    CHECK_NO_ERROR(os);

    // Verify no modification steps
    qint64 modStepsNum = ModSQLiteSpecificTestData::getModStepsNum(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(0, modStepsNum, "mod steps num");
}

IMPLEMENT_TEST(ModDbiSQLiteSpecificUnitTests, setNewRowsOrder_severalSteps) {
    //6 changes, 5 undo steps, 3 redo steps, 4 undo step, 3 redo steps
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get some base data
    qint64 baseObjVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    QList<U2SingleModStep> baseModStepList = ModSQLiteSpecificTestData::getAllModSteps(msaId, os);
    CHECK_NO_ERROR(os);
    QList<qint64> baseRowOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);

    // Prepare value list
    QList<QList<qint64> > rowOrders;
    rowOrders << baseRowOrder;

    QList<qint64> rowOrder;
    for (int i = 0; i < 6; ++i) {
        rowOrder.clear();
        for (int j = 1; j <= baseRowOrder.length(); ++j) {
            // On every step:
            // 0 1 2 3 4 5 6
            // 4 1 5 2 6 3 0
            rowOrder << rowOrders[i][(j * 11) % baseRowOrder.length()];
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

    // Prepare modStep list
    QList<U2SingleModStep> modSteps;
    for (int i = 0; i < valuesCount - 1; ++i) {
        U2SingleModStep modStep;
        modStep.modType = U2ModType::msaSetNewRowsOrder;
        modStep.objectId = msaId;
        modStep.version = baseObjVersion + i;
        //PackUtils::VERSION hardcoded, correct after test failure.
        QByteArray orderToByteArrayFirst;
        QByteArray orderToByteArraySecond;
        orderToByteArrayFirst += "\"";
        foreach(qint64 rowId, rowOrders[i]) {
            if (orderToByteArrayFirst.length() > 1) {
                orderToByteArrayFirst += ",";
            }
            orderToByteArrayFirst += QByteArray::number(rowId);
        }
        orderToByteArrayFirst += "\"";
        orderToByteArraySecond += "\"";
        foreach(qint64 rowId, rowOrders[i + 1]) {
            if (orderToByteArraySecond.length() > 1) {
                orderToByteArraySecond += ",";
            }
            orderToByteArraySecond += QByteArray::number(rowId);
        }
        orderToByteArraySecond += "\"";

        modStep.details = "0\t" + orderToByteArrayFirst + "\t" + orderToByteArraySecond;
        modSteps << modStep;
    }
    QList<U2SingleModStep> expectedModStepList = baseModStepList + modSteps;

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

    // Check modSteps
    QList<U2SingleModStep> finalModStepList = ModSQLiteSpecificTestData::getAllModSteps(msaId, os);
    CHECK_EQUAL(expectedModStepList.length(), finalModStepList.length(), "mod steps table size");
    for (int i = 0; i < expectedModStepList.length(); ++i) {
        CHECK_EQUAL(expectedModStepList[i].modType, finalModStepList[i].modType, "mod type");
        CHECK_EQUAL(QString(expectedModStepList[i].objectId), QString(finalModStepList[i].objectId), "object id");
        CHECK_EQUAL(expectedModStepList[i].version, finalModStepList[i].version, "version");
        CHECK_EQUAL(QString(expectedModStepList[i].details), QString(finalModStepList[i].details), "details");
    }
}

IMPLEMENT_TEST(ModDbiSQLiteSpecificUnitTests, setNewRowsOrder_severalUndoThenAction) {
    //6 changes, 5 undo steps, 1 action
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get some base data
    qint64 baseObjVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);
    QList<U2SingleModStep> baseModStepList = ModSQLiteSpecificTestData::getAllModSteps(msaId, os);
    CHECK_NO_ERROR(os);
    QList<qint64> baseRowOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);

    // Prepare value list
    QList<QList<qint64> > rowOrders;
    rowOrders << baseRowOrder;
    QList<qint64> newRowOrder = QList<qint64>() << baseRowOrder[6]
                                                << baseRowOrder[5]
                                                << baseRowOrder[4]
                                                << baseRowOrder[3]
                                                << baseRowOrder[2]
                                                << baseRowOrder[1]
                                                << baseRowOrder[0];

    QList<qint64> rowOrder;
    for (int i = 0; i < 6; ++i) {
        rowOrder.clear();
        for (int j = 1; j <= baseRowOrder.length(); ++j) {
            // On every step:
            // 0 1 2 3 4 5 6
            // 4 1 5 2 6 3 0
            rowOrder << rowOrders[i][(j * 11) % baseRowOrder.length()];
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

    // Prepare modStep list
    QList<U2SingleModStep> modSteps;
    for (int i = 0; i < valuesCount - 1; ++i) {
        U2SingleModStep modStep;
        modStep.modType = U2ModType::msaSetNewRowsOrder;
        modStep.objectId = msaId;
        modStep.version = baseObjVersion + i;
        //PackUtils::VERSION hardcoded, correct after test failure.
        QByteArray orderToByteArrayFirst;
        QByteArray orderToByteArraySecond;
        orderToByteArrayFirst += "\"";
        foreach(qint64 rowId, rowOrders[i]) {
            if (orderToByteArrayFirst.length() > 1) {
                orderToByteArrayFirst += ",";
            }
            orderToByteArrayFirst += QByteArray::number(rowId);
        }
        orderToByteArrayFirst += "\"";
        orderToByteArraySecond += "\"";
        foreach(qint64 rowId, rowOrders[i + 1]) {
            if (orderToByteArraySecond.length() > 1) {
                orderToByteArraySecond += ",";
            }
            orderToByteArraySecond += QByteArray::number(rowId);
        }
        orderToByteArraySecond += "\"";

        modStep.details = "0\t" + orderToByteArrayFirst + "\t" + orderToByteArraySecond;
        modSteps << modStep;
    }

    QList<U2SingleModStep> expectedModStepList = baseModStepList;
    for (int i = 0; i < expectedIndex; ++i) {
        expectedModStepList << modSteps[i];
    }

    U2SingleModStep actionModStep;
    actionModStep.modType = U2ModType::msaSetNewRowsOrder;
    actionModStep.objectId = msaId;
    actionModStep.version = baseObjVersion + expectedIndex;
    //PackUtils::VERSION hardcoded, correct after test failure.
    QByteArray orderToByteArrayFirst;
    QByteArray orderToByteArraySecond;
    orderToByteArrayFirst += "\"";
    foreach(qint64 rowId, rowOrders[expectedIndex]) {
        if (orderToByteArrayFirst.length() > 1) {
            orderToByteArrayFirst += ",";
        }
        orderToByteArrayFirst += QByteArray::number(rowId);
    }
    orderToByteArrayFirst += "\"";
    orderToByteArraySecond += "\"";
    foreach(qint64 rowId, newRowOrder) {
        if (orderToByteArraySecond.length() > 1) {
            orderToByteArraySecond += ",";
        }
        orderToByteArraySecond += QByteArray::number(rowId);
    }
    orderToByteArraySecond += "\"";

    actionModStep.details = "0\t" + orderToByteArrayFirst + "\t" + orderToByteArraySecond;
    expectedModStepList << actionModStep;

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

    // Additional action
    sqliteDbi->getMsaDbi()->setNewRowsOrder(msaId, newRowOrder, os);
    CHECK_NO_ERROR(os);

    // Check modSteps
    QList<U2SingleModStep> finalModStepList = ModSQLiteSpecificTestData::getAllModSteps(msaId, os);
    CHECK_EQUAL(expectedModStepList.length(), finalModStepList.length(), "mod steps table size");
    for (int i = 0; i < expectedModStepList.length(); ++i) {
        CHECK_EQUAL(expectedModStepList[i].modType, finalModStepList[i].modType, "mod type");
        CHECK_EQUAL(QString(expectedModStepList[i].objectId), QString(finalModStepList[i].objectId), "object id");
        CHECK_EQUAL(expectedModStepList[i].version, finalModStepList[i].version, "version");
        CHECK_EQUAL(QString(expectedModStepList[i].details), QString(finalModStepList[i].details), "details");
    }
}

IMPLEMENT_TEST(ModDbiSQLiteSpecificUnitTests, updateRowName_noModTrack) {
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = ModSQLiteSpecificTestData::createTestMsa(false, os);
    CHECK_NO_ERROR(os);

    // Get some base data
    QList<U2MsaRow> baseRows = sqliteDbi->getMsaDbi()->getRows(msaId, os);
    CHECK_NO_ERROR(os);

    // Update row name
    U2MsaRow oldRow = sqliteDbi->getMsaDbi()->getRow(msaId, baseRows[0].rowId, os);
    CHECK_NO_ERROR(os);
    QString oldName = sqliteDbi->getSequenceDbi()->getSequenceObject(oldRow.sequenceId, os).visualName;
    CHECK_NO_ERROR(os);
    QString newName = oldName + "_new";
    sqliteDbi->getMsaDbi()->updateRowName(msaId, baseRows[0].rowId, newName, os);
    CHECK_NO_ERROR(os);

    // Verify no modification steps
    qint64 modStepsNum = ModSQLiteSpecificTestData::getModStepsNum(msaId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(0, modStepsNum, "mod steps num");
}

IMPLEMENT_TEST(ModDbiSQLiteSpecificUnitTests, updateRowName_severalSteps) {
    //6 changes, 3 undo steps, 1 redo steps, 4 undo step, 3 redo steps
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get some base data
    QList<qint64> baseRowOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    U2MsaRow baseRow = sqliteDbi->getMsaDbi()->getRow(msaId, baseRowOrder[1], os);
    CHECK_NO_ERROR(os);
    qint64 baseObjVersion = sqliteDbi->getObjectDbi()->getObjectVersion(baseRow.sequenceId, os);
    CHECK_NO_ERROR(os);
    QList<U2SingleModStep> baseModStepList = ModSQLiteSpecificTestData::getAllModSteps(baseRow.sequenceId, os);
    CHECK_NO_ERROR(os);

    // Prepare value list
    QStringList rowNames;
    rowNames << sqliteDbi->getSequenceDbi()->getSequenceObject(baseRow.sequenceId, os).visualName;
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

    // Prepare modStep list
    QList<U2SingleModStep> modSteps;
    for (int i = 0; i < valuesCount - 1; ++i) {
        U2SingleModStep modStep;
        modStep.modType = U2ModType::objUpdatedName;
        modStep.objectId = baseRow.sequenceId;
        modStep.version = baseObjVersion + i;
        //PackUtils::VERSION hardcoded, correct after test failure.
        modStep.details = "0\t" + rowNames[i].toLatin1() + "\t" + rowNames[i + 1].toLatin1();
        modSteps << modStep;
    }
    QList<U2SingleModStep> expectedModStepList = modSteps;

    // Update row name
    for (int i = 1; i < rowNames.length(); ++i) {
        sqliteDbi->getMsaDbi()->updateRowName(msaId, baseRowOrder[1], rowNames[i], os);
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

    // Check modSteps
    QList<U2SingleModStep> finalModStepList = ModSQLiteSpecificTestData::getAllModSteps(baseRow.sequenceId, os);
    CHECK_EQUAL(expectedModStepList.length(), finalModStepList.length(), "mod steps table size");
    for (int i = 0; i < expectedModStepList.length(); ++i) {
        CHECK_EQUAL(expectedModStepList[i].modType, finalModStepList[i].modType, "mod type");
        CHECK_EQUAL(QString(expectedModStepList[i].objectId), QString(finalModStepList[i].objectId), "object id");
        CHECK_EQUAL(expectedModStepList[i].version, finalModStepList[i].version, "version");
        CHECK_EQUAL(QString(expectedModStepList[i].details), QString(finalModStepList[i].details), "details");
    }
}

IMPLEMENT_TEST(ModDbiSQLiteSpecificUnitTests, updateRowName_severalUndoThenAction) {
    //6 changes, 3 undo steps, 1 action
    U2OpStatusImpl os;
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2DataId msaId = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);

    // Get some base data
    QList<qint64> baseRowOrder = sqliteDbi->getMsaDbi()->getRowsOrder(msaId, os);
    CHECK_NO_ERROR(os);
    U2MsaRow baseRow = sqliteDbi->getMsaDbi()->getRow(msaId, baseRowOrder[1], os);
    CHECK_NO_ERROR(os);
    qint64 baseObjVersion = sqliteDbi->getObjectDbi()->getObjectVersion(baseRow.sequenceId, os);
    CHECK_NO_ERROR(os);
    QList<U2SingleModStep> baseModStepList = ModSQLiteSpecificTestData::getAllModSteps(baseRow.sequenceId, os);
    CHECK_NO_ERROR(os);

    // Prepare value list
    QStringList rowNames;
    rowNames << sqliteDbi->getSequenceDbi()->getSequenceObject(baseRow.sequenceId, os).visualName;
    CHECK_NO_ERROR(os);

    for (int i = 0; i < 6; ++i) {
        rowNames << rowNames[i] + "test";
    }
    QString newRowName("Action occurred");

    // Steps count
    int valuesCount = rowNames.length();     // changes = valuesCount - 1;
    QList<int> steps;                        // negative - undo steps, positive - redo steps;
    steps << -3 << 1 << -4 << 3;
    int expectedIndex = valuesCount - 1;
    for (int i = 0; i < steps.length(); ++i) {
        expectedIndex += steps[i];
    }

    // Prepare modStep list
    QList<U2SingleModStep> modSteps;
    for (int i = 0; i < valuesCount - 1; ++i) {
        U2SingleModStep modStep;
        modStep.modType = U2ModType::objUpdatedName;
        modStep.objectId = baseRow.sequenceId;
        modStep.version = baseObjVersion + i;
        //PackUtils::VERSION hardcoded, correct after test failure.
        modStep.details = "0\t" + rowNames[i].toLatin1() + "\t" + rowNames[i + 1].toLatin1();
        modSteps << modStep;
    }

    QList<U2SingleModStep> expectedModStepList = baseModStepList;
    for (int i = 0; i < expectedIndex; ++i) {
        expectedModStepList << modSteps[i];
    }

    U2SingleModStep actionModStep;
    actionModStep.modType = U2ModType::objUpdatedName;
    actionModStep.objectId = baseRow.sequenceId;
    actionModStep.version = baseObjVersion + expectedIndex;
    //PackUtils::VERSION hardcoded, correct after test failure.
    actionModStep.details = "0\t" + rowNames[expectedIndex].toLatin1() + "\t" + newRowName.toLatin1();
    expectedModStepList << actionModStep;

    // Update row name
    for (int i = 1; i < rowNames.length(); ++i) {
        sqliteDbi->getMsaDbi()->updateRowName(msaId, baseRowOrder[1], rowNames[i], os);
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

    // Additional action
    sqliteDbi->getMsaDbi()->updateRowName(msaId, baseRowOrder[1], newRowName, os);
    CHECK_NO_ERROR(os);

    // Check modSteps
    QList<U2SingleModStep> finalModStepList = ModSQLiteSpecificTestData::getAllModSteps(baseRow.sequenceId, os);
    CHECK_EQUAL(expectedModStepList.length(), finalModStepList.length(), "mod steps table size");
    for (int i = 0; i < expectedModStepList.length(); ++i) {
        CHECK_EQUAL(expectedModStepList[i].modType, finalModStepList[i].modType, "mod type");
        CHECK_EQUAL(QString(expectedModStepList[i].objectId), QString(finalModStepList[i].objectId), "object id");
        CHECK_EQUAL(expectedModStepList[i].version, finalModStepList[i].version, "version");
        CHECK_EQUAL(QString(expectedModStepList[i].details), QString(finalModStepList[i].details), "details");
    }
}



IMPLEMENT_MOD_TEST(ModDbiSQLiteSpecificUnitTests, createStep_noMultiAndUser) {
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2OpStatusImpl os;

    U2SingleModStep singleStep = ModSQLiteSpecificTestData::prepareSingleStep(0, os);
    CHECK_NO_ERROR(os);

    sqliteDbi->getSQLiteModDbi()->createModStep(singleStep.objectId, singleStep, os);
    CHECK_NO_ERROR(os);

    QList<U2SingleModStep> actualSingleSteps;
    QList<U2MultiModStep4Test> actualMultiSteps;
    QList<U2UserModStep4Test> actualUserSteps;
    ModSQLiteSpecificTestData::getAllSteps(actualSingleSteps, actualMultiSteps, actualUserSteps, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(1, actualSingleSteps.count(), "single steps num");
    CHECK_EQUAL(1, actualMultiSteps.count(), "multi steps num");
    CHECK_EQUAL(1, actualUserSteps.count(), "user steps num");

    CHECK_EQUAL(actualSingleSteps[0].multiStepId, actualMultiSteps[0].id, "multi step id");
    CHECK_EQUAL(actualMultiSteps[0].userStepId, actualUserSteps[0].id, "user step id");
    CHECK_EQUAL(actualSingleSteps[0].objectId, actualUserSteps[0].masterObjId, "user step master object");

    bool multiStepStarted = sqliteDbi->getSQLiteModDbi()->isMultiStepStarted(singleStep.objectId);
    bool userStepStarted = sqliteDbi->getSQLiteModDbi()->isUserStepStarted(singleStep.objectId);
    CHECK_FALSE(multiStepStarted, "Multi step must be ended!");
    CHECK_FALSE(userStepStarted, "User step must be ended!");
}

IMPLEMENT_MOD_TEST(ModDbiSQLiteSpecificUnitTests, createStep_noMultiAndUser2Steps) {
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2OpStatusImpl os;

    U2SingleModStep singleStep1 = ModSQLiteSpecificTestData::prepareSingleStep(0, os); CHECK_NO_ERROR(os);
    U2SingleModStep singleStep2 = ModSQLiteSpecificTestData::prepareSingleStep(1, os); CHECK_NO_ERROR(os);

    sqliteDbi->getSQLiteModDbi()->createModStep(singleStep1.objectId, singleStep1, os); CHECK_NO_ERROR(os);
    sqliteDbi->getSQLiteModDbi()->createModStep(singleStep2.objectId, singleStep2, os); CHECK_NO_ERROR(os);

    QList<U2SingleModStep> actualSingleSteps;
    QList<U2MultiModStep4Test> actualMultiSteps;
    QList<U2UserModStep4Test> actualUserSteps;
    ModSQLiteSpecificTestData::getAllSteps(actualSingleSteps, actualMultiSteps, actualUserSteps, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(2, actualSingleSteps.count(), "single steps num");
    CHECK_EQUAL(2, actualMultiSteps.count(), "multi steps num");
    CHECK_EQUAL(2, actualUserSteps.count(), "user steps num");

    CHECK_EQUAL(actualSingleSteps[0].multiStepId, actualMultiSteps[0].id, "multi step id 1");
    CHECK_EQUAL(actualMultiSteps[0].userStepId, actualUserSteps[0].id, "user step id 1");
    CHECK_EQUAL(QString(actualSingleSteps[0].objectId), QString(actualUserSteps[0].masterObjId), "user step master object 1");

    CHECK_EQUAL(actualSingleSteps[1].multiStepId, actualMultiSteps[1].id, "multi step id 2");
    CHECK_EQUAL(actualMultiSteps[1].userStepId, actualUserSteps[1].id, "user step id 2");
    CHECK_EQUAL(actualSingleSteps[1].objectId, actualUserSteps[1].masterObjId, "user step master object 2");

    bool multiStepStarted1 = sqliteDbi->getSQLiteModDbi()->isMultiStepStarted(singleStep1.objectId);
    bool userStepStarted1 = sqliteDbi->getSQLiteModDbi()->isUserStepStarted(singleStep1.objectId);
    CHECK_FALSE(multiStepStarted1, "Multi step must be ended!");
    CHECK_FALSE(userStepStarted1, "User step must be ended!");

    bool multiStepStarted2 = sqliteDbi->getSQLiteModDbi()->isMultiStepStarted(singleStep2.objectId);
    bool userStepStarted2 = sqliteDbi->getSQLiteModDbi()->isUserStepStarted(singleStep2.objectId);
    CHECK_FALSE(multiStepStarted2, "Multi step must be ended!");
    CHECK_FALSE(userStepStarted2, "User step must be ended!");
}

IMPLEMENT_MOD_TEST(ModDbiSQLiteSpecificUnitTests, createStep_startMulti) {
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2OpStatusImpl os;

    U2SingleModStep singleStep1 = ModSQLiteSpecificTestData::prepareSingleStep(0, os); CHECK_NO_ERROR(os);
    U2SingleModStep singleStep2 = ModSQLiteSpecificTestData::prepareSingleStep(1, os); CHECK_NO_ERROR(os);

    {
        U2UseCommonMultiModStep useMultiStep(sqliteDbi, singleStep2.objectId, os); CHECK_NO_ERROR(os);
        sqliteDbi->getSQLiteModDbi()->createModStep(singleStep2.objectId, singleStep1, os); CHECK_NO_ERROR(os);
        sqliteDbi->getSQLiteModDbi()->createModStep(singleStep2.objectId, singleStep2, os); CHECK_NO_ERROR(os);

        bool multiStepStarted = sqliteDbi->getSQLiteModDbi()->isMultiStepStarted(singleStep2.objectId);
        bool userStepStarted = sqliteDbi->getSQLiteModDbi()->isUserStepStarted(singleStep2.objectId);
        CHECK_TRUE(multiStepStarted, "Multi step must be started!");
        CHECK_TRUE(userStepStarted, "User step must be ended!");
    }

    QList<U2SingleModStep> actualSingleSteps;
    QList<U2MultiModStep4Test> actualMultiSteps;
    QList<U2UserModStep4Test> actualUserSteps;
    ModSQLiteSpecificTestData::getAllSteps(actualSingleSteps, actualMultiSteps, actualUserSteps, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(2, actualSingleSteps.count(), "single steps num");
    CHECK_EQUAL(1, actualMultiSteps.count(), "multi steps num");
    CHECK_EQUAL(1, actualUserSteps.count(), "user steps num");

    qint64 multiStepId  = actualMultiSteps[0].id;

    CHECK_EQUAL(multiStepId, actualSingleSteps[0].multiStepId, "multi step id of single1");
    CHECK_EQUAL(multiStepId, actualSingleSteps[1].multiStepId, "multi step id of single2");
    CHECK_EQUAL(actualMultiSteps[0].userStepId, actualUserSteps[0].id, "user step of multi");
    CHECK_EQUAL(actualSingleSteps[1].objectId, actualUserSteps[0].masterObjId, "master object id");

    bool multiStepStarted = sqliteDbi->getSQLiteModDbi()->isMultiStepStarted(singleStep2.objectId);
    bool userStepStarted = sqliteDbi->getSQLiteModDbi()->isUserStepStarted(singleStep2.objectId);
    CHECK_FALSE(multiStepStarted, "Multi step must be ended!");
    CHECK_FALSE(userStepStarted, "User step must be ended!");
}

IMPLEMENT_MOD_TEST(ModDbiSQLiteSpecificUnitTests, createStep_start2MultiNoUser) {
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2OpStatusImpl os;

    U2SingleModStep singleStep1 = ModSQLiteSpecificTestData::prepareSingleStep(0, os); CHECK_NO_ERROR(os);
    U2SingleModStep singleStep2 = ModSQLiteSpecificTestData::prepareSingleStep(1, os); CHECK_NO_ERROR(os);
    U2SingleModStep singleStep3 = ModSQLiteSpecificTestData::prepareSingleStep(2, os); CHECK_NO_ERROR(os);

    {
        U2UseCommonMultiModStep useMultiStep1(sqliteDbi, singleStep2.objectId, os); CHECK_NO_ERROR(os);
        sqliteDbi->getSQLiteModDbi()->createModStep(singleStep2.objectId, singleStep1, os); CHECK_NO_ERROR(os);
        sqliteDbi->getSQLiteModDbi()->createModStep(singleStep2.objectId, singleStep2, os); CHECK_NO_ERROR(os);
    }
    {
        U2UseCommonMultiModStep useMultiStep2(sqliteDbi, singleStep3.objectId, os); CHECK_NO_ERROR(os);
        sqliteDbi->getSQLiteModDbi()->createModStep(singleStep3.objectId, singleStep3, os); CHECK_NO_ERROR(os);
    }

    QList<U2SingleModStep> actualSingleSteps;
    QList<U2MultiModStep4Test> actualMultiSteps;
    QList<U2UserModStep4Test> actualUserSteps;
    ModSQLiteSpecificTestData::getAllSteps(actualSingleSteps, actualMultiSteps, actualUserSteps, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(3, actualSingleSteps.count(), "single steps num");
    CHECK_EQUAL(2, actualMultiSteps.count(), "multi steps num");
    CHECK_EQUAL(2, actualUserSteps.count(), "user steps num");

    qint64 multiStepId1  = actualMultiSteps[0].id;
    qint64 multiStepId2  = actualMultiSteps[1].id;

    CHECK_EQUAL(multiStepId1, actualSingleSteps[0].multiStepId, "multi step id of single1");
    CHECK_EQUAL(multiStepId1, actualSingleSteps[1].multiStepId, "multi step id of single2");
    CHECK_EQUAL(actualMultiSteps[0].userStepId, actualUserSteps[0].id, "user step of multi1");
    CHECK_EQUAL(actualSingleSteps[1].objectId, actualUserSteps[0].masterObjId, "master object id");

    CHECK_EQUAL(multiStepId2, actualSingleSteps[2].multiStepId, "multi step id of single3");
    CHECK_EQUAL(actualMultiSteps[1].userStepId, actualUserSteps[1].id, "user step of multi2");
    CHECK_EQUAL(actualSingleSteps[2].objectId, actualUserSteps[1].masterObjId, "master object id");

    bool multiStepStarted1 = sqliteDbi->getSQLiteModDbi()->isMultiStepStarted(singleStep1.objectId);
    bool userStepStarted1 = sqliteDbi->getSQLiteModDbi()->isUserStepStarted(singleStep1.objectId);
    CHECK_FALSE(multiStepStarted1, "Multi step must be ended!");
    CHECK_FALSE(userStepStarted1, "User step must be ended!");

    bool multiStepStarted2 = sqliteDbi->getSQLiteModDbi()->isMultiStepStarted(singleStep2.objectId);
    bool userStepStarted2 = sqliteDbi->getSQLiteModDbi()->isUserStepStarted(singleStep2.objectId);
    CHECK_FALSE(multiStepStarted2, "Multi step must be ended!");
    CHECK_FALSE(userStepStarted2, "User step must be ended!");

    bool multiStepStarted3 = sqliteDbi->getSQLiteModDbi()->isMultiStepStarted(singleStep3.objectId);
    bool userStepStarted3 = sqliteDbi->getSQLiteModDbi()->isUserStepStarted(singleStep3.objectId);
    CHECK_FALSE(multiStepStarted3, "Multi step must be ended!");
    CHECK_FALSE(userStepStarted3, "User step must be ended!");

}

IMPLEMENT_MOD_TEST(ModDbiSQLiteSpecificUnitTests, createStep_startUser) {
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2OpStatusImpl os;

    U2DataId masterObjId = ModSQLiteSpecificTestData::createObject(os); CHECK_NO_ERROR(os);
    U2SingleModStep singleStep = ModSQLiteSpecificTestData::prepareSingleStep(0, os); CHECK_NO_ERROR(os);

    {
        U2UseCommonUserModStep useUserStep(sqliteDbi, masterObjId, os); CHECK_NO_ERROR(os);
        bool multiStepStarted = sqliteDbi->getSQLiteModDbi()->isMultiStepStarted(masterObjId);
        bool userStepStarted = sqliteDbi->getSQLiteModDbi()->isUserStepStarted(masterObjId);
        CHECK_TRUE(userStepStarted, "User step must be started!");
        CHECK_FALSE(multiStepStarted, "Multi step must be ended!");

        sqliteDbi->getSQLiteModDbi()->createModStep(masterObjId, singleStep, os); CHECK_NO_ERROR(os);
    }

    QList<U2SingleModStep> single;
    QList<U2MultiModStep4Test> multi;
    QList<U2UserModStep4Test> user;
    ModSQLiteSpecificTestData::getAllSteps(single, multi, user, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(1, single.count(), "single steps num");
    CHECK_EQUAL(1, multi.count(), "multi steps num");
    CHECK_EQUAL(1, user.count(), "user steps num");

    U2DataId userActualMasterObjId = user[0].masterObjId;
    CHECK_EQUAL(multi[0].id, single[0].multiStepId, "multi step id of single");
    CHECK_EQUAL(user[0].id, multi[0].userStepId, "user step id of multi");
    CHECK_EQUAL(masterObjId, user[0].masterObjId, "master object id");

    bool multiStepStarted = sqliteDbi->getSQLiteModDbi()->isMultiStepStarted(masterObjId);
    bool userStepStarted = sqliteDbi->getSQLiteModDbi()->isUserStepStarted(masterObjId);
    CHECK_FALSE(multiStepStarted, "Multi step must be ended!");
    CHECK_FALSE(userStepStarted, "User step must be ended!");
}

IMPLEMENT_MOD_TEST(ModDbiSQLiteSpecificUnitTests, createStep_oneUser2Multi) {
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2OpStatusImpl os;

    U2DataId masterObjId = ModSQLiteSpecificTestData::createObject(os); CHECK_NO_ERROR(os);
    U2SingleModStep singleStep1 = ModSQLiteSpecificTestData::prepareSingleStep(0, os); CHECK_NO_ERROR(os);
    U2SingleModStep singleStep2 = ModSQLiteSpecificTestData::prepareSingleStep(1, os); CHECK_NO_ERROR(os);

    {
        U2UseCommonUserModStep useUserStep(sqliteDbi, masterObjId, os); CHECK_NO_ERROR(os);
        {
            U2UseCommonMultiModStep useMultiStep1(sqliteDbi, masterObjId, os); CHECK_NO_ERROR(os);
            sqliteDbi->getSQLiteModDbi()->createModStep(masterObjId, singleStep1, os); CHECK_NO_ERROR(os);
        }
        {
            U2UseCommonMultiModStep useMultiStep2(sqliteDbi, masterObjId, os); CHECK_NO_ERROR(os);
            sqliteDbi->getSQLiteModDbi()->createModStep(masterObjId, singleStep2, os); CHECK_NO_ERROR(os);
        }
    }

    QList<U2SingleModStep> single;
    QList<U2MultiModStep4Test> multi;
    QList<U2UserModStep4Test> user;
    ModSQLiteSpecificTestData::getAllSteps(single, multi, user, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(2, single.count(), "single steps num");
    CHECK_EQUAL(2, multi.count(), "multi steps num");
    CHECK_EQUAL(1, user.count(), "user steps num");

    U2DataId userActualMasterObjId = user[0].masterObjId;

    CHECK_EQUAL(multi[0].id, single[0].multiStepId, "multi step id of single1");
    CHECK_EQUAL(user[0].id, multi[0].userStepId, "user step id of multi1");
    CHECK_EQUAL(masterObjId, user[0].masterObjId, "master object id");

    CHECK_EQUAL(multi[1].id, single[1].multiStepId, "multi step id of single2");
    CHECK_EQUAL(user[0].id, multi[1].userStepId, "user step id of multi2");
    CHECK_EQUAL(masterObjId, user[0].masterObjId, "master object id");

    bool multiStepStarted = sqliteDbi->getSQLiteModDbi()->isMultiStepStarted(masterObjId);
    bool userStepStarted = sqliteDbi->getSQLiteModDbi()->isUserStepStarted(masterObjId);
    CHECK_FALSE(multiStepStarted, "Multi step must be ended!");
    CHECK_FALSE(userStepStarted, "User step must be ended!");
}

IMPLEMENT_MOD_TEST(ModDbiSQLiteSpecificUnitTests, createStep_severalUser) {
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2OpStatusImpl os;

    U2DataId masterObjId = ModSQLiteSpecificTestData::createObject(os); CHECK_NO_ERROR(os);
    U2SingleModStep singleStep1 = ModSQLiteSpecificTestData::prepareSingleStep(0, os); CHECK_NO_ERROR(os);
    U2SingleModStep singleStep2 = ModSQLiteSpecificTestData::prepareSingleStep(1, os); CHECK_NO_ERROR(os);

    bool multiStepStarted = sqliteDbi->getSQLiteModDbi()->isMultiStepStarted(masterObjId);
    bool userStepStarted = sqliteDbi->getSQLiteModDbi()->isUserStepStarted(masterObjId);
    CHECK_FALSE(multiStepStarted, "Multi step must be ended!");
    CHECK_FALSE(userStepStarted, "User step must be ended!");

    {
        U2UseCommonUserModStep useUserStep(sqliteDbi, masterObjId, os); CHECK_NO_ERROR(os);
        {
            U2UseCommonMultiModStep useMultiStep1(sqliteDbi, masterObjId, os); CHECK_NO_ERROR(os);
            sqliteDbi->getSQLiteModDbi()->createModStep(masterObjId, singleStep1, os); CHECK_NO_ERROR(os);

            multiStepStarted = sqliteDbi->getSQLiteModDbi()->isMultiStepStarted(masterObjId);
            userStepStarted = sqliteDbi->getSQLiteModDbi()->isUserStepStarted(masterObjId);
            CHECK_TRUE(multiStepStarted, "Multi step must be ended!");
            CHECK_TRUE(userStepStarted, "User step must be ended!");
        }
        multiStepStarted = sqliteDbi->getSQLiteModDbi()->isMultiStepStarted(masterObjId);
        userStepStarted = sqliteDbi->getSQLiteModDbi()->isUserStepStarted(masterObjId);
        CHECK_FALSE(multiStepStarted, "Multi step must be ended!");
        CHECK_TRUE(userStepStarted, "User step must be ended!");
    }
    multiStepStarted = sqliteDbi->getSQLiteModDbi()->isMultiStepStarted(masterObjId);
    userStepStarted = sqliteDbi->getSQLiteModDbi()->isUserStepStarted(masterObjId);
    CHECK_FALSE(multiStepStarted, "Multi step must be ended!");
    CHECK_FALSE(userStepStarted, "User step must be ended!");
    {
        U2UseCommonUserModStep useUserStep(sqliteDbi, masterObjId, os); CHECK_NO_ERROR(os);
        {
            U2UseCommonMultiModStep useMultiStep2(sqliteDbi, masterObjId, os); CHECK_NO_ERROR(os);
            sqliteDbi->getSQLiteModDbi()->createModStep(masterObjId, singleStep2, os); CHECK_NO_ERROR(os);
        }
    }

    QList<U2SingleModStep> single;
    QList<U2MultiModStep4Test> multi;
    QList<U2UserModStep4Test> user;
    ModSQLiteSpecificTestData::getAllSteps(single, multi, user, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(2, single.count(), "single steps num");
    CHECK_EQUAL(2, multi.count(), "multi steps num");
    CHECK_EQUAL(2, user.count(), "user steps num");

    U2DataId userActualMasterObjId = user[0].masterObjId;

    CHECK_EQUAL(multi[0].id, single[0].multiStepId, "multi step id of single1");
    CHECK_EQUAL(user[0].id, multi[0].userStepId, "user step id of multi1");
    CHECK_EQUAL(masterObjId, user[0].masterObjId, "master object id");

    CHECK_EQUAL(multi[1].id, single[1].multiStepId, "multi step id of single2");
    CHECK_EQUAL(user[1].id, multi[1].userStepId, "user step id of multi2");
    CHECK_EQUAL(masterObjId, user[1].masterObjId, "master object id");

    multiStepStarted = sqliteDbi->getSQLiteModDbi()->isMultiStepStarted(userActualMasterObjId);
    userStepStarted = sqliteDbi->getSQLiteModDbi()->isUserStepStarted(userActualMasterObjId);
    CHECK_FALSE(multiStepStarted, "Multi step must be ended!");
    CHECK_FALSE(userStepStarted, "User step must be ended!");
}

IMPLEMENT_MOD_TEST(ModDbiSQLiteSpecificUnitTests, createStep_separateThread) {
    class TestThread : public QThread {
        U2Dbi *dbi;
        const U2DataId &objId;
        U2OpStatus &os;
    public:
        TestThread(U2Dbi *_dbi, const U2DataId &_objId, U2OpStatus &_os)
            : dbi(_dbi), objId(_objId), os(_os) {}
        void run() {
            U2UseCommonUserModStep(dbi, objId, os);
        }
    };

    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2OpStatusImpl os;
    U2DataId masterObjId = ModSQLiteSpecificTestData::createObject(os);
    CHECK_NO_ERROR(os);

    TestThread t(sqliteDbi, masterObjId, os);
    t.start();
    bool finished = t.wait();
    CHECK_TRUE(finished, "The thread is not finished");
    CHECK_TRUE(os.hasError(), "No error");
}

IMPLEMENT_MOD_TEST(ModDbiSQLiteSpecificUnitTests, createStep_emptyUser) {
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2OpStatusImpl os;
    U2DataId masterObjId = ModSQLiteSpecificTestData::createObject(os); CHECK_NO_ERROR(os);

    QList<U2SingleModStep> actualSingleSteps;
    QList<U2MultiModStep4Test> actualMultiSteps;
    QList<U2UserModStep4Test> actualUserSteps;

    {
        U2UseCommonUserModStep userModStep(sqliteDbi, masterObjId, os); CHECK_NO_ERROR(os);
        Q_UNUSED(userModStep);
        ModSQLiteSpecificTestData::getAllSteps(actualSingleSteps, actualMultiSteps, actualUserSteps, os); CHECK_NO_ERROR(os);

        CHECK_EQUAL(1, actualUserSteps.count(), "user steps count");
        CHECK_EQUAL(0, actualMultiSteps.count(), "multi steps count");
        CHECK_EQUAL(0, actualSingleSteps.count(), "single steps count");
    }

    ModSQLiteSpecificTestData::getAllSteps(actualSingleSteps, actualMultiSteps, actualUserSteps, os); CHECK_NO_ERROR(os);

    CHECK_EQUAL(0, actualUserSteps.count(), "user steps count");
    CHECK_EQUAL(0, actualMultiSteps.count(), "multi steps count");
    CHECK_EQUAL(0, actualSingleSteps.count(), "single steps count");
}

IMPLEMENT_MOD_TEST(ModDbiSQLiteSpecificUnitTests, createStep_emptyMultiAutoUser) {
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2OpStatusImpl os;
    U2DataId masterObjId = ModSQLiteSpecificTestData::createObject(os); CHECK_NO_ERROR(os);

    QList<U2SingleModStep> actualSingleSteps;
    QList<U2MultiModStep4Test> actualMultiSteps;
    QList<U2UserModStep4Test> actualUserSteps;

    {
        U2UseCommonMultiModStep multiModStep(sqliteDbi, masterObjId, os); CHECK_NO_ERROR(os);
        Q_UNUSED(multiModStep);
        ModSQLiteSpecificTestData::getAllSteps(actualSingleSteps, actualMultiSteps, actualUserSteps, os); CHECK_NO_ERROR(os);

        CHECK_EQUAL(1, actualUserSteps.count(), "user steps count");
        CHECK_EQUAL(1, actualMultiSteps.count(), "multi steps count");
        CHECK_EQUAL(0, actualSingleSteps.count(), "single steps count");
    }

    ModSQLiteSpecificTestData::getAllSteps(actualSingleSteps, actualMultiSteps, actualUserSteps, os); CHECK_NO_ERROR(os);

    CHECK_EQUAL(1, actualUserSteps.count(), "user steps count");
    CHECK_EQUAL(1, actualMultiSteps.count(), "multi steps count");
    CHECK_EQUAL(0, actualSingleSteps.count(), "single steps count");
}

IMPLEMENT_MOD_TEST(ModDbiSQLiteSpecificUnitTests, createStep_emptyMultiManUser) {
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2OpStatusImpl os;
    U2DataId masterObjId = ModSQLiteSpecificTestData::createObject(os); CHECK_NO_ERROR(os);

    QList<U2SingleModStep> actualSingleSteps;
    QList<U2MultiModStep4Test> actualMultiSteps;
    QList<U2UserModStep4Test> actualUserSteps;

    {
        U2UseCommonUserModStep userModStep(sqliteDbi, masterObjId, os); CHECK_NO_ERROR(os);
        Q_UNUSED(userModStep);
        U2UseCommonMultiModStep multiModStep(sqliteDbi, masterObjId, os); CHECK_NO_ERROR(os);
        Q_UNUSED(multiModStep);
        ModSQLiteSpecificTestData::getAllSteps(actualSingleSteps, actualMultiSteps, actualUserSteps, os); CHECK_NO_ERROR(os);

        CHECK_EQUAL(1, actualUserSteps.count(), "user steps count");
        CHECK_EQUAL(1, actualMultiSteps.count(), "multi steps count");
        CHECK_EQUAL(0, actualSingleSteps.count(), "single steps count");
    }

    ModSQLiteSpecificTestData::getAllSteps(actualSingleSteps, actualMultiSteps, actualUserSteps, os); CHECK_NO_ERROR(os);

    CHECK_EQUAL(1, actualUserSteps.count(), "user steps count");
    CHECK_EQUAL(1, actualMultiSteps.count(), "multi steps count");
    CHECK_EQUAL(0, actualSingleSteps.count(), "single steps count");
}

IMPLEMENT_MOD_TEST(ModDbiSQLiteSpecificUnitTests, userSteps_oneAct_auto) {
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2OpStatusImpl os;

    U2DataId msaId = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    qint64 baseVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Action
    sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId, BaseDNAAlphabetIds::NUCL_DNA_EXTENDED(), os);
    CHECK_NO_ERROR(os);

    QList<U2SingleModStep> actualSingleSteps;
    QList<U2MultiModStep4Test> actualMultiSteps;
    QList<U2UserModStep4Test> actualUserSteps;
    ModSQLiteSpecificTestData::getAllSteps(actualSingleSteps, actualMultiSteps, actualUserSteps, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(1, actualUserSteps.count(), "user steps count");
    CHECK_EQUAL(baseVersion, actualUserSteps[0].version, "user step version");
}

IMPLEMENT_MOD_TEST(ModDbiSQLiteSpecificUnitTests, userSteps_severalAct_auto) {
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2OpStatusImpl os;

    U2DataId msaId = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    qint64 baseVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Three actions
    sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId, BaseDNAAlphabetIds::NUCL_DNA_EXTENDED(), os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId, BaseDNAAlphabetIds::NUCL_DNA_EXTENDED(), os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId, BaseDNAAlphabetIds::NUCL_DNA_EXTENDED(), os);
    CHECK_NO_ERROR(os);

    QList<U2SingleModStep> actualSingleSteps;
    QList<U2MultiModStep4Test> actualMultiSteps;
    QList<U2UserModStep4Test> actualUserSteps;
    ModSQLiteSpecificTestData::getAllSteps(actualSingleSteps, actualMultiSteps, actualUserSteps, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(3, actualUserSteps.count(), "user steps count");
    CHECK_EQUAL(baseVersion, actualUserSteps[0].version, "user step version");
    CHECK_EQUAL(baseVersion + 1, actualUserSteps[1].version, "user step version");
    CHECK_EQUAL(baseVersion + 2, actualUserSteps[2].version, "user step version");
}

IMPLEMENT_MOD_TEST(ModDbiSQLiteSpecificUnitTests, userSteps_severalActUndo_auto) {
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2OpStatusImpl os;

    U2DataId msaId = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    qint64 baseVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Three actions
    sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId, BaseDNAAlphabetIds::NUCL_DNA_EXTENDED(), os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId, BaseDNAAlphabetIds::NUCL_DNA_EXTENDED(), os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId, BaseDNAAlphabetIds::NUCL_DNA_EXTENDED(), os);
    CHECK_NO_ERROR(os);

    // Undo twice
    sqliteDbi->getObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    QList<U2SingleModStep> actualSingleSteps;
    QList<U2MultiModStep4Test> actualMultiSteps;
    QList<U2UserModStep4Test> actualUserSteps;
    ModSQLiteSpecificTestData::getAllSteps(actualSingleSteps, actualMultiSteps, actualUserSteps, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(3, actualUserSteps.count(), "user steps count");
    CHECK_EQUAL(baseVersion, actualUserSteps[0].version, "user step version");
    CHECK_EQUAL(baseVersion + 1, actualUserSteps[1].version, "user step version");
    CHECK_EQUAL(baseVersion + 2, actualUserSteps[2].version, "user step version");
}

IMPLEMENT_MOD_TEST(ModDbiSQLiteSpecificUnitTests, userSteps_severalActUndoRedo_auto) {
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2OpStatusImpl os;

    U2DataId msaId = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    qint64 baseVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Three actions
    sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId, BaseDNAAlphabetIds::NUCL_DNA_EXTENDED(), os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId, BaseDNAAlphabetIds::NUCL_DNA_EXTENDED(), os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId, BaseDNAAlphabetIds::NUCL_DNA_EXTENDED(), os);
    CHECK_NO_ERROR(os);

    // Undo twice
    sqliteDbi->getObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Redo once
    sqliteDbi->getObjectDbi()->redo(msaId, os);
    CHECK_NO_ERROR(os);

    QList<U2SingleModStep> actualSingleSteps;
    QList<U2MultiModStep4Test> actualMultiSteps;
    QList<U2UserModStep4Test> actualUserSteps;
    ModSQLiteSpecificTestData::getAllSteps(actualSingleSteps, actualMultiSteps, actualUserSteps, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(3, actualUserSteps.count(), "user steps count");
    CHECK_EQUAL(baseVersion, actualUserSteps[0].version, "user step version");
    CHECK_EQUAL(baseVersion + 1, actualUserSteps[1].version, "user step version");
    CHECK_EQUAL(baseVersion + 2, actualUserSteps[2].version, "user step version");
}

IMPLEMENT_MOD_TEST(ModDbiSQLiteSpecificUnitTests, userSteps_severalActUndoRedoAct_auto) {
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2OpStatusImpl os;

    U2DataId msaId = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    qint64 baseVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Three actions
    sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId, BaseDNAAlphabetIds::NUCL_DNA_EXTENDED(), os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId, BaseDNAAlphabetIds::NUCL_DNA_EXTENDED(), os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId, BaseDNAAlphabetIds::NUCL_DNA_EXTENDED(), os);
    CHECK_NO_ERROR(os);

    // Undo thrice
    sqliteDbi->getObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Redo once
    sqliteDbi->getObjectDbi()->redo(msaId, os);
    CHECK_NO_ERROR(os);

    // One action
    sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId, BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), os);
    CHECK_NO_ERROR(os);

    QList<U2SingleModStep> actualSingleSteps;
    QList<U2MultiModStep4Test> actualMultiSteps;
    QList<U2UserModStep4Test> actualUserSteps;
    ModSQLiteSpecificTestData::getAllSteps(actualSingleSteps, actualMultiSteps, actualUserSteps, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(2, actualUserSteps.count(), "user steps count");
    CHECK_EQUAL(baseVersion, actualUserSteps[0].version, "user step version");
    CHECK_EQUAL(baseVersion + 1, actualUserSteps[1].version, "user step version");
}

IMPLEMENT_MOD_TEST(ModDbiSQLiteSpecificUnitTests, userSteps_oneAct_man) {
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2OpStatusImpl os;

    U2DataId msaId = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    qint64 baseVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    // Create user step
    {
        U2UseCommonUserModStep userModStep(sqliteDbi, msaId, os);
        CHECK_NO_ERROR(os);
        Q_UNUSED(userModStep);

        // Action
        sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId, BaseDNAAlphabetIds::NUCL_DNA_EXTENDED(), os);
        CHECK_NO_ERROR(os);
        sqliteDbi->getMsaDbi()->updateMsaName(msaId, "New name", os);
        CHECK_NO_ERROR(os);
    }

    QList<U2SingleModStep> actualSingleSteps;
    QList<U2MultiModStep4Test> actualMultiSteps;
    QList<U2UserModStep4Test> actualUserSteps;
    ModSQLiteSpecificTestData::getAllSteps(actualSingleSteps, actualMultiSteps, actualUserSteps, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(1, actualUserSteps.count(), "user steps count");
    CHECK_EQUAL(baseVersion, actualUserSteps[0].version, "user step version");
}

IMPLEMENT_MOD_TEST(ModDbiSQLiteSpecificUnitTests, userSteps_severalAct_man) {
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2OpStatusImpl os;

    U2DataId msaId = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    qint64 baseVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    for (int i = 0; i < 3; ++i) {
        // Create user step
        U2UseCommonUserModStep userModStep(sqliteDbi, msaId, os);
        CHECK_NO_ERROR(os);
        Q_UNUSED(userModStep);

        // Some actions
        sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId, BaseDNAAlphabetIds::NUCL_DNA_EXTENDED(), os);
        CHECK_NO_ERROR(os);
        sqliteDbi->getMsaDbi()->updateMsaName(msaId, QString("New name ") + QString::number(i), os);
        CHECK_NO_ERROR(os);
    }

    QList<U2SingleModStep> actualSingleSteps;
    QList<U2MultiModStep4Test> actualMultiSteps;
    QList<U2UserModStep4Test> actualUserSteps;
    ModSQLiteSpecificTestData::getAllSteps(actualSingleSteps, actualMultiSteps, actualUserSteps, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(3, actualUserSteps.count(), "user steps count");
    CHECK_EQUAL(baseVersion, actualUserSteps[0].version, "user step version");
    CHECK_EQUAL(baseVersion + 2, actualUserSteps[1].version, "user step version");
    CHECK_EQUAL(baseVersion + 4, actualUserSteps[2].version, "user step version");
}

IMPLEMENT_MOD_TEST(ModDbiSQLiteSpecificUnitTests, userSteps_severalActUndo_man) {
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2OpStatusImpl os;

    U2DataId msaId = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    qint64 baseVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    for (int i = 0; i < 3; ++i) {
        // Create user step
        U2UseCommonUserModStep userModStep(sqliteDbi, msaId, os);
        CHECK_NO_ERROR(os);
        Q_UNUSED(userModStep);

        // Some actions
        sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId, BaseDNAAlphabetIds::NUCL_DNA_EXTENDED(), os);
        CHECK_NO_ERROR(os);
        sqliteDbi->getMsaDbi()->updateMsaName(msaId, QString("New name ") + QString::number(i), os);
        CHECK_NO_ERROR(os);
    }

    // Undo twice
    sqliteDbi->getObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    QList<U2SingleModStep> actualSingleSteps;
    QList<U2MultiModStep4Test> actualMultiSteps;
    QList<U2UserModStep4Test> actualUserSteps;
    ModSQLiteSpecificTestData::getAllSteps(actualSingleSteps, actualMultiSteps, actualUserSteps, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(3, actualUserSteps.count(), "user steps count");
    CHECK_EQUAL(baseVersion, actualUserSteps[0].version, "user step version");
    CHECK_EQUAL(baseVersion + 2, actualUserSteps[1].version, "user step version");
    CHECK_EQUAL(baseVersion + 4, actualUserSteps[2].version, "user step version");
}

IMPLEMENT_MOD_TEST(ModDbiSQLiteSpecificUnitTests, userSteps_severalActUndoRedo_man) {
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2OpStatusImpl os;

    U2DataId msaId = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    qint64 baseVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    for (int i = 0; i < 3; ++i) {
        // Create user step
        U2UseCommonUserModStep userModStep(sqliteDbi, msaId, os);
        CHECK_NO_ERROR(os);
        Q_UNUSED(userModStep);

        // Some actions
        sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId, BaseDNAAlphabetIds::NUCL_DNA_EXTENDED(), os);
        CHECK_NO_ERROR(os);
        sqliteDbi->getMsaDbi()->updateMsaName(msaId, QString("New name ") + QString::number(i), os);
        CHECK_NO_ERROR(os);
    }

    // Undo twice
    sqliteDbi->getObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Redo once
    sqliteDbi->getObjectDbi()->redo(msaId, os);
    CHECK_NO_ERROR(os);

    QList<U2SingleModStep> actualSingleSteps;
    QList<U2MultiModStep4Test> actualMultiSteps;
    QList<U2UserModStep4Test> actualUserSteps;
    ModSQLiteSpecificTestData::getAllSteps(actualSingleSteps, actualMultiSteps, actualUserSteps, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(3, actualUserSteps.count(), "user steps count");
    CHECK_EQUAL(baseVersion, actualUserSteps[0].version, "user step version");
    CHECK_EQUAL(baseVersion + 2, actualUserSteps[1].version, "user step version");
    CHECK_EQUAL(baseVersion + 4, actualUserSteps[2].version, "user step version");
}

IMPLEMENT_MOD_TEST(ModDbiSQLiteSpecificUnitTests, userSteps_severalActUndoRedoAct_man) {
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2OpStatusImpl os;

    U2DataId msaId = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    qint64 baseVersion = sqliteDbi->getObjectDbi()->getObjectVersion(msaId, os);
    CHECK_NO_ERROR(os);

    for (int i = 0; i < 3; ++i) {
        // Create user step
        U2UseCommonUserModStep userModStep(sqliteDbi, msaId, os);
        CHECK_NO_ERROR(os);
        Q_UNUSED(userModStep);

        // Some actions
        sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId, BaseDNAAlphabetIds::NUCL_DNA_EXTENDED(), os);
        CHECK_NO_ERROR(os);
        sqliteDbi->getMsaDbi()->updateMsaName(msaId, QString("New name ") + QString::number(i), os);
        CHECK_NO_ERROR(os);
    }

    // Undo thrice
    sqliteDbi->getObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->undo(msaId, os);
    CHECK_NO_ERROR(os);

    // Redo once
    sqliteDbi->getObjectDbi()->redo(msaId, os);
    CHECK_NO_ERROR(os);

    // One action
    {
        // Create user step
        U2UseCommonUserModStep userModStep(sqliteDbi, msaId, os);
        CHECK_NO_ERROR(os);
        Q_UNUSED(userModStep);

        // Some actions
        sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId, BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), os);
        CHECK_NO_ERROR(os);
        sqliteDbi->getMsaDbi()->updateMsaName(msaId, QString("The newest name"), os);
        CHECK_NO_ERROR(os);
    }

    QList<U2SingleModStep> actualSingleSteps;
    QList<U2MultiModStep4Test> actualMultiSteps;
    QList<U2UserModStep4Test> actualUserSteps;
    ModSQLiteSpecificTestData::getAllSteps(actualSingleSteps, actualMultiSteps, actualUserSteps, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(2, actualUserSteps.count(), "user steps count");
    CHECK_EQUAL(baseVersion, actualUserSteps[0].version, "user step version");
    CHECK_EQUAL(baseVersion + 2, actualUserSteps[1].version, "user step version");
}

IMPLEMENT_MOD_TEST(ModDbiSQLiteSpecificUnitTests, userSteps_oneAct_diffObj) {
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2OpStatusImpl os;

    U2DataId msaId1 = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    U2DataId msaId2 = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    qint64 baseVersion1 = sqliteDbi->getObjectDbi()->getObjectVersion(msaId1, os);
    CHECK_NO_ERROR(os);
    qint64 baseVersion2 = sqliteDbi->getObjectDbi()->getObjectVersion(msaId2, os);
    CHECK_NO_ERROR(os);

    // Create user step, msaId1 - master
    {
        U2UseCommonUserModStep userModStep1(sqliteDbi, msaId1, os);
        CHECK_NO_ERROR(os);
        U2UseCommonUserModStep userModStep2(sqliteDbi, msaId2, os);
        CHECK_NO_ERROR(os);
        Q_UNUSED(userModStep1);
        Q_UNUSED(userModStep2);

        // Action for msaId1
        sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId1, BaseDNAAlphabetIds::NUCL_DNA_EXTENDED(), os);
        CHECK_NO_ERROR(os);
        sqliteDbi->getMsaDbi()->updateMsaName(msaId1, "New name", os);
        CHECK_NO_ERROR(os);

        // Action for msaId2
        sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId2, BaseDNAAlphabetIds::NUCL_DNA_EXTENDED(), os);
        CHECK_NO_ERROR(os);
        sqliteDbi->getMsaDbi()->updateMsaName(msaId2, "New name", os);
        CHECK_NO_ERROR(os);
    }

    qint64 finalVersion1 = sqliteDbi->getObjectDbi()->getObjectVersion(msaId1, os);
    CHECK_NO_ERROR(os);
    qint64 finalVersion2 = sqliteDbi->getObjectDbi()->getObjectVersion(msaId2, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseVersion1 - baseVersion2, finalVersion1 - finalVersion2, "objects versions");

    QList<U2SingleModStep> actualSingleSteps;
    QList<U2MultiModStep4Test> actualMultiSteps;
    QList<U2UserModStep4Test> actualUserSteps;
    ModSQLiteSpecificTestData::getAllSteps(actualSingleSteps, actualMultiSteps, actualUserSteps, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(2, actualUserSteps.count(), "user steps count");
    CHECK_EQUAL(baseVersion1, actualUserSteps[0].version, "user step version");
    CHECK_EQUAL(baseVersion1, actualUserSteps[1].version, "user step version");
}

IMPLEMENT_MOD_TEST(ModDbiSQLiteSpecificUnitTests, userSteps_severalAct_diffObj) {
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2OpStatusImpl os;

    U2DataId msaId1 = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    U2DataId msaId2 = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    qint64 baseVersion1 = sqliteDbi->getObjectDbi()->getObjectVersion(msaId1, os);
    CHECK_NO_ERROR(os);
    qint64 baseVersion2 = sqliteDbi->getObjectDbi()->getObjectVersion(msaId2, os);
    CHECK_NO_ERROR(os);

    for (int i = 0; i < 3; ++i) {
        // Create user step, msaId1 - master
        U2UseCommonUserModStep userModStep1(sqliteDbi, msaId1, os);
        CHECK_NO_ERROR(os);
        U2UseCommonUserModStep userModStep2(sqliteDbi, msaId2, os);
        CHECK_NO_ERROR(os);
        Q_UNUSED(userModStep1);
        Q_UNUSED(userModStep2);

        // Action for msaId1
        sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId1, BaseDNAAlphabetIds::NUCL_DNA_EXTENDED(), os);
        CHECK_NO_ERROR(os);
        sqliteDbi->getMsaDbi()->updateMsaName(msaId1, QString("New name ") + QString::number(i), os);
        CHECK_NO_ERROR(os);

        // Action for msaId2
        sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId2, BaseDNAAlphabetIds::NUCL_DNA_EXTENDED(), os);
        CHECK_NO_ERROR(os);
        sqliteDbi->getMsaDbi()->updateMsaName(msaId2, QString("New name ") + QString::number(i), os);
        CHECK_NO_ERROR(os);
    }

    qint64 finalVersion1 = sqliteDbi->getObjectDbi()->getObjectVersion(msaId1, os);
    CHECK_NO_ERROR(os);
    qint64 finalVersion2 = sqliteDbi->getObjectDbi()->getObjectVersion(msaId2, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseVersion1 - baseVersion2, finalVersion1 - finalVersion2, "objects versions");

    QList<U2SingleModStep> actualSingleSteps;
    QList<U2MultiModStep4Test> actualMultiSteps;
    QList<U2UserModStep4Test> actualUserSteps;
    ModSQLiteSpecificTestData::getAllSteps(actualSingleSteps, actualMultiSteps, actualUserSteps, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(6, actualUserSteps.count(), "user steps count");
    CHECK_EQUAL(baseVersion1, actualUserSteps[0].version, "user step version");
    CHECK_EQUAL(baseVersion1, actualUserSteps[1].version, "user step version");
    CHECK_EQUAL(baseVersion1 + 2, actualUserSteps[2].version, "user step version");
    CHECK_EQUAL(baseVersion1 + 2, actualUserSteps[3].version, "user step version");
    CHECK_EQUAL(baseVersion1 + 4, actualUserSteps[4].version, "user step version");
    CHECK_EQUAL(baseVersion1 + 4, actualUserSteps[5].version, "user step version");
}

IMPLEMENT_MOD_TEST(ModDbiSQLiteSpecificUnitTests, userSteps_severalActUndo_diffObj) {
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2OpStatusImpl os;

    U2DataId msaId1 = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    U2DataId msaId2 = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    qint64 baseVersion1 = sqliteDbi->getObjectDbi()->getObjectVersion(msaId1, os);
    CHECK_NO_ERROR(os);
    qint64 baseVersion2 = sqliteDbi->getObjectDbi()->getObjectVersion(msaId2, os);
    CHECK_NO_ERROR(os);

    for (int i = 0; i < 3; ++i) {
        // Create user step, msaId1 - master
        U2UseCommonUserModStep userModStep1(sqliteDbi, msaId1, os);
        CHECK_NO_ERROR(os);
        U2UseCommonUserModStep userModStep2(sqliteDbi, msaId2, os);
        CHECK_NO_ERROR(os);
        Q_UNUSED(userModStep1);
        Q_UNUSED(userModStep2);

        // Action for msaId1
        sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId1, BaseDNAAlphabetIds::NUCL_DNA_EXTENDED(), os);
        CHECK_NO_ERROR(os);
        sqliteDbi->getMsaDbi()->updateMsaName(msaId1, QString("New name ") + QString::number(i), os);
        CHECK_NO_ERROR(os);

        // Action for msaId2
        sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId2, BaseDNAAlphabetIds::NUCL_DNA_EXTENDED(), os);
        CHECK_NO_ERROR(os);
        sqliteDbi->getMsaDbi()->updateMsaName(msaId2, QString("New name ") + QString::number(i), os);
        CHECK_NO_ERROR(os);
    }

    qint64 currentVersion1 = sqliteDbi->getObjectDbi()->getObjectVersion(msaId1, os);
    CHECK_NO_ERROR(os);
    qint64 currentVersion2 = sqliteDbi->getObjectDbi()->getObjectVersion(msaId2, os);
    CHECK_NO_ERROR(os)
    CHECK_EQUAL(baseVersion1 - baseVersion2, currentVersion1 - currentVersion2, "objects versions");


    // Undo twice (for master object)
    sqliteDbi->getObjectDbi()->undo(msaId1, os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->undo(msaId1, os);
    CHECK_NO_ERROR(os);

    currentVersion1 = sqliteDbi->getObjectDbi()->getObjectVersion(msaId1, os);
    CHECK_NO_ERROR(os);
    currentVersion2 = sqliteDbi->getObjectDbi()->getObjectVersion(msaId2, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(currentVersion2 - currentVersion1, 4, "objects versions");

    QList<U2SingleModStep> actualSingleSteps;
    QList<U2MultiModStep4Test> actualMultiSteps;
    QList<U2UserModStep4Test> actualUserSteps;
    ModSQLiteSpecificTestData::getAllSteps(actualSingleSteps, actualMultiSteps, actualUserSteps, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(6, actualUserSteps.count(), "user steps count");
    CHECK_EQUAL(baseVersion1, actualUserSteps[0].version, "user step version");
    CHECK_EQUAL(baseVersion1, actualUserSteps[1].version, "user step version");
    CHECK_EQUAL(baseVersion1 + 2, actualUserSteps[2].version, "user step version");
    CHECK_EQUAL(baseVersion1 + 2, actualUserSteps[3].version, "user step version");
    CHECK_EQUAL(baseVersion1 + 4, actualUserSteps[4].version, "user step version");
    CHECK_EQUAL(baseVersion1 + 4, actualUserSteps[5].version, "user step version");
}

IMPLEMENT_MOD_TEST(ModDbiSQLiteSpecificUnitTests, userSteps_severalActUndoRedo_diffObj) {
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2OpStatusImpl os;

    U2DataId msaId1 = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    U2DataId msaId2 = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    qint64 baseVersion1 = sqliteDbi->getObjectDbi()->getObjectVersion(msaId1, os);
    CHECK_NO_ERROR(os);
    qint64 baseVersion2 = sqliteDbi->getObjectDbi()->getObjectVersion(msaId2, os);
    CHECK_NO_ERROR(os);

    for (int i = 0; i < 3; ++i) {
        // Create user step, msaId1 - master
        U2UseCommonUserModStep userModStep1(sqliteDbi, msaId1, os);
        CHECK_NO_ERROR(os);
        U2UseCommonUserModStep userModStep2(sqliteDbi, msaId2, os);
        CHECK_NO_ERROR(os);
        Q_UNUSED(userModStep1);
        Q_UNUSED(userModStep2);

        // Action for msaId1
        sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId1, BaseDNAAlphabetIds::NUCL_DNA_EXTENDED(), os);
        CHECK_NO_ERROR(os);
        sqliteDbi->getMsaDbi()->updateMsaName(msaId1, QString("New name ") + QString::number(i), os);
        CHECK_NO_ERROR(os);

        // Action for msaId2
        sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId2, BaseDNAAlphabetIds::NUCL_DNA_EXTENDED(), os);
        CHECK_NO_ERROR(os);
        sqliteDbi->getMsaDbi()->updateMsaName(msaId2, QString("New name ") + QString::number(i), os);
        CHECK_NO_ERROR(os);
    }

    qint64 currentVersion1 = sqliteDbi->getObjectDbi()->getObjectVersion(msaId1, os);
    CHECK_NO_ERROR(os);
    qint64 currentVersion2 = sqliteDbi->getObjectDbi()->getObjectVersion(msaId2, os);
    CHECK_NO_ERROR(os)
    CHECK_EQUAL(baseVersion1 - baseVersion2, currentVersion1 - currentVersion2, "objects versions");

    // Undo thrice (for master object)
    sqliteDbi->getObjectDbi()->undo(msaId1, os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->undo(msaId1, os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->undo(msaId1, os);
    CHECK_NO_ERROR(os);

    // Redo once (for master object)
    sqliteDbi->getObjectDbi()->redo(msaId1, os);
    CHECK_NO_ERROR(os);

    currentVersion1 = sqliteDbi->getObjectDbi()->getObjectVersion(msaId1, os);
    CHECK_NO_ERROR(os);
    currentVersion2 = sqliteDbi->getObjectDbi()->getObjectVersion(msaId2, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(currentVersion2 - currentVersion1, 4, "objects versions");

    QList<U2SingleModStep> actualSingleSteps;
    QList<U2MultiModStep4Test> actualMultiSteps;
    QList<U2UserModStep4Test> actualUserSteps;
    ModSQLiteSpecificTestData::getAllSteps(actualSingleSteps, actualMultiSteps, actualUserSteps, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(6, actualUserSteps.count(), "user steps count");
    CHECK_EQUAL(baseVersion1, actualUserSteps[0].version, "user step version");
    CHECK_EQUAL(baseVersion1, actualUserSteps[1].version, "user step version");
    CHECK_EQUAL(baseVersion1 + 2, actualUserSteps[2].version, "user step version");
    CHECK_EQUAL(baseVersion1 + 2, actualUserSteps[3].version, "user step version");
    CHECK_EQUAL(baseVersion1 + 4, actualUserSteps[4].version, "user step version");
    CHECK_EQUAL(baseVersion1 + 4, actualUserSteps[5].version, "user step version");
}

IMPLEMENT_MOD_TEST(ModDbiSQLiteSpecificUnitTests, userSteps_severalActUndoRedoAct_diffObj) {
    SQLiteDbi *sqliteDbi = ModSQLiteSpecificTestData::getSQLiteDbi();
    U2OpStatusImpl os;

    U2DataId msaId1 = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    U2DataId msaId2 = ModSQLiteSpecificTestData::createTestMsa(true, os);
    CHECK_NO_ERROR(os);
    qint64 baseVersion1 = sqliteDbi->getObjectDbi()->getObjectVersion(msaId1, os);
    CHECK_NO_ERROR(os);

    for (int i = 0; i < 3; ++i) {
        // Create user step, msaId1 - master
        U2UseCommonUserModStep userModStep1(sqliteDbi, msaId1, os);
        CHECK_NO_ERROR(os);
        U2UseCommonUserModStep userModStep2(sqliteDbi, msaId2, os);
        CHECK_NO_ERROR(os);
        Q_UNUSED(userModStep1);
        Q_UNUSED(userModStep2);

        // Action for msaId1
        sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId1, BaseDNAAlphabetIds::NUCL_DNA_EXTENDED(), os);
        CHECK_NO_ERROR(os);
        sqliteDbi->getMsaDbi()->updateMsaName(msaId1, QString("New name ") + QString::number(i), os);
        CHECK_NO_ERROR(os);

        // Action for msaId2
        sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId2, BaseDNAAlphabetIds::NUCL_DNA_EXTENDED(), os);
        CHECK_NO_ERROR(os);
        sqliteDbi->getMsaDbi()->updateMsaName(msaId2, QString("New name ") + QString::number(i), os);
        CHECK_NO_ERROR(os);
    }

    // Undo thrice (for master object)
    sqliteDbi->getObjectDbi()->undo(msaId1, os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->undo(msaId1, os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->undo(msaId1, os);
    CHECK_NO_ERROR(os);

    // Redo once (for master object)
    sqliteDbi->getObjectDbi()->redo(msaId1, os);
    CHECK_NO_ERROR(os);

    // One action
    {
        // Create user step, msaId1 - master
        U2UseCommonUserModStep userModStep1(sqliteDbi, msaId1, os);
        CHECK_NO_ERROR(os);
        U2UseCommonUserModStep userModStep2(sqliteDbi, msaId2, os);
        CHECK_NO_ERROR(os);
        Q_UNUSED(userModStep1);
        Q_UNUSED(userModStep2);

        // Action for msaId1
        sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId1, BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), os);
        CHECK_NO_ERROR(os);
        sqliteDbi->getMsaDbi()->updateMsaName(msaId1, QString("The newest name 1"), os);
        CHECK_NO_ERROR(os);

        // Action for msaId2
        sqliteDbi->getMsaDbi()->updateMsaAlphabet(msaId2, BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), os);
        CHECK_NO_ERROR(os);
        sqliteDbi->getMsaDbi()->updateMsaName(msaId2, QString("The newest name 2"), os);
        CHECK_NO_ERROR(os);
    }

    qint64 finalVersion1 = sqliteDbi->getObjectDbi()->getObjectVersion(msaId1, os);
    CHECK_NO_ERROR(os);
    qint64 finalVersion2 = sqliteDbi->getObjectDbi()->getObjectVersion(msaId2, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(finalVersion2 - finalVersion1, 4, "objects versions");

    QList<U2SingleModStep> actualSingleSteps;
    QList<U2MultiModStep4Test> actualMultiSteps;
    QList<U2UserModStep4Test> actualUserSteps;
    ModSQLiteSpecificTestData::getAllSteps(actualSingleSteps, actualMultiSteps, actualUserSteps, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(6, actualUserSteps.count(), "user steps count");
    CHECK_EQUAL(baseVersion1, actualUserSteps[0].version, "user step version");
    CHECK_EQUAL(baseVersion1, actualUserSteps[1].version, "user step version");
    CHECK_EQUAL(baseVersion1 + 2, actualUserSteps[2].version, "user step version");
    CHECK_EQUAL(baseVersion1 + 4, actualUserSteps[3].version, "user step version");
    CHECK_EQUAL(baseVersion1 + 2, actualUserSteps[4].version, "user step version");
    CHECK_EQUAL(baseVersion1 + 6, actualUserSteps[5].version, "user step version");
}

} // namespace
