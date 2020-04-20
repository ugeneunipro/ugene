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

#include <math.h>
#include <limits>

#include <QGraphicsLineItem>

#include <U2Algorithm/GeomUtils.h>

#include <U2Core/AnnotationSettings.h>
#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/U2SafePoints.h>

#include "CircularItems.h"
#include "CircularView.h"
#include "CircularViewPlugin.h"

namespace U2 {

/************************************************************************/
/* CircularAnnotationItem                                               */
/************************************************************************/

CircularAnnotationItem::CircularAnnotationItem(Annotation *ann, CircularAnnotationRegionItem* region, CircularViewRenderArea* _ra)
    : annotation(ann), ra(_ra)
{
    assert(region->parent == NULL);
    region->parent = this;
    regions.append(region);
    _boundingRect = region->boundingRect();
    isSelected = false;
}

CircularAnnotationItem::CircularAnnotationItem(Annotation *ann, QList<CircularAnnotationRegionItem*>& _regions, CircularViewRenderArea* _ra)
    : annotation(ann), regions(_regions), ra(_ra)
{
    isSelected = false;
    QPainterPath path;
    foreach(CircularAnnotationRegionItem* item, regions) {
        assert(item->parent == NULL);
        item->parent = this;
        path = path.united(item->path());
    }
    _boundingRect = path.boundingRect();
}


CircularAnnotationItem::~CircularAnnotationItem()
{
    foreach(CircularAnnotationRegionItem* item, regions) {
        delete item->label;
        delete item;
    }
    regions.clear();
}

void CircularAnnotationItem::paint(QPainter *p,const QStyleOptionGraphicsItem *item, QWidget *widget) {
    Q_UNUSED(item);
    AnnotationSettingsRegistry *asr = AppContext::getAnnotationsSettingsRegistry();
    AnnotationSettings *as = asr->getAnnotationSettings(annotation->getData());
    this->color = as->color;
    foreach (CircularAnnotationRegionItem *item, regions) {
        item->paint(p, NULL, widget);
    }
}

QRectF CircularAnnotationItem::boundingRect() const {
    return _boundingRect;
}

Annotation * CircularAnnotationItem::getAnnotation() const {
    return annotation;
}

bool CircularAnnotationItem::contains(const QPointF &point) const {
    if(_boundingRect.contains(point)) {
        foreach(CircularAnnotationRegionItem* item, regions)
            if(item->contains(point)) {
                return true;
            }
    }
    return false;
}

int CircularAnnotationItem::containsRegion(const QPointF &point) {
    CircularAnnotationRegionItem* r = getContainingRegion(point);
    return (r == NULL) ? -1 : r->number;
}

CircularAnnotationRegionItem* CircularAnnotationItem::getContainingRegion(const QPointF &point) {
    if(_boundingRect.contains(point)) {
        foreach(CircularAnnotationRegionItem* item, regions) {
            if(item->contains(point)) {
                return item;
            }
        }
    }
    return NULL;
}

void CircularAnnotationItem::setSelected(bool isSelected) {
    this->isSelected = isSelected;
}

const QList<CircularAnnotationRegionItem*>& CircularAnnotationItem::getRegions() {
    return regions;
}

/************************************************************************/
/* CircularAnnotationRegionItem                                        */
/************************************************************************/

CircularAnnotationRegionItem::CircularAnnotationRegionItem(const QPainterPath& path, bool _isShort, int _number)
    : QGraphicsPathItem(path),
      parent(NULL),
      label(NULL),
      number(_number),
      isShort(_isShort),
      arrowCenterPercentage(0)
{}

void CircularAnnotationRegionItem::paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */) {
    Q_UNUSED(widget);
    Q_UNUSED(option);
    assert(parent!=NULL);
    QPen pen(Qt::black);
    pen.setWidth(1);
    if(parent->isSelected) {
        pen.setWidth(2);
    }

    p->setPen(pen);

    CircularViewRenderArea* renderArea = parent->ra;

    int yLevel = renderArea->annotationYLevel[parent->annotation];
    int innerRadius = renderArea->innerEllipseSize/2 + yLevel * renderArea->ellipseDelta/2;
    int outerRadius = renderArea->outerEllipseSize/2 + yLevel * renderArea->ellipseDelta/2;
    QColor &color(parent->color);
    QRadialGradient radialGrad(QPointF(0, 0), outerRadius);
    radialGrad.setColorAt(1, color);
    radialGrad.setColorAt((float)innerRadius / (float)outerRadius, QColor(color.red()*0.7,color.green()*0.7,color.blue()*0.7));
    radialGrad.setColorAt(0, Qt::black);

    p->fillPath(path(), radialGrad);

    if(!isShort || parent->isSelected) {
        p->drawPath(path());
    }
}

