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

#include "primitives/PopupChooser.h"

#include <QApplication>
#include <QMenu>

#include "GTWidget.h"
#include "drivers/GTKeyboardDriver.h"
#include "drivers/GTMouseDriver.h"
#include "primitives/GTMenu.h"

namespace HI {
#define GT_CLASS_NAME "PopupChooser"

PopupChooser::PopupChooser(GUITestOpStatus& os, const QStringList& namePath, GTGlobals::UseMethod useMethod)
    : Filler(os, GUIDialogWaiter::WaitSettings(QString(), GUIDialogWaiter::DialogType::Popup)),
      namePath(namePath),
      useMethod(useMethod) {
}

#define GT_METHOD_NAME "getMenuPopup"
QMenu* PopupChooser::getMenuPopup(GUITestOpStatus& os) {
    GTGlobals::sleep(100);  // TODO: do we need this sleep?
    GTMouseDriver::release();  // TODO: do we need this release?
    return GTWidget::getActivePopupMenu(os);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "commonScenario"
void PopupChooser::commonScenario() {
    QMenu* activePopupMenu = getMenuPopup(os);
    if (namePath.isEmpty()) {
        clickEsc(os);
        return;
    }
    GTMenu::clickMenuItemByName(os, activePopupMenu, namePath, useMethod);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickEsc"
void PopupChooser::clickEsc(GUITestOpStatus&) {
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

#define GT_CLASS_NAME "PopupChooserByText"

PopupChooserByText::PopupChooserByText(GUITestOpStatus& os, const QStringList& namePath, GTGlobals::UseMethod useMethod, Qt::MatchFlag matchFlag)
    : Filler(os, GUIDialogWaiter::WaitSettings(QString(), GUIDialogWaiter::DialogType::Popup)),
      namePath(namePath),
      useMethod(useMethod),
      matchFlag(matchFlag) {
}

#define GT_METHOD_NAME "commonScenario"
void PopupChooserByText::commonScenario() {
    QMenu* activePopupMenu = PopupChooser::getMenuPopup(os);
    if (namePath.isEmpty()) {
        PopupChooser::clickEsc(os);
        return;
    }
    GTMenu::clickMenuItemByText(os, activePopupMenu, namePath, useMethod, matchFlag);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "PopupChecker"

PopupChecker::PopupChecker(GUITestOpStatus& os, CustomScenario* scenario)
    : Filler(os, GUIDialogWaiter::WaitSettings(QString(), GUIDialogWaiter::DialogType::Popup), scenario), useMethod(GTGlobals::UseMouse) {
}

PopupChecker::PopupChecker(GUITestOpStatus& os, const QStringList& namePath, CheckOptions options, GTGlobals::UseMethod useMethod)
    : Filler(os, GUIDialogWaiter::WaitSettings(QString(), GUIDialogWaiter::DialogType::Popup)),
      namePath(namePath),
      options(options),
      useMethod(useMethod) {
}

#define GT_METHOD_NAME "commonScenario"
void PopupChecker::commonScenario() {
    QMenu* activePopupMenu = PopupChooser::getMenuPopup(os);
    if (namePath.isEmpty()) {
        PopupChooser::clickEsc(os);
        return;
    }
    QAction* act;
    QString actName;
    if (namePath.size() > 1) {
        actName = namePath.takeLast();
        GTMenu::clickMenuItemByName(os, activePopupMenu, namePath, useMethod);
        QMenu* activePopupMenuToCheck = qobject_cast<QMenu*>(QApplication::activePopupWidget());
        act = GTMenu::getMenuItem(os, activePopupMenuToCheck, actName);
    } else {
        QMenu* activePopupMenuToCheck = qobject_cast<QMenu*>(QApplication::activePopupWidget());
        actName = namePath.last();
        act = GTMenu::getMenuItem(os, activePopupMenuToCheck, actName);
    }
    if (options.testFlag(Exists)) {
        GT_CHECK(act != NULL, "action '" + actName + "' not found");
        qDebug("GT_DEBUG_MESSAGE options.testFlag(Exists)");
    } else {
        GT_CHECK(act == NULL, "action '" + actName + "' unexpectedly found");
    }
    if (options.testFlag(IsEnabled)) {
        GT_CHECK(act->isEnabled(), "action '" + act->objectName() + "' is not enabled");
        qDebug("GT_DEBUG_MESSAGE options.testFlag(IsEnabled)");
    }
    if (options.testFlag(IsDisabled)) {
        GT_CHECK(!act->isEnabled(), "action '" + act->objectName() + "' is enabled");
        qDebug("GT_DEBUG_MESSAGE options.testFlag(IsDisabled");
    }
    if (options.testFlag(IsCheckable)) {
        GT_CHECK(act->isCheckable(), "action '" + act->objectName() + "' is not checkable");
        qDebug("GT_DEBUG_MESSAGE options.testFlag(IsCheckable)");
    }
    if (options.testFlag(IsChecked)) {
        GT_CHECK(act->isChecked(), "action '" + act->objectName() + "' is not checked");
        qDebug("GT_DEBUG_MESSAGE options.testFlag(IsChecked)");
    }
    if (options.testFlag(PopupChecker::IsUnchecked)) {
        GT_CHECK(!act->isChecked(), "action '" + act->objectName() + "' is checked");
        qDebug("GT_DEBUG_MESSAGE options.testFlag(IsUnchecked)");
    }
    for (int i = 0; i < namePath.size(); i++) {
        PopupChooser::clickEsc(os);
        GTGlobals::sleep(250);
    }
    PopupChooser::clickEsc(os);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

#define GT_CLASS_NAME "PopupCheckerByText"

PopupCheckerByText::PopupCheckerByText(GUITestOpStatus& os, CustomScenario* scenario)
    : Filler(os, GUIDialogWaiter::WaitSettings(QString(), GUIDialogWaiter::DialogType::Popup), scenario) {
}

PopupCheckerByText::PopupCheckerByText(GUITestOpStatus& os,
                                       const QStringList& namePath,
                                       PopupChecker::CheckOptions options,
                                       GTGlobals::UseMethod useMethod,
                                       Qt::MatchFlag _matchFlag)
    : Filler(os, GUIDialogWaiter::WaitSettings(QString(), GUIDialogWaiter::DialogType::Popup)),
      menuPath(namePath.mid(0, namePath.size() - 1)),
      itemsNames(namePath.isEmpty() ? "" : namePath.last()),
      options(options),
      useMethod(useMethod),
      matchFlag(_matchFlag) {
}

PopupCheckerByText::PopupCheckerByText(GUITestOpStatus& os,
                                       const QStringList& menuPath,
                                       const QStringList& itemsNames,
                                       PopupChecker::CheckOptions options,
                                       GTGlobals::UseMethod useMethod,
                                       Qt::MatchFlag _matchFlag)
    : Filler(os, GUIDialogWaiter::WaitSettings(QString(), GUIDialogWaiter::DialogType::Popup)),
      menuPath(menuPath),
      itemsNames(itemsNames),
      options(options),
      useMethod(useMethod),
      matchFlag(_matchFlag) {
}

PopupCheckerByText::PopupCheckerByText(GUITestOpStatus& os,
                                       const QStringList& menuPath,
                                       const QMap<QString, QKeySequence>& namesAndShortcuts,
                                       PopupChecker::CheckOptions options,
                                       GTGlobals::UseMethod useMethod,
                                       Qt::MatchFlag _matchFlag)
    : PopupCheckerByText(os, menuPath, namesAndShortcuts.keys(), options, useMethod, _matchFlag) {
    itemsShortcuts = namesAndShortcuts.values();
}

#define GT_METHOD_NAME "commonScenario"
void PopupCheckerByText::commonScenario() {
    QMenu* activePopupMenu = PopupChooser::getMenuPopup(os);
    if (menuPath.isEmpty()) {
        PopupChooser::clickEsc(os);
        return;
    }
    QAction* act = NULL;
    if (!menuPath.isEmpty()) {
        GTMenu::clickMenuItemByText(os, activePopupMenu, menuPath, useMethod, matchFlag);
    }

    QMenu* activePopupMenuToCheck = qobject_cast<QMenu*>(QApplication::activePopupWidget());

    foreach (const QString& itemName, itemsNames) {
        act = GTMenu::getMenuItem(os, activePopupMenuToCheck, itemName, true, matchFlag);
        if (options.testFlag(PopupChecker::Exists)) {
            GT_CHECK(act != NULL, "action '" + itemName + "' not found");
            qDebug("GT_DEBUG_MESSAGE options.testFlag(Exists)");
        } else {
            GT_CHECK(act == NULL, "action '" + itemName + "' unexpectedly found");
        }

        if (options.testFlag(PopupChecker::IsEnabled)) {
            GT_CHECK(act->isEnabled(), "action '" + act->objectName() + "' is not enabled");
            qDebug("GT_DEBUG_MESSAGE options.testFlag(IsEnabled)");
        }

        if (options.testFlag(PopupChecker::IsDisabled)) {
            GT_CHECK(!act->isEnabled(), "action '" + act->objectName() + "' is enabled");
            qDebug("GT_DEBUG_MESSAGE options.testFlag(IsDisabled");
        }

        if (options.testFlag(PopupChecker::IsCheckable)) {
            GT_CHECK(act->isCheckable(), "action '" + act->objectName() + "' is not checkable");
            qDebug("GT_DEBUG_MESSAGE options.testFlag(IsCheckable)");
        }

        if (options.testFlag(PopupChecker::IsChecked)) {
            GT_CHECK(act->isCheckable(), "action '" + act->objectName() + "' is not checked");
            qDebug("GT_DEBUG_MESSAGE options.testFlag(IsChecked)");
        }

        if (options.testFlag(PopupChecker::IsChecked)) {
            GT_CHECK(act->isChecked(), "action '" + act->objectName() + "' is not checked");
            qDebug("GT_DEBUG_MESSAGE options.testFlag(IsChecked)");
        }

        if (options.testFlag(PopupChecker::IsUnchecked)) {
            GT_CHECK(!act->isChecked(), "action '" + act->objectName() + "' is checked");
            qDebug("GT_DEBUG_MESSAGE options.testFlag(IsUnchecked)");
        }

        if (options.testFlag(PopupChecker::isNotVisible)) {
            GT_CHECK(!act->isVisible(), "action '" + act->objectName() + "' is visible, but shouldn't be");
            qDebug("GT_DEBUG_MESSAGE options.testFlag(isNotVisible)");
        }

        if (!itemsShortcuts.isEmpty()) {
            int index = itemsNames.indexOf(itemName);
            GT_CHECK(0 <= index && index < itemsShortcuts.size(), "Unexpected shortcut list size");
            GT_CHECK(itemsShortcuts.at(index) == act->shortcut(), "action '" + act->text() + "' unexpected shortcut");
        }
    }

    for (int i = 0; i < menuPath.size() + 1; i++) {
        PopupChooser::clickEsc(os);
        GTGlobals::sleep(250);
    }
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME
}  // namespace HI
