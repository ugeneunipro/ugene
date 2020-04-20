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
#include "GraphicsCircularBranchItem.h"
#include "GraphicsRectangularBranchItem.h"
#include "TreeViewerUtils.h"

namespace U2 {

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

GraphicsCircularBranchItem::GraphicsCircularBranchItem(QGraphicsItem* parent, qreal h, GraphicsRectangularBranchItem* from, double nodeValue)
: GraphicsBranchItem(true, nodeValue), height(h), direction(from->getDirection()), visible(true) {
    setParentItem(parent);
    settings = from->getSettings();
    qreal w = from->getWidth();
    setWidthW(w);
    setDist(from->getDist());
    setPos(w, 0);
    QPointF p = mapFromScene(0, 0);
    setTransform(QTransform().translate(p.x(), p.y()).rotate((direction == GraphicsBranchItem::up ? -1 : 1) * h / M_PI * 180).translate(-p.x(), -p.y()));

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

QRectF GraphicsCircularBranchItem::boundingRect() const {
    QPointF p = scenePos();
    qreal rad = qSqrt(p.x() * p.x() + p.y() * p.y());
    qreal w = width + rad * (1 - qCos(height));
    qreal h = rad * qSin(height);
    return QRectF(-w, direction == GraphicsBranchItem::up ? 0 : -h, w, h);
}

void GraphicsCircularBranchItem::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*) {
    if (!visible)
        return;
    painter->setPen(pen());
    QPointF p = scenePos();
    qreal rad = qSqrt(p.x() * p.x() + p.y() * p.y()) - width;
    QRectF rect(-2 * rad - width, -rad, 2 * rad, 2 * rad);
    painter->drawArc(rect, 0, (direction == GraphicsBranchItem::up ? -1 : 1) * height * 16 * 180 / M_PI);
    painter->drawLine(0, 0, -width, 0);
}

QPainterPath GraphicsCircularBranchItem::shape() const {

    QPainterPath path;

    qreal rad = 30.0; // all hardcode will be deleted later during complete refactoring
    QRectF rect(-2 * rad - width, -rad, 2 * rad, 2 * rad);

    path.lineTo(width, 0);
    path.arcTo(rect, 0, (direction == GraphicsBranchItem::up ? -1 : 1) * height * 16 * 180 / M_PI);

    return path;
}

void GraphicsCircularBranchItem::setLabelPositions() {
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

}//namespace
