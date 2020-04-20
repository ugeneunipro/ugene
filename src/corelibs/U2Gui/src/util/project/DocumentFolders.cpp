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

#include <U2Core/DocumentModel.h>
#include <U2Core/Folder.h>
#include <U2Core/Timer.h>
#include <U2Core/U2SafePoints.h>

#include "ProjectUtils.h"

#include "ConnectionHelper.h"
#include "DocumentFolders.h"

namespace U2 {

bool folderPathLessThan(const QString &first, const QString &second) {
    const bool firstPathInRecycle = ProjectUtils::isFolderInRecycleBinSubtree(first);
    const bool secondPathInRecycle = ProjectUtils::isFolderInRecycleBinSubtree(second);
    if (firstPathInRecycle && !secondPathInRecycle) {
        return true;
    } else if (secondPathInRecycle && !firstPathInRecycle) {
        return false;
    } else {
        return QString::compare(first, second, Qt::CaseInsensitive) < 0;
    }
}

DocumentFoldersUpdate::DocumentFoldersUpdate() {

}

DocumentFoldersUpdate::DocumentFoldersUpdate(const U2DbiRef &dbiRef, U2OpStatus &os) {
    ConnectionHelper con(dbiRef, os);
    CHECK_OP(os, );

    u2objectFolders = con.oDbi->getObjectFolders(os);
    CHECK_OP(os, );

    folders = con.oDbi->getFolders(os);
    CHECK_OP(os, );
    std::sort(folders.begin(), folders.end(), folderPathLessThan);
    QHash<U2Object, QString>::ConstIterator i = u2objectFolders.constBegin();
    for (; i!=u2objectFolders.constEnd(); i++) {
        objectIdFolders[i.key().id] = i.value();
    }
}

DocumentFolders::DocumentFolders()
    : doc(NULL)
{

}

void DocumentFolders::init(Document *doc, U2OpStatus &os) {
    GTIMER(c, t, "DocumentFolders::init");
    this->doc = doc;
    if (ProjectUtils::isConnectedDatabaseDoc(doc)) {
        setLastUpdate(DocumentFoldersUpdate(doc->getDbiRef(), os));
        CHECK_OP(os, );

        foreach (GObject *obj, doc->getObjects()) {
            U2DataId id = obj->getEntityRef().entityId;
            if (hasFolderInfo(id)) {
                addObject(obj, getFolderByObjectId(id));
            } else {
                coreLog.error("Unknown object id");
            }
        }

        foreach (const QString &path, allFolders()) {
            const QStringList pathList = path.split(U2ObjectDbi::PATH_SEP, QString::SkipEmptyParts);
            QString fullPath;
            foreach (const QString &folder, pathList) {
                fullPath += U2ObjectDbi::PATH_SEP + folder;
                if (!hasFolder(fullPath)) {
                    foldersMap[fullPath] = new Folder(doc, fullPath);
                }
            }
        }
    } else {
        foreach (GObject *obj, doc->getObjects()) {
            addObject(obj, U2ObjectDbi::ROOT_FOLDER);
        }
    }
}

bool DocumentFolders::hasFolder(const QString &path) const {
    return foldersMap.contains(path) || U2ObjectDbi::ROOT_FOLDER == path;
}

Folder * DocumentFolders::getFolder(const QString &path) const {
    SAFE_POINT(U2ObjectDbi::ROOT_FOLDER != path, "Unexpected folder path", NULL);
    SAFE_POINT(foldersMap.contains(path), "Unknown path", NULL);
    return foldersMap[path];
}

void DocumentFolders::addFolder(const QString &path) {
    SAFE_POINT(!hasFolder(path), "The folder already exists", );

    if (!ProjectUtils::isFolderInRecycleBinSubtree(path)) { // add all folders from @path if they don't exist
        const QStringList pathList = path.split(U2ObjectDbi::PATH_SEP, QString::SkipEmptyParts);
        QString fullPath;
        foreach (const QString &folder, pathList) {
            fullPath += U2ObjectDbi::PATH_SEP + folder;
            if (hasFolder(fullPath)) {
                continue;
            }
            foldersMap[fullPath] = new Folder(doc, fullPath);

            // There is a new folder in the model -> update caches
            onFolderAdded(fullPath);
        }
    } else { // if some folder is being removed to Recycle Bin, then all its parent folders do not have to be created,
        // since actual parent folders exist and they can produce name conflicts if they are removed.
        foldersMap[path] = new Folder(doc, path);
        onFolderAdded(path);
    }

    addFolderToStorage(path);
}

void DocumentFolders::renameFolder(const QString &oldPath, const QString &newPath) {
    QStringList foldersToRename;
    foldersToRename << oldPath;
    foldersToRename << getAllSubFolders(oldPath);

    const int oldPathLength = oldPath.length();
    while (!foldersToRename.isEmpty()) {
        Folder *folder = getFolder(foldersToRename.takeLast());
        if (NULL == folder) {
            continue;
        }
        const QString folderPrevPath = folder->getFolderPath();
        QString folderNewPath = folderPrevPath;
        folderNewPath.replace(0, oldPathLength, newPath);

        // remove objects of the folder
        QList<GObject*> objs = getObjects(folderPrevPath);
        foreach (GObject *obj, objs) {
            moveObject(obj, folderPrevPath, folderNewPath);
        }

        // update caches
        onFolderRemoved(folder);

        removeFolderFromStorage(folderPrevPath);
        foldersMap.remove(folderPrevPath);

        folder->setFolderPath(folderNewPath);
        foldersMap.insert(folderNewPath, folder);

        addFolderToCache(folderNewPath);
        addFolderToStorage(folderNewPath);
    }
}

void DocumentFolders::removeFolder(const QString &path) {
    QStringList foldersToRemove;
    foldersToRemove << path;
    foldersToRemove << getAllSubFolders(path);

    while (!foldersToRemove.isEmpty()) {
        Folder *folder = getFolder(foldersToRemove.takeLast());
        if (NULL == folder) {
            continue;
        }
        const QString folderPath = folder->getFolderPath();

        // remove objects of the folder
        QList<GObject*> objs = getObjectsNatural(folderPath);
        foreach (GObject *obj, objs) {
            removeObject(obj, folderPath);
        }

        // update caches
        onFolderRemoved(folder);

        delete foldersMap[folderPath]; // now @folder is bad pointer
        foldersMap.remove(folderPath);
        removeFolderFromStorage(folderPath);
    }
}

int DocumentFolders::getNewFolderRowInParent(const QString &path) const {
    QString parentPath = Folder::getFolderParentPath(path);
    QString name = Folder::getFolderName(path);

    QStringList subFoldersNames;
    if (hasCachedSubFolders.value(parentPath, false)) {
        subFoldersNames = cachedSubFoldersNames[parentPath];
    } else {
        subFoldersNames = calculateSubFoldersNames(parentPath);
        cacheSubFoldersNames(parentPath, subFoldersNames);
    }
    SAFE_POINT(!subFoldersNames.contains(name), "The name is already in model", 0);
    int result = FolderObjectTreeStorage::insertSorted(name, subFoldersNames);

    return result;
}

int DocumentFolders::getNewObjectRowInParent(GObject *obj, const QString &parentPath) const {
    SAFE_POINT(!hasFolderInfo(obj), "Object is already in model", -1);
    const QList<GObject *> objects = getObjects(parentPath);
    QList<GObject *>::const_iterator i = std::upper_bound(objects.constBegin(), objects.constEnd(), obj, GObject::objectLessThan);
    return getSubFolders(parentPath).size() + (i - objects.constBegin());
}

QList<Folder*> DocumentFolders::getSubFolders(const QString &parentPath) const {
    if (hasCachedSubFolders.value(parentPath, false)) {
        return cachedSubFolders[parentPath];
    }

    if (ProjectUtils::isFolderInRecycleBin(parentPath)) {
        return QList<Folder*>();
    } else {
        QStringList subFoldersNames = calculateSubFoldersNames(parentPath);
        return cacheSubFoldersNames(parentPath, subFoldersNames);
    }
}

QList<Folder*> DocumentFolders::getSubFoldersNatural(const QString &parentPath) const {
    QStringList subFoldersNames = calculateSubFoldersNames(parentPath);
    QList<Folder*> result;
    foreach (const QString &name, subFoldersNames) {
        QString path = Folder::createPath(parentPath, name);
        Folder *f = getFolder(path);
        if (NULL != f) {
            result << f;
        }
    }
    return result;
}

void DocumentFolders::addFolderToCache(const QString &path) {
    QString parentPath = Folder::getFolderParentPath(path);

    if (hasCachedSubFolders.value(parentPath, false)) {
        int pos = FolderObjectTreeStorage::insertSorted(Folder::getFolderName(path), cachedSubFoldersNames[parentPath]);
        cachedSubFolders[parentPath].insert(pos, getFolder(path));
    }
}

QList<GObject*> DocumentFolders::getObjects(const QString &parentPath) const {
    if (ProjectUtils::isFolderInRecycleBin(parentPath)) {
        return QList<GObject*>();
    } else {
        return getObjectsNatural(parentPath);
    }
}

QString DocumentFolders::getParentFolder(const QString &path) {
    if (ProjectUtils::isFolderInRecycleBin(path)) {
        return ProjectUtils::RECYCLE_BIN_FOLDER_PATH;
    } else {
        return Folder::getFolderParentPath(path);
    }
}

QStringList DocumentFolders::calculateSubFoldersNames(const QString &parentPath) const {
    QStringList result;
    QString parentPathFin = (U2ObjectDbi::ROOT_FOLDER == parentPath) ? parentPath : parentPath + U2ObjectDbi::PATH_SEP;
    QString lastName;
    foreach (const QString &path, allFolders()) {
        if (U2ObjectDbi::ROOT_FOLDER == path) {
            continue;
        }
        if (path.startsWith(parentPathFin)) {
            QString subPath = path.mid(parentPathFin.size());
            int sepPos = subPath.indexOf(U2ObjectDbi::PATH_SEP);
            QString name = (-1 == sepPos) ? subPath : subPath.left(sepPos);
            if (lastName != name) {
                result << name;
                lastName = name;
            }
        }
    }
    return result;
}

QList<Folder*> & DocumentFolders::cacheSubFoldersNames(const QString &parentPath, const QStringList &subFoldersNames) const {
    QList<Folder*> result;
    foreach (const QString &name, subFoldersNames) {
        QString path = Folder::createPath(parentPath, name);
        Folder *f = getFolder(path);
        if (NULL != f) {
            result << f;
        }
    }
    hasCachedSubFolders[parentPath] = true;
    cachedSubFolders[parentPath] = result;
    cachedSubFoldersNames[parentPath] = subFoldersNames;
    return cachedSubFolders[parentPath];
}

QStringList DocumentFolders::getAllSubFolders(const QString &path) const {
    QStringList result;
    QList<Folder*> subFolders = getSubFoldersNatural(path);
    while (!subFolders.isEmpty()) {
        Folder *folder = subFolders.takeFirst();
        result << folder->getFolderPath();

        subFolders << getSubFoldersNatural(folder->getFolderPath());
    }
    return result;
}

void DocumentFolders::onFolderAdded(const QString &path) {
    QString parentPath = getParentFolder(path);
    addFolderToCache(path);
}

void DocumentFolders::onFolderRemoved(Folder *folder) {
    QString path = folder->getFolderPath();
    // remove cached subfolders
    if (hasCachedSubFolders.value(path, false)) {
        cachedSubFoldersNames.remove(path);
        cachedSubFolders.remove(path);
    }

    // if parent folder has cached subfolders -> remove folder from its cache
    QString parentPath = getParentFolder(path);
    if (hasCachedSubFolders.value(parentPath, false)) {
        cachedSubFoldersNames[parentPath].removeAll(folder->getFolderName());
        cachedSubFolders[parentPath].removeAll(folder);
    }
}

/************************************************************************/
/* FolderObjectTreeStorage */
/************************************************************************/
bool FolderObjectTreeStorage::hasObject(const U2DataId &id) const {
    return objectsIds.contains(id);
}

GObject * FolderObjectTreeStorage::getObject(const U2DataId &id) const {
    SAFE_POINT(hasObject(id), "Unknown object id", NULL);
    return objectsIds.value(id, NULL);
}

namespace {

void insertObjectToSortedList(QList<GObject *> &list, GObject *obj) {
    QList<GObject *>::iterator insertPos = std::upper_bound(list.begin(), list.end(), obj, GObject::objectLessThan);
    list.insert(insertPos, obj);
}

}

void FolderObjectTreeStorage::addObject(GObject *obj, const QString &path) {
    objectsIds[obj->getEntityRef().entityId] = obj;
    objectFolders[obj] = path;
    insertObjectToSortedList(folderObjects[path], obj);
    lastUpdate.objectIdFolders[obj->getEntityRef().entityId] = path;
}

void FolderObjectTreeStorage::removeObject(GObject *obj, const QString &path) {
    int count1 = objectsIds.remove(obj->getEntityRef().entityId);
    int count2 = objectFolders.remove(obj);
    int count3 = folderObjects[path].removeAll(obj);
    int count4 = lastUpdate.objectIdFolders.remove(obj->getEntityRef().entityId);
    SAFE_POINT(obj->isUnloaded() || 1 == count1, "Object was not in objectsIds", );
    SAFE_POINT(1 == count2, "Object was not in objectFolders", );
    SAFE_POINT(1 == count3, "Object was not in folderObjects", );
    SAFE_POINT(obj->isUnloaded() || 1 == count4, "Object was not in objectIdFolders", );
}

void FolderObjectTreeStorage::moveObject(GObject *obj, const QString &oldPath, const QString &newPath) {
    const U2DataId objId = obj->getEntityRef().entityId;

    SAFE_POINT(objectFolders.contains(obj) && objectFolders[obj] == oldPath, "Invalid object path", );
    SAFE_POINT(folderObjects[oldPath].contains(obj), "Invalid object path", );
    SAFE_POINT(lastUpdate.objectIdFolders.contains(objId) && lastUpdate.objectIdFolders[objId] == oldPath, "Invalid object path", );

    objectFolders[obj] = newPath;
    folderObjects[oldPath].removeAll(obj);
    insertObjectToSortedList(folderObjects[newPath], obj);
    lastUpdate.objectIdFolders[objId] = newPath;
}

QString FolderObjectTreeStorage::getObjectFolder(GObject *obj) const {
    SAFE_POINT(objectFolders.contains(obj), "Unknown object", U2ObjectDbi::ROOT_FOLDER);
    return objectFolders[obj];
}

QList<GObject*> FolderObjectTreeStorage::getObjectsNatural(const QString &parentPath) const {
    return folderObjects.value(parentPath);
}

const DocumentFoldersUpdate & FolderObjectTreeStorage::getLastUpdate() const {
    return lastUpdate;
}

void FolderObjectTreeStorage::setLastUpdate(const DocumentFoldersUpdate &value) {
    lastUpdate = value;
}

void FolderObjectTreeStorage::setIgnoredObjects(const QSet<U2DataId> &ids) {
    ignoredObjects = ids;
}

void FolderObjectTreeStorage::addIgnoredObject(const U2DataId &id) {
    SAFE_POINT(!ignoredObjects.contains(id), "Attempting to ignore object repeatedly", );
    ignoredObjects.insert(id);
}

void FolderObjectTreeStorage::setIgnoredFolders(const QSet<QString> &paths) {
    ignoredPaths = paths;
}

void FolderObjectTreeStorage::addIgnoredFolder(const QString &path) {
    SAFE_POINT(!ignoredPaths.contains(path), "Attempting to ignore folder repeatedly", );
    ignoredPaths.insert(path);
}

void FolderObjectTreeStorage::excludeFromObjFilter(const QSet<U2DataId> &ids) {
    foreach (const U2DataId &id, ids) {
        SAFE_POINT(ignoredObjects.contains(id), "Unexpected object detected", );
        ignoredObjects.remove(id);
    }
}

void FolderObjectTreeStorage::excludeFromFolderFilter(const QSet<QString> &paths) {
    foreach (const QString &path, paths) {
        SAFE_POINT(ignoredPaths.contains(path), "Unexpected path detected", );
        ignoredPaths.remove(path);
    }
}

bool FolderObjectTreeStorage::isObjectIgnored(const U2DataId &id) const {
    return ignoredObjects.contains(id);
}

bool FolderObjectTreeStorage::isFolderIgnored(const QString &path) const {
    if (ignoredPaths.contains(path)) {
        return true;
    }

    foreach (const QString &ignoredPath, ignoredPaths) {
        if (Folder::isSubFolder(ignoredPath, path)) {
            return true;
        }
    }
    return false;
}

const QStringList & FolderObjectTreeStorage::allFolders() const {
    return lastUpdate.folders;
}

void FolderObjectTreeStorage::addFolderToStorage(const QString &path) {
    insertSorted(path, lastUpdate.folders);
}

void FolderObjectTreeStorage::removeFolderFromStorage(const QString &path) {
    folderObjects.remove(path);
    lastUpdate.folders.removeAll(path);
}

bool FolderObjectTreeStorage::hasFolderInfo(const U2DataId &id) const {
    return lastUpdate.objectIdFolders.contains(id);
}

bool FolderObjectTreeStorage::hasFolderInfo(GObject *obj) const {
    return objectFolders.contains(obj);
}

QString FolderObjectTreeStorage::getFolderByObjectId(const U2DataId &id) const {
    return lastUpdate.objectIdFolders[id];
}

int FolderObjectTreeStorage::insertSorted(const QString &value, QStringList &list) {
    GTIMER(c, t, "FolderObjectTreeStorage::insertSorted");

    if (U2ObjectDbi::RECYCLE_BIN_FOLDER == value) {
        list.prepend(value);
        return 0;
    } else {
        QList<QString>::iterator i;
        if (value.startsWith(U2ObjectDbi::ROOT_FOLDER)) {
            i = std::upper_bound(list.begin(), list.end(), value, folderPathLessThan);
        } else {
            i = std::upper_bound(list.begin(), list.end(), value, Folder::folderNameLessThan);
        }
        if (list.end() != i && *i == U2ObjectDbi::RECYCLE_BIN_FOLDER) {
            ++i;
        }
        i = list.insert(i, value);
        return i - list.begin();
    }
}

} // U2
