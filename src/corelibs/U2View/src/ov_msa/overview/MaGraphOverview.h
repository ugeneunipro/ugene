/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include <U2Core/BackgroundTaskRunner.h>
#include <U2Core/global.h>

#include "MaOverview.h"

namespace U2 {

class MaGraphCalculationTask;

class U2VIEW_EXPORT MaGraphOverviewDisplaySettings {
public:
    enum GraphType {
        Histogram = 0,
        Line = 1,
        Area = 2
    };

    enum OrientationMode {
        FromTopToBottom = 0,
        FromBottomToTop = 1
    };

    QColor color = Qt::gray;
    GraphType type = Area;
    OrientationMode orientation = FromBottomToTop;
};

enum class MaGraphCalculationMethod {
    Strict,  // the most frequent nucleotide
    Gaps,  // percent of gaps
    Clustal,  // 0-30-60-100 groups
    Highlighting  // count only highlighted cells
};

/** State of the Graph overview. */
class U2VIEW_EXPORT MaGraphOverviewState {
public:
    /** MA object version.*/
    int maObjectVersion = -1;

    /** Widget width to render (height is fixed). */
    int width;

    /** Algorithm type. */
    MaGraphCalculationMethod method = MaGraphCalculationMethod::Strict;

    /** Highlighting scheme id. Set only for MaGraphCalculationMethod::Highlighting. */
    QString highlightingSchemeId;

    /** Color scheme id. Set only for MaGraphCalculationMethod::Highlighting. */
    QString colorSchemeId;
};

class U2VIEW_EXPORT MaGraphOverview : public MaOverview {
    Q_OBJECT

public:
    MaGraphOverview(MaEditor *editor, QWidget *ui);
    bool isValid() const {
        return graphCalculationTaskRunner.getError().isEmpty();
    }

    QPixmap getView() override {
        return cachedConsensus;
    }

    const static int FIXED_HEIGHT = 70;

    const MaGraphOverviewDisplaySettings& getDisplaySettings() const;

    const MaGraphOverviewState& getState() const;

    /** Restarts graph computation if the widget is visible and 'state' != 'renderedState'. */
    void recomputeGraphIfNeeded();

signals:
    void si_renderingStateChanged(bool isRendering);

public slots:
    void sl_redraw() override final;
    void sl_highlightingChanged();

    void sl_graphOrientationChanged(const MaGraphOverviewDisplaySettings::OrientationMode& orientation);
    void sl_graphTypeChanged(const MaGraphOverviewDisplaySettings::GraphType& type);
    void sl_graphColorChanged(const QColor& color);
    void sl_calculationMethodChanged(const MaGraphCalculationMethod& method);

protected:
    void paintEvent(QPaintEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;
    void hideEvent(QHideEvent*) override;
    void showEvent(QShowEvent* event) override;

    void drawVisibleRange(QPainter& p) override;
    void drawOverview(QPainter& p) override;
    void moveVisibleRange(QPoint pos) override;

    /** Updates current highlighting schemes in state based on the selected method & MSA Editor state. */
    void updateHighlightingSchemes();

    QPixmap cachedConsensus;

    /** Current pending state. May be not rendered yet. */
    MaGraphOverviewState state;

    /** State used by the background task to compute new results. */
    MaGraphOverviewState inProgressState;

    /** Last successfully rendered state. */
    MaGraphOverviewState renderedState;

    /** Current display settings. Change in display settings does not require graph recalculation. */
    MaGraphOverviewDisplaySettings displaySettings;

    /** Set to 'true' between 'si_startMaChanging' and 'si_stopMaChanging' signals. */
    bool isMaChangeInProgress = false;

    bool redrawGraph = true;

    BackgroundTaskRunner<QPolygonF> graphCalculationTaskRunner;
};

inline bool operator==(const MaGraphOverviewState& s1, const MaGraphOverviewState& s2) {
    return s1.width == s2.width &&
           s1.method == s2.method &&
           s1.maObjectVersion == s2.maObjectVersion &&
           s1.highlightingSchemeId == s2.highlightingSchemeId &&
           s1.colorSchemeId == s2.colorSchemeId;
}

inline bool operator!=(const MaGraphOverviewState& s1, const MaGraphOverviewState& s2) {
    return !(s1 == s2);
}

}  // namespace U2

#endif  // _U2_MSA_GRAPH_OVERVIEW_H_
