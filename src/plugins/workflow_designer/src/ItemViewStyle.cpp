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

#include <QAbstractTextDocumentLayout>
#include <QBitmap>
#include <QColorDialog>
#include <QDomDocument>
#include <QFontDialog>
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
#include <U2Lang/WorkflowSettings.h>

#include "WorkflowViewItems.h"
#include "WorkflowViewController.h"
#include "ItemViewStyle.h"

namespace U2 {

const StyleId ItemStyles::SIMPLE = "simple";
const StyleId ItemStyles::EXTENDED = "ext";

#define BGC QString("-bgc")
#define FONT QString("-font")

const QColor ITEM_WITH_ENABLED_BREAKPOINT_BORDER_COLOR = QColor(178, 34, 34);
const QColor ITEM_WITH_DISABLED_BREAKPOINT_BORDER_COLOR = QColor(184, 134, 11);

ItemViewStyle::ItemViewStyle(WorkflowProcessItem* p, const QString& id) : QGraphicsObject(p),
    defFont(WorkflowSettings::defaultFont()), id(id)
{
    setVisible(false);
    bgColorAction = new QAction(tr("Background color"), this);
    connect(bgColorAction, SIGNAL(triggered()), SLOT(selectBGColor()));

    fontAction = new QAction(tr("Font"), this);
    connect(fontAction, SIGNAL(triggered()), SLOT(selectFont()));
}

void ItemViewStyle::selectBGColor() {
    QColor res = QColorDialog::getColor(bgColor, owner->scene()->views().first());
    if (res.isValid()) {
        bgColor = res;
        WorkflowScene* sc = qobject_cast<WorkflowScene*>(owner->scene());
        if(sc != NULL) {
            sc->setModified(true);
        }
    }
}

void ItemViewStyle::selectFont() {
    bool ok;
    QFont res = QFontDialog::getFont(&ok, defFont, owner->scene()->views().first(), tr("Characters Font"), QFontDialog::DontUseNativeDialog);
    if (ok) {
        defFont = res;
        WorkflowScene* sc = qobject_cast<WorkflowScene*>(owner->scene());
        if(sc != NULL) {
            sc->setModified(true);
        }
    }
}

void ItemViewStyle::saveState(QDomElement& el) const {
    if (bgColor != defaultColor())
        el.setAttribute(id+BGC, QVariantUtils::var2String(bgColor));
    if (defFont != QFont())
        el.setAttribute(id+FONT, defFont.toString());
}

void ItemViewStyle::loadState(QDomElement& el) {
    if (el.hasAttribute(id+BGC)) {
        QColor bgc = QVariantUtils::String2Var(el.attribute(id+BGC)).value<QColor>();
        if (bgc.isValid()) {
            bgColor = bgc;
        }
    }
    if (el.hasAttribute(id+FONT)) {
        defFont.fromString(el.attribute(id+FONT));
    }
}

SimpleProcStyle::SimpleProcStyle(WorkflowProcessItem* pit) : ItemViewStyle(pit, ItemStyles::SIMPLE) {
    owner = (pit);
    owner->connect(owner->getProcess(), SIGNAL(si_labelChanged()), SLOT(sl_update()));
    bgColor = defaultColor();
}

QColor SimpleProcStyle::defaultColor() const {
    /*QColor ret(Qt::darkCyan);
    ret.setAlpha(200);*/
    QColor ret = WorkflowSettings::getBGColor();
    return ret;
}

QRectF SimpleProcStyle::boundingRect(void) const {
    // extra space added for clean antialiased painting
    return QRectF(-R-2, -R-2, R*2+4, R*2+4);
}

QPainterPath SimpleProcStyle::shape () const {
    QPainterPath contour;
    contour.addEllipse(QPointF(0,0), R, R);
    return contour;
}

void SimpleProcStyle::paint(QPainter *painter,
                                const QStyleOptionGraphicsItem *option,
                                QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    //painter->fillRect(boundingRect(), QBrush(Qt::magenta, Qt::Dense6Pattern));
    painter->setRenderHint(QPainter::Antialiasing);
    QPainterPath contour;
    contour.addEllipse(QPointF(0,0), R, R);

    QPen pen;
    if(owner->isBreakpointInserted()) {
        const QColor borderColor = (owner->isBreakpointEnabled())
            ? ITEM_WITH_ENABLED_BREAKPOINT_BORDER_COLOR
            : ITEM_WITH_DISABLED_BREAKPOINT_BORDER_COLOR;
        pen.setColor(borderColor);
    }
    if (owner->isSelected()) {
        pen.setWidthF(2);
        pen.setStyle(Qt::DashLine);
    }
    painter->setPen(pen);

    QRadialGradient rg(R/2, -R/2, R*2);
    rg.setColorAt(1, bgColor);
    rg.setColorAt(0, QColor(Qt::white));
    QBrush procBrush(rg);
    painter->drawPath(contour);
    painter->fillPath(contour, procBrush);

    painter->save();
    QTextDocument d;
    d.setDefaultFont(defFont);
    d.setHtml("<center>" + owner->getProcess()->getLabel().toHtmlEscaped() + "</center>");
    d.setTextWidth(R*2);
    //d.setDefaultTextOption(QTextOption(Qt::AlignHCenter));
    painter->translate(-d.size().width()/2, -d.size().height()/2);
    //painter->translate(-R, -R);
    d.drawContents(painter, QRectF(0,0,2*R,2*R));
    painter->restore();
}

//QPainterPath shape () const;

#define MARGIN 5

ExtendedProcStyle::ExtendedProcStyle(WorkflowProcessItem* pit) : ItemViewStyle(pit, ItemStyles::EXTENDED),
autoResize(true), resizing(NoResize) {
    owner = (pit);
    Actor* process = pit->getProcess();

    doc = process->getDescription();
    if (doc) {
        owner->connect(doc, SIGNAL(contentsChanged()), SLOT(sl_update()));
    } else {
        doc = new QTextDocument(pit);
        doc->setHtml(QString("<center><b>%1</b></center><hr>%2<br>aLSKDJALSK LASDJ LASKD LASJD ALSKDJ XCKLJSLC Jas dlkjsdf sdlkjsdlfj sdlkfjlsdkfjs dlkfjsdlkfjsld flsdkjflsd kfjlsdkfj lsdkfjlsd flskfjsldkfjsldf jsdlkfjsdlkfjsdlfkjsdlfj")
            .arg(process->getLabel()).arg(process->getProto()->getDocumentation()));
    }
    owner->connect(fontAction, SIGNAL(triggered()), SLOT(sl_update()));
    desc = new DescriptionItem(this);
    refresh();

    resizeModeAction = new QAction(tr("Auto-resize to text"), this);
    resizeModeAction->setCheckable(true);
    resizeModeAction->setChecked(autoResize);
    connect(resizeModeAction, SIGNAL(toggled(bool)), SLOT(setAutoResizeEnabled(bool)));

    bgColor = defaultColor();
}

QColor ExtendedProcStyle::defaultColor() const {
    return WorkflowSettings::getBGColor();
}

#define MINW 2*R
#define MAXW 6*R

void ExtendedProcStyle::refresh() {
    doc->setDefaultFont(defFont);
    if (autoResize) {
        bool snap2grid = WorkflowSettings::snap2Grid();
        qreal w,h;
        int cycle = 0;
        do {
            QSizeF docFrame = doc->size();
            w = docFrame.width() + MARGIN*2;
            if (snap2grid) {
                w = roundUp(w, GRID_STEP);
                doc->setTextWidth(w - MARGIN*2);
                docFrame = doc->size();
            }
            h = qMax(2*R, docFrame.height()) + MARGIN*2;
            if (snap2grid) {
                h = roundUp(h, GRID_STEP);
            }
            //printf("ideal=%f, actual=%f\n",doc->idealWidth(),w);

            // try to improve docFrame proportions
            if (++cycle > 2) {
                break;
            }
            if ((h/w < 0.6 && w > (MINW+MAXW)/2)  //doc is disproportionately wide
                || (h/w > 1.6 && w < MAXW)        //doc is disproportionately long and can be widen
                || (w < MINW || w > MAXW)) {      //width is out of bounds
                doc->setTextWidth(qBound(MINW, (qreal)(h/1.6), MAXW-MARGIN*2));
            }
        } while (true);

        bounds = QRectF(-R, -R, w, h);
    } else {
        //bounds.setSize(bounds.size().expandedTo(doc->size() + QSizeF(MARGIN*2,MARGIN*2)));
    }
    desc->setDocument(doc);
}

QPainterPath ExtendedProcStyle::shape () const {
    QPainterPath contour;
    contour.addRoundedRect(bounds, MARGIN, MARGIN);
    return contour;
}


void ExtendedProcStyle::paint(QPainter *painter,
                            const QStyleOptionGraphicsItem *option,
                            QWidget *)
{
    if (owner->isSelected()) {
        ((QStyleOptionGraphicsItem*)option)->state |= QStyle::State_Selected;
    }
    bgColor.setAlpha(64);
    QRectF tb = boundingRect();
    painter->fillRect(tb, QBrush(bgColor));

    painter->setRenderHint(QPainter::Antialiasing);

    if (doc->pageCount() > 1) {
        QPointF tp = bounds.bottomRight();

        // draw a page corner
        //QPen pen;
        //pen.setWidthF(1.2);
        //painter->setPen(pen);
        //qreal len = 6;
        //painter->drawLine(tp.x() - len, tp.y() - len, tp.x(), tp.y() - len);
        //painter->drawLine(tp.x() - len, tp.y() - len, tp.x() - len, tp.y());

        // draw 3 dots at corner
        QPointF dt(tp.x() - 7, tp.y() - 5);
        QPainterPath dot; dot.addEllipse(dt, 1.2,1.2);
        dot.addEllipse(dt - QPointF(4,0), 1.2,1.2);
        dot.addEllipse(dt - QPointF(8,0), 1.2,1.2);
        painter->fillPath(dot, QBrush(QColor(0x33, 0x33, 0x33)));
    }

    QPen pen;
    pen.setWidthF(1.3);
    if (owner->isSelected()) {
        pen.setStyle(Qt::DashLine);
    }
    if(owner->isBreakpointInserted()) {
        const QColor borderColor = (owner->isBreakpointEnabled())
            ? ITEM_WITH_ENABLED_BREAKPOINT_BORDER_COLOR
            : ITEM_WITH_DISABLED_BREAKPOINT_BORDER_COLOR;
        pen.setColor(borderColor);
    }

    painter->setPen(pen);
    painter->drawRoundedRect(tb, MARGIN, MARGIN);
}

#define RESIZE_AREA 4
bool ExtendedProcStyle::sceneEventFilter( QGraphicsItem * watched, QEvent * event )
{
    assert(watched == owner);
    Q_UNUSED(watched);

    bool ret = false;

    switch (event->type()) {
    case QEvent::GraphicsSceneHoverEnter:
    case QEvent::GraphicsSceneHoverMove:
        {
            QGraphicsSceneHoverEvent* he = dynamic_cast<QGraphicsSceneHoverEvent*>(event);
            ret = updateCursor(he->pos());
        }
        break;
    case QEvent::GraphicsSceneMouseRelease:
        desc->mouseReleaseEvent(dynamic_cast<QGraphicsSceneMouseEvent *>(event));
    case QEvent::GraphicsSceneHoverLeave:
        if (resizing) {
            owner->unsetCursor();
        }
        resizing = NoResize;
        break;
    case QEvent::GraphicsSceneMouseMove:
        if (resizing && event->spontaneous()) {
            QGraphicsSceneMouseEvent* me = (dynamic_cast<QGraphicsSceneMouseEvent *>(event));
            WorkflowSettings::setSnap2Grid(false);
            QPointF newPos;
            if ((me->buttons() & Qt::LeftButton)) {
                ret = true;
                QRectF b2 = bounds;
                QPointF p = me->pos();
                QPointF p2 = p - me->lastPos();

                if (resizing & RightResize
                    &&  // border is either "pulled" or "pushed" by mouse pointer
                        // in the latter case pointer should be close to the border
                        ((p2.x() < 0 && b2.right() > p.x())
                        || (p2.x() > 0 && b2.right() < p.x())
                        || (qAbs(b2.right() - p.x()) < RESIZE_AREA)))
                {

                    qreal rb = b2.right() + p2.x();
                    b2.setRight(rb);

                    owner->updatePorts();

                    if (b2.width() < MARGIN*2 + R) {
                        return true;
                    }
                }

                if (resizing & LeftResize
                    &&  ((p2.x() < 0 && b2.left() > p.x())
                        || (p2.x() > 0 && b2.left() < p.x())
                        || (qAbs(b2.left() - p.x()) < RESIZE_AREA)))
                {
                    b2.setWidth(b2.width() - p2.x());
                    newPos = owner->scenePos();
                    newPos.setX(newPos.x() - (b2.width() - bounds.width()));

                    if (b2.width() < MARGIN*2 + R) {
                        return true;
                    }

                    setFixedBounds(b2);
                    owner->setPos(newPos);
                }

                if(resizing & TopResize &&
                    ((p2.y() < 0 && b2.top() > p.y())
                    || (p2.y() > 0 && b2.top() < p.y())
                    || (qAbs(b2.top() - p.y()) < RESIZE_AREA))) {

                    b2.setHeight(b2.height() - p2.y());

                    newPos = owner->scenePos();
                    newPos.setY(newPos.y() - (b2.height() - bounds.height()));

                    qreal minHeight = R + MARGIN*2;

                    WorkflowScene * sc = qobject_cast<WorkflowScene*>(owner->scene());
                    if (b2.height() < minHeight || newPos.y() < sc->sceneRect().top()) {
                        return true;
                    }

                    setFixedBounds(b2);
                    owner->setPos(newPos);
                }

                if (resizing & BottomResize
                    && ((p2.y() < 0 && b2.bottom() > p.y())
                        || (p2.y() > 0 && b2.bottom() < p.y())
                        || (qAbs(b2.bottom() - p.y()) < RESIZE_AREA)))
                {
                    b2.setBottom(b2.bottom() + p2.y());
                    owner->updatePorts();
                }
                //qreal minHeight = qMax(doc->size().height(), R) + MARGIN*2;
                qreal minHeight = R + MARGIN*2;
                if (b2.height() < minHeight) {
                    b2.setHeight(minHeight);
                }

                setFixedBounds(b2);

                WorkflowScene * sc = qobject_cast<WorkflowScene*>(owner->scene());
                if(sc != NULL) {
                    sc->setModified(true);
                }
            }
        }
        break;
    /*case QEvent::GraphicsSceneMousePress:
    mousePressEvent(static_cast<QGraphicsSceneMouseEvent *>(event));
    break;
    case QEvent::GraphicsSceneMouseDoubleClick:
    mouseDoubleClickEvent(static_cast<QGraphicsSceneMouseEvent *>(event));
    break;
    case QEvent::GraphicsSceneWheel:
    wheelEvent(static_cast<QGraphicsSceneWheelEvent *>(event));
    break;
    case QEvent::KeyPress:
    keyPressEvent(static_cast<QKeyEvent *>(event));
    break;
    case QEvent::KeyRelease:
    keyReleaseEvent(static_cast<QKeyEvent *>(event));
    break;
    case QEvent::InputMethod:
    inputMethodEvent(static_cast<QInputMethodEvent *>(event));
    break;*/
    default:
        return false;
    }

    return ret;
}

bool ExtendedProcStyle::updateCursor(const QPointF& p) {
    bool ret = false;
    resizing = NoResize;
    qreal dx = qAbs(bounds.right() - p.x());
    qreal dy = qAbs(bounds.bottom() - p.y());
    if (dx < RESIZE_AREA) {
        resizing |= RightResize;
    }
    if(dx > (bounds.width() - RESIZE_AREA)) {
        resizing |= LeftResize;
    }
    if (dy < RESIZE_AREA) {
        resizing |= BottomResize;
    }
    if(dy > (bounds.height() - RESIZE_AREA)) {
        resizing |= TopResize;
    }

    switch (resizing)
    {
    case NoResize:
        owner->unsetCursor();
        break;
    case RightResize:
    case LeftResize:
        owner->setCursor(Qt::SizeHorCursor);
        break;
    case BottomResize:
    case TopResize:
        owner->setCursor(Qt::SizeVerCursor);
        break;
    case RBResize:
    case LTResize:
        owner->setCursor(Qt::SizeFDiagCursor);
        break;
    case LBResize:
    case RTResize:
        owner->setCursor(Qt::SizeBDiagCursor);
        break;
    }
    ret = (resizing != NoResize);
    return ret;
}

void ExtendedProcStyle::setFixedBounds( const QRectF& b)
{
    doc->setPageSize(b.size() - QSizeF(MARGIN*2,MARGIN*2));
    if (bounds != b) {
        bounds = b;
        owner->prepareUpdate();

        foreach(WorkflowPortItem* pit, owner->getPortItems()) {
            pit->adaptOwnerShape();
        }
    }
    owner->update();
    resizeModeAction->setChecked(false);
}

void ExtendedProcStyle::setAutoResizeEnabled(bool b) {
    autoResize = b;
    if (autoResize) {
        doc->setPageSize(QSizeF(-1,-1));
        owner->sl_update();
    }
}

QList<QAction*> ExtendedProcStyle::getContextMenuActions() const
{
    QList<QAction*> ret;
    ret << resizeModeAction << bgColorAction << fontAction;
    return ret;
}

#define ARM QString("arm")
#define BOUNDS QString("bounds")

void ExtendedProcStyle::saveState(QDomElement& el) const {
    //el.setAttribute(ARM, autoResize);
    if (!autoResize) {
        el.setAttribute(BOUNDS, QVariantUtils::var2String(bounds));
    }
    ItemViewStyle::saveState(el);
}

void ExtendedProcStyle::loadState(QDomElement& el) {
    if (el.hasAttribute(BOUNDS)) {
        QRectF b = QVariantUtils::String2Var(el.attribute(BOUNDS)).toRectF();
        if (!b.isNull()) {
            setFixedBounds(b);
        }
    }
    ItemViewStyle::loadState(el);
}

void ExtendedProcStyle::linkHovered(const QString& url) {
    if (url.isEmpty()) {
        owner->unsetCursor();
    } else {
        owner->setCursor(Qt::PointingHandCursor);
    }
}

HintItem::HintItem( const QString & text, QGraphicsItem * parent)
: QGraphicsTextItem(text, parent), dragging(false) {
    setFlag(QGraphicsItem::ItemIsSelectable);
#if (QT_VERSION >= QT_VERSION_CHECK(4, 6, 0))
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
#endif
    document()->setDefaultTextOption(QTextOption(Qt::AlignCenter));
    setTextWidth(qMin(3*R, document()->idealWidth()));
    QRectF tb = boundingRect();
    setPos(-tb.width()/2, -tb.height()- 3);
    setDefaultTextColor(QColor(Qt::gray).darker());
    QFont f = font();
    f.setWeight(QFont::Light);
    setFont(f);
}

QVariant HintItem::itemChange( GraphicsItemChange change, const QVariant & value )
{
    if (change == ItemSelectedChange && value.toBool()) {
        parentItem()->setSelected(true);
        return false;
    }
    if (change == ItemPositionChange) {
        QPointF newPos = value.toPointF();
        if (scene()) {
            QRectF bound = boundingRect();
            QRectF sceneRect = scene()->sceneRect();
            // scene topLeft in parent coords
            QPointF tl = mapToParent(mapFromScene(sceneRect.topLeft()));
            sceneRect.moveTopLeft(tl);

            qreal x0 = sceneRect.left() - bound.left();
            qreal x1 = sceneRect.left() + sceneRect.width() - bound.right();
            qreal y0 = sceneRect.top() - bound.top();
            qreal y1 = sceneRect.top() + sceneRect.height() - bound.bottom();

            newPos.setX( qBound(x0, newPos.x(), x1) );
            newPos.setY( qBound(y0, newPos.y(), y1) );
        }
        return newPos;
    }
    if (change == ItemPositionHasChanged) {
        parentItem()->update();
        if (scene()) {
            foreach(QGraphicsView* v, scene()->views()) {
                v->ensureVisible(this, 0, 0);
            }
        }
    }
    return QGraphicsItem::itemChange(change, value);
}

void HintItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        if (!dragging) {
            initPos = pos();
            dragging = true;
        }

