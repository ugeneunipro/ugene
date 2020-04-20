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

#include "SQLiteFeatureDbi.h"
#include "SQLiteMsaDbi.h"
#include "SQLiteObjectDbi.h"
#include "SQLiteUdrDbi.h"

#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2DbiPackUtils.h>
#include <U2Core/U2FeatureUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SqlHelpers.h>

#include <U2Formats/SQLiteModDbi.h>
#include <U2Formats/SQLiteSequenceDbi.h>

namespace U2 {

SQLiteObjectDbi::SQLiteObjectDbi(SQLiteDbi* dbi) : U2ObjectDbi(dbi), SQLiteChildDBICommon(dbi) {

}

void SQLiteObjectDbi::initSqlSchema(U2OpStatus& os) {
    // objects table - stores IDs and types for all objects. It also stores 'top_level' flag to simplify queries
    // rank: see U2DbiObjectRank
    // name is a visual name of the object shown to user.
    SQLiteWriteQuery("CREATE TABLE Object (id INTEGER PRIMARY KEY AUTOINCREMENT, type INTEGER NOT NULL, "
                                    "version INTEGER NOT NULL DEFAULT 1, rank INTEGER NOT NULL, "
                                    "name TEXT NOT NULL, trackMod INTEGER NOT NULL DEFAULT 0)", db, os).execute();
    CHECK_OP(os, );

    // parent-child object relation
    SQLiteWriteQuery("CREATE TABLE Parent (parent INTEGER, child INTEGER, "
                       "PRIMARY KEY (parent, child), "
                       "FOREIGN KEY(parent) REFERENCES Object(id) ON DELETE CASCADE, "
                       "FOREIGN KEY(child) REFERENCES Object(id) ON DELETE CASCADE)", db, os).execute();
    SQLiteWriteQuery("CREATE INDEX Parent_parent_child on Parent(parent, child)" , db, os).execute();
    SQLiteWriteQuery("CREATE INDEX Parent_child on Parent(child)" , db, os).execute();
    CHECK_OP(os, );

    // folders
    SQLiteWriteQuery("CREATE TABLE Folder (id INTEGER PRIMARY KEY AUTOINCREMENT, path TEXT UNIQUE NOT NULL, "
                                    "vlocal INTEGER NOT NULL DEFAULT 1, vglobal INTEGER NOT NULL DEFAULT 1 )", db, os).execute();
    CHECK_OP(os, );

    // folder-object relation
    SQLiteWriteQuery("CREATE TABLE FolderContent (folder INTEGER, object INTEGER, "
                        "PRIMARY KEY (folder, object), "
                        "FOREIGN KEY(folder) REFERENCES Folder(id) ON DELETE CASCADE,"
                        "FOREIGN KEY(object) REFERENCES Object(id) ON DELETE CASCADE)", db, os).execute();
    CHECK_OP(os, );
    SQLiteWriteQuery("CREATE INDEX FolderContent_object on FolderContent(object)", db, os).execute();
    CHECK_OP(os, );

    createFolder(U2ObjectDbi::ROOT_FOLDER, os);
}

//////////////////////////////////////////////////////////////////////////
// Read methods for objects
#define TOP_LEVEL_FILTER  ("rank = " + QString::number(U2DbiObjectRank_TopLevel))

qint64 SQLiteObjectDbi::countObjects(U2OpStatus& os) {
    return SQLiteReadQuery("SELECT COUNT (*) FROM Object WHERE " + TOP_LEVEL_FILTER, db, os).selectInt64();
}

qint64 SQLiteObjectDbi::countObjects(U2DataType type, U2OpStatus& os) {
    SQLiteReadQuery q("SELECT COUNT (*) FROM Object WHERE " + TOP_LEVEL_FILTER + " AND type = ?1", db, os);
    q.bindType(1, type);
    return q.selectInt64();
}

QList<U2DataId> SQLiteObjectDbi::getObjects(qint64 offset, qint64 count, U2OpStatus& os) {
    return SQLiteReadQuery("SELECT id, type FROM Object WHERE " + TOP_LEVEL_FILTER, offset, count, db, os).selectDataIdsExt();
}

QList<U2DataId> SQLiteObjectDbi::getObjects(U2DataType type, qint64 offset, qint64 count, U2OpStatus& os) {
    SQLiteReadQuery q("SELECT id, type FROM Object WHERE " + TOP_LEVEL_FILTER + " AND type = ?1", offset, count, db, os );
    q.bindType(1, type);
    return q.selectDataIdsExt();
}

QList<U2DataId> SQLiteObjectDbi::getParents(const U2DataId& entityId, U2OpStatus& os) {
    SQLiteReadQuery q("SELECT o.id AS id, o.type AS type FROM Parent AS p, Object AS o WHERE p.parent = o.id AND p.child = ?1", db, os);
    q.bindDataId(1, entityId);
    return q.selectDataIdsExt();
}

U2DbiIterator<U2DataId>* SQLiteObjectDbi::getObjectsByVisualName(const QString& visualName, U2DataType type, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    bool checkType = (type != U2Type::Unknown);
    QString query = "SELECT id, type FROM Object WHERE " + TOP_LEVEL_FILTER
            + " AND name = ?1 " + (checkType ? "AND type = ?2" : "" + QString(" ORDER BY id"));
    QSharedPointer<SQLiteQuery> q = t.getPreparedQuery(query, db, os);
    q->bindString(1, visualName);
    if (checkType) {
        q->bindType(2, type);
    }
    return new SQLiteResultSetIterator<U2DataId>(q, new SQLiteDataIdResultSetLoaderEx(), NULL, U2DataId(), os);
}

//////////////////////////////////////////////////////////////////////////
// Write methods for objects

bool SQLiteObjectDbi::removeObject(const U2DataId& dataId, bool /*force*/, U2OpStatus& os) {
    bool result = removeObjectImpl(dataId, os);
    CHECK_OP(os, result);
    if (result) {
        onFolderUpdated("");
    }
    return result;
}

bool SQLiteObjectDbi::removeObject(const U2DataId &dataId, U2OpStatus &os) {
    return removeObject(dataId, false, os);
}

namespace {

QString createDeleteObjectQueryStr(int objectCount) {
    static const QString queryStartStr("DELETE FROM Object WHERE id IN (");
    static const int bindingStrLength = 5; // five characters for ",?nnn"

    QString result(queryStartStr);
    result.reserve(result.length() + bindingStrLength * objectCount);
    for (int placeholderCount = 0; placeholderCount < objectCount; ) {
        const QString bindingStr = QString("?%1,").arg(++placeholderCount);
        result.append(bindingStr);
    }
    result.replace(result.length() - 1, 1, ')');
    return result;
}

}

bool SQLiteObjectDbi::removeObjects(const QList<U2DataId>& dataIds, bool /*force*/, U2OpStatus& os) {
    CHECK(!dataIds.isEmpty(), true);

    SQLiteTransaction t(db, os);
    Q_UNUSED(t);

    // remove specific objects' data first
    foreach(const U2DataId &objectId, dataIds) {
        removeObjectSpecificData(objectId, os);
        CHECK_OP(os, false);
    }

    // then remove general ones
    const int idsCount = dataIds.count();
    const int residualBindQueryCount = idsCount % SQLiteDbi::BIND_PARAMETERS_LIMIT;
    const int fullBindQueryCount = idsCount / SQLiteDbi::BIND_PARAMETERS_LIMIT;

    QString fullQueryStr;
    QString residualQueryStr;
    residualQueryStr = createDeleteObjectQueryStr(residualBindQueryCount);
    if (fullBindQueryCount > 0) {
        fullQueryStr = createDeleteObjectQueryStr(SQLiteDbi::BIND_PARAMETERS_LIMIT);
    }

    // execute deletion of residual objects
    SQLiteWriteQuery residualDeletionQuery(residualQueryStr, db, os);
    for (int i = 0; i < residualBindQueryCount; ++i) {
        residualDeletionQuery.bindDataId(i + 1, dataIds.at(i));
    }
    residualDeletionQuery.update(residualBindQueryCount);
    CHECK_OP(os, false);

    // execute deletion of objects by parts of `SQLiteDbi::BIND_PARAMETERS_LIMIT` in size
    if (fullBindQueryCount > 0) {
        SQLiteWriteQuery fullDeletionQuery(fullQueryStr, db, os);
        for (int currentFullQuery = 0; currentFullQuery < fullBindQueryCount; ++currentFullQuery) {
            const int firstBindingPos = residualBindQueryCount + currentFullQuery * SQLiteDbi::BIND_PARAMETERS_LIMIT;
            const int lastBindingPos = residualBindQueryCount + (currentFullQuery + 1) * SQLiteDbi::BIND_PARAMETERS_LIMIT;
            for (int idNum = firstBindingPos, paramNum = 1; idNum < lastBindingPos; ++idNum, ++paramNum) {
                fullDeletionQuery.bindDataId(paramNum, dataIds.at(idNum));
            }
            fullDeletionQuery.update(SQLiteDbi::BIND_PARAMETERS_LIMIT);
            CHECK_OP(os, false);
            fullDeletionQuery.reset();
        }
    }

    onFolderUpdated("");
    return !os.hasError();
}

bool SQLiteObjectDbi::removeObjects(const QList<U2DataId> &dataIds, U2OpStatus &os) {
    return removeObjects(dataIds, false, os);
}

void SQLiteObjectDbi::renameObject(const U2DataId &id, const QString &newName, U2OpStatus &os) {
    SQLiteTransaction t(db, os);
    static const QString queryString("UPDATE Object SET name = ?1 WHERE id = ?2");
    QSharedPointer<SQLiteQuery> q = t.getPreparedQuery(queryString, db, os);
    SAFE_POINT_OP(os, );
    q->bindString(1, newName);
    q->bindDataId(2, id);
    q->execute();
    CHECK_OP(os, );

    incrementVersion(id, os);
}

void SQLiteObjectDbi::updateObjectCore(U2Object &obj, U2OpStatus &os) {
    SQLiteTransaction t(db, os);
    static const QString queryString("UPDATE Object SET name = ?1, version = version WHERE id = ?2");
    QSharedPointer<SQLiteQuery> q = t.getPreparedQuery(queryString, db, os);
    SAFE_POINT_OP(os, );
    q->bindString(1, obj.visualName);
    q->bindDataId(2, obj.id);
    q->execute();
}

void SQLiteObjectDbi::removeObjectFromFolder(const U2DataId &id, const QString &folder, U2OpStatus &os) {
    const qint64 folderId = getFolderId(folder, true, db, os);
    CHECK_OP(os, );

    static const QString deleteString = "DELETE FROM FolderContent WHERE folder = ?1 AND object = ?2";
    SQLiteWriteQuery deleteQ(deleteString, db, os);
    CHECK_OP(os, );
    deleteQ.bindInt64(1, folderId);
    deleteQ.bindDataId(2, id);
    deleteQ.execute();
}

void SQLiteObjectDbi::removeObjectFromAllFolders(const U2DataId &id, U2OpStatus &os) {
    static const QString deleteString("DELETE FROM FolderContent WHERE object = ?1");
    SQLiteWriteQuery deleteQ(deleteString, db, os);
    CHECK_OP(os, );
    deleteQ.bindDataId(1, id);
    deleteQ.update();
}

bool SQLiteObjectDbi::removeObjectImpl(const U2DataId& objectId, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    Q_UNUSED(t);

    removeObjectSpecificData(objectId, os);
    CHECK_OP(os, false);

    SQLiteUtils::remove("Object", "id", objectId, 1, db, os);
    return !os.hasError();
}

void SQLiteObjectDbi::removeObjectSpecificData(const U2DataId &objectId, U2OpStatus &os) {
    U2DataType type = getRootDbi()->getEntityTypeById(objectId);
    if (!U2Type::isObjectType(type)) {
        os.setError(U2DbiL10n::tr("Not an object! Id: %1, type: %2").arg(U2DbiUtils::text(objectId)).arg(type));
        return;
    }

    switch (type) {
        case U2Type::Sequence:
        case U2Type::VariantTrack:
            // nothing has to be done for object of these types
            break;
        case U2Type::Mca:
        case U2Type::Msa:
            dbi->getSQLiteMsaDbi()->deleteRowsData(objectId, os);
            break;
        case U2Type::AnnotationTable:
            dbi->getSQLiteFeatureDbi()->removeAnnotationTableData(objectId, os);
            break;
        case U2Type::Assembly:
            dbi->getAssemblyDbi()->removeAssemblyData(objectId, os);
            break;
        case U2Type::CrossDatabaseReference:
            dbi->getCrossDatabaseReferenceDbi()->removeCrossReferenceData(objectId, os);
            break;
        default:
            if (!U2Type::isUdrObjectType(type)) {
                os.setError(U2DbiL10n::tr("Unknown object type! Id: %1, type: %2").arg(U2DbiUtils::text(objectId)).arg(type));
            }
    }
    CHECK_OP(os, );
}

void SQLiteObjectDbi::removeObjectAttributes(const U2DataId& id, U2OpStatus& os) {
    U2AttributeDbi* attributeDbi = dbi->getAttributeDbi();
    attributeDbi->removeObjectAttributes(id, os);
}

void SQLiteObjectDbi::removeObjectModHistory(const U2DataId& id, U2OpStatus& os) {
    U2ModDbi* modDbi = dbi->getModDbi();
    SAFE_POINT(NULL != modDbi, "NULL Mod Dbi!", );

    modDbi->removeObjectMods(id, os);
}

//////////////////////////////////////////////////////////////////////////
// Read methods for folders

QStringList SQLiteObjectDbi::getFolders(U2OpStatus& os) {
    // Comparison is case sensitive by default
    return SQLiteReadQuery("SELECT path FROM Folder ORDER BY path", db, os).selectStrings();
}

QHash<U2Object, QString> SQLiteObjectDbi::getObjectFolders(U2OpStatus &os) {
    QHash<U2Object, QString> result;

    static const QString queryString =
        "SELECT o.id, o.type, o.version, o.name, o.trackMod, f.path "
        "FROM Object AS o, FolderContent AS fc, Folder AS f WHERE fc.object=o.id AND "
        "fc.folder=f.id AND " + TOP_LEVEL_FILTER;
    SQLiteReadQuery q(queryString, db, os);
    CHECK_OP(os, result);

    const QString dbId = dbi->getDbiId();

    while (q.step()) {
        U2Object object;
        const U2DataType type = q.getDataType(1);
        object.id = q.getDataId(0, type);
        object.version = q.getInt64(2);
        object.visualName = q.getString(3);
        object.trackModType = static_cast<U2TrackModType>(q.getInt32(4));
        const QString path = q.getString(5);
        object.dbiId = dbId;
        result[object] = path;
    }
    return result;
}

void SQLiteObjectDbi::renameFolder(const QString &oldPath, const QString &newPath, U2OpStatus &os) {
    const QString oldCPath = U2DbiUtils::makeFolderCanonical(oldPath);
    const QString newCPath = U2DbiUtils::makeFolderCanonical(newPath);

    const QStringList allFolders = getFolders(os);
    CHECK_OP(os, );

    static const QString renameFolderQueryStr = "UPDATE Folder SET path = ?1 where path = ?2";
    if (allFolders.contains(oldCPath)) {
        SQLiteWriteQuery q(renameFolderQueryStr, db, os);
        q.bindString(1, newCPath);
        q.bindString(2, oldPath);
        q.update();
        CHECK_OP(os, );
    }

    QString parent = oldCPath + PATH_SEP;
    QString newParent = newCPath + PATH_SEP;
    foreach (const QString &path, allFolders) {
        if (path.startsWith(parent)) {
            QString newPath = newParent + path.mid(parent.size());
            SQLiteWriteQuery q(renameFolderQueryStr, db, os);
            q.bindString(1, newPath);
            q.bindString(2, path);
            q.update();
            CHECK_OP(os, );
        }
    }
}

QString SQLiteObjectDbi::getFolderPreviousPath(const QString & /*currentPath*/, U2OpStatus &os) {
    os.setError("Operation is not supported");
    return "";
}

qint64 SQLiteObjectDbi::countObjects(const QString& folder, U2OpStatus& os) {
    SQLiteReadQuery q("SELECT COUNT(fc.*) FROM FolderContent AS fc, Folder AS f WHERE f.path = ?1 AND fc.folder = f.id", db, os);
    q.bindString(1, folder);
    return q.selectInt64();
}

QList<U2DataId> SQLiteObjectDbi::getObjects(const QString& folder, qint64 , qint64 , U2OpStatus& os) {
    SQLiteReadQuery q("SELECT o.id, o.type FROM Object AS o, FolderContent AS fc, Folder AS f WHERE f.path = ?1 AND fc.folder = f.id AND fc.object=o.id AND o." + TOP_LEVEL_FILTER, db, os);
    q.bindString(1, folder);
    return q.selectDataIdsExt();
}

QStringList SQLiteObjectDbi::getObjectFolders(const U2DataId& objectId, U2OpStatus& os) {
    SQLiteReadQuery q("SELECT f.path FROM FolderContent AS fc, Folder AS f WHERE fc.object = ?1 AND fc.folder = f.id", db, os);
    q.bindDataId(1, objectId);
    return q.selectStrings();
}

//////////////////////////////////////////////////////////////////////////
// Write methods for  folders

void SQLiteObjectDbi::createFolder(const QString& path, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    Q_UNUSED(t);
    CHECK_OP(os, );

    const QString canonicalPath = U2DbiUtils::makeFolderCanonical(path);

    qint64 folderId = getFolderId(canonicalPath, false, db, os);
    CHECK_OP(os, );
    CHECK(-1 == folderId, );

    QString parentFolder = canonicalPath;
    if (U2ObjectDbi::ROOT_FOLDER != parentFolder) {
        parentFolder.truncate(parentFolder.lastIndexOf(U2ObjectDbi::PATH_SEP));
        if (parentFolder.isEmpty()) {
            parentFolder = U2ObjectDbi::ROOT_FOLDER;
        }
        createFolder(parentFolder, os);
    }

    SQLiteWriteQuery q("INSERT INTO Folder(path) VALUES(?1)", db, os);
    q.bindString(1, canonicalPath);
    q.execute();

    if (!os.hasError()) {
        onFolderUpdated(path);
    }
}

bool SQLiteObjectDbi::removeFolder(const QString& folder, U2OpStatus& os) {
    // remove subfolders first
    SQLiteReadQuery q("SELECT path FROM Folder WHERE path LIKE ?1 ORDER BY LENGTH(path) DESC", db, os);
    q.bindString(1, folder + "/%");
    QStringList subfolders = q.selectStrings();
    CHECK_OP(os, false);

    bool result = true;
    foreach (const QString &subfolder, subfolders) {
        result = removeFolder(subfolder, os);
        CHECK_OP(os, false);
    }

    // remove all objects from folder
    qint64 nObjects = countObjects(folder, os);
    CHECK_OP(os, false);

    int nObjectsPerIteration = 1000;
    for (int i = 0; i < nObjects; i += nObjectsPerIteration) {
        QList<U2DataId> objects = getObjects(folder, i, nObjectsPerIteration, os);
        CHECK_OP(os, false);

        // Remove all objects in the folder
        if (!objects.isEmpty()) {
            bool deleted = removeObjects(objects, false, os);
            CHECK_OP(os, false);
            if (result && !deleted) {
                result = false;
            }
        }
    }

    if (result) {
        // remove folder record
        SQLiteWriteQuery dq("DELETE FROM Folder WHERE path = ?1", db, os);
        dq.bindString(1, folder);
        dq.execute();
        CHECK_OP(os, false);

        onFolderUpdated(folder);
    }
    return result;
}

void SQLiteObjectDbi::addObjectsToFolder(const QList<U2DataId>& objectIds, const QString& folder, U2OpStatus& os) {
    qint64 folderId = getFolderId(folder, true, db, os);
    if (os.hasError()) {
        return;
    }
    QList<U2DataId> addedObjects;
    SQLiteReadQuery countQ("SELECT count(object) FROM FolderContent WHERE folder = ?1", db, os);
    SQLiteWriteQuery insertQ("INSERT INTO FolderContent(folder, object) VALUES(?1, ?2)", db, os);
    SQLiteWriteQuery toplevelQ("UPDATE Object SET rank = " + QString::number(U2DbiObjectRank_TopLevel) + " WHERE id = ?1", db, os);

    foreach(const U2DataId& objectId, objectIds) {
        countQ.reset();
        countQ.bindInt64(1, folderId);
        int c = countQ.selectInt64();
        if (c != 0) {
            continue; // object is already in folder, skip it
        }
        insertQ.reset();
        insertQ.bindInt64(1, folderId);
        insertQ.bindDataId(2, objectId);
        insertQ.execute();

        toplevelQ.reset();
        toplevelQ.bindDataId(1, objectId);
        toplevelQ.execute();

        if (os.hasError()) {
            break;
        }

        addedObjects.append(objectId);
    }
    onFolderUpdated(folder);
}

void SQLiteObjectDbi::moveObjects(const QList<U2DataId>& objectIds, const QString& fromFolder,
    const QString& toFolder, U2OpStatus& os, bool saveFromFolder)
{
    const QString canonicalFromFolder = U2DbiUtils::makeFolderCanonical(fromFolder);
    const QString canonicalToFolder = U2DbiUtils::makeFolderCanonical(toFolder);

    CHECK(canonicalFromFolder != canonicalToFolder, );

    addObjectsToFolder(objectIds, toFolder, os);
    CHECK_OP(os, );

    removeObjects(objectIds, false, os);

    if (saveFromFolder) {
        CHECK_OP(os, );

        U2AttributeDbi *attrDbi = dbi->getAttributeDbi();
        foreach (const U2DataId &id, objectIds) {
            const QList<U2DataId> attributes = attrDbi->getObjectAttributes(id,
                PREV_OBJ_PATH_ATTR_NAME, os);
            CHECK_OP(os, );

            CHECK_EXT(attributes.size() <= 1,
                os.setError("Multiple attribute definition detected!"), );

            if (!attributes.isEmpty()) {
                attrDbi->removeAttributes(attributes, os);
                CHECK_OP(os, );
            }

            U2StringAttribute prevPath(id, PREV_OBJ_PATH_ATTR_NAME, fromFolder);
            attrDbi->createStringAttribute(prevPath, os);
        }
    }
}

QStringList SQLiteObjectDbi::restoreObjects(const QList<U2DataId> &objectIds, U2OpStatus &os) {
    U2AttributeDbi *attrDbi = dbi->getAttributeDbi();
    QList<U2DataId> attributesWithStoredPath;
    QStringList result;
    foreach (const U2DataId &objId, objectIds) {
        const QList<U2DataId> attributes = attrDbi->getObjectAttributes(objId,
            PREV_OBJ_PATH_ATTR_NAME, os);
        CHECK_OP(os, result);

        CHECK_EXT(attributes.size() == 1,
            os.setError("Stored folder path not found!"), result);

        const U2DataId attrId = attributes.first();

        const U2StringAttribute storedPath = attrDbi->getStringAttribute(attrId, os);
        CHECK_OP(os, result);
        attributesWithStoredPath.append(attrId);
        result.append(storedPath.value);

        const QStringList folders = getObjectFolders(objId, os);
        CHECK_EXT(1 == folders.size(),
            os.setError("Multiple reference to object from folders found!"), result);
        moveObjects(QList<U2DataId>() << objId, folders.first(), storedPath.value, os);
        CHECK_OP(os, result);
    }

    attrDbi->removeAttributes(attributesWithStoredPath, os);
    return result;
}

void SQLiteObjectDbi::incrementVersion(const U2DataId& id, U2OpStatus& os) {
    SQLiteWriteQuery q("UPDATE Object SET version = version + 1 WHERE id = ?1", db, os);
    CHECK_OP(os, );

    q.bindDataId(1, id);
    q.update(1);
}

void SQLiteObjectDbi::setVersion(const U2DataId& id, qint64 version, U2OpStatus& os) {
    SQLiteWriteQuery q("UPDATE Object SET version = ?1 WHERE id = ?2", db, os);
    SAFE_POINT_OP(os, );

    q.bindInt64(1, version);
    q.bindDataId(2, id);
    q.update(1);
}

bool SQLiteObjectDbi::canUndo(const U2DataId& objId, U2OpStatus& os) {
    return dbi->getSQLiteModDbi()->canUndo(objId, os);
}

bool SQLiteObjectDbi::canRedo(const U2DataId& objId, U2OpStatus& os) {
    return dbi->getSQLiteModDbi()->canRedo(objId, os);
}

void SQLiteObjectDbi::undo(const U2DataId& objId, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    Q_UNUSED(t);

    QString errorDescr = U2DbiL10n::tr("Can't undo an operation for the object!");

    // Get the object
    U2Object obj;
    getObject(obj, objId, os);
    if (os.hasError()) {
        coreLog.trace("Error getting an object: " + os.getError());
        os.setError(errorDescr);
        return;
    }

    // Verify that modifications tracking is enabled for the object
    if (TrackOnUpdate != obj.trackModType) {
        coreLog.trace("Called 'undo' for an object without modifications tracking enabled!");
        os.setError(errorDescr);
        return;
    }

    // Get all single modifications steps
    qint64 userModStepVersion = dbi->getSQLiteModDbi()->getNearestUserModStepVersion(objId, obj.version - 1, os);
    if (os.hasError()) {
        coreLog.trace("Error getting the nearest userModStep version: " + os.getError());
        os.setError(errorDescr);
        return;
    }

    QList< QList<U2SingleModStep> > modSteps = dbi->getSQLiteModDbi()->getModSteps(objId, userModStepVersion, os);
    if (os.hasError()) {
        coreLog.trace("Error getting modSteps for an object: " + os.getError());
        os.setError(errorDescr);
        return;
    }


    QList< QList<U2SingleModStep> >::const_iterator multiIt = modSteps.end();
    while (multiIt != modSteps.begin()) {
        --multiIt;
        QList<U2SingleModStep> multiStepSingleSteps = *multiIt;

        foreach (U2SingleModStep modStep, multiStepSingleSteps) {
            // Call an appropriate "undo" depending on the object type
            if (U2ModType::isUdrModType(modStep.modType)) {
                dbi->getSQLiteUdrDbi()->undo(modStep, os);
            } else if (U2ModType::isMsaModType(modStep.modType)) {
                dbi->getSQLiteMsaDbi()->undo(modStep.objectId, modStep.modType, modStep.details, os);
            } else if (U2ModType::isSequenceModType(modStep.modType)) {
                dbi->getSQLiteSequenceDbi()->undo(modStep.objectId, modStep.modType, modStep.details, os);
            } else if (U2ModType::isObjectModType(modStep.modType)) {
                if (U2ModType::objUpdatedName == modStep.modType) {
                    undoUpdateObjectName(modStep.objectId, modStep.details, os);
                    CHECK_OP(os, );
                }
                else {
                    coreLog.trace(QString("Can't undo an unknown operation: '%1'!").arg(QString::number(modStep.modType)));
                    os.setError(errorDescr);
                    return;
                }
            }
            if (os.hasError()) {
                coreLog.trace(QString("Can't undo a single step: '%1'!").arg(os.getError()));
                os.setError(errorDescr);
                return;
            }

            setVersion(modStep.objectId, modStep.version, os);
            if (os.hasError()) {
                coreLog.trace("Failed to set an object version: " + os.getError());
                os.setError(errorDescr);
                return;
            }
        }
    }

    setVersion(objId, userModStepVersion, os);
    if (os.hasError()) {
        coreLog.trace("Failed to set an object version: " + os.getError());
        os.setError(errorDescr);
        return;
    }
}

void SQLiteObjectDbi::redo(const U2DataId& objId, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    Q_UNUSED(t);

    QString errorDescr = U2DbiL10n::tr("Can't redo an operation for the object!");

    // Get the object
    U2Object obj;
    getObject(obj, objId, os);
    if (os.hasError()) {
        coreLog.trace("Error getting an object: " + os.getError());
        os.setError(errorDescr);
        return;
    }

    // Verify that modifications tracking is enabled for the object
    if (TrackOnUpdate != obj.trackModType) {
        coreLog.trace("Called 'redo' for an object without modifications tracking enabled!");
        os.setError(errorDescr);
        return;
    }

    // Get all single modification steps
    QList< QList<U2SingleModStep> > modSteps = dbi->getSQLiteModDbi()->getModSteps(objId, obj.version, os);
    if (os.hasError()) {
        coreLog.trace("Error getting modSteps for an object: " + os.getError());
        os.setError(errorDescr);
        return;
    }

    foreach (QList<U2SingleModStep> multiStepSingleSteps, modSteps) {
        QSet<U2DataId> objectIds;

        foreach (U2SingleModStep modStep, multiStepSingleSteps) {
            if (U2ModType::isUdrModType(modStep.modType)) {
                dbi->getSQLiteUdrDbi()->redo(modStep, os);
            } else if (U2ModType::isMsaModType(modStep.modType)) {
                dbi->getSQLiteMsaDbi()->redo(modStep.objectId, modStep.modType, modStep.details, os);
            } else if (U2ModType::isSequenceModType(modStep.modType)) {
                dbi->getSQLiteSequenceDbi()->redo(modStep.objectId, modStep.modType, modStep.details, os);
            } else if (U2ModType::isObjectModType(modStep.modType)) {
                if (U2ModType::objUpdatedName == modStep.modType) {
                    redoUpdateObjectName(modStep.objectId, modStep.details, os);
                    CHECK_OP(os, );
                }
                else {
                    coreLog.trace(QString("Can't redo an unknown operation: '%1'!").arg(QString::number(modStep.modType)));
                    os.setError(errorDescr);
                    return;
                }
            }

            objectIds.insert(modStep.objectId);
        }
        objectIds.insert(objId);

        foreach (U2DataId objId, objectIds) {
            incrementVersion(objId, os);
            if (os.hasError()) {
                coreLog.trace("Can't increment an object version!");
                os.setError(errorDescr);
                return;
            }
        }
    }
}

void SQLiteObjectDbi::undoUpdateObjectName(const U2DataId& id, const QByteArray& modDetails, U2OpStatus& os) {
    // Parse the input
    QString oldName;
    QString newName;
    bool ok = U2DbiPackUtils::unpackObjectNameDetails(modDetails, oldName, newName);
    if (!ok) {
        os.setError("An error occurred during updating an object name!");
        return;
    }

    // Update the value
    SQLiteWriteQuery q("UPDATE Object SET name = ?1 WHERE id = ?2", db, os);
    CHECK_OP(os, );

    q.bindString(1, oldName);
    q.bindDataId(2, id);
    q.update(1);
}

void SQLiteObjectDbi::redoUpdateObjectName(const U2DataId& id, const QByteArray& modDetails, U2OpStatus& os) {
    QString oldName;
    QString newName;
    bool ok = U2DbiPackUtils::unpackObjectNameDetails(modDetails, oldName, newName);
    if (!ok) {
        os.setError("An error occurred during updating an object name!");
        return;
    }

    U2Object obj;
    getObject(obj, id, os);
    CHECK_OP(os, );

    obj.visualName = newName;
    updateObjectCore(obj, os);
    CHECK_OP(os, );
}

void SQLiteObjectDbi::removeParent(const U2DataId& parentId, const U2DataId& childId, bool removeDeadChild, U2OpStatus& os) {
    SQLiteWriteQuery q("DELETE FROM Parent WHERE parent = ?1 AND child = ?2", db, os);
    q.bindDataId(1, parentId);
    q.bindDataId(2, childId);
    /*qint64 res = */q.update(1);
    if (os.hasError()) {
        return;
    }
    if (!removeDeadChild) {
        return;
    }
    QList<U2DataId> parents = getParents(childId, os);
    if (!parents.isEmpty() || os.hasError()) {
        return;
    }
    QList<QString> folders = getObjectFolders(childId, os);
    if (!folders.isEmpty() || os.hasError()) {
        return;
    }
    removeObjects(QList<U2DataId>() << childId, "", os);
}


void SQLiteObjectDbi::setParent(const U2DataId& parentId, const U2DataId& childId, U2OpStatus& os) {
    SQLiteWriteQuery insertQ("INSERT OR IGNORE INTO Parent (parent, child) VALUES (?1, ?2)", db, os);
    insertQ.bindDataId(1, parentId);
    insertQ.bindDataId(2, childId);
    insertQ.execute();
}

//////////////////////////////////////////////////////////////////////////
// Helper methods

void SQLiteObjectDbi::incrementVersion(const U2DataId& objectId, DbRef* db, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    static const QString queryString("UPDATE Object SET version = version + 1 WHERE id = ?1");
    QSharedPointer<SQLiteQuery> q = t.getPreparedQuery(queryString, db, os);
    CHECK_OP(os, );
    q->bindDataId(1, objectId);
    q->update(1);
}

qint64 SQLiteObjectDbi::getObjectVersion(const U2DataId& objectId, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    static const QString queryString("SELECT version FROM Object WHERE id = ?1");
    QSharedPointer<SQLiteQuery> q = t.getPreparedQuery(queryString, db, os);
    CHECK_OP(os, -1);
    q->bindDataId(1, objectId);
    return q->selectInt64();
}

void SQLiteObjectDbi::setTrackModType(const U2DataId& objectId, U2TrackModType trackModType, U2OpStatus& os) {
    SQLiteWriteQuery q("UPDATE Object SET trackMod = ?1 WHERE id IN "
                    "(SELECT o.id FROM Object o, Parent p WHERE p.parent = ?2 AND p.child = o.id) OR id = ?2", db, os);
    CHECK_OP(os, );
    q.bindInt32(1, trackModType);
    q.bindDataId(2, objectId);
    q.update(1);
}

U2TrackModType SQLiteObjectDbi::getTrackModType(const U2DataId& objectId, U2OpStatus& os) {
    SQLiteReadQuery q("SELECT trackMod FROM Object WHERE id = ?1", db, os);
    CHECK_OP(os, NoTrack);

    q.bindDataId(1, objectId);
    if (q.step()) {
        int res = q.getInt32(0);
        SAFE_POINT(res >= 0 && res < TRACK_MOD_TYPE_NR_ITEMS, "Incorrect trackMod value!", NoTrack);
        q.ensureDone();
        return (U2TrackModType)res;
    }
    else if (!os.hasError()) {
        os.setError(U2DbiL10n::tr("Object not found!"));
        return NoTrack;
    }

    return NoTrack;
}

U2DataId SQLiteObjectDbi::createObject(U2Object & object, const QString& folder, U2DbiObjectRank rank, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    U2DataType type = object.getType();
    const QString &vname = object.visualName;
    int trackMod = object.trackModType;
    static const QString i1String("INSERT INTO Object(type, rank, name, trackMod) VALUES(?1, ?2, ?3, ?4)");
    QSharedPointer<SQLiteQuery> i1 = t.getPreparedQuery(i1String, db, os);
    CHECK_OP(os, U2DataId());
    i1->bindType(1, type);
    i1->bindInt32(2, rank);
    i1->bindString(3, vname);
    i1->bindInt32(4, trackMod);
    U2DataId res = i1->insert(type);
    CHECK_OP(os, res);

    if (U2DbiObjectRank_TopLevel == rank) {
        const QString canonicalFolder = U2DbiUtils::makeFolderCanonical(folder);
        qint64 folderId = getFolderId(canonicalFolder, true, db, os);
        CHECK_OP(os, res);

        static const QString i2String("INSERT INTO FolderContent(folder, object) VALUES(?1, ?2)");
        QSharedPointer<SQLiteQuery> i2 = t.getPreparedQuery(i2String, db, os);
        CHECK_OP(os, res);
        i2->bindInt64(1, folderId);
        i2->bindDataId(2, res);
        i2->execute();
        CHECK_OP(os, res);
    }

    object.id = res;
    object.dbiId = dbi->getDbiId();
    object.version = getObjectVersion(object.id, os);
    SAFE_POINT_OP(os, res);
    return res;
}

void SQLiteObjectDbi::getObject(U2Object& object, const U2DataId& id, U2OpStatus& os) {
    SQLiteReadQuery q("SELECT name, version, trackMod FROM Object WHERE id = ?1", db, os);
    q.bindDataId(1, id);
    if (q.step()) {
        object.id = id;
        object.dbiId = dbi->getDbiId();
        object.visualName = q.getString(0);
        object.version = q.getInt64(1);
        int trackMod = q.getInt32(2);
        if (trackMod >= 0 && trackMod < TRACK_MOD_TYPE_NR_ITEMS) {
            object.trackModType = (U2TrackModType)trackMod;
        }
        else {
            os.setError("Incorrect trackMod value in an object!");
            object.trackModType = NoTrack;
        }
        q.ensureDone();
    } else if (!os.hasError()) {
        os.setError(U2DbiL10n::tr("Object not found."));
    }
}

U2DataId SQLiteObjectDbi::getObject(qint64 objectId, U2OpStatus& os) {
    SQLiteReadQuery q("SELECT id, type FROM Object WHERE id = ?1", db, os);
    q.bindInt64(1, objectId);
    if (q.step()) {
        U2DataId result = q.getDataIdExt(0);
        q.ensureDone();
        return result;
    } else if (!os.hasError()) {
        os.setError(U2DbiL10n::tr("Object not found."));
    }
    return U2DataId();
}

QHash<U2DataId, QString> SQLiteObjectDbi::getObjectNames(qint64 offset, qint64 count, U2OpStatus &os) {
    QHash<U2DataId, QString> result;

    static const QString queryString = "SELECT id, type, name FROM Object WHERE " + TOP_LEVEL_FILTER;

    SQLiteReadQuery q(queryString, offset, count, db, os);
    CHECK_OP(os, result);
    while (q.step()) {
        const U2DataType type = q.getDataType(1);
        const U2DataId id = q.getDataId(0, type);
        const QString name = q.getString(2);
        result.insert(id, name);
    }
    return result;
}

void SQLiteObjectDbi::updateObject(U2Object& obj, U2OpStatus& os) {
    updateObjectCore(obj, os);
    SAFE_POINT_OP(os, );

    obj.version = getObjectVersion(obj.id, os);
}

qint64 SQLiteObjectDbi::getFolderId(const QString& path, bool mustExist, DbRef* db, U2OpStatus& os) {
    static const QString queryString("SELECT id FROM Folder WHERE path = ?1");
    SQLiteReadQuery q(queryString, db, os);
    q.bindString(1, path);
    qint64 res = q.selectInt64();
    if (os.hasError()) {
        return -1;
    }
    if (mustExist && res == -1) {
        os.setError(U2DbiL10n::tr("Folder not found: %1").arg(path));
    }
    return res;
}

qint64 SQLiteObjectDbi::getFolderLocalVersion(const QString& folder, U2OpStatus& os) {
    SQLiteReadQuery q("SELECT vlocal FROM Folder WHERE path = ?1", db, os);
    q.bindString(1, folder);
    return q.selectInt64();
}

qint64 SQLiteObjectDbi::getFolderGlobalVersion(const QString& folder, U2OpStatus& os) {
    SQLiteReadQuery q("SELECT vglobal FROM Folder WHERE path = ?1", db, os);
    q.bindString(1, folder);
    return q.selectInt64();
}

void SQLiteObjectDbi::setObjectRank(const U2DataId &objectId, U2DbiObjectRank newRank, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    static const QString queryStr("UPDATE Object SET rank = ?1 WHERE id = ?2");
    QSharedPointer<SQLiteQuery> query = t.getPreparedQuery(queryStr, db, os);
    CHECK_OP(os, );
    query->bindInt32(1, newRank);
    query->bindDataId(2, objectId);
    const int affectedRowsCount = query->update(-1);
    SAFE_POINT_EXT(-1 == affectedRowsCount || 0 == affectedRowsCount || affectedRowsCount == 1, os.setError(U2DbiL10n::tr("Unexpected row count. Query: '%1', rows: %2").arg(query->getQueryText()).arg(affectedRowsCount)), );
}

U2DbiObjectRank SQLiteObjectDbi::getObjectRank(const U2DataId &objectId, U2OpStatus& os) {
    SQLiteReadQuery q("SELECT rank FROM Object WHERE id = ?1", db, os);
    q.bindDataId(1, objectId);
    return static_cast<U2DbiObjectRank> (q.selectInt32());
}

void SQLiteObjectDbi::onFolderUpdated(const QString& ) {
    //TODO: update local version of the given folder & global for all parents
}


//////////////////////////////////////////////////////////////////////////
// cross references dbi

SQLiteCrossDatabaseReferenceDbi::SQLiteCrossDatabaseReferenceDbi(SQLiteDbi* dbi)
: U2CrossDatabaseReferenceDbi(dbi), SQLiteChildDBICommon(dbi)
{
}

void SQLiteCrossDatabaseReferenceDbi::initSqlSchema(U2OpStatus& os) {
    if (os.hasError()) {
        return;
    }
    // cross database reference object
    // factory - remote dbi factory
    // dbi - remote dbi id (url)
    // rid  - remote object id
    // version - remove object version
    SQLiteWriteQuery("CREATE TABLE CrossDatabaseReference (object INTEGER, factory TEXT NOT NULL, dbi TEXT NOT NULL, "
                            "rid BLOB NOT NULL, version INTEGER NOT NULL, "
                            " FOREIGN KEY(object) REFERENCES Object(id) )", db, os).execute();
}


void SQLiteCrossDatabaseReferenceDbi::createCrossReference(U2CrossDatabaseReference& reference, const QString& folder, U2OpStatus& os) {
    dbi->getSQLiteObjectDbi()->createObject(reference, folder, U2DbiObjectRank_TopLevel, os);
    if (os.hasError()) {
        return;
    }

    SQLiteWriteQuery q("INSERT INTO CrossDatabaseReference(object, factory, dbi, rid, version) VALUES(?1, ?2, ?3, ?4, ?5)", db, os);
    q.bindDataId(1, reference.id);
    q.bindString(2, reference.dataRef.dbiRef.dbiFactoryId);
    q.bindString(3, reference.dataRef.dbiRef.dbiId);
    q.bindBlob(4, reference.dataRef.entityId);
    q.bindInt64(5, reference.dataRef.version);
    q.execute();
}

void SQLiteCrossDatabaseReferenceDbi::removeCrossReferenceData(const U2DataId &referenceId, U2OpStatus &os) {
    static const QString queryString = "DELETE FROM CrossDatabaseReference WHERE object = ?1";
    SQLiteWriteQuery q(queryString, db, os);
    q.bindDataId(1, referenceId);
    q.execute();
}

U2CrossDatabaseReference SQLiteCrossDatabaseReferenceDbi::getCrossReference(const U2DataId& objectId, U2OpStatus& os) {
    U2CrossDatabaseReference res(objectId, dbi->getDbiId(), 0);
    SQLiteReadQuery q("SELECT r.factory, r.dbi, r.rid, r.version, o.name, o.version FROM CrossDatabaseReference AS r, Object AS o "
        " WHERE o.id = ?1 AND r.object = o.id", db, os);
    q.bindDataId(1, objectId);
    if (q.step())  {
        res.dataRef.dbiRef.dbiFactoryId= q.getString(0);
        res.dataRef.dbiRef.dbiId = q.getString(1);
        res.dataRef.entityId = q.getBlob(2);
        res.dataRef.version = q.getInt64(3);
        res.visualName = q.getString(4);
        res.version = q.getInt64(5);
        q.ensureDone();
    }
    return res;
}

void SQLiteCrossDatabaseReferenceDbi::updateCrossReference(const U2CrossDatabaseReference& reference, U2OpStatus& os) {
    SQLiteWriteQuery q("UPDATE CrossDatabaseReference SET factory = ?1, dbi = ?2, rid = ?3, version = ?4 WHERE object = ?5", db, os);
    q.bindString(1, reference.dataRef.dbiRef.dbiFactoryId);
    q.bindString(2, reference.dataRef.dbiRef.dbiId);
    q.bindBlob(3, reference.dataRef.entityId);
    q.bindInt64(4, reference.dataRef.version);
    q.bindDataId(5, reference.id);
    q.execute();
}


SQLiteModificationAction::SQLiteModificationAction(SQLiteDbi* _dbi, const U2DataId& _masterObjId)
    : ModificationAction(_dbi, _masterObjId)
{

}

U2TrackModType SQLiteModificationAction::prepare(U2OpStatus& os) {
    trackMod = dbi->getObjectDbi()->getTrackModType(masterObjId, os);
    if (os.hasError()) {
        trackMod = NoTrack;
        FAIL("Failed to get trackMod!", NoTrack);
    }

    if (TrackOnUpdate == trackMod) {
        qint64 masterObjVersionToTrack = dbi->getObjectDbi()->getObjectVersion(masterObjId, os);
        SAFE_POINT_OP(os, trackMod);

        // If a user mod step has already been created for this action
        // then it can not be deleted. The version must be incremented.
        // Obsolete duplicate step must be deleted
        if (getDbi()->getSQLiteModDbi()->isUserStepStarted(masterObjId)) {
            getDbi()->getSQLiteModDbi()->removeDuplicateUserStep(masterObjId, masterObjVersionToTrack, os);

            // Increment the object version
            masterObjVersionToTrack++;
        }

        // A user pressed "Undo" (maybe several times), did another action => there is no more "Redo" history
        getDbi()->getSQLiteModDbi()->removeModsWithGreaterVersion(masterObjId, masterObjVersionToTrack, os);
        if (os.hasError()) {
            getDbi()->getSQLiteModDbi()->cleanUpAllStepsOnError();
            return trackMod;
        }
    }

    return trackMod;
}

void SQLiteModificationAction::addModification(const U2DataId& objId, qint64 modType, const QByteArray& modDetails, U2OpStatus& os) {
    objIds.insert(objId);

    if (TrackOnUpdate == trackMod) {
        SAFE_POINT(!modDetails.isEmpty(), "Empty modification details!", );

        qint64 objVersion = dbi->getObjectDbi()->getObjectVersion(objId, os);
        SAFE_POINT_OP(os, );

        if ((objId == masterObjId) && (getDbi()->getSQLiteModDbi()->isUserStepStarted(masterObjId))) {
            objVersion++;
        }

        U2SingleModStep singleModStep;
        singleModStep.objectId = objId;
        singleModStep.version = objVersion;
        singleModStep.modType = modType;
        singleModStep.details = modDetails;

        singleSteps.append(singleModStep);
    }
}

void SQLiteModificationAction::complete(U2OpStatus& os) {
    // Save modification tracks, if required
    if (TrackOnUpdate == trackMod) {
        if (0 == singleSteps.size()) {
            // do nothing
        }
        else if (1 == singleSteps.size()) {
            getDbi()->getSQLiteModDbi()->createModStep(masterObjId, singleSteps.first(), os);
            SAFE_POINT_OP(os, );
        }
        else {
            U2UseCommonMultiModStep multi(getDbi(), masterObjId, os);
            SAFE_POINT_OP(os, );

            foreach (U2SingleModStep singleStep, singleSteps) {
                getDbi()->getSQLiteModDbi()->createModStep(masterObjId, singleStep, os);
                SAFE_POINT_OP(os, );
            }
        }
    }

    // Increment versions of all objects
    foreach (const U2DataId& objId, objIds) {
        SQLiteObjectDbi::incrementVersion(objId, getDbi()->getDbRef(), os);
        SAFE_POINT_OP(os, );
    }
}

SQLiteDbi* SQLiteModificationAction::getDbi() const {
    return static_cast<SQLiteDbi*>(dbi);
}

/************************************************************************/
/* SQLiteObjectDbiUtils */
/************************************************************************/
void SQLiteObjectDbiUtils::renameObject(SQLiteDbi *dbi, U2Object &object, const QString &newName, U2OpStatus &os) {
    SAFE_POINT(NULL != dbi, "NULL dbi!", );
    SQLiteTransaction t(dbi->getDbRef(), os);
    Q_UNUSED(t);

    SQLiteModificationAction updateAction(dbi, object.id);
    updateAction.prepare(os);
    SAFE_POINT_OP(os, );

    renameObject(updateAction, dbi, object, newName, os);
    SAFE_POINT_OP(os, );

    // Increment version; track the modification, if required
    updateAction.complete(os);
    SAFE_POINT_OP(os, );
}

void SQLiteObjectDbiUtils::renameObject(SQLiteModificationAction& updateAction, SQLiteDbi *dbi, U2Object &object, const QString &newName, U2OpStatus &os) {
    SAFE_POINT(NULL != dbi, "NULL dbi!", );
    SQLiteTransaction t(dbi->getDbRef(), os);
    Q_UNUSED(t);

    QByteArray modDetails;
    if (TrackOnUpdate == updateAction.getTrackModType()) {
        modDetails = U2DbiPackUtils::packObjectNameDetails(object.visualName, newName);
    }

    object.visualName = newName;
    dbi->getSQLiteObjectDbi()->updateObject(object, os);
    SAFE_POINT_OP(os, );

    updateAction.addModification(object.id, U2ModType::objUpdatedName, modDetails, os);
    SAFE_POINT_OP(os, );
}

} //namespace
