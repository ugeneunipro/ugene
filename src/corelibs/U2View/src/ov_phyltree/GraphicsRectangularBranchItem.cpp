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

#include "GraphicsRectangularBranchItem.h"
#include "GraphicsButtonItem.h"
#include "TreeViewerUtils.h"

#include <QPainter>
#include <QPen>
#include <QStack>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include <U2Core/PhyTreeObject.h>
#include <U2Core/AppContext.h>

namespace U2 {

const qreal GraphicsRectangularBranchItem::DEFAULT_WIDTH = 25.0;
const qreal GraphicsRectangularBranchItem::MAXIMUM_WIDTH = 500.0;
const int GraphicsRectangularBranchItem::DEFAULT_HEIGHT = 25;
const qreal GraphicsRectangularBranchItem::EPSILON = 0.0000000001;

void GraphicsRectangularBranchItem::collapse() {
    collapsed = !collapsed;
    QList<QGraphicsItem*> items = childItems();

    QStack<GraphicsBranchItem*> graphicsItems;
    graphicsItems.push(this);
    do {
        GraphicsBranchItem* branchItem = graphicsItems.pop();

        QList<QGraphicsItem*> items = branchItem->childItems();

        foreach(QGraphicsItem* graphItem, items) {

            if (dynamic_cast<QGraphicsRectItem*>(graphItem) && !branchItem->isCollapsed()) {
                graphItem->setParentItem(NULL);
                scene()->removeItem(graphItem);
                continue;
            }

            GraphicsRectangularBranchItem* childItem = dynamic_cast<GraphicsRectangularBranchItem*>(graphItem);
            if (NULL == childItem) {
                continue;
            }

            childItem->setCollapsed(!childItem->isCollapsed());
            if(NULL == childItem->getNameText()) {
                childItem->setVisible(branchItem->isVisible() &&!branchItem->isCollapsed());
            }
            if(childItem->isCollapsed() && !branchItem->isCollapsed()) {
                childItem->drawCollapsedRegion();
            }
            graphicsItems.push(childItem);
        }
    } while(!graphicsItems.isEmpty());

    if(collapsed) {
        drawCollapsedRegion();
    }
    else {
        setSelectedRecurs(true, true);
    }

    GraphicsRectangularBranchItem* root = this;
    while(NULL != dynamic_cast<GraphicsRectangularBranchItem*>(root->parentItem())) {
        root = dynamic_cast<GraphicsRectangularBranchItem*>(root->parentItem());
    }
    root->branchCollapsed(this);
}

void GraphicsRectangularBranchItem::drawCollapsedRegion() {
    QList<QGraphicsItem*> items= childItems();
    qreal xMin = 0;
    qreal yMin = 0;
    qreal yMax = 0;
    bool isFirstIteration = true;

    foreach(QGraphicsItem* graphItem, items) {
        GraphicsRectangularBranchItem* branchItem =  dynamic_cast<GraphicsRectangularBranchItem*>(graphItem);
        if (!branchItem)
            continue;
        QPointF pos1 = branchItem ->pos();
        if(isFirstIteration) {
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
    blackPen.setWidth(SelectedPenWidth);
    blackPen.setCosmetic(true);
    const int defHeight = qMin((int)(yMax - yMin) / 2, 30);
    QGraphicsRectItem *r = new QGraphicsRectItem(0, -defHeight / 2, xMin, defHeight, this);
    r->setPen(blackPen);
}

GraphicsRectangularBranchItem::GraphicsRectangularBranchItem(const QString& name, GraphicsRectangularBranchItem* pitem)
: GraphicsBranchItem(name), height(0.0), cur_height_coef(1), direction(GraphicsRectangularBranchItem::up), phyBranch(NULL)
{
    setParentItem(pitem);
    setPos(0, 0);
}

GraphicsRectangularBranchItem::GraphicsRectangularBranchItem(qreal x, qreal y, const QString& name)
: GraphicsBranchItem(false), height(0.0), cur_height_coef(1), direction(GraphicsRectangularBranchItem::up), phyBranch(NULL)
{
    new GraphicsRectangularBranchItem(name, this);
    setPos(x, y);
}

GraphicsRectangularBranchItem::GraphicsRectangularBranchItem(qreal x, qreal y, const QString& name, qreal d, PhyBranch *branch)
: GraphicsBranchItem(d, false), height(0.0), cur_height_coef(1), direction(GraphicsRectangularBranchItem::up), phyBranch(branch)
{
    new GraphicsRectangularBranchItem(name, this);
    setPos(x, y);
}

GraphicsRectangularBranchItem::GraphicsRectangularBranchItem(qreal d, PhyBranch *branch, double nodeValue)
: GraphicsBranchItem(d, true, nodeValue), height(0.0), cur_height_coef(1), direction(GraphicsRectangularBranchItem::up), phyBranch(branch)
{

}

GraphicsRectangularBranchItem::GraphicsRectangularBranchItem()
:  cur_height_coef(1), direction(GraphicsRectangularBranchItem::up), phyBranch(NULL) {}

void GraphicsRectangularBranchItem::setParentItem(QGraphicsItem *item) {
    prepareGeometryChange();
    height = direction == up ? pos().y() - item->pos().y() : item->pos().y() - pos().y();
    setPos(width, direction == up ? height : -height);

    QAbstractGraphicsShapeItem::setParentItem(item);
}

void GraphicsRectangularBranchItem::setDirection(Direction d) {
    prepareGeometryChange();
    direction = d;
}

QRectF GraphicsRectangularBranchItem::boundingRect() const {
    return QRectF(- width - 0.5, direction == up ? -height: -0.5, width + 0.5, height + 0.5);
}

void GraphicsRectangularBranchItem::paint(QPainter *painter, const QStyleOptionGraphicsItem* optionItem, QWidget*) {
    painter->setPen(pen());
    if(false == qFuzzyCompare(width, 0)) {
        painter->drawLine(QPointF(0, 0), QPointF(-width, 0));
        painter->drawLine(QPointF(-width, 0), QPointF(-width, direction == up ? -height : height));
    }
    GraphicsBranchItem::paint(painter, optionItem);
}

void GraphicsRectangularBranchItem::setHeight(qreal h) {
    if (height == h) {
        return;
    }

    if (direction == up) {
        setPos(pos().x(), pos().y() - height + h);
    }else{
        setPos(pos().x(), pos().y() + height - h);
    }

    prepareGeometryChange();
    height = h;
}

void GraphicsRectangularBranchItem::setHeightCoef(int coef){
    if(coef == cur_height_coef){
        return ;
    }

    qreal h = (height / (qreal)cur_height_coef) * coef;
    cur_height_coef = coef;
    setHeight(h);
}

void GraphicsRectangularBranchItem::swapSiblings() {
    if (!phyBranch) {
        return ;
    }

    PhyNode* nodeTo = phyBranch->node2;
    int branchCount = nodeTo->branchCount();
    if (branchCount > 2) {
        nodeTo->swapBranches(0, 2);
    }
}

void GraphicsRectangularBranchItem::redrawBranches(int& current, qreal& minDistance, qreal& maxDistance, const PhyNode* root){
    int branches = 0;
    const PhyNode* node = NULL;

    if(phyBranch){
        node = phyBranch->node2;
    }else if(root){
        node = root;
    }

    if (node == NULL) {
        return ;
    }

    branches = node->branchCount();
    if(branches > 1){
        QList<GraphicsRectangularBranchItem*> items;
        for (int i = 0; i < branches; ++i) {
            if (node->getSecondNodeOfBranch(i) != node) {
                GraphicsRectangularBranchItem *item = getChildItemByPhyBranch(node->getBranch(i));
                if(item->isVisible()) {
                    item->redrawBranches(current, minDistance, maxDistance, NULL);
                }
                items.append(item);
            } else {
                items.append(NULL);
            }
        }

        int size = items.size();
        assert(size > 0);

        GraphicsRectangularBranchItem *item = this;
        {
            int xmin = 0, ymin = items[0] ? items[0]->pos().y() : items[1]->pos().y(), ymax = 0;
            for (int i = 0; i < size; ++i) {
                if (items[i] == NULL) {
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
            if(!item->isCollapsed()) {
                y = (ymax + ymin) / 2;
                item->setPos(xmin, y);
            }
            else {
                y = (current++ + 0.5) * GraphicsRectangularBranchItem::DEFAULT_HEIGHT;
                item->setPos(0, y);
            }

            for (int i = 0; i < size; ++i) {
                if (items[i] == NULL) {
                    continue;
                }
                qreal dist = qAbs(node->getBranchesDistance(i));
                if (minDistance > -1) {
                    minDistance = qMin(minDistance, dist);
                } else {
                    minDistance = dist;
                }
                maxDistance = qMax(maxDistance, dist);
                items[i]->setDirection(items[i]->pos().y() > y ? GraphicsRectangularBranchItem::up : GraphicsRectangularBranchItem::down);
                items[i]->setWidthW(dist);
                items[i]->setDist(dist);
                items[i]->setHeightCoefW(1);
                items[i]->setParentItem(item);
                QRectF rect = items[i]->getDistanceText()->boundingRect();
                items[i]->getDistanceText()->setPos(-(items[i]->getWidth() + rect.width()) / 2, 0);
            }
        }
    }else{
        int y = (current++ + 0.5) * GraphicsRectangularBranchItem::DEFAULT_HEIGHT;
        setPos(0,y);
    }
}

GraphicsRectangularBranchItem* GraphicsRectangularBranchItem::getChildItemByPhyBranch(const PhyBranch* branch){

    foreach (QGraphicsItem* ci, this->childItems()) {
        GraphicsRectangularBranchItem* gbi = dynamic_cast<GraphicsRectangularBranchItem*>(ci);
        if (gbi != NULL) {
            if(gbi->getPhyBranch() == branch){
                return gbi;
            }
        }
    }
    return NULL;

}

}//namespace
