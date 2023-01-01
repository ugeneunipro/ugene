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

#include "TvCircularBranchItem.h"

#include <QGraphicsView>
#include <QPainter>
#include <QPen>
#include <QtMath>

#include <U2Core/PhyTreeObject.h>
#include <U2Core/U2SafePoints.h>

#include "TvRectangularBranchItem.h"
#include "TvTextItem.h"

namespace U2 {

TvCircularBranchItem::TvCircularBranchItem(QGraphicsItem* parent, double h, TvRectangularBranchItem* from, const QString& nodeName)
    : TvBranchItem(true, from->getSide(), nodeName), height(h) {
    setParentItem(parent);
    correspondingRectangularBranchItem = from;
    settings = from->getSettings();
    width = from->getWidth();
    setDist(from->getDist());
    setPos(width, 0);
    QPointF p = mapFromScene(0, 0);
    double angle = (side == TvBranchItem::Side::Right ? 1 : -1) * height / M_PI * 180;
    setTransform(QTransform().translate(p.x(), p.y()).rotate(angle).translate(-p.x(), -p.y()));

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

QRectF TvCircularBranchItem::boundingRect() const {
    QPointF p = scenePos();
    double rad = qSqrt(p.x() * p.x() + p.y() * p.y());
    double w = width + rad * (1 - qCos(height));
    double h = rad * qSin(height);
    return {-w, side == TvBranchItem::Side::Right ? -h : 0, w, h};
}

void TvCircularBranchItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    CHECK(!isRoot(), );  // Do not render root branch.
    setUpPainter(painter);
    QPointF p = scenePos();
    double rad = qSqrt(p.x() * p.x() + p.y() * p.y()) - width;
    QRectF rect(-2 * rad - width, -rad, 2 * rad, 2 * rad);
    painter->drawArc(rect, 0, (side == TvBranchItem::Side::Right ? 1 : -1) * height * 16 * 180 / M_PI);
    painter->drawLine(0, 0, -width, 0);
}

QPainterPath TvCircularBranchItem::shape() const {
    QPainterPath path;

    double rad = 30.0;  // all hardcode will be deleted later during complete refactoring
    QRectF rect(-2 * rad - width, -rad, 2 * rad, 2 * rad);

    path.lineTo(width, 0);
    path.arcTo(rect, 0, (side == TvBranchItem::Side::Right ? 1 : -1) * height * 16 * 180 / M_PI);

    return path;
}

void TvCircularBranchItem::setLabelPositions() {
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

}  // namespace U2
