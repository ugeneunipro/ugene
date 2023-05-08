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

#include "drivers/GTKeyboardDriver.h"
#include "drivers/GTMouseDriver.h"
#include "primitives/GTAction.h"
#include "primitives/GTMainWindow.h"
#include "primitives/PopupChooser.h"
#include "utils/GTUtilsDialog.h"

namespace HI {

#define GT_CLASS_NAME "GTMenu"

#define GT_METHOD_NAME "clickMainMenuItem"
void GTMenuPrivate::clickMainMenuItem(GUITestOpStatus& os, const QStringList& itemPath, GTGlobals::UseMethod method, Qt::MatchFlag matchFlag) {
    GT_CHECK(itemPath.count() > 1, QString("Menu item path is too short: { %1 }").arg(itemPath.join(" -> ")));
    qWarning("clickMainMenuItem is going to click menu: '%s'", itemPath.join(" -> ").toLocal8Bit().constData());

    QStringList cutItemPath = itemPath;
    QString menuName = cutItemPath.takeFirst();
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, cutItemPath, method, matchFlag));
    showMainMenu(os, menuName, method);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkMainMenuItemState"
void GTMenuPrivate::checkMainMenuItemState(GUITestOpStatus& os, const QStringList& itemPath, PopupChecker::CheckOption expectedState) {
    GT_CHECK(itemPath.count() > 1, QString("Menu item path is too short: { %1 }").arg(itemPath.join(" -> ")));

    QStringList cutItemPath = itemPath;
    const QString menuName = cutItemPath.takeFirst();
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, cutItemPath, expectedState, GTGlobals::UseMouse));
    showMainMenu(os, menuName, GTGlobals::UseMouse);
    GTGlobals::sleep(100);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkMainMenuItemState"
void GTMenuPrivate::checkMainMenuItemsState(GUITestOpStatus& os, const QStringList& menuPath, const QStringList& itemsNames, PopupChecker::CheckOption expectedState) {
    GT_CHECK(menuPath.count() > 0, QString("Menu path is too short: { %1 }").arg(menuPath.join(" -> ")));
    GT_CHECK(itemsNames.count() > 0, QString("There are no menu items to check: %1").arg(itemsNames.join(", ")));

    QStringList cutMenuPath = menuPath;
    const QString menuName = cutMenuPath.takeFirst();
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, cutMenuPath, itemsNames, expectedState, GTGlobals::UseMouse));
    showMainMenu(os, menuName, GTGlobals::UseMouse);
    GTGlobals::sleep(100);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "showMainMenu"
void GTMenuPrivate::showMainMenu(GUITestOpStatus& os, const QString& menuName, GTGlobals::UseMethod m) {
    QMainWindow* mainWindow = nullptr;
    QList<QAction*> resultList;
    foreach (QWidget* parent, GTMainWindow::getMainWindowsAsWidget(os)) {
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
    GT_CHECK_RESULT(resultList.count() != 0, "action not found", );
    GT_CHECK_RESULT(resultList.count() < 2, QString("There are %1 actions with this text").arg(resultList.count()), );

    QAction* menu = resultList.takeFirst();
#ifdef Q_OS_DARWIN
    m = GTGlobals::UseMouse;  // On MacOS menu shortcuts do not work by prefix (like Alt-F for the &File).
#endif
    switch (m) {
        case GTGlobals::UseMouse: {
            GT_CHECK_RESULT(mainWindow != nullptr, "mainWindow is null!", );
            QPoint pos = mainWindow->menuBar()->actionGeometry(menu).center();
            QPoint gPos = mainWindow->menuBar()->mapToGlobal(pos);
            GTMouseDriver::click(gPos);
            break;
        }
        case GTGlobals::UseKey: {
            QString menuText = menu->text();
            int hotkeyIndex = menuText.indexOf('&') + 1;
            GT_CHECK_RESULT(hotkeyIndex > 0, "Menu has no hotkey: " + menuText, );
            int key = menuText.at(hotkeyIndex).toLatin1();
            GTKeyboardDriver::keyClick(key, Qt::AltModifier);
            break;
        }
        default:
            break;
    }

    GTGlobals::sleep(1000);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace HI
