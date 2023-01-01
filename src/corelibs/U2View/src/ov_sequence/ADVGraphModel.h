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

#ifndef _U2_AVD_GRAPH_MODEL_H_
#define _U2_AVD_GRAPH_MODEL_H_

#include <QPixmap>
#include <QPointer>
#include <QVector>

#include <U2Core/BackgroundTaskRunner.h>
#include <U2Core/U2Region.h>
#include <U2Core/global.h>

#include "GraphLabelModel.h"

namespace U2 {

class U2SequenceObject;
class GSequenceGraphData;
class GSequenceGraphView;
class CalculatePointsTask;

/** An window/step based algorithm that computes a data series for DNA sequence object. */
class U2VIEW_EXPORT GSequenceGraphAlgorithm {
public:
    virtual ~GSequenceGraphAlgorithm() = default;

    /**
     * Calculates graph data for the whole sequence range.
     * TODO: the method is called from non-UI thread and should use objectId instead of GObject.
     */
    virtual void calculate(QVector<float>& result, U2SequenceObject* sequenceObject, qint64 window, qint64 step, U2OpStatus& os) = 0;
};

/** Cut-off mode state of graph. Contains min & max cutoffs thresholds. */
class U2VIEW_EXPORT GSequenceGraphMinMaxCutOffState {
public:
    virtual ~GSequenceGraphMinMaxCutOffState() = default;

    double min = 0;
    double max = 0;
    bool isEnabled = false;
};

/** A collection of static methods to work with graph values. */
class U2VIEW_EXPORT GSequenceGraphUtils {
public:
    /** Returns number of steps (data points) to cover 'range' using 'window' & 'step'.*/
    static qint64 getNumSteps(const U2Region& range, qint64 window, qint64 step);

    /** Returns true if the value is 'UNDEFINED_GRAPH_VALUE'. */
    static bool isUndefined(float value);

    /** Returns min of 2 values. Filters out 'UNDEFINED_GRAPH_VALUE'. */
    static float getMinValue(float value1, float value2);

    /** Returns max of 2 values. Filters out 'UNDEFINED_GRAPH_VALUE'. */
    static float getMaxValue(float value1, float value2);

    /** Returns pair of min and max values in the given screen range. */
    static QPair<float, float> getMinAndMaxInRange(const QSharedPointer<GSequenceGraphData>& graph, const U2Region& screenRange);

    /** Returns a value for the given x coordinate. In useIntervals mode the value is average between min & max values. */
    static float getPointValue(const QSharedPointer<GSequenceGraphData>& graph, int x);

    /**
     * A constant with undefined value for a point in a graph.
     * Such points must now be drawn at all or have a tooltip.
     * */
    static const float UNDEFINED_GRAPH_VALUE;
};

/** Map of colors by data series name. */
typedef QMap<QString, QColor> ColorMap;

/** Stateful graph drawing controller. Holds current setting of the graph widget. */
class U2VIEW_EXPORT GSequenceGraphDrawer : public QObject, private GSequenceGraphUtils {
    Q_OBJECT
public:
    GSequenceGraphDrawer(GSequenceGraphView* view, qint64 window, qint64 step);

    virtual ~GSequenceGraphDrawer();

    /** Re-draws graphs in the given rect. */
    void draw(QPainter& p, const QList<QSharedPointer<GSequenceGraphData>>& graphs, const QRect& rect);

    /** Updates moving labels state. */
    void updateMovingLabels(const QList<QSharedPointer<GSequenceGraphData>>& graphs, const QRect& rect) const;

    /** Shows graph set-up dialog. */
    void showSettingsDialog();

    /**
     * Adds local min/max labels for the given sequence region.
     * Only local max points with a value > median value and local min points with a value <a median value got labels.
     */
    void addLabelsForLocalMinMaxPoints(const QSharedPointer<GSequenceGraphData>& graph, const U2Region& sequenceRange, const QRect& rect);

    /** Returns current window value. */
    qint64 getWindow() const;

    /** Returns current step value. */
    qint64 getStep() const;

    /** Returns current cut-off filter state. */
    const GSequenceGraphMinMaxCutOffState& getCutOffState() const;

    /** Returns colors map per graph series name. */
    const ColorMap& getColors() const;

    /** Updates colors map per graph series name. */
    void setColors(const ColorMap& colorMap);

signals:
    /** Emitted on successful graph calculation completion. */
    void si_graphDataUpdated();

