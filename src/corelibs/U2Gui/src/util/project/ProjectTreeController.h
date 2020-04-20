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

#ifndef _U2_PROJECTTREECONTROLLER_H_
#define _U2_PROJECTTREECONTROLLER_H_

#include <U2Core/DocumentSelection.h>
#include <U2Core/FolderSelection.h>
#include <U2Core/GObjectSelection.h>
#include <U2Core/ProjectTreeControllerModeSettings.h>

class QAbstractItemDelegate;
class QAction;
class QMenu;
class QModelIndex;
class QTreeView;

namespace U2 {

class EditableTreeView;
class GObjectView;
class MWMDIWindow;
class ProjectFilterProxyModel;
class ProjectViewFilterModel;
class ProjectUpdater;
class ProjectViewModel;
class Task;

class U2GUI_EXPORT ProjectTreeController : public QObject {
    Q_OBJECT
public:
    ProjectTreeController(EditableTreeView *tree, const ProjectTreeControllerModeSettings &settings, QObject *parent);
    ~ProjectTreeController();

    const DocumentSelection * getDocumentSelection() const;
    const GObjectSelection * getGObjectSelection() const;
    QList<Folder> getSelectedFolders() const;

    bool isObjectInRecycleBin(GObject *obj) const;
    bool isObjectInFolder(GObject *obj, const Folder &folder) const;
    const ProjectTreeControllerModeSettings & getModeSettings() const;
    void highlightItem(Document *doc);
    void refreshObject(GObject *object);
    QAction * getLoadSeletectedDocumentsAction() const;
    void updateSettings(const ProjectTreeControllerModeSettings &settings);
    QSet<Document*> getDocsInSelection(bool deriveFromObjects) const;

private slots:
    void sl_onDocumentAdded(Document *doc);
    void sl_onDocumentRemoved(Document *doc);
    void sl_mergeData();
    void sl_updateSelection();
    void sl_updateActions();
    void sl_doubleClicked(const QModelIndex &index);
    void sl_documentContentChanged(Document *doc);

    void sl_onCreateFolder();
    void sl_onAddObjectToSelectedDocument();
    void sl_onLoadSelectedDocuments();
    void sl_onUnloadSelectedDocuments();
    void sl_onContextMenuRequested(const QPoint &pos);
    void sl_onDocumentLoadedStateChanged();
    void sl_onToggleReadonly();
    void sl_onRemoveSelectedItems();
    void sl_onLockedStateChanged();
    void sl_onImportToDatabase();
    void sl_windowActivated(MWMDIWindow *w);
    void sl_windowDeactivated(MWMDIWindow *w);
    void sl_objectAddedToActiveView(GObjectView *w, GObject *obj);
    void sl_objectRemovedFromActiveView(GObjectView *w, GObject *obj);
    void sl_onResourceUserRegistered(const QString &res, Task *t);
    void sl_onResourceUserUnregistered(const QString &res, Task *t);
    void sl_onLoadingDocumentProgressChanged();
    void sl_onRename();
    void sl_onRestoreSelectedItems();
    void sl_onEmptyRecycleBin();
    void sl_onProjectItemRenamed(const QModelIndex &index);
    void sl_onObjRemovalTaskFinished();
    void sl_onFolderRemovalTaskFinished();

    void sl_filterGroupAdded(const QModelIndex &groupIndex);

signals:
    void si_onPopupMenuRequested(QMenu &popup);
    void si_doubleClicked(GObject *obj);
    void si_doubleClicked(Document *doc);
    void si_returnPressed(GObject *obj);
    void si_returnPressed(Document *doc);

    void si_filteringStarted();
    void si_filteringFinished();

private:
    // QObject
    bool eventFilter(QObject *o, QEvent *e);

    void setupActions();
    void connectDocument(Document *doc);
    void disconnectDocument(Document *doc);
    void connectToResourceTracker();
    void updateLoadingState(Document *doc);
    void runLoadDocumentTasks(const QList<Document*> &docs) const;
    void removeItems(const QList<Document*> &docs, QList<Folder> folders, QList<GObject *> objs);
    bool removeObjects(const QList<GObject*> &objs, const QList<Document*> &excludedDocs, const QList<Folder> &excludedFolders, bool removeFromDbi);
    bool removeFolders(const QList<Folder> &folders, const QList<Document*> &excludedDocs);
    void removeDocuments(const QList<Document*> &docs);
    void updateObjectActiveStateVisual(GObject *obj);
    bool canCreateSubFolder() const;
    bool canRenameFolder() const;
    void restoreSelectedObjects();
    void restoreSelectedFolders();

    void updateAddObjectAction();
    void updateImportToDbAction();
    void updateReadOnlyFlagActions();
    void updateRenameAction();
    void updateLoadDocumentActions();
    QModelIndex getIndexForDoc(Document *doc) const;
    QModelIndex getOriginalModelIndex(const QModelIndex &index) const;
    
    // auto expands/collapses document node based on loaded state & current documents count in project
    void handleAutoExpand(Document* doc);

    // after folders or objects has been removed from Project View,
    // they can still present in the database during the next merge procedure (due to their large sizes).
    // Therefore when delete task is created, corresponding objects and folders are added to some ignore filters
    // which keep them untouched by merging. After the task has finished the objects/folders have to be
    // removed from filters.
    //
    // Two methods below store removed objects and folders respectively in order to remove them on delete task finish.
    void startTrackingRemovedObjects(Task *deleteTask, const QHash<GObject *, Document *> &objs2Docs);
    void startTrackingRemovedFolders(Task *deleteTask, const QList<Folder> &folders);

    static bool isObjectRemovable(GObject *object);
    static bool isFolderRemovable(const Folder &folder);
    bool isAnyObjectInRecycleBin(const QList <GObject *> &objects);
    static bool isAnyFolderInRecycleBin(const QList <Folder> &folders);
    static void excludeUnremovableObjectsFromList(QList<GObject *> &objects);
    static void excludeUnremovableFoldersFromList(QList<Folder> &folders);
    static bool isSubFolder(const QList<Folder> &folders, const Folder &expectedSubFolder, bool trueIfSamePath);

    EditableTreeView *tree;
    ProjectTreeControllerModeSettings settings;
    ProjectUpdater *updater;
    ProjectViewModel *model;
    ProjectViewFilterModel *filterModel;
    QAbstractItemDelegate *previousItemDelegate;
    ProjectFilterProxyModel *proxyModel;

    // Actions
    QAction *createFolderAction;
    QAction *addObjectToDocumentAction;
    QAction *loadSelectedDocumentsAction;
    QAction *unloadSelectedDocumentsAction;
    QAction *addReadonlyFlagAction;
    QAction *renameAction;
    QAction *removeReadonlyFlagAction;
    QAction *removeSelectedItemsAction;
    QAction *importToDatabaseAction;
    QAction *restoreSelectedItemsAction;
    QAction *emptyRecycleBinAction;

    DocumentSelection documentSelection;
    FolderSelection folderSelection;
    GObjectSelection objectSelection;
    GObjectView *markActiveView;
    GObject *objectIsBeingRecycled;

    QHash<Task *, QHash<Document *, QSet<U2DataId> > > task2ObjectsBeingDeleted;
    QHash<Task *, QHash<Document *, QSet<QString> > > task2FoldersBeingDeleted;
};

} // U2

#endif // _U2_PROJECTTREECONTROLLER_H_
