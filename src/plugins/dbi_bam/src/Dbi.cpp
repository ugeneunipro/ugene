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

#include <QDir>

#include "IOException.h"
#include "CancelledException.h"
#include "BAMDbiPlugin.h"
#include "Dbi.h"
#include "BAMFormat.h"

#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/AppContext.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/IOAdapterUtils.h>

#include <3rdparty/sqlite3/sqlite3.h>

namespace U2 {
namespace BAM {

// Dbi

Dbi::Dbi() : U2AbstractDbi(DbiFactory::ID), assembliesCount(0)
{
}

void Dbi::init(const QHash<QString, QString> &properties, const QVariantMap & /*persistentData*/, U2OpStatus &os) {
    try {
        if(U2DbiState_Void != state) {
            throw Exception(BAMDbiPlugin::tr("Invalid DBI state"));
        }
        state = U2DbiState_Starting;
        if(properties.value("url").isEmpty()) {
            throw Exception(BAMDbiPlugin::tr("URL is not specified"));
        }
        url = GUrl(properties.value(U2DbiOptions::U2_DBI_OPTION_URL));
        if(!url.isLocalFile()) {
            throw Exception(BAMDbiPlugin::tr("Non-local files are not supported"));
        }
        {
            IOAdapterFactory *factory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url));
            ioAdapter.reset(factory->createIOAdapter());
        }
        if(!ioAdapter->open(url, IOAdapterMode_Read)) {
            throw IOException(BAMDbiPlugin::tr("Can't open file '%1'").arg(url.getURLString()));
        }
        reader.reset(new BamReader(*ioAdapter));
        QFileInfo fileInfo(url.getURLString());
        sqliteUrl = GUrl(QDir::temp().absoluteFilePath(url.fileName() + "." + QString::number(fileInfo.lastModified().toTime_t()) + "." + QString::number(fileInfo.size()) + ".sqlite"));
        bool exists = false;
        if(QFile::exists(sqliteUrl.getURLString())) {
            exists = true;
        }

        if(SQLITE_OK != sqlite3_open(sqliteUrl.getURLString().toUtf8().constData(), &dbRef.handle)) {
            throw IOException(BAMDbiPlugin::tr("Can't open index database"));
        }

        dbRef.useTransaction = true;
        if(!exists) {
            buildIndex(os);
        }
        assembliesCount = reader->getHeader().getReferences().size();
        objectDbi.reset(new ObjectDbi(*this, dbRef, assembliesCount));
        {
            QList<qint64> maxReadLengths;
            for(int index = 0;index < assembliesCount;index++) {
                U2OpStatusImpl opStatus;
                SQLiteReadQuery q("SELECT maxReadLength FROM assemblies WHERE id = ?1;", &dbRef, opStatus);
                q.bindInt64(1, index + 1);
                maxReadLengths.append(q.selectInt64());
                if(opStatus.hasError()) {
                    throw Exception(opStatus.getError());
                }
            }
            assemblyDbi.reset(new AssemblyDbi(*this, *reader, dbRef, maxReadLengths));
        }
        initProperties = properties;
        features.insert(U2DbiFeature_ReadSequence);
        features.insert(U2DbiFeature_ReadAssembly);
        features.insert(U2DbiFeature_AssemblyReadsPacking);
        dbiId = url.getURLString();
        state = U2DbiState_Ready;
    } catch(const Exception &e) {
        os.setError(e.getMessage());
        assemblyDbi.reset();
        objectDbi.reset();
        reader.reset();
        ioAdapter.reset();
        if(NULL != dbRef.handle) {
            sqlite3_close(dbRef.handle);
            dbRef.handle = NULL;
        }
        assert(sqliteUrl.isLocalFile());
        QFile::remove(sqliteUrl.getURLString());
        state = U2DbiState_Void;
    }
}

QVariantMap Dbi::shutdown(U2OpStatus &os) {
    try {
        if(U2DbiState_Ready != state) {
            throw Exception(BAMDbiPlugin::tr("Invalid DBI state"));
        }
        state = U2DbiState_Stopping;
        assemblyDbi.reset();
        objectDbi.reset();
        reader.reset();
        ioAdapter.reset();
        if(NULL != dbRef.handle) {
            sqlite3_close(dbRef.handle);
            dbRef.handle = NULL;
        }
        state = U2DbiState_Void;
        return QVariantMap();
    } catch(const Exception &e) {
        os.setError(e.getMessage());
        return QVariantMap();
    }
}

