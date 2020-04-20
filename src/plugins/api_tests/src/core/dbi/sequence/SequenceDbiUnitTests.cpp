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

#include "SequenceDbiUnitTests.h"
#include <U2Core/AppContext.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>

#include <QDir>

namespace U2 {

const QString& INVALID_SEQUENCE_ID = "invalid_seq_id";
const QString& GET_SEQUENCE_IN = "get_seq_in";
const QString& GET_SEQUENCE_OUT = "get_seq_out";
const QString& GET_SEQUENCE_DATA_ID = "get_seq_data_id";
const QString& GET_SEQUENCE_DATA_REGION = "get_seq_data_region";
const QString& GET_SEQUENCE_DATA_OUT = "get_seq_data_out";

const QString& SequenceTestData::SEQ_DB_URL("sequence-dbi.ugenedb");
QList<U2DataId>* SequenceTestData::sequences = NULL;
U2SequenceDbi* SequenceTestData::sequenceDbi = NULL;
TestDbiProvider SequenceTestData::dbiProvider = TestDbiProvider();

static bool registerTests(){
    qRegisterMetaType<U2::SequenceDbiUnitTests_createSequenceObject>("SequenceDbiUnitTests_createSequenceObject");
    qRegisterMetaType<U2::SequenceDbiUnitTests_getAllSequenceObjects>("SequenceDbiUnitTests_getAllSequenceObjects");
    qRegisterMetaType<U2::SequenceDbiUnitTests_getSequenceData>("SequenceDbiUnitTests_getSequenceData");
    qRegisterMetaType<U2::SequenceDbiUnitTests_getLongSequenceData>("SequenceDbiUnitTests_getLongSequenceData");
    qRegisterMetaType<U2::SequenceDbiUnitTests_getSequenceDataInvalid>("SequenceDbiUnitTests_getSequenceDataInvalid");
    qRegisterMetaType<U2::SequenceDbiUnitTests_getSequenceObject>("SequenceDbiUnitTests_getSequenceObject");
    qRegisterMetaType<U2::SequenceDbiUnitTests_getSequenceObjectInvalid>("SequenceDbiUnitTests_getSequenceObjectInvalid");
    qRegisterMetaType<U2::SequenceDbiUnitTests_updateHugeSequenceData>("SequenceDbiUnitTests_updateHugeSequenceData");
    qRegisterMetaType<U2::SequenceDbiUnitTests_updateSequenceData>("SequenceDbiUnitTests_updateSequenceData");
    qRegisterMetaType<U2::SequenceDbiUnitTests_updateSequenceObject>("SequenceDbiUnitTests_updateSequenceObject");
    qRegisterMetaType<U2::SequenceDbiUnitTests_updateSequencesData>("SequenceDbiUnitTests_updateSequencesData");
    return true;
}

bool SequenceTestData::registerTest = registerTests();

void SequenceTestData::init() {
    bool ok = dbiProvider.init(SEQ_DB_URL, false);
    SAFE_POINT(ok, "dbi provider failed to initialize",);
    U2Dbi* dbi = dbiProvider.getDbi();
    U2ObjectDbi* objDbi = dbi->getObjectDbi();
    U2OpStatusImpl opStatus;

    sequenceDbi = dbi->getSequenceDbi();
    SAFE_POINT(NULL != sequenceDbi, "sequence database not loaded",);

    sequences = new QList<U2DataId>(objDbi->getObjects(U2Type::Sequence, 0, U2DbiOptions::U2_DBI_NO_LIMIT, opStatus));
    SAFE_POINT_OP(opStatus,);
}

U2SequenceDbi* SequenceTestData::getSequenceDbi() {
    if (sequenceDbi == NULL) {
        SequenceTestData::init();
    }
    return sequenceDbi;
}

void SequenceTestData::shutdown() {
    if (sequenceDbi != NULL) {
        U2OpStatusImpl opStatus;
        dbiProvider.close();
        sequenceDbi = NULL;
        SAFE_POINT_OP(opStatus, );
    }
}

bool SequenceTestData::compareSequences(const U2Sequence& s1, const U2Sequence& s2) {
    if (s1.id == s2.id && s1.alphabet.id == s2.alphabet.id &&
        s1.circular == s2.circular && s1.length == s2.length) {
        return true;
    }
    return false;
}

void SequenceTestData::checkUpdateSequence(UnitTest *t, const UpdateSequenceArgs& args) {
    U2SequenceDbi* sequenceDbi = SequenceTestData::getSequenceDbi();
    const U2DataId& id = sequences->at(args.sequenceId);

    for (int j=0, max = args.regionsToReplace.size(); j<max; j++) {
        U2Region regionToReplace = args.regionsToReplace.at(j);
        QByteArray dataToInsert = args.datazToInsert.at(j);

        qint64 originalLen(0);
        QByteArray originalSequence;

        {
            U2OpStatusImpl os;
            const U2Sequence& so = sequenceDbi->getSequenceObject(id, os);
            CHECK_OP(os, );
            originalLen = so.length;
        }

        {
            U2OpStatusImpl os;
            originalSequence = sequenceDbi->getSequenceData(id, U2Region(0, originalLen), os);
            CHECK_OP(os, );
        }

        {
            QVariantMap hints;
            U2OpStatusImpl os;
            sequenceDbi->updateSequenceData(id,
            regionToReplace,
            dataToInsert,
            hints,
            os);
            CHECK_OP(os,);
        }

        qint64 updatedLen(0);
        {
            U2OpStatusImpl os;
            const U2Sequence& so = sequenceDbi->getSequenceObject(id, os);
            CHECK_OP(os, );

            updatedLen = so.length;
            qint64 expectedLen = 0;
            const U2Region& intersection = U2Region(0, originalLen).intersect(regionToReplace);
            expectedLen = originalLen - intersection.length + dataToInsert.length();
            CHECK_EXT(expectedLen == updatedLen, t->SetError("incorrect updated sequence length"), );
        }

        {
            U2OpStatusImpl os;
            const QByteArray& actualSeq = sequenceDbi->getSequenceData(id, U2Region(0, updatedLen), os);
            CHECK_OP(os, );
            QByteArray expectedSeq;
            replaceRegion(t, originalSequence, dataToInsert, regionToReplace, expectedSeq);
            CHECK_EXT(expectedSeq == actualSeq, t->SetError("incorrect updated sequence"), );
        }
    }
}

void SequenceTestData::replaceRegion(UnitTest *t, const QByteArray& originalSequence,
                   const QByteArray& dataToInsert,
                   const U2Region& region,
                   QByteArray& resultSequence){
    U2Region regionToReplace = U2Region(0, originalSequence.length()).intersect(region);
    if (regionToReplace.length == 0) {
        if (region.startPos == originalSequence.length()) {
            resultSequence += originalSequence + dataToInsert;
        } else {
            CHECK_EXT(region.startPos == 0, t->SetError("region start position should be 0"), );
        }
    } else {
        QByteArray left = originalSequence.mid(0, regionToReplace.startPos);
        QByteArray right = originalSequence.mid(regionToReplace.endPos());
        resultSequence += left + dataToInsert + right;
    }
}



void SequenceDbiUnitTests_getSequenceObject::Test() {
    U2SequenceDbi* sequenceDbi = SequenceTestData::getSequenceDbi();

    APITestData testData;
    testData.addValue(GET_SEQUENCE_IN, 1);

    U2Sequence seq;
    seq.alphabet = BaseDNAAlphabetIds::NUCL_DNA_DEFAULT();
    seq.circular = false;
    seq.length = 30;
    testData.addValue(GET_SEQUENCE_OUT, seq);

    int i = testData.getValue<int>(GET_SEQUENCE_IN);
    U2DataId id = SequenceTestData::getSequences()->at(i);

    U2Sequence expected = testData.getValue<U2Sequence>(GET_SEQUENCE_OUT);
    expected.id = id;

    U2OpStatusImpl os;
    const U2Sequence& actual = sequenceDbi->getSequenceObject(id, os);
    CHECK_NO_ERROR(os);
    CHECK_EXT(SequenceTestData::compareSequences(actual, expected), SetError("incorrect expected sequence"), );
}

void SequenceDbiUnitTests_getSequenceObjectInvalid::Test(){

    U2SequenceDbi* sequenceDbi = SequenceTestData::getSequenceDbi();
    APITestData testData;
    testData.addValue<QByteArray>(INVALID_SEQUENCE_ID, "anmr%");

    const U2DataId& invalidId = testData.getValue<U2DataId>(INVALID_SEQUENCE_ID);
    CHECK_EXT(!SequenceTestData::getSequences()->contains(invalidId), SetError("invalid id should not be in sequences"), );
    U2OpStatusImpl os;
    const U2Sequence& seq = sequenceDbi->getSequenceObject(invalidId, os);
    CHECK_OP(os, );
    CHECK_EXT(os.hasError(), SetError("error should be thrown"), );
    CHECK_EXT(seq.id.isEmpty(), SetError("sequence id shuld be empty"), );
}

void SequenceDbiUnitTests_getAllSequenceObjects::Test() {
    U2SequenceDbi* sequenceDbi = SequenceTestData::getSequenceDbi();

    for (int i =0; i < SequenceTestData::getSequences()->size(); i++) {
        U2OpStatusImpl os;
        const U2DataId& id = SequenceTestData::getSequences()->at(i);
        const U2Sequence& seq = sequenceDbi->getSequenceObject(id, os);
        CHECK_OP(os, );
        CHECK_EXT(id == seq.id, SetError("incorrect expected sequence id"), );
    }
}

void SequenceDbiUnitTests_createSequenceObject::Test() {
    U2SequenceDbi* sequenceDbi = SequenceTestData::getSequenceDbi();

    U2Sequence seq;
    seq.circular = true;
    seq.alphabet = BaseDNAAlphabetIds::AMINO_DEFAULT();

    U2OpStatusImpl os;
    sequenceDbi->createSequenceObject(seq, "/", os);
    CHECK_OP(os, );

    const U2Sequence& actual = sequenceDbi->getSequenceObject(seq.id, os);
    CHECK_OP(os, );
    CHECK_EXT(!seq.id.isEmpty(), SetError("sequence id should not be empty"), );
    CHECK_EXT(seq.alphabet.id == actual.alphabet.id, SetError("incorrect expected sequence alphabet"), );
    CHECK_EXT(seq.circular == actual.circular, SetError("incorrect expected sequence circular"), );
}


void SequenceDbiUnitTests_getSequenceData::Test(){

    U2SequenceDbi* sequenceDbi = SequenceTestData::getSequenceDbi();
    APITestData testData;

    testData.addValue(GET_SEQUENCE_DATA_ID, 5);
    testData.addValue(GET_SEQUENCE_DATA_REGION, U2Region(5, 20));
    testData.addValue<QByteArray>(GET_SEQUENCE_DATA_OUT, "AAGTGATCGTCCTACGATCG");

    int i = testData.getValue<int>(GET_SEQUENCE_DATA_ID);
    const U2DataId& id = SequenceTestData::getSequences()->at(i);
    const U2Region& region = testData.getValue<U2Region>(GET_SEQUENCE_DATA_REGION);
    U2OpStatusImpl os;

    const QByteArray& expected = testData.getValue<QByteArray>(GET_SEQUENCE_DATA_OUT);
    const QByteArray& actual = sequenceDbi->getSequenceData(id, region, os);
    CHECK_OP(os, );

    CHECK_EXT(expected == actual, SetError("incorrect expected sequence data"), );
}

void SequenceDbiUnitTests_getLongSequenceData::Test(){

    U2SequenceDbi* sequenceDbi = SequenceTestData::getSequenceDbi();
    APITestData testData;

    testData.addValue(GET_SEQUENCE_DATA_ID, 10);
    testData.addValue(GET_SEQUENCE_DATA_REGION, U2Region(6, 1048570));
    testData.addValue<QByteArray>(GET_SEQUENCE_DATA_OUT, QByteArray(1048570, 'A'));

    int i = testData.getValue<int>(GET_SEQUENCE_DATA_ID);
    const U2DataId& id = SequenceTestData::getSequences()->at(i);
    const U2Region& region = testData.getValue<U2Region>(GET_SEQUENCE_DATA_REGION);
    U2OpStatusImpl os;

    const QByteArray& expected = testData.getValue<QByteArray>(GET_SEQUENCE_DATA_OUT);
    const QByteArray& actual = sequenceDbi->getSequenceData(id, region, os);
    CHECK_OP(os, );

    CHECK_EXT(expected == actual, SetError("incorrect expected sequence data"), );
}

void SequenceDbiUnitTests_getSequenceDataInvalid::Test() {
    U2SequenceDbi* sequenceDbi = SequenceTestData::getSequenceDbi();
    APITestData testData;
    testData.addValue<QByteArray>(INVALID_SEQUENCE_ID, "anmr%");
    const U2DataId& invalidId = testData.getValue<U2DataId>(INVALID_SEQUENCE_ID);

    U2OpStatusImpl os;

    const QByteArray& res = sequenceDbi->getSequenceData(invalidId, U2Region(0, 10), os);

    CHECK_EXT(res.isEmpty(), SetError("sequence data should be empty"), );
};

void SequenceDbiUnitTests_updateSequenceData::Test() {
    UpdateSequenceArgs usd;
    usd.sequenceId = 0;
    usd.datazToInsert << "AAAAAAAAAAAAA";
    usd.regionsToReplace << U2Region(20,5);

    usd.datazToInsert << "AAAAAAAAAAAAA";
    usd.regionsToReplace << U2Region(0,40);

    usd.datazToInsert << "AAAAAAAAAAAAA";
    usd.regionsToReplace << U2Region(13,13);

    SequenceTestData::checkUpdateSequence(this, usd);
}

void SequenceDbiUnitTests_updateHugeSequenceData::Test() {
    UpdateSequenceArgs usd;
    usd.sequenceId = 1;
    qint64 long_max = Q_INT64_C(9223372036854775807);
    usd.regionsToReplace << U2Region(0, long_max);
    usd.datazToInsert << QByteArray(long_max, 'A');
    SequenceTestData::checkUpdateSequence(this, usd);
};

void SequenceDbiUnitTests_updateSequencesData::Test() {
    UpdateSequenceArgs usd;
    usd.sequenceId = 1;
    qint64 length = Q_INT64_C(100000000000);
    for (int i = 0; i < 10000; i++){
        usd.regionsToReplace << U2Region((length -1) * i, length);
        usd.datazToInsert << QByteArray(length, 'A');
    }
    SequenceTestData::checkUpdateSequence(this, usd);
};

void SequenceDbiUnitTests_updateSequenceObject::Test() {
    U2SequenceDbi* sequenceDbi = SequenceTestData::getSequenceDbi();

    U2Sequence seq;
    seq.circular = true;
    seq.alphabet = BaseDNAAlphabetIds::AMINO_DEFAULT();

    U2OpStatusImpl os;
    sequenceDbi->createSequenceObject(seq, "/", os);
    CHECK_OP(os, );

    const U2Sequence& actual = sequenceDbi->getSequenceObject(seq.id, os);
    CHECK_OP(os, );
    CHECK_EXT(!seq.id.isEmpty(), SetError("sequence id should not be empty"), );
    CHECK_EXT(seq.alphabet.id == actual.alphabet.id, SetError("incorrect expected sequence alphabet"), );
    CHECK_EXT(seq.circular == actual.circular, SetError("incorrect expected sequence circular"), );

    seq.alphabet = BaseDNAAlphabetIds::RAW();
    seq.circular = true;
    seq.length = 0;
    sequenceDbi->updateSequenceObject(seq, os);
    CHECK_OP(os, );

    const U2Sequence& updated = sequenceDbi->getSequenceObject(seq.id, os);
    CHECK_OP(os, );
    CHECK_EXT(seq.id == updated.id, SetError("ids should be the same"), );
    CHECK_EXT(seq.alphabet.id == updated.alphabet.id, SetError("incorrect updated sequence alphabet"), );
    CHECK_EXT(seq.circular == updated.circular, SetError("incorrect updated sequence circular"), );
};

} //namespace
