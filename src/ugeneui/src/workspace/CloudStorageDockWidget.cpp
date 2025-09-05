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

#include "CloudStorageDockWidget.h"

#include <QApplication>
#include <QContextMenuEvent>
#include <QDesktopServices>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QToolBar>
#include <QVBoxLayout>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/L10n.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/MainWindow.h>
#include <U2Gui/Theme.h>

#include "CloudStorageService.h"
#include "WorkspaceService.h"
#include "main_window/DockManagerImpl.h"

namespace U2 {
constexpr auto CLOUD_STORAGE_LAST_OPENED_DOWNLOAD_DIR = "CloudStorageDownloadDir";
constexpr auto CLOUD_STORAGE_LAST_OPENED_UPLOAD_DIR = "CloudStorageUploadDir";

constexpr qint64 USER_DATA_SESSION_LOCAL_ID = Qt::UserRole + 1;
constexpr qint64 USER_DATA_SIZE = Qt::UserRole + 2;
constexpr qint64 USER_DATA_PATH = Qt::UserRole + 3;
constexpr qint64 USER_DATA_IS_FOLDER = Qt::UserRole + 4;
constexpr qint64 USER_DATA_SECONDARY_ICON = Qt::UserRole + 5;
constexpr qint64 USER_DATA_SHARED_WITH_EMAILS = Qt::UserRole + 6;
constexpr qint64 USER_DATA_ICON = Qt::UserRole + 7;

/** Custom delegate to add a secondary icon to tree item. */
class MultiIconStyledItemDelegate final : public QStyledItemDelegate {
public:
    explicit MultiIconStyledItemDelegate(QObject* parent = nullptr)
        : QStyledItemDelegate(parent) {
    }

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        QStyle* style = opt.widget ? opt.widget->style() : QApplication::style();
        QRect rect = opt.rect;

        // Draw the background (including selection)
        style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);

        // Draw the main icon.
        auto mainIcon = index.data(Qt::DecorationRole).value<QIcon>();
        if (!mainIcon.isNull()) {
            QSize iconSize(20, 20);
            QRect iconRect(rect.left(), rect.top() + (rect.height() - iconSize.height()) / 2, iconSize.width(), iconSize.height());
            mainIcon.paint(painter, iconRect);
            rect.setLeft(iconRect.right() + 5);
        }

        // Draw the text.
        style->drawItemText(painter, rect, Qt::AlignVCenter | Qt::AlignLeft, opt.palette, opt.state & QStyle::State_Enabled, opt.text);
        int textWidth = painter->fontMetrics().horizontalAdvance(opt.text);
        rect.setLeft(rect.left() + textWidth + 5);

        // Draw the secondary icon.
        auto secondaryIconPath = index.data(USER_DATA_SECONDARY_ICON).toString();
        if (!secondaryIconPath.isEmpty()) {
            QSize iconSize(16, 16);
            QRect secondaryIconRect(rect.left(), rect.top() + (rect.height() - iconSize.height()) / 2, iconSize.width(), iconSize.height());
            GUIUtils::getThemedIcon(secondaryIconPath).paint(painter, secondaryIconRect);
        }
    }

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        QSize defaultSize = QStyledItemDelegate::sizeHint(option, index);
        return {defaultSize.width(), 22};
    }
};

static bool isLess(const QStandardItem* a, const QStandardItem* b) {
    return a->data(USER_DATA_IS_FOLDER) != b->data(USER_DATA_IS_FOLDER)
               ? a->data(USER_DATA_IS_FOLDER).toBool()
               : a->text() < b->text();
}

