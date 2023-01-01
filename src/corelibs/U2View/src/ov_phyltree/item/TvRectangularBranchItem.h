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

#ifndef _U2_TV_RECTANGULAR_BRANCH_ITEM_H_
#define _U2_TV_RECTANGULAR_BRANCH_ITEM_H_

#include "TvBranchItem.h"

namespace U2 {

class PhyNode;
class PhyBranch;
class TvNodeItem;

class U2VIEW_EXPORT TvRectangularBranchItem : public TvBranchItem {
    Q_OBJECT
public:
    static constexpr double DEFAULT_WIDTH = 25.0;
    static constexpr double MAXIMUM_WIDTH = 500.0;
    static constexpr double EPSILON = 0.0000000001;
    static constexpr int DEFAULT_HEIGHT = 25;

    TvRectangularBranchItem(const PhyBranch* branch, const QString& name, bool isRoot);

    QRectF boundingRect() const override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    void setParentItem(QGraphicsItem* item);

    Side getSide() const;

    double getHeight() const;

    void setHeight(double newHeight);

    void setBreathScaleAdjustment(double newBreadthScaleAdjustment);

    void setSide(const Side& side);

    void toggleCollapsedState() override;

    const PhyBranch* getPhyBranch() const;

    void drawCollapsedRegion();

    void setCurvature(double newCurvature);

private:
    /** Height (breadth) of the branch in pixels. */
    double height = 0;

    double curvature = 0;

    /** See BREADTH_SCALE_ADJUSTMENT doc. */
    double breadthScaleAdjustment = 1;

    /**
     * Leaf branches have additional UI element to show selected state.
     * TODO: this must be a fixed size (non-scaling) component same as node or branch text.
     */
    QGraphicsEllipseItem* leafBranchSelectionMarker = nullptr;
};

}  // namespace U2

#endif
