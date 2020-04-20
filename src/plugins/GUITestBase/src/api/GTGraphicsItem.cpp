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

#include "api/GTGraphicsItem.h"
#include <QGraphicsView>

namespace U2 {
#define GT_CLASS_NAME "GTGraphicsItem"
#define GT_METHOD_NAME "getGraphicsItemRect"
QRect GTGraphicsItem::getGraphicsItemRect(HI::GUITestOpStatus &os, QGraphicsItem* it){
    QGraphicsView* sceneView = qobject_cast<QGraphicsView*>(it->scene()->views().at(0));
    GT_CHECK_RESULT(sceneView,"sceneView not found", QRect());

    QPointF scenePButton = it->mapToScene(it->boundingRect().bottomRight());
    QPoint viewP = sceneView->mapFromScene(scenePButton);
    QPoint globalBottomRightPos = sceneView->viewport()->mapToGlobal(viewP);

    QPointF scenePTop = it->mapToScene(it->boundingRect().topLeft());
    viewP = sceneView->mapFromScene(scenePTop);
    QPoint globalTopLeftPos = sceneView->viewport()->mapToGlobal(viewP);

    QRect globalRect(globalTopLeftPos,globalBottomRightPos);

    return globalRect;
}
#undef GT_METHOD_NAME

QPoint GTGraphicsItem::getItemCenter(HI::GUITestOpStatus &os, QGraphicsItem *it){
    QRect r = getGraphicsItemRect(os, it);
    QPoint p = r.center();
    return p;
}
#undef GT_CLASS_NAME
} //namespace
