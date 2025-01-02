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

#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTMenu.h>
#include <primitives/GTTreeView.h>
#include <primitives/GTWidget.h>
#include <system/GTFile.h>

#include <QFileInfo>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/MainWindow.h>

#include "GTUtilsCloudStorageView.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsCloudStorageView"

void GTUtilsCloudStorageView::toggleDockView() {
    MainWindow* mw = AppContext::getMainWindow();
    GT_CHECK(mw != nullptr, "MainWindow is NULL");
    QMainWindow* qmw = mw->getQMainWindow();
    GT_CHECK(qmw != nullptr, "QMainWindow is NULL");

    bool isVisibleBefore = GTWidget::findWidget(DOCK_CLOUD_STORAGE_VIEW, nullptr, {false}) != nullptr;
    GTWidget::click(GTWidget::findWidget(QString("doc_label__") + DOCK_CLOUD_STORAGE_VIEW));
    bool isVisibleAfter = GTWidget::findWidget(DOCK_CLOUD_STORAGE_VIEW, nullptr, {false}) != nullptr;

    GT_CHECK(isVisibleBefore != isVisibleAfter, "DOCK_CLOUD_STORAGE_VIEW widget visibility is not changed")
}

void GTUtilsCloudStorageView::clickLogin() {
    GTMenu::clickMainMenuItem({"File", "Login to Workspace"});
    GTMenu::checkMainMenuItemState({"File", "Logout from Workspace"}, HI::PopupChecker::IsEnabled);
}

void GTUtilsCloudStorageView::clickLogout() {
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::No));
    GTMenu::clickMainMenuItem({"File", "Logout from Workspace"});
    GTMenu::checkMainMenuItemState({"File", "Login to Workspace"}, HI::PopupChecker::IsEnabled);
}

QModelIndex GTUtilsCloudStorageView::checkItemIsPresent(const QList<QString>& path) {
    GT_LOG("GTUtilsCloudStorageView::checkItemIsPresent: [" + path.join("/") + "]");
    QTreeView* tree = getStorageTreeView();
    return GTTreeView::findIndexWithWait(tree, QVariant::fromValue(path), Qt::ItemDataRole(Qt::UserRole + 3));
}

QModelIndex GTUtilsCloudStorageView::checkItemIsShared(const QList<QString>& path, const QString& email) {
    GT_LOG("GTUtilsCloudStorageView::checkItemIsShared: [" + path.join("/") + "]: " + email);
    auto modelIndex = checkItemIsPresent(path);
    auto sharedEmails = modelIndex.data(Qt::ItemDataRole(Qt::UserRole + 6)).value<QList<QString>>();
    GT_CHECK_RESULT(sharedEmails.contains(email), "Shared email is not found: " + email, modelIndex);
    return modelIndex;
}

QModelIndex GTUtilsCloudStorageView::checkItemIsNotShared(const QList<QString>& path, const QString& email) {
    GT_LOG("GTUtilsCloudStorageView::checkItemIsNotShared: [" + path.join("/") + "]: " + email);
    auto modelIndex = checkItemIsPresent(path);
    auto sharedEmails = modelIndex.data(Qt::ItemDataRole(Qt::UserRole + 6)).value<QList<QString>>();
    GT_CHECK_RESULT(!sharedEmails.contains(email), "Shared email is present: " + email, modelIndex);
    return modelIndex;
}

void GTUtilsCloudStorageView::checkItemIsNotPresent(const QList<QString>& path) {
    GT_LOG("GTUtilsCloudStorageView::checkItemIsNotPresent: [" + path.join("/") + "]");
    QTreeView* tree = getStorageTreeView();
    GTTreeView::checkItemIsNotPresentWithWait(tree, QVariant::fromValue(path), Qt::ItemDataRole(Qt::UserRole + 3));
}

QTreeView* GTUtilsCloudStorageView::getStorageTreeView() {
    auto dockWidget = GTWidget::findWidget(DOCK_CLOUD_STORAGE_VIEW);
    return GTWidget::findTreeView("cloudStorageTreeView", dockWidget);
}

