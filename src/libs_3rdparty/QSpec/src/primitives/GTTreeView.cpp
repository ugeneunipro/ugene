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

#include "primitives/GTTreeView.h"

#include <QAbstractItemModel>

#include "utils/GTThread.h"

namespace HI {

#define GT_CLASS_NAME "GTTreeView"
#define GT_METHOD_NAME "findIndex"
QModelIndex GTTreeView::findIndex(GUITestOpStatus &os, QTreeView *treeView, QVariant data, Qt::ItemDataRole role, const GTGlobals::FindOptions &options) {
    return findIndex(os, treeView, data, QModelIndex(), role, options);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findIndex"
QModelIndex GTTreeView::findIndex(GUITestOpStatus &os, QTreeView *treeView, QVariant data, QModelIndex parent, Qt::ItemDataRole role, const GTGlobals::FindOptions &options) {
    GT_CHECK_RESULT(treeView != NULL, "Tree widget is NULL", QModelIndex());

    QModelIndexList foundIndexes = findIndexes(os, treeView, data, role, parent, 0, options);
    if (foundIndexes.isEmpty()) {
        if (options.failIfNotFound) {
            GT_CHECK_RESULT(foundIndexes.size() != 0, QString("Item whith name %1 not found").arg(data.toString()), QModelIndex());
        } else {
            return QModelIndex();
        }
    }

    GT_CHECK_RESULT(foundIndexes.size() == 1, QString("there are %1 items with name %2").arg(foundIndexes.size()).arg(data.toString()), QModelIndex());

    treeView->scrollTo(foundIndexes.at(0));
    return foundIndexes.at(0);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findIndexes"
QModelIndexList GTTreeView::findIndexes(GUITestOpStatus &os, QTreeView *treeView, QVariant data, Qt::ItemDataRole role, QModelIndex parent, int depth, const GTGlobals::FindOptions &options) {
    QModelIndexList foundIndexes;
    if (!(GTGlobals::FindOptions::INFINITE_DEPTH == options.depth || depth < options.depth)) {
        return foundIndexes;
    }

    QAbstractItemModel *model = treeView->model();
    CHECK_SET_ERR_RESULT(NULL != model, "Model is NULL", foundIndexes);

    int rowcount = model->rowCount(parent);
    for (int i = 0; i < rowcount; i++) {
        const QModelIndex index = model->index(i, 0, parent);
        QVariant indexData = index.data(role);

        if (data == indexData) {
            foundIndexes << index;
        } else {
            foundIndexes << findIndexes(os, treeView, data, role, index, depth + 1, options);
        }
    }

    return foundIndexes;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getItemCenter"
QPoint GTTreeView::getItemCenter(GUITestOpStatus &, QTreeView *treeView, const QModelIndex &itemIndex) {
    QRect r = treeView->visualRect(itemIndex);

    return treeView->mapToGlobal(r.center());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkItemIsExpanded"
void GTTreeView::checkItemIsExpanded(HI::GUITestOpStatus &os, QTreeView *treeView, const QModelIndex &itemIndex) {
    GT_CHECK(treeView != nullptr, "Tree view is null!");
    GT_CHECK(itemIndex.isValid(), "Item index is not valid!");

    bool isExpanded = false;
    for (int time = 0; time < GT_OP_WAIT_MILLIS && !isExpanded; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        isExpanded = treeView->isExpanded(itemIndex);
    }
    GT_CHECK(isExpanded, "Item is not expanded");
    GTThread::waitForMainThread();
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME
}  // namespace HI
