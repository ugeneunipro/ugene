/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>

#include <QGraphicsItem>
#include <QTreeWidget>

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
