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

#include <QBitmap>
#include <QDomElement>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QPainter>
#include <QRadialGradient>
#include <QStyleOptionGraphicsItem>
#include <QTextDocument>
#include <QtMath>

#include <U2Core/Log.h>
#include <U2Core/QVariantUtils.h>

#include <U2Lang/ActorModel.h>
#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/IntegralBus.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/WorkflowRunTask.h>
#include <U2Lang/WorkflowSettings.h>
#include <U2Lang/WorkflowUtils.h>

#include "ItemViewStyle.h"
#include "WorkflowEditor.h"
#include "WorkflowViewController.h"
#include "WorkflowViewItems.h"

namespace U2 {


WorkflowProcessItem::WorkflowProcessItem(Actor* prc) : process(prc), hasBreakpoint(false), highlighting(NULL)/*, inspectionItem(NULL)*/ {
    setToolTip(process->getProto()->getDocumentation());
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
#if (QT_VERSION >= QT_VERSION_CHECK(4, 6, 0))
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
#endif
    setAcceptHoverEvents(true);

    styles[ItemStyles::SIMPLE] = new SimpleProcStyle(this);
    styles[ItemStyles::EXTENDED] = new ExtendedProcStyle(this);
    currentStyle = getStyleByIdSafe(WorkflowSettings::defaultStyle());
    currentStyle->setVisible(true);
    createPorts();
    connect(prc, SIGNAL(si_descriptionChanged()), SLOT(sl_descriptionChanged()));
}

WorkflowProcessItem::~WorkflowProcessItem()
{
    qDeleteAll(styles.values());
    qDeleteAll(ports);
    delete highlighting;
}

ItemViewStyle* WorkflowProcessItem::getStyleByIdSafe(StyleId id) const {
    if (!styles.contains(id)) {
        uiLog.trace(QString("Unknown workflow item style: %1").arg(id));
        id = ItemStyles::EXTENDED;
    }
    ItemViewStyle* result = styles.value(id);
    assert(result!=NULL);
    return result;
}

ItemViewStyle* WorkflowProcessItem::getStyleById(const StyleId & id) const {
    return styles.value(id);
}

bool WorkflowProcessItem::containsStyle(const StyleId & id) const {
    return styles.contains(id);
}

WorkflowPortItem * WorkflowProcessItem::getPort(const QString& id) const {
    foreach(WorkflowPortItem* pit, ports) {
        if (pit->getPort()->getId() == id) {
            return pit;
        }
    }
    return NULL;
}

void WorkflowProcessItem::createPorts() {
    assert(ports.isEmpty());

    int num = process->getInputPorts().size() + 1;
    qreal pie = 180/num;
    int i = 1;
    QGraphicsScene* sc = scene();
    foreach(Port* port, process->getInputPorts()) {
        WorkflowPortItem* pit = new WorkflowPortItem(this, port);
        connect(port, SIGNAL(si_enabledChanged(bool)), pit, SLOT(sl_onVisibleChanged(bool)));
        ports << pit;
        pit->setOrientation(90 + pie*i++);
        if (sc) {
            sc->addItem(pit);
        }
        pit->sl_onVisibleChanged(port->isEnabled());
    }
    num = process->getOutputPorts().size() + 1;
    pie = 180/num;
    i = 1;
    foreach(Port* port, process->getOutputPorts()) {
        WorkflowPortItem* pit = new WorkflowPortItem(this, port);
        connect(port, SIGNAL(si_enabledChanged(bool)), pit, SLOT(sl_onVisibleChanged(bool)));
        ports << pit;
        pit->setOrientation(270 + pie*i++);
        if (sc) {
            sc->addItem(pit);
        }
        pit->sl_onVisibleChanged(port->isEnabled());
    }
}

void WorkflowProcessItem::sl_update() {
    prepareGeometryChange();
    currentStyle->refresh();
    foreach(WorkflowPortItem* pit, ports) {
        pit->adaptOwnerShape();
    }
    update();
}

void WorkflowProcessItem::sl_descriptionChanged() {
    setToolTip(process->getProto()->getDocumentation());
}

void WorkflowProcessItem::setStyle(StyleId s) {
    prepareGeometryChange();
    currentStyle->setVisible(false);
    currentStyle = getStyleByIdSafe(s);
    currentStyle->setVisible(true);
    currentStyle->refresh();
    foreach(WorkflowPortItem* pit, ports) {
        pit->setStyle(s);
    }
    assert(currentStyle);
    update();
}

QRectF WorkflowProcessItem::boundingRect(void) const {
    QRectF brect = currentStyle->boundingRect();
    brect.setTop(brect.top() - QFontMetrics(QFont()).height()*2 - 2);
    return brect;
}

QRectF WorkflowProcessItem::portsBoundingRect() const {
    QRectF rect; // null rect
    foreach(WorkflowPortItem* p, getPortItems()) {
        QRectF pBound = p->boundingRect();
        QPointF pCenter = pBound.center();
        pCenter = p->mapToItem(this, pCenter);
        pBound.moveCenter(pCenter);
        rect |= pBound;
    }
    return rect;
}

QPainterPath WorkflowProcessItem::shape () const {
    return currentStyle->shape();
}

void WorkflowProcessItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    WorkflowAbstractRunner* rt = getWorkflowScene()->getRunner();
    if (rt) {
        //{WorkerWaiting, WorkerReady, WorkerRunning, WorkerDone};
        static QColor rsColors[5] = {QColor(234,143,7),"#04AA04", "#AA0404", "#0404AA", "#9B30FF"};
            //{QColor(234,143,7),QColor(Qt::red),QColor(Qt::green),QColor(0,0,255)};
        static QString rsNames[5] = {("Waiting"), ("Ready"), ("Running"), ("Done"), ("Paused")};

        const QList<WorkerState> rsList = rt->getState(this->process);
        WorkerState state = WorkerDone;
        if (rsList.contains(WorkerRunning)) {
            state = WorkerRunning;
        } else if (rsList.contains(WorkerReady)) {
            state = WorkerReady;
        } else if (rsList.contains(WorkerWaiting)) {
            state = WorkerWaiting;
        } else if (rsList.contains(WorkerPaused)) {
            state = WorkerPaused;
        }

        QString stateName = rsNames[state];
        QColor scolor = rsColors[state];
        painter->setPen(scolor);
        QRectF brect = boundingRect();
        qreal fh = QFontMetrics(QFont()).height();

        if (rsList.size() == 1) {
            brect.setTop(brect.top()+2+fh);
        }

        painter->drawRoundedRect(brect, 5, 5);
        painter->drawText(brect, Qt::AlignHCenter, stateName);
        if (rsList.size() == 1) {

        } else {
            uint vals[4] = {0,0,0,0};
            for (int i = rsList.size();i>0;)
            {
                vals[rsList.at(--i)]++;
            }

            // draw progress bar
            if (state != WorkerDone) {

                QRectF textRect(brect.topLeft()+QPointF(0,fh-1),QSizeF(brect.width(), 3));
                textRect.setLeft(brect.left()+1);
                textRect.setRight(brect.right()-1);
                QColor fc = /*rsColors[WorkerDone];//*/ QColor(0,80,222);
                fc.setAlpha(90);
                painter->setPen(fc);
                painter->drawRect(textRect);
                qreal done = (qreal)vals[WorkerDone]/rsList.size();
                QBrush brush(fc);
//                 QLinearGradient lg(textRect.topLeft(),textRect.topRight());
//                 lg.setColorAt(0, fc);
//                 lg.setColorAt(done, fc.lighter(128));
//                 lg.setColorAt(done + 1./rsList.size(), QColor(Qt::white));
                //lg.setColorAt(1, QColor(Qt::white));
                //brush = QBrush(lg);
                textRect.setRight(textRect.left() + textRect.width()*done);
                painter->fillRect(textRect, brush);
            }

            // draw extended text
            painter->save();
            QTextDocument d;
            d.setHtml("<center><font size='-1'>"
                + QString("<font color='%1'>%2/</font>"
                " <!--font color='%3'>%4/</font-->"
                " <font color='%5'>%6/</font>"
                " <font color='%7'>%8/</font>"
                " <font color='black'>%9</font>")
                .arg(rsColors[WorkerWaiting].name()).arg(vals[WorkerWaiting])
                .arg(rsColors[WorkerReady].name()).arg(vals[WorkerReady])
                .arg(rsColors[WorkerRunning].name()).arg(vals[WorkerRunning])
                .arg(rsColors[WorkerDone].name()).arg(vals[WorkerDone])
                .arg(rsList.size())
                + "</font></center>");
            //d.setTextWidth(brect.width());
            painter->translate(brect.center().x() - d.idealWidth()/2, brect.top() + fh);
            d.drawContents(painter/*, brect*/);
            painter->restore();

//             qreal unit = brect.width()/rsList.size();
//             qreal step = 3;
//             qreal w = 10;
//             QPointF base(brect.topLeft());
//             base.ry() += 5;

//             for (int i = 3; i>=0; i--) {
//  //             base.ry() -= step;
//  //             painter->setPen(rsColors[i]); painter->drawLine(base, base + QPointF(unit*vals[i],0));
//                 QRectF rt(base, base + QPointF(unit*vals[i],w)); base = rt.topRight();
//                 painter->setPen(rsColors[i]);
//                 QColor fc = rsColors[i]; fc.setAlpha(128);
//                 painter->fillRect(rt, QBrush(fc));
//                 painter->drawText(rt, Qt::AlignJustify|Qt::AlignVCenter, rsNames[i]);
//             }
        }
    }
}

