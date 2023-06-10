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

#include <U2View/TreeViewer.h>
#include <U2View/TvNodeItem.h>

#include "GTGlobals.h"

namespace U2 {

class GTUtilsPhyTree {
public:
    /** Returns active Tree Viewer window or fails. */
    static QWidget* getActiveTreeViewerWindow();

    /** Checks that the active MDI window is a Tree Viewer window or fails. */
    static void checkTreeViewerWindowIsActive(const QString& titlePart = "");

    static QList<TvNodeItem*> getSelectedNodes();
    static QList<TvNodeItem*> getUnselectedNodes();
    static QList<TvNodeItem*> getNodes();

    /** Returns bounding rectangle in view-local on-screen coordinates. */
    static QRect getItemViewRect(QGraphicsItem* item);

    /** Finds 'button' node in the tree by 2 branch texts: left & right. */
    static TvNodeItem* getNodeByBranchText(const QString& leftBranchText, const QString& rightBranchText);

    static QList<QGraphicsSimpleTextItem*> getLabels(QGraphicsView* treeView = nullptr);
    static QList<QGraphicsSimpleTextItem*> getVisibleLabels(QGraphicsView* treeView = nullptr);
    static QList<QGraphicsSimpleTextItem*> getDistances(QGraphicsView* treeView = nullptr);
    static QList<QGraphicsSimpleTextItem*> getVisibleDistances(QGraphicsView* treeView = nullptr);
    static QStringList getLabelsText();
    static QList<double> getDistancesValues();
    static QPoint getGlobalCenterCoord(QGraphicsItem* item);

    static void clickNode(TvNodeItem* node, const Qt::MouseButton& mouseButton = Qt::LeftButton);
    static void doubleClickNode(TvNodeItem* node);
    static qreal getNodeDistance(TvNodeItem* node);

    static TreeViewerUI* getTreeViewerUi();

    /** Returns image of the current tree view. */
    static QImage captureTreeImage();

    /** Zooms in (positive steps) or zooms out (negative steps) using mouse wheel. */
    static void zoomWithMouseWheel(int steps);

    /** Zooms in (positive steps) or zooms out (negative steps) using mouse wheel. */
    static void zoomWithMouseWheel(QWidget* treeViewer, int steps);

    /** Clicks zoom-in button. */
    static void clickZoomInButton();

    /** Clicks zoom-out button. */
    static void clickZoomOutButton();

    /** Clicks fit-to-view zoom button. */
    static void clickZoomFitButton();

    /** Clicks zoom to 100% button. */
    static void clickZoom100Button();

    /** Returns current on-screen scene width. */
    static int getSceneWidth();

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
    static QList<TvNodeItem*> getOrderedRectangularNodes(int expectedNodeCount = -1);
    static QList<TvRectangularBranchItem*> getOrderedRectangularBranches();

    static TvNodeItem* getRootNode();
    static TvBranchItem* getRootBranch();
    static TvRectangularBranchItem* getRootRectangularBranch();

    /** Sets branch color. */
    static void setBranchColor(int r, int g, int b, bool checkNoActiveDialogWaiters = true);

    /** Returns percentage of the widget area filled with the given color. */
    static double getColorPercent(QWidget* widget, const QString& colorName);

private:
    static QList<TvRectangularBranchItem*> getSubtreeOrderedRectangularBranches(TvRectangularBranchItem* rootBranch);
    static bool rectangularBranchLessThan(TvRectangularBranchItem* first, TvRectangularBranchItem* second);
};

}  // namespace U2
