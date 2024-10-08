/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "CloudStorageDockWidget.h"

#include <QContextMenuEvent>
#include <QDesktopServices>
#include <QHeaderView>
#include <QIcon>
#include <QInputDialog>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QToolBar>
#include <QVBoxLayout>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/MainWindow.h>
#include <U2Gui/U2FileDialog.h>

#include "CloudStorageService.h"
#include "WorkspaceService.h"

namespace U2 {

constexpr const char* CLOUD_STORAGE_LAST_OPENED_DOWNLOAD_DIR = "CloudStorageDownloadDir";
constexpr const char* CLOUD_STORAGE_LAST_OPENED_UPLOAD_DIR = "CloudStorageUploadDir";

constexpr qint64 USER_DATA_SESSION_LOCAL_ID = Qt::UserRole + 1;
constexpr qint64 USER_DATA_SIZE = Qt::UserRole + 2;
constexpr qint64 USER_DATA_PATH = Qt::UserRole + 3;
constexpr qint64 USER_DATA_IS_FOLDER = Qt::UserRole + 4;

static bool isLess(QStandardItem* a, QStandardItem* b) {
    return a->data(USER_DATA_IS_FOLDER) != b->data(USER_DATA_IS_FOLDER)
               ? a->data(USER_DATA_IS_FOLDER).toBool()
               : a->text() < b->text();
}

static void updateModel(QTreeView* tree,
                        QStandardItem* parentItem,
                        const CloudStorageEntry& entry,
                        QList<QStandardItem*>& expandedItems,
                        QSet<QStandardItem*>& deletedItems) {
    QMap<qint64, QStandardItem*> childrenMap;
    for (int i = 0; i < parentItem->rowCount(); i++) {
        QStandardItem* childItem = parentItem->child(i);
        auto childEntryKey = childItem->data(USER_DATA_SESSION_LOCAL_ID).toLongLong();
        childrenMap[childEntryKey] = childItem;
    }
    for (const CloudStorageEntry& childEntry : qAsConst(entry->children)) {
        QIcon icon(childEntry->isFolder ? ":U2Designer/images/directory.png" : ":core/images/document.png");
        auto childEntryKey = childEntry->sessionLocalId;
        if (childrenMap.contains(childEntryKey)) {
            QStandardItem* nameItem = childrenMap[childEntryKey];
            nameItem->setIcon(icon);
            nameItem->setText(childEntry->getName());
            nameItem->setData(childEntry->size, USER_DATA_SIZE);
            nameItem->setData(childEntry->isFolder, USER_DATA_IS_FOLDER);
            nameItem->setData(QVariant::fromValue(childEntry->path), USER_DATA_PATH);

            updateModel(tree, nameItem, childEntry, expandedItems, deletedItems);

            // Unregister it from the map, because all entries that left in the map will be removed from the view as unused.
            childrenMap.remove(childEntryKey);
        } else {
            auto nameItem = new QStandardItem(icon, childEntry->getName());
            nameItem->setData(childEntry->size, USER_DATA_SIZE);
            nameItem->setData(childEntry->isFolder, USER_DATA_IS_FOLDER);
            nameItem->setData(QVariant::fromValue(childEntry->path), USER_DATA_PATH);
            nameItem->setData(childEntry->sessionLocalId, USER_DATA_SESSION_LOCAL_ID);

            parentItem->appendRow(nameItem);

            updateModel(tree, nameItem, childEntry, expandedItems, deletedItems);
        }
    }

    for (auto it = childrenMap.constBegin(); it != childrenMap.constEnd(); ++it) {
        auto item = it.value();
        deletedItems.insert(item);
        parentItem->removeRow(item->row());
    }

    // Resort all children. Do not break expanded state.
    QList<QStandardItem*> sortedChildren;
    int rowCount = parentItem->rowCount();
    for (int i = 0; i < rowCount; i++) {
        auto child = parentItem->child(i);
        if (tree->isExpanded(child->index())) {
            expandedItems.append(child);
        }
        sortedChildren.push_back(parentItem->takeChild(i));
    }
    std::sort(sortedChildren.begin(), sortedChildren.end(), [](QStandardItem* a, QStandardItem* b) { return isLess(a, b); });
    for (int i = 0; i < rowCount; i++) {
        parentItem->setChild(i, sortedChildren[i]);
    }
}

static void updateTree(QTreeView* tree, const QStandardItemModel& model, const CloudStorageEntry& entry) {
    tree->setUpdatesEnabled(false);
    QModelIndexList selectedIndexes = tree->selectionModel()->selectedIndexes();
    QStandardItem* selectedItem = selectedIndexes.isEmpty() ? nullptr : model.itemFromIndex(selectedIndexes.first());

    QList<QStandardItem*> expandedItems;  // List of item that should be expanded after model update.
    QSet<QStandardItem*> deletedItems;  // List of items that were deleted during model update. Contains deleted pointers.
    updateModel(tree, model.invisibleRootItem(), entry, expandedItems, deletedItems);

    // Resorting in updateModel breaks expanded & selected states.
    // We will restore it from the root (last in the list) to leafs ( first in the list).
    for (int i = expandedItems.length(); --i >= 0;) {
        tree->setExpanded(expandedItems[i]->index(), true);
    }

    if (selectedItem != nullptr && !deletedItems.contains(selectedItem)) {
        tree->selectionModel()->select(selectedItem->index(), QItemSelectionModel::Select);
    }

    tree->setUpdatesEnabled(true);
    tree->update();
}

CloudStorageDockWidget::CloudStorageDockWidget(WorkspaceService* _workspaceService)
    : workspaceService(_workspaceService) {
    setObjectName(DOCK_CLOUD_STORAGE_VIEW);
    setWindowTitle(tr("Cloud Storage"));
    setWindowIcon(QIcon(":ugene/images/cloud_storage.svg"));

    stateLabel = new QLabel();
    stateLabel->setTextFormat(Qt::RichText);
    stateLabel->setOpenExternalLinks(false);
    stateLabel->setStyleSheet("background: white; padding: 10px;");

    treeView = new QTreeView();
    treeView->setModel(&treeViewModel);
    treeView->setVisible(false);
    treeView->setEditTriggers(QTreeView::NoEditTriggers);
    treeView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    treeView->setObjectName("cloudStorageTreeView");
    treeView->header()->hide();
    treeView->viewport()->installEventFilter(this);

    auto toolbar = new QToolBar();
    toolbar->setIconSize(QSize(20, 20));
    toolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);

