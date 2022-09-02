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

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QPainter>
#include <QPen>
#include <QStack>

#include <U2Core/PhyTreeObject.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

GraphicsRectangularBranchItem::GraphicsRectangularBranchItem(const QString& name, GraphicsRectangularBranchItem* pitem)
    : GraphicsBranchItem(name) {
    setParentItem(pitem);
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
    QList<QGraphicsItem*> items = childItems();

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
        setSelectedRecurs(true, true);
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
    int defHeight = qMin((int)(yMax - yMin) / 2, 30);
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
    return QRectF(-width - 0.5, side == Right ? -height : -0.5, width + 0.5, height + 0.5);
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

void GraphicsRectangularBranchItem::setHeightCoef(int newCoef) {
    CHECK(newCoef != currentHeightCoef, );
    SAFE_POINT(newCoef > 0, "Invalid newCoef", );
    double newHeight = height * newCoef / currentHeightCoef;
    currentHeightCoef = newCoef;
    setHeight(newHeight);
}

void GraphicsRectangularBranchItem::swapSiblings() {
    CHECK(phyBranch != nullptr, );
    PhyNode* nodeTo = phyBranch->node2;
    int branchCount = nodeTo->branchCount();
    if (branchCount > 2) {
        nodeTo->swapBranches(0, 2);
    }
}

// TODO: move to the algorithm.
void GraphicsRectangularBranchItem::recalculateBranches(int& current, const PhyNode* root) {
    int branches = 0;
    const PhyNode* node = nullptr;

    if (phyBranch) {
        node = phyBranch->node2;
    } else if (root) {
        node = root;
    }
    CHECK(node != nullptr, );

    branches = node->branchCount();
    if (branches > 1) {
        QList<GraphicsRectangularBranchItem*> items;
        for (int i = 0; i < branches; ++i) {
            if (node->getSecondNodeOfBranch(i) != node) {
                GraphicsRectangularBranchItem* item = getChildItemByPhyBranch(node->getBranch(i));
                if (item->isVisible()) {
                    item->recalculateBranches(current, nullptr);
                }
                items.append(item);
            } else {
                items.append(nullptr);
            }
        }

        int size = items.size();
        assert(size > 0);

        GraphicsRectangularBranchItem* item = this;
        {
            int xmin = 0, ymin = items[0] ? items[0]->pos().y() : items[1]->pos().y(), ymax = 0;
            for (int i = 0; i < size; ++i) {
                if (items[i] == nullptr) {
                    continue;
                }
                QPointF pos1 = items[i]->pos();
                if (pos1.x() < xmin)
                    xmin = pos1.x();
                if (pos1.y() < ymin)
                    ymin = pos1.y();
                if (pos1.y() > ymax)
                    ymax = pos1.y();
            }
            xmin -= GraphicsRectangularBranchItem::DEFAULT_WIDTH;

            int y = 0;
            if (!item->isCollapsed()) {
                y = (ymax + ymin) / 2;
                item->setPos(xmin, y);
            } else {
                y = (current++ + 0.5) * GraphicsRectangularBranchItem::DEFAULT_HEIGHT;
                item->setPos(0, y);
            }

            for (int i = 0; i < size; ++i) {
                if (items[i] == nullptr) {
                    continue;
                }
                double dist = qAbs(node->getBranchesDistance(i));
                items[i]->setSide(items[i]->pos().y() > y ? GraphicsRectangularBranchItem::Right : GraphicsRectangularBranchItem::Left);
                items[i]->setWidthW(dist);
                items[i]->setDist(dist);
                items[i]->setHeightCoefW(1);
                items[i]->setParentItem(item);
                QRectF rect = items[i]->getDistanceTextItem()->boundingRect();
                items[i]->getDistanceTextItem()->setPos(-(items[i]->getWidth() + rect.width()) / 2, 0);
            }
        }
    } else {
        int y = (current++ + 0.5) * GraphicsRectangularBranchItem::DEFAULT_HEIGHT;
        setPos(0, y);
    }
}

GraphicsRectangularBranchItem* GraphicsRectangularBranchItem::getChildItemByPhyBranch(const PhyBranch* branch) {
    QList<QGraphicsItem*> childItems = this->childItems();
    for (QGraphicsItem* ci : qAsConst(childItems)) {
        if (auto gbi = dynamic_cast<GraphicsRectangularBranchItem*>(ci)) {
            if (gbi->getPhyBranch() == branch) {
                return gbi;
            }
        }
    }
    return nullptr;
}

GraphicsRectangularBranchItem::Side GraphicsRectangularBranchItem::getSide() const {
    return side;
}

double GraphicsRectangularBranchItem::getHeight() const {
    return height;
}

void GraphicsRectangularBranchItem::setHeightW(double h) {
    height = h;
}

void GraphicsRectangularBranchItem::setHeightCoefW(int coef) {
    currentHeightCoef = coef;
}

const PhyBranch* GraphicsRectangularBranchItem::getPhyBranch() const {
    return phyBranch;
}

}  // namespace U2