void WorkflowProcessItem::updatePorts(){
    foreach(WorkflowPortItem* pit, ports) {
        pit->setPos(pos());
        foreach(WorkflowBusItem*bit, pit->getDataFlows()) {
            bit->updatePos();
        }
    }
}

QVariant WorkflowProcessItem::itemChange ( GraphicsItemChange change, const QVariant & value )
{
    switch(change) {
    case ItemSelectedHasChanged:
    {
        currentStyle->update();
    }
        break;
    case ItemZValueHasChanged:
    {
        qreal z = value.value<qreal>();
        foreach(WorkflowPortItem* pit, ports) {
            pit->setZValue(z);
        }
    }
        break;
    case ItemPositionChange:
    {
        // value is the new position.
        QPointF newPos = value.toPointF();
        if (scene() && pos() != QPointF(0,0)) {
            QRectF bound = boundingRect() | childrenBoundingRect() | portsBoundingRect();
            QRectF sceneRect = scene()->sceneRect();

            qreal x0 = sceneRect.left() - bound.left();
            qreal x1 = sceneRect.left() + sceneRect.width() - bound.right() - 10; //extra space for scroll bars
            qreal y0 = sceneRect.top() - bound.top();
            qreal y1 = sceneRect.top() + sceneRect.height() - bound.bottom() - 10;

            newPos.setX( qBound(x0, newPos.x(), x1) );
            newPos.setY( qBound(y0, newPos.y(), y1) );
        }
        if (WorkflowSettings::snap2Grid()) {
            newPos.setX(round(newPos.x(), GRID_STEP));
            newPos.setY(round(newPos.y(), GRID_STEP));
        }
        return newPos;
        /*foreach(WorkflowPortItem* pit, ports) {
        foreach(WorkflowBusItem*bit, pit->getDataFlows()) {
        bit->prepareGeometryChange();
        }
        }*/
    }
        break;
    case ItemPositionHasChanged:
        {

            updatePorts();

            WorkflowScene * sc = qobject_cast<WorkflowScene*>(scene());
            if (sc != NULL) {
                if (!sc->views().isEmpty()) {
                    foreach(QGraphicsView* view, sc->views()) {
                        QRectF itemRect = boundingRect() | childrenBoundingRect();
                        // ports are not the child items atm
                        // unite with their bounds
                        itemRect |= portsBoundingRect();
                        QPointF itemCenter = itemRect.center();
                        itemCenter = mapToScene(itemCenter);
                        itemRect.moveCenter(itemCenter);
                        view->ensureVisible(itemRect, 0, 0);
                    }
                }
                sc->setModified(true);
            }
            if (scene()) {
                scene()->update();
            }
        }
        break;
    case ItemSceneHasChanged:
        {
            WorkflowScene* ws = getWorkflowScene();
            if (ws) {
                ItemViewStyle* viewStyle = styles.value(ItemStyles::EXTENDED);
                ExtendedProcStyle* extStyle = qgraphicsitem_cast<ExtendedProcStyle*>(viewStyle);
                assert(extStyle);
                WorkflowView* view = ws->getController();
                if (view) {
                    connect(extStyle, SIGNAL(linkActivated(const QString&)),
                    view->getPropertyEditor(), SLOT(sl_linkActivated(const QString&)));
                }

                foreach(WorkflowPortItem* pit, ports) {
                    ws->addItem(pit);
                }
            }
        }
        break;
    case ItemSceneChange:
        if ((value.value<QGraphicsScene*>()) == NULL) {
            foreach(WorkflowPortItem* pit, ports) {
                scene()->removeItem(pit);
            }
            //scene()->removeItem(inspectionItem);
            //delete inspectionItem;
        }
        break;
/*    case ItemSelectedChange:
        if (NULL != inspectionItem) {
            inspectionItem->setPermanent(!inspectionItem->isPermanent());
            if (!inspectionItem->isPermanent()) {
                inspectionItem->eraseFromScene();
            }
        }
        break;*/
    default:
        break;
    }
    return QGraphicsItem::itemChange(change, value);
}

