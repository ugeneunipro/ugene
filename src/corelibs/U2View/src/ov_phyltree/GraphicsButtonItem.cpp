/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "GraphicsButtonItem.h"

#include <QGraphicsSceneMouseEvent>
#include <QList>
#include <QMenu>
#include <QPainter>
#include <QPen>

#include <U2Core/PhyTreeObject.h>
#include <U2Core/U2SafePoints.h>

#include "GraphicsBranchItem.h"
#include "GraphicsRectangularBranchItem.h"
#include "TreeViewer.h"
#include "TreeViewerUtils.h"

namespace U2 {

const qreal GraphicsButtonItem::radius = 5.0;
const QBrush GraphicsButtonItem::highlightingBrush = QBrush(QColor("#ea9700"));
const QBrush GraphicsButtonItem::ordinaryBrush = QBrush(Qt::gray);

GraphicsButtonItem::GraphicsButtonItem(double nodeValue)
    : QGraphicsEllipseItem(QRectF(-radius, -radius, 2 * radius, 2 * radius)),
      nodeValue(nodeValue) {
    setPen(QColor(0, 0, 0));
    setBrush(ordinaryBrush);
    setAcceptHoverEvents(true);
    setZValue(2);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setToolTip(QObject::tr("Left click to select the branch\nDouble-click to collapse the branch"));

    if (nodeValue >= 0) {
        nodeLabel = new QGraphicsSimpleTextItem(QString::number(nodeValue), this);
        nodeLabel->setFont(TreeViewerUtils::getFont());
        nodeLabel->setBrush(Qt::darkGray);
        QRectF rect = nodeLabel->boundingRect();
        nodeLabel->setPos(GraphicsBranchItem::TextSpace, -rect.height() / 2);
        nodeLabel->setParentItem(this);
        nodeLabel->setFlag(QGraphicsItem::ItemIgnoresTransformations, false);
        nodeLabel->setZValue(1);
    }
}

const QGraphicsSimpleTextItem* GraphicsButtonItem::getLabel() const {
    return nodeLabel;
}

void GraphicsButtonItem::mousePressEvent(QGraphicsSceneMouseEvent* e) {
    auto parentBranchItem = dynamic_cast<GraphicsBranchItem*>(parentItem());
    SAFE_POINT(parentBranchItem != nullptr, "No parentBranchItem", );
    TreeViewerUI* ui = getTreeViewerUI();
    if (e->button() == Qt::LeftButton && e->modifiers().testFlag(Qt::ShiftModifier)) {
        // Invert selection state on Shift.
        parentBranchItem->setSelectedRecurs(!isSelected, true);
    } else {
        // Set a new selection .
        ui->getRoot()->setSelectedRecurs(false, true);
        parentBranchItem->setSelectedRecurs(true, true);
    }
    ui->isSelectionStateManagedByChildOnClick = true;
    e->accept();
    update();
}

void GraphicsButtonItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* e) {
    uiLog.trace("Tree button double-clicked");
    collapse();
    QAbstractGraphicsShapeItem::mouseDoubleClickEvent(e);
}

void GraphicsButtonItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    if (isSelected) {
        return;
    }
    QGraphicsItem::hoverEnterEvent(event);
    setHighlighting(true);
}
void GraphicsButtonItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    if (isSelected) {
        return;
    }
    QGraphicsItem::hoverLeaveEvent(event);
    setHighlighting(false);
}

void GraphicsButtonItem::setSelected(bool selected) {
    isSelected = selected;
    setHighlighting(isSelected);
}

void GraphicsButtonItem::setHighlighting(bool isOn) {
    setBrush(isOn ? highlightingBrush : ordinaryBrush);
    update();
}

void GraphicsButtonItem::collapse() {
    auto branch = dynamic_cast<GraphicsBranchItem*>(parentItem());
    SAFE_POINT(branch != nullptr, "Collapsing is impossible because button has not parent branch", );
    if (dynamic_cast<GraphicsBranchItem*>(branch->parentItem()) != nullptr) {
        branch->toggleCollapsedState();
    }
}

