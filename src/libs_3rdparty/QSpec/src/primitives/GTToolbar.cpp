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

#include "primitives/GTToolbar.h"

#include <QAction>

#include "primitives/GTWidget.h"

namespace HI {
#define GT_CLASS_NAME "GTToolbar"

#define GT_METHOD_NAME "getToolbar"
QToolBar* GTToolbar::getToolbar(GUITestOpStatus& os, const QString& toolbarSysName) {
    auto toolbar = qobject_cast<QToolBar*>(GTWidget::findWidget(os, toolbarSysName));
    GT_CHECK_RESULT(toolbar != nullptr, "No such toolbar: " + toolbarSysName, nullptr);
    return toolbar;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getWidgetForAction"
QWidget* GTToolbar::getWidgetForAction(GUITestOpStatus& os, const QToolBar* toolbar, QAction* action) {
    GT_CHECK_RESULT(toolbar != nullptr, "Toolbar is nullptr", nullptr);  // the found widget is not a qtoolbar or doesn't exist

    QWidget* widget = toolbar->widgetForAction(action);
    GT_CHECK_RESULT(widget != nullptr, "No widget for action", nullptr);

    return widget;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getWidgetForActionObjectName"
QWidget* GTToolbar::getWidgetForActionObjectName(GUITestOpStatus& os, const QToolBar* toolbar, const QString& actionName) {
    GT_CHECK_RESULT(toolbar != nullptr, "Toolbar is nullptr", nullptr);
    QAction* action = GTToolbar::getActionByObjectName(os, actionName, toolbar);
    return GTToolbar::getWidgetForAction(os, toolbar, action);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getToolButtonByAction"
QToolButton* GTToolbar::getToolButtonByAction(GUITestOpStatus& os, const QToolBar* toolbar, const QString& actionName) {
    auto widget = GTToolbar::getWidgetForActionObjectName(os, toolbar, actionName);
    auto button = qobject_cast<QToolButton*>(widget);
    GT_CHECK_RESULT(button != nullptr, "Not a tool button: " + actionName, nullptr);
    return button;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getWidgetForActionTooltip"
QWidget* GTToolbar::getWidgetForActionTooltip(GUITestOpStatus& os, const QToolBar* toolbar, const QString& tooltip) {
    GT_CHECK_RESULT(toolbar != nullptr, "Toolbar is nullptr", nullptr);
    for (QAction* action : toolbar->actions()) {
        if (action->toolTip() == tooltip) {
            return GTToolbar::getWidgetForAction(os, toolbar, action);
        }
    }
    GT_CHECK_RESULT(false, "No action with such tooltip: " + tooltip, nullptr);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickButtonByTooltipOnToolbar"
void GTToolbar::clickButtonByTooltipOnToolbar(GUITestOpStatus& os, const QString& toolbarSysName, const QString& tooltip) {
    GTWidget::click(os, GTToolbar::getWidgetForActionTooltip(os, getToolbar(os, toolbarSysName), tooltip));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getActionByObjectName"
QAction* GTToolbar::getActionByObjectName(GUITestOpStatus& os, const QString& actionName, const QToolBar* toolbar) {
    GT_CHECK_RESULT(toolbar != nullptr, "Toolbar is nullptr", nullptr);
    for (QAction* action : toolbar->actions()) {
        if (actionName == action->objectName()) {
            return action;
        }
    }
    GT_CHECK_RESULT(false, "No such action:" + actionName, nullptr);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickWidgetByActionName"
void GTToolbar::clickWidgetByActionName(GUITestOpStatus& os, const QString& toolbarSysName, const QString& actionObjectName) {
    QWidget* widget = GTToolbar::getWidgetForActionObjectName(os, getToolbar(os, toolbarSysName), actionObjectName);
    GT_CHECK(widget->isEnabled(), "Widget is disabled: " + actionObjectName);
    GTWidget::click(os, widget);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace HI
