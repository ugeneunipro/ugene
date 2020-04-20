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

#include "SequenceDbiSQLiteSpecificUnitTests.h"

#include <U2Core/DNAAlphabet.h>
#include <U2Core/U2SequenceDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2Mod.h>
#include <U2Core/U2SequenceUtils.h>

#include <U2Formats/SQLiteDbi.h>
#include <U2Formats/SQLiteObjectDbi.h>

namespace U2 {

TestDbiProvider SequenceSQLiteSpecificTestData::dbiProvider = TestDbiProvider();
const QString& SequenceSQLiteSpecificTestData::SQLITE_SEQUENCE_DB_URL("sqlite-sequence-dbi.ugenedb");
SQLiteDbi* SequenceSQLiteSpecificTestData::sqliteDbi = NULL;

const QString SequenceSQLiteSpecificTestData::TEST_SEQUENCE_NAME = "Test sequence";

void SequenceSQLiteSpecificTestData::init() {
    SAFE_POINT(NULL == sqliteDbi, "sqliteDbi has already been initialized!", );

    // Get URL
    bool ok = dbiProvider.init(SQLITE_SEQUENCE_DB_URL, false);
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

    // Get sequences IDs
    QList<U2DataId> ids = sqliteDbi->getObjectDbi()->getObjects(U2Type::Sequence, 0,
        U2DbiOptions::U2_DBI_NO_LIMIT, os);
    SAFE_POINT_OP(os,);
}

void SequenceSQLiteSpecificTestData::shutdown() {
    if (NULL != sqliteDbi) {
        U2OpStatusImpl os;
        sqliteDbi->shutdown(os);
        SAFE_POINT_OP(os, );
        delete sqliteDbi;
        sqliteDbi = NULL;
    }
}

SQLiteDbi* SequenceSQLiteSpecificTestData::getSQLiteDbi() {
    if (NULL == sqliteDbi) {
        init();
    }
    return sqliteDbi;
}

U2DataId SequenceSQLiteSpecificTestData::createTestSequence(bool enableModTracking, qint64 seqLength, U2OpStatus& os) {
    U2Sequence sequence;
    sequence.alphabet = BaseDNAAlphabetIds::NUCL_DNA_DEFAULT();
    sequence.visualName = TEST_SEQUENCE_NAME;
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

U2DataId SequenceSQLiteSpecificTestData::createTestSequence(bool enableModTracking, const QByteArray& seqData, U2OpStatus& os) {
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

    sqliteDbi->getSequenceDbi()->updateSequenceData(sequence.id, U2_REGION_MAX, seqData, QVariantMap(), os);
    CHECK_OP(os, "");

    return sequence.id;
}

qint64 SequenceSQLiteSpecificTestData::getModStepsNum(const U2DataId& objId, U2OpStatus& os) {
    SQLiteReadQuery qModSteps("SELECT COUNT(*) FROM SingleModStep WHERE object = ?1", sqliteDbi->getDbRef(), os);
    qModSteps.bindDataId(1, objId);
    return qModSteps.selectInt64();
}

IMPLEMENT_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_noModTrack) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = SequenceSQLiteSpecificTestData::getSQLiteDbi();

    U2Sequence sequence;
    sequence.alphabet = BaseDNAAlphabetIds::NUCL_DNA_DEFAULT();
    sequence.visualName = SequenceSQLiteSpecificTestData::TEST_SEQUENCE_NAME;
    sequence.trackModType = NoTrack;
    sqliteDbi->getSequenceDbi()->createSequenceObject(sequence, "", os);
    CHECK_NO_ERROR(os);

    // Some base values
    qint64 baseSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(sequence.id, os);
    CHECK_NO_ERROR(os);
    U2TrackModType baseSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(sequence.id, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(sequence.trackModType, baseSeqModType, "sequence track mod type");
    qint64 baseModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(sequence.id, os);
    CHECK_NO_ERROR(os);

    // Prepare test data
    QByteArray data("AAAAAAA");
    U2Region region = U2_REGION_MAX;
    QVariantMap hints;

    // Call test function
    sqliteDbi->getSequenceDbi()->updateSequenceData(sequence.id, region, data, hints, os);
    CHECK_NO_ERROR(os);

    // Verify version
    qint64 finalSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(sequence.id, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqVersion + 1, finalSeqVersion, "sequence version");

    // Verify mod type
    U2TrackModType finalSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(sequence.id, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqModType, finalSeqModType, "sequence track mod type");

    // Verify mod steps num
    qint64 finalModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(sequence.id, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseModStepsNum, finalModStepsNum, "mod steps num");

    // Verify sequence object
    U2Sequence finalSeq = sqliteDbi->getSequenceDbi()->getSequenceObject(sequence.id, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(sequence.alphabet.id, finalSeq.alphabet.id, "sequence alphabet");
    CHECK_EQUAL(sequence.visualName, finalSeq.visualName, "sequence name");

    // Verify sequence data
    QByteArray finalData = sqliteDbi->getSequenceDbi()->getSequenceData(sequence.id, U2_REGION_MAX, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(data, QString(finalData), "sequence data");
}

IMPLEMENT_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_emptyHint) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = SequenceSQLiteSpecificTestData::getSQLiteDbi();

    qint64 seqLength = 0;
    U2DataId seqId = SequenceSQLiteSpecificTestData::createTestSequence(true, seqLength, os);
    CHECK_NO_ERROR(os);

    // Some base values
    qint64 baseSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    U2TrackModType baseSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    qint64 baseModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);

