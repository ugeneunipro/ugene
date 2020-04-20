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

#include "QueryViewItems.h"
#include "QueryViewController.h"
#include "QueryPalette.h"

#include <U2Core/global.h>
#include <U2Core/Log.h>

#include <U2Core/QVariantUtils.h>

#include <QPainter>
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsView>
#include <QInputDialog>
#include <QMenu>
#include <QTextDocument>

#define ANNOTATION_MIN_SIZE GRID_STEP
#define ANNOTATION_MAX_SIZE 4 * GRID_STEP
#define MARGIN 4
#define ARR_W 15

namespace U2 {

/************************************************************************/
/* Annotation Item                                                      */
/************************************************************************/

inline qreal round(qreal val, int step) {
    if (0 > val) {
        step *= -1;
    }
    int tmp = int(val) + step /2;
    tmp -= tmp % step;
    return qreal(tmp);
}

QDElement::QDElement(QDSchemeUnit* _unit)
: highlighted(false), unit(_unit), font(QFont()), bound(0,0,3*ANNOTATION_MIN_SIZE,ANNOTATION_MIN_SIZE),
dragging(false), extendedHeight(ANNOTATION_MIN_SIZE), itemResizeFlags(0) {
    setFlag(QGraphicsItem::ItemIsSelectable, true);
#if (QT_VERSION >= QT_VERSION_CHECK(4, 6, 0))
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
#endif
    setAcceptHoverEvents(true);
    setZValue(1);

    doc = new QTextDocument(this);

    QDParameters* params = unit->getActor()->getParameters();
    connect(params, SIGNAL(si_modified()), SLOT(sl_refresh()));
    connect(unit->getActor(), SIGNAL(si_strandChanged(QDStrandOption)), SLOT(sl_refresh()));

    itemDescription = new QDElementDescription(this);
    itemDescription->setDocument(doc);

    const QString& s = getHeaderString();
    const QFont& f = itemDescription->font();
    QFontMetrics fm(f);
    if (getActor()->hasAnyDirection()) {
        bound.setWidth(fm.width(s) + 2*ARR_W);
    } else {
        bound.setWidth(fm.width(s) + ARR_W);
    }
}

void QDElement::updateDescription() {
    if (getActor()->hasAnyDirection()) {
        itemDescription->setTextWidth(bound.width()-2*ARR_W);
    } else {
        itemDescription->setTextWidth(bound.width()-ARR_W);
    }
    itemDescription->setHeight(bound.height()-MARGIN);
    itemDescription->document()->setPageSize(itemDescription->boundingRect().size());
}

void QDElement::rememberSize() {
    QueryScene* qs = qobject_cast<QueryScene*>(scene());
    assert(qs);
    if (!qs->showActorDesc()) {
        extendedHeight = bound.height();
    }
}

void QDElement::adaptSize() {
    QueryScene* qs = qobject_cast<QueryScene*>(scene());
    assert(qs);
    prepareGeometryChange();
    if (qs->showActorDesc()) {
        qreal raY = scenePos().y()+bound.height();
        QRectF requiredArea(QPointF(scenePos().x(), raY), QSizeF(bound.width(), extendedHeight-bound.height()));
        int requiredRowsNum = requiredArea.height() / GRID_STEP;
        for (int i=0; i< requiredRowsNum; i++) {
            QRectF reqRowFragment(requiredArea.left(), requiredArea.top() + i*GRID_STEP,
                requiredArea.width(), GRID_STEP);
            QPainterPath rowPath;
            rowPath.addRect(reqRowFragment);
            QList<QGraphicsItem*> containedItems = qs->items(rowPath, Qt::IntersectsItemShape);
            QList<QGraphicsItem*> containedUnits;
            foreach(QGraphicsItem* it, containedItems) {
                if (it->type()==QDElementType) {
                    containedUnits.append(it);
                }
            }
            if (containedUnits.size()>0) {
                int insertAt = qs->getRow(this) + i +1;
                qs->insertRow(insertAt);
            }
        }
        bound.setHeight(extendedHeight);
    } else {
        bound.setHeight(ANNOTATION_MIN_SIZE);
    }

    doc->setPageSize(bound.size() - QSizeF(MARGIN*2,MARGIN*2));
    updateDescription();

    if (bound.bottom() + scenePos().y()<qs->annotationsArea().bottom()) {
        qs->sl_adaptRowsNumber();
    } else if(bound.bottom() + scenePos().y()>qs->annotationsArea().bottom()) {
        int prevRowsNum = qs->getRowsNumber();
        qreal bottomEdge = scenePos().y() + boundingRect().height();
        int reqRowNum = (bottomEdge - qs->annotationsArea().top())/GRID_STEP;
        int rowNum = qMax(prevRowsNum, reqRowNum);
        qs->setRowsNumber(rowNum);
    }
}

void QDElement::loadState(QDElementStatement* el) {
    assert(el->getType()==Element);
    const QString& geometryStr = el->getAttribute(QDElementStatement::GEOMETRY_ATTR_NAME);
    QStringList attrs = geometryStr.split(',');
    const QString& xPosStr = attrs.at(0);
    const QString& yPosStr = attrs.at(1);
    QPointF pos(xPosStr.toInt(), yPosStr.toInt());
    assert(!pos.isNull());
    const QString& wS = attrs.at(2);
    const QString& hS = attrs.at(3);
    QSizeF size(wS.toInt(), hS.toInt());
    bound.setSize(size);
    setPos(pos);
    doc->setPageSize(bound.size() - QSizeF(MARGIN*2,MARGIN*2));
    const QString& eHS = attrs.at(4);
    extendedHeight = eHS.toInt();
}

void QDElement::saveState(QDElementStatement* el) const {
    assert(el->getType()==Element);
    QGraphicsScene* s = scene();
    QueryScene* qs = qobject_cast<QueryScene*>(s);
    assert(qs);
    qreal extHeight = extendedHeight;
    if (qs->showActorDesc()) {
        extHeight = bound.height();
    }
    QString geomStr = QString("%1,%2,%3,%4,%5")
        .arg(scenePos().x())
        .arg(scenePos().y())
        .arg(boundingRect().width())
        .arg(boundingRect().height())
        .arg(extHeight);
    el->setAttribute(QDElementStatement::GEOMETRY_ATTR_NAME, geomStr);
}

void QDElement::sl_refresh() {
    QString baseHtml = "<center>" + getHeaderString();
    QueryScene* qs = qobject_cast<QueryScene*>(scene());
    if (qs && !qs->showActorDesc()) {
        doc->setHtml(baseHtml);
    }
    else {
        QString infoStr = unit->getActor()->getText();
        doc->setHtml(QString("%1<hr>%2")
            .arg(baseHtml)
            .arg(infoStr));
    }
    update();

    if ( getActor()->hasAnyDirection() || getActor()->hasBackwardDirection() ) {
        itemDescription->setPos(ARR_W,0);
    } else {
        itemDescription->setPos(0,0);
    }
    updateDescription();
}

QString QDElement::getHeaderString() const {
    QString res;
    QString annotateAsStr = unit->getActor()->annotateAs();
    annotateAsStr.replace('<', "&lt;");
    annotateAsStr.replace('>', "&gt;");
    QueryScene* qs = qobject_cast<QueryScene*>(scene());
    QDActorParameters* params = unit->getActor()->getParameters();
    if (getActor()->getSchemeUnits().size()==1) {
        res = QString("<b>%1 (\"%3\")</b>")
            .arg(params->getLabel())
            .arg(annotateAsStr);
    }
    else {
        res = QString("<b>%1.%2 (\"%3\")</b>")
            .arg(params->getLabel())
            .arg(unit->getId())
            .arg(annotateAsStr);
    }
    if (qs && qs->showActorOrder()) {
        int serialNumber = qs->getScheme()->getActors().indexOf(getActor()) + 1;
        res+=QString("<b> Order: %1</b>").arg(serialNumber);
    }
    return res;
}

bool QDElement::isLinkedWith(QDElement* /*other*/, QDDistanceType /*kind*/) {
    /*foreach(Footnote* linkIt, links) {
        if(other->links.contains(linkIt) && linkIt->kind()==kind) {
            return true;
        }
    }*/
    return false;
}

QList<Footnote*> QDElement::getOutcomeFootnotes() const {
    QList<Footnote*> res;
    foreach(Footnote* fn, links) {
        if (fn->getSrc()==this) {
            res.append(fn);
        }
    }
    return res;
}

QList<Footnote*> QDElement::getIncomeFootnotes() const {
    QList<Footnote*> res;
    foreach(Footnote* fn, links) {
        if (fn->getDst()==this) {
            res.append(fn);
        }
    }
    return res;
}

#define CORNER_DIAM 6
void QDElement::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);
    QPen pen;
    if(isSelected()) {
        pen.setStyle(Qt::DashLine);
        painter->setPen(pen);
    }
    if(highlighted) {
        pen.setWidth(2);
        painter->setPen(pen);
    }

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(getActor()->defaultColor());

    qreal w = boundingRect().width();
    qreal h = boundingRect().height();
    QPainterPath path;

    if (!getActor()->hasAnyDirection()) {
        path.moveTo(QPointF(CORNER_DIAM/2.0, 0));
        path.lineTo(w - ARR_W, 0);
        path.lineTo(w, h/2);
        path.lineTo(w - ARR_W, h);
        path.lineTo(CORNER_DIAM/2.0, h);
        path.arcTo(0, h-CORNER_DIAM, CORNER_DIAM, CORNER_DIAM, -90, -90);
        path.lineTo(0, CORNER_DIAM/2);
        path.arcTo(0, 0, CORNER_DIAM, CORNER_DIAM, 180, -90);
        if (getActor()->hasBackwardDirection()) {
            painter->rotate(180);
            painter->translate(-w,-h);
        }
    } else {
        path.moveTo(0, h/2);
        path.lineTo(ARR_W, 0);
        path.lineTo(w-ARR_W, 0);
        path.lineTo(w, h/2);
        path.lineTo(w-ARR_W, h);
        path.lineTo(ARR_W, h);
        path.lineTo(0, h/2);
        path.lineTo(ARR_W, 0);
    }

    painter->fillPath(path, painter->brush());
    painter->drawPath(path);
}

