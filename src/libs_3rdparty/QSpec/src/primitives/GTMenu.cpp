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

QMenu* GTMenu::showMainMenu(const QString& menuName) {
    QMainWindow* mainWindow = NULL;
    QList<QAction*> list;
    foreach (QWidget* window, GTMainWindow::getMainWindowsAsWidget()) {
        if (window->findChild<QAction*>(menuName) != NULL) {
            list.append(window->findChild<QAction*>(menuName));
            mainWindow = qobject_cast<QMainWindow*>(window);
        }
    }
    GT_CHECK_RESULT(list.count() != 0, "action not found", NULL);
    GT_CHECK_RESULT(list.count() < 2, QString("There are %1 actions with this text").arg(list.count()), NULL);

    QAction* menu = list.takeFirst();
    GT_CHECK_RESULT(menu != nullptr, QString("menu \"%1\" not found").arg(menuName), NULL);
    QPoint pos = mainWindow->menuBar()->actionGeometry(menu).center();
    QPoint gPos = mainWindow->menuBar()->mapToGlobal(pos);
    GTMouseDriver::moveTo(gPos);
    GTMouseDriver::click();
    GTGlobals::sleep(1000);
    return menu->menu();
}

static QStringList fixMenuItemPath(const QStringList& itemPath) {
    QStringList fixedItemPath = itemPath;
#ifdef Q_OS_DARWIN
    // Some actions are moved to the application menu on mac
#    ifdef _DEBUG
    const QString appName = "ugeneuid";
#    else
    const QString appName = "Unipro UGENE";
#    endif

#    ifdef USE_FAKE_MENU_ITEM_ON_MAC
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
#    endif
#endif
    return fixedItemPath;
}

static bool compare(QString s1, QString s2, Qt::MatchFlag matchFlag) {
    s1.remove('&');
    s2.remove('&');
    switch (matchFlag) {
        case Qt::MatchContains:
            return s1.contains(s2);
        case Qt::MatchExactly:
            return s1 == s2;
        default:
            return false;
    }
}

void GTMenu::clickMainMenuItem(const QStringList& itemPath, GTGlobals::UseMethod popupChooserMethod, Qt::MatchFlag matchFlag) {
    QStringList path = fixMenuItemPath(itemPath);
    GT_LOG("Click main menu path: " + path.join(","));
    GTMenuPrivate::clickMainMenuItem(path, popupChooserMethod, matchFlag);
}

void GTMenu::checkMainMenuItemState(const QStringList& itemPath, PopupChecker::CheckOption expectedState) {
    GTMenuPrivate::checkMainMenuItemState(fixMenuItemPath(itemPath), expectedState);
    GTGlobals::sleep(100);
}

void GTMenu::checkMainMenuItemsState(const QStringList& menuPath, const QStringList& itemsNames, PopupChecker::CheckOption expectedState) {
    const QStringList fixedPath = fixMenuItemPath(menuPath);
    GTMenuPrivate::checkMainMenuItemsState(fixedPath, itemsNames, expectedState);
    GTGlobals::sleep(100);
}

void GTMenu::showContextMenu(QWidget* target) {
    GT_CHECK(target != nullptr, "ground widget is NULL");
    GTWidget::click(target, Qt::RightButton);
    GTGlobals::sleep(1000);
}

