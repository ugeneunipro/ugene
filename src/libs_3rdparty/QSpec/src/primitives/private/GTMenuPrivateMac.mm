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

#include "GTMenuPrivateMac.h"

namespace HI {

#ifdef __OBJC__

#define GT_CLASS_NAME "GTMenuPrivateMac"

#define GT_METHOD_NAME "clickMainMenuItem"
void GTMenuPrivateMac::clickMainMenuItem(GUITestOpStatus &os, const QStringList &itemPath, Qt::MatchFlag matchFlag) {
    NSMenu *menu = [NSApp mainMenu];
    foreach (const QString &itemTitle, itemPath) {
        GT_CHECK(NULL != menu, QString("Menu not found: '%1'").arg(itemTitle));
        menu = clickMenuItem(os, menu, itemTitle, matchFlag);
        GTGlobals::sleep(1000);
    }
    [menu cancelTrackingWithoutAnimation];
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkMainMenuItemState"
void GTMenuPrivateMac::checkMainMenuItemState(GUITestOpStatus &os, const QStringList &itemPath, PopupChecker::CheckOption expectedState) {
    const QStringList itemContainerMenuPath = itemPath.mid(0, itemPath.size() - 1);
    NSMenu *menu = [NSApp mainMenu];
    foreach (const QString &itemTitle, itemContainerMenuPath) {
        GT_CHECK(NULL != menu, QString("Menu not found: '%1'").arg(itemTitle));
        menu = clickMenuItem(os, menu, itemTitle, Qt::MatchExactly);
        GTGlobals::sleep(500);
    }

    checkMenuItemState(os, menu, itemPath.last(), expectedState);
    [menu cancelTrackingWithoutAnimation];
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkMainMenuItemsState"
void GTMenuPrivateMac::checkMainMenuItemsState(GUITestOpStatus &os, const QStringList &menuPath, const QStringList &itemsNames, PopupChecker::CheckOption expectedState) {
    NSMenu *menu = [NSApp mainMenu];
    foreach (const QString &itemTitle, menuPath) {
        GT_CHECK(NULL != menu, QString("Menu not found: '%1'").arg(itemTitle));
        menu = clickMenuItem(os, menu, itemTitle, Qt::MatchExactly);
        GTGlobals::sleep(500);
    }

    foreach (const QString &itemName, itemsNames) {
        checkMenuItemState(os, menu, itemName, expectedState);
    }
    [menu cancelTrackingWithoutAnimation];
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickMenuItem"
NSMenu * GTMenuPrivateMac::clickMenuItem(GUITestOpStatus &os, NSMenu *menu, const QString &itemTitle, Qt::MatchFlag matchFlag) {
    NSMenuItem *item = getMenuItem(os, menu, itemTitle, matchFlag);
    if (NULL == item) {
        QStringList items;
        for (NSInteger i = 0, itemsCount = [menu  numberOfItems]; i < itemsCount; i++) {
            items << QString::fromNSString([[menu itemAtIndex:i] title]);
        }
        GT_CHECK_RESULT(NULL != item, QString("Menu item not found: '%1', available items: %2")
                        .arg(itemTitle).arg(items.join(", ")), NULL);
    }
    NSMenu *submenu = [item submenu];
    [menu performActionForItemAtIndex:[menu indexOfItem:item]];
    return submenu;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getMenuItem"
NSMenuItem *GTMenuPrivateMac::getMenuItem(GUITestOpStatus &os, NSMenu *menu, const QString &itemTitle, Qt::MatchFlag matchFlag) {
    switch (matchFlag) {
    case Qt::MatchExactly:
        return [menu itemWithTitle:(itemTitle.toNSString())];
    case Qt::MatchContains: {
        NSInteger itemsCount = [menu numberOfItems];
        for (NSInteger i = 0; i < itemsCount; i++) {
            NSMenuItem *menuItem = [menu itemAtIndex:i];
            if (QString::fromNSString([menuItem title]).contains(itemTitle)) {
                return menuItem;
            }
        }
        break;
    }
    default:
        GT_CHECK_RESULT(false, "Match flag is not supported", NULL);
    }
    return NULL;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkMenuItemState"
void GTMenuPrivateMac::checkMenuItemState(GUITestOpStatus &os, NSMenu *containerMenu, const QString &itemTitle, PopupChecker::CheckOption expectedState) {
    switch (expectedState) {
    case PopupChecker::NotExists: {
        NSMenuItem *menuItem = getMenuItem(os, containerMenu, itemTitle, Qt::MatchExactly);
        GT_CHECK(NULL == menuItem, QString("A menu item unexpectedly exists: %1").arg(itemTitle));
        break;
    }

    case PopupChecker::Exists: {
        NSMenuItem *menuItem = getMenuItem(os, containerMenu, itemTitle, Qt::MatchExactly);
        GT_CHECK(NULL != menuItem, QString("A menu item unexpectedly doesn't exist: %1").arg(itemTitle));
        break;
    }

    case PopupChecker::IsEnabled: {
        NSMenuItem *menuItem = getMenuItem(os, containerMenu, itemTitle, Qt::MatchExactly);
        GT_CHECK(NULL != menuItem, QString("A menu item unexpectedly doesn't exist: %1").arg(itemTitle));
        GT_CHECK([menuItem isEnabled], QString("A menu item is unexpectedly disabled: %1").arg(itemTitle));
        break;
    }

    case PopupChecker::IsDisabled: {
        NSMenuItem *menuItem = getMenuItem(os, containerMenu, itemTitle, Qt::MatchExactly);
        GT_CHECK(NULL != menuItem, QString("A menu item unexpectedly doesn't exist: %1").arg(itemTitle));
        GT_CHECK(![menuItem isEnabled], QString("A menu item is unexpectedly enabled: %1").arg(itemTitle));
        break;
    }

    case PopupChecker::IsChecable: {
        GT_CHECK(false, "Not implemented. Do you really need to check is menu item checkable or not?");
        break;
    }

    case PopupChecker::IsChecked: {
        NSMenuItem *menuItem = getMenuItem(os, containerMenu, itemTitle, Qt::MatchExactly);
        GT_CHECK(NULL != menuItem, QString("A menu item unexpectedly doesn't exist: %1").arg(itemTitle));
        GT_CHECK(NSOnState == [menuItem state], QString("A menu item is unexpectedly not checked: %1").arg(itemTitle));
        break;
    }
    }
}

#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}   // namespace

#endif // __OBJC__
