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

#include "ADVGraphModel.h"
#include <cmath>
#include <limits>

#include <U2Core/DNASequenceObject.h>
#include <U2Core/L10n.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Timer.h>
#include <U2Core/U2SafePoints.h>

#include "GSequenceGraphView.h"
#include "GraphSettingsDialog.h"
#include "WindowStepSelectorWidget.h"

namespace U2 {

GSequenceGraphData::GSequenceGraphData(GSequenceGraphView* view, const QString& _graphName, GSequenceGraphAlgorithm* _algorithm)
    : graphName(_graphName), algorithm(_algorithm), labels(view->getRenderArea()) {
}

void GSequenceGraphData::clearAllPoints() {
    dataPoints.clear();
    viewPoints.clear();
    minViewPoints.clear();
    useIntervals = false;
    visibleMin = GSequenceGraphUtils::UNDEFINED_GRAPH_VALUE;
    visibleMax = GSequenceGraphUtils::UNDEFINED_GRAPH_VALUE;
    visibleRange = {};
    labels.deleteAllLabels();
    labels.getMovingLabel()->setVisible(false);
}

const float GSequenceGraphUtils::UNDEFINED_GRAPH_VALUE = std::numeric_limits<float>::quiet_NaN();

/** Checks if the given graph value must be considered as 'undefined'. */
bool GSequenceGraphUtils::isUndefined(float value) {
    return std::isnan(value);
}

float GSequenceGraphUtils::getMinValue(float value1, float value2) {
    return isUndefined(value1) ? value2 : isUndefined(value2) ? value1
                                                              : qMin(value1, value2);
}

float GSequenceGraphUtils::getMaxValue(float value1, float value2) {
    return isUndefined(value1) ? value2 : isUndefined(value2) ? value1
                                                              : qMax(value1, value2);
}

qint64 GSequenceGraphUtils::getNumSteps(const U2Region& range, qint64 window, qint64 step) {
    return range.length <= window ? 1 : (range.length - window) / step + 1;
}

float GSequenceGraphUtils::getPointValue(const QSharedPointer<GSequenceGraphData>& graph, int x) {
    SAFE_POINT(!graph->viewPoints.isEmpty(), "calculatePointValue is called on non-ready state", 0);
    SAFE_POINT(x >= 0 && x < graph->viewPoints.size(), "Illegal X coordinate", 0);
    if (!graph->useIntervals) {
        return graph->viewPoints.at(x);
    }
    float maxValue = graph->viewPoints.at(x);
    float minValue = graph->minViewPoints.at(x);
    return isUndefined(maxValue) || isUndefined(minValue) ? UNDEFINED_GRAPH_VALUE : (maxValue + minValue) / 2;
}

QPair<float, float> GSequenceGraphUtils::getMinAndMaxInRange(const QSharedPointer<GSequenceGraphData>& graph, const U2Region& screenRegion) {
    QPair<float, float> result(UNDEFINED_GRAPH_VALUE, UNDEFINED_GRAPH_VALUE);
    for (qint64 x = screenRegion.startPos, n = screenRegion.endPos(); x < n; x++) {
        float value = getPointValue(graph, x);
        result.first = getMinValue(value, result.first);
        result.second = getMaxValue(value, result.second);
    }
    return result;
}

//////////////////////////////////////////////////////////////////////////
// drawer
GSequenceGraphDrawer::GSequenceGraphDrawer(GSequenceGraphView* view, qint64 _window, qint64 _step)
    : QObject(view), view(view), window(_window), step(_step), visibleMin(UNDEFINED_GRAPH_VALUE), visibleMax(UNDEFINED_GRAPH_VALUE) {
    DEFAULT_COLOR = tr("Default color");
    defFont = new QFont("Arial", 8);  // TODO: unsafe. MS-only font.
    seriesColorByName.insert(DEFAULT_COLOR, Qt::black);

    connect(&calculationTaskRunner, &BackgroundTaskRunner_base::si_finished, this, &GSequenceGraphDrawer::sl_calculationTaskFinished);
}

GSequenceGraphDrawer::~GSequenceGraphDrawer() {
    delete defFont;
}

qint64 GSequenceGraphDrawer::getWindow() const {
    return window;
}

qint64 GSequenceGraphDrawer::getStep() const {
    return step;
}

const GSequenceGraphMinMaxCutOffState& GSequenceGraphDrawer::getCutOffState() const {
    return cutOffState;
}

const ColorMap& GSequenceGraphDrawer::getColors() const {
    return seriesColorByName;
}

void GSequenceGraphDrawer::setColors(const ColorMap& colorMap) {
    seriesColorByName = colorMap;
}

void GSequenceGraphDrawer::sl_calculationTaskFinished() {
    if (calculationTaskRunner.isSuccessful()) {
        emit si_graphDataUpdated();
    } else {
        emit si_graphRenderError();
    }
}

void GSequenceGraphDrawer::draw(QPainter& p, const QList<QSharedPointer<GSequenceGraphData>>& graphs, const QRect& rect) {
    calculatePoints(graphs, rect.width());
    if (!calculationTaskRunner.isIdle()) {
        p.fillRect(rect, Qt::gray);
        p.drawText(rect, Qt::AlignCenter, tr("Graph is rendering..."));
        return;
    }
    visibleMin = cutOffState.isEnabled ? cutOffState.min : UNDEFINED_GRAPH_VALUE;
    visibleMax = cutOffState.isEnabled ? cutOffState.max : UNDEFINED_GRAPH_VALUE;
    if (!cutOffState.isEnabled) {
        for (const QSharedPointer<GSequenceGraphData>& graph : qAsConst(graphs)) {
            visibleMin = getMinValue(graph->visibleMin, visibleMin);
            visibleMax = getMaxValue(graph->visibleMax, visibleMax);
        }
    }
    if (isUndefined(visibleMin) || isUndefined(visibleMax) || graphs.isEmpty() || graphs[0]->viewPoints.isEmpty()) {
        p.drawText(rect, Qt::AlignCenter, tr("Graph is not available. Try to change calculation settings."));
        return;
    }

    for (const QSharedPointer<GSequenceGraphData>& graph : qAsConst(graphs)) {
        drawGraph(p, graph, rect);
        const QList<GraphLabel*>& labels = graph->labels.getLabels();
        for (GraphLabel* label : qAsConst(labels)) {
            bool isVisible = updateLabel(graph, label, rect);
            label->setVisible(isVisible);
        }
    }
    updateMovingLabels(graphs, rect);

    // Draw min & max lines & labels.
    QPen minMaxPen(Qt::DashDotDotLine);
    minMaxPen.setWidth(1);
    p.setPen(minMaxPen);
    p.setFont(*defFont);

    // Visible max value.
    p.drawLine(rect.topLeft(), rect.topRight());
    QRect maxTextRect(rect.x(), rect.y(), rect.width(), 12);
    p.drawText(maxTextRect, Qt::AlignRight, QString::number(visibleMax, 'g', 4));

    // Visible min value.
    p.drawLine(rect.bottomLeft(), rect.bottomRight());
    QRect minTextRect(rect.x(), rect.bottom() - 12, rect.width(), 12);
    p.drawText(minTextRect, Qt::AlignRight, QString::number(visibleMin, 'g', 4));
}

void GSequenceGraphDrawer::drawGraph(QPainter& p, const QSharedPointer<GSequenceGraphData>& graph, const QRect& rect) const {
    int width = rect.width();
    SAFE_POINT(graph->viewPoints.size() == width, "Invalid count of view points in graph", );

    QPen graphPen(Qt::SolidLine);
    graphPen.setColor(seriesColorByName.value(seriesColorByName.contains(graph->graphName) ? graph->graphName : DEFAULT_COLOR));
    graphPen.setWidth(1);
    p.setPen(graphPen);

    int graphHeight = rect.height() - 2;
    float valueToHeightScale = visibleMin == visibleMax ? 1 : (graphHeight - 1) / (visibleMax - visibleMin);

    int prevX = INT_MAX;  // INT_MAX is a default value that means that prevX/Y is not defined yet.
    int prevY = INT_MAX;
    auto hasPrevPoint = [&prevX]() { return prevX != INT_MAX; };
    auto drawNextPoint = [&](int xOffset, float value) {
        CHECK(!isUndefined(value), );
        if (cutOffState.isEnabled) {
            value = value >= visibleMax ? visibleMax : value <= visibleMin ? visibleMin
                                                                           : (visibleMax + visibleMin) / 2;
        }
        int height = visibleMin == visibleMax ? graphHeight / 2 : qRound((value - visibleMin) * valueToHeightScale);
        int y = rect.bottom() - 1 - height;
        int x = rect.left() + xOffset;
        if (hasPrevPoint()) {
            p.drawLine(x, y, prevX, prevY);
        } else {
            p.drawPoint(x, y);
        }
        prevX = x;
        prevY = y;
    };

    if (graph->useIntervals) {
        SAFE_POINT(graph->viewPoints.size() == graph->minViewPoints.size(), "viewPoints.size != minViewPoints.size", );
        for (int xOffset = 0, n = width; xOffset < n; xOffset++) {
            float maxValue = graph->viewPoints[xOffset];
            float minValue = graph->minViewPoints[xOffset];
            if (cutOffState.isEnabled) {
                drawNextPoint(xOffset, (minValue + maxValue) / 2);
            } else {
                drawNextPoint(xOffset, maxValue);
                if (maxValue != minValue) {
                    drawNextPoint(xOffset, minValue);
                }
            }
        }
    } else {
        for (int xOffset = 0, n = width; xOffset < n; xOffset++) {
            drawNextPoint(xOffset, graph->viewPoints[xOffset]);
        }
    }
}

void GSequenceGraphDrawer::addLabelsForLocalMinMaxPoints(const QSharedPointer<GSequenceGraphData>& graph, const U2Region& sequenceRange, const QRect& rect) {
    const QVector<float>& data = graph->dataPoints;
    int firstDataIndex = (int)(qMax((qint64)0, sequenceRange.startPos - (window + 1) / 2) / step);
    int lastDataIndex = qMin((int)((sequenceRange.endPos() - window / 2) / step), data.size() - 1);
    CHECK(firstDataIndex < lastDataIndex, );

    // Find a median value. Maximums will be marked above the median, minimums below.
    double sumOfValues = 0;
    for (qint64 dataIndex = firstDataIndex; dataIndex <= lastDataIndex; dataIndex++) {
        sumOfValues += data[dataIndex];
    }
    double medianValue = sumOfValues / (lastDataIndex - firstDataIndex + 1);
    // Find all minimums & maximums. // TODO: do not add too many labels! Show an error.
    for (qint64 dataIndex = firstDataIndex + 1; dataIndex < lastDataIndex; dataIndex++) {
        float prevValue = data[dataIndex - 1];
        float value = data[dataIndex];
        float nextValue = data[dataIndex + 1];
        bool isLocalMin = value < prevValue && value < nextValue && value < medianValue;
        bool isLocalMax = value > prevValue && value > nextValue && value > medianValue;
        if (!isLocalMin && !isLocalMax) {
            continue;
        }
        float labelPos = (float)window / 2 + dataIndex * step;
        if (graph->labels.findLabelByPosition(labelPos) != nullptr) {
            continue;  // Do not add the same label twice.
        }
        auto label = new GraphLabel(labelPos, view->getRenderArea());
        graph->labels.addLabel(label);
        bool isVisible = updateLabel(graph, label, rect);
        label->setVisible(isVisible);
    }
}

bool GSequenceGraphDrawer::updateLabel(const QSharedPointer<GSequenceGraphData>& graph, GraphLabel* label, const QRect& rect) const {
    QColor color = seriesColorByName.value(seriesColorByName.contains(graph->graphName) ? graph->graphName : DEFAULT_COLOR);
    label->setColor(color, color);

    bool isVisible = updateCoordinatesAndText(graph, rect, label);
    CHECK(isVisible, false);

    QRectF labelTextRect = label->getTextBoxRect();  // After 'updateCoordinatesAndText' this is a pure text bounding box.
    int labelBorderWidth = label->getTextBox()->lineWidth();
    int boxHeight = qRound(labelTextRect.height()) + 2 * labelBorderWidth;
    int boxWidth = qRound(labelTextRect.width()) + 2 * labelBorderWidth;

    int areaWidth = rect.width();
    int boxX = qBound(2, label->getCoord().x() - boxWidth / 2, areaWidth - boxWidth - 2);
    int boxY = label->getCoord().y();

    bool isTextBelowDot = rect.top() > boxY - label->getDotRadius() - boxHeight;
    boxY += isTextBelowDot ? label->getDotRadius() + 1 : -(label->getDotRadius() + boxHeight);
    label->setTextRect(QRect(boxX, boxY, boxWidth, boxHeight));
    return true;
}

void GSequenceGraphDrawer::adjustMovingLabelGroupPositions(const QList<GraphLabel*>& labels, int viewWidth) {
    CHECK(labels.size() > 1, );
    int groupWidth = 0;
    int labelSpacing = 4;  // Spacing between labels.
    int commonY = INT_MAX;  // Align labels on top of all dots to avoid label & dot overlap.
    for (GraphLabel* label : qAsConst(labels)) {
        QRect rect = label->getTextBoxRect();
        groupWidth += rect.width() + (groupWidth > 0 ? labelSpacing : 0);
        int dotY = label->getCoord().y();
        commonY = qMin(dotY > rect.y() ? rect.y() : dotY - rect.height() - 1, commonY);
    }
    int x = qBound(2, labels[0]->getCoord().x() - groupWidth / 2, viewWidth - (groupWidth + 2));
    for (GraphLabel* label : qAsConst(labels)) {
        QRect rect = label->getTextBoxRect();
        int xShift = x - rect.x();
        int yShift = commonY - rect.y();
        label->setTextRect(rect.adjusted(xShift, yShift, xShift, yShift));
        x += rect.width() + labelSpacing;
    }
}

void GSequenceGraphDrawer::updateMovingLabels(const QList<QSharedPointer<GSequenceGraphData>>& graphs, const QRect& rect) const {
    QList<GraphLabel*> visibleMovingLabels;
    for (const QSharedPointer<GSequenceGraphData>& graph : qAsConst(graphs)) {
        GraphLabel* label = graph->labels.getMovingLabel();
        bool isVisible = updateLabel(graph, label, rect);
        label->setVisible(isVisible);
        if (isVisible) {
            updateMovingLabelMarkState(graph, label);
            label->setColor(label->getFillColor(), Qt::red);  // TODO: what if the custom color of the label is already red (like in Frame Plot chart)?
            visibleMovingLabels.append(label);
        }
    }

    adjustMovingLabelGroupPositions(visibleMovingLabels, rect.width());
}

int GSequenceGraphDrawer::getScreenOffsetByPos(double sequencePos, int screenWidth) const {
    const U2Region& visibleRange = view->getVisibleRange();
    CHECK(sequencePos >= visibleRange.startPos && sequencePos < visibleRange.endPos(), -1);
    double sequenceToScreenScale = (double)screenWidth / visibleRange.length;
    double x = qRound64((sequencePos - visibleRange.startPos) * sequenceToScreenScale);
    return x < 0 || x >= screenWidth ? -1 : (int)x;
}

bool GSequenceGraphDrawer::updateCoordinatesAndText(const QSharedPointer<GSequenceGraphData>& graph, const QRect& rect, GraphLabel* label) const {
    const U2Region& visibleRange = view->getVisibleRange();
    CHECK(visibleRange.contains(label->getPosition()), false);

    int labelX = getScreenOffsetByPos(label->getPosition(), rect.width());
    CHECK(labelX >= 0 && labelX < graph->viewPoints.size(), false);

    float value = getPointValue(graph, labelX);
    label->setValue(value);

    CHECK(!isUndefined(value), false);

    int pos = qRound(label->getPosition());
    QString text = GSequenceGraphView::tr("[%2, %3]").arg(QString::number(pos)).arg(QString::number(value));
    if (graph->useIntervals) {
        float maxValue = graph->viewPoints.at(labelX);
        float minValue = graph->minViewPoints.at(labelX);
        if (maxValue != minValue) {
            text = GSequenceGraphView::tr("[%2, max:%3, min:%4]").arg(QString::number(pos)).arg(QString::number(maxValue)).arg(QString::number(minValue));
        }
    }

    int labelY;
    int graphHeight = rect.bottom() - rect.top() - 2;
    if (cutOffState.isEnabled) {
        if (value >= cutOffState.max) {
            labelY = graphHeight;
        } else if (value > cutOffState.min && value < cutOffState.max) {
            labelY = graphHeight / 2;
        } else {
            labelY = 0;
        }
    } else if (visibleMin == visibleMax) {
        labelY = graphHeight / 2;
    } else {
        float valueToHeightScale = (graphHeight - 1) / (visibleMax - visibleMin);
        labelY = qRound((value - visibleMin) * valueToHeightScale);
    }

    QPoint labelCoord(labelX, rect.bottom() - 1 - labelY);
    label->setCoord(labelCoord);
    label->setText(text);
    label->getTextBox()->adjustSize();
    return true;
}

void GSequenceGraphDrawer::updateMovingLabelMarkState(const QSharedPointer<GSequenceGraphData>& graph, GraphLabel* label) const {
    int labelX = label->getCoord().x();
    int rangeToCheck = 50;
    int nPoints = graph->viewPoints.size();
    int startX = qBound(0, labelX - rangeToCheck / 2, nPoints);
    int endX = qBound(startX, labelX + rangeToCheck / 2, nPoints);
    QPair<float, float> minAndMax = getMinAndMaxInRange(graph, {startX, endX - startX});
    bool flat = qFuzzyCompare(minAndMax.first, minAndMax.second);
    bool isMin = qFuzzyCompare(label->getValue(), minAndMax.first);
    bool isMax = qFuzzyCompare(label->getValue(), minAndMax.second);
    bool isExtremum = !flat && (isMin || isMax);
    if (isExtremum) {
        label->mark();
    } else {
        label->unmark();
    }
}

void GSequenceGraphDrawer::calculatePoints(const QList<QSharedPointer<GSequenceGraphData>>& graphs, int viewWidth) {
    SAFE_POINT(viewWidth > 0, "Illegal view width", );
    SAFE_POINT(!graphs.isEmpty(), "Graphs are empty!", );

    U2SequenceObject* sequenceObject = view->getSequenceObject();
    qint64 sequenceLength = sequenceObject->getSequenceLength();
    SAFE_POINT(sequenceLength > 0, "Illegal sequence length", );

    const QSharedPointer<GSequenceGraphData>& blueprintGraph = graphs[0];
    bool isAlgorithmParamsChanged = blueprintGraph->window != window ||
                                    blueprintGraph->step != step ||
                                    blueprintGraph->sequenceLength != sequenceLength;
    if (isAlgorithmParamsChanged) {
        // Recompute data points.
        for (const QSharedPointer<GSequenceGraphData>& graph : qAsConst(graphs)) {
            graph->clearAllPoints();
            graph->window = window;
            graph->step = step;
            graph->sequenceLength = sequenceLength;
        }
        calculationTaskRunner.run(new CalculatePointsTask(graphs, sequenceObject));
        return;
    }
    if (!calculationTaskRunner.isIdle()) {
        return;  // Calculation is in progress with the same params. Wait until it is finished.
    }
    const U2Region& visibleRange = view->getVisibleRange();
    if (blueprintGraph->visibleRange == visibleRange && blueprintGraph->viewPoints.size() == viewWidth) {
        return;  // Graphs are up-to-date.
    }
    for (const QSharedPointer<GSequenceGraphData>& graph : qAsConst(graphs)) {
        graph->visibleRange = visibleRange;
        double dataPointsPerViewRange = graph->dataPoints.size() * (double)graph->visibleRange.length / sequenceLength;
        double dataPointsPerPixel = dataPointsPerViewRange / viewWidth;
        if (dataPointsPerPixel < 1) {
            expandDataPointsToView(graph, viewWidth);
        } else {
            packDataPointsIntoView(graph, viewWidth);
        }
    }
}

void GSequenceGraphDrawer::showSettingsDialog() {
    QObjectScopedPointer<GraphSettingsDialog> dlg = new GraphSettingsDialog(this, U2Region(1, view->getSequenceLength() - 1), view);
    dlg->exec();
    CHECK(!dlg.isNull(), );

    if (dlg->result() == QDialog::Accepted) {
        window = dlg->getWindowSelector()->getWindow();
        step = dlg->getWindowSelector()->getStep();
        cutOffState.isEnabled = dlg->getMinMaxSelector()->getState();
        cutOffState.min = dlg->getMinMaxSelector()->getMin();
        cutOffState.max = dlg->getMinMaxSelector()->getMax();
        seriesColorByName = dlg->getColors();
        view->update();  // View update will trigger graphs & labels redraw.
    }
}

CalculatePointsTask::CalculatePointsTask(const QList<QSharedPointer<GSequenceGraphData>>& _graphDataList, U2SequenceObject* _sequenceObject)
    : BackgroundTask<QList<QVector<float>>>(tr("Calculate graph points"), TaskFlag_None),
      graphDataList(_graphDataList), sequenceObject(_sequenceObject) {
}

void CalculatePointsTask::run() {
    CHECK(!sequenceObject.isNull(), );

    GTIMER(cvar, tvar, "GraphPointsUpdater::calculateAlgorithmPoints");
    for (auto graphData : qAsConst(graphDataList)) {
        try {
            QVector<float> dataPoints;
            graphData->algorithm->calculate(dataPoints, sequenceObject, graphData->window, graphData->step, stateInfo);
            CHECK_OP(stateInfo, );
            result << dataPoints;
        } catch (const std::bad_alloc&) {
            setError(L10N::outOfMemory() + ", graph: " + graphData->graphName);
        } catch (...) {
            setError(L10N::internalError("graph " + graphData->graphName));
        }
        if (hasError()) {
            result.clear();
            break;
        }
    }
}

Task::ReportResult CalculatePointsTask::report() {
    CHECK(!stateInfo.isCoR(), Task::ReportResult_Finished);

    QList<QVector<float>> result = getResult();
    CHECK_EXT(result.size() == graphDataList.size(), tr("Graph implementation didn't produce expected result"), ReportResult_Finished);

    for (int i = 0; i < result.size(); i++) {
        graphDataList[i]->dataPoints = result[i];
    }
    return Task::ReportResult_Finished;
}

void GSequenceGraphDrawer::packDataPointsIntoView(const QSharedPointer<GSequenceGraphData>& graph, int viewWidth) const {
    graph->useIntervals = true;
    const QVector<float>& data = graph->dataPoints;
    double dataPointsPerBase = (double)data.size() / graph->sequenceLength;
    double dataPointsPerViewRange = dataPointsPerBase * graph->visibleRange.length;
    double dataPointsPerPixel = dataPointsPerViewRange / viewWidth;
    double x0DataPoint = dataPointsPerBase * graph->visibleRange.startPos;
    graph->viewPoints.clear();
    graph->minViewPoints.clear();
    for (int x = 0; x < viewWidth; x++) {
        double dataPointOffset = x0DataPoint + x * dataPointsPerPixel;
        int fromIndex = qRound(dataPointOffset);
        int length = qMax(1, qRound(dataPointsPerPixel));

        float min = *std::min_element(data.begin() + fromIndex, data.begin() + fromIndex + length);
        float max = *std::max_element(data.begin() + fromIndex, data.begin() + fromIndex + length);
        graph->visibleMin = x == 0 ? min : qMin(min, graph->visibleMin);
        graph->visibleMax = x == 0 ? max : qMax(max, graph->visibleMax);
        graph->viewPoints.append(max);
        graph->minViewPoints.append(min);
    }
}

/** Helper data structure used by expandDataPointsToView. */
struct DataByX {
    int x = 0;
    float value = GSequenceGraphUtils::UNDEFINED_GRAPH_VALUE;
};

void GSequenceGraphDrawer::expandDataPointsToView(const QSharedPointer<GSequenceGraphData>& graph, int viewWidth) const {
    SAFE_POINT(graph->window == window && graph->step == step, "Computing graph with illegal window & step values!", );
    graph->useIntervals = false;
    const QVector<float>& data = graph->dataPoints;
    const U2Region& visibleRange = graph->visibleRange;

    graph->viewPoints = QVector<float>(viewWidth, UNDEFINED_GRAPH_VALUE);

    // The first and last data points in used to compute on-screen values.
    // All data points are placed in the middle of their window, so using window/2 shifts.
    int firstDataIndex = (int)(qMax((qint64)0, visibleRange.startPos - (window + 1) / 2) / step);
    int lastDataIndex = qMin((int)((visibleRange.endPos() - window / 2) / step) + 1, data.size() - 1);
    if (lastDataIndex < firstDataIndex) {
        // No data points to map. The visible range is before the first or after the last data points.
        return;
    }

    // Map all data values from the on-screen data range to screen points with a defined 'x' offset.
    QList<DataByX> dataByX;
    double posToXScale = (double)viewWidth / visibleRange.length;
    for (int dataIndex = firstDataIndex; dataIndex <= lastDataIndex; dataIndex++) {
        int windowStartPos = dataIndex * step;
        double dataValuePos = windowStartPos + (double)window / 2;
        int x = qRound(posToXScale * (dataValuePos - visibleRange.startPos));
        dataByX.append({x, data[dataIndex]});
    }
    SAFE_POINT(!dataByX.isEmpty(), "Must be at least one data point in visible range!", );
    if (dataByX.size() == 1) {
        // Only 1 point in the range. Can't build a line -> save this point and return.
        const DataByX& d = dataByX[0];
        if (d.x >= 0 && d.x < viewWidth) {
            graph->viewPoints[d.x] = d.value;
            graph->visibleMin = d.value;
            graph->visibleMax = d.value;
        }
        return;
    }
    // Build continuous linear segments between all defined data points. Save results to 'viewPoints'.
    DataByX prev = dataByX[0];
    int nextDataIndex = 1;
    graph->visibleMin = UNDEFINED_GRAPH_VALUE;
    graph->visibleMax = UNDEFINED_GRAPH_VALUE;
    for (int x = qMax(0, prev.x); x < viewWidth && nextDataIndex < dataByX.size(); x++) {
        const DataByX& next = dataByX[nextDataIndex];
        float value;
        if (x == next.x) {
            value = next.value;
            prev = next;
            nextDataIndex++;
        } else {
            // Somewhere between prev & next.
            float xToValueScale = (next.value - prev.value) / (next.x - prev.x);
            value = prev.value + xToValueScale * (x - prev.x);
        }
        graph->viewPoints[x] = value;
        graph->visibleMin = getMinValue(value, graph->visibleMin);
        graph->visibleMax = getMaxValue(value, graph->visibleMax);
    }
}

}  // namespace U2
