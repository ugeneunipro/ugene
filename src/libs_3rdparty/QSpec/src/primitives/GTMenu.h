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

#ifndef GTMENU_H
#define GTMENU_H

#include "GTGlobals.h"
#include "primitives/PopupChooser.h"

#include <QAction>
#include <QMenu>

namespace HI {

class HI_EXPORT GTMenu {
public:
    static QMenu* showMainMenu(GUITestOpStatus &os, const QString &menuName, GTGlobals::UseMethod m = GTGlobals::UseMouse);  // should be removed in Qt5, use clickMainMenuItem instead
    static void clickMainMenuItem(GUITestOpStatus &os, const QStringList &itemPath, GTGlobals::UseMethod method = GTGlobals::UseMouse, Qt::MatchFlag matchFlag = Qt::MatchExactly);
    static void checkMainMenuItemState(GUITestOpStatus &os, const QStringList &itemPath, PopupChecker::CheckOption expectedState);
    static void checkMainMenuItemsState(GUITestOpStatus &os, const QStringList &menuPath, const QStringList &itemsNames, PopupChecker::CheckOption expectedState);
    static QMenu* showContextMenu(GUITestOpStatus &os, QWidget *ground, GTGlobals::UseMethod m = GTGlobals::UseMouse);

    static void clickMenuItemByName(GUITestOpStatus &os, const QMenu *menu, const QStringList &itemPath, GTGlobals::UseMethod m = GTGlobals::UseMouse, Qt::MatchFlag matchFlag = Qt::MatchExactly);
    static void clickMenuItemByText(GUITestOpStatus &os, const QMenu *menu, const QStringList &itemPath, GTGlobals::UseMethod m = GTGlobals::UseMouse, Qt::MatchFlag matchFlag = Qt::MatchExactly);

    // moves cursor to menu item, clicks on menu item;

    static QAction* clickMenuItem(GUITestOpStatus &os, const QMenu *menu, const QString &itemName, GTGlobals::UseMethod m = GTGlobals::UseMouse, bool byText = false, Qt::MatchFlag matchFlag = Qt::MatchExactly);

    static QAction* getMenuItem(GUITestOpStatus &os, const QMenu* menu, const QString &itemName, bool byText = false, Qt::MatchFlag matchFlag = Qt::MatchExactly);

    // global position of menu action's center
    static QPoint actionPos(GUITestOpStatus &os, const QMenu* menu, QAction* action);

    static const QString FILE;
    static const QString ACTIONS;
    static const QString SETTINGS;
    static const QString TOOLS;
    static const QString WINDOW;
    static const QString HELP;

private:
    static void clickMenuItemPrivate(GUITestOpStatus &os, const QMenu *menu, const QStringList &itemName, GTGlobals::UseMethod m = GTGlobals::UseMouse,
                                     bool byText = false, Qt::MatchFlag matchFlag = Qt::MatchExactly);
};

}
#endif // GTMENU_H