    auto layout = new QVBoxLayout();
    layout->addWidget(toolbar);
    layout->addWidget(stateLabel);
    layout->addWidget(treeView);
    setLayout(layout);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    connect(workspaceService, &WorkspaceService::si_authenticationEvent, this, [this] {
        stateLabel->setVisible(true);
        treeView->setVisible(false);
        updateActionsState();
        treeViewModel.clear();
        treeViewModel.setHorizontalHeaderLabels({"Name"});
        treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
        updateStateLabelText();
    });

    connect(getCloudStorageService(), &CloudStorageService::si_storageStateChanged, this, [this](const CloudStorageEntry& rootEntry) {
        stateLabel->setVisible(false);
        treeView->setVisible(true);
        updateActionsState();
        uiLog.trace("CloudStorageDockWidget: Received new cloud storage state");
        updateTree(treeView, treeViewModel, rootEntry);
    });

    createDirAction = new QAction(QIcon(":ugene/images/new_folder.svg"), tr("New Folder"), this);
    createDirAction->setObjectName("cloudStorageCreateDirAction");
    createDirAction->setShortcut(QKeySequence(Qt::Key_Insert));
    createDirAction->setToolTip(tr("Create New Folder on Cloud Storage"));
    connect(createDirAction, &QAction::triggered, this, &CloudStorageDockWidget::createDir);
    treeView->addAction(createDirAction);

