/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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

#include <U2Core/U2IdTypes.h>
#include "GTUtilsQueryDesigner.h"
#include "api/GTGraphicsItem.h"
#include "primitives/GTMenu.h"
#include <primitives/GTWidget.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTTreeWidget.h>
#include "GTUtilsMdi.h"

#include <U2View/MSAEditor.h>
#include <QTreeWidget>
#include <QGraphicsItem>
#include <QGraphicsView>

namespace U2 {
using namespace HI;
#define GT_CLASS_NAME "GTUtilsQueryDesigner"

#define GT_METHOD_NAME "findTreeItem"
void GTUtilsQueryDesigner::openQueryDesigner(HI::GUITestOpStatus &os) {
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "Query Designer...");
    GTGlobals::sleep(500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findTreeItem"
QTreeWidgetItem* GTUtilsQueryDesigner::findAlgorithm(HI::GUITestOpStatus &os,QString itemName){
    QTreeWidgetItem* foundItem = NULL;
    QTreeWidget *w=qobject_cast<QTreeWidget*>(GTWidget::findWidget(os,"palette"));

    QList<QTreeWidgetItem*> outerList = w->findItems("",Qt::MatchContains);

    for (int i=0;i<outerList.count();i++){
        QList<QTreeWidgetItem*> innerList;

        for(int j=0;j<outerList.value(i)->childCount();j++ ){
           innerList.append(outerList.value(i)->child(j));
        }

        foreach(QTreeWidgetItem* item, innerList){
            if(item->text(0)==itemName){
                foundItem = item;
            }
        }
    }
    CHECK_SET_ERR_RESULT(foundItem!=NULL,"Item is null", NULL);
    return foundItem;
}
#undef GT_METHOD_NAME

void GTUtilsQueryDesigner::addAlgorithm(HI::GUITestOpStatus &os, QString algName){
    QTreeWidgetItem *w = findAlgorithm(os, algName);
    GTGlobals::sleep(500);
    CHECK_SET_ERR(w!=NULL,"algorithm is NULL");

    GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os,w));

    GTWidget::click(os, GTWidget::findWidget(os,"sceneView"));
}

QPoint GTUtilsQueryDesigner::getItemCenter(HI::GUITestOpStatus &os,QString itemName){
    QRect r = getItemRect(os, itemName);
    QPoint p = r.center();
    return p;
}

int GTUtilsQueryDesigner::getItemLeft(HI::GUITestOpStatus &os, QString itemName){
    QRect r = getItemRect(os, itemName);
    int i = r.left();
    return i;
}

int GTUtilsQueryDesigner::getItemRight(HI::GUITestOpStatus &os, QString itemName){
    QRect r = getItemRect(os, itemName);
    int i = r.right() - 1;
    return i;
}

int GTUtilsQueryDesigner::getItemTop(HI::GUITestOpStatus &os, QString itemName){
    QRect r = getItemRect(os, itemName);
    int i = r.top();
    return i;
}

int GTUtilsQueryDesigner::getItemBottom(HI::GUITestOpStatus &os, QString itemName){
    QRect r = getItemRect(os, itemName);
    int i = r.bottom();
    return i;
}

QRect GTUtilsQueryDesigner::getItemRect(HI::GUITestOpStatus &os,QString itemName){

    QGraphicsView* sceneView = qobject_cast<QGraphicsView*>(GTWidget::findWidget(os,"sceneView"));
    QList<QGraphicsItem *> items = sceneView->items();

    foreach(QGraphicsItem* it, items) {
        QGraphicsObject *itObj = it->toGraphicsObject();

        QGraphicsTextItem* textItemO = qobject_cast<QGraphicsTextItem*>(itObj);
        if (textItemO) {
            QString text = textItemO->toPlainText();
            if (text.contains(itemName)) {
                return GTGraphicsItem::getGraphicsItemRect(os, it->parentItem());
            }
        }
    }
   return QRect();
}

#undef GT_CLASS_NAME
} // namespace
