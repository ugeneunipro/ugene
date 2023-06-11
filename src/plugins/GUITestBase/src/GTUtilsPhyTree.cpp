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

#include <base_dialogs/ColorDialogFiller.h>
#include <drivers/GTKeyboardDriver.h>
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
TvNodeItem* GTUtilsPhyTree::getNodeByBranchText(const QString& leftBranchText, const QString& rightBranchText) {
    auto treeView = GTWidget::findGraphicsView("treeView");
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
QList<TvNodeItem*> GTUtilsPhyTree::getNodes() {
    auto treeView = GTWidget::findGraphicsView("treeView");
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
QList<TvNodeItem*> GTUtilsPhyTree::getSelectedNodes() {
    QList<TvNodeItem*> nodes = getNodes();
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
QList<TvNodeItem*> GTUtilsPhyTree::getUnselectedNodes() {
    QList<TvNodeItem*> nodes = getNodes();
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
QList<QGraphicsSimpleTextItem*> GTUtilsPhyTree::getLabels(QGraphicsView* treeView) {
    QList<QGraphicsSimpleTextItem*> result;
    if (treeView == nullptr) {
        treeView = GTWidget::findGraphicsView("treeView");
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

QList<QGraphicsSimpleTextItem*> GTUtilsPhyTree::getVisibleLabels(QGraphicsView* treeView) {
    QList<QGraphicsSimpleTextItem*> result;
    foreach (QGraphicsSimpleTextItem* item, getLabels(treeView)) {
        if (item->isVisible() && !item->text().isEmpty()) {
            result << item;
        }
    }
    return result;
}

#define GT_METHOD_NAME "getDistances"
QList<QGraphicsSimpleTextItem*> GTUtilsPhyTree::getDistances(QGraphicsView* treeView) {
    QList<QGraphicsSimpleTextItem*> result;
    if (treeView == nullptr) {
        treeView = GTWidget::findGraphicsView("treeView");
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
QList<QGraphicsSimpleTextItem*> GTUtilsPhyTree::getVisibleDistances(QGraphicsView* treeView) {
    QList<QGraphicsSimpleTextItem*> result;
    const QList<QGraphicsSimpleTextItem*> textItemList = getDistances(treeView);
    for (QGraphicsSimpleTextItem* item : qAsConst(textItemList)) {
        if (item->isVisible()) {
            result << item;
        }
    }
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getDistancesValues"
QList<double> GTUtilsPhyTree::getDistancesValues() {
    QList<double> result;
    QList<QGraphicsSimpleTextItem*> distList = getDistances();

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
QStringList GTUtilsPhyTree::getLabelsText() {
    QStringList result;
    QList<QGraphicsSimpleTextItem*> labelList = getLabels();
    for (QGraphicsSimpleTextItem* item : qAsConst(labelList)) {
        result << item->text();
    }

    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getGlobalCenterCoord"
QPoint GTUtilsPhyTree::getGlobalCenterCoord(QGraphicsItem* item) {
    auto treeView = GTWidget::findGraphicsView("treeView");
    QRectF viewRect = getItemViewRect(item);
    return treeView->mapToGlobal(viewRect.center().toPoint());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getItemViewRect"
QRect GTUtilsPhyTree::getItemViewRect(QGraphicsItem* item) {
    auto treeView = GTWidget::findGraphicsView("treeView");
    QRectF sceneRect = item->mapToScene(item->boundingRect()).boundingRect();
    return treeView->mapFromScene(sceneRect).boundingRect();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickNode"
void GTUtilsPhyTree::clickNode(TvNodeItem* node, const Qt::MouseButton& mouseButton) {
    GT_CHECK(node != nullptr, "Node to click is NULL");
    node->ensureVisible();  // TODO: do not run from the non-UI thread.
    GTThread::waitForMainThread();
    GTMouseDriver::moveTo(getGlobalCenterCoord(node));
    GTMouseDriver::click(mouseButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "doubleClickNode"
void GTUtilsPhyTree::doubleClickNode(TvNodeItem* node) {
    GT_CHECK(node != nullptr, "Node to doubleClickNode is NULL");
    node->ensureVisible();
    GTThread::waitForMainThread();
    GTMouseDriver::moveTo(getGlobalCenterCoord(node));
    GTMouseDriver::doubleClick();
    GTThread::waitForMainThread();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getNodeDistance"
qreal GTUtilsPhyTree::getNodeDistance(TvNodeItem* node) {
    GT_CHECK_RESULT(node != nullptr, "Node is NULL", 0);
    auto branch = dynamic_cast<TvRectangularBranchItem*>(node->parentItem());
    GT_CHECK_RESULT(branch != nullptr, "Node's branch' is NULL", 0);
    return branch->getDist();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getTreeViewerUi"
TreeViewerUI* GTUtilsPhyTree::getTreeViewerUi() {
    return GTWidget::findExactWidget<TreeViewerUI*>("treeView", GTUtilsMdi::activeWindow());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getOrderedRectangularNodes"
QList<TvNodeItem*> GTUtilsPhyTree::getOrderedRectangularNodes(int expectedNodeCount) {
    QList<TvNodeItem*> orderedRectangularNodes;
    QList<TvRectangularBranchItem*> graphicsRectangularBranchItems = getOrderedRectangularBranches();
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
QList<TvRectangularBranchItem*> GTUtilsPhyTree::getOrderedRectangularBranches() {
    return getSubtreeOrderedRectangularBranches(getRootRectangularBranch());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getRootNode"
TvNodeItem* GTUtilsPhyTree::getRootNode() {
    TvBranchItem* rootItem = getRootBranch();
    TvNodeItem* buttonItem = rootItem->getNodeItem();
    GT_CHECK_RESULT(buttonItem != nullptr, "Root branch has no button", nullptr);
    return buttonItem;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getRootBranch"
TvBranchItem* GTUtilsPhyTree::getRootBranch() {
    auto treeView = GTWidget::findGraphicsView("treeView");
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
TvRectangularBranchItem* GTUtilsPhyTree::getRootRectangularBranch() {
    TreeViewerUI* treeViewerUi = getTreeViewerUi();
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
QList<TvRectangularBranchItem*> GTUtilsPhyTree::getSubtreeOrderedRectangularBranches(TvRectangularBranchItem* rootBranch) {
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
        subtreeOrderedRectangularBranches << getSubtreeOrderedRectangularBranches(childRectangularBranch);
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
QWidget* GTUtilsPhyTree::getActiveTreeViewerWindow() {
    QWidget* widget = GTUtilsMdi::getActiveObjectViewWindow(TreeViewerFactory::ID);
    GTThread::waitForMainThread();
    return widget;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "captureTreeImage"
QImage GTUtilsPhyTree::captureTreeImage() {
    return GTWidget::getImage(getTreeViewerUi());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkTreeViewerWindowIsActive"
void GTUtilsPhyTree::checkTreeViewerWindowIsActive(const QString& titlePart) {
    getActiveTreeViewerWindow();
    if (!titlePart.isEmpty()) {
        auto windowTitle = GTUtilsMdi::activeWindowTitle();
        GT_CHECK_RESULT(windowTitle.contains(titlePart),
                        QString("Wrong window title part. Expected part: '%1', actual title: '%2'")
                            .arg(titlePart)
                            .arg(windowTitle), );
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickZoomInButton"
void GTUtilsPhyTree::clickZoomInButton() {
    GTToolbar::clickWidgetByActionName(MWTOOLBAR_ACTIVEMDI, "zoomInTreeViewerAction");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickZoomOutButton"
void GTUtilsPhyTree::clickZoomOutButton() {
    GTToolbar::clickWidgetByActionName(MWTOOLBAR_ACTIVEMDI, "zoomOutTreeViewerAction");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickZoomFitButton"
void GTUtilsPhyTree::clickZoomFitButton() {
    GTToolbar::clickWidgetByActionName(MWTOOLBAR_ACTIVEMDI, "zoomFitAction");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickZoom100Button"
void GTUtilsPhyTree::clickZoom100Button() {
    GTToolbar::clickWidgetByActionName(MWTOOLBAR_ACTIVEMDI, "zoom100Action");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getSceneWidth"
int GTUtilsPhyTree::getSceneWidth() {
    TreeViewerUI* ui = getTreeViewerUi();
    QRect rect = ui->mapFromScene(ui->sceneRect()).boundingRect();
    return rect.width();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "zoomWithMouseWheel"
void GTUtilsPhyTree::zoomWithMouseWheel(int steps) {
    TreeViewerUI* treeViewer = getTreeViewerUi();
    zoomWithMouseWheel(treeViewer, steps);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "zoomWithMouseWheelWithTreeViewer"
void GTUtilsPhyTree::zoomWithMouseWheel(QWidget* treeViewer, int steps) {
    QPoint treeViewCenter = treeViewer->mapToGlobal(treeViewer->rect().center());
    GTMouseDriver::moveTo(treeViewCenter);
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    for (int i = 0; i < qAbs(steps); i++) {
        GTMouseDriver::scroll(steps > 0 ? 1 : -1);
    }
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setBranchColor"
void GTUtilsPhyTree::setBranchColor(int r, int g, int b, bool checkNoActiveDialogWaiters) {
    GTUtilsDialog::waitForDialog(new ColorDialogFiller(r, g, b));
    auto branchesColorButton = GTWidget::findWidget("branchesColorButton");
    GTWidget::click(branchesColorButton);
    if (checkNoActiveDialogWaiters) {
        GTUtilsDialog::checkNoActiveWaiters();
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getColorPercent"
double GTUtilsPhyTree::getColorPercent(QWidget* widget, const QString& colorName) {
    int total = 0;
    int found = 0;
    const QImage img = GTWidget::getImage(widget);
    QRect r = widget->rect();
    int wid = r.width();
    int heig = r.height();
    for (int i = 0; i < wid; i++) {
        for (int j = 0; j < heig; j++) {
            total++;
            QPoint p(i, j);
            QRgb rgb = img.pixel(p);
            QColor color = QColor(rgb);
            QString name = color.name();
            if (name == colorName) {
                found++;
            }
        }
    }
    double result = static_cast<double>(found) / total;
    return result;
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
