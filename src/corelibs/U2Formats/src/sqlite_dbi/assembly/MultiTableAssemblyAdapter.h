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

#ifndef _U2_SQLITE_ASSEMBLY_MULTI_TABLE_DBI_H_
#define _U2_SQLITE_ASSEMBLY_MULTI_TABLE_DBI_H_

#include "SingleTableAssemblyAdapter.h"
#include "util/AssemblyPackAlgorithm.h"

#include <U2Core/U2SqlHelpers.h>

#include <QReadWriteLock>

namespace U2 {

class MTASingleTableAdapter {
public:
    /** Wrapper over 1 table in database. If singleTableAdapter == NULL the table was not created yet */
    MTASingleTableAdapter(SingleTableAssemblyAdapter* a, int _rowPos, int _elenPos, const QByteArray& extra)
        : singleTableAdapter(a), rowPos(_rowPos), elenPos(_elenPos), idExtra(extra) {}

    SingleTableAssemblyAdapter* singleTableAdapter;
    int                         rowPos;
    int                         elenPos;
    QByteArray                  idExtra;
};

class MultiTableAssemblyAdapter : public SQLiteAssemblyAdapter {
public:
    MultiTableAssemblyAdapter(SQLiteDbi* dbi, const U2DataId& assemblyId, const AssemblyCompressor* compressor, DbRef* ref, U2OpStatus& os);

    ~MultiTableAssemblyAdapter();

    virtual qint64 countReads(const U2Region& r, U2OpStatus& os);

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

    virtual void createReadsIndexes(U2OpStatus& os);

    int getElenRangePosByLength(qint64 readLength) const;
    int getElenRangePosById(const U2DataId& id) const;
    int getNumberOfElenRanges() const {return elenRanges.size();}

    int getRowRangePosByRow(quint64 row) const;
    int getRowRangePosById(const U2DataId& id) const;
    int getRowsPerRange() const {return rowsPerRange;}

    const QVector<MTASingleTableAdapter*>& getAdapters() const {return adapters;}
    const QVector<QByteArray>& getIdExtrasPerRange() const {return idExtras;}

    DbRef* getDbRef() const {return dbi->getDbRef();}

    MTASingleTableAdapter* getAdapterByRowAndElenRange(int rowRange, int elenRange, bool createIfNotExits, U2OpStatus& os);

protected:

    QString getTableSuffix(int rowRange, int elenRange);
    static QByteArray getIdExtra(int rowRange, int elenRange);

    void addTableAdapter(int minLen, int maxLen, const U2DataId& assemblyId, const AssemblyCompressor* compressor, bool last, U2OpStatus& os);

    /** Checks if table info must be re-read from DB and calls re-read if needed */
    void syncTables(U2OpStatus& os);

    /** For a new and empty assembly analyzes reads data and calculate ranges */
    void initTables(const QList<U2AssemblyRead>& reads, U2OpStatus& os);

    /** Re-reads table info from DB */
    void rereadTables(const QByteArray& idata, U2OpStatus& os);

    /** Flushes tables info into database */
    void flushTables(U2OpStatus& os);

    void clearTableAdaptersInfo();

    MTASingleTableAdapter* createAdapter(int rowRange, int elenRange, U2OpStatus& os);

    void initAdaptersGrid(int nRows, int nRanges);

    SQLiteDbi*                                  dbi;

    /** All non-NUL adapters */
    QVector<MTASingleTableAdapter*>             adapters;

    /** outer dim -> prow, inner dim -> elen */
    QVector< QVector<MTASingleTableAdapter*> >  adaptersGrid;

    /** id extras for every table, same hierarchy with tableAdapters */
    QVector<QByteArray>                         idExtras;

    /** assembly object version adapters are used for */
    qint32                                      version;

    /** effective length ranges */
    QVector<U2Region>                           elenRanges;

    /** prow range per table */
    qint32                                      rowsPerRange;

    //TODO: add read-locks into all methods
    QReadWriteLock                              tablesSyncLock;
};

class SQLiteReadTableMigrationData {
public:
    SQLiteReadTableMigrationData() : readId (-1), oldTable(NULL), newProw(-1){}
    SQLiteReadTableMigrationData(qint64 oldId, MTASingleTableAdapter* oldT, int newP)
        : readId(oldId), oldTable(oldT), newProw(newP) {}

    qint64                  readId;
    MTASingleTableAdapter*  oldTable;
    int                     newProw;
};

class MultiTablePackAlgorithmAdapter : public PackAlgorithmAdapter {
public:
    MultiTablePackAlgorithmAdapter(MultiTableAssemblyAdapter* a);
    ~MultiTablePackAlgorithmAdapter();

    virtual U2DbiIterator<PackAlgorithmData>* selectAllReads(U2OpStatus& os);
    virtual void assignProw(const U2DataId& readId, qint64 prow, U2OpStatus& os);

    void releaseDbResources();
    void migrateAll(U2OpStatus& os);

private:
    void ensureGridSize(int nRows);
    void migrate(MTASingleTableAdapter* newA, const QVector<SQLiteReadTableMigrationData>& data, qint64 migratedBefore, qint64 totalMigrationCount, U2OpStatus& os);

    MultiTableAssemblyAdapter*                              multiTableAdapter;
    QVector<SingleTablePackAlgorithmAdapter*>               packAdapters;
    QVector< QVector<SingleTablePackAlgorithmAdapter*> >    packAdaptersGrid;
    QHash<MTASingleTableAdapter*, QVector<SQLiteReadTableMigrationData> > migrations;
};

// Class that multiplexes multiple read iterators into 1
class MTAReadsIterator : public U2DbiIterator<U2AssemblyRead> {
public:
    MTAReadsIterator(QVector< U2DbiIterator<U2AssemblyRead>* >& iterators, const QVector<QByteArray>& idExtras, bool sortedHint);

    virtual ~MTAReadsIterator();

    virtual bool hasNext();

    virtual U2AssemblyRead next();

    virtual U2AssemblyRead peek();

private:
    QVector<U2DbiIterator<U2AssemblyRead>*> iterators;
    int                                     currentRange;
    QVector<QByteArray>                     idExtras;
    bool                                    sortedHint;
};

// Class that multiplexes multiple read packed data iterators into 1 and supports ordering
class MTAPackAlgorithmDataIterator : public U2DbiIterator<PackAlgorithmData> {
public:
    MTAPackAlgorithmDataIterator(QVector< U2DbiIterator<PackAlgorithmData>* >& iterators, const QVector<QByteArray>& idExtras);

    virtual ~MTAPackAlgorithmDataIterator();

    virtual bool hasNext();

    virtual PackAlgorithmData next();

    virtual PackAlgorithmData peek();

private:
    void fetchNextData();

    QVector<U2DbiIterator<PackAlgorithmData>*>      iterators;
    PackAlgorithmData                               nextData;
    QVector<QByteArray>                             idExtras;
};

} //namespace

#endif
