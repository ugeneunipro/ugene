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

#include "TvRectangularLayoutAlgorithm.h"

#include <QStack>

#include <U2Core/PhyTreeObject.h>
#include <U2Core/U2SafePoints.h>

#include "../item/TvRectangularBranchItem.h"
#include "../item/TvTextItem.h"

namespace U2 {

static TvRectangularBranchItem* createBranch(const PhyNode* phyNode) {
    const PhyBranch* parentPhyBranch = phyNode->getParentBranch();
    auto parentTvBranch = new TvRectangularBranchItem(parentPhyBranch, "", phyNode->isRootNode());
    if (phyNode->isLeafNode()) {
        // TODO: do not use branches to draw names.
        auto branchTvItemForName = new TvRectangularBranchItem(nullptr, phyNode->name, false);
        branchTvItemForName->setParentItem(parentTvBranch);
    }

    const QList<PhyBranch*>& childPhyBranches = phyNode->getChildBranches();
    for (auto childPhyBranch : qAsConst(childPhyBranches)) {
        auto childTvBranch = createBranch(childPhyBranch->childNode);
        childTvBranch->setParentItem(parentTvBranch);
    }
    return parentTvBranch;
}

TvRectangularBranchItem* TvRectangularLayoutAlgorithm::buildTreeLayout(const PhyNode* phyRoot) {
    TvRectangularBranchItem* rectRoot = createBranch(phyRoot);
    recalculateTreeLayout(rectRoot, phyRoot);
    return rectRoot;
}

static TvRectangularBranchItem* getChildItemByPhyBranch(TvRectangularBranchItem* branchItem, const PhyBranch* branch) {
    QList<QGraphicsItem*> childItems = branchItem->childItems();
    for (QGraphicsItem* ci : qAsConst(childItems)) {
        if (auto gbi = dynamic_cast<TvRectangularBranchItem*>(ci)) {
            if (gbi->getPhyBranch() == branch) {
                return gbi;
            }
        }
    }
    return nullptr;
}

void static recalculateBranches(TvRectangularBranchItem* branch, const PhyNode* rootPhyNode, int& currentRow) {
    const PhyNode* phyNode = branch->getPhyBranch() != nullptr ? branch->getPhyBranch()->childNode : rootPhyNode;
    CHECK(phyNode != nullptr, );

    const QList<PhyBranch*>& childPhyBranches = phyNode->getChildBranches();
    if (childPhyBranches.isEmpty()) {
        double y = (currentRow + 0.5) * TvRectangularBranchItem::DEFAULT_HEIGHT;
        branch->setPos(0, y);
        currentRow++;
        return;
    }
    QList<TvRectangularBranchItem*> childTvBranches;
    for (const PhyBranch* childPhyBranch : qAsConst(childPhyBranches)) {
        TvRectangularBranchItem* childTvBranch = getChildItemByPhyBranch(branch, childPhyBranch);
        if (childTvBranch->isVisible()) {
            recalculateBranches(childTvBranch, nullptr, currentRow);
        }
        childTvBranches.append(childTvBranch);
    }

    QPointF firstPos = childTvBranches.first()->pos();
    double xMin = firstPos.x();
    double yMin = firstPos.y();
    double yMax = firstPos.y();
    for (int i = 1; i < childTvBranches.size(); ++i) {
        QPointF pos1 = childTvBranches[i]->pos();
        xMin = qMin(xMin, pos1.x());
        yMin = qMin(yMin, pos1.y());
        yMax = qMax(yMax, pos1.y());
    }
    xMin -= TvRectangularBranchItem::DEFAULT_WIDTH;

    double y;
    if (branch->isCollapsed()) {
        y = (currentRow + 0.5) * TvRectangularBranchItem::DEFAULT_HEIGHT;
        branch->setPos(0, y);
        currentRow++;
    } else {
        y = (yMax + yMin) / 2;
        branch->setPos(xMin, y);
    }

    for (auto childTvBranch : qAsConst(childTvBranches)) {
        double dist = qAbs(childTvBranch->getDist());
        auto side = childTvBranch->pos().y() > y
                        ? TvBranchItem::Side::Right
                        : TvBranchItem::Side::Left;
        childTvBranch->setSide(side);
        childTvBranch->setWidthW(dist);
        childTvBranch->setDist(dist);
        childTvBranch->setParentItem(branch);

        TvTextItem* distanceTextItem = childTvBranch->getDistanceTextItem();
        QRectF rect = distanceTextItem->boundingRect();
        double textX = -(childTvBranch->getWidth() + rect.width()) / 2;
        distanceTextItem->setPos(textX, 0);
    }
}

void TvRectangularLayoutAlgorithm::recalculateTreeLayout(TvRectangularBranchItem* rootBranchItem, const PhyNode* rootPhyNode) {
    int currentRow = 0;
    recalculateBranches(rootBranchItem, rootPhyNode, currentRow);
}

}  // namespace U2
