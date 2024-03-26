/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#pragma once

#include "SQLiteDbi.h"

namespace U2 {

class U2FORMATS_EXPORT SQLiteObjectDbi : public U2ObjectDbi, public SQLiteChildDBICommon {
public:
    SQLiteObjectDbi(SQLiteDbi* dbi);

    // Read methods for objects

    /**  Returns number of top-level U2Objects in database */
    qint64 countObjects(U2OpStatus& os) override;

    /**  Returns number of top-level U2Objects with the specified type in database */
    qint64 countObjects(U2DataType type, U2OpStatus& os) override;

    /**
     Retrieves U2Object fields from database entry with 'id'
     and sets these fields for 'object'
    */
    U2DataType getObject(U2Object& object, const U2DataId& id, U2OpStatus& os) override;

    /**
    Retrieves database object by objectId.
    */
    U2DataId getObject(qint64 objectId, U2OpStatus& os) override;

    QHash<U2DataId, QString> getObjectNames(qint64 offset, qint64 count, U2OpStatus& os) override;

    /** Lists database top-level objects, starts with 'offset' and limits by 'count' */
    QList<U2DataId> getObjects(qint64 offset, qint64 count, U2OpStatus& os) override;

    /** Lists database top-level objects of the specified type, starts with 'offset' and limits by 'count' */
    QList<U2DataId> getObjects(U2DataType type, qint64 offset, qint64 count, U2OpStatus& os) override;

    /**  Returns parents for entity.
        If entity is object, returns other object this object is a part of
        If object is not a part of any other object and does not belongs to any folder - it's automatically removed.
     */
    QList<U2DataId> getParents(const U2DataId& entityId, U2OpStatus& os) override;

    U2DbiIterator<U2DataId>* getObjectsByVisualName(const QString& visualName, U2DataType type, U2OpStatus& os) override;

    // Read methods for folders

    /**  Returns list of folders stored in database.
        Folders are separated by '/' character.
        At least one root folder is required.
    */
    QStringList getFolders(U2OpStatus& os) override;
    QHash<U2Object, QString> getObjectFolders(U2OpStatus& os) override;
    void renameFolder(const QString& oldPath, const QString& newPath, U2OpStatus& os) override;

    /** Returns the folder's previous path if it is set or an empty string if folder has not been moved.
     *  The operation is not supported now to avoid the useless database upgrade;
     */
    QString getFolderPreviousPath(const QString& currentPath, U2OpStatus& os) override;

    /** Returns number of top-level U2Objects in folder */
    qint64 countObjects(const QString& folder, U2OpStatus& os) override;

    /** Lists database top-level objects of the specified type, starts with 'offset' and limits by 'count' */
    QList<U2DataId> getObjects(const QString& folder, qint64 offset, qint64 count, U2OpStatus& os) override;

    /**  Returns all folders this object must be shown in  */
    QStringList getObjectFolders(const U2DataId& objectId, U2OpStatus& os) override;

    /** Returns object rank of the given object */
    U2DbiObjectRank getObjectRank(const U2DataId& objectId, U2OpStatus& os) override;

    // Write methods for objects

    /**
        Removes the object from the database.
    */
    bool removeObject(const U2DataId& dataId, bool force, U2OpStatus& os) override;
    bool removeObject(const U2DataId& dataId, U2OpStatus& os) override;

    /**
        Removes collection of objects from the database
    */
    bool removeObjects(const QList<U2DataId>& dataIds, bool force, U2OpStatus& os) override;
    bool removeObjects(const QList<U2DataId>& dataIds, U2OpStatus& os) override;

    void renameObject(const U2DataId& id, const QString& newName, U2OpStatus& os) override;

    // Write methods for folders

    /** Creates folder in the database.
        If the specified path is already presented in the database, nothing will be done.
        It is not required that parent folders must exist, they are created automatically.
    */
    void createFolder(const QString& path, U2OpStatus& os) override;

    /** Removes folder. The folder must be existing path. Runs GC check for all objects in the folder */
    bool removeFolder(const QString& folder, U2OpStatus& os) override;

