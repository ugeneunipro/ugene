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

#ifndef _U2_AVD_GRAPH_MODEL_H_
#define _U2_AVD_GRAPH_MODEL_H_

#include <U2Core/global.h>
#include <U2Core/U2Region.h>
#include <U2Core/BackgroundTaskRunner.h>

#include <QVector>
#include <QPixmap>
#include <QPointer>

#include "GraphLabelModel.h"

namespace U2 {

class U2SequenceObject;
class GSequenceGraphData;
class GSequenceGraphWindowData;
class GSequenceGraphView;
class CalculatePointsTask;

//BUG:402: refactor to remove cross references in structures!

class U2VIEW_EXPORT GSequenceGraphAlgorithm {
public:
    virtual ~GSequenceGraphAlgorithm();

    virtual void calculate(QVector<float> &res, U2SequenceObject *o, const U2Region &r,
        const GSequenceGraphWindowData *d, U2OpStatus &os) = 0;

protected:
    GSequenceGraphAlgorithm();

    const QByteArray & getSequenceData(U2SequenceObject* seqObj, U2OpStatus &os);

private:
    U2SequenceObject *lastSeqObj;
    QByteArray lastSeqData;
};

class U2VIEW_EXPORT GSequenceGraphWindowData {
public:
    GSequenceGraphWindowData() :  step(0), window(0){}
    GSequenceGraphWindowData(qint64 _step, qint64 w) :  step(_step), window(w){}

    virtual ~GSequenceGraphWindowData(){}

    qint64 step;
    qint64 window;
};

class U2VIEW_EXPORT GSequenceGraphMinMaxCutOffData {
public:
    GSequenceGraphMinMaxCutOffData() : minEdge(0.0), maxEdge(0.0), enableCuttoff(false) {}
    GSequenceGraphMinMaxCutOffData(double _min, double _max, bool _enableCuttoff) : minEdge(_min), maxEdge(_max), enableCuttoff(_enableCuttoff){}

    virtual ~GSequenceGraphMinMaxCutOffData(){}

    double minEdge;
    double maxEdge;
    bool enableCuttoff;
};

struct PairVector {
    PairVector();
    QVector<float>  firstPoints;  //max if use both
    QVector<float>  secondPoints;
    QVector<float>  cutoffPoints;
    QVector<float>  allCutoffPoints;
    bool useIntervals;

    bool isEmpty() const;
};

typedef QMap<QString,QColor> ColorMap;

class U2VIEW_EXPORT GSequenceGraphDrawer : public QObject{
    Q_OBJECT
public:
    GSequenceGraphDrawer(GSequenceGraphView* v, const GSequenceGraphWindowData& wd,
        ColorMap colorMap = ColorMap());
    virtual ~GSequenceGraphDrawer();

    virtual void draw(QPainter& p, const QList<QSharedPointer<GSequenceGraphData> >& graphs, const QRect& rect);

    virtual void showSettingsDialog();

    float getGlobalMin(){return globalMin;}
    float getGlobalMax(){return globalMax;}

    void selectExtremumPoints(const QSharedPointer<GSequenceGraphData>& graph, const QRect& graphRect, int windowSize, const U2Region &visibleRange);

    const GSequenceGraphWindowData& getWindowData() {return wdata;}
    const GSequenceGraphMinMaxCutOffData& getCutOffData() {return commdata;}
    const ColorMap& getColors() {return lineColors;}
    void setColors(const ColorMap& colorMap) {lineColors = colorMap;}

    static bool isUnknownValue(float value) {return qFuzzyCompare(value, UNKNOWN_VAL);}

    static const float UNKNOWN_VAL;

signals:
    void si_graphDataUpdated();
    void si_graphRenderError();

protected:
    void drawGraph(QPainter& p, const QSharedPointer<GSequenceGraphData>& graph, const QRect& rect);

    void calculatePoints(const QSharedPointer<GSequenceGraphData>& d, PairVector& points, float& min, float& max, int numPoints);