QHash<QString, QString> Dbi::getDbiMetaInfo(U2OpStatus &os) {
    try {
        if(U2DbiState_Ready != state) {
            throw Exception(BAMDbiPlugin::tr("Invalid DBI state"));
        }
        QHash<QString, QString> result;
        result["url"] = url.getURLString();
        return result;
    } catch(const Exception &e) {
        os.setError(e.getMessage());
        return QHash<QString, QString>();
    }
}

U2DataType Dbi::getEntityTypeById(const U2DataId& id) const {
    if(id.isEmpty()) {
        return U2Type::Unknown;
    } else {
        quint64 dbId = U2DbiUtils::toDbiId(id);
        if(dbId <= (quint64)assembliesCount) {
            return U2Type::Assembly;
        } else {
            return U2Type::AssemblyRead;
        }
    }
}

U2ObjectDbi *Dbi::getObjectDbi() {
    if(U2DbiState_Ready == state) {
        return objectDbi.data();
    } else {
        return NULL;
    }
}

U2AssemblyDbi *Dbi::getAssemblyDbi() {
    if(U2DbiState_Ready == state) {
        return assemblyDbi.data();
    } else {
        return NULL;
    }
}

bool Dbi::isReadOnly() const {
    return SQLiteUtils::isDatabaseReadOnly(&dbRef, "main") == 1;
}


