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

#include "GraphicsBranchItem.h"

#include <QGraphicsScene>
#include <QPainter>
#include <QStack>

#include <U2Core/U2SafePoints.h>

#include "GraphicsButtonItem.h"
#include "GraphicsRectangularBranchItem.h"
#include "TreeViewerUtils.h"

namespace U2 {

GraphicsBranchItem::GraphicsBranchItem(bool withButton, const GraphicsBranchItem::Side& _side, double nodeValue)
    : side(_side) {
    settings[BRANCH_THICKNESS] = 1;
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(false);
    setAcceptedMouseButtons(Qt::NoButton);

    if (withButton) {
        buttonItem = new GraphicsButtonItem(nodeValue);
        buttonItem->setParentItem(this);
    }

    QColor branchColor = qvariant_cast<QColor>(settings[BRANCH_COLOR]);
    setBrush(branchColor);
    QPen pen1(branchColor);
    pen1.setCosmetic(true);
    setPen(pen1);
}

GraphicsBranchItem::GraphicsBranchItem(const QString& name) {
    settings[BRANCH_THICKNESS] = 1;
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(false);
    setAcceptedMouseButtons(Qt::NoButton);

    QColor branchColor = qvariant_cast<QColor>(settings[BRANCH_COLOR]);
    QPen pen1(branchColor);
    pen1.setStyle(Qt::DotLine);
    pen1.setCosmetic(true);
    setPen(pen1);

    nameText = new QGraphicsSimpleTextItem(name);
    nameText->setFont(TreeViewerUtils::getFont());
    nameText->setBrush(Qt::darkGray);
    setLabelPositions();
    nameText->setParentItem(this);
    nameText->setZValue(1);
}

GraphicsBranchItem::GraphicsBranchItem(double _distance, bool withButton, double nodeValue)
    : distance(_distance) {
    settings[BRANCH_THICKNESS] = 1;
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(false);
    setAcceptedMouseButtons(Qt::NoButton);

    if (withButton) {
        buttonItem = new GraphicsButtonItem(nodeValue);
        buttonItem->setParentItem(this);
    }

    initText(distance);
    QColor branchColor = qvariant_cast<QColor>(settings[BRANCH_COLOR]);
    QPen pen1(branchColor);
    pen1.setCosmetic(true);
    if (distance < 0) {
        pen1.setStyle(Qt::DashLine);
    }
    setPen(pen1);
    setBrush(branchColor);
}

void GraphicsBranchItem::updateSettings(const OptionsMap& newSettings) {
    settings[BRANCH_COLOR] = newSettings[BRANCH_COLOR];
    settings[BRANCH_THICKNESS] = newSettings[BRANCH_THICKNESS];

    int penWidth = settings[BRANCH_THICKNESS].toUInt();
    if (isSelected()) {
        penWidth += SELECTED_PEN_WIDTH;
    }

    QColor branchColor = qvariant_cast<QColor>(settings[BRANCH_COLOR]);
    QPen currentPen = this->pen();
    currentPen.setColor(branchColor);
    currentPen.setWidth(penWidth);

    this->setPen(currentPen);
}

void GraphicsBranchItem::updateChildSettings(const OptionsMap& newSettings) {
    QList<QGraphicsItem*> childItems = this->childItems();
    for (QGraphicsItem* graphItem : qAsConst(childItems)) {
        if (auto branchItem = dynamic_cast<GraphicsBranchItem*>(graphItem)) {
            branchItem->updateSettings(newSettings);
            branchItem->updateChildSettings(newSettings);
        }
    }
}

void GraphicsBranchItem::updateTextProperty(TreeViewOption property, const QVariant& propertyVal) {
    QFont dtFont = distanceText ? distanceText->font() : QFont();
    QFont ntFont = nameText ? nameText->font() : QFont();
    switch (property) {
        case U2::LABEL_FONT_TYPE:
            dtFont.setFamily(qvariant_cast<QFont>(propertyVal).family());
            ntFont.setFamily(qvariant_cast<QFont>(propertyVal).family());
            break;
        case U2::LABEL_FONT_SIZE:
            dtFont.setPointSize(qvariant_cast<int>(propertyVal));
            ntFont.setPointSize(qvariant_cast<int>(propertyVal));
            break;
        case U2::LABEL_FONT_BOLD:
            dtFont.setBold(qvariant_cast<bool>(propertyVal));
            ntFont.setBold(qvariant_cast<bool>(propertyVal));
            break;
        case U2::LABEL_FONT_ITALIC:
            dtFont.setItalic(qvariant_cast<bool>(propertyVal));
            ntFont.setItalic(qvariant_cast<bool>(propertyVal));
            break;
        case U2::LABEL_FONT_UNDERLINE:
            dtFont.setUnderline(qvariant_cast<bool>(propertyVal));
            ntFont.setUnderline(qvariant_cast<bool>(propertyVal));
            break;
        case U2::LABEL_COLOR:
            if (distanceText) {
                distanceText->setBrush(qvariant_cast<QColor>(propertyVal));
            }
            if (nameText) {
                nameText->setBrush(qvariant_cast<QColor>(propertyVal));
            }
            break;
        default:
            break;
    }

    if (distanceText) {
        distanceText->setFont(dtFont);
    }
    if (nameText) {
        nameText->setFont(ntFont);
    }
}

const OptionsMap& GraphicsBranchItem::getSettings() const {
    return settings;
}

void GraphicsBranchItem::toggleCollapsedState() {
    collapsed = !collapsed;
    QList<QGraphicsItem*> items = childItems();
    if (collapsed) {
        for (auto item : qAsConst(items)) {
            if (dynamic_cast<GraphicsBranchItem*>(item)) {
                item->hide();
            }
        }
        int penWidth = settings[BRANCH_THICKNESS].toUInt();
        if (isSelected()) {
            penWidth += SELECTED_PEN_WIDTH;
        }

        QColor branchColor = qvariant_cast<QColor>(settings[BRANCH_COLOR]);
        QPen pen1(branchColor);
        pen1.setWidth(penWidth);
        pen1.setCosmetic(true);
        auto rectItem = new QGraphicsRectItem(0, -4, 16, 8, this);
        rectItem->setPen(pen1);
    } else {
        for (int i = 0, s = items.size(); i < s; ++i) {
            if (auto rectItem = dynamic_cast<QGraphicsRectItem*>(items[i])) {
                rectItem->setParentItem(nullptr);
                scene()->removeItem(rectItem);
            } else if (items[i] != getDistanceTextItem() && items[i] != getNameTextItem()) {
                items[i]->show();
            }
        }
        setSelectedRecurs(true, true);
    }
    getRoot()->emitBranchCollapsed(this);
}

void GraphicsBranchItem::setSelectedRecurs(bool sel, bool selectChilds) {
    if (!selectChilds) {
        setSelected(sel);
        scene()->update();
        return;
    }

    // Set selected for child items
    QStack<GraphicsBranchItem*> graphicsItems;
    graphicsItems.push(this);
    do {
        GraphicsBranchItem* branchItem = graphicsItems.pop();
        branchItem->setSelected(sel);
        QList<QGraphicsItem*> childItems = branchItem->childItems();
        for (QGraphicsItem* graphItem : qAsConst(childItems)) {
            if (auto childItem = dynamic_cast<GraphicsBranchItem*>(graphItem)) {
                graphicsItems.push(childItem);
            }
        }
    } while (!graphicsItems.isEmpty());

    scene()->update();
}

void GraphicsBranchItem::setSelected(bool isSelected) {
    if (buttonItem) {
        buttonItem->setSelected(isSelected);
    }

    int penWidth = settings[BRANCH_THICKNESS].toUInt();
    if (isSelected) {
        penWidth += SELECTED_PEN_WIDTH;
    }
    QPen currentPen = this->pen();
    currentPen.setWidth(penWidth);
    this->setPen(currentPen);

    QAbstractGraphicsShapeItem::setSelected(isSelected);
}

void GraphicsBranchItem::initText(double d) {
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

qreal GraphicsBranchItem::getNodeLabelValue() const {
    return buttonItem != nullptr ? buttonItem->getNodeValue() : -1;
}

void GraphicsBranchItem::setLabelPositions() {
    if (nameText != nullptr) {
        QRectF rect = nameText->boundingRect();
        nameText->setPos(GraphicsBranchItem::TEXT_SPACING, -rect.height() / 2);
    }
    if (distanceText != nullptr) {
        QRectF rect = distanceText->boundingRect();
        distanceText->setPos(-rect.width() / 2 - width / 2, 0);
    }
}

void GraphicsBranchItem::setDistanceText(const QString& text) {
    if (distanceText != nullptr) {
        distanceText->setText(text);
    }
}

void GraphicsBranchItem::setWidth(double newWidth) {
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

bool GraphicsBranchItem::isCollapsed() const {
    return collapsed;
}

void GraphicsBranchItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    CHECK(nameText != nullptr, );
    if (isSelected()) {
        qreal radius = settings[BRANCH_THICKNESS].toUInt() + 1.5;
        QRectF rect(-radius, -radius, radius * 2, radius * 2);
        QColor branchColor = qvariant_cast<QColor>(settings[BRANCH_COLOR]);
        painter->setBrush(branchColor);
        if (nameItemSelection == nullptr) {
            nameItemSelection = scene()->addEllipse(rect, QPen(branchColor), QBrush(branchColor));
            nameItemSelection->setParentItem(this);
            nameItemSelection->setFlag(QGraphicsItem::ItemIgnoresTransformations);
            nameItemSelection->setPen(QPen(Qt::gray));
            nameItemSelection->setBrush(QBrush(branchColor));
        } else if (!nameItemSelection->isVisible()) {
            nameItemSelection->setRect(rect);
            nameItemSelection->show();
        }
    } else if (nameItemSelection != nullptr) {
        nameItemSelection->hide();
    }
}

void GraphicsBranchItem::initDistanceText(const QString& text) {
    distanceText = new QGraphicsSimpleTextItem(text);
    distanceText->setFont(TreeViewerUtils::getFont());
    distanceText->setBrush(Qt::darkGray);
    setLabelPositions();
    distanceText->setParentItem(this);
    distanceText->setZValue(1);
}

QRectF GraphicsBranchItem::visibleChildrenBoundingRect(const QTransform& viewTransform) const {
    QRectF childsBoundingRect;
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
            childsBoundingRect |= itemRect;
            graphicsItems.push(graphItem);
        }
    } while (!graphicsItems.isEmpty());
    return childsBoundingRect;
}

