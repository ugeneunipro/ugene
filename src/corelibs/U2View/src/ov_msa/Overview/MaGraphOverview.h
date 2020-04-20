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

#ifndef _U2_MSA_GRAPH_OVERVIEW_H_
#define _U2_MSA_GRAPH_OVERVIEW_H_

#include <U2Core/global.h>
#include <U2Core/BackgroundTaskRunner.h>

#include "MaOverview.h"

namespace U2 {

class MaGraphCalculationTask;

class MaGraphOverviewDisplaySettings {
public:
    enum GraphType {
        Hystogram = 0,
        Line = 1,
        Area = 2
    };

    enum OrientationMode {
        FromTopToBottom = 0,
        FromBottomToTop = 1
    };

    MaGraphOverviewDisplaySettings()
        : color( Qt::gray ),
          type(Area),
          orientation(FromBottomToTop) {}

    QColor      color;
    GraphType   type;
    OrientationMode orientation;
};

enum MaGraphCalculationMethod {
    Strict,         // the most frequent nucleotide
    Gaps,           // percent of gaps
    Clustal,        // 0-30-60-100 groups
    Highlighting    // count only highlighted cells
};

#define MSA_GRAPH_OVERVIEW_COLOR_KEY "msa_graph_overview_color"
#define MSA_GRAPH_OVERVIEW_TYPE_KEY "msa_graph_overview_type"
#define MSA_GRAPH_OVERVIEW_ORIENTAION_KEY "msa_graph_overview_orientation_key"

class U2VIEW_EXPORT MaGraphOverview : public MaOverview {
    Q_OBJECT
public:
    MaGraphOverview(MaEditorWgt* ui);
    bool isValid() const { return graphCalculationTaskRunner.getError().isEmpty(); }
    QPixmap getView() { return cachedConsensus; }

    const static int FIXED_HEIGHT = 70;

    void cancelRendering();

    QColor getCurrentColor() const { return displaySettings->color; }
    MaGraphOverviewDisplaySettings::GraphType getCurrentGraphType() const { return displaySettings->type; }
    MaGraphOverviewDisplaySettings::OrientationMode getCurrentOrientationMode() const
    { return displaySettings->orientation; }
    MaGraphCalculationMethod getCurrentCalculationMethod() const { return method; }

signals:
    void si_renderingStateChanged(bool isRendering);

public slots:
    void sl_redraw();
    void sl_drawGraph();
    void sl_highlightingChanged();

    void sl_graphOrientationChanged(MaGraphOverviewDisplaySettings::OrientationMode orientation);
    void sl_graphTypeChanged(MaGraphOverviewDisplaySettings::GraphType type);
    void sl_graphColorChanged(QColor color);
    void sl_calculationMethodChanged(MaGraphCalculationMethod method);

    void sl_startRendering();
    void sl_stopRendering();

    void sl_blockRendering();
    void sl_unblockRendering(bool update);

protected:
    void paintEvent(QPaintEvent* e);
    void resizeEvent(QResizeEvent* e);

private:
    void drawVisibleRange(QPainter& p);
    void drawOverview(QPainter& p);
    void moveVisibleRange(QPoint pos);

    QPixmap             cachedConsensus;

    bool redrawGraph;
    bool isRendering;
    bool isBlocked;
    int lastDrawnVersion;

    BackgroundTaskRunner<QPolygonF>    graphCalculationTaskRunner;

    MaGraphOverviewDisplaySettings*    displaySettings;
    MaGraphCalculationMethod           method;

    MaGraphCalculationTask*            graphCalculationTask;
};

} // namespace

#endif // _U2_MSA_GRAPH_OVERVIEW_H_