CircularAnnotationLabel* CircularAnnotationRegionItem::getLabel() const {
    return label;
}

void CircularAnnotationRegionItem::setLabel(CircularAnnotationLabel* label) {
    this->label = label;
}

/************************************************************************/
/* CircularAnnotationLabel                                              */
/************************************************************************/

bool CircularAnnotationLabel::labelLengthLessThan(CircularAnnotationLabel *l1, CircularAnnotationLabel *l2) {
    const int annLevel1 = l1->ra->getAnnotationYLevel(l1->getAnnotation());
    const int annLevel2 = l2->ra->getAnnotationYLevel(l2->getAnnotation());
    if (annLevel1 < annLevel2) {
        return false;
    } else if (annLevel1 > annLevel2) { // annotations from higher orbits are drawn first
        return true;
    } else {
        const U2Region region1 = l1->getAnnotation()->getRegions()[l1->getRegion()];
        const U2Region region2 = l2->getAnnotation()->getRegions()[l2->getRegion()];
        return region1.startPos < region2.startPos ? true : region1.startPos == region2.startPos ? region1.length < region2.length : false;
    }
}

CircularAnnotationLabel::CircularAnnotationLabel(Annotation *ann, const QVector<U2Region> &annLocation, bool isAutoAnnotation,
    int _region, int sequenceLength, const QFont &font, CircularViewRenderArea *renderArea)
    : annotation(ann),
      location(annLocation),
      isAutoAnnotation(isAutoAnnotation),
      labelFont(font),
      region(_region),
      ra(renderArea),
      regionItem(NULL),
      hasPosition(false)
{
    SAFE_POINT(0 <= region && region < location.size(), "Invalid location index", );

    const U2Region &r = location[region];
    qreal startAngle = renderArea->rotationDegree + 360.0 * r.startPos / sequenceLength;
    while (startAngle > 360) {
        startAngle -= 360;
    }
    const qreal spanAngle = 360.0 * r.length / sequenceLength;

    annotationAngle = (startAngle + spanAngle / 2.0) * Deg2Rad;
    if (annotationAngle < 0) {
        annotationAngle += 2 * PI;
    } else if (annotationAngle > 2 * PI) {
        annotationAngle -= 2 * PI;
    }

    startA = startAngle * Deg2Rad;
    endA = spanAngle * Deg2Rad + startA;
    spanA = spanAngle * Deg2Rad;
    if (startA > 2 * PI) {
        startA -= 2 * PI;
    } else if (startA < 0) {
        startA += 2 * PI;
    }
    if (endA > 2 * PI) {
        endA -= 2 * PI;
    } else if (endA < 0) {
        endA += 2 * PI;
    }

    const int yLevel = renderArea->annotationYLevel[annotation];

    midRect = QRectF(-renderArea->middleEllipseSize / 2 - yLevel * renderArea->ellipseDelta / 2,
        -renderArea->middleEllipseSize / 2 - yLevel * renderArea->ellipseDelta / 2,
        renderArea->middleEllipseSize + yLevel * renderArea->ellipseDelta,
        renderArea->middleEllipseSize + yLevel * renderArea->ellipseDelta);
    setVisible(false);

    AnnotationSettingsRegistry *asr = AppContext::getAnnotationsSettingsRegistry();
    const SharedAnnotationData &aData = annotation->getData();
    AnnotationSettings *as = asr->getAnnotationSettings(aData);
    labelText = GSequenceLineViewAnnotated::prepareAnnotationText(aData, as);
}

void CircularAnnotationLabel::setAnnRegion(CircularAnnotationRegionItem *region) {
    regionItem = region;
}