bool GraphicsBranchItem::isRoot() const {
    return parentItem() == nullptr;
}

GraphicsBranchItem* GraphicsBranchItem::getRoot() {
    auto root = dynamic_cast<GraphicsBranchItem*>(topLevelItem());
    SAFE_POINT(root != nullptr, "Top level item is not a branch item", root);
    return root;
}

void GraphicsBranchItem::emitBranchCollapsed(GraphicsBranchItem* branch) {
    SAFE_POINT(isRoot(), "Not a root branch!", );
    emit si_branchCollapsed(branch);
}

GraphicsButtonItem* GraphicsBranchItem::getButtonItem() const {
    return buttonItem;
}

GraphicsBranchItem* GraphicsBranchItem::getChildBranch(const GraphicsBranchItem::Side& childBranchSide) const {
    QList<QGraphicsItem*> children = childItems();
    for (QGraphicsItem* childItem : qAsConst(children)) {
        if (auto childBranch = dynamic_cast<GraphicsBranchItem*>(childItem)) {
            if (childBranch->side == childBranchSide) {
                return childBranch;
            }
        }
    }
    return nullptr;
}

QGraphicsSimpleTextItem* GraphicsBranchItem::getDistanceTextItem() const {
    return distanceText;
}

QString GraphicsBranchItem::getDistanceText() const {
    return distanceText == nullptr ? "" : distanceText->text();
}

QGraphicsSimpleTextItem* GraphicsBranchItem::getNameTextItem() const {
    return nameText;
}

double GraphicsBranchItem::getWidth() const {
    return width;
}

double GraphicsBranchItem::getDist() const {
    return distance;
}

void GraphicsBranchItem::setWidthW(double w) {
    width = w;
}

void GraphicsBranchItem::setDist(double d) {
    distance = d;
}

}  // namespace U2
