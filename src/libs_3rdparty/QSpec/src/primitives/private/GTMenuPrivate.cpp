/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "GTMenuPrivate.h"

#include <QMainWindow>
#include <QMenuBar>

#include "drivers/GTMouseDriver.h"
#include "primitives/GTAction.h"
#include "primitives/GTMainWindow.h"
#include "primitives/PopupChooser.h"
#include "utils/GTUtilsDialog.h"

namespace HI {

#define GT_CLASS_NAME "GTMenu"

void GTMenuPrivate::clickMainMenuItem(const QStringList& itemPath, GTGlobals::UseMethod popupChooserMethod, Qt::MatchFlag matchFlag) {
    GT_CHECK(itemPath.count() > 1, QString("Menu item path validation: { %1 }").arg(itemPath.join(" -> ")));
    QStringList cutItemPath = itemPath;
    QString menuName = cutItemPath.takeFirst();
    GTUtilsDialog::waitForDialog(new PopupChooserByText(cutItemPath, popupChooserMethod, matchFlag));
    showMainMenu(menuName);
}

void GTMenuPrivate::checkMainMenuItemState(const QStringList& itemPath, PopupChecker::CheckOption expectedState) {
    GT_CHECK(itemPath.count() > 1, QString("Menu item path validation: { %1 }").arg(itemPath.join(" -> ")));

    QStringList cutItemPath = itemPath;
    const QString menuName = cutItemPath.takeFirst();
    GTUtilsDialog::waitForDialog(new PopupCheckerByText(cutItemPath, expectedState, GTGlobals::UseMouse));
    showMainMenu(menuName);
    GTGlobals::sleep(100);
}

void GTMenuPrivate::checkMainMenuItemsState(const QStringList& menuPath, const QStringList& itemsNames, PopupChecker::CheckOption expectedState) {
    GT_CHECK(menuPath.count() > 0, QString("Menu path is too short: { %1 }").arg(menuPath.join(" -> ")));
    GT_CHECK(itemsNames.count() > 0, QString("There are no menu items to check: %1").arg(itemsNames.join(", ")));

    QStringList cutMenuPath = menuPath;
    const QString menuName = cutMenuPath.takeFirst();
    GTUtilsDialog::waitForDialog(new PopupCheckerByText(cutMenuPath, itemsNames, expectedState, GTGlobals::UseMouse));
    showMainMenu(menuName);
    GTGlobals::sleep(100);
}

void GTMenuPrivate::showMainMenu(const QString& menuName) {
    QMainWindow* mainWindow = nullptr;
    QList<QAction*> resultList;
    foreach (QWidget* parent, GTMainWindow::getMainWindowsAsWidget()) {
        QList<QAction*> list = parent->findChildren<QAction*>();
        bool isContainMenu = false;
        for (QAction* act : qAsConst(list)) {
            QString name = act->text().replace('&', "");
            if (name == menuName) {
                resultList << act;
                isContainMenu = true;
            }
        }
        if (isContainMenu) {
            mainWindow = qobject_cast<QMainWindow*>(parent);
        }
    }
    GT_CHECK_RESULT(!resultList.isEmpty(), "menu action list is not empty", );
    GT_CHECK_RESULT(resultList.count() < 2, QString("There are %1 actions with this text").arg(resultList.count()), );

    QAction* menu = resultList.takeFirst();
    GT_CHECK_RESULT(mainWindow != nullptr, "mainWindow is null!", );
    QPoint pos = mainWindow->menuBar()->actionGeometry(menu).center();
    QPoint gPos = mainWindow->menuBar()->mapToGlobal(pos);
    GTMouseDriver::click(gPos);
    GTGlobals::sleep(1000);
}

#undef GT_CLASS_NAME

}  // namespace HI
