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

#include "RectangularTreeLayoutAlgorithm.h"

#include <QStack>

#include <U2Core/PhyTreeObject.h>
#include <U2Core/U2SafePoints.h>

#include "GraphicsRectangularBranchItem.h"

namespace U2 {

static GraphicsRectangularBranchItem* createBranch(const PhyNode* phyNode) {
    const QList<PhyBranch*> branches = phyNode->getBranches();
    int branchCount = branches.size();
    if (branchCount == 1 && (phyNode->name.isEmpty() || phyNode->name == "ROOT")) {
        SAFE_POINT(phyNode != phyNode->getSecondNodeOfBranch(0), "Invalid getSecondNodeOfBranch", nullptr);
        return createBranch(phyNode->getSecondNodeOfBranch(0));
    }
    if (branchCount == 0) {
        return new GraphicsRectangularBranchItem(0, 0, phyNode->name);
    }
    if (branchCount == 1) {
        PhyBranch* firstBranch = branches.at(0);
        return new GraphicsRectangularBranchItem(0, 0, phyNode->name, firstBranch->distance, firstBranch);
    }
    QList<GraphicsRectangularBranchItem*> childRectBranches;
    int branchIndex = -1;
    for (int i = 0; i < branchCount; i++) {
        if (phyNode->getSecondNodeOfBranch(i) == phyNode) {
            branchIndex = i;
            continue;
        }
        childRectBranches.append(createBranch(phyNode->getSecondNodeOfBranch(i)));
    }
    GraphicsRectangularBranchItem* rectBranch;
    if (branchIndex < 0) {
        rectBranch = new GraphicsRectangularBranchItem();
    } else {
        const PhyBranch* parentPhyBranch = phyNode->getParentBranch();
        SAFE_POINT(parentPhyBranch != nullptr, "An internal error: a tree is in an incorrect state, can't create a branch", nullptr);
        PhyBranch* phyBranch = branches.at(branchIndex);
        rectBranch = new GraphicsRectangularBranchItem(phyBranch->distance, phyBranch, parentPhyBranch->nodeValue);
    }
    for (auto childRectBranch : qAsConst(childRectBranches)) {
        childRectBranch->setParentItem(rectBranch);
    }
    return rectBranch;
}

GraphicsRectangularBranchItem* RectangularTreeLayoutAlgorithm::buildTreeLayout(const PhyNode* phyRoot) {
    GraphicsRectangularBranchItem* rectRoot = createBranch(phyRoot);
    recalculateTreeLayout(rectRoot, phyRoot);
    return rectRoot;
}

static GraphicsRectangularBranchItem* getChildItemByPhyBranch(GraphicsRectangularBranchItem* branchItem, const PhyBranch* branch) {
    QList<QGraphicsItem*> childItems = branchItem->childItems();
    for (QGraphicsItem* ci : qAsConst(childItems)) {
        if (auto gbi = dynamic_cast<GraphicsRectangularBranchItem*>(ci)) {
            if (gbi->getPhyBranch() == branch) {
                return gbi;
            }
        }
    }
    return nullptr;
}

void static recalculateBranches(GraphicsRectangularBranchItem* branch, const PhyNode* rootPhyNode, int& currentRow) {
    const PhyNode* phyNode = branch->getPhyBranch() != nullptr ? branch->getPhyBranch()->node2 : rootPhyNode;
    CHECK(phyNode != nullptr, );

    const QList<PhyBranch*> branches = phyNode->getBranches();
    if (branches.size() <= 1) {
        double y = (currentRow + 0.5) * GraphicsRectangularBranchItem::DEFAULT_HEIGHT;
        branch->setPos(0, y);
        currentRow++;
        return;
    }
    QList<GraphicsRectangularBranchItem*> childBranches;
    for (int i = 0; i < branches.size(); ++i) {
        if (phyNode->getSecondNodeOfBranch(i) != phyNode) {
            GraphicsRectangularBranchItem* childBranch = getChildItemByPhyBranch(branch, branches.at(i));
            if (childBranch->isVisible()) {
                recalculateBranches(childBranch, nullptr, currentRow);
            }
            childBranches.append(childBranch);
        } else {
            childBranches.append(nullptr);
        }
    }

    SAFE_POINT(childBranches.size() == branches.size(), "Invalid count of child branches", );

    QPointF firstPos = childBranches[0] ? childBranches[0]->pos() : childBranches[1]->pos();
    double xMin = firstPos.x();
    double yMin = firstPos.y();
    double yMax = firstPos.y();
    for (int i = 1; i < childBranches.size(); ++i) {
        if (childBranches[i] == nullptr) {
            continue;
        }
        QPointF pos1 = childBranches[i]->pos();
        xMin = qMin(xMin, pos1.x());
        yMin = qMin(yMin, pos1.y());
        yMax = qMax(yMax, pos1.y());
    }
    xMin -= GraphicsRectangularBranchItem::DEFAULT_WIDTH;

    double y;
    if (branch->isCollapsed()) {
        y = (currentRow + 0.5) * GraphicsRectangularBranchItem::DEFAULT_HEIGHT;
        branch->setPos(0, y);
        currentRow++;
    } else {
        y = (yMax + yMin) / 2;
        branch->setPos(xMin, y);
    }

    for (int i = 0; i < childBranches.size(); ++i) {
        GraphicsRectangularBranchItem* childBranch = childBranches[i];
        if (childBranch == nullptr) {
            continue;
        }
        double dist = qAbs(branches.at(i)->distance);
        auto side = childBranch->pos().y() > y
                        ? GraphicsBranchItem::Side::Right
                        : GraphicsBranchItem::Side::Left;
        childBranch->setSide(side);
        childBranch->setWidthW(dist);
        childBranch->setDist(dist);
        childBranch->setParentItem(branch);

        QRectF rect = childBranch->getDistanceTextItem()->boundingRect();
        double textX = -(childBranch->getWidth() + rect.width()) / 2;
        childBranch->getDistanceTextItem()->setPos(textX, 0);
    }
}

void RectangularTreeLayoutAlgorithm::recalculateTreeLayout(GraphicsRectangularBranchItem* rootBranchItem, const PhyNode* rootPhyNode) {
    int currentRow = 0;
    recalculateBranches(rootBranchItem, rootPhyNode, currentRow);
}

}  // namespace U2
