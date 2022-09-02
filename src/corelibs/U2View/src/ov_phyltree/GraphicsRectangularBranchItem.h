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

#ifndef _U2_GRAPHICS_RECTANGULAR_BRANCH_ITEM_H_
#define _U2_GRAPHICS_RECTANGULAR_BRANCH_ITEM_H_

#include "GraphicsBranchItem.h"

namespace U2 {

class PhyNode;
class PhyBranch;
class GraphicsButtonItem;

class U2VIEW_EXPORT GraphicsRectangularBranchItem : public GraphicsBranchItem {
    Q_OBJECT
public:
    static constexpr double DEFAULT_WIDTH = 25.0;
    static constexpr double MAXIMUM_WIDTH = 500.0;
    static constexpr double EPSILON = 0.0000000001;
    static constexpr int DEFAULT_HEIGHT = 25;

    GraphicsRectangularBranchItem(const QString& name, GraphicsRectangularBranchItem* pitem);
    GraphicsRectangularBranchItem();
    GraphicsRectangularBranchItem(double distance, PhyBranch* branch, double nodeValue);
    GraphicsRectangularBranchItem(double x, double y, const QString& name, double distance, PhyBranch* branch);
    GraphicsRectangularBranchItem(double x, double y, const QString& name);

    QRectF boundingRect() const override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    void setParentItem(QGraphicsItem* item);

    Side getSide() const;

    double getHeight() const;

    void setHeightW(double h);

    void setHeight(double newHeight);

    void setHeightCoef(int newCoef);

    void setHeightCoefW(int coef);

    void setSide(const Side& side);

    void toggleCollapsedState() override;

    void swapSiblings();

    void recalculateBranches(int& current, const PhyNode* root);

    const PhyBranch* getPhyBranch() const;

    GraphicsRectangularBranchItem* getChildItemByPhyBranch(const PhyBranch* branch);

    void drawCollapsedRegion();

private:
    double height = 0;
    int currentHeightCoef = 1;
    PhyBranch* phyBranch = nullptr;
};
}  // namespace U2

#endif
