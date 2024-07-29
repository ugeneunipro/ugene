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

#include <QAction>
#include <QContextMenuEvent>
#include <QDebug>
#include <QHeaderView>
#include <QIcon>
#include <QInputDialog>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QScreen>
#include <QVBoxLayout>

#include <U2Core/L10n.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/MainWindow.h>

#include "../../../plugins_3rdparty/phylip/src/phylip.h"
#include "CloudStorageService.h"
#include "WorkspaceService.h"

namespace U2 {

constexpr qint64 USER_DATA_SESSION_LOCAL_ID = Qt::UserRole + 1;
constexpr qint64 USER_DATA_SIZE = Qt::UserRole + 2;
constexpr qint64 USER_DATA_PATH = Qt::UserRole + 3;
constexpr qint64 USER_DATA_IS_FOLDER = Qt::UserRole + 4;

void updateModel(QStandardItem* parentItem, const CloudStorageEntry& entry) {
    QMap<qint64, QStandardItem*> childrenMap;
    for (int i = 0; i < parentItem->rowCount(); i++) {
        QStandardItem* childItem = parentItem->child(i);
        auto childEntryKey = childItem->data(USER_DATA_SESSION_LOCAL_ID).toLongLong();
        childrenMap[childEntryKey] = childItem;
    }

    for (const CloudStorageEntry& childEntry : entry->children) {
        QIcon icon(childEntry->isFolder ? ":U2Designer/images/directory.png" : ":core/images/document.png");
        auto childEntryKey = childEntry->sessionLocalId;
        if (childrenMap.contains(childEntryKey)) {
            QStandardItem* nameItem = childrenMap[childEntryKey];
            nameItem->setIcon(icon);
            nameItem->setText(childEntry->getName());
            nameItem->setData(childEntry->size, USER_DATA_SIZE);
            nameItem->setData(childEntry->isFolder, USER_DATA_IS_FOLDER);
            nameItem->setData(QVariant::fromValue(childEntry->path), USER_DATA_PATH);

            updateModel(nameItem, childEntry);

            // Unregister it from the map, because all entries that left int he map will be removed from the view as unused.
            childrenMap.remove(childEntryKey);
        } else {
            auto nameItem = new QStandardItem(icon, childEntry->getName());
            nameItem->setData(childEntry->size, USER_DATA_SIZE);
            nameItem->setData(childEntry->isFolder, USER_DATA_IS_FOLDER);
            nameItem->setData(QVariant::fromValue(childEntry->path), USER_DATA_PATH);
            nameItem->setData(childEntry->sessionLocalId, USER_DATA_SESSION_LOCAL_ID);

            parentItem->appendRow({nameItem});

            updateModel(nameItem, childEntry);
        }
    }

    for (auto it = childrenMap.constBegin(); it != childrenMap.constEnd(); ++it) {
        parentItem->removeRow(it.value()->row());
    }
}

CloudStorageDockWidget::CloudStorageDockWidget(WorkspaceService* _workspaceService)
    : workspaceService(_workspaceService) {
    setObjectName(DOCK_CLOUD_STORAGE_VIEW);
    setWindowTitle(tr("Storage"));
    setWindowIcon(QIcon(":ugene/images/cloud_storage.svg"));

    stateLabel = new QLabel();

    treeView = new QTreeView();
    treeView->setModel(&treeViewModel);
    treeView->setVisible(false);
    treeView->setEditTriggers(QTreeView::NoEditTriggers);
    treeView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto layout = new QVBoxLayout();
    layout->addWidget(stateLabel);
    layout->addWidget(treeView);
    setLayout(layout);

    connect(workspaceService, &WorkspaceService::si_authenticationEvent, this, [this](bool isLoggedIn) {
        stateLabel->setVisible(true);
        treeView->setVisible(false);
        treeViewModel.clear();
        treeViewModel.setHorizontalHeaderLabels({"Name", "Size"});
        treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
        if (!isLoggedIn) {
            stateLabel->setText("Please login");
        } else {
            stateLabel->setText("Loading file list");
        }
    });

    connect(workspaceService->getCloudStorageService(), &CloudStorageService::si_storageStateChanged, this, [this](const CloudStorageEntry& rootEntry) {
        stateLabel->setVisible(false);
        treeView->setVisible(true);

        qDebug() << "CloudStorageDockWidget: got new cloud storage state";
        QStandardItem* rootItem = treeViewModel.invisibleRootItem();
        treeView->setUpdatesEnabled(false);
        updateModel(rootItem, rootEntry);
        treeView->setUpdatesEnabled(true);
        treeView->update();
        // TODO: preserve selection using 'sessionLocalId'
    });

    createDirAction = new QAction(tr("New Folder"), this);
    createDirAction->setShortcut(QKeySequence(Qt::Key_Insert));
    connect(createDirAction, &QAction::triggered, this, &CloudStorageDockWidget::createDir);
    treeView->addAction(createDirAction);

    deleteAction = new QAction(tr("Delete"), this);
    deleteAction->setShortcut(QKeySequence::Delete);
    connect(deleteAction, &QAction::triggered, this, &CloudStorageDockWidget::deleteItem);
    treeView->addAction(deleteAction);

    renameAction = new QAction(tr("Rename"), this);
    renameAction->setShortcut(QKeySequence(Qt::Key_F2));
    connect(renameAction, &QAction::triggered, this, &CloudStorageDockWidget::renameItem);
    treeView->addAction(renameAction);

    downloadAction = new QAction(tr("Download"), this);
    connect(downloadAction, &QAction::triggered, this, &CloudStorageDockWidget::downloadItem);
    treeView->addAction(downloadAction);

    uploadAction = new QAction(tr("Upload"), this);
    connect(uploadAction, &QAction::triggered, this, &CloudStorageDockWidget::uploadItem);
    treeView->addAction(uploadAction);

    treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(treeView, &QTreeView::customContextMenuRequested, this, &CloudStorageDockWidget::showContextMenu);
    connect(treeView, &QTreeView::doubleClicked, this, &CloudStorageDockWidget::renameItem);
    connect(treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &CloudStorageDockWidget::updateActionsState);
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
    auto selectedIndexes = treeView->selectionModel()->selectedIndexes();
    QModelIndex currentIndex = selectedIndexes.isEmpty() ? QModelIndex() : selectedIndexes.first();
    auto path = treeView->model()->data(currentIndex, USER_DATA_PATH).value<QList<QString>>();
    auto isFolder = treeView->model()->data(currentIndex, USER_DATA_IS_FOLDER).toBool() || path.length() == 0;

    bool ok;
    QString dirName = QInputDialog::getText(nullptr, tr("Create New Folder"), tr("New Folder Name"), QLineEdit::Normal, "", &ok);

    CHECK(ok && !dirName.isEmpty(), );
    if (!CloudStorageService::checkCloudStorageEntryName(dirName)) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("Folder name contains illegal characters"));
        return;
    }
    if (!isFolder) {
        path.pop_back();
    }
    path.append(dirName);
    workspaceService->getCloudStorageService()->createDir(path);
}

