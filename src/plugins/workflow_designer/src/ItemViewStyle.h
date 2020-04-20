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

#ifndef _U2_WORKFLOW_ITEM_STYLE_H_
#define _U2_WORKFLOW_ITEM_STYLE_H_

#include <QTextDocument>
#include <QAction>
#include <QGraphicsScene>
#include <QGraphicsItem>

class QDomElement;

namespace U2 {

class WorkflowProcessItem;

const qreal R = 30;
const qreal A = 8;

class ItemViewStyle : public QGraphicsObject {
    Q_OBJECT
public:
    ItemViewStyle(WorkflowProcessItem* p, const QString& id);
    QString getId() const {return id;}

    virtual void refresh() {}
    virtual bool sceneEventFilter(QGraphicsItem *, QEvent *) {return false;}
    virtual QList<QAction*> getContextMenuActions() const {return (QList<QAction*>() << bgColorAction << fontAction);}
    virtual void saveState(QDomElement& ) const;
    virtual void loadState(QDomElement& );
    virtual QColor defaultColor() const = 0;
    QColor getBgColor() const {return bgColor;}
    void setBgColor(const QColor & color) {bgColor = color;}
    QFont defaultFont() const {return defFont;}
    void setDefaultFont(const QFont & font) { defFont = font; }
    WorkflowProcessItem const* getOwner() const { return owner; }

protected:
    WorkflowProcessItem* owner;
    QColor bgColor;
    QFont defFont;

    QAction* bgColorAction;
    QAction* fontAction;
    QString id;

private slots:
    void selectBGColor();
    void selectFont();
};

class SimpleProcStyle : public ItemViewStyle {
public:
    SimpleProcStyle(WorkflowProcessItem* pit);
    QRectF boundingRect(void) const;
    QPainterPath shape() const;
    QColor defaultColor() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
};

class DescriptionItem;

class ExtendedProcStyle : public ItemViewStyle {
    Q_OBJECT
public:
    ExtendedProcStyle(WorkflowProcessItem* pit);
    QRectF boundingRect(void) const {return bounds;}
    QPainterPath shape () const;
    QColor defaultColor() const;
    void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
    virtual void refresh();
    virtual bool sceneEventFilter(QGraphicsItem * watched, QEvent * event);
    virtual QList<QAction*> getContextMenuActions() const;
    virtual void saveState(QDomElement& ) const;
    virtual void loadState(QDomElement& );

    bool isAutoResized() const {return autoResize;}
    void setFixedBounds(const QRectF& b);

    bool updateCursor(const QPointF& pos);

signals:
    void linkActivated(const QString&);

private slots:
    void setAutoResizeEnabled(bool b);
    void linkHovered(const QString&);

private:
    QTextDocument* doc;
    QRectF bounds;
    bool autoResize;

    enum ResizeMode {NoResize = 0, RightResize = 1, LeftResize = 2, BottomResize = 4, TopResize = 8,
        RBResize = RightResize + BottomResize, RTResize = RightResize + TopResize,
        LBResize = LeftResize + BottomResize, LTResize = LeftResize + TopResize};
    int resizing;

    QAction* resizeModeAction;
    DescriptionItem* desc;
    friend class DescriptionItem;
};

class HintItem : public QGraphicsTextItem {
public:
    HintItem(const QString & text, QGraphicsItem * parent);
protected:
    virtual QVariant itemChange ( GraphicsItemChange change, const QVariant & value );
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
private:
    QPointF initPos;
    bool dragging;
};

class DescriptionItem : public QGraphicsTextItem {
    Q_OBJECT
public:
    DescriptionItem(ExtendedProcStyle* p);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    bool sceneEvent(QEvent *event);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
};

}//namespace

#endif
