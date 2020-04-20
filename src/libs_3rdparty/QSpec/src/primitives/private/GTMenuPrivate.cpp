/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2017 UniPro <ugene@unipro.ru>
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
#ifdef Q_OS_MAC
#include "GTMenuPrivateMac.h"
#endif
#include "drivers/GTKeyboardDriver.h"
#include "drivers/GTMouseDriver.h"
#include "primitives/GTAction.h"
#include "primitives/GTMainWindow.h"
#include "primitives/PopupChooser.h"
#include "utils/GTUtilsDialog.h"

#include <QMainWindow>
#include <QMenuBar>

namespace HI {

#define GT_CLASS_NAME "GTMenu"

#define GT_METHOD_NAME "clickMainMenuItem"
void GTMenuPrivate::clickMainMenuItem(GUITestOpStatus &os, const QStringList &itemPath, GTGlobals::UseMethod method, Qt::MatchFlag matchFlag) {
    GT_CHECK(itemPath.count() > 1, QString("Menu item path is too short: { %1 }").arg(itemPath.join(" -> ")));
    qWarning("clickMainMenuItem is going to click menu: '%s'", itemPath.join(" -> ").toLocal8Bit().constData());
#ifdef Q_OS_MAC
    Q_UNUSED(method);
    GTMenuPrivateMac::clickMainMenuItem(os, itemPath, matchFlag);
#else
    QStringList cuttedItemPath = itemPath;
    const QString menuName = cuttedItemPath.takeFirst();
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, cuttedItemPath, method, matchFlag));
    showMainMenu(os, menuName, method);
#endif
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkMainMenuItemState"
void GTMenuPrivate::checkMainMenuItemState(GUITestOpStatus &os, const QStringList &itemPath, PopupChecker::CheckOption expectedState) {
    GT_CHECK(itemPath.count() > 1, QString("Menu item path is too short: { %1 }").arg(itemPath.join(" -> ")));

#ifdef Q_OS_MAC
    GTMenuPrivateMac::checkMainMenuItemState(os, itemPath, expectedState);
#else
    QStringList cuttedItemPath = itemPath;
    const QString menuName = cuttedItemPath.takeFirst();
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, cuttedItemPath, expectedState, GTGlobals::UseMouse));
    showMainMenu(os, menuName, GTGlobals::UseMouse);
    GTGlobals::sleep(100);
#endif
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkMainMenuItemState"
void GTMenuPrivate::checkMainMenuItemsState(GUITestOpStatus &os, const QStringList &menuPath, const QStringList &itemsNames, PopupChecker::CheckOption expectedState) {
    GT_CHECK(menuPath.count() > 0, QString("Menu path is too short: { %1 }").arg(menuPath.join(" -> ")));
    GT_CHECK(itemsNames.count() > 0, QString("There are no menu items to check: %1").arg(itemsNames.join(", ")));

#ifdef Q_OS_MAC
    GTMenuPrivateMac::checkMainMenuItemsState(os, menuPath, itemsNames, expectedState);
#else
    QStringList cutMenuPath = menuPath;
    const QString menuName = cutMenuPath.takeFirst();
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, cutMenuPath, itemsNames, expectedState, GTGlobals::UseMouse));
    showMainMenu(os, menuName, GTGlobals::UseMouse);
    GTGlobals::sleep(100);
#endif
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "showMainMenu"
void GTMenuPrivate::showMainMenu(GUITestOpStatus &os, const QString &menuName, GTGlobals::UseMethod m) {

    QMainWindow* mainWindow = NULL;
    QList<QAction*> resultList;
    foreach(QWidget* parent, GTMainWindow::getMainWindowsAsWidget(os)){
        QList<QAction*> list = parent->findChildren<QAction*>();
        bool isContainMenu = false;
        foreach(QAction* act, list){
            QString name = act->text().replace('&',"");
            if(name == menuName){
                resultList<<act;
                isContainMenu = true;
            }
        }
        if (isContainMenu){
            mainWindow = qobject_cast<QMainWindow*>(parent);
        }
    }
    GT_CHECK_RESULT(resultList.count()!=0,"action not found", );
    GT_CHECK_RESULT(resultList.count()<2, QString("There are %1 actions with this text").arg(resultList.count()), );

    QAction *menu = resultList.takeFirst();

    QPoint pos;
    QPoint gPos;
    QString menuText;
    int key = 0;
    int key_pos = 0;

    switch(m) {
    case GTGlobals::UseMouse:
        pos = mainWindow->menuBar()->actionGeometry(menu).center();
        gPos = mainWindow->menuBar()->mapToGlobal(pos);

        GTMouseDriver::moveTo(gPos);
        GTMouseDriver::click();
        break;

    case GTGlobals::UseKey:
        menuText = menu->text();
        key_pos = menuText.indexOf('&');
        key = (menuText.at(key_pos + 1)).toLatin1();

        GTKeyboardDriver::keyClick( key, Qt::AltModifier);
        break;

    default:
        break;
    }

    GTGlobals::sleep(1000);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}   // namespace
