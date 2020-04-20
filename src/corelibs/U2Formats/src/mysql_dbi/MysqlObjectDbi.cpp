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

#include <QCryptographicHash>
#include <QSqlError>
#include <QSqlQuery>

#include <U2Core/Folder.h>
#include <U2Core/U2DbiPackUtils.h>
#include <U2Core/U2FeatureUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/Version.h>

#include "MysqlFeatureDbi.h"
#include "MysqlModDbi.h"
#include "MysqlMsaDbi.h"
#include "MysqlObjectDbi.h"
#include "MysqlSequenceDbi.h"
#include "MysqlUdrDbi.h"
#include "util/MysqlHelpers.h"

namespace U2 {

static const int OBJ_USAGE_CHECK_INTERVAL = 2 * U2ObjectDbi::OBJECT_ACCESS_UPDATE_INTERVAL / 1000;

MysqlObjectDbi::MysqlObjectDbi(MysqlDbi* dbi) :
    U2ObjectDbi(dbi),
    MysqlChildDbiCommon(dbi)
{
}

void MysqlObjectDbi::initSqlSchema(U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    // objects table - stores IDs and types for all objects. It also stores 'top_level' flag to simplify queries
    // rank: see U2DbiObjectRank
    // name is a visual name of the object shown to user.
    U2SqlQuery("CREATE TABLE Object (id BIGINT PRIMARY KEY AUTO_INCREMENT, type INTEGER NOT NULL, "
                                    "version BIGINT NOT NULL DEFAULT 1, rank INTEGER NOT NULL, "
                                    "name TEXT NOT NULL, trackMod INTEGER NOT NULL DEFAULT 0) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();
    CHECK_OP(os, );

    U2SqlQuery("CREATE TABLE ObjectAccessTrack (object BIGINT PRIMARY KEY, lastAccessTime TIMESTAMP, "
                "FOREIGN KEY(object) REFERENCES Object(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();
    CHECK_OP(os, );

    // parent-child object relation
    U2SqlQuery("CREATE TABLE Parent (parent BIGINT, child BIGINT, "
                       "PRIMARY KEY (parent, child), "
                       "FOREIGN KEY(parent) REFERENCES Object(id) ON DELETE CASCADE, "
                       "FOREIGN KEY(child) REFERENCES Object(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();
    CHECK_OP(os, );

    U2SqlQuery("CREATE INDEX Parent_parent_child on Parent(parent, child)", db, os).execute();
    U2SqlQuery("CREATE INDEX Parent_child on Parent(child)", db, os).execute();
    CHECK_OP(os, );

    // folders
    U2SqlQuery("CREATE TABLE Folder (id BIGINT PRIMARY KEY AUTO_INCREMENT, path LONGTEXT NOT NULL, hash VARCHAR(32) UNIQUE NOT NULL, previousPath LONGTEXT, "
               "vlocal BIGINT NOT NULL DEFAULT 1, vglobal BIGINT NOT NULL DEFAULT 1) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();
    CHECK_OP(os, );

    // folder-object relation
    U2SqlQuery("CREATE TABLE FolderContent (folder BIGINT, object BIGINT, "
              "PRIMARY KEY (folder, object), "
              "FOREIGN KEY(folder) REFERENCES Folder(id) ON DELETE CASCADE,"
              "FOREIGN KEY(object) REFERENCES Object(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();
    CHECK_OP(os, );
    U2SqlQuery("CREATE INDEX FolderContent_object on FolderContent(object)", db, os).execute();
    CHECK_OP(os, );

    createFolder(ROOT_FOLDER, os);
    CHECK_OP(os, );
    createFolder(PATH_SEP + RECYCLE_BIN_FOLDER, os);
}

//////////////////////////////////////////////////////////////////////////
// Read methods for objects
#define TOP_LEVEL_FILTER ("rank = " + QString::number(U2DbiObjectRank_TopLevel))

qint64 MysqlObjectDbi::countObjects(U2OpStatus& os) {
    static const QString queryString = "SELECT COUNT (*) FROM Object WHERE " + TOP_LEVEL_FILTER;
    return U2SqlQuery(queryString, db, os).selectInt64();
}

qint64 MysqlObjectDbi::countObjects(U2DataType type, U2OpStatus& os) {
    static const QString queryString = "SELECT COUNT (*) FROM Object WHERE " + TOP_LEVEL_FILTER + " AND type = :type";
    U2SqlQuery q(queryString, db, os);
    q.bindType(":type", type);
    return q.selectInt64();
}

QList<U2DataId> MysqlObjectDbi::getObjects(qint64 offset, qint64 count, U2OpStatus& os) {
    static const QString queryString = "SELECT id, type, '' FROM Object WHERE " + TOP_LEVEL_FILTER;
    return U2SqlQuery(queryString, offset, count, db, os).selectDataIdsExt();
}

QList<U2DataId> MysqlObjectDbi::getObjects(U2DataType type, qint64 offset, qint64 count, U2OpStatus& os) {
    static const QString queryString = "SELECT id, type, '' FROM Object WHERE " + TOP_LEVEL_FILTER + " AND type = :type";
    U2SqlQuery q(queryString, offset, count, db, os );
    q.bindType(":type", type);
    return q.selectDataIdsExt();
}

QList<U2DataId> MysqlObjectDbi::getParents(const U2DataId& entityId, U2OpStatus& os) {
    static const QString queryString = "SELECT o.id AS id, o.type AS type, '' FROM Parent AS p, Object AS o WHERE p.parent = o.id AND p.child = :child";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":child", entityId);
    return q.selectDataIdsExt();
}

U2DbiIterator<U2DataId>* MysqlObjectDbi::getObjectsByVisualName(const QString& visualName, U2DataType type, U2OpStatus& os) {
    bool checkType = (type != U2Type::Unknown);
    static const QString query = "SELECT id, type FROM Object WHERE " + TOP_LEVEL_FILTER
            + " AND name = :name " + (checkType ? "AND type = :type" : "" + QString(" ORDER BY id"));
    QSharedPointer<U2SqlQuery> q(new U2SqlQuery(query, db, os));
    q->bindString(":name", visualName);
    if (checkType) {
        q->bindType(":type", type);
    }

    return new MysqlRSIterator<U2DataId>(q, new MysqlDataIdRSLoaderEx(), NULL, U2DataId(), os);
}


//////////////////////////////////////////////////////////////////////////
// Read methods for folders

QStringList MysqlObjectDbi::getFolders(U2OpStatus& os) {
    static const QString queryString = "SELECT path FROM Folder ORDER BY BINARY(path)";
    return U2SqlQuery(queryString, db, os).selectStrings();
}

QHash<U2Object, QString> MysqlObjectDbi::getObjectFolders(U2OpStatus &os) {
    QHash<U2Object, QString> result;

    static const QString queryString = "SELECT o.id, o.type, o.version, o.name, o.trackMod, f.path "
        "FROM Object AS o, FolderContent AS fc, Folder AS f WHERE fc.object=o.id AND "
        "fc.folder=f.id AND " + TOP_LEVEL_FILTER;
    U2SqlQuery q(queryString, db, os);
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

void MysqlObjectDbi::renameFolder(const QString &oldPath, const QString &newPath, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    CHECK_OP(os, );
    Q_UNUSED(t);

    const QString oldCPath = U2DbiUtils::makeFolderCanonical(oldPath);
    const QString newCPath = U2DbiUtils::makeFolderCanonical(newPath);
    const QByteArray oldHash = QCryptographicHash::hash(oldCPath.toLatin1(), QCryptographicHash::Md5).toHex();
    const QByteArray newHash = QCryptographicHash::hash(newCPath.toLatin1(), QCryptographicHash::Md5).toHex();

    const QStringList allFolders = getFolders(os);
    CHECK_OP(os, );

    static const QString queryString = "UPDATE Folder SET path = :newPath, hash = :newHash, previousPath = :oldPath WHERE hash = :oldHash";
    if (allFolders.contains(oldCPath)) {
        U2SqlQuery q(queryString, db, os);
        q.bindString(":newPath", newCPath);
        q.bindBlob(":newHash", newHash);
        q.bindString(":oldPath", oldPath);
        q.bindBlob(":oldHash", oldHash);
        q.update();
        CHECK_OP(os, );
    }

    QString parent = oldCPath + PATH_SEP;
    QString newParent = newCPath + PATH_SEP;
    foreach (const QString &path, allFolders) {
        if (path.startsWith(parent)) {
            QString newSubfolderPath = newParent + path.mid(parent.size());
            const QByteArray oldSubfolderHash = QCryptographicHash::hash(path.toLatin1(), QCryptographicHash::Md5).toHex();
            const QByteArray newSubfolderHash = QCryptographicHash::hash(newSubfolderPath.toLatin1(), QCryptographicHash::Md5).toHex();
            U2SqlQuery q(queryString, db, os);
            q.bindString(":newPath", newSubfolderPath);
            q.bindBlob(":newHash", newSubfolderHash);
            q.bindString(":oldPath", path);
            q.bindBlob(":oldHash", oldSubfolderHash);
            q.update();
            CHECK_OP(os, );
        }
    }
}

QString MysqlObjectDbi::getFolderPreviousPath(const QString &currentPath, U2OpStatus &os) {
    const QString canonicalFolder = U2DbiUtils::makeFolderCanonical(currentPath);
    const QByteArray hash = QCryptographicHash::hash(canonicalFolder.toLatin1(), QCryptographicHash::Md5).toHex();

    static const QString queryString = "SELECT previousPath FROM Folder WHERE hash = :hash";
    U2SqlQuery query(queryString, db, os);
    query.bindBlob(":hash", hash);
    const QStringList result = query.selectStrings();
    SAFE_POINT_EXT(1 == result.size(), os.setError("Unconsistent data about folders in the database"), "");
    return result.first();
}

qint64 MysqlObjectDbi::countObjects(const QString& folder, U2OpStatus& os) {
    const QString canonicalFolder = U2DbiUtils::makeFolderCanonical(folder);
    const QByteArray hash = QCryptographicHash::hash(canonicalFolder.toLatin1(), QCryptographicHash::Md5).toHex();

    static const QString queryString = "SELECT COUNT(*) FROM FolderContent AS fc, Folder AS f "
        "WHERE f.hash = :hash AND fc.folder = f.id";
    U2SqlQuery q(queryString, db, os);
    q.bindBlob(":hash", hash);
    return q.selectInt64();
}

QList<U2DataId> MysqlObjectDbi::getObjects(const QString& folder, qint64 offset, qint64 count, U2OpStatus& os) {
    const QString canonicalFolder = U2DbiUtils::makeFolderCanonical(folder);
    const QByteArray hash = QCryptographicHash::hash(canonicalFolder.toLatin1(), QCryptographicHash::Md5).toHex();

    static const QString queryString = "SELECT o.id, o.type, '' FROM Object AS o, FolderContent AS fc, Folder AS f WHERE f.hash = :hash AND fc.folder = f.id AND fc.object = o.id AND o." + TOP_LEVEL_FILTER;
    U2SqlQuery q(queryString, offset, count, db, os);
    q.bindString(":hash", hash);
    return q.selectDataIdsExt();
}

QStringList MysqlObjectDbi::getObjectFolders(const U2DataId& objectId, U2OpStatus& os) {
    static const QString queryString = "SELECT f.path FROM FolderContent AS fc, Folder AS f WHERE fc.object = :object AND fc.folder = f.id";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":object", objectId);
    QStringList result = q.selectStrings();
    return result;
}

qint64 MysqlObjectDbi::getFolderLocalVersion(const QString& folder, U2OpStatus& os) {
    const QString canonicalFolder = U2DbiUtils::makeFolderCanonical(folder);
    const QByteArray hash = QCryptographicHash::hash(canonicalFolder.toLatin1(), QCryptographicHash::Md5).toHex();

    static const QString queryString = "SELECT vlocal FROM Folder WHERE hash = :hash LIMIT 1";
    U2SqlQuery q(queryString, db, os);
    q.bindString(":hash", hash);
    return q.selectInt64();
}

qint64 MysqlObjectDbi::getFolderGlobalVersion(const QString& folder, U2OpStatus& os) {
    const QString canonicalFolder = U2DbiUtils::makeFolderCanonical(folder);
    const QByteArray hash = QCryptographicHash::hash(canonicalFolder.toLatin1(), QCryptographicHash::Md5).toHex();

    static const QString queryString = "SELECT vglobal FROM Folder WHERE hash = :hash LIMIT 1";
    U2SqlQuery q(queryString, db, os);
    q.bindString(":hash", hash);
    return q.selectInt64();
}

U2DbiObjectRank MysqlObjectDbi::getObjectRank(const U2DataId &objectId, U2OpStatus& os) {
    static const QString queryString("SELECT rank FROM Object WHERE id = :id");
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":id", objectId);
    return static_cast<U2DbiObjectRank> (q.selectInt32());
}


//////////////////////////////////////////////////////////////////////////
// Write methods for objects

bool MysqlObjectDbi::removeObject(const U2DataId& dataId, bool force, U2OpStatus& os) {
    bool result = removeObjectImpl(dataId, force, os);
    CHECK_OP(os, result);

    if (result) {
        onFolderUpdated("");
    }
    return result;
}

bool MysqlObjectDbi::removeObject(const U2DataId &dataId, U2OpStatus &os) {
    return removeObject(dataId, false, os);
}

namespace {

QString createDeleteObjectQueryStr(int objectCount) {
    static const QString queryStartStr("DELETE FROM Object WHERE id IN (");
    static const QString bindingStr("?,");
    static const QString lastBindingStr("?)");

    QString result(queryStartStr);
    result.append(bindingStr.repeated(objectCount - 1)).append(lastBindingStr);
    return result;
}

}

bool MysqlObjectDbi::removeObjects(const QList<U2DataId>& dataIds, bool force, U2OpStatus& os) {
    Q_UNUSED(force)
    CHECK(!dataIds.isEmpty(), true);

    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    // remove specific objects' data first
    foreach(const U2DataId &objectId, dataIds) {
        removeObjectSpecificData(objectId, os);
        CHECK_OP(os, false);
    }

    // then remove general ones
    const int idsCount = dataIds.count();
    const int residualBindQueryCount = idsCount % MysqlDbi::BIND_PARAMETERS_LIMIT;
    const int fullBindQueryCount = idsCount / MysqlDbi::BIND_PARAMETERS_LIMIT;

    // prepare query strings
    const QString residualQueryStr = createDeleteObjectQueryStr(residualBindQueryCount);
    QString fullQueryStr;
    if (fullBindQueryCount > 0) {
        fullQueryStr = createDeleteObjectQueryStr(MysqlDbi::BIND_PARAMETERS_LIMIT);
    }

    // execute deletion of residual objects
    U2SqlQuery residualDeletionQuery(residualQueryStr, db, os);
    for (int i = 0; i < residualBindQueryCount; ++i) {
        residualDeletionQuery.addBindDataId(dataIds.at(i));
    }
    CHECK(residualBindQueryCount == residualDeletionQuery.update(), false);
    CHECK_OP(os, false);

    // execute deletion of other objects in a batch fashion
    if (fullBindQueryCount > 0) {
        U2SqlQuery fullDeletionQuery(fullQueryStr, db, os);
        for (int row = 0; row < fullBindQueryCount; ++row) {
            for (int col = 0; col < MysqlDbi::BIND_PARAMETERS_LIMIT; ++col) {
                fullDeletionQuery.addBindDataId(dataIds.at(residualBindQueryCount + row * MysqlDbi::BIND_PARAMETERS_LIMIT + col));
            }
            CHECK(residualBindQueryCount == fullDeletionQuery.update(), false);
            CHECK_OP(os, false);
            fullDeletionQuery.finish();
        }
    }

    onFolderUpdated("");
    return !os.hasError();
}

bool MysqlObjectDbi::removeObjects(const QList<U2DataId> &dataIds, U2OpStatus &os) {
    return removeObjects(dataIds, false, os);
}

void MysqlObjectDbi::renameObject(const U2DataId &id, const QString &newName, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString = "UPDATE Object SET name = :name WHERE id = :id";
    U2SqlQuery q(queryString, db, os);
    q.bindString(":name", newName);
    q.bindDataId(":id", id);
    q.execute();
    CHECK_OP(os, );

    incrementVersion(id, os);
}

//////////////////////////////////////////////////////////////////////////
// Write methods for folders

void MysqlObjectDbi::createFolder(const QString& path, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);
    CHECK_OP(os, );

    const QString canonicalPath = U2DbiUtils::makeFolderCanonical(path);
    const QByteArray hash = QCryptographicHash::hash(canonicalPath.toLatin1(), QCryptographicHash::Md5).toHex();

    static const QString queryString = "INSERT IGNORE INTO Folder(path, hash) VALUES(:path, :hash)";
    U2SqlQuery q(queryString, db, os);
    q.bindString(":path", canonicalPath);
    q.bindBlob(":hash", hash);
    qint64 affected = q.update();
    CHECK_OP(os, );
    CHECK(affected != 0, );

    QString parentFolder = canonicalPath;
    if (U2ObjectDbi::ROOT_FOLDER != parentFolder) {
        parentFolder.truncate(parentFolder.lastIndexOf(U2ObjectDbi::PATH_SEP));
        if (parentFolder.isEmpty()) {
            parentFolder = U2ObjectDbi::ROOT_FOLDER;
        }
        createFolder(parentFolder, os);
    }

    onFolderUpdated(canonicalPath);
}

bool MysqlObjectDbi::removeFolder(const QString& folder, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    const QString canonicalFolder = U2DbiUtils::makeFolderCanonical(folder);
    const QByteArray hash = QCryptographicHash::hash(canonicalFolder.toLatin1(), QCryptographicHash::Md5).toHex();

    // remove subfolders first
    static const QString selectSubfoldersString = "SELECT path FROM Folder WHERE path LIKE BINARY :path "
        "ORDER BY LENGTH(path) DESC";
    U2SqlQuery selectSubfoldersQuery(selectSubfoldersString, db, os);
    selectSubfoldersQuery.bindString(":path", canonicalFolder + U2ObjectDbi::PATH_SEP + "%");
    const QStringList subfolders = selectSubfoldersQuery.selectStrings();
    CHECK_OP(os, false);

    bool result = true;
    foreach (const QString &subfolder, subfolders) {
        result = removeFolder(subfolder, os);
        CHECK_OP(os, false);
    }

    // remove all objects from folder
    QList<U2DataId> objects = getObjects(canonicalFolder, 0, U2DbiOptions::U2_DBI_NO_LIMIT, os);
    CHECK_OP(os, false);
    bool deleted = removeObjects(objects, false, os);
    CHECK_OP(os, false);
    if (result && !deleted) {
        result = false;
    }

    if (result) {
        // remove folder record
        static const QString deleteFolderString = "DELETE FROM Folder WHERE hash = :hash";
        U2SqlQuery deleteFolderQuery(deleteFolderString, db, os);
        deleteFolderQuery.bindString(":hash", hash);
        deleteFolderQuery.execute();
        CHECK_OP(os, false);

        onFolderUpdated(canonicalFolder);
    }
    return result;
}

void MysqlObjectDbi::addObjectsToFolder(const QList<U2DataId>& objectIds, const QString& folder, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    const QString canonicalFolder = U2DbiUtils::makeFolderCanonical(folder);

    qint64 folderId = getFolderId(canonicalFolder, true, db, os);
    CHECK_OP(os, );

    static const QString countString = "SELECT count(*) FROM FolderContent WHERE folder = :folder AND object = :object";
    static const QString insertString = "INSERT INTO FolderContent(folder, object) VALUES(:folder, :object)";
    static const QString rankString = "UPDATE Object SET " + TOP_LEVEL_FILTER + " WHERE id = :id";

    U2SqlQuery countQ(countString, db, os);
    U2SqlQuery insertQ(insertString, db, os);
    U2SqlQuery rankQ(rankString, db, os);

    foreach (const U2DataId& objectId, objectIds) {
        countQ.bindInt64(":folder", folderId);
        countQ.bindDataId(":object", objectId);
        int count = countQ.selectInt64();
        CHECK_OP(os, );

        if (count != 0) {
            // object is already in folder, skip it
            continue;
        }

        insertQ.bindInt64(":folder", folderId);
        insertQ.bindDataId(":object", objectId);
        insertQ.execute();
        CHECK_OP(os, );

        rankQ.bindDataId(":id", objectId);
        rankQ.execute();
        CHECK_OP(os, );
    }

    onFolderUpdated(canonicalFolder);
}

void MysqlObjectDbi::moveObjects(const QList<U2DataId>& objectIds, const QString& fromFolder,
    const QString& toFolder, U2OpStatus& os, bool saveFromFolder)
{
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    const QString canonicalFromFolder = U2DbiUtils::makeFolderCanonical(fromFolder);
    const QString canonicalToFolder = U2DbiUtils::makeFolderCanonical(toFolder);

    CHECK(canonicalFromFolder != canonicalToFolder, );

    addObjectsToFolder(objectIds, canonicalToFolder, os);
    CHECK_OP(os, );

    foreach (const U2DataId &id, objectIds) {
        removeObjectFromFolder(id, canonicalFromFolder, os);
        CHECK_OP(os, );
    }

    if (saveFromFolder) {
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

QStringList MysqlObjectDbi::restoreObjects(const QList<U2DataId> &objectIds, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

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

void MysqlObjectDbi::updateObjectAccessTime(const U2DataId &objectId, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString = "UPDATE ObjectAccessTrack SET lastAccessTime = NOW() WHERE object = :object";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":object", objectId);
    q.update();
}

//////////////////////////////////////////////////////////////////////////
// Undo/redo methods

void MysqlObjectDbi::undo(const U2DataId& objId, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    QString errorDescr = U2DbiL10n::tr("Can't undo an operation for the object");

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
        coreLog.trace("Called 'undo' for an object without modifications tracking enabled");
        os.setError(errorDescr);
        return;
    }

    // Get all single modifications steps
    qint64 userModStepVersion = dbi->getMysqlModDbi()->getNearestUserModStepVersion(objId, obj.version - 1, os);
    if (os.hasError()) {
        coreLog.trace("Error getting the nearest userModStep version: " + os.getError());
        os.setError(errorDescr);
        return;
    }

    QList< QList<U2SingleModStep> > modSteps = dbi->getMysqlModDbi()->getModSteps(objId, userModStepVersion, os);
    if (os.hasError()) {
        coreLog.trace("Error getting modSteps for an object: " + os.getError());
        os.setError(errorDescr);
        return;
    }

    QList< QList<U2SingleModStep> >::const_iterator multiIt = modSteps.end();
    while (multiIt != modSteps.begin()) {
        --multiIt;
        const QList<U2SingleModStep>& multiStepSingleSteps = *multiIt;

        foreach (const U2SingleModStep& modStep, multiStepSingleSteps) {
            undoSingleModStep(modStep, os);
            CHECK_OP(os, );

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

void MysqlObjectDbi::redo(const U2DataId& objId, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    QString errorDescr = U2DbiL10n::tr("Can't redo an operation for the object");

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
        coreLog.trace("Called 'redo' for an object without modifications tracking enabled");
        os.setError(errorDescr);
        return;
    }

    // Get all single modification steps
    QList< QList<U2SingleModStep> > modSteps = dbi->getMysqlModDbi()->getModSteps(objId, obj.version, os);
    if (os.hasError()) {
        coreLog.trace("Error getting modSteps for an object: " + os.getError());
        os.setError(errorDescr);
        return;
    }

    foreach (const QList<U2SingleModStep>& multiStepSingleSteps, modSteps) {
        QSet<U2DataId> objectIds;

        foreach (const U2SingleModStep& modStep, multiStepSingleSteps) {
            redoSingleModStep(modStep, os);
            CHECK_OP(os, );
            objectIds.insert(modStep.objectId);
        }
        objectIds.insert(objId);

        foreach (U2DataId objId, objectIds) {
            incrementVersion(objId, os);
            if (os.hasError()) {
                coreLog.trace("Can't increment an object version");
                os.setError(errorDescr);
                return;
            }
        }
    }
}

bool MysqlObjectDbi::canUndo(const U2DataId& objId, U2OpStatus& os) {
    return dbi->getMysqlModDbi()->canUndo(objId, os);
}

bool MysqlObjectDbi::canRedo(const U2DataId& objId, U2OpStatus& os) {
    return dbi->getMysqlModDbi()->canRedo(objId, os);
}


//////////////////////////////////////////////////////////////////////////
// Helper methods

U2DataId MysqlObjectDbi::createObject(U2Object& object, const QString& folder, U2DbiObjectRank rank, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    U2DataType type = object.getType();
    const QString &vname = object.visualName;
    int trackMod = object.trackModType;

    static const QString objectInsertString = "INSERT INTO Object(type, rank, name, trackMod) VALUES(:type, :rank, :name, :trackMod)";
    U2SqlQuery objectInsertQuery(objectInsertString, db, os);
    objectInsertQuery.bindType(":type", type);
    objectInsertQuery.bindInt32(":rank", rank);
    objectInsertQuery.bindString(":name", vname);
    objectInsertQuery.bindInt32(":trackMod", trackMod);
    const U2DataId res = objectInsertQuery.insert(type);
    CHECK_OP(os, res);

    static const QString objectAccessTrackString = "INSERT INTO ObjectAccessTrack(object) VALUES(:object)";
    U2SqlQuery objectAccessTrackQuery(objectAccessTrackString, db, os);
    objectAccessTrackQuery.bindDataId(":object", res);
    objectAccessTrackQuery.execute();
    CHECK_OP(os, res);

    if (U2DbiObjectRank_TopLevel == rank) {
        const QString canonicalFolder = U2DbiUtils::makeFolderCanonical(folder);
        qint64 folderId = getFolderId(canonicalFolder, false, db, os);
        CHECK_OP(os, res);

        if (-1 == folderId) {
            createFolder(canonicalFolder, os);
            CHECK_OP(os, res);
            folderId = getFolderId(canonicalFolder, true, db, os);
            CHECK_OP(os, res);
        }

        static const QString folderInsertString = "INSERT INTO FolderContent(folder, object) VALUES(:folder, :object)";
        U2SqlQuery folderInsertQuery(folderInsertString, db, os);
        folderInsertQuery.bindInt64(":folder", folderId);
        folderInsertQuery.bindDataId(":object", res);
        folderInsertQuery.execute();
        CHECK_OP(os, res);
    }

    object.id = res;
    object.dbiId = dbi->getDbiId();
    object.version = getObjectVersion(object.id, os);

    return res;
}

void MysqlObjectDbi::setObjectRank(const U2DataId &objectId, U2DbiObjectRank newRank, U2OpStatus& os) {
    static const QString queryStr("UPDATE Object SET rank = :rank WHERE id = :id");
    U2SqlQuery query(queryStr, db, os);
    CHECK_OP(os, );
    query.bindInt32(":rank", newRank);
    query.bindDataId(":id", objectId);
    const qint64 modifiedRowsNumber = query.update();
    if (modifiedRowsNumber > 1) {
        os.setError(QObject::tr("Unexpected number of modified objects. Expected: 1, actual: %1").arg(modifiedRowsNumber));
    }
}

void MysqlObjectDbi::getObject(U2Object& object, const U2DataId& id, U2OpStatus& os) {
    static const QString queryString = "SELECT name, version, trackMod FROM Object WHERE id = :id";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":id", id);

    if (q.step()) {
        object.id = id;
        object.dbiId = dbi->getDbiId();
        object.visualName = q.getString(0);
        object.version = q.getInt64(1);

        int trackMod = q.getInt32(2);
        if (0 <= trackMod && trackMod < TRACK_MOD_TYPE_NR_ITEMS) {
            object.trackModType = (U2TrackModType)trackMod;
        } else {
            os.setError("Incorrect trackMod value in an object!");
            object.trackModType = NoTrack;
        }

        q.ensureDone();
    } else if (!os.hasError()) {
        os.setError(U2DbiL10n::tr("Object not found"));
    }
}

U2DataId MysqlObjectDbi::getObject(qint64 objectId, U2OpStatus& os) {
    U2SqlQuery q("SELECT id, type FROM Object WHERE id = :id", db, os);
    q.bindInt64(":id", objectId);
    if (q.step()) {
        U2DataId result = q.getDataIdExt(0);
        q.ensureDone();
        return result;
    } else if (!os.hasError()) {
        os.setError(U2DbiL10n::tr("Object not found."));
    }
    return U2DataId();
}

QHash<U2DataId, QString> MysqlObjectDbi::getObjectNames(qint64 offset, qint64 count, U2OpStatus &os) {
    QHash<U2DataId, QString> result;

    static const QString queryString = "SELECT id, type, name FROM Object WHERE " + TOP_LEVEL_FILTER;

    U2SqlQuery q(queryString, offset, count, db, os);
    CHECK_OP(os, result);
    while (q.step()) {
        const U2DataType type = q.getDataType(1);
        const U2DataId id = q.getDataId(0, type);
        const QString name = q.getString(2);
        result.insert(id, name);
    }
    return result;
}

void MysqlObjectDbi::updateObject(U2Object& obj, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    updateObjectCore(obj, os);
    CHECK_OP(os, );

    obj.version = getObjectVersion(obj.id, os);
}

qint64 MysqlObjectDbi::getFolderId(const QString& path, bool mustExist, MysqlDbRef* db, U2OpStatus& os) {
    const QString canonicalPath = U2DbiUtils::makeFolderCanonical(path);
    const QByteArray hash = QCryptographicHash::hash(canonicalPath.toLatin1(), QCryptographicHash::Md5).toHex();

    static const QString queryString = "SELECT id FROM Folder WHERE hash = :hash LIMIT 1";
    U2SqlQuery q(queryString, db, os);
    q.bindString(":hash", hash);
    qint64 res = q.selectInt64();
    CHECK_OP(os, -1);

    if (mustExist && res == -1) {
        os.setError(U2DbiL10n::tr("Folder not found: %1 (canonical: %2)").arg(path).arg(canonicalPath));
    }

    return res;
}

void MysqlObjectDbi::incrementVersion(const U2DataId& objectId, MysqlDbRef* db, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString = "UPDATE Object SET version = version + 1 WHERE id = :id";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":id", objectId);
    const int affectedRows = q.update();
    if (1 != affectedRows) {
        os.setError(QString("Invalid affected rows count for the object version update. Object ID: %1, affected rows: %2")
            .arg(QString(objectId)).arg(affectedRows));
    }
}

qint64 MysqlObjectDbi::getObjectVersion(const U2DataId& objectId, U2OpStatus& os) {
    static const QString queryString = "SELECT version FROM Object WHERE id = :id";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":id", objectId);
    return q.selectInt64();
}

void MysqlObjectDbi::setTrackModType(const U2DataId& objectId, U2TrackModType trackModType, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString updateObjectString = "UPDATE Object AS O LEFT OUTER JOIN Parent AS P ON O.id = P.child "
        "SET O.trackMod = :trackMod WHERE O.id = :id OR P.parent = :parent";
    U2SqlQuery updateObjectQuery(updateObjectString, db, os);
    updateObjectQuery.bindInt32(":trackMod", trackModType);
    updateObjectQuery.bindDataId(":id", objectId);
    updateObjectQuery.bindDataId(":parent", objectId);
    updateObjectQuery.execute();
}

U2TrackModType MysqlObjectDbi::getTrackModType(const U2DataId& objectId, U2OpStatus& os) {
    static const QString queryString = "SELECT trackMod FROM Object WHERE id = :id";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":id", objectId);

    if (q.step()) {
        int res = q.getInt32(0);
        SAFE_POINT(0 <= res && res < TRACK_MOD_TYPE_NR_ITEMS, "Incorrect trackMod value", NoTrack);
        q.ensureDone();
        return (U2TrackModType)res;
    } else if (!os.hasError()) {
        os.setError(U2DbiL10n::tr("Object not found"));
        return NoTrack;
    }

    return NoTrack;
}

void MysqlObjectDbi::removeParent(const U2DataId& parentId, const U2DataId& childId, bool removeDeadChild, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString = "DELETE FROM Parent WHERE parent = :parent AND child = :child";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":parent", parentId);
    q.bindDataId(":child", childId);
    q.update();
    CHECK_OP(os, );

    if (!removeDeadChild) {
        return;
    }

    const QList<U2DataId> parents = getParents(childId, os);
    if (!parents.isEmpty() || os.hasError()) {
        return;
    }

    const QList<QString> folders = getObjectFolders(childId, os);
    if (!folders.isEmpty() || os.hasError()) {
        return;
    }
    removeObject(childId, false, os);
}

void MysqlObjectDbi::setParent(const U2DataId& parentId, const U2DataId& childId, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString insertString = "INSERT IGNORE INTO Parent (parent, child) VALUES (:parent, :child)";
    U2SqlQuery insertQ(insertString, db, os);
    insertQ.bindDataId(":parent", parentId);
    insertQ.bindDataId(":child", childId);
    insertQ.execute();
}

//////////////////////////////////////////////////////////////////////////
// Private methods

void MysqlObjectDbi::updateObjectCore(U2Object &obj, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString = "UPDATE Object SET name = :name, version = version WHERE id = :id";
    U2SqlQuery q(queryString, db, os);
    q.bindString(":name", obj.visualName);
    q.bindDataId(":id", obj.id);
    q.execute();
}

void MysqlObjectDbi::updateObjectType(U2Object &obj, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString = "UPDATE Object SET type = :type WHERE id = :id";
    U2SqlQuery q(queryString, db, os);
    q.bindType(":type", obj.getType());
    q.bindDataId(":id", obj.id);
    q.execute();
}

bool MysqlObjectDbi::isObjectInUse(const U2DataId& id, U2OpStatus& os) {
    static const QString queryString = QString("SELECT COUNT(*) FROM ObjectAccessTrack "
        "WHERE object = :object AND lastAccessTime + INTERVAL %1 SECOND > NOW()").arg(OBJ_USAGE_CHECK_INTERVAL);
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":object", id);
    return 1 == q.selectInt64();
}