static void updateModel(QTreeView* tree,
                        QStandardItem* parentItem,
                        const CloudStorageEntry& entry,
                        QList<QStandardItem*>& expandedItems) {
    QMap<qint64, QStandardItem*> childrenMap;
    for (int i = 0; i < parentItem->rowCount(); i++) {
        QStandardItem* childItem = parentItem->child(i);
        auto childEntryKey = childItem->data(USER_DATA_SESSION_LOCAL_ID).toLongLong();
        childrenMap[childEntryKey] = childItem;
    }
    for (const CloudStorageEntry& childEntry : qAsConst(entry->children)) {
        QString iconPath = childEntry->isFolder
                       ? (childEntry->getName() == "Shared" && childEntry->path.length() == 1
                              ? ":ugene/images/folder_shared.svg"
                              : ":ugene/images/folder.svg")
                       : ":ugene/images/document.svg";
        QIcon icon = GUIUtils::getThemedIcon(iconPath);
        auto childEntryKey = childEntry->sessionLocalId;
        QStandardItem* nameItem;
        if (childrenMap.contains(childEntryKey)) {
            nameItem = childrenMap[childEntryKey];
            nameItem->setIcon(icon);
            nameItem->setText(childEntry->getName());
            nameItem->setData(childEntry->size, USER_DATA_SIZE);
            nameItem->setData(childEntry->isFolder, USER_DATA_IS_FOLDER);
            nameItem->setData(QVariant::fromValue(childEntry->path), USER_DATA_PATH);
            nameItem->setData(QVariant::fromValue(iconPath), USER_DATA_ICON);

            updateModel(tree, nameItem, childEntry, expandedItems);

            // Unregister it from the map, because all entries that left in the map will be removed from the view as unused.
            childrenMap.remove(childEntryKey);
        } else {
            nameItem = new QStandardItem(icon, childEntry->getName());
            nameItem->setData(childEntry->size, USER_DATA_SIZE);
            nameItem->setData(childEntry->isFolder, USER_DATA_IS_FOLDER);
            nameItem->setData(QVariant::fromValue(childEntry->path), USER_DATA_PATH);
            nameItem->setData(childEntry->sessionLocalId, USER_DATA_SESSION_LOCAL_ID);
            nameItem->setData(QVariant::fromValue(iconPath), USER_DATA_ICON);

            parentItem->appendRow(nameItem);

            updateModel(tree, nameItem, childEntry, expandedItems);
        }
        bool isShared = !childEntry->sharedWithEmails.isEmpty();
        if (isShared) {
            nameItem->setData(":ugene/images/group.svg", USER_DATA_SECONDARY_ICON);
            nameItem->setToolTip(CloudStorageDockWidget::tr("Shared with:\n%1").arg(childEntry->sharedWithEmails.join("\n")));
            nameItem->setData(QVariant::fromValue(childEntry->sharedWithEmails), USER_DATA_SHARED_WITH_EMAILS);
        } else {
            nameItem->setData(QVariant(), USER_DATA_SECONDARY_ICON);
            nameItem->setData(QVariant(), USER_DATA_SHARED_WITH_EMAILS);
        }
    }

    for (auto it = childrenMap.constBegin(); it != childrenMap.constEnd(); ++it) {
        auto item = it.value();
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
    std::sort(sortedChildren.begin(), sortedChildren.end(), [](const QStandardItem* a, const QStandardItem* b) {
        return isLess(a, b);
    });
    for (int i = 0; i < rowCount; i++) {
        parentItem->setChild(i, sortedChildren[i]);
    }
}

static bool isItemInTree(const QStandardItemModel& model, const QStandardItem* item) {
    CHECK(item, false);
    // Traverse the entire model hierarchy to check if the item is reachable.
    QStandardItem* rootItem = model.invisibleRootItem();

    // Helper function to search for the item recursively.
    std::function<bool(const QStandardItem*)> isItemInHierarchy = [&](const QStandardItem* parent) {
        if (!parent) {
            return false;
        }
        for (int row = 0; row < parent->rowCount(); row++) {
            QStandardItem* child = parent->child(row);
            if (child == item) {
                return true;  // Found the item.
            }
            // Recurse into the child's children.
            if (isItemInHierarchy(child)) {
                return true;
            }
        }
        return false;
    };

    return isItemInHierarchy(rootItem);
}

static void updateTree(QTreeView* tree, const QStandardItemModel& model, const CloudStorageEntry& entry) {
    tree->setUpdatesEnabled(false);
    QModelIndexList selectedIndexes = tree->selectionModel()->selectedIndexes();
    QStandardItem* selectedItem = selectedIndexes.isEmpty()
                                      ? nullptr
                                      : model.itemFromIndex(selectedIndexes.first());

    QList<QStandardItem*> expandedItems;  // List of item that should be expanded after model update.
    updateModel(tree, model.invisibleRootItem(), entry, expandedItems);

    // Resorting in updateModel breaks expanded & selected states.
    // We will restore it from the root (last in the list) to leafs ( first in the list).
    for (int i = expandedItems.length(); --i >= 0;) {
        tree->setExpanded(expandedItems[i]->index(), true);
    }

    if (selectedItem != nullptr && isItemInTree(model, selectedItem)) {
        tree->selectionModel()->select(selectedItem->index(), QItemSelectionModel::Select);
    }

    tree->setUpdatesEnabled(true);
    tree->update();
}

CloudStorageDockWidget::CloudStorageDockWidget(WorkspaceService* _workspaceService)
    : workspaceService(_workspaceService) {
    setObjectName(DOCK_CLOUD_STORAGE_VIEW);
    setWindowTitle(tr("Cloud Storage"));
    GUIUtils::setThemedWindowIcon(this, ":ugene/images/cloud_storage.svg");

    stateLabel = new QLabel();
    stateLabel->setTextFormat(Qt::RichText);
    stateLabel->setOpenExternalLinks(false);
    stateLabel->setStyleSheet(QString("background: %1; padding: 10px;").arg(QPalette().base().color().name()));

    treeView = new QTreeView();
    treeView->setModel(&treeViewModel);
    treeView->setVisible(false);
    treeView->setEditTriggers(QTreeView::NoEditTriggers);
    treeView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    treeView->setObjectName("cloudStorageTreeView");
    treeView->header()->hide();
    treeView->viewport()->installEventFilter(this);
    treeView->setItemDelegate(new MultiIconStyledItemDelegate(treeView));
    treeView->setUniformRowHeights(true);

    const auto toolbar = new QToolBar();
    toolbar->setIconSize(QSize(20, 20));
    toolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);

    const auto layout = new QVBoxLayout();
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

    createDirAction = new QAction(tr("New Folder"), this);
    GUIUtils::setThemedIcon(createDirAction, ":ugene/images/new_folder.svg");
    createDirAction->setObjectName("cloudStorageCreateDirAction");
    createDirAction->setShortcut(QKeySequence(Qt::Key_Insert));
    createDirAction->setToolTip(tr("Create New Folder on Cloud Storage"));
    connect(createDirAction, &QAction::triggered, this, &CloudStorageDockWidget::createDir);
    treeView->addAction(createDirAction);

    deleteAction = new QAction(tr("Delete"), this);
    GUIUtils::setThemedIcon(deleteAction, ":ugene/images/trash.svg");
    deleteAction->setObjectName("cloudStorageDeleteAction");
    deleteAction->setShortcut(QKeySequence::Delete);
    deleteAction->setToolTip(tr("Delete selected file from Cloud Storage"));
    connect(deleteAction, &QAction::triggered, this, &CloudStorageDockWidget::deleteItem);
    treeView->addAction(deleteAction);

    renameAction = new QAction(tr("Rename"), this);
    GUIUtils::setThemedIcon(renameAction, ":ugene/images/file_rename.svg");
    renameAction->setObjectName("cloudStorageRenameAction");
    renameAction->setToolTip(tr("Rename File on Cloud Storage"));
    renameAction->setShortcut(QKeySequence(Qt::Key_F2));
    connect(renameAction, &QAction::triggered, this, &CloudStorageDockWidget::renameItem);
    treeView->addAction(renameAction);

    downloadAction = new QAction(tr("Download"), this);
    GUIUtils::setThemedIcon(downloadAction, ":ugene/images/file_download.svg");
    downloadAction->setObjectName("cloudStorageDownloadAction");
    downloadAction->setToolTip(tr("Download File from Cloud Storage"));
    connect(downloadAction, &QAction::triggered, this, &CloudStorageDockWidget::downloadItem);
    treeView->addAction(downloadAction);

    uploadAction = new QAction(tr("Upload"), this);
    GUIUtils::setThemedIcon(uploadAction, ":ugene/images/file_upload.svg");
    uploadAction->setObjectName("cloudStorageUploadAction");
    uploadAction->setToolTip(tr("Upload File to Cloud Storage"));
    connect(uploadAction, &QAction::triggered, this, &CloudStorageDockWidget::uploadItem);
    treeView->addAction(uploadAction);

    shareAction = new QAction(tr("Share"), this);
    GUIUtils::setThemedIcon(shareAction, ":ugene/images/file_share.svg");
    shareAction->setObjectName("cloudStorageShareAction");
    shareAction->setToolTip(tr("Share file or folder with other users"));
    connect(shareAction, &QAction::triggered, this, &CloudStorageDockWidget::shareItem);
    treeView->addAction(shareAction);

    openWebWorkspaceAction = new QAction(tr("Open web interface"));
    GUIUtils::setThemedIcon(openWebWorkspaceAction, ":ugene/images/web_link.svg");
    openWebWorkspaceAction->setToolTip(tr("Open Cloud Storage Web Interface in Browser"));
    connect(openWebWorkspaceAction, &QAction::triggered, this, [this] {
        QDesktopServices::openUrl(workspaceService->getWebWorkspaceUrl() + "/storage");
    });
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
    toolbar->addAction(shareAction);
    toolbar->addAction(openWebWorkspaceAction);

    updateActionsState();
    updateStateLabelText();

    connect(AppContext::getMainWindow(), &MainWindow::si_colorThemeSwitched, this, &CloudStorageDockWidget::sl_colorThemeSwitched);
}

