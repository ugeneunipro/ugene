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

#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Core/QObjectScopedPointer.h>

#include "ADVGraphModel.h"
#include "GSequenceGraphView.h"
#include "GraphSettingsDialog.h"
#include "SaveGraphCutoffsDialogController.h"
#include "WindowStepSelectorWidget.h"

namespace U2 {

GSequenceGraphAlgorithm::GSequenceGraphAlgorithm()
    : lastSeqObj(NULL)
{

}

GSequenceGraphAlgorithm::~GSequenceGraphAlgorithm() {

}

const QByteArray & GSequenceGraphAlgorithm::getSequenceData(U2SequenceObject *seqObj, U2OpStatus &os) {
    if(seqObj != lastSeqObj) {
        const QByteArray seqData = seqObj->getWholeSequenceData(os);
        CHECK_OP(os, lastSeqData);
        lastSeqData = seqData;
        lastSeqObj = seqObj;
    }
    return lastSeqData;
}

GSequenceGraphData::GSequenceGraphData(const QString& _graphName)
    : graphName(_graphName),
      ga(NULL),
      cachedFrom(0),
      cachedLen(0),
      cachedW(0),
      cachedS(0),
      alignedFC(0),
      alignedLC(0)
{

}

GSequenceGraphData::~GSequenceGraphData() {
    delete ga;
}

void GSequenceGraphUtils::calculateMinMax(const QVector<float>& data, float& min, float& max, U2OpStatus &os)  {
    assert(data.size() > 0);
    min = max = data.first();
    const float* d = data.constData();
    for (int i=1, n = data.size() ; i<n ; i++) {
        CHECK_OP(os, );
        float val = d[i];
        if (min > val) {
            min = val;
        } else if (max < val) {
            max = val;
        }
    }
}

#define ACCEPTABLE_FLOAT_PRESISION_LOSS 0.0001
float GSequenceGraphUtils::calculateAverage(const QVector<float>& data, float start, float range) {
    float result;
    if (int(start)!=int(start+range)) {
        //result constructed from 3 parts: ave[start, startIdx] + ave[startIdx, endIdx] + ave[endIdx, end]
        float part1 = 0;
        float part2 = 0;
        float part3 = 0;

        int startIdx = int(floor(start));
        float startDiff = 1 - (start - startIdx);
        float end = start + range;
        int endIdx = int(end);
        float endDiff = end - endIdx;

        assert(qAbs(startDiff + (endIdx-(startIdx+1)) + endDiff - range) / range <= ACCEPTABLE_FLOAT_PRESISION_LOSS);

        //calculating part1
        if (startDiff > ACCEPTABLE_FLOAT_PRESISION_LOSS) {
            float v1 = data[startIdx];
            float v2 = data[startIdx+1];
            float k = v2-v1;
            float valInStart = v2 - k*startDiff;
            part1 = startDiff * (valInStart + v2) / 2;
        }
        int firstIdxInRange = int(ceil(start));
        //calculating part2
        for(int i =firstIdxInRange; i < endIdx; i++) {
            part2+=data[i];
        }
        //calculating part3
        if (endDiff > ACCEPTABLE_FLOAT_PRESISION_LOSS && endIdx+1 < (int)data.size()) {
            float v1 = data[endIdx];
            float v2 = data[endIdx+1];
            float k = v2-v1;
            float valInEnd= v1+k*endDiff;
            part3 = endDiff * (v1 + valInEnd) / 2;
        }
        //sum
        result = (part1 + part2 + part3 ) / range;
    } else {
        //result constructed from 1 part: ave[start, end], no data points between
        int startIdx = int(start);
        float startDiff = start - float(startIdx);
        float endDiff = startDiff + range;
        assert(endDiff < 1);
        float v1 = data[startIdx];
        float v2 = data[startIdx+1];
        float k = v2-v1;
        float valInStart = v1 + k*startDiff;
        float valInEnd = v1 + k*endDiff;
        result = (valInEnd+valInStart)/2;
    }
    return result;
}

void GSequenceGraphUtils::fitToScreen(const QVector<float>& data, int dataStartBase, int dataEndBase, QVector<float>& results,
                                       int resultStartBase, int resultEndBase, int screenWidth, float unknownVal)
{
    //BUG:422: use intervals and max/min values instead of average!
    float basesPerPixel = (resultEndBase - resultStartBase) / (float) screenWidth;
    float basesInDataPerIndex = (dataEndBase - dataStartBase) / (float) (data.size() - 1);
    float currentBase = resultStartBase;
    results.reserve(results.size() + screenWidth);
    for (int i=0; i < screenWidth; i++, currentBase+=basesPerPixel) {
        float dataStartIdx = (currentBase - basesPerPixel / 2 - dataStartBase) / basesInDataPerIndex;
        float dataEndIdx =  (currentBase  + basesPerPixel / 2 - dataStartBase) / basesInDataPerIndex;
        dataStartIdx = qMax((float)0, dataStartIdx);
        dataEndIdx = qMin((float)data.size()-1, dataEndIdx);
        float nDataPointsToAverage = dataEndIdx - dataStartIdx;
        float val = unknownVal;
        if (nDataPointsToAverage >= ACCEPTABLE_FLOAT_PRESISION_LOSS) {
            val = calculateAverage(data, dataStartIdx, nDataPointsToAverage);
        }
        results.append(val);
    }
}

int GSequenceGraphUtils::getNumSteps(const U2Region& range, int w, int s) {
    if(range.length < w) return 1;
    qint64 steps = (range.length  - w) / s + 1;
    return (int) steps;
}

//////////////////////////////////////////////////////////////////////////
//drawer

const float GSequenceGraphDrawer::UNKNOWN_VAL = -1000; // todo: use numeric limits and test all platforms

GSequenceGraphDrawer::GSequenceGraphDrawer(GSequenceGraphView* v, const GSequenceGraphWindowData& wd,
                                           QMap<QString,QColor> colors)
: QObject(v), view(v), lineColors(colors), globalMin(0), globalMax(0), wdata(wd)
{
    DEFAULT_COLOR = tr("Default color");

    connect(v, SIGNAL(si_labelAdded(const QSharedPointer<GSequenceGraphData>&, GraphLabel*, const QRect&)),
            this, SLOT(sl_labelAdded(const QSharedPointer<GSequenceGraphData>&, GraphLabel*, const QRect&)));
    connect(v, SIGNAL(si_labelMoved(const QSharedPointer<GSequenceGraphData>&, GraphLabel*, const QRect&)),
            this, SLOT(sl_labelMoved(const QSharedPointer<GSequenceGraphData>&, GraphLabel*, const QRect&)));
    connect(v, SIGNAL(si_labelsColorChange(const QSharedPointer<GSequenceGraphData>&)),
            this, SLOT(sl_labelsColorChange(const QSharedPointer<GSequenceGraphData>&)));
    defFont = new QFont("Arial", 8);
    if (colors.isEmpty()) {
        lineColors.insert(DEFAULT_COLOR, Qt::black);
    }
    connect(&calculationTaskRunner, SIGNAL(si_finished()), SLOT(sl_calculationTaskFinished()));
}

GSequenceGraphDrawer::~GSequenceGraphDrawer() {
    delete defFont;
}

void GSequenceGraphDrawer::sl_calculationTaskFinished() {
    if (calculationTaskRunner.isSuccessful()) {
        emit si_graphDataUpdated();
    } else {
        emit si_graphRenderError();
    }
}

void GSequenceGraphDrawer::draw(QPainter& p, const QList<QSharedPointer<GSequenceGraphData> > &graphs, const QRect& rect) {

    globalMin = 0;
    globalMax = 0;

    foreach (const QSharedPointer<GSequenceGraphData>& graph, graphs) {
        drawGraph(p, graph, rect);
        foreach(GraphLabel *label, graph->graphLabels.getLabels()) {
            bool labelIsVisible = updateStaticLabels(graph, label, rect);
            if (labelIsVisible) {
                label->show();
            } else {
                label->hide();
            }
        }
    }

    {
         //draw min/max
         QPen minMaxPen(Qt::DashDotDotLine);
         minMaxPen.setWidth(1);
         p.setPen(minMaxPen);
         p.setFont(*defFont);

         //max
         p.drawLine(rect.topLeft(), rect.topRight());
         QRect maxTextRect(rect.x(), rect.y(), rect.width(), 12);
         p.drawText(maxTextRect, Qt::AlignRight, QString::number((double) globalMax, 'g', 4));

         //min
         p.drawLine(rect.bottomLeft(), rect.bottomRight());
         QRect minTextRect(rect.x(), rect.bottom()-12, rect.width(), 12);
         p.drawText(minTextRect, Qt::AlignRight, QString::number((double) globalMin, 'g', 4));
     }
}
void GSequenceGraphDrawer::sl_labelAdded(const QSharedPointer<GSequenceGraphData>& graph, GraphLabel *label, const QRect &rect) {
    updateStaticLabels(graph, label, rect);
}

void  GSequenceGraphDrawer::sl_labelMoved(const QSharedPointer<GSequenceGraphData>& graph, GraphLabel *label, const QRect &rect) {
    updateMovingLabels(graph, label, rect);
}

void  GSequenceGraphDrawer::sl_labelsColorChange(const QSharedPointer<GSequenceGraphData>& graph) {
    QColor color;

    if  (lineColors.contains(graph->graphName)) {
        color = lineColors.value(graph->graphName);
    } else {
        color = lineColors.value(DEFAULT_COLOR);
    }

    foreach(GraphLabel *label, graph->graphLabels.getLabels()) {
        label->setColor(color, color);
    }

    graph->graphLabels.getMovingLabel().setColor(color, Qt::red);
}


void GSequenceGraphDrawer::drawGraph(QPainter& p, const QSharedPointer<GSequenceGraphData>& d, const QRect& rect) {
    float min=0;
    float max=0;
    PairVector points;
    int nPoints = rect.width();
    calculatePoints(d, points, min, max, nPoints);

    if (!points.isEmpty()) {
        assert(points.firstPoints.size() == nPoints);

        double comin = commdata.minEdge, comax = commdata.maxEdge;
        if (commdata.enableCuttoff){
            min = comin;
            max = comax;
        }

        globalMin = min;
        globalMax = max;

        QPen graphPen(Qt::SolidLine);
        if (lineColors.contains(d->graphName)) {
            graphPen.setColor(lineColors.value(d->graphName));
        } else {
            graphPen.setColor(lineColors.value(DEFAULT_COLOR));
        }

        graphPen.setWidth(1);
        p.setPen(graphPen);


        int graphHeight = rect.bottom() - rect.top() - 2;
        float kh = (min == max) ? 1 : graphHeight / (max - min);

        int prevY = -1;
        int prevX = -1;

        if (!commdata.enableCuttoff) {
            ////////cutoff off
            for (int i = 0, n = nPoints; i < n; i++) {
                float fy1 = points.firstPoints[i];
                if (isUnknownValue(fy1)) {
                    continue;
                }
                int dy1 = qRound((fy1 - min) * kh);
                assert(dy1 <= graphHeight);
                int y1 = rect.bottom() - 1 - dy1;
                int x = rect.left() + i;
                assert(y1 > rect.top() && y1 < rect.bottom());
                if (prevX != -1) {
                    p.drawLine(prevX, prevY , x, y1);
                }
                prevY = y1;
                prevX = x;
                if (points.useIntervals) {
                    float fy2 = points.secondPoints[i];
                    if (isUnknownValue(fy2)) {
                        continue;
                    }
                    int dy2 = qRound((fy2 - min) * kh);
                    assert(dy2 <= graphHeight);
                    int y2 = rect.bottom() - 1 - dy2;
                    assert(y2 > rect.top() && y2 < rect.bottom());
                    if (prevX != -1){
                        p.drawLine(prevX, prevY , x, y2);
                    }
                    prevY = y2;
                    prevX = x;
                }
            }
        } else    {
            ////////cutoff on

            float fymin = comin;
            float fymax = comax;
            float fymid = (comin + comax)/2;
            float fy;
            int prevFY = -1;
            bool rp = false, lp = false;
            if(!points.useIntervals){
                for (int i=0, n = points.firstPoints.size(); i < n; i++) {
                    fy = calculatePointValue(points, i);
                    rp = false;
                    lp = false;
                    if (isUnknownValue(fy)) {
                        continue;
                    }
                    if (fy >= fymax) {
                        fy = fymax;
                        if (prevFY == int(fymid)) lp=true;
                    }else if(fy < fymax && fy > fymin){
                        fy = fymid;
                        if (prevFY == int(fymax)) rp=true;
                        if (prevFY == int(fymin)) lp=true;
                    }else{
                        fy = fymin;
                        if (prevFY == int(fymid)) lp=true;
                    }

                    int dy = qRound((fy - min) * kh);
                    assert(dy <= graphHeight);
                    int y = rect.bottom() - 1 - dy;
                    int x = rect.left() + i;

                    assert(y > rect.top() && y < rect.bottom());
                    if (prevX!=-1){
                        p.drawLine(prevX, prevY , x, prevY);
                    }

                    if (prevY != y && prevX != -1){ // common case for cutoffs
                        p.drawLine(x, prevY , x, y);
                    }
                    prevY = y;
                    prevX = x;
                    prevFY = (int) fy;
                }
            } else {
                for (int i=0, n = points.firstPoints.size(); i < n; i++) {
                    assert(points.firstPoints.size() == points.secondPoints.size());
                    fy = calculatePointValue(points, i);
                    float fy2 = calculatePointValue(points, i);;
                    rp = false;
                    lp = false;
                    if (isUnknownValue(fy)) {
                        continue;
                    }

                    assert(!isUnknownValue(fy2));

                    if (fy2 >= fymax) {
                        fy2 = fymax;
                    }else if(fy2 < fymax && fy2 > fymin){
                        fy2 = fymid;
                    }else{
                        fy2 = fymin;
                    }

                    if (fy >= fymax) {
                        fy = fymax;
                        if (prevFY == int(fymid)) lp=true;
                    }else if(fy < fymax && fy > fymin){
                        fy = fymid;
                        if (prevFY == int(fymax)) rp=true;
                        if (prevFY == int(fymin)) lp=true;
                    }else{
                        fy = fymin;
                        if (prevFY == int(fymid)) lp=true;
                    }

                    int dy = qRound((fy - min) * kh);
                    assert(dy <= graphHeight);
                    int y = rect.bottom() - 1 - dy;
                    int x = rect.left() + i;
                    if (lp) {
                        p.drawLine(prevX, prevY, x, prevY);
                        prevX = x;
                    }
                    if (rp) {
                        p.drawLine(prevX,prevY,prevX,y);
                        prevY = y;
                    }
                    assert(y > rect.top() && y < rect.bottom());
                    if (prevX!=-1){
                        p.drawLine(prevX, prevY , x, y);
                    }
                    if(fy != fy2){
                        int dy2 = qRound((fy2 - min) * kh);
                        int y2 = rect.bottom() - 1 - dy2;
                        p.drawLine(x,y, x, y2);
                    }
                    prevY = y;
                    prevX = x;
                    prevFY = (int) fy;
                }

            }
        }
    } else if(!calculationTaskRunner.isIdle()) {
        p.fillRect(rect, Qt::gray);
        p.drawText(rect, Qt::AlignCenter, tr("Graph is rendering..."));
    } else {
        p.drawText(rect, Qt::AlignCenter, tr("Graph is not available. Try to change calculation settings."));
    }
}
const int mLabelCoordY = 20;

void GSequenceGraphDrawer::selectExtremumPoints(const QSharedPointer<GSequenceGraphData>& graph, const QRect &graphRect, int windowSize, const U2Region &visibleRange)
{
    qint64 sequenceLength = view->getSequenceLength();
    int startPos = visibleRange.startPos;
    int endPos = visibleRange.endPos();

    PairVector points;
    if(sequenceLength <= 0) {
        return;
    }
    calculatePoints(graph, points, globalMin, globalMax, sequenceLength);

    int pos = startPos;
    QVector<float> &firstPoints = points.firstPoints;
    endPos = qMin(endPos, firstPoints.size() - 1);
    while(pos < endPos) {
        int maxValue = firstPoints.at(pos);
        int minValue = globalMax * 2;
        int posOfMax = pos;
        int posOfMin = 0;
        for(int i = 0; i < windowSize; i++, pos++) {
            if (pos >= endPos)
                break;
            if (isUnknownValue(firstPoints.at(pos)))
                continue;
            if (maxValue < firstPoints.at(pos)) {
                maxValue = firstPoints.at(pos);
                posOfMax = pos;
            }
            if (minValue > firstPoints.at(pos)) {
                minValue = firstPoints.at(pos);
                posOfMin = pos;
            }
        }
        GraphLabel* maxLabel = new GraphLabel(posOfMax, view);
        maxLabel->show();
        graph->graphLabels.addLabel(maxLabel);
        updateStaticLabels(graph, maxLabel, graphRect);
        GraphLabel* minLabel = new GraphLabel(posOfMin, view);
        minLabel->show();
        graph->graphLabels.addLabel(minLabel);
        updateStaticLabels(graph, minLabel, graphRect);
    }
}
bool GSequenceGraphDrawer::updateStaticLabels(const QSharedPointer<GSequenceGraphData>& graph, GraphLabel *label, const QRect &rect)
{
    int nPoints = rect.width();
    PairVector points;
    if (nPoints <= 0) {
        return false;
    }
    calculatePoints(graph, points, globalMin, globalMax, nPoints);

    QColor color;
    if (lineColors.contains(graph->graphName)) {
        color = lineColors.value(graph->graphName);
    } else {
        color = lineColors.value(DEFAULT_COLOR);
    }
    label->setColor(color, color);

    qint64 sequenceLength = view->getSequenceLength();
    int position = label->getPosition() * nPoints / sequenceLength;
    if (position < 0 || position >= nPoints) {
        return false;
    }

    bool isCalculated = calculateLabelData(rect, points, label);
    if (!isCalculated) {
        return false;
    }

    QRectF boundingRect = label->getHintRect();

    int lineWidth = label->getTextLabel().lineWidth();
    int height = qRound(boundingRect.height()) + 2*lineWidth;
    int width = qRound(boundingRect.width()) + 2*lineWidth;

    int x = label->getCoord().x();
    int y = label->getCoord().y();

    if (x + width/2 - 1 > nPoints - 2) {
        x = nPoints - width - 2;
    } else {
        x = (x > width/2 + 1) ? (x - width/2 - 1) : 2;
    }

    if (rect.top() > y - label->getSize() - height) {
        label->setHintRect(QRect(x, y + label->getSize() + 1, width, height));
    } else {
        label->setHintRect(QRect(x, y - label->getSize() - height, width, height));
    }

    if (label->attachedLabel != NULL) {
        calculatePositionOfLabel(label, nPoints);
    }

    return true;
}
void GSequenceGraphDrawer::calculatePositionOfLabel(GraphLabel *label, int nPoints)
{
    GraphLabel *attachedLabel = label;

    int groupWidth = 0;
    while(NULL != attachedLabel) {
        groupWidth += attachedLabel->getHintRect().width();
        attachedLabel = attachedLabel->attachedLabel;
    }
    int x = label->getCoord().x();
    if (x + groupWidth/2 - 1 > nPoints) {
        x = nPoints - groupWidth/2;
    } else {
        x = (x > groupWidth/2) ? x : groupWidth/2;
    }

    attachedLabel = label;
    while(NULL != attachedLabel) {
        QRect hintRect = attachedLabel->getHintRect();
        groupWidth -= 2*hintRect.width();
        int shift = x + groupWidth/2 - hintRect.left();
        hintRect.adjust(shift, 0, shift, 0);
        attachedLabel->setHintRect(hintRect);
        attachedLabel = attachedLabel->attachedLabel;
    }
}

void GSequenceGraphDrawer::updateMovingLabels(const QSharedPointer<GSequenceGraphData>& graph, GraphLabel *label, const QRect &rect)
{
    bool isVisible = updateStaticLabels(graph, label, rect);
    if (!isVisible) {
        label->hide();
        return;
    } else {
        label->show();
    }
    QRect textRect = label->getHintRect();
    int height = textRect.height();
    if(mLabelCoordY + height + label->getSize() > label->getCoord().y()) {
        textRect.setTop(label->getCoord().y() - height - label->getSize()/2 - 4);
        textRect.setBottom(label->getCoord().y() - label->getSize()/2 - 4);
    } else {
        textRect.setTop(mLabelCoordY);
        textRect.setBottom(mLabelCoordY + height);
    }

    label->setColor(label->getFillingColor(), Qt::red);
    label->setHintRect(textRect);
}

bool GSequenceGraphDrawer::calculateLabelData(const QRect &rect, const PairVector &points, GraphLabel *label) {
    int graphHeight = rect.bottom() - rect.top() - 2;
    const U2Region& visibleRange = view->getVisibleRange();
    int xcoordInRect;
    if (visibleRange.contains(label->getPosition())) {
        xcoordInRect = qRound((label->getPosition() - visibleRange.startPos) * rect.width() / visibleRange.length);
    } else {
        label->hide();
        return false;
    }
    if (xcoordInRect >= points.firstPoints.size() || xcoordInRect < 0) {
        label->hide();
        return false;
    }
    int nPoints = rect.width();
    float value = calculateLabelValue(nPoints, points, label, xcoordInRect);
    if (2 * globalMax == value) {
        return false;
    }
    int ycoordInRect = 0;
    int pos = qRound(label->getPosition());
    QString text = GSequenceGraphView::tr("[%2, %3]").arg(QString::number(pos)).arg(QString::number(value));
    float heightScalingFactor = (globalMin == globalMax) ? 1 : graphHeight / (globalMax - globalMin);
    if (points.useIntervals) {
        float maxValue = points.firstPoints.at(xcoordInRect);
        float minValue = points.secondPoints.at(xcoordInRect);
        if(maxValue != minValue) {
            text = GSequenceGraphView::tr("[%2, max:%3, min:%4]").arg(QString::number(pos)).arg(QString::number(maxValue)).arg(QString::number(minValue));
        }
    }

    label->setValue(value);
    if (commdata.enableCuttoff) {
        if (value >= commdata.maxEdge) {
            ycoordInRect = graphHeight;
        } else if(value > commdata.minEdge && value < commdata.maxEdge) {
            ycoordInRect = graphHeight / 2;
        } else {
            ycoordInRect = 0;
        }
    } else {
        ycoordInRect = qRound((value - globalMin) * heightScalingFactor);
    }

    QPoint labelCoord(xcoordInRect, rect.bottom() - 1 - ycoordInRect);
    label->setCoord(labelCoord);
    label->setHintText(text);
    label->getTextLabel().adjustSize();
    return true;
}

QPair<float, float> GSequenceGraphDrawer::getMinAndMaxInRange(const PairVector& points, const U2Region& region) {
    QPair<float, float> result(UNKNOWN_VAL, UNKNOWN_VAL);
    for (qint64 x = region.startPos, n = region.endPos(); x < n; x++) {
        float value = calculatePointValue(points, x);
        if (isUnknownValue(value)) {
            continue;
        }
        result.first = qFuzzyCompare(result.first, UNKNOWN_VAL) ? value : qMin(value, result.first) ;
        result.second = qFuzzyCompare(result.second, UNKNOWN_VAL) ? value : qMax(value, result.second) ;
    }
    return result;
}



float GSequenceGraphDrawer::calculateLabelValue(int nPoints, const PairVector &points, GraphLabel *label, int xcoordInRect) {
    float value = calculatePointValue(points, xcoordInRect);
    if (value == UNKNOWN_VAL) {
        return 2 * globalMax;
    }

    int rangeToCheck = 50;
    int startPos = qBound(0, xcoordInRect - rangeToCheck/2, nPoints);
    int endPos = qBound(startPos, xcoordInRect + rangeToCheck/2, nPoints);
    QPair<float, float> minAndMax = getMinAndMaxInRange(points, U2Region(startPos, endPos - startPos));
    bool flat = qFuzzyCompare(minAndMax.first, minAndMax.second);
    bool isMin = qFuzzyCompare(value, minAndMax.first);
    bool isMax = qFuzzyCompare(value, minAndMax.second);
    bool isExtremum = !flat && (isMin || isMax);
    if (isExtremum) {
        label->mark();
    } else {
        label->unmark();
    }

    return value;
}

float GSequenceGraphDrawer::calculatePointValue(const PairVector &points, int xcoordInRect) {
    float value = UNKNOWN_VAL;
    if (points.useIntervals) {
        float maxValue = points.firstPoints.at(xcoordInRect);
        float minValue = points.secondPoints.at(xcoordInRect);
        value = (maxValue + minValue) / 2;
    } else {
        value = points.firstPoints.at(xcoordInRect);
    }

    float prevValue = value;
    int prevX;
    for(prevX = xcoordInRect; isUnknownValue(prevValue); prevX--) {
        if (prevX < 0) {
            return UNKNOWN_VAL;
        }
        prevValue = points.firstPoints.at(prevX);
    }
    float nextValue = points.firstPoints.at(xcoordInRect);
    int   nextX;
    for(nextX = xcoordInRect; isUnknownValue(nextValue); nextX++) {
        if (nextX >= points.firstPoints.size()) {
            return UNKNOWN_VAL;
        }
        nextValue = points.firstPoints.at(nextX);
    }
    if (prevX != nextX) {
        value = prevValue + (nextValue-prevValue)*(xcoordInRect - prevX)/(nextX-prevX);
    }

    return value;
}

static void align(int start, int end, int win, int step, int seqLen, int& alignedFirst, int& alignedLast) {
    int win2 = (win + 1) / 2;
    int notAlignedFirst = start - win2;
    alignedFirst = qMax(0, notAlignedFirst - notAlignedFirst % step);

    int notAlignedLast = end + win + step;
    alignedLast = notAlignedLast - notAlignedLast % step;
    while (alignedLast + win2 >= end + step) {
        alignedLast-=step;
    }
    while (alignedLast > seqLen - win) {
        alignedLast-=step;
    }
    assert(alignedLast % step == 0);
    assert(alignedFirst % step == 0);
}

void GSequenceGraphDrawer::calculatePoints(const QSharedPointer<GSequenceGraphData>& d, PairVector& points, float& min, float& max, int numPoints) {
    const U2Region& vr = view->getVisibleRange();

    int step = wdata.step;
    int win = wdata.window, win2 = (win+1)/2;
    qint64 seqLen = view->getSequenceLength();

    points.firstPoints.resize(numPoints);
    points.firstPoints.fill(UNKNOWN_VAL);
    points.secondPoints.resize(numPoints);
    points.secondPoints.fill(UNKNOWN_VAL);

    min = UNKNOWN_VAL;
    max = UNKNOWN_VAL;
    int alignedFirst = 0; //start point for the first window
    int alignedLast = 0; //start point for the last window
    align(vr.startPos, vr.endPos(), win, step, seqLen, alignedFirst, alignedLast);
    int nSteps = (alignedLast - alignedFirst) / step;

    bool winStepNotChanged = win == d->cachedW && step == d->cachedS ;
    bool numPointsNotChanged = numPoints == d->cachedData.firstPoints.size();

    if (!calculationTaskRunner.isIdle() && winStepNotChanged && d->cachedData.firstPoints.size() == 0) { //first time calculation condition
        return;
    }
    CalculatePointsTask *calculationTask = NULL;
    bool useIntervals = nSteps > numPoints;
    if (winStepNotChanged) {
        bool isCacheValid = vr.length == d->cachedLen && vr.startPos == d->cachedFrom;
        if (!isCacheValid || d->cachedData.firstPoints.size() != numPoints) {
            U2OpStatusImpl os;
            GraphPointsUpdater graphUpdater(d, numPoints, alignedFirst, alignedLast, !useIntervals, wdata, view->getSequenceObject(), vr, os);
            graphUpdater.updateGraphData();
        }

        points = d->cachedData;
    } else if (useIntervals) {
        int stepsPerPoint = nSteps / points.firstPoints.size();
        int basesPerPoint = stepsPerPoint * step;

        //<=step because of boundary conditions -> number of steps can be changed if alignedLast+w2 == end
        bool offsetIsTooSmall = qAbs((d->alignedLC - d->alignedFC) - (alignedLast - alignedFirst)) <= step
                        && (qAbs(alignedFirst - d->alignedFC) < basesPerPoint);

        if (offsetIsTooSmall && winStepNotChanged && numPointsNotChanged && vr.length == d->cachedLen ) {
            points = d->cachedData;
        } else {
            calculationTask = new CalculatePointsTask(d, numPoints, alignedFirst, alignedLast, false, wdata, view->getSequenceObject(), vr);
        }
    } else {
        if(vr.startPos + win2 <= seqLen){
            calculationTask = new CalculatePointsTask(d, numPoints, alignedFirst, alignedLast, true, wdata, view->getSequenceObject(), vr);
        }
    }

    if (calculationTask != NULL) {
        calculationTaskRunner.run(calculationTask);
        d->cachedData = PairVector();
        return;
    }

    // Calculate min-max values, ignore unknown values
    bool inited = false; min = 0; max = 0;
    foreach(float p, points.firstPoints) {
        if (isUnknownValue(p)) {
            continue;
        }
        if (!inited) {
            inited = true;
            min = p;
            max = p;
        } else {
            min = qMin(p, min);
            max = qMax(p, max);
        }
    }
    // If interval based graph -> adjust min-max values with second graph data
    if (points.useIntervals)    {
        foreach(float p, points.secondPoints) {
            if (isUnknownValue(p)) {
                continue;
            }
            min = qMin(p, min);
            max = qMax(p, max);

        }
    }
}

void GSequenceGraphDrawer::showSettingsDialog() {
    QObjectScopedPointer<GraphSettingsDialog> dlg = new GraphSettingsDialog(this, U2Region(1, view->getSequenceLength()-1), view);
    dlg->exec();
    CHECK(!dlg.isNull(), );

    if (dlg->result() == QDialog::Accepted) {
        wdata.window = dlg->getWindowSelector()->getWindow();
        wdata.step = dlg->getWindowSelector()->getStep();
        commdata.enableCuttoff = dlg->getMinMaxSelector()->getState();
        commdata.minEdge = dlg->getMinMaxSelector()->getMin();
        commdata.maxEdge = dlg->getMinMaxSelector()->getMax();
        lineColors = dlg->getColors();
        view->update();
        view->changeLabelsColor();
    }
}

bool PairVector::isEmpty()const {
    QVector<float> emptyFp(firstPoints.size(), GSequenceGraphDrawer::UNKNOWN_VAL);
    QVector<float> emptySp(secondPoints.size(), GSequenceGraphDrawer::UNKNOWN_VAL);
    QVector<float> emptyCutoff(0);
    return firstPoints == emptyFp && secondPoints == emptySp && cutoffPoints == emptyCutoff;
}

CalculatePointsTask::CalculatePointsTask(const QSharedPointer<GSequenceGraphData>& d, int numPoints, int alignedFirst, int alignedLast, bool expandMode, const GSequenceGraphWindowData &wdata, U2SequenceObject* o, const U2Region &visibleRange)
    : BackgroundTask<PairVector>(tr("Calculate graph points"), TaskFlag_None),
    graphUpdater(d, numPoints, alignedFirst, alignedLast, expandMode, wdata, o, visibleRange, stateInfo)
{
}

void CalculatePointsTask::run() {
    graphUpdater.recalculateGraphData();
}

GraphPointsUpdater::GraphPointsUpdater(const QSharedPointer<GSequenceGraphData>& d, int numPoints, int alignedFirst, int alignedLast, bool expandMode, const GSequenceGraphWindowData &wdata,
    U2SequenceObject* o, const U2Region &visibleRange, U2OpStatus& os)
    : d(d),
    alignedFirst(alignedFirst),
    alignedLast(alignedLast),
    expandMode(expandMode),
    wdata(wdata),
    o(o),
    visibleRange(visibleRange),
    os(os)
{
    result.firstPoints.resize(numPoints);
    result.firstPoints.fill(GSequenceGraphDrawer::UNKNOWN_VAL);
    result.secondPoints.resize(numPoints);
    result.secondPoints.fill(GSequenceGraphDrawer::UNKNOWN_VAL);
}

void GraphPointsUpdater::recalculateGraphData() {
    CHECK(!o.isNull(),);

    QVector<float> newCutoff;
    int lastAligned = o->getSequenceLength() - o->getSequenceLength() % wdata.step;
    U2Region r = U2Region(0, lastAligned);
    d->ga->calculate(result.allCutoffPoints, o, r, &wdata, os);

    updateGraphData();
}

void GraphPointsUpdater::updateGraphData() {
    setChahedDataParametrs();

    if (result.allCutoffPoints.isEmpty()) {
        result.allCutoffPoints = d->cachedData.allCutoffPoints;
    }
    calculateCutoffPoints();
    CHECK_OP(os, );
    if (expandMode) {
        calculateWithExpand();
    } else {
        calculateWithFit();
    }
    CHECK_OP(os, );

    d->cachedData = result;
    d->cachedData.useIntervals = !expandMode;
}

QVector<float> GraphPointsUpdater::getCutoffRegion(int regionStart, int regionEnd) {
    int firstPointIndex = regionStart / wdata.step;
    int lastPointIndex = qMin(regionEnd / wdata.step + 1, (qint64)result.allCutoffPoints.length());

    return result.allCutoffPoints.mid(firstPointIndex, lastPointIndex - firstPointIndex);
}

void GraphPointsUpdater::calculateWithFit() {
    int nPoints = result.firstPoints.size();
    float basesPerPoint = (alignedLast - alignedFirst) / float(nPoints);
    CHECK(int(basesPerPoint) >= wdata.step, ); //ensure that every point is associated with some step data
    QVector<float> pointData;
    qint64 len = qMax(qint64(basesPerPoint), wdata.window);

    int lastBase = alignedLast + wdata.window;

    for (int i = 0; i < nPoints; i++) {
        pointData.clear();
        qint64 startPos = alignedFirst + qint64(i * basesPerPoint);
        qint64 endPos = startPos + len;
        CHECK(endPos <= lastBase, );

        pointData = GraphPointsUpdater::getCutoffRegion(startPos, endPos - wdata.window);

        CHECK_OP(os, );
        float min, max;
        GSequenceGraphUtils::calculateMinMax(pointData, min, max, os);
        CHECK_OP(os, );

        result.firstPoints[i] = max; //BUG:422: support interval based graph!!!
        result.secondPoints[i] = min;
    }
}

void GraphPointsUpdater::calculateWithExpand() {
    int win = wdata.window;
    int win2 = (win + 1) / 2;
    int step = wdata.step;
    SAFE_POINT((alignedLast - alignedFirst) % step == 0, "Incorrect region for graph calculation is detected", );

    if (alignedFirst + win > o->getSequenceLength()){
        return;
    }

    QVector<float> res = getCutoffRegion(alignedFirst, alignedLast);

    //0 or 1 step is before the visible range
    SAFE_POINT(alignedFirst + win2 + step >= visibleRange.startPos, "Incorrect region for graph calculation is detected", );
    SAFE_POINT(alignedLast + win2 - step <= visibleRange.endPos(), "Incorrect region for graph calculation is detected", );

    bool hasBeforeStep = alignedFirst + win2 < visibleRange.startPos;
    bool hasAfterStep = alignedLast + win2 >= visibleRange.endPos();

    int firstBaseOffset = hasBeforeStep ?
        (step - (visibleRange.startPos - (alignedFirst + win2)))
        : (alignedFirst + win2 - visibleRange.startPos);
    int lastBaseOffset = hasAfterStep ?
        (step - (alignedLast + win2 - visibleRange.endPos()))  //extra step on the right is available
        : (visibleRange.endPos() - (alignedLast + win2)); // no extra step available -> end of the sequence

    SAFE_POINT(firstBaseOffset >= 0 && lastBaseOffset >= 0, "Incorrect offset is detected",);
    SAFE_POINT(hasBeforeStep ? (firstBaseOffset < step && firstBaseOffset != 0) : firstBaseOffset <= win2, "Incorrect offset is detected",);
    SAFE_POINT(hasAfterStep ? (lastBaseOffset <= step && lastBaseOffset != 0) : lastBaseOffset < win2 + step, "Incorrect offset is detected",);

    float base2point = result.firstPoints.size() / (float)visibleRange.length;

    int ri = hasBeforeStep ? 1 : 0;
    int rn = hasAfterStep ? res.size() - 1 : res.size();
    for (int i = 0; ri < rn; ri++, i++) {
        int b = firstBaseOffset + i * step;
        int px = int(b * base2point);
        CHECK_BREAK(px < result.firstPoints.size());
        result.firstPoints[px] = res[ri];
    }

    //restore boundary points if possible
    if (res.size() < 2){
        return;
    }

    if (hasBeforeStep && !GSequenceGraphDrawer::isUnknownValue(res[0]) && !GSequenceGraphDrawer::isUnknownValue(res[1])) {
        assert(firstBaseOffset > 0);
        float k = firstBaseOffset / (float)step;
        float val = res[1] + (res[0] - res[1])*k;
        result.firstPoints[0] = val;
    }

    if (hasAfterStep && !GSequenceGraphDrawer::isUnknownValue(res[rn - 1]) && !GSequenceGraphDrawer::isUnknownValue(res[rn])) {
        assert(lastBaseOffset > 0);
        float k = lastBaseOffset / (float)step;
        float val = res[rn - 1] + (res[rn] - res[rn - 1])*k;
        result.firstPoints[result.firstPoints.size() - 1] = val;
    }
}

void GraphPointsUpdater::calculateCutoffPoints(){
    if (alignedFirst + wdata.window > o->getSequenceLength()) {
        return;
    }
    result.cutoffPoints = GraphPointsUpdater::getCutoffRegion(alignedFirst, alignedLast);
}

void GraphPointsUpdater::setChahedDataParametrs() {
    d->cachedFrom = visibleRange.startPos;
    d->cachedLen = visibleRange.length;
    d->cachedW = wdata.window;
    d->cachedS = wdata.step;
    d->alignedFC = alignedFirst;
    d->alignedLC = alignedLast;
}

PairVector::PairVector():useIntervals(false) {}

} // namespace