QList<U2DataId> MysqlObjectDbi::getAllObjectsInUse(U2OpStatus &os) {
    static const QString queryString = QString("SELECT oat.object, o.type, '' FROM ObjectAccessTrack AS oat, Object AS o "
        "WHERE lastAccessTime + INTERVAL %1 SECOND > NOW() AND o.id = oat.object").arg(OBJ_USAGE_CHECK_INTERVAL);
    U2SqlQuery q(queryString, db, os);
    return q.selectDataIdsExt();
}

void MysqlObjectDbi::removeObjectFromFolder(const U2DataId &id, const QString &folder, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    const qint64 folderId = getFolderId(folder, true, db, os);
    CHECK_OP(os, );

    static const QString deleteString = "DELETE FROM FolderContent WHERE folder = :folder AND object = :object";
    U2SqlQuery deleteQ(deleteString, db, os);
    CHECK_OP(os, );
    deleteQ.bindInt64(":folder", folderId);
    deleteQ.bindDataId(":object", id);
    deleteQ.execute();
}

void MysqlObjectDbi::removeObjectFromAllFolders(const U2DataId &id, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString deleteString = "DELETE FROM FolderContent WHERE object = :object";
    U2SqlQuery deleteQ(deleteString, db, os);
    CHECK_OP(os, );
    deleteQ.bindDataId(":object", id);
    deleteQ.execute();
}

