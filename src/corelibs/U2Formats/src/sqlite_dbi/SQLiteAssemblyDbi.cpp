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

#include "SQLiteAssemblyDbi.h"
#include "SQLiteObjectDbi.h"
#include "assembly/SingleTableAssemblyAdapter.h"
#include "assembly/RTreeAssemblyAdapter.h"
#include "assembly/MultiTableAssemblyAdapter.h"

#include <SamtoolsAdapter.h>

#include <QVarLengthArray>

#include <U2Core/AppContext.h>
#include <U2Core/Timer.h>
#include <U2Core/U2AssemblyUtils.h>
#include <U2Core/U2SqlHelpers.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/BAMUtils.h>

namespace U2 {

SQLiteAssemblyDbi::SQLiteAssemblyDbi(SQLiteDbi* dbi) : U2AssemblyDbi(dbi), SQLiteChildDBICommon(dbi) {
}

SQLiteAssemblyDbi::~SQLiteAssemblyDbi() {
    assert(adaptersById.isEmpty());
}

void SQLiteAssemblyDbi::initSqlSchema(U2OpStatus& os) {
    if (os.hasError()) {
        return;
    }
    // assembly object
    // reference            - reference sequence id
    // imethod - indexing method - method used to handle read location
    // cmethod - method used to handle compression of reads data
    // idata - additional indexing method data
    // cdata - additional compression method data
    SQLiteWriteQuery(getCreateAssemblyTableQuery(), db, os).execute();
}

void SQLiteAssemblyDbi::shutdown(U2OpStatus& os) {
    foreach(AssemblyAdapter* a, adaptersById.values()) {
        a->shutdown(os);
        delete a;
    }
    adaptersById.clear();
}

AssemblyAdapter* SQLiteAssemblyDbi::getAdapter(const U2DataId& assemblyId, U2OpStatus& os) {
    qint64 sqliteId = U2DbiUtils::toDbiId(assemblyId);
    AssemblyAdapter* res = adaptersById.value(sqliteId);
    if (res != NULL) {
        return res;
    }

    SQLiteReadQuery q("SELECT imethod, cmethod FROM Assembly WHERE object = ?1", db, os);
    q.bindDataId(1, assemblyId);
    if (!q.step()) {
        os.setError(U2DbiL10n::tr("There is no assembly object with the specified id."));
        return NULL;
    }
    QString indexMethod = q.getString(0);
    QByteArray idata = q.getBlob(2);
    assert(!indexMethod.isEmpty());
    //TODO    QString comp = q.getString(1);


    if (indexMethod == SQLITE_DBI_ASSEMBLY_READ_ELEN_METHOD_SINGLE_TABLE) {
        res = new SingleTableAssemblyAdapter(dbi, assemblyId, 'S', "", NULL, db, os);
    } else if (indexMethod == SQLITE_DBI_ASSEMBLY_READ_ELEN_METHOD_MULTITABLE_V1) {
        res = new MultiTableAssemblyAdapter(dbi, assemblyId, NULL, db, os);
    } else if (indexMethod == SQLITE_DBI_ASSEMBLY_READ_ELEN_METHOD_RTREE) {
        res = new RTreeAssemblyAdapter(dbi, assemblyId, NULL, db, os);
    } else {
        os.setError(U2DbiL10n::tr("Unsupported reads storage type: %1").arg(indexMethod));
        return NULL;
    }
    adaptersById[sqliteId] = res;
    return res;
}


U2Assembly SQLiteAssemblyDbi::getAssemblyObject(const U2DataId& assemblyId, U2OpStatus& os) {
    U2Assembly res;

    DBI_TYPE_CHECK(assemblyId, U2Type::Assembly, os, res);
    dbi->getSQLiteObjectDbi()->getObject(res, assemblyId, os);

    CHECK_OP(os, res);

    SQLiteReadQuery q("SELECT Assembly.reference, Object.type, '' FROM Assembly, Object "
                  " WHERE Assembly.object = ?1 AND Object.id = Assembly.reference", db, os);

    q.bindDataId(1, assemblyId);
    if (q.step())  {
        res.referenceId = q.getDataIdExt(0);
        q.ensureDone();
    }

    return res;
}

qint64 SQLiteAssemblyDbi::countReads(const U2DataId& assemblyId, const U2Region& r, U2OpStatus& os) {
    GTIMER(c2, t2, "SQLiteAssemblyDbi::countReadsAt");
    AssemblyAdapter* a = getAdapter(assemblyId, os);
    if ( a == NULL ) {
        return -1;
    }
    return a->countReads(r, os);
}


U2DbiIterator<U2AssemblyRead>* SQLiteAssemblyDbi::getReads(const U2DataId& assemblyId, const U2Region& r, U2OpStatus& os, bool sortedHint) {
    GTIMER(c2, t2, "SQLiteAssemblyDbi::getReadsAt");
    AssemblyAdapter* a = getAdapter(assemblyId, os);
    if ( a != NULL ) {
        return a->getReads(r, os, sortedHint);
    }
    return NULL;
}

U2DbiIterator<U2AssemblyRead>* SQLiteAssemblyDbi::getReadsByRow(const U2DataId& assemblyId, const U2Region& r, qint64 minRow, qint64 maxRow, U2OpStatus& os) {
    GTIMER(c2, t2, "SQLiteAssemblyDbi::getReadsAt");

    quint64 t0 = GTimer::currentTimeMicros();
    AssemblyAdapter* a = getAdapter(assemblyId, os);

    if ( a == NULL ) {
        return NULL;
    }

    U2DbiIterator<U2AssemblyRead>* res = a->getReadsByRow(r, minRow, maxRow, os);

    t2.stop();
    perfLog.trace(QString("Assembly: reads 2D select time: %1 seconds").arg((GTimer::currentTimeMicros() - t0) / float(1000*1000)));

    return res;
}

U2DbiIterator<U2AssemblyRead>* SQLiteAssemblyDbi::getReadsByName(const U2DataId& assemblyId, const QByteArray& name, U2OpStatus& os)  {
    GTIMER(c2, t2, "SQLiteAssemblyDbi::getReadsByName");
    AssemblyAdapter* a = getAdapter(assemblyId, os);
    if ( a != NULL ) {
        return a->getReadsByName(name, os);
    }
    return NULL;
}


qint64 SQLiteAssemblyDbi::getMaxPackedRow(const U2DataId& assemblyId, const U2Region& r, U2OpStatus& os) {
    quint64 t0 = GTimer::currentTimeMicros();

    AssemblyAdapter* a = getAdapter(assemblyId, os);

    if ( a == NULL ) {
        return -1;
    }
    qint64 res = a->getMaxPackedRow(r, os);

    perfLog.trace(QString("Assembly: get max packed row: %1 seconds").arg((GTimer::currentTimeMicros() - t0) / (1000*1000)));
    return res;
}



qint64 SQLiteAssemblyDbi::getMaxEndPos(const U2DataId& assemblyId, U2OpStatus& os) {
    quint64 t0 = GTimer::currentTimeMicros();

    AssemblyAdapter* a = getAdapter(assemblyId, os);
    if ( a == NULL ) {
        return -1;
    }
    quint64 res = a->getMaxEndPos(os);

    perfLog.trace(QString("Assembly: get max end pos: %1 seconds").arg((GTimer::currentTimeMicros() - t0) / (1000*1000)));
    return res;
}



void SQLiteAssemblyDbi::createAssemblyObject(U2Assembly& assembly, const QString& folder,
                                             U2DbiIterator<U2AssemblyRead>* it,
                                             U2AssemblyReadsImportInfo& importInfo,
                                             U2OpStatus& os)
{
    dbi->getSQLiteObjectDbi()->createObject(assembly, folder, U2DbiObjectRank_TopLevel, os);
    SAFE_POINT_OP(os,);

    //QString elenMethod = dbi->getProperty(SQLITE_DBI_ASSEMBLY_READ_ELEN_METHOD_KEY, SQLITE_DBI_ASSEMBLY_READ_ELEN_METHOD_RTREE, os);
    QString elenMethod = dbi->getProperty(SQLITE_DBI_ASSEMBLY_READ_ELEN_METHOD_KEY, SQLITE_DBI_ASSEMBLY_READ_ELEN_METHOD_MULTITABLE_V1, os);
    //QString elenMethod = dbi->getProperty(SQLITE_DBI_ASSEMBLY_READ_ELEN_METHOD_KEY, SQLITE_DBI_ASSEMBLY_READ_ELEN_METHOD_SINGLE_TABLE, os);

    SQLiteWriteQuery q("INSERT INTO Assembly(object, reference, imethod, cmethod) VALUES(?1, ?2, ?3, ?4)", db, os);
    q.bindDataId(1, assembly.id);
    q.bindDataId(2, assembly.referenceId);
    q.bindString(3, elenMethod);
    q.bindString(4, SQLITE_DBI_ASSEMBLY_READ_COMPRESSION_METHOD_NO_COMPRESSION);
    q.insert();
    SAFE_POINT_OP(os,);

    AssemblyAdapter* a = getAdapter(assembly.id, os);
    SAFE_POINT_OP(os,);

    a->createReadsTables(os);
    SAFE_POINT_OP(os,);

    if (it != NULL) {
        addReads(a, it, importInfo, os);
        SAFE_POINT_OP(os,);
    }
}

void SQLiteAssemblyDbi::finalizeAssemblyObject(U2Assembly &assembly, U2OpStatus &os) {
    const quint64 t0 = GTimer::currentTimeMicros();

    AssemblyAdapter* a = getAdapter(assembly.id, os);
    SAFE_POINT_OP(os,);

    a->createReadsIndexes(os);
    SAFE_POINT_OP(os,);

    perfLog.trace(QString("Assembly: re-indexing pack time: %1 seconds").arg((GTimer::currentTimeMicros() - t0) / float(1000 * 1000)));
}

void SQLiteAssemblyDbi::removeAssemblyData(const U2DataId &assemblyId, U2OpStatus &os) {
    SQLiteTransaction t(db, os);
    Q_UNUSED(t);
    CHECK_OP(os, );

    removeTables(assemblyId, os);
    CHECK_OP(os, );
    removeAssemblyEntry(assemblyId, os);
}

void SQLiteAssemblyDbi::updateAssemblyObject(U2Assembly& assembly, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    Q_UNUSED(t);

    SQLiteWriteQuery q("UPDATE Assembly SET reference = ?1 WHERE object = ?2", db, os);
    q.bindDataId(1, assembly.referenceId);
    q.bindDataId(2, assembly.id);
    q.execute();
    SAFE_POINT_OP(os, );

    dbi->getSQLiteObjectDbi()->updateObject(assembly, os);
    SAFE_POINT_OP(os, );

    SQLiteObjectDbi::incrementVersion(assembly.id, db, os);
    SAFE_POINT_OP(os, );
}

void SQLiteAssemblyDbi::removeReads(const U2DataId& assemblyId, const QList<U2DataId>& rowIds, U2OpStatus& os){
    AssemblyAdapter* a = getAdapter(assemblyId, os);
    if ( a != NULL ) {
        a->removeReads(rowIds, os);
    }
}

QString SQLiteAssemblyDbi::getCreateAssemblyTableQuery(const QString &tableAlias) {
    return QString("CREATE TABLE %1 (object INTEGER PRIMARY KEY, "
        "reference INTEGER, imethod TEXT NOT NULL, cmethod TEXT NOT NULL, idata BLOB, cdata BLOB, "
        "FOREIGN KEY(object) REFERENCES Object(id), "
        "FOREIGN KEY(reference) REFERENCES Object(id) ON DELETE SET NULL)").arg(tableAlias);
}

void SQLiteAssemblyDbi::addReads(AssemblyAdapter* a, U2DbiIterator<U2AssemblyRead>* it, U2AssemblyReadsImportInfo& ii, U2OpStatus& os) {
    GTIMER(c2, t2, "SQLiteAssemblyDbi::addReads");

    quint64 t0 = GTimer::currentTimeMicros();

    a->addReads(it, ii, os);

    t2.stop();
    perfLog.trace(QString("Assembly: %1 reads added in %2 seconds. Auto-packing: %3")
                  .arg(ii.nReads).arg((GTimer::currentTimeMicros() - t0) / float(1000*1000)).arg(ii.packStat.readsCount > 0 ? "yes" : "no"));
}

void SQLiteAssemblyDbi::removeTables(const U2DataId &assemblyId, U2OpStatus &os) {
    AssemblyAdapter* adapter = getAdapter(assemblyId, os);
    CHECK(NULL != adapter, );
    adapter->dropReadsTables(os);
}

void SQLiteAssemblyDbi::removeAssemblyEntry(const U2DataId &assemblyId, U2OpStatus &os) {
    static const QString queryString("DELETE FROM Assembly WHERE object = ?1");
    SQLiteWriteQuery q(queryString, db, os);
    CHECK_OP(os, );
    q.bindDataId(1, assemblyId);
    q.execute();
}

void SQLiteAssemblyDbi::addReads(const U2DataId& assemblyId, U2DbiIterator<U2AssemblyRead>* it, U2OpStatus& os) {
    AssemblyAdapter* a = getAdapter(assemblyId, os);
    if ( a != NULL ) {
        U2AssemblyReadsImportInfo ii;
        addReads(a, it, ii, os);
    }
}


/**  Packs assembly rows: assigns packedViewRow value for every read in assembly */
void SQLiteAssemblyDbi::pack(const U2DataId& assemblyId, U2AssemblyPackStat& stat, U2OpStatus& os) {
    GTIMER(c2, t2, "SQLiteAssemblyDbi::pack");

    quint64 t0 = GTimer::currentTimeMicros();

    AssemblyAdapter* a = getAdapter(assemblyId, os);
    if ( a == NULL ) {
        return;
    }
    stat.readsCount = a->countReads(U2_REGION_MAX, os);
    a->pack(stat, os);
    perfLog.trace(QString("Assembly: full pack time: %1 seconds").arg((GTimer::currentTimeMicros() - t0) / float(1000*1000)));
}

void SQLiteAssemblyDbi::calculateCoverage(const U2DataId& assemblyId, const U2Region& region, U2AssemblyCoverageStat & coverage, U2OpStatus& os) {
    GTIMER(c2, t2, "SQLiteAssemblyDbi::calculateCoverage");

    quint64 t0 = GTimer::currentTimeMicros();

    AssemblyAdapter* a = getAdapter(assemblyId, os);
    if ( a == NULL ) {
        return;
    }
    a->calculateCoverage(region, coverage, os);
    perfLog.trace(QString("Assembly: full coverage calculation time for %2..%3: %1 seconds").arg((GTimer::currentTimeMicros() - t0) / float(1000*1000)).arg(region.startPos).arg(region.endPos()));
}


//////////////////////////////////////////////////////////////////////////
// SQLiteAssemblyUtils

static QByteArray getQuality(const U2AssemblyRead &read) {
    if (read->readSequence.length() == read->quality.length()) {
        return read->quality;
    }
    return QByteArray(read->readSequence.length(), char(0xFF));
}

QByteArray SQLiteAssemblyUtils::packData(SQLiteAssemblyDataMethod method, const U2AssemblyRead &read, U2OpStatus& os)
{
    const QByteArray &name = read->name;
    const QByteArray &seq = read->readSequence;
    QByteArray cigarText = U2AssemblyUtils::cigar2String(read->cigar);
    QByteArray qualityString = getQuality(read);
    const QByteArray &rnext = read->rnext;
    QByteArray pnext = QByteArray::number(read->pnext);
    QByteArray aux = SamtoolsAdapter::aux2string(read->aux);

    assert(method == SQLiteAssemblyDataMethod_NSCQ);
    if (method != SQLiteAssemblyDataMethod_NSCQ) {
        os.setError(U2DbiL10n::tr("Packing method is not supported: %1").arg(method));
        return QByteArray();
    }
    int nBytes = 1 + name.length() + 1  + seq.length() + 1 + cigarText.length() + 1 + qualityString.length();
    nBytes += 1 + rnext.length() + 1 + pnext.length();
    if (!aux.isEmpty()) {
        if (!aux.isEmpty()) {
            nBytes += 1 + aux.length();
        }
    }
#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
    QByteArray res(nBytes, Qt::Uninitialized);
#else
    QByteArray res(nBytes, char(0));
#endif
    char* data = res.data();
    int pos = 0;

    // packing type
    data[pos] = '0';
    pos++;

    // name
    memcpy(data + pos, name.constData(), name.length());
    pos+=name.length();
    data[pos] = '\n';
    pos++;

    // sequence
    memcpy(data + pos, seq.constData(), seq.length());
    pos+=seq.length();
    data[pos] = '\n';
    pos++;

    // cigar
    memcpy(data + pos, cigarText.constData(), cigarText.length());
    pos+=cigarText.length();
    data[pos] = '\n';
    pos++;

    // quality
    memcpy(data + pos, qualityString.constData(), qualityString.length());
    pos+=qualityString.length();
    data[pos] = '\n';
    pos++;

    // rnext
    memcpy(data + pos, rnext.constData(), rnext.length());
    pos+=rnext.length();
    data[pos] = '\n';
    pos++;

    // pnext
    memcpy(data + pos, pnext.constData(), pnext.length());
    if (!aux.isEmpty()) {
        pos+=pnext.length();
        data[pos] = '\n';
        pos++;

        // aux
        memcpy(data + pos, aux.constData(), aux.length());
    }

//#define _SQLITE_CHECK_ASSEMBLY_DATA_PACKING_
#ifdef _SQLITE_CHECK_ASSEMBLY_DATA_PACKING_
    U2AssemblyRead tmp(new U2AssemblyReadData());
    unpackData(res, tmp, os);
    QByteArray tmpCigar = U2AssemblyUtils::cigar2String(tmp->cigar);
    QByteArray tmpAux = SamtoolsAdapter::aux2string(tmp->aux);
    QByteArray tmpQual = getQuality(tmp);
    assert(tmp->name == name);
    assert(tmp->readSequence == seq);
    assert(tmpCigar == cigarText);
    assert(tmpQual == qualityString);
    assert(tmp->rnext == read->rnext);
    assert(tmp->pnext == read->pnext);
    assert(tmpAux == aux);
    assert(!os.hasError());
#endif
    return res;
}

void SQLiteAssemblyUtils::unpackData(const QByteArray& packedData, U2AssemblyRead &read, U2OpStatus& os) {
    QByteArray &name = read->name;
    QByteArray &sequence = read->readSequence;
    QByteArray &qualityString = read->quality;

    if (packedData.isEmpty()) {
        os.setError(U2DbiL10n::tr("Packed data are empty!"));
        return;
    }
    const char* data = packedData.constData();

    // packing type
    if (data[0] != '0') {
        os.setError(U2DbiL10n::tr("Packing method prefix is not supported: %1").arg(data));
        return;
    }

    // name
    int nameStart = 1;
    int nameEnd = packedData.indexOf('\n', nameStart);
    if (nameEnd == -1) {
        os.setError(U2DbiL10n::tr("Data are corrupted, no name end marker found: %1").arg(data));
        return;
    }
    name.append(QByteArray(data + nameStart, nameEnd - nameStart));

    // sequence
    int sequenceStart = nameEnd + 1;
    int sequenceEnd = packedData.indexOf('\n', sequenceStart);
    if (sequenceEnd == -1) {
        os.setError(U2DbiL10n::tr("Data are corrupted, no sequence end marker found: %1").arg(data));
        return;
    }
    sequence.append(data + sequenceStart, sequenceEnd - sequenceStart);

    // cigar
    int cigarStart = sequenceEnd + 1;
    int cigarEnd = packedData.indexOf('\n', cigarStart);
    if (cigarEnd == -1) {
        os.setError(U2DbiL10n::tr("Data are corrupted, no CIGAR end marker found: %1").arg(data));
        return;
    }
    QByteArray cigarText(data + cigarStart, cigarEnd - cigarStart);

    // quality
    int qualityStart = cigarEnd + 1;
    int qualityEnd = qualityStart + sequence.length();
    if (qualityEnd > packedData.length()) {
        assert(packedData.length() == qualityStart);
        qualityEnd = packedData.length();
    }
    qualityString.append(data + qualityStart, qualityEnd - qualityStart);

    if (qualityEnd != packedData.length()) {
        // rnext
        int rnextStart = qualityEnd + 1;
        int rnextEnd = packedData.indexOf('\n', rnextStart);
        if (rnextEnd == -1) {
            os.setError(U2DbiL10n::tr("Data are corrupted, no rnext end marker found: %1").arg(data));
            return;
        }
        read->rnext = QByteArray(data + rnextStart, rnextEnd - rnextStart);

        // pnext
        int pnextStart = rnextEnd + 1;
        int pnextEnd = packedData.indexOf('\n', pnextStart);
        if (pnextEnd == -1) {
            pnextEnd = packedData.length();
        }
        QByteArray pnext(data + pnextStart, pnextEnd - pnextStart);
        bool ok = false;
        read->pnext = pnext.toLongLong(&ok);
        if (!ok) {
            os.setError(U2DbiL10n::tr("Can not convert pnext to a number: %1").arg(pnext.data()));
            return;
        }

        // aux
        int auxStart = pnextEnd + 1;
        int auxEnd = packedData.length();
        if (auxStart < auxEnd) {
            read->aux = SamtoolsAdapter::string2aux(QByteArray(data + auxStart, auxEnd - auxStart));
        }
    }

    // parse cigar
    QString err;
    read->cigar = U2AssemblyUtils::parseCigar(cigarText, err);
    if (!err.isEmpty()) {
        os.setError(err);
    }
}
#if (QT_VERSION < 0x050400) //Qt 5.4
namespace {
int removeAll(QVector<U2CigarOp> *vector,const U2CigarOp &t)
{
    const QVector<U2CigarOp>::const_iterator ce = vector->cend(), cit = std::find(vector->cbegin(), ce, t);
    if (cit == ce)
        return 0;
    // next operation detaches, so ce, cit may become invalidated:
    const int firstFoundIdx = std::distance(vector->cbegin(), cit);
    const QVector<U2CigarOp>::iterator e = vector->end(), it = std::remove(vector->begin() + firstFoundIdx, e, t);
    const int result = std::distance(it, e);
    vector->erase(it, e);
    return result;
}
}
#endif
void SQLiteAssemblyUtils::calculateCoverage(SQLiteReadQuery& q, const U2Region& r, U2AssemblyCoverageStat& coverage, U2OpStatus& os) {
    int csize = coverage.size();
    SAFE_POINT(csize > 0, "illegal coverage vector size!", );

    double basesPerRange = double(r.length) / csize;
    while (q.step() && !os.isCoR()) {
        qint64 startPos = q.getInt64(0);
        qint64 len = q.getInt64(1);
        //read data and convert to data with cigar
        QByteArray data = q.getBlob(2);
        U2AssemblyRead read(new U2AssemblyReadData());
        unpackData(data,read,os);

        U2Region readRegion(startPos, len);
        U2Region readCroppedRegion = readRegion.intersect(r);

        if (readCroppedRegion.isEmpty()) {
            continue;
        }

        // we have used effective length of the read, so insertions/deletions are already taken into account
        // cigarString can be longer than needed
        QVector<U2CigarOp> cigarVector;
        foreach (const U2CigarToken &cigar, read->cigar) {
            cigarVector += QVector<U2CigarOp>(cigar.count, cigar.op);
        }
#if (QT_VERSION < 0x050400) //Qt 5.4
        removeAll(&cigarVector,U2CigarOp_I);
        removeAll(&cigarVector,U2CigarOp_S);
        removeAll(&cigarVector,U2CigarOp_P);
#else
        cigarVector.removeAll(U2CigarOp_I);
        cigarVector.removeAll(U2CigarOp_S);
        cigarVector.removeAll(U2CigarOp_P);
#endif

        if(r.startPos > startPos){
            cigarVector = cigarVector.mid(r.startPos - startPos);//cut unneeded cigar string
        }

        int firstCoverageIdx = (int)((readCroppedRegion.startPos - r.startPos)/ basesPerRange);
        int lastCoverageIdx = (int)((readCroppedRegion.startPos + readCroppedRegion.length - r.startPos ) / basesPerRange) - 1;
        for (int i = firstCoverageIdx; i <= lastCoverageIdx && i < csize; i++) {
            switch (cigarVector[(i-firstCoverageIdx)*basesPerRange]){
            case U2CigarOp_D: // skip the deletion
            case U2CigarOp_N: // skip the skiped
                continue;
            default:
                coverage[i]++;
            }

        }
    }
}

void SQLiteAssemblyUtils::addToCoverage(U2AssemblyCoverageImportInfo& ii, const U2AssemblyRead& read) {
    if (!ii.computeCoverage) {
        return;
    }
    int csize = ii.coverage.size();

    QVector<U2CigarOp> cigarVector;
    foreach (const U2CigarToken &cigar, read->cigar) {
        cigarVector += QVector<U2CigarOp>(cigar.count, cigar.op);
    }
#if (QT_VERSION < 0x050400) //Qt 5.4
    removeAll(&cigarVector,U2CigarOp_I);
    removeAll(&cigarVector,U2CigarOp_S);
    removeAll(&cigarVector,U2CigarOp_P);
#else
    cigarVector.removeAll(U2CigarOp_I);
    cigarVector.removeAll(U2CigarOp_S);
    cigarVector.removeAll(U2CigarOp_P);
#endif

    int startPos = (int)(read->leftmostPos / ii.coverageBasesPerPoint);
    int endPos = (int)((read->leftmostPos + read->effectiveLen) / ii.coverageBasesPerPoint) - 1;
    if(endPos > csize - 1) {
        endPos = csize - 1;
    }
    int* coverageData = ii.coverage.data();
    for (int i = startPos; i <= endPos && i < csize; i++) {
        switch (cigarVector[(i-startPos)*ii.coverageBasesPerPoint]){
        case U2CigarOp_D: // skip the deletion
        case U2CigarOp_N: // skip the skiped
            continue;
        default:
            coverageData[i]++;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// read loader
U2AssemblyRead SimpleAssemblyReadLoader::load(SQLiteQuery* q) {
    U2AssemblyRead read(new U2AssemblyReadData());

    read->id = q->getDataId(0, U2Type::AssemblyRead);
    read->packedViewRow = q->getInt64(1);
    if (q->hasError()) {
        return U2AssemblyRead();
    }
    read->leftmostPos= q->getInt64(2);
    read->effectiveLen = q->getInt64(3);
    read->flags = q->getInt64(4);
    read->mappingQuality = (quint8)q->getInt32(5);
    QByteArray data = q->getBlob(6);
    if (q->hasError()) {
        return U2AssemblyRead();
    }
    SQLiteAssemblyUtils::unpackData(data, read, q->getOpStatus());
    if (q->hasError()) {
        return U2AssemblyRead();
    }
#ifdef _DEBUG
    //additional check to ensure that db stores correct info
    qint64 effectiveLengthFromCigar = read->readSequence.length() + U2AssemblyUtils::getCigarExtraLength(read->cigar);
    assert(effectiveLengthFromCigar == read->effectiveLen);
#endif
    return read;
}


PackAlgorithmData SimpleAssemblyReadPackedDataLoader::load(SQLiteQuery* q) {
    PackAlgorithmData data;
    data.readId = q->getDataId(0, U2Type::AssemblyRead);
    data.leftmostPos = q->getInt64(1);
    data.effectiveLen = q->getInt64(2);
    return data;
}


} //namespace