    // Prepare test data
    QByteArray data("AAAAAAA");
    U2Region region = U2_REGION_MAX;
    QVariantMap hints;
    hints.insert(U2SequenceDbiHints::EMPTY_SEQUENCE, true);

    // Call test function
    sqliteDbi->getSequenceDbi()->updateSequenceData(seqId, region, data, hints, os);
    CHECK_NO_ERROR(os);

    // Verify version
    qint64 finalSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqVersion + 1, finalSeqVersion, "sequence version");

    // Verify mod type
    U2TrackModType finalSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqModType, finalSeqModType, "sequence track mod type");

    // Verify mod steps num
    qint64 finalModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseModStepsNum + 1, finalModStepsNum, "mod steps num");

    //Verify modSteps
    U2SingleModStep modStep = sqliteDbi->getModDbi()->getModStep(seqId, finalSeqVersion - 1, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(U2ModType::sequenceUpdatedData, modStep.modType, "mod type");
    CHECK_EQUAL(seqId, modStep.objectId, "object ID");
    CHECK_EQUAL(finalSeqVersion - 1, modStep.version, "modStep version");
    CHECK_EQUAL("0\t0\t\tAAAAAAA\t\"empty-sequence,true\"", QString(modStep.details), "modStep details");

    // Verify sequence data
    QByteArray finalData = sqliteDbi->getSequenceDbi()->getSequenceData(seqId, U2_REGION_MAX, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(data, QString(finalData), "sequence data");
}

IMPLEMENT_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_emptyNoHint) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = SequenceSQLiteSpecificTestData::getSQLiteDbi();

    qint64 seqLength = 0;
    U2DataId seqId = SequenceSQLiteSpecificTestData::createTestSequence(true, seqLength, os);
    CHECK_NO_ERROR(os);

    // Some base values
    qint64 baseSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    U2TrackModType baseSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    qint64 baseModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);

    // Prepare test data
    QByteArray data("AAAAAAA");
    U2Region region = U2_REGION_MAX;
    QVariantMap hints;

    // Call test function
    sqliteDbi->getSequenceDbi()->updateSequenceData(seqId, region, data, hints, os);
    CHECK_NO_ERROR(os);

    // Verify version
    qint64 finalSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqVersion + 1, finalSeqVersion, "sequence version");

    // Verify mod type
    U2TrackModType finalSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqModType, finalSeqModType, "sequence track mod type");

    // Verify mod steps num
    qint64 finalModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseModStepsNum + 1, finalModStepsNum, "mod steps num");

    //Verify modSteps
    U2SingleModStep modStep = sqliteDbi->getModDbi()->getModStep(seqId, finalSeqVersion - 1, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(U2ModType::sequenceUpdatedData, modStep.modType, "mod type");
    CHECK_EQUAL(seqId, modStep.objectId, "object ID");
    CHECK_EQUAL(finalSeqVersion - 1, modStep.version, "modStep version");
    CHECK_EQUAL("0\t0\t\tAAAAAAA\t\"\"", QString(modStep.details), "modStep details");

    // Verify sequence data
    QByteArray finalData = sqliteDbi->getSequenceDbi()->getSequenceData(seqId, U2_REGION_MAX, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(data, QString(finalData), "sequence data");
}

IMPLEMENT_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_clear) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = SequenceSQLiteSpecificTestData::getSQLiteDbi();

    QByteArray baseData("AAAAAAA");
    U2DataId seqId = SequenceSQLiteSpecificTestData::createTestSequence(true, baseData, os);
    CHECK_NO_ERROR(os);

    // Some base values
    qint64 baseSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    U2TrackModType baseSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    qint64 baseModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);

    // Prepare test data
    QByteArray data("");
    U2Region region = U2_REGION_MAX;
    QVariantMap hints;

    // Call test function
    sqliteDbi->getSequenceDbi()->updateSequenceData(seqId, region, data, hints, os);
    CHECK_NO_ERROR(os);

    // Verify version
    qint64 finalSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqVersion + 1, finalSeqVersion, "sequence version");

    // Verify mod type
    U2TrackModType finalSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqModType, finalSeqModType, "sequence track mod type");

    // Verify mod steps num
    qint64 finalModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseModStepsNum + 1, finalModStepsNum, "mod steps num");

    //Verify modSteps
    U2SingleModStep modStep = sqliteDbi->getModDbi()->getModStep(seqId, finalSeqVersion - 1, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(U2ModType::sequenceUpdatedData, modStep.modType, "mod type");
    CHECK_EQUAL(seqId, modStep.objectId, "object ID");
    CHECK_EQUAL(finalSeqVersion - 1, modStep.version, "modStep version");
    CHECK_EQUAL("0\t0\tAAAAAAA\t\t\"\"", QString(modStep.details), "modStep details");

    // Verify sequence data
    QByteArray finalData = sqliteDbi->getSequenceDbi()->getSequenceData(seqId, U2_REGION_MAX, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(data, QString(finalData), "sequence data");
}

IMPLEMENT_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_begin) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = SequenceSQLiteSpecificTestData::getSQLiteDbi();

    QByteArray baseData("AAAAAAA");
    U2DataId seqId = SequenceSQLiteSpecificTestData::createTestSequence(true, baseData, os);
    CHECK_NO_ERROR(os);

    // Some base values
    qint64 baseSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    U2TrackModType baseSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    qint64 baseModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);

    // Prepare test data
    QByteArray data("CC");
    U2Region region = U2Region(0, 3);
    QVariantMap hints;

    //Prepare expected data
    QByteArray expectedData = "CCAAAA";

    // Call test function
    sqliteDbi->getSequenceDbi()->updateSequenceData(seqId, region, data, hints, os);
    CHECK_NO_ERROR(os);

    // Verify version
    qint64 finalSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqVersion + 1, finalSeqVersion, "sequence version");

    // Verify mod type
    U2TrackModType finalSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqModType, finalSeqModType, "sequence track mod type");

    // Verify mod steps num
    qint64 finalModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseModStepsNum + 1, finalModStepsNum, "mod steps num");

    //Verify modSteps
    U2SingleModStep modStep = sqliteDbi->getModDbi()->getModStep(seqId, finalSeqVersion - 1, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(U2ModType::sequenceUpdatedData, modStep.modType, "mod type");
    CHECK_EQUAL(seqId, modStep.objectId, "object ID");
    CHECK_EQUAL(finalSeqVersion - 1, modStep.version, "modStep version");
    CHECK_EQUAL("0\t0\tAAA\tCC\t\"\"", QString(modStep.details), "modStep details");

    // Verify sequence data
    QByteArray finalData = sqliteDbi->getSequenceDbi()->getSequenceData(seqId, U2_REGION_MAX, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(expectedData, QString(finalData), "sequence data");
}

IMPLEMENT_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_middle) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = SequenceSQLiteSpecificTestData::getSQLiteDbi();

    QByteArray baseData("AAAAAAA");
    U2DataId seqId = SequenceSQLiteSpecificTestData::createTestSequence(true, baseData, os);
    CHECK_NO_ERROR(os);

    // Some base values
    qint64 baseSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    U2TrackModType baseSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    qint64 baseModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);

    // Prepare test data
    QByteArray data("CC");
    U2Region region = U2Region(2, 3);
    QVariantMap hints;

    //Prepare expected data
    QByteArray expectedData = "AACCAA";

    // Call test function
    sqliteDbi->getSequenceDbi()->updateSequenceData(seqId, region, data, hints, os);
    CHECK_NO_ERROR(os);

    // Verify version
    qint64 finalSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqVersion + 1, finalSeqVersion, "sequence version");

    // Verify mod type
    U2TrackModType finalSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqModType, finalSeqModType, "sequence track mod type");

    // Verify mod steps num
    qint64 finalModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseModStepsNum + 1, finalModStepsNum, "mod steps num");

    //Verify modSteps
    U2SingleModStep modStep = sqliteDbi->getModDbi()->getModStep(seqId, finalSeqVersion - 1, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(U2ModType::sequenceUpdatedData, modStep.modType, "mod type");
    CHECK_EQUAL(seqId, modStep.objectId, "object ID");
    CHECK_EQUAL(finalSeqVersion - 1, modStep.version, "modStep version");
    CHECK_EQUAL("0\t2\tAAA\tCC\t\"\"", QString(modStep.details), "modStep details");

    // Verify sequence data
    QByteArray finalData = sqliteDbi->getSequenceDbi()->getSequenceData(seqId, U2_REGION_MAX, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(expectedData, QString(finalData), "sequence data");
}

IMPLEMENT_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_middle_middleNoLength) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = SequenceSQLiteSpecificTestData::getSQLiteDbi();

    QByteArray baseData("AAAAAAA");
    U2DataId seqId = SequenceSQLiteSpecificTestData::createTestSequence(true, baseData, os);
    CHECK_NO_ERROR(os);

    // Some base values
    qint64 baseSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    U2TrackModType baseSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    qint64 baseModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);

    // Prepare test data
    QByteArray data("CC");
    U2Region region = U2Region(2, 2);
    QVariantMap hints;
    hints.insert(U2SequenceDbiHints::UPDATE_SEQUENCE_LENGTH, false);

    //Prepare expected data
    QByteArray expectedData = "AACCAAA";

    // Call test function
    sqliteDbi->getSequenceDbi()->updateSequenceData(seqId, region, data, hints, os);
    CHECK_NO_ERROR(os);

    // Verify version
    qint64 finalSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqVersion + 1, finalSeqVersion, "sequence version");

    // Verify mod type
    U2TrackModType finalSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqModType, finalSeqModType, "sequence track mod type");

    // Verify mod steps num
    qint64 finalModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseModStepsNum + 1, finalModStepsNum, "mod steps num");

    //Verify modSteps
    U2SingleModStep modStep = sqliteDbi->getModDbi()->getModStep(seqId, finalSeqVersion - 1, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(U2ModType::sequenceUpdatedData, modStep.modType, "mod type");
    CHECK_EQUAL(seqId, modStep.objectId, "object ID");
    CHECK_EQUAL(finalSeqVersion - 1, modStep.version, "modStep version");
    CHECK_EQUAL("0\t2\tAA\tCC\t\"update-length,false\"", QString(modStep.details), "modStep details");

    // Verify sequence data
    QByteArray finalData = sqliteDbi->getSequenceDbi()->getSequenceData(seqId, U2_REGION_MAX, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(expectedData, QString(finalData), "sequence data");
}

IMPLEMENT_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_end) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = SequenceSQLiteSpecificTestData::getSQLiteDbi();

    QByteArray baseData("AAAAAAA");
    U2DataId seqId = SequenceSQLiteSpecificTestData::createTestSequence(true, baseData, os);
    CHECK_NO_ERROR(os);

    // Some base values
    qint64 baseSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    U2TrackModType baseSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    qint64 baseModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);

    // Prepare test data
    QByteArray data("CC");
    U2Region region = U2Region(4, 3);
    QVariantMap hints;

    //Prepare expected data
    QByteArray expectedData = "AAAACC";

    // Call test function
    sqliteDbi->getSequenceDbi()->updateSequenceData(seqId, region, data, hints, os);
    CHECK_NO_ERROR(os);

    // Verify version
    qint64 finalSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqVersion + 1, finalSeqVersion, "sequence version");

    // Verify mod type
    U2TrackModType finalSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqModType, finalSeqModType, "sequence track mod type");

    // Verify mod steps num
    qint64 finalModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseModStepsNum + 1, finalModStepsNum, "mod steps num");

    //Verify modSteps
    U2SingleModStep modStep = sqliteDbi->getModDbi()->getModStep(seqId, finalSeqVersion - 1, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(U2ModType::sequenceUpdatedData, modStep.modType, "mod type");
    CHECK_EQUAL(seqId, modStep.objectId, "object ID");
    CHECK_EQUAL(finalSeqVersion - 1, modStep.version, "modStep version");
    CHECK_EQUAL("0\t4\tAAA\tCC\t\"\"", QString(modStep.details), "modStep details");

    // Verify sequence data
    QByteArray finalData = sqliteDbi->getSequenceDbi()->getSequenceData(seqId, U2_REGION_MAX, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(expectedData, QString(finalData), "sequence data");
}


IMPLEMENT_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_emptyHint_undo) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = SequenceSQLiteSpecificTestData::getSQLiteDbi();

    qint64 seqLength = 0;
    U2DataId seqId = SequenceSQLiteSpecificTestData::createTestSequence(true, seqLength, os);
    CHECK_NO_ERROR(os);

    // Some base values
    qint64 baseSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    U2TrackModType baseSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    qint64 baseModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);

    // Prepare test data
    QByteArray data("AAAAAAA");
    U2Region region = U2_REGION_MAX;
    QVariantMap hints;
    hints.insert(U2SequenceDbiHints::EMPTY_SEQUENCE, true);

    // Call test function, undo changes
    sqliteDbi->getSequenceDbi()->updateSequenceData(seqId, region, data, hints, os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->undo(seqId, os);
    CHECK_NO_ERROR(os);

    // Verify version
    qint64 finalSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqVersion, finalSeqVersion, "sequence version");

    // Verify mod type
    U2TrackModType finalSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqModType, finalSeqModType, "sequence track mod type");

    // Verify mod steps num
    qint64 finalModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseModStepsNum + 1, finalModStepsNum, "mod steps num");

    //Verify modSteps
    U2SingleModStep modStep = sqliteDbi->getModDbi()->getModStep(seqId, finalSeqVersion, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(U2ModType::sequenceUpdatedData, modStep.modType, "mod type");
    CHECK_EQUAL(seqId, modStep.objectId, "object ID");
    CHECK_EQUAL(finalSeqVersion, modStep.version, "modStep version");
    CHECK_EQUAL("0\t0\t\tAAAAAAA\t\"empty-sequence,true\"", QString(modStep.details), "modStep details");

    // Verify sequence data
    QByteArray finalData = sqliteDbi->getSequenceDbi()->getSequenceData(seqId, U2_REGION_MAX, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("", QString(finalData), "sequence data");
}

IMPLEMENT_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_emptyNoHint_undo) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = SequenceSQLiteSpecificTestData::getSQLiteDbi();

    qint64 seqLength = 0;
    U2DataId seqId = SequenceSQLiteSpecificTestData::createTestSequence(true, seqLength, os);
    CHECK_NO_ERROR(os);

    // Some base values
    qint64 baseSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    U2TrackModType baseSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    qint64 baseModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);

    // Prepare test data
    QByteArray data("AAAAAAA");
    U2Region region = U2_REGION_MAX;
    QVariantMap hints;

    // Call test function, undo changes
    sqliteDbi->getSequenceDbi()->updateSequenceData(seqId, region, data, hints, os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->undo(seqId, os);
    CHECK_NO_ERROR(os);

    // Verify version
    qint64 finalSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqVersion, finalSeqVersion, "sequence version");

    // Verify mod type
    U2TrackModType finalSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqModType, finalSeqModType, "sequence track mod type");

    // Verify mod steps num
    qint64 finalModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseModStepsNum + 1, finalModStepsNum, "mod steps num");

    //Verify modSteps
    U2SingleModStep modStep = sqliteDbi->getModDbi()->getModStep(seqId, finalSeqVersion, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(U2ModType::sequenceUpdatedData, modStep.modType, "mod type");
    CHECK_EQUAL(seqId, modStep.objectId, "object ID");
    CHECK_EQUAL(finalSeqVersion, modStep.version, "modStep version");
    CHECK_EQUAL("0\t0\t\tAAAAAAA\t\"\"", QString(modStep.details), "modStep details");

    // Verify sequence data
    QByteArray finalData = sqliteDbi->getSequenceDbi()->getSequenceData(seqId, U2_REGION_MAX, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL("", QString(finalData), "sequence data");
}

IMPLEMENT_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_clear_undo) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = SequenceSQLiteSpecificTestData::getSQLiteDbi();

    QByteArray baseData("AAAAAAA");
    U2DataId seqId = SequenceSQLiteSpecificTestData::createTestSequence(true, baseData, os);
    CHECK_NO_ERROR(os);

    // Some base values
    qint64 baseSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    U2TrackModType baseSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    qint64 baseModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);

    // Prepare test data
    QByteArray data("");
    U2Region region = U2_REGION_MAX;
    QVariantMap hints;

    // Call test function, undo changes
    sqliteDbi->getSequenceDbi()->updateSequenceData(seqId, region, data, hints, os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->undo(seqId, os);
    CHECK_NO_ERROR(os);

    // Verify version
    qint64 finalSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqVersion, finalSeqVersion, "sequence version");

    // Verify mod type
    U2TrackModType finalSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqModType, finalSeqModType, "sequence track mod type");

    // Verify mod steps num
    qint64 finalModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseModStepsNum + 1, finalModStepsNum, "mod steps num");

    //Verify modSteps
    U2SingleModStep modStep = sqliteDbi->getModDbi()->getModStep(seqId, finalSeqVersion, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(U2ModType::sequenceUpdatedData, modStep.modType, "mod type");
    CHECK_EQUAL(seqId, modStep.objectId, "object ID");
    CHECK_EQUAL(finalSeqVersion, modStep.version, "modStep version");
    CHECK_EQUAL("0\t0\tAAAAAAA\t\t\"\"", QString(modStep.details), "modStep details");

    // Verify sequence data
    QByteArray finalData = sqliteDbi->getSequenceDbi()->getSequenceData(seqId, U2_REGION_MAX, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(QString(baseData), QString(finalData), "sequence data");
}

IMPLEMENT_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_begin_undo) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = SequenceSQLiteSpecificTestData::getSQLiteDbi();

    QByteArray baseData("AAAAAAA");
    U2DataId seqId = SequenceSQLiteSpecificTestData::createTestSequence(true, baseData, os);
    CHECK_NO_ERROR(os);

    // Some base values
    qint64 baseSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    U2TrackModType baseSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    qint64 baseModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);

    // Prepare test data
    QByteArray data("CC");
    U2Region region = U2Region(0, 3);
    QVariantMap hints;

    // Call test function, undo changes
    sqliteDbi->getSequenceDbi()->updateSequenceData(seqId, region, data, hints, os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->undo(seqId, os);
    CHECK_NO_ERROR(os);

    // Verify version
    qint64 finalSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqVersion, finalSeqVersion, "sequence version");

    // Verify mod type
    U2TrackModType finalSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqModType, finalSeqModType, "sequence track mod type");

    // Verify mod steps num
    qint64 finalModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseModStepsNum + 1, finalModStepsNum, "mod steps num");

    //Verify modSteps
    U2SingleModStep modStep = sqliteDbi->getModDbi()->getModStep(seqId, finalSeqVersion, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(U2ModType::sequenceUpdatedData, modStep.modType, "mod type");
    CHECK_EQUAL(seqId, modStep.objectId, "object ID");
    CHECK_EQUAL(finalSeqVersion, modStep.version, "modStep version");
    CHECK_EQUAL("0\t0\tAAA\tCC\t\"\"", QString(modStep.details), "modStep details");

    // Verify sequence data
    QByteArray finalData = sqliteDbi->getSequenceDbi()->getSequenceData(seqId, U2_REGION_MAX, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(QString(baseData), QString(finalData), "sequence data");
}

IMPLEMENT_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_middle_undo) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = SequenceSQLiteSpecificTestData::getSQLiteDbi();

    QByteArray baseData("AAAAAAA");
    U2DataId seqId = SequenceSQLiteSpecificTestData::createTestSequence(true, baseData, os);
    CHECK_NO_ERROR(os);

    // Some base values
    qint64 baseSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    U2TrackModType baseSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    qint64 baseModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);

    // Prepare test data
    QByteArray data("CC");
    U2Region region = U2Region(2, 3);
    QVariantMap hints;

    // Call test function, undo changes
    sqliteDbi->getSequenceDbi()->updateSequenceData(seqId, region, data, hints, os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->undo(seqId, os);
    CHECK_NO_ERROR(os);

    // Verify version
    qint64 finalSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqVersion, finalSeqVersion, "sequence version");

    // Verify mod type
    U2TrackModType finalSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqModType, finalSeqModType, "sequence track mod type");

    // Verify mod steps num
    qint64 finalModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseModStepsNum + 1, finalModStepsNum, "mod steps num");

    //Verify modSteps
    U2SingleModStep modStep = sqliteDbi->getModDbi()->getModStep(seqId, finalSeqVersion, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(U2ModType::sequenceUpdatedData, modStep.modType, "mod type");
    CHECK_EQUAL(seqId, modStep.objectId, "object ID");
    CHECK_EQUAL(finalSeqVersion, modStep.version, "modStep version");
    CHECK_EQUAL("0\t2\tAAA\tCC\t\"\"", QString(modStep.details), "modStep details");

    // Verify sequence data
    QByteArray finalData = sqliteDbi->getSequenceDbi()->getSequenceData(seqId, U2_REGION_MAX, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(QString(baseData), QString(finalData), "sequence data");
}

IMPLEMENT_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_middle_middleNoLength_undo) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = SequenceSQLiteSpecificTestData::getSQLiteDbi();

    QByteArray baseData("AAAAAAA");
    U2DataId seqId = SequenceSQLiteSpecificTestData::createTestSequence(true, baseData, os);
    CHECK_NO_ERROR(os);

    // Some base values
    qint64 baseSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    U2TrackModType baseSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    qint64 baseModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);

    // Prepare test data
    QByteArray data("CC");
    U2Region region = U2Region(2, 2);
    QVariantMap hints;
    hints.insert(U2SequenceDbiHints::UPDATE_SEQUENCE_LENGTH, false);

    // Call test function, undo changes
    sqliteDbi->getSequenceDbi()->updateSequenceData(seqId, region, data, hints, os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->undo(seqId, os);
    CHECK_NO_ERROR(os);

    // Verify version
    qint64 finalSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqVersion, finalSeqVersion, "sequence version");

    // Verify mod type
    U2TrackModType finalSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqModType, finalSeqModType, "sequence track mod type");

    // Verify mod steps num
    qint64 finalModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseModStepsNum + 1, finalModStepsNum, "mod steps num");

    //Verify modSteps
    U2SingleModStep modStep = sqliteDbi->getModDbi()->getModStep(seqId, finalSeqVersion, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(U2ModType::sequenceUpdatedData, modStep.modType, "mod type");
    CHECK_EQUAL(seqId, modStep.objectId, "object ID");
    CHECK_EQUAL(finalSeqVersion, modStep.version, "modStep version");
    CHECK_EQUAL("0\t2\tAA\tCC\t\"update-length,false\"", QString(modStep.details), "modStep details");

    // Verify sequence data
    QByteArray finalData = sqliteDbi->getSequenceDbi()->getSequenceData(seqId, U2_REGION_MAX, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(QString(baseData), QString(finalData), "sequence data");
}

IMPLEMENT_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_end_undo) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = SequenceSQLiteSpecificTestData::getSQLiteDbi();

    QByteArray baseData("AAAAAAA");
    U2DataId seqId = SequenceSQLiteSpecificTestData::createTestSequence(true, baseData, os);
    CHECK_NO_ERROR(os);

    // Some base values
    qint64 baseSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    U2TrackModType baseSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    qint64 baseModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);

    // Prepare test data
    QByteArray data("CC");
    U2Region region = U2Region(4, 3);
    QVariantMap hints;

    // Call test function, undo changes
    sqliteDbi->getSequenceDbi()->updateSequenceData(seqId, region, data, hints, os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->undo(seqId, os);
    CHECK_NO_ERROR(os);

    // Verify version
    qint64 finalSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqVersion, finalSeqVersion, "sequence version");

    // Verify mod type
    U2TrackModType finalSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqModType, finalSeqModType, "sequence track mod type");

    // Verify mod steps num
    qint64 finalModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseModStepsNum + 1, finalModStepsNum, "mod steps num");

    //Verify modSteps
    U2SingleModStep modStep = sqliteDbi->getModDbi()->getModStep(seqId, finalSeqVersion, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(U2ModType::sequenceUpdatedData, modStep.modType, "mod type");
    CHECK_EQUAL(seqId, modStep.objectId, "object ID");
    CHECK_EQUAL(finalSeqVersion, modStep.version, "modStep version");
    CHECK_EQUAL("0\t4\tAAA\tCC\t\"\"", QString(modStep.details), "modStep details");

    // Verify sequence data
    QByteArray finalData = sqliteDbi->getSequenceDbi()->getSequenceData(seqId, U2_REGION_MAX, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(QString(baseData), QString(finalData), "sequence data");
}


IMPLEMENT_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_emptyHint_redo) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = SequenceSQLiteSpecificTestData::getSQLiteDbi();

    qint64 seqLength = 0;
    U2DataId seqId = SequenceSQLiteSpecificTestData::createTestSequence(true, seqLength, os);
    CHECK_NO_ERROR(os);

    // Some base values
    qint64 baseSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    U2TrackModType baseSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    qint64 baseModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);

    // Prepare test data
    QByteArray data("AAAAAAA");
    U2Region region = U2_REGION_MAX;
    QVariantMap hints;
    hints.insert(U2SequenceDbiHints::EMPTY_SEQUENCE, true);

    // Call test function, undo and redo changes
    sqliteDbi->getSequenceDbi()->updateSequenceData(seqId, region, data, hints, os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->undo(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->redo(seqId, os);
    CHECK_NO_ERROR(os);

    // Verify version
    qint64 finalSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqVersion + 1, finalSeqVersion, "sequence version");

    // Verify mod type
    U2TrackModType finalSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqModType, finalSeqModType, "sequence track mod type");

    // Verify mod steps num
    qint64 finalModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseModStepsNum + 1, finalModStepsNum, "mod steps num");

    //Verify modSteps
    U2SingleModStep modStep = sqliteDbi->getModDbi()->getModStep(seqId, finalSeqVersion - 1, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(U2ModType::sequenceUpdatedData, modStep.modType, "mod type");
    CHECK_EQUAL(seqId, modStep.objectId, "object ID");
    CHECK_EQUAL(finalSeqVersion - 1, modStep.version, "modStep version");
    CHECK_EQUAL("0\t0\t\tAAAAAAA\t\"empty-sequence,true\"", QString(modStep.details), "modStep details");

    // Verify sequence data
    QByteArray finalData = sqliteDbi->getSequenceDbi()->getSequenceData(seqId, U2_REGION_MAX, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(data, QString(finalData), "sequence data");
}

IMPLEMENT_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_emptyNoHint_redo) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = SequenceSQLiteSpecificTestData::getSQLiteDbi();

    qint64 seqLength = 0;
    U2DataId seqId = SequenceSQLiteSpecificTestData::createTestSequence(true, seqLength, os);
    CHECK_NO_ERROR(os);

    // Some base values
    qint64 baseSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    U2TrackModType baseSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    qint64 baseModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);

    // Prepare test data
    QByteArray data("AAAAAAA");
    U2Region region = U2_REGION_MAX;
    QVariantMap hints;

    // Call test function, undo and redo changes
    sqliteDbi->getSequenceDbi()->updateSequenceData(seqId, region, data, hints, os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->undo(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->redo(seqId, os);
    CHECK_NO_ERROR(os);

    // Verify version
    qint64 finalSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqVersion + 1, finalSeqVersion, "sequence version");

    // Verify mod type
    U2TrackModType finalSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqModType, finalSeqModType, "sequence track mod type");

    // Verify mod steps num
    qint64 finalModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseModStepsNum + 1, finalModStepsNum, "mod steps num");

    //Verify modSteps
    U2SingleModStep modStep = sqliteDbi->getModDbi()->getModStep(seqId, finalSeqVersion - 1, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(U2ModType::sequenceUpdatedData, modStep.modType, "mod type");
    CHECK_EQUAL(seqId, modStep.objectId, "object ID");
    CHECK_EQUAL(finalSeqVersion - 1, modStep.version, "modStep version");
    CHECK_EQUAL("0\t0\t\tAAAAAAA\t\"\"", QString(modStep.details), "modStep details");

    // Verify sequence data
    QByteArray finalData = sqliteDbi->getSequenceDbi()->getSequenceData(seqId, U2_REGION_MAX, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(data, QString(finalData), "sequence data");
}

IMPLEMENT_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_clear_redo) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = SequenceSQLiteSpecificTestData::getSQLiteDbi();

    QByteArray baseData("AAAAAAA");
    U2DataId seqId = SequenceSQLiteSpecificTestData::createTestSequence(true, baseData, os);
    CHECK_NO_ERROR(os);

    // Some base values
    qint64 baseSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    U2TrackModType baseSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    qint64 baseModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);

    // Prepare test data
    QByteArray data("");
    U2Region region = U2_REGION_MAX;
    QVariantMap hints;

    // Call test function, undo and redo changes
    sqliteDbi->getSequenceDbi()->updateSequenceData(seqId, region, data, hints, os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->undo(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->redo(seqId, os);
    CHECK_NO_ERROR(os);

    // Verify version
    qint64 finalSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqVersion + 1, finalSeqVersion, "sequence version");

    // Verify mod type
    U2TrackModType finalSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqModType, finalSeqModType, "sequence track mod type");

    // Verify mod steps num
    qint64 finalModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseModStepsNum + 1, finalModStepsNum, "mod steps num");

    //Verify modSteps
    U2SingleModStep modStep = sqliteDbi->getModDbi()->getModStep(seqId, finalSeqVersion - 1, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(U2ModType::sequenceUpdatedData, modStep.modType, "mod type");
    CHECK_EQUAL(seqId, modStep.objectId, "object ID");
    CHECK_EQUAL(finalSeqVersion - 1, modStep.version, "modStep version");
    CHECK_EQUAL("0\t0\tAAAAAAA\t\t\"\"", QString(modStep.details), "modStep details");

    // Verify sequence data
    QByteArray finalData = sqliteDbi->getSequenceDbi()->getSequenceData(seqId, U2_REGION_MAX, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(data, QString(finalData), "sequence data");
}

IMPLEMENT_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_begin_redo) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = SequenceSQLiteSpecificTestData::getSQLiteDbi();

    QByteArray baseData("AAAAAAA");
    U2DataId seqId = SequenceSQLiteSpecificTestData::createTestSequence(true, baseData, os);
    CHECK_NO_ERROR(os);

    // Some base values
    qint64 baseSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    U2TrackModType baseSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    qint64 baseModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);

    // Prepare test data
    QByteArray data("CC");
    U2Region region = U2Region(0, 3);
    QVariantMap hints;

    //Prepare expected data
    QByteArray expectedData = "CCAAAA";

    // Call test function, undo and redo changes
    sqliteDbi->getSequenceDbi()->updateSequenceData(seqId, region, data, hints, os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->undo(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->redo(seqId, os);
    CHECK_NO_ERROR(os);

    // Verify version
    qint64 finalSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqVersion + 1, finalSeqVersion, "sequence version");

    // Verify mod type
    U2TrackModType finalSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqModType, finalSeqModType, "sequence track mod type");

    // Verify mod steps num
    qint64 finalModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseModStepsNum + 1, finalModStepsNum, "mod steps num");

    //Verify modSteps
    U2SingleModStep modStep = sqliteDbi->getModDbi()->getModStep(seqId, finalSeqVersion - 1, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(U2ModType::sequenceUpdatedData, modStep.modType, "mod type");
    CHECK_EQUAL(seqId, modStep.objectId, "object ID");
    CHECK_EQUAL(finalSeqVersion - 1, modStep.version, "modStep version");
    CHECK_EQUAL("0\t0\tAAA\tCC\t\"\"", QString(modStep.details), "modStep details");

    // Verify sequence data
    QByteArray finalData = sqliteDbi->getSequenceDbi()->getSequenceData(seqId, U2_REGION_MAX, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(expectedData, QString(finalData), "sequence data");
}

IMPLEMENT_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_middle_redo) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = SequenceSQLiteSpecificTestData::getSQLiteDbi();

    QByteArray baseData("AAAAAAA");
    U2DataId seqId = SequenceSQLiteSpecificTestData::createTestSequence(true, baseData, os);
    CHECK_NO_ERROR(os);

    // Some base values
    qint64 baseSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    U2TrackModType baseSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    qint64 baseModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);

    // Prepare test data
    QByteArray data("CC");
    U2Region region = U2Region(2, 3);
    QVariantMap hints;

    //Prepare expected data
    QByteArray expectedData = "AACCAA";

    // Call test function, undo and redo changes
    sqliteDbi->getSequenceDbi()->updateSequenceData(seqId, region, data, hints, os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->undo(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->redo(seqId, os);
    CHECK_NO_ERROR(os);

    // Verify version
    qint64 finalSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqVersion + 1, finalSeqVersion, "sequence version");

    // Verify mod type
    U2TrackModType finalSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqModType, finalSeqModType, "sequence track mod type");

    // Verify mod steps num
    qint64 finalModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseModStepsNum + 1, finalModStepsNum, "mod steps num");

    //Verify modSteps
    U2SingleModStep modStep = sqliteDbi->getModDbi()->getModStep(seqId, finalSeqVersion - 1, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(U2ModType::sequenceUpdatedData, modStep.modType, "mod type");
    CHECK_EQUAL(seqId, modStep.objectId, "object ID");
    CHECK_EQUAL(finalSeqVersion - 1, modStep.version, "modStep version");
    CHECK_EQUAL("0\t2\tAAA\tCC\t\"\"", QString(modStep.details), "modStep details");

    // Verify sequence data
    QByteArray finalData = sqliteDbi->getSequenceDbi()->getSequenceData(seqId, U2_REGION_MAX, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(expectedData, QString(finalData), "sequence data");
}

IMPLEMENT_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_middle_middleNoLength_redo) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = SequenceSQLiteSpecificTestData::getSQLiteDbi();

    QByteArray baseData("AAAAAAA");
    U2DataId seqId = SequenceSQLiteSpecificTestData::createTestSequence(true, baseData, os);
    CHECK_NO_ERROR(os);

    // Some base values
    qint64 baseSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    U2TrackModType baseSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    qint64 baseModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);

    // Prepare test data
    QByteArray data("CC");
    U2Region region = U2Region(2, 2);
    QVariantMap hints;
    hints.insert(U2SequenceDbiHints::UPDATE_SEQUENCE_LENGTH, false);

    //Prepare expected data
    QByteArray expectedData = "AACCAAA";

    // Call test function, undo and redo changes
    sqliteDbi->getSequenceDbi()->updateSequenceData(seqId, region, data, hints, os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->undo(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->redo(seqId, os);
    CHECK_NO_ERROR(os);

    // Verify version
    qint64 finalSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqVersion + 1, finalSeqVersion, "sequence version");

    // Verify mod type
    U2TrackModType finalSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqModType, finalSeqModType, "sequence track mod type");

    // Verify mod steps num
    qint64 finalModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseModStepsNum + 1, finalModStepsNum, "mod steps num");

    //Verify modSteps
    U2SingleModStep modStep = sqliteDbi->getModDbi()->getModStep(seqId, finalSeqVersion - 1, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(U2ModType::sequenceUpdatedData, modStep.modType, "mod type");
    CHECK_EQUAL(seqId, modStep.objectId, "object ID");
    CHECK_EQUAL(finalSeqVersion - 1, modStep.version, "modStep version");
    CHECK_EQUAL("0\t2\tAA\tCC\t\"update-length,false\"", QString(modStep.details), "modStep details");

    // Verify sequence data
    QByteArray finalData = sqliteDbi->getSequenceDbi()->getSequenceData(seqId, U2_REGION_MAX, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(expectedData, QString(finalData), "sequence data");
}

IMPLEMENT_TEST(SequenceDbiSQLiteSpecificUnitTests, updateSeqData_end_redo) {
    U2OpStatusImpl os;
    SQLiteDbi* sqliteDbi = SequenceSQLiteSpecificTestData::getSQLiteDbi();

    QByteArray baseData("AAAAAAA");
    U2DataId seqId = SequenceSQLiteSpecificTestData::createTestSequence(true, baseData, os);
    CHECK_NO_ERROR(os);

    // Some base values
    qint64 baseSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    U2TrackModType baseSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    qint64 baseModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);

    // Prepare test data
    QByteArray data("CC");
    U2Region region = U2Region(4, 3);
    QVariantMap hints;

    //Prepare expected data
    QByteArray expectedData = "AAAACC";

    // Call test function, undo and redo changes
    sqliteDbi->getSequenceDbi()->updateSequenceData(seqId, region, data, hints, os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->undo(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_NO_ERROR(os);
    sqliteDbi->getObjectDbi()->redo(seqId, os);
    CHECK_NO_ERROR(os);

    // Verify version
    qint64 finalSeqVersion = sqliteDbi->getObjectDbi()->getObjectVersion(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqVersion + 1, finalSeqVersion, "sequence version");

    // Verify mod type
    U2TrackModType finalSeqModType = sqliteDbi->getObjectDbi()->getTrackModType(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseSeqModType, finalSeqModType, "sequence track mod type");

    // Verify mod steps num
    qint64 finalModStepsNum = SequenceSQLiteSpecificTestData::getModStepsNum(seqId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(baseModStepsNum + 1, finalModStepsNum, "mod steps num");

    //Verify modSteps
    U2SingleModStep modStep = sqliteDbi->getModDbi()->getModStep(seqId, finalSeqVersion - 1, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(U2ModType::sequenceUpdatedData, modStep.modType, "mod type");
    CHECK_EQUAL(seqId, modStep.objectId, "object ID");
    CHECK_EQUAL(finalSeqVersion - 1, modStep.version, "modStep version");
    CHECK_EQUAL("0\t4\tAAA\tCC\t\"\"", QString(modStep.details), "modStep details");

    // Verify sequence data
    QByteArray finalData = sqliteDbi->getSequenceDbi()->getSequenceData(seqId, U2_REGION_MAX, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(expectedData, QString(finalData), "sequence data");
}

}   // namespace