QPointF QDElement::getRightConnector() {
    return mapToScene(QPointF(boundingRect().right(), (boundingRect().top() + boundingRect().bottom())/2));
}

QPointF QDElement::getLeftConnector() {
    return mapToScene(QPointF(boundingRect().left(), (boundingRect().top() + boundingRect().bottom())/2));
}

void QDElement::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if(event->buttons() & Qt::LeftButton) {
        foreach(Footnote* link, links) {
            link->draging = true;
        }

        if (!dragging) {
            dragPoint = event->pos();
            dragging = true;
        }

        QPointF newPos = scenePos();
        const QPointF& mousePos = event->pos();
        const QPointF& p = mousePos - dragPoint;
        newPos.rx() += p.x();
        if (qAbs(p.y()) >= GRID_STEP/2) {
            newPos.ry() += p.y();
        }
        setPos(newPos);
    } else {
        QGraphicsItem::mouseMoveEvent(event);
    }
}

void QDElement::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    foreach(Footnote* link, links) {
        link->draging = false;
        link->update();
    }
    dragging = false;
    QGraphicsItem::mouseReleaseEvent(event);
}

void QDElement::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
    QueryScene* qs = qobject_cast<QueryScene*>(scene());
    assert(qs);
    QDScheme* scheme = qs->getScheme();
    const QList<QDActor*>& actors = scheme->getActors();
    int serialNum = actors.indexOf(getActor());

    QMenu menu;
    QMenu* orderMenu = new QMenu(tr("Set order"), &menu);
    for (int i=0; i<actors.size(); i++) {
        QDActor* actor = actors.at(i);
        QAction* a = orderMenu->addAction(QString("%1 %2")
            .arg(i+1)
            .arg(actor->getParameters()->getLabel())
            );
        a->setCheckable(true);
        a->setChecked(false);
        if (i==serialNum) {
            a->setChecked(true);
        }
        a->setData(qVariantFromValue(i));
    }

    QAction* removeFromGroupAction = NULL;
    QAction* addToGroupAction = NULL;
    if (!scheme->getActorGroups().isEmpty()) {
        if (!scheme->getActorGroup(getActor()).isEmpty()) {
            removeFromGroupAction = menu.addAction(tr("Remove from group"));
        } else {
            addToGroupAction = menu.addAction(tr("Add to group"));
        }
    }
    menu.addSeparator();
    menu.addMenu(orderMenu);
    menu.addSeparator();
    QAction* upAction = menu.addAction(tr("Up"));
    QAction* downAction = menu.addAction(tr("Down"));
    QAction* action = menu.exec(event->screenPos());

    if (!action) {
        return;
    }

    if (action==addToGroupAction) {
        bool ok;

        QString sel = QInputDialog::getItem(NULL,
            tr("Add '%1' to group").arg(getActor()->getParameters()->getLabel()),
            tr("Group:"), scheme->getActorGroups(), 0, false, &ok);

        if(!ok) {
            return;
        }

        scheme->addActorToGroup(getActor(), sel);
        qs->getViewController()->switchToGroupsTab();
        return;
    }

    if (action==removeFromGroupAction) {
        scheme->removeActorFromGroup(getActor());
        qs->getViewController()->switchToGroupsTab();
        return;
    }

    int newSerialNum = serialNum;
    if (action==upAction) {
        newSerialNum = serialNum + 1;
    } else if (action==downAction) {
        newSerialNum = serialNum-1;
    } else {
        newSerialNum = action->data().toInt();
    }

    int from = 0;
    int to = 0;
    if (serialNum < newSerialNum) {
        from = qMax(serialNum,0);
        to = qMin(newSerialNum, actors.size()-1);
    } else {
        from = qMax(newSerialNum,0);
        to = qMin(serialNum, actors.size()-1);
    }
    scheme->setOrder(getActor(), newSerialNum);
    for(int i=from; i<=to; i++) {
        QDActor* a = actors.at(i);
        const QList<QDSchemeUnit*>& suList = a->getSchemeUnits();
        foreach(QDSchemeUnit* su, suList) {
            foreach(QGraphicsItem* it, qs->getElements()) {
                QDElement* uv = qgraphicsitem_cast<QDElement*>(it);
                assert(uv);
                if (uv->getSchemeUnit()==su) {
                    uv->sl_refresh();
                }
            }
        }
    }
}