    deleteAction = new QAction(QIcon(":ugene/images/trash.svg"), tr("Delete"), this);
    deleteAction->setObjectName("cloudStorageDeleteAction");
    deleteAction->setShortcut(QKeySequence::Delete);
    deleteAction->setToolTip(tr("Delete selected file from Cloud Storage"));
    connect(deleteAction, &QAction::triggered, this, &CloudStorageDockWidget::deleteItem);
    treeView->addAction(deleteAction);

    renameAction = new QAction(QIcon(":ugene/images/file_rename.svg"), tr("Rename"), this);
    renameAction->setObjectName("cloudStorageRenameAction");
    renameAction->setToolTip(tr("Rename File on Cloud Storage"));
    renameAction->setShortcut(QKeySequence(Qt::Key_F2));
    connect(renameAction, &QAction::triggered, this, &CloudStorageDockWidget::renameItem);
    treeView->addAction(renameAction);

    downloadAction = new QAction(QIcon(":ugene/images/file_download.svg"), tr("Download"), this);
    downloadAction->setObjectName("cloudStorageDownloadAction");
    downloadAction->setToolTip(tr("Download File from Cloud Storage"));
    connect(downloadAction, &QAction::triggered, this, &CloudStorageDockWidget::downloadItem);
    treeView->addAction(downloadAction);

    uploadAction = new QAction(QIcon(":ugene/images/file_upload.svg"), tr("Upload"), this);
    uploadAction->setObjectName("cloudStorageUploadAction");
    uploadAction->setToolTip(tr("Upload File to Cloud Storage"));
    connect(uploadAction, &QAction::triggered, this, &CloudStorageDockWidget::uploadItem);
    treeView->addAction(uploadAction);

    openWebWorkspaceAction = new QAction(QIcon(":ugene/images/web_link.svg"), tr("Open web interface"));
    openWebWorkspaceAction->setToolTip(tr("Open Cloud Storage Web Interface in Browser"));
    connect(openWebWorkspaceAction, &QAction::triggered, this, [this]() { QDesktopServices::openUrl(workspaceService->getWebWorkspaceUrl() + "/storage"); });
    treeView->addAction(openWebWorkspaceAction);

    treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(treeView, &QTreeView::customContextMenuRequested, this, &CloudStorageDockWidget::showContextMenu);
    connect(treeView, &QTreeView::doubleClicked, this, &CloudStorageDockWidget::downloadItemSilently);
    connect(treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &CloudStorageDockWidget::updateActionsState);

    toolbar->addAction(downloadAction);
    toolbar->addAction(uploadAction);
    toolbar->addAction(createDirAction);
    toolbar->addAction(renameAction);
    toolbar->addAction(deleteAction);
    toolbar->addAction(openWebWorkspaceAction);

    updateActionsState();
    updateStateLabelText();
}

bool CloudStorageDockWidget::eventFilter(QObject* watched, QEvent* event) {
    if (watched == treeView->viewport() && event->type() == QEvent::MouseButtonPress) {
        auto mouseEvent = (QMouseEvent*)event;
        if (mouseEvent->button() == Qt::MouseButton::LeftButton) {
            QModelIndex index = treeView->indexAt(mouseEvent->pos());
            // Clear selection on right-click on empty space.
            if (!index.isValid()) {
                treeView->clearSelection();
            }
        }
    }
    return false;
}

void CloudStorageDockWidget::showContextMenu(const QPoint& point) {
    QObjectScopedPointer<QMenu> contextMenu = new QMenu();
    contextMenu->addAction(createDirAction);
    contextMenu->addAction(deleteAction);
    contextMenu->addAction(renameAction);
    contextMenu->addAction(downloadAction);
    contextMenu->addAction(uploadAction);
    contextMenu->exec(treeView->viewport()->mapToGlobal(point));
}