void GraphicsButtonItem::swapSiblings() {
    uiLog.trace("Swapping siblings");

    auto branchItem = dynamic_cast<GraphicsBranchItem*>(parentItem());
    if (!branchItem) {
        return;
    }
    auto rectBranchItem = dynamic_cast<GraphicsRectangularBranchItem*>(branchItem);
    if (rectBranchItem == nullptr) {
        SAFE_POINT(branchItem->correspondingRectangularBranchItem, "No correspondingRectangularBranchItem", );
        rectBranchItem = branchItem->correspondingRectangularBranchItem;
    }
    rectBranchItem->swapSiblings();
}

bool GraphicsButtonItem::isPathToRootSelected() const {
    if (!isSelected) {
        return false;
    }
    GraphicsBranchItem* branchItem = dynamic_cast<GraphicsBranchItem*>(parentItem());
    if (branchItem == nullptr) {
        return true;
    }
    GraphicsBranchItem* parentBranchItem = dynamic_cast<GraphicsBranchItem*>(branchItem->parentItem());
    return parentBranchItem == nullptr || !parentBranchItem->isSelected();
}

bool GraphicsButtonItem::isCollapsed() {
    GraphicsBranchItem* parent = dynamic_cast<GraphicsBranchItem*>(parentItem());
    Q_ASSERT(parent);
    CHECK(parent, false)
    return parent->isCollapsed();
}

void GraphicsButtonItem::rerootTree(PhyTreeObject* treeObject) {
    uiLog.trace("Re-rooting of the PhyTree");
    SAFE_POINT(treeObject != nullptr, "Null pointer argument 'treeObject' was passed to 'PhyTreeUtils::rerootPhyTree' function", );

    auto parentBranchItem = dynamic_cast<GraphicsBranchItem*>(parentItem());
    CHECK(parentBranchItem != nullptr, );

    auto parentRectBranchItem = dynamic_cast<GraphicsRectangularBranchItem*>(parentBranchItem);
    if (parentRectBranchItem == nullptr) {
        SAFE_POINT(parentBranchItem->correspondingRectangularBranchItem, "No correspondingRectangularBranchItem", );
        parentRectBranchItem = parentBranchItem->correspondingRectangularBranchItem;
    }

    const PhyBranch* nodeBranch = parentRectBranchItem->getPhyBranch();
    CHECK(nodeBranch != nullptr, );
    PhyNode* newRoot = nodeBranch->node2;
    CHECK(newRoot != nullptr, );

    treeObject->rerootPhyTree(newRoot);
}

void GraphicsButtonItem::updateSettings(const OptionsMap& settings) {
    CHECK(nullptr != nodeLabel, );
    QFont newFont = qvariant_cast<QFont>(settings[LABEL_FONT_TYPE]);
    newFont.setPointSize(qvariant_cast<int>(settings[LABEL_FONT_SIZE]));
    newFont.setBold(qvariant_cast<bool>(settings[LABEL_FONT_BOLD]));
    newFont.setItalic(qvariant_cast<bool>(settings[LABEL_FONT_ITALIC]));
    newFont.setUnderline(qvariant_cast<bool>(settings[LABEL_FONT_UNDERLINE]));
    nodeLabel->setFont(newFont);
    QColor labelsColor = qvariant_cast<QColor>(settings[LABEL_COLOR]);
    nodeLabel->setBrush(labelsColor);
    bool showNodeLabels = settings[SHOW_NODE_LABELS].toBool();
    nodeLabel->setVisible(showNodeLabels);
}

TreeViewerUI* GraphicsButtonItem::getTreeViewerUI() const {
    QList<QGraphicsView*> views = scene()->views();
    SAFE_POINT(views.size() == 1, "getTreeViewerUI: invalid number of views: " + QString::number(views.size()), nullptr);
    auto ui = qobject_cast<TreeViewerUI*>(views[0]);
    SAFE_POINT(ui != nullptr, "getTreeViewerUI: ui is null", nullptr);
    return ui;
}
}  // namespace U2