    bool calculateLabelData(const QRect& rect, const PairVector& points, GraphLabel* label);
    void calculatePositionOfLabel(GraphLabel *label, int nPoints);
    float calculateLabelValue(int nPoints, const PairVector &points, GraphLabel *label, int xcoordInRect);
    float calculatePointValue(const PairVector &points, int xcoordInRect);
    bool updateStaticLabels(const QSharedPointer<GSequenceGraphData>& graph, GraphLabel* label, const QRect& rect);
    void updateMovingLabels(const QSharedPointer<GSequenceGraphData>& graph, GraphLabel* label, const QRect& rect);
    void updateStaticLabels(MultiLabel& multiLabel, const QRect& rect);
    QPair<float, float> getMinAndMaxInRange(const PairVector& points, const U2Region& region);

protected slots:
    void sl_labelAdded(const QSharedPointer<GSequenceGraphData>&, GraphLabel*, const QRect&);
    void sl_labelMoved(const QSharedPointer<GSequenceGraphData>&, GraphLabel*, const QRect&);
    void sl_labelsColorChange(const QSharedPointer<GSequenceGraphData>&);
    void sl_calculationTaskFinished();

protected:
    GSequenceGraphView*             view;
    QFont*                          defFont;
    ColorMap                        lineColors;
    float                           globalMin, globalMax;

    GSequenceGraphWindowData        wdata;
    GSequenceGraphMinMaxCutOffData  commdata;
    BackgroundTaskRunner<PairVector> calculationTaskRunner;

private:
    QString DEFAULT_COLOR;
};


class U2VIEW_EXPORT GSequenceGraphData {
public:
    GSequenceGraphData(const QString& _graphName);
    virtual ~GSequenceGraphData();

    QString                     graphName;
    GSequenceGraphAlgorithm*    ga;

    int                         cachedFrom, cachedLen, cachedW, cachedS;
    int                         alignedFC, alignedLC;
    PairVector                  cachedData;

    MultiLabel                  graphLabels;
};


class U2VIEW_EXPORT GSequenceGraphUtils {
public:
    static int getNumSteps(const U2Region& range, int w, int s);

    static void fitToScreen(const QVector<float>& data, int dataStartBase, int dataEndBase,
                            QVector<float>& results,  int resultStartBase, int resultEndBase,
                            int screenWidth, float unknownVal);

    static float calculateAverage(const QVector<float>& data, float start, float range);
    static void calculateMinMax(const QVector<float>& data, float& min, float& max, U2OpStatus &os);

};

class GraphPointsUpdater {
public:
    GraphPointsUpdater(const QSharedPointer<GSequenceGraphData>& d, int numPoints, int alignedFirst, int alignedLast, bool expandMode, const GSequenceGraphWindowData &wdata, U2SequenceObject* o, const U2Region &visibleRange, U2OpStatus& os);

    void recalculateGraphData();

    void updateGraphData();

    void calculateWithFit();

    // calculates points (< visual area size) and expands points to fill all visual area size
    void calculateWithExpand();

    void calculateCutoffPoints();

    QVector<float> getCutoffRegion(int regionStart, int regionEnd);

private:
    void setChahedDataParametrs();

    QSharedPointer<GSequenceGraphData> d;
    PairVector result;
    int alignedFirst;
    int alignedLast;
    bool expandMode;
    const GSequenceGraphWindowData wdata;
    QPointer<U2SequenceObject> o;
    const U2Region visibleRange;
    U2OpStatus& os;
};

class CalculatePointsTask : public BackgroundTask<PairVector> {
    Q_OBJECT
public:
    CalculatePointsTask(const QSharedPointer<GSequenceGraphData>& d, int numPoints, int alignedFirst, int alignedLast, bool expandMode, const GSequenceGraphWindowData &wdata, U2SequenceObject* o, const U2Region &visibleRange);
    virtual void run();
private:
    GraphPointsUpdater graphUpdater;
};

} // namespace

#endif
