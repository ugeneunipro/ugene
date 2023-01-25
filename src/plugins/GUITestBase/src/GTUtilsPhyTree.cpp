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

#include <drivers/GTMouseDriver.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTWidget.h>
#include <utils/GTThread.h>

#include <QGraphicsItem>

#include <U2Core/U2SafePoints.h>

#include <U2View/TreeViewerFactory.h>
#include <U2View/TvNodeItem.h>
#include <U2View/TvRectangularBranchItem.h>

#include "GTUtilsMdi.h"
#include "GTUtilsPhyTree.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsPhyTree"

#define GT_METHOD_NAME "getNodeByBranchText"
TvNodeItem* GTUtilsPhyTree::getNodeByBranchText(HI::GUITestOpStatus& os, const QString& leftBranchText, const QString& rightBranchText) {
    auto treeView = GTWidget::findGraphicsView(os, "treeView");
    QList<QGraphicsItem*> itemList = treeView->scene()->items();
    for (QGraphicsItem* item : qAsConst(itemList)) {
        if (auto node = dynamic_cast<TvNodeItem*>(item)) {
            TvBranchItem* left = node->getLeftBranchItem();
            TvBranchItem* right = node->getRightBranchItem();
            if (left != nullptr && right != nullptr && left->getDistanceText() == leftBranchText && right->getDistanceText() == rightBranchText) {
                return node;
            }
        }
    }
    GT_FAIL("Failed to find name by child branches: " + leftBranchText + ", " + rightBranchText, nullptr);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getNodes"
QList<TvNodeItem*> GTUtilsPhyTree::getNodes(HI::GUITestOpStatus& os) {
    auto treeView = GTWidget::findGraphicsView(os, "treeView");
    QList<QGraphicsItem*> itemList = treeView->scene()->items();
    QList<TvNodeItem*> result;
    for (QGraphicsItem* item : qAsConst(itemList)) {
        if (auto nodeItem = dynamic_cast<TvNodeItem*>(item)) {
            result.append(nodeItem);
        }
    }
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSelectedNodes"
QList<TvNodeItem*> GTUtilsPhyTree::getSelectedNodes(HI::GUITestOpStatus& os) {
    QList<TvNodeItem*> nodes = getNodes(os);
    QList<TvNodeItem*> selectedNodes;
    for (auto node : qAsConst(nodes)) {
        if (node->isSelected()) {
            selectedNodes << node;
        }
    }
    return selectedNodes;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getUnselectedNodes"
QList<TvNodeItem*> GTUtilsPhyTree::getUnselectedNodes(HI::GUITestOpStatus& os) {
    QList<TvNodeItem*> nodes = getNodes(os);
    QList<TvNodeItem*> unselectedNodes;
    for (auto node : qAsConst(nodes)) {
        if (node->isSelected()) {
            unselectedNodes << node;
        }
    }
    return unselectedNodes;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getLabels"
QList<QGraphicsSimpleTextItem*> GTUtilsPhyTree::getLabels(HI::GUITestOpStatus& os, QGraphicsView* treeView) {
    QList<QGraphicsSimpleTextItem*> result;
    if (treeView == nullptr) {
        treeView = GTWidget::findGraphicsView(os, "treeView");
    }
    GT_CHECK_RESULT(treeView, "treeView not found", result);
    QList<QGraphicsItem*> list = treeView->scene()->items();

    foreach (QGraphicsItem* item, list) {
        if (auto textItem = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item)) {
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

QList<QGraphicsSimpleTextItem*> GTUtilsPhyTree::getVisibleLabels(HI::GUITestOpStatus& os, QGraphicsView* treeView) {
    QList<QGraphicsSimpleTextItem*> result;
    foreach (QGraphicsSimpleTextItem* item, getLabels(os, treeView)) {
        if (item->isVisible() && !item->text().isEmpty()) {
            result << item;
        }
    }
    return result;
}

#define GT_METHOD_NAME "getDistances"
QList<QGraphicsSimpleTextItem*> GTUtilsPhyTree::getDistances(HI::GUITestOpStatus& os, QGraphicsView* treeView) {
    QList<QGraphicsSimpleTextItem*> result;
    if (treeView == nullptr) {
        treeView = GTWidget::findGraphicsView(os, "treeView");
    }
    GT_CHECK_RESULT(treeView, "treeView not found", result);
    QList<QGraphicsItem*> list = treeView->scene()->items();

    for (QGraphicsItem* item : qAsConst(list)) {
        if (auto textItem = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item)) {
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
QList<QGraphicsSimpleTextItem*> GTUtilsPhyTree::getVisibleDistances(HI::GUITestOpStatus& os, QGraphicsView* treeView) {
    QList<QGraphicsSimpleTextItem*> result;
    const QList<QGraphicsSimpleTextItem*> textItemList = getDistances(os, treeView);
    for (QGraphicsSimpleTextItem* item : qAsConst(textItemList)) {
        if (item->isVisible()) {
            result << item;
        }
    }
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getDistancesValues"
QList<double> GTUtilsPhyTree::getDistancesValues(HI::GUITestOpStatus& os) {
    QList<double> result;
    QList<QGraphicsSimpleTextItem*> distList = getDistances(os);

    for (QGraphicsSimpleTextItem* item : qAsConst(distList)) {
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
QStringList GTUtilsPhyTree::getLabelsText(HI::GUITestOpStatus& os) {
    QStringList result;
    QList<QGraphicsSimpleTextItem*> labelList = getLabels(os);
    for (QGraphicsSimpleTextItem* item : qAsConst(labelList)) {
        result << item->text();
    }

    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getGlobalCenterCoord"
QPoint GTUtilsPhyTree::getGlobalCenterCoord(HI::GUITestOpStatus& os, QGraphicsItem* item) {
    auto treeView = GTWidget::findGraphicsView(os, "treeView");
    QRectF viewRect = getItemViewRect(os, item);
    return treeView->mapToGlobal(viewRect.center().toPoint());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getItemViewRect"
QRect GTUtilsPhyTree::getItemViewRect(HI::GUITestOpStatus& os, QGraphicsItem* item) {
    auto treeView = GTWidget::findGraphicsView(os, "treeView");
    QRectF sceneRect = item->mapToScene(item->boundingRect()).boundingRect();
    return treeView->mapFromScene(sceneRect).boundingRect();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickNode"
void GTUtilsPhyTree::clickNode(HI::GUITestOpStatus& os, TvNodeItem* node, const Qt::MouseButton& mouseButton) {
    GT_CHECK(node != nullptr, "Node to click is NULL");
    node->ensureVisible();  // TODO: do not run from the non-UI thread.
    GTThread::waitForMainThread();
    GTMouseDriver::moveTo(getGlobalCenterCoord(os, node));
    GTMouseDriver::click(mouseButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "doubleClickNode"
void GTUtilsPhyTree::doubleClickNode(HI::GUITestOpStatus& os, TvNodeItem* node) {
    GT_CHECK(node != nullptr, "Node to doubleClickNode is NULL");
    node->ensureVisible();
    GTThread::waitForMainThread();
    GTMouseDriver::moveTo(getGlobalCenterCoord(os, node));
    GTMouseDriver::doubleClick();
    GTThread::waitForMainThread();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getNodeDistance"
qreal GTUtilsPhyTree::getNodeDistance(HI::GUITestOpStatus& os, TvNodeItem* node) {
    GT_CHECK_RESULT(node != nullptr, "Node is NULL", 0);
    TvRectangularBranchItem* branch = dynamic_cast<TvRectangularBranchItem*>(node->parentItem());
    GT_CHECK_RESULT(branch != nullptr, "Node's branch' is NULL", 0);
    return branch->getDist();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getTreeViewerUi"
TreeViewerUI* GTUtilsPhyTree::getTreeViewerUi(HI::GUITestOpStatus& os) {
    return GTWidget::findExactWidget<TreeViewerUI*>(os, "treeView", GTUtilsMdi::activeWindow(os));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getOrderedRectangularNodes"
QList<TvNodeItem*> GTUtilsPhyTree::getOrderedRectangularNodes(HI::GUITestOpStatus& os, int expectedNodeCount) {
    QList<TvNodeItem*> orderedRectangularNodes;
    QList<TvRectangularBranchItem*> graphicsRectangularBranchItems = getOrderedRectangularBranches(os);
    for (TvRectangularBranchItem* rectangularBranch : qAsConst(graphicsRectangularBranchItems)) {
        GT_CHECK_RESULT(rectangularBranch != nullptr, "Rectangular branch is NULL", QList<TvNodeItem*>());
        TvNodeItem* rectangularNode = rectangularBranch->getNodeItem();
        if (rectangularNode != nullptr) {
            orderedRectangularNodes << rectangularNode;
        }
    }
    if (expectedNodeCount >= 0) {
        int nodeCount = orderedRectangularNodes.size();
        GT_CHECK_RESULT(nodeCount == expectedNodeCount, "Invalid node count: " + QString::number(nodeCount), {});
    }
    return orderedRectangularNodes;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getOrderedRectangularBranches"
QList<TvRectangularBranchItem*> GTUtilsPhyTree::getOrderedRectangularBranches(HI::GUITestOpStatus& os) {
    return getSubtreeOrderedRectangularBranches(os, getRootRectangularBranch(os));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getRootNode"
TvNodeItem* GTUtilsPhyTree::getRootNode(HI::GUITestOpStatus& os) {
    TvBranchItem* rootItem = getRootBranch(os);
    TvNodeItem* buttonItem = rootItem->getNodeItem();
    GT_CHECK_RESULT(buttonItem != nullptr, "Root branch has no button", nullptr);
    return buttonItem;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getRootBranch"
TvBranchItem* GTUtilsPhyTree::getRootBranch(HI::GUITestOpStatus& os) {
    auto treeView = GTWidget::findGraphicsView(os, "treeView");
    QList<QGraphicsItem*> itemList = treeView->scene()->items();
    for (QGraphicsItem* item : qAsConst(itemList)) {
        if (auto branchItem = dynamic_cast<TvBranchItem*>(item)) {
            if (branchItem->isRoot()) {
                return branchItem;
            }
        }
    }
    GT_FAIL("Failed to find root branch", nullptr);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getRootRectangularBranch"
TvRectangularBranchItem* GTUtilsPhyTree::getRootRectangularBranch(HI::GUITestOpStatus& os) {
    TreeViewerUI* treeViewerUi = getTreeViewerUi(os);
    GT_CHECK_RESULT(treeViewerUi != nullptr, "TreeViewerUI is NULL", nullptr);

    QList<QGraphicsItem*> items = treeViewerUi->scene()->items();
    for (QGraphicsItem* item : qAsConst(items)) {
        auto rectangularBranch = dynamic_cast<TvRectangularBranchItem*>(item);
        if (rectangularBranch != nullptr && rectangularBranch->isRoot()) {
            return rectangularBranch;
        }
    }

    return nullptr;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSubtreeOrderedRectangularBranches"
QList<TvRectangularBranchItem*> GTUtilsPhyTree::getSubtreeOrderedRectangularBranches(HI::GUITestOpStatus& os, TvRectangularBranchItem* rootBranch) {
    GT_CHECK_RESULT(rootBranch != nullptr, "Subtree root branch is NULL", QList<TvRectangularBranchItem*>());

    const QList<QGraphicsItem*> childItems = rootBranch->childItems();
    QList<TvRectangularBranchItem*> childRectangularBranches;
    for (QGraphicsItem* childItem : qAsConst(childItems)) {
        auto childRectangularBranch = dynamic_cast<TvRectangularBranchItem*>(childItem);
        if (childRectangularBranch != nullptr && childRectangularBranch->getDistanceTextItem() != nullptr) {
            childRectangularBranches << childRectangularBranch;
        }
    }

    std::sort(childRectangularBranches.begin(), childRectangularBranches.end(), rectangularBranchLessThan);

    QList<TvRectangularBranchItem*> subtreeOrderedRectangularBranches;
    for (TvRectangularBranchItem* childRectangularBranch : qAsConst(childRectangularBranches)) {
        subtreeOrderedRectangularBranches << getSubtreeOrderedRectangularBranches(os, childRectangularBranch);
    }
    subtreeOrderedRectangularBranches << rootBranch;

    return subtreeOrderedRectangularBranches;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "rectangularBranchLessThan"
bool GTUtilsPhyTree::rectangularBranchLessThan(TvRectangularBranchItem* first, TvRectangularBranchItem* second) {
    SAFE_POINT(first != nullptr, "First rectangular branch item is NULL", true);
    SAFE_POINT(second != nullptr, "Second rectangular branch item is NULL", false);

    if (first->getSide() == second->getSide()) {
        return first->getSide() == TvBranchItem::Side::Right
                   ? first->getDist() < second->getDist()
                   : first->getDist() > second->getDist();
    }

    return first->getSide() > second->getSide();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getActiveTreeViewerWindow"
QWidget* GTUtilsPhyTree::getActiveTreeViewerWindow(GUITestOpStatus& os) {
    QWidget* widget = GTUtilsMdi::getActiveObjectViewWindow(os, TreeViewerFactory::ID);
    GTThread::waitForMainThread();
    return widget;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "captureTreeImage"
QImage GTUtilsPhyTree::captureTreeImage(GUITestOpStatus& os) {
    return GTWidget::getImage(os, getTreeViewerUi(os));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkTreeViewerWindowIsActive"
void GTUtilsPhyTree::checkTreeViewerWindowIsActive(GUITestOpStatus& os, const QString& titlePart) {
    getActiveTreeViewerWindow(os);
    if (!titlePart.isEmpty()) {
        auto windowTitle = GTUtilsMdi::activeWindowTitle(os);
        GT_CHECK_RESULT(windowTitle.contains(titlePart),
                        QString("Wrong window title part. Expected part: '%1', actual title: '%2'")
                            .arg(titlePart)
                            .arg(windowTitle), );
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickZoomInButton"
void GTUtilsPhyTree::clickZoomInButton(HI::GUITestOpStatus& os) {
    GTToolbar::clickWidgetByActionName(os, MWTOOLBAR_ACTIVEMDI, "zoomInTreeViewerAction");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickZoomOutButton"
void GTUtilsPhyTree::clickZoomOutButton(HI::GUITestOpStatus& os) {
    GTToolbar::clickWidgetByActionName(os, MWTOOLBAR_ACTIVEMDI, "zoomOutTreeViewerAction");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickResetZoomButton"
void GTUtilsPhyTree::clickResetZoomButton(HI::GUITestOpStatus& os) {
    GTToolbar::clickWidgetByActionName(os, MWTOOLBAR_ACTIVEMDI, "resetZoomTreeViewerAction");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSceneWidth"
int GTUtilsPhyTree::getSceneWidth(HI::GUITestOpStatus& os) {
    TreeViewerUI* ui = getTreeViewerUi(os);
    QRect rect = ui->mapFromScene(ui->sceneRect()).boundingRect();
    return rect.width();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "zoomWithMouseWheel"
void GTUtilsPhyTree::zoomWithMouseWheel(GUITestOpStatus& os, int steps) {
    TreeViewerUI* treeViewer = getTreeViewerUi(os);
    QPoint treeViewCenter = treeViewer->mapToGlobal(treeViewer->rect().center());
    GTMouseDriver::moveTo(treeViewCenter);
    for (int i = 0; i < qAbs(steps); i++) {
        GTMouseDriver::scroll(steps > 0 ? 1 : -1);
    }
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