bool WorkflowProcessItem::sceneEvent( QEvent * event ) {
    if (currentStyle->sceneEventFilter(this, event)) {
        return true;
    }
    return QGraphicsItem::sceneEvent(event);
}

QList<QAction*> WorkflowProcessItem::getContextMenuActions() const
{
    return currentStyle->getContextMenuActions();
}

void WorkflowProcessItem::saveState(QDomElement& el) const {
    el.setAttribute("pos", QVariantUtils::var2String(pos()));
    el.setAttribute("style", styles.key(currentStyle));
    foreach(ItemViewStyle* style, styles) {
        QDomElement stel = el.ownerDocument().createElement(style->getId());
        style->saveState(stel);
        if (stel.hasAttributes() || stel.hasChildNodes()) {
            el.appendChild(stel);
        }
    }
}

void WorkflowProcessItem::loadState(QDomElement& el) {
    const QString posS = el.attribute("pos");
    const QPointF pos = QVariantUtils::String2Var(posS).toPointF();
    assert(!pos.isNull());
    setPos(pos);

    foreach(ItemViewStyle* style, styles) {
        QDomElement stel = el.elementsByTagName(style->getId()).item(0).toElement();
        if (stel.isNull()) continue;
        style->loadState(stel);
    }
    QString key = el.attribute("style");
    if (styles.contains(key)) {
        setStyle(key);
    }
}

void WorkflowProcessItem::mouseMoveEvent( QGraphicsSceneMouseEvent *event ) {
    if (event->buttons() & Qt::LeftButton) {
        if (initialPositions.isEmpty()) {
            if (isSelected()) {
                QList<QGraphicsItem *> selectedItems = scene()->selectedItems();
                foreach(QGraphicsItem* item, selectedItems) {
                    if (item->type()==WorkflowProcessItemType) {
                        initialPositions[item] = item->scenePos();
                    }
                }
            } else {
                initialPositions[this] = scenePos();
            }
        }

        // Find the active view.
        QGraphicsView *view = 0;
        if (event->widget()) {
            view = qobject_cast<QGraphicsView *>(event->widget()->parentWidget());
        }

        for (int i=0, n=initialPositions.keys().size(); i<n; i++) {
            QGraphicsItem *item = initialPositions.keys().at(i);

            QPointF currentParentPos = view->mapToScene(view->mapFromGlobal(event->screenPos()));
            QPointF buttonDownParentPos = view->mapToScene(view->mapFromGlobal(event->buttonDownScreenPos(Qt::LeftButton)));

            item->setPos(initialPositions.value(item) + currentParentPos - buttonDownParentPos);
        }
    } else {
        event->ignore();
    }
}

