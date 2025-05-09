/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "ProjectViewModel.h"

#include <U2Core/AppContext.h>
#include <U2Core/BunchMimeData.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/ImportDocumentToDatabaseTask.h>
#include <U2Core/ImportObjectToDatabaseTask.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/MimeDataIterator.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/Timer.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/ObjectViewModel.h>

#include "ConnectionHelper.h"
#include "Notification.h"
#include "ProjectUtils.h"

namespace U2 {

const QString ProjectViewModel::MODIFIED_ITEM_COLOR = "#0032a0";

ProjectViewModel::ProjectViewModel(const ProjectTreeControllerModeSettings& settings, QObject* parent)
    : QAbstractItemModel(parent), settings(settings) {
}

void ProjectViewModel::updateSettings(const ProjectTreeControllerModeSettings& newSettings) {
    settings = newSettings;
}

void ProjectViewModel::updateData(const QModelIndex& index) {
    emit dataChanged(index, index);
}

int ProjectViewModel::columnCount(const QModelIndex& /*parent*/) const {
    return 1;
}

QVariant ProjectViewModel::data(const QModelIndex& index, int role) const {
    CHECK(index.isValid(), QVariant());

    switch (itemType(index)) {
        case DOCUMENT: {
            Document* doc = toDocument(index);
            SAFE_POINT(doc != nullptr, "NULL document", QVariant());
            return data(doc, role);
        }
        case FOLDER: {
            Folder* folder = toFolder(index);
            SAFE_POINT(folder != nullptr, "NULL folder", QVariant());
            return data(folder, role);
        }
        case OBJECT: {
            GObject* obj = toObject(index);
            SAFE_POINT(obj != nullptr, "NULL object", QVariant());
            return data(obj, role);
        }
        default:
            FAIL("Unexpected item type", QVariant());
    }
}

bool ProjectViewModel::setData(const QModelIndex& index, const QVariant& value, int /*role*/) {
    CHECK(index.isValid(), false);
    CHECK(value.isValid(), false);

    const QString newName = value.toString();
    CHECK(!newName.isEmpty(), false);

    QModelIndex newItemIndex;
    switch (itemType(index)) {
        case DOCUMENT:
            FAIL("Document cannot be renamed!", false);
        case FOLDER:
            newItemIndex = setFolderData(toFolder(index), newName);
            break;
        case OBJECT:
            newItemIndex = setObjectData(toObject(index), newName);
            break;
        default:
            FAIL("Unexpected project item type", false);
    }
    if (!newItemIndex.isValid()) {
        return false;
    }
    emit si_projectItemRenamed(newItemIndex);

    return true;
}

QModelIndex ProjectViewModel::setObjectData(GObject* obj, const QString& newName) {
    SAFE_POINT(obj != nullptr, "Invalid object detected", QModelIndex());
    CHECK(newName != obj->getGObjectName(), QModelIndex());
    obj->setGObjectName(newName);

    Document* doc = obj->getDocument();
    const QString objPath = getObjectFolder(doc, obj);
    removeObject(doc, obj);
    insertObject(doc, obj, objPath);
    return getIndexForObject(obj);
}

QModelIndex ProjectViewModel::setFolderData(Folder* folder, const QString& newName) {
    SAFE_POINT(folder != nullptr, "Invalid folder detected", QModelIndex());

    Document* doc = folder->getDocument();
    SAFE_POINT(doc != nullptr, "Invalid document detected", QModelIndex());
    const QString parentPath = folder->getParentPath();
    const QString newPath = U2ObjectDbi::ROOT_FOLDER == parentPath ? parentPath + newName : parentPath + U2ObjectDbi::PATH_SEP + newName;

    const QString oldPath = folder->getFolderPath();
    CHECK(newPath != oldPath, QModelIndex());

    renameFolder(doc, oldPath, newPath);
    return getIndexForPath(doc, newPath);
}

QModelIndex ProjectViewModel::index(int row, int column, const QModelIndex& parent) const {
    CHECK(parent.isValid() || row < rowCount(parent), QModelIndex());
    if (!parent.isValid()) {
        return getTopLevelItemIndex(row, column);
    }

    switch (itemType(parent)) {
        case DOCUMENT: {
            Document* doc = toDocument(parent);
            SAFE_POINT(doc != nullptr, "NULL document", QModelIndex());
            QList<Folder*> subFolders = folders[doc]->getSubFolders(U2ObjectDbi::ROOT_FOLDER);
            QList<GObject*> subObjects = folders[doc]->getObjects(U2ObjectDbi::ROOT_FOLDER);
            if (row < subFolders.size()) {
                return createIndex(row, column, subFolders[row]);
            }
            SAFE_POINT(row < subFolders.size() + subObjects.size(), "Out of range object number", QModelIndex());
            return createIndex(row, column, subObjects[row - subFolders.size()]);
        }
        case FOLDER: {
            Folder* folder = toFolder(parent);
            SAFE_POINT(folder != nullptr, "NULL folder", QModelIndex());
            QList<Folder*> subFolders = folders[folder->getDocument()]->getSubFolders(folder->getFolderPath());
            QList<GObject*> subObjects = folders[folder->getDocument()]->getObjects(folder->getFolderPath());
            if (row < subFolders.size()) {
                return createIndex(row, column, subFolders[row]);
            }
            SAFE_POINT(row < subFolders.size() + subObjects.size(), "Out of range object number", QModelIndex());
            return createIndex(row, column, subObjects[row - subFolders.size()]);
        }
        default:
            FAIL("Unexpected item type", QModelIndex());
    }
}

QModelIndex ProjectViewModel::parent(const QModelIndex& index) const {
    CHECK(index.isValid(), QModelIndex());

    switch (itemType(index)) {
        case DOCUMENT: {
            return QModelIndex();  //  Documents are top level items
        }
        case FOLDER: {
            Folder* folder = toFolder(index);
            SAFE_POINT(folder != nullptr, "NULL folder", QModelIndex());

            return getIndexForPath(folder->getDocument(), DocumentFolders::getParentFolder(folder->getFolderPath()));
        }
        case OBJECT: {
            GObject* obj = toObject(index);
            SAFE_POINT(obj != nullptr, "NULL object", QModelIndex());

            Document* doc = getObjectDocument(obj);
            SAFE_POINT(doc != nullptr, "NULL document", QModelIndex());

            QString parentPath = folders[doc]->getObjectFolder(obj);
            return getIndexForPath(doc, parentPath);
        }
        default:
            FAIL("Unexpected item type", QModelIndex());
    }
}

int ProjectViewModel::rowCount(const QModelIndex& parent) const {
    if (!parent.isValid()) {
        return getTopLevelItemsCount();
    }

    switch (itemType(parent)) {
        case DOCUMENT: {
            Document* doc = toDocument(parent);
            SAFE_POINT(doc != nullptr, "NULL document", 0);
            return getChildrenCount(doc, U2ObjectDbi::ROOT_FOLDER);
        }
        case FOLDER: {
            Folder* folder = toFolder(parent);
            SAFE_POINT(folder != nullptr, "NULL folder", 0);
            return getChildrenCount(folder->getDocument(), folder->getFolderPath());
        }
        case OBJECT: {
            return 0;
        }
        default:
            FAIL("Unexpected item type", 0);
    }
}

Qt::ItemFlags ProjectViewModel::flags(const QModelIndex& index) const {
    Qt::ItemFlags result = QAbstractItemModel::flags(index);
    CHECK(index.isValid(), result);

    switch (itemType(index)) {
        case DOCUMENT: {
            Document* doc = toDocument(index);
            SAFE_POINT(doc != nullptr, "NULL document", result);
            if (isDropEnabled(doc)) {
                result |= Qt::ItemIsDropEnabled;
            }
            result |= Qt::ItemIsDragEnabled;
            return result;
        }
        case FOLDER: {
            Folder* folder = toFolder(index);
            SAFE_POINT(folder != nullptr, "NULL folder", result);
            Document* doc = folder->getDocument();
            if (isDropEnabled(doc)) {
                result |= Qt::ItemIsDropEnabled;
            }
            result |= Qt::ItemIsDragEnabled;
            if (isWritableDoc(doc)) {
                result |= Qt::ItemIsEditable;
            }
            return result;
        }
        case OBJECT: {
            GObject* obj = toObject(index);
            SAFE_POINT(obj != nullptr, "NULL object", result);
            Document* doc = obj->getDocument();
            if ((GObjectTypes::UNLOADED == obj->getGObjectType()) && !settings.allowSelectUnloaded) {
                result &= ~QFlags<Qt::ItemFlag>(Qt::ItemIsEnabled);
            } else if (isWritableDoc(doc)) {
                result |= QFlags<Qt::ItemFlag>(Qt::ItemIsEditable);
            }
            if (isDropEnabled(obj->getDocument())) {
                result |= Qt::ItemIsDropEnabled;
            }
            result |= Qt::ItemIsDragEnabled;
            return result;
        }
        default:
            FAIL("Unexpected item type", result);
    }
}

QMimeData* ProjectViewModel::mimeData(const QModelIndexList& indexes) const {
    QList<QPointer<Document>> docs;
    QList<Folder> folders;
    QList<QPointer<GObject>> objects;

    foreach (const QModelIndex& index, indexes) {
        switch (itemType(index)) {
            case DOCUMENT:
                docs << toDocument(index);
                break;
            case FOLDER:
                folders << *toFolder(index);
                break;
            case OBJECT:
                objects << toObject(index);
                break;
            default:
                FAIL("Unexpected item type", nullptr);
        }
    }

    if ((1 == objects.size()) && docs.isEmpty() && folders.isEmpty()) {
        return new GObjectMimeData(objects.first().data());
    } else if ((1 == docs.size()) && objects.isEmpty() && folders.isEmpty()) {
        return new DocumentMimeData(docs.first().data());
    } else if ((1 == folders.size()) && objects.isEmpty() && docs.isEmpty()) {
        return new FolderMimeData(folders.first());
    } else {
        auto bmd = new BunchMimeData();
        bmd->objects = objects;
        bmd->folders = folders;
        return bmd;
    }
}

QStringList ProjectViewModel::mimeTypes() const {
    QStringList result;
    result << GObjectMimeData::MIME_TYPE;
    result << FolderMimeData::MIME_TYPE;
    result << DocumentMimeData::MIME_TYPE;
    result << BunchMimeData::MIME_TYPE;
    return result;
}

bool ProjectViewModel::dropMimeData(const QMimeData* data, Qt::DropAction /*action*/, int row, int /*column*/, const QModelIndex& parent) {
    CHECK(parent.isValid(), false);
    Folder target = getDropFolder(parent);
    const QString folderPath = target.getFolderPath();
    Document* targetDoc = target.getDocument();
    SAFE_POINT(targetDoc != nullptr, "NULL document", false);
    CHECK(!targetDoc->isStateLocked(), false);
    SAFE_POINT(row == -1, "Wrong insertion row", false);

    MimeDataIterator iter(data);

    while (iter.hasNextObject()) {
        dropObject(iter.nextObject(), targetDoc, folderPath);
    }

    while (iter.hasNextFolder()) {
        dropFolder(iter.nextFolder(), targetDoc, folderPath);
    }

    while (iter.hasNextDocument()) {
        dropDocument(iter.nextDocument(), targetDoc, folderPath);
    }

    return true;
}

Qt::DropActions ProjectViewModel::supportedDropActions() const {
    return Qt::CopyAction | Qt::MoveAction;
}

void ProjectViewModel::addDocument(Document* doc) {
    auto f = new DocumentFolders;
    U2OpStatus2Log os;
    f->init(doc, os);
    CHECK_OP(os, );

    int newRow = beforeInsertDocument(doc);
    docs << doc;
    folders[doc] = f;
    afterInsert(newRow);

    justAddedDocs.insert(doc);

    connectDocument(doc);

    connect(doc, SIGNAL(si_objectAdded(GObject*)), SLOT(sl_objectAdded(GObject*)));
    connect(doc, SIGNAL(si_objectRemoved(GObject*)), SLOT(sl_objectRemoved(GObject*)));
}

void ProjectViewModel::removeDocument(Document* doc) {
    disconnectDocument(doc);
    int row = beforeRemoveDocument(doc);
    docs.removeAll(doc);
    delete folders[doc];
    folders.remove(doc);
    afterRemove(row);
}

void ProjectViewModel::findFoldersDiff(QStringList oldFolders, QStringList newFolders, QStringList& added, QStringList& deleted) {
    oldFolders.sort();
    newFolders.sort();

    QStringList::ConstIterator oldI = oldFolders.constBegin();
    QStringList::ConstIterator newI = newFolders.constBegin();

    while (oldI != oldFolders.constEnd() || newI != newFolders.constEnd()) {
        if (oldI == oldFolders.constEnd()) {  // There are no more old folders. Remaining new folders are added
            added << *newI;
            newI++;
        } else if (newI == newFolders.constEnd()) {  // There are no more new folders. Remaining old folders are deleted
            deleted << *oldI;
            oldI++;
        } else if (*oldI == *newI) {
            oldI++;
            newI++;
        } else if (*oldI < *newI) {
            deleted << *oldI;
            oldI++;
        } else {
            added << *newI;
            newI++;
        }
    }
}

void ProjectViewModel::addToIgnoreObjFilter(Document* doc, const U2DataId& objId) {
    SAFE_POINT(folders.contains(doc), "Unknown document", );
    folders[doc]->addIgnoredObject(objId);
}

void ProjectViewModel::addToIgnoreFolderFilter(Document* doc, const QString& folderPath) {
    SAFE_POINT(folders.contains(doc), "Unknown document", );
    folders[doc]->addIgnoredFolder(folderPath);
}

void ProjectViewModel::excludeFromObjIgnoreFilter(Document* doc, const QSet<U2DataId>& ids) {
    SAFE_POINT(folders.contains(doc), "Unknown document", );
    folders[doc]->excludeFromObjFilter(ids);
}

void ProjectViewModel::excludeFromFolderIgnoreFilter(Document* doc, const QSet<QString>& paths) {
    SAFE_POINT(folders.contains(doc), "Unknown document", );
    folders[doc]->excludeFromFolderFilter(paths);
}

void ProjectViewModel::moveObject(Document* doc, GObject* obj, const QString& newFolderPath) {
    SAFE_POINT(doc != nullptr, "NULL document", );
    SAFE_POINT(folders.contains(doc), "Unknown document", );

    const QString oldFolderPath = folders[doc]->getObjectFolder(obj);
    CHECK(oldFolderPath != newFolderPath, );

    U2OpStatus2Log os;
    DbiOperationsBlock opBlock(doc->getDbiRef(), os);
    CHECK_OP(os, );

    DbiConnection con(doc->getDbiRef(), os);
    CHECK_OP(os, );
    U2ObjectDbi* objDbi = con.dbi->getObjectDbi();

    // create folder if it does not exist
    DocumentFoldersUpdate lastUpdate = folders[doc]->getLastUpdate();
    if (!lastUpdate.folders.contains(newFolderPath)) {
        objDbi->createFolder(newFolderPath, os);
        CHECK_OP(os, );
        insertFolder(doc, newFolderPath);
    }

    // move object in model
    QList<U2DataId> objList;
    objList << obj->getEntityRef().entityId;
    objDbi->moveObjects(objList, oldFolderPath, newFolderPath, os, false);
    CHECK_OP(os, );

    // move object in view
    removeObject(doc, obj);

    insertObject(doc, obj, newFolderPath);

    emit si_documentContentChanged(doc);
}

QList<GObject*> ProjectViewModel::getFolderObjects(Document* doc, const QString& path) const {
    QList<GObject*> result;
    SAFE_POINT(doc != nullptr, "NULL document", result);
    SAFE_POINT(folders.contains(doc), "Unknown document", result);

    QStringList subFolders = folders[doc]->getAllSubFolders(path);
    subFolders << path;
    foreach (const QString& folder, subFolders) {
        result << folders[doc]->getObjectsNatural(folder);
    }
    return result;
}

QString ProjectViewModel::getObjectFolder(Document* doc, GObject* obj) const {
    SAFE_POINT(doc != nullptr, "NULL document", "");
    SAFE_POINT(folders.contains(doc), "Unknown document", "");
    return folders[doc]->getObjectFolder(obj);
}

namespace {

void rollNewFolderPath(QString& originalPath, U2ObjectDbi* oDbi, U2OpStatus& os) {
    const QStringList allFolders = oDbi->getFolders(os);
    SAFE_POINT_OP(os, );

    QString resultPath = originalPath;
    int sameFolderNameCount = 0;
    while (allFolders.contains(resultPath)) {
        resultPath = originalPath + QString(" (%1)").arg(++sameFolderNameCount);
    }
    originalPath = resultPath;
}

}  // namespace

void ProjectViewModel::createFolder(Document* doc, QString& path) {
    CHECK(doc != nullptr && folders.contains(doc), );

    U2OpStatus2Log os;
    DbiOperationsBlock opBlock(doc->getDbiRef(), os);
    CHECK_OP(os, );
    DbiConnection con(doc->getDbiRef(), os);
    CHECK_OP(os, );
    U2ObjectDbi* oDbi = con.dbi->getObjectDbi();

    QString resultNewPath = path;
    rollNewFolderPath(resultNewPath, oDbi, os);
    CHECK_OP(os, );

    con.dbi->getObjectDbi()->createFolder(resultNewPath, os);
    CHECK_OP(os, );

    path = resultNewPath;

    int newRow = beforeInsertPath(doc, path);
    folders[doc]->addFolder(path);
    afterInsert(newRow);
}

bool ProjectViewModel::renameFolderInDb(Document* doc, const QString& oldPath, QString& newPath) const {
    U2OpStatus2Log os;
    DbiOperationsBlock opBlock(doc->getDbiRef(), os);
    CHECK_OP(os, false);
    DbiConnection con(doc->getDbiRef(), os);
    CHECK_OP(os, false);
    U2ObjectDbi* oDbi = con.dbi->getObjectDbi();

    QString resultNewPath = newPath;
    rollNewFolderPath(resultNewPath, oDbi, os);
    CHECK_OP(os, false);

    oDbi->renameFolder(oldPath, resultNewPath, os);
    SAFE_POINT_OP(os, false);

    newPath = resultNewPath;

    return true;
}

bool ProjectViewModel::isFolderVisible(Document* doc) const {
    SAFE_POINT(doc != nullptr, "NULL document", false);
    SAFE_POINT(folders.contains(doc), "Unknown document", false);
    return true;
}

int ProjectViewModel::beforeInsertDocument(Document* /*doc*/) {
    int newRow = docs.size();
    beginInsertRows(QModelIndex(), newRow, newRow);
    return newRow;
}

int ProjectViewModel::beforeInsertPath(Document* doc, const QString& path) {
    SAFE_POINT(doc != nullptr, "NULL document", -1);
    SAFE_POINT(folders.contains(doc), "Unknown document", -1);
    CHECK(isFolderVisible(doc), -1);

    const int newRow = folders[doc]->getNewFolderRowInParent(path);
    CHECK(-1 != newRow, -1);
    beginInsertRows(getIndexForPath(doc, DocumentFolders::getParentFolder(path)), newRow, newRow);
    return newRow;
}

int ProjectViewModel::beforeInsertObject(Document* doc, GObject* obj, const QString& path) {
    int newRow = folders[doc]->getNewObjectRowInParent(obj, path);
    CHECK(newRow != -1, -1);
    beginInsertRows(getIndexForPath(doc, path), newRow, newRow);
    return newRow;
}

void ProjectViewModel::afterInsert(int newRow) {
    if (-1 != newRow) {
        endInsertRows();
    }
}

int ProjectViewModel::beforeRemoveDocument(Document* doc) {
    int row = docRow(doc);
    SAFE_POINT(-1 != row, "Unknown document", -1);

    beginRemoveRows(QModelIndex(), row, row);
    return row;
}

int ProjectViewModel::beforeRemovePath(Document* doc, const QString& path) {
    SAFE_POINT(doc != nullptr, "NULL document", -1);
    SAFE_POINT(folders.contains(doc), "Unknown document", -1);
    CHECK(isFolderVisible(doc), -1);

    const QModelIndex index = getIndexForPath(doc, path);
    int row = index.row();
    CHECK(-1 != row, -1);
    beginRemoveRows(index.parent(), row, row);
    return row;
}

int ProjectViewModel::beforeRemoveObject(Document* doc, GObject* obj) {
    QString path = folders[doc]->getObjectFolder(obj);
    int row = objectRow(obj);
    CHECK(-1 != row, -1);

    beginRemoveRows(getIndexForPath(doc, path), row, row);
    return row;
}

void ProjectViewModel::afterRemove(int row) {
    if (-1 != row) {
        endRemoveRows();
    }
}

bool ProjectViewModel::renameFolder(Document* doc, const QString& oldPath, const QString& newPath) {
    CHECK(doc != nullptr && folders.contains(doc) && folders[doc]->hasFolder(oldPath), false);

    QString resultNewPath = newPath;
    // 1. update content in DB
    renameFolderInDb(doc, oldPath, resultNewPath);

    // 2. update model
    DocumentFolders* docFolders = folders[doc];
    SAFE_POINT(!docFolders->hasFolder(resultNewPath), "The folder already exists", false);

    QStringList foldersToRename;
    foldersToRename << oldPath << docFolders->getAllSubFolders(oldPath);

    // 2.1 copy folder tree
    int newRow = beforeInsertPath(doc, resultNewPath);
    QStringList newFolderNames;  // cache new subfolder paths
    foreach (const QString& folderPrevPath, foldersToRename) {
        QString folderNewPath = folderPrevPath;
        folderNewPath.replace(0, oldPath.length(), resultNewPath);
        newFolderNames.append(folderNewPath);

        docFolders->addFolder(folderNewPath);
    }
    afterInsert(newRow);

    // 2.2 for each object: remove it from old folder, then add to new folder
    moveObjectsBetweenFolderTrees(doc, foldersToRename, newFolderNames);

    // 2.3 remove old folder along with its subfolders
    int row = beforeRemovePath(doc, oldPath);
    docFolders->removeFolder(oldPath);
    afterRemove(row);

    emit si_documentContentChanged(doc);

    return true;
}

void ProjectViewModel::moveObjectsBetweenFolderTrees(Document* doc, const QStringList& srcTree, const QStringList& dstTree) {
    DocumentFolders* docFolders = folders[doc];

    for (int i = 0, n = srcTree.size(); i < n; ++i) {
        const QString folderPrevPath = srcTree.at(i);
        const QString folderNewPath = dstTree.at(i);

        const QList<GObject*> objects = docFolders->getObjectsNatural(folderPrevPath);
        foreach (GObject* obj, objects) {
            removeObject(doc, obj);
            insertObject(doc, obj, folderNewPath);
        }
    }
}

QModelIndex ProjectViewModel::getIndexForDoc(Document* doc) const {
    SAFE_POINT(doc != nullptr, "NULL document", QModelIndex());
    int row = docRow(doc);
    SAFE_POINT(-1 != row, "Out of range row", QModelIndex());
    return createIndex(row, 0, doc);
}

QModelIndex ProjectViewModel::getIndexForPath(Document* doc, const QString& path) const {
    SAFE_POINT(doc != nullptr, "NULL document", QModelIndex());

    if (U2ObjectDbi::ROOT_FOLDER == path) {
        return getIndexForDoc(doc);
    } else {
        Folder* folder = folders[doc]->getFolder(path);
        SAFE_POINT(folder != nullptr, "NULL folder", QModelIndex());
        int row = folderRow(folder);
        SAFE_POINT(-1 != row, "Out of range row", QModelIndex());
        return createIndex(row, 0, folder);
    }
}

QModelIndex ProjectViewModel::getIndexForObject(GObject* obj) const {
    int row = objectRow(obj);
    CHECK(-1 != row, QModelIndex());
    return createIndex(row, 0, obj);
}

Document* ProjectViewModel::findDocument(const U2DbiRef& dbiRef) const {
    foreach (Document* doc, docs) {
        if (doc->getDbiRef() == dbiRef) {
            return doc;
        }
    }
    return nullptr;
}

void ProjectViewModel::insertFolder(Document* doc, const QString& path) {
    SAFE_POINT(doc != nullptr, "NULL document", );
    SAFE_POINT(folders.contains(doc), "Unknown document", );
    CHECK(!folders[doc]->hasFolder(path), );

    QString absentPath;
    {  // Find the path to the folder which is not in the model. It some parent of @path or @path itself
        const QStringList pathList = path.split(U2ObjectDbi::PATH_SEP, Qt::SkipEmptyParts);
        QString fullPath;
        foreach (const QString& folder, pathList) {
            fullPath += U2ObjectDbi::PATH_SEP + folder;
            if (folders[doc]->hasFolder(fullPath)) {
                continue;
            }
            absentPath = fullPath;
            break;
        }
    }
    SAFE_POINT(!absentPath.isEmpty(), "The folder is already inserted", );

    int newRow = beforeInsertPath(doc, absentPath);
    folders[doc]->addFolder(path);
    afterInsert(newRow);
}

void ProjectViewModel::removeFolder(Document* doc, const QString& path) {
    SAFE_POINT(doc != nullptr, "NULL document", );
    SAFE_POINT(folders.contains(doc), "Unknown document", );

    CHECK(!path.isEmpty(), );
    CHECK(U2ObjectDbi::ROOT_FOLDER != path, );
    CHECK(folders[doc]->hasFolder(path), );

    Folder* folder = folders[doc]->getFolder(path);
    SAFE_POINT(folder != nullptr, "NULL folder", );
    int row = beforeRemovePath(doc, path);
    folders[doc]->removeFolder(path);
    afterRemove(row);
}

void ProjectViewModel::insertObject(Document* doc, GObject* obj, const QString& path) {
    int newRow = beforeInsertObject(doc, obj, path);
    folders[doc]->addObject(obj, path);
    afterInsert(newRow);
}

void ProjectViewModel::removeObject(Document* doc, GObject* obj) {
    QString path = folders[doc]->getObjectFolder(obj);
    int row = beforeRemoveObject(doc, obj);
    folders[doc]->removeObject(obj, path);
    afterRemove(row);
}

ProjectViewModel::Type ProjectViewModel::itemType(const QModelIndex& index) {
    QObject* obj = toQObject(index);
    SAFE_POINT(obj != nullptr, "NULL QObject", DOCUMENT);

    if (qobject_cast<Document*>(obj) != nullptr) {
        return DOCUMENT;
    } else if (qobject_cast<Folder*>(obj) != nullptr) {
        return FOLDER;
    } else if (qobject_cast<GObject*>(obj) != nullptr) {
        return OBJECT;
    }
    FAIL("Unexpected data type", DOCUMENT);
}

QObject* ProjectViewModel::toQObject(const QModelIndex& index) {
    void* ptr = index.internalPointer();
    SAFE_POINT(ptr != nullptr, "Internal error. No index data", nullptr);
    return static_cast<QObject*>(ptr);
}

Document* ProjectViewModel::toDocument(const QModelIndex& index) {
    return qobject_cast<Document*>(toQObject(index));
}

Folder* ProjectViewModel::toFolder(const QModelIndex& index) {
    return qobject_cast<Folder*>(toQObject(index));
}

GObject* ProjectViewModel::toObject(const QModelIndex& index) {
    return qobject_cast<GObject*>(toQObject(index));
}

Document* ProjectViewModel::getObjectDocument(GObject* obj) const {
    Document* result = obj->getDocument();
    if (result == nullptr) {
        return qobject_cast<Document*>(sender());
    }
    return result;
}

int ProjectViewModel::getTopLevelItemsCount() const {
    return docs.size();
}

QModelIndex ProjectViewModel::getTopLevelItemIndex(int row, int column) const {
    SAFE_POINT(row < docs.size(), "Out of range document number", QModelIndex());
    return createIndex(row, column, docs[row]);
}

int ProjectViewModel::getChildrenCount(Document* doc, const QString& path) const {
    SAFE_POINT(doc != nullptr, "NULL document", 0);
    SAFE_POINT(folders.contains(doc), "Unknown document", 0);
    SAFE_POINT(folders[doc]->hasFolder(path), "Unknown folder path", 0);

    QList<Folder*> subFolders = folders[doc]->getSubFolders(path);
    QList<GObject*> subObjects = folders[doc]->getObjects(path);
    return subFolders.size() + subObjects.size();
}

QList<Folder*> ProjectViewModel::getSubfolders(Document* doc, const QString& path) const {
    SAFE_POINT(doc != nullptr, "NULL document", QList<Folder*>());
    SAFE_POINT(folders.contains(doc), "Unknown document", QList<Folder*>());
    SAFE_POINT(folders[doc]->hasFolder(path), "Unknown folder path", QList<Folder*>());

    return folders[doc]->getSubFolders(path);
}

int ProjectViewModel::docRow(Document* doc) const {
    return docs.indexOf(doc);
}

bool ProjectViewModel::hasDocument(Document* doc) const {
    return -1 != docRow(doc);
}

bool ProjectViewModel::hasObject(Document* doc, GObject* obj) const {
    CHECK(hasDocument(doc), false);
    CHECK(folders[doc]->hasObject(obj->getEntityRef().entityId), false);
    return true;
}

int ProjectViewModel::folderRow(Folder* subFolder) const {
    SAFE_POINT(U2ObjectDbi::ROOT_FOLDER != subFolder->getFolderPath(), "Unexpected folder path", -1);
    Document* doc = subFolder->getDocument();
    SAFE_POINT(doc != nullptr, "NULL document", -1);
    SAFE_POINT(folders.contains(doc), "Unknown document", -1);

    QString parentPath = DocumentFolders::getParentFolder(subFolder->getFolderPath());
    QList<Folder*> allSubFolders = folders[doc]->getSubFolders(parentPath);
    return allSubFolders.indexOf(subFolder);
}

int ProjectViewModel::objectRow(GObject* obj) const {
    Document* doc = getObjectDocument(obj);
    SAFE_POINT(doc != nullptr, "NULL document", -1);
    SAFE_POINT(folders.contains(doc), "Unknown document", -1);

    QString parentPath = folders[doc]->getObjectFolder(obj);
    QList<Folder*> subFolders = folders[doc]->getSubFolders(parentPath);
    QList<GObject*> subObjects = folders[doc]->getObjects(parentPath);

    int objRow = subObjects.indexOf(obj);
    SAFE_POINT(-1 != objRow, "Unknown object", -1);

    return subFolders.size() + objRow;
}

QVariant ProjectViewModel::data(Document* doc, int role) const {
    switch (role) {
        case Qt::ForegroundRole:
            return getDocumentTextColorData(doc);
        case Qt::FontRole:
            return getDocumentFontData(doc);
        case Qt::DisplayRole:
            return getDocumentDisplayData(doc);
        case Qt::DecorationRole:
            return getDocumentDecorationData(doc);
        case Qt::ToolTipRole:
            return getDocumentToolTipData(doc);
        default:
            return QVariant();
    }
}

QVariant ProjectViewModel::getDocumentTextColorData(Document* doc) const {
    if (doc->isModified()) {
        return QColor(MODIFIED_ITEM_COLOR);
    } else {
        return QVariant();
    }
}

QVariant ProjectViewModel::getDocumentFontData(Document* doc) const {
    const bool markedAsActive = settings.markActive && isActive(doc);
    if (markedAsActive) {
        return settings.activeFont;
    } else {
        return QVariant();
    }
}

QVariant ProjectViewModel::getDocumentDisplayData(Document* doc) const {
    QString text;
    if (!doc->isLoaded()) {
        LoadUnloadedDocumentTask* t = LoadUnloadedDocumentTask::findActiveLoadingTask(doc);
        if (t == nullptr) {
            text += ProjectViewModel::tr("[unloaded] ");
        } else {
            if (t->getProgress() == -1) {
                text += ProjectViewModel::tr("[loading] ");
            } else {
                text += ProjectViewModel::tr("[loading %1%] ").arg(t->getProgress());
            }
        }
    }
    return text + doc->getName();
}

QVariant ProjectViewModel::getDocumentDecorationData(Document* doc) const {
    static const QIcon roDocumentIcon(":/core/images/ro_document.png");
    static const QIcon documentIcon(":/core/images/document.png");

    bool showLockedIcon = doc->isStateLocked();
    if (!doc->isLoaded() && doc->getStateLocks().size() == 1 && doc->getDocumentModLock(DocumentModLock_UNLOADED_STATE) != nullptr) {
        showLockedIcon = false;
    }
    return showLockedIcon ? roDocumentIcon : documentIcon;
}

QVariant ProjectViewModel::getDocumentToolTipData(Document* doc) const {
    QString tooltip = doc->getURLString();
    if (doc->isStateLocked()) {
        tooltip.append("<br><br>").append(ProjectViewModel::tr("Locks:"));
        StateLockableItem* docContext = doc->getParentStateLockItem();
        if (docContext != nullptr && docContext->isStateLocked()) {
            tooltip.append("<br>&nbsp;*&nbsp;").append(ProjectViewModel::tr("Project is locked"));
        }
        foreach (StateLock* lock, doc->getStateLocks()) {
            if (!doc->isLoaded() && lock == doc->getDocumentModLock(DocumentModLock_FORMAT_AS_INSTANCE)) {
                continue;  // do not visualize some locks for unloaded document
            }
            tooltip.append("<br>&nbsp;*&nbsp;").append(lock->getUserDesc());
        }
    }
    return tooltip;
}

QVariant ProjectViewModel::data(Folder* folder, int role) const {
    switch (role) {
        case Qt::DecorationRole:
            return getFolderDecorationData();
        case Qt::DisplayRole:
        case Qt::EditRole:
            return folder->getFolderName();
        default:
            return QVariant();
    }
}

QVariant ProjectViewModel::getFolderDecorationData() const {
    return getIcon(QIcon(":U2Designer/images/directory.png"), true);
}

QVariant ProjectViewModel::data(GObject* obj, int role) const {
    Document* parentDoc = getObjectDocument(obj);
    SAFE_POINT(parentDoc != nullptr, "Invalid parent document detected!", QVariant());
    SAFE_POINT(folders.contains(parentDoc), "Unknown document", QVariant());

    switch (role) {
        case Qt::ForegroundRole:
            return getObjectTextColorData(obj);
        case Qt::FontRole:
            return getObjectFontData(obj, true);
        case Qt::ToolTipRole:
            return getObjectToolTipData(obj, parentDoc);
        case Qt::DisplayRole:
            return getObjectDisplayData(obj, parentDoc);
        case Qt::EditRole:
            return obj->getGObjectName();
        case Qt::DecorationRole:
            return getObjectDecorationData(obj, true);
        default:
            return {};
    }
}

QVariant ProjectViewModel::getObjectDisplayData(GObject* obj, Document* parentDoc) const {
    GObjectType objectType = obj->getGObjectType();
    bool isUnloaded = objectType == GObjectTypes::UNLOADED;
    if (isUnloaded) {
        objectType = qobject_cast<UnloadedObject*>(obj)->getLoadedObjectType();
    }
    QString text;
    const GObjectTypeInfo& ti = GObjectTypes::getTypeInfo(objectType);
    text += "[" + ti.treeSign + "] ";

    if (isUnloaded && parentDoc->getObjects().size() < ProjectUtils::MAX_OBJS_TO_SHOW_LOAD_PROGRESS) {
        LoadUnloadedDocumentTask* loadTask = LoadUnloadedDocumentTask::findActiveLoadingTask(parentDoc);
        if (loadTask != nullptr) {
            if (loadTask->getProgress() == -1) {
                text += ProjectViewModel::tr("[loading] ");
            } else {
                text += ProjectViewModel::tr("[loading %1%] ").arg(loadTask->getProgress());
            }
        }
    }

    text += obj->getGObjectName();

    ProjectTreeGroupMode groupMode = settings.groupMode;
    if (groupMode == ProjectTreeGroupMode_Flat) {
        text += " [" + parentDoc->getName() + "]";
    }
    return text;
}

QVariant ProjectViewModel::getObjectFontData(GObject* obj, bool itemIsEnabled) const {
    const bool markedAsActive = settings.markActive && isActive(obj) && itemIsEnabled;
    if (markedAsActive) {
        return settings.activeFont;
    } else {
        return {};
    }
}

QVariant ProjectViewModel::getObjectToolTipData(GObject* /*obj*/, Document* parentDoc) const {
    QString tooltip;
    // todo: make tooltip for object items
    if (ProjectTreeGroupMode_Flat == settings.groupMode) {
        tooltip.append(parentDoc->getURLString());
    }
    return tooltip;
}

QVariant ProjectViewModel::getObjectDecorationData(GObject* obj, bool itemIsEnabled) const {
    // There is a special case: circular sequence object should have an icon that differs from the standard sequence icon!
    if (obj->getGObjectType() == GObjectTypes::SEQUENCE) {
        auto seqObj = qobject_cast<U2SequenceObject*>(obj);
        SAFE_POINT(seqObj != nullptr, "Cannot cast GObject to U2SequenceObject", QVariant());
        if (seqObj->isCircular()) {
            const QIcon circIcon(":core/images/circular_seq.png");
            return getIcon(circIcon, itemIsEnabled);
        }
    }

    const GObjectTypeInfo& ti = GObjectTypes::getTypeInfo(obj->getGObjectType());
    const QIcon& icon = (obj->getGObjectModLock(GObjectModLock_IO) != nullptr ? ti.lockedIcon : ti.icon);
    return getIcon(icon, itemIsEnabled);
}

QVariant ProjectViewModel::getObjectTextColorData(GObject* obj) const {
    if (obj->isItemModified()) {
        return QColor(MODIFIED_ITEM_COLOR);
    } else {
        return {};
    }
}

bool ProjectViewModel::isActive(Document* doc) {
    GObjectViewWindow* w = GObjectViewUtils::getActiveObjectViewWindow();
    CHECK(w != nullptr, false);
    return w->getObjectView()->containsDocumentObjects(doc);
}

bool ProjectViewModel::isActive(GObject* obj) {
    GObjectViewWindow* w = GObjectViewUtils::getActiveObjectViewWindow();
    CHECK(w != nullptr, false);
    return w->getObjectView()->containsObject(obj);
}

QIcon ProjectViewModel::getIcon(const QIcon& icon, bool enabled) {
    const QList<QSize> sizes = icon.availableSizes();
    CHECK(sizes.size() > 0, icon);
    const QPixmap newPic = icon.pixmap(sizes.first(), enabled ? QIcon::Normal : QIcon::Disabled);
    return QIcon(newPic);
}

bool ProjectViewModel::isWritableDoc(const Document* doc) {
    CHECK(doc != nullptr, false);
    return !doc->isStateLocked();
}

bool ProjectViewModel::isDropEnabled(const Document*) {
    return false;  // Was available only shared DB mode.
}

bool ProjectViewModel::isAcceptableFolder(Document* targetDoc, const QString& targetFolderPath, const Folder& folder) {
    CHECK(folder.getDocument() != nullptr, false);

    if (folder.getDocument() == targetDoc) {
        QString srcPath = folder.getFolderPath();
        return (srcPath != targetFolderPath) && (Folder::getFolderParentPath(srcPath) != targetFolderPath) && !Folder::isSubFolder(srcPath, targetFolderPath);
    }
    return false;
}

void ProjectViewModel::connectDocument(Document* doc) {
    connect(doc, SIGNAL(si_loadedStateChanged()), SLOT(sl_documentLoadedStateChanged()));
    connect(doc, SIGNAL(si_modifiedStateChanged()), SLOT(sl_documentModifiedStateChanged()));
    connect(doc, SIGNAL(si_lockedStateChanged()), SLOT(sl_lockedStateChanged()));
    connect(doc, SIGNAL(si_urlChanged()), SLOT(sl_documentURLorNameChanged()));
    connect(doc, SIGNAL(si_nameChanged()), SLOT(sl_documentURLorNameChanged()));

    foreach (GObject* obj, doc->getObjects()) {
        connectGObject(obj);
    }
}

void ProjectViewModel::disconnectDocument(Document* doc) {
    disconnect(doc, SIGNAL(si_loadedStateChanged()), this, SLOT(sl_documentLoadedStateChanged()));
    disconnect(doc, SIGNAL(si_modifiedStateChanged()), this, SLOT(sl_documentModifiedStateChanged()));
    disconnect(doc, SIGNAL(si_lockedStateChanged()), this, SLOT(sl_lockedStateChanged()));
    disconnect(doc, SIGNAL(si_urlChanged()), this, SLOT(sl_documentURLorNameChanged()));
    disconnect(doc, SIGNAL(si_nameChanged()), this, SLOT(sl_documentURLorNameChanged()));

    foreach (GObject* obj, doc->getObjects()) {
        obj->disconnect(this);
    }

    Task* t = LoadUnloadedDocumentTask::findActiveLoadingTask(doc);
    CHECK(t != nullptr, );
    t->disconnect(this);
    t->cancel();
}

void ProjectViewModel::connectGObject(GObject* obj) {
    connect(obj, SIGNAL(si_modifiedStateChanged()), SLOT(sl_objectModifiedStateChanged()));
    connect(obj, &GObject::si_failedModifyObjectName, this, [obj]() {
        NotificationStack::addNotification(tr("The object has been renamed, but as far as the \"%1\" format could not store this information,"
                                              " name will not be saved to the corresponding file")
                                               .arg(obj->getDocument()->getDocumentFormatId()),
                                           NotificationType::Warning_Not);
    });
}

Folder ProjectViewModel::getDropFolder(const QModelIndex& index) const {
    Document* doc = nullptr;
    QString path;
    switch (itemType(index)) {
        case DOCUMENT:
            doc = toDocument(index);
            path = U2ObjectDbi::ROOT_FOLDER;
            break;
        case FOLDER: {
            Folder* folder = toFolder(index);
            SAFE_POINT(folder != nullptr, "NULL folder", Folder());
            doc = folder->getDocument();
            path = folder->getFolderPath();
            break;
        }
        case OBJECT: {
            GObject* obj = toObject(index);
            SAFE_POINT(obj != nullptr, "NULL object", Folder());
            doc = obj->getDocument();
            path = getObjectFolder(doc, obj);
            break;
        }
        default:
            FAIL("Unexpected item type", Folder());
    }
    return Folder(doc, path);
}

void ProjectViewModel::dropObject(GObject* obj, Document* targetDoc, const QString& targetFolderPath) {
    QString result = targetFolderPath;

    if (obj->getDocument() == targetDoc) {
        moveObject(targetDoc, obj, result);
        emit si_documentContentChanged(targetDoc);
    } else {
        auto task = new ImportObjectToDatabaseTask(obj, targetDoc->getDbiRef(), result);
        connect(task, SIGNAL(si_stateChanged()), SLOT(sl_objectImported()));
        AppContext::getTaskScheduler()->registerTopLevelTask(task);
    }
}

void ProjectViewModel::dropFolder(const Folder& folder, Document* targetDoc, const QString& targetFolderPath) {
    QString result;
    result = targetFolderPath;
    const QString actualDstPath = result;  // we can drop only to the 'Recycle Bin' folder but not to its subfolder
    CHECK(isAcceptableFolder(targetDoc, actualDstPath, folder), );

    QString newPath = Folder::createPath(actualDstPath, folder.getFolderName());
    renameFolder(targetDoc, folder.getFolderPath(), newPath);
    emit si_documentContentChanged(targetDoc);
}

void ProjectViewModel::dropDocument(Document* doc, Document* targetDoc, const QString& targetFolderPath) {
    CHECK(doc != targetDoc, );
    ImportToDatabaseOptions options;
    auto task = new ImportDocumentToDatabaseTask(doc, targetDoc->getDbiRef(), targetFolderPath, options);
    connect(task, SIGNAL(si_stateChanged()), SLOT(sl_documentImported()));
    AppContext::getTaskScheduler()->registerTopLevelTask(task);
}

void ProjectViewModel::sl_documentImported() {
    auto task = dynamic_cast<ImportDocumentToDatabaseTask*>(sender());
    CHECK(task != nullptr, );
    CHECK(task->isFinished(), );
    CHECK(!task->getStateInfo().isCoR(), );

    Document* doc = findDocument(task->getDstDbiRef());
    CHECK(doc != nullptr, );

    const QString resultPath = task->getDstFolder();

    if (!folders[doc]->hasFolder(resultPath)) {
        insertFolder(doc, resultPath);
    }
    foreach (GObject* importedObj, task->getImportedObjects()) {
        if (doc->getObjectById(importedObj->getEntityRef().entityId) == nullptr) {
            doc->addObject(importedObj);
            insertObject(doc, importedObj, resultPath);
        } else {  // object has been already detected on a previous merging phase
            delete importedObj;
        }
    }
    emit si_documentContentChanged(doc);
}

void ProjectViewModel::sl_objectImported() {
    auto task = dynamic_cast<ImportObjectToDatabaseTask*>(sender());
    CHECK(task != nullptr, );
    CHECK(task->isFinished(), );
    CHECK(!task->getStateInfo().isCoR(), );

    Document* doc = findDocument(task->getDbiRef());
    CHECK(doc != nullptr, );

    GObject* newObj = task->takeResult();
    CHECK(newObj != nullptr, );

    doc->addObject(newObj);
    insertObject(doc, newObj, task->getFolder());
    emit si_documentContentChanged(doc);
}

void ProjectViewModel::sl_objectAdded(GObject* obj) {
    Document* doc = getObjectDocument(obj);
    SAFE_POINT(doc != nullptr, "NULL document", );
    SAFE_POINT(folders.contains(doc), "Unknown document", );

    connectGObject(obj);

    if (!obj->isUnloaded() && folders[doc]->hasObject(obj->getEntityRef().entityId)) {
        // Object has been added during merging
        return;
    }

    insertObject(doc, obj, U2ObjectDbi::ROOT_FOLDER);
    emit si_modelChanged();
}

void ProjectViewModel::sl_objectRemoved(GObject* obj) {
    Document* doc = getObjectDocument(obj);
    SAFE_POINT(doc != nullptr, "NULL document", );
    SAFE_POINT(folders.contains(doc), "Unknown document", );

    removeObject(doc, obj);
    emit si_modelChanged();
}

void ProjectViewModel::sl_documentModifiedStateChanged() {
    auto doc = qobject_cast<Document*>(sender());
    SAFE_POINT(doc != nullptr, "NULL document", );
    SAFE_POINT(folders.contains(doc), "Unknown document", );

    QModelIndex idx = getIndexForDoc(doc);
    emit dataChanged(idx, idx);
    emit si_modelChanged();
}

void ProjectViewModel::sl_documentLoadedStateChanged() {
    auto doc = qobject_cast<Document*>(sender());
    SAFE_POINT(doc != nullptr, "NULL document", );
    SAFE_POINT(folders.contains(doc), "Unknown document", );

    if (doc->isLoaded()) {
        if (!justAddedDocs.contains(doc)) {
            connectDocument(doc);
        } else {
            justAddedDocs.remove(doc);  // this document has already been connected
        }
    } else {
        disconnectDocument(doc);
        connect(doc, SIGNAL(si_loadedStateChanged()), SLOT(sl_documentLoadedStateChanged()));
    }

    QModelIndex idx = getIndexForDoc(doc);
    emit dataChanged(idx, idx);
    emit si_modelChanged();
}

void ProjectViewModel::sl_lockedStateChanged() {
    auto doc = qobject_cast<Document*>(sender());
    SAFE_POINT(doc != nullptr, "NULL document", );
    SAFE_POINT(folders.contains(doc), "Unknown document", );

    if (settings.readOnlyFilter != TriState_Unknown) {
        bool remove = (doc->isStateLocked() && settings.readOnlyFilter == TriState_Yes) || (!doc->isStateLocked() && settings.readOnlyFilter == TriState_No);
        if (remove) {
            disconnectDocument(doc);
        } else {
            connectDocument(doc);
        }
    }

    QModelIndex idx = getIndexForDoc(doc);
    emit dataChanged(idx, idx);
    emit si_modelChanged();
}

void ProjectViewModel::sl_documentURLorNameChanged() {
    auto doc = qobject_cast<Document*>(sender());
    SAFE_POINT(doc != nullptr, "NULL document", );
    SAFE_POINT(folders.contains(doc), "Unknown document", );

    QModelIndex idx = getIndexForDoc(doc);
    emit dataChanged(idx, idx);
    emit si_modelChanged();
}

void ProjectViewModel::sl_objectModifiedStateChanged() {
    auto obj = qobject_cast<GObject*>(sender());
    SAFE_POINT(obj != nullptr, "NULL object", );
    QModelIndex idx = getIndexForObject(obj);
    emit dataChanged(idx, idx);
    emit si_modelChanged();
}

}  // namespace U2
