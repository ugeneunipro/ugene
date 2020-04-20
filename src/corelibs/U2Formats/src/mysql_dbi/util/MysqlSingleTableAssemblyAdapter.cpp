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

#include <U2Core/U2AssemblyUtils.h>
#include <U2Core/U2SafePoints.h>

#include "MysqlAssemblyUtils.h"
#include "MysqlHelpers.h"
#include "MysqlSingleTableAssemblyAdapter.h"
#include "mysql_dbi/MysqlDbi.h"
#include "mysql_dbi/MysqlObjectDbi.h"

namespace U2 {

// :end -> end of the region
// :start -> start of the region  (- max read size)
// :realStart -> real start of the region
const QString MysqlSingleTableAssemblyAdapter::DEFAULT_RANGE_CONDITION_CHECK = " (gstart < :end AND gstart + elen > :start) ";
const QString MysqlSingleTableAssemblyAdapter::RTM_RANGE_CONDITION_CHECK = " ((gstart < :end AND gstart > :start) AND gstart + elen > :realStart) ";
const QString MysqlSingleTableAssemblyAdapter::RTM_RANGE_CONDITION_CHECK_COUNT = "  (gstart < :end AND gstart > :start) ";
const QString MysqlSingleTableAssemblyAdapter::ALL_READ_FIELDS = " id, prow, gstart, elen, flags, mq, data";
const QString MysqlSingleTableAssemblyAdapter::SORTED_READS = " ORDER BY gstart ASC ";

MysqlSingleTableAssemblyAdapter::MysqlSingleTableAssemblyAdapter(MysqlDbi* dbi,
                                                       const U2DataId& assemblyId,
                                                       char tablePrefix,
                                                       const QString& tableSuffix,
                                                       const AssemblyCompressor* compressor,
                                                       MysqlDbRef* db,
                                                       U2OpStatus& ) :
    MysqlAssemblyAdapter(assemblyId, compressor, db),
    dbi(dbi),
    readsTable(getReadsTableName(assemblyId, tablePrefix, tableSuffix)),
    rangeConditionCheck(DEFAULT_RANGE_CONDITION_CHECK),
    rangeConditionCheckForCount(DEFAULT_RANGE_CONDITION_CHECK),
    minReadLength(0),
    maxReadLength(0),
    rangeMode(false),
    inited(false)
{
}

void MysqlSingleTableAssemblyAdapter::createReadsTables(U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    // id - id of the read
    // name - read name hash
    // prow - packed view row
    // gstart - start of the read
    // elen - effective length of the read
    // flags - read flags
    // mq - mapping quality
    // data - packed data: CIGAR, read sequence, quality string
    static QString q = "CREATE TABLE IF NOT EXISTS %1 (id BIGINT PRIMARY KEY AUTO_INCREMENT, name BIGINT NOT NULL, prow BIGINT NOT NULL, "
        "gstart BIGINT NOT NULL, elen BIGINT NOT NULL, flags BIGINT NOT NULL, mq TINYINT UNSIGNED NOT NULL, data LONGBLOB NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=utf8";

    U2SqlQuery(q.arg(readsTable), db, os).execute();
    CHECK_OP(os, );
    inited = true;
}

//! %1 - database, %2 - table name, %3 - index name, %4 - index column
//! Example of working query:
// select if (EXISTS(SELECT distinct index_name FROM INFORMATION_SCHEMA.STATISTICS
// WHERE table_schema = 'uu' AND table_name = 'AssemblyRead_M2_800_4000_0' and index_name like 'AssemblyRead_M2_800_4000_0_name'),
// "SELECT id FROM AssemblyRead_M2_800_4000_0;", "CREATE INDEX AssemblyRead_M2_800_4000_0_name ON uu.AssemblyRead_M2_800_4000_0(name);") into @a;
// prepare smt from @a; execute smt; deallocate prepare smt;
static const QString CREATE_INDEX_IF_NOT_EXISTS_QUERY =
        "select if"
        "("
            "EXISTS(SELECT distinct index_name FROM INFORMATION_SCHEMA.STATISTICS "
            "WHERE table_schema = '%1' AND table_name = '%2' and index_name like '%3')"
        ","
            "\"SELECT %4 FROM %2;\""
        ","
            "\"CREATE INDEX %3 ON %1.%2(%4);\""
        ") into @a; prepare smt from @a; execute smt; deallocate prepare smt;";

void MysqlSingleTableAssemblyAdapter::createReadsIndexes(U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    U2SqlQuery(CREATE_INDEX_IF_NOT_EXISTS_QUERY.arg(db->handle.databaseName())
               .arg(readsTable).arg(readsTable + "_gstart").arg("gstart"), db, os).execute();
    CHECK_OP(os, );

    U2SqlQuery(CREATE_INDEX_IF_NOT_EXISTS_QUERY.arg(db->handle.databaseName())
               .arg(readsTable).arg(readsTable + "_name").arg("name"), db, os).execute();
}

qint64 MysqlSingleTableAssemblyAdapter::countReads(const U2Region& r, U2OpStatus& os) {
    if (r == U2_REGION_MAX) {
        return U2SqlQuery(QString("SELECT COUNT(*) FROM %1").arg(readsTable), db, os).selectInt64();
    }

    static const QString qStr = "SELECT COUNT(*) FROM %1 WHERE %2";
    U2SqlQuery q(qStr.arg(readsTable).arg(rangeConditionCheckForCount), db, os);
    bindRegion(q, r, true);
    return q.selectInt64();
}

qint64 MysqlSingleTableAssemblyAdapter::countReadsPrecise(const U2Region& r, U2OpStatus& os) {
    if (!rangeMode) {
        return countReads(r, os);
    }

    //here we use not-optimized rangeConditionCheck but not rangeConditionCheckForCount
    static const QString qStr = "SELECT COUNT(*) FROM %1 WHERE %2";
    U2SqlQuery q(qStr.arg(readsTable).arg(rangeConditionCheck), db, os);
    bindRegion(q, r, false);
    return q.selectInt64();
}

qint64 MysqlSingleTableAssemblyAdapter::getMaxPackedRow(const U2Region& r, U2OpStatus& os) {
    static const QString queryString = "SELECT MAX(prow) FROM %1 WHERE %2";
    U2SqlQuery q(queryString.arg(readsTable).arg(rangeConditionCheck), db, os);
    bindRegion(q, r);
    return q.selectInt64();
}

qint64 MysqlSingleTableAssemblyAdapter::getMaxEndPos(U2OpStatus& os) {
    static const QString queryString = "SELECT MAX(gstart + elen) FROM %1";
    return U2SqlQuery(queryString.arg(readsTable), db, os).selectInt64();
}

U2DbiIterator<U2AssemblyRead>* MysqlSingleTableAssemblyAdapter::getReads(const U2Region& r, U2OpStatus& os, bool sortedHint) {
    static const QString qStr = "SELECT " + ALL_READ_FIELDS + " FROM %1 WHERE %2";

    QSharedPointer<U2SqlQuery> q (new U2SqlQuery(qStr.arg(readsTable).arg(rangeConditionCheck) + (sortedHint ? SORTED_READS : ""), db, os));
    bindRegion(*q, r);
    return new MysqlRSIterator<U2AssemblyRead>(q, new MysqlSimpleAssemblyReadLoader(), NULL, U2AssemblyRead(), os);
}

U2DbiIterator<U2AssemblyRead>* MysqlSingleTableAssemblyAdapter::getReadsByRow(const U2Region& r, qint64 minRow, qint64 maxRow, U2OpStatus& os) {
    static const QString qStr = "SELECT " + ALL_READ_FIELDS + " FROM %1 WHERE %2 AND (prow >= :minRow AND prow < :maxRow)";
    QSharedPointer<U2SqlQuery> q (new U2SqlQuery(qStr.arg(readsTable).arg(rangeConditionCheck), db, os));
    bindRegion(*q, r);
    q->bindInt64(":minRow", minRow);
    q->bindInt64(":maxRow", maxRow);
    return new MysqlRSIterator<U2AssemblyRead>(q, new MysqlSimpleAssemblyReadLoader(), NULL, U2AssemblyRead(), os);
}

U2DbiIterator<U2AssemblyRead>* MysqlSingleTableAssemblyAdapter::getReadsByName(const QByteArray& name, U2OpStatus& os) {
    static const QString qStr = "SELECT " + ALL_READ_FIELDS + " FROM %1 WHERE name = :name";
    QSharedPointer<U2SqlQuery> q (new U2SqlQuery(qStr.arg(readsTable), db, os));
    int hash = qHash(name);
    q->bindInt64(":name", hash);
    return new MysqlRSIterator<U2AssemblyRead>(q, new MysqlSimpleAssemblyReadLoader(),
        new MysqlAssemblyNameFilter(name), U2AssemblyRead(), os);
}

void MysqlSingleTableAssemblyAdapter::addReads(U2DbiIterator<U2AssemblyRead>* it, U2AssemblyReadsImportInfo& ii, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    if (!inited) {
        createReadsTables(os);
    }

    static const QString q = "INSERT INTO %1(name, prow, flags, gstart, elen, mq, data) VALUES (:name, :prow, :flags, :gstart, :elen, :mq, :data)";

    while (it->hasNext() && !os.isCoR()) {
        U2SqlQuery insertQ(q.arg(readsTable), db, os);

        U2AssemblyRead read = it->next();
        bool dnaExt = false; //TODO:
        qint64 flags = read->flags;
        flags = flags | (dnaExt ? DnaExtAlphabet : 0);

        if (rangeMode) {
            SAFE_POINT(read->effectiveLen >= minReadLength && read->effectiveLen < maxReadLength, "Effective read length must be precomputed in the range mode", );
        } else {
            int readLen = read->readSequence.length();
            int effectiveReadLength = readLen + U2AssemblyUtils::getCigarExtraLength(read->cigar);
            read->effectiveLen = effectiveReadLength;
        }

        int hash = qHash(read->name);
        QByteArray packedData = MysqlAssemblyUtils::packData(MysqlAssemblyDataMethod_NSCQ, read, os);

        insertQ.bindInt64(":name", hash);
        insertQ.bindInt64(":prow", read->packedViewRow);
        insertQ.bindInt64(":flags", flags);
        insertQ.bindInt64(":gstart", read->leftmostPos);
        insertQ.bindInt64(":elen", read->effectiveLen);
        insertQ.bindInt32(":mq", read->mappingQuality);
        insertQ.bindBlob(":data", packedData);
        insertQ.insert();
        CHECK_OP(os, );

        MysqlAssemblyUtils::addToCoverage(ii.coverageInfo, read);

        ii.nReads++;
        ii.onReadImported();
    }
}

void MysqlSingleTableAssemblyAdapter::removeReads(const QList<U2DataId>& readIds, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    //TODO: add transaction per pack or reads
    //TODO: remove multiple reads in 1 SQL at once
    //SQLiteObjectDbi* objDbi = dbi->getSQLiteObjectDbi();
    foreach(U2DataId readId, readIds) {
        MysqlUtils::remove(readsTable, "id", readId, 1, db, os);
        CHECK_OP_BREAK(os);
    }

    MysqlObjectDbi::incrementVersion(assemblyId, db, os);
}

void MysqlSingleTableAssemblyAdapter::dropReadsTables(U2OpStatus &os) {
    static QString queryString = "DROP TABLE IF EXISTS %1";
    U2SqlQuery(queryString.arg(readsTable), db, os).execute();
    CHECK_OP(os, );
    MysqlObjectDbi::incrementVersion(assemblyId, db, os);
}

void MysqlSingleTableAssemblyAdapter::pack(U2AssemblyPackStat& stat, U2OpStatus& os) {
    MysqlSingleTablePackAlgorithmAdapter packAdapter(db, readsTable);
    AssemblyPackAlgorithm::pack(packAdapter, stat, os);
    createReadsIndexes(os);
}

void MysqlSingleTableAssemblyAdapter::calculateCoverage(const U2Region& r, U2AssemblyCoverageStat& coverage, U2OpStatus& os) {
    QString queryString = "SELECT gstart, elen, data FROM " + readsTable;
    bool rangeArgs = (r != U2_REGION_MAX);

    if (rangeArgs) {
        queryString += " WHERE " + rangeConditionCheck;
    }

    U2SqlQuery q(queryString, db, os);

    if (rangeArgs) {
        bindRegion(q, r, false);
    }

    MysqlAssemblyUtils::calculateCoverage(q, r, coverage, os);
}

const QString& MysqlSingleTableAssemblyAdapter::getReadsTableName() const {
    return readsTable;
}

void MysqlSingleTableAssemblyAdapter::enableRangeTableMode(int minLen, int maxLen) {
    rangeMode = true;
    minReadLength = minLen;
    maxReadLength = maxLen;
    rangeConditionCheck = RTM_RANGE_CONDITION_CHECK;
    rangeConditionCheckForCount = RTM_RANGE_CONDITION_CHECK_COUNT;
}

QString MysqlSingleTableAssemblyAdapter::getReadsTableName(const U2DataId& assemblyId, char prefix, const QString& suffix) {
    return QString("AssemblyRead_%1%2_%3").arg(prefix).arg(U2DbiUtils::toDbiId(assemblyId)).arg(suffix);;
}

void MysqlSingleTableAssemblyAdapter::dropReadsIndexes(U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString q1 = "DROP INDEX IF EXISTS %1_gstart";
    U2SqlQuery(q1.arg(readsTable), db, os).execute();

    static const QString q2 = "DROP INDEX IF EXISTS %1_name";
    U2SqlQuery(q2.arg(readsTable), db, os).execute();
}

qint64 MysqlSingleTableAssemblyAdapter::getMinReadLength() const {
    return minReadLength;
}

qint64 MysqlSingleTableAssemblyAdapter::getMaxReadLength() const {
    return maxReadLength;
}

void MysqlSingleTableAssemblyAdapter::bindRegion(U2SqlQuery& q, const U2Region& r, bool forCount) {
    if (rangeMode) {
        q.bindInt64(":end", r.endPos());
        q.bindInt64(":start", r.startPos - maxReadLength);
        if (!forCount) {
            q.bindInt64(":realStart", r.startPos);
        }
    } else {
        q.bindInt64(":end", r.endPos());
        q.bindInt64(":start", r.startPos);
    }
}

//////////////////////////////////////////////////////////////////////////
// pack adapter

MysqlSingleTablePackAlgorithmAdapter::MysqlSingleTablePackAlgorithmAdapter(MysqlDbRef* db, const QString& readsTable) :
    db(db),
    readsTable(readsTable),
    updateQuery(NULL)
{
}

MysqlSingleTablePackAlgorithmAdapter::~MysqlSingleTablePackAlgorithmAdapter() {
    releaseDbResources();
}

U2DbiIterator<PackAlgorithmData>* MysqlSingleTablePackAlgorithmAdapter::selectAllReads(U2OpStatus& os) {
    QSharedPointer<U2SqlQuery> q (new U2SqlQuery("SELECT id, gstart, elen FROM " + readsTable + " ORDER BY gstart", db, os));
    return new MysqlRSIterator<PackAlgorithmData>(q, new MysqlSimpleAssemblyReadPackedDataLoader(), NULL, PackAlgorithmData(), os);
}

void MysqlSingleTablePackAlgorithmAdapter::assignProw(const U2DataId& readId, qint64 prow, U2OpStatus& os) {
    if (updateQuery != NULL) {
        delete updateQuery;
    }

    updateQuery = new U2SqlQuery("UPDATE " + readsTable + " SET prow = :prow WHERE id = :id", db, os);
    updateQuery->bindInt64(":prow", prow);
    updateQuery->bindDataId(":id", readId);
    updateQuery->execute();
}

void MysqlSingleTablePackAlgorithmAdapter::releaseDbResources() {
    delete updateQuery;
    updateQuery = NULL;
}

} //namespace
