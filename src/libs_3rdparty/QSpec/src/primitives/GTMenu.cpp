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

#include "primitives/GTMenu.h"
#include <utils/GTThread.h>

#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>

#include "drivers/GTKeyboardDriver.h"
#include "drivers/GTMouseDriver.h"
#include "primitives/GTMainWindow.h"
#include "primitives/GTWidget.h"
#include "primitives/private/GTMenuPrivate.h"

namespace HI {

#define GT_CLASS_NAME "GTMenu"

#define GT_METHOD_NAME "showMainMenu"
QMenu *GTMenu::showMainMenu(GUITestOpStatus &os, const QString &menuName, GTGlobals::UseMethod m) {
    QMainWindow *mainWindow = NULL;
    QList<QAction *> list;
    foreach (QWidget *window, GTMainWindow::getMainWindowsAsWidget(os)) {
        if (window->findChild<QAction *>(menuName) != NULL) {
            list.append(window->findChild<QAction *>(menuName));
            mainWindow = qobject_cast<QMainWindow *>(window);
        }
    }
    GT_CHECK_RESULT(list.count() != 0, "action not found", NULL);
    GT_CHECK_RESULT(list.count() < 2, QString("There are %1 actions with this text").arg(list.count()), NULL);

    QAction *menu = list.takeFirst();

    GT_CHECK_RESULT(menu != NULL, QString("menu \"%1\" not found").arg(menuName), NULL);

    QPoint pos;
    QPoint gPos;
    QString menuText;
    int key = 0, key_pos = 0;

    switch (m) {
    case GTGlobals::UseMouse:
        pos = mainWindow->menuBar()->actionGeometry(menu).center();
        gPos = mainWindow->menuBar()->mapToGlobal(pos);

        GTMouseDriver::moveTo(gPos);
        GTMouseDriver::click();
        break;

    case GTGlobals::UseKeyBoard:
    case GTGlobals::UseKey:
        menuText = menu->text();
        key_pos = menuText.indexOf('&');
        key = (menuText.at(key_pos + 1)).toLatin1();

        GTKeyboardDriver::keyClick(key, Qt::AltModifier);
        break;

    default:
        break;
    }

    GTGlobals::sleep(1000);

    return menu->menu();
}
#undef GT_METHOD_NAME

namespace {

QStringList fixMenuItemPath(const QStringList &itemPath) {
    QStringList fixedItemPath = itemPath;
#ifdef Q_OS_MAC
    // Some actions are moved to the application menu on mac
#    ifdef _DEBUG
    const QString appName = "ugeneuid";
#    else
    const QString appName = "Unipro UGENE";
#    endif
    const QString menuName = "Apple";

    static const QStringList appSettingsPath = QStringList() << "Settings"
                                                             << "Preferences...";
    if (appSettingsPath == itemPath) {
        fixedItemPath = QStringList() << menuName << "Preferences...";
    }

    static const QStringList aboutPath = QStringList() << "Help"
                                                       << "About";
    if (aboutPath == itemPath) {
        fixedItemPath = QStringList() << menuName << "About " + appName;
    }
#endif
    return fixedItemPath;
}
bool compare(QString s1, QString s2, Qt::MatchFlag mathcFlag, bool replaceSpecSymbol = true) {
    if (replaceSpecSymbol) {
        s1.remove('&');
        s2.remove('&');
    }
    switch (mathcFlag) {
    case Qt::MatchContains:
        return s1.contains(s2);
    case Qt::MatchExactly:
        return s1 == s2;
    default:
        return false;
    }
}

}    // namespace

#define GT_METHOD_NAME "clickMainMenuItem"
void GTMenu::clickMainMenuItem(GUITestOpStatus &os, const QStringList &itemPath, GTGlobals::UseMethod method, Qt::MatchFlag matchFlag) {
    GTMenuPrivate::clickMainMenuItem(os, fixMenuItemPath(itemPath), method, matchFlag);
    GTGlobals::sleep(100);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkMainMenuItemState"
void GTMenu::checkMainMenuItemState(GUITestOpStatus &os, const QStringList &itemPath, PopupChecker::CheckOption expectedState) {
    GTMenuPrivate::checkMainMenuItemState(os, fixMenuItemPath(itemPath), expectedState);
    GTGlobals::sleep(100);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkMainMenuItemState"
void GTMenu::checkMainMenuItemsState(GUITestOpStatus &os, const QStringList &menuPath, const QStringList &itemsNames, PopupChecker::CheckOption expectedState) {
    const QStringList fixedPath = fixMenuItemPath(menuPath);
    GTMenuPrivate::checkMainMenuItemsState(os, fixedPath, itemsNames, expectedState);
    GTGlobals::sleep(100);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "showContextMenu"
QMenu *GTMenu::showContextMenu(GUITestOpStatus &os, QWidget *ground, GTGlobals::UseMethod m) {
    GT_CHECK_RESULT(ground != NULL, "ground widget is NULL", NULL);

    switch (m) {
    case GTGlobals::UseMouse:
        GTWidget::click(os, ground, Qt::RightButton);
        break;

    case GTGlobals::UseKey:

        break;
    default:
        break;
    }

    GTGlobals::sleep(1000);

    QMenu *menu = static_cast<QMenu *>(QApplication::activePopupWidget());
    return menu;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getMenuItem"
QAction *GTMenu::getMenuItem(GUITestOpStatus &os, const QMenu *menu, const QString &itemName, bool byText, Qt::MatchFlag matchFlag) {
    GT_CHECK_RESULT(menu != nullptr, "menu is null", nullptr);

    QAction *action = nullptr;

    // If menu is built dynamically not all actions may be available immidiately.
    // In this case wait up to 2 seconds before returning nullptr.
    for (int time = 0; time < 2000 && action == nullptr; time += GT_OP_CHECK_MILLIS) {
        if (time > 0) {
            qDebug("GT_DEBUG_MESSAGE: Action not found, waiting...");
        }
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);

        QList<QAction *> menuActions = menu->actions();
        foreach (QAction *menuAction, menuActions) {
            QString value = byText ? menuAction->text() : menuAction->objectName();
            qDebug("GT_DEBUG_MESSAGE: Action by %s: '%s'", byText ? "text" : "object name", value.toLocal8Bit().constData());
            if (compare(value, itemName, matchFlag)) {
                qDebug("GT_DEBUG_MESSAGE: Found action by text");
                action = menuAction;
                break;
            }
        }
    }

    if (action == nullptr) {
        qDebug("GT_DEBUG_MESSAGE: Not found action");
    }
    return action;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "actionPos"
QPoint GTMenu::actionPos(GUITestOpStatus &os, const QMenu *menu, QAction *action) {
    GT_CHECK_RESULT(menu != nullptr, "menu == NULL", QPoint());
    GT_CHECK_RESULT(action != nullptr, "action == NULL", QPoint());

    QPoint p = menu->actionGeometry(action).center();
    return menu->mapToGlobal(p);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickMenuItem"
QAction *GTMenu::clickMenuItem(GUITestOpStatus &os, const QMenu *menu, const QString &itemName, GTGlobals::UseMethod m, bool byText, Qt::MatchFlag matchFlag) {
    GT_CHECK_RESULT(menu != nullptr, "menu not found", nullptr);
    GT_CHECK_RESULT(!itemName.isEmpty(), "itemName is empty", nullptr);

    QAction *action = getMenuItem(os, menu, itemName, byText, matchFlag);
    GT_CHECK_RESULT(action != nullptr, "action not found for item " + itemName, nullptr);
    GT_CHECK_RESULT(action->isEnabled(), "action <" + itemName + "> is not enabled", nullptr);

    QPoint cursorPosition = GTMouseDriver::getMousePosition();
    QPoint menuCorner = menu->mapToGlobal(QPoint(0, 0));

    switch (m) {
    case GTGlobals::UseMouse: {
        QPoint actionPosition = actionPos(os, menu, action);
        bool isVerticalMenu = cursorPosition.y() < menuCorner.y();    // TODO: assuming here that submenu is always lower then menu
        QPoint firstMoveTo = isVerticalMenu ? QPoint(cursorPosition.x(), actionPosition.y()) :  // move by Y first
                                              QPoint(actionPosition.x(), cursorPosition.y());   // move by X first

        GTMouseDriver::moveTo(firstMoveTo);
        GTGlobals::sleep(200);

        GTMouseDriver::moveTo(actionPosition);    // move cursor to action
        GTGlobals::sleep(200);


#ifdef Q_OS_WIN
        GTMouseDriver::click();
#else
        QMenu *actionMenu = action->menu();
        bool isClickingSubMenu = actionMenu != nullptr;
        if (!isClickingSubMenu) {
            GTMouseDriver::click();
        }
#endif
        break;
    }
    case GTGlobals::UseKey:
        while (action != menu->activeAction()) {
            GTKeyboardDriver::keyClick(Qt::Key_Down);
        }
        GTKeyboardDriver::keyClick(Qt::Key_Enter, Qt::NoModifier, false);
        break;
    default:
        break;
    }
    GTThread::waitForMainThread();
    QMenu *activePopupMenu = qobject_cast<QMenu *>(QApplication::activePopupWidget());
    return activePopupMenu == nullptr ? nullptr : action;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickMenuItemPrivate"
void GTMenu::clickMenuItemPrivate(GUITestOpStatus &os, const QMenu *menu, const QStringList &itemPath, GTGlobals::UseMethod useMethod, bool byText, Qt::MatchFlag matchFlag) {
    GT_CHECK(menu != nullptr, "menu is NULL");
    GT_CHECK(!itemPath.isEmpty(), "itemPath is empty");

    foreach (QString itemName, itemPath) {
        GT_CHECK(menu != nullptr, "menu not found for item " + itemName);
        GTGlobals::sleep(500);
        QAction *action = clickMenuItem(os, menu, itemName, useMethod, byText, matchFlag);
        menu = action ? action->menu() : nullptr;
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickMenuItemByName"
void GTMenu::clickMenuItemByName(GUITestOpStatus &os, const QMenu *menu, const QStringList &itemPath, GTGlobals::UseMethod m, Qt::MatchFlag matchFlag) {
    clickMenuItemPrivate(os, menu, itemPath, m, false, matchFlag);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickMenuItemByText"
void GTMenu::clickMenuItemByText(GUITestOpStatus &os, const QMenu *menu, const QStringList &itemPath, GTGlobals::UseMethod m, Qt::MatchFlag matchFlag) {
    clickMenuItemPrivate(os, menu, itemPath, m, true, matchFlag);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}    // namespace HI
