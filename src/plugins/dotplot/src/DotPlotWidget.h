/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#pragma once

#include <QMenu>
#include <QTimer>
#include <QToolButton>

#include <U2Core/U2Region.h>

#include <U2View/ADVSplitWidget.h>
#include <U2View/PanView.h>

namespace U2 {

class Task;
class ADVSequenceObjectContext;
class ADVSequenceWidget;
class GObjectViewController;
class LRegionsSelection;

class DotPlotImageExportSettings;
class DotPlotResultsListener;
class DotPlotRevComplResultsListener;
class DotPlotMiniMap;
struct DotPlotResults;
class GSequenceLineView;

class DotPlotWidget : public ADVSplitWidget {
    Q_OBJECT

    friend class DotPlotImageExportToBitmapTask;
    friend class DotPlotImageExportController;

public:
    DotPlotWidget(AnnotatedDNAView* dnaView);
    ~DotPlotWidget();

    bool acceptsGObject(GObject*) override {
        return false;
    }
    void updateState(const QVariantMap&) override {
    }
    void saveState(QVariantMap&) override {
    }

    void buildPopupMenu(QMenu*) const;

    AnnotatedDNAView* getDnaView() const {
        return dnaView;
    }

    void setShiftZoom(ADVSequenceObjectContext*, ADVSequenceObjectContext*, float, float, const QPointF&);
    bool hasSelection() const;
    bool hasSelectedArea() const;

    void setIgnorePanView(bool);
    void setKeepAspectRatio(bool);
    void zoomIn();
    void zoomOut();
    void zoomReset();

    bool canZoomOut();
    bool canZoomIn();

    void setSelActive(bool state);

    QString getXSequenceName();
    QString getYSequenceName();

    void setSequences(U2SequenceObject* seqX, U2SequenceObject* seqY);
    bool isShowDeleteDialogOnDotPlotDestroying() const;

    bool onCloseEvent() override;

signals:
    void si_removeDotPlot();
    void si_dotPlotChanged(ADVSequenceObjectContext*, ADVSequenceObjectContext*, float, float, QPointF);
    void si_dotPlotSelecting();

public slots:
    bool sl_showSettingsDialog(bool disableLoad = false);
    void sl_filter();
    void sl_showDeleteDialog(bool isCancelable);

private slots:
    void sl_taskStateChanged();
    void sl_filteringTaskStateChanged();
    void sl_buildDotplotTaskStateChanged();
    void sl_showSaveImageDialog();
    bool sl_showSaveFileDialog();
    bool sl_showLoadFileDialog();

    void sl_onSequenceSelectionChanged(LRegionsSelection*, const QVector<U2Region>&, const QVector<U2Region>&);

    void sl_sequenceWidgetRemoved(ADVSequenceWidget*);
    void sl_panViewChanged();
    void sl_timer();

protected:
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent*) override;

    void wheelEvent(QWheelEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void focusInEvent(QFocusEvent* fe) override;
    void focusOutEvent(QFocusEvent* fe) override;
    bool event(QEvent* event) override;

private:
    AnnotatedDNAView* dnaView;

    QCursor cursor;

    bool selecting, shifting, miniMapLooking, selActive, nearestSelecting;
    LRegionsSelection *selectionX, *selectionY;
    ADVSequenceObjectContext *sequenceX, *sequenceY;
    bool direct, inverted, nearestInverted, ignorePanView, keepAspectRatio;

    QPointF zoom;
    float shiftX, shiftY;
    int minLen, identity;

    bool pixMapUpdateNeeded, deleteDotPlotFlag, filtration;
    bool dotPlotIsCalculating;

    Task* dotPlotTask;
    QPixmap* pixMap;
    DotPlotMiniMap* miniMap;

    const DotPlotResults* nearestRepeat;

    QTimer* timer;
    QToolButton* exitButton;

    QPointF clickedFirst, clickedSecond;

    DotPlotResultsListener* dpDirectResultListener;
    DotPlotRevComplResultsListener* dpRevComplResultsListener;
    QSharedPointer<QList<DotPlotResults>> dpFilteredResults;
    QSharedPointer<QList<DotPlotResults>> dpFilteredResultsRevCompl;

    QAction* showSettingsDialogAction;
    QAction* saveImageAction;
    QAction* saveDotPlotAction;
    QAction* loadDotPlotAction;
    QAction* deleteDotPlotAction;
    QAction* filterDotPlotAction;
    
    bool showDeleteDialogOnDotPlotDestroying = true;

    int textSpace;
    static const int rulerNotchSize = 2;

    int w;
    int h;

    QColor dotPlotBGColor;
    QColor dotPlotDirectColor;
    QColor dotPlotInvertedColor;
    QColor dotPlotNearestRepeatColor;

    bool clearedByRepitSel;
    QByteArray seqXCache, seqYCache;  // cached sequence, used only during DP computation
    QString seqXCachedName;
    QString seqYCachedName;

    void pixMapUpdate();

    void initActionsAndSignals();
    void connectSequenceSelectionSignals();

    void drawAll(QPainter& p, QSize& size, qreal fontScale, DotPlotImageExportSettings& exportSettings);
    void drawAll(QPainter& p, qreal rulerFontScale = 1, bool drawFocus = true, bool drawAreaSelection = true, bool drawRepeatSelection = true);
    void drawNames(QPainter&) const;
    void drawAxises(QPainter&) const;
    void drawDots(QPainter&);
    void drawSelection(QPainter&) const;
    void drawRulers(QPainter&, qreal fontScale = 1) const;
    void drawMiniMap(QPainter&) const;
    void drawNearestRepeat(QPainter&) const;
    void drawFocus(QPainter& p) const;

    void sequencesMouseSelection(const QPointF&, const QPointF&);
    void sequencesCoordsSelection(const QPointF&, const QPointF&);
    void sequenceClearSelection();
    void clearRepeatSelection();

    void selectNearestRepeat(const QPointF&);
    float calculateDistance(float x, float y, DotPlotResults r, bool isReverse) const;
    const DotPlotResults* findNearestRepeat(const QPoint&);  // sets nearestInverted

    void calcZooming(const QPointF& oldzoom, const QPointF& newzoom, const QPoint& p, bool emitSignal = true);
    void multZooming(float multzoom);

    void resetZooming();
    void checkShift(bool emitSignal = true);

    void updateCursor();

    QString makeToolTipText() const;

    QPointF zoomTo(Qt::Axis axis, const U2Region& lr, bool emitSignal = true);
    U2Region getVisibleRange(Qt::Axis axis);
    int getLrDifference(const U2Region& a, const U2Region& b);

    void miniMapShift();

    void drawRectCorrect(QPainter& p, QRectF rect) const;

    QPoint toInnerCoords(int x, int y) const;
    QPoint toInnerCoords(const QPoint& p) const;

    QPointF unshiftedUnzoomed(const QPointF& p) const;
    QPoint sequenceCoords(const QPointF& c) const;

    QString getRoundedText(QPainter& p, int num, int size) const;
    bool getLineToDraw(const DotPlotResults& r, QLine* line, float ratioX, float ratioY, bool invert = false) const;

    void cancelRepeatFinderTask();
};

}  // namespace U2