void CloudStorageDockWidget::deleteItem() {
    QModelIndex currentIndex = treeView->currentIndex();
    auto name = treeView->model()->data(currentIndex, Qt::DisplayRole).toString();
    auto path = treeView->model()->data(currentIndex, USER_DATA_PATH).value<QList<QString>>();
    qDebug() << "CloudStorageDockWidget::delete: " + path.join("/");
    QMessageBox::StandardButton result = QMessageBox::question(treeView, tr("Question?"), tr("Do you want to delete %1").arg(name));
    CHECK(result == QMessageBox::Yes, );
    workspaceService->getCloudStorageService()->deleteEntry(path);
}

void CloudStorageDockWidget::renameItem() {
    auto selectedIndexes = treeView->selectionModel()->selectedIndexes();
    QModelIndex currentIndex = selectedIndexes.isEmpty() ? QModelIndex() : selectedIndexes.first();
    auto path = treeView->model()->data(currentIndex, USER_DATA_PATH).value<QList<QString>>();
    qDebug() << "CloudStorageDockWidget::rename: " + path.join("/");
    CHECK(path.length() > 0, );

    bool ok;
    QString newName = QInputDialog::getText(nullptr, tr("Rename %1").arg(path.last()), tr("New Name"), QLineEdit::Normal, "", &ok);

    CHECK(ok && !newName.isEmpty(), );
    if (!CloudStorageService::checkCloudStorageEntryName(newName)) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("New name contains illegal characters"));
        return;
    }
    // TODO: check if the entry already exists.
    QList<QString> newPath = path;
    newPath[newPath.length() - 1] = newName;
    workspaceService->getCloudStorageService()->renameEntry(path, newPath);
}

void CloudStorageDockWidget::downloadItem() {
    qDebug() << "Download item";
    // Implement the logic to download the item
}

void CloudStorageDockWidget::uploadItem() {
    qDebug() << "Upload item";
    // Implement the logic to upload items
}

void CloudStorageDockWidget::updateActionsState() {
    bool hasSelection = treeView->selectionModel()->hasSelection();
    deleteAction->setEnabled(hasSelection);
    renameAction->setEnabled(hasSelection);
    downloadAction->setEnabled(hasSelection);
}

}  // namespace U2
