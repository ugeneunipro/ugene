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

#ifndef _U2_GSEQUENCE_GRAPH_VIEW_H_
#define _U2_GSEQUENCE_GRAPH_VIEW_H_

#include <QMenu>

#include "ADVGraphModel.h"
#include "GSequenceLineView.h"

namespace U2 {

class GSequenceGraphViewRA;

//////////////////////////////////////////////////////////////////////////
/// View
class U2VIEW_EXPORT GSequenceGraphView : public GSequenceLineView {
    Q_OBJECT
public:
    GSequenceGraphView(QWidget* p, SequenceObjectContext* ctx, GSequenceLineView* baseView, const QString& graphViewName);

    const QString& getGraphViewName() const;

    void getSavedLabelsState(QList<QVariant>& savedLabels);

    void setLabelsFromSavedState(const QList<QVariant>& savedLabels);

    void addGraph(const QSharedPointer<GSequenceGraphData>& graph);

    const QList<QSharedPointer<GSequenceGraphData>>& getGraphs() const;

    void setGraphDrawer(GSequenceGraphDrawer* gd);

    GSequenceGraphDrawer* getGraphDrawer() const;

    void buildPopupMenu(QMenu& m) override;

protected:
    void pack() override;

    void leaveEvent(QEvent* le) override;
    void mousePressEvent(QMouseEvent* me) override;
    void mouseMoveEvent(QMouseEvent* me) override;

    void addActionsToGraphMenu(QMenu* graphMenu);
    GSequenceGraphViewRA* getGraphRenderArea() const;
    void updateMovingLabels();

private slots:
    void sl_onShowVisualProperties(bool);
    void sl_showLocalMinMaxLabels();
    void sl_onDeleteAllLabels();
    void sl_onSaveGraphCutoffs(bool);

private:
    /** Sequence view this graph view is in sync with: usually a related PanView. */
    GSequenceLineView* baseView;

    /** Name (title) of the graph widget. */
    QString graphViewName;

    /** List of visible graph series. */
    QList<QSharedPointer<GSequenceGraphData>> graphs;

    /** Class responsible to compute and draw graphs & graph labels. */
    GSequenceGraphDrawer* graphDrawer;

    /** Opens graph properties dialog. */
    QAction* visualPropertiesAction;

    /** Opens dialog to export cutoff regions as annotations. */
    QAction* saveGraphCutoffsAction;

    /** Removes all labels from all graphs. */
    QAction* deleteAllLabelsAction;

    /** Shows all min/max labels for the current graph state. */
    QAction* showLocalMinMaxLabelsAction;
};

class U2VIEW_EXPORT GSequenceGraphViewRA : public GSequenceLineViewRenderArea {
    Q_OBJECT
public:
    GSequenceGraphViewRA(GSequenceGraphView* view);

    double getCurrentScale() const override;

    /** Returns coordinates of the area to draw a line chart inside of the render area. */
    const QRect& getGraphRect() const;

protected:
    void drawAll(QPaintDevice* pd) override;

    void drawHeader(QPainter& p);

    void drawSelection(QPainter& p);

    GSequenceGraphView* getGraphView() const;

private:
    const int headerHeight = 20;
    QFont headerFont;
    QRect graphRect;
};

}  // namespace U2

#endif
