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

#ifndef _U2_PROJECTVIEWMODEL_H_
#define _U2_PROJECTVIEWMODEL_H_

#include <QAbstractItemModel>

#include <U2Core/Folder.h>
#include <U2Core/ProjectTreeControllerModeSettings.h>

#include "DocumentFolders.h"

namespace U2 {

class Document;
class GObject;
class Project;

class U2GUI_EXPORT ProjectViewModel : public QAbstractItemModel {
    Q_OBJECT
public:
    enum Type {
        DOCUMENT,
        FOLDER,
        OBJECT
    };

    ProjectViewModel(const ProjectTreeControllerModeSettings &settings, QObject *parent);

    void updateSettings(const ProjectTreeControllerModeSettings &settings);
    void updateData(const QModelIndex &index);

    // QAbstractItemModel
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QMimeData * mimeData(const QModelIndexList &indexes) const;
    QStringList mimeTypes() const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
    Qt::DropActions supportedDropActions() const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    void addDocument(Document *doc);
    void removeDocument(Document *doc);
    void merge(Document *doc, const DocumentFoldersUpdate &update);
    bool hasDocument(Document *doc) const;
    bool hasObject(Document *doc, GObject *obj) const;

    void moveObject(Document *doc, GObject *obj, const QString &newFolder);
    bool restoreObjectItemFromRecycleBin(Document *doc, GObject *obj);
    bool restoreFolderItemFromRecycleBin(Document *doc, const QString &oldPath);
    QList<GObject *> getFolderObjects(Document *doc, const QString &path) const;
    QList<Folder *> getSubfolders(Document *doc, const QString &path) const;
    void removeFolder(Document *doc, const QString &path);

    QString getObjectFolder(Document *doc, GObject *obj) const;

    void createFolder(Document *doc, QString &path);
    bool renameFolder(Document *doc, const QString &oldPath, const QString &newPath);

    QModelIndex getIndexForDoc(Document *doc) const;
    QModelIndex getIndexForPath(Document *doc, const QString &path) const;
    QModelIndex getIndexForObject(GObject *obj) const;

    static Type itemType(const QModelIndex &index);
    static Document * toDocument(const QModelIndex &index);
    static Folder * toFolder(const QModelIndex &index);
    static GObject * toObject(const QModelIndex &index);

    void addToIgnoreObjFilter(Document *doc, const U2DataId &objId);
    void addToIgnoreFolderFilter(Document *doc, const QString &folderPath);

    void excludeFromObjIgnoreFilter(Document *doc, const QSet<U2DataId> &ids);
    void excludeFromFolderIgnoreFilter(Document *doc, const QSet<QString> &paths);

signals:
    void si_modelChanged();
    void si_documentContentChanged(Document *doc);
    void si_projectItemRenamed(const QModelIndex &index);

private slots:
    void sl_objectAdded(GObject *obj);
    void sl_objectRemoved(GObject *obj);

    void sl_documentModifiedStateChanged();
    void sl_documentLoadedStateChanged();
    void sl_lockedStateChanged();
    void sl_documentURLorNameChanged();
    void sl_objectModifiedStateChanged();
    void sl_objectImported();
    void sl_documentImported();

private:
    static QObject * toQObject(const QModelIndex &index);
    static bool isActive(Document *doc);
    static bool isActive(GObject *obj);
    static QIcon getIcon(const QIcon &icon, bool enabled);
    static bool isWritableDoc(const Document *doc);
    static bool isDropEnabled(const Document *doc);
    static bool isAcceptableFolder(Document *targetDoc, const QString &targetFolderPath, const Folder &folder);
    /** NOTE: the method supposes that @oldFolders and @newFolders are sorted */
    static void findFoldersDiff(QStringList oldFolders, QStringList newFolders, QStringList &added, QStringList &deleted);

    Document * getObjectDocument(GObject *obj) const;

    int getTopLevelItemsCount() const;
    QModelIndex getTopLevelItemIndex(int row, int column) const;
    int getChildrenCount(Document *doc, const QString &path) const;
    int docRow(Document *doc) const;
    int folderRow(Folder *folder) const;
    int objectRow(GObject *obj) const;
    QVariant data(Document *doc, int role) const;
    QVariant data(Folder *folder, int role) const;
    QVariant data(GObject *obj, int role) const;

    Document * findDocument(const U2DbiRef &dbiRef) const;

    QVariant getDocumentTextColorData(Document *doc) const;
    QVariant getDocumentFontData(Document *doc) const;
    QVariant getDocumentDisplayData(Document *doc) const;
    QVariant getDocumentDecorationData(Document *doc) const;
    QVariant getDocumentToolTipData(Document *doc) const;

    QVariant getFolderDecorationData(Folder *folder) const;

    QVariant getObjectDisplayData(GObject *obj, Document *parentDoc) const;
    QVariant getObjectFontData(GObject *obj, bool itemIsEnabled) const;
    QVariant getObjectToolTipData(GObject *obj, Document *parentDoc) const;
    QVariant getObjectDecorationData(GObject *obj, bool itemIsEnabled) const;
    QVariant getObjectTextColorData(GObject *obj) const;

    QModelIndex setFolderData(Folder *folder, const QString &newName);
    QModelIndex setObjectData(GObject *obj, const QString &newName);

    void insertFolder(Document *doc, const QString &path);
    void insertFolderInRecycleBin(Document *doc, const QString &path);

    void insertObject(Document *doc, GObject *obj, const QString &path);
    void removeObject(Document *doc, GObject *obj);
    void moveObjectsBetweenFolderTrees(Document *doc, const QStringList &srcTree, const QStringList &dstTree);

    void connectDocument(Document *doc);
    void disconnectDocument(Document *doc);
    void connectGObject(GObject *obj);

    Folder getDropFolder(const QModelIndex &index) const;
    void dropObject(GObject *obj, Document *targetDoc, const QString &targetFolderPath);
    void dropFolder(const Folder &folder, Document *targetDoc, const QString &targetFolderPath);
    void dropDocument(Document *doc, Document *targetDoc, const QString &targetFolderPath);

    bool renameFolderInDb(Document *doc, const QString &oldPath, QString &newPath) const;
    bool isFolderVisible(Document *doc, const QString &path) const;

    int beforeInsertDocument(Document *doc);
    int beforeInsertPath(Document *doc, const QString &path);
    int beforeInsertObject(Document *doc, GObject *obj, const QString &path);
    void afterInsert(int newRow);

    int beforeRemoveDocument(Document *doc);
    int beforeRemovePath(Document *doc, const QString &path);
    int beforeRemoveObject(Document *doc, GObject *obj);
    void afterRemove(int row);

    QList<Document*> docs;
    QSet<Document *> justAddedDocs; // documents that have been added to a project but not loaded yet
    mutable QHash<Document*, DocumentFolders *> folders;
    ProjectTreeControllerModeSettings settings;

    static const QString MODIFIED_ITEM_COLOR;
};

} // U2

#endif // _U2_PROJECTVIEWMODEL_H_
