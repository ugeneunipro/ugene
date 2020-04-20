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

#include "GraphicsButtonItem.h"
#include "GraphicsBranchItem.h"
#include "GraphicsRectangularBranchItem.h"
#include "TreeViewerUtils.h"

#include <U2Core/U2SafePoints.h>
#include <U2Core/PhyTreeObject.h>
#include <QList>

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QPainter>
#include <QPen>

namespace U2 {

const qreal GraphicsButtonItem::radiusMin = 3.0;
const qreal GraphicsButtonItem::radiusMax = 4.0;
const QBrush GraphicsButtonItem::highlightingBrush = QBrush(QColor(170, 170, 230));
const QBrush GraphicsButtonItem::ordinaryBrush = QBrush(Qt::gray);


GraphicsButtonItem::GraphicsButtonItem(double nodeValue)
    : QGraphicsEllipseItem(QRectF(-radiusMin, -radiusMin, 2 * radiusMin, 2 * radiusMin)), isSelected(false), nodeLabel(NULL), scaleFactor(1.0), nodeValue(nodeValue) {
    setPen(QColor(0, 0, 0));
    setBrush(ordinaryBrush);
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::LeftButton);
    setZValue(2);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setToolTip(QObject::tr("Left click to select the branch\nDouble-click to collapse the branch"));

    if(nodeValue >= 0) {
        nodeLabel = new QGraphicsSimpleTextItem(QString::number(nodeValue), this);
        nodeLabel->setFont(TreeViewerUtils::getFont());
        nodeLabel->setBrush(Qt::darkGray);
        QRectF rect = nodeLabel->boundingRect();
        nodeLabel->setPos(GraphicsBranchItem::TextSpace, -rect.height() / 2);
        nodeLabel->setParentItem(this);
        nodeLabel->setFlag(QGraphicsItem::ItemIgnoresTransformations,false);
        nodeLabel->setZValue(1);
    }
}

void GraphicsButtonItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    QTransform worldTransform = painter->worldTransform();
    //worldTransform = worldTransform.scale(1.0 / worldTransform.m11(), 1.0 / worldTransform.m22());
    scaleFactor = 1.0 / worldTransform.m11();
    worldTransform.setMatrix(1.0, 0, 0, 0, 1.0, 0, worldTransform.m31(), worldTransform.m32(), worldTransform.m33());
    painter->setWorldTransform(worldTransform);
    QGraphicsEllipseItem::paint(painter, option, widget);
}

QRectF GraphicsButtonItem::boundingRect() const {
    QRectF resultRect = QGraphicsEllipseItem::boundingRect();
    qreal delta = (resultRect.width() / scaleFactor - resultRect.width()) / 2;
    return resultRect.adjusted(delta, delta, delta, delta);
}

const QGraphicsSimpleTextItem* GraphicsButtonItem::getLabel() const{
    return nodeLabel;
}


void GraphicsButtonItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
    uiLog.trace("Tree button pressed");

    bool shiftPressed = e->modifiers() & Qt::ShiftModifier;
    bool leftButton = e->button() == Qt::LeftButton;
    GraphicsBranchItem *p = dynamic_cast<GraphicsBranchItem*>(parentItem());
    if (leftButton && p!=NULL) {
        bool newSelection = true;
        if (shiftPressed) {
            newSelection = !isSelected;
        }
        p->setSelectedRecurs(newSelection, true);

        e->accept();
        update();
    }
}


void GraphicsButtonItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *e) {
    uiLog.trace("Tree button double-clicked");
    collapse();
    QAbstractGraphicsShapeItem::mouseDoubleClickEvent(e);
}

void GraphicsButtonItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    if(isSelected) {
        return;
    }
    QGraphicsItem::hoverEnterEvent(event);
    setHighlighting(true);
}
void GraphicsButtonItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    if(isSelected) {
        return;
    }
    QGraphicsItem::hoverLeaveEvent(event);
    setHighlighting(false);
}

void GraphicsButtonItem::setSelected(bool selected){
    isSelected = selected;
    if(selected) {
        setHighlighting(true);
    }
    else {
        setHighlighting(false);
    }
}