//resize processing
#define EDGE_WIDTH 4
bool QDElement::sceneEvent(QEvent *event) {
    switch(event->type()) {
        case QEvent::GraphicsSceneHoverEnter:
        case QEvent::GraphicsSceneHoverMove:
        {
            itemResizeFlags = 0;
            QGraphicsSceneHoverEvent* he = static_cast<QGraphicsSceneHoverEvent *>(event);
            QPointF p = he->pos();
            int dxRight = qAbs(boundingRect().right() - p.x());
            int dxLeft = qAbs(boundingRect().left() - p.x());
            int dyBottom = qAbs(boundingRect().bottom() - p.y());
            int dyTop = qAbs(boundingRect().top() - p.y());
            if (dxRight < EDGE_WIDTH) {
                itemResizeFlags |= ResizeRight;
                setCursor(Qt::SizeHorCursor);
            } else if(dxLeft < EDGE_WIDTH) {
                itemResizeFlags |= ResizeLeft;
                setCursor(Qt::SizeHorCursor);
            }
            if (dyBottom < EDGE_WIDTH) {
                itemResizeFlags |= ResizeBottom;
                setCursor(Qt::SizeVerCursor);
            } else if(dyTop < EDGE_WIDTH) {
                itemResizeFlags |= ResizeTop;
                setCursor(Qt::SizeVerCursor);
            }

            if((itemResizeFlags&ResizeBottom&&itemResizeFlags&ResizeRight)||
                (itemResizeFlags&ResizeTop&&itemResizeFlags&ResizeLeft)) {
                setCursor(Qt::SizeFDiagCursor);
            } else if((itemResizeFlags&ResizeTop&&itemResizeFlags&ResizeRight)||
                (itemResizeFlags&ResizeBottom&&itemResizeFlags&ResizeLeft)) {
                setCursor(Qt::SizeBDiagCursor);
            }
            if(!itemResizeFlags && cursor().shape()!=Qt::PointingHandCursor) {
                unsetCursor();
            }
            break;
        }
        case QEvent::GraphicsSceneHoverLeave:
        case QEvent::GraphicsSceneMouseRelease:
            itemResizeFlags = 0;
            break;
        case QEvent::GraphicsSceneMouseMove:
        {
            if(itemResizeFlags) {
                QueryScene* qs = qobject_cast<QueryScene*>(scene());
                QGraphicsSceneMouseEvent* me = static_cast<QGraphicsSceneMouseEvent *>(event);
                if(me->buttons() & Qt::LeftButton) {
                    QPointF p = me->pos();
                    p.setY(round(p.y(), GRID_STEP));

                    QRectF newBound(bound);

                    if(itemResizeFlags & ResizeRight) {
                        newBound.setRight(p.x());
                    } else if (itemResizeFlags & ResizeLeft && me->scenePos().x() > 0) {
                        newBound.setWidth(newBound.width() - p.x());
                    }
                    if(itemResizeFlags & ResizeTop && (0 <= (scenePos().y() - GRID_STEP) || 0 <= p.y())) {
                        newBound.setHeight(newBound.height() - p.y() + newBound.top());
                    } else if(itemResizeFlags & ResizeBottom) {
                        newBound.setBottom(p.y());
                    }

                    //keep minimum size
                    if(newBound.width()<ANNOTATION_MIN_SIZE*2
                        || newBound.height()<ANNOTATION_MIN_SIZE) {
                        break;
                    }
                    //remove empty rows if any
                    if (newBound.bottom() + scenePos().y() < qs->annotationsArea().bottom()) {
                        qs->sl_adaptRowsNumber();
                    }
                    //expand rows area if necessary
                    if(newBound.bottom() + scenePos().y() > qs->annotationsArea().bottom()) {
                        int prevRowsNum = qs->getRowsNumber();
                        qreal bottomEdge = scenePos().y() + boundingRect().height();
                        int reqRowNum = (bottomEdge - qs->annotationsArea().top())/GRID_STEP;
                        int rowNum = qMax(prevRowsNum, reqRowNum);
                        qs->setRowsNumber(rowNum);
                    }

                    //find new position for left|top resize
                    QPointF newPos = scenePos();
                    if (itemResizeFlags&ResizeTop) {
                        qreal deltaY = newBound.height() - bound.height();
                        newPos.setY(newPos.y() - deltaY);
                    }
                    if (itemResizeFlags&ResizeLeft) {
                        newPos.setX(newPos.x() + p.x());
                    }

                    //check for collisions
                    QRectF rect = newBound;
                    rect.moveTopLeft(newPos);
                    QPainterPath path;
                    path.addRect(rect);
                    QList<QGraphicsItem*> items = scene()->items(path, Qt::IntersectsItemShape);
                    items.removeAll(this);
                    foreach(QGraphicsItem* item, items) {
                        if(item->type()!=QDElementType) {
                            items.removeAll(item);
                        }
                    }
                    if(!items.isEmpty()) {
                        break;
                    }
                    //check if affected footnotes have positive length
                    if ( itemResizeFlags & ResizeRight ) {
                        foreach(Footnote* fn, links) {
                            if (fn->getDst()==this) {
                                if (fn->getDistType()==S2E || fn->getDistType()==E2E) {
                                    if (newPos.x()+newBound.width()-fn->getSrcPoint().x()<=0) {
                                        return true;
                                    }
                                }
                            } else {
                                if (fn->getDistType()==E2S || fn->getDistType()==E2E) {
                                    if (fn->getDstPoint().x()-newPos.x()-newBound.width()<=0) {
                                        return true;
                                    }
                                }
                            }
                        }
                    }
                    if ( itemResizeFlags & ResizeLeft ) {
                        foreach(Footnote* fn, links) {
                            if (fn->getDst()==this) {
                                if (fn->getDistType()==S2S || fn->getDistType()==E2S) {
                                    if (newPos.x() - fn->getSrcPoint().x()<=0) {
                                        return true;
                                    }
                                }
                            } else {
                                if (fn->getDistType()==S2S || fn->getDistType()==S2E) {
                                    if (fn->getDstPoint().x()-newPos.x()<=0) {
                                        return true;
                                    }
                                }
                            }
                        }
                    }

                    bound.setRect(newBound.x(),newBound.y(),newBound.width(),newBound.height());

                    if (itemResizeFlags & (ResizeTop|ResizeLeft)) {
                        if (newPos!=scenePos()) {
                            setPos(newPos);
                        }
                    }

                    //update
                    updateDescription();
                    updateFootnotes();
                    qs->setModified(true);
                    qs->update();
                }
            }
        }
        break;
        default:;
    }

    if(itemResizeFlags) {
        return true;
    } else {
        return QGraphicsItem::sceneEvent(event);
    }
}

