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
#pragma once

#include <QJsonObject>
#include <QLabel>
#include <QStandardItemModel>
#include <QTreeView>
#include <QWidget>

namespace U2 {

class CloudStorageService;
class WorkspaceService;

/** Dock widget that shows cloud storage content and allows basic operations. */
class CloudStorageDockWidget : public QWidget {
    Q_OBJECT

public:
    CloudStorageDockWidget(WorkspaceService* workspaceService);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void showContextMenu(const QPoint& point);
    void createDir();
    void deleteItem();
    void renameItem();
    void downloadItem();
    void downloadItemSilently();
    void uploadItem();
    void shareItem();

    void handleCloudStorageResponse(const QJsonObject& response);

    QList<QString> getSelectedItemPath() const;
    QModelIndex getSelectedItemIndex() const;

    void updateActionsState() const;
    void updateStateLabelText();
    CloudStorageService* getCloudStorageService() const;

    WorkspaceService* workspaceService = nullptr;
    QLabel* stateLabel = nullptr;
    QTreeView* treeView = nullptr;
    QAction* createDirAction = nullptr;
    QAction* deleteAction = nullptr;
    QAction* renameAction = nullptr;
    QAction* downloadAction = nullptr;
    QAction* uploadAction = nullptr;
    QAction* shareAction = nullptr;
    QAction* openWebWorkspaceAction = nullptr;
    QStandardItemModel treeViewModel;
};

}  // namespace U2
