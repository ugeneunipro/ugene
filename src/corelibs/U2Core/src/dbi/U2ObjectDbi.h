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

#ifndef _U2_OBJECT_DBI_H_
#define _U2_OBJECT_DBI_H_

#include <U2Core/U2Dbi.h>

namespace U2 {
/**
    An primary interface to access database content
    Any database contains objects that are placed into folders
    Folders have unique string IDs - constructed similar to full folders names on Unix systems
    The root folder "/" is required for any DBI
    */

enum DbFolderFlag {
    DbFolderFlag_None = 0,

    DbFolderFlag_Undeletable = 1 << 0,

    DbFolderFlag_Unchangeable = 1 << 1,

    DbFolderFlag_RestrictObjectView = 1 << 2
};

class U2CORE_EXPORT U2ObjectDbi : public U2ChildDbi {
protected:
    U2ObjectDbi(U2Dbi* rootDbi) : U2ChildDbi(rootDbi) {}

public:
    /**  Returns number of top-level U2Objects in database */
    virtual qint64 countObjects(U2OpStatus& os) = 0;

    /**  Returns number of top-level U2Objects with the specified type in database */
    virtual qint64 countObjects(U2DataType type, U2OpStatus& os) = 0;

    /**
     Retrieves U2Object fields from database entry with 'id'
     and sets these fields for 'object'
     */
    virtual void getObject(U2Object& object, const U2DataId& id, U2OpStatus& os) = 0;

    /**
    Retrieves database object by objectId.
    */
    virtual U2DataId getObject(qint64 objectId, U2OpStatus& os) = 0;

    /** Lists database top-level objects, starts with 'offset' and limits by 'count'.
    The 'offset' and 'count' can be arbitrarily large but should not be negative. Also, 'count' can have special value 'DBI_NO_LIMIT'. */
    virtual QList<U2DataId> getObjects(qint64 offset, qint64 count, U2OpStatus& os) = 0;

    /** Lists database top-level objects, starts with 'offset' and limits by 'count'.
    The 'offset' and 'count' can be arbitrarily large but should not be negative. Also, 'count' can have special value 'DBI_NO_LIMIT'. */
    virtual QHash<U2DataId, QString> getObjectNames(qint64 offset, qint64 count, U2OpStatus& os) = 0;

    /** Lists database top-level objects of the specified type, starts with 'offset' and limits by 'count'.
    The 'offset' and 'count' can be arbitrarily large but should not be negative. Also, 'count' can have special value 'DBI_NO_LIMIT'. */
    virtual QList<U2DataId> getObjects(U2DataType type, qint64 offset, qint64 count, U2OpStatus& os) = 0;

    /**  Returns parents for the entity.
        If entity is object, returns other object this object is a part of
        If object is not a part of any other object and does not belongs to any folder - it's automatically removed.
        */
    virtual QList<U2DataId> getParents(const U2DataId& entityId, U2OpStatus& os) = 0;


    /** Returns objects iterator with a given name and type. If type is Unknown -> checks objects of all types */
    virtual U2DbiIterator<U2DataId>* getObjectsByVisualName(const QString& visualName, U2DataType type, U2OpStatus& os) = 0;


    /**  Returns list of folders stored in database.
        Folders are represented as paths, separated by '/' character.
        At least one root folder is required.
        */
    virtual QStringList getFolders(U2OpStatus& os) = 0;

    /** Returns the map: object -> folder path. */
    virtual QHash<U2Object, QString> getObjectFolders(U2OpStatus& os) = 0;

    /** Returns version of the folder.
        The folder version increases if new object(s)/subfolder(s) are added into this folder
        Note that if object(s)/folder(s) are added into one of the subfolders the folder version is not changed
        */
    virtual qint64 getFolderLocalVersion(const QString& folder, U2OpStatus& os) = 0;

    /** Returns version of the folder that changes every time object(s)/folder(s) added
        to the specified folder or any of its child folders
        */
    virtual qint64 getFolderGlobalVersion(const QString& folder, U2OpStatus& os) = 0;

    /** Returns number of top-level U2Objects in folder */
    virtual qint64 countObjects(const QString& folder, U2OpStatus& os) = 0;

    /** Lists database top-level objects of the specified type, starts with 'offset' and limits by 'count'.
    The 'offset' and 'count' can be arbitrarily large but should not be negative. Also, 'count' can have special value 'U2DbiOptions::U2_DBI_NO_LIMIT'. */
    virtual QList<U2DataId> getObjects(const QString& folder, qint64 offset, qint64 count, U2OpStatus& os) = 0;

    /**  Returns all folders this object must be shown in  */
    virtual QStringList getObjectFolders(const U2DataId& objectId, U2OpStatus& os) = 0;

    /** Returns version of the given object */
    virtual qint64 getObjectVersion(const U2DataId& objectId, U2OpStatus& os) = 0;

    /** Specified whether modifications in object must be tracked or not */
    virtual void setTrackModType(const U2DataId& objectId, U2TrackModType trackModType, U2OpStatus& os) = 0;

    /** Updates object rank, e.g. a top-level object can be transmuted into a child object */
    virtual void setObjectRank(const U2DataId &objectId, U2DbiObjectRank newRank, U2OpStatus& os) = 0;