void CloudStorageDockWidget::createDir() {
    QModelIndex currentIndex = getSelectedItemIndex();
    auto path = treeView->model()->data(currentIndex, USER_DATA_PATH).value<QList<QString>>();
    auto isFolder = treeView->model()->data(currentIndex, USER_DATA_IS_FOLDER).toBool() || path.isEmpty();

    bool ok;
    QString dirName = QInputDialog::getText(nullptr, tr("Create New Folder"), tr("New Folder Name"), QLineEdit::Normal, "", &ok);

    CHECK(ok && !dirName.isEmpty(), );
    if (!CloudStorageService::checkCloudStorageEntryName(dirName)) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("Folder name contains illegal characters: %1").arg(dirName));
        return;
    }
    if (!isFolder) {
        path.pop_back();
    }
    path.append(dirName);
    getCloudStorageService()->createDir(path, this, [this](const auto& response) { handleCloudStorageResponse(response); });
}

void CloudStorageDockWidget::deleteItem() {
    auto path = getSelectedItemPath();
    uiLog.trace("CloudStorageDockWidget::delete: " + path.join("/"));
    CHECK(!path.isEmpty(), );
    QMessageBox::StandardButton result = QMessageBox::question(treeView, tr("Question?"), tr("Do you want to delete %1").arg(path.last()));
    CHECK(result == QMessageBox::Yes, );
    getCloudStorageService()->deleteEntry(path, this, [this](const auto& response) { handleCloudStorageResponse(response); });
}

QModelIndex CloudStorageDockWidget::getSelectedItemIndex() const {
    auto selectedIndexes = treeView->selectionModel()->selectedIndexes();
    QModelIndex currentIndex = selectedIndexes.isEmpty() ? QModelIndex() : selectedIndexes.first();
    return currentIndex;
}

QList<QString> CloudStorageDockWidget::getSelectedItemPath() const {
    auto currentIndex = getSelectedItemIndex();
    QList<QString> path = treeView->model()->data(currentIndex, USER_DATA_PATH).value<QList<QString>>();
    return path;
}

void CloudStorageDockWidget::renameItem() {
    QList<QString> path = getSelectedItemPath();
    uiLog.trace("CloudStorageDockWidget::rename: " + path.join("/"));
    CHECK(!path.isEmpty(), );

    bool ok;
    QString newName = QInputDialog::getText(nullptr, tr("Rename %1").arg(path.last()), tr("New Name"), QLineEdit::Normal, path.last(), &ok);

    CHECK(ok && !newName.isEmpty(), );
    if (!CloudStorageService::checkCloudStorageEntryName(newName)) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("New name contains illegal characters: %1").arg(newName));
        return;
    }
    QList<QString> newPath = path;
    newPath[newPath.length() - 1] = newName;
    getCloudStorageService()->renameEntry(path, newPath, this, [this](const auto& response) { handleCloudStorageResponse(response); });
}

void CloudStorageDockWidget::downloadItemSilently() {
    QModelIndex currentIndex = getSelectedItemIndex();
    auto path = treeView->model()->data(currentIndex, USER_DATA_PATH).value<QList<QString>>();
    auto isFolder = treeView->model()->data(currentIndex, USER_DATA_IS_FOLDER).toBool() || path.isEmpty();
    CHECK(!isFolder, );

    uiLog.trace("CloudStorageDockWidget::downloadItem: " + path.join("/"));
    CHECK(!path.isEmpty(), );
    LastUsedDirHelper lod(CLOUD_STORAGE_LAST_OPENED_DOWNLOAD_DIR, AppContext::getAppSettings()->getUserAppsSettings()->getDownloadDirPath());
    getCloudStorageService()->downloadFile(path, lod.dir, this, [this](const auto& response) { handleCloudStorageResponse(response); });
}