bool CircularAnnotationLabel::canFitToTheRegion() const {
    CHECK(regionItem != NULL, false);

    QPainterPath path = regionItem->path();
    QFontMetrics metrics(labelFont, ra);
    qreal labelTextPixSize = metrics.width(labelText);
    // (can fit in width) && (can fit in height)
    if ((regionItem->getArrowCenterPercentage() * path.length() > labelTextPixSize / 2)
        && (metrics.height() + ra->FREE_SPACE_HEIGHT_FOR_INTERNAL_LABELS < ra->circularView->CV_REGION_ITEM_WIDTH))
    {
        return true;
    }
    return false;
}

void CircularAnnotationLabel::drawLabelInsideRegion(QPainter *p, bool canFit) {
    QPainterPath path = regionItem->path();
    SAFE_POINT(path.length() != 0, "Region path has zero length",);
    QFontMetrics metrics(labelFont,ra);

    if (metrics.height() + ra->FREE_SPACE_HEIGHT_FOR_INTERNAL_LABELS
            > ra->circularView->CV_REGION_ITEM_WIDTH) {
        // The font is too big for curent region width
        return;
    }

    QString labelText = this->labelText;
    qreal labelCenter = regionItem->getArrowCenterPercentage();
    if (!canFit) {
        qreal arrowShaft = path.length() * 2 * labelCenter;
        labelText = metrics.elidedText(labelText, Qt::ElideRight, arrowShaft);
    }

    int textWidth = metrics.size(Qt::TextSingleLine, labelText).width();
    qreal labelPercentage = textWidth / path.length();

    qreal percent = 0;
    qreal percentIncreaseSgn = -1;
    qreal arrowWidthPercentage = (ra->outerEllipseSize - ra->innerEllipseSize + 5) / (2 * path.length());

    const SharedAnnotationData &aData = annotation->getData();
    bool isDirect = aData->getStrand().isDirect();
    // percent of annotation arrow path where label start position is located
    if (isDirect) {
        qreal a = path.angleAtPercent(labelCenter);
        if (a < 90 || a > 270) {
            // top
            percent = 1 - labelCenter - arrowWidthPercentage + labelPercentage / 2;
        } else {
            // bottom
            percent = labelCenter + labelPercentage / 2;
        }
    } else {
        qreal a = path.angleAtPercent(1 - labelCenter);
        if (a < 90 || a > 270) {
            // bottom
            percent = 1 - labelCenter - labelPercentage / 2;
        } else {
            // top
            percent = 1 - labelCenter * 3 - arrowWidthPercentage - labelPercentage / 2;
        }
        percentIncreaseSgn = 1;
    }


    for (int i = 0; i < labelText.size(); i++) {
        QPointF point = path.pointAtPercent(percent);
        qreal angle = path.angleAtPercent(percent);   // Clockwise is negative

        p->save();
        // Move the virtual origin to the point on the curve
        p->translate(point);
        // Rotate to match the angle of the curve
        // Clockwise is positive so we negate the angle from above
        p->rotate(-angle + isDirect*180);
        // Draw a line width above the origin to move the text above the line
        // and let Qt do the transformations
        p->drawText(QPoint(0, - p->pen().width()), QString(labelText[i]));
        p->restore();

        percent += percentIncreaseSgn * metrics.width(labelText[i]) / path.length();
    }
}

void CircularAnnotationLabel::drawLabelOutsideRegion(QPainter *p, QPen& pen) {
    QRectF bound = boundingRect();
    p->fillRect(bound, Qt::white);
    p->drawText(bound, labelText);

    pen.setStyle(Qt::DotLine);
    p->setPen(pen);

    p->drawLine(connectionStart, connectionEnd);
    pen.setStyle(Qt::SolidLine);
    p->setPen(pen);
}

void CircularAnnotationLabel::setLabelsVisible(QList<CircularAnnotationLabel *> &labelItems) {
    qSort<QList<CircularAnnotationLabel *>::iterator>(labelItems.begin(), labelItems.end(), labelLengthLessThan);
    foreach (CircularAnnotationLabel *l, labelItems) {
        l->setVisible(true);
    }
}