    /** Adds a record, representing parent-child relationship between entities, to a DB */
    virtual void setParent(const U2DataId& parentId, const U2DataId& childId, U2OpStatus& os) = 0;

    /** Returns object rank of the given object */
    virtual U2DbiObjectRank getObjectRank(const U2DataId &objectId, U2OpStatus& os) = 0;

    /** Gets the trackMod value for the object */
    virtual U2TrackModType getTrackModType(const U2DataId& objectId, U2OpStatus& os) = 0;

    /**
        Removes object from the database.
        If @force is true, the object will be removed if it is possible,
        else dbi implementation can refuse to remove it.
        Requires: U2DbiFeature_RemoveObjects feature support
        */
    virtual bool removeObject(const U2DataId& dataId, bool force, U2OpStatus& os) = 0;
    virtual bool removeObject(const U2DataId& dataId, U2OpStatus& os) { return removeObject(dataId, false, os); }

    /**
        Removes collection of objects from the database.
        If @force is true, the object will be removed if it is possible,
        else dbi implementation can refuse to remove it.
        Requires: U2DbiFeature_RemoveObjects feature support
        */
    virtual bool removeObjects(const QList<U2DataId>& dataIds, bool force, U2OpStatus& os) = 0;
    virtual bool removeObjects(const QList<U2DataId>& dataIds, U2OpStatus& os) { return removeObjects(dataIds, false, os); }


    /** Creates folder in the database.
        If the specified path is already presented in the database, nothing will be done.
        It is not required that parent folders must exist, they are created automatically.
        Requires: U2DbiFeature_ChangeFolders feature support
        */
    virtual void createFolder(const QString& path, U2OpStatus& os) = 0;

    /**
        Removes folder. The folder must be existing path. Runs GC check for all objects in the folder
        Requires: U2DbiFeature_ChangeFolders feature support
        */
    virtual bool removeFolder(const QString& folder, U2OpStatus& os) = 0;

    /** Renames the folder. Replaces all inner folders into the new one.
        The specified new path must be a valid unique path, not existing in the database.
        It is not required that parent folders must exist, they are created automatically.
        Requires: U2DbiFeature_ChangeFolders feature support
        */
    virtual void renameFolder(const QString &oldPath, const QString &newPath, U2OpStatus &os) = 0;

    /** Returns the folder's previous path if it is set or an empty string if folder has not been moved. */
    virtual QString getFolderPreviousPath(const QString &currentPath, U2OpStatus &os) = 0;

    /** Adds objects to the specified folder.
        All objects must exist and have a top-level type.
        Requires: U2DbiFeature_ChangeFolders feature support
        */
    virtual void addObjectsToFolder(const QList<U2DataId>& objectIds, const QString& toFolder, U2OpStatus& os) = 0;

    /** Moves objects between folders.
        'fromFolder' must be existing path containing all specified objects.
        'toFolder' must be existing path or empty string.
        If 'toFolder' is empty, removes the objects from 'fromFolder' and
        deletes non-top-level objects without parents, if any appear in the specified list.
        Otherwise, moves the specified objects between the specified folders, omitting duplicates.

        'saveFromFolder' parameter specifies whether the 'fromFolder' path has to be stored
        for further usage. For instance, when the object is moved to the recycle bin its previous folder
        is needed in order to restore the object afterwards.

        Requires: U2DbiFeature_ChangeFolders feature support
        */
    virtual void moveObjects(const QList<U2DataId>& objectIds, const QString& fromFolder,
        const QString& toFolder, U2OpStatus& os, bool saveFromFolder = false) = 0;

    /**
     Sets the new name @newName for the object with the given @id.
     */
    virtual void renameObject(const U2DataId &id, const QString &newName, U2OpStatus &os) = 0;

    /** Moves objects to the folder whose path was previously stored by the 'moveObjects' method
        Returns the new path
        */
    virtual QStringList restoreObjects(const QList<U2DataId> &objectIds, U2OpStatus &os) = 0;

    /** Method for databases allowing multiple clients. Updates timestamp of last access time
        to the object having `objectId`
        */
    virtual void updateObjectAccessTime(const U2DataId &objectId, U2OpStatus &os);

    /** Returns whether the object is used by some client of the DB.
        The method should be overridden for DBIs with multiple clients
        */
    virtual bool isObjectInUse(const U2DataId &id, U2OpStatus &os);
    virtual QList<U2DataId> getAllObjectsInUse(U2OpStatus &os);

    /** Undo the last update operation for the object. */
    virtual void undo(const U2DataId& objId, U2OpStatus& os) = 0;

    /** Redo the last update operation for the object. */
    virtual void redo(const U2DataId& objId, U2OpStatus& os) = 0;

    /** Returns "true" if there are modifications of the object that can be undone */
    virtual bool canUndo(const U2DataId& objId, U2OpStatus& os) = 0;

    /** Returns "true" if there are modifications of the object that can be redone */
    virtual bool canRedo(const U2DataId& objId, U2OpStatus& os) = 0;

    static const QString ROOT_FOLDER;
    static const QString RECYCLE_BIN_FOLDER;
    static const QString PATH_SEP;

    /** Time interval in milliseconds during which timestamps are updated in DB for active objects */
    static const int OBJECT_ACCESS_UPDATE_INTERVAL;

protected:
    static const QString PREV_OBJ_PATH_ATTR_NAME;
};

} //namespace

#endif