    /** Returns version of the folder.
        The folder version increases if new object(s)/subfolder(s) are added into this folder
        Note that if object(s)/folder(s) are added into one of the subfolders the folder version is not changed
    */
    qint64 getFolderLocalVersion(const QString& folder, U2OpStatus& os) override;

    /** Returns version of the folder that changes every time object(s)/folder(s) added
        to the specified folder or any of its child folders
    */
    qint64 getFolderGlobalVersion(const QString& folder, U2OpStatus& os) override;

    /** Updates object rank, e.g. a top-level object can be transmuted into a child object */
    void setObjectRank(const U2DataId& objectId, U2DbiObjectRank newRank, U2OpStatus& os) override;

    /** Adds objects to the specified folder.
    All objects must exist and have a top-level type.*/
    void addObjectsToFolder(const QList<U2DataId>& objectIds, const QString& toFolder, U2OpStatus& os) override;

    /** Moves objects between folders.
    'fromFolder' must be existing path containing all specified objects.
    'toFolder' must be existing path or empty string.
    If 'toFolder' is empty, removes the objects from 'fromFolder' and
    deletes non-top-level objects without parents, if any appear in the specified list.
    Otherwise, moves the specified objects between the specified folders, omitting duplicates.
    */
    void moveObjects(const QList<U2DataId>& objectIds, const QString& fromFolder, const QString& toFolder, U2OpStatus& os, bool saveFromFolder = false) override;

    QStringList restoreObjects(const QList<U2DataId>& objectIds, U2OpStatus& os) override;

    /** Undo the last update operation for the object. */
    void undo(const U2DataId& objId, U2OpStatus& os) override;

    /** Redo the last update operation for the object. */
    void redo(const U2DataId& objId, U2OpStatus& os) override;

    /** Returns "true", if there are steps to undo/redo modifications of the object*/
    bool canUndo(const U2DataId& objId, U2OpStatus& os) override;
    bool canRedo(const U2DataId& objId, U2OpStatus& os) override;

    //////////////////////////////////////////////////////////////////////////
    // Helper methods

    /**
        Creates database entry for 'object'. Puts object into the 'folder'.
        Retrieves type and name from 'object', sets its id and version.

        Returns result object data id
    */
    U2DataId createObject(U2Object& object, const QString& folder, U2DbiObjectRank rank, U2OpStatus& os);

    /**
        Updates database entry for 'obj'.
        Does NOT increment the object version!!
    */
    void updateObject(U2Object& obj, U2OpStatus& os);

    /**
        Returns internal database folder id
        Fails if folder not found and 'mustExist' == true
    */
    static qint64 getFolderId(const QString& path, bool mustExist, DbRef* db, U2OpStatus& os);

    /** Increment object version count */
    static void incrementVersion(const U2DataId& id, DbRef* db, U2OpStatus& os);

    /** Returns version of the given object */
    qint64 getObjectVersion(const U2DataId& objectId, U2OpStatus& os) override;

    /** Specified whether modifications in object must be tracked or not */
    void setTrackModType(const U2DataId& objectId, U2TrackModType trackModType, U2OpStatus& os) override;

    /** Gets the trackMod value for the object */
    U2TrackModType getTrackModType(const U2DataId& objectId, U2OpStatus& os) override;

    /**
        Removes objects parent relation.
        If child object has no parents and is not top level
        it will be automatically removed if 'removeDeadChild' is true
    */
    void removeParent(const U2DataId& parentId, const U2DataId& childId, bool removeDeadChild, U2OpStatus& os);

    /** Adds a record, representing parent-child relationship between entities, to a DB */
    void setParent(const U2DataId& parentId, const U2DataId& childId, U2OpStatus& os) override;

    void initSqlSchema(U2OpStatus& os) override;

private:
    /** Updates database entry for 'obj'. It does not increment its version. */
    void updateObjectCore(U2Object& obj, U2OpStatus& os);