void Dbi::buildIndex(U2OpStatus &os) {
    {
        U2OpStatusImpl opStatus;
        int tableCount = SQLiteReadQuery("SELECT COUNT(*) FROM sqlite_master WHERE type = 'table' AND (name = 'assemblies' OR name = 'assemblyReads');", &dbRef, opStatus).selectInt64();
        if (opStatus.hasError()) {
            throw Exception(opStatus.getError());
        }
        if(2 == tableCount) {
            return;
        }
    }
    {
        U2OpStatusImpl opStatus;
        SQLiteWriteQuery("DROP TABLE IF EXISTS assemblyReads;", &dbRef, opStatus).execute();
        if(opStatus.hasError()) {
            throw Exception(opStatus.getError());
        }
    }
    {
        U2OpStatusImpl opStatus;
        SQLiteWriteQuery("DROP TABLE IF EXISTS assemblies;", &dbRef, opStatus).execute();
        if(opStatus.hasError()) {
            throw Exception(opStatus.getError());
        }
    }
    {
        U2OpStatusImpl transactionOpStatus;
        SQLiteTransaction t(&dbRef, transactionOpStatus);
        if (transactionOpStatus.hasError()) {
            throw Exception(transactionOpStatus.getError());
        }
        try {
            {
                U2OpStatusImpl opStatus;
                SQLiteWriteQuery("CREATE TABLE assemblies (id INTEGER PRIMARY KEY, maxEndPos INTEGER, maxReadLength INTEGER);",&dbRef, opStatus).execute();
                if(opStatus.hasError()) {
                    throw Exception(opStatus.getError());
                }
            }
            {
                U2OpStatusImpl opStatus;
                SQLiteWriteQuery("CREATE TABLE assemblyReads (id INTEGER PRIMARY KEY, assemblyId INTEGER REFERENCES assemblies, startPosition INTEGER, endPosition INTEGER, packedRow INTEGER);", &dbRef, opStatus).execute();
                if(opStatus.hasError()) {
                    throw Exception(opStatus.getError());
                }
            }
            const Header &header = reader->getHeader();
            QList<qint64> maxEndPositions;
            QList<qint64> maxReadLengths;
            QList<QList<qint64> > rowEnds;
            QList<qint64> columnStarts;
            QList<qint64> packedRows;
            for(int referenceId = 0;referenceId < header.getReferences().size();referenceId++) {
                {
                    U2OpStatusImpl opStatus;
                    SQLiteWriteQuery insertAssemblyQ("INSERT INTO assemblies(id) VALUES (?1);", &dbRef, opStatus);
                    insertAssemblyQ.bindInt64(1, referenceId + 1);
                    insertAssemblyQ.execute();
                    if(opStatus.hasError()) {
                        throw Exception(opStatus.getError());
                    }
                }
                maxEndPositions.append(0);
                maxReadLengths.append(0);
                rowEnds.append(QList<qint64>());
                columnStarts.append(0);
                packedRows.append(0);
            }
            {
                U2OpStatusImpl insertReadOpStatus;
                SQLiteWriteQuery insertReadQ("INSERT INTO assemblyReads(id, assemblyId, startPosition, endPosition, packedRow) VALUES (?1, ?2, ?3, ?4, ?5);", &dbRef, insertReadOpStatus);
                while(!reader->isEof()) {
                    VirtualOffset alignmentOffset = reader->getOffset();
                    Alignment alignment = reader->readAlignment();
                    if(-1 != alignment.getReferenceId()) {
                        qint64 readLength = Alignment::computeLength(alignment.getCigar());
                        qint64 endPosition = alignment.getPosition() + readLength;
                        if(alignment.getPosition() - columnStarts[alignment.getReferenceId()] > COLUMN_DISTANCE) {
                            packedRows[alignment.getReferenceId()] = 0;
                            columnStarts[alignment.getReferenceId()] = alignment.getPosition();
                        }
                        while(packedRows[alignment.getReferenceId()] < rowEnds[alignment.getReferenceId()].size() &&
                            rowEnds[alignment.getReferenceId()][packedRows[alignment.getReferenceId()]] >= alignment.getPosition()) {
                                packedRows[alignment.getReferenceId()]++;
                        }
                        if(rowEnds[alignment.getReferenceId()].size() == packedRows[alignment.getReferenceId()]) {
                            rowEnds[alignment.getReferenceId()].append(0);
                        }
                        rowEnds[alignment.getReferenceId()][packedRows[alignment.getReferenceId()]] = endPosition;

                        insertReadQ.reset();
                        insertReadQ.bindInt64(1, alignmentOffset.getPackedOffset());
                        insertReadQ.bindInt64(2, alignment.getReferenceId() + 1);
                        insertReadQ.bindInt64(3, alignment.getPosition());
                        insertReadQ.bindInt64(4, endPosition);
                        insertReadQ.bindInt64(5, packedRows[alignment.getReferenceId()]);
                        insertReadQ.execute();
                        if(insertReadOpStatus.hasError()) {
                            throw Exception(insertReadOpStatus.getError());
                        }
                        maxEndPositions[alignment.getReferenceId()] = qMax(maxEndPositions[alignment.getReferenceId()], endPosition);
                        maxReadLengths[alignment.getReferenceId()] = qMax(maxReadLengths[alignment.getReferenceId()], readLength);
                    }
                    if(os.isCanceled()) {
                        throw CancelledException(BAMDbiPlugin::tr("Operation was cancelled"));
                    }
                    os.setProgress(ioAdapter->getProgress());
                }
            }
            for(int referenceId = 0;referenceId < header.getReferences().size();referenceId++) {
                U2OpStatusImpl opStatus;
                SQLiteWriteQuery updateEndQ("UPDATE assemblies SET maxEndPos = ?1, maxReadLength = ?2 WHERE id = ?3;", &dbRef, opStatus);
                updateEndQ.reset();
                updateEndQ.bindInt64(1, maxEndPositions[referenceId]);
                updateEndQ.bindInt64(2, maxReadLengths[referenceId]);
                updateEndQ.bindInt64(3, referenceId + 1);
                updateEndQ.execute();
                if(opStatus.hasError()) {
                    throw Exception(opStatus.getError());
                }
            }
            {
                U2OpStatusImpl opStatus;
                SQLiteWriteQuery("CREATE INDEX startPosition ON assemblyReads(assemblyId, startPosition)", &dbRef, opStatus).execute();
                if(opStatus.hasError()) {
                    throw Exception(opStatus.getError());
                }
            }
        } catch(const Exception &e) {
            transactionOpStatus.setError(e.getMessage());
            throw;
        }
    }
}

// DbiFactory

DbiFactory::DbiFactory():
    U2DbiFactory()
{
}

U2Dbi *DbiFactory::createDbi() {
    return new Dbi();
}

U2DbiFactoryId DbiFactory::getId()const {
    return ID;
}

