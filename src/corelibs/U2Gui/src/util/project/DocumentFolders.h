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

#ifndef _U2_DOCUMENTFOLDERS_H_
#define _U2_DOCUMENTFOLDERS_H_

#include <QStringList>

#include <U2Core/U2OpStatus.h>
#include <U2Core/U2Type.h>

namespace U2 {

class GObject;
class Folder;

class DocumentFoldersUpdate {
public:
    DocumentFoldersUpdate();
    DocumentFoldersUpdate(const U2DbiRef &dbiRef, U2OpStatus &os);
    QStringList folders; // all folders
    QHash<U2DataId, QString> objectIdFolders; // objectId -> path
    QHash<U2Object, QString> u2objectFolders; // U2Object -> path
};

/**
 * The auxiliary class for encapsulating object <-> folder info
 */
class FolderObjectTreeStorage {
public:
    bool hasObject(const U2DataId &id) const;
    GObject * getObject(const U2DataId &id) const;
    void addObject(GObject *obj, const QString &path);
    void removeObject(GObject *obj, const QString &path);
    void moveObject(GObject *obj, const QString &oldPath, const QString &newPath);
    QString getObjectFolder(GObject *obj) const;

    QList<GObject*> getObjectsNatural(const QString &path) const;

    const DocumentFoldersUpdate & getLastUpdate() const;

    // objects from DB which have IDs from the `ids` list won't be affected by merging procedure
    // pass an empty set as `ids` to unset the filter
    void setIgnoredObjects(const QSet<U2DataId> &ids);
    void addIgnoredObject(const U2DataId &id);
    // folders from DB which have paths from the `paths` list won't be affected by merging procedure
    // pass an empty set as `paths` to unset the filter
    void setIgnoredFolders(const QSet<QString> &paths);
    void addIgnoredFolder(const QString &path);

    void excludeFromObjFilter(const QSet<U2DataId> &ids);
    void excludeFromFolderFilter(const QSet<QString> &paths);

    bool isObjectIgnored(const U2DataId &id) const;
    bool isFolderIgnored(const QString &path) const;

protected:
    void setLastUpdate(const DocumentFoldersUpdate &value);
    const QStringList & allFolders() const;
    void addFolderToStorage(const QString &path); // insert sorted
    void removeFolderFromStorage(const QString &path);
    bool hasFolderInfo(const U2DataId &id) const;
    bool hasFolderInfo(GObject *obj) const;
    QString getFolderByObjectId(const U2DataId &id) const;

    /** Returns the insertion pos */
    static int insertSorted(const QString &value, QStringList &list);

private:
    /**
     * Four structures:
     *    1) lastUpdate.objectIdFolders
     *    2) objectsIds
     *    3) objectFolders
     *    4) folderObjects
     * can be only simultaneously modified! Support the consistency for them!
     */
    DocumentFoldersUpdate lastUpdate;
    QHash<U2DataId, GObject*> objectsIds; // objectId -> GObject
    QHash<GObject*, QString> objectFolders; // GObject -> path
    QHash<QString, QList<GObject *> > folderObjects; // path -> GObject

    // these objects and folders won't be affected during merge
    QSet<U2DataId> ignoredObjects;
    QSet<QString> ignoredPaths;
};

class DocumentFolders : public FolderObjectTreeStorage {
public:
    DocumentFolders();
    void init(Document *doc, U2OpStatus &os);

    bool hasFolder(const QString &path) const;
    Folder * getFolder(const QString &path) const;
    void addFolder(const QString &path);
    void removeFolder(const QString &path);
    void renameFolder(const QString &oldPath, const QString &newPath);

    int getNewFolderRowInParent(const QString &path) const;
    int getNewObjectRowInParent(GObject *obj, const QString &parentPath) const;
    QList<Folder*> getSubFolders(const QString &path) const;
    QList<GObject*> getObjects(const QString &path) const;
    QStringList getAllSubFolders(const QString &path) const;

    static QString getParentFolder(const QString &path);

private:
    QStringList calculateSubFoldersNames(const QString &parentPath) const;
    QList<Folder*> & cacheSubFoldersNames(const QString &parentPath, const QStringList &subFoldersNames) const;
    void onFolderAdded(const QString &path); // updates caches
    void onFolderRemoved(Folder *folder); // updates caches
    QList<Folder*> getSubFoldersNatural(const QString &path) const; // without recycle bin rules
    void addFolderToCache(const QString &path);

private:
    Document *doc;

    mutable QHash<QString, Folder *> foldersMap; // path -> Folder

    mutable QHash<QString, bool> hasCachedSubFolders;
    mutable QHash<QString, QStringList> cachedSubFoldersNames; // sorted
    mutable QHash<QString, QList<Folder *> > cachedSubFolders;
};

} // U2

#endif // _U2_DOCUMENTFOLDERS_H_
