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

#include <QIcon>
#include <QLabel>
#include <QVBoxLayout>

#include <U2Gui/MainWindow.h>

#include "CloudStorageService.h"
#include "WorkspaceService.h"

namespace U2 {

CloudStorageDockWidget::CloudStorageDockWidget(WorkspaceService* _workspaceService)
    : workspaceService(_workspaceService) {
    setObjectName(DOCK_CLOUD_STORAGE_VIEW);
    setWindowTitle(tr("Storage"));
    setWindowIcon(QIcon(":ugene/images/cloud_storage.svg"));

    contentLabel = new QLabel("empty");
    auto layout = new QVBoxLayout();
    layout->addWidget(contentLabel);
    setLayout(layout);

    connect(workspaceService, &WorkspaceService::si_authenticationEvent, this, [this](bool isLoggedIn) {
        if (!isLoggedIn) {
            contentLabel->setText("Please login");
        } else {
            contentLabel->setText("Loading file list");
            ;
        }
    });

    connect(workspaceService->getCloudStorageService(), &CloudStorageService::si_storageStateChanged, this, [this](const QString& newContent) {
        contentLabel->setText(newContent);
    });
}

}  // namespace U2
