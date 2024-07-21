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

#include <QDebug>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QVBoxLayout>

#include <U2Gui/MainWindow.h>

#include "CloudStorageService.h"
#include "WorkspaceService.h"

namespace U2 {

constexpr qint64 USER_DATA_SESSION_LOCAL_ID = Qt::UserRole + 1;
constexpr qint64 USER_DATA_SIZE = Qt::UserRole + 2;

void updateModel(QStandardItem* parentItem, const CloudStorageEntry& entry) {
    QMap<qint64, QStandardItem*> currentChildrenMap;
    for (int i = 0; i < parentItem->rowCount(); ++i) {
        QStandardItem* childItem = parentItem->child(i);
        currentChildrenMap[childItem->data(USER_DATA_SESSION_LOCAL_ID).toLongLong()] = childItem;
    }

    for (const CloudStorageEntry& childEntry : entry->children) {
        QIcon icon(childEntry.isFolder() ? ":U2Designer/images/directory.png" : ":core/images/document.png");
        if (currentChildrenMap.contains(childEntry->sessionLocalId)) {
            QStandardItem* childItem = currentChildrenMap[childEntry->sessionLocalId];
            childItem->setText(childEntry->name);
            childItem->setIcon(icon);
            childItem->setData(childEntry->size, USER_DATA_SIZE);
            updateModel(childItem, childEntry);
            currentChildrenMap.remove(childEntry->sessionLocalId);
        } else {
            auto nameItem = new QStandardItem(icon, childEntry->name);
            nameItem->setData(childEntry->sessionLocalId, USER_DATA_SESSION_LOCAL_ID);
            auto sizeItem = new QStandardItem(QString::number(childEntry->size));
            parentItem->appendRow({nameItem, sizeItem});
            updateModel(nameItem, childEntry);
        }
    }

    for (auto it = currentChildrenMap.constBegin(); it != currentChildrenMap.constEnd(); ++it) {
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
}

}  // namespace U2
