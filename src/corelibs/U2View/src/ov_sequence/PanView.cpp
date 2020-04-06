/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2019 UniPro <ugene@unipro.ru>
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

#include "PanView.h"

#include "ADVSequenceObjectContext.h"
#include "ADVSingleSequenceWidget.h"
#include "PanViewRows.h"

#include <U2Core/AnnotationModification.h>
#include <U2Core/AnnotationSettings.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/Log.h>
#include <U2Core/SelectionModel.h>
#include <U2Core/Timer.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Gui/GraphUtils.h>
#include <U2Gui/GScrollBar.h>

#include <QDialog>
#include <QFontMetrics>
#include <QGridLayout>
#include <QPainter>
#include <QTextEdit>


namespace U2 {

PanViewLinesSettings::PanViewLinesSettings()
    : numLines(0),
      rowLinesOffset(0),
      showMainRuler(true),
      showCustomRulers(true) {
}

int PanViewLinesSettings::getRowLine(int i) const {
    int line = getFirstRowLine() - i + rowLinesOffset;
    if (line < 0 || line > getFirstRowLine()) {
        return -1;
    }
    return line;
}

int PanViewLinesSettings::getSelectionLine() const {
    return numLines - 1;
}

int PanViewLinesSettings::getRulerLine() const {
    SAFE_POINT(showMainRuler, "Trying to get ruler line, but it's not visible", -1);
    return numLines - 2;
}

int PanViewLinesSettings::getCustomRulerLine(int n) const {
    SAFE_POINT(showCustomRulers, "Trying to get custom ruler line, but it's not visible", -1);
    SAFE_POINT(n >= 0 && n < customRulers.count(), "Invalid number of custom ruler", -1);

    return numLines - (showMainRuler ? 3 : 2) - n;
}

int PanViewLinesSettings::getFirstRowLine() const {
    return numLines - 2 - (showMainRuler ? 1 : 0) - (showCustomRulers ? customRulers.count() : 0 );
}

int PanViewLinesSettings::getAdditionalLines() const {
    return 1 + (showMainRuler ? 1 : 0) + (showCustomRulers ? customRulers.size() : 0);
}

int PanViewLinesSettings::getNumVisibleRows() const {
    return getFirstRowLine() + 1;
}

bool PanViewLinesSettings::isRowVisible(int row) const {
    return getRowLine(row) >= 0;
}

PanView::ZoomUseObject::ZoomUseObject()
: usingZoom(false), panView(NULL) {}

PanView::ZoomUseObject::ZoomUseObject(PanView *pv)
: usingZoom(false) {

    setPanView(pv);
}

PanView::ZoomUseObject::~ZoomUseObject(){

    releaseZoom();
}

void PanView::ZoomUseObject::setPanView(PanView *pv) {

    releaseZoom();

    Q_ASSERT(pv);
    panView = pv;

    useZoom();
}

void PanView::ZoomUseObject::useZoom() {

    if (usingZoom || !panView) {
        return;
    }
    usingZoom = true;
    panView->useZoom();
}

void PanView::ZoomUseObject::releaseZoom() {

    if (!usingZoom || !panView) {
        return;
    }
    usingZoom = false;
    panView->releaseZoom();
}

#define MAX_VISIBLE_ROWS_ON_START 10
PanView::PanView(QWidget* p, SequenceObjectContext* ctx, const PanViewRenderAreaFactory &renderFactory)
    : GSequenceLineViewAnnotated(p, ctx)
{
    rowBar = new QScrollBar(this);
    setObjectName("pan_view");
    settings = new PanViewLinesSettings();
    rowsManager = new PVRowsManager();
    renderArea = renderFactory.createRenderArea(this);
    renderArea->setMouseTracking(true);

    updateNumVisibleRows();

    visibleRange.length = seqLen;
    minNuclsPerScreen = qMin(seqLen, qint64(0));

    zoomUsing = 0;

    zoomInAction = new QAction(QIcon(":/core/images/zoom_in.png"), tr("Zoom In"), this);
    zoomInAction->setObjectName("action_zoom_in_" + ctx->getSequenceObject()->getGObjectName());
    connect(zoomInAction, SIGNAL(triggered()), SLOT(sl_zoomInAction()));

    zoomOutAction = new QAction(QIcon(":/core/images/zoom_out.png"), tr("Zoom Out"), this);
    zoomOutAction->setObjectName("action_zoom_out_" + ctx->getSequenceObject()->getGObjectName());
    connect(zoomOutAction, SIGNAL(triggered()), SLOT(sl_zoomOutAction()));

    zoomToSelectionAction= new QAction(QIcon(":/core/images/zoom_sel.png"), tr("Zoom to Selection"), this);
    zoomToSelectionAction->setObjectName("action_zoom_to_selection_" + ctx->getSequenceObject()->getGObjectName());
    connect(zoomToSelectionAction, SIGNAL(triggered()), SLOT(sl_zoomToSelection()));

    zoomToSequenceAction = new QAction(QIcon(":/core/images/zoom_whole.png"), tr("Zoom to Whole Sequence"), this);
    zoomToSequenceAction->setObjectName("action_zoom_to_sequence_" + ctx->getSequenceObject()->getGObjectName());
    connect(zoomToSequenceAction, SIGNAL(triggered()), SLOT(sl_zoomToSequence()));

    toggleMainRulerAction = new QAction(tr("Show Main Ruler"), this);
    toggleMainRulerAction->setObjectName("Show Main Ruler");
    toggleMainRulerAction->setCheckable(true);
    toggleMainRulerAction->setChecked(settings->showMainRuler);
    connect(toggleMainRulerAction, SIGNAL(triggered(bool)), SLOT(sl_toggleMainRulerVisibility(bool)));

    toggleCustomRulersAction = new QAction(tr("Show Custom Rulers"), this);
    toggleCustomRulersAction->setObjectName("Show Custom Rulers");
    toggleCustomRulersAction->setCheckable(true);
    toggleCustomRulersAction->setChecked(settings->showCustomRulers);
    toggleCustomRulersAction->setEnabled(!settings->customRulers.isEmpty());
    connect(toggleCustomRulersAction, SIGNAL(triggered(bool)), SLOT(sl_toggleCustomRulersVisibility(bool)));

    addActionToLocalToolbar(zoomInAction);
    addActionToLocalToolbar(zoomOutAction);
    addActionToLocalToolbar(zoomToSelectionAction);
    addActionToLocalToolbar(zoomToSequenceAction);

    syncOffset = 0;

    //can't move to the GSequenceLineViewAnnotated -> virtual calls does not work in  constructor
    foreach(AnnotationTableObject *obj, ctx->getAnnotationObjects(true)) {
        registerAnnotations(obj->getAnnotations());
    }

    connect(this, SIGNAL(si_updateRows()), SLOT(sl_updateRows()));

    updateActions();
    updateRowBar();

    settings->numLines = qMin(MAX_VISIBLE_ROWS_ON_START, rowsManager->getNumRows() + settings->getAdditionalLines());

    resize(width(), getRenderArea()->getRowLineHeight() * settings->numLines );

    pack();
}

void PanView::pack() {
    QGridLayout* layout = new QGridLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(renderArea, 0, 0, 1, 1);
    layout->addWidget(rowBar, 0, 1, 1, 1);
    layout->addWidget(scrollBar, 1, 0, 1, 1);
    setContentLayout(layout);
}

PanView::~PanView() {
    delete rowsManager;
}

void PanView::registerAnnotations(const QList<Annotation *> &l) {
    GTIMER(c1, t1, "PanView::registerAnnotations");
    AnnotationSettingsRegistry* asr = AppContext::getAnnotationsSettingsRegistry();
    foreach (Annotation *a, l) {
        AnnotationSettings *as = asr->getAnnotationSettings(a->getData());
        if (as->visible) {
            rowsManager->addAnnotation(a);
        }
    }
    updateRows();
}

void PanView::unregisterAnnotations(const QList<Annotation *> &l) {
    AnnotationSettingsRegistry *asr = AppContext::getAnnotationsSettingsRegistry();
    foreach (Annotation *a, l) {
        AnnotationSettings *as = asr->getAnnotationSettings(a->getData());
        if (as->visible) {
            rowsManager->removeAnnotation(a);
        }
    }
    emit si_updateRows();
}

void PanView::updateRows() {
    PanViewRenderArea* ra = getRenderArea();
    SAFE_POINT(ra != NULL, "PanViewRenderArea is NULL", );
    /*ra->*/updateNumVisibleRows();
    int maxSteps = calculateNumRowBarSteps();
    if (qAbs(rowBar->maximum() - rowBar->minimum())!=maxSteps) {
        updateRowBar();
    }
    updateActions();
}

int  PanView::calculateNumRowBarSteps() const {
    int visibleRows = settings->getNumVisibleRows();
    int numRows = rowsManager->getNumRows();
    int res = qMax(0, numRows - visibleRows);
    return res;
}

void PanView::setNumVisibleRows(int rowNum) {
    settings->numLines = qMin( rowNum, rowsManager->getNumRows() + settings->getAdditionalLines());

    addUpdateFlags(GSLV_UF_ViewResized);
    update();
}

void PanView::updateNumVisibleRows() {
    setNumVisibleRows( renderArea->height() / getRenderArea()->getRowLineHeight() );
}

void PanView::updateRowBar() {
    rowBar->disconnect(this);

    int visibleRows = settings->getNumVisibleRows();
    int maxSteps = calculateNumRowBarSteps();

    rowBar->setMinimum(-maxSteps); //inverted appearance
    rowBar->setMaximum(0);
    rowBar->setSingleStep(1);
    rowBar->setPageStep(visibleRows - 1);
    int rowsOffset = qMin(maxSteps, settings->rowLinesOffset);
    settings->rowLinesOffset = rowsOffset;
    rowBar->setSliderPosition( - rowsOffset);
    rowBar->setEnabled(maxSteps > 0);

    connect(rowBar, SIGNAL(valueChanged(int)), SLOT(sl_onRowBarMoved(int)));
}

void PanView::sl_onRowBarMoved(int v) {
    settings->rowLinesOffset = - v; // '-' because of inverted appearance
    addUpdateFlags(GSLV_UF_NeedCompleteRedraw);
    update();
}

void PanView::sl_onAnnotationsModified(const QList<AnnotationModification> &annotationModifications) {
    QList<Annotation *> modified;
    foreach (const AnnotationModification &annotationModification, annotationModifications) {
        modified << annotationModification.annotation;
    }
    unregisterAnnotations(modified);
    registerAnnotations(modified);

    addUpdateFlags(GSLV_UF_AnnotationsChanged);
    update();
    GSequenceLineViewAnnotated::sl_onAnnotationsModified(annotationModifications);
}

void PanView::sl_onAnnotationSettingsChanged(const QStringList& changedSettings) {
    AnnotationSettingsRegistry *asr = AppContext::getAnnotationsSettingsRegistry();
    foreach (const QString &name, changedSettings) {
        AnnotationSettings *as = asr->getAnnotationSettings(name);
        bool hasRow = rowsManager->contains(name);
        if (as->visible == hasRow) {
            continue;
        }
        QList<Annotation *> changed;
        foreach (AnnotationTableObject *ao, ctx->getAnnotationObjects(true)) {
            changed << ao->getAnnotationsByName(name);
        }
        if (changed.isEmpty()) {
            continue;
        }
        foreach (Annotation *a, changed) {
            if (as->visible) {
                rowsManager->addAnnotation(a);
            } else  {
                rowsManager->removeAnnotation(a);
            }
        }
    }
    updateRows();
    GSequenceLineViewAnnotated::sl_onAnnotationSettingsChanged(changedSettings);
}

void PanView::setSelection(const U2Region& r) {
    ctx->getSequenceSelection()->setRegion(r);
}

void PanView::onVisibleRangeChanged(bool signal) {
    updateActions();
    GSequenceLineView::onVisibleRangeChanged(signal);
}

void PanView::useZoom() {

    zoomUsing++;
    updateActions();
}

void PanView::releaseZoom() {

    if (zoomUsing) {
        zoomUsing--;
        updateActions();
    }

    Q_ASSERT(zoomUsing >= 0);
}

void PanView::updateActions() {

    if (!zoomUsing) {
        zoomInAction->setDisabled(true);
        zoomOutAction->setDisabled(true);

        zoomToSelectionAction->setDisabled(true);
        zoomToSequenceAction->setDisabled(true);

        return;
    }

    zoomInAction->setEnabled(visibleRange.length > minNuclsPerScreen);
    zoomOutAction->setEnabled(visibleRange.length < seqLen);

    const QVector<U2Region>& sel = ctx->getSequenceSelection()->getSelectedRegions();
    if (!sel.isEmpty() && sel.first().length >= minNuclsPerScreen) {
        zoomToSelectionAction->setEnabled(true);
    } else {
        zoomToSelectionAction->setEnabled(false);
    }
    zoomToSequenceAction->setEnabled(visibleRange.startPos != 0 || visibleRange.endPos() != seqLen);

}

void PanView::sl_zoomInAction() {
    assert(visibleRange.length >= minNuclsPerScreen);
    const U2Region selRange = getRegionToZoom();
    if (!selRange.isEmpty() && selRange.length >= minNuclsPerScreen && visibleRange.contains(selRange) && visibleRange != selRange) {
        sl_zoomToSelection();
        return;
    }
    U2Region newVisibleRange = visibleRange;
    newVisibleRange.length = qMax((visibleRange.length + 1) / 2, (qint64)minNuclsPerScreen);
    if (newVisibleRange.length!=visibleRange.length) {
        newVisibleRange.startPos = visibleRange.startPos + (visibleRange.length - newVisibleRange.length)/2;
        setVisibleRange(newVisibleRange);
    }
}

void PanView::sl_zoomOutAction() {
    assert(visibleRange.length <= seqLen);
    U2Region newVisibleRange = visibleRange;
    newVisibleRange.length = qMin(visibleRange.length * 2, seqLen);
    if (newVisibleRange.length != visibleRange.length) {
        newVisibleRange.startPos = qBound(qint64(0), visibleRange.startPos - (newVisibleRange.length - visibleRange.length)/2, seqLen-newVisibleRange.length);
        assert(newVisibleRange.startPos >=0 && newVisibleRange.endPos() <= seqLen); //todo: move to setVisibleRange
        setVisibleRange(newVisibleRange);
    }
}

void PanView::sl_onDNASelectionChanged(LRegionsSelection* s, const QVector<U2Region>& added, const QVector<U2Region>& removed) {
    GSequenceLineView::sl_onDNASelectionChanged(s, added, removed);
    updateActions();
}

void PanView::sl_zoomToSelection() {
    const U2Region selRegion = getRegionToZoom();
    if (selRegion.isEmpty()) {
        return;
    }
    if (selRegion.length < minNuclsPerScreen) {
        return;
    }
    if (visibleRange == selRegion) {
        return;
    }
    SAFE_POINT(U2Region(0, ctx->getSequenceObject()->getSequenceLength()).contains(selRegion), "Invalid selection region",);
    visibleRange = selRegion;
    onVisibleRangeChanged();
}

void PanView::sl_zoomToSequence() {
    U2Region wholeRange(0, seqLen);
    assert(visibleRange != wholeRange);
    visibleRange = wholeRange;
    onVisibleRangeChanged();
}

void PanView::setVisibleRange(const U2Region& newRange, bool signal) {
    assert(newRange.startPos >=0 && newRange.endPos() <= seqLen);

    if (newRange.length < minNuclsPerScreen) {
        minNuclsPerScreen = newRange.length;
    }
    GSequenceLineView::setVisibleRange(newRange, signal);
}

void PanView::ensureVisible(Annotation *a, int locationIdx) {
    AnnotationSettingsRegistry *asr = AppContext::getAnnotationsSettingsRegistry();
    const AnnotationSettings *as = asr->getAnnotationSettings(a->getData());
    if (as->visible) {
        const int row = rowsManager->getAnnotationRowIdx(a);
        if (!settings->isRowVisible(row)) {
            centerRow(row);
        }
    }
    GSequenceLineViewAnnotated::ensureVisible(a, locationIdx);
}

void PanView::centerRow(int row) {
    int targetFirstRowLine = qMax(0, row - settings->getNumVisibleRows() / 2);
    int rowOnTheFirstLine = settings->rowLinesOffset;
    if (targetFirstRowLine == rowOnTheFirstLine) {
        return;
    }
    int dPos = targetFirstRowLine - rowOnTheFirstLine;
    int sliderPos = qBound(rowBar->minimum(), rowBar->value() - dPos, rowBar->maximum());
    rowBar->setSliderPosition(sliderPos);
}


void PanView::sl_onRangeChangeRequest(qint64 start, qint64 end) {
    uiLog.trace(tr("range change request: [%1, %2]").arg(start).arg(end));
    setVisibleRange(U2Region(start-1, end));
}

void PanView::setNumBasesVisible(qint64 n) {
    qint64 nBases = qBound((qint64)minNuclsPerScreen, n, seqLen);
    qint64 center = visibleRange.startPos + visibleRange.length / 2;
    qint64 newStart = qMax(qint64(0), center - nBases / 2);
    assert(newStart + nBases <= seqLen);
    setVisibleRange(U2Region(newStart, nBases));
}

PanViewRenderArea* PanView::getRenderArea() const {
    return qobject_cast<PanViewRenderArea*>(renderArea);
}

QList<RulerInfo> PanView::getCustomRulers() const {
    return settings->customRulers;
}

void PanView::addCustomRuler(const RulerInfo& r) {
    settings->customRulers.append(r);
    if (settings->showCustomRulers) {
        addUpdateFlags(GSLV_UF_NeedCompleteRedraw);
        update();
    }
    toggleCustomRulersAction->setEnabled(true);
}

void PanView::removeCustomRuler(const QString& name) {
    for (int i=0, n = settings->customRulers.count(); i < n; i++) {
        if (settings->customRulers[i].name == name) {
            settings->customRulers.removeAt(i);
            break;
        }
    }
    toggleCustomRulersAction->setEnabled(!settings->customRulers.isEmpty());
    if (settings->showCustomRulers) {
        addUpdateFlags(GSLV_UF_NeedCompleteRedraw);
        update();
    }
}

void PanView::removeAllCustomRulers() {
    toggleCustomRulersAction->setEnabled(false);
    if (!settings->customRulers.isEmpty()) {
        settings->customRulers.clear();

        addUpdateFlags(GSLV_UF_NeedCompleteRedraw);
        update();
    }
}

void PanView::sl_toggleMainRulerVisibility(bool visible) {
    settings->showMainRuler = visible;

    addUpdateFlags(GSLV_UF_NeedCompleteRedraw);
    update();
}

void PanView::sl_toggleCustomRulersVisibility(bool visible) {
    settings->showCustomRulers = visible;

    addUpdateFlags(GSLV_UF_NeedCompleteRedraw);
    update();
}


void PanView::setSyncOffset(int o) {
    if (o == syncOffset) {
        return;
    }
    syncOffset = o;
    addUpdateFlags(GSLV_UF_NeedCompleteRedraw);
    update();
}

void PanView::sl_sequenceChanged(){
    seqLen = ctx->getSequenceLength();
    U2Region curSource(0, ctx->getSequenceLength()), newRange(0,0);
    if (!curSource.contains(visibleRange)) {
        if (curSource.length > visibleRange.length){
            newRange.startPos = visibleRange.startPos - (visibleRange.endPos() - curSource.endPos());
            newRange.length = visibleRange.length;
        } else {
            newRange = curSource;
        }
        setVisibleRange(newRange);
    }
    GSequenceLineView::sl_sequenceChanged();
    updateActions();
}

void PanView::hideEvent(QHideEvent *ev){
    zoomInAction->setDisabled(true);
    zoomOutAction->setDisabled(true);
    zoomToSelectionAction->setDisabled(true);
    zoomToSequenceAction->setDisabled(true);
    QWidget::hideEvent(ev);
}

void PanView::showEvent(QShowEvent *ev){
    QWidget::showEvent(ev);
    updateActions();
}

void PanView::sl_updateRows(){
    updateRows();
}

const U2Region PanView::getRegionToZoom() const {
    const QVector<U2Region>& sel = ctx->getSequenceSelection()->getSelectedRegions();
    const QList<AnnotationSelectionData> annotationSel = getSequenceContext()->getAnnotationsSelection()->getSelection();
    U2Region selRegion;
    if (!sel.isEmpty()) {
        selRegion = sel.first();
    } else if (!annotationSel.isEmpty()) {
        selRegion = U2Region::containingRegion(annotationSel.first().annotation->getRegions());
    }

    return selRegion;
}

//////////////////////////////////////////////////////////////////////////
/// render
PanViewRenderArea::PanViewRenderArea(PanView* d, PanViewRenderer *renderer)
    : GSequenceLineViewAnnotatedRenderArea(d),
      panView(d),
      renderer(renderer)
{
    SAFE_POINT(NULL != renderer, "Renderer is NULL", );
}

PanViewRenderArea::~PanViewRenderArea() {
    delete renderer;
}

void PanViewRenderArea::drawAll(QPaintDevice* pd) {
    GTIMER(c2,t2,"PanViewRenderArea::drawAll");
    GSLV_UpdateFlags uf = view->getUpdateFlags();
    bool completeRedraw = uf.testFlag(GSLV_UF_NeedCompleteRedraw) || uf.testFlag(GSLV_UF_ViewResized) ||
                          uf.testFlag(GSLV_UF_VisibleRangeChanged) || uf.testFlag(GSLV_UF_AnnotationsChanged);

    QPainter p(pd);
    if (completeRedraw) {
        QPainter pCached(cachedView);
        renderer->drawAll(pCached, QSize(pd->width(), pd->height()), view->getVisibleRange());
        pCached.end();
    }

    p.drawPixmap(0, 0, *cachedView);

    // SANGER_TODO: should not be this kind of connection
//    ADVSingleSequenceWidget* ssw = panView->seqWidget;
//    SAFE_POINT(ssw != NULL, "ADVSingleSequenceWidget is NULL", );
//    if (!ssw->isOverviewCollapsed()) {
//        //! VIEW_RENDERER_REFACTORING: consider to move frame drawing to renderer
//        drawFrame(p);
//    }

    renderer->drawSelection(p, QSize(pd->width(), pd->height()), view->getVisibleRange());

    if (view->hasFocus()) {
        drawFocus(p);
    }
}

U2Region PanViewRenderArea::getAnnotationYRange(Annotation *a, int r, const AnnotationSettings *as) const {
    U2Region region = renderer->getAnnotationYRange(a, r, as, size(), view->getVisibleRange());
    region.startPos += renderer->getContentIndentY(size(), view->getVisibleRange());
    return region;
}

int PanViewRenderArea::getRowLineHeight() const {
    return renderer->getRowLineHeight();
}

void PanViewRenderArea::setRenderer(PanViewRenderer *newRenderer) {
    SAFE_POINT(NULL != newRenderer, "New renderer is NULL", );
    delete renderer;
    renderer = newRenderer;
}

bool PanViewRenderArea::isSequenceCharsVisible() const {
    return getCurrentScale() >= smallCharWidth;
}

void PanViewRenderArea::resizeEvent(QResizeEvent *e) {
    view->addUpdateFlags(GSLV_UF_ViewResized);

    PanView* pv = getPanView();
    SAFE_POINT(pv != NULL, "Panview is NULL", );
    pv->updateNumVisibleRows();
    pv->updateRowBar();

    QWidget::resizeEvent(e);
}

} //namespace
