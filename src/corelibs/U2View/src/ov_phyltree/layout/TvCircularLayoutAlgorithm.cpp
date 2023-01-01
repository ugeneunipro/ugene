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

#include "TvCircularLayoutAlgorithm.h"

#include <U2Core/PhyTreeObject.h>

#include "../item/TvCircularBranchItem.h"
#include "../item/TvRectangularBranchItem.h"

namespace U2 {

static constexpr double DEGENERATED_WIDTH = 300;
static constexpr double WIDTH_RADIUS = 30;
static constexpr double SCALE = 6.0;

static TvCircularBranchItem* convertBranch(TvRectangularBranchItem* originalBranchItem,
                                           TvCircularBranchItem* convertedParentBranchItem,
                                           double coef) {
    double height = coef * originalBranchItem->getHeight();
    auto convertedBranch = new TvCircularBranchItem(convertedParentBranchItem, height, originalBranchItem, originalBranchItem->getNodeNameFromNodeItem());
    const QList<QGraphicsItem*>& originalChildItems = originalBranchItem->childItems();
    for (QGraphicsItem* originalChildItem : qAsConst(originalChildItems)) {
        if (auto ri = dynamic_cast<TvRectangularBranchItem*>(originalChildItem)) {
            convertBranch(ri, convertedBranch, coef);
        }
    }
    return convertedBranch;
}

TvBranchItem* TvCircularLayoutAlgorithm::convert(TvRectangularBranchItem* rectRoot, bool degeneratedCase) {
    double coef = SCALE / rectRoot->childrenBoundingRect().height();
    double originalWidth = rectRoot->getWidth();
    rectRoot->setWidthW(degeneratedCase ? DEGENERATED_WIDTH : WIDTH_RADIUS);
    TvCircularBranchItem* circularLayoutRoot = convertBranch(rectRoot, nullptr, coef);
    rectRoot->setWidthW(originalWidth);
    return circularLayoutRoot;
}

}  // namespace U2