bool MysqlObjectDbi::removeObjectImpl(const U2DataId& objectId, bool force, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    if (!force && isObjectInUse(objectId, os)) {
        return false;
    }

    removeObjectSpecificData(objectId, os);
    CHECK_OP(os, false);

    MysqlUtils::remove("Object", "id", objectId, 1, db, os);

    return !os.hasError();
}

void MysqlObjectDbi::removeObjectSpecificData(const U2DataId &objectId, U2OpStatus &os) {
    U2DataType type = dbi->getEntityTypeById(objectId);
    if (!U2Type::isObjectType(type)) {
        os.setError(U2DbiL10n::tr("Not an object, id: %1, type: %2").arg(U2DbiUtils::text(objectId)).arg(type));
        return;
    }

    switch (type) {
    case U2Type::Sequence:
    case U2Type::VariantTrack:
        // nothing has to be done for objects of these types
        break;
    case U2Type::Mca:
    case U2Type::Msa:
        dbi->getMysqlMsaDbi()->deleteRowsData(objectId, os);
        break;
    case U2Type::AnnotationTable:
        dbi->getMysqlFeatureDbi()->removeAnnotationTableData(objectId, os);
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
}

void MysqlObjectDbi::removeObjectAttributes(const U2DataId& id, U2OpStatus& os) {
    dbi->getAttributeDbi()->removeObjectAttributes(id, os);
}

void MysqlObjectDbi::removeObjectModHistory(const U2DataId& id, U2OpStatus& os) {
    dbi->getModDbi()->removeObjectMods(id, os);
}

void MysqlObjectDbi::incrementVersion(const U2DataId& id, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString = "UPDATE Object SET version = version + 1 WHERE id = :id";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":id", id);
    q.update();
}

