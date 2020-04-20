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

#include <U2Core/Timer.h>
#include <U2Core/U2AssemblyUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
//#include <U2Core/U2SqlHelpers.h>

#include "MysqlMultiTableAssemblyAdapter.h"
#include "MysqlSingleTableAssemblyAdapter.h"
#include "mysql_dbi/MysqlDbi.h"
#include "mysql_dbi/MysqlAssemblyDbi.h"
#include "mysql_dbi/MysqlObjectDbi.h"
#include "util/AssemblyPackAlgorithm.h"

namespace U2 {

/****************************************************************/
/* Support functions */
/****************************************************************/

namespace {

QVector<U2Region> toRange(const QVector<int>& startPos) {
    QVector<U2Region> res;

    int prev = 0;
    foreach (int pos, startPos){
        res << U2Region(prev, pos - prev);
        prev = pos;
    }

    return res;
}

U2DataId addTable2Id(const U2DataId& id, const QByteArray& idExtra) {
    SAFE_POINT(U2DbiUtils::toDbExtra(id).isEmpty(), "Extra field of the input U2DataId is not empty", U2DataId());
    const quint64 dbiId = U2DbiUtils::toDbiId(id);
    const U2DataId res = U2DbiUtils::toU2DataId(dbiId, U2Type::AssemblyRead, idExtra);
    return res;
}

void ensureGridSize(QVector <QVector < QList<U2AssemblyRead> > >& grid, int rowPos, int nElens) {
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

}   // unnamed namespace


/****************************************************************/
/* MysqlMtaSingleTableAdapter */
/****************************************************************/

MysqlMtaSingleTableAdapter::MysqlMtaSingleTableAdapter(MysqlSingleTableAssemblyAdapter* adapter,
                                                       int rowPos,
                                                       int elenPos,
                                                       const QByteArray &extra) :
    singleTableAdapter(adapter),
    rowPos(rowPos),
    elenPos(elenPos),
    idExtra(extra)
{
}

/****************************************************************/
/* MysqlMultiTableAssemblyAdapter */
/****************************************************************/

MysqlMultiTableAssemblyAdapter::MysqlMultiTableAssemblyAdapter(MysqlDbi* dbi,
                                                               const U2DataId& assemblyId,
                                                               const AssemblyCompressor* compressor,
                                                               MysqlDbRef *db,
                                                               U2OpStatus& os) :
    MysqlAssemblyAdapter(assemblyId, compressor, db),
    dbi(dbi),
    version(-1),
    rowsPerRange(DEFAULT_ROWS_PER_TABLE)
{
    syncTables(os);
}

MysqlMultiTableAssemblyAdapter::~MysqlMultiTableAssemblyAdapter() {
    clearTableAdaptersInfo();
}

qint64 MysqlMultiTableAssemblyAdapter::countReads(const U2Region& r, U2OpStatus& os) {
    bool all = (r == U2_REGION_MAX);
    qint64 sum = 0;

    // use more sensitive algorithm for smaller regions with low amount of reads
    // and not-very sensitive for huge regions with a lot of reads
    int nReadsToUseNotPreciseAlgorithms = 1000 / (r.length + 1);
    foreach(MysqlMtaSingleTableAdapter* a, adapters) {
        int n = a->singleTableAdapter->countReads(r, os);
        if (n != 0 && !all && n < nReadsToUseNotPreciseAlgorithms) {
            n = a->singleTableAdapter->countReadsPrecise(r, os);
        }
        CHECK_OP(os, sum);
        sum += n;
    }

    return sum;
}

qint64 MysqlMultiTableAssemblyAdapter::getMaxPackedRow(const U2Region& r, U2OpStatus& os) {
    qint64 max = 0;

    // process only hi row adapters
    int nRows = adaptersGrid.size();
    for (int rowPos = nRows; --rowPos >= 0 && max == 0;) {
        QVector<MysqlMtaSingleTableAdapter*> elenAdapters = adaptersGrid.at(rowPos);
        for (int elenPos = 0, nElens = elenAdapters.size(); elenPos < nElens; elenPos++) {
            MysqlMtaSingleTableAdapter* a = elenAdapters.at(elenPos);
            if (a == NULL) {
                continue;
            }

            SAFE_POINT(a->rowPos == rowPos, "Incorrect row position", max);
            qint64 n = a->singleTableAdapter->getMaxPackedRow(r, os);
            SAFE_POINT(U2Region(rowsPerRange * rowPos, rowsPerRange).contains(n), "Invalid region", max);
            max = qMax(max, n);
        }
    }

    return max;
}

qint64 MysqlMultiTableAssemblyAdapter::getMaxEndPos(U2OpStatus& os) {
    //TODO: optimize by using gstart + maxReadLen for first n-1 tables
    qint64 max = 0;

    foreach(MysqlMtaSingleTableAdapter* a, adapters) {
        qint64 n = a->singleTableAdapter->getMaxEndPos(os);
        CHECK_OP(os, max);
        max = qMax(max, n);
    }

    return max;
}

U2DbiIterator<U2AssemblyRead>* MysqlMultiTableAssemblyAdapter::getReads(const U2Region& r, U2OpStatus& os, bool sortedHint) {
    QVector< U2DbiIterator<U2AssemblyRead>* > iterators;

    foreach (MysqlMtaSingleTableAdapter* a, adapters) {
        iterators << a->singleTableAdapter->getReads(r, os, sortedHint);
        CHECK_OP_EXT(os, qDeleteAll(iterators), NULL);
    }

    return new MysqlMtaReadsIterator(iterators, idExtras, sortedHint);
}

U2DbiIterator<U2AssemblyRead>* MysqlMultiTableAssemblyAdapter::getReadsByRow(const U2Region& r, qint64 minRow, qint64 maxRow, U2OpStatus& os) {
    QVector< U2DbiIterator<U2AssemblyRead>* > iterators;
    QVector<QByteArray> selectedIdExtras;
    U2Region targetRowRange(minRow, maxRow - minRow);

    foreach (MysqlMtaSingleTableAdapter* a, adapters) {
        const U2Region rowRegion(a->rowPos * rowsPerRange, rowsPerRange);
        if (!rowRegion.intersects(targetRowRange)) {
            continue;
        }

        iterators << a->singleTableAdapter->getReadsByRow(r, minRow, maxRow, os);
        CHECK_OP_EXT(os, qDeleteAll(iterators), NULL);
        selectedIdExtras << a->idExtra;
    }

    return new MysqlMtaReadsIterator(iterators, selectedIdExtras, false);
}

U2DbiIterator<U2AssemblyRead>* MysqlMultiTableAssemblyAdapter::getReadsByName(const QByteArray& name, U2OpStatus& os) {
    QVector< U2DbiIterator<U2AssemblyRead>* > iterators;

    foreach(MysqlMtaSingleTableAdapter* a, adapters) {
        iterators << a->singleTableAdapter->getReadsByName(name, os);
        CHECK_OP_EXT(os, qDeleteAll(iterators), NULL);
    }

    return new MysqlMtaReadsIterator(iterators, idExtras, false);
}

void MysqlMultiTableAssemblyAdapter::addReads(U2DbiIterator<U2AssemblyRead>* it, U2AssemblyReadsImportInfo& ii, U2OpStatus& os) {
    bool empty = adaptersGrid.isEmpty();
    if (empty) {
        // TODO: fetch some reads for analysis. By now not needed, since regions are hard-coded anyway
        initTables(QList<U2AssemblyRead>(), os);
        CHECK_OP(os, );
    }

    bool packIsOn = empty;
    qint64 prevLeftmostPos = -1;
    PackAlgorithmContext packContext;

    QVector <QVector < QList<U2AssemblyRead> > > readsGrid; //reads sorted by range
    bool lastIteration = false;
    qint64 readsInGrid = 0;

    while (!os.isCoR()) {
        int nElens = elenRanges.size();
        if (it->hasNext()) {
            U2AssemblyRead read = it->next();
            CHECK_OP_BREAK(os);
            int readLen = read->readSequence.length();
            read->effectiveLen = readLen + U2AssemblyUtils::getCigarExtraLength(read->cigar);
            int elenPos = getElenRangePosByLength(read->effectiveLen);

            packIsOn = packIsOn && (read->leftmostPos >= prevLeftmostPos);
            read->packedViewRow = packIsOn ? AssemblyPackAlgorithm::packRead(U2Region(read->leftmostPos, read->effectiveLen), packContext, os) : 0;
            int rowPos = getRowRangePosByRow(read->packedViewRow);
            ensureGridSize(readsGrid, rowPos, nElens);
            readsGrid[rowPos][elenPos] << read;
            ++ii.nReads;
            ++readsInGrid;
        } else {
            lastIteration = true;
        }

        int nRows = readsGrid.size();
        if (lastIteration || readsInGrid > N_READS_TO_FLUSH_TOTAL) {
            for (int rowPos = 0; rowPos < nRows && !os.isCoR(); rowPos++) {
                for (int elenPos = 0; elenPos < nElens && !os.isCoR(); elenPos++) {
                    QList<U2AssemblyRead>& rangeReads = readsGrid[rowPos][elenPos];
                    int nRangeReads = rangeReads.size();
                    if (nRangeReads == 0 || (!lastIteration && nRangeReads < N_READS_TO_FLUSH_PER_RANGE)) {
                        continue;
                    }

                    MysqlMtaSingleTableAdapter* adapter = getAdapterByRowAndElenRange(rowPos, elenPos, true, os);
                    U2AssemblyReadsImportInfo rangeReadsImportInfo(&ii);
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
    createReadsIndexes(os);

    if (packIsOn && !os.hasError()) {
        ii.packStat.readsCount = ii.nReads;
        ii.packStat.maxProw = packContext.maxProw;
        ii.packed = true;
        flushTables(os);
    }
}

void MysqlMultiTableAssemblyAdapter::removeReads(const QList<U2DataId>& readIds, U2OpStatus& os) {
    int nReads = readIds.size();

    QHash<MysqlMtaSingleTableAdapter*, QList<U2DataId> > readsByAdapter;
    for (int i = 0; i < nReads; i++) {
        const U2DataId& readId = readIds[i];
        int rowPos = getRowRangePosById(readId);
        int elenPos = getElenRangePosById(readId);
        MysqlMtaSingleTableAdapter* a = getAdapterByRowAndElenRange(rowPos, elenPos, false, os);

        SAFE_POINT(a != NULL, QString("No table adapter was found. row: %1, elen: %2").arg(rowPos).arg(elenPos),);

        if (!readsByAdapter.contains(a)) {
            readsByAdapter[a] = QList<U2DataId>();
        }
        readsByAdapter[a].append(readId);
    }

    foreach(MysqlMtaSingleTableAdapter* a, readsByAdapter.keys()) {
        QList<U2DataId>& rangeReadIds = readsByAdapter[a];
        a->singleTableAdapter->removeReads(rangeReadIds, os);
        //TODO: remove adapters for empty tables. And tables as well
    }
}

void MysqlMultiTableAssemblyAdapter::dropReadsTables(U2OpStatus &os) {
    foreach (QVector<MysqlMtaSingleTableAdapter*> adaptersVector, adaptersGrid) {
        foreach (MysqlMtaSingleTableAdapter* adapter, adaptersVector) {
            if (NULL != adapter) {
                adapter->singleTableAdapter->dropReadsTables(os);
            }
        }
    }
}

void MysqlMultiTableAssemblyAdapter::pack(U2AssemblyPackStat& stat, U2OpStatus& os) {
    MysqlMultiTablePackAlgorithmAdapter packAdapter(this);

    AssemblyPackAlgorithm::pack(packAdapter, stat, os);
    CHECK_OP(os, );
    packAdapter.releaseDbResources();

    quint64 t0 = GTimer::currentTimeMicros();
    packAdapter.migrateAll(os);
    CHECK_OP(os, );
    perfLog.trace(QString("Assembly: table migration pack time: %1 seconds").arg((GTimer::currentTimeMicros() - t0) / float(1000*1000)));

    t0 = GTimer::currentTimeMicros();
    // if new tables created during the pack algorithm -> create indexes
    createReadsIndexes(os);
    CHECK_OP(os, );
    perfLog.trace(QString("Assembly: re-indexing pack time: %1 seconds").arg((GTimer::currentTimeMicros() - t0) / float(1000*1000)));

    flushTables(os);
}

void MysqlMultiTableAssemblyAdapter::calculateCoverage(const U2Region& region, U2AssemblyCoverageStat& coverage, U2OpStatus& os) {
    for (int i = 0; i < adapters.size(); ++i) {
        MysqlMtaSingleTableAdapter * a = adapters.at(i);
        a->singleTableAdapter->calculateCoverage(region, coverage, os);
        CHECK_OP(os, );

        os.setProgress((double(i + 1) / adapters.size()) * 100);
    }
}

void MysqlMultiTableAssemblyAdapter::createReadsIndexes(U2OpStatus& os) {
    foreach(MysqlMtaSingleTableAdapter* a, adapters) {
        a->singleTableAdapter->createReadsIndexes(os);
        CHECK_OP(os, );
    }
}

int MysqlMultiTableAssemblyAdapter::getElenRangePosByLength(qint64 readLength) const {
    int nElenRanges = elenRanges.size();
    for (int i = 0; i < nElenRanges; i++) {
        const U2Region& r = elenRanges[i];
        if (r.contains(readLength)) {
            return i;
        }
    }

    FAIL(QString("Read length does not fit any range: %1, number of ranges: %2").arg(readLength).arg(nElenRanges), nElenRanges - 1);
}

int MysqlMultiTableAssemblyAdapter::getElenRangePosById(const U2DataId& id) const {
    QByteArray extra = U2DbiUtils::toDbExtra(id);

    SAFE_POINT(extra.size() == 4, QString("Illegal assembly read ID extra part. HEX: %1").arg(extra.toHex().constData()), -1);

    const qint16* data = (const qint16*)extra.constData();
    return int(data[1]);
}

int MysqlMultiTableAssemblyAdapter::getNumberOfElenRanges() const {
    return elenRanges.size();
}

int MysqlMultiTableAssemblyAdapter::getRowRangePosByRow(quint64 row) const {
    return row / rowsPerRange;
}

int MysqlMultiTableAssemblyAdapter::getRowRangePosById(const U2DataId& id) const {
    QByteArray extra = U2DbiUtils::toDbExtra(id);

    SAFE_POINT(extra.size() == 4, QString("Extra part size of assembly read ID is not correct. HEX(Extra): %1").arg(extra.toHex().constData()), -1);

    const qint16* data = (const qint16*)extra.constData();
    return int(data[0]);
}

int MysqlMultiTableAssemblyAdapter::getRowsPerRange() const {
    return rowsPerRange;
}

const QVector<MysqlMtaSingleTableAdapter*>& MysqlMultiTableAssemblyAdapter::getAdapters() const {
    return adapters;
}
const QVector<QByteArray>& MysqlMultiTableAssemblyAdapter::getIdExtrasPerRange() const {
    return idExtras;
}

MysqlDbRef* MysqlMultiTableAssemblyAdapter::getDbRef() const {
    return dbi->getDbRef();
}

MysqlMtaSingleTableAdapter* MysqlMultiTableAssemblyAdapter::getAdapterByRowAndElenRange(int rowPos, int elenPos, bool createIfNotExits, U2OpStatus& os) {
    int nElens = elenRanges.size();
    SAFE_POINT(elenPos < nElens, "Out of range", NULL);
    if (rowPos >= adaptersGrid.size()) {
        SAFE_POINT(createIfNotExits, "Adapter is not exists", NULL);
        int oldRowSize = adaptersGrid.size();
        int newRowSize = rowPos + 1;
        adaptersGrid.resize(newRowSize);
        for (int i = oldRowSize; i < newRowSize; i++) {
            adaptersGrid[i].resize(nElens);
        }
    }

    QVector<MysqlMtaSingleTableAdapter*> elenAdapters = adaptersGrid.at(rowPos);
    SAFE_POINT(elenAdapters.size() == nElens, "Invalid adapters array", NULL);
    MysqlMtaSingleTableAdapter* adapter = elenAdapters.at(elenPos);
    if (adapter == NULL && createIfNotExits) {
        adapter = createAdapter(rowPos, elenPos, os);
    }

    return adapter;
}

QString MysqlMultiTableAssemblyAdapter::getTableSuffix(int rowPos, int elenPos) {
    SAFE_POINT(0 <= elenPos && elenPos < elenRanges.size(), "Out of range", "");

    const U2Region eRegion = elenRanges[elenPos];
    bool last = (elenPos + 1 == elenRanges.size());
    return QString("%1_%2_%3").arg(eRegion.startPos).arg(last ? QString("U") : QString::number(eRegion.endPos())).arg(rowPos);
}

QByteArray MysqlMultiTableAssemblyAdapter::getIdExtra(int rowPos, int elenPos) {
    QByteArray res(4, 0);
    qint16* data = (qint16*)res.data();
    data[0] = (qint16)rowPos;
    data[1] = (qint16)elenPos;
    return res;
}

void MysqlMultiTableAssemblyAdapter::syncTables(U2OpStatus& os) {
    qint64 versionInDb = dbi->getObjectDbi()->getObjectVersion(assemblyId, os);
    CHECK_OP(os, );

    if (versionInDb <= version) {
        return;
    }

    static const QString queryString = "SELECT idata FROM Assembly WHERE object = :object";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":object", assemblyId);
    if (q.step()) {
        const QByteArray data = q.getBlob(0);
        rereadTables(data, os);
        CHECK_OP_EXT(os, version = versionInDb, );
    }
}

void MysqlMultiTableAssemblyAdapter::initTables(const QList<U2AssemblyRead>& reads, U2OpStatus& os) {
    SAFE_POINT(elenRanges.isEmpty(), "Effective ranges are already initialized", );

    const int nReads = reads.size();
    // TODO
    if (false && nReads > 1000) {
//        // get reads distribution first
//        QVector<int> distribution(nReads / 10, 0);
//        int* data = distribution.data();
//        foreach(const U2AssemblyRead& read, reads) {
//            int elen = read->readSequence.size() + U2AssemblyUtils::getCigarExtraLength(read->cigar);
//            int idx = elen / 10;
//            data[idx]++;
//        }
//        // derive regions
//        // TODO:
    } else {
        QVector<int> starts;
        starts << 50 << 100 << 200 << 400 << 800 << 4 * 1000 << 25 * 1000 << 100 * 1000 << 500 * 1000 << 2 * 1000 * 1000;
        elenRanges << toRange(starts);
    }

    initAdaptersGrid(1, elenRanges.size());
    flushTables(os);
}

void MysqlMultiTableAssemblyAdapter::rereadTables(const QByteArray& idata, U2OpStatus& os) {
    QWriteLocker wl(&tablesSyncLock);

    clearTableAdaptersInfo();

    // format: N, N, N, N | N, N |.....
    // elements are separated by | sign. First element encodes ranges, second prow step and max prow, others are for future extension
    if (idata.isEmpty()) {
        //assembly is empty - no index data was created
        return;
    }

    const QList<QByteArray> elements = idata.split('|');
    if (elements.size() < 2) {
        os.setError(U2DbiL10n::tr("Failed to detect assembly storage format: %1").arg(idata.constData()));
        return;
    }
    QByteArray elenData = elements.at(0);
    QByteArray prowData = elements.at(1);

    const QList<QByteArray> elenTokens = elenData.split(',');
    U2Region prev(-1, 1);
    bool parseOk = true;
    foreach(const QByteArray& elenTok, elenTokens) {
        int start = elenTok.toInt(&parseOk);
        if (!parseOk || start < prev.endPos()) {
            os.setError(U2DbiL10n::tr("Failed to parse range: %1, full: %2").arg(elenTok.constData()).arg(elenData.constData()));
            return;
        }

        U2Region region(prev.endPos(), start - prev.endPos());
        elenRanges << region;
        prev = region;
    }
    elenRanges << U2Region(prev.endPos(), INT_MAX);

    const QList<QByteArray> prowTokens = prowData.split(',');
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
            const QString suffix = getTableSuffix(rowPos, elenPos);
            const QString tableName = MysqlSingleTableAssemblyAdapter::getReadsTableName(assemblyId, 'M', suffix);
            if (MysqlUtils::isTableExists(tableName, db, os)) {
                createAdapter(rowPos, elenPos, os);
            }
        }
    }
}

void MysqlMultiTableAssemblyAdapter::flushTables(U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    if (adaptersGrid.isEmpty()) {
        // TODO: fetch some reads for analysis. By now not needed, since regions are hard-coded anyway
        initTables(QList<U2AssemblyRead>(), os);
        CHECK_OP(os, );
    }

    QByteArray idata;
    for (int i = 0; i < elenRanges.size(); i++) {
        int rangeStart = elenRanges[i].startPos;
        if (!idata.isEmpty()) {
            idata += ',';
        }
        idata += QByteArray::number(rangeStart);
    }
    idata += '|' + QByteArray::number(rowsPerRange) + ',' + QByteArray::number(adaptersGrid.size());

    static const QString queryString = "UPDATE Assembly SET idata = :idata WHERE object = :object";
    U2SqlQuery q(queryString, db, os);
    q.bindBlob(":idata", idata);
    q.bindDataId(":object", assemblyId);
    q.execute();
}

void MysqlMultiTableAssemblyAdapter::clearTableAdaptersInfo() {
    qDeleteAll(adapters);
    adaptersGrid.clear();
    idExtras.clear();
    elenRanges.clear();
}

MysqlMtaSingleTableAdapter* MysqlMultiTableAssemblyAdapter::createAdapter(int rowPos, int elenPos, U2OpStatus& os) {
    SAFE_POINT(0 <= rowPos && rowPos < adaptersGrid.size(), "Out of range", NULL);
    SAFE_POINT(0 <= elenPos && elenPos < adaptersGrid.at(rowPos).size(), "Out of range", NULL);
    SAFE_POINT(NULL == adaptersGrid.at(rowPos).at(elenPos), "Adapter is already created", NULL);

    const QString suffix = getTableSuffix(rowPos, elenPos);
    const U2Region& elenRange = elenRanges.at(elenPos);
    const QByteArray idExtra = getIdExtra(rowPos, elenPos);

    MysqlSingleTableAssemblyAdapter* sa = new MysqlSingleTableAssemblyAdapter(dbi, assemblyId, 'M', suffix, compressor, db, os);
    sa->enableRangeTableMode(elenRange.startPos, elenRange.endPos());

    MysqlMtaSingleTableAdapter* ma = new MysqlMtaSingleTableAdapter(sa, rowPos, elenPos, idExtra);

    adapters << ma;
    idExtras << idExtra;
    adaptersGrid[rowPos][elenPos] =  ma;

    return ma;
}

void MysqlMultiTableAssemblyAdapter::initAdaptersGrid(int nRows, int nElens) {
    SAFE_POINT(adaptersGrid.isEmpty(), "Adapters are already initialized", );

    adaptersGrid.resize(nRows);
    for (int i = 0; i < nRows; i++) {
        adaptersGrid[i] = QVector<MysqlMtaSingleTableAdapter*>(nElens, NULL);
    }
}


//////////////////////////////////////////////////////////////////////////
// MysqlReadTableMigrationData

MysqlReadTableMigrationData::MysqlReadTableMigrationData() :
    readId(-1),
    oldTable(NULL),
    newProw(-1)
{
}

MysqlReadTableMigrationData::MysqlReadTableMigrationData(qint64 oldId, MysqlMtaSingleTableAdapter* oldT, int newP) :
    readId(oldId),
    oldTable(oldT),
    newProw(newP)
{
}


//////////////////////////////////////////////////////////////////////////
// pack adapter

MysqlMultiTablePackAlgorithmAdapter::MysqlMultiTablePackAlgorithmAdapter(MysqlMultiTableAssemblyAdapter* ma) :
    multiTableAdapter(ma)
{
    MysqlDbRef* db = multiTableAdapter->getDbRef();
    int nElens = multiTableAdapter->getNumberOfElenRanges();
    ensureGridSize(nElens);

    foreach (MysqlMtaSingleTableAdapter* a, multiTableAdapter->getAdapters()) {
        MysqlSingleTablePackAlgorithmAdapter* sa = new MysqlSingleTablePackAlgorithmAdapter(db, a->singleTableAdapter->getReadsTableName());
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

MysqlMultiTablePackAlgorithmAdapter::~MysqlMultiTablePackAlgorithmAdapter() {
    qDeleteAll(packAdapters);
}

U2DbiIterator<PackAlgorithmData>* MysqlMultiTablePackAlgorithmAdapter::selectAllReads(U2OpStatus& os) {
    QVector< U2DbiIterator<PackAlgorithmData>* > iterators;
    foreach (MysqlSingleTablePackAlgorithmAdapter* a, packAdapters) {
        iterators << a->selectAllReads(os);
    }

    return new MysqlMTAPackAlgorithmDataIterator(iterators, multiTableAdapter->getIdExtrasPerRange());
}

void MysqlMultiTablePackAlgorithmAdapter::assignProw(const U2DataId& readId, qint64 prow, U2OpStatus& os) {
    int elenPos = multiTableAdapter->getElenRangePosById(readId);
    int oldRowPos = multiTableAdapter->getRowRangePosById(readId);
    int newRowPos = multiTableAdapter->getRowRangePosByRow(prow);

    MysqlSingleTablePackAlgorithmAdapter* sa = NULL;
    if (newRowPos == oldRowPos) {
        sa = packAdaptersGrid[oldRowPos][elenPos];
        sa->assignProw(readId, prow, os);
        return;
    }
    ensureGridSize(newRowPos + 1);

    sa = packAdaptersGrid[newRowPos][elenPos];
    MysqlMtaSingleTableAdapter* oldA = multiTableAdapter->getAdapterByRowAndElenRange(oldRowPos, elenPos, false, os);
    MysqlMtaSingleTableAdapter* newA = multiTableAdapter->getAdapterByRowAndElenRange(newRowPos, elenPos, true, os);

    SAFE_POINT(oldA != NULL, QString("Can't find reads table adapter: row: %1, elen: %2").arg(oldRowPos).arg(elenPos) ,);
    SAFE_POINT(newA != NULL, QString("Can't find reads table adapter: row: %1, elen: %2").arg(newRowPos).arg(elenPos) ,);
    SAFE_POINT_OP(os, );

    if (sa == NULL) {
        sa = new MysqlSingleTablePackAlgorithmAdapter(multiTableAdapter->getDbRef(), newA->singleTableAdapter->getReadsTableName());
        packAdapters << sa;
        packAdaptersGrid[newRowPos][elenPos] = sa;
    }

    QVector<MysqlReadTableMigrationData>& newTableData = migrations[newA];
    newTableData.append(MysqlReadTableMigrationData(U2DbiUtils::toDbiId(readId), oldA, prow));
    //TODO: add mem check here!
}

void MysqlMultiTablePackAlgorithmAdapter::releaseDbResources() {
    foreach (MysqlSingleTablePackAlgorithmAdapter* a, packAdapters) {
        a->releaseDbResources();
    }
}

void MysqlMultiTablePackAlgorithmAdapter::migrateAll(U2OpStatus& os) {
    qint64 nReadsToMigrate = 0;
    foreach (MysqlMtaSingleTableAdapter* newTable, migrations.keys()) {
        const QVector<MysqlReadTableMigrationData>& data = migrations[newTable];
        nReadsToMigrate+=data.size();
    }

    if (nReadsToMigrate == 0) {
        return;
    }

    qint64 nReadsTotal = multiTableAdapter->countReads(U2_REGION_MAX, os);
    qint64 migrationPercent = nReadsToMigrate * 100 / nReadsTotal;

    perfLog.trace(QString("Assembly: starting reads migration process. Reads to migrate: %1, total: %2 (%3%)").arg(nReadsToMigrate).arg(nReadsTotal).arg(migrationPercent));

    if (migrationPercent > MAX_PERCENT_TO_REINDEX) {
        perfLog.trace("Assembly: dropping old indexes first");
        foreach (MysqlMtaSingleTableAdapter* adapter, multiTableAdapter->getAdapters()) {
            adapter->singleTableAdapter->dropReadsIndexes(os);
        }
        perfLog.trace("Assembly: indexes are dropped");
    }

    SAFE_POINT_OP(os, );
    int nMigrated = 0;
    foreach (MysqlMtaSingleTableAdapter* newTable, migrations.keys()) {
        const QVector<MysqlReadTableMigrationData>& data = migrations[newTable];
        migrate(newTable, data, nMigrated, nReadsToMigrate, os);
        nMigrated += data.size();
    }
    migrations.clear();
}

void MysqlMultiTablePackAlgorithmAdapter::ensureGridSize(int nRows) {
    int oldNRows = packAdaptersGrid.size();
    if (oldNRows < nRows) {
        int nElens = multiTableAdapter->getNumberOfElenRanges();
        packAdaptersGrid.resize(nRows);
        for (int i = oldNRows; i < nRows; i++) {
            packAdaptersGrid[i].resize(nElens);
        }
    }
}

void MysqlMultiTablePackAlgorithmAdapter::migrate(MysqlMtaSingleTableAdapter* newA, const QVector<MysqlReadTableMigrationData>& data, qint64 migratedBefore, qint64 totalMigrationCount, U2OpStatus& os) {
    //delete reads from old tables, and insert into new one
    QHash<MysqlMtaSingleTableAdapter*, QVector<MysqlReadTableMigrationData> > readsByOldTable;
    foreach (const MysqlReadTableMigrationData& d, data) {
        readsByOldTable[d.oldTable].append(d);
    }

    MysqlDbRef* db = multiTableAdapter->getDbRef();
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    foreach (MysqlMtaSingleTableAdapter* oldA, readsByOldTable.keys()) {

        const QVector<MysqlReadTableMigrationData>& migData = readsByOldTable[oldA];
        if (migData.isEmpty()) {
            continue;
        }

        QString oldTable = oldA->singleTableAdapter->getReadsTableName();
        QString newTable = newA->singleTableAdapter->getReadsTableName();
        QString idsTable = "tmp_mig_" + oldTable; //TODO

#ifdef _DEBUG
        qint64 nOldReads1 = U2SqlQuery("SELECT COUNT(*) FROM " + oldTable, db, os).selectInt64();
        qint64 nNewReads1 = U2SqlQuery("SELECT COUNT(*) FROM " + newTable, db, os).selectInt64();
        int readsMoved = migData.size();
        int rowsPerRange = multiTableAdapter->getRowsPerRange();
        U2Region newProwRegion(newA->rowPos * rowsPerRange, rowsPerRange);
#endif

        perfLog.trace(QString("Assembly: running reads migration from %1 to %2 number of reads: %3").arg(oldTable).arg(newTable).arg(migData.size()));
        quint64 t0 = GTimer::currentTimeMicros();

        { //nested block is needed to ensure all queries are finalized

            static const QString tempTableQuery = "CREATE TEMPORARY TABLE %1(id INTEGER PRIMARY KEY, prow INTEGER NOT NULL)";
            U2SqlQuery(tempTableQuery.arg(idsTable), db, os).execute();
            CHECK_OP(os, );

            static const QString insertQuery = "INSERT INTO %1(id, prow) VALUES(:id, :prow)";
            foreach(const MysqlReadTableMigrationData& d, migData) {
                U2SqlQuery insertIds(insertQuery.arg(idsTable), db, os);
                insertIds.bindInt64(":id", d.readId);
                insertIds.bindInt32(":prow", d.newProw);
#ifdef _DEBUG
                SAFE_POINT(newProwRegion.contains(d.newProw), "Invalid region", );
#endif
                insertIds.execute();
                CHECK_OP_BREAK(os);
            }

            if (!os.isCoR()) {
                static const QString insertString = "INSERT INTO %1(prow, name, gstart, elen, flags, mq, data) "
                        "SELECT %3.prow, name, gstart, elen, flags, mq, data FROM %2, %3 WHERE %2.id = %3.id";
                U2SqlQuery(insertString.arg(newTable).arg(oldTable).arg(idsTable), db, os).execute();

                static const QString deleteString ="DELETE A.* FROM %1 AS A INNER JOIN %2 AS B ON A.id = B.id";
                U2SqlQuery(deleteString.arg(oldTable).arg(idsTable), db, os).execute();
            }

        }
        U2OpStatusImpl osStub; // using stub here -> this operation must be performed even if any of internal queries failed
        static const QString dropTableString = "DROP TABLE IF EXISTS %1";
        U2SqlQuery(dropTableString.arg(idsTable), db, osStub).execute();

        qint64 nMigrated = migratedBefore + migData.size();
        perfLog.trace(QString("Assembly: reads migration from %1 to %2 finished, time %3 seconds, progress: %4/%5 (%6%)")
            .arg(oldTable).arg(newTable).arg((GTimer::currentTimeMicros() - t0)/float(1000*1000))
            .arg(nMigrated).arg(totalMigrationCount).arg(100*nMigrated/totalMigrationCount));

#ifdef _DEBUG
        qint64 nOldReads2 = U2SqlQuery("SELECT COUNT(*) FROM " + oldTable, db, os).selectInt64();
        qint64 nNewReads2 = U2SqlQuery("SELECT COUNT(*) FROM " + newTable, db, os).selectInt64();
        SAFE_POINT(nOldReads1 + nNewReads1 == nOldReads2 + nNewReads2, "Invalid reads count", );
        SAFE_POINT(nNewReads1 + readsMoved == nNewReads2, "Invalid reads count", );
#endif
    }
}


//////////////////////////////////////////////////////////////////////////
// MTAReadsIterator

MysqlMtaReadsIterator::MysqlMtaReadsIterator(QVector< U2DbiIterator<U2AssemblyRead>* >& i, const QVector<QByteArray>& ie, bool sorted)
: iterators (i), currentRange(0), idExtras(ie), sortedHint(sorted)
{
}

MysqlMtaReadsIterator::~MysqlMtaReadsIterator() {
    qDeleteAll(iterators);
}

// TODO: remove copy-paste from this code
bool MysqlMtaReadsIterator::hasNext() {
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

U2AssemblyRead MysqlMtaReadsIterator::next() {
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

U2AssemblyRead MysqlMtaReadsIterator::peek() {
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

MysqlMTAPackAlgorithmDataIterator::MysqlMTAPackAlgorithmDataIterator(QVector< U2DbiIterator<PackAlgorithmData>* >& i, const QVector<QByteArray>& ie)
:  iterators (i), idExtras(ie)
{
    fetchNextData();
}

MysqlMTAPackAlgorithmDataIterator::~MysqlMTAPackAlgorithmDataIterator() {
    qDeleteAll(iterators);
}

bool MysqlMTAPackAlgorithmDataIterator::hasNext() {
    return !nextData.readId.isEmpty();
}

PackAlgorithmData MysqlMTAPackAlgorithmDataIterator::next() {
    PackAlgorithmData res = nextData;
    fetchNextData();
    return res;
}

PackAlgorithmData MysqlMTAPackAlgorithmDataIterator::peek() {
    return nextData;
}

void MysqlMTAPackAlgorithmDataIterator::fetchNextData() {
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

}   // namespace U2