QAction* GTMenu::getMenuItem(const QMenu* menu, const QString& itemName, bool byText, Qt::MatchFlag matchFlag) {
    GT_CHECK_RESULT(menu != nullptr, "menu is null", nullptr);

    QAction* action = nullptr;

    // If menu is built dynamically not all actions may be available immediately.
    // In this case wait up to 2 seconds before returning nullptr.
    for (int time = 0; time < 2000 && action == nullptr; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);

        QList<QAction*> menuActions = menu->actions();
        for (QAction* menuAction : qAsConst(menuActions)) {
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

QPoint GTMenu::actionPos(const QMenu* menu, QAction* action) {
    GT_CHECK_RESULT(menu != nullptr, "menu == NULL", QPoint());
    GT_CHECK_RESULT(action != nullptr, "action == NULL", QPoint());

    QPoint p = menu->actionGeometry(action).center();
    return menu->mapToGlobal(p);
}

QAction* GTMenu::clickMenuItem(const QMenu* menu, const QString& itemName, GTGlobals::UseMethod useMethod, bool byText, Qt::MatchFlag matchFlag) {
    GT_LOG("clickMenuItem " + itemName);
    GT_CHECK_RESULT(menu != nullptr, "menu not found", nullptr);
    GT_CHECK_RESULT(!itemName.isEmpty(), "itemName is empty", nullptr);

    QAction* action = getMenuItem(menu, itemName, byText, matchFlag);
    GT_CHECK_RESULT(action != nullptr, "action not found for item " + itemName, nullptr);
    GT_CHECK_RESULT(action->isEnabled(), "action <" + itemName + "> is not enabled", nullptr);

    QPoint cursorPosition = GTMouseDriver::getMousePosition();
    QPoint menuCorner = menu->mapToGlobal(QPoint(0, 0));

    switch (useMethod) {
        case GTGlobals::UseMouse: {
            QPoint actionPosition = actionPos(menu, action);
            bool isVerticalMenu = cursorPosition.y() < menuCorner.y();  // TODO: assuming here that submenu is always lower then menu
            QPoint firstMoveTo = isVerticalMenu ? QPoint(cursorPosition.x(), actionPosition.y()) :  // move by Y first
                                     QPoint(actionPosition.x(), cursorPosition.y());  // move by X first

            GTMouseDriver::moveTo(firstMoveTo);
            GTGlobals::sleep(200);

            GTMouseDriver::moveTo(actionPosition);  // move cursor to action
            GTGlobals::sleep(200);

            QMenu* actionMenu = action->menu();
            bool isSubmenu = actionMenu != nullptr;
            if (isSubmenu) {
                // Hover on Linux, click on Windows & Mac to expand the child menu.
#ifndef Q_OS_LINUX
                GTMouseDriver::click();
#endif
            } else {  // Click the final action.
                GTMouseDriver::click();
            }
            break;
        }
        case GTGlobals::UseKey:
            while (action != menu->activeAction()) {
                GTKeyboardDriver::keyClick(Qt::Key_Down);
            }
            GTKeyboardDriver::keyClick(Qt::Key_Enter, Qt::NoModifier, false);
            break;
        default:
            GT_FAIL("clickMenuItem: unsupported method" + QString::number(useMethod), nullptr);
    }
    GT_LOG("clickMenuItem " + itemName + " DONE");
    GTThread::waitForMainThread();
    auto activePopupMenu = qobject_cast<QMenu*>(QApplication::activePopupWidget());
    return activePopupMenu == nullptr ? nullptr : action;
}

void GTMenu::clickMenuItemPrivate(const QMenu* menu, const QStringList& itemPath, GTGlobals::UseMethod useMethod, bool byText, Qt::MatchFlag matchFlag) {
    GT_CHECK(menu != nullptr, "menu is NULL");
    GT_CHECK(!itemPath.isEmpty(), "itemPath is empty");

    foreach (QString itemName, itemPath) {
        GT_CHECK(menu != nullptr, "menu not found for item " + itemName);
        GTGlobals::sleep(500);
        QAction* action = clickMenuItem(menu, itemName, useMethod, byText, matchFlag);
        menu = action ? action->menu() : nullptr;
    }
}

void GTMenu::clickMenuItemByName(const QMenu* menu, const QStringList& itemPath, GTGlobals::UseMethod m, Qt::MatchFlag matchFlag) {
    clickMenuItemPrivate(menu, itemPath, m, false, matchFlag);
}

void GTMenu::clickMenuItemByText(const QMenu* menu, const QStringList& itemPath, GTGlobals::UseMethod m, Qt::MatchFlag matchFlag) {
    clickMenuItemPrivate(menu, itemPath, m, true, matchFlag);
}

#undef GT_CLASS_NAME

}  // namespace HI