FormatCheckResult DbiFactory::isValidDbi(const QHash<QString, QString> &properties, const QByteArray &rawData, U2OpStatus & /*os*/) const {
    BAMFormatUtils f;
    FormatCheckResult res = f.checkRawData(rawData, properties.value(U2DbiOptions::U2_DBI_OPTION_URL));
    return res;
}


bool DbiFactory::isDbiExists(const U2DbiId& id) const {
    return QFile::exists(id);
}

const QString DbiFactory::ID = "BAMDbi";

// ObjectDbi

ObjectDbi::ObjectDbi(Dbi &dbi, DbRef &dbRef, int assembliesCount):
    U2SimpleObjectDbi(&dbi),
    dbi(dbi),
    dbRef(dbRef),
    assembliesCount(assembliesCount)
{
}

qint64 ObjectDbi::countObjects(U2OpStatus &os) {
    return countObjects(U2Type::Assembly, os);
}

qint64 ObjectDbi::countObjects(U2DataType type, U2OpStatus &os) {
    try {
        if(U2DbiState_Ready != dbi.getState()) {
            throw Exception(BAMDbiPlugin::tr("Invalid DBI state"));
        }
        if(U2Type::Assembly == type) {
            return assembliesCount;
        } else {
            return 0;
        }
    } catch(const Exception &e) {
        os.setError(e.getMessage());
        return 0;
    }
}

QHash<U2DataId, QString> ObjectDbi::getObjectNames(qint64 offset, qint64 count, U2OpStatus &os) {
    QHash<U2DataId, QString> result;

    try {
        if(U2DbiState_Ready != dbi.getState()) {
            throw Exception(BAMDbiPlugin::tr("Invalid DBI state"));
        }

        U2OpStatusImpl opStatus;
        SQLiteReadQuery q("SELECT id, name FROM Object WHERE type = ?1;", offset, count, &dbRef, opStatus);
        q.bindType(1, U2Type::Assembly);

        while (q.step()) {
            const U2DataId id = q.getDataId(0, U2Type::Assembly);
            const QString name = q.getString(1);
            result.insert(id, name);
        }
        return result;
    } catch(const Exception &e) {
        os.setError(e.getMessage());
        return result;
    }
}

QList<U2DataId> ObjectDbi::getObjects(qint64 offset, qint64 count, U2OpStatus &os) {
    return getObjects(U2Type::Assembly, offset, count, os);
}

QList<U2DataId> ObjectDbi::getObjects(U2DataType type, qint64 offset, qint64 count, U2OpStatus &os) {
    try {
        if(U2DbiState_Ready != dbi.getState()) {
            throw Exception(BAMDbiPlugin::tr("Invalid DBI state"));
        }
        if(U2Type::Assembly == type) {
            U2OpStatusImpl opStatus;
            SQLiteReadQuery q("SELECT id FROM assemblies;", offset, count, &dbRef, opStatus);
            QList<U2DataId> result = q.selectDataIds(0);
            return result;
        } else {
            return QList<U2DataId>();
        }
    } catch(const Exception &e) {
        os.setError(e.getMessage());
        return QList<U2DataId>();
    }
}

QList<U2DataId> ObjectDbi::getParents(const U2DataId& /*entityId*/, U2OpStatus &os) {
    try {
        if(U2DbiState_Ready != dbi.getState()) {
            throw Exception(BAMDbiPlugin::tr("Invalid DBI state"));
        }
        return QList<U2DataId>();
    } catch(const Exception &e) {
        os.setError(e.getMessage());
        return QList<U2DataId>();
    }
}

QStringList ObjectDbi::getFolders(U2OpStatus &os) {
    try {
        if(U2DbiState_Ready != dbi.getState()) {
            throw Exception(BAMDbiPlugin::tr("Invalid DBI state"));
        }
        return QStringList("/");
    } catch(const Exception &e) {
        os.setError(e.getMessage());
        return QStringList();
    }
}

qint64 ObjectDbi::countObjects(const QString &folder, U2OpStatus &os) {
    try {
        if(U2DbiState_Ready != dbi.getState()) {
            throw Exception(BAMDbiPlugin::tr("Invalid DBI state"));
        }
        if("/" != folder) {
            throw Exception(BAMDbiPlugin::tr("No such folder: %1").arg(folder));
        }
        return countObjects(os);
    } catch(const Exception &e) {
        os.setError(e.getMessage());
        return 0;
    }
}

