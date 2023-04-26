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

#include <U2Core/Log.h>
#include <U2Core/PhyTree.h>
#include <U2Core/U2SafePoints.h>

#include "../TreeViewerUtils.h"
#include "TvNodeItem.h"
#include "TvTextItem.h"

namespace U2 {

TvBranchItem::TvBranchItem(TvBranchItem* parentTvBranch, const PhyBranch* _phyBranch, const TvBranchItem::Side& _side, const QString& nodeName)
    : QAbstractGraphicsShapeItem(parentTvBranch), phyBranch(_phyBranch), side(_side) {
    settings[BRANCH_THICKNESS] = 1;
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(false);
    setAcceptedMouseButtons(Qt::NoButton);

    nodeItem = new TvNodeItem(this, nodeName);

    QColor branchColor = qvariant_cast<QColor>(settings[BRANCH_COLOR]);
    setBrush(branchColor);
    QPen pen1(branchColor);
    pen1.setCosmetic(true);
    setPen(pen1);
}

TvBranchItem::TvBranchItem(TvBranchItem* parentTvBranch, const PhyBranch* _phyBranch, const QString& sequenceName, bool isRoot)
    : QAbstractGraphicsShapeItem(parentTvBranch), phyBranch(_phyBranch) {
    distance = phyBranch == nullptr ? 0.0 : phyBranch->distance;
    settings[BRANCH_THICKNESS] = 1;
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(false);
    setAcceptedMouseButtons(Qt::NoButton);

    QColor branchColor = qvariant_cast<QColor>(settings[BRANCH_COLOR]);
    setBrush(branchColor);

    QPen pen(branchColor);
    pen.setCosmetic(true);
    setPen(pen);

    if (phyBranch != nullptr || isRoot) {
        QString nodeName = phyBranch == nullptr ? "" : phyBranch->childNode->name;
        nodeItem = new TvNodeItem(this, nodeName);
    }

    if (!sequenceName.isEmpty()) {
        // 'this' is not a real branch item, but the name label only with dotted alignment lines.
        nameTextItem = new TvTextItem(this, sequenceName);
        updateLabelPositions();
        nameTextItem->setZValue(1);
        pen.setStyle(Qt::DotLine);
        setPen(pen);
    } else {
        addDistanceTextItem(distance);
    }
}

void TvBranchItem::updateSettings(const QMap<TreeViewOption, QVariant>& newSettings) {
    prepareGeometryChange();
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
    if (nodeItem != nullptr) {
        nodeItem->updateSettings(settings);
    }
    updateLabelPositions();
}

const QMap<TreeViewOption, QVariant>& TvBranchItem::getSettings() const {
    return settings;
}

void TvBranchItem::toggleCollapsedState() {
    CHECK(!isLeaf(), );
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
            if (dynamic_cast<QGraphicsRectItem*>(item)) {
                delete item;
                continue;
            }
            if (item != getDistanceTextItem() && item != getNameTextItem()) {
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

void TvBranchItem::updateLabelPositions() {
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
    prepareGeometryChange();
    double delta = newWidth - width;
    setPos(pos() + QPointF(delta, 0));
    updateLabelPositions();
    if (getDistanceTextItem() != nullptr) {
        QPointF newPos = getDistanceTextItem()->pos() + QPointF(-delta / 2, 0);
        getDistanceTextItem()->setPos(newPos);
    }
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
    updateLabelPositions();
    distanceTextItem->setZValue(1);
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

bool TvBranchItem::isLeaf() const {
    // Handles both rectangular & circular/unrooted layouts that have different properties for leaf nodes.
    return (phyBranch != nullptr && phyBranch->childNode->isLeafNode()) || getChildBranch(Side::Left) == nullptr;
}

const TvBranchItem::Side& TvBranchItem::getSide() const {
    return side;
}

void TvBranchItem::setSide(const TvBranchItem::Side& newSide) {
    CHECK(side != newSide, );
    prepareGeometryChange();
    side = newSide;
}

}  // namespace U2
