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

#ifndef _U2_GT_UTILS_PHY_TREE_H_
#define _U2_GT_UTILS_PHY_TREE_H_

#include <U2View/TreeViewer.h>

#include "GTGlobals.h"

namespace U2 {

class GraphicsButtonItem;

class GTUtilsPhyTree {
public:
    static QList<QGraphicsItem *> getSelectedNodes(HI::GUITestOpStatus& os);
    static QList<QGraphicsItem *> getUnselectedNodes(HI::GUITestOpStatus& os);
    static QList<QGraphicsItem *> getNodes(HI::GUITestOpStatus& os);

    static QList<QGraphicsSimpleTextItem*> getLabels(HI::GUITestOpStatus& os, QGraphicsView* treeView = NULL);
    static QList<QGraphicsSimpleTextItem*> getVisiableLabels(HI::GUITestOpStatus& os, QGraphicsView* treeView = NULL);
    static QList<QGraphicsSimpleTextItem*> getDistances(HI::GUITestOpStatus& os, QGraphicsView* treeView = NULL);
    static QList<QGraphicsSimpleTextItem*> getVisiableDistances(HI::GUITestOpStatus& os, QGraphicsView* treeView = NULL);
    static QStringList getLabelsText(HI::GUITestOpStatus& os);
    static QList<double> getDistancesValues(HI::GUITestOpStatus& os);
    static QPoint getGlobalCoord(HI::GUITestOpStatus& os, QGraphicsItem* item);

    static void clickNode(HI::GUITestOpStatus &os, GraphicsButtonItem *node);
    static qreal getNodeDistance(HI::GUITestOpStatus &os, GraphicsButtonItem *node);

    static TreeViewerUI * getTreeViewerUi(HI::GUITestOpStatus &os);

    /**
     * Branches are enumerated:
     * - right subtree is enumerated
     * - left subtree is enumerated
     * - root branch is enumerated
     *
     * An example:
     *        |-0--
     *    |-4--
     *    |   |   |-1--
     * -11-   |-3--
     *    |       |-2--
     *    |
     *    |       |-5--
     *    |   |-7--
     *    |   |   |-6--
     *    |-9--
     *    |   |-8--
     *    |
     *    |-10-
     *
     */
    static QList<GraphicsButtonItem *> getOrderedRectangularNodes(HI::GUITestOpStatus &os);
    static QList<GraphicsRectangularBranchItem *> getOrderedRectangularBranches(HI::GUITestOpStatus &os);
    static QList<qreal> getOrderedRectangularBranchesDistances(HI::GUITestOpStatus &os);

    static GraphicsButtonItem * getRootRectangularNode(HI::GUITestOpStatus &os);
    static GraphicsRectangularBranchItem * getRootRectangularBranch(HI::GUITestOpStatus &os);

private:
    static QList<GraphicsRectangularBranchItem *> getSubtreeOrderedRectangularBranches(HI::GUITestOpStatus &os, GraphicsRectangularBranchItem *rootBranch);
    static bool rectangularBranchLessThan(GraphicsRectangularBranchItem *first, GraphicsRectangularBranchItem *second);
    static QList<QGraphicsItem *> getNodes(HI::GUITestOpStatus& os, int width);
};

}   // namespace U2

#endif // _U2_GT_UTILS_PHY_TREE_H_