void WorkflowProcessItem::mouseReleaseEvent( QGraphicsSceneMouseEvent *event ) {
    initialPositions.clear();
    QGraphicsItem::mouseReleaseEvent(event);
}

void WorkflowProcessItem::toggleBreakpoint() {
    hasBreakpoint = !hasBreakpoint;
    if(!hasBreakpoint) {
        hasEnabledBreakpoint = false;
    }
    else {
        if(NULL == highlighting) {
            highlighting = new WorkflowHighlightItem(this);
        }
        hasEnabledBreakpoint = true;
    }
}

void WorkflowProcessItem::toggleBreakpointState() {
    Q_ASSERT(hasBreakpoint);
    hasEnabledBreakpoint = !hasEnabledBreakpoint;
}

bool WorkflowProcessItem::isBreakpointInserted() {
    return hasBreakpoint;
}

bool WorkflowProcessItem::isBreakpointEnabled() {
    return hasEnabledBreakpoint;
}

void WorkflowProcessItem::highlightItem() {
    highlighting->replay();
}

///////////// PIO /////////////

WorkflowPortItem* WorkflowPortItem::findNearbyBindingCandidate(const QPointF& pos) const {
    QPainterPath neighbourhood;
    neighbourhood.addEllipse(pos, R/2, R/2);
    //QRectF neighbourhood(pos.x() - R/2, pos.y() + R/2, R, R);
    WorkflowPortItem* candidate = NULL;
    qreal distance = R*2;
    foreach(QGraphicsItem* it, scene()->items(neighbourhood, Qt::IntersectsItemBoundingRect)) {
        WorkflowPortItem* next = qgraphicsitem_cast<WorkflowPortItem*>(it);
        if (next) {
            if (bindCandidates.contains(next)) {
                QLineF l(pos, next->headToScene());
                qreal len = l.length();
                if (distance > len) {
                    distance = len;
                    candidate = next;
                }
            }
        }
    }
    return candidate;
}

//static const QCursor portRotationCursor = QCursor(QBitmap(":workflow_designer/images/rot_cur.png")); //FIXME
static const int portRotationModifier = Qt::AltModifier;
static const int bl = (int) A/4;

WorkflowPortItem::WorkflowPortItem(WorkflowProcessItem* owner, Port* p)
    : /*StyledItem(owner), */ currentStyle(owner->getStyle()),port(p),owner(owner),orientation(0), dragging(false), rotating(false),
    sticky(false), highlight(false), mouseMoveIsBeingProcessed(false)
{
    setFlags(ItemIsSelectable | ItemIsFocusable);
    setAcceptHoverEvents(true);
    QString tt = p->isInput() ? "Input port (" : "Output port (";
    tt += p->getDocumentation();
    tt += ").\nDrag it to connect to other process/port."
        "\nHold Alt key while dragging to change port orientation";
    setToolTip(tt);

    setPos(owner->pos());
    setZValue(owner->zValue());
}

WorkflowPortItem::~WorkflowPortItem()
{
    assert(flows.isEmpty());
}


void WorkflowPortItem::setStyle(StyleId s) {
    Q_UNUSED(s);
    currentStyle = owner->getStyle();
    adaptOwnerShape();
}

void WorkflowPortItem::adaptOwnerShape() {
    setOrientation(orientation);
}

void WorkflowPortItem::setOrientation(qreal angle) {
    qreal oldOrientation = orientation;
    orientation = angle;

    if (ItemStyles::SIMPLE == currentStyle) {
        angle = -angle;
        qreal x = R * qCos(angle * 2 * M_PI / 360);
        qreal y = R * qSin(angle * 2 * M_PI / 360);

        resetTransform();
        setTransform(QTransform::fromTranslate(x, y), true);
        setRotation(angle);
    } else { // EXTENDED STYLE
        resetTransform();
        QRectF rec = owner->boundingRect();
        QPolygonF pol(owner->shape().toFillPolygon());
        qreal radius = qMax(rec.width(), rec.height()) * 2;
        QLineF centerLine(0,0,radius, 0);
        assert(pol.containsPoint(centerLine.p1(), Qt::WindingFill));
        assert(!pol.containsPoint(centerLine.p2(), Qt::WindingFill));
        centerLine.setAngle(angle);
        QPointF p1 = pol.first();
        QPointF p2;
        QLineF polyLine;
        QPointF intersectPoint;
        for (int i = 1; i < pol.count(); ++i) {
            p2 = pol.at(i);
            polyLine = QLineF(p1, p2);
            if (QLineF::BoundedIntersection == polyLine.intersect(centerLine, &intersectPoint)) {
                break;
            }
            p1 = p2;
        }

        setTransform(QTransform::fromTranslate(intersectPoint.x(), intersectPoint.y()), true);
        qreal norm = polyLine.normalVector().angle();
        qreal df = qAbs(norm - angle);
        if (df > 90 && df < 270) {
            norm += 180;
        }
        setRotation(-norm);
    }
    if (oldOrientation != orientation) {
        WorkflowScene * sc = qobject_cast<WorkflowScene*>(owner->scene());
        if (sc != NULL) {
            sc->setModified(true);
            sc->update();
        }
    }
}

WorkflowBusItem* WorkflowPortItem::getDataFlow(const WorkflowPortItem* otherPit) const {
    foreach(WorkflowBusItem* dit, flows) {
        if ((port->isInput() ? dit->getOutPort() : dit->getInPort()) == otherPit) {
            return dit;
        }
    }
    return NULL;
}