    /** Emitted when graph calculation is failed. */
    void si_graphRenderError();

private:
    /** Draws a single graph series. */
    void drawGraph(QPainter& p, const QSharedPointer<GSequenceGraphData>& graph, const QRect& rect) const;

    /** Re-calculates graph series for the given view width. */
    void calculatePoints(const QList<QSharedPointer<GSequenceGraphData>>& graphs, int viewWidth);

    /** Updates label text and coordinates. Returns true if label is visible. */
    bool updateCoordinatesAndText(const QSharedPointer<GSequenceGraphData>& graph, const QRect& rect, GraphLabel* label) const;

    /** Updates static label state. Returns true if label is visible. */
    bool updateLabel(const QSharedPointer<GSequenceGraphData>& graph, GraphLabel* label, const QRect& rect) const;

    /** Packs multiple data points into visible range. Used when there are more data points than pixels. */
    void packDataPointsIntoView(const QSharedPointer<GSequenceGraphData>& graph, int viewWidth) const;

    /** Expands a data point on visible range. Used when there are more pixels than data points. */
    void expandDataPointsToView(const QSharedPointer<GSequenceGraphData>& state, int viewWidth) const;

    /**
     * Returns X coordinate for the sequence position.
     * Using 'double' to support X-offset between sequence bases.
     * If the sequence position can't be mapped to the current screenWidth range a '-1' value is returned.
     */
    int getScreenOffsetByPos(double sequencePos, int screenWidth) const;

    /** Updates label 'mark' (local min/max) state. Returns label value. */
    void updateMovingLabelMarkState(const QSharedPointer<GSequenceGraphData>& graph, GraphLabel* label) const;

    /** Updates moving labels coordinates, so they do not overlap. */
    static void adjustMovingLabelGroupPositions(const QList<GraphLabel*>& labels, int viewWidth);

protected slots:
    /** Emits 'si_graphDataUpdated' or 'si_graphRenderError' based on the calculation task state. */
    void sl_calculationTaskFinished();

private:
    GSequenceGraphView* view;
    QFont* defFont;
    ColorMap seriesColorByName;

    qint64 window;
    qint64 step;
    float visibleMin;
    float visibleMax;
    GSequenceGraphMinMaxCutOffState cutOffState;
    BackgroundTaskRunner<QList<QVector<float>>> calculationTaskRunner;

    QString DEFAULT_COLOR;
};

class U2VIEW_EXPORT GSequenceGraphData {
public:
    GSequenceGraphData(GSequenceGraphView* view, const QString& graphName, GSequenceGraphAlgorithm* algorithm);

    void clearAllPoints();

    QString graphName;

    QSharedPointer<GSequenceGraphAlgorithm> algorithm;

    GraphLabelSet labels;

    /** Sequence range view points were calculated for. */
    U2Region visibleRange;

    /** On-screen points. Contains max interval values if 'useIntervals' is true. */
    QVector<float> viewPoints;

    /** On-screen points. Used only if 'useIntervals' is true and contains min internal values in this case.. */
    QVector<float> minViewPoints;

    /** Original points computed by the graph algorithm in sequence domain. */
    QVector<float> dataPoints;

    /** Minimal visible value. */
    float visibleMin = GSequenceGraphUtils::UNDEFINED_GRAPH_VALUE;

    /** Maximum visible value. */
    float visibleMax = GSequenceGraphUtils::UNDEFINED_GRAPH_VALUE;

    /** If true every point on screen is an interval: {min,max}.*/
    bool useIntervals = false;

    /** Window size using to construct graph data. */
    qint64 window = 0;

    /** Step size used to construct graph data. */
    qint64 step = 0;

    /** Sequence length used to compute the data. */
    qint64 sequenceLength = 0;
};

/** Runs graph algorithms specified in graphData in a separate thread. Saves results to the shared data. */
class CalculatePointsTask : public BackgroundTask<QList<QVector<float>>> {
    Q_OBJECT
public:
    CalculatePointsTask(const QList<QSharedPointer<GSequenceGraphData>>& graphData, U2SequenceObject* o);

    void run() override;

    ReportResult report() override;

private:
    QList<QSharedPointer<GSequenceGraphData>> graphDataList;
    QPointer<U2SequenceObject> sequenceObject;
};

}  // namespace U2

#endif