        QPointF delta = event->screenPos() - event->buttonDownScreenPos(Qt::LeftButton);
        setPos(initPos + delta);
    } else {
        event->ignore();
    }
}

void HintItem::mouseReleaseEvent( QGraphicsSceneMouseEvent *event ) {
    dragging = false;
    QGraphicsTextItem::mouseReleaseEvent(event);
}

DescriptionItem::DescriptionItem(ExtendedProcStyle* p) : QGraphicsTextItem(p) {
    setPos(-R + MARGIN, -R + MARGIN);
    setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::LinksAccessibleByKeyboard);
    p->connect(this, SIGNAL(linkActivated(const QString&)), SIGNAL(linkActivated(const QString&)));
    p->connect(this, SIGNAL(linkHovered(const QString&)), SLOT(linkHovered(const QString&)));
}

QRectF DescriptionItem::boundingRect() const {
    QRectF bounds = parentItem()->boundingRect();
    bounds.setBottomRight(bounds.bottomRight() - QPointF(MARGIN, MARGIN));
    bounds.translate(R - MARGIN, R - MARGIN);
    return bounds;
}

void DescriptionItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QStyleOptionGraphicsItem deselectedOption = *option;
    deselectedOption.state &= ~(QStyle::State_Selected | QStyle::State_HasFocus);
    QGraphicsTextItem::paint(painter, &deselectedOption, widget);
}

void DescriptionItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    event->setPos(mapFromParent(event->pos()));
    QGraphicsTextItem::mouseReleaseEvent(event);
}

bool DescriptionItem::sceneEvent(QEvent *event) {
    switch (event->type()) {
        case QEvent::GraphicsSceneHoverMove:
        case QEvent::GraphicsSceneHoverEnter:
            {
                ExtendedProcStyle* owner = qgraphicsitem_cast<ExtendedProcStyle*>(parentItem());
                if (owner->resizing) {
                    QGraphicsSceneHoverEvent* he = static_cast<QGraphicsSceneHoverEvent*>(event);
                    const QPointF& p = mapToParent(he->pos());
                    owner->updateCursor(p);
                }
            }
            break;
        default:
            break;
    }
    return QGraphicsTextItem::sceneEvent(event);
}

void DescriptionItem::contextMenuEvent( QGraphicsSceneContextMenuEvent *event ) {
    QAbstractTextDocumentLayout* layout = document()->documentLayout();
    const QString& href = layout->anchorAt(event->pos());

    if (href.isEmpty()) {
        event->ignore();
        return;
    }

    ItemViewStyle* style = qgraphicsitem_cast<ItemViewStyle*>(parentItem());
    WorkflowProcessItem const* procItem = style->getOwner();
    Actor* actor = procItem->getProcess();
    WorkflowScene* ws = procItem->getWorkflowScene();
    ws->setupLinkCtxMenu(href, actor, event->screenPos());
}

}//namespace
