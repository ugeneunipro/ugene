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

#ifndef _U2_PAN_VIEW_H_
#define _U2_PAN_VIEW_H_

#include <QAction>
#include <QFont>
#include <QScrollBar>
#include <QToolButton>

#include <U2Core/Annotation.h>

#include <U2Gui/GraphUtils.h>

#include "GSequenceLineViewAnnotated.h"
#include "view_rendering/PanViewRenderer.h"

namespace U2 {

class PanViewRenderArea;
class GScrollBar;
class GObjectView;
class PVRowsManager;
class ADVSingleSequenceWidget;
class PVRowData;

class RulerInfo {
public:
    RulerInfo()
        : offset(0) {
    }
    RulerInfo(const QString& _name, int _offset, const QColor& _color)
        : name(_name), offset(_offset), color(_color) {
    }

    QString name;
    int offset;
    QColor color;
};

class PanViewLinesSettings {
public:
    PanViewLinesSettings();

    int getRowLine(int i) const;
    int getSelectionLine() const;

    int getRulerLine() const;
    int getCustomRulerLine(int n) const;

    int getFirstRowLine() const;
    int getAdditionalLines() const;

    int getNumVisibleRows() const;

    bool isRowVisible(int row) const;

    int numLines;  // number of visible lines
    int rowLinesOffset;  // row number on the first row line

    bool showMainRuler;
    bool showCustomRulers;
    QList<RulerInfo> customRulers;
};

class U2VIEW_EXPORT PanView : public GSequenceLineViewAnnotated {
    Q_OBJECT

public:
    class U2VIEW_EXPORT ZoomUseObject {
    public:
        ZoomUseObject();
        ZoomUseObject(PanView* pv);
        virtual ~ZoomUseObject();

        void useZoom();
        void releaseZoom();
        void setPanView(PanView* pv);

    private:
        bool usingZoom;
        PanView* panView;
    };

    PanView(QWidget* p, SequenceObjectContext* ctx, const PanViewRenderAreaFactory& rendererFactory = PanViewRenderAreaFactory());
    ~PanView();

    const U2Region& getFrameRange() const {
        return frameView->getVisibleRange();
    }

    QAction* getZoomInAction() const override;

    QAction* getZoomOutAction() const override;

    QAction* getZoomToSelectionAction() const override;

    QAction* getZoomToSequenceAction() const override;

    // [0..seqLen)
    void setVisibleRange(const U2Region& reg, bool signal = true) override;

    PVRowsManager* getRowsManager() const;

    virtual void setNumBasesVisible(qint64 n);

    void setSyncOffset(int o);

    int getSyncOffset() const;

    QList<RulerInfo> getCustomRulers() const;

    void addCustomRuler(const RulerInfo& r);

    void removeCustomRuler(const QString& name);

    void removeAllCustomRulers();

    QAction* getToggleMainRulerAction() const;

    QAction* getToggleCustomRulersAction() const;

    void hideEvent(QHideEvent* ev) override;

    void showEvent(QShowEvent* ev) override;

    PanViewLinesSettings* getLinesSettings() const;

protected:
    qint64 getSingleStep() const override;

    qint64 getPageStep() const override;

    void onVisibleRangeChanged(bool signal = true) override;

    void pack() override;

    void registerAnnotations(const QList<Annotation*>& l) override;
    void unregisterAnnotations(const QList<Annotation*>& l) override;
    void ensureVisible(Annotation* a, int locationIdx) override;

protected slots:
    void sl_sequenceChanged() override;
    void sl_onAnnotationsModified(const QList<AnnotationModification>& annotationModifications) override;

private slots:
    void sl_zoomInAction();
    void sl_zoomOutAction();
    void sl_zoomToSelection();
    void sl_zoomToSequence();

    void sl_onRowBarMoved(int);

    void sl_onRangeChangeRequest(qint64 start, qint64 end);

    void sl_onDNASelectionChanged(LRegionsSelection* s, const QVector<U2Region>& added, const QVector<U2Region>& removed) override;
    void sl_onAnnotationSettingsChanged(const QStringList& changedSettings) override;

    void sl_toggleMainRulerVisibility(bool visible);
    void sl_toggleCustomRulersVisibility(bool visible);
    void sl_updateRows();

signals:
    void si_updateRows();

public:
    void setSelection(const U2Region& r) override;
    void centerRow(int row);
    int calculateNumRowBarSteps() const;

    void setNumVisibleRows(int numRows);
    void updateNumVisibleRows();

    void updateActions();
    void updateRows();
    void updateRowBar();

    void useZoom();
    void releaseZoom();

    PanViewRenderArea* getRenderArea() const;

private:
    U2Region getRegionToZoom() const;

public:
    U2Region frameRange;
    int minNuclsPerScreen;

    QAction* zoomInAction;
    QAction* zoomOutAction;
    QAction* zoomToSelectionAction;
    QAction* zoomToSequenceAction;
    QAction* toggleMainRulerAction;
    QAction* toggleCustomRulersAction;

    PVRowsManager* rowsManager;
    QScrollBar* rowBar;
    int syncOffset;  // used by ADVSyncViewManager only

    PanViewLinesSettings* settings;

    int zoomUsing;
};

class U2VIEW_EXPORT PanViewRenderArea : public GSequenceLineViewGridAnnotationRenderArea {
    friend class PanView;
    Q_OBJECT
public:
    PanViewRenderArea(PanView* d, PanViewRenderer* renderer);

    PanViewRenderer* getRenderer() const {
        return renderer;
    }

    /** Returns all y-regions covered by the annotation location region. */
    QList<U2Region> getAnnotationYRegions(Annotation* annotation, int locationRegionIndex, const AnnotationSettings* annotationSettings) const override;

    /** Returns Y range for the annotation within render area. */
    U2Region getAnnotationYRange(Annotation* annotation, int locationRegionIndex, const AnnotationSettings* annotationSettings) const;

    int getRowLineHeight() const;

protected:
    void drawAll(QPaintDevice* pd) override;

    void resizeEvent(QResizeEvent* e) override;

private:
    bool isSequenceCharsVisible() const;

    PanView* getPanView() const {
        return static_cast<PanView*>(view);
    }

    PanView* const panView;
    PanViewRenderer* const renderer;
};

}  // namespace U2

#endif