void CloudStorageDockWidget::downloadItem() {
    QModelIndex currentIndex = getSelectedItemIndex();
    auto path = treeView->model()->data(currentIndex, USER_DATA_PATH).value<QList<QString>>();
    auto isFolder = treeView->model()->data(currentIndex, USER_DATA_IS_FOLDER).toBool() || path.isEmpty();
    CHECK(!isFolder, );
    uiLog.trace("CloudStorageDockWidget::downloadItem: " + path.join("/"));
    CHECK(!path.isEmpty(), );
    LastUsedDirHelper lod(CLOUD_STORAGE_LAST_OPENED_DOWNLOAD_DIR, AppContext::getAppSettings()->getUserAppsSettings()->getDownloadDirPath());
    QString dir = U2FileDialog::getExistingDirectory(this, tr("Select a folder to save the downloaded file"), lod.dir);
    CHECK(!dir.isEmpty(), );
    lod.dir = dir;
    getCloudStorageService()->downloadFile(path, dir, this, [this](const auto& response) { handleCloudStorageResponse(response); });
}

void CloudStorageDockWidget::uploadItem() {
    QModelIndex currentIndex = getSelectedItemIndex();
    auto path = treeView->model()->data(currentIndex, USER_DATA_PATH).value<QList<QString>>();
    auto isFolder = treeView->model()->data(currentIndex, USER_DATA_IS_FOLDER).toBool() || path.isEmpty();

    LastUsedDirHelper lod(CLOUD_STORAGE_LAST_OPENED_UPLOAD_DIR, AppContext::getAppSettings()->getUserAppsSettings()->getDownloadDirPath());
    QString localFilePath = U2FileDialog::getOpenFileName(this, tr("Select a file to upload"), lod.dir);
    CHECK(!localFilePath.isEmpty(), );
    lod.dir = QFileInfo(localFilePath).absolutePath();

    if (!isFolder) {
        path.pop_back();
    }
    getCloudStorageService()->uploadFile(path, localFilePath, this, [this](const QJsonObject& response) { handleCloudStorageResponse(response); });
}

void CloudStorageDockWidget::handleCloudStorageResponse(const QJsonObject& response) {
    auto errorMessage = WorkspaceService::getErrorMessageFromResponse(response);
    if (!errorMessage.isEmpty()) {
        QMessageBox::critical(this, L10N::errorTitle(), errorMessage);
    }
}

void CloudStorageDockWidget::updateActionsState() {
    bool hasTreeView = treeView->isVisible();

    bool hasSelection = false;
    bool hasFileSelection = false;
    if (hasTreeView) {
        QModelIndex currentIndex = getSelectedItemIndex();
        hasSelection = currentIndex.isValid();
        hasFileSelection = hasSelection && treeView->model()->data(currentIndex, USER_DATA_IS_FOLDER).toBool() == false;
    }
    createDirAction->setEnabled(hasTreeView);
    deleteAction->setEnabled(hasTreeView && hasSelection);
    renameAction->setEnabled(hasTreeView && hasSelection);
    downloadAction->setEnabled(hasTreeView && hasFileSelection);
    uploadAction->setEnabled(hasTreeView);
}

void CloudStorageDockWidget::updateStateLabelText() {
    disconnect(stateLabel, &QLabel::linkActivated, this, nullptr);
    auto isLoggedIn = workspaceService->isLoggedIn();
    if (!isLoggedIn) {
        stateLabel->setText(tr(R"(Please <a href="login">log in to Workspace</a> to access cloud storage)"));
    } else {
        stateLabel->setText(tr(R"(Loading file list...<br><br><br><a href="logout">Logout</a>)"));
    }

    connect(stateLabel, &QLabel::linkActivated, this, [&](const QString& link) {
        if (link == "login") {
            workspaceService->login();
        } else if (link == "logout") {
            workspaceService->logout();
        }
    });
}

CloudStorageService* CloudStorageDockWidget::getCloudStorageService() const {
    return workspaceService->getCloudStorageService();
}

}  // namespace U2
