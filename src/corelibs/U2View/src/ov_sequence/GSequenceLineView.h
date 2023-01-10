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

#ifndef _U2_GSEQUENCE_LINE_VIEW_H_
#define _U2_GSEQUENCE_LINE_VIEW_H_

#include <QFlag>
#include <QFocusEvent>
#include <QHBoxLayout>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QScopedPointer>
#include <QToolButton>
#include <QWheelEvent>
#include <QWidget>

#include <U2Core/U2Region.h>

#include <U2Gui/SelectionModificationHelper.h>
#include <U2Gui/WidgetWithLocalToolbar.h>

namespace U2 {

class DNASequenceSelection;
class LRegionsSelection;
class GScrollBar;
class GSequenceLineViewRenderArea;
class GObject;
class GObjectViewOpConstraints;
class SequenceObjectContext;
class U2SequenceObject;

enum GSLV_UpdateFlag {
    GSLV_UF_NeedCompleteRedraw = 1 << 0,
    GSLV_UF_ViewResized = 1 << 1,
    GSLV_UF_VisibleRangeChanged = 1 << 2,
    GSLV_UF_SelectionChanged = 1 << 3,
    GSLV_UF_FocusChanged = 1 << 4,
    GSLV_UF_FrameChanged = 1 << 5,
    GSLV_UF_AnnotationsChanged = 1 << 6
};

enum GSLV_FeatureFlag {
    /** If set the view can show any requested custom range. Same with Zooming. */
    GSLV_FF_SupportsCustomRange = 0x1
};

typedef QFlags<GSLV_UpdateFlag> GSLV_UpdateFlags;
typedef QFlags<GSLV_FeatureFlag> GSLV_FeatureFlags;

// single-line sequence view
class U2VIEW_EXPORT GSequenceLineView : public WidgetWithLocalToolbar {
    Q_OBJECT
public:
    GSequenceLineView(QWidget* p, SequenceObjectContext* ctx);

    const U2Region& getVisibleRange() const {
        return visibleRange;
    }

    SequenceObjectContext* getSequenceContext() const {
        return ctx;
    }

    GSequenceLineViewRenderArea* getRenderArea() const {
        return renderArea;
    }

    qint64 getLastPressPos() const {
        return lastPressPos;
    }

    virtual void setStartPos(qint64 pos);

    virtual void setCenterPos(qint64 pos);

    qint64 getSequenceLength() const {
        return seqLen;
    }

    virtual void addUpdateFlags(GSLV_UpdateFlags newFlags) {
        lastUpdateFlags |= newFlags;
    }

    virtual void clearUpdateFlags() {
        lastUpdateFlags = 0;
    }

    GSLV_UpdateFlags getUpdateFlags() const {
        return lastUpdateFlags;
    }

    virtual void setFrameView(GSequenceLineView* frameView);

    virtual GSequenceLineView* getFrameView() const {
        return frameView;
    }

    virtual void setCoherentRangeView(GSequenceLineView* rangeView);

    virtual GSequenceLineView* getConherentRangeView() const {
        return coherentRangeView;
    }

    // [0..seqLen)
    virtual void setVisibleRange(const U2Region& reg, bool signal = true);

    virtual QAction* getZoomInAction() const {
        return coherentRangeView == nullptr ? nullptr : coherentRangeView->getZoomInAction();
    }

    virtual QAction* getZoomOutAction() const {
        return coherentRangeView == nullptr ? nullptr : coherentRangeView->getZoomOutAction();
    }

    virtual QAction* getZoomToSelectionAction() const {
        return coherentRangeView == nullptr ? nullptr : coherentRangeView->getZoomToSelectionAction();
    }

    virtual QAction* getZoomToSequenceAction() const {
        return coherentRangeView == nullptr ? nullptr : coherentRangeView->getZoomToSequenceAction();
    }

    virtual U2SequenceObject* getSequenceObject() const;

    virtual void buildPopupMenu(QMenu& m) {
        Q_UNUSED(m);
    }

    virtual bool isWidgetOnlyObject(GObject* o) const {
        Q_UNUSED(o);
        return false;
    }

