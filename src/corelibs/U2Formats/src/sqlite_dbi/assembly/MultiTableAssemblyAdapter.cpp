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

#include "MultiTableAssemblyAdapter.h"
#include "../SQLiteDbi.h"
#include "../SQLiteAssemblyDbi.h"
#include "../SQLiteObjectDbi.h"

#include <U2Core/U2AssemblyUtils.h>
#include <U2Core/U2SqlHelpers.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/Timer.h>

namespace U2 {

#define DEFAULT_ROWS_PER_TABLE 5000

MultiTableAssemblyAdapter::MultiTableAssemblyAdapter(SQLiteDbi* _dbi, const U2DataId& assemblyId,
                                                       const AssemblyCompressor* compressor,
                                                       DbRef* db, U2OpStatus& os)
                                                       : SQLiteAssemblyAdapter(assemblyId, compressor, db)
{
    dbi = _dbi;
    version = -1;
    syncTables(os);
    rowsPerRange = DEFAULT_ROWS_PER_TABLE;
}

MultiTableAssemblyAdapter::~MultiTableAssemblyAdapter() {
    clearTableAdaptersInfo();
}

void MultiTableAssemblyAdapter::clearTableAdaptersInfo() {
    qDeleteAll(adapters);
    adaptersGrid.clear();
    idExtras.clear();
    elenRanges.clear();
}

void MultiTableAssemblyAdapter::syncTables(U2OpStatus& os) {
    qint64 versionInDb = dbi->getObjectDbi()->getObjectVersion(assemblyId, os);
    if (versionInDb <= version) {
        return;
    }
    SQLiteReadQuery q("SELECT idata FROM Assembly WHERE object = ?1", db, os);
    q.bindDataId(1, assemblyId);
    if (q.step()) {
        QByteArray data = q.getBlob(0);
        rereadTables(data, os);
        if (!os.hasError()) {
            version = versionInDb;
        }
    }
}

static QVector<U2Region> toRange(const QVector<int>& startPos) {
    QVector<U2Region> res;
    int prev = 0;
    foreach(int pos, startPos){
        res << U2Region(prev, pos - prev);
        prev = pos;
    }
    return res;
}

void MultiTableAssemblyAdapter::initTables(const QList<U2AssemblyRead>& reads, U2OpStatus& os) {
    if (os.hasError()) {
        return;
    }
    SAFE_POINT(elenRanges.isEmpty(), "Effective ranges are already initialized!", );

    int nReads = reads.size();
    if (false && nReads > 1000) {
    /*    // get reads distribution first
        QVector<int> distribution(nReads / 10, 0);
        int* data = distribution.data();
        foreach(const U2AssemblyRead& read, reads) {
            int elen = read->readSequence.size() + U2AssemblyUtils::getCigarExtraLength(read->cigar);
            int idx = elen / 10;
            data[idx]++;
        }
        // derive regions
        // TODO:*/
    } else {
        QVector<int> starts;
        starts << 50 << 100 << 200 << 400 << 800 << 4*1000 << 25*1000 << 100*1000 << 500*1000 << 2*1000*1000;
        elenRanges << toRange( starts);
    }

    initAdaptersGrid(1, elenRanges.size());

    flushTables(os);
}

void MultiTableAssemblyAdapter::rereadTables(const QByteArray& idata, U2OpStatus& os) {
    QWriteLocker wl(&tablesSyncLock);

    clearTableAdaptersInfo();

    // format: N, N, N, N | N, N |.....
    // elements are separated by | sign. First element encodes ranges, second prow step and max prow, others are for future extension
    if (idata.isEmpty()) {
        //assembly is empty - no index data was created
        return;
    }
    QList<QByteArray> elements = idata.split('|');
    if (elements.size() < 2) {
        os.setError(U2DbiL10n::tr("Failed to detect assembly storage format: %1").arg(idata.constData()));
        return;
    }
    QByteArray elenData = elements.at(0);
    QByteArray prowData = elements.at(1);


    QList<QByteArray> elenTokens = elenData.split(',');
    U2Region prev(-1, 1);
    bool parseOk = true;
    foreach(const QByteArray& elenTok, elenTokens) {
        int start = elenTok.toInt(&parseOk);
        if (!parseOk || start < prev.endPos()) {
            os.setError(QString("Failed to parse range: %1, full: %2").arg(elenTok.constData()).arg(elenData.constData()));
            return;
        }
        U2Region region(prev.endPos(), start - prev.endPos());
        elenRanges << region;
        prev = region;
    }
    elenRanges << U2Region(prev.endPos(), INT_MAX);

    QList<QByteArray> prowTokens = prowData.split(',');
    int prange = prowTokens.at(0).toInt(&parseOk);
    if (prange < 1 || !parseOk) {
        os.setError(U2DbiL10n::tr("Failed to parse packed row range info %1").arg(idata.constData()));
        return;
    }
    if (prowTokens.size() != 2) {
        os.setError(U2DbiL10n::tr("Failed to parse packed row range info %1").arg(idata.constData()));
        return;
    }
    int nRows = prowTokens.at(1).toInt(&parseOk);
    if (nRows < 0 || !parseOk) {
        os.setError(U2DbiL10n::tr("Failed to parse packed row range info %1").arg(idata.constData()));
        return;
    }

    // ok, all regions parsed, now create adapters
    int nElens = elenRanges.size();
    initAdaptersGrid(nRows, nElens);
    for (int rowPos = 0; rowPos < nRows; rowPos++) {
        for (int elenPos = 0; elenPos < nElens; elenPos++) {
            QString suffix = getTableSuffix(rowPos, elenPos);
            QString tableName = SingleTableAssemblyAdapter::getReadsTableName(assemblyId, 'M', suffix);
            if (SQLiteUtils::isTableExists(tableName, db, os)) {
                createAdapter(rowPos, elenPos, os);
            }
        }
    }
}

void MultiTableAssemblyAdapter::flushTables(U2OpStatus& os)  {
    bool empty = adaptersGrid.isEmpty();
    if (empty) {
        // TODO: fetch some reads for analysis. By now not needed, since regions are hard-coded anyway
        initTables(QList<U2AssemblyRead>(), os);
        if (os.hasError()) {
            return;
        }
    }
    QByteArray idata;
    for(int i = 0; i < elenRanges.size(); i++) {
        int rangeStart = elenRanges[i].startPos;
        if (!idata.isEmpty()) {
            idata.append(',');
        }
        idata.append(QByteArray::number(rangeStart));
    }
    idata.append('|').append(QByteArray::number(rowsPerRange)).append(',').append(QByteArray::number(adaptersGrid.size()));

    SQLiteWriteQuery q("UPDATE Assembly SET idata = ?1 WHERE object = ?2", db, os);
    q.bindBlob(1, idata);
    q.bindDataId(2, assemblyId);
    q.execute();
}


QString MultiTableAssemblyAdapter::getTableSuffix(int rowPos, int elenPos) {
    U2Region eRegion = elenRanges[elenPos];
    bool last = (elenPos + 1== elenRanges.size());
    return QString("%1_%2_%3").arg(eRegion.startPos).arg(last ? QString("U") : QString::number(eRegion.endPos())).arg(rowPos);
}

void MultiTableAssemblyAdapter::initAdaptersGrid(int nRows, int nElens) {
    assert(adaptersGrid.isEmpty());
    adaptersGrid.resize(nRows);
    for (int i = 0; i < nRows; i++) {
        adaptersGrid[i] = QVector<MTASingleTableAdapter*>(nElens, NULL);
    }
}

MTASingleTableAdapter* MultiTableAssemblyAdapter::createAdapter(int rowPos, int elenPos, U2OpStatus& os) {
    assert(adaptersGrid.at(rowPos).at(elenPos) == NULL);

    QString suffix = getTableSuffix(rowPos, elenPos);
    SingleTableAssemblyAdapter* sa = new SingleTableAssemblyAdapter(dbi, assemblyId, 'M', suffix, compressor, db, os);
    const U2Region& elenRange = elenRanges.at(elenPos);
    sa->enableRangeTableMode(elenRange.startPos, elenRange.endPos());
    QByteArray idExtra = getIdExtra(rowPos, elenPos);
    MTASingleTableAdapter * ma = new MTASingleTableAdapter(sa, rowPos, elenPos, idExtra);
    ma->singleTableAdapter->createReadsTables(os);
    adapters << ma;
    idExtras << idExtra;
    adaptersGrid[rowPos][elenPos] =  ma;
    return ma;
}

void MultiTableAssemblyAdapter::createReadsIndexes(U2OpStatus& os) {
    SQLiteWriteQuery("PRAGMA temp_store = FILE", db, os).execute();
    CHECK_OP(os, );
    foreach(MTASingleTableAdapter* a, adapters) {
        a->singleTableAdapter->createReadsIndexes(os);
        if (os.hasError()) {
            break;
        }
    }
    SQLiteWriteQuery("PRAGMA temp_store = MEMORY", db, os).execute();
}

QByteArray MultiTableAssemblyAdapter::getIdExtra(int rowPos, int elenPos) {
    QByteArray res(4, 0);
    qint16* data = (qint16*)res.data();
    data[0] = (qint16)rowPos;
    data[1] = (qint16)elenPos;
    return res;
}



qint64 MultiTableAssemblyAdapter::countReads(const U2Region& r, U2OpStatus& os) {
    bool all = r == U2_REGION_MAX;
    qint64 sum = 0;
    // use more sensitive algorithm for smaller regions with low amount of reads
    // and not-very sensitive for huge regions with a lot of reads
    int nReadsToUseNotPreciseAlgorithms = 1000 / (r.length + 1);
    foreach(MTASingleTableAdapter* a, adapters) {
        int n = a->singleTableAdapter->countReads(r, os);
        if (n != 0 && !all && n < nReadsToUseNotPreciseAlgorithms) {
            n = a->singleTableAdapter->countReadsPrecise(r, os);
        }
        if (os.hasError()) {
            break;
        }
        sum+=n;
    }
    return sum;
}

qint64 MultiTableAssemblyAdapter::getMaxPackedRow(const U2Region& r, U2OpStatus& os) {
    qint64 max = 0;
    // process only hi row adapters
    int nRows = adaptersGrid.size();
    for (int rowPos = nRows; --rowPos>=0 && max == 0;) {
        QVector<MTASingleTableAdapter*> elenAdapters = adaptersGrid.at(rowPos);
        for (int elenPos = 0, nElens = elenAdapters.size(); elenPos < nElens; elenPos++) {
            MTASingleTableAdapter* a = elenAdapters.at(elenPos);
            if (a == NULL) {
                continue;
            }
            assert(a->rowPos == rowPos);
            qint64 n = a->singleTableAdapter->getMaxPackedRow(r, os);
            assert(U2Region(rowsPerRange * rowPos, rowsPerRange).contains(n));
            max = qMax(max, n);
        }
    }
    return max;
}

qint64 MultiTableAssemblyAdapter::getMaxEndPos(U2OpStatus& os) {
    //TODO: optimize by using gstart + maxReadLen for first n-1 tables
    qint64 max = 0;
    foreach(MTASingleTableAdapter* a, adapters) {
        qint64 n = a->singleTableAdapter->getMaxEndPos(os);
        if (os.hasError()) {
            break;
        }
        max = qMax(max, n);
    }
    return max;
}

U2DbiIterator<U2AssemblyRead>* MultiTableAssemblyAdapter::getReads(const U2Region& r, U2OpStatus& os, bool sortedHint) {
    QVector< U2DbiIterator<U2AssemblyRead>* > iterators;
    foreach(MTASingleTableAdapter* a, adapters) {
        iterators << a->singleTableAdapter->getReads(r, os, sortedHint);
        if (os.hasError()) {
            break;
        }
    }
    if (os.hasError()) {
        qDeleteAll(iterators);
        return NULL;
    }
    return new MTAReadsIterator(iterators, idExtras, sortedHint);
}

U2DbiIterator<U2AssemblyRead>* MultiTableAssemblyAdapter::getReadsByRow(const U2Region& r, qint64 minRow, qint64 maxRow, U2OpStatus& os) {
    QVector< U2DbiIterator<U2AssemblyRead>* > iterators;
    QVector<QByteArray> selectedIdExtras;
    U2Region targetRowRange(minRow, maxRow - minRow);
    foreach(MTASingleTableAdapter* a, adapters) {
        const U2Region rowRegion(a->rowPos * rowsPerRange, rowsPerRange);
        if (!rowRegion.intersects(targetRowRange)) {
            continue;
        }
        iterators << a->singleTableAdapter->getReadsByRow(r, minRow, maxRow, os);
        selectedIdExtras << a->idExtra;
        if (os.hasError()) {
            break;
        }
    }
    if (os.hasError()) {
        qDeleteAll(iterators);
        return NULL;
    }
    return new MTAReadsIterator(iterators, selectedIdExtras, false);
}

U2DbiIterator<U2AssemblyRead>* MultiTableAssemblyAdapter::getReadsByName(const QByteArray& name, U2OpStatus& os) {
    QVector< U2DbiIterator<U2AssemblyRead>* > iterators;
    foreach(MTASingleTableAdapter* a, adapters) {
        iterators << a->singleTableAdapter->getReadsByName(name, os);
        if (os.hasError()) {
            break;
        }
    }
    if (os.hasError()) {
        qDeleteAll(iterators);
        return NULL;
    }
    return new MTAReadsIterator(iterators, idExtras, false);
}

int MultiTableAssemblyAdapter::getElenRangePosById(const U2DataId& id) const {
    QByteArray extra = U2DbiUtils::toDbExtra(id);

    SAFE_POINT(extra.size() == 4, QString("Illegal assembly read ID extra part! HEX: %1").arg(extra.toHex().constData()), -1);

    const qint16* data = (const qint16*)extra.constData();
    return int(data[1]);
}

int MultiTableAssemblyAdapter::getElenRangePosByLength(qint64 readLength) const {
    int nElenRanges = elenRanges.size();
    for (int i = 0; i < nElenRanges; i++) {
        const U2Region& r = elenRanges[i];
        if (r.contains(readLength)) {
            return i;
        }
    }
    FAIL(QString("Read length does not fit any range: %1, number of ranges: %2").arg(readLength).arg(nElenRanges), nElenRanges - 1);
}

int MultiTableAssemblyAdapter::getRowRangePosByRow(quint64 row) const {
    return row / rowsPerRange;
}

int MultiTableAssemblyAdapter::getRowRangePosById(const U2DataId& id) const {
    QByteArray extra = U2DbiUtils::toDbExtra(id);

    SAFE_POINT(extra.size() == 4, QString("Extra part size of assembly read ID is not correct! HEX(Extra): %1").arg(extra.toHex().constData()), -1);

    const qint16* data = (const qint16*)extra.constData();
    return int(data[0]);
}

static U2DataId addTable2Id(const U2DataId& id, const QByteArray& idExtra) {
    assert(U2DbiUtils::toDbExtra(id).isEmpty());
    U2DataId res = U2DbiUtils::toU2DataId(U2DbiUtils::toDbiId(id), U2Type::AssemblyRead, idExtra);
    return res;
}

static void ensureGridSize(QVector <QVector < QList<U2AssemblyRead> > > & grid, int rowPos, int nElens) {
    int oldRows = grid.size();
    if (oldRows > rowPos) {
        return;
    }
    int newRows = rowPos + 1;
    grid.resize(newRows);
    for (int r = oldRows; r < newRows; r++) {
        grid[r].resize(nElens);
    }
}

#define N_READS_TO_FLUSH_TOTAL 100000
#define N_READS_TO_FLUSH_PER_RANGE 10000

void MultiTableAssemblyAdapter::addReads(U2DbiIterator<U2AssemblyRead>* it, U2AssemblyReadsImportInfo& ii, U2OpStatus& os) {
    bool empty = adaptersGrid.isEmpty();
    if (empty) {
        // TODO: fetch some reads for analysis. By now not needed, since regions are hard-coded anyway
        initTables(QList<U2AssemblyRead>(), os);
        if (os.hasError()) {
            return;
        }
    }

    bool packIsOn = empty;
    qint64 prevLeftmostPos = -1;
    PackAlgorithmContext packContext;

    QVector <QVector < QList<U2AssemblyRead> > > readsGrid; //reads sorted by range
    bool lastIteration = false;
    qint64 readsInGrid = 0;
    while(!os.isCoR()) {
        int nElens = elenRanges.size();
        if(it->hasNext()) {
            U2AssemblyRead read = it->next();
            if (os.isCoR()) {
                break;
            }
            int readLen = read->readSequence.length();
            read->effectiveLen = readLen + U2AssemblyUtils::getCigarExtraLength(read->cigar);
            int elenPos = getElenRangePosByLength(read->effectiveLen);

            packIsOn = packIsOn && read->leftmostPos >= prevLeftmostPos;
            read->packedViewRow = packIsOn ?  AssemblyPackAlgorithm::packRead(U2Region(read->leftmostPos, read->effectiveLen), packContext, os): 0;
            int rowPos = getRowRangePosByRow(read->packedViewRow);
            ensureGridSize(readsGrid, rowPos, nElens);
            readsGrid[rowPos][elenPos] << read;
            ++ii.nReads;
            ++readsInGrid;
        } else {
            lastIteration = true;
        }

        int nRows = readsGrid.size();
        if(lastIteration || readsInGrid > N_READS_TO_FLUSH_TOTAL) {
            for (int rowPos = 0; rowPos < nRows && !os.isCoR(); rowPos++) {
                for (int elenPos = 0; elenPos < nElens && !os.isCoR(); elenPos++) {
                    QList<U2AssemblyRead>& rangeReads = readsGrid[rowPos][elenPos];
                    int nRangeReads = rangeReads.size();
                    if (nRangeReads == 0 || (!lastIteration && nRangeReads < N_READS_TO_FLUSH_PER_RANGE)) {
                        continue;
                    }
                    MTASingleTableAdapter* adapter = getAdapterByRowAndElenRange(rowPos, elenPos, true, os);
                    U2AssemblyReadsImportInfo rangeReadsImportInfo;
                    // pass the same coverage info through all adapters to accumulate coverage
                    rangeReadsImportInfo.coverageInfo = ii.coverageInfo;
                    BufferedDbiIterator<U2AssemblyRead> rangeReadsIterator(rangeReads);
                    adapter->singleTableAdapter->addReads(&rangeReadsIterator, rangeReadsImportInfo, os);
                    ii.coverageInfo = rangeReadsImportInfo.coverageInfo;
                    readsInGrid -= rangeReads.size();
                    rangeReads.clear();
                }
            }
        }
        if (lastIteration) {
            break;
        }
    }

    if (packIsOn && !os.hasError()) {
        ii.packStat.readsCount = ii.nReads;
        ii.packStat.maxProw = packContext.maxProw;
        ii.packed = true;
        flushTables(os);
    }
}

MTASingleTableAdapter* MultiTableAssemblyAdapter::getAdapterByRowAndElenRange(int rowPos, int elenPos, bool createIfNotExits, U2OpStatus& os) {
    int nElens = elenRanges.size();
    assert(elenPos < nElens);
    if (rowPos >= adaptersGrid.size()) {
        assert(createIfNotExits);
        if (!createIfNotExits) {
            return NULL;
        }
        int oldRowSize = adaptersGrid.size();
        int newRowSize = rowPos + 1;
        adaptersGrid.resize(newRowSize);
        for (int i = oldRowSize; i < newRowSize; i++) {
            adaptersGrid[i].resize(nElens);
        }
    }
    QVector<MTASingleTableAdapter*> elenAdapters = adaptersGrid.at(rowPos);
    assert(elenAdapters.size() == nElens);
    MTASingleTableAdapter* adapter = elenAdapters.at(elenPos);
    if (adapter == NULL && createIfNotExits) {
        adapter = createAdapter(rowPos, elenPos, os);
    }
    return adapter;
}

void MultiTableAssemblyAdapter::removeReads(const QList<U2DataId>& readIds, U2OpStatus& os) {
    int nReads = readIds.size();

    QHash<MTASingleTableAdapter*, QList<U2DataId> > readsByAdapter;
    for (int i = 0; i < nReads; i++) {
        const U2DataId& readId = readIds[i];
        int rowPos = getRowRangePosById(readId);
        int elenPos = getElenRangePosById(readId);
        MTASingleTableAdapter* a = getAdapterByRowAndElenRange(rowPos, elenPos, false, os);

        SAFE_POINT(a != NULL, QString("No table adapter was found! row: %1, elen: %2").arg(rowPos).arg(elenPos),);

        if (!readsByAdapter.contains(a)) {
            readsByAdapter[a] = QList<U2DataId>();
        }
        readsByAdapter[a].append(readId);
    }
    foreach(MTASingleTableAdapter* a, readsByAdapter.keys()) {
        QList<U2DataId>& rangeReadIds = readsByAdapter[a];
        a->singleTableAdapter->removeReads(rangeReadIds, os);
        //TODO: remove adapters for empty tables. And tables as well
    }
}

void MultiTableAssemblyAdapter::dropReadsTables(U2OpStatus &os) {
    // remove prepared queries to finalize them and prevent SQLite errors on table drop
    db->preparedQueries.clear();

    foreach (QVector<MTASingleTableAdapter*> adaptersVector, adaptersGrid) {
        foreach (MTASingleTableAdapter* adapter, adaptersVector) {
            if (NULL != adapter) {
                adapter->singleTableAdapter->dropReadsTables(os);
            }
        }
    }
}

void MultiTableAssemblyAdapter::pack(U2AssemblyPackStat& stat, U2OpStatus& os) {
    MultiTablePackAlgorithmAdapter packAdapter(this);

    AssemblyPackAlgorithm::pack(packAdapter, stat, os);
    packAdapter.releaseDbResources();

    quint64 t0 = GTimer::currentTimeMicros();
    packAdapter.migrateAll(os);
    perfLog.trace(QString("Assembly: table migration pack time: %1 seconds").arg((GTimer::currentTimeMicros() - t0) / float(1000*1000)));

    flushTables(os);
}

void MultiTableAssemblyAdapter::calculateCoverage(const U2Region& region, U2AssemblyCoverageStat& coverage, U2OpStatus& os) {
    for(int i = 0; i < adapters.size(); ++i) {
        MTASingleTableAdapter * a = adapters.at(i);
        a->singleTableAdapter->calculateCoverage(region, coverage, os);
        if (os.isCoR()) {
            break;
        }

        os.setProgress((double(i + 1) / adapters.size()) * 100);
    }
}

//////////////////////////////////////////////////////////////////////////
// pack adapter

MultiTablePackAlgorithmAdapter::MultiTablePackAlgorithmAdapter(MultiTableAssemblyAdapter* ma) {
    multiTableAdapter = ma;
    DbRef* db = multiTableAdapter->getDbRef();
    int nElens = multiTableAdapter->getNumberOfElenRanges();
    ensureGridSize(nElens);
    foreach(MTASingleTableAdapter* a , multiTableAdapter->getAdapters()) {
        SingleTablePackAlgorithmAdapter* sa = new SingleTablePackAlgorithmAdapter(db, a->singleTableAdapter->getReadsTableName());
        packAdapters << sa;
        if (packAdaptersGrid.size() <= a->rowPos) {
            packAdaptersGrid.resize(a->rowPos + 1);
        }
        if(packAdaptersGrid[a->rowPos].size() <= a->elenPos) {
            packAdaptersGrid[a->rowPos].resize(a->elenPos + 1);
        }
        packAdaptersGrid[a->rowPos][a->elenPos] = sa;
    }
}

U2DbiIterator<PackAlgorithmData>* MultiTablePackAlgorithmAdapter::selectAllReads(U2OpStatus& os) {
    QVector< U2DbiIterator<PackAlgorithmData>* > iterators;
    foreach(SingleTablePackAlgorithmAdapter* a, packAdapters) {
        iterators << a->selectAllReads(os);
    }
    return new MTAPackAlgorithmDataIterator(iterators, multiTableAdapter->getIdExtrasPerRange());
}

MultiTablePackAlgorithmAdapter::~MultiTablePackAlgorithmAdapter() {
    qDeleteAll(packAdapters);
}

void MultiTablePackAlgorithmAdapter::assignProw(const U2DataId& readId, qint64 prow, U2OpStatus& os) {
    int elenPos = multiTableAdapter->getElenRangePosById(readId);
    int oldRowPos = multiTableAdapter->getRowRangePosById(readId);
    int newRowPos = multiTableAdapter->getRowRangePosByRow(prow);

    SingleTablePackAlgorithmAdapter* sa = NULL;
    if (newRowPos == oldRowPos) {
        sa = packAdaptersGrid[oldRowPos][elenPos];
        sa->assignProw(readId, prow, os);
        return;
    }
    ensureGridSize(newRowPos + 1);

    sa = packAdaptersGrid[newRowPos][elenPos];
    MTASingleTableAdapter* oldA = multiTableAdapter->getAdapterByRowAndElenRange(oldRowPos, elenPos, false, os);
    MTASingleTableAdapter* newA = multiTableAdapter->getAdapterByRowAndElenRange(newRowPos, elenPos, true, os);

    SAFE_POINT(oldA!=NULL, QString("Can't find reads table adapter: row: %1, elen: %2").arg(oldRowPos).arg(elenPos) ,);
    SAFE_POINT(newA!=NULL, QString("Can't find reads table adapter: row: %1, elen: %2").arg(newRowPos).arg(elenPos) ,);
    SAFE_POINT_OP(os,);

    if (sa == NULL) {
        sa = new SingleTablePackAlgorithmAdapter(multiTableAdapter->getDbRef(), newA->singleTableAdapter->getReadsTableName());
        packAdapters << sa;
        packAdaptersGrid[newRowPos][elenPos] = sa;
    }

    QVector<SQLiteReadTableMigrationData>& newTableData = migrations[newA];
    newTableData.append(SQLiteReadTableMigrationData(U2DbiUtils::toDbiId(readId), oldA, prow));
    //TODO: add mem check here!
}

void MultiTablePackAlgorithmAdapter::releaseDbResources() {
    foreach(SingleTablePackAlgorithmAdapter* a, packAdapters) {
        a->releaseDbResources();
    }
}

void MultiTablePackAlgorithmAdapter::migrate(MTASingleTableAdapter* newA, const QVector<SQLiteReadTableMigrationData>& data, qint64 migratedBefore, qint64 totalMigrationCount, U2OpStatus& os) {
    SAFE_POINT_OP(os,);
    //delete reads from old tables, and insert into new one
    QHash<MTASingleTableAdapter*, QVector<SQLiteReadTableMigrationData> > readsByOldTable;
    foreach(const SQLiteReadTableMigrationData& d, data) {
        readsByOldTable[d.oldTable].append(d);
    }
    DbRef* db = multiTableAdapter->getDbRef();
    foreach(MTASingleTableAdapter* oldA, readsByOldTable.keys()) {

        const QVector<SQLiteReadTableMigrationData>& migData  = readsByOldTable[oldA];
        if (migData.isEmpty()) {
            continue;
        }
        QString oldTable = oldA->singleTableAdapter->getReadsTableName();
        QString newTable = newA->singleTableAdapter->getReadsTableName();
        QString idsTable = "tmp_mig_" + oldTable; //TODO

#ifdef _DEBUG
        qint64 nOldReads1 = SQLiteReadQuery("SELECT COUNT(*) FROM " + oldTable, db, os).selectInt64();
        qint64 nNewReads1 = SQLiteReadQuery("SELECT COUNT(*) FROM " + newTable, db, os).selectInt64();
        int readsMoved = migData.size();
        int rowsPerRange = multiTableAdapter->getRowsPerRange();
        U2Region newProwRegion(newA->rowPos * rowsPerRange, rowsPerRange);
#endif

        perfLog.trace(QString("Assembly: running reads migration from %1 to %2 number of reads: %3").arg(oldTable).arg(newTable).arg(migData.size()));
        quint64 t0 = GTimer::currentTimeMicros();

        { //nested block is needed to ensure all queries are finalized

            SQLiteWriteQuery(QString("CREATE TEMPORARY TABLE %1(id INTEGER PRIMARY KEY, prow INTEGER NOT NULL)").arg(idsTable), db, os).execute();
            SQLiteWriteQuery insertIds(QString("INSERT INTO %1(id, prow) VALUES(?1, ?2)").arg(idsTable), db, os);
            foreach(const SQLiteReadTableMigrationData& d, migData) {
                insertIds.reset(false);
                insertIds.bindInt64(1, d.readId);
                insertIds.bindInt32(2, d.newProw);
#ifdef _DEBUG
                assert(newProwRegion.contains(d.newProw));
#endif
                insertIds.execute();
                if (os.hasError()) {
                    break;
                }
            }

            SQLiteWriteQuery(QString("INSERT INTO %1(prow, name, gstart, elen, flags, mq, data) "
                "SELECT %3.prow, name, gstart, elen, flags, mq, data FROM %2, %3 WHERE %2.id = %3.id")
                .arg(newTable).arg(oldTable).arg(idsTable), db, os).execute();

            SQLiteWriteQuery(QString("DELETE FROM %1 WHERE id IN (SELECT id FROM %2)").arg(oldTable).arg(idsTable), db, os).execute();

        }
        U2OpStatusImpl osStub; // using stub here -> this operation must be performed even if any of internal queries failed
        SQLiteWriteQuery(QString("DROP TABLE IF EXISTS %1").arg(idsTable), db, osStub).execute();

        qint64 nMigrated = migratedBefore + migData.size();
        perfLog.trace(QString("Assembly: reads migration from %1 to %2 finished, time %3 seconds, progress: %4/%5 (%6%)")
            .arg(oldTable).arg(newTable).arg((GTimer::currentTimeMicros() - t0)/float(1000*1000))
            .arg(nMigrated).arg(totalMigrationCount).arg(100*nMigrated/totalMigrationCount));

#ifdef _DEBUG
        qint64 nOldReads2 = SQLiteReadQuery("SELECT COUNT(*) FROM " + oldTable, db, os).selectInt64();
        qint64 nNewReads2 = SQLiteReadQuery("SELECT COUNT(*) FROM " + newTable, db, os).selectInt64();
        assert(nOldReads1 + nNewReads1 == nOldReads2 + nNewReads2);
        assert(nNewReads1 + readsMoved == nNewReads2);
#endif

    }
}

void MultiTablePackAlgorithmAdapter::migrateAll(U2OpStatus& os) {
    SAFE_POINT_OP(os,);

    qint64 nReadsToMigrate = 0;
    foreach(MTASingleTableAdapter* newTable, migrations.keys()) {
        const QVector<SQLiteReadTableMigrationData>& data = migrations[newTable];
        nReadsToMigrate+=data.size();
    }
    if (nReadsToMigrate == 0) {
        return;
    }
    qint64 nReadsTotal = multiTableAdapter->countReads(U2_REGION_MAX, os);
    qint64 migrationPercent = nReadsToMigrate * 100 / nReadsTotal;

    perfLog.trace(QString("Assembly: starting reads migration process. Reads to migrate: %1, total: %2 (%3%)").arg(nReadsToMigrate).arg(nReadsTotal).arg(migrationPercent));

#define MAX_PERCENT_TO_REINDEX 20
    if (migrationPercent > MAX_PERCENT_TO_REINDEX) {
        perfLog.trace("Assembly: dropping old indexes first");
        foreach(MTASingleTableAdapter* adapter, multiTableAdapter->getAdapters()) {
            adapter->singleTableAdapter->dropReadsIndexes(os);
        }
        perfLog.trace("Assembly: indexes are dropped");
    }

    SAFE_POINT_OP(os, );
    int nMigrated = 0;
    foreach(MTASingleTableAdapter* newTable, migrations.keys()) {
        const QVector<SQLiteReadTableMigrationData>& data = migrations[newTable];
        migrate(newTable, data, nMigrated, nReadsToMigrate, os);
        nMigrated+=data.size();
    }
    migrations.clear();
}



void MultiTablePackAlgorithmAdapter::ensureGridSize(int nRows) {
    int oldNRows = packAdaptersGrid.size();
    if (oldNRows < nRows) {
        int nElens = multiTableAdapter->getNumberOfElenRanges();
        packAdaptersGrid.resize(nRows);
        for (int i = oldNRows; i < nRows; i++) {
            packAdaptersGrid[i].resize(nElens);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// MTAReadsIterator

MTAReadsIterator::MTAReadsIterator(QVector< U2DbiIterator<U2AssemblyRead>* >& i, const QVector<QByteArray>& ie, bool sorted)
: iterators (i), currentRange(0), idExtras(ie), sortedHint(sorted)
{
}

MTAReadsIterator::~MTAReadsIterator() {
    qDeleteAll(iterators);
}

// TODO: remove copy-paste from this code
bool MTAReadsIterator::hasNext() {
    if (sortedHint) {
        foreach (U2DbiIterator<U2AssemblyRead> *it, iterators) {
            if (it->hasNext()) {
                return true;
            }
        }
        return false;
    } else {
        bool res = currentRange < iterators.size();
        if (res) {
            do {
                U2DbiIterator<U2AssemblyRead>* it = iterators[currentRange];
                res = it->hasNext();
                if (res) {
                    break;
                }
                currentRange++;
            }  while (currentRange < iterators.size());
        }
        return res;
    }
}

U2AssemblyRead MTAReadsIterator::next() {
    U2AssemblyRead res;
    if (sortedHint) {
        qint64 minPos = LLONG_MAX;
        U2DbiIterator<U2AssemblyRead> *minIt = NULL;
        foreach (U2DbiIterator<U2AssemblyRead> *it, iterators) {
            if (it->hasNext()) {
                U2AssemblyRead candidate = it->peek();
                SAFE_POINT(NULL != candidate.data(), "NULL assembly read", candidate);
                if (candidate->leftmostPos < minPos) {
                    minIt = it;
                    minPos = candidate->leftmostPos;
                }
            }
        }
        if (NULL != minIt) {
            res = minIt->next();
            SAFE_POINT(NULL != res.data(), "NULL assembly read", res);
            int currentIt = iterators.indexOf(minIt);
            const QByteArray& idExtra = idExtras.at(currentIt);
            res->id = addTable2Id(res->id, idExtra);
        }
        return res;
    } else {
        if (currentRange < iterators.size()) {
            do {
                U2DbiIterator<U2AssemblyRead>* it = iterators[currentRange];
                if (it->hasNext()) {
                    res = it->next();
                    SAFE_POINT(NULL != res.data(), "NULL assembly read", res);
                    const QByteArray& idExtra = idExtras.at(currentRange);
                    res->id = addTable2Id(res->id, idExtra);
                    break;
                }
                currentRange++;
            } while (currentRange < iterators.size());
        }
        return res;
    }
}

U2AssemblyRead MTAReadsIterator::peek() {
    U2AssemblyRead res;
    if (sortedHint) {
        qint64 minPos = LLONG_MAX;
        U2DbiIterator<U2AssemblyRead> *minIt = NULL;
        foreach (U2DbiIterator<U2AssemblyRead> *it, iterators) {
            if (it->hasNext()) {
                U2AssemblyRead candidate = it->peek();
                SAFE_POINT(NULL != candidate.data(), "NULL assembly read", candidate);
                if (candidate->leftmostPos < minPos) {
                    minIt = it;
                    minPos = candidate->leftmostPos;
                }
            }
        }
        if (NULL != minIt) {
            res = minIt->next();
            SAFE_POINT(NULL != res.data(), "NULL assembly read", res);
            int currentIt = iterators.indexOf(minIt);
            const QByteArray& idExtra = idExtras.at(currentIt);
            res->id = addTable2Id(res->id, idExtra);
        }
        return res;
    } else {
        if (currentRange < iterators.size()) {
            do {
                U2DbiIterator<U2AssemblyRead>* it = iterators[currentRange];
                if (it->hasNext()) {
                    res = it->peek();
                    SAFE_POINT(NULL != res.data(), "NULL assembly read", res);
                    const QByteArray& idExtra = idExtras.at(currentRange);
                    res->id = addTable2Id(res->id, idExtra);
                    break;
                }
                currentRange++;
            } while (currentRange < iterators.size());
        }
        return res;
    }
}

//////////////////////////////////////////////////////////////////////////
// MTAPackAlgorithmDataIterator

MTAPackAlgorithmDataIterator::MTAPackAlgorithmDataIterator(QVector< U2DbiIterator<PackAlgorithmData>* >& i, const QVector<QByteArray>& ie)
:  iterators (i), idExtras(ie)
{
    fetchNextData();
}

MTAPackAlgorithmDataIterator::~MTAPackAlgorithmDataIterator() {
    qDeleteAll(iterators);
}

bool MTAPackAlgorithmDataIterator::hasNext() {
    return !nextData.readId.isEmpty();
}

PackAlgorithmData MTAPackAlgorithmDataIterator::next() {
    PackAlgorithmData res = nextData;
    fetchNextData();
    return res;
}

PackAlgorithmData MTAPackAlgorithmDataIterator::peek() {
    return nextData;
}

void MTAPackAlgorithmDataIterator::fetchNextData() {
    PackAlgorithmData bestCandidate;
    int bestRange = 0;
    for (int i = 0; i < iterators.size(); i++) {
        U2DbiIterator<PackAlgorithmData>* it = iterators[i];
        if (!it->hasNext()) {
            continue;
        }
        PackAlgorithmData d = it->peek();
        if (bestCandidate.readId.isEmpty() || bestCandidate.leftmostPos > d.leftmostPos) {
            bestCandidate = d;
            bestRange = i;
        }
    }
    nextData = bestCandidate;
    if (!nextData.readId.isEmpty()) {
        iterators[bestRange]->next();
        const QByteArray& idExtra = idExtras.at(bestRange);
        nextData.readId = addTable2Id(nextData.readId, idExtra);
    }
}


} //namespace
