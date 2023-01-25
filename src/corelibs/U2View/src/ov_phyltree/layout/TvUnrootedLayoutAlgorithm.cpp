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

#include "TvUnrootedLayoutAlgorithm.h"

#include <QStack>

#include <U2Core/PhyTreeObject.h>

#include "../item/TvRectangularBranchItem.h"
#include "../item/TvUnrootedBranchItem.h"

namespace U2 {

static TvUnrootedBranchItem* convertBranch(TvRectangularBranchItem* originalBranchItem,
                                           TvUnrootedBranchItem* convertedParentBranchItem,
                                           double coef) {
    double angle = coef * originalBranchItem->getHeight();
    auto convertedBranch = new TvUnrootedBranchItem(convertedParentBranchItem, angle, originalBranchItem, originalBranchItem->getNodeNameFromNodeItem());
    const QList<QGraphicsItem*>& originalChildBranchItems = originalBranchItem->childItems();
    for (QGraphicsItem* originalChildItem : qAsConst(originalChildBranchItems)) {
        if (auto originalChildBranchItem = dynamic_cast<TvRectangularBranchItem*>(originalChildItem)) {
            convertBranch(originalChildBranchItem, convertedBranch, coef);
        }
    }
    return convertedBranch;
}

TvBranchItem* TvUnrootedLayoutAlgorithm::convert(TvRectangularBranchItem* rectRoot) {
    double coef = 360.0 / rectRoot->childrenBoundingRect().height();
    return convertBranch(rectRoot, nullptr, coef);
}

}  // namespace U2
