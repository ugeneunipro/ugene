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

#include <QVarLengthArray>

#include <U2Core/AppContext.h>
#include <U2Core/Timer.h>
#include <U2Core/U2AssemblyUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SqlHelpers.h>

#include <U2Formats/BAMUtils.h>

#include "MysqlAssemblyDbi.h"
#include "MysqlObjectDbi.h"
#include "util/MysqlMultiTableAssemblyAdapter.h"
#include "util/MysqlSingleTableAssemblyAdapter.h"

namespace U2 {

MysqlAssemblyDbi::MysqlAssemblyDbi(MysqlDbi* dbi) : U2AssemblyDbi(dbi), MysqlChildDbiCommon(dbi) {
}

MysqlAssemblyDbi::~MysqlAssemblyDbi() {
    SAFE_POINT(adaptersById.isEmpty(), "Adapters list is not empty", );
}

void MysqlAssemblyDbi::initSqlSchema(U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    // assembly object
    // reference            - reference sequence id
    // imethod - indexing method - method used to handle read location
    // cmethod - method used to handle compression of reads data
    // idata - additional indexing method data
    // cdata - additional compression method data
    U2SqlQuery("CREATE TABLE Assembly (object BIGINT PRIMARY KEY, reference BIGINT, imethod LONGTEXT NOT NULL,"
        " cmethod LONGTEXT NOT NULL, idata LONGBLOB, cdata LONGBLOB, "
        " FOREIGN KEY(object) REFERENCES Object(id), "
        " FOREIGN KEY(reference) REFERENCES Object(id) ON DELETE SET NULL ) "
        " ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();
}

void MysqlAssemblyDbi::shutdown(U2OpStatus& os) {
    foreach(MysqlAssemblyAdapter* a, adaptersById.values()) {
        a->shutdown(os);
        delete a;
    }

    adaptersById.clear();
}

MysqlAssemblyAdapter* MysqlAssemblyDbi::getAdapter(const U2DataId& assemblyId, U2OpStatus& os) {
    qint64 dbiId = U2DbiUtils::toDbiId(assemblyId);
    MysqlAssemblyAdapter* res = adaptersById.value(dbiId);
    if (res != NULL) {
        return res;
    }

    static const QString qString = "SELECT imethod FROM Assembly WHERE object = :object";
    U2SqlQuery q(qString, db, os);
    q.bindDataId(":object", assemblyId);
    if (!q.step()) {
        os.setError(U2DbiL10n::tr("There is no assembly object with the specified id."));
        return NULL;
    }

    res = new MysqlMultiTableAssemblyAdapter(dbi, assemblyId, NULL, db, os);
//    const QString indexMethod = q.getString(0);
//    if (indexMethod == MYSQL_DBI_ASSEMBLY_READ_ELEN_METHOD_SINGLE_TABLE) {
//        res = new SingleTableAssemblyAdapter(dbi, assemblyId, 'S', "", NULL, db, os);
//    } else if (indexMethod == MYSQL_DBI_ASSEMBLY_READ_ELEN_METHOD_MULTITABLE_V1) {
//    } else if (indexMethod == MYSQL_DBI_ASSEMBLY_READ_ELEN_METHOD_RTREE) {
//        res = new RTreeAssemblyAdapter(dbi, assemblyId, NULL, db, os);
//    } else {
//        os.setError(U2DbiL10n::tr("Unsupported reads storage type: %1").arg(indexMethod));
//        return NULL;
//    }

    adaptersById[dbiId] = res;
    return res;
}


U2Assembly MysqlAssemblyDbi::getAssemblyObject(const U2DataId& assemblyId, U2OpStatus& os) {
    U2Assembly res;

    DBI_TYPE_CHECK(assemblyId, U2Type::Assembly, os, res);
    dbi->getMysqlObjectDbi()->getObject(res, assemblyId, os);

    CHECK_OP(os, res);

    U2SqlQuery q("SELECT Assembly.reference, Object.type, '' FROM Assembly, Object "
                " WHERE Assembly.object = :object AND Object.id = Assembly.reference", db, os);
    q.bindDataId(":object", assemblyId);
    if (q.step())  {
        res.referenceId = q.getDataIdExt(0);
        q.ensureDone();
    }

    return res;
}

qint64 MysqlAssemblyDbi::countReads(const U2DataId& assemblyId, const U2Region& r, U2OpStatus& os) {
    GTIMER(c2, t2, "MysqlAssemblyDbi::countReadsAt");
    MysqlAssemblyAdapter* a = getAdapter(assemblyId, os);
    if ( a == NULL ) {
        return -1;
    }
    return a->countReads(r, os);
}


U2DbiIterator<U2AssemblyRead>* MysqlAssemblyDbi::getReads(const U2DataId& assemblyId, const U2Region& r, U2OpStatus& os, bool sortedHint) {
    GTIMER(c2, t2, "MysqlAssemblyDbi::getReadsAt");
    MysqlAssemblyAdapter* a = getAdapter(assemblyId, os);
    if ( a != NULL ) {
        return a->getReads(r, os, sortedHint);
    }
    return NULL;
}

U2DbiIterator<U2AssemblyRead>* MysqlAssemblyDbi::getReadsByRow(const U2DataId& assemblyId, const U2Region& r, qint64 minRow, qint64 maxRow, U2OpStatus& os) {
    GTIMER(c2, t2, "MysqlAssemblyDbi::getReadsAt");

    quint64 t0 = GTimer::currentTimeMicros();
    MysqlAssemblyAdapter* a = getAdapter(assemblyId, os);

    if ( a == NULL ) {
        return NULL;
    }

    U2DbiIterator<U2AssemblyRead>* res = a->getReadsByRow(r, minRow, maxRow, os);

    t2.stop();
    perfLog.trace(QString("Assembly: reads 2D select time: %1 seconds").arg((GTimer::currentTimeMicros() - t0) / float(1000*1000)));

    return res;
}

U2DbiIterator<U2AssemblyRead>* MysqlAssemblyDbi::getReadsByName(const U2DataId& assemblyId, const QByteArray& name, U2OpStatus& os)  {
    GTIMER(c2, t2, "MysqlAssemblyDbi::getReadsByName");
    MysqlAssemblyAdapter* a = getAdapter(assemblyId, os);
    if ( a != NULL ) {
        return a->getReadsByName(name, os);
    }
    return NULL;
}


qint64 MysqlAssemblyDbi::getMaxPackedRow(const U2DataId& assemblyId, const U2Region& r, U2OpStatus& os) {
    quint64 t0 = GTimer::currentTimeMicros();

    MysqlAssemblyAdapter* a = getAdapter(assemblyId, os);

    if ( a == NULL ) {
        return -1;
    }
    qint64 res = a->getMaxPackedRow(r, os);

    perfLog.trace(QString("Assembly: get max packed row: %1 seconds").arg((GTimer::currentTimeMicros() - t0) / (1000*1000)));
    return res;
}



qint64 MysqlAssemblyDbi::getMaxEndPos(const U2DataId& assemblyId, U2OpStatus& os) {
    quint64 t0 = GTimer::currentTimeMicros();

    MysqlAssemblyAdapter* a = getAdapter(assemblyId, os);
    if ( a == NULL ) {
        return -1;
    }
    quint64 res = a->getMaxEndPos(os);

    perfLog.trace(QString("Assembly: get max end pos: %1 seconds").arg((GTimer::currentTimeMicros() - t0) / (1000*1000)));
    return res;
}



void MysqlAssemblyDbi::createAssemblyObject(U2Assembly& assembly,
                                            const QString& folder,
                                            U2DbiIterator<U2AssemblyRead>* it,
                                            U2AssemblyReadsImportInfo& importInfo,
                                            U2OpStatus& os)
{
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    U2Object fakeObject;
    fakeObject.visualName = assembly.visualName;
    fakeObject.trackModType = assembly.trackModType;

    dbi->getMysqlObjectDbi()->createObject(fakeObject, folder, U2DbiObjectRank_TopLevel, os);
    SAFE_POINT_OP(os, );

    assembly.id = U2DbiUtils::toU2DataId(U2DbiUtils::toDbiId(fakeObject.id), U2Type::Assembly, U2DbiUtils::toDbExtra(fakeObject.id));
    assembly.dbiId = fakeObject.dbiId;
    assembly.version = fakeObject.version;

    QString elenMethod = "multi-table-v1";
    //QString elenMethod = dbi->getProperty(Mysql_DBI_ASSEMBLY_READ_ELEN_METHOD_KEY, Mysql_DBI_ASSEMBLY_READ_ELEN_METHOD_RTREE, os);
//    QString elenMethod = dbi->getProperty(Mysql_DBI_ASSEMBLY_READ_ELEN_METHOD_KEY, Mysql_DBI_ASSEMBLY_READ_ELEN_METHOD_MULTITABLE_V1, os);
    //QString elenMethod = dbi->getProperty(Mysql_DBI_ASSEMBLY_READ_ELEN_METHOD_KEY, Mysql_DBI_ASSEMBLY_READ_ELEN_METHOD_SINGLE_TABLE, os);

    U2SqlQuery q("INSERT INTO Assembly(object, reference, imethod, cmethod) VALUES(:object, :reference, :imethod, :cmethod)", db, os);
    q.bindDataId(":object", assembly.id);
    q.bindDataId(":reference", assembly.referenceId);
    q.bindString(":imethod", elenMethod);
    q.bindString(":cmethod", "no-compression");
    q.insert();
    SAFE_POINT_OP(os,);

    MysqlAssemblyAdapter* a = getAdapter(assembly.id, os);
    SAFE_POINT_OP(os,);

    a->createReadsTables(os);
    SAFE_POINT_OP(os,);

    if (it != NULL) {
        addReads(a, it, importInfo, os);
        SAFE_POINT_OP(os,);
    }

    a->createReadsIndexes(os);
    SAFE_POINT_OP(os, );
}

void MysqlAssemblyDbi::finalizeAssemblyObject(U2Assembly &assembly, U2OpStatus &os) {
    U2OpStatusImpl correctTypeOs;
    U2OpStatusImpl removeObjectOs;

    correctAssemblyType(assembly, correctTypeOs);

    if (os.isCoR() || correctTypeOs.isCoR()) {
        dbi->getMysqlObjectDbi()->removeObject(assembly.id, true, removeObjectOs);
    }

    if (!os.isCoR() && correctTypeOs.isCoR()) {
        os.setError(correctTypeOs.getError());
    } else if (!os.isCoR() && removeObjectOs.isCoR()) {
        os.setError(removeObjectOs.getError());
    }
}

void MysqlAssemblyDbi::removeAssemblyData(const U2DataId &assemblyId, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);
    CHECK_OP(os, );

    removeTables(assemblyId, os);
    CHECK_OP(os, );
    removeAssemblyEntry(assemblyId, os);
}

void MysqlAssemblyDbi::updateAssemblyObject(U2Assembly& assembly, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    U2SqlQuery q("UPDATE Assembly SET reference = :reference WHERE object = :object", db, os);
    q.bindDataId(":reference", assembly.referenceId);
    q.bindDataId(":object", assembly.id);
    q.execute();

    SAFE_POINT_OP(os, );

    dbi->getMysqlObjectDbi()->updateObject(assembly, os);
    SAFE_POINT_OP(os, );

    MysqlObjectDbi::incrementVersion(assembly.id, db, os);
    SAFE_POINT_OP(os, );
}

void MysqlAssemblyDbi::removeReads(const U2DataId& assemblyId, const QList<U2DataId>& rowIds, U2OpStatus& os){
    MysqlAssemblyAdapter* a = getAdapter(assemblyId, os);
    if ( a != NULL ) {
        a->removeReads(rowIds, os);
    }
}

void MysqlAssemblyDbi::addReads(MysqlAssemblyAdapter* a, U2DbiIterator<U2AssemblyRead>* it, U2AssemblyReadsImportInfo& ii, U2OpStatus& os) {
    GTIMER(c2, t2, "MysqlAssemblyDbi::addReads");

    quint64 t0 = GTimer::currentTimeMicros();

    a->addReads(it, ii, os);

    t2.stop();
    perfLog.trace(QString("Assembly: %1 reads added in %2 seconds. Auto-packing: %3")
                  .arg(ii.nReads).arg((GTimer::currentTimeMicros() - t0) / float(1000*1000)).arg(ii.packStat.readsCount > 0 ? "yes" : "no"));
}

void MysqlAssemblyDbi::removeTables(const U2DataId &assemblyId, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);
    CHECK_OP(os, );

