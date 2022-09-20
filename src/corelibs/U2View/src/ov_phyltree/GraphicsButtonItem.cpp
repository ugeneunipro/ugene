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
#include <QPen>
#include <QStyleOptionGraphicsItem>

#include <U2Core/PhyTreeObject.h>
#include <U2Core/U2SafePoints.h>

#include "GraphicsBranchItem.h"
#include "GraphicsRectangularBranchItem.h"
#include "TreeViewer.h"
#include "TreeViewerUtils.h"

namespace U2 {

/** Button radius in pixels. */
static constexpr double radius = 5;

static const QBrush normalStateBrush(Qt::gray);
static const QBrush selectedStateBrush(QColor("#EA9700"));
static const QBrush hoveredStateBrush(QColor("#FFA500")); // The same hue as selected but lighter.

GraphicsButtonItem::GraphicsButtonItem(double nodeValue)
    : QGraphicsEllipseItem(QRectF(-radius, -radius, 2 * radius, 2 * radius)),
      nodeValue(nodeValue) {
    setPen(QColor(0, 0, 0));
    setAcceptHoverEvents(true);
    setZValue(2);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setToolTip(QObject::tr("Left click to select the branch\nDouble-click to collapse the branch"));
    if (nodeValue >= 0) {
        nodeLabel = new QGraphicsSimpleTextItem(QString::number(nodeValue), this);
        nodeLabel->setFont(TreeViewerUtils::getFont());
        nodeLabel->setBrush(Qt::darkGray);
        QRectF rect = nodeLabel->boundingRect();
        nodeLabel->setPos(GraphicsBranchItem::TEXT_SPACING, -rect.height() / 2);
        nodeLabel->setParentItem(this);
        nodeLabel->setFlag(QGraphicsItem::ItemIgnoresTransformations, false);
        nodeLabel->setZValue(1);
    }
}

void GraphicsButtonItem::mousePressEvent(QGraphicsSceneMouseEvent* e) {
    auto parentBranchItem = dynamic_cast<GraphicsBranchItem*>(parentItem());
    SAFE_POINT(parentBranchItem != nullptr, "No parentBranchItem", );
    TreeViewerUI* ui = getTreeViewerUI();
    if (e->button() == Qt::LeftButton && e->modifiers().testFlag(Qt::ShiftModifier)) {
        // Invert selection state on Shift.
        parentBranchItem->setSelected(!isSelected());
    } else {
        // Set a new selection.
        ui->getRoot()->setSelected(false);
        parentBranchItem->setSelected(true);
    }
    ui->isSelectionStateManagedByChildOnClick = true;
    e->accept();
}

void GraphicsButtonItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* e) {
    toggleCollapsedState();
    QAbstractGraphicsShapeItem::mouseDoubleClickEvent(e);
}

void GraphicsButtonItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    isHovered = true;
    QGraphicsItem::hoverEnterEvent(event);
}

void GraphicsButtonItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    isHovered = false;
    QGraphicsItem::hoverLeaveEvent(event);
}

void GraphicsButtonItem::toggleCollapsedState() {
    auto branch = dynamic_cast<GraphicsBranchItem*>(parentItem());
    SAFE_POINT(branch != nullptr, "Collapsing is impossible because button has not parent branch", );
    if (dynamic_cast<GraphicsBranchItem*>(branch->parentItem()) != nullptr) {
        branch->toggleCollapsedState();
    }
}

void GraphicsButtonItem::swapSiblings() {
    auto branchItem = dynamic_cast<GraphicsBranchItem*>(parentItem());
    CHECK(branchItem != nullptr, );
    auto rectBranchItem = dynamic_cast<GraphicsRectangularBranchItem*>(branchItem);
    if (rectBranchItem == nullptr) {
        SAFE_POINT(branchItem->correspondingRectangularBranchItem, "No correspondingRectangularBranchItem", );
        rectBranchItem = branchItem->correspondingRectangularBranchItem;
    }
    rectBranchItem->swapSiblings();
}

bool GraphicsButtonItem::isPathToRootSelected() const {
    CHECK(isSelected(), false);

    auto branchItem = dynamic_cast<GraphicsBranchItem*>(parentItem());
    CHECK(branchItem != nullptr, true);

    auto parentBranchItem = dynamic_cast<GraphicsBranchItem*>(branchItem->parentItem());
    return parentBranchItem == nullptr || !parentBranchItem->isSelected();
}

bool GraphicsButtonItem::isCollapsed() {
    auto parent = dynamic_cast<GraphicsBranchItem*>(parentItem());
    return parent != nullptr && parent->isCollapsed();
}

void GraphicsButtonItem::rerootTree(PhyTreeObject* treeObject) {
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
    CHECK(nodeLabel != nullptr, );
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

double GraphicsButtonItem::getNodeValue() const {
    return nodeValue;
}

const QGraphicsSimpleTextItem* GraphicsButtonItem::getLabel() const {
    return nodeLabel;
}

GraphicsBranchItem* GraphicsButtonItem::getParentBranchItem() const {
    auto result = dynamic_cast<GraphicsBranchItem*>(parentItem());
    SAFE_POINT(result != nullptr, "Node item has no parent branch", nullptr);
    return result;
}

GraphicsBranchItem* GraphicsButtonItem::getLeftBranchItem() const {
    return getParentBranchItem()->getChildBranch(GraphicsBranchItem::Side::Left);
}

GraphicsBranchItem* GraphicsButtonItem::getRightBranchItem() const {
    return getParentBranchItem()->getChildBranch(GraphicsBranchItem::Side::Right);
}

void GraphicsButtonItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    setBrush(isHovered ? hoveredStateBrush : (isSelected() ? selectedStateBrush : normalStateBrush));

    // Drop default 'selected' / 'focused': we draw it by ourselves using a custom brush.
    QStyleOptionGraphicsItem clonedStyleOption(*option);
    clonedStyleOption.state.setFlag(QStyle::State_Selected, false);
    clonedStyleOption.state.setFlag(QStyle::State_HasFocus, false);

    QGraphicsEllipseItem::paint(painter, &clonedStyleOption, widget);
}

}  // namespace U2
