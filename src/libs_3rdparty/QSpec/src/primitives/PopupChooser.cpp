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

#include "primitives/PopupChooser.h"
#include "drivers/GTKeyboardDriver.h"
#include "primitives/GTMenu.h"
#include "drivers/GTMouseDriver.h"

#include <QApplication>
#include <QMenu>

namespace HI {
#define GT_CLASS_NAME "PopupChooser"

PopupChooser::PopupChooser(GUITestOpStatus &os, const QStringList &namePath, GTGlobals::UseMethod useMethod)
    : Filler(os, GUIDialogWaiter::WaitSettings(QString(), GUIDialogWaiter::Popup)),
      namePath(namePath),
      useMethod(useMethod)
{

}

#define GT_METHOD_NAME "run"
void PopupChooser::commonScenario() {
    GTGlobals::sleep(1000);
    GTMouseDriver::release();
    QMenu* activePopupMenu = qobject_cast<QMenu*>(QApplication::activePopupWidget());
    GT_CHECK(NULL != activePopupMenu, "Active popup menu is NULL");

    if (!namePath.isEmpty()) {
        GTMenu::clickMenuItemByName(os, activePopupMenu, namePath, useMethod);
    } else {
        clickEsc(os);
    }

    if (os.hasError()) {
        clickEsc(os);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickEsc"
void PopupChooser::clickEsc(GUITestOpStatus &) {
    GTKeyboardDriver::keyClick(Qt::Key_Escape);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

PopupChooserByText::PopupChooserByText(GUITestOpStatus &os, const QStringList &namePath, GTGlobals::UseMethod useMethod, Qt::MatchFlag matchFlag)
    : Filler(os, GUIDialogWaiter::WaitSettings(QString(), GUIDialogWaiter::Popup)),
      namePath(namePath),
      useMethod(useMethod),
      matchFlag(matchFlag)
{

}

void PopupChooserByText::commonScenario()
{
    GTGlobals::sleep(1000);
    GTMouseDriver::release();
    QMenu* activePopupMenu = qobject_cast<QMenu*>(QApplication::activePopupWidget());
    GTMenu::clickMenuItemByText(os, activePopupMenu, namePath, useMethod, matchFlag);
}

#define GT_CLASS_NAME "PopupChecker"

PopupChecker::PopupChecker(GUITestOpStatus &os, CustomScenario *scenario) :
    Filler(os, GUIDialogWaiter::WaitSettings(QString(), GUIDialogWaiter::Popup), scenario)
{

}

PopupChecker::PopupChecker(GUITestOpStatus &os, const QStringList &namePath, CheckOptions options, GTGlobals::UseMethod useMethod)
    : Filler(os, GUIDialogWaiter::WaitSettings(QString(), GUIDialogWaiter::Popup)),
      namePath(namePath),
      options(options),
      useMethod(useMethod)
{

}

#define GT_METHOD_NAME "commonScenario"
void PopupChecker::commonScenario() {
    GTGlobals::sleep(1000);
    GTMouseDriver::release();
    QMenu* activePopupMenu = qobject_cast<QMenu*>(QApplication::activePopupWidget());
    GT_CHECK(NULL != activePopupMenu, "Active popup menu is NULL");

    QAction* act;
    if (!namePath.isEmpty()) {
        QString actName;
        int escCount = namePath.size();
        if(namePath.size()>1){
            actName = namePath.takeLast();
            GTMenu::clickMenuItemByName(os, activePopupMenu, namePath, useMethod);
            QMenu* activePopupMenuToCheck = qobject_cast<QMenu*>(QApplication::activePopupWidget());
            act = GTMenu::getMenuItem(os, activePopupMenuToCheck, actName);
        }else{
            QMenu* activePopupMenuToCheck = qobject_cast<QMenu*>(QApplication::activePopupWidget());
            actName = namePath.last();
            act = GTMenu::getMenuItem(os, activePopupMenuToCheck, actName);
        }

        if(options.testFlag(Exists)){
            GT_CHECK(act != NULL, "action '" + actName + "' not found");
            qDebug("GT_DEBUG_MESSAGE options.testFlag(Exists)");
        }else{
            GT_CHECK(act == NULL, "action '" + actName + "' unexpectidly found");
        }
        if(options.testFlag(IsEnabled)){
            GT_CHECK(act->isEnabled(), "action '" + act->objectName() + "' is not enabled");
            qDebug("GT_DEBUG_MESSAGE options.testFlag(IsEnabled)");
        }
        if(options.testFlag(IsDisabled)){
            GT_CHECK(!act->isEnabled(), "action '" + act->objectName() + "' is enabled");
            qDebug("GT_DEBUG_MESSAGE options.testFlag(IsDisabled");
        }
        if(options.testFlag(IsChecable)){
            GT_CHECK(act->isCheckable(), "action '" + act->objectName() + "' is not checkable");
            qDebug("GT_DEBUG_MESSAGE options.testFlag(IsChecable)");
        }
        if(options.testFlag(IsChecked)){
            GT_CHECK(act->isChecked(), "action '" + act->objectName() + "' is not checked");
            qDebug("GT_DEBUG_MESSAGE options.testFlag(IsChecked)");
        }
        if (options.testFlag(PopupChecker::IsUnchecked)){
            GT_CHECK(!act->isChecked(), "action '" + act->objectName() + "' is checked");
            qDebug("GT_DEBUG_MESSAGE options.testFlag(IsUnchecked)");
        }
        for(int i = 0; i<escCount; i++){
            PopupChooser::clickEsc(os);
            GTGlobals::sleep(300);
        }
    } else {
        PopupChooser::clickEsc(os);
    }

    if (os.hasError()) {
        PopupChooser::clickEsc(os);
    }
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

#define GT_CLASS_NAME "PopupCheckerByText"

PopupCheckerByText::PopupCheckerByText(GUITestOpStatus &os, CustomScenario *scenario) :
    Filler(os, GUIDialogWaiter::WaitSettings(QString(), GUIDialogWaiter::Popup), scenario)
{

}

PopupCheckerByText::PopupCheckerByText(GUITestOpStatus &os,
                                       const QStringList &namePath,
                                       PopupChecker::CheckOptions options,
                                       GTGlobals::UseMethod useMethod) :
    Filler(os, GUIDialogWaiter::WaitSettings(QString(), GUIDialogWaiter::Popup)),
    menuPath(namePath.mid(0, namePath.size() - 1)),
    itemsNames(namePath.isEmpty() ? "" : namePath.last()),
    options(options),
    useMethod(useMethod)
{

}

PopupCheckerByText::PopupCheckerByText(GUITestOpStatus &os,
                                       const QStringList &menuPath,
                                       const QStringList &itemsNames,
                                       PopupChecker::CheckOptions options,
                                       GTGlobals::UseMethod useMethod) :
    Filler(os, GUIDialogWaiter::WaitSettings(QString(), GUIDialogWaiter::Popup)),
    menuPath(menuPath),
    itemsNames(itemsNames),
    options(options),
    useMethod(useMethod)
{

}

PopupCheckerByText::PopupCheckerByText(GUITestOpStatus &os,
                                       const QStringList &menuPath,
                                       const QMap<QString, QKeySequence> &namesAndShortcuts,
                                       PopupChecker::CheckOptions options,
                                       GTGlobals::UseMethod useMethod)
    : PopupCheckerByText(os, menuPath, namesAndShortcuts.keys(), options, useMethod)
{
    itemsShortcuts = namesAndShortcuts.values();
}


PopupCheckerByText::PopupCheckerByText(GUITestOpStatus &os,
                                       const QList<QStringList> &itemsPaths,
                                       PopupChecker::CheckOptions options,
                                       GTGlobals::UseMethod useMethod) :
    Filler(os, GUIDialogWaiter::WaitSettings(QString(), GUIDialogWaiter::Popup)),
    options(options),
    useMethod(useMethod)
{
    CHECK_SET_ERR(!itemsPaths.isEmpty(), "itemsPaths is empty");
    menuPath = itemsPaths.first().mid(0, itemsPaths.first().size() - 1);
    foreach (const QStringList &itemPath, itemsPaths) {
        CHECK_SET_ERR(!itemPath.isEmpty(), "itemPath is empty");
        CHECK_SET_ERR(itemPath.mid(0, itemPath.size() - 1) == menuPath, "Items from different submenus were passed to the PopupCheckerByText constructor");
        itemsNames << itemPath.last();
    }
}

#define GT_METHOD_NAME "commonScenario"
void PopupCheckerByText::commonScenario() {
    GTGlobals::sleep(1000);
    GTMouseDriver::release();
    QMenu* activePopupMenu = qobject_cast<QMenu *>(QApplication::activePopupWidget());
    GT_CHECK(NULL != activePopupMenu, "Active popup menu is NULL");

    QAction* act = NULL;

    if (itemsNames.isEmpty()) {
        PopupChooser::clickEsc(os);
        return;
    }

    if (!menuPath.isEmpty()) {
        GTMenu::clickMenuItemByText(os, activePopupMenu, menuPath, useMethod);
    }

    QMenu *activePopupMenuToCheck = qobject_cast<QMenu *>(QApplication::activePopupWidget());

    foreach (const QString &itemName, itemsNames) {
        act = GTMenu::getMenuItem(os, activePopupMenuToCheck, itemName, true);
        if (options.testFlag(PopupChecker::Exists)) {
            GT_CHECK(act != NULL, "action '" + itemName + "' not found");
            qDebug("GT_DEBUG_MESSAGE options.testFlag(Exists)");
        } else {
            GT_CHECK(act == NULL, "action '" + itemName + "' unexpectidly found");
        }

        if (options.testFlag(PopupChecker::IsEnabled)) {
            GT_CHECK(act->isEnabled(), "action '" + act->objectName() + "' is not enabled");
            qDebug("GT_DEBUG_MESSAGE options.testFlag(IsEnabled)");
        }

        if (options.testFlag(PopupChecker::IsDisabled)) {
            GT_CHECK(!act->isEnabled(), "action '" + act->objectName() + "' is enabled");
            qDebug("GT_DEBUG_MESSAGE options.testFlag(IsDisabled");
        }

        if (options.testFlag(PopupChecker::IsChecable)) {
            GT_CHECK(act->isCheckable(), "action '" + act->objectName() + "' is not checkable");
            qDebug("GT_DEBUG_MESSAGE options.testFlag(IsChecable)");
        }

        if (options.testFlag(PopupChecker::IsChecked)) {
            GT_CHECK(act->isCheckable(), "action '" + act->objectName() + "' is not checked");
            qDebug("GT_DEBUG_MESSAGE options.testFlag(IsChecked)");
        }

        if (options.testFlag(PopupChecker::IsChecked)) {
            GT_CHECK(act->isChecked(), "action '" + act->objectName() + "' is not checked");
            qDebug("GT_DEBUG_MESSAGE options.testFlag(IsChecked)");
        }

        if (options.testFlag(PopupChecker::IsUnchecked) ){
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
            GT_CHECK(itemsShortcuts.at(index)  == act->shortcut(), "action '" + act->text() + "' unexpected shortcut");

        }
    }

    for (int i = 0; i < menuPath.size(); i++) {
        PopupChooser::clickEsc(os);
        GTGlobals::sleep(100);
    }

    PopupChooser::clickEsc(os);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME
}

