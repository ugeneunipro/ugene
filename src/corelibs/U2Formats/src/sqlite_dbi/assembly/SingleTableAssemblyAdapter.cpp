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

#include "SingleTableAssemblyAdapter.h"
#include "../SQLiteDbi.h"
#include "../SQLiteObjectDbi.h"

#include <U2Core/U2AssemblyUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SqlHelpers.h>

namespace U2 {

// ?1 -> end of the region
// ?2 -> start of the region  (- max read size)
// ?3 -> real start of the region
#define DEFAULT_RANGE_CONDITION_CHECK       QString(" (gstart < ?1 AND gstart + elen > ?2) ")
#define RTM_RANGE_CONDITION_CHECK           QString(" ((gstart < ?1 AND gstart > ?2) AND gstart + elen > ?3) ")
#define RTM_RANGE_CONDITION_CHECK_COUNT     QString("  (gstart < ?1 AND gstart > ?2) ")
#define ALL_READ_FIELDS                     QString(" id, prow, gstart, elen, flags, mq, data")
#define SORTED_READS                        QString(" ORDER BY gstart ASC ")


SingleTableAssemblyAdapter::SingleTableAssemblyAdapter(SQLiteDbi* _dbi, const U2DataId& assemblyId,
                                                       char tablePrefix, const QString& tableSuffix,
                                                       const AssemblyCompressor* compressor,
                                                       DbRef* db, U2OpStatus& )
                                                       : SQLiteAssemblyAdapter(assemblyId, compressor, db)
{
    dbi = _dbi;
    rangeConditionCheck  = DEFAULT_RANGE_CONDITION_CHECK;
    rangeConditionCheckForCount = DEFAULT_RANGE_CONDITION_CHECK;
    readsTable = getReadsTableName(assemblyId, tablePrefix, tableSuffix);
    rangeMode = false;
    minReadLength = 0;
    maxReadLength = 0;
}

QString SingleTableAssemblyAdapter::getReadsTableName(const U2DataId& assemblyId, char prefix, const QString& suffix) {
    return QString("AssemblyRead_%1%2_%3").arg(prefix).arg(U2DbiUtils::toDbiId(assemblyId)).arg(suffix);
}

void SingleTableAssemblyAdapter::enableRangeTableMode(int minLen, int maxLen) {
    rangeMode = true;
    minReadLength = minLen;
    maxReadLength = maxLen;
    rangeConditionCheck = RTM_RANGE_CONDITION_CHECK;
    rangeConditionCheckForCount = RTM_RANGE_CONDITION_CHECK_COUNT;
}

void SingleTableAssemblyAdapter::createReadsTables(U2OpStatus& os) {
    // id - id of the read
    // name - read name hash
    // prow - packed view row
    // gstart - start of the read
    // elen - effective length of the read
    // flags - read flags
    // mq - mapping quality
    // data - packed data: CIGAR, read sequence, quality string
    static QString q = "CREATE TABLE IF NOT EXISTS %1 (id INTEGER PRIMARY KEY AUTOINCREMENT, name INTEGER NOT NULL, prow INTEGER NOT NULL, "
        "gstart INTEGER NOT NULL, elen INTEGER NOT NULL, flags INTEGER NOT NULL, mq INTEGER NOT NULL, data BLOB NOT NULL)";

    SQLiteWriteQuery(q.arg(readsTable), db, os).execute();
}

void SingleTableAssemblyAdapter::createReadsIndexes(U2OpStatus& os) {
    static QString q1 = "CREATE INDEX IF NOT EXISTS %1_gstart ON %1(gstart)";
    SQLiteWriteQuery(q1.arg(readsTable), db, os).execute();

    static QString q2 = "CREATE INDEX IF NOT EXISTS %1_name ON %1(name)";
    SQLiteWriteQuery(q2.arg(readsTable), db, os).execute();
}

void SingleTableAssemblyAdapter::dropReadsIndexes(U2OpStatus& os) {
    static QString q1 = "DROP INDEX IF EXISTS %1_gstart";
    SQLiteWriteQuery(q1.arg(readsTable), db, os).execute();

    static QString q2 = "DROP INDEX IF EXISTS %1_name";
    SQLiteWriteQuery(q2.arg(readsTable), db, os).execute();
}


void SingleTableAssemblyAdapter::bindRegion(SQLiteQuery& q, const U2Region& r, bool forCount) {
    if (rangeMode) {
        q.bindInt64(1, r.endPos());
        q.bindInt64(2, r.startPos - maxReadLength);
        if (!forCount) {
            q.bindInt64(3, r.startPos);
        }
    } else {
        q.bindInt64(1, r.endPos());
        q.bindInt64(2, r.startPos);
    }

}

qint64 SingleTableAssemblyAdapter::countReads(const U2Region& r, U2OpStatus& os) {
    if (r == U2_REGION_MAX) {
        return SQLiteReadQuery(QString("SELECT COUNT(*) FROM %1").arg(readsTable), db, os).selectInt64();
    }
    QString qStr = QString("SELECT COUNT(*) FROM %1 WHERE " + rangeConditionCheckForCount).arg(readsTable);
    SQLiteReadQuery q(qStr, db, os);
    bindRegion(q, r, true);
    return q.selectInt64();
}

qint64 SingleTableAssemblyAdapter::countReadsPrecise(const U2Region& r, U2OpStatus& os) {
    if (!rangeMode) {
        return countReads(r, os);
    }
    //here we use not-optimized rangeConditionCheck but not rangeConditionCheckForCount
    QString qStr = QString("SELECT COUNT(*) FROM %1 WHERE " + rangeConditionCheck).arg(readsTable);
    SQLiteReadQuery q(qStr, db, os);
    bindRegion(q, r, false);
    return q.selectInt64();
}

qint64 SingleTableAssemblyAdapter::getMaxPackedRow(const U2Region& r, U2OpStatus& os) {
    SQLiteReadQuery q(QString("SELECT MAX(prow) FROM %1 WHERE " + rangeConditionCheck).arg(readsTable), db, os);
    bindRegion(q, r);
    return q.selectInt64();
}

qint64 SingleTableAssemblyAdapter::getMaxEndPos(U2OpStatus& os) {
    return SQLiteReadQuery(QString("SELECT MAX(gstart + elen) FROM %1").arg(readsTable), db, os).selectInt64();
}

U2DbiIterator<U2AssemblyRead>* SingleTableAssemblyAdapter::getReads(const U2Region& r, U2OpStatus& os, bool sortedHint) {
    QString qStr = QString("SELECT " + ALL_READ_FIELDS + " FROM %1 WHERE " + rangeConditionCheck).arg(readsTable);
    if (sortedHint) {
        qStr += SORTED_READS;
    }

    QSharedPointer<SQLiteReadQuery> q (new SQLiteReadQuery(qStr, db, os));
    bindRegion(*q, r);
    return new SQLiteResultSetIterator<U2AssemblyRead>(q, new SimpleAssemblyReadLoader(), NULL, U2AssemblyRead(), os);
}

U2DbiIterator<U2AssemblyRead>* SingleTableAssemblyAdapter::getReadsByRow(const U2Region& r, qint64 minRow, qint64 maxRow, U2OpStatus& os) {
    int rowFieldPos = rangeMode ? 4 : 3;
    QString qStr = QString("SELECT " + ALL_READ_FIELDS + " FROM %1 WHERE " + rangeConditionCheck
        + " AND (prow >= ?%2 AND prow < ?%3)").arg(readsTable).arg(rowFieldPos).arg(rowFieldPos + 1);
    QSharedPointer<SQLiteReadQuery> q ( new SQLiteReadQuery(qStr, db, os) );
    bindRegion(*q, r);
    q->bindInt64(rowFieldPos, minRow);
    q->bindInt64(rowFieldPos + 1, maxRow);
    return new SQLiteResultSetIterator<U2AssemblyRead>(q, new SimpleAssemblyReadLoader(), NULL, U2AssemblyRead(), os);
}

U2DbiIterator<U2AssemblyRead>* SingleTableAssemblyAdapter::getReadsByName(const QByteArray& name, U2OpStatus& os) {
    QString qStr = QString("SELECT " + ALL_READ_FIELDS + " FROM %1 WHERE name = ?1").arg(readsTable);
    QSharedPointer<SQLiteReadQuery> q (new SQLiteReadQuery(qStr, db, os));
    int hash = qHash(name);
    q->bindInt64(1, hash);
    return new SQLiteResultSetIterator<U2AssemblyRead>(q, new SimpleAssemblyReadLoader(),
        new SQLiteAssemblyNameFilter(name), U2AssemblyRead(), os);
}

void SingleTableAssemblyAdapter::addReads(U2DbiIterator<U2AssemblyRead>* it, U2AssemblyReadsImportInfo& ii, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    QString q = "INSERT INTO %1(name, prow, flags, gstart, elen, mq, data) VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7)";
    SQLiteWriteQuery insertQ(q.arg(readsTable), db, os);
    while (it->hasNext() && !os.isCoR()) {
        U2AssemblyRead read = it->next();
        bool dnaExt = false; //TODO:
        qint64 flags = read->flags;
        flags = flags | (dnaExt ? DnaExtAlphabet : 0);

        if (rangeMode) { //effective read length must be precomputed in this mode
            assert(read->effectiveLen >= minReadLength && read->effectiveLen < maxReadLength);
        } else {
            int readLen = read->readSequence.length();
            int effectiveReadLength = readLen + U2AssemblyUtils::getCigarExtraLength(read->cigar);
            read->effectiveLen = effectiveReadLength;
        }

        int hash = qHash(read->name);
        insertQ.reset();
        insertQ.bindInt64(1, hash);
        insertQ.bindInt64(2, read->packedViewRow);
        insertQ.bindInt64(3, flags);
        insertQ.bindInt64(4, read->leftmostPos);
        insertQ.bindInt64(5, read->effectiveLen);
        insertQ.bindInt32(6, read->mappingQuality);
        QByteArray packedData = SQLiteAssemblyUtils::packData(SQLiteAssemblyDataMethod_NSCQ, read, os);
        insertQ.bindBlob(7, packedData, false);

        insertQ.insert();

        SQLiteAssemblyUtils::addToCoverage(ii.coverageInfo, read);

        ii.nReads++;
    }
}

void SingleTableAssemblyAdapter::removeReads(const QList<U2DataId>& readIds, U2OpStatus& os) {
    //TODO: add transaction per pack or reads
    //TODO: remove multiple reads in 1 SQL at once
    //SQLiteObjectDbi* objDbi = dbi->getSQLiteObjectDbi();
    foreach(U2DataId readId, readIds) {
        SQLiteUtils::remove(readsTable, "id", readId, 1, db, os);
        if (os.hasError()) {
            break;
        }
    }
    SQLiteObjectDbi::incrementVersion(assemblyId, db, os);
}

void SingleTableAssemblyAdapter::dropReadsTables(U2OpStatus &os) {
    QString queryString = "DROP TABLE IF EXISTS %1";
    SQLiteWriteQuery(queryString.arg(readsTable), db, os).execute();
    CHECK_OP(os, );
    SQLiteObjectDbi::incrementVersion(assemblyId, db, os);
}

void SingleTableAssemblyAdapter::pack(U2AssemblyPackStat& stat, U2OpStatus& os) {
    SingleTablePackAlgorithmAdapter packAdapter(db, readsTable);
    AssemblyPackAlgorithm::pack(packAdapter, stat, os);
}

void SingleTableAssemblyAdapter::calculateCoverage(const U2Region& r, U2AssemblyCoverageStat& coverage, U2OpStatus& os) {
    QString queryString = "SELECT gstart, elen, data FROM " + readsTable;
    bool rangeArgs = r != U2_REGION_MAX;

    if (rangeArgs) {
        queryString+=" WHERE " + rangeConditionCheck;
    }
    SQLiteReadQuery q(queryString, db, os);
    if (rangeArgs) {
        bindRegion(q, r, false);
    }
    SQLiteAssemblyUtils::calculateCoverage(q, r, coverage, os);
}

//////////////////////////////////////////////////////////////////////////
// pack adapter

U2DbiIterator<PackAlgorithmData>* SingleTablePackAlgorithmAdapter::selectAllReads(U2OpStatus& os) {
    QSharedPointer<SQLiteReadQuery> q (new SQLiteReadQuery("SELECT id, gstart, elen FROM " + readsTable + " ORDER BY gstart", db, os));
    return new SQLiteResultSetIterator<PackAlgorithmData>(q, new SimpleAssemblyReadPackedDataLoader(), NULL, PackAlgorithmData(), os);
}

SingleTablePackAlgorithmAdapter::~SingleTablePackAlgorithmAdapter() {
    releaseDbResources();
}

void SingleTablePackAlgorithmAdapter::assignProw(const U2DataId& readId, qint64 prow, U2OpStatus& os) {
    if (updateQuery == NULL) {
        updateQuery = new SQLiteWriteQuery("UPDATE " + readsTable + " SET prow = ?1 WHERE id = ?2", db, os);
    }
    updateQuery->setOpStatus(os);
    updateQuery->reset();
    updateQuery->bindInt64(1, prow);
    updateQuery->bindDataId(2, readId);
    updateQuery->execute();
}

void SingleTablePackAlgorithmAdapter::releaseDbResources() {
    delete updateQuery;
    updateQuery = NULL;
}

} //namespace
