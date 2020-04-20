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

#include "CreateCircularBranchesTask.h"
#include "GraphicsCircularBranchItem.h"
#include "GraphicsRectangularBranchItem.h"
#include <U2Core/PhyTreeObject.h>
#include "TreeViewerUtils.h"

namespace U2 {

const qreal CreateCircularBranchesTask::DEGENERATED_WIDTH = 300;
const qreal CreateCircularBranchesTask::WIDTH_RADIUS = 30;
const qreal CreateCircularBranchesTask::SCALE = 6.0;

CreateCircularBranchesTask::CreateCircularBranchesTask(GraphicsRectangularBranchItem *r, bool _degeneratedCase): root1(r), degeneratedCase(_degeneratedCase) {}

GraphicsCircularBranchItem* CreateCircularBranchesTask::getBranch(GraphicsRectangularBranchItem *from, GraphicsCircularBranchItem* parent) {
    GraphicsCircularBranchItem* res = new GraphicsCircularBranchItem(parent, coef * from->getHeight(), from, from->getNodeLabel());
    foreach (QGraphicsItem* item, from->childItems()) {
        GraphicsRectangularBranchItem* ri = dynamic_cast<GraphicsRectangularBranchItem*>(item);
        if (ri != NULL) {
            getBranch(ri, res);
        }
    }
    res->setCorrespondingItem(from);
    return res;
}

void CreateCircularBranchesTask::run() {
    coef = SCALE / root1->childrenBoundingRect().height();
    if (degeneratedCase){
        root1->setWidthW(DEGENERATED_WIDTH);
    }else{
        root1->setWidthW(WIDTH_RADIUS);
    }

    GraphicsCircularBranchItem* r = getBranch(root1, NULL);
    r->setVisibleW(false);
    root = r;
    root1->setWidthW(0);
}

}
