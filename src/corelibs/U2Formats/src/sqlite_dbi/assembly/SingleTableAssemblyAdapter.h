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

#ifndef _U2_SQLITE_ASSEMBLY_SINGLE_TABLE_DBI_H_
#define _U2_SQLITE_ASSEMBLY_SINGLE_TABLE_DBI_H_

#include "../SQLiteAssemblyDbi.h"
#include "util/AssemblyPackAlgorithm.h"

#include <U2Core/U2SqlHelpers.h>

namespace U2 {


class SingleTableAssemblyAdapter : public SQLiteAssemblyAdapter {
public:
    SingleTableAssemblyAdapter(SQLiteDbi* dbi, const U2DataId& assemblyId, char tablePrefix, const QString& tableSuffix,
        const AssemblyCompressor* compressor, DbRef* ref, U2OpStatus& os);

    virtual void createReadsTables(U2OpStatus& os);
    virtual void createReadsIndexes(U2OpStatus& os);

    virtual qint64 countReads(const U2Region& r, U2OpStatus& os);
    virtual qint64 countReadsPrecise(const U2Region& r, U2OpStatus& os);

    virtual qint64 getMaxPackedRow(const U2Region& r, U2OpStatus& os);
    virtual qint64 getMaxEndPos(U2OpStatus& os);

    virtual U2DbiIterator<U2AssemblyRead>* getReads(const U2Region& r, U2OpStatus& os, bool sortedHint = false);
    virtual U2DbiIterator<U2AssemblyRead>* getReadsByRow(const U2Region& r, qint64 minRow, qint64 maxRow, U2OpStatus& os);
    virtual U2DbiIterator<U2AssemblyRead>* getReadsByName(const QByteArray& name, U2OpStatus& os);

    virtual void addReads(U2DbiIterator<U2AssemblyRead>* it, U2AssemblyReadsImportInfo& ii, U2OpStatus& os);
    virtual void removeReads(const QList<U2DataId>& readIds, U2OpStatus& os);
    virtual void dropReadsTables(U2OpStatus& os);

    virtual void pack(U2AssemblyPackStat& stat, U2OpStatus& os);

    virtual void calculateCoverage(const U2Region& region, U2AssemblyCoverageStat& coverage, U2OpStatus& os);

    const QString& getReadsTableName() const {return readsTable;}

    void enableRangeTableMode(int minLength, int maxLength);

    static QString getReadsTableName(const U2DataId& assemblyId, char prefix, const QString& suffix);

    void dropReadsIndexes(U2OpStatus& os);

    qint64 getMinReadLength() const {return minReadLength;}
    qint64 getMaxReadLength() const {return maxReadLength;}

protected:
    void bindRegion(SQLiteQuery& q, const U2Region& r, bool forCount = false);

    SQLiteDbi*  dbi;
    QString     readsTable;
    QString     rangeConditionCheck;
    QString     rangeConditionCheckForCount;
    int         minReadLength; // used in range mode
    int         maxReadLength; // used in range mode
    bool        rangeMode;     // flag to show that range mode is in use
};

class SingleTablePackAlgorithmAdapter : public PackAlgorithmAdapter {
public:
    SingleTablePackAlgorithmAdapter(DbRef* _db, const QString& _readsTable) : db(_db), readsTable(_readsTable), updateQuery(NULL){}
    ~SingleTablePackAlgorithmAdapter();

    virtual U2DbiIterator<PackAlgorithmData>* selectAllReads(U2OpStatus& os);
    virtual void assignProw(const U2DataId& readId, qint64 prow, U2OpStatus& os);

    void releaseDbResources();
private:
    DbRef*          db;
    QString         readsTable;
    SQLiteWriteQuery*    updateQuery;
};


} //namespace

#endif
