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

#include <QStack>

#include "CreateUnrootedBranchesTask.h"
#include "GraphicsUnrootedBranchItem.h"
#include "GraphicsRectangularBranchItem.h"
#include <U2Core/PhyTreeObject.h>
#include "TreeViewerUtils.h"

namespace U2 {

CreateUnrootedBranchesTask::CreateUnrootedBranchesTask(GraphicsRectangularBranchItem *r): root1(r) {}

GraphicsUnrootedBranchItem* CreateUnrootedBranchesTask::getBranch(GraphicsRectangularBranchItem *from, GraphicsUnrootedBranchItem* parent) {
    GraphicsUnrootedBranchItem* res = new GraphicsUnrootedBranchItem(parent, coef * from->getHeight(), from, from->getNodeLabel());
    foreach (QGraphicsItem* item, from->childItems()) {
        GraphicsRectangularBranchItem* ri = dynamic_cast<GraphicsRectangularBranchItem*>(item);
        if (ri != NULL) {
            getBranch(ri, res);
        }
    }
    res->setCorrespondingItem(from);
    return res;
}

void CreateUnrootedBranchesTask::run() {
    coef = 360.0 / root1->childrenBoundingRect().height();
    root = getBranch(root1, NULL);
}

}
