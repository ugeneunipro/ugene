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
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QVBoxLayout>

#include <U2Core/L10n.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/MainWindow.h>

#include "CloudStorageService.h"
#include "WorkspaceService.h"

namespace U2 {

constexpr qint64 USER_DATA_SESSION_LOCAL_ID = Qt::UserRole + 1;
constexpr qint64 USER_DATA_SIZE = Qt::UserRole + 2;
constexpr qint64 USER_DATA_PATH = Qt::UserRole + 3;

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
            nameItem->setData(QVariant::fromValue(childEntry->path), USER_DATA_PATH);
            updateModel(nameItem, childEntry);

            int rowIndex = nameItem->row();
            QStandardItemModel* model = nameItem->model();
            QStandardItem* sizeItem = model->item(rowIndex, 1);
            sizeItem->setText(QString::number(childEntry->size));
            // Unregister it from the map, because all entries that left int he map will be removed from the view as unused.
            childrenMap.remove(childEntryKey);
        } else {
            auto nameItem = new QStandardItem(icon, childEntry->getName());
            nameItem->setData(childEntry->size, USER_DATA_SIZE);
            nameItem->setData(QVariant::fromValue(childEntry->path), USER_DATA_PATH);
            nameItem->setData(childEntry->sessionLocalId, USER_DATA_SESSION_LOCAL_ID);

            auto sizeItem = new QStandardItem(QString::number(childEntry->size));
            parentItem->appendRow({nameItem, sizeItem});
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
        updateModel(rootItem, rootEntry);
        // TODO: preserve selection using 'sessionLocalId'
    });

    createDirAction = new QAction(tr("New Folder"), this);
    connect(createDirAction, &QAction::triggered, this, &CloudStorageDockWidget::createDir);

    deleteAction = new QAction(tr("Delete"), this);
    connect(deleteAction, &QAction::triggered, this, &CloudStorageDockWidget::deleteItem);

    renameAction = new QAction(tr("Rename"), this);
    connect(renameAction, &QAction::triggered, this, &CloudStorageDockWidget::renameItem);

    downloadAction = new QAction(tr("Download"), this);
    connect(downloadAction, &QAction::triggered, this, &CloudStorageDockWidget::downloadItem);

    uploadAction = new QAction(tr("Upload"), this);
    connect(uploadAction, &QAction::triggered, this, &CloudStorageDockWidget::uploadItem);

    treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(treeView, &QTreeView::customContextMenuRequested, this, &CloudStorageDockWidget::showContextMenu);
}

void CloudStorageDockWidget::showContextMenu(const QPoint& point) {
    QModelIndex index = treeView->indexAt(point);
    CHECK(index.isValid(), );

    QObjectScopedPointer<QMenu> contextMenu = new QMenu();
    contextMenu->addAction(createDirAction);
    contextMenu->addAction(deleteAction);
    contextMenu->addAction(renameAction);
    contextMenu->addAction(downloadAction);
    contextMenu->addAction(uploadAction);
    contextMenu->exec(treeView->viewport()->mapToGlobal(point));
}

void CloudStorageDockWidget::createDir() {
    qDebug() << "CloudStorageDockWidget::createDir";
    // TODO: if dir is selected, create inside dir. Otherwise create in current dir.
}

void CloudStorageDockWidget::deleteItem() {
    QModelIndex currentIndex = treeView->currentIndex();
    auto name = treeView->model()->data(currentIndex, Qt::DisplayRole).toString();
    auto path = treeView->model()->data(currentIndex, USER_DATA_PATH).value<QList<QString>>();
    qDebug() << "CloudStorageDockWidget::deleteItem: " + path.join("/");
    QMessageBox::StandardButton result = QMessageBox::question(treeView, tr("Question?"), tr("Do you want to delete %1").arg(name));
    CHECK(result == QMessageBox::Yes, );
    workspaceService->getCloudStorageService()->deleteItem(path);
}

void CloudStorageDockWidget::renameItem() {
    qDebug() << "Rename item";
    // Implement the logic to rename the item
}

void CloudStorageDockWidget::downloadItem() {
    qDebug() << "Download item";
    // Implement the logic to download the item
}

void CloudStorageDockWidget::uploadItem() {
    qDebug() << "Upload item";
    // Implement the logic to upload items
}

}  // namespace U2