QList<U2DataId> ObjectDbi::getObjects(const QString &folder, qint64 offset, qint64 count, U2OpStatus &os) {
    try {
        if(U2DbiState_Ready != dbi.getState()) {
            throw Exception(BAMDbiPlugin::tr("Invalid DBI state"));
        }
        if("/" != folder) {
            throw Exception(BAMDbiPlugin::tr("No such folder: %1").arg(folder));
        }
        return getObjects(offset, count, os);
    } catch(const Exception &e) {
        os.setError(e.getMessage());
        return QList<U2DataId>();
    }
}

QStringList ObjectDbi::getObjectFolders(const U2DataId& objectId, U2OpStatus &os) {
    try {
        if(U2DbiState_Ready != dbi.getState()) {
            throw Exception(BAMDbiPlugin::tr("Invalid DBI state"));
        }
        if(U2Type::Assembly == dbi.getEntityTypeById(objectId)) {
            return QStringList("/");
        } else {
            return QStringList();
        }
    } catch(const Exception &e) {
        os.setError(e.getMessage());
        return QStringList();
    }
}

qint64 ObjectDbi::getObjectVersion(const U2DataId& /*objectId*/, U2OpStatus &os) {
    try {
        if(U2DbiState_Ready != dbi.getState()) {
            throw Exception(BAMDbiPlugin::tr("Invalid DBI state"));
        }
        return 0;
    } catch(const Exception &e) {
        os.setError(e.getMessage());
        return 0;
    }
}

qint64 ObjectDbi::getFolderLocalVersion(const QString &folder, U2OpStatus &os) {
    try {
        if(U2DbiState_Ready != dbi.getState()) {
            throw Exception(BAMDbiPlugin::tr("Invalid DBI state"));
        }
        if("/" != folder) {
            throw Exception(BAMDbiPlugin::tr("No such folder: %1").arg(folder));
        }
        return 0;
    } catch(const Exception &e) {
        os.setError(e.getMessage());
        return 0;
    }
}

qint64 ObjectDbi::getFolderGlobalVersion(const QString &folder, U2OpStatus &os) {
    try {
        if(U2DbiState_Ready != dbi.getState()) {
            throw Exception(BAMDbiPlugin::tr("Invalid DBI state"));
        }
        if("/" != folder) {
            throw Exception(BAMDbiPlugin::tr("No such folder: %1").arg(folder));
        }
        return 0;
    } catch(const Exception &e) {
        os.setError(e.getMessage());
        return 0;
    }
}

U2DbiIterator<U2DataId>* ObjectDbi::getObjectsByVisualName(const QString& , U2DataType , U2OpStatus& ) {
    //todo:
    return NULL;
}

void ObjectDbi::renameObject(const U2DataId & /*id*/, const QString & /*newName*/, U2OpStatus &os) {
    os.setError("Not implemented!");
}

void ObjectDbi::setObjectRank(const U2DataId & /*objectId*/, U2DbiObjectRank /*newRank*/, U2OpStatus &os) {
    os.setError("Not implemented!");
}

U2DbiObjectRank ObjectDbi::getObjectRank(const U2DataId & /*objectId*/, U2OpStatus &os) {
    os.setError("Not implemented!");
    return U2DbiObjectRank_TopLevel;
}

void ObjectDbi::setParent(const U2DataId& /*parentId*/, const U2DataId& /*childId*/, U2OpStatus& os){
    os.setError("Not implemented!");
}

// AssemblyDbi
AssemblyDbi::AssemblyDbi(Dbi &dbi, BamReader &reader, DbRef &dbRef, QList<qint64> maxReadLengths):
    U2SimpleAssemblyDbi(&dbi),
    dbi(dbi),
    reader(reader),
    dbRef(dbRef),
    maxReadLengths(maxReadLengths)
{
}

U2Assembly AssemblyDbi::getAssemblyObject(const U2DataId& id, U2OpStatus &os) {
    try {
        if(U2DbiState_Ready != dbi.getState()) {
            throw Exception(BAMDbiPlugin::tr("Invalid DBI state"));
        }
        if(dbi.getEntityTypeById(id) != U2Type::Assembly) {
            throw Exception(BAMDbiPlugin::tr("The specified object is not an assembly"));
        }
        U2Assembly result;
        result.id = id;
        result.dbiId = dbi.getDbiId();
        quint64 dbiDataId = U2DbiUtils::toDbiId(id);
        result.visualName = reader.getHeader().getReferences()[dbiDataId - 1].getName();
        return result;
    } catch(const Exception &e) {
        os.setError(e.getMessage());
        return U2Assembly();
    }
}