static bool checkTypes(Port* p1, Port* p2) {
    Port* ip = p1->isInput() ? p1 : p2;
    Port* op = p1->isInput() ? p2 : p1;
    DataTypePtr idt = ip->getType();
    DataTypePtr odt = op->getType();
    if (idt->isSingle() && odt->isMap()) {
        foreach(Descriptor d, odt->getAllDescriptors()) {
            if (idt == odt->getDatatypeByDescriptor(d)) return true;
        }
    }
    if (idt->isMap() && odt->isMap()) {
        if (idt->getDatatypesMap().isEmpty()) {
            ActorPrototype *proto = ip->owner()->getProto();
            return proto->isAllowsEmptyPorts();
        }
        foreach(Descriptor d1, idt->getAllDescriptors()) {
            foreach(Descriptor d2, odt->getAllDescriptors()) {
                if (idt->getDatatypeByDescriptor(d1) == odt->getDatatypeByDescriptor(d2)) return true;
            }
        }
    }
    return odt == idt;
}

WorkflowPortItem* WorkflowPortItem::checkBindCandidate(const QGraphicsItem* it) const
{
    switch (it->type()) {
        case WorkflowProcessItemType:
            {
                const WorkflowProcessItem* receiver = static_cast<const WorkflowProcessItem*>(it);
                // try best matches first
                foreach(WorkflowPortItem* otherPit, receiver->getPortItems()) {
                    if (port->canBind(otherPit->getPort()) && checkTypes(port, otherPit->getPort())) {
                        return otherPit;
                    }
                }
                // take first free port
                foreach(WorkflowPortItem* otherPit, receiver->getPortItems()) {
                    if (port->canBind(otherPit->getPort())) {
                        return otherPit;
                    }
                }
            }
            break;
        case WorkflowPortItemType:
            {
                WorkflowPortItem* otherPit = (WorkflowPortItem*)(it);
                if (port->canBind(otherPit->getPort())) {
                    return otherPit;
                }
            }
            break;
    }
    return NULL;
}

void WorkflowPortItem::removeDataFlow(WorkflowBusItem* flow) {
    assert(flows.contains(flow));
    flows.removeOne(flow);
    assert(!flows.contains(flow));
}

void WorkflowPortItem::addDataFlow(WorkflowBusItem *flow) {
    flows << flow;
}

QPointF WorkflowPortItem::head(const QGraphicsItem* item) const {
    return mapToItem(item, A/2 + bl, 0);
}

QPointF WorkflowPortItem::headToScene() const {
    return mapToScene(A, 0);
}

QLineF WorkflowPortItem::arrow(const QGraphicsItem* item) const {
    return QLineF(mapToItem(item, 0,0),mapToItem(item, A,0));
}

QRectF WorkflowPortItem::boundingRect(void) const {
    QRectF rect(0, -A, A+A/2, 2*A);
    if (dragging) {
        rect |= QRectF(QPointF(A, 0), dragPoint);
        //FIXME arrow tip inclusion
    }
    return rect;
}

static void drawArrow(QPainter *painter, const QPen& pen, const QPointF& p1, const QPointF& p2) {
    painter->setPen(pen);
    QLineF l(p1, p2);
    painter->drawLine(l);
    //draw arrow tip
    painter->save();
    painter->translate(p2);
    painter->rotate(-l.angle());
    QRectF rf(-3*A, -A/2, A*1.5, A);
    QPainterPath tip(QPointF(0,0));
    tip.arcTo(rf, -50, 100);
    tip.closeSubpath();
    painter->fillPath(tip, QBrush(pen.color()));
    painter->restore();
}

static QList<WorkflowPortItem*> getCandidates(WorkflowPortItem* port) {
    QList<WorkflowPortItem*> l;
    foreach(QGraphicsItem* it, port->scene()->items()) {
        if (it->type() == WorkflowPortItemType) {
            WorkflowPortItem* next = qgraphicsitem_cast<WorkflowPortItem*>(it);
            if (port->getPort()->canBind(next->getPort())
                && checkTypes(next->getPort(), port->getPort())
                && !WorkflowUtils::isPathExist(port->getPort(), next->getPort()))
            {
                l.append(next);
            }
        }
    }
    return l;
}

