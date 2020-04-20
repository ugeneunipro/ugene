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

#ifndef _U2_GSEQUENCE_GRAPH_VIEW_H_
#define _U2_GSEQUENCE_GRAPH_VIEW_H_

#include "GSequenceLineView.h"
#include "ADVGraphModel.h"

#include <QMenu>

namespace U2 {

class GSequenceGraphWindowData;
class GSequenceGraphView;
class GObjectView;

//////////////////////////////////////////////////////////////////////////
/// View
class U2VIEW_EXPORT GSequenceGraphView : public GSequenceLineView {
    Q_OBJECT
public:
    GSequenceGraphView(QWidget* p, SequenceObjectContext* ctx, GSequenceLineView* baseView, const QString& vName);

    ~GSequenceGraphView();

    const QString& getGraphViewName() const {return vName;}

    void getLabelPositions(QList<QVariant> &labelPositions);

    void createLabelsOnPositions(const QList<QVariant>& positions);

    void addGraphData(const QSharedPointer<GSequenceGraphData> &g);

    void setGraphDrawer(GSequenceGraphDrawer* gd);

    const QList<QSharedPointer<GSequenceGraphData> >& getGraphs() const {return graphs;}

    GSequenceGraphDrawer* getGSequenceGraphDrawer() const {return graphDrawer;}

    void buildPopupMenu(QMenu& m);

    void changeLabelsColor();

protected:
    virtual void pack();
    virtual void addActionsToGraphMenu(QMenu* graphMenu);
    void leaveEvent(QEvent *le);
    void mousePressEvent(QMouseEvent* me);
    void mouseMoveEvent(QMouseEvent* me);
    void addLabel(float xPos);
    void moveLabel(float xPos);
    void hideLabel();
    void onVisibleRangeChanged(bool signal = true);

signals:
    void si_labelAdded(const QSharedPointer<GSequenceGraphData>& , GraphLabel*, const QRect&);
    void si_labelMoved(const QSharedPointer<GSequenceGraphData>&, GraphLabel*, const QRect&);
    void si_frameRangeChanged(const QSharedPointer<GSequenceGraphData>&, const QRect&);
    void si_labelsColorChange(const QSharedPointer<GSequenceGraphData>&);
private slots:
    void sl_onShowVisualProperties(bool);
    void sl_onSelectExtremumPoints();
    void sl_onDeleteAllLabels();
    void sl_onSaveGraphCutoffs(bool);
    void sl_graphRectChanged(const QRect&);

private:
    GSequenceLineView*          baseView;
    QString                     vName;
    QList<QSharedPointer<GSequenceGraphData> >  graphs;
    GSequenceGraphDrawer*       graphDrawer;
    QAction*                    visualPropertiesAction;
    QAction*                    saveGraphCutoffsAction;
    QAction*                    deleteAllLabelsAction;
    QAction*                    selectAllExtremumPoints;
};


class U2VIEW_EXPORT GSequenceGraphViewRA : public GSequenceLineViewRenderArea {
    Q_OBJECT
public:
    GSequenceGraphViewRA(GSequenceGraphView* g);
    ~GSequenceGraphViewRA();
    virtual GSequenceGraphView* getGraphView() const {return static_cast<GSequenceGraphView*>(view);}

    double getCurrentScale() const;

    const QRect& getGraphRect() const { return graphRect;}

protected:
    virtual void drawAll(QPaintDevice* pd);
    virtual void drawHeader(QPainter& p);
    void drawSelection(QPainter& p);
signals:
    void si_graphRectChanged(const QRect&);
private slots:
    void sl_graphDataUpdated();
private:

    QFont *headerFont;
    int headerHeight;
    QRect graphRect;
    GSequenceGraphDrawer* gd;

};

} // namespace

#endif
