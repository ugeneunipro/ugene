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

#include "primitives/GTTreeView.h"

#include <QAbstractItemModel>

#include "drivers/GTKeyboardDriver.h"
#include "drivers/GTMouseDriver.h"
#include "utils/GTThread.h"

namespace HI {

#define GT_CLASS_NAME "GTTreeView"
QModelIndex GTTreeView::findIndex(QTreeView* tree, QVariant data, Qt::ItemDataRole role, const GTGlobals::FindOptions& options) {
    return findIndex(tree, data, QModelIndex(), role, options);
}

QModelIndex GTTreeView::findIndex(QTreeView* tree, QVariant data, QModelIndex parent, Qt::ItemDataRole role, const GTGlobals::FindOptions& options) {
    GT_CHECK_RESULT(tree != NULL, "Tree widget is NULL", QModelIndex());

    QModelIndexList foundIndexes = findIndexes(tree, data, role, parent, 0, options);
    if (foundIndexes.isEmpty()) {
        if (options.failIfNotFound) {
            GT_CHECK_RESULT(foundIndexes.size() != 0, QString("Item whith name %1 not found").arg(data.toString()), QModelIndex());
        } else {
            return QModelIndex();
        }
    }

    GT_CHECK_RESULT(foundIndexes.size() == 1, QString("there are %1 items with name %2").arg(foundIndexes.size()).arg(data.toString()), QModelIndex());

    tree->scrollTo(foundIndexes.at(0));
    return foundIndexes.at(0);
}

QModelIndexList GTTreeView::findIndexes(QTreeView* tree, QVariant data, Qt::ItemDataRole role, QModelIndex parent, int depth, const GTGlobals::FindOptions& options) {
    QModelIndexList foundIndexes;
    if (!(GTGlobals::FindOptions::INFINITE_DEPTH == options.depth || depth < options.depth)) {
        return foundIndexes;
    }

    QAbstractItemModel* model = tree->model();
    CHECK_SET_ERR_RESULT(NULL != model, "Model is NULL", foundIndexes);

    int rowcount = model->rowCount(parent);
    for (int i = 0; i < rowcount; i++) {
        const QModelIndex index = model->index(i, 0, parent);
        QVariant indexData = index.data(role);

        if (data == indexData) {
            foundIndexes << index;
        } else {
            foundIndexes << findIndexes(tree, data, role, index, depth + 1, options);
        }
    }

    return foundIndexes;
}

QPoint GTTreeView::getItemCenter(QTreeView* tree, const QModelIndex& itemIndex) {
    QRect r = tree->visualRect(itemIndex);

    return tree->mapToGlobal(r.center());
}

void GTTreeView::checkItemIsExpanded(QTreeView* tree, const QModelIndex& itemIndex) {
    GT_CHECK(tree != nullptr, "Tree view is null!");
    GT_CHECK(itemIndex.isValid(), "Item index is not valid!");

    bool isExpanded = false;
    for (int time = 0; time < GT_OP_WAIT_MILLIS && !isExpanded; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        isExpanded = tree->isExpanded(itemIndex);
    }
    GT_CHECK(isExpanded, "Item is not expanded");
    GTThread::waitForMainThread();
}

void GTTreeView::expand(QTreeView* tree, const QModelIndex& itemIndex) {
    GT_CHECK_RESULT(tree != nullptr, "tree is NULL", );
    GT_CHECK_RESULT(itemIndex.isValid(), "itemIndex is not valid", );
    if (tree->isExpanded(itemIndex)) {
        scrollToItem(tree, itemIndex);
        return;
    }
    // Using API call to expand instead of the mouse: because we do not know expander position inside of the item exactly.
    class ExpandInMainThreadScenario : public CustomScenario {
    public:
        ExpandInMainThreadScenario(QTreeView* _treeView, const QModelIndex& _itemIndex)
            : tree(_treeView), itemIndex(_itemIndex) {
        }
        void run() override {
            tree->expand(itemIndex);
        }
        QTreeView* tree = nullptr;
        QModelIndex itemIndex;
    };
    GTThread::runInMainThread(new ExpandInMainThreadScenario(tree, itemIndex));
    scrollToItem(tree, itemIndex);
    checkItemIsExpanded(tree, itemIndex);
}

void GTTreeView::scrollToItem(QTreeView* tree, const QModelIndex& itemIndex) {
    GT_CHECK_RESULT(tree != nullptr, "tree is NULL", );
    GT_CHECK_RESULT(itemIndex.isValid(), "itemIndex is not valid", );

    class ScrollInMainThreadScenario : public CustomScenario {
    public:
        ScrollInMainThreadScenario(QTreeView* _treeView, const QModelIndex& _itemIndex)
            : tree(_treeView), itemIndex(_itemIndex) {
        }
        void run() override {
            tree->scrollTo(itemIndex);
        }
        QTreeView* tree = nullptr;
        QModelIndex itemIndex;
    };
    GTThread::runInMainThread(new ScrollInMainThreadScenario(tree, itemIndex));
    GTThread::waitForMainThread();
}

void GTTreeView::click(QTreeView* tree, const QModelIndex& itemIndex, const Qt::Key& keyModifier) {
    GT_CHECK_RESULT(tree != nullptr, "tree is NULL", );
    GT_CHECK_RESULT(itemIndex.isValid(), "itemIndex is not valid", );

    scrollToItem(tree, itemIndex);

    QPoint point = getItemCenter(tree, itemIndex);
    GTMouseDriver::moveTo(point);
    if (keyModifier != Qt::Key_unknown) {
        GTKeyboardDriver::keyPress(keyModifier);
    }
    GTMouseDriver::click();
    if (keyModifier != Qt::Key_unknown) {
        GTKeyboardDriver::keyRelease(keyModifier);
    }
}

void GTTreeView::doubleClick(QTreeView* tree, const QModelIndex& itemIndex) {
    GT_CHECK_RESULT(tree != nullptr, "tree is NULL", );
    GT_CHECK_RESULT(itemIndex.isValid(), "itemIndex is not valid", );

    scrollToItem(tree, itemIndex);

    QPoint point = getItemCenter(tree, itemIndex);
    GTMouseDriver::moveTo(point);
    GTMouseDriver::doubleClick();
}

#undef GT_CLASS_NAME
}  // namespace HI