void WorkflowPortItem::paint(QPainter *painter,
                                const QStyleOptionGraphicsItem *option,
                                QWidget *widget)
{
    Q_UNUSED(widget);
    QPointF p1(A/2 + bl, 0);
    QColor greenLight(0,0x99,0x33, 128);
    QColor stickyLight(0,0x77,0x33);

    if (highlight) {
        QPen pen;
        pen.setColor(greenLight);
        painter->setPen(pen);
    }

    //painter->fillRect(boundingRect(), QBrush(Qt::magenta, Qt::Dense4Pattern));
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawLine(0, 0, bl, 0);

    if (port->isInput()) {
        if (highlight) {
            QPainterPath path;
            path.addEllipse(bl, -A/2, A, A);
            painter->fillPath(path, QBrush(greenLight));
        } else {
            painter->drawArc(QRectF(bl, -A/2, A, A), 90*16, 180*16);
        }
    } else {
        if (highlight) {
            QPainterPath path;
            path.addEllipse(p1, A/2, A/2);
            painter->fillPath(path, QBrush(greenLight));
        } else {
            painter->drawEllipse(p1, A/2, A/2);
        }
    }
    if (0 && port->getWidth() == 0) {
        // draw a hint
        painter->save();
        QPen pen;
        //pen.setWidthF(2);
        pen.setStyle(Qt::DashLine);
        painter->setPen(pen);
        QPointF hc(R, 0);
        drawArrow(painter, pen, hc, p1);

        painter->translate(R, 0);
        painter->rotate(orientation);
        QRectF approx(-10, -10, 200, 100);
        QRectF htb = painter->boundingRect(approx, Qt::AlignCenter, port->getDisplayName());
        //painter->drawRoundedRect(htb, 30, 30, Qt::RelativeSize);

        painter->drawRoundedRect(htb, 30, 30, Qt::RelativeSize);
        painter->drawText(approx, Qt::AlignCenter, port->getDisplayName());
        painter->setPen(Qt::red);painter->drawPoint(0,0);
        painter->restore();
    }
    if (dragging) {
        QPen pen;
        //pen.setWidthF(3);
        pen.setStyle(Qt::DotLine);
        if (sticky) {
            pen.setColor(stickyLight);
        }
        //put drag point inside of the scene rect
        QPointF pp = dragPoint;
        QRectF scRect = scene()->sceneRect();
        QList<QLineF> sceneEdges;
        sceneEdges << QLineF(scRect.topLeft(), scRect.topRight())
            << QLineF(scRect.topRight(), scRect.bottomRight())
            << QLineF(scRect.bottomLeft(), scRect.bottomRight())
            << QLineF(scRect.topLeft(), scRect.bottomLeft());
        QLineF arr(mapToScene(dragPoint), mapToScene(p1));
        QPointF crossPt;
        foreach(QLineF scEdge, sceneEdges) {
            if (scEdge.intersect(arr, &crossPt) == QLineF::BoundedIntersection) {
                pp = mapFromScene(crossPt);
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        if (port->isInput())
            drawArrow(painter, pen, pp, p1);
        else
            drawArrow(painter, pen, p1, pp);
    }
    else if (option->state & QStyle::State_Selected) {
        QPen pen;
        //pen.setWidthF(2);
        pen.setStyle(Qt::DotLine);
        painter->setPen(pen);
        painter->drawRoundedRect(boundingRect(), 30, 30, Qt::RelativeSize);
    }
}

void WorkflowPortItem::focusOutEvent(QFocusEvent * /*event*/) {
    if(dragging) {
        dragging = false;
        scene()->update();
    }
}

namespace {

class ScopedFlagFlipper {
public:
    ScopedFlagFlipper(bool &flag)
        : flag(flag)
    {
        flag = true;
    }

    ~ScopedFlagFlipper() {
        flag = false;
    }

private:
    bool &flag;
};

}

void WorkflowPortItem::mouseMoveEvent( QGraphicsSceneMouseEvent * event ) {
    CHECK(!mouseMoveIsBeingProcessed, );

    ScopedFlagFlipper guard(mouseMoveIsBeingProcessed);
    Q_UNUSED(guard);
    if (!dragging && !rotating && (event->buttons() & Qt::LeftButton) && !dragPoint.isNull()) {
        //log.debug("port grabbed mouse");
        if ((event->pos().toPoint() - dragPoint.toPoint()).manhattanLength() < 10) return;
        event->accept();
        //grabMouse();
        if (event->modifiers() & portRotationModifier) {
            rotating = true;
            //setCursor(portRotationCursor);
            setCursor(QCursor(QPixmap(":workflow_designer/images/rot_cur.png")));
        } else {
            dragging = true;
            setCursor(Qt::ClosedHandCursor);
            bindCandidates = getCandidates(this);
            foreach(WorkflowPortItem* it, bindCandidates) {
                it->setHighlight(true);
                it->update(it->boundingRect());
            }
        }
    }

    sticky = false;

    if (!dragging && !rotating) {
        return;
    }
    event->accept();
    prepareGeometryChange();
    if (rotating) {
        qreal angle = QLineF(owner->pos(), event->scenePos()).angle();
        setOrientation(angle);
    }
    if (dragging) {
        foreach(QGraphicsView* v, scene()->views()) {
            QRectF r(0,0,5,5);
            r.moveCenter(mapToScene(dragPoint));
            v->ensureVisible(r,0,0);
        }
        dragPoint += event->pos() - event->lastPos();

        WorkflowPortItem* preferable = findNearbyBindingCandidate(event->scenePos());
        if (preferable) {
            dragPoint = preferable->head(this);
            sticky = true;
        }
    }
}

void WorkflowPortItem::mousePressEvent ( QGraphicsSceneMouseEvent * event ) {
    dragPoint = QPointF();
    if ((event->buttons() & Qt::LeftButton) && !getWorkflowScene()->isLocked()) {
        dragPoint = event->pos();
        if ((event->modifiers() & portRotationModifier) && !dragging) {
            rotating = true;
            setCursor(QCursor(QPixmap(":workflow_designer/images/rot_cur.png")));
        } else {
            setCursor(Qt::ClosedHandCursor);
        }
    }
    else
    {
        QGraphicsItem::mousePressEvent(event);
    }
}


void WorkflowPortItem::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event ) {
    ungrabMouse();
    unsetCursor();
    QGraphicsItem::mouseReleaseEvent(event);
    rotating = false;
    if (dragging && (event->button() == Qt::LeftButton)) {
        event->accept();
        QList<QGraphicsItem *> li = scene()->items(/*event->scenePos()*/ mapToScene(dragPoint));
        //bool done = false;
        WorkflowPortItem* otherPit = NULL;
        foreach(QGraphicsItem * it, li) {
            WorkflowView *ctl = getWorkflowScene()->getController();
            if (ctl) {
                otherPit = checkBindCandidate(it);
                WorkflowBusItem* dit;
                if (otherPit && (dit = ctl->tryBind(this, otherPit))) {
                    scene()->clearSelection();
                    IntegralBusPort* bp = qobject_cast<IntegralBusPort*>(dit->getInPort()->getPort());
                    if (bp) {
                        bp->setupBusMap();
                    }
                    dit->getInPort()->setSelected(true);
                    break;
                }
            }
        }
        prepareGeometryChange();
        dragging = false;
        dragPoint = QPointF();
        foreach(WorkflowPortItem* it, bindCandidates) {
            it->setHighlight(false);
        }
        scene()->update();
        bindCandidates.clear();
    }
}

void WorkflowPortItem::hoverEnterEvent ( QGraphicsSceneHoverEvent * event ) {
    if (getWorkflowScene()->isLocked()) {
        return;
    }
    setCursor((event->modifiers() & portRotationModifier) ? QCursor(QPixmap(":workflow_designer/images/rot_cur.png")) :
                                                                                                    QCursor(Qt::OpenHandCursor));
}

void WorkflowPortItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *) {
    unsetCursor();
}