qint64 AssemblyDbi::countReads(const U2DataId& assemblyId, const U2Region &r, U2OpStatus &os) {
    try {
        if(U2DbiState_Ready != dbi.getState()) {
            throw Exception(BAMDbiPlugin::tr("Invalid DBI state"));
        }
        if(dbi.getEntityTypeById(assemblyId) != U2Type::Assembly) {
            throw Exception(BAMDbiPlugin::tr("The specified object is not an assembly"));
        }
        qint64 result;
        {
            U2OpStatusImpl opStatus;
            SQLiteReadQuery q("SELECT COUNT(*) FROM assemblyReads WHERE assemblyId = ?1 AND startPosition < ?2 AND startPosition > ?3 AND endPosition > ?4;", &dbRef, opStatus);
            q.bindDataId(1, assemblyId);
            q.bindInt64(2, r.endPos());
            q.bindInt64(3, r.startPos - getMaxReadLength(assemblyId, r));
            q.bindInt64(4, r.startPos);
            result = q.selectInt64();
            if(opStatus.hasError()) {
                throw Exception(opStatus.getError());
            }
        }
        return result;
    } catch(const Exception &e) {
        os.setError(e.getMessage());
        return 0;
    }
}

U2DbiIterator<U2AssemblyRead>* AssemblyDbi::getReads(const U2DataId& assemblyId, const U2Region& r, U2OpStatus& os, bool ) {
    try {
        if(U2DbiState_Ready != dbi.getState()) {
            throw Exception(BAMDbiPlugin::tr("Invalid DBI state"));
        }
        if(dbi.getEntityTypeById(assemblyId) != U2Type::Assembly) {
            throw Exception(BAMDbiPlugin::tr("The specified object is not an assembly"));
        }
        QList<U2DataId> rowIds;
        QList<qint64> packedRows;
        {
            U2OpStatusImpl opStatus;
            SQLiteReadQuery q("SELECT id, packedRow FROM assemblyReads WHERE assemblyId = ?1 AND startPosition < ?2 AND startPosition > ?3 AND endPosition > ?4", &dbRef, opStatus);
            q.bindDataId(1, assemblyId);
            q.bindInt64(2, r.endPos());
            q.bindInt64(3, r.startPos - getMaxReadLength(assemblyId, r));
            q.bindInt64(4, r.startPos);

            while(q.step()) {
                rowIds.append(U2DbiUtils::toU2DataId(q.getInt64(0), U2Type::AssemblyRead));
                packedRows.append(q.getInt64(1));
            }
            if(opStatus.hasError()) {
                throw Exception(opStatus.getError());
            }
        }
        QList<U2AssemblyRead> result;
        {
            U2OpStatusImpl opStatus;
            result = getReadsByIds(rowIds, packedRows, opStatus);
            if(opStatus.hasError()) {
                throw Exception(opStatus.getError());
            }
        }
        return new BufferedDbiIterator<U2AssemblyRead>(result, U2AssemblyRead());
    } catch(const Exception &e) {
        os.setError(e.getMessage());
        return NULL;
    }
}

U2AssemblyRead AssemblyDbi::getReadById(const U2DataId& rowId, U2OpStatus &os) {
    try {
        if(U2DbiState_Ready != dbi.getState()) {
            throw Exception(BAMDbiPlugin::tr("Invalid DBI state"));
        }
        if(dbi.getEntityTypeById(rowId) != U2Type::AssemblyRead) {
            throw Exception(BAMDbiPlugin::tr("The specified object is not an assembly read"));
        }
        qint64 packedRow = 0;
        {
            U2OpStatusImpl opStatus;
            SQLiteReadQuery q("SELECT packedRow FROM assemblyReads WHERE id = ?1;", &dbRef, opStatus);
            q.bindDataId(1, rowId);
            packedRow = q.getInt64(0);
            if(opStatus.hasError()) {
                throw Exception(opStatus.getError());
            }
        }
        U2AssemblyRead result(new U2AssemblyReadData());
        {
            U2OpStatusImpl opStatus;
            result = getReadById(rowId, packedRow, opStatus);
            if(opStatus.hasError()) {
                throw Exception(opStatus.getError());
            }
        }
        return result;
    } catch(const Exception &e) {
        os.setError(e.getMessage());
        return U2AssemblyRead();
    }
}