bool CloudStorageDockWidget::eventFilter(QObject* watched, QEvent* event) {
    if (watched == treeView->viewport() && event->type() == QEvent::MouseButtonPress) {
        auto mouseEvent = dynamic_cast<QMouseEvent*>(event);
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

void CloudStorageDockWidget::sl_colorThemeSwitched() {
    updateTreeViewIconsRecursively(treeViewModel.invisibleRootItem());
}

void CloudStorageDockWidget::updateTreeViewIconsRecursively(QStandardItem* item) {
    SAFE_POINT_NN(item, );

    auto iconPath = item->data(USER_DATA_ICON).toString();
    auto icon = GUIUtils::getThemedIcon(iconPath);
    item->setIcon(icon);
    for (int i = 0; i < item->rowCount(); i++) {
        updateTreeViewIconsRecursively(item->child(i));
    }
}

void CloudStorageDockWidget::showContextMenu(const QPoint& point) {
    QObjectScopedPointer<QMenu> contextMenu = new QMenu();
    contextMenu->addAction(createDirAction);
    contextMenu->addAction(deleteAction);
    contextMenu->addAction(renameAction);
    contextMenu->addAction(downloadAction);
    contextMenu->addAction(uploadAction);
    contextMenu->addAction(shareAction);

    QModelIndex currentIndex = getSelectedItemIndex();
    if (currentIndex.isValid()) {
        auto path = treeView->model()->data(currentIndex, USER_DATA_PATH).value<QList<QString>>();
        auto sharedWithEmails = treeView->model()->data(currentIndex, USER_DATA_SHARED_WITH_EMAILS).value<QList<QString>>();
        if (!sharedWithEmails.isEmpty()) {
            auto stopSharingMenu = new QMenu(tr("Stop sharing with ..."));
            contextMenu->addMenu(stopSharingMenu);
            for (const auto& email : qAsConst(sharedWithEmails)) {
                stopSharingMenu->addAction(email, [this, path, email] {
                    getCloudStorageService()->unshareEntry(path, email, this, [this](const auto& response) {
                        handleCloudStorageResponse(response);
                    });
                });
            }
        }
    }

    contextMenu->exec(treeView->viewport()->mapToGlobal(point));
}

void CloudStorageDockWidget::createDir() {
    QModelIndex currentIndex = getSelectedItemIndex();
    auto path = treeView->model()->data(currentIndex, USER_DATA_PATH).value<QList<QString>>();
    auto isFolder = treeView->model()->data(currentIndex, USER_DATA_IS_FOLDER).toBool() || path.isEmpty();

    bool ok;
    QString dirName = GUIUtils::getTextWithDialog(tr("Create New Folder"), tr("New Folder Name"), "", ok);

    CHECK(ok && !dirName.isEmpty(), );
    if (!CloudStorageService::checkCloudStorageEntryName(dirName)) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("Folder name contains illegal characters: %1").arg(dirName));
        return;
    }
    if (!isFolder) {
        path.pop_back();
    }
    path.append(dirName);
    getCloudStorageService()->createDir(path, this, [this](const auto& response) {
        handleCloudStorageResponse(response);
    });
}

