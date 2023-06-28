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

#pragma once
#include <QAction>
#include <QMenu>

#include "GTGlobals.h"
#include "primitives/PopupChooser.h"

namespace HI {

class HI_EXPORT GTMenu {
public:
    static QMenu* showMainMenu(const QString& menuName);
    static void clickMainMenuItem(const QStringList& itemPath, GTGlobals::UseMethod popupChooserMethod = GTGlobals::UseMouse, Qt::MatchFlag matchFlag = Qt::MatchExactly);
    static void checkMainMenuItemState(const QStringList& itemPath, PopupChecker::CheckOption expectedState);
    static void checkMainMenuItemsState(const QStringList& menuPath, const QStringList& itemsNames, PopupChecker::CheckOption expectedState);

    /** Activates context menu using right mouse click to the center of the target widget. */
    static void showContextMenu(QWidget* target);

    static void clickMenuItemByName(const QMenu* menu, const QStringList& itemPath, GTGlobals::UseMethod m = GTGlobals::UseMouse, Qt::MatchFlag matchFlag = Qt::MatchExactly);
    static void clickMenuItemByText(const QMenu* menu, const QStringList& itemPath, GTGlobals::UseMethod m = GTGlobals::UseMouse, Qt::MatchFlag matchFlag = Qt::MatchExactly);

    // moves cursor to menu item, clicks on menu item;

    static QAction* clickMenuItem(const QMenu* menu, const QString& itemName, GTGlobals::UseMethod useMethod = GTGlobals::UseMouse, bool byText = false, Qt::MatchFlag matchFlag = Qt::MatchExactly);

    static QAction* getMenuItem(const QMenu* menu, const QString& itemName, bool byText = false, Qt::MatchFlag matchFlag = Qt::MatchExactly);

    // global position of menu action's center
    static QPoint actionPos(const QMenu* menu, QAction* action);

    static const QString FILE;
    static const QString ACTIONS;
    static const QString SETTINGS;
    static const QString TOOLS;
    static const QString WINDOW;
    static const QString HELP;

private:
    static void clickMenuItemPrivate(const QMenu* menu, const QStringList& itemName, GTGlobals::UseMethod m = GTGlobals::UseMouse, bool byText = false, Qt::MatchFlag matchFlag = Qt::MatchExactly);
};

}  // namespace HI