void CircularAnnotationLabel::engageLabelPosition(int pos) {
    SAFE_POINT(!ra->engagedLabelPositionToLabel.contains(pos), "Attempting to use an engaged label position", );

    engagedLabelPosition = ra->positionsAvailableForLabels.at(pos);
    ra->engagedLabelPositionToLabel[pos] = this;

    calculateConnectionStart();
    calculateConnectionEnd();
    hasPosition = true;
}

void CircularAnnotationLabel::setLabelPosition() {
    // find closest empty label position
    calculateSuitablePositions(ra->positionsAvailableForLabels);
    int closest = findClosestPoint(ra->positionsAvailableForLabels);
    CHECK(-1 != closest, );

    if (ra->engagedLabelPositionToLabel.contains(closest)) { // the closest position is engaged
        int currentTargetPosIndex = closest;
        int currentSuitableIndex = suitableLabelPositionIndexes.indexOf(closest);
        SAFE_POINT(-1 != currentSuitableIndex, "Unexpected suitable position for a label", );
        while (ra->engagedLabelPositionToLabel.contains(currentTargetPosIndex) && currentSuitableIndex < suitableLabelPositionIndexes.size() - 1) {
            currentTargetPosIndex = suitableLabelPositionIndexes[++currentSuitableIndex];
        }
        if (ra->engagedLabelPositionToLabel.contains(currentTargetPosIndex)) {
            while (ra->engagedLabelPositionToLabel.contains(currentTargetPosIndex) && currentSuitableIndex > 0) {
                currentTargetPosIndex = suitableLabelPositionIndexes[--currentSuitableIndex];
            }
            if (ra->engagedLabelPositionToLabel.contains(currentTargetPosIndex)) {
                return;
            }
        }
        closest = currentTargetPosIndex;
    }

    engageLabelPosition(closest);
    avoidLinesIntersections();
}

namespace {

bool labelConnectionLinesIntersect(CircularAnnotationLabel *const label1, CircularAnnotationLabel *const label2) {
    const QLineF line1(label1->getConnectionStart(), label1->getConnectionEnd());
    const QLineF line2(label2->getConnectionStart(), label2->getConnectionEnd());

    QPointF intersectionPoint;
    const QLineF::IntersectType intersection = line1.intersect(line2, &intersectionPoint);
    return QLineF::BoundedIntersection == intersection;
}

}

const QPoint & CircularAnnotationLabel::getConnectionStart() const {
    return connectionStart;
}

const QPoint & CircularAnnotationLabel::getConnectionEnd() const {
    return connectionEnd;
}

void CircularAnnotationLabel::avoidLinesIntersections() {
    CHECK(hasPosition, );
    const int engagedIndex = ra->engagedLabelPositionToLabel.key(this, -1);
    SAFE_POINT(-1 != engagedIndex, "Unexpected label position index", );

    for (int i = -2; i < 3; i += (i == -1) ? 2 : 1) {
        CircularAnnotationLabel *neighbour = ra->engagedLabelPositionToLabel.value(engagedIndex + i, NULL);
        bool swapSuccessful = true;
        if (NULL != neighbour && labelConnectionLinesIntersect(this, neighbour)) {
            ra->engagedLabelPositionToLabel.remove(engagedIndex);
            if (i < 0 ? neighbour->tryPushClockwise() : neighbour->tryPushCounterclockwise()) {
                engageLabelPosition(engagedIndex + i);
                break;
            } else {
                swapSuccessful = false;
            }
        }
        if (!swapSuccessful) {
            SAFE_POINT(!ra->engagedLabelPositionToLabel.contains(engagedIndex), "Label position is engaged unexpectedly", );
            ra->engagedLabelPositionToLabel[engagedIndex] = this;
        }
    }
}

bool CircularAnnotationLabel::tryPushClockwise() {
    const int engagedIndex = ra->engagedLabelPositionToLabel.key(this, -1);
    SAFE_POINT(-1 != engagedIndex, "Unexpected label position index", false);
    const int numberOfEngagedIndex = suitableLabelPositionIndexes.indexOf(engagedIndex);
    SAFE_POINT(-1 != numberOfEngagedIndex, "Unexpected label position index", false);

    if ((suitableLabelPositionIndexes.size() - 1) == numberOfEngagedIndex) {
        return false;
    }

    const int currentLabelPosIndex = suitableLabelPositionIndexes[numberOfEngagedIndex + 1];
    if (!ra->engagedLabelPositionToLabel.contains(currentLabelPosIndex) || ra->engagedLabelPositionToLabel[currentLabelPosIndex]->tryPushClockwise()) {
        ra->engagedLabelPositionToLabel.remove(engagedIndex);
        engageLabelPosition(currentLabelPosIndex);
        return true;
    } else {
        return false;
    }
}