void CloudStorageDockWidget::deleteItem() {
    auto path = getSelectedItemPath();
    uiLog.trace("CloudStorageDockWidget::delete: " + path.join("/"));
    CHECK(!path.isEmpty(), );
    QMessageBox::StandardButton result = QMessageBox::question(treeView, tr("Question?"), tr("Do you want to delete %1").arg(path.last()));
    CHECK(result == QMessageBox::Yes, );
    getCloudStorageService()->deleteEntry(path, this, [this](const auto& response) {
        handleCloudStorageResponse(response);
    });
}

QModelIndex CloudStorageDockWidget::getSelectedItemIndex() const {
    auto selectedIndexes = treeView->selectionModel()->selectedIndexes();
    QModelIndex currentIndex = selectedIndexes.isEmpty() ? QModelIndex() : selectedIndexes.first();
    return currentIndex;
}

QList<QString> CloudStorageDockWidget::getSelectedItemPath() const {
    auto currentIndex = getSelectedItemIndex();
    auto path = treeView->model()->data(currentIndex, USER_DATA_PATH).value<QList<QString>>();
    return path;
}

void CloudStorageDockWidget::renameItem() {
    QList<QString> path = getSelectedItemPath();
    uiLog.trace("CloudStorageDockWidget::rename: " + path.join("/"));
    CHECK(!path.isEmpty(), );

    bool ok;
    QString newName = GUIUtils::getTextWithDialog(tr("Rename %1").arg(path.last()), tr("New Name"), path.last(), ok);

    CHECK(ok && !newName.isEmpty(), );
    if (!CloudStorageService::checkCloudStorageEntryName(newName)) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("New name contains illegal characters: %1").arg(newName));
        return;
    }
    QList<QString> newPath = path;
    newPath[newPath.length() - 1] = newName;
    getCloudStorageService()->renameEntry(path, newPath, this, [this](const auto& response) {
        handleCloudStorageResponse(response);
    });
}

