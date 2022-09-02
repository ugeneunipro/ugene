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

#include "GraphicsUnrootedBranchItem.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainter>
#include <QPen>
#include <QStack>
#include <QtMath>

#include <U2Core/AppContext.h>
#include <U2Core/PhyTreeObject.h>

#include "GraphicsRectangularBranchItem.h"

namespace U2 {

GraphicsUnrootedBranchItem::GraphicsUnrootedBranchItem(QGraphicsItem* parent, double angle, GraphicsRectangularBranchItem* from, double nodeValue)
    : GraphicsBranchItem(true, nodeValue) {
    setParentItem(parent);
    correspondingRectangularBranchItem = from;
    settings = from->getSettings();
    width = from->getWidth();
    setDist(from->getDist());
    setPos(width, 0);
    angle = from->getSide() == GraphicsBranchItem::Right ? angle : -angle;
    setTransform(QTransform().translate(-width, 0).rotate(angle).translate(width, 0));
    //    setTransformOriginPoint(-w, 0);
    //    setRotation(angle);

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

void GraphicsUnrootedBranchItem::setLabelPositions() {
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

QRectF GraphicsUnrootedBranchItem::boundingRect() const {
    double penWidth = 1;
    return QRectF(-width, -penWidth * 0.5, width, penWidth);
}

void GraphicsUnrootedBranchItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    painter->setPen(pen());
    painter->drawLine(0, 0, -width, 0);
}

}  // namespace U2