bool CircularAnnotationLabel::tryPushCounterclockwise() {
    const int engagedIndex = ra->engagedLabelPositionToLabel.key(this, -1);
    SAFE_POINT(-1 != engagedIndex, "Unexpected label position index", false);
    const int numberOfEngagedIndex = suitableLabelPositionIndexes.indexOf(engagedIndex);
    SAFE_POINT(-1 != numberOfEngagedIndex, "Unexpected label position index", false);

    if (0 == numberOfEngagedIndex) {
        return false;
    }

    const int currentLabelPosIndex = suitableLabelPositionIndexes[numberOfEngagedIndex - 1];
    if (!ra->engagedLabelPositionToLabel.contains(currentLabelPosIndex) || ra->engagedLabelPositionToLabel[currentLabelPosIndex]->tryPushCounterclockwise()) {
        ra->engagedLabelPositionToLabel.remove(engagedIndex);
        engageLabelPosition(currentLabelPosIndex);
        return true;
    } else {
        return false;
    }
}

namespace {

qreal calculateAngleDiff(const QPoint &p, float annotationAngle) {
    return qAbs(annotationAngle - CircularView::coordToAngle(p));
}

QPoint getRectangleCornerForAnnotationAngle(float annotationAngle, const QRect &rect) {
    return (annotationAngle >= 0 && annotationAngle < PI) ? rect.topLeft() : rect.bottomLeft();
}

}

int CircularAnnotationLabel::findClosestPoint(const QVector<QRect> &rects) {
    foreach (int idx, suitableLabelPositionIndexes) {
        SAFE_POINT(idx >= 0 && idx < rects.size(), "Array index is out of range", -1);
        if (boundingRect().width() > rects.at(idx).width()) {
            int i = suitableLabelPositionIndexes.indexOf(idx);
            suitableLabelPositionIndexes.remove(i);
        }
    }

    int size = suitableLabelPositionIndexes.size();
    CHECK(0 != size, -1);

    int index = suitableLabelPositionIndexes.at(0);
    SAFE_POINT(index >= 0 && index < rects.size(), "Array index is out of range", -1);

    if (1 == size) {
        return index;
    }

    qreal minAngleDiff = std::numeric_limits<qreal>::max();
    int result = -1;

    foreach (int idx, suitableLabelPositionIndexes) {
        SAFE_POINT(idx >= 0 && idx < rects.size(), "Array index is out of range", -1);

        const QRect currentLabelRect = rects.at(idx);
        const qreal currentMinAngleDiff = calculateAngleDiff(getRectangleCornerForAnnotationAngle(annotationAngle, currentLabelRect), annotationAngle);
        if (currentMinAngleDiff < minAngleDiff) {
            minAngleDiff = currentMinAngleDiff;
            result = idx;
        }
    }
    return result;
}

#define HALF_DELTA_ANGLE PI / 32
void CircularAnnotationLabel::calculateSuitablePositions(const QVector<QRect> &rects) {
    suitableLabelPositionIndexes.clear();
    foreach (const QRect &rect, rects) {
        qreal arcsin = CircularView::coordToAngle(rect.topLeft());
        if (startA > endA) { //region contains 2*PI angle
            if (arcsin > startA - HALF_DELTA_ANGLE || arcsin < endA + HALF_DELTA_ANGLE) {
                suitableLabelPositionIndexes << rects.indexOf(rect);
            }
        } else {
            if (arcsin > startA - HALF_DELTA_ANGLE && arcsin < endA + HALF_DELTA_ANGLE) {
                suitableLabelPositionIndexes << rects.indexOf(rect);
            }
        }
    }
}

