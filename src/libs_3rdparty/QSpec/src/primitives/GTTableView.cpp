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

#include "primitives/GTTableView.h"
#include <primitives/GTWidget.h>
#include <utils/GTThread.h>

namespace HI {

#define GT_CLASS_NAME "GTSpinBox"
#define GT_METHOD_NAME "getCellPosition"
QPoint GTTableView::getCellPosition(GUITestOpStatus& os, QTableView* table, int column, int row) {
    GT_CHECK_RESULT(table, "table view is NULL", {});
    QPoint p(table->columnViewportPosition(column) + table->columnWidth(column) / 2,
             table->rowViewportPosition(row) + table->rowHeight(row) * 1.5);
    return table->mapToGlobal(p);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "scrollTo"
void GTTableView::scrollTo(GUITestOpStatus& os, QTableView* table, const QModelIndex& index) {
    // TODO: set index by mouse/keyboard
    class MainThreadAction : public CustomScenario {
    public:
        MainThreadAction(QTableView* _table, const QModelIndex& _index)
            : table(_table), index(_index) {
        }
        void run(HI::GUITestOpStatus&) override {
            table->scrollTo(index);
        }
        QTableView* table = nullptr;
        QModelIndex index;
    };
    GTThread::runInMainThread(os, new MainThreadAction(table, index));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getCellPoint"
QPoint GTTableView::getCellPoint(GUITestOpStatus& os, QTableView* table, int row, int column) {
    QModelIndex idx = table->model()->index(row, column);
    scrollTo(os, table, idx);
    QRect cellRect = table->visualRect(idx);
    QWidget* viewport = table->viewport();
    return viewport->mapToGlobal(cellRect.center());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "rowCount"
int GTTableView::rowCount(GUITestOpStatus& os, QTableView* table) {
    GT_CHECK_RESULT(table != nullptr, "Table view is NULL", -1);
    GT_CHECK_RESULT(table->model() != nullptr, "Table view model is NULL", -1);
    return table->model()->rowCount(QModelIndex());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "data"
QString GTTableView::data(GUITestOpStatus& os, QTableView* table, int row, int column) {
    GT_CHECK_RESULT(table != nullptr, "Table view is NULL", "");
    GT_CHECK_RESULT(table->model() != nullptr, "Table view model is NULL", "");

    QModelIndex idx = table->model()->index(row, column);
    GT_CHECK_RESULT(idx.isValid(), "Item index is invalid", "");
    return table->model()->data(idx, Qt::DisplayRole).toString();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "click"
void GTTableView::click(GUITestOpStatus& os, QTableView* table, int row, int column) {
    GT_CHECK(table != nullptr, "Table is nullptr");
    GT_CHECK(table->model() != nullptr, "Table model is nullptr");

    QModelIndex modelIndex;
    class FindModelIndexScenario : public CustomScenario {
        QTableView* table;
        int rowNum;
        int colNum;
        QModelIndex& modelInd;

    public:
        FindModelIndexScenario(QTableView* table_, int rowNum, int colNum, QModelIndex& modelInd)
            : table(table_), rowNum(rowNum), colNum(colNum), modelInd(modelInd) {
        }
        void run(GUITestOpStatus&) override {
            modelInd = table->model()->index(rowNum, colNum);
        }
    };
    GTThread::runInMainThread(os, new FindModelIndexScenario(table, row, column, modelIndex));
    GT_CHECK(modelIndex.isValid(), "Model index is invalid");

    GTWidget::scrollToIndex(os, table, modelIndex);
    GTWidget::moveToAndClick(GTTableView::getCellPosition(os, table, column, row));
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace HI
