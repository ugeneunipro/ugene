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

#include "TvUnrootedBranchItem.h"

#include <QGraphicsView>
#include <QPainter>
#include <QPen>
#include <QtMath>

#include <U2Core/PhyTreeObject.h>

#include "TvRectangularBranchItem.h"
#include "TvTextItem.h"

namespace U2 {

TvUnrootedBranchItem::TvUnrootedBranchItem(QGraphicsItem* parent, double angle, TvRectangularBranchItem* from, const QString& nodeName)
    : TvBranchItem(true, from->getSide(), nodeName) {
    setParentItem(parent);
    correspondingRectangularBranchItem = from;
    settings = from->getSettings();
    width = from->getWidth();
    setDist(from->getDist());
    setPos(width, 0);
    angle = side == TvBranchItem::Side::Right ? angle : -angle;
    setTransform(QTransform().translate(-width, 0).rotate(angle).translate(width, 0));
    //    setTransformOriginPoint(-w, 0);
    //    setRotation(angle);

    if (from->getNameTextItem() != nullptr) {
        nameTextItem = new TvTextItem(this, from->getNameTextItem()->text());
        nameTextItem->setFont(from->getNameTextItem()->font());
        nameTextItem->setBrush(from->getNameTextItem()->brush());
    }
    if (from->getDistanceTextItem() != nullptr) {
        distanceTextItem = new TvTextItem(this, from->getDistanceTextItem()->text());
        distanceTextItem->setFont(from->getDistanceTextItem()->font());
        distanceTextItem->setBrush(from->getDistanceTextItem()->brush());
    }
    setLabelPositions();
    setPen(from->pen());
}

void TvUnrootedBranchItem::setLabelPositions() {
    if (nameTextItem != nullptr) {
        QRectF rect = nameTextItem->boundingRect();
        double h = rect.height();
        nameTextItem->setPos(TvBranchItem::TEXT_SPACING, -h * 0.5);
        if (nameTextItem->scenePos().x() < 0.0) {
            QPointF p = rect.center();
            nameTextItem->setTransform(QTransform().translate(p.x(), p.y()).rotate(180).translate(-p.x(), -p.y()));
        }
    }
    if (distanceTextItem != nullptr) {
        QRectF rect = distanceTextItem->boundingRect();
        if (distanceTextItem->scenePos().x() < 0) {
            QPointF p(rect.center().x(), rect.height());
            distanceTextItem->setTransform(QTransform().translate(p.x(), p.y()).rotate(180).translate(-p.x(), -p.y()));
        }
        distanceTextItem->setPos(-0.5 * (width + rect.width()), -rect.height());
    }
}

QRectF TvUnrootedBranchItem::boundingRect() const {
    double penWidth = 1;
    return {-width, -penWidth * 0.5, width, penWidth};
}

void TvUnrootedBranchItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    setUpPainter(painter);
    painter->drawLine(0, 0, -width, 0);
}

}  // namespace U2
