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

#pragma once

#include <QAction>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QWidget>

#include "U2Lang/LocalDomain.h"
#include <U2Lang/ActorModel.h>

class QDomElement;

namespace U2 {
using namespace Workflow;
class WorkflowScene;
class ItemViewStyle;
class WorkflowHighlightItem;

typedef QString StyleId;

#define GRID_STEP 15

inline qreal round(qreal val, int step) {
    int tmp = int(val) + step / 2;
    tmp -= tmp % step;
    return qreal(tmp);
}
inline qreal roundUp(qreal val, int step) {
    int tmp = int(val) + step / 2;
    tmp -= tmp % step;
    if (tmp < int(val)) {
        tmp += step;
    }
    return qreal(tmp);
}

class ItemStyles {
public:
    static const StyleId SIMPLE;
    static const StyleId EXTENDED;
};

class StyledItem : public QGraphicsItem {
public:
    StyledItem(QGraphicsItem* p = nullptr)
        : QGraphicsItem(p) {
    }
    WorkflowScene* getWorkflowScene() const;
    virtual void setStyle(StyleId) {
    }
    virtual StyleId getStyle() const {
        return ItemStyles::SIMPLE;
    }
    virtual QList<QAction*> getContextMenuActions() const {
        return QList<QAction*>();
    }
};

class WorkflowPortItem;
class WorkflowBusItem;
enum {
    WorkflowProcessItemType = QGraphicsItem::UserType + 1,
    WorkflowPortItemType,
    WorkflowBusItemType
};

enum InspectionItemOrientationType {
    InspectionLeft,
    InspectionRight,
    InspectionUp,
    InspectionDown
};

class WorkflowProcessItem : public QObject, public StyledItem {
    Q_OBJECT
public:
    WorkflowProcessItem(Actor* process);
    ~WorkflowProcessItem() override;
    Actor* getProcess() const {
        return process;
    }
    WorkflowPortItem* getPort(const QString& id) const;
    QList<WorkflowPortItem*> getPortItems() const {
        return ports;
    }
    QRectF boundingRect() const override;
    QRectF portsBoundingRect() const;
    QPainterPath shape() const override;

    void setStyle(StyleId) override;
    StyleId getStyle() const override {
        return styles.key(currentStyle);
    }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    enum { Type = WorkflowProcessItemType };
    int type() const override {
        return Type;
    }
    void prepareUpdate() {
        prepareGeometryChange();
    }

    QList<QAction*> getContextMenuActions() const override;

    void saveState(QDomElement&) const;
    void loadState(QDomElement&);

    ItemViewStyle* getStyleByIdSafe(StyleId id) const;
    ItemViewStyle* getStyleById(const StyleId& id) const;
    bool containsStyle(const StyleId& id) const;
    void updatePorts();

    void toggleBreakpointState();
    void toggleBreakpoint();
    bool isBreakpointInserted() const;
    bool isBreakpointEnabled() const;
    void highlightItem();

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    bool sceneEvent(QEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

public slots:
    void sl_update();

private slots:
    void sl_descriptionChanged();

private:
    void createPorts();
    QMap<StyleId, ItemViewStyle*> styles;
    ItemViewStyle* currentStyle;
    Actor* process;
    QList<WorkflowPortItem*> ports;
    QMap<QGraphicsItem*, QPointF> initialPositions;
    bool hasBreakpoint;
    bool hasEnabledBreakpoint;
    WorkflowHighlightItem* highlighting;
};

class WorkflowPortItem : public QObject, public StyledItem {
    Q_OBJECT
public:
    WorkflowPortItem(WorkflowProcessItem* owner, Port* port);
    ~WorkflowPortItem() override;
    Port* getPort() const {
        return port;
    }
    WorkflowProcessItem* getOwner() const {
        return owner;
    }
    QList<WorkflowBusItem*> getDataFlows() const {
        return flows;
    }
    WorkflowBusItem* getDataFlow(const WorkflowPortItem* other) const;
    WorkflowPortItem* checkBindCandidate(const QGraphicsItem* it) const;
    WorkflowPortItem* findNearbyBindingCandidate(const QPointF& at) const;
    void addDataFlow(WorkflowBusItem* flow);
    void removeDataFlow(WorkflowBusItem* flow);
    // position of the arrow tip in items coordinates
    QPointF head(const QGraphicsItem* item) const;
    // position of the arrow tip in scene coordinates
    QPointF headToScene() const;
    // direction of the arrow in items coordinates
    QLineF arrow(const QGraphicsItem* item) const;
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

    qreal getOrientarion() const {
        return orientation;
    }
    void setOrientation(qreal);

    void setStyle(StyleId) override;
    StyleId getStyle() const override {
        return currentStyle;
    }
    void adaptOwnerShape();

    void setHighlight(bool v) {
        highlight = v;
    }

    enum { Type = WorkflowPortItemType };
    int type() const override {
        return Type;
    }

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
public slots:
    void sl_onVisibleChanged(bool);

private:
    StyleId currentStyle;

    Port* port;
    WorkflowProcessItem* owner;
    qreal orientation;
    QList<WorkflowBusItem*> flows;
    bool dragging;
    bool rotating;
    bool sticky;
    bool highlight;
    bool mouseMoveIsBeingProcessed;  // the field is needed to prevent a recursion during mouse events processing
    QList<WorkflowPortItem*> bindCandidates;
    QPointF dragPoint;
};

class WorkflowBusItem : public QObject, public StyledItem {
    Q_OBJECT
    friend class WorkflowPortItem;
    friend class WorkflowProcessItem;

public:
    WorkflowBusItem(WorkflowPortItem* p1, WorkflowPortItem* p2, Link* link);
    ~WorkflowBusItem() override;
    WorkflowPortItem* getInPort() const {
        return dst;
    }
    WorkflowPortItem* getOutPort() const {
        return src;
    }
    Link* getBus() const {
        return bus;
    }
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    enum { Type = WorkflowBusItemType };
    int type() const override {
        return Type;
    }
    bool validate();

    void saveState(QDomElement&) const;
    void loadState(QDomElement&);

    QGraphicsItem* getText() const {
        return text;
    }
    void updatePos();

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    // void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
    // void mousePressEvent ( QGraphicsSceneMouseEvent * event );
    // void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private slots:
    void sl_update();

private:
    Link* bus;
    WorkflowPortItem *dst, *src;
    QGraphicsItem* text;
    // bool dragging;
    // QPointF dragPoint;
};

class WorkflowHighlightItem : public StyledItem {
public:
    WorkflowHighlightItem(WorkflowProcessItem* owner);
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

    void replay();

private:
    quint8 countOfAnimationStepsLeft;
};

}  // namespace U2

Q_DECLARE_METATYPE(U2::StyleId);
