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
#include <QIcon>
#include <QLabel>
#include <QVBoxLayout>

#include <U2Gui/MainWindow.h>

#include "CloudStorageService.h"
#include "WorkspaceService.h"

namespace U2 {

static void addEntryToModel(QStandardItem* parentItem, const CloudStorageEntry& entry) {
    auto item = new QStandardItem(entry.name);
    auto sizeItem = new QStandardItem(QString::number(entry.size));
    auto modificationTimeItem = new QStandardItem(entry.modificationTime.toString(Qt::ISODate));

    parentItem->appendRow({item, sizeItem, modificationTimeItem});

    for (const CloudStorageEntry& child : qAsConst(entry.children)) {
        addEntryToModel(item, child);
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
        if (!isLoggedIn) {
            stateLabel->setText("Please login");
        } else {
            stateLabel->setText("Loading file list");
        }
    });

    connect(workspaceService->getCloudStorageService(), &CloudStorageService::si_storageStateChanged, this, [this](const CloudStorageEntry& rootEntry) {
        qDebug() << "CloudStorageDockWidget: got new cloud storage state";
        stateLabel->setVisible(false);
        treeViewModel.clear();
        treeViewModel.setHorizontalHeaderLabels({"Name", "Size", "Modification Time"});
        QStandardItem* rootItem = treeViewModel.invisibleRootItem();
        addEntryToModel(rootItem, rootEntry);
        treeView->setVisible(true);
    });
}

}  // namespace U2
