/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2016 UniPro <ugene@unipro.ru>
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

#ifndef _U2_GT_MENU_PRIVATE_MAC_H_
#define _U2_GT_MENU_PRIVATE_MAC_H_


#ifdef __OBJC__
#import <AppKit/AppKit.h>
#endif // __OBJC__

#include "GTGlobals.h"
#include "primitives/PopupChooser.h"

namespace HI {

/**
 * It is supposed that Cocoa is used on OSX
 */
class GTMenuPrivateMac {
public:
    static void clickMainMenuItem(GUITestOpStatus &os, const QStringList &itemPath, Qt::MatchFlag matchFlag = Qt::MatchExactly);
    static void checkMainMenuItemState(GUITestOpStatus &os, const QStringList &itemPath, PopupChecker::CheckOption expectedState);
    static void checkMainMenuItemsState(GUITestOpStatus &os, const QStringList &menuPath, const QStringList &itemsNames, PopupChecker::CheckOption expectedState);

private:
#ifdef __OBJC__
    static NSMenu *clickMenuItem(GUITestOpStatus &os, NSMenu *menu, const QString &itemTitle, Qt::MatchFlag matchFlag);
    static NSMenuItem *getMenuItem(GUITestOpStatus &os, NSMenu *menu, const QString &itemTitle, Qt::MatchFlag matchFlag);
    static void checkMenuItemState(GUITestOpStatus &os, NSMenu *containerMenu, const QString &itemTitle, PopupChecker::CheckOption expectedState);
#endif // __OBJC__NSMenuItem
};

}   // namespace U2

#endif // _U2_GT_MENU_PRIVATE_MAC_H_