QVariant QDElement::itemChange( GraphicsItemChange change, const QVariant & value ) {
    switch(change) {
        case ItemPositionChange:
            {
                //value is the new position
                QPointF newPos = value.toPointF();
                QueryScene* qs = qobject_cast<QueryScene*>(scene());
                if (qs==NULL) {
                    return newPos;
                }
                // Adjust position for item to fit in row
                qreal start = qs->annotationsArea().top();
                newPos.setY(round(newPos.y() - start, GRID_STEP) + start);
                QRectF rect = qs->annotationsArea();
                rect.setHeight(rect.height()-boundingRect().height());
                rect.setWidth(rect.width()-boundingRect().width());
                if (!rect.contains(newPos)) {
                    if (newPos.y()>rect.bottom()) {
                        int prevRowsNum = qs->getRowsNumber();
                        qreal bottomEdge = newPos.y() + boundingRect().height();
                        int reqRowNum = (bottomEdge - qs->annotationsArea().top())/GRID_STEP;
                        int rowNum = qMax(prevRowsNum, reqRowNum);
                        qs->setRowsNumber(rowNum);
                    }
                    // Keep the item inside the annotation area
                    newPos.setX(qBound(rect.left(), newPos.x(), rect.left() + QueryScene::MAX_SCENE_SIZE.width()));
                    newPos.setY(qMax(newPos.y(), rect.top()));
                }

                //prevent collision
                QRectF itemRect = boundingRect();
                const QPointF& topLeft = mapToScene(itemRect.topLeft());
                itemRect.moveTopLeft(topLeft);
                itemRect.translate(newPos - scenePos());
                QPainterPath path;
                path.addRect(itemRect);
                QList<QGraphicsItem*> items = scene()->items(path, Qt::IntersectsItemShape);
                items.removeAll(this);
                foreach(QGraphicsItem* item, items) {
                    if(item->type()!=QDElementType) {
                        items.removeAll(item);
                    }
                }
                const QPointF& oldPos = scenePos();
                if(!items.isEmpty()) {
                    return oldPos;
                }
                //check if links have positive length
                foreach(Footnote* fn, links) {
                    if(fn->from==this) {
                        //future connector position
                        QPointF left = fn->getSrcPoint();
                        left+=newPos-scenePos();
                        QPointF right = fn->getDstPoint();
                        if(right.x()-left.x()<=0) {
                            return oldPos;
                        }
                    } else {
                        QPointF right = fn->getDstPoint();
                        right+=newPos-scenePos();
                        QPointF left = fn->getSrcPoint();
                        if(right.x()-left.x()<=0) {
                            return oldPos;
                        }
                    }
                }
                return newPos;
            }
            break;
        case ItemPositionHasChanged:
            {
                QueryScene* qs = qobject_cast<QueryScene*>(scene());
                if (qs==NULL) {
                    return QGraphicsItem::itemChange(change, value);
                }
                qs->sl_adaptRowsNumber();

                QRectF rect = qs->sceneRect();
                qreal rightEdge = mapRectToScene(boundingRect()).right();
                qreal min = rect.left() + QueryScene::DEFAULT_SCENE_SIZE.width();
                qreal max = rect.left() + QueryScene::MAX_SCENE_SIZE.width();
                rightEdge = qBound(min, rightEdge, max);
                if (rightEdge > rect.right()) {
                    rect.setRight(rightEdge);
                    qs->setSceneRect(rect);
                }

                updateFootnotes();
                qs->setModified(true);
            }
            break;
        case ItemSceneChange:
            if((value.value<QGraphicsScene*>())==NULL) {
                foreach(Footnote* fn, links) {
                    scene()->removeItem(fn);
                    delete fn;
                }
            }
            break;
        case ItemSceneHasChanged:
            if((value.value<QGraphicsScene*>())!=NULL) {
                sl_refresh();
                adaptSize();
                QueryScene* qs = qobject_cast<QueryScene*>(scene());
                QueryViewController* view = qs->getViewController();
                if (view) {
                    connect(itemDescription, SIGNAL(linkActivated(const QString&)), view, SLOT(sl_selectEditorCell(const QString&)));
                    connect(itemDescription, SIGNAL(linkHovered(const QString&)), SLOT(sl_onHoverLink(const QString&)));
                }
            }
            break;
        default:
            break;
    }
    return QGraphicsItem::itemChange(change, value);
}

