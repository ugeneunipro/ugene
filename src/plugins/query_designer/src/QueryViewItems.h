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

#ifndef _U2_QUERY_VIEW_ITEMS_H_
#define _U2_QUERY_VIEW_ITEMS_H_

#include <U2Lang/QDScheme.h>
#include <U2Lang/QDConstraint.h>
#include "QueryPalette.h"
#include "QDDocument.h"

#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QGraphicsSceneMouseEvent>
#include <QFontMetricsF>
#include <QKeyEvent>
#include <QFlag>

#define GRID_STEP 40

class QTextDocument;

namespace U2 {

class QueryViewController;
class QueryScene;

enum {
    QDElementType = QGraphicsItem::UserType + 1,
    FootnoteItemType
};

class QDElementDescription : public QGraphicsTextItem {
    Q_OBJECT
public:
    QDElementDescription(QGraphicsItem* parent=NULL);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setHeight(qreal h) { height=h; }
signals:
    void linkClicked(const QString& link);
protected:
    bool sceneEvent(QEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) { event->ignore(); }
private:
    qreal height;
};

class Footnote;
class QDElement : public QGraphicsObject {
    friend class QDElementDescription;
    friend class Footnote;
    friend class QueryViewController;
    Q_OBJECT
public:
    QDElement(QDSchemeUnit* _unit);
    ~QDElement() { assert(links.isEmpty()); }
    void setBoundingRect(const QRectF& r) { bound=r; }
    QRectF boundingRect() const { return bound; }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */);

    void saveState(QDElementStatement* el) const;
    void loadState(QDElementStatement* el);
    //returns constraint's connection point in scene coords
    QPointF getRightConnector();
    QPointF getLeftConnector();
    bool isLinkedWith(QDElement* other, QDDistanceType kind);

    QDActor* getActor() const { return unit->getActor(); }
    QDSchemeUnit* getSchemeUnit() const { return unit; }
    const QList<Footnote*>& getFootnotes() const { return links; }
    QList<Footnote*> getOutcomeFootnotes() const;
    QList<Footnote*> getIncomeFootnotes() const;

    void adaptSize();
    void rememberSize();

    bool highlighted;

    enum {Type = QDElementType};
    int type() const {return Type;}
    enum ResizeFlag {
        ResizeRight         = 1<<1,
        ResizeBottom        = 1<<2,
        ResizeLeft          = 1<<3,
        ResizeTop           = 1<<4,
    };
public slots:
    void sl_refresh();
private slots:
    void sl_onHoverLink(const QString& link);
protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    QVariant itemChange( GraphicsItemChange change, const QVariant & value );
    bool sceneEvent(QEvent *event);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
private:
    void updateDescription();
    void updateFootnotes();
    QString getHeaderString() const;
private:
    QDSchemeUnit* unit;
    QFont font;
    QRectF bound;
    QPointF dragPoint;
    bool dragging;
    QList<Footnote*> links;
    QTextDocument* doc;
    qreal extendedHeight;

    typedef QFlags<ResizeFlag> ResizeFlags;
    ResizeFlags itemResizeFlags;

    QDElementDescription* itemDescription;
};

class Footnote : public QGraphicsObject {
    friend class QDElement;
    Q_OBJECT
public:
    Footnote(QDElement* _from, QDElement* _to, QDDistanceType _distType, QDConstraint* parent, const QFont& _font = QFont());
    ~Footnote();
    void init();
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */);
    void updatePos();

    enum {Type = FootnoteItemType};
    int type() const {return Type;}

    QPointF getSrcPoint() const;
    QPointF getDstPoint() const;
    QDElement* getSrc() const { return from; }
    QDElement* getDst() const { return to; }
    QDElement* adjacent(QDElement* uv) const;
    QDConstraint* getConstraint() const { return constraint; }
    QDDistanceType getDistType() const { return distType; }
public slots:
    void sl_update();
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
private:
    void updateLines(const QPointF& p);
    QString getText() const;

    QDElement*          from;
    QDElement*          to;
    QDDistanceType      distType;
    QDConstraint*       constraint;
    QFont               font;
    bool                draging;
    QGraphicsLineItem*  leftRef;
    QGraphicsLineItem*  rightRef;
};

class QDLabelItem : public QGraphicsTextItem {
    Q_OBJECT
public:
    QDLabelItem(const QString& text="");
    void keyPressEvent(QKeyEvent *event);
    void focusOutEvent(QFocusEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
signals:
    void si_editingFinished();
};

class QDDescriptionItem : public QGraphicsTextItem {
    Q_OBJECT
public:
    QDDescriptionItem(const QString& text="");
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    bool sceneEvent(QEvent *event);
    void focusOutEvent(QFocusEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    QPainterPath shape() const;
private:
    int resize;
};

class QDRulerItem : public QGraphicsObject {
    Q_OBJECT
public:
    QDRulerItem() : leftPos(0), rightPos(0) {}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */);
    QRectF boundingRect() const;
public slots:
    void sl_updateGeometry();
    void sl_updateText();
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
private:
    QRectF txtBound() const;
private:
    qreal leftPos;
    qreal rightPos;
    QString text;
    QFont font;
};

}//namespace

Q_DECLARE_METATYPE(U2::QDElement*);

#endif
