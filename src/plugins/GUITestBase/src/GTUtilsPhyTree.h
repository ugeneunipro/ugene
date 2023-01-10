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

#ifndef _U2_GT_UTILS_PHY_TREE_H_
#define _U2_GT_UTILS_PHY_TREE_H_

#include <U2View/TreeViewer.h>
#include <U2View/TvNodeItem.h>

#include "GTGlobals.h"

namespace U2 {

class GTUtilsPhyTree {
public:
    /** Returns active Tree Viewer window or fails. */
    static QWidget* getActiveTreeViewerWindow(HI::GUITestOpStatus& os);

    /** Checks that the active MDI window is a Tree Viewer window or fails. */
    static void checkTreeViewerWindowIsActive(HI::GUITestOpStatus& os, const QString& titlePart = "");

    static QList<TvNodeItem*> getSelectedNodes(HI::GUITestOpStatus& os);
    static QList<TvNodeItem*> getUnselectedNodes(HI::GUITestOpStatus& os);
    static QList<TvNodeItem*> getNodes(HI::GUITestOpStatus& os);

    /** Returns bounding rectangle in view-local on-screen coordinates. */
    static QRect getItemViewRect(HI::GUITestOpStatus& os, QGraphicsItem* item);

    /** Finds 'button' node in the tree by 2 branch texts: left & right. */
    static TvNodeItem* getNodeByBranchText(HI::GUITestOpStatus& os, const QString& leftBranchText, const QString& rightBranchText);

    static QList<QGraphicsSimpleTextItem*> getLabels(HI::GUITestOpStatus& os, QGraphicsView* treeView = nullptr);
    static QList<QGraphicsSimpleTextItem*> getVisibleLabels(HI::GUITestOpStatus& os, QGraphicsView* treeView = nullptr);
    static QList<QGraphicsSimpleTextItem*> getDistances(HI::GUITestOpStatus& os, QGraphicsView* treeView = nullptr);
    static QList<QGraphicsSimpleTextItem*> getVisibleDistances(HI::GUITestOpStatus& os, QGraphicsView* treeView = nullptr);
    static QStringList getLabelsText(HI::GUITestOpStatus& os);
    static QList<double> getDistancesValues(HI::GUITestOpStatus& os);
    static QPoint getGlobalCenterCoord(HI::GUITestOpStatus& os, QGraphicsItem* item);

    static void clickNode(HI::GUITestOpStatus& os, TvNodeItem* node, const Qt::MouseButton& mouseButton = Qt::LeftButton);
    static void doubleClickNode(HI::GUITestOpStatus& os, TvNodeItem* node);
    static qreal getNodeDistance(HI::GUITestOpStatus& os, TvNodeItem* node);

    static TreeViewerUI* getTreeViewerUi(HI::GUITestOpStatus& os);

    /** Returns image of the current tree view. */
    static QImage captureTreeImage(HI::GUITestOpStatus& os);

    /** Zooms in (positive steps) or zooms out (negative steps) using mouse wheel. */
    static void zoomWithMouseWheel(HI::GUITestOpStatus& os, int steps);

    /** Clicks zoom-in button once. */
    static void clickZoomInButton(HI::GUITestOpStatus& os);

    /** Clicks zoom-out button once. */
    static void clickZoomOutButton(HI::GUITestOpStatus& os);

    /** Clicks reset zoom button once. */
    static void clickResetZoomButton(HI::GUITestOpStatus& os);

    /** Returns current on-screen scene width. */
    static int getSceneWidth(HI::GUITestOpStatus& os);

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
    static QList<TvNodeItem*> getOrderedRectangularNodes(HI::GUITestOpStatus& os, int expectedNodeCount = -1);
    static QList<TvRectangularBranchItem*> getOrderedRectangularBranches(HI::GUITestOpStatus& os);

    static TvNodeItem* getRootNode(HI::GUITestOpStatus& os);
    static TvBranchItem* getRootBranch(HI::GUITestOpStatus& os);
    static TvRectangularBranchItem* getRootRectangularBranch(HI::GUITestOpStatus& os);

private:
    static QList<TvRectangularBranchItem*> getSubtreeOrderedRectangularBranches(HI::GUITestOpStatus& os, TvRectangularBranchItem* rootBranch);
    static bool rectangularBranchLessThan(TvRectangularBranchItem* first, TvRectangularBranchItem* second);
};

}  // namespace U2

#endif  // _U2_GT_UTILS_PHY_TREE_H_