    virtual bool eventFilter(QObject* watched, QEvent* event);

signals:
    void si_visibleRangeChanged();
    void si_centerPosition(qint64 pos);

protected:
    void resizeEvent(QResizeEvent* e);
    void mousePressEvent(QMouseEvent* me);
    void mouseReleaseEvent(QMouseEvent* me);
    void mouseMoveEvent(QMouseEvent* me);
    void mouseDoubleClickEvent(QMouseEvent* me);
    void wheelEvent(QWheelEvent* we);
    void focusInEvent(QFocusEvent* fe);
    void focusOutEvent(QFocusEvent* fe);
    void keyPressEvent(QKeyEvent* e);
    virtual void onVisibleRangeChanged(bool signal = true);

public slots:
    void sl_centerPosition(int pos) {
        setCenterPos(pos);
    }

protected slots:
    virtual void sl_onScrollBarMoved(int pos);
    virtual void sl_onDNASelectionChanged(LRegionsSelection* thiz, const QVector<U2Region>& added, const QVector<U2Region>& removed);
    virtual void sl_sequenceChanged();
    void sl_onFrameRangeChanged();
    void sl_onCoherentRangeViewRangeChanged();
    void sl_onLocalCenteringRequest(qint64 pos);
    void completeUpdate();

protected:
    QPoint toRenderAreaPoint(const QPoint& p) const;

    /**
     * Returns a valid Y-range to react to mouse events for the given 'pos'.
     * Normally this is a whole vertical range of the widget area, but in some widgets, like DetView it may be a limited space.
     * Reason for this is that DetView is a 'multi-line', while all methods inside GSequenceLineView are 'single-line'.
     * Uses 'renderArea' local coordinates.
     */
    virtual U2Region getCapturingRenderAreaYRegionForPos(qint64 pos) const;

    virtual void updateScrollBar();
    virtual void setSelection(const U2Region& r);
    void addSelection(const U2Region& r);
    virtual void updateCursorShapeOnMouseMove(const QPoint& p);
    virtual void moveBorder(const QPoint& p);
    virtual void pack();
    virtual qint64 getSingleStep() const;
    virtual qint64 getPageStep() const;
    void autoScrolling(const QPoint& areaPoint);
    virtual void resizeSelection(const QPoint& areaPoint);
    void cancelSelectionResizing();
    void changeSelectionOnScrollbarMoving(const U2Region& newSelection);
    void changeSelection(QVector<U2Region>& regions, const U2Region& newSelection);

    SequenceObjectContext* ctx;
    GSequenceLineViewRenderArea* renderArea;
    U2Region visibleRange;
    GScrollBar* scrollBar;
    qint64 lastPressPos;
    U2Region resizableRegion;
    QList<U2Region> overlappedRegions;
    qint64 seqLen;
    GSLV_UpdateFlags lastUpdateFlags;
    GSLV_FeatureFlags featureFlags;
    GSequenceLineView* frameView;
    GSequenceLineView* coherentRangeView;
    double coefScrollBarMapping;

    // special flag setup by child classes that tells to this class do or skip
    // any changes to selection on mouse ops
    bool ignoreMouseSelectionEvents;
    bool singleBaseSelection;
    bool isSelectionResizing;
};

class U2VIEW_EXPORT GSequenceLineViewRenderArea : public QWidget {
    Q_OBJECT
public:
    GSequenceLineViewRenderArea(GSequenceLineView* p);

    /** Returns in-sequence base index by the current on-screen coordinate. */
    virtual qint64 coordToPos(const QPoint& coord) const;

    /** Returns a minimal on-screen X coordinate of the given sequence position. */
    virtual int posToCoord(qint64 pos, bool useVirtualSpace = false) const;

    /** Returns number of pixels per-base. */
    virtual double getCurrentScale() const;

    /** Returns width in pixels required to draw a single text character using sequenceFont. */
    int getCharWidth() const {
        return charWidth;
    }

protected:
    void paintEvent(QPaintEvent* e) override;

    virtual void drawAll(QPaintDevice* pd) = 0;
    void drawFrame(QPainter& p);
    virtual void drawFocus(QPainter& p);

    void updateFontMetrics();

    /** Returns a cached pixmap used to render the whole area. */
    QPixmap* getCachedPixmap() const {
        return cachedView.data();
    }

    GSequenceLineView* view;
    QScopedPointer<QPixmap> cachedView;

    //! VIEW_RENDERER_REFACTORING: the following parameters should be stored only in renderer (until they cannot be modified in view).
    //! Currently they are doubled in SequenceViewRenderer class.
    // per char and per line metrics
    QFont sequenceFont;
    QFont smallSequenceFont;
    QFont rulerFont;

    int charWidth = 0;
    int smallCharWidth = 0;

    int lineHeight = 0;
    int yCharOffset = 0;
    int xCharOffset = 0;
};

}  // namespace U2

#endif
