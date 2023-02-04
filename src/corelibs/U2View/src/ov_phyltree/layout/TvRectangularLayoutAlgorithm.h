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

#pragma once

namespace U2 {

class PhyNode;
class TvRectangularBranchItem;

class TvRectangularLayoutAlgorithm {
public:
    TvRectangularLayoutAlgorithm() = delete;

    /** Builds new TvTree* hierarchy with no layout coordinates. */
    static TvRectangularBranchItem* buildTvTreeHierarchy(const PhyNode* phyRoot);

    /** Recalculates tree layout in-place. Checks isCollapsed() state on branches. */
    static void recalculateTreeLayout(TvRectangularBranchItem* rootBranchItem, const PhyNode* rootPhyNode);
};

}  // namespace U2
