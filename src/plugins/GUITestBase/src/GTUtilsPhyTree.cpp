/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#include <drivers/GTMouseDriver.h>
#include <math.h>
#include <primitives/GTWidget.h>
#include <utils/GTThread.h>

#include <QGraphicsItem>

#include <U2Core/U2SafePoints.h>

#include <U2View/GraphicsButtonItem.h>
#include <U2View/GraphicsRectangularBranchItem.h>

#include "GTUtilsMdi.h"
#include "GTUtilsPhyTree.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsPhyTree"

#define GT_METHOD_NAME "getNodes"
QList<GraphicsButtonItem *> GTUtilsPhyTree::getNodes(HI::GUITestOpStatus &os) {
    QList<GraphicsButtonItem *> result;
    QGraphicsView *treeView = qobject_cast<QGraphicsView *>(GTWidget::findWidget(os, "treeView"));
    GT_CHECK_RESULT(treeView, "treeView not found", result);
    const QList<QGraphicsItem *> itemList = treeView->scene()->items();
    for (QGraphicsItem *item : qAsConst(itemList)) {
        auto nodeItem = dynamic_cast<GraphicsButtonItem *>(item);
        if (nodeItem != nullptr) {
            result.append(nodeItem);
        }
    }
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSelectedNodes"
QList<GraphicsButtonItem *> GTUtilsPhyTree::getSelectedNodes(HI::GUITestOpStatus &os) {
    QList<GraphicsButtonItem *> nodes = getNodes(os);
    QList<GraphicsButtonItem *> selectedNodes;
    for (auto node : qAsConst(nodes)) {
        if (node->isNodeSelected()) {
            selectedNodes << node;
        }
    }
    return selectedNodes;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getUnselectedNodes"
QList<GraphicsButtonItem *> GTUtilsPhyTree::getUnselectedNodes(HI::GUITestOpStatus &os) {
    QList<GraphicsButtonItem *> nodes = getNodes(os);
    QList<GraphicsButtonItem *> unselectedNodes;
    for (auto node : qAsConst(nodes)) {
        if (node->isNodeSelected()) {
            unselectedNodes << node;
        }
    }
    return unselectedNodes;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getLabels"
QList<QGraphicsSimpleTextItem *> GTUtilsPhyTree::getLabels(HI::GUITestOpStatus &os, QGraphicsView *treeView) {
    QList<QGraphicsSimpleTextItem *> result;
    if (treeView == nullptr) {
        treeView = qobject_cast<QGraphicsView *>(GTWidget::findWidget(os, "treeView"));
    }
    GT_CHECK_RESULT(treeView, "treeView not found", result);
    QList<QGraphicsItem *> list = treeView->scene()->items();

    foreach (QGraphicsItem *item, list) {
        QGraphicsSimpleTextItem *textItem = qgraphicsitem_cast<QGraphicsSimpleTextItem *>(item);
        if (textItem) {
            bool ok;
            QString s = textItem->text();
            s.toDouble(&ok);
            if (!ok) {
                result << textItem;
            }
        }
    }
    return result;
}
#undef GT_METHOD_NAME

QList<QGraphicsSimpleTextItem *> GTUtilsPhyTree::getVisibleLabels(HI::GUITestOpStatus &os, QGraphicsView *treeView) {
    QList<QGraphicsSimpleTextItem *> result;
    foreach (QGraphicsSimpleTextItem *item, getLabels(os, treeView)) {
        if (item->isVisible() && !item->text().isEmpty()) {
            result << item;
        }
    }
    return result;
}

#define GT_METHOD_NAME "getDistances"
QList<QGraphicsSimpleTextItem *> GTUtilsPhyTree::getDistances(HI::GUITestOpStatus &os, QGraphicsView *treeView) {
    QList<QGraphicsSimpleTextItem *> result;
    if (treeView == NULL) {
        treeView = qobject_cast<QGraphicsView *>(GTWidget::findWidget(os, "treeView"));
    }
    GT_CHECK_RESULT(treeView, "treeView not found", result);
    QList<QGraphicsItem *> list = treeView->scene()->items();

    foreach (QGraphicsItem *item, list) {
        QGraphicsSimpleTextItem *textItem = qgraphicsitem_cast<QGraphicsSimpleTextItem *>(item);
        if (textItem) {
            bool ok;
            textItem->text().toDouble(&ok);
            if (ok) {
                result << textItem;
            }
        }
    }
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getVisibleDistances"
QList<QGraphicsSimpleTextItem *> GTUtilsPhyTree::getVisibleDistances(HI::GUITestOpStatus &os, QGraphicsView *treeView) {
    QList<QGraphicsSimpleTextItem *> result;
    const QList<QGraphicsSimpleTextItem *> textItemList = getDistances(os, treeView);
    for (QGraphicsSimpleTextItem *item : qAsConst(textItemList)) {
        if (item->isVisible()) {
            result << item;
        }
    }
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getDistancesValues"
QList<double> GTUtilsPhyTree::getDistancesValues(HI::GUITestOpStatus &os) {
    QList<double> result;
    QList<QGraphicsSimpleTextItem *> distList = getDistances(os);

    foreach (QGraphicsSimpleTextItem *item, distList) {
        bool ok;
        QString s = item->text();
        double d = s.toDouble(&ok);
        if (ok) {
            result << d;
        }
    }

    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getLabelsText"
QStringList GTUtilsPhyTree::getLabelsText(HI::GUITestOpStatus &os) {
    QStringList result;
    QList<QGraphicsSimpleTextItem *> labelList = getLabels(os);

    foreach (QGraphicsSimpleTextItem *item, labelList) {
        result << item->text();
    }

    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getGlobalCenterCoord"
QPoint GTUtilsPhyTree::getGlobalCenterCoord(HI::GUITestOpStatus &os, QGraphicsItem *item) {
    QGraphicsView *treeView = qobject_cast<QGraphicsView *>(GTWidget::findWidget(os, "treeView"));
    GT_CHECK_RESULT(treeView, "treeView not found", QPoint());

    QPointF sceneCoord = item->mapToScene(item->boundingRect().topLeft());
    QPoint viewCord = treeView->mapFromScene(sceneCoord);
    QPoint globalCoord = treeView->mapToGlobal(viewCord);
    globalCoord += QPoint(item->boundingRect().width() / 2, item->boundingRect().height() / 2);

    return globalCoord;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickNode"
void GTUtilsPhyTree::clickNode(HI::GUITestOpStatus &os, GraphicsButtonItem *node) {
    GT_CHECK(node != nullptr, "Node to click is NULL");
    node->ensureVisible();
    GTThread::waitForMainThread();
    GTMouseDriver::moveTo(getGlobalCenterCoord(os, node));
    GTMouseDriver::click();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "doubleClickNode"
void GTUtilsPhyTree::doubleClickNode(HI::GUITestOpStatus &os, GraphicsButtonItem *node) {
    GT_CHECK(node != nullptr, "Node to doubleClickNode is NULL");
    node->ensureVisible();
    GTThread::waitForMainThread();
    GTMouseDriver::moveTo(getGlobalCenterCoord(os, node));
    GTMouseDriver::doubleClick();
    GTThread::waitForMainThread();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getNodeDistance"
qreal GTUtilsPhyTree::getNodeDistance(HI::GUITestOpStatus &os, GraphicsButtonItem *node) {
    GT_CHECK_RESULT(NULL != node, "Node is NULL", 0);
    GraphicsRectangularBranchItem *branch = dynamic_cast<GraphicsRectangularBranchItem *>(node->parentItem());
    GT_CHECK_RESULT(NULL != branch, "Node's branch' is NULL", 0);
    return branch->getDist();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getTreeViewerUi"
TreeViewerUI *GTUtilsPhyTree::getTreeViewerUi(HI::GUITestOpStatus &os) {
    return GTWidget::findExactWidget<TreeViewerUI *>(os, "treeView", GTUtilsMdi::activeWindow(os));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getOrderedRectangularNodes"
QList<GraphicsButtonItem *> GTUtilsPhyTree::getOrderedRectangularNodes(HI::GUITestOpStatus &os) {
    QList<GraphicsButtonItem *> orderedRectangularNodes;
    QList<GraphicsRectangularBranchItem *> graphicsRectangularBranchItems = getOrderedRectangularBranches(os);
    foreach (GraphicsRectangularBranchItem *rectangularBranch, graphicsRectangularBranchItems) {
        GT_CHECK_RESULT(NULL != rectangularBranch, "Rectangular branch is NULL", QList<GraphicsButtonItem *>());
        GraphicsButtonItem *rectangularNode = rectangularBranch->getButton();
        if (NULL != rectangularNode) {
            orderedRectangularNodes << rectangularNode;
        }
    }
    return orderedRectangularNodes;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getOrderedRectangularBranches"
QList<GraphicsRectangularBranchItem *> GTUtilsPhyTree::getOrderedRectangularBranches(HI::GUITestOpStatus &os) {
    return getSubtreeOrderedRectangularBranches(os, getRootRectangularBranch(os));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getRootRectangularNode"
QList<qreal> GTUtilsPhyTree::getOrderedRectangularBranchesDistances(HI::GUITestOpStatus &os) {
    QList<GraphicsRectangularBranchItem *> orderedBranches = getOrderedRectangularBranches(os);
    QList<qreal> orderedDistances;
    foreach (GraphicsRectangularBranchItem *branch, orderedBranches) {
        GT_CHECK_RESULT(NULL != branch, "Branch is NULL", QList<qreal>());
        orderedDistances << branch->getDist();
    }
    return orderedDistances;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getRootRectangularNode"
GraphicsButtonItem *GTUtilsPhyTree::getRootRectangularNode(HI::GUITestOpStatus &os) {
    GraphicsRectangularBranchItem *rootBranch = getRootRectangularBranch(os);
    GT_CHECK_RESULT(NULL != rootBranch, "Root branch is NULL", NULL);
    return rootBranch->getButton();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getRootRectangularBranch"
GraphicsRectangularBranchItem *GTUtilsPhyTree::getRootRectangularBranch(HI::GUITestOpStatus &os) {
    TreeViewerUI *treeViewerUi = getTreeViewerUi(os);
    GT_CHECK_RESULT(NULL != treeViewerUi, "TreeViewerUI is NULL", NULL);

    QList<QGraphicsItem *> items = treeViewerUi->scene()->items();
    foreach (QGraphicsItem *item, items) {
        GraphicsRectangularBranchItem *rectangularBranch = dynamic_cast<GraphicsRectangularBranchItem *>(item);
        if (NULL != rectangularBranch && NULL == rectangularBranch->getParentItem()) {
            return rectangularBranch;
        }
    }

    return NULL;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSubtreeOrderedRectangularBranches"
QList<GraphicsRectangularBranchItem *> GTUtilsPhyTree::getSubtreeOrderedRectangularBranches(HI::GUITestOpStatus &os, GraphicsRectangularBranchItem *rootBranch) {
    GT_CHECK_RESULT(NULL != rootBranch, "Subtree root branch is NULL", QList<GraphicsRectangularBranchItem *>());

    const QList<QGraphicsItem *> childItems = rootBranch->getChildItems();
    QList<GraphicsRectangularBranchItem *> childRectangularBranches;
    foreach (QGraphicsItem *childItem, childItems) {
        GraphicsRectangularBranchItem *childRectangularBranch = dynamic_cast<GraphicsRectangularBranchItem *>(childItem);
        if (NULL != childRectangularBranch && NULL != childRectangularBranch->getDistanceText()) {
            childRectangularBranches << childRectangularBranch;
        }
    }

    std::sort(childRectangularBranches.begin(), childRectangularBranches.end(), rectangularBranchLessThan);

    QList<GraphicsRectangularBranchItem *> subtreeOrderedRectangularBranches;
    foreach (GraphicsRectangularBranchItem *childRectangularBranch, childRectangularBranches) {
        subtreeOrderedRectangularBranches << getSubtreeOrderedRectangularBranches(os, childRectangularBranch);
    }
    subtreeOrderedRectangularBranches << rootBranch;

    return subtreeOrderedRectangularBranches;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "rectangularBranchLessThan"
bool GTUtilsPhyTree::rectangularBranchLessThan(GraphicsRectangularBranchItem *first, GraphicsRectangularBranchItem *second) {
    SAFE_POINT(NULL != first, "First rectangular branch item is NULL", true);
    SAFE_POINT(NULL != second, "Second rectangular branch item is NULL", false);

    if (first->getDirection() == second->getDirection()) {
        if (first->getDirection() == GraphicsBranchItem::up) {
            return first->getDist() < second->getDist();
        } else {
            return first->getDist() > second->getDist();
        }
    }

    return first->getDirection() > second->getDirection();
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}    // namespace U2
