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

#include "TvNodeItem.h"

#include <QGraphicsSceneMouseEvent>
#include <QList>
#include <QPen>
#include <QStyleOptionGraphicsItem>
#include <QTimer>

#include <U2Core/PhyTree.h>
#include <U2Core/U2SafePoints.h>

#include "../TreeViewer.h"
#include "../TreeViewerUtils.h"
#include "TvBranchItem.h"
#include "TvRectangularBranchItem.h"
#include "TvTextItem.h"

namespace U2 {

/** Button radius in pixels. */
static constexpr double radius = 5;

static const QBrush normalStateBrush(Qt::lightGray);
static const QBrush selectedStateBrush(QColor("#EA9700"));
static const QBrush hoveredStateBrush(QColor("#FFA500"));  // The same hue as selected but lighter.

TvNodeItem::TvNodeItem(TvBranchItem* parentItem, const QString& _nodeName)
    : QGraphicsEllipseItem(QRectF(-radius, -radius, 2 * radius, 2 * radius), parentItem),
      nodeName(_nodeName) {
    setPen(QColor(Qt::black));
    setAcceptHoverEvents(true);
    setZValue(2);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setToolTip(QObject::tr("Left click to select the branch\nDouble-click to collapse the branch"));
}

void TvNodeItem::mousePressEvent(QGraphicsSceneMouseEvent* e) {
    auto parentBranchItem = dynamic_cast<TvBranchItem*>(parentItem());
    SAFE_POINT(parentBranchItem != nullptr, "No parentBranchItem", );
    TreeViewerUI* ui = getTreeViewerUI();
    if (e->button() == Qt::LeftButton && e->modifiers().testFlag(Qt::ShiftModifier)) {
        // Invert selection state on Shift.
        parentBranchItem->setSelectedRecursively(!isSelected());
    } else {
        // Set a new selection.
        ui->getRoot()->setSelectedRecursively(false);
        parentBranchItem->setSelectedRecursively(true);
    }
    ui->isSelectionStateManagedByChildOnClick = true;
    e->accept();
}

void TvNodeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* e) {
    QTimer::singleShot(0, [this]() { toggleCollapsedState(); });
    QAbstractGraphicsShapeItem::mouseDoubleClickEvent(e);
}

void TvNodeItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    isHovered = true;
    QGraphicsItem::hoverEnterEvent(event);
}

void TvNodeItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    isHovered = false;
    QGraphicsItem::hoverLeaveEvent(event);
}

void TvNodeItem::toggleCollapsedState() const {
    getParentBranchItem()->toggleCollapsedState();
}

bool TvNodeItem::isSelectionRoot() const {
    CHECK(isSelected(), false);  // The node itself must be selected.

    auto branchItem = dynamic_cast<TvBranchItem*>(parentItem());
    CHECK(branchItem != nullptr, true);  // If node is a root node -> return true.

    // If node is not a root node -> check that its parent branch is not selected.
    auto parentBranchItem = dynamic_cast<TvBranchItem*>(branchItem->parentItem());
    return parentBranchItem == nullptr || !parentBranchItem->isSelected();
}

bool TvNodeItem::isCollapsed() const {
    TvBranchItem* parent = getParentBranchItem();
    return parent != nullptr && parent->isCollapsed();
}

PhyNode* TvNodeItem::getPhyNode() const {
    TvBranchItem* parentBranchItem = getParentBranchItem();
    return parentBranchItem->phyBranch ? parentBranchItem->phyBranch->childNode : nullptr;
}

void TvNodeItem::updateSettings(const QMap<TreeViewOption, QVariant>& settings) {
    bool isTipNode = getParentBranchItem()->isLeaf();
    isShapeVisible = settings[isTipNode ? SHOW_TIP_SHAPE : SHOW_NODE_SHAPE].toBool();

    bool isLabelVisible = !isTipNode && settings[SHOW_INNER_NODE_LABELS].toBool();
    if (labelItem == nullptr && isLabelVisible && !nodeName.isEmpty()) {
        labelItem = new TvTextItem(this, nodeName);
        labelItem->setZValue(1);
    }
    if (labelItem != nullptr) {
        labelItem->setFont(TreeViewerUtils::getFontFromSettings(settings));
        labelItem->setBrush(qvariant_cast<QColor>(settings[LABEL_COLOR]));
        QRectF rect = labelItem->boundingRect();
        labelItem->setPos(TvBranchItem::TEXT_SPACING, -rect.height() / 2);
        labelItem->setVisible(isLabelVisible);
    }
}

TreeViewerUI* TvNodeItem::getTreeViewerUI() const {
    QList<QGraphicsView*> views = scene()->views();
    SAFE_POINT(views.size() == 1, "getTreeViewerUI: invalid number of views: " + QString::number(views.size()), nullptr);

    auto ui = qobject_cast<TreeViewerUI*>(views[0]);
    SAFE_POINT(ui != nullptr, "getTreeViewerUI: ui is null", nullptr);
    return ui;
}

TvBranchItem* TvNodeItem::getParentBranchItem() const {
    auto result = dynamic_cast<TvBranchItem*>(parentItem());
    SAFE_POINT(result != nullptr, "Node item has no parent branch", nullptr);
    return result;
}

TvBranchItem* TvNodeItem::getLeftBranchItem() const {
    return getParentBranchItem()->getChildBranch(TvBranchItem::Side::Left);
}

TvBranchItem* TvNodeItem::getRightBranchItem() const {
    return getParentBranchItem()->getChildBranch(TvBranchItem::Side::Right);
}

void TvNodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    CHECK(isShapeVisible || isHovered || isSelected(), );

    setBrush(isHovered ? hoveredStateBrush : (isSelected() ? selectedStateBrush : normalStateBrush));
    painter->setRenderHint(QPainter::Antialiasing);
    // Drop the default 'selected' & 'focused' decoration: we draw these states by ourselves using a custom brush.
    QStyleOptionGraphicsItem clonedStyleOption(*option);
    clonedStyleOption.state.setFlag(QStyle::State_Selected, false);
    clonedStyleOption.state.setFlag(QStyle::State_HasFocus, false);

    QGraphicsEllipseItem::paint(painter, &clonedStyleOption, widget);
}

}  // namespace U2
