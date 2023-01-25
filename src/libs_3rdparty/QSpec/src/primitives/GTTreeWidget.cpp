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

#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTWidget.h>

#include <QHeaderView>

#include "primitives/GTTreeWidget.h"

namespace HI {

#define GT_CLASS_NAME "GTUtilsTreeView"

#define GT_METHOD_NAME "expand"
void GTTreeWidget::expand(GUITestOpStatus& os, QTreeWidgetItem* item) {
    GT_CHECK_RESULT(item != nullptr, "item is NULL", );
    if (item->isExpanded() || item == item->treeWidget()->invisibleRootItem()) {
        return;
    }
    // Using API call to expand instead of the mouse: because we do not know expander position inside of the item exactly.
    class ExpandInMainThreadScenario : public CustomScenario {
    public:
        ExpandInMainThreadScenario(QTreeWidgetItem* _item)
            : item(_item) {
        }
        void run(HI::GUITestOpStatus& os) override {
            QTreeWidget* tree = item->treeWidget();
            GT_CHECK_RESULT(tree != nullptr, "Tree is nullptr!", );
            tree->expandItem(item);
        }
        QTreeWidgetItem* item = nullptr;
    };
    GTThread::runInMainThread(os, new ExpandInMainThreadScenario(item));
    scrollToItem(os, item);
    checkItemIsExpanded(os, item);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkItem"
void GTTreeWidget::checkItem(GUITestOpStatus& os, QTreeWidgetItem* item, int column, GTGlobals::UseMethod method) {
    GT_CHECK(item != nullptr, "treeWidgetItem is NULL");
    GT_CHECK(column >= 0, "The column number is invalid");

    QTreeWidget* tree = item->treeWidget();
    GT_CHECK(tree != nullptr, "The tree widget is NULL");

    GTTreeWidget::scrollToItem(os, item);
    QRect itemRect = getItemRect(os, item);
    QPoint indentationOffset(tree->indentation(), 0);
    QPoint itemStartPos = QPoint(itemRect.left(), itemRect.center().y()) - indentationOffset;
    QPoint columnOffset(tree->columnViewportPosition(column), 0);
    QPoint itemLevelOffset(getItemLevel(os, item) * tree->indentation(), 0);

    switch (method) {
        case GTGlobals::UseKeyBoard: {
            const QPoint cellCenterOffset(tree->columnWidth(column) / 2, itemRect.height() / 2);
            GTMouseDriver::moveTo(itemStartPos + itemLevelOffset + columnOffset + cellCenterOffset);
            GTMouseDriver::click();
            GTKeyboardDriver::keyClick(Qt::Key_Space);
            break;
        }
        case GTGlobals::UseMouse: {
            const QPoint magicCheckBoxOffset = QPoint(15, 0);
            GTMouseDriver::moveTo(tree->viewport()->mapToGlobal(itemStartPos + itemLevelOffset + columnOffset + magicCheckBoxOffset));
            GTMouseDriver::click();
            break;
        }
        default:
            GT_FAIL("Method is not implemented", );
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getItemRect"
QRect GTTreeWidget::getItemRect(GUITestOpStatus& os, QTreeWidgetItem* item) {
    GT_CHECK_RESULT(item != nullptr, "treeWidgetItem is NULL", {});
    GT_CHECK_RESULT(!item->isHidden(), "item is hidden", {});

    QTreeWidget* treeWidget = item->treeWidget();
    GT_CHECK_RESULT(treeWidget != nullptr, "treeWidget is NULL", {});
    // The item parent must already be expanded. Expanding it with mouse/keyboard may break current selection state.
    GT_CHECK_RESULT(item->parent() == nullptr || item->parent()->isExpanded(), "Item parent is not expanded: " + item->text(0), {});
    return treeWidget->visualItemRect(item);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getItemCenter"
QPoint GTTreeWidget::getItemCenter(GUITestOpStatus& os, QTreeWidgetItem* item) {
    GT_CHECK_RESULT(item != nullptr, "item is NULL", {});

    QTreeWidget* treeWidget = item->treeWidget();
    GT_CHECK_RESULT(treeWidget != nullptr, "treeWidget is NULL", {});

    GTTreeWidget::scrollToItem(os, item);

    QPoint itemRectCenterPoint = getItemRect(os, item).center();
    return treeWidget->viewport()->mapToGlobal(itemRectCenterPoint);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getItems"
QList<QTreeWidgetItem*> GTTreeWidget::getItems(QTreeWidgetItem* root) {
    QList<QTreeWidgetItem*> treeItems;
    for (int i = 0; i < root->childCount(); i++) {
        QTreeWidgetItem* childItem = root->child(i);
        treeItems.append(childItem);
        treeItems.append(getItems(childItem));
    }
    return treeItems;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getItems"
QList<QTreeWidgetItem*> GTTreeWidget::getItems(GUITestOpStatus& os, QTreeWidget* treeWidget) {
    GT_CHECK_RESULT(treeWidget != nullptr, "Tree widget is NULL", QList<QTreeWidgetItem*>());
    return getItems(treeWidget->invisibleRootItem());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getItemNames"
QStringList GTTreeWidget::getItemNames(GUITestOpStatus& os, QTreeWidget* treeWidget) {
    QStringList itemNames;
    QList<QTreeWidgetItem*> items = getItems(os, treeWidget);
    for (QTreeWidgetItem* item : qAsConst(items)) {
        itemNames << item->text(0);
    }
    return itemNames;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findItemPrivate"
QTreeWidgetItem* GTTreeWidget::findItemPrivate(GUITestOpStatus& os, QTreeWidget* tree, const QString& text, QTreeWidgetItem* parent, int column, const GTGlobals::FindOptions& options) {
    GT_CHECK_RESULT(tree != nullptr, "tree widget is NULL", nullptr);

    if (parent == nullptr) {
        parent = tree->invisibleRootItem();
    }

    GTGlobals::FindOptions innerOptions(options);
    if (options.depth != GTGlobals::FindOptions::INFINITE_DEPTH) {
        innerOptions.depth--;
    }

    const QList<QTreeWidgetItem*> list = getItems(parent);
    for (QTreeWidgetItem* item : qAsConst(list)) {
        const QString itemText = item->text(column);
        if (options.matchPolicy.testFlag(Qt::MatchExactly) && itemText == text) {
            return item;
        } else if (options.matchPolicy.testFlag(Qt::MatchContains) && itemText.contains(text)) {
            return item;
        }

        if (options.depth == GTGlobals::FindOptions::INFINITE_DEPTH ||
            innerOptions.depth > 0) {
            QTreeWidgetItem* childItem = findItemPrivate(os, tree, text, item, column, innerOptions);
            if (childItem != nullptr) {
                return childItem;
            }
        }
    }
    return nullptr;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findItem"
QTreeWidgetItem* GTTreeWidget::findItem(GUITestOpStatus& os,
                                        QTreeWidget* tree,
                                        const QString& text,
                                        QTreeWidgetItem* parent,
                                        int column,
                                        const GTGlobals::FindOptions& options,
                                        bool expandParent) {
    QTreeWidgetItem* item = findItemPrivate(os, tree, text, parent, column, options);
    CHECK_SET_ERR_RESULT(!options.failIfNotFound || item != nullptr, QString("Item '%1' not found").arg(text), nullptr);
    if (item != nullptr && item->parent() != nullptr && expandParent) {
        expand(os, item->parent());
    }
    return item;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findItems"
QList<QTreeWidgetItem*> GTTreeWidget::findItems(GUITestOpStatus& os, QTreeWidget* tree, const QString& text, QTreeWidgetItem* parent, int column, const GTGlobals::FindOptions& options) {
    GT_CHECK_RESULT(tree != nullptr, "tree widget is NULL", {});
    if (parent == nullptr) {
        parent = tree->invisibleRootItem();
    }

    GTGlobals::FindOptions innerOptions(options);
    if (options.depth != GTGlobals::FindOptions::INFINITE_DEPTH) {
        innerOptions.depth--;
    }

    QList<QTreeWidgetItem*> items;
    const QList<QTreeWidgetItem*> list = getItems(parent);
    for (QTreeWidgetItem* item : qAsConst(list)) {
        const QString itemText = item->text(column);
        if (options.matchPolicy.testFlag(Qt::MatchExactly) && itemText == text) {
            items << item;
        } else if (options.matchPolicy.testFlag(Qt::MatchContains) && itemText.contains(text)) {
            items << item;
        }

        if (options.depth == GTGlobals::FindOptions::INFINITE_DEPTH ||
            innerOptions.depth > 0) {
            items << findItems(os, tree, text, item, column, innerOptions);
        }
    }

    return items;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "click"
void GTTreeWidget::click(GUITestOpStatus& os, QTreeWidgetItem* item, int column) {
    GT_CHECK(item != nullptr, "item is NULL");
    GTTreeWidget::scrollToItem(os, item);

    QPoint point;
    if (column == -1) {
        point = getItemCenter(os, item);
    } else {
        QTreeWidget* tree = item->treeWidget();
        QRect itemRect = getItemRect(os, item);
        point = tree->viewport()->mapToGlobal(itemRect.topLeft());
        point += QPoint(tree->columnViewportPosition(column) + tree->columnWidth(column) / 2, itemRect.height() / 2);
    }

    GTMouseDriver::moveTo(point);
    GTMouseDriver::click();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "click"
void GTTreeWidget::doubleClick(GUITestOpStatus& os, QTreeWidgetItem* item, int column) {
    GT_CHECK(item != nullptr, "item is NULL");
    GTTreeWidget::scrollToItem(os, item);

    QPoint point;
    if (column == -1) {
        point = getItemCenter(os, item);
    } else {
        QTreeWidget* tree = item->treeWidget();
        QRect itemRect = getItemRect(os, item);
        point = tree->viewport()->mapToGlobal(itemRect.topLeft());
        point += QPoint(tree->columnViewportPosition(column) + tree->columnWidth(column) / 2, itemRect.height() / 2);
    }

    GTMouseDriver::moveTo(point);
    GTMouseDriver::doubleClick();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getItemLevel"
int GTTreeWidget::getItemLevel(GUITestOpStatus& os, QTreeWidgetItem* item) {
    GT_CHECK_RESULT(item != nullptr, "item is NULL", -1);

    int level = 0;
    while (item->parent() != nullptr) {
        level++;
        item = item->parent();
    }

    return level;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "scrollToItem"
void GTTreeWidget::scrollToItem(GUITestOpStatus& os, QTreeWidgetItem* item) {
    GT_CHECK_RESULT(item != nullptr, "item is NULL", );
    class ScrollInMainThreadScenario : public CustomScenario {
    public:
        ScrollInMainThreadScenario(QTreeWidgetItem* _item)
            : item(_item) {
        }
        void run(HI::GUITestOpStatus& os) override {
            QTreeWidget* tree = item->treeWidget();
            GT_CHECK_RESULT(tree != nullptr, "Tree is nullptr!", );
            tree->scrollToItem(item);
        }
        QTreeWidgetItem* item = nullptr;
    };
    GTThread::runInMainThread(os, new ScrollInMainThreadScenario(item));
    GTThread::waitForMainThread();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkItemIsExpanded"
void GTTreeWidget::checkItemIsExpanded(HI::GUITestOpStatus& os, QTreeWidgetItem* item) {
    GT_CHECK(item != nullptr, "Item is null!");

    bool isExpanded = item->isExpanded();
    for (int time = 0; time < GT_OP_WAIT_MILLIS && !isExpanded; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(GT_OP_CHECK_MILLIS);
        isExpanded = item->isExpanded();
    }
    GT_CHECK(isExpanded, "Item is not expanded: " + toString(item));
}
#undef GT_METHOD_NAME

QString GTTreeWidget::toString(QTreeWidgetItem* item) {
    return item == nullptr            ? "<nullptr>"
           : item->columnCount() == 0 ? "?"
                                      : item->text(0);
}
#undef GT_CLASS_NAME

}  // namespace HI
