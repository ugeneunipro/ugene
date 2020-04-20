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

#include "AssemblyDbiUnitTests.h"
#include "AssemblyDbiTestUtil.h"

#include <U2Core/U2AssemblyDbi.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>

#include <U2Test/TestRunnerSettings.h>

namespace U2 {

static const QString& INVALID_ASSEMBLY_ID = "invalid_assembly_id";

static const QString& TOTAL_NUM_READS = "total_num_reads";
static const QString& NUM_READS_IN = "num_reads_in";
static const QString& NUM_READS_OUT = "num_reads_out";

static const QString& GET_READS_IN = "get_reads_region";
static const QString& GET_READS_OUT = "get_reads_out";

static const QString& GET_READS_BY_ROW_REGION = "get_reads_by_row_region";
static const QString& GET_READS_BY_ROW_BEGIN = "get_reads_by_row_begin";
static const QString& GET_READS_BY_ROW_END = "get_reads_by_row_end";
static const QString& GET_READS_BY_ROW_OUT = "get_reads_by_row_out";

static const QString& READS_BY_NAME_IN = "reads_by_name_in";

static const QString& MAX_END_POS = "max_end_pos";

static const QString& MAX_PACKED_ROW_IN = "max_packed_row_in";
static const QString& MAX_PACKED_ROW_OUT = "max_packed_row_out";

static const QString& REMOVE_READS_IN = "remove_reads_in";

static const QString& ADD_READ = "add_read";

const QString& AssemblyTestData::ASS_DB_URL("assembly-dbi.ugenedb");

U2AssemblyDbi* AssemblyTestData::assemblyDbi = NULL;
QList<U2DataId>* AssemblyTestData::assemblyIds = NULL;
TestDbiProvider AssemblyTestData::dbiProvider = TestDbiProvider();

static bool registerTests(){
    qRegisterMetaType<U2::AssemblyDbiUnitTests_addReads>("AssemblyDbiUnitTests_addReads");
    qRegisterMetaType<U2::AssemblyDbiUnitTests_addReadsInvalid>("AssemblyDbiUnitTests_addReadsInvalid");
    qRegisterMetaType<U2::AssemblyDbiUnitTests_calculateCoverage>("AssemblyDbiUnitTests_calculateCoverage");
    qRegisterMetaType<U2::AssemblyDbiUnitTests_calculateCoverageInvalid>("AssemblyDbiUnitTests_calculateCoverageInvalid");
    qRegisterMetaType<U2::AssemblyDbiUnitTests_countReads>("AssemblyDbiUnitTests_countReads");
    qRegisterMetaType<U2::AssemblyDbiUnitTests_countReadsInvalid>("AssemblyDbiUnitTests_countReadsInvalid");
    qRegisterMetaType<U2::AssemblyDbiUnitTests_createAssemblyObject>("AssemblyDbiUnitTests_createAssemblyObject");
    qRegisterMetaType<U2::AssemblyDbiUnitTests_getAssemblyObject>("AssemblyDbiUnitTests_getAssemblyObject");
    qRegisterMetaType<U2::AssemblyDbiUnitTests_getAssemblyObjectInvalid>("AssemblyDbiUnitTests_getAssemblyObjectInvalid");
    qRegisterMetaType<U2::AssemblyDbiUnitTests_getMaxEndPos>("AssemblyDbiUnitTests_getMaxEndPos");
    qRegisterMetaType<U2::AssemblyDbiUnitTests_getMaxEndPosInvalid>("AssemblyDbiUnitTests_getMaxEndPosInvalid");
    qRegisterMetaType<U2::AssemblyDbiUnitTests_getMaxPackedRow>("AssemblyDbiUnitTests_getMaxPackedRow");
    qRegisterMetaType<U2::AssemblyDbiUnitTests_getMaxPackedRowInvalid>("AssemblyDbiUnitTests_getMaxPackedRowInvalid");
    qRegisterMetaType<U2::AssemblyDbiUnitTests_getReads>("AssemblyDbiUnitTests_getReads");
    qRegisterMetaType<U2::AssemblyDbiUnitTests_getReadsInvalid>("AssemblyDbiUnitTests_getReadsInvalid");
    qRegisterMetaType<U2::AssemblyDbiUnitTests_getReadsByName>("AssemblyDbiUnitTests_getReadsByName");
    qRegisterMetaType<U2::AssemblyDbiUnitTests_getReadsByNameInvalid>("AssemblyDbiUnitTests_getReadsByNameInvalid");
    qRegisterMetaType<U2::AssemblyDbiUnitTests_getReadsByRow>("AssemblyDbiUnitTests_getReadsByRow");
    qRegisterMetaType<U2::AssemblyDbiUnitTests_getReadsByRowInvalid>("AssemblyDbiUnitTests_getReadsByRowInvalid");
    qRegisterMetaType<U2::AssemblyDbiUnitTests_pack>("AssemblyDbiUnitTests_pack");
    qRegisterMetaType<U2::AssemblyDbiUnitTests_packInvalid>("AssemblyDbiUnitTests_packInvalid");
    qRegisterMetaType<U2::AssemblyDbiUnitTests_removeReads>("AssemblyDbiUnitTests_removeReads");
    qRegisterMetaType<U2::AssemblyDbiUnitTests_removeReadsInvalid>("AssemblyDbiUnitTests_removeReadsInvalid");
    return true;
}

bool AssemblyTestData::registerTest = registerTests();

void AssemblyTestData::init() {
    bool ok = dbiProvider.init(ASS_DB_URL, false);
    SAFE_POINT(ok, "dbi provider failed to initialize",);
    U2Dbi* dbi = dbiProvider.getDbi();
    U2ObjectDbi* objDbi = dbi->getObjectDbi();
    U2OpStatusImpl opStatus;

    assemblyIds = new QList<U2DataId>(objDbi->getObjects(U2Type::Assembly, 0, U2DbiOptions::U2_DBI_NO_LIMIT, opStatus));
    SAFE_POINT_OP(opStatus,);

    assemblyDbi = dbi->getAssemblyDbi();
    SAFE_POINT(NULL != assemblyDbi, "assembly database not loaded",);
}

U2AssemblyDbi* AssemblyTestData::getAssemblyDbi() {
    if (assemblyDbi == NULL) {
        AssemblyTestData::init();
    }
    return assemblyDbi;
}

void AssemblyTestData::shutdown(){
    if (assemblyDbi != NULL) {
        U2OpStatusImpl opStatus;
        dbiProvider.close();
        assemblyDbi = NULL;
        SAFE_POINT_OP(opStatus, );
    }
}


void AssemblyDbiUnitTests_getAssemblyObject::Test() {
    U2AssemblyDbi* assemblyDbi = AssemblyTestData::getAssemblyDbi();

    const U2DataId id = AssemblyTestData::getAssemblyIds()->first();
    U2OpStatusImpl os;
    U2Assembly assembly = assemblyDbi->getAssemblyObject(id, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(id, assembly.id,"assembly id is not equals to oiginal id" );
}

void AssemblyDbiUnitTests_getAssemblyObjectInvalid::Test() {
    U2AssemblyDbi* assemblyDbi = AssemblyTestData::getAssemblyDbi();

    APITestData testData;
    testData.addValue(INVALID_ASSEMBLY_ID, QByteArray("zZÿÿ"));

    const U2DataId& invalidId = testData.getValue<QByteArray>(INVALID_ASSEMBLY_ID);
    U2OpStatusImpl os;
    const U2Assembly& assembly = assemblyDbi->getAssemblyObject(invalidId, os);
    CHECK_TRUE(assembly.id.isEmpty(), "assembly id is empty");
}

void AssemblyDbiUnitTests_countReads::Test() {
    U2AssemblyDbi* assemblyDbi = AssemblyTestData::getAssemblyDbi();

    APITestData testData;

    // countReads
    testData.addValue(TOTAL_NUM_READS, 48);
    testData.addValue(NUM_READS_IN, U2Region(10,10));
    testData.addValue(NUM_READS_OUT, 6);

    const U2DataId id = AssemblyTestData::getAssemblyIds()->first();

    {
        U2OpStatusImpl os;
        qint64 numReads = assemblyDbi->countReads(id, U2_REGION_MAX, os);
        CHECK_NO_ERROR(os);
        CHECK_EQUAL(numReads, testData.getValue<qint64>(TOTAL_NUM_READS), "incorrect total num reads");
    }

    U2OpStatusImpl os;
    const U2Region& testRegion = testData.getValue<U2Region>(NUM_READS_IN);
    qint64 numReads = assemblyDbi->countReads(id, testRegion, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(numReads, testData.getValue<qint64>(NUM_READS_OUT), "incorrect total num reads");
}

void AssemblyDbiUnitTests_countReadsInvalid::Test() {
    U2AssemblyDbi* assemblyDbi = AssemblyTestData::getAssemblyDbi();

    APITestData testData;
    testData.addValue(INVALID_ASSEMBLY_ID, QByteArray("zZÿÿ"));
    const U2DataId& id = testData.getValue<U2DataId>(INVALID_ASSEMBLY_ID);
    U2OpStatusImpl os;
    qint64 val = assemblyDbi->countReads(id, U2_REGION_MAX, os);
    CHECK_EQUAL(-1, val, "count reads should be -1")
}

void AssemblyDbiUnitTests_getReads::Test() {
    U2AssemblyDbi* assemblyDbi = AssemblyTestData::getAssemblyDbi();

    APITestData testData;
    //getReads
    {
        testData.addValue(GET_READS_IN, U2Region(140, 145));

        U2AssemblyRead read1(new U2AssemblyReadData());
        read1->name = "2797 Example sequence FW - secondary sequence 5464";
        read1->leftmostPos = 93;
        read1->effectiveLen = 49;
        read1->packedViewRow = 15;
        read1->readSequence = "AAGATCCTCATGTTATATCGGCAGTGGGTTGATCAATCCACGTGGATAG";
        read1->cigar.append(U2CigarToken(U2CigarOp_M, 49));
        read1->flags = None;

        U2AssemblyRead read2(new U2AssemblyReadData());
        read2->name = "4940 Example sequence FW - secondary sequence 5466";
        read2->leftmostPos = 95;
        read2->effectiveLen = 49;
        read2->packedViewRow = 0;
        read2->readSequence = "GATCCTCATGTTATATCGGCAGTGGGTTGATCAATCCACGTGGATAGAT";
        read2->cigar.append(U2CigarToken(U2CigarOp_M, 49));
        read2->flags = None;

        U2AssemblyRead read3(new U2AssemblyReadData());
        read3->name = "2378 Example sequence RV - secondary sequence 5468";
        read3->leftmostPos = 97;
        read3->effectiveLen = 49;
        read3->packedViewRow = 16;
        read3->readSequence = "TCCTCATGTTATATCGGCAGTGGGTTGATCAATCCACGTGGATAGATGC";
        read3->cigar.append(U2CigarToken(U2CigarOp_M, 49));
        read3->flags = None;

        QVariantList reads;
        reads << qVariantFromValue(read1) << qVariantFromValue(read2) << qVariantFromValue(read3);
        testData.addValue(GET_READS_OUT, reads);
    }

    const U2DataId& id = AssemblyTestData::getAssemblyIds()->first();
    const U2Region& region = testData.getValue<U2Region>(GET_READS_IN);
    U2OpStatusImpl os;

    QScopedPointer< U2DbiIterator<U2AssemblyRead> > iter;
    iter.reset(assemblyDbi->getReads(id, region, os));
    CHECK_NO_ERROR(os);

    QVariantList expectedVar = testData.getValue<QVariantList>(GET_READS_OUT);
    QList<U2AssemblyRead> expectedReads;
    AssemblyDbiTestUtil::var2readList(expectedVar, expectedReads);
    CHECK_TRUE(AssemblyDbiTestUtil::compareReadLists(iter.data(), expectedReads), "incorrect expected reads");
}

void AssemblyDbiUnitTests_getReadsInvalid::Test() {
    U2AssemblyDbi* assemblyDbi = AssemblyTestData::getAssemblyDbi();

    APITestData testData;
    testData.addValue(INVALID_ASSEMBLY_ID, QByteArray("zZÿÿ"));

    const U2DataId& id = testData.getValue<U2DataId>(INVALID_ASSEMBLY_ID);
    U2OpStatusImpl os;
    QScopedPointer< U2DbiIterator<U2AssemblyRead> > iter;
    iter.reset(assemblyDbi->getReads(id, U2_REGION_MAX, os));
    CHECK_TRUE(iter.isNull(), "expected reads should be NULL");
}

void AssemblyDbiUnitTests_getReadsByRow::Test() {
    U2AssemblyDbi* assemblyDbi = AssemblyTestData::getAssemblyDbi();

    APITestData testData;
    // getReadsByRow
    {
        U2AssemblyRead read1(new U2AssemblyReadData());
        read1->name = "4924 Example sequence RV -E(46=3|) secondary sequence 5400";
        read1->leftmostPos = 29;
        read1->effectiveLen = 49;
        read1->packedViewRow = 10;
        read1->readSequence = "TGACGCAGCGAGCCGCCTTCATACTTTGTTAGCACGGCTCCACACATAT";
        read1->flags = None;
        read1->cigar.append(U2CigarToken(U2CigarOp_M, 49));

        U2AssemblyRead read2(new U2AssemblyReadData());
        read2->name = "2728 Example sequence FW - secondary sequence 5440";
        read2->leftmostPos = 69;
        read2->effectiveLen = 49;
        read2->packedViewRow = 5;
        read2->readSequence = "CACACATATGGTCCGACTCTAATGAAGATCCTCATGTTATATCGGCAGT";
        read2->flags = None;
        read2->cigar.append(U2CigarToken(U2CigarOp_M, 49));

        QVariantList reads;
        reads << qVariantFromValue(read1) << qVariantFromValue(read2);

        testData.addValue(GET_READS_BY_ROW_REGION, U2Region(74, 3));
        testData.addValue(GET_READS_BY_ROW_BEGIN, 5);
        testData.addValue(GET_READS_BY_ROW_END, 11);
        testData.addValue(GET_READS_BY_ROW_OUT, reads);
    }

    const U2DataId& id = AssemblyTestData::getAssemblyIds()->first();
    const U2Region& region = testData.getValue<U2Region>(GET_READS_BY_ROW_REGION);
    U2OpStatusImpl os;

    qint64 begin = testData.getValue<qint64>(GET_READS_BY_ROW_BEGIN);
    qint64 end = testData.getValue<qint64>(GET_READS_BY_ROW_END);

    QScopedPointer< U2DbiIterator<U2AssemblyRead> > iter;
    iter.reset(assemblyDbi->getReadsByRow(id, region, begin, end, os));
    CHECK_NO_ERROR(os);

    QVariantList expectedVar = testData.getValue<QVariantList>(GET_READS_BY_ROW_OUT);
    QList<U2AssemblyRead> expectedReads;
    AssemblyDbiTestUtil::var2readList(expectedVar, expectedReads);
    CHECK_TRUE(AssemblyDbiTestUtil::compareReadLists(iter.data(), expectedReads), "incorrect expected reads");
}

void AssemblyDbiUnitTests_getReadsByRowInvalid::Test() {
    U2AssemblyDbi* assemblyDbi = AssemblyTestData::getAssemblyDbi();

    APITestData testData;
    {
        testData.addValue(INVALID_ASSEMBLY_ID, QByteArray("zZÿÿ"));
        testData.addValue(GET_READS_BY_ROW_REGION, U2Region(74, 3));
        testData.addValue(GET_READS_BY_ROW_BEGIN, 5);
        testData.addValue(GET_READS_BY_ROW_END, 11);
    }

    const U2DataId& id = testData.getValue<U2DataId>(INVALID_ASSEMBLY_ID);
    qint64 begin = testData.getValue<qint64>(GET_READS_BY_ROW_BEGIN);
    qint64 end = testData.getValue<qint64>(GET_READS_BY_ROW_END);
    const U2Region& region = testData.getValue<U2Region>(GET_READS_BY_ROW_REGION);
    U2OpStatusImpl os;

    QScopedPointer< U2DbiIterator<U2AssemblyRead> > iter;
    iter.reset(assemblyDbi->getReadsByRow(id, region, begin, end, os));
    CHECK_TRUE(iter.isNull(), "expected reads by row should be NULL");
}

void AssemblyDbiUnitTests_getReadsByName::Test() {
    U2AssemblyDbi* assemblyDbi = AssemblyTestData::getAssemblyDbi();

    APITestData testData;
    // getReadsByName
    {
        U2AssemblyRead read(new U2AssemblyReadData());
        read->name = "Test read";
        read->leftmostPos = 93;
        read->effectiveLen = 49;
        read->packedViewRow = 0;
        read->readSequence = "AAGATCCTCATGTTATATCGGCAGTGGGTTGATCAATCCACGTGGATAG";
        read->flags = None;

        QVariantList readsByName;
        readsByName.append(qVariantFromValue(read));
        testData.addValue(READS_BY_NAME_IN, readsByName);
    }

    QVariantList readsVar = testData.getValue<QVariantList>(READS_BY_NAME_IN);
    CHECK_TRUE(!readsVar.isEmpty(), "reads list should not be empty");

    QList<U2AssemblyRead> reads;
    AssemblyDbiTestUtil::var2readList(readsVar, reads);
    const QByteArray& name = reads.first()->name;
    const U2DataId& id = AssemblyTestData::getAssemblyIds()->first();

    {
        U2OpStatusImpl os;
        BufferedDbiIterator<U2AssemblyRead> it(reads);
        assemblyDbi->addReads(id, &it, os);
        CHECK_NO_ERROR(os);
    }

    U2OpStatusImpl os;
    QScopedPointer< U2DbiIterator<U2AssemblyRead> > iter;
    iter.reset(assemblyDbi->getReadsByName(id, name, os));
    CHECK_NO_ERROR(os);
    CHECK_TRUE(AssemblyDbiTestUtil::compareReadLists(iter.data(), reads), "incorrect expected read list");
}

void AssemblyDbiUnitTests_getReadsByNameInvalid::Test() {
    U2AssemblyDbi* assemblyDbi = AssemblyTestData::getAssemblyDbi();

    APITestData testData;
    testData.addValue(INVALID_ASSEMBLY_ID, QByteArray("zZÿÿ"));

    const U2DataId& id = testData.getValue<U2DataId>(INVALID_ASSEMBLY_ID);

    U2OpStatusImpl os;
    QScopedPointer< U2DbiIterator<U2AssemblyRead> > iter;
    iter.reset(assemblyDbi->getReadsByName(id, "", os));
    CHECK_TRUE(iter.isNull(), "reads by name should be NULL");
}

void AssemblyDbiUnitTests_getMaxPackedRow::Test() {
    U2AssemblyDbi* assemblyDbi = AssemblyTestData::getAssemblyDbi();

    APITestData testData;
    testData.addValue(MAX_PACKED_ROW_IN, U2Region(10,6));
    testData.addValue(MAX_PACKED_ROW_OUT, 3);

    const U2DataId& id = AssemblyTestData::getAssemblyIds()->first();
    U2OpStatusImpl os;

    qint64 actual = assemblyDbi->getMaxPackedRow(id, testData.getValue<U2Region>(MAX_PACKED_ROW_IN), os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(actual == testData.getValue<qint64>(MAX_PACKED_ROW_OUT), "incorrect max packed row");
}

void AssemblyDbiUnitTests_getMaxPackedRowInvalid::Test() {
    U2AssemblyDbi* assemblyDbi = AssemblyTestData::getAssemblyDbi();

    APITestData testData;
    testData.addValue(INVALID_ASSEMBLY_ID, QByteArray("zZÿÿ"));

    const U2DataId& id = testData.getValue<U2DataId>(INVALID_ASSEMBLY_ID);
    U2OpStatusImpl os;
    qint64 val = assemblyDbi->getMaxPackedRow(id, U2_REGION_MAX, os);
    CHECK_TRUE(val == -1, "max packed row should be -1");
}

void AssemblyDbiUnitTests_getMaxEndPos::Test() {
    U2AssemblyDbi* assemblyDbi = AssemblyTestData::getAssemblyDbi();
    APITestData testData;
    testData.addValue(MAX_END_POS, qint64(146));

    U2DataId id = AssemblyTestData::getAssemblyIds()->first();
    U2OpStatusImpl os;
    qint64 val = assemblyDbi->getMaxEndPos(id, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(testData.getValue<qint64>(MAX_END_POS) == val, "incorrect max end position");
}

void AssemblyDbiUnitTests_getMaxEndPosInvalid::Test() {
    U2AssemblyDbi* assemblyDbi = AssemblyTestData::getAssemblyDbi();

    APITestData testData;
    testData.addValue(INVALID_ASSEMBLY_ID, QByteArray("zZÿÿ"));

    const U2DataId& id = testData.getValue<U2DataId>(INVALID_ASSEMBLY_ID);
    U2OpStatusImpl os;
    qint64 val = assemblyDbi->getMaxEndPos(id, os);
    CHECK_TRUE(val == -1, "max end position should be -1");
}

void AssemblyDbiUnitTests_createAssemblyObject::Test() {
    U2AssemblyDbi* assemblyDbi = AssemblyTestData::getAssemblyDbi();

    U2Assembly assembly;
    U2AssemblyReadsImportInfo importInfo;
    U2OpStatusImpl os;

    assemblyDbi->createAssemblyObject(assembly, "/", NULL, importInfo, os);
    CHECK_NO_ERROR(os);

    U2Assembly res = assemblyDbi->getAssemblyObject(assembly.id, os);
    CHECK_NO_ERROR(os);
}

void AssemblyDbiUnitTests_removeReads::Test() {
    U2AssemblyDbi* assemblyDbi = AssemblyTestData::getAssemblyDbi();
    APITestData testData;
    testData.addValue(REMOVE_READS_IN, U2Region(10, 10));

    const U2DataId& id = AssemblyTestData::getAssemblyIds()->first();
    const U2Region& region = testData.getValue<U2Region>(REMOVE_READS_IN);
    U2OpStatusImpl os;

    QList<U2DataId> readIds;
    {
        QScopedPointer< U2DbiIterator<U2AssemblyRead> > iter;
        iter.reset(assemblyDbi->getReads(id, region, os));
        CHECK_NO_ERROR(os);

        while (iter->hasNext()) {
            const U2AssemblyRead& read = iter->next();
            readIds.append(read->id);
        }
    }

    assemblyDbi->removeReads(id, readIds, os);
    CHECK_NO_ERROR(os);

    QScopedPointer< U2DbiIterator<U2AssemblyRead> > iter;
    iter.reset(assemblyDbi->getReads(id, region, os));
    CHECK_TRUE(!iter->hasNext(), "reads list should be empty");

}

void AssemblyDbiUnitTests_removeReadsInvalid::Test() {
    U2AssemblyDbi* assemblyDbi = AssemblyTestData::getAssemblyDbi();

    APITestData testData;
    testData.addValue(INVALID_ASSEMBLY_ID, QByteArray("zZÿÿ"));

    const U2DataId& id = testData.getValue<U2DataId>(INVALID_ASSEMBLY_ID);
    U2OpStatusImpl os;
    assemblyDbi->removeReads(id, QList<U2DataId>(), os);
    CHECK_TRUE(os.hasError(), "error should be thrown");
}

void AssemblyDbiUnitTests_addReads::Test() {
    U2AssemblyDbi* assemblyDbi = AssemblyTestData::getAssemblyDbi();
    APITestData testData;
    // addReads
    {
        U2AssemblyRead read2add(new U2AssemblyReadData());
        read2add->name = "Test read";
        read2add->leftmostPos = 90;
        read2add->readSequence = "GTTATATCGGCAGTGGGTTGATC";
        read2add->effectiveLen = read2add->readSequence.length();
        read2add->flags = None;
        QVariantList reads2add;
        reads2add.append(qVariantFromValue(read2add));
        testData.addValue(ADD_READ, reads2add);
    }

    QVariantList reads2add = testData.getValue<QVariantList>(ADD_READ);
    QList<U2AssemblyRead> reads;
    foreach (const QVariant &var, reads2add) {
        const U2AssemblyRead read = var.value<U2AssemblyRead>();
        reads.append(read);
    }

    const U2DataId& id = AssemblyTestData::getAssemblyIds()->first();
    BufferedDbiIterator<U2AssemblyRead> it(reads);
    U2OpStatusImpl os;

    assemblyDbi->addReads(id, &it, os);
    CHECK_NO_ERROR(os);

    foreach(U2AssemblyRead read, reads) {
        QScopedPointer< U2DbiIterator<U2AssemblyRead> > iter;
        iter.reset(assemblyDbi->getReads(id, U2_REGION_MAX, os));
        CHECK_NO_ERROR(os);

        bool added = false;
        while (iter->hasNext()) {
            if (AssemblyDbiTestUtil::compareReads(read, iter->next())) {
                added = true;
                break;
            }
        }
        CHECK_TRUE(added, "incorrect compared reads");
    }
}

void AssemblyDbiUnitTests_addReadsInvalid::Test() {
    U2AssemblyDbi* assemblyDbi = AssemblyTestData::getAssemblyDbi();

    APITestData testData;
    testData.addValue(INVALID_ASSEMBLY_ID, QByteArray("zZÿÿ"));

    const U2DataId& id = testData.getValue<U2DataId>(INVALID_ASSEMBLY_ID);
    QList<U2AssemblyRead> reads;
    BufferedDbiIterator<U2AssemblyRead> it(reads);
    U2OpStatusImpl os;
    assemblyDbi->addReads(id, &it, os);
    CHECK_TRUE(os.hasError(), "error should be thrown");
}

void AssemblyDbiUnitTests_pack::Test() {
    U2AssemblyDbi* assemblyDbi = AssemblyTestData::getAssemblyDbi();

    const U2DataId& id = AssemblyTestData::getAssemblyIds()->first();
    U2AssemblyPackStat stats;
    U2OpStatusImpl os;

    assemblyDbi->pack(id, stats, os);
    CHECK_NO_ERROR(os);

    CHECK_TRUE(stats.maxProw == 29, "incorrect max prow");
    CHECK_TRUE(stats.readsCount == 44, "incorrect count reads");
}

void AssemblyDbiUnitTests_packInvalid::Test() {
    U2AssemblyDbi* assemblyDbi = AssemblyTestData::getAssemblyDbi();

    APITestData testData;
    testData.addValue(INVALID_ASSEMBLY_ID, QByteArray("zZÿÿ"));

    const U2DataId& id = testData.getValue<U2DataId>(INVALID_ASSEMBLY_ID);
    U2AssemblyPackStat stat;
    U2OpStatusImpl os;
    assemblyDbi->pack(id, stat, os);
    CHECK_TRUE(os.hasError(), "error should be thrown");
}

void AssemblyDbiUnitTests_calculateCoverage::Test() {
    U2AssemblyDbi* assemblyDbi = AssemblyTestData::getAssemblyDbi();

    U2OpStatusImpl os;
    const U2DataId& id = AssemblyTestData::getAssemblyIds()->first();
    const U2Region& region = U2Region(20, 1);
    U2AssemblyCoverageStat c;
    c.resize(1);
    assemblyDbi->calculateCoverage(id, region, c, os);
    CHECK_NO_ERROR(os);
    int res = c.first();
    CHECK_TRUE(res == 1, "incorrect calculate Coverage");
}

void AssemblyDbiUnitTests_calculateCoverageInvalid::Test() {
    U2AssemblyDbi* assemblyDbi = AssemblyTestData::getAssemblyDbi();
    APITestData testData;
    testData.addValue(INVALID_ASSEMBLY_ID, QByteArray("zZÿÿ"));

    const U2DataId& id = testData.getValue<U2DataId>(INVALID_ASSEMBLY_ID);
    const U2Region& region = U2Region(20, 1);
    U2AssemblyCoverageStat c;
    U2OpStatusImpl os;
    assemblyDbi->calculateCoverage(id, region, c, os);
    CHECK_TRUE(os.hasError(), "error should be thrown");
}

} //namespace
