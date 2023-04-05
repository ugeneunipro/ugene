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

#include "TvRectangularBranchItem.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QPainter>
#include <QStack>

#include <U2Core/Log.h>
#include <U2Core/PhyTreeObject.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

TvRectangularBranchItem::TvRectangularBranchItem(TvRectangularBranchItem* parentBranch, const PhyBranch* branch, const QString& name, bool isRoot)
    : TvBranchItem(parentBranch, branch, name, isRoot) {
}

QRectF TvRectangularBranchItem::boundingRect() const {
    return {-width - 0.5, side == Side::Right ? -height : -0.5, width + 0.5, height + 0.5};
}

void TvRectangularBranchItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    setUpPainter(painter);
    CHECK(width > 0, );

    // (0, 0) is a coordinate of the tip of the branch -> move (0, 0) to the root side point.
    // Note: can't use QMatrix directly here because the current painter's matrix is not empty at this point.
    int ySign = side == Side::Right ? 1 : -1;
    QPointF translation(-width, -ySign * height);
    painter->translate(translation);

    double curveSegmentWidth = width * curvature / 100;
    double curveSegmentHeight = height * curvature / 100;

    // Use the same curve width (depth) with sibling branch: minimum for both.
    // It makes branches of different length to look symmetrical and helps to avoid intersections.
    QList<QGraphicsItem*> siblings = parentItem()->childItems();
    for (QGraphicsItem* item : qAsConst(siblings)) {
        CHECK_CONTINUE(item != this);
        if (auto rectBranchItem = dynamic_cast<TvRectangularBranchItem*>(item)) {
            double branchCurveSegmentWidth = rectBranchItem->width * curvature / 100;
            curveSegmentWidth = qMin(branchCurveSegmentWidth, curveSegmentWidth);
        }
    }

    QPointF curveStartPoint = QPointF(0, ySign * (height - curveSegmentHeight));
    QPointF curveEndPoint = QPointF(curveSegmentWidth, ySign * height);

    // Draw straight line segments.
    painter->drawLine(QPointF(0, 0), curveStartPoint);  // Vertical segment.
    painter->drawLine(curveEndPoint, QPointF(width, ySign * height));  // Horizontal segment.

    // Draw curve between straight line segments if needed.
    if (curvature > 0) {
        double controlPointOffset = qMin(curveSegmentHeight, curveSegmentWidth) / 2;
        QPointF controlPoint1(0, ySign * (height - controlPointOffset));
        QPointF controlPoint2(controlPointOffset, ySign * height);
        QPainterPath path(curveStartPoint);
        path.cubicTo(controlPoint1, controlPoint2, curveEndPoint);
        painter->drawPath(path);
    }
    painter->translate(-translation);  // Restore original offsets.
}

void TvRectangularBranchItem::setHeight(double newHeight) {
    CHECK(height != newHeight, );
    double delta = newHeight - height;
    setPos(pos() + QPointF(0, side == Side::Right ? delta : -delta));
    prepareGeometryChange();
    height = newHeight;
}

void TvRectangularBranchItem::setBreathScaleAdjustment(double newBreadthScaleAdjustment) {
    SAFE_POINT(newBreadthScaleAdjustment > 0, "Illegal breadth scale adjustment: " + QString::number(newBreadthScaleAdjustment), )
    CHECK(newBreadthScaleAdjustment != breadthScaleAdjustment, )
    double newHeight = height * newBreadthScaleAdjustment / breadthScaleAdjustment;
    breadthScaleAdjustment = newBreadthScaleAdjustment;
    setHeight(newHeight);
}

void TvRectangularBranchItem::setCurvature(double newCurvature) {
    CHECK(newCurvature != curvature, );
    curvature = newCurvature;
    prepareGeometryChange();
}

double TvRectangularBranchItem::getHeight() const {
    return height;
}

}  // namespace U2