void CloudStorageDockWidget::downloadItemSilently() {
    QModelIndex currentIndex = getSelectedItemIndex();
    auto path = treeView->model()->data(currentIndex, USER_DATA_PATH).value<QList<QString>>();
    auto isFolder = treeView->model()->data(currentIndex, USER_DATA_IS_FOLDER).toBool() || path.isEmpty();
    CHECK(!isFolder, );

    uiLog.trace("CloudStorageDockWidget::downloadItem: " + path.join("/"));
    CHECK(!path.isEmpty(), );
    LastUsedDirHelper lod(CLOUD_STORAGE_LAST_OPENED_DOWNLOAD_DIR,
                          AppContext::getAppSettings()->getUserAppsSettings()->getDownloadDirPath());
    getCloudStorageService()->downloadFile(path, lod.dir, this, [this](const auto& response) {
        handleCloudStorageResponse(response);
    });
}

void CloudStorageDockWidget::downloadItem() {
    QModelIndex currentIndex = getSelectedItemIndex();
    auto path = treeView->model()->data(currentIndex, USER_DATA_PATH).value<QList<QString>>();
    auto isFolder = treeView->model()->data(currentIndex, USER_DATA_IS_FOLDER).toBool() || path.isEmpty();
    CHECK(!isFolder, );
    uiLog.trace("CloudStorageDockWidget::downloadItem: " + path.join("/"));
    CHECK(!path.isEmpty(), );
    LastUsedDirHelper lod(CLOUD_STORAGE_LAST_OPENED_DOWNLOAD_DIR,
                          AppContext::getAppSettings()->getUserAppsSettings()->getDownloadDirPath());
    QString dir = U2FileDialog::getExistingDirectory(this, tr("Select a folder to save the downloaded file"), lod.dir);
    CHECK(!dir.isEmpty(), );
    lod.dir = dir;
    getCloudStorageService()->downloadFile(path, dir, this, [this](const auto& response) {
        handleCloudStorageResponse(response);
    });
}

void CloudStorageDockWidget::uploadItem() {
    QModelIndex currentIndex = getSelectedItemIndex();
    auto path = treeView->model()->data(currentIndex, USER_DATA_PATH).value<QList<QString>>();
    auto isFolder = treeView->model()->data(currentIndex, USER_DATA_IS_FOLDER).toBool() || path.isEmpty();

    LastUsedDirHelper lod(CLOUD_STORAGE_LAST_OPENED_UPLOAD_DIR,
                          AppContext::getAppSettings()->getUserAppsSettings()->getDownloadDirPath());
    QString localFilePath = U2FileDialog::getOpenFileName(this, tr("Select a file to upload"), lod.dir);
    CHECK(!localFilePath.isEmpty(), );
    lod.dir = QFileInfo(localFilePath).absolutePath();

    if (!isFolder) {
        path.pop_back();
    }
    getCloudStorageService()->uploadFile(path, localFilePath, this, [this](const QJsonObject& response) {
        handleCloudStorageResponse(response);
    });
}