void QDElement::updateFootnotes() {
    QueryScene* qs = qobject_cast<QueryScene*>(scene());
    QGraphicsView* view = qs->views().at(0);
    assert(view);
    QGraphicsView::ViewportUpdateMode mode = view->viewportUpdateMode();
    view->setViewportUpdateMode(QGraphicsView::NoViewportUpdate);
    foreach(Footnote* fn, links) {
        fn->updatePos();
    }
    view->setViewportUpdateMode(mode);
}

void QDElement::sl_onHoverLink(const QString &link) {
    if (link.isEmpty()) {
        unsetCursor();
    } else {
        setCursor(Qt::PointingHandCursor);
    }
}

/************************************************************************/
/* QDElementDescription                                                 */
/************************************************************************/

QDElementDescription::QDElementDescription(QGraphicsItem* parent/* =NULL */) : QGraphicsTextItem(parent) {
    setAcceptHoverEvents(true);
    setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::LinksAccessibleByKeyboard);
}

QRectF QDElementDescription::boundingRect() const {
    QRectF bound = QGraphicsTextItem::boundingRect();
    bound.setHeight(height);
    return bound;
}

void QDElementDescription::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QStyleOptionGraphicsItem deselectedOption = *option;
    deselectedOption.state &= ~(QStyle::State_Selected | QStyle::State_HasFocus);
    QGraphicsTextItem::paint(painter, &deselectedOption, widget);
}

// subscribe to mouse release event (link activation in QGraphicsTextItem) by accepting mouse press
void QDElementDescription::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsTextItem::mousePressEvent(event);
    event->accept();
}