void MysqlObjectDbi::setVersion(const U2DataId& id, qint64 version, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString = "UPDATE Object SET version = :version WHERE id = :id";
    U2SqlQuery q(queryString, db, os);
    q.bindInt64(":version", version);
    q.bindDataId(":id", id);
    q.update();
}

void MysqlObjectDbi::undoSingleModStep(const U2SingleModStep& modStep, U2OpStatus& os) {
    if (U2ModType::isUdrModType(modStep.modType)) {
        dbi->getMysqlUdrDbi()->undo(modStep, os);
    } else if (U2ModType::isMsaModType(modStep.modType)) {
        dbi->getMysqlMsaDbi()->undo(modStep.objectId, modStep.modType, modStep.details, os);
    } else if (U2ModType::isSequenceModType(modStep.modType)) {
        dbi->getMysqlSequenceDbi()->undo(modStep.objectId, modStep.modType, modStep.details, os);
    } else if (U2ModType::isObjectModType(modStep.modType)) {
        undoCore(modStep.objectId, modStep.modType, modStep.details, os);
    } else {
        os.setError(U2DbiL10n::tr("Can't undo an unknown operation: '%1'").arg(QString::number(modStep.modType)));
        return;
    }
}

void MysqlObjectDbi::redoSingleModStep(const U2SingleModStep& modStep, U2OpStatus &os) {
    if (U2ModType::isUdrModType(modStep.modType)) {
        dbi->getMysqlUdrDbi()->redo(modStep, os);
    } else if (U2ModType::isMsaModType(modStep.modType)) {
        dbi->getMysqlMsaDbi()->redo(modStep.objectId, modStep.modType, modStep.details, os);
    } else if (U2ModType::isSequenceModType(modStep.modType)) {
        dbi->getMysqlSequenceDbi()->redo(modStep.objectId, modStep.modType, modStep.details, os);
    } else if (U2ModType::isObjectModType(modStep.modType)) {
        redoCore(modStep.objectId, modStep.modType, modStep.details, os);
    } else {
        os.setError(U2DbiL10n::tr("Can't redo an unknown operation: '%1'").arg(QString::number(modStep.modType)));
        return;
    }
}

