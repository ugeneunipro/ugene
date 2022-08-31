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

static GraphicsRectangularBranchItem* createBranch(const PhyNode* node, int& current) {
    int branches = node->branchCount();
    if (branches == 1 && (node->getName() == "" || node->getName() == "ROOT")) {
        assert(node != node->getSecondNodeOfBranch(0));
        return createBranch(node->getSecondNodeOfBranch(0), current);
    }
    if (branches > 1) {
        QList<GraphicsRectangularBranchItem*> items;
        int ind = -1;
        for (int i = 0; i < branches; i++) {
            if (node->getSecondNodeOfBranch(i) != node) {
                GraphicsRectangularBranchItem* item = createBranch(node->getSecondNodeOfBranch(i), current);
                items.append(item);
            } else {
                items.append(nullptr);
                ind = i;
            }
        }

        GraphicsRectangularBranchItem* item = nullptr;
        if (ind < 0) {
            item = new GraphicsRectangularBranchItem();
        } else {
            const PhyBranch* parentBranch = node->getParentBranch();
            if (parentBranch != nullptr) {
                item = new GraphicsRectangularBranchItem(node->getBranchesDistance(ind), node->getBranch(ind), parentBranch->nodeValue);
            }
        }
        SAFE_POINT(item != nullptr, "An internal error: a tree is in an incorrect state, can't create a branch", nullptr);
        int itemSize = items.size();
        assert(itemSize > 0);

        {
            double xmin = 0;
            double ymin = items[0] ? items[0]->pos().y() : items[1]->pos().y();
            double ymax = 0;
            for (int i = 0; i < itemSize; i++) {
                if (items[i] == nullptr) {
                    continue;
                }
                QPointF pos1 = items[i]->pos();
                xmin = qMin(xmin, pos1.x());
                ymin = qMin(ymin, pos1.y());
                ymax = qMax(ymax, pos1.y());
            }
            xmin -= GraphicsRectangularBranchItem::DEFAULT_WIDTH;

            int y = (ymax + ymin) / 2;
            item->setPos(xmin, y);

            for (int i = 0; i < itemSize; i++) {
                if (items[i] == nullptr) {
                    continue;
                }
                double dist = qAbs(node->getBranchesDistance(i));
                items[i]->setSide(items[i]->pos().y() > y ? GraphicsRectangularBranchItem::Right : GraphicsRectangularBranchItem::Left);
                items[i]->setWidthW(dist);
                items[i]->setDist(dist);
                items[i]->setParentItem(item);
                QRectF rect = items[i]->getDistanceTextItem()->boundingRect();
                items[i]->getDistanceTextItem()->setPos(-(items[i]->getWidth() + rect.width()) / 2, 0);
            }
        }
        return item;
    } else {
        int y = (current++ + 0.5) * GraphicsRectangularBranchItem::DEFAULT_HEIGHT;
        auto item = branches != 1
                        ? new GraphicsRectangularBranchItem(0, y, node->getName())
                        : new GraphicsRectangularBranchItem(0, y, node->getName(), node->getBranchesDistance(0), node->getBranch(0));

        return item;
    }
}

GraphicsRectangularBranchItem* RectangularTreeLayoutAlgorithm::buildTreeLayout(const PhyNode* node) {
    int current = 0;
    return createBranch(node, current);
}

}  // namespace U2
