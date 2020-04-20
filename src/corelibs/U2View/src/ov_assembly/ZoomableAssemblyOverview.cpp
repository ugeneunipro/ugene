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


#include "ZoomableAssemblyOverview.h"
#include "AssemblyBrowser.h"
#include "AssemblyReadsArea.h"
#include "AssemblyBrowserSettings.h"

#include <U2Core/Log.h>
#include <U2Core/AppContext.h>

#include <QApplication>
#include <QLabel>
#include <QMenu>
#include <QPainter>
#include <QMouseEvent>

#include <math.h>

namespace U2 {

static const QColor labelForegroundColor = Qt::darkRed;
static const QColor labelBackgroundColor(255, 255, 255, 180);

//==============================================================================
// ZoomableAssemblyOverview
//==============================================================================

const double ZoomableAssemblyOverview::ZOOM_MULT = 2.;

ZoomableAssemblyOverview::ZoomableAssemblyOverview(AssemblyBrowserUi * ui_, bool zoomable_): QWidget(ui_), ui(ui_), browser(ui->getWindow()),
    model(ui_->getModel()), zoomable(zoomable_), zoomFactor(1.), redrawSelection(true), redrawBackground(true), previousCoverageLength(0), selectionScribbling(false), visibleRangeScribbling(false),
scaleType(AssemblyBrowserSettings::getOverviewScaleType()) {
    setObjectName("Zoomable assembly overview");

    U2OpStatusImpl os;
    visibleRange.startPos = 0;
    visibleRange.length = model->getModelLength(os);
    setFixedHeight(FIXED_HEIGHT);
    setMouseTracking(true);
    connectSlots();
    setupActions();
    setContextMenuPolicy(Qt::DefaultContextMenu);
    initSelectionRedraw();
}

void ZoomableAssemblyOverview::setupActions() {
    zoomInAction = new QAction(tr("Zoom in"), this);
    zoomOutAction = new QAction(tr("Zoom out"), this);
    zoomIn100xActon = new QAction(tr("Zoom in 100x"), this);
    restoreGlobalOverviewAction = new QAction(tr("Restore global overview"), this);
    QAction *exportCoverageAction = new QAction(tr("Export coverage..."), this);
    exportCoverageAction->setObjectName("Export coverage");

    connect(zoomInAction, SIGNAL(triggered()), SLOT(sl_zoomInContextMenu()));
    connect(zoomOutAction, SIGNAL(triggered()), SLOT(sl_zoomOutContextMenu()));
    connect(zoomIn100xActon, SIGNAL(triggered()), SLOT(sl_zoom100xContextMenu()));
    connect(restoreGlobalOverviewAction, SIGNAL(triggered()), SLOT(sl_restoreGlobalOverview()));
    connect(exportCoverageAction, SIGNAL(triggered()), browser, SLOT(sl_exportCoverage()));

    contextMenu = new QMenu(this);

    contextMenu->addAction(zoomInAction);
    contextMenu->addAction(zoomOutAction);
    contextMenu->addAction(zoomIn100xActon);
    contextMenu->addAction(restoreGlobalOverviewAction);
    contextMenu->addAction(exportCoverageAction);
    updateActions();
}

void ZoomableAssemblyOverview::updateActions() {
    bool showingGlobalRegion = model->getGlobalRegion() == visibleRange;
    zoomOutAction->setDisabled(showingGlobalRegion);
    restoreGlobalOverviewAction->setDisabled(showingGlobalRegion);

    bool cantZoomIn = visibleRange.length == minimalOverviewedLen();
    zoomIn100xActon->setDisabled(cantZoomIn);
    zoomInAction->setDisabled(cantZoomIn);
}

void ZoomableAssemblyOverview::connectSlots() {
    connect(&coverageTaskRunner, SIGNAL(si_finished()), SLOT(sl_redraw()));
    connect(&coverageTaskRunner, SIGNAL(si_finished()), SIGNAL(si_coverageReady()));
    connect(browser, SIGNAL(si_zoomOperationPerformed()), SLOT(sl_visibleAreaChanged()));
    connect(browser, SIGNAL(si_offsetsChanged()), SLOT(sl_redraw()));
}

void ZoomableAssemblyOverview::initSelectionRedraw() {
    redrawSelection = true;
    cachedView = QPixmap(size() * devicePixelRatio());
    cachedView.setDevicePixelRatio(devicePixelRatio());
}

void ZoomableAssemblyOverview::drawAll() {
    if(!model->isEmpty()) {
        //no coverage -> draw nothing
        if(!coverageTaskRunner.isIdle()) {
            cachedBackground = QPixmap(size() * devicePixelRatio());
            cachedBackground.setDevicePixelRatio(devicePixelRatio());
            QPainter p(&cachedBackground);
            p.fillRect(rect(), Qt::gray);
            p.drawText(rect(), Qt::AlignCenter, tr("Background is rendering..."));
        }
        //coverage is ready -> redraw background if needed
        else if(redrawBackground) {
            cachedBackground = QPixmap(size() * devicePixelRatio());
            cachedBackground.setDevicePixelRatio(devicePixelRatio());
            QPainter p(&cachedBackground);
            drawBackground(p);
            redrawBackground = false;
        }
        //draw selection and labels
        if (redrawSelection) {
            cachedView = cachedBackground;
            QPainter p(&cachedView);
            drawSelection(p);
            drawCoordLabels(p);
            redrawSelection = false;
        }
        QPixmap cachedViewCopy(cachedView);
        if(zoomToRegionSelector.scribbling) {
            QPainter p(&cachedViewCopy);
            drawZoomToRegion(p);
        }
        QPainter p(this);
        p.drawPixmap(0, 0, cachedViewCopy);
    }
}

void ZoomableAssemblyOverview::drawZoomToRegion(QPainter & p) {
    if(!zoomToRegionSelector.scribbling) {
        assert(false);
        return;
    }
    QPoint topLeft;
    QPoint bottomRight;
    int curX = mapFromGlobal(QCursor::pos()).x();
    if(zoomToRegionSelector.startPos.x() <= curX) {
        topLeft = QPoint(zoomToRegionSelector.startPos.x(), 0);
        bottomRight = QPoint(curX, height());
    } else {
        topLeft = QPoint(curX, 0);
        bottomRight = QPoint(zoomToRegionSelector.startPos.x(), height());
    }
    p.fillRect(QRect(topLeft, bottomRight), QColor(128, 0, 0, 100));
}

void ZoomableAssemblyOverview::drawBackground(QPainter & p) {
    CoverageInfo ci = coverageTaskRunner.getResult();
    if(ci.region == model->getGlobalRegion()) {
        browser->setGlobalCoverageInfo(ci);
    }

    const int widgetHeight = height();
    const int widgetWidth = width();

    //1. calc reads per pixel
    static double logMax = .0;
    double readsPerYPixel = .0;
    switch(scaleType) {
    case AssemblyBrowserSettings::Scale_Linear:
        readsPerYPixel = double(ci.maxCoverage) / widgetHeight;
        break;
    case AssemblyBrowserSettings::Scale_Logarithmic:
        logMax = log((double)ci.maxCoverage);
        readsPerYPixel = double(logMax) / widgetHeight;
        break;
    default:
        assert(false);
    }

    p.fillRect(rect(), Qt::white);

    //2. draw the graph line-by-line
    for(int i = 0 ; i < widgetWidth; ++i) {
        quint64 columnPixels = 0;
        double grayCoeffD = 0.;
        if (!ci.coverageInfo.isEmpty()) {
            switch(scaleType) {
            case AssemblyBrowserSettings::Scale_Linear:
                if(ci.maxCoverage != 0){
                    grayCoeffD = double(ci.coverageInfo[i]) / ci.maxCoverage;
                }
                columnPixels = qint64(double(ci.coverageInfo[i]) / readsPerYPixel + 0.5);
                //grayCoeff = 255 - int(double(255) / ci.maxCoverage * ci.coverageInfo[i] + 0.5);
                break;
            case AssemblyBrowserSettings::Scale_Logarithmic:
                grayCoeffD = log((double)ci.coverageInfo[i]) / logMax;
                columnPixels = qint64(double(log((double)ci.coverageInfo[i])) / readsPerYPixel + 0.5);
                //grayCoeff = 255 - int(double(255) / logMax * log((double)ci.coverageInfo[i]) + 0.5);
                break;
            }
        }

        //UGENE-style colors
        p.setPen(ui->getCoverageColor(grayCoeffD));
        p.drawLine(i, 0, i, columnPixels);
    }

    //3. draw gray border
    p.setPen(Qt::gray);
    p.drawRect(rect().adjusted(0,0,-1,-1));

}

static inline bool isRectVerySmall(const QRect & r) {
    return r.width() <=2 && r.height() <= 15;
}

static const int CROSS_HALF_SIZE = 3;
static const QPoint CROSS_LEFT_CORNER(CROSS_HALF_SIZE, 0);
static const QPoint CROSS_RIGHT_CORNER(0, CROSS_HALF_SIZE);

void ZoomableAssemblyOverview::drawSelection(QPainter & p) {
    cachedSelection = calcCurrentSelection();
    //if selection is TOO small - enlarge it a bit
    if(0 == cachedSelection.width())  {
        cachedSelection.setWidth(1);
    }
    if(0 == cachedSelection.height()) {
        cachedSelection.setHeight(1);
    }

    //cached selection can be outside the visible range, so intersect it with it
    QRect selectionToDraw = rect().intersected(cachedSelection);
    if(selectionToDraw.isNull()) {
        return;
    }
    if(isRectVerySmall(selectionToDraw)) {
        //draw red cross
        QPoint c = selectionToDraw.center();
        QPen oldPen = p.pen();
        p.setPen(Qt::red);
        p.drawLine(c - CROSS_LEFT_CORNER, c + CROSS_LEFT_CORNER);
        p.drawLine(c - CROSS_RIGHT_CORNER, c + CROSS_RIGHT_CORNER);
        p.setPen(oldPen);
    } else {
        //draw transparent rectangle
        if(selectionToDraw.width() < 5 || selectionToDraw.height() < 5) {
            //red borders if rect is thin
            p.setPen(Qt::red);
        }
        p.fillRect(selectionToDraw, QColor(230, 230, 230, 180));
        p.drawRect(selectionToDraw.adjusted(0, 0, -1, -1));
    }
}

static void insertSpaceSeparators(QString & str) {
    for(int i = str.length()-3; i > 0; i-=3) {
        str.insert(i, " ");
    }
}

void ZoomableAssemblyOverview::drawCoordLabels(QPainter & p) {
    const static int xoffset = 4;
    const static int yoffset = 3;

    U2OpStatusImpl status;

    //Prepare text
    QString visibleStartText = QString::number(visibleRange.startPos);
    QString visibleEndText = QString::number(visibleRange.endPos());
    QString visibleDiffText = QString::number(visibleRange.length);
    insertSpaceSeparators(visibleStartText);
    insertSpaceSeparators(visibleEndText);
    insertSpaceSeparators(visibleDiffText);

    //Prepare font
    QFont font = p.font();
#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
        font.setStyleHint(QFont::SansSerif, (QFont::StyleStrategy)(QFont::PreferAntialias | QFont::ForceIntegerMetrics));
#else
        font.setStyleHint(QFont::SansSerif, QFont::PreferAntialias);
#endif
    p.setFont(font);
    QFontMetrics fontMetrics(font, this);

    p.setPen(labelForegroundColor);

    //draw Visible Region
    QString visibleRegionText = tr("%1 to %2 (%3 bp)").arg(visibleStartText).arg(visibleEndText).arg(visibleDiffText);
    QRect grtRect = QRect(0, 0, fontMetrics.width(visibleRegionText), fontMetrics.height());
    grtRect.translate(xoffset, rect().height() - yoffset - grtRect.height());
    if(rect().contains(grtRect)) {
        p.fillRect(grtRect, labelBackgroundColor);
        p.drawText(grtRect, visibleRegionText);
    }

    //draw Selected Region
    qint64 from = browser->getXOffsetInAssembly();
    qint64 to = qMin(browser->getXOffsetInAssembly() + browser->basesCanBeVisible(), model->getModelLength(status));

    //prepare text
    QString fromText = QString::number(from + 1); // because of 1-based coords
    QString toText = QString::number(to);
    QString diff = QString::number(to - from);

    insertSpaceSeparators(fromText);
    insertSpaceSeparators(toText);
    insertSpaceSeparators(diff);

    //draw text
    QString selectedRegionText = tr("%1 to %2 (%3 bp)").arg(fromText, toText, diff);
    QRect srtRect = QRect(0, 0, fontMetrics.width(selectedRegionText), fontMetrics.height());
    srtRect.translate(rect().width() - srtRect.width() - xoffset, rect().height() - yoffset - grtRect.height());
    if(rect().contains(srtRect) && !srtRect.intersects(grtRect)) {
        p.fillRect(srtRect, labelBackgroundColor);
        p.drawText(srtRect, selectedRegionText);
    }
}

qint64 ZoomableAssemblyOverview::calcXAssemblyCoord(int x) const {
    U2OpStatusImpl status;
    qint64 result = double(visibleRange.length) / width()  * x + 0.5 + visibleRange.startPos;
    return result;
}

qint64 ZoomableAssemblyOverview::calcYAssemblyCoord(int y) const {
    U2OpStatusImpl status;
    qint64 result = double(model->getModelHeight(status)) / height() * y + 0.5;
    return result;
}

QRect ZoomableAssemblyOverview::calcCurrentSelection() const {
    U2OpStatusImpl status;
    int w = rect().width();
    int h = rect().height();

//    qint64 x_ass_start = qMax(0, browser->getXOffsetInAssembly()-visibleRange.startPos);

    int x_pix_start = (double(w) / visibleRange.length * (browser->getXOffsetInAssembly()-visibleRange.startPos)) + 0.5;
    int y_pix_start = double(h) / model->getModelHeight(status) * browser->getYOffsetInAssembly() + 0.5;
    int pix_width = (double(w) / visibleRange.length * browser->basesVisible()) + 0.5;
    int pix_height = double(h) / model->getModelHeight(status) * browser->rowsVisible() + 0.5;

    return QRect(x_pix_start, y_pix_start, pix_width, pix_height);
}

//prevents selection from crossing widget borders.
//Tries to move selection center to pos.
void ZoomableAssemblyOverview::moveSelectionToPos( QPoint pos, bool moveModel )
{
    const QRect & thisRect = rect();
    QRect newSelection(cachedSelection);
    newSelection.moveCenter(pos);

    int dy = 0;
    int dx = 0;

    if(!thisRect.contains(newSelection)) {
        QRect uneeon = thisRect.united(newSelection);
        if(uneeon.bottom() > thisRect.height()) {
            dy = uneeon.bottom() - thisRect.height();
        } else if(uneeon.top() < 0) {
            dy = uneeon.top();
        }
//         if(uneeon.right() > thisRect.right()) {
//             dx = uneeon.right() - thisRect.right();
//         } else if(uneeon.left() < 0) {
//             dx = uneeon.left();
//         }
        newSelection.translate(-dx, -dy);
    }

    U2OpStatusImpl status;
    qint64 newXoffset = 0;
    qint64 newYoffset = 0;
    if(dx) {
        newXoffset = (dx < 0) ? 0 : model->getModelLength(status) - browser->basesVisible();
        moveModel = true;
    } else {
        newXoffset = calcXAssemblyCoord(newSelection.x());
    }
    if(dy) {
        newYoffset = (dy < 0) ? 0 : model->getModelHeight(status) - browser->rowsVisible();
        moveModel = true;
    } else {
        newYoffset = calcYAssemblyCoord(newSelection.y());
    }

    if(moveModel) {
        browser->setOffsetsInAssembly(browser->normalizeXoffset(newXoffset), browser->normalizeYoffset(newYoffset));
    }
}

void ZoomableAssemblyOverview::zoomToPixRange(int x_pix_start, int x_pix_end) {
    assert(x_pix_start <= x_pix_end);
    qint64 left_asm = calcXAssemblyCoord(x_pix_start);
    qint64 right_asm = calcXAssemblyCoord(x_pix_end);
    checkedSetVisibleRange(left_asm, right_asm-left_asm);
    sl_redraw();
}

void ZoomableAssemblyOverview::checkedMoveVisibleRange(qint64 newStartPos) {
    if(!zoomable) return;
    U2OpStatusImpl os;
    qint64 realNewStartPos = qBound((qint64)0, newStartPos, model->getModelLength(os)-visibleRange.length);
    visibleRange.startPos = realNewStartPos;
    launchCoverageCalculation();
}

qint64 ZoomableAssemblyOverview::minimalOverviewedLen() const {
    U2OpStatusImpl os;
    qint64 modelLen = model->getModelLength(os);
    LOG_OP(os);
    // 1 letter == 1 pixel. We are not going to draw more than one pixel per letter
    // Allow more pixels per letter only if model is shorter than screen
    return qMin(modelLen, (qint64)width());
}

bool ZoomableAssemblyOverview::canZoomToRange(const U2Region & range)const {
    return minimalOverviewedLen() != range.length;
}

void ZoomableAssemblyOverview::checkedSetVisibleRange(qint64 newStartPos, qint64 newLen, bool force) {
    if(!zoomable) return;
    U2OpStatusImpl os;
    qint64 modelLen = model->getModelLength(os);
    Q_UNUSED(modelLen);
    assert(newLen <= modelLen);
    if(newLen != visibleRange.length || newStartPos != visibleRange.startPos || force) {
        qint64 minLen = minimalOverviewedLen();
        qint64 minLenDiff = 0;
        if(newLen < minLen) {
            minLenDiff = minLen - newLen;
        }
        //shift start pos to keep needed region in the center, if
        //requested length is smaller than minimal allowed length
        newStartPos -= minLenDiff / 2;

        newStartPos = qMax((qint64)0, newStartPos);
        visibleRange.length = qMax(newLen, minLen);
        checkedMoveVisibleRange(newStartPos);

        emit si_visibleRangeChanged(visibleRange);
    }
}

void ZoomableAssemblyOverview::checkedSetVisibleRange(const U2Region & newRegion, bool force) {
    checkedSetVisibleRange(newRegion.startPos, newRegion.length, force);
}

void ZoomableAssemblyOverview::paintEvent(QPaintEvent * e) {
    drawAll();
    QWidget::paintEvent(e);
}

void ZoomableAssemblyOverview::resizeEvent(QResizeEvent * e) {
    cachedSelection = calcCurrentSelection();
    moveSelectionToPos(cachedSelection.center(), false);
    // force re-check visible range to avoid violating 1-pixel limit
    checkedSetVisibleRange(visibleRange, true);
    launchCoverageCalculation();
    sl_redraw();
    QWidget::resizeEvent(e);
}

void ZoomableAssemblyOverview::mousePressEvent(QMouseEvent * me) {
    //background scribbling
    if(me->button() == Qt::MidButton) {
        visibleRangeScribbling = true;
        visibleRangeLastPos = me->pos();
        setCursor(Qt::ClosedHandCursor);
    }

    if (me->button() == Qt::LeftButton) {
        //zoom in
        if(me->modifiers() & Qt::AltModifier) {
            QPoint pos = me->pos();
            int left_pix = qMax(0, pos.x() - 2);
            int right_pix = qMin(width(), pos.x() + 2);
            zoomToPixRange(left_pix, right_pix);
        }
        // select region
        else if(me->modifiers() & Qt::ShiftModifier) {
            zoomToRegionSelector.scribbling = true;
            zoomToRegionSelector.startPos = me->pos();
        }
        //selection scribbling
        else {
            selectionScribbling = true;
            if(!cachedSelection.contains(me->pos())) {
                selectionDiff = QPoint();
                moveSelectionToPos(me->pos());
            } else {
                selectionDiff = me->pos() - cachedSelection.center();
            }
        }
    }

    QWidget::mousePressEvent(me);
}

void ZoomableAssemblyOverview::mouseMoveEvent(QMouseEvent * me) {
    //selection scribbling
    if((me->buttons() & Qt::LeftButton) && selectionScribbling) {
        if(!ui->getReadsArea()->isScrolling()) {
            ui->getReadsArea()->setScrolling(true);
        }
        moveSelectionToPos(me->pos() - selectionDiff);
    }
    //background scribbling (Ctrl-Click)
    else if((me->buttons() & Qt::MidButton) && visibleRangeScribbling){
        int pixelDiff = visibleRangeLastPos.x() - me->pos().x();
        qint64 asmDiff = calcXAssemblyCoord(pixelDiff);
        checkedMoveVisibleRange(asmDiff);
        visibleRangeLastPos = me->pos();
    }

    if(zoomToRegionSelector.scribbling) {
        sl_redraw();
    }

    QWidget::mouseMoveEvent(me);
}

void ZoomableAssemblyOverview::mouseReleaseEvent(QMouseEvent * me) {
    if(me->button() == Qt::LeftButton) {
        if(selectionScribbling) {
            selectionScribbling = false;
            if(ui->getReadsArea()->isScrolling()) {
                ui->getReadsArea()->setScrolling(false);
            }
        }
        if(zoomToRegionSelector.scribbling) {
            zoomToRegionSelector.scribbling = false;
            QPoint curPoint = me->pos();
            int regionStartPix = qMin(zoomToRegionSelector.startPos.x(), curPoint.x());
            int regionEndPix = qMax(zoomToRegionSelector.startPos.x(), curPoint.x());
            if(regionEndPix != regionStartPix) {
                zoomToPixRange(qMax(0, regionStartPix), qMin(width(), regionEndPix));
                update();
            }
        }
        return;
    }
    if((me->button() == Qt::MidButton) && visibleRangeScribbling){
        visibleRangeScribbling = false;
        setCursor(Qt::ArrowCursor);
    }
    QWidget::mouseReleaseEvent(me);
}

void ZoomableAssemblyOverview::wheelEvent(QWheelEvent * e) {
    bool positive = e->delta() > 0;
    int numDegrees = abs(e->delta()) / 8;
    int numSteps = numDegrees / 15;

    // zoom
    if(Qt::NoButton == e->buttons()) {
        if(e->modifiers() & Qt::ControlModifier) {
            // with Ctrl: zoom overview
            for(int i = 0; i < numSteps; ++i) {
                if(positive) {
                    sl_zoomIn(e->pos());
                } else {
                    sl_zoomOut(e->pos());
                }
            }
        } else {
            // without Ctrl: zoom reads area
            QApplication::sendEvent(ui->getReadsArea(), e);
        }
    }
    QWidget::wheelEvent(e);
}

void ZoomableAssemblyOverview::contextMenuEvent(QContextMenuEvent * e) {
    updateActions();
    contextMenu->move(e->globalPos());
    contextMenu->show();
    contextMenuPos = e->pos();

    QWidget::contextMenuEvent(e);
}

void ZoomableAssemblyOverview::sl_visibleAreaChanged() {
    cachedSelection = calcCurrentSelection();
    moveSelectionToPos(cachedSelection.center(), false);
    sl_redraw();
}

void ZoomableAssemblyOverview::sl_redraw() {
    initSelectionRedraw();
    update();
}

void ZoomableAssemblyOverview::sl_zoomIn(const QPoint & pos) {
    if(!zoomable) return;

    //1. count new length of the visible region
    qint64 newLen = visibleRange.length / ZOOM_MULT + 0.5;
//     if(newLen < browser->basesCanBeVisible()) {
//         newLen = browser->basesCanBeVisible();
//     }

    qint64 oldLen = visibleRange.length;
    qint64 minLen = minimalOverviewedLen();
    if(newLen < minLen) {
        newLen = minLen;
    }

    if(oldLen == newLen) {
        return;
    }

    //2. count new start position of the visible region
    qint64 newStart = 0;
    if(!pos.isNull()) {
        //Black magic. Zooms overview so that region under mouse pointer remains
        //under pointer after zoom
        newStart = calcXAssemblyCoord(pos.x()) - double(newLen)/width()*pos.x();
    } else {
        //simply zoom to center
        newStart = visibleRange.startPos + (visibleRange.length-newLen)/2;
    }

    //3. if nothing changed -> do nothing
    if(newLen == visibleRange.length && newStart == visibleRange.startPos) {
        return;
    }

    //4. set new values and update widget
    checkedSetVisibleRange(newStart,newLen);
    sl_redraw();
}

void ZoomableAssemblyOverview::sl_zoomInContextMenu() {
    sl_zoomIn(contextMenuPos);
}

void ZoomableAssemblyOverview::sl_zoomOutContextMenu() {
    sl_zoomOut(contextMenuPos);
}

void ZoomableAssemblyOverview::sl_zoom100xContextMenu() {
    QPoint pos = contextMenuPos;
    int left_pix = qMax(0, pos.x() - 2);
    int right_pix = qMin(width(), pos.x() + 2);
    zoomToPixRange(left_pix, right_pix);
}

void ZoomableAssemblyOverview::sl_restoreGlobalOverview() {
    checkedSetVisibleRange(model->getGlobalRegion());
    sl_redraw();
}

void ZoomableAssemblyOverview::sl_zoomOut(const QPoint & pos) {
    if(!zoomable) return;

    // 1. count new length of the visible region
    qint64 newLen = visibleRange.length * ZOOM_MULT + 0.5;
    U2OpStatusImpl os;
    if(newLen > model->getModelLength(os)) {
        newLen = model->getModelLength(os);
    }

    // 2. count new start of the visible region
    qint64 newStart = 0;
    if(!pos.isNull()) {
        //Black magic. Zooms overview so that region under mouse pointer remains
        //under pointer after zoom
        newStart = calcXAssemblyCoord(pos.x()) - double(newLen)/width()*pos.x();
    } else {
        //simply zoom to center
        newStart = visibleRange.startPos + (visibleRange.length-newLen)/2;
    }

    //3. if nothing changed -> do nothing
    if(newLen == visibleRange.length && newStart == visibleRange.startPos) {
        return;
    }

    //4. set new values and update widget
    checkedSetVisibleRange(newStart, newLen);
    sl_redraw();
}

void ZoomableAssemblyOverview::setScaleType(AssemblyBrowserSettings::OverviewScaleType t) {
    AssemblyBrowserSettings::setOverviewScaleType(t);
    if(scaleType != t) {
        scaleType = t;
        launchCoverageCalculation();
    }
}

AssemblyBrowserSettings::OverviewScaleType ZoomableAssemblyOverview::getScaleType() const {
    return scaleType;
}

void ZoomableAssemblyOverview::launchCoverageCalculation() {
    if(previousCoverageRegion != visibleRange || width() != previousCoverageLength) {
        CalcCoverageInfoTaskSettings settings;
        settings.model = model;
        settings.visibleRange = previousCoverageRegion = visibleRange;
        settings.regions = previousCoverageLength =  width();

        coverageTaskRunner.run(new CalcCoverageInfoTask(settings));
    }
    redrawBackground = true;
    sl_redraw();
}

} //ns
