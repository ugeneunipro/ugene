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

#include "UnrootedTreeLayoutAlgorithm.h"

#include <QStack>

#include <U2Core/PhyTreeObject.h>

#include "GraphicsRectangularBranchItem.h"
#include "GraphicsUnrootedBranchItem.h"

namespace U2 {

static GraphicsUnrootedBranchItem* convertBranch(GraphicsRectangularBranchItem* originalBranchItem,
                                                 GraphicsUnrootedBranchItem* convertedParentBranchItem,
                                                 double coef) {
    double angle = coef * originalBranchItem->getHeight();
    auto convertedBranch = new GraphicsUnrootedBranchItem(convertedParentBranchItem, angle, originalBranchItem, originalBranchItem->getNodeLabelValue());
    const QList<QGraphicsItem*>& originalChildBranchItems = originalBranchItem->childItems();
    for (QGraphicsItem* originalChildItem : qAsConst(originalChildBranchItems)) {
        if (auto originalChildBranchItem = dynamic_cast<GraphicsRectangularBranchItem*>(originalChildItem)) {
            convertBranch(originalChildBranchItem, convertedBranch, coef);
        }
    }
    return convertedBranch;
}

GraphicsBranchItem* UnrootedTreeLayoutAlgorithm::convert(GraphicsRectangularBranchItem* rectRoot) {
    double coef = 360.0 / rectRoot->childrenBoundingRect().height();
    return convertBranch(rectRoot, nullptr, coef);
}

}  // namespace U2
