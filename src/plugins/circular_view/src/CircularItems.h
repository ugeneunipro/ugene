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

#ifndef _CIRCULAR_ITEMS_H_
#define _CIRCULAR_ITEMS_H_

#include <QFont>
#include <QGraphicsPathItem>

#include <U2Core/Annotation.h>

namespace U2 {

class AnnotationSettings;
class CircularAnnotationItem;
class CircularViewRenderArea;
class CircularAnnotationLabel;
class CircularAnnotationRegionItem;

/************************************************************************/
/* CircularAnnotationItem                                               */
/************************************************************************/

class CircularAnnotationItem : public QGraphicsItem {
    Q_DISABLE_COPY(CircularAnnotationItem)
friend class CircularAnnotationRegionItem;
friend class CircularAnnotationLabel;
public:
    CircularAnnotationItem(Annotation *ann, CircularAnnotationRegionItem* region, CircularViewRenderArea* _ra);
    CircularAnnotationItem(Annotation *ann, QList<CircularAnnotationRegionItem*>& regions, CircularViewRenderArea* _ra);
    ~CircularAnnotationItem();
    virtual void paint(QPainter *p,const QStyleOptionGraphicsItem *item,QWidget *widget);
    Annotation * getAnnotation() const;
    virtual QRectF boundingRect() const;
    virtual bool contains(const QPointF &point) const;
    int containsRegion(const QPointF &point);
    CircularAnnotationRegionItem* getContainingRegion(const QPointF & point);

    void setSelected(bool isSelected);
    const QList<CircularAnnotationRegionItem*>& getRegions();
protected:
    bool isSelected;
    QColor color;
    Annotation *annotation;
    QList<CircularAnnotationRegionItem*> regions;
    QRectF _boundingRect;
    CircularViewRenderArea* ra;
};

/************************************************************************/
/* CircularAnnotationRegionItem                                        */
/************************************************************************/
class CircularAnnotationRegionItem : public QGraphicsPathItem{
friend class CircularAnnotationItem;
public:
    CircularAnnotationRegionItem(const QPainterPath& path, bool isShort, int number );
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */);
    CircularAnnotationLabel* getLabel() const;
    void setLabel(CircularAnnotationLabel* label);

    void setArrowCenterPercentage(qreal arrowCenter) { arrowCenterPercentage = arrowCenter; }
    qreal getArrowCenterPercentage() const { return arrowCenterPercentage; }

    int getNumber() const { return number; }

    bool hasJoinedRegion() const { return !joinedRegion.isEmpty(); }
    void setJoinedRegion(const U2Region& reg) { joinedRegion = reg; }
    const U2Region& getJoinedRegion() const { return joinedRegion; }

protected:
    CircularAnnotationItem *parent;
    CircularAnnotationLabel *label;
    int number;
    bool isShort;
    qreal arrowCenterPercentage;
    U2Region joinedRegion;
};

/************************************************************************/
/* CircularAnnotationLabel                                              */
/************************************************************************/
class CircularAnnotationLabel : public QGraphicsItem {
public:
    CircularAnnotationLabel(Annotation *ann, const QVector<U2Region> &annLocation, bool isAutoAnnotation, int _region, int sequenceLength,
        const QFont& font, CircularViewRenderArea* renderArea);

    void paint(QPainter *p,const QStyleOptionGraphicsItem *item,QWidget *widget);
    Annotation * getAnnotation() const;
    QRectF boundingRect() const;
    bool contains(const QPointF &point) const;
    int getRegion() const;
    void setLabelPosition();
    static void setLabelsVisible(QList<CircularAnnotationLabel *> &labelItems);

    void setAnnRegion(CircularAnnotationRegionItem* region);
    bool canFitToTheRegion() const;

    void drawLabelInsideRegion(QPainter *p, bool canFit);
    void drawLabelOutsideRegion(QPainter *p, QPen& pen);

    bool tryPushCounterclockwise();
    bool tryPushClockwise();

    const QPoint & getConnectionStart() const;
    const QPoint & getConnectionEnd() const;

private:
    int findClosestPoint(const QVector<QRect> &rects);
    void calculateSuitablePositions(const QVector<QRect> &rects);
    void calculateConnectionEnd();
    void calculateConnectionStart();
    void engageLabelPosition(int pos);
    void avoidLinesIntersections();

    static bool labelLengthLessThan(CircularAnnotationLabel *l1, CircularAnnotationLabel *l2);

    Annotation *annotation;
    QVector<U2Region> location;
    bool isAutoAnnotation;
    float annotationAngle;
    QFont labelFont;
    QRectF midRect;
    QPointF midRegionPoint;
    int region;

    qreal startA;
    qreal endA;
    qreal spanA;
    QRect engagedLabelPosition;
    QVector<int> suitableLabelPositionIndexes;
    QPoint connectionEnd;
    QPoint connectionStart;
    CircularViewRenderArea* ra;
    CircularAnnotationRegionItem *regionItem;
    bool hasPosition;
    QString labelText;
};

} // namespace U2

#endif // _CIRCULAR_ITEMS_H_