qint64 AssemblyDbi::getMaxPackedRow(const U2DataId& assemblyId, const U2Region &r, U2OpStatus &os) {
    try {
        if(U2DbiState_Ready != dbi.getState()) {
            throw Exception(BAMDbiPlugin::tr("Invalid DBI state"));
        }
        if(dbi.getEntityTypeById(assemblyId) != U2Type::Assembly) {
            throw Exception(BAMDbiPlugin::tr("The specified object is not an assembly"));
        }
        qint64 result = 0;
        {
            U2OpStatusImpl opStatus;
            SQLiteReadQuery q("SELECT MAX(packedRow) FROM assemblyReads WHERE assemblyId = ?1 AND startPosition < ?2 AND startPosition > ?3 AND endPosition > ?4;", &dbRef, opStatus);
            q.bindDataId(1, assemblyId);
            q.bindInt64(2, r.endPos());
            q.bindInt64(3, r.startPos - getMaxReadLength(assemblyId, r));
            q.bindInt64(4, r.startPos);
            result = q.selectInt64();
            if(opStatus.hasError()) {
                throw Exception(opStatus.getError());
            }
        }
        return result;
    } catch(const Exception &e) {
        os.setError(e.getMessage());
        return 0;
    }
}

U2DbiIterator<U2AssemblyRead>* AssemblyDbi::getReadsByRow(const U2DataId& assemblyId, const U2Region& r, qint64 minRow, qint64 maxRow, U2OpStatus& os) {
    try {
        if(U2DbiState_Ready != dbi.getState()) {
            throw Exception(BAMDbiPlugin::tr("Invalid DBI state"));
        }
        if(dbi.getEntityTypeById(assemblyId) != U2Type::Assembly) {
            throw Exception(BAMDbiPlugin::tr("The specified object is not an assembly"));
        }
        QList<U2DataId> rowIds;
        QList<qint64> packedRows;
        {
            U2OpStatusImpl opStatus;
            SQLiteReadQuery q("SELECT id, packedRow FROM assemblyReads WHERE assemblyId = ?1 AND startPosition < ?2 AND startPosition > ?3 AND endPosition > ?4 AND packedRow >= ?5 AND packedRow <= ?6;", &dbRef, opStatus);
            q.bindDataId(1, assemblyId);
            q.bindInt64(2, r.endPos());
            q.bindInt64(3, r.startPos - getMaxReadLength(assemblyId, r));
            q.bindInt64(4, r.startPos);
            q.bindInt64(5, minRow);
            q.bindInt64(6, maxRow);
            while(q.step()) {
                rowIds.append(q.getDataId(0, U2Type::AssemblyRead));
                packedRows.append(q.getInt64(1));
            }
            if(opStatus.hasError()) {
                throw Exception(opStatus.getError());
            }
        }
        QList<U2AssemblyRead> result;
        {
            U2OpStatusImpl opStatus;
            result = getReadsByIds(rowIds, packedRows, opStatus);
            if(opStatus.hasError()) {
                throw Exception(opStatus.getError());
            }
        }
        return new BufferedDbiIterator<U2AssemblyRead>(result, U2AssemblyRead());
    } catch(const Exception &e) {
        os.setError(e.getMessage());
        return NULL;
    }
}

U2DbiIterator<U2AssemblyRead>* AssemblyDbi::getReadsByName(const U2DataId&, const QByteArray&, U2OpStatus& os) {
    os.setError("Operation not supported: BAM::AssemblyDbi::getReadsByName");
    return NULL;
}

