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

void updateModel(QStandardItem* parentItem, const CloudStorageEntry& entry) {
    QMap<QString, QStandardItem*> currentChildrenMap;
    for (int i = 0; i < parentItem->rowCount(); ++i) {
        QStandardItem* childItem = parentItem->child(i);
        currentChildrenMap[childItem->text()] = childItem;
    }

    for (const CloudStorageEntry& child : entry.children) {
        QIcon icon(child.isFolder() ? ":U2Designer/images/directory.png" : ":core/images/document.png");
        if (currentChildrenMap.contains(child.name)) {
            QStandardItem* childItem = currentChildrenMap[child.name];
            childItem->setText(child.name);
            childItem->setIcon(icon);
            childItem->setData(child.size, Qt::UserRole + 1);
            updateModel(childItem, child);
            currentChildrenMap.remove(child.name);
        } else {
            auto nameItem = new QStandardItem(icon, child.name);
            auto sizeItem = new QStandardItem(QString::number(child.size));
            parentItem->appendRow({
                nameItem,
                sizeItem,
            });
            updateModel(nameItem, child);
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
    });
}

}  // namespace U2
