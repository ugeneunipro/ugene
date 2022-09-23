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

#include "GraphicsRectangularBranchItem.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QPainter>
#include <QPen>
#include <QStack>

#include <U2Core/Log.h>
#include <U2Core/PhyTreeObject.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

GraphicsRectangularBranchItem::GraphicsRectangularBranchItem(const QString& name, GraphicsRectangularBranchItem* parentBranchItem)
    : GraphicsBranchItem(name) {
    setParentItem(parentBranchItem);
    setPos(0, 0);
}

GraphicsRectangularBranchItem::GraphicsRectangularBranchItem(double x, double y, const QString& name)
    : GraphicsBranchItem(false, Left) {
    new GraphicsRectangularBranchItem(name, this);
    setPos(x, y);
}

GraphicsRectangularBranchItem::GraphicsRectangularBranchItem(double x, double y, const QString& name, double distance, PhyBranch* branch)
    : GraphicsBranchItem(distance, false), phyBranch(branch) {
    new GraphicsRectangularBranchItem(name, this);
    setPos(x, y);
}

GraphicsRectangularBranchItem::GraphicsRectangularBranchItem(double distance, PhyBranch* branch, double nodeValue)
    : GraphicsBranchItem(distance, true, nodeValue), phyBranch(branch) {
}

GraphicsRectangularBranchItem::GraphicsRectangularBranchItem()
    : GraphicsBranchItem(true, Left) {
}

void GraphicsRectangularBranchItem::toggleCollapsedState() {
    collapsed = !collapsed;
    QStack<GraphicsBranchItem*> graphicsItems;
    graphicsItems.push(this);
    do {
        GraphicsBranchItem* branchItem = graphicsItems.pop();
        QList<QGraphicsItem*> childItems = branchItem->childItems();
        for (QGraphicsItem* graphItem : qAsConst(childItems)) {
            if (dynamic_cast<QGraphicsRectItem*>(graphItem) && !branchItem->isCollapsed()) {
                graphItem->setParentItem(nullptr);
                scene()->removeItem(graphItem);
                continue;
            }

            auto childItem = dynamic_cast<GraphicsRectangularBranchItem*>(graphItem);
            CHECK_CONTINUE(childItem != nullptr);

            childItem->collapsed = !childItem->collapsed;
            if (childItem->getNameTextItem() == nullptr) {
                childItem->setVisible(branchItem->isVisible() && !branchItem->isCollapsed());
            }
            if (childItem->isCollapsed() && !branchItem->isCollapsed()) {
                childItem->drawCollapsedRegion();
            }
            graphicsItems.push(childItem);
        }
    } while (!graphicsItems.isEmpty());

    if (collapsed) {
        drawCollapsedRegion();
    } else {
        setSelected(true);
    }
    getRoot()->emitBranchCollapsed(this);
}

void GraphicsRectangularBranchItem::drawCollapsedRegion() {
    QList<QGraphicsItem*> items = childItems();
    double xMin = 0;
    double yMin = 0;
    double yMax = 0;
    bool isFirstIteration = true;

    for (QGraphicsItem* graphItem : qAsConst(items)) {
        auto branchItem = dynamic_cast<GraphicsRectangularBranchItem*>(graphItem);
        if (!branchItem) {
            continue;
        }
        QPointF pos1 = branchItem->pos();
        if (isFirstIteration) {
            xMin = pos1.x();
            yMin = yMax = pos1.y();
            isFirstIteration = false;
            continue;
        }

        xMin = qMin(xMin, pos1.x());
        yMin = qMin(yMin, pos1.y());
        yMax = qMax(yMax, pos1.y());
    }
    if (xMin >= 2 * GraphicsRectangularBranchItem::DEFAULT_WIDTH)
        xMin /= 2;
    if (xMin < GraphicsRectangularBranchItem::DEFAULT_WIDTH)
        xMin = GraphicsRectangularBranchItem::DEFAULT_WIDTH;

    QPen blackPen(Qt::black);
    prepareGeometryChange();
    blackPen.setWidth(SELECTED_PEN_WIDTH);
    blackPen.setCosmetic(true);
    double defHeight = qMin((int)(yMax - yMin) / 2, 30);
    auto rectItem = new QGraphicsRectItem(0, -defHeight / 2, xMin, defHeight, this);
    rectItem->setPen(blackPen);
}

void GraphicsRectangularBranchItem::setParentItem(QGraphicsItem* item) {
    prepareGeometryChange();
    height = side == Right ? pos().y() - item->pos().y() : item->pos().y() - pos().y();
    setPos(width, side == Right ? height : -height);

    QAbstractGraphicsShapeItem::setParentItem(item);
}

void GraphicsRectangularBranchItem::setSide(const Side& newSide) {
    CHECK(side != newSide, );
    prepareGeometryChange();
    side = newSide;
}

QRectF GraphicsRectangularBranchItem::boundingRect() const {
    return {-width - 0.5, side == Right ? -height : -0.5, width + 0.5, height + 0.5};
}

void GraphicsRectangularBranchItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* optionItem, QWidget*) {
    painter->setPen(pen());
    if (!qFuzzyCompare(width, 0)) {
        painter->drawLine(QPointF(0, 0), QPointF(-width, 0));
        painter->drawLine(QPointF(-width, 0), QPointF(-width, side == Right ? -height : height));
    }
    GraphicsBranchItem::paint(painter, optionItem);
}

void GraphicsRectangularBranchItem::setHeight(double newHeight) {
    CHECK(height != newHeight, );
    double delta = newHeight - height;
    setPos(pos() + QPointF(0, side == Right ? delta : -delta));
    prepareGeometryChange();
    height = newHeight;
}

void GraphicsRectangularBranchItem::setBreathScaleAdjustment(double newBreadthScaleAdjustment) {
    SAFE_POINT(newBreadthScaleAdjustment > 0, "Illegal breadth scale adjustment: " + QString::number(newBreadthScaleAdjustment), )
    CHECK(newBreadthScaleAdjustment != breadthScaleAdjustment, )
    double newHeight = height * newBreadthScaleAdjustment / breadthScaleAdjustment;
    breadthScaleAdjustment = newBreadthScaleAdjustment;
    setHeight(newHeight);
}

void GraphicsRectangularBranchItem::swapSiblings() {
    CHECK(phyBranch != nullptr, );
    PhyNode* nodeTo = phyBranch->node2;
    int branchCount = nodeTo->getBranches().size();
    if (branchCount > 2) {
        nodeTo->swapBranches(0, 2);
    }
}

GraphicsRectangularBranchItem::Side GraphicsRectangularBranchItem::getSide() const {
    return side;
}

double GraphicsRectangularBranchItem::getHeight() const {
    return height;
}

const PhyBranch* GraphicsRectangularBranchItem::getPhyBranch() const {
    return phyBranch;
}

}  // namespace U2
