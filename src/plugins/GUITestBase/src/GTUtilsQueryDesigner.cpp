/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#define GT_METHOD_NAME "openQueryDesigner"
void GTUtilsQueryDesigner::openQueryDesigner(HI::GUITestOpStatus& os) {
    GTMenu::clickMainMenuItem(os, {"Tools", "Query Designer..."});
    GTGlobals::sleep(500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findAlgorithm"
QTreeWidgetItem* GTUtilsQueryDesigner::findAlgorithm(HI::GUITestOpStatus& os, const QString& itemName) {
    QTreeWidgetItem* foundItem = nullptr;
    auto w = GTWidget::findTreeWidget(os, "palette");

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
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "addAlgorithm"
void GTUtilsQueryDesigner::addAlgorithm(HI::GUITestOpStatus& os, const QString& algName) {
    QTreeWidgetItem* w = findAlgorithm(os, algName);
    GTMouseDriver::click(GTTreeWidget::getItemCenter(os, w));
    GTWidget::click(os, GTWidget::findWidget(os, "sceneView"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getItemCenter"
QPoint GTUtilsQueryDesigner::getItemCenter(HI::GUITestOpStatus& os, const QString& itemName) {
    QRect r = getItemRect(os, itemName);
    return r.center();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getItemLeft"
int GTUtilsQueryDesigner::getItemLeft(HI::GUITestOpStatus& os, const QString& itemName) {
    QRect r = getItemRect(os, itemName);
    return r.left();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getItemRight"
int GTUtilsQueryDesigner::getItemRight(HI::GUITestOpStatus& os, const QString& itemName) {
    QRect r = getItemRect(os, itemName);
    return r.right() - 1;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getItemTop"
int GTUtilsQueryDesigner::getItemTop(HI::GUITestOpStatus& os, const QString& itemName) {
    QRect r = getItemRect(os, itemName);
    return r.top();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getItemBottom"
int GTUtilsQueryDesigner::getItemBottom(HI::GUITestOpStatus& os, const QString& itemName) {
    QRect r = getItemRect(os, itemName);
    return r.bottom();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getItemRect"
QRect GTUtilsQueryDesigner::getItemRect(HI::GUITestOpStatus& os, const QString& itemName) {
    auto sceneView = GTWidget::findGraphicsView(os, "sceneView");
    QList<QGraphicsItem*> items = sceneView->items();

    for (QGraphicsItem* it : qAsConst(items)) {
        QGraphicsObject* itObj = it->toGraphicsObject();
        if (auto textItemO = qobject_cast<QGraphicsTextItem*>(itObj)) {
            QString text = textItemO->toPlainText();
            if (text.contains(itemName)) {
                return GTGraphicsItem::getGraphicsItemRect(os, it->parentItem());
            }
        }
    }
    FAIL("Item not found: " + itemName, {});
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME
}  // namespace U2