void CloudStorageDockWidget::shareItem() {
    QModelIndex currentIndex = getSelectedItemIndex();
    SAFE_POINT(currentIndex.isValid(), "No selection found", );
    auto path = treeView->model()->data(currentIndex, USER_DATA_PATH).value<QList<QString>>();

    QVector<FormFieldDescriptor> fields = {
        {"email", "Recipient Email", "", "Enter the email address of the recipient for sharing."},
        {"sharedName", "Shared File Name", path.last(), "Specify the name of the shared file that will be displayed to the recipient."},
    };
    QMap<QString, QString> fieldValues = GUIUtils::fillFormWithDialog(tr("Share %1 with email").arg(path.last()), fields);
    CHECK(!fieldValues.isEmpty(), );
    QString shareWithEmail = fieldValues["email"];
    if (!CloudStorageService::checkEmail(shareWithEmail)) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("Invalid email: %1").arg(shareWithEmail));
        return;
    }
    if (workspaceService->getCurrentUserEmail() == shareWithEmail) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("You cannot share with yourself."));
        return;
    }
    QString sharedName = fieldValues["sharedName"];
    if (!CloudStorageService::checkCloudStorageEntryName(sharedName)) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("Invalid shared file name: %1").arg(sharedName));
        return;
    }
    getCloudStorageService()->shareEntry(path, shareWithEmail, sharedName, this, [this](const auto& response) {
        handleCloudStorageResponse(response);
    });
}

void CloudStorageDockWidget::handleCloudStorageResponse(const QJsonObject& response) {
    const auto errorMessage = WorkspaceService::getErrorMessageFromResponse(response);
    if (!errorMessage.isEmpty()) {
        QMessageBox::critical(this, L10N::errorTitle(), errorMessage);
    }
}

static bool isSharedFolderOrChild(const QList<QString>& path) {
    return path.startsWith("Shared");
}

void CloudStorageDockWidget::updateActionsState() const {
    const bool hasTreeView = treeView->isVisible();

    bool hasSelection = false;
    bool hasFileSelection = false;
    bool isSharedContentSelected = false;
    bool isTopLevelSharedEntrySelected = false;
    if (hasTreeView) {
        const QModelIndex currentIndex = getSelectedItemIndex();
        hasSelection = currentIndex.isValid();
        const auto* treeModel = treeView->model();
        hasFileSelection = hasSelection && treeModel->data(currentIndex, USER_DATA_IS_FOLDER).toBool() == false;
        if (hasSelection) {
            const auto path = treeModel->data(currentIndex, USER_DATA_PATH).value<QList<QString>>();
            isSharedContentSelected = isSharedFolderOrChild(path);
            isTopLevelSharedEntrySelected = isSharedContentSelected && (path.length() == 2 || path.length() == 3);
        }
    }
    createDirAction->setEnabled(hasTreeView && !isSharedContentSelected);
    deleteAction->setEnabled(hasTreeView && hasSelection && (!isSharedContentSelected || isTopLevelSharedEntrySelected));
    renameAction->setEnabled(hasTreeView && hasSelection && !isSharedContentSelected);
    downloadAction->setEnabled(hasTreeView && hasFileSelection);
    uploadAction->setEnabled(hasTreeView && !isSharedContentSelected);
    shareAction->setEnabled(hasTreeView && hasSelection && !isSharedContentSelected);
}

void CloudStorageDockWidget::updateStateLabelText() {
    disconnect(stateLabel, &QLabel::linkActivated, this, nullptr);
    const auto isLoggedIn = workspaceService->isLoggedIn();
    stateLabel->setText(isLoggedIn
                            ? tr(R"(Loading file list...<br><br><br><a href="logout"><span style=\"color: %1;\">Logout</span></a>)").arg(Theme::hyperlinkColorLabelHtmlStr())
                            : tr(R"(Please <a href="login"><span style=\"color: %1;\">log in to Workspace</span></a> to access cloud storage)").arg(Theme::hyperlinkColorLabelHtmlStr()));

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