void MysqlObjectDbi::undoCore(const U2DataId &objId, qint64 modType, const QByteArray &modDetails, U2OpStatus &os) {
    if (U2ModType::objUpdatedName == modType) {
        undoUpdateObjectName(objId, modDetails, os);
    } else {
        os.setError(U2DbiL10n::tr("Unexpected modification type '%1'").arg(QString::number(modType)));
        return;
    }
}

void MysqlObjectDbi::redoCore(const U2DataId &objId, qint64 modType, const QByteArray &modDetails, U2OpStatus &os) {
    if (U2ModType::objUpdatedName == modType) {
        redoUpdateObjectName(objId, modDetails, os);
    } else {
        os.setError(U2DbiL10n::tr("Unexpected modification type '%1'").arg(QString::number(modType)));
        return;
    }
}

void MysqlObjectDbi::undoUpdateObjectName(const U2DataId& id, const QByteArray& modDetails, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    QString oldName;
    QString newName;

    bool ok = U2DbiPackUtils::unpackObjectNameDetails(modDetails, oldName, newName);
    CHECK_EXT(ok, os.setError(U2DbiL10n::tr("An error occurred during updating an object name")), );

    // Update the value
    static const QString queryString = "UPDATE Object SET name = :name WHERE id = :id";
    U2SqlQuery q(queryString, db, os);
    q.bindString(":name", oldName);
    q.bindDataId(":id", id);
    q.update();
}

void MysqlObjectDbi::redoUpdateObjectName(const U2DataId& id, const QByteArray& modDetails, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    QString oldName;
    QString newName;

    bool ok = U2DbiPackUtils::unpackObjectNameDetails(modDetails, oldName, newName);
    CHECK_EXT(ok, os.setError(U2DbiL10n::tr("An error occurred during updating an object name!")), );

    U2Object obj;
    getObject(obj, id, os);
    CHECK_OP(os, );

    obj.visualName = newName;
    updateObjectCore(obj, os);
}

void MysqlObjectDbi::onFolderUpdated(const QString& ) {
    // Do nothing.
}

}   // namespace U2