bool QDElementDescription::sceneEvent(QEvent *event) {
    bool res = QGraphicsTextItem::sceneEvent(event);
    switch (event->type()) {
        case QEvent::GraphicsSceneHoverEnter:
        case QEvent::GraphicsSceneHoverMove:
        case QEvent::GraphicsSceneMouseRelease:
        case QEvent::GraphicsSceneMouseMove:
        case QEvent::GraphicsSceneMousePress:
            {
                QGraphicsSceneMouseEvent* me = static_cast<QGraphicsSceneMouseEvent*>(event);
                QDElement* parent = qgraphicsitem_cast<QDElement*>(parentItem());
                assert(parent);
                me->setPos(mapToParent(me->pos()));
                res = parent->sceneEvent(me);
            }
            break;
        default:
            break;
    }
    return res;
}

/************************************************************************/
/* Footnote Item                                                        */
/************************************************************************/

Footnote::Footnote(QDElement* _from, QDElement* _to, QDDistanceType _distType, QDConstraint* parent, const QFont& _font)
: from(_from), to(_to), distType(_distType), constraint(parent), font(_font), draging(false) {
#if (QT_VERSION >= QT_VERSION_CHECK(4, 6, 0))
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
#endif
    connect(constraint->getParameters(), SIGNAL(si_modified()), SLOT(sl_update()));
    init();
}

void Footnote::init() {
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    from->links << this;
    to->links << this;

    QPen refPen(Qt::black);
    refPen.setStyle(Qt::DotLine);
    leftRef = new QGraphicsLineItem;
    rightRef = new QGraphicsLineItem;
    leftRef->setPen(refPen);
    leftRef->setZValue(-1);
    rightRef->setPen(refPen);
    rightRef->setZValue(-1);

    sl_update();
}

Footnote::~Footnote() {}

void Footnote::sl_update() {
    update();
}

#define FOOTNOTE_MARGIN 6
void Footnote::updatePos() {
    qreal xPos = getSrcPoint().x();
    int step = boundingRect().height() + FOOTNOTE_MARGIN;
    QueryScene* qs = qobject_cast<QueryScene*>(scene());
    assert(qs);
    const QRectF& area = qs->footnotesArea();
    //look for vacant position
    int y;
    for(y=area.top() + FOOTNOTE_MARGIN; y<area.bottom(); y+=step) {
        QRectF bound = boundingRect();
        bound.translate(QPointF(xPos,y));

        QList<QGraphicsItem*> overlapingItems = qs->items(bound, Qt::IntersectsItemBoundingRect);
        overlapingItems.removeAll(this);
        foreach(QGraphicsItem* item, overlapingItems) {
            if (item->type()!=FootnoteItemType) {
                overlapingItems.removeAll(item);
            }
        }

        if(overlapingItems.isEmpty()) {
            setPos(xPos, y);
            updateLines(QPointF(xPos, y));
            return;
        }
    }
    y+=step;
    //assert(qs->footnotesArea().contains(QPointF(xPos,y)));
    setPos(xPos, y);
    updateLines(QPointF(xPos, y));
}

void Footnote::updateLines(const QPointF& p) {
    QPointF p3 = getSrcPoint();
    QLineF rightLine(p3,p);

    QPointF p1 = getDstPoint();
    QPointF p2 = p + QPointF(p1.x()-p3.x(),0);
    QLineF leftLine(p1, p2);

    leftRef->setLine(leftLine);
    rightRef->setLine(rightLine);
}

QString Footnote::getText() const {
    return constraint->getText(from->getSchemeUnit(), to->getSchemeUnit());
}

QVariant Footnote::itemChange(GraphicsItemChange change, const QVariant &value) {
    if(change==ItemSceneHasChanged) {
        if(scene()) {
            scene()->addItem(leftRef);
            scene()->addItem(rightRef);
        }
    } else if(change==ItemSceneChange) {
        if((value.value<QGraphicsScene*>())==NULL) {
            scene()->removeItem(leftRef);
            scene()->removeItem(rightRef);
            delete leftRef;
            delete rightRef;
            from->links.removeAll(this);
            to->links.removeAll(this);
        }
    } else if (change==ItemPositionHasChanged) {
        updateLines(scenePos());
        leftRef->update();
        rightRef->update();
    }
    return QGraphicsItem::itemChange(change, value);
}

#define ARROW_DELTA 2
#define ARROW_WIDTH 4
QRectF Footnote::boundingRect() const {
    const QString& text = getText();
    QFontMetricsF fm(font);
    //fm.boundingRect().width() and fm.width() provide different values
    QRectF textBound(0,0,fm.width(text),fm.height());
    textBound.moveTop(ARROW_DELTA);

    qreal arrW = getDstPoint().x() - getSrcPoint().x();
    QRectF arrowBound(0,-ARROW_DELTA, arrW, 2*ARROW_DELTA);
    return textBound | arrowBound;
}

void Footnote::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->fillRect(boundingRect(),Qt::white);
    qreal arrW = getDstPoint().x() - getSrcPoint().x();
    QPen pen(Qt::black);
    if(isSelected()) {
        pen.setStyle(Qt::DashLine);
    }
    if(draging) {
        pen.setColor(Qt::green);
    }
    QDDistanceConstraint* dc = static_cast<QDDistanceConstraint*>(constraint);
    if (dc && dc->getMin()>dc->getMax()) {
        pen.setColor(Qt::red);
    }
    painter->setPen(pen);
    painter->drawLine(0, 0, arrW, 0);
    //draw arrow endings
    painter->drawLine(0,0,ARROW_WIDTH,ARROW_DELTA);
    painter->drawLine(0,0,ARROW_WIDTH,-ARROW_DELTA);
    painter->drawLine(arrW,0,arrW-ARROW_WIDTH,ARROW_DELTA);
    painter->drawLine(arrW,0,arrW-ARROW_WIDTH,-ARROW_DELTA);

    //draw text
    const QString& text = getText();
    QFontMetrics fm(font);
    QRectF textBound(0,0,fm.width(text),fm.height());
    textBound.moveTop(ARROW_DELTA);
    QPointF c(boundingRect().center().x(), textBound.center().y());
    textBound.moveCenter(c);
    painter->drawText(textBound, text);
}

