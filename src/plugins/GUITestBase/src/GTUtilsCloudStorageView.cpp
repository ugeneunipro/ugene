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

#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <primitives/GTMenu.h>
#include <primitives/GTWidget.h>
#include <primitives/GTTreeView.h>

#include <QMainWindow>
#include <QSortFilterProxyModel>

#include <U2Core/AppContext.h>

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

void GTUtilsCloudStorageView::checkItemIsPresent(const QList<QString>& path) {
    auto dockWidget= GTWidget::findWidget(DOCK_CLOUD_STORAGE_VIEW);
    auto tree = GTWidget::findTreeView("cloudStorageTreeView", dockWidget);
    GTTreeView::findIndex(tree, path[0]); // Works only with top-level items today.
}

}  // namespace U2