void GraphicsButtonItem::setHighlighting(bool enabled) {
    if(enabled) {
        setRect(QRectF(-radiusMax, -radiusMax, 2 * radiusMax, 2 * radiusMax));
        setBrush(highlightingBrush);
    }
    else {
        setRect(QRectF(-radiusMin, -radiusMin, 2 * radiusMin, 2 * radiusMin));
        setBrush(ordinaryBrush);
    }
    update();
}

void GraphicsButtonItem::collapse() {
    GraphicsBranchItem *branch = dynamic_cast<GraphicsBranchItem*>(parentItem());
    SAFE_POINT(NULL != branch, "Collapsing is impossible because button has not parent branch",);

    GraphicsBranchItem *parentBranch = dynamic_cast<GraphicsBranchItem*>(branch->parentItem());
    if(NULL != parentBranch) {
        branch->collapse();
    }
}

void GraphicsButtonItem::swapSiblings() {
    uiLog.trace("Swapping siblings");

    GraphicsBranchItem *branchItem = dynamic_cast<GraphicsBranchItem*>(parentItem());
    if (!branchItem) {
        return;
    }

    GraphicsRectangularBranchItem *rectBranchItem = dynamic_cast<GraphicsRectangularBranchItem*>(branchItem);
    if(!rectBranchItem){
        if(!branchItem->getCorrespondingItem()){
            return;
        }

        rectBranchItem = dynamic_cast<GraphicsRectangularBranchItem*>(branchItem->getCorrespondingItem());
        if(!rectBranchItem){
            return;
        }
    }

    rectBranchItem->swapSiblings();
}

bool GraphicsButtonItem::isSelectedTop() {
    if (!isSelected) {
        return false;
    }
    GraphicsBranchItem *branchItem = dynamic_cast<GraphicsBranchItem*>(parentItem());
    if (!branchItem) {
        return true;
    }
    GraphicsBranchItem *parentBranchItem = dynamic_cast<GraphicsBranchItem*>(branchItem->parentItem());
    if (!parentBranchItem) {
        return true;
    }
    bool parentBranchSelected = parentBranchItem->isSelected();
    return !parentBranchSelected;
}

bool GraphicsButtonItem::isCollapsed(){
    GraphicsBranchItem *p = dynamic_cast<GraphicsBranchItem*>(parentItem());
    Q_ASSERT(p);
    if (p) {
        return p->isCollapsed();
    }
    return false;

}

void GraphicsButtonItem::rerootTree(PhyTreeObject* treeObject) {
    uiLog.trace("Rerooting of the PhyTree");
    SAFE_POINT(NULL != treeObject, "Null pointer argument 'treeObject' was passed to 'PhyTreeUtils::rerootPhyTree' function",);

    GraphicsBranchItem *branchItem = dynamic_cast<GraphicsBranchItem*>(parentItem());
    CHECK(NULL != branchItem , );

    GraphicsRectangularBranchItem *rectBranchItem = dynamic_cast<GraphicsRectangularBranchItem*>(branchItem);
    if(NULL == rectBranchItem){
        CHECK(branchItem->getCorrespondingItem(), );

        rectBranchItem = dynamic_cast<GraphicsRectangularBranchItem*>(branchItem->getCorrespondingItem());
        CHECK(NULL != rectBranchItem,);
    }

    const PhyBranch* nodeBranch = rectBranchItem->getPhyBranch();
    CHECK(NULL != nodeBranch, );
    PhyNode* newRoot = nodeBranch->node2;
    CHECK(NULL != newRoot, );

    treeObject->rerootPhyTree(newRoot);
}

void GraphicsButtonItem::updateSettings(const OptionsMap& settings) {
    CHECK(NULL != nodeLabel,);
    QFont newFont = qvariant_cast<QFont>(settings[LABEL_FONT]);
    nodeLabel->setFont(newFont);
    QColor labelsColor = qvariant_cast<QColor>(settings[LABEL_COLOR]);
    nodeLabel->setBrush(labelsColor);
    bool showNodeLabels = settings[SHOW_NODE_LABELS].toBool();
    nodeLabel->setVisible(showNodeLabels);
}

bool GraphicsButtonItem::getIsSelected() const {
    return isSelected;
}

}//namespace
