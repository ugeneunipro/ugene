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

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainter>
#include <QPen>
#include <QStack>
#include <QtMath>

#include <U2Core/AppContext.h>
#include <U2Core/PhyTreeObject.h>

#include "GraphicsButtonItem.h"
#include "GraphicsRectangularBranchItem.h"
#include "GraphicsUnrootedBranchItem.h"
#include "TreeViewerUtils.h"

namespace U2 {

GraphicsUnrootedBranchItem::GraphicsUnrootedBranchItem(QGraphicsItem* parent, qreal angle, GraphicsRectangularBranchItem* from, double nodeValue)
: GraphicsBranchItem(true, nodeValue) {
    setParentItem(parent);
    qreal w = from->getWidth();
    settings = from->getSettings();
    setWidthW(w);
    setDist(from->getDist());
    setPos(w, 0);
    angle = from->getDirection() == GraphicsBranchItem::up ? angle : -angle;
    setTransform(QTransform().translate(-w, 0).rotate(angle).translate(w, 0));
//    setTransformOriginPoint(-w, 0);
//    setRotation(angle);

    if (from->getNameText() != NULL) {
        nameText = new QGraphicsSimpleTextItem(from->getNameText()->text(), this);
        nameText->setFont(from->getNameText()->font());
        nameText->setBrush(from->getNameText()->brush());
    }
    if (from->getDistanceText() != NULL) {
        distanceText = new QGraphicsSimpleTextItem(from->getDistanceText()->text(), this);
        distanceText->setFont(from->getDistanceText()->font());
        distanceText->setBrush(from->getDistanceText()->brush());
    }
    setLabelPositions();
    setPen(from->pen());
}

void GraphicsUnrootedBranchItem::setLabelPositions() {
    if(nameText != NULL) {
        QRectF rect = nameText->boundingRect();
        qreal h = rect.height();
        nameText->setPos(GraphicsBranchItem::TextSpace, -h * 0.5);
        if (nameText->scenePos().x() < 0.0) {
            QPointF p = rect.center();
            nameText->setTransform(QTransform().translate(p.x(), p.y()).rotate(180).translate(-p.x(), -p.y()));
        }
    }
    if(distanceText != NULL) {
        QRectF rect = distanceText->boundingRect();
        if (distanceText->scenePos().x() < 0) {
            QPointF p(rect.center().x(), rect.height());
            distanceText->setTransform(QTransform().translate(p.x(), p.y()).rotate(180).translate(-p.x(), -p.y()));
        }
        distanceText->setPos(-0.5 * (width + rect.width()), -rect.height());

    }
}

QRectF GraphicsUnrootedBranchItem::boundingRect() const {
    qreal penWidth = 1;
    return QRectF(-width, -penWidth * 0.5, width, penWidth);
}

void GraphicsUnrootedBranchItem::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*) {
    painter->setPen(pen());
    painter->drawLine(0, 0, -width, 0);
}

}//namespace