QPointF Footnote::getSrcPoint() const {
    switch (distType) {
        case E2S:
        case E2E:
            return from->getRightConnector();
        case S2E:
        case S2S:
            return from->getLeftConnector();
    }
    assert(false);
    return QPointF(0,0);
}

QPointF Footnote::getDstPoint() const {
    switch (distType) {
    case E2S:
    case S2S:
        return to->getLeftConnector();
    case S2E:
    case E2E:
        return to->getRightConnector();
    }
    assert(false);
    return QPointF(0,0);
}

QDElement* Footnote::adjacent(QDElement* uv) const {
    if (uv==from) {
        return to;
    }
    if (uv==to) {
        return from;
    }
    return NULL;
}

/************************************************************************/
/* QDLabelItem                                                          */
/************************************************************************/

QDLabelItem::QDLabelItem(const QString& text) : QGraphicsTextItem(text) {
    setTextInteractionFlags(Qt::TextEditorInteraction);
}

void QDLabelItem::keyPressEvent(QKeyEvent *event) {
    if (event->key()==Qt::Key_Return || event->key()==Qt::Key_Enter || event->key()==Qt::Key_Space) {
        setSelected(false);
        return;
    }
    QGraphicsTextItem::keyPressEvent(event);
}

void QDLabelItem::focusOutEvent(QFocusEvent *event) {
    setTextInteractionFlags(Qt::NoTextInteraction);
    QGraphicsTextItem::focusOutEvent(event);
    emit si_editingFinished();
}

void QDLabelItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    setTextInteractionFlags(Qt::TextEditorInteraction);
    QGraphicsTextItem::mousePressEvent(event);
}

/************************************************************************/
/* QDDescriptionItem                                                    */
/************************************************************************/
#define TEXT_MARGINS 5
QDDescriptionItem::QDDescriptionItem(const QString& text)
: QGraphicsTextItem(text), resize(0) {
    setTextInteractionFlags(Qt::TextEditorInteraction);
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

QRectF QDDescriptionItem::boundingRect() const {
    QRectF bound = QGraphicsTextItem::boundingRect();
    const QPointF& cp = bound.center();
    bound.setWidth(bound.width()+TEXT_MARGINS*2);
    bound.setHeight(bound.height()+TEXT_MARGINS*2);
    bound.moveCenter(cp);
    return bound;
}

QPainterPath QDDescriptionItem::shape() const {
    QRectF bound = boundingRect();
    QPainterPath path;
    path.addRect(bound);
    return path;
}

void QDDescriptionItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);
    QRectF bound = boundingRect();
    bound.setHeight(bound.height()-1);
    //bound.setLeft(bound.left()+1);
    bound.setWidth(bound.width()-1);
    if (!hasFocus()) {
        painter->drawRect(bound);
    }
    QGraphicsTextItem::paint(painter, option, widget);
}

bool QDDescriptionItem::sceneEvent(QEvent *event) {
    switch(event->type()) {
        case QEvent::GraphicsSceneHoverEnter:
        case QEvent::GraphicsSceneHoverMove: {
            QGraphicsSceneHoverEvent* he = static_cast<QGraphicsSceneHoverEvent *>(event);
            QPointF p = he->pos();
            qreal dxRight = qAbs(boundingRect().right() - p.x());
            qreal dxLeft = qAbs(boundingRect().left() - p.x());
            if (p.y()>=boundingRect().top() && p.y()<=boundingRect().bottom()) {
                if (dxRight < EDGE_WIDTH) {
                    setCursor(Qt::SizeHorCursor);
                    resize = QDElement::ResizeRight;
                }
                else if(dxLeft < EDGE_WIDTH) {
                    setCursor(Qt::SizeHorCursor);
                    resize = QDElement::ResizeLeft;
                }
                else {
                    unsetCursor();
                    resize = 0;
                }
            }
            break;
        }
        case QEvent::GraphicsSceneHoverLeave:
        case QEvent::GraphicsSceneMouseRelease:
            unsetCursor();
            resize=0;
            break;
        case QEvent::GraphicsSceneMouseMove:
        {
            if(resize) {
                //QueryScene* qs = qobject_cast<QueryScene*>(scene());
                QGraphicsSceneMouseEvent* me = static_cast<QGraphicsSceneMouseEvent *>(event);
                if(me->buttons()&Qt::LeftButton) {
                    QPointF p = me->pos();
                    QPointF oldPos = me->lastPos();
                    if (resize==QDElement::ResizeRight) {
                        setTextWidth(textWidth()+p.x()-oldPos.x());
                    }
                    else if (resize==QDElement::ResizeLeft) {
                        QPointF newPos = scenePos();
                        newPos.setX(me->scenePos().x());
                        setTextWidth(textWidth()-newPos.x()+scenePos().x());
                        setPos(newPos);
                    }
                }
            }
        }
        break;
        default:;
    }
    return QGraphicsTextItem::sceneEvent(event);
}

void QDDescriptionItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    setTextInteractionFlags(Qt::TextEditorInteraction);
    QGraphicsTextItem::mousePressEvent(event);
}

