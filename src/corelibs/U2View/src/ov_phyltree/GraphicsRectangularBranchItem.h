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

#ifndef _U2_GRAPHICS_RECTANGULAR_BRANCH_ITEM_H_
#define _U2_GRAPHICS_RECTANGULAR_BRANCH_ITEM_H_

#include <U2Core/Task.h>
#include "GraphicsBranchItem.h"

namespace U2 {

class PhyNode;
class PhyBranch;
class GraphicsButtonItem;

class GraphicsRectangularBranchItem: public QObject, public GraphicsBranchItem {
    Q_OBJECT
public:
    static const qreal DEFAULT_WIDTH;
    static const qreal MAXIMUM_WIDTH;
    static const qreal EPSILON;
    static const int DEFAULT_HEIGHT;


    GraphicsRectangularBranchItem(const QString& name, GraphicsRectangularBranchItem* pitem);
    GraphicsRectangularBranchItem();
    GraphicsRectangularBranchItem(qreal d, PhyBranch *branch, double nodeValue);
    GraphicsRectangularBranchItem(qreal x, qreal y, const QString& name, qreal d, PhyBranch *branch);
    GraphicsRectangularBranchItem(qreal x, qreal y, const QString& name);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setParentItem(QGraphicsItem *item);

    Direction getDirection() const { return direction; }
    qreal getHeight() const { return height; }
    void setHeightW(qreal h) { height = h; }
    void setHeight(qreal h);
    void setHeightCoef(int coef);
    void setHeightCoefW(int coef) {cur_height_coef = coef;}
    void setDirection(Direction d);

    void collapse();
    void setCollapsed(bool isCollapsed) {collapsed = isCollapsed;}
    void swapSiblings();
    void redrawBranches(int& current, qreal& minDistance, qreal& maxDistance, const PhyNode* root);

    const PhyBranch* getPhyBranch() const {return phyBranch;}
    GraphicsRectangularBranchItem* getChildItemByPhyBranch(const PhyBranch* branch);

    void drawCollapsedRegion();
    void branchCollapsed(GraphicsRectangularBranchItem* branch) {emit si_branchCollapsed(branch);}

signals:
    void si_branchCollapsed(GraphicsRectangularBranchItem* collapsedBranch);
private:
    qreal height;
    int cur_height_coef;
    Direction direction;
    PhyBranch* phyBranch;
};
}//namespace;

#endif
