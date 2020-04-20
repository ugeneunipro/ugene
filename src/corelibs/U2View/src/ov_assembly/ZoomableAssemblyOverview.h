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

#ifndef __ZOOMABLE_ASSEMBLY_OVERVIEW_H__
#define __ZOOMABLE_ASSEMBLY_OVERVIEW_H__

#include "AssemblyBrowserSettings.h"
#include "CoverageInfo.h"

#include <U2Core/U2Region.h>

#include <QSharedPointer>
#include <QWidget>

class QMenu;

namespace U2 {

class AssemblyModel;
class AssemblyBrowserUi;
class AssemblyBrowser;


class ZoomableAssemblyOverview: public QWidget {
    Q_OBJECT
public:
    ZoomableAssemblyOverview(AssemblyBrowserUi * ui, bool zoomable = false);

    void setScaleType(AssemblyBrowserSettings::OverviewScaleType t);
    AssemblyBrowserSettings::OverviewScaleType getScaleType()const;

    void checkedSetVisibleRange(qint64 newStartPos, qint64 newLen, bool force = false);
    void checkedSetVisibleRange(const U2Region & newRegion, bool force = false);
    inline U2Region getVisibleRange() const {return visibleRange;}

signals:
    void si_visibleRangeChanged(const U2Region &);
    void si_coverageReady();

protected:
    void paintEvent(QPaintEvent * e);
    void resizeEvent(QResizeEvent * e);
    void mousePressEvent(QMouseEvent * me);
    void mouseMoveEvent(QMouseEvent * me);
    void mouseReleaseEvent(QMouseEvent * me);
    void wheelEvent(QWheelEvent * e);
    void contextMenuEvent(QContextMenuEvent * e);

private slots:
    void sl_visibleAreaChanged();
    void sl_redraw();
    void sl_zoomIn(const QPoint & pos);
    void sl_zoomOut(const QPoint & pos);

    void sl_zoomInContextMenu();
    void sl_zoomOutContextMenu();
    void sl_zoom100xContextMenu();
    void sl_restoreGlobalOverview();

private:
    qint64 calcXAssemblyCoord(int x) const;
    qint64 calcYAssemblyCoord(int y) const;

    QRect calcCurrentSelection() const;
    U2Region calcVisibleAssemblyRange() const;
    void moveSelectionToPos(QPoint pos, bool moveModel = true);

    void zoomToPixRange(int x_pix_start, int x_pix_end);
    void checkedMoveVisibleRange(qint64 newStartPos);
    qint64 minimalOverviewedLen() const;
    bool canZoomToRange(const U2Region & range) const;

    void setupActions();
    void updateActions();
    void connectSlots();
    void initSelectionRedraw();

    void drawAll();
    void drawBackground(QPainter & p);
    void drawSelection(QPainter & p);
    void drawCoordLabels(QPainter & p);
    void drawZoomToRegion(QPainter & p);

    void launchCoverageCalculation();

private:
    AssemblyBrowserUi * ui;
    AssemblyBrowser * browser;
    QSharedPointer<AssemblyModel> model;

    QMenu * contextMenu;
    QPoint contextMenuPos;

    //context menu actions
    QAction * zoomInAction;
    QAction * zoomOutAction;
    QAction * zoomIn100xActon;
    QAction * restoreGlobalOverviewAction;

    bool zoomable;
    U2Region visibleRange;
    double zoomFactor;

    QRect cachedSelection;

    QPixmap cachedView;
    bool redrawSelection;

    QPixmap cachedBackground;
    bool redrawBackground;
    U2Region previousCoverageRegion;
    int previousCoverageLength;
    BackgroundTaskRunner<CoverageInfo> coverageTaskRunner;

    bool selectionScribbling;
    QPoint selectionDiff;
    bool visibleRangeScribbling;
    QPoint visibleRangeLastPos;

    AssemblyBrowserSettings::OverviewScaleType scaleType;

    struct ZoomToRegionSelector {
        ZoomToRegionSelector() : scribbling(false) {}
        bool scribbling;
        QPoint startPos;
    } zoomToRegionSelector;

    const static int FIXED_HEIGHT = 70;
    const static double ZOOM_MULT;
};

} //ns

#endif
