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

#include "drivers/GTKeyboardDriver.h"
#include "drivers/GTMouseDriver.h"
#include "primitives/GTListWidget.h"

namespace HI{

#define GT_CLASS_NAME "GTListWidget"

#define GT_METHOD_NAME "click"
void GTListWidget::click(GUITestOpStatus &os, QListWidget *listWidget, const QString &text, Qt::MouseButton button){
    QList<QListWidgetItem*> list = listWidget->findItems(text, Qt::MatchExactly);
    GT_CHECK(!list.isEmpty(), QString("item %1 not found").arg(text));

    QListWidgetItem* item = list.first();
    listWidget->scrollToItem(item);

    QRect r = listWidget->visualItemRect(item);
    QPoint p = QPoint(r.left() + 30, r.center().y());
    QPoint global = listWidget->viewport()->mapToGlobal(p);
    GTMouseDriver::moveTo(global);
    GTMouseDriver::click(button);
    GTGlobals::sleep();
    GT_CHECK(true, "click method completed");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isItemChecked"
bool GTListWidget::isItemChecked(GUITestOpStatus &os, QListWidget *listWidget, const QString &text) {
    Q_UNUSED(os);
    GT_CHECK_RESULT(NULL != listWidget, "List widget is NULL", false);
    QList<QListWidgetItem *> list = listWidget->findItems(text, Qt::MatchExactly);
    GT_CHECK_RESULT(!list.isEmpty(), QString("Item '%1' wasn't' not found").arg(text), false);
    QListWidgetItem *item = list.first();
    return Qt::Checked == item->checkState();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkItem"
void GTListWidget::checkItem(GUITestOpStatus &os, QListWidget *listWidget, const QString &text, bool newState) {
    Q_UNUSED(os);
    GT_CHECK(NULL != listWidget, "List widget is NULL");
    if (newState != isItemChecked(os, listWidget, text)) {
        click(os, listWidget, text);
        GTKeyboardDriver::keyClick( Qt::Key_Space);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkAllItems"
void GTListWidget::checkAllItems(GUITestOpStatus &os, QListWidget *listWidget, bool newState) {
    Q_UNUSED(os);
    GT_CHECK(NULL != listWidget, "List widget is NULL");

    const QStringList itemTexts = getItems(os, listWidget);
    foreach (const QString &itemText, itemTexts) {
        checkItem(os, listWidget, itemText, newState);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkOnlyItems"
void GTListWidget::checkOnlyItems(GUITestOpStatus &os, QListWidget *listWidget, const QStringList &itemTexts, bool newState) {
    Q_UNUSED(os);
    GT_CHECK(NULL != listWidget, "List widget is NULL");

    const QStringList allItemTexts = getItems(os, listWidget);
    int checkedItemsCount = 0;
    foreach (const QString &itemText, allItemTexts) {
        if (!itemTexts.contains(itemText)) {
            checkItem(os, listWidget, itemText, !newState);
        } else {
            checkedItemsCount++;
            checkItem(os, listWidget, itemText, newState);
        }
    }
    GT_CHECK(checkedItemsCount == itemTexts.count(), "Some items weren't found");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getItems"
QStringList GTListWidget::getItems(GUITestOpStatus &os, QListWidget *listWidget) {
    Q_UNUSED(os);
    GT_CHECK_RESULT(NULL != listWidget, "List widget is NULL", QStringList());
    QStringList itemTexts;
    QList<QListWidgetItem *> items = listWidget->findItems("", Qt::MatchContains);
    foreach (QListWidgetItem *item, items) {
        itemTexts << item->text();
    }
    return itemTexts;
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME
}
