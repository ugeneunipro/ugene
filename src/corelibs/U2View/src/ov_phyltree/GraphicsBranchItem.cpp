/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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
#include "GraphicsButtonItem.h"
#include "TreeViewerUtils.h"

#include <QPainter>
#include <QStack>
#include <U2Core/U2SafePoints.h>
#include <QGraphicsScene>
#include <QEvent>

namespace U2 {

const int GraphicsBranchItem::TextSpace = 8;
const int GraphicsBranchItem::SelectedPenWidth = 1;

void GraphicsBranchItem::updateSettings(const OptionsMap& newSettings) {
    settings[BRANCH_COLOR] = newSettings[BRANCH_COLOR];
    settings[BRANCH_THICKNESS] = newSettings[BRANCH_THICKNESS];

    int penWidth = settings[BRANCH_THICKNESS].toUInt();
    if (isSelected()) {
        penWidth += SelectedPenWidth;
    }

    QColor branchColor = qvariant_cast<QColor>(settings[BRANCH_COLOR]);
    QPen currentPen = this->pen();
    currentPen.setColor(branchColor);
    currentPen.setWidth(penWidth);

    this->setPen(currentPen);
}

void GraphicsBranchItem::updateChildSettings(const OptionsMap& newSettings) {
    foreach(QGraphicsItem* graphItem, this->childItems()) {
        GraphicsBranchItem *branchItem = dynamic_cast<GraphicsBranchItem*>(graphItem);
        if (branchItem) {
            branchItem->updateSettings(newSettings);
            branchItem->updateChildSettings(newSettings);
        }
    }
}

void GraphicsBranchItem::updateTextSettings(const QFont& font, const QColor& color){
    QFont prevFont;
    if(distanceText){
        prevFont = distanceText->font();
        distanceText->setFont(font);
        distanceText->setBrush(color);
    }
    if(nameText){
        prevFont = nameText->font();
        nameText->setFont(font);
        nameText->setBrush(color);
    }
    if(font != prevFont) {
        setLabelPositions();
    }
}

const OptionsMap& GraphicsBranchItem::getSettings() const {
    return settings;
}

void GraphicsBranchItem::collapse() {
    collapsed = !collapsed;
    QList<QGraphicsItem*> items = childItems();
    if (collapsed) {
        for (int i = 0, s = items.size(); i < s; ++i) {
            if (dynamic_cast<GraphicsBranchItem*>(items[i])) {
                items[i]->hide();
            }
        }

        int penWidth = settings[BRANCH_THICKNESS].toUInt();
        if (isSelected()) {
            penWidth += SelectedPenWidth;
        }

        QColor branchColor = qvariant_cast<QColor>(settings[BRANCH_COLOR]);
        QPen pen1(branchColor);
        pen1.setWidth(penWidth);
        pen1.setCosmetic(true);
        QGraphicsRectItem *r = new QGraphicsRectItem(0, -4, 16, 8, this);
        r->setPen(pen1);
    } else {
        for (int i = 0, s = items.size(); i < s; ++i) {
            if (dynamic_cast<QGraphicsRectItem*>(items[i])) {
                items[i]->setParentItem(NULL);
                scene()->removeItem(items[i]);
            } else {
                if (items[i] != getDistanceText() && items[i] != getNameText()) {
                    items[i]->show();
                }
            }
        }
        setSelectedRecurs(true,true);
    }
}

void GraphicsBranchItem::setSelectedRecurs(bool sel, bool selectChilds) {
    if (!selectChilds) {
        setSelected(sel);
        scene()->update();
        return;
    }

    //Set selected for child items
    QStack<GraphicsBranchItem*> graphicsItems;
    graphicsItems.push(this);
    do {
        GraphicsBranchItem* branchItem = graphicsItems.pop();
        branchItem->setSelected(sel);

        foreach(QGraphicsItem* graphItem, branchItem->childItems()) {
            GraphicsBranchItem *childItem = dynamic_cast<GraphicsBranchItem*>(graphItem);
            if(childItem) {
                graphicsItems.push(childItem);
            }
        }
    } while(!graphicsItems.isEmpty());

    scene()->update();
}

void GraphicsBranchItem::setSelected(bool sel) {
    if (buttonItem) {
        buttonItem->setSelected(sel);
    }

    int penWidth = settings[BRANCH_THICKNESS].toUInt();
    if (sel) {
        penWidth += SelectedPenWidth;
    }
    QPen currentPen = this->pen();
    currentPen.setWidth(penWidth);
    this->setPen(currentPen);

    QAbstractGraphicsShapeItem::setSelected(sel);
}

void GraphicsBranchItem::initText(qreal d) {
    QString str = QString::number(d, 'f', 3);
    int i = str.length() - 1;
    for (; i >= 0 && str[i] == '0'; --i) ;
    if (str[i] == '.')
        --i;
    str.truncate(i + 1);

    //it doesn't show zeroes by default. only in cladogramm mode
    if(str == "0"){
        str = "";
    }
    //test
    distanceText = new QGraphicsSimpleTextItem(str);
    distanceText->setFont(TreeViewerUtils::getFont());
    distanceText->setBrush(Qt::darkGray);
    setLabelPositions();
    distanceText->setParentItem(this);
    distanceText->setZValue(1);
}

GraphicsBranchItem::GraphicsBranchItem(bool withButton, double nodeValue)
: correspondingItem(NULL),
  buttonItem(NULL),
  branchLength(0),
  nameItemSelection(NULL),
  distanceText(NULL),
  nameText(NULL),
  width(0),
  dist(0),
  collapsed(false),
  lengthCoef(1)
{
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

GraphicsBranchItem::GraphicsBranchItem(const QString& name)
: correspondingItem(NULL),
  buttonItem(NULL),
  branchLength(0),
  nameItemSelection(NULL),
  distanceText(NULL),
  collapsed(false),
  lengthCoef(1)
{
    settings[BRANCH_THICKNESS] = 1;
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(false);
    setAcceptedMouseButtons(Qt::NoButton);

    QColor branchColor = qvariant_cast<QColor>(settings[BRANCH_COLOR]);
    QPen pen1(branchColor);
    pen1.setStyle(Qt::DotLine);
    pen1.setCosmetic(true);
    setPen(pen1);

    width = 0;
    dist = 0;

    nameText = new QGraphicsSimpleTextItem(name);
    nameText->setFont(TreeViewerUtils::getFont());
    nameText->setBrush(Qt::darkGray);
    setLabelPositions();
    nameText->setParentItem(this);
    nameText->setZValue(1);
}

GraphicsBranchItem::GraphicsBranchItem(qreal d, bool withButton, double nodeValue)
: correspondingItem(NULL),
  buttonItem(NULL),
  branchLength(0),
  nameItemSelection(NULL),
  distanceText(NULL),
  nameText(NULL),
  width(0),
  dist(0),
  collapsed(false),
  lengthCoef(1)
{
    settings[BRANCH_THICKNESS] = 1;
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(false);
    setAcceptedMouseButtons(Qt::NoButton);

    if (withButton) {
        buttonItem = new GraphicsButtonItem(nodeValue);
        buttonItem->setParentItem(this);
    }

    initText(d);
    QColor branchColor = qvariant_cast<QColor>(settings[BRANCH_COLOR]);
    QPen pen1(branchColor);
    pen1.setCosmetic(true);
    if (d < 0) {
        pen1.setStyle(Qt::DashLine);
    }
    setPen(pen1);
    setBrush(branchColor);
}

qreal GraphicsBranchItem::getNodeLabel() const {
    if(buttonItem != NULL) {
        return buttonItem->getNodeValue();
    } else {
        return -1;
    }
}


void GraphicsBranchItem::setLabelPositions() {
    if(nameText != NULL) {
        QRectF rect = nameText->boundingRect();
        nameText->setPos(GraphicsBranchItem::TextSpace, -rect.height() / 2);
    }
    if(distanceText != NULL) {
        QRectF rect = distanceText->boundingRect();
        distanceText->setPos(-rect.width() / 2 - width / 2, 0);
    }
}

void GraphicsBranchItem::setDistanceText(const QString& text){
    if(distanceText){
        distanceText->setText(text);
    }
}

void GraphicsBranchItem::setWidth(qreal w) {
    if (width == w) {
        return;
    }

    setPos(pos().x() - width + w, pos().y());
    setLabelPositions();
    if (getDistanceText() != NULL) {
        QPointF pos = getDistanceText()->pos();
        getDistanceText()->setPos(pos.x() + (width - w) * 0.5, pos.y());
    }

    prepareGeometryChange();
    width = w;
}

bool GraphicsBranchItem::isCollapsed() const{
    return collapsed;
}

void GraphicsBranchItem::paint(QPainter* painter,const QStyleOptionGraphicsItem*, QWidget*) {
    CHECK(NULL != nameText,);
    if(isSelected()) {
        qreal radius = settings[BRANCH_THICKNESS].toUInt() + 1.5;
        QRectF rect(-radius, -radius, radius*2, radius*2);
        QColor branchColor = qvariant_cast<QColor>(settings[BRANCH_COLOR]);
        painter->setBrush(branchColor);
        if(NULL == nameItemSelection) {
            nameItemSelection = scene()->addEllipse(rect, QPen(branchColor), QBrush(branchColor));
            nameItemSelection->setParentItem(this);
            nameItemSelection->setFlag(QGraphicsItem::ItemIgnoresTransformations);
            nameItemSelection->setPen(QPen(Qt::gray));
            nameItemSelection->setBrush(QBrush(branchColor));
        }
        else if (!nameItemSelection->isVisible()) {
            nameItemSelection->setRect(rect);
            nameItemSelection->show();
        }
    }
    else {
        if(NULL != nameItemSelection) {
            nameItemSelection->hide();
        }
    }
}

QRectF GraphicsBranchItem::visibleChildrenBoundingRect (const QTransform& viewTransform) const {
    QRectF childsBoundingRect;
    QStack<const QGraphicsItem*> graphicsItems;
    graphicsItems.push(this);

    QTransform invertedTransform = viewTransform.inverted();
    do {
        const QGraphicsItem* branchItem = graphicsItems.pop();

        QList<QGraphicsItem*> items = branchItem->childItems();

        foreach(QGraphicsItem* graphItem, items) {
            if(!graphItem->isVisible()) {
                continue;
            }
            QRectF itemRect = graphItem->sceneBoundingRect();
            if(graphItem->flags().testFlag(QGraphicsItem::ItemIgnoresTransformations)) {
                QRectF transformedRect = invertedTransform.mapRect(itemRect);
                itemRect.setWidth(transformedRect.width());
            }
            childsBoundingRect |= itemRect;
            graphicsItems.push(graphItem);
        }
    } while(!graphicsItems.isEmpty());
    return childsBoundingRect;
}

} //namespace
