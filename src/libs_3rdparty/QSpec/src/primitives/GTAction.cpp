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
#include "primitives/GTAction.h"

#include <QAbstractButton>
#include <QApplication>
#include <QWidget>

#include "primitives/GTWidget.h"

namespace HI {

#define GT_CLASS_NAME "GTAction"

#define GT_METHOD_NAME "button"
QAbstractButton* GTAction::button(GUITestOpStatus& os, const QString& objectName, QWidget* parent, const GTGlobals::FindOptions& options) {
    QAction* action = findAction(os, objectName, parent, options);
    if (action == nullptr) {
        return nullptr;
    }
    QAbstractButton* resultButton = button(os, action);
    GT_CHECK_RESULT(resultButton != nullptr || !options.failIfNotFound, "Button with object name not found: " + objectName, nullptr);
    return resultButton;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "button"
QAbstractButton* GTAction::button(GUITestOpStatus& os, const QAction* action) {
    GT_CHECK_RESULT(action != nullptr, "action is NULL", nullptr);
    QList<QWidget*> associatedWidgets = action->associatedWidgets();
    for (QWidget* associatedWidget : associatedWidgets) {
        if (auto button = qobject_cast<QAbstractButton*>(associatedWidget)) {
            return button;
        }
    }
    return nullptr;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findAction"
QAction* GTAction::findAction(GUITestOpStatus& os, const QString& objectName, QWidget* parent, const GTGlobals::FindOptions& options) {
    QList<QAction*> actions = GTWidget::findChildren<QAction>(os, parent, [&objectName](auto action) { return action->objectName() == objectName; });
    GT_CHECK_RESULT(actions.size() < 2, QString("There are %1 actions with object name %2").arg(actions.size()).arg(objectName), nullptr);
    if (actions.size() == 1) {
        return actions[0];
    }
    GT_CHECK_RESULT(!options.failIfNotFound, "Action with object name not found: " + objectName, nullptr);
    return nullptr;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findActionByText"
QAction* GTAction::findActionByText(GUITestOpStatus& os, const QString& text, QWidget* parent) {
    QList<QAction*> actions = GTWidget::findChildren<QAction>(os, parent, [text](auto action) { return action->text() == text; });
    GT_CHECK_RESULT(!actions.isEmpty(), "Action with text not found: " + text, nullptr);
    GT_CHECK_RESULT(actions.size() == 1, QString("There are %1 actions with text: %2").arg(actions.size()).arg(text), nullptr);
    return actions[0];
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace HI