QVariant WorkflowPortItem::itemChange ( GraphicsItemChange change, const QVariant & value ) {
    if (change == ItemPositionChange || change == ItemTransformChange) {
        foreach(WorkflowBusItem* dit, flows) {
            dit->prepareGeometryChange();
        }
    } else if (change == ItemPositionHasChanged || change == ItemTransformHasChanged) {
        foreach(WorkflowBusItem* dit, flows) {
            //TODO correct update
            //dit->update(dit->boundingRect());
            dit->updatePos();
        }
    } else if (change == ItemSceneChange && (value.value<QGraphicsScene*>()) == NULL) {
        foreach(WorkflowBusItem* dit, flows) {
            scene()->removeItem(dit);
            delete dit;
        }
    }

    return QGraphicsItem::itemChange(change, value);
}

void WorkflowPortItem::sl_onVisibleChanged(bool isVisible) {
    setVisible(isVisible);
    if(false == isVisible) {
        foreach(WorkflowBusItem* flow, flows) {
            WorkflowScene* ws = getWorkflowScene();
            if(NULL != ws) {
                WorkflowView* view = ws->getController();
                view->removeBusItem(flow);
            }
        }
    }
}

////////////////// Flow //////////////

WorkflowBusItem::WorkflowBusItem(WorkflowPortItem* p1, WorkflowPortItem* p2, Link *link)
: bus(link)
{
    if (p1->getPort()->isInput()) {
        assert(!p2->getPort()->isInput());
        dst = p1;
        src = p2;
    } else {
        assert(p2->getPort()->isInput());
        dst = p2;
        src = p1;
    }

    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setZValue(-1000);

    this->text = new HintItem(src->getPort()->getDisplayName(), this);
    connect(dst->getPort(), SIGNAL(bindingChanged()), this, SLOT(sl_update()));
}

WorkflowBusItem::~WorkflowBusItem()
{
    assert(bus == NULL);
}


void WorkflowBusItem::updatePos() {
    QPointF p1 = dst->headToScene();
    QPointF p2 = src->headToScene();

    setPos((p1.x() + p2.x())/2, (p1.y() + p2.y())/2);
}

QVariant WorkflowBusItem::itemChange ( GraphicsItemChange change, const QVariant & value ) {
    if (change == ItemSceneChange && (value.value<QGraphicsScene*>()) == NULL) {
        dst->removeDataFlow(this);
        src->removeDataFlow(this);
        disconnect(dst->getPort(), SIGNAL(bindingChanged()), this, SLOT(sl_update()));

        WorkflowView *ctl = getWorkflowScene()->getController();
        if (NULL != ctl) {
            ctl->onBusRemoved(bus);
        } else {
            delete bus;
        }
        bus = NULL;
    }

    return QGraphicsItem::itemChange(change, value);
}

void WorkflowBusItem::hoverEnterEvent ( QGraphicsSceneHoverEvent * event ) {
    Q_UNUSED(event);
    setCursor(QCursor(Qt::PointingHandCursor));
}
void WorkflowBusItem::hoverLeaveEvent ( QGraphicsSceneHoverEvent * event ) {
    Q_UNUSED(event);
    unsetCursor();
}

QRectF WorkflowBusItem::boundingRect(void) const {
    QRectF brect(mapFromItem(dst, dst->boundingRect()).boundingRect() | mapFromItem(src, src->boundingRect()).boundingRect());
    QRectF trect(text->boundingRect().translated(text->pos()));
    if (/*getWorkflowScene()->getRunner()*/ true) {
        trect.setTop(trect.top() - trect.height());
    }
    return brect | trect;
}

QPainterPath WorkflowBusItem::shape () const {
    QPainterPath p;
    QPointF p1 = dst->head(this);
    QPointF p2 = src->head(this);
    QLineF direct(p2,p1);
    QLineF n = direct.normalVector();
    n.setLength(A/2);
    p.moveTo(n.p2());
    p.lineTo(n.translated(p1-p2).p2());
    QLineF rn(n.p2(), n.p1());
    rn.translate(n.p1()-n.p2());
    p.lineTo(rn.translated(p1-p2).p2());
    p.lineTo(rn.p2());
    p.closeSubpath();
    p.addRect(text->boundingRect().translated(text->pos()));
    return p;
}

void WorkflowBusItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) {
    Q_UNUSED(widget);
    painter->setRenderHint(QPainter::Antialiasing);
    QColor baseColor(0x66, 0x66, 0x66);
    painter->setPen(baseColor);
    //painter->fillRect(boundingRect(), QBrush(Qt::blue));
    QPointF p1 = dst->head(this);
    QPointF p2 = src->head(this);


    QPainterPath path;
    path.addEllipse(p2, A/2 - 2, A/2 - 2);
    path.addEllipse(p1, A/2 - 2, A/2 - 2);
    painter->fillPath(path, QBrush(baseColor));

    QPen pen = painter->pen();
    if (option->state & QStyle::State_Selected) {
        pen.setWidthF(1.5);
        pen.setStyle(Qt::DashLine);
    }
    if (!validate()) {
        pen.setColor(Qt::red);
    }

    drawArrow(painter, pen, p2, p1);
    //update();

    painter->setRenderHint(QPainter::NonCosmeticDefaultPen);
    QColor yc = QColor(Qt::yellow).lighter();yc.setAlpha(127);
    QRectF textRec = text->boundingRect().translated(text->pos());
    painter->fillRect(textRec, QBrush(yc));
    painter->drawRect(textRec);

    WorkflowAbstractRunner* rt = getWorkflowScene()->getRunner();
    if (rt) {
        int msgsInQueue = rt->getMsgNum(this->bus);
        int passed = rt->getMsgPassed(this->bus);

        QString rts = QString("%1 in queue, %2 passed").arg(msgsInQueue).arg(passed);
        QRectF rtb = textRec.translated(0, -QFontMetricsF(QFont()).height());
        qreal shift = (QFontMetricsF(QFont()).width(rts) - rtb.width()) / 2;
        rtb.setLeft(rtb.left() - shift);
        rtb.setRight(rtb.right() + shift);
        painter->drawText(rtb, Qt::AlignHCenter, rts);
        if (msgsInQueue == 0) {
            return;
        }
        qreal dx = (p2.x() - p1.x()) / msgsInQueue;
        qreal dy = (p2.y() - p1.y()) / msgsInQueue;
        QPointF dp(dx,dy);
        QColor c1("#AA0404");
        painter->setPen(c1);
        c1.setAlphaF(0.8);
        QColor c2(Qt::white);
        c2.setAlphaF(0.8);
        while (msgsInQueue-- > 0) {
            QPainterPath p;
            p.addEllipse(p1, 3,3);
            QRadialGradient rg(p1+QPointF(1.,-1.), 3);

            rg.setColorAt(1, c1);
            rg.setColorAt(0, c2);
            QBrush br(rg);

            painter->fillPath(p,br);
            //painter->drawEllipse(p1, 3,3);
            p1 += dp;
        }
    }
}

void WorkflowBusItem::sl_update() {
    update();
}

bool WorkflowBusItem::validate() {
    NotificationsList lst;
    return dst->getPort()->validate(lst);
}

void WorkflowBusItem::saveState(QDomElement& el) const {
    el.setAttribute("hint-pos", QVariantUtils::var2String(text->pos()));
}
void WorkflowBusItem::loadState(QDomElement& el) {
    if (el.hasAttribute("hint-pos")) {
        QPointF pos = QVariantUtils::String2Var(el.attribute("hint-pos")).toPointF();
        if (!pos.isNull()) {
            text->setPos(pos);
        }
    }
}

WorkflowScene* StyledItem::getWorkflowScene() const
{
    return qobject_cast<WorkflowScene*>(scene());
}

///////////////// WorkflowHighlightItem /////////////////////////////////////////////////////////

const quint8 INIT_ANIMATION_STEPS_NUMBER = 50;
const qreal HALVED_MAXIMUM_SIZE_RELATION_TO_PROCESS_ITEM_SIZE = 0.15;
const QColor BORDER_COLOR = QColor(205, 133, 63);
const QPointF INIT_POSITION = QPointF(0.0, 0.0);

WorkflowHighlightItem::WorkflowHighlightItem(WorkflowProcessItem *owner)
    : StyledItem(owner), countOfAnimationStepsLeft(0) {
    setPos(INIT_POSITION);
    setZValue(owner->zValue());
    setVisible(false);
}

QRectF WorkflowHighlightItem::boundingRect() const {
    WorkflowProcessItem *owner = dynamic_cast<WorkflowProcessItem *>(parentItem());
    const QRectF parentBoundary = owner->getStyleById(owner->getStyle())->boundingRect();
    const qreal sizeFactor = HALVED_MAXIMUM_SIZE_RELATION_TO_PROCESS_ITEM_SIZE
        * countOfAnimationStepsLeft / INIT_ANIMATION_STEPS_NUMBER;

    return parentBoundary.adjusted(-parentBoundary.width() * sizeFactor, -parentBoundary.height()
        * sizeFactor, parentBoundary.width() * sizeFactor, parentBoundary.height() * sizeFactor);
}

void WorkflowHighlightItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/) {
    if(0 != countOfAnimationStepsLeft) {
        painter->setPen(BORDER_COLOR);
        painter->drawRoundedRect(boundingRect(), 5, 5);
        prepareGeometryChange();
        --countOfAnimationStepsLeft;
        if(0 == countOfAnimationStepsLeft) {
            setVisible(false);
        }
    }
}

void WorkflowHighlightItem::replay() {
    countOfAnimationStepsLeft = INIT_ANIMATION_STEPS_NUMBER;
    setVisible(true);
    update();
}

}//namespace