void GTUtilsCloudStorageView::renameItem(const QList<QString>& path, const QString& newName) {
    QTreeView* tree = getStorageTreeView();
    QModelIndex index = checkItemIsPresent(path);

    GTTreeView::click(tree, index);

    GTUtilsDialog::add(new PopupChooser({"cloudStorageRenameAction"}, GTGlobals::UseMouse));
    GTUtilsDialog::add(new InputDialogFiller(newName));
    GTMouseDriver::click(Qt::RightButton);

    QList<QString> renamedPath = path;
    renamedPath[renamedPath.length() - 1] = newName;
    checkItemIsPresent(renamedPath);
}

/** Clicks to the empty area to drop focus and selection */
static void clickEmptyAreaInTreeView(QTreeView* tree) {
    GTMouseDriver::click(tree->mapToGlobal(tree->rect().bottomLeft() + QPoint(20, -20)));
}

void GTUtilsCloudStorageView::createDir(const QList<QString>& path) {
    QTreeView* tree = getStorageTreeView();
    if (path.length() > 1) {
        QList<QString> parentPath = path;
        parentPath.pop_back();
        QModelIndex parentDirIndex = checkItemIsPresent(parentPath);
        GTTreeView::click(tree, parentDirIndex);
    } else {
        clickEmptyAreaInTreeView(tree);
    }

    GTUtilsDialog::add(new PopupChooser({"cloudStorageCreateDirAction"}, GTGlobals::UseMouse));
    GTUtilsDialog::add(new InputDialogFiller(path.last()));
    GTMouseDriver::click(Qt::RightButton);

    checkItemIsPresent(path);
}

void GTUtilsCloudStorageView::deleteEntry(const QList<QString>& path) {
    QTreeView* tree = getStorageTreeView();
    QModelIndex index = checkItemIsPresent(path);

    GTUtilsDialog::add(new PopupChooser({"cloudStorageDeleteAction"}, GTGlobals::UseMouse));
    GTUtilsDialog::add(new MessageBoxDialogFiller(QMessageBox::Yes, "Do you want to delete"));
    GTTreeView::click(tree, index);
    GTMouseDriver::click(Qt::RightButton);

    checkItemIsNotPresent(path);
}

void GTUtilsCloudStorageView::uploadFile(const QList<QString>& dirPath, const QString& localFileUrl) {
    QTreeView* tree = getStorageTreeView();
    if (!dirPath.isEmpty()) {
        QModelIndex index = checkItemIsPresent(dirPath);
        GTTreeView::click(tree, index);
    } else {
        clickEmptyAreaInTreeView(tree);
    }

    GTUtilsDialog::add(new PopupChooser({"cloudStorageUploadAction"}, GTGlobals::UseMouse));
    GTUtilsDialog::add(new GTFileDialogUtils(localFileUrl));
    GTMouseDriver::click(Qt::RightButton);

    QList<QString> uploadedPath = dirPath;
    uploadedPath.append(QFileInfo(localFileUrl).fileName());
    checkItemIsPresent(uploadedPath);
}

void GTUtilsCloudStorageView::downloadFileWithDoubleClick(const QList<QString>& dirPath, int expectedFileSize) {
    QTreeView* tree = getStorageTreeView();
    QModelIndex index = checkItemIsPresent(dirPath);
    GTTreeView::doubleClick(tree, index);
    QString downloadedFilePath = AppContext::getAppSettings()->getUserAppsSettings()->getDownloadDirPath() + "/" + dirPath.last();
    GTFile::checkFileExistsWithWait(downloadedFilePath, 20000, expectedFileSize);
}

void GTUtilsCloudStorageView::shareItem(const QList<QString>& path, const QString& email) {
    QTreeView* tree = getStorageTreeView();
    QModelIndex index = checkItemIsPresent(path);

    GTTreeView::click(tree, index);

    GTUtilsDialog::add(new PopupChooser({"cloudStorageShareAction"}, GTGlobals::UseMouse));
    GTUtilsDialog::add(new InputDialogFiller(email, "formDialogField-email"));
    GTMouseDriver::click(Qt::RightButton);

    checkItemIsShared(path, email);
}

void GTUtilsCloudStorageView::unshareItem(const QList<QString>& path, const QString& email) {
    QTreeView* tree = getStorageTreeView();
    QModelIndex index = checkItemIsPresent(path);

    GTTreeView::click(tree, index);

    GTUtilsDialog::add(new PopupChooserByText({"Stop sharing with ...", email}, GTGlobals::UseMouse));
    GTMouseDriver::click(Qt::RightButton);

    checkItemIsNotShared(path, email);
}

}  // namespace U2