void QDDescriptionItem::focusOutEvent(QFocusEvent *event) {
    setTextInteractionFlags(Qt::NoTextInteraction);
    QGraphicsTextItem::focusOutEvent(event);
}

/************************************************************************/
/* Ruler                                                                */
/************************************************************************/
#define NOTCH_SIZE 6
#define QD_RULER_MARGINS 5
#define MIN_LEN_TO_DRAW 20

QRectF QDRulerItem::boundingRect() const {
    QRectF bound(leftPos, 0, rightPos, QD_RULER_MARGINS*2 + NOTCH_SIZE);
    QRectF resBound = bound.united(txtBound());
    resBound.setHeight(resBound.height()+QD_RULER_MARGINS);
    return resBound;
}

QRectF QDRulerItem::txtBound() const {
    QFontMetricsF fm(font);
    QRectF txtBound(0,0,fm.width(text),fm.height());
    qreal txtX = leftPos + (rightPos - leftPos) / 2;
    qreal txtY = QD_RULER_MARGINS + NOTCH_SIZE + txtBound.height()/2;
    txtBound.moveCenter(QPointF(txtX, txtY));
    return txtBound;
}

void QDRulerItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget * ) {
    if (rightPos - leftPos < MIN_LEN_TO_DRAW) {
        return;
    }
    painter->drawLine(leftPos, NOTCH_SIZE/2 + QD_RULER_MARGINS, rightPos, NOTCH_SIZE/2+QD_RULER_MARGINS);
    painter->drawLine(leftPos, QD_RULER_MARGINS, leftPos, NOTCH_SIZE + QD_RULER_MARGINS);
    painter->drawLine(rightPos, QD_RULER_MARGINS, rightPos, NOTCH_SIZE + QD_RULER_MARGINS);

    painter->setFont(font);
    painter->drawText(txtBound(), text);
}

QVariant QDRulerItem::itemChange(GraphicsItemChange change, const QVariant &value) {
    if(change==ItemSceneHasChanged) {
        if(scene()) {
            connect(scene(), SIGNAL(changed(const QList<QRectF>&)), SLOT(sl_updateGeometry()));
        }
    }
    return QGraphicsItem::itemChange(change, value);
}

void QDRulerItem::sl_updateGeometry() {
    QueryScene* qs = qobject_cast<QueryScene*>(scene());
    assert(qs);
    QList<QDElement*> items;
    foreach(QGraphicsItem* it, qs->getElements()) {
        QDElement* uv = qgraphicsitem_cast<QDElement*>(it);
        assert(uv);
        items.append(uv);
    }
    if (items.isEmpty()) {
        leftPos = 0;
        rightPos = 0;
    }
    else {
        leftPos = items.first()->scenePos().x();
        rightPos = items.first()->scenePos().x() + items.first()->boundingRect().width();
        foreach(QDElement* item, items) {
            qreal curLeft = item->scenePos().x();
            qreal curRight = curLeft + item->boundingRect().right();
            if (curLeft < leftPos) {
                leftPos = curLeft;
            }
            if (curRight > rightPos) {
                rightPos = curRight;
            }
        }
    }
}

void QDRulerItem::sl_updateText() {
    int minDist = 0;
    int maxDist = 0;
    QueryScene* qs = qobject_cast<QueryScene*>(scene());
    assert(qs);
    QDScheme* scheme = qs->getScheme();
    QList<QDSchemeUnit*> units;
    foreach(QDActor* a, scheme->getActors()) {
        units << a->getSchemeUnits();
    }
    for (int i=0, n=units.size(); i<n-1; i++) {
        for (int j=i+1; j<n; j++) {
            QDSchemeUnit* src = units.at(i);
            QDSchemeUnit* dst = units.at(j);
            QList<QDPath*> paths = scheme->findPaths(src, dst);
            foreach(QDPath* path, paths) {
                QDDistanceConstraint* dc = path->toConstraint();
                if(!dc) {
                    text = tr("N/A");
                    update();
                    return;
                }
                int curMinDist = dc->getMin();
                int curMaxDist = dc->getMax();
                QDSchemeUnit* dcSrc = dc->getSource();
                QDSchemeUnit* dcDst = dc->getDestination();
                if (dc->distanceType()==S2S) {
                    curMinDist+=dcDst->getActor()->getMinResultLen();
                    curMaxDist+=dcDst->getActor()->getMaxResultLen();
                }
                if (dc->distanceType()==E2E) {
                    curMinDist+=dcSrc->getActor()->getMinResultLen();
                    curMaxDist+=dcSrc->getActor()->getMaxResultLen();
                }
                if (dc->distanceType()==E2S) {
                    curMinDist+=dcSrc->getActor()->getMinResultLen();
                    curMaxDist+=dcSrc->getActor()->getMaxResultLen();
                    curMinDist+=dcDst->getActor()->getMinResultLen();
                    curMaxDist+=dcDst->getActor()->getMaxResultLen();
                }
                minDist = qMax(minDist, curMinDist);
                maxDist = qMax(maxDist, curMaxDist);
            }
        }
    }
    foreach(QDSchemeUnit* su, units) {
        minDist = qMax(su->getActor()->getMinResultLen(), minDist);
        maxDist = qMax(su->getActor()->getMaxResultLen(), maxDist);
    }
    if (minDist==maxDist) {
        text= QString("%1 bp").arg(minDist);
    }
    else {
        text = QString("%1..%2 bp").arg(minDist).arg(maxDist);
    }
    update();
}

}//namespace
