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

#include "AssemblyCoverageGraph.h"
#include "AssemblyBrowser.h"

#include <U2Core/U2SafePoints.h>
#include <QPainter>
#include <QMouseEvent>


namespace U2 {

AssemblyCoverageGraph::AssemblyCoverageGraph(AssemblyBrowserUi * ui_) :
QWidget(ui_), ui(ui_), browser(ui_->getWindow()), model(ui_->getModel()), canceled(false) {
    setFixedHeight(FIXED_HEIGHT);
    connectSlots();
    doRedraw();
    setMouseTracking(true);
}

void AssemblyCoverageGraph::connectSlots() {
    connect(browser, SIGNAL(si_zoomOperationPerformed()), SLOT(sl_launchCoverageCalculation()));
    connect(browser, SIGNAL(si_offsetsChanged()), SLOT(sl_onOffsetsChanged()));
    connect(&coverageTaskRunner, SIGNAL(si_finished()), SLOT(sl_coverageReady()));
}

void AssemblyCoverageGraph::drawAll() {
    if(!model->isEmpty()) {
        if(cachedView.size() != size()) {
            cachedView = QPixmap(size() * devicePixelRatio());
            cachedView.setDevicePixelRatio(devicePixelRatio());
            redraw = true;
        }
        if (redraw) {
            cachedView.fill(Qt::transparent);
            QPainter p(&cachedView);

            if(browser->areCellsVisible()) {
                U2Region visibleRegion = browser->getVisibleBasesRegion();
                if(!coverageTaskRunner.isIdle() || canceled) {
                    if(browser->intersectsLocalCoverageCache(visibleRegion)) {
                        CoverageInfo ci = browser->extractFromLocalCoverageCache(visibleRegion);
                        drawGraph(p, ci, 128);
                    }
                    QString message = coverageTaskRunner.isIdle() ? tr("Coverage calculation canceled") : tr("Calculating coverage...");
                    p.drawText(rect(), Qt::AlignCenter, message);
                } else if(lastResult.region == visibleRegion) {
                    drawGraph(p, lastResult);
                } else if(browser->isInLocalCoverageCache(visibleRegion)) {
                    lastResult = browser->extractFromLocalCoverageCache(visibleRegion);
                    drawGraph(p, lastResult);
                } else {
                    sl_launchCoverageCalculation();
                }
            }
        }
        QPainter p(this);
        p.drawPixmap(0, 0, cachedView);
    }
}

void AssemblyCoverageGraph::drawGraph(QPainter & p, const CoverageInfo &ci, int alpha) {
    int cellWidth = browser->getCellWidth();
    int visibleBases = browser->basesVisible();
    const U2AssemblyCoverageStat & coverageInfo = ci.coverageInfo;
    qint32 maxCoverage = ci.maxCoverage;

    SAFE_POINT(visibleBases == coverageInfo.size(), "in AssemblyCoverageGraph::drawGraph: incorrect coverageInfo size",)
    CHECK(maxCoverage > 0,);

    //draw coverage for each visible column
    double readsPerYPixel = double(maxCoverage)/height();
    for(int ibase = 0; ibase < visibleBases; ++ibase) {
        int columnPixels = qint64(double(coverageInfo[ibase]) / readsPerYPixel + 0.5);
        double grayCoeffD = double(coverageInfo[ibase]) / maxCoverage;
        QColor color = ui->getCoverageColor(grayCoeffD);
        color.setAlpha(alpha);
        p.fillRect(ibase*cellWidth, height()-columnPixels, cellWidth, height(), color);
    }
    redraw = false;
}

void AssemblyCoverageGraph::paintEvent(QPaintEvent * e) {
    drawAll();
    QWidget::paintEvent(e);
}

void AssemblyCoverageGraph::mouseMoveEvent(QMouseEvent * e) {
    emit si_mouseMovedToPos(e->pos());
    QWidget::mouseMoveEvent(e);
}

void AssemblyCoverageGraph::doRedraw() {
    redraw = true;
    update();
}

void AssemblyCoverageGraph::sl_launchCoverageCalculation()
{
    if(browser->areCellsVisible()) {
        U2Region visibleRegion = browser->getVisibleBasesRegion();
        previousRegion = visibleRegion;

        if(browser->isInLocalCoverageCache(visibleRegion)) {
            lastResult = browser->extractFromLocalCoverageCache(visibleRegion);
            coverageTaskRunner.cancel();
        } else {
            CalcCoverageInfoTaskSettings settings;
            settings.model = model;
            settings.visibleRange = visibleRegion;
            settings.regions = visibleRegion.length;

            coverageTaskRunner.run(new CalcCoverageInfoTask(settings));
        }
    }
    canceled = false;
    doRedraw();
}

void AssemblyCoverageGraph::sl_coverageReady() {
    if(coverageTaskRunner.isIdle()) {
        if(coverageTaskRunner.isSuccessful()) {
            browser->setLocalCoverageCache(coverageTaskRunner.getResult());
            lastResult = coverageTaskRunner.getResult();
            canceled = false;
        } else {
            canceled = true;
        }
        doRedraw();
    }
}

void AssemblyCoverageGraph::sl_onOffsetsChanged() {
    if(browser->areCellsVisible()) {
        if(previousRegion != browser->getVisibleBasesRegion()) {
            sl_launchCoverageCalculation();
        }
    }
}

} //ns