qint64 AssemblyDbi::getMaxEndPos(const U2DataId& assemblyId, U2OpStatus &os) {
    try {
        if(U2DbiState_Ready != dbi.getState()) {
            throw Exception(BAMDbiPlugin::tr("Invalid DBI state"));
        }
        if(dbi.getEntityTypeById(assemblyId) != U2Type::Assembly) {
            throw Exception(BAMDbiPlugin::tr("The specified object is not an assembly"));
        }
        quint64 result = 0;
        {
            U2OpStatusImpl opStatus;
            SQLiteReadQuery q("SELECT maxEndPos FROM assemblies WHERE id = ?1;", &dbRef, opStatus);
            q.bindDataId(1, assemblyId);
            result = q.selectInt64();
            if(opStatus.hasError()) {
                throw Exception(opStatus.getError());
            }
        }
        return result;
    } catch(const Exception &e) {
        os.setError(e.getMessage());
        return 0;
    }
}

U2AssemblyRead AssemblyDbi::alignmentToRead(const Alignment &alignment) {
    U2AssemblyRead row(new U2AssemblyReadData());
    row->name = alignment.getName();
    row->leftmostPos = alignment.getPosition();
    row->effectiveLen = Alignment::computeLength(alignment.getCigar());
    row->readSequence = alignment.getSequence();
    row->quality = alignment.getQuality();
    row->mappingQuality = alignment.getMapQuality();
    row->flags = alignment.getFlags();
    row->rnext = alignment.getNextReferenceName();
    row->pnext = alignment.getNextPosition();
    row->aux = alignment.getAuxData();
    foreach(const Alignment::CigarOperation &cigarOperation, alignment.getCigar()) {
        U2CigarOp cigarOp = U2CigarOp_Invalid;
        switch(cigarOperation.getOperation()) {
        case Alignment::CigarOperation::AlignmentMatch:
            cigarOp = U2CigarOp_M;
            break;
        case Alignment::CigarOperation::Insertion:
            cigarOp = U2CigarOp_I;
            break;
        case Alignment::CigarOperation::Deletion:
            cigarOp = U2CigarOp_D;
            break;
        case Alignment::CigarOperation::Skipped:
            cigarOp = U2CigarOp_N;
            break;
        case Alignment::CigarOperation::SoftClip:
            cigarOp = U2CigarOp_S;
            break;
        case Alignment::CigarOperation::HardClip:
            cigarOp = U2CigarOp_H;
            break;
        case Alignment::CigarOperation::Padding:
            cigarOp = U2CigarOp_P;
            break;
        case Alignment::CigarOperation::SequenceMatch:
            cigarOp = U2CigarOp_EQ;
            break;
        case Alignment::CigarOperation::SequenceMismatch:
            cigarOp = U2CigarOp_X;
            break;
        default:
            assert(false);
        }
        row->cigar.append(U2CigarToken(cigarOp, cigarOperation.getLength()));
    }
    return row;
}

qint64 AssemblyDbi::getMaxReadLength(const U2DataId& assemblyId, const U2Region &/*r*/) {
    qint64 dbDataId = U2DbiUtils::toDbiId(assemblyId);
    return maxReadLengths[dbDataId - 1];
}

U2AssemblyRead AssemblyDbi::getReadById(const U2DataId& rowId, qint64 packedRow, U2OpStatus &os) {
    try {
        quint64 dbDataId = (quint64)U2DbiUtils::toDbiId(rowId);
        reader.seek(VirtualOffset(dbDataId));
        U2AssemblyRead row = alignmentToRead(reader.readAlignment());
        row->id = rowId;
        row->packedViewRow = packedRow;
        return row;
    } catch(const Exception &e) {
        os.setError(e.getMessage());
        return U2AssemblyRead();
    }
}

QList<U2AssemblyRead> AssemblyDbi::getReadsByIds(QList<U2DataId> rowIds, QList<qint64> packedRows, U2OpStatus &os) {
    try {
        QList<U2AssemblyRead> result;
        for(int index = 0;index < rowIds.size();index++) {
            assert(dbi.getEntityTypeById(rowIds[index]) == U2Type::AssemblyRead);
            U2AssemblyRead read(new U2AssemblyReadData());
            {
                U2OpStatusImpl opStatus;
                read = getReadById(rowIds[index], packedRows[index], opStatus);
                if(opStatus.hasError()) {
                    throw Exception(opStatus.getError());
                }
            }
            result.append(read);
        }
        return result;
    } catch(const Exception &e) {
        os.setError(e.getMessage());
        return QList<U2AssemblyRead>();
    }
}

} // namespace BAM
} // namespace U2