    AssemblyAdapter* adapter = getAdapter(assemblyId, os);
    CHECK(NULL != adapter, );
    adapter->dropReadsTables(os);
}

void MysqlAssemblyDbi::removeAssemblyEntry(const U2DataId &assemblyId, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);
    CHECK_OP(os, );

    static const QString queryString("DELETE FROM Assembly WHERE object = :object");
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":object", assemblyId);
    q.execute();
}

void MysqlAssemblyDbi::correctAssemblyType(U2Assembly &assembly, U2OpStatus &os) {
    dbi->getMysqlObjectDbi()->updateObjectType(assembly, os);
    SAFE_POINT_OP(os, );
}

void MysqlAssemblyDbi::addReads(const U2DataId& assemblyId, U2DbiIterator<U2AssemblyRead>* it, U2OpStatus& os) {
    MysqlAssemblyAdapter* a = getAdapter(assemblyId, os);
    if ( a != NULL ) {
        U2AssemblyReadsImportInfo ii;
        addReads(a, it, ii, os);
    }
}


/**  Packs assembly rows: assigns packedViewRow value for every read in assembly */
void MysqlAssemblyDbi::pack(const U2DataId& assemblyId, U2AssemblyPackStat& stat, U2OpStatus& os) {
    GTIMER(c2, t2, "MysqlAssemblyDbi::pack");

    quint64 t0 = GTimer::currentTimeMicros();

    MysqlAssemblyAdapter* a = getAdapter(assemblyId, os);
    if ( a == NULL ) {
        return;
    }
    stat.readsCount = a->countReads(U2_REGION_MAX, os);
    a->pack(stat, os);
    perfLog.trace(QString("Assembly: full pack time: %1 seconds").arg((GTimer::currentTimeMicros() - t0) / float(1000*1000)));
}

void MysqlAssemblyDbi::calculateCoverage(const U2DataId& assemblyId, const U2Region& region, U2AssemblyCoverageStat& coverage, U2OpStatus& os) {
    GTIMER(c2, t2, "MysqlAssemblyDbi::calculateCoverage");

    quint64 t0 = GTimer::currentTimeMicros();

    MysqlAssemblyAdapter* a = getAdapter(assemblyId, os);
    if ( a == NULL ) {
        return;
    }
    a->calculateCoverage(region, coverage, os);
    perfLog.trace(QString("Assembly: full coverage calculation time for %2..%3: %1 seconds").arg((GTimer::currentTimeMicros() - t0) / float(1000*1000)).arg(region.startPos).arg(region.endPos()));
}

}   // namespace U2
