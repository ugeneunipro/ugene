/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include "GTUtilsQueryDesigner.h"
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTTableView.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>

#include <QGraphicsItem>
#include <QTreeWidget>

#include <U2Core/AppContext.h>
#include <U2Core/U2IdTypes.h>

#include "GTUtilsMdi.h"
#include "api/GTGraphicsItem.h"
#include "primitives/GTMenu.h"

namespace U2 {
using namespace HI;
#define GT_CLASS_NAME "GTUtilsQueryDesigner"

void GTUtilsQueryDesigner::openQueryDesigner() {
    GTMenu::clickMainMenuItem({"Tools", "Query Designer..."});
    GTGlobals::sleep(500);
}

QWidget* GTUtilsQueryDesigner::getActiveQueryDesignerWindow() {
    QWidget* wdWindow = nullptr;
    for (int time = 0; time < GT_OP_WAIT_MILLIS && wdWindow == nullptr; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        MainWindow* mainWindow = AppContext::getMainWindow();
        QWidget* mdiWindow = mainWindow == nullptr ? nullptr : mainWindow->getMDIManager()->getActiveWindow();
        if (mdiWindow != nullptr && mdiWindow->objectName() == "Query Designer") {
            wdWindow = mdiWindow;
        }
    }
    GT_CHECK_RESULT(wdWindow != nullptr, "No active QD window!", nullptr);
    GTThread::waitForMainThread();
    return wdWindow;
}

namespace {

int getParameterRow(QTableView* table, const QString& parameter) {
    QAbstractItemModel* model = table->model();
    int iMax = model->rowCount();
    for (int i = 0; i < iMax; i++) {
        QString s = model->data(model->index(i, 0)).toString();
        if (s == parameter) {
            return i;
        }
    }
    return -1;
}

}  // namespace

void GTUtilsQueryDesigner::clickParameter(const QString& parameter) {
    QWidget* wdWindow = getActiveQueryDesignerWindow();
    auto table = GTWidget::findTableView("table", wdWindow);

    // FIND CELL
    const int row = getParameterRow(table, parameter);
    GT_CHECK_RESULT(row != -1, "parameter not found", );

    QAbstractItemModel* model = table->model();
    table->scrollTo(model->index(row, 1));
    GTMouseDriver::moveTo(GTTableView::getCellPosition(table, 1, row));
    GTMouseDriver::click();
    GTGlobals::sleep(500);
}


QTreeWidgetItem* GTUtilsQueryDesigner::findAlgorithm(const QString& itemName) {
    QTreeWidgetItem* foundItem = nullptr;
    auto w = GTWidget::findTreeWidget("palette");

    QList<QTreeWidgetItem*> outerList = w->findItems("", Qt::MatchContains);

    for (int i = 0; i < outerList.count(); i++) {
        QList<QTreeWidgetItem*> innerList;

        for (int j = 0; j < outerList.value(i)->childCount(); j++) {
            innerList.append(outerList.value(i)->child(j));
        }

        foreach (QTreeWidgetItem* item, innerList) {
            if (item->text(0) == itemName) {
                foundItem = item;
            }
        }
    }
    CHECK_SET_ERR_RESULT(foundItem != nullptr, "Item is null", nullptr);
    return foundItem;
}

void GTUtilsQueryDesigner::addAlgorithm(const QString& algName) {
    QTreeWidgetItem* w = findAlgorithm(algName);
    GTMouseDriver::click(GTTreeWidget::getItemCenter(w));
    GTWidget::click(GTWidget::findWidget("sceneView"));
}

QPoint GTUtilsQueryDesigner::getItemCenter(const QString& itemName) {
    QRect r = getItemRect(itemName);
    return r.center();
}

int GTUtilsQueryDesigner::getItemLeft(const QString& itemName) {
    QRect r = getItemRect(itemName);
    return r.left();
}

int GTUtilsQueryDesigner::getItemRight(const QString& itemName) {
    QRect r = getItemRect(itemName);
    return r.right() - 1;
}

int GTUtilsQueryDesigner::getItemTop(const QString& itemName) {
    QRect r = getItemRect(itemName);
    return r.top();
}

int GTUtilsQueryDesigner::getItemBottom(const QString& itemName) {
    QRect r = getItemRect(itemName);
    return r.bottom();
}

QRect GTUtilsQueryDesigner::getItemRect(const QString& itemName) {
    auto sceneView = GTWidget::findGraphicsView("sceneView");
    QList<QGraphicsItem*> items = sceneView->items();

    for (QGraphicsItem* it : qAsConst(items)) {
        QGraphicsObject* itObj = it->toGraphicsObject();
        if (auto textItemO = qobject_cast<QGraphicsTextItem*>(itObj)) {
            QString text = textItemO->toPlainText();
            if (text.contains(itemName)) {
                return GTGraphicsItem::getGraphicsItemRect(it->parentItem());
            }
        }
    }
    FAIL("Item not found: " + itemName, {});
}

#undef GT_CLASS_NAME
}  // namespace U2