    /** Removes object from database, returns 'true' if object is completely erased */
    bool removeObjectImpl(const U2DataId& id, U2OpStatus& os);

    void removeObjectAttributes(const U2DataId& id, U2OpStatus& os);

    /** Removes all modification tracks and steps for the object */
    void removeObjectModHistory(const U2DataId& id, U2OpStatus& os);

    /** Updates versions */
    void onFolderUpdated(const QString& folder);

    /** Increments an object version by 1 */
    void incrementVersion(const U2DataId& id, U2OpStatus& os);

    /** Sets an object version to the specified value */
    void setVersion(const U2DataId& id, qint64 version, U2OpStatus& os);

    ///////////////////////////////////////////////////////////
    // Undo methods does not modify object version
    void undoUpdateObjectName(const U2DataId& id, const QByteArray& modDetails, U2OpStatus& os);

    // Redo methods parse the modification details and call the corresponding method
    // (i.e. change version, save further modSteps, etc.)
    void redoUpdateObjectName(const U2DataId& id, const QByteArray& modDetails, U2OpStatus& os);

    void removeObjectFromFolder(const U2DataId& id, const QString& folder, U2OpStatus& os);

    void removeObjectFromAllFolders(const U2DataId& id, U2OpStatus& os);

    void removeObjectSpecificData(const U2DataId& id, U2OpStatus& os);
};

class SQLiteCrossDatabaseReferenceDbi : public U2CrossDatabaseReferenceDbi, public SQLiteChildDBICommon {
public:
    SQLiteCrossDatabaseReferenceDbi(SQLiteDbi* dbi);

    /**
        Adds new remote object to database.
        Sets local object id assigned to the new value
        Requires: U2DbiFeature_WriteCrossDatabaseReferences
    */
    void createCrossReference(U2CrossDatabaseReference& reference, const QString& folder, U2OpStatus& os) override;

    /**
        Removes a DB representation of CrossDatabaseReference
    */
    void removeCrossReferenceData(const U2DataId& referenceId, U2OpStatus& os) override;

    /**
        Loads remote object information from DB
        Requires: U2DbiFeature_ReadCrossDatabaseReferences
    */
    U2CrossDatabaseReference getCrossReference(const U2DataId& objectId, U2OpStatus& os) override;

    /**
        Updates all fields of cross database reference object
        Requires: U2DbiFeature_WriteCrossDatabaseReferences
    */
    void updateCrossReference(const U2CrossDatabaseReference& reference, U2OpStatus& os) override;

    void initSqlSchema(U2OpStatus& os) override;
};

/** Helper class to track info about an object */
class U2FORMATS_EXPORT SQLiteModificationAction : public ModificationAction {
public:
    SQLiteModificationAction(SQLiteDbi* dbi, const U2DataId& masterObjId);

    /**
        Verifies if modification tracking is enabled for the object.
        If it is, gets the object version.
        If there are tracking steps with greater or equal version (e.g. left from "undo"), removes these records.
        Returns the type of modifications  tracking for the object.
     */
    U2TrackModType prepare(U2OpStatus& os) override;

    /**
        Adds the object ID to the object IDs set.
        If tracking is enabled, adds a new single step to the list.
     */
    void addModification(const U2DataId& objId, qint64 modType, const QByteArray& modDetails, U2OpStatus& os) override;

    /**
        If tracking is enabled, creates modification steps in the database.
        Increments version of all objects in the set.
     */
    void complete(U2OpStatus& os) override;

private:
    SQLiteDbi* getDbi() const;
};

class SQLiteObjectDbiUtils {
public:
    /**
     * Updates the object name and increments the version.
     * The changes are tracked if it is needed.
     * Applies all changes to @object too.
     */
    static void renameObject(SQLiteDbi* dbi, U2Object& object, const QString& newName, U2OpStatus& os);
    static void renameObject(SQLiteModificationAction& updateAction, SQLiteDbi* dbi, U2Object& object, const QString& newName, U2OpStatus& os);
};

}  // namespace U2
