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

#include "primitives/GTListWidget.h"

#include "drivers/GTKeyboardDriver.h"
#include "drivers/GTMouseDriver.h"
#include "utils/GTThread.h"

namespace HI {

#define GT_CLASS_NAME "GTListWidget"

#define GT_METHOD_NAME "click"
void GTListWidget::click(GUITestOpStatus& os, QListWidget* listWidget, const QString& text, Qt::MouseButton button, int foundItemsNum) {
    QList<QListWidgetItem*> list = listWidget->findItems(text, Qt::MatchExactly);
    GT_CHECK(0 <= foundItemsNum && foundItemsNum < list.size(), QString("item %1 not found").arg(text));

    QListWidgetItem* item = list.at(foundItemsNum);
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
bool GTListWidget::isItemChecked(GUITestOpStatus& os, QListWidget* listWidget, const QString& text) {
    Q_UNUSED(os);
    GT_CHECK_RESULT(NULL != listWidget, "List widget is NULL", false);
    QList<QListWidgetItem*> list = listWidget->findItems(text, Qt::MatchExactly);
    GT_CHECK_RESULT(!list.isEmpty(), QString("Item '%1' wasn't' not found").arg(text), false);
    QListWidgetItem* item = list.first();
    return Qt::Checked == item->checkState();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkItem"
void GTListWidget::checkItem(GUITestOpStatus& os, QListWidget* listWidget, const QString& text, bool newState) {
    Q_UNUSED(os);
    GT_CHECK(NULL != listWidget, "List widget is NULL");
    if (newState != isItemChecked(os, listWidget, text)) {
        click(os, listWidget, text);
        GTKeyboardDriver::keyClick(Qt::Key_Space);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkAllItems"
void GTListWidget::checkAllItems(GUITestOpStatus& os, QListWidget* listWidget, bool newState) {
    Q_UNUSED(os);
    GT_CHECK(NULL != listWidget, "List widget is NULL");

    const QStringList itemTexts = getItems(os, listWidget);
    foreach (const QString& itemText, itemTexts) {
        checkItem(os, listWidget, itemText, newState);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getItems"
QStringList GTListWidget::getItems(GUITestOpStatus& os, QListWidget* listWidget) {
    Q_UNUSED(os);
    GT_CHECK_RESULT(NULL != listWidget, "List widget is NULL", QStringList());
    QStringList itemTexts;
    QList<QListWidgetItem*> items = listWidget->findItems("", Qt::MatchContains);
    foreach (QListWidgetItem* item, items) {
        itemTexts << item->text();
    }
    return itemTexts;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectItemsByText"
QListWidgetItem* GTListWidget::findItemByText(GUITestOpStatus& os, QListWidget* listWidget, const QString& text) {
    QList<QListWidgetItem*> result;
    for (int time = 0; time < GT_OP_WAIT_MILLIS && result.isEmpty(); time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        result = listWidget->findItems(text, Qt::MatchExactly);
    }
    GT_CHECK_RESULT(!result.isEmpty(), "Item not found: " + text, nullptr);
    GT_CHECK_RESULT(result.count() > 0, "Found multiple items with the name: " + text, nullptr);
    return result[0];
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findItemsByText"
QList<QListWidgetItem*> GTListWidget::findItemsByText(GUITestOpStatus& os, QListWidget* listWidget, const QStringList& itemTexts) {
    QList<QListWidgetItem*> result;
    for (int time = 0; time < GT_OP_WAIT_MILLIS && result.isEmpty(); time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        for (const QString& text : qAsConst(itemTexts)) {
            auto itemList = listWidget->findItems(text, Qt::MatchExactly);
            if (itemList.isEmpty()) {
                result.clear();
                break;
            }
            result << itemList;
        }
    }
    GT_CHECK_RESULT(!result.isEmpty(), "Item not found: " + itemTexts.join(","), {});
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectItemsByText"
void GTListWidget::selectItemsByText(HI::GUITestOpStatus& os, QListWidget* listWidget, const QStringList& itemTexts) {
    GT_CHECK_RESULT(!itemTexts.empty(), "List of items to select is empty", );
    QList<QListWidgetItem*> items;
    for (const QString& name : qAsConst(itemTexts)) {
        items << findItemByText(os, listWidget, name);
    }
    selectItems(os, items);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectItems"
void GTListWidget::selectItems(HI::GUITestOpStatus& os, const QList<QListWidgetItem*>& items) {
    GT_CHECK_RESULT(!items.empty(), "List of items to select is empty", );

    // Click on the first item to remove current selection. After this point only the first item is selected.
    GTListWidget::scrollToItem(os, items.first());
    GTMouseDriver::moveTo(getItemCenter(os, items.first()));
    GTMouseDriver::click();

    for (QListWidgetItem* item : qAsConst(items)) {
        if (!item->isSelected()) {
            GTListWidget::scrollToItem(os, item);
            GTMouseDriver::moveTo(getItemCenter(os, item));
            GTKeyboardDriver::keyPress(Qt::Key_Control);
            GTMouseDriver::click();
            GTKeyboardDriver::keyRelease(Qt::Key_Control);
            GTThread::waitForMainThread();
        }
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "scrollToItem"
void GTListWidget::scrollToItem(GUITestOpStatus& os, QListWidgetItem* item) {
    GT_CHECK_RESULT(item != nullptr, "item is NULL", );
    class ScrollInMainThreadScenario : public CustomScenario {
    public:
        ScrollInMainThreadScenario(QListWidgetItem* _item)
            : item(_item) {
        }
        void run(HI::GUITestOpStatus& os) override {
            QListWidget* list = item->listWidget();
            GT_CHECK_RESULT(list != nullptr, "List widget is nullptr!", );
            list->scrollToItem(item);
        }
        QListWidgetItem* item = nullptr;
    };
    GTThread::runInMainThread(os, new ScrollInMainThreadScenario(item));
    GTThread::waitForMainThread();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getItemCenter"
QPoint GTListWidget::getItemCenter(GUITestOpStatus& os, QListWidgetItem* item) {
    QPoint itemRectCenterPoint = getItemRect(os, item).center();
    QListWidget* listWidget = item->listWidget();
    return listWidget->viewport()->mapToGlobal(itemRectCenterPoint);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getItemRect"
QRect GTListWidget::getItemRect(GUITestOpStatus& os, QListWidgetItem* item) {
    GT_CHECK_RESULT(item != nullptr, "item is nullptr", {});
    GT_CHECK_RESULT(!item->isHidden(), "item is hidden", {});

    QListWidget* listWidget = item->listWidget();
    GT_CHECK_RESULT(listWidget != nullptr, "listWidget is nullptr", {});
    return listWidget->visualItemRect(item);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkSelection"
void GTListWidget::checkSelection(GUITestOpStatus& os, QListWidget* listWidget, const QStringList& itemTexts) {
    QList<QListWidgetItem*> items = findItemsByText(os, listWidget, itemTexts);
    QList<QListWidgetItem*> selectedItems = listWidget->selectedItems();
    std::sort(items.begin(), items.end());
    std::sort(selectedItems.begin(), selectedItems.end());
    GT_CHECK_RESULT(items == selectedItems, "Invalid selection", );
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME
}  // namespace HI