#define CONNECTION_POINT_PAD PI / 36
void CircularAnnotationLabel::calculateConnectionEnd() {
    qreal labelAngle = CircularView::coordToAngle(connectionStart);

    const qreal diffBetweenLabelAndStart = startA + CONNECTION_POINT_PAD + ((startA + CONNECTION_POINT_PAD < labelAngle) ? 2 * PI : 0) - labelAngle;
    const qreal diffBetweenLabelAndEnd = labelAngle + ((labelAngle < endA - CONNECTION_POINT_PAD) ? 2 * PI : 0) - (endA - CONNECTION_POINT_PAD);

    qreal endPointAngle = 0.0;
    if (spanA < CONNECTION_POINT_PAD * 2) {
        endPointAngle = startA + spanA / 2;
    } else if (startA > endA) { // annotation passes through 0 angle
        const qreal shiftedEndA = endA + 2 * PI;
        const qreal shiftedLabelAngle = (labelAngle >= 0 && labelAngle <= (endA + startA) / 2) ? labelAngle + 2 * PI : labelAngle;

        if (shiftedLabelAngle > startA + CONNECTION_POINT_PAD && shiftedLabelAngle < shiftedEndA - CONNECTION_POINT_PAD) {
            endPointAngle = labelAngle;
        } else if (shiftedLabelAngle < startA + CONNECTION_POINT_PAD) {
            endPointAngle = startA + CONNECTION_POINT_PAD;
        } else {
            endPointAngle = endA - CONNECTION_POINT_PAD;
        }
    } else if (labelAngle > startA + CONNECTION_POINT_PAD && labelAngle < endA - CONNECTION_POINT_PAD) {
        endPointAngle = labelAngle;
    } else if (diffBetweenLabelAndStart < diffBetweenLabelAndEnd) {
        endPointAngle = startA + CONNECTION_POINT_PAD;
    } else {
        endPointAngle = endA - CONNECTION_POINT_PAD;
    }
    connectionEnd = QPoint(midRect.width() / 2 * cos(endPointAngle), midRect.width() / 2 * sin(endPointAngle));
}

void CircularAnnotationLabel::calculateConnectionStart() {
    connectionStart = getRectangleCornerForAnnotationAngle(annotationAngle, engagedLabelPosition);
}

void CircularAnnotationLabel::paint(QPainter *p,const QStyleOptionGraphicsItem *item,QWidget *widget) {
    Q_UNUSED(item);
    Q_UNUSED(widget);

    if (!isVisible()) {
        return;
    }

    CircularAnnotationItem* ai= ra->circItems.value(annotation);

    p->save();
    QPen pen(Qt::black);
    pen.setWidth(1);
    labelFont.setBold(false);
    if(ai->isSelected) {
        labelFont.setBold(true);
        pen.setWidth(2);
    }
    p->setFont(labelFont);
    p->setPen(pen);

    bool canFit = canFitToTheRegion();

    SAFE_POINT(ra->settings != NULL, "Circular view settings is NULL",);
    if (ra->settings->labelMode == CircularViewSettings::Inside
        || (ra->settings->labelMode == CircularViewSettings::Mixed && canFit && !isAutoAnnotation))
    {
        drawLabelInsideRegion(p, canFit);
    }
    if (hasPosition && (ra->settings->labelMode == CircularViewSettings::Outside ||
            (ra->settings->labelMode == CircularViewSettings::Mixed && (!canFit || isAutoAnnotation)))) {
        drawLabelOutsideRegion(p, pen);
    }
    p->restore();
}

Annotation * CircularAnnotationLabel::getAnnotation() const {
    return annotation;
}

QRectF CircularAnnotationLabel::boundingRect() const {
    const QFontMetrics fm(labelFont);
    QRectF bound = fm.boundingRect(" " + labelText);
    if (engagedLabelPosition.x() >= 0) {
        bound.moveTopLeft(engagedLabelPosition.topLeft());
    } else {
        bound.moveTopRight(engagedLabelPosition.topLeft());
    }
    return bound;
}

bool CircularAnnotationLabel::contains(const QPointF &point) const {
    return boundingRect().contains(point) || QGraphicsLineItem(QLine(connectionStart, connectionEnd)).contains(point);
}

int CircularAnnotationLabel::getRegion() const {
    return region;
}

} // namespace U2
