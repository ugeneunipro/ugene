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

#include "GraphicsCircularBranchItem.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainter>
#include <QPen>
#include <QStack>
#include <QtMath>

#include <U2Core/AppContext.h>
#include <U2Core/PhyTreeObject.h>
#include <U2Core/U2SafePoints.h>

#include "GraphicsRectangularBranchItem.h"

namespace U2 {

GraphicsCircularBranchItem::GraphicsCircularBranchItem(QGraphicsItem* parent, double h, GraphicsRectangularBranchItem* from, double nodeValue)
    : GraphicsBranchItem(true, from->getSide(), nodeValue), height(h) {
    setParentItem(parent);
    correspondingRectangularBranchItem = from;
    settings = from->getSettings();
    width = from->getWidth();
    setDist(from->getDist());
    setPos(width, 0);
    QPointF p = mapFromScene(0, 0);
    double angle = (side == GraphicsBranchItem::Right ? 1 : -1) * height / M_PI * 180;
    setTransform(QTransform().translate(p.x(), p.y()).rotate(angle).translate(-p.x(), -p.y()));

    if (from->getNameTextItem() != nullptr) {
        nameText = new QGraphicsSimpleTextItem(from->getNameTextItem()->text(), this);
        nameText->setFont(from->getNameTextItem()->font());

        nameText->setBrush(from->getNameTextItem()->brush());
    }
    if (from->getDistanceTextItem() != nullptr) {
        distanceText = new QGraphicsSimpleTextItem(from->getDistanceTextItem()->text(), this);
        distanceText->setFont(from->getDistanceTextItem()->font());
        distanceText->setBrush(from->getDistanceTextItem()->brush());
    }
    setLabelPositions();
    setPen(from->pen());
}

QRectF GraphicsCircularBranchItem::boundingRect() const {
    QPointF p = scenePos();
    double rad = qSqrt(p.x() * p.x() + p.y() * p.y());
    double w = width + rad * (1 - qCos(height));
    double h = rad * qSin(height);
    return QRectF(-w, side == GraphicsBranchItem::Right ? -h : 0, w, h);
}

void GraphicsCircularBranchItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    CHECK(!isRoot(), );  // Do not render root branch.
    painter->setPen(pen());
    QPointF p = scenePos();
    double rad = qSqrt(p.x() * p.x() + p.y() * p.y()) - width;
    QRectF rect(-2 * rad - width, -rad, 2 * rad, 2 * rad);
    painter->drawArc(rect, 0, (side == GraphicsBranchItem::Right ? 1 : -1) * height * 16 * 180 / M_PI);
    painter->drawLine(0, 0, -width, 0);
}

QPainterPath GraphicsCircularBranchItem::shape() const {
    QPainterPath path;

    double rad = 30.0;  // all hardcode will be deleted later during complete refactoring
    QRectF rect(-2 * rad - width, -rad, 2 * rad, 2 * rad);

    path.lineTo(width, 0);
    path.arcTo(rect, 0, (side == GraphicsBranchItem::Right ? 1 : -1) * height * 16 * 180 / M_PI);

    return path;
}

void GraphicsCircularBranchItem::setLabelPositions() {
    if (nameText != nullptr) {
        QRectF rect = nameText->boundingRect();
        double h = rect.height();
        nameText->setPos(GraphicsBranchItem::TEXT_SPACING, -h * 0.5);
        if (nameText->scenePos().x() < 0.0) {
            QPointF p = rect.center();
            nameText->setTransform(QTransform().translate(p.x(), p.y()).rotate(180).translate(-p.x(), -p.y()));
        }
    }
    if (distanceText != nullptr) {
        QRectF rect = distanceText->boundingRect();
        if (distanceText->scenePos().x() < 0) {
            QPointF p(rect.center().x(), rect.height());
            distanceText->setTransform(QTransform().translate(p.x(), p.y()).rotate(180).translate(-p.x(), -p.y()));
        }
        distanceText->setPos(-0.5 * (width + rect.width()), -rect.height());
    }
}

}  // namespace U2
