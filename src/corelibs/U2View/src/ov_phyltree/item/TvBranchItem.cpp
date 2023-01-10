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

#include "TvBranchItem.h"

#include <QGraphicsScene>
#include <QPainter>
#include <QStack>

#include <U2Core/PhyTree.h>
#include <U2Core/U2SafePoints.h>

#include "../TreeViewerUtils.h"
#include "TvNodeItem.h"
#include "TvRectangularBranchItem.h"
#include "TvTextItem.h"

namespace U2 {

TvBranchItem::TvBranchItem(bool withNode, const TvBranchItem::Side& _side, const QString& nodeName)
    : side(_side) {
    settings[BRANCH_THICKNESS] = 1;
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(false);
    setAcceptedMouseButtons(Qt::NoButton);

    if (withNode) {
        nodeItem = new TvNodeItem(nodeName);
        nodeItem->setParentItem(this);
    }

    QColor branchColor = qvariant_cast<QColor>(settings[BRANCH_COLOR]);
    setBrush(branchColor);
    QPen pen1(branchColor);
    pen1.setCosmetic(true);
    setPen(pen1);
}

TvBranchItem::TvBranchItem(const PhyBranch* branch, const QString& name, bool isRoot)
    : phyBranch(branch) {
    distance = branch == nullptr ? 0.0 : branch->distance;
    settings[BRANCH_THICKNESS] = 1;
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(false);
    setAcceptedMouseButtons(Qt::NoButton);

    QColor branchColor = qvariant_cast<QColor>(settings[BRANCH_COLOR]);
    setBrush(branchColor);

    QPen pen(branchColor);
    pen.setCosmetic(true);
    setPen(pen);

    if ((branch != nullptr && !branch->childNode->isLeafNode()) || isRoot) {
        QString nodeName = branch == nullptr ? "" : branch->childNode->name;
        nodeItem = new TvNodeItem(nodeName);
        nodeItem->setParentItem(this);
    }

    if (name.isEmpty()) {
        addDistanceTextItem(distance);
    } else {  // 'this' is not a real branch item, but the name label only with dotted alignment lines.
        nameTextItem = new TvTextItem(this, name);
        setLabelPositions();
        nameTextItem->setZValue(1);
        pen.setStyle(Qt::DotLine);
        setPen(pen);
    }
}

void TvBranchItem::updateSettings(const OptionsMap& newSettings) {
    settings = newSettings;
    int penWidth = settings[BRANCH_THICKNESS].toInt();
    if (isSelected()) {
        penWidth += SELECTED_PEN_WIDTH_DELTA;
    }

    QColor branchColor = qvariant_cast<QColor>(settings[BRANCH_COLOR]);
    QPen currentPen = pen();
    currentPen.setColor(branchColor);
    currentPen.setWidth(penWidth);
    setPen(currentPen);

    QFont font = TreeViewerUtils::getFontFromSettings(settings);
    QColor labelColor = qvariant_cast<QColor>(settings[LABEL_COLOR]);
    if (distanceTextItem != nullptr) {
        distanceTextItem->setFont(font);
        distanceTextItem->setBrush(labelColor);
    }
    if (nameTextItem != nullptr) {
        nameTextItem->setFont(font);
        nameTextItem->setBrush(labelColor);
    }
    setLabelPositions();
}

const OptionsMap& TvBranchItem::getSettings() const {
    return settings;
}

void TvBranchItem::toggleCollapsedState() {
    collapsed = !collapsed;
    QList<QGraphicsItem*> items = childItems();
    if (collapsed) {
        for (auto item : qAsConst(items)) {
            if (dynamic_cast<TvBranchItem*>(item)) {
                item->hide();
            }
        }
        int penWidth = settings[BRANCH_THICKNESS].toInt();
        if (isSelected()) {
            penWidth += SELECTED_PEN_WIDTH_DELTA;
        }

        QColor branchColor = qvariant_cast<QColor>(settings[BRANCH_COLOR]);
        QPen pen1(branchColor);
        pen1.setWidth(penWidth);
        pen1.setCosmetic(true);
        auto rectItem = new QGraphicsRectItem(0, -4, 16, 8, this);
        rectItem->setPen(pen1);
    } else {
        for (auto& item : qAsConst(items)) {
            if (auto rectItem = dynamic_cast<QGraphicsRectItem*>(item)) {
                rectItem->setParentItem(nullptr);
                scene()->removeItem(rectItem);
            } else if (item != getDistanceTextItem() && item != getNameTextItem()) {
                item->show();
            }
        }
        setSelectedRecursively(true);
    }
    getRoot()->emitBranchCollapsed(this);
}

void TvBranchItem::setSelectedRecursively(bool isSelected) {
    if (nodeItem) {
        nodeItem->setSelected(isSelected);
    }
    QList<QGraphicsItem*> children = childItems();
    for (QGraphicsItem* graphItem : qAsConst(children)) {
        if (auto childBranch = dynamic_cast<TvBranchItem*>(graphItem)) {
            childBranch->setSelectedRecursively(isSelected);
        }
    }
    QAbstractGraphicsShapeItem::setSelected(isSelected);
}

void TvBranchItem::addDistanceTextItem(double d) {
    QString str = QString::number(d, 'f', 3);
    // Trim trailing zeros.
    int i = str.length() - 1;
    for (; i >= 0 && str[i] == '0'; --i) {
    }
    if (str[i] == '.') {
        --i;
    }
    str.truncate(i + 1);
    // Do not show zeroes.
    if (str == "0") {
        str = "";
    }
    initDistanceText(str);
}

QString TvBranchItem::getNodeNameFromNodeItem() const {
    return nodeItem != nullptr ? nodeItem->nodeName : "";
}

void TvBranchItem::setLabelPositions() {
    if (nameTextItem != nullptr) {
        QRectF rect = nameTextItem->boundingRect();
        nameTextItem->setPos(TvBranchItem::TEXT_SPACING, -rect.height() / 2);
    }
    if (distanceTextItem != nullptr) {
        QRectF rect = distanceTextItem->boundingRect();
        distanceTextItem->setPos(-rect.width() / 2 - width / 2, 0);
    }
}

void TvBranchItem::setDistanceText(const QString& text) {
    if (distanceTextItem != nullptr) {
        distanceTextItem->setText(text);
    }
}

void TvBranchItem::setWidth(double newWidth) {
    CHECK(width != newWidth, )
    double delta = newWidth - width;
    setPos(pos() + QPointF(delta, 0));
    setLabelPositions();
    if (getDistanceTextItem() != nullptr) {
        QPointF newPos = getDistanceTextItem()->pos() + QPointF(-delta / 2, 0);
        getDistanceTextItem()->setPos(newPos);
    }
    prepareGeometryChange();
    width = newWidth;
}

bool TvBranchItem::isCollapsed() const {
    return collapsed;
}

void TvBranchItem::setUpPainter(QPainter* p) {
    QPen currentPen = pen();
    currentPen.setWidth(settings[BRANCH_THICKNESS].toInt() + (isSelected() ? SELECTED_PEN_WIDTH_DELTA : 0));
    setPen(currentPen);
    p->setPen(currentPen);
}

void TvBranchItem::initDistanceText(const QString& text) {
    distanceTextItem = new TvTextItem(this, text);
    setLabelPositions();
    distanceTextItem->setZValue(1);
}

QRectF TvBranchItem::visibleChildrenBoundingRect(const QTransform& viewTransform) const {
    QRectF childrenBoundingRect;
    QStack<const QGraphicsItem*> graphicsItems;
    graphicsItems.push(this);

    QTransform invertedTransform = viewTransform.inverted();
    do {
        const QGraphicsItem* branchItem = graphicsItems.pop();
        QList<QGraphicsItem*> items = branchItem->childItems();
        for (QGraphicsItem* graphItem : qAsConst(items)) {
            if (!graphItem->isVisible()) {
                continue;
            }
            QRectF itemRect = graphItem->sceneBoundingRect();
            if (graphItem->flags().testFlag(QGraphicsItem::ItemIgnoresTransformations)) {
                QRectF transformedRect = invertedTransform.mapRect(itemRect);
                itemRect.setWidth(transformedRect.width());
            }
            childrenBoundingRect |= itemRect;
            graphicsItems.push(graphItem);
        }
    } while (!graphicsItems.isEmpty());
    return childrenBoundingRect;
}

bool TvBranchItem::isRoot() const {
    return parentItem() == nullptr;
}

TvBranchItem* TvBranchItem::getRoot() {
    auto root = dynamic_cast<TvBranchItem*>(topLevelItem());
    SAFE_POINT(root != nullptr, "Top level item is not a branch item", root);
    return root;
}

void TvBranchItem::emitBranchCollapsed(TvBranchItem* branch) {
    SAFE_POINT(isRoot(), "Not a root branch!", );
    emit si_branchCollapsed(branch);
}

TvNodeItem* TvBranchItem::getNodeItem() const {
    return nodeItem;
}

TvBranchItem* TvBranchItem::getChildBranch(const TvBranchItem::Side& childBranchSide) const {
    QList<QGraphicsItem*> children = childItems();
    for (QGraphicsItem* childItem : qAsConst(children)) {
        if (auto childBranch = dynamic_cast<TvBranchItem*>(childItem)) {
            if (childBranch->side == childBranchSide) {
                return childBranch;
            }
        }
    }
    return nullptr;
}

TvTextItem* TvBranchItem::getDistanceTextItem() const {
    return distanceTextItem;
}

QString TvBranchItem::getDistanceText() const {
    return distanceTextItem == nullptr ? "" : distanceTextItem->text();
}

TvTextItem* TvBranchItem::getNameTextItem() const {
    return nameTextItem;
}

double TvBranchItem::getWidth() const {
    return width;
}

double TvBranchItem::getDist() const {
    return distance;
}

void TvBranchItem::setWidthW(double w) {
    width = w;
}

void TvBranchItem::setDist(double d) {
    distance = d;
}

}  // namespace U2
