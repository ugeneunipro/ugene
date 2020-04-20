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

#include <QApplication>
#include <QDialog>
#include <QMessageBox>
#include <QToolButton>
#include <QWidgetAction>

#include <U2Core/AnnotationSelection.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GHints.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/ExportImageDialog.h>
#include <U2Gui/GUIUtils.h>
#include <U2Gui/OrderedToolbar.h>
#include <U2Gui/PositionSelector.h>
#include <U2Gui/RangeSelector.h>

#include "ADVConstants.h"
#include "ADVSequenceObjectContext.h"
#include "ADVSingleSequenceWidget.h"
#include "AnnotatedDNAView.h"
#include "CreateRulerDialogController.h"
#include "DetView.h"
#include "GSequenceGraphView.h"
#include "Overview.h"
#include "image_export/SingleSequenceImageExportController.h"
#include "ov_sequence/codon_table/CodonTable.h"

namespace U2 {

#define ADV_HEADER_HEIGHT 30
#define ADV_HEADER_TOOLBAR_SPACING 6
#define ADV_HEADER_TOP_BOTTOM_INDENT 2
#define IMAGE_DIR   "image"

const QString ADVSingleSequenceWidget::SEQUENCE_SETTINGS = "sequenceViewSettings";
const QString ADVSingleSequenceWidget::DET_VIEW_COLLAPSED = SEQUENCE_SETTINGS + "/detViewCollapsed";
const QString ADVSingleSequenceWidget::ZOOM_VIEW_COLLAPSED = SEQUENCE_SETTINGS + "/zoomViewState";
const QString ADVSingleSequenceWidget::OVERVIEW_COLLAPSED = SEQUENCE_SETTINGS + "/overviewState";

ADVSingleSequenceWidget::ADVSingleSequenceWidget(ADVSequenceObjectContext* seqCtx, AnnotatedDNAView* ctx)
    : ADVSequenceWidget(ctx),
      detView(NULL),
      panView(NULL),
      overview(NULL)
{
    seqContexts.append(seqCtx);

    toggleViewAction = new QAction(this);
    toggleViewAction->setObjectName("show_hide_all_views");
    connect(toggleViewAction, SIGNAL(triggered()), SLOT(sl_toggleAllSubViews()));

    togglePanViewAction = new QAction(this);
    togglePanViewAction->setCheckable(true);
    togglePanViewAction->setObjectName("show_hide_zoom_view");
    togglePanViewAction->setIcon(QIcon(":/core/images/zoom_view.png"));
    connect(togglePanViewAction, SIGNAL(triggered(bool)), SLOT(sl_togglePanView(bool)));

    toggleDetViewAction = new QAction(this);
    toggleDetViewAction->setCheckable(true);
    toggleDetViewAction->setObjectName("show_hide_details_view");
    toggleDetViewAction->setIcon(QIcon(":/core/images/details_view.png"));
    connect(toggleDetViewAction, SIGNAL(triggered(bool)), SLOT(sl_toggleDetView(bool)));

    toggleOverviewAction = new QAction(this);
    toggleOverviewAction->setCheckable(true);
    toggleOverviewAction->setObjectName("show_hide_overview");
    toggleOverviewAction->setIcon(QIcon(":/core/images/overview.png"));
    connect(toggleOverviewAction, SIGNAL(triggered(bool)), SLOT(sl_toggleOverview(bool)));

    connect(seqCtx->getAnnotatedDNAView()->getAnnotationsSelection(),
        SIGNAL(si_selectionChanged(AnnotationSelection *, const QList<Annotation *> &, const QList<Annotation *> &)),
        SLOT(sl_onAnnotationSelectionChanged(AnnotationSelection *, const QList<Annotation *> &, const QList<Annotation *> &)));

    selectRangeAction1 = new QAction(QIcon(":/core/images/select_region.png"), tr("Select sequence region..."), this);
    selectRangeAction1->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_A));
    selectRangeAction1->setObjectName("select_range_action");
    selectRangeAction1->setShortcutContext(Qt::WidgetShortcut);
    connect(selectRangeAction1, SIGNAL(triggered()), SLOT(sl_onSelectRange()));

    selectRangeAction2 = new QAction(QIcon(":/core/images/select_region.png"), tr("Sequence region..."), this);
    selectRangeAction2->setObjectName("Sequence region");
    connect(selectRangeAction2, SIGNAL(triggered()), SLOT(sl_onSelectRange()));

    selectInAnnotationRangeAction= new QAction(tr("Sequence between selected annotations"), this);
    selectInAnnotationRangeAction->setObjectName("Sequence between selected annotations");
    connect(selectInAnnotationRangeAction, SIGNAL(triggered()), SLOT(sl_onSelectInRange()));

    selectOutAnnotationRangeAction= new QAction(tr("Sequence around selected annotations"), this);
    selectOutAnnotationRangeAction->setObjectName("Sequence around selected annotations");
    connect(selectOutAnnotationRangeAction, SIGNAL(triggered()), SLOT(sl_onSelectOutRange()));

    zoomToRangeAction = new QAction(QIcon(":/core/images/zoom_reg.png"), tr("Zoom to range..."), this);
    zoomToRangeAction->setObjectName("zoom_to_range_" + getSequenceObject()->getGObjectName());
    connect(zoomToRangeAction, SIGNAL(triggered()), SLOT(sl_zoomToRange()));

    createNewRulerAction = new QAction(tr("Create new ruler..."), this);
    createNewRulerAction->setObjectName("Create new ruler");
    connect(createNewRulerAction, SIGNAL(triggered()), SLOT(sl_createCustomRuler()));

    linesLayout = new QVBoxLayout();
    linesLayout->setMargin(0);
    linesLayout->setSpacing(0);
    linesLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    linesSplitter = new QSplitter(Qt::Vertical);
    linesSplitter->setChildrenCollapsible(false);
    linesSplitter->setBackgroundRole(QPalette::Window);
    linesSplitter->setAutoFillBackground(true);

    QWidget *linesLayoutWidget = new QWidget();
    linesLayoutWidget->setObjectName("lines_layout_widget");
    linesLayoutWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    linesLayoutWidget->setLayout(linesLayout);

    linesSplitter->addWidget(linesLayoutWidget);

    QVBoxLayout* l = new QVBoxLayout(this);
    l->setMargin(0);
    l->setSpacing(0);
    l->addWidget(linesSplitter);
    l->setSizeConstraint(QLayout::SetMinAndMaxSize);
    setLayout(l);

    headerWidget = new ADVSingleSequenceHeaderWidget(this);
    headerWidget->installEventFilter(this);
    headerWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    linesLayout->addWidget(headerWidget);

    init();
    updateMinMaxHeight();
    setDetViewCollapsed(AppContext::getSettings()->getValue(DET_VIEW_COLLAPSED, QVariant(false)).toBool());
    setPanViewCollapsed(AppContext::getSettings()->getValue(ZOOM_VIEW_COLLAPSED, QVariant(false)).toBool());
    setOverviewCollapsed(AppContext::getSettings()->getValue(OVERVIEW_COLLAPSED, QVariant(false)).toBool());
}

void ADVSingleSequenceWidget::init() {
    ADVSequenceObjectContext* seqCtx = getSequenceContext();
    detView = new DetView(this, seqCtx);
    const QString objName = getSequenceObject()->getGObjectName();
    detView->setObjectName("det_view_" + objName);
    detView->setMouseTracking(true);
    detView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    panView = new PanView(this, seqCtx);
    panView->setObjectName("pan_view_" + objName);
    panView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    connect(panView, SIGNAL(si_centerPosition(qint64)), SLOT(sl_onLocalCenteringRequest(qint64)));

    zoomUseObject.setPanView(panView);

    addSequenceView(panView);
    addSequenceView(detView, panView);

    panView->setFrameView(detView);
    overview = new Overview(this, seqCtx);
    overview->setObjectName("overview_" + objName);
    overview->setMouseTracking(true);
    overview->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    lineViews.append(overview);
    linesLayout->addWidget(overview);

    buttonTabOrederedNames = new QList<QString>;

    QToolBar* hStandardBar = headerWidget->getStandardToolBar();
    QToolBar* hViewsBar = headerWidget->getViewsToolBar();
    hViewsBar->addSeparator();
    addButtonWithActionToToolbar(toggleOverviewAction, hViewsBar);
    addButtonWithActionToToolbar(togglePanViewAction, hViewsBar);
    addButtonWithActionToToolbar(toggleDetViewAction, hViewsBar);

    addButtonWithActionToToolbar(selectRangeAction1, hStandardBar);
    buttonTabOrederedNames->append(selectRangeAction1->objectName());

    QAction* shotScreenAction = new QAction(QIcon(":/core/images/cam2.png"), tr("Export image"), this);
    shotScreenAction->setObjectName("export_image");
    connect(shotScreenAction, SIGNAL(triggered()), this, SLOT(sl_saveScreenshot()));

    addButtonWithActionToToolbar(shotScreenAction, hStandardBar);
    buttonTabOrederedNames->append(shotScreenAction->objectName());

    panView->addActionToLocalToolbar(zoomToRangeAction);

    toggleDetViewAction->setChecked(true);
    togglePanViewAction->setChecked(true);
    toggleOverviewAction->setChecked(true);

    addButtonWithActionToToolbar(toggleViewAction, hViewsBar);
    hViewsBar->addSeparator();

    closeViewAction = new QAction(tr("Remove sequence"), this);
    closeViewAction->setObjectName("remove_sequence");
    closeViewAction->setIcon(QIcon(":core/images/close_small.png"));
    addButtonWithActionToToolbar(closeViewAction, hViewsBar);
    connect(closeViewAction, SIGNAL(triggered()), SLOT(sl_closeView()));

    dynamic_cast<OrderedToolbar *>(hStandardBar)->setButtonTabOrderList(buttonTabOrederedNames);

    updateSelectionActions();

#define MIN_SEQUENCE_LEN_TO_USE_FULL_MODE 100
    if (seqCtx->getSequenceLength() < MIN_SEQUENCE_LEN_TO_USE_FULL_MODE) {
        //sequence is rather small -> show panview only by default
        setOverviewCollapsed(true);
        setDetViewCollapsed(true);
    }

    updateViewButtonState();
    togglePanViewAction->setText(isPanViewCollapsed() ? tr("Show zoom view") : tr("Hide zoom view"));
    toggleDetViewAction->setText(isDetViewCollapsed() ? tr("Show details view") : tr("Hide details view"));
    toggleOverviewAction->setText(isOverviewCollapsed() ? tr("Show overview") : tr("Hide overview"));
}


ADVSingleSequenceWidget::~ADVSingleSequenceWidget() {
    delete buttonTabOrederedNames;
}

QToolButton* ADVSingleSequenceWidget::addButtonWithActionToToolbar(QAction * buttonAction, QToolBar * toolBar, int position) const {
    SAFE_POINT(NULL != buttonAction, "buttonAction is NULL", NULL);
    SAFE_POINT(NULL != toolBar, "toolBar is NULL", NULL);
    SAFE_POINT(!buttonAction->objectName().isEmpty(), "Action's object name is empty", NULL);

    if (position == -1) {
        toolBar->addAction(buttonAction);
    } else {
        QAction* before = toolBar->actions().at(position);
        if (before != NULL) {
            toolBar->insertAction(before, buttonAction);
        } else {
            toolBar->addAction(buttonAction);
        }
    }
    QToolButton* button = qobject_cast<QToolButton*>(toolBar->widgetForAction(buttonAction));
    button->setFixedHeight(ADV_HEADER_HEIGHT - ADV_HEADER_TOP_BOTTOM_INDENT);

    SAFE_POINT(button, QString("ToolButton for %1 is NULL").arg(buttonAction->objectName()), NULL);
    button->setObjectName(buttonAction->objectName());
    if (buttonAction->menu() != NULL) {
        button->setPopupMode(QToolButton::InstantPopup);
    }

    return button;
}

bool ADVSingleSequenceWidget::isPanViewCollapsed() const {
    return panView->isHidden();
}

bool ADVSingleSequenceWidget::isDetViewCollapsed() const {
    return detView->isHidden();
}

bool ADVSingleSequenceWidget::isOverviewCollapsed() const {
    return overview->isHidden();
}

bool ADVSingleSequenceWidget::isViewCollapsed() const {
    return isPanViewCollapsed() && isDetViewCollapsed() && isOverviewCollapsed();
}

void ADVSingleSequenceWidget::setViewCollapsed(bool collapsed) {
    setOverviewCollapsed(collapsed);
    setPanViewCollapsed(collapsed);
    setDetViewCollapsed(collapsed);
}

void ADVSingleSequenceWidget::updateViewButtonState() {
    toggleViewAction->setText(isViewCollapsed() ? tr("Show all views") : tr("Hide all views"));
    toggleViewAction->setIcon(isViewCollapsed() ? QIcon(":core/images/show_all_views.png") : QIcon(":core/images/hide_all_views.png"));
}

void ADVSingleSequenceWidget::setPanViewCollapsed(bool collapsed) {
    if (collapsed == panView->isHidden()) {
        return;
    }

    AppContext::getSettings()->setValue(ZOOM_VIEW_COLLAPSED, QVariant(collapsed));

    panView->setHidden(collapsed);
    togglePanViewAction->setChecked(!collapsed);
    togglePanViewAction->setText(collapsed ? tr("Show zoom view") : tr("Hide zoom view"));
    updateMinMaxHeight();

    if (isPanViewCollapsed()) {
        zoomUseObject.releaseZoom();
    } else {
        zoomUseObject.useZoom();
    }
    zoomToRangeAction->setDisabled(collapsed);

    updateViewButtonState();
}

void ADVSingleSequenceWidget::setDetViewCollapsed(bool collapsed) {
    if (collapsed == detView->isHidden()) {
        return;
    }

    AppContext::getSettings()->setValue(DET_VIEW_COLLAPSED, QVariant(collapsed));

    detView->setHidden(collapsed);
    detView->setDisabledDetViewActions(collapsed);
    toggleDetViewAction->setChecked(!collapsed);
    toggleDetViewAction->setText(collapsed ? tr("Show details view") : tr("Hide details view"));
    updateMinMaxHeight();
    updateViewButtonState();
}

void ADVSingleSequenceWidget::setOverviewCollapsed(bool collapsed) {
    if (collapsed == overview->isHidden()) {
        return;
    }

    AppContext::getSettings()->setValue(OVERVIEW_COLLAPSED, QVariant(collapsed));

    overview->setHidden(collapsed);
    toggleOverviewAction->setChecked(!collapsed);
    toggleOverviewAction->setText(collapsed ? tr("Show overview") : tr("Hide overview"));
    updateMinMaxHeight();
    updateViewButtonState();
}

void ADVSingleSequenceWidget::addSequenceView(GSequenceLineView* v, QWidget* after) {
    assert(!lineViews.contains(v));
    lineViews.append(v);
    if (after==NULL) {
        linesSplitter->insertWidget(1, v);
    }
    else {
        int after_ = linesSplitter->indexOf(after);
        assert(after_!=-1);
        linesSplitter->insertWidget(after_ + 1, v);
    }
    v->setVisible(true);
    v->installEventFilter(this);
    updateMinMaxHeight();
    connect(v, SIGNAL(destroyed(QObject*)), SLOT(sl_onViewDestroyed(QObject*)));
}

void ADVSingleSequenceWidget::removeSequenceView(GSequenceLineView* v, bool deleteView) {
    assert(lineViews.contains(v));
    lineViews.removeOne(v);
    v->setVisible(false); // making widget invisible removes it from the splitter automatically
    v->disconnect(this);
    v->removeEventFilter(this);
    if (deleteView) {
        delete v;
    }
    updateMinMaxHeight();
}

U2Region ADVSingleSequenceWidget::getVisibleRange() const {
    return panView->getVisibleRange();
}

void ADVSingleSequenceWidget::setVisibleRange(const U2Region& r) {
    return panView->setVisibleRange(r);
}

int ADVSingleSequenceWidget::getNumBasesVisible() const {
    return panView->getVisibleRange().length;
}

void ADVSingleSequenceWidget::setNumBasesVisible(qint64 n) {
    panView->setNumBasesVisible(n);
}


void ADVSingleSequenceWidget::sl_onViewDestroyed(QObject* o) {
    GSequenceLineView* v = static_cast<GSequenceLineView*>(o);
    bool r = lineViews.removeOne(v);
    assert(r);
    Q_UNUSED(r);
    updateMinMaxHeight();
}

void ADVSingleSequenceWidget::centerPosition(int pos, QWidget* skipView) {
    foreach(GSequenceLineView* v, lineViews) {
        if (v == skipView) {
            continue;
        }
        v->setCenterPos(pos);
    }
}

void ADVSingleSequenceWidget::updateMinMaxHeight() {
    if (lineViews.size() == 1 && lineViews.first() == overview) {
        setMaximumHeight(ADV_HEADER_HEIGHT + lineViews.first()->minimumHeight());
    } else {
        setMaximumHeight(QWIDGETSIZE_MAX);
    }
}


void ADVSingleSequenceWidget::addZoomMenu(const QPoint& globalPos, QMenu* m) {
    GSequenceLineView* lineView = findSequenceViewByPos(globalPos);
    if (lineView == NULL) {
        return;
    }

    QAction* first = m->actions().isEmpty() ? NULL : m->actions().first();

    QAction * zoomInAction = lineView->getZoomInAction();
    QAction * zoomOutAction = lineView->getZoomOutAction();
    QAction * zoomToSelection = lineView->getZoomToSelectionAction();
    QAction * zoomToSequence = lineView->getZoomToSequenceAction();

    if (zoomInAction == NULL && zoomOutAction == NULL && zoomToSelection == NULL && zoomToSequence == NULL) {
        return;
    }

    QMenu* zm = m->addMenu(tr("Zoom"));

    if (zoomInAction!=NULL) {
        zm->insertAction(first, zoomInAction);
    }
    if (zoomOutAction!=NULL) {
        zm->insertAction(first, zoomOutAction);
    }
    if (zoomToSelection!=NULL) {
        zm->insertAction(first, zoomToSelection);
    }
    if (lineView == panView || lineView->getConherentRangeView() == panView) {
        zm->insertAction(first, zoomToRangeAction);
    }
    if (zoomToSequence!=NULL) {
        zm->insertAction(first, zoomToSequence);
    }
    zm->menuAction()->setObjectName(ADV_MENU_ZOOM);
    m->addSeparator();
}

GSequenceLineView* ADVSingleSequenceWidget::findSequenceViewByPos(const QPoint& globalPos) const {
    Q_UNUSED(globalPos);
    assert(0);
    return NULL;
}

int ADVSingleSequenceWidget::getSequenceLength() const {
    return getSequenceContext()->getSequenceLength();
}


DNATranslation* ADVSingleSequenceWidget::getComplementTT() const {
    ADVSequenceObjectContext* seqCtx = getSequenceContext();
    return seqCtx->getComplementTT();
}

DNATranslation* ADVSingleSequenceWidget::getAminoTT() const {
    ADVSequenceObjectContext* seqCtx = getSequenceContext();
    return seqCtx->getAminoTT();
}

DNASequenceSelection* ADVSingleSequenceWidget::getSequenceSelection() const {
    ADVSequenceObjectContext* seqCtx = getSequenceContext();
    return seqCtx->getSequenceSelection();
}

U2SequenceObject* ADVSingleSequenceWidget::getSequenceObject() const {
    ADVSequenceObjectContext* seqCtx = getSequenceContext();
    return seqCtx->getSequenceObject();
}

GSequenceLineView* ADVSingleSequenceWidget::getPanGSLView() const {
    return panView;
}

GSequenceLineView* ADVSingleSequenceWidget::getDetGSLView() const {
    return detView;
}

void ADVSingleSequenceWidget::buildPopupMenu(QMenu& m) {
    m.insertAction(GUIUtils::findActionAfter(m.actions(), ADV_GOTO_ACTION), getAnnotatedDNAView()->getCreateAnnotationAction());
    m.insertAction(GUIUtils::findActionAfter(m.actions(), ADV_GOTO_ACTION), selectRangeAction1);

    addSelectMenu(m);

    if (panView->isVisible()) {
        addRulersMenu(m);
    }

    ADVSequenceWidget::buildPopupMenu(m);
    foreach(GSequenceLineView* v, lineViews) {
        v->buildPopupMenu(m);
    }
}

void ADVSingleSequenceWidget::addSelectMenu(QMenu& m) {
    QMenu* selectMenu = new QMenu(tr("Select"),  &m);
    selectMenu->menuAction()->setObjectName("Select");

    selectMenu->addAction(selectRangeAction2);
    selectMenu->addAction(selectInAnnotationRangeAction);
    selectMenu->addAction(selectOutAnnotationRangeAction);

    QAction* aBefore = GUIUtils::findActionAfter(m.actions(), ADV_MENU_COPY);
    m.insertMenu(aBefore, selectMenu);
}

void ADVSingleSequenceWidget::addRulersMenu(QMenu& m) {
    qDeleteAll(rulerActions.qlist);
    rulerActions.qlist.clear();

    QMenu* rulersM = new QMenu(tr("Rulers..."),  &m);
    rulersM->menuAction()->setObjectName("Rulers");
    rulersM->setIcon(QIcon(":core/images/ruler.png"));

    rulersM->addAction(createNewRulerAction);
    rulersM->addSeparator();
    rulersM->addAction(panView->getToggleMainRulerAction());
    rulersM->addAction(panView->getToggleCustomRulersAction());
    rulersM->addSeparator();

    foreach(const RulerInfo& ri, panView->getCustomRulers()) {
        QAction* rulerAction = new QAction(tr("Remove '%1'").arg(ri.name), this);
        rulerAction->setData(ri.name);
        connect(rulerAction, SIGNAL(triggered()), SLOT(sl_removeCustomRuler()));
        rulerActions.qlist.append(rulerAction);
    }
    rulersM->addActions(rulerActions.qlist);

    QAction* aBefore = GUIUtils::findActionAfter(m.actions(), ADV_MENU_SECTION2_SEP);
    m.insertMenu(aBefore, rulersM);
    m.insertSeparator(aBefore)->setObjectName("SECOND_SEP");
}

bool ADVSingleSequenceWidget::isWidgetOnlyObject(GObject* o) const {
    foreach(GSequenceLineView* v, lineViews) {
        SequenceObjectContext *ctx = v->getSequenceContext();
        if(ctx->getSequenceGObject() == o) {
            return true;
        }
    }
    return false;
}


bool ADVSingleSequenceWidget::eventFilter(QObject* o, QEvent* e) {
    QEvent::Type t = e->type();
    if (t == QEvent::Resize) {
        GSequenceLineView* v = qobject_cast<GSequenceLineView*>(o);
        if (lineViews.contains(v)) {
            updateMinMaxHeight();
        }
    } else if (t == QEvent::FocusIn || t == QEvent::MouseButtonPress || t == QEvent::MouseButtonRelease) {
        ctx->setFocusedSequenceWidget(this);
    }

    if (o == headerWidget && t == QEvent::MouseButtonPress) {
        QMouseEvent* event = dynamic_cast<QMouseEvent*>(e);
        CHECK(event, false);
        if (event->buttons() == Qt::LeftButton) {
            emit si_titleClicked(this);
        }
    }
    return false;
}

void ADVSingleSequenceWidget::sl_onLocalCenteringRequest(qint64 pos) {
    detView->setCenterPos(pos);
}

void ADVSingleSequenceWidget::addADVSequenceWidgetAction(ADVSequenceWidgetAction* a) {
    ADVSequenceWidget::addADVSequenceWidgetAction(a);
    if (a->addToBar) {
        QToolBar* tb = headerWidget->getStandardToolBar();
        addButtonWithActionToToolbar(a, tb, 0);
        buttonTabOrederedNames->prepend(a->objectName());
    }
}

void ADVSingleSequenceWidget::addADVSequenceWidgetActionToViewsToolbar(ADVSequenceWidgetAction *a) {
    ADVSequenceWidget::addADVSequenceWidgetAction(a);
    if (a->addToBar) {
        QToolBar* tb = headerWidget->getViewsToolBar();
        addButtonWithActionToToolbar(a, tb, 1);
    }
}

void ADVSingleSequenceWidget::sl_onSelectRange() {
    ADVSequenceObjectContext* ctx = getSequenceContext();
    DNASequenceSelection* selection = ctx->getSequenceSelection();

    const QVector<U2Region>& seqRegions = selection->getSelectedRegions();

    QObjectScopedPointer<MultipleRangeSelector> mrs = new MultipleRangeSelector(this, seqRegions, ctx->getSequenceLength(), ctx->getSequenceObject()->isCircular());
    mrs->exec();
    CHECK(!mrs.isNull(), );

    if (mrs->result() == QDialog::Accepted) {
        QVector<U2Region> curRegions = mrs->getSelectedRegions();
        if(curRegions.isEmpty()){
            return;
        }
        if(curRegions.size() == 1) {
            U2Region r = curRegions.first();
            setSelectedRegion(r);
            if (!detView->getVisibleRange().intersects(r)) {
                detView->setCenterPos(r.startPos);
            }
        } else {
            getSequenceContext()->getSequenceSelection()->setSelectedRegions(curRegions);
        }
    }
}

QVector<U2Region> ADVSingleSequenceWidget::getSelectedAnnotationRegions(int max) {
    ADVSequenceObjectContext* seqCtx = getSequenceContext();
    const QList<Annotation*> selection = seqCtx->getAnnotatedDNAView()->getAnnotationsSelection()->getAnnotations();
    const QSet<AnnotationTableObject *> myAnns = seqCtx->getAnnotationObjects(true);

    QVector<U2Region> res;
    foreach(const Annotation* annotation, selection) {
        AnnotationTableObject *aObj = annotation->getGObject();
        if (myAnns.contains(aObj)) {
            res << U2Region::containingRegion(annotation->getRegions());
            if (max > 0 && res.size() >= max) {
                break;
            }
        }
    }
    return res;
}

void ADVSingleSequenceWidget::sl_onSelectInRange() {
    QVector<U2Region> selRegs = getSelectedAnnotationRegions(3);
    assert(selRegs.size() == 2);

    const U2Region& r1 = selRegs.at(0);
    const U2Region& r2 = selRegs.at(1);
    assert(!r1.intersects(r2));

    U2Region r;
    r.startPos = qMin(r1.endPos(), r2.endPos());
    r.length = qMax(r1.startPos, r2.startPos) - r.startPos;

    setSelectedRegion(r);
}

void ADVSingleSequenceWidget::sl_onSelectOutRange() {
    QVector<U2Region> selRegs = getSelectedAnnotationRegions(0);
    assert(!selRegs.isEmpty());
    U2Region r = U2Region::containingRegion(selRegs);

    setSelectedRegion(r);
}

void ADVSingleSequenceWidget::setSelectedRegion(const U2Region& region)
{
    getSequenceContext()->getSequenceSelection()->setRegion(region);

}

void ADVSingleSequenceWidget::sl_zoomToRange() {
    DNASequenceSelection* sel = getSequenceSelection();
    int start = getVisibleRange().startPos + 1;
    int end = getVisibleRange().endPos();
    if (!sel->isEmpty()) {
        const QVector<U2Region>& regions = sel->getSelectedRegions();
        start = regions.first().startPos+1;
        end = regions.first().endPos();
    }

    QObjectScopedPointer<QDialog> dlg = new QDialog(this);
    dlg->setModal(true);
    dlg->setWindowTitle(tr("Zoom to range"));

    RangeSelector* rs = new RangeSelector(dlg.data(), start, end, getSequenceLength(), true);

    const int rc = dlg->exec();
    CHECK(!dlg.isNull(), );

    if (rc == QDialog::Accepted) {
        U2Region r(rs->getStart() - 1, rs->getEnd() - rs->getStart() + 1);
        panView->setVisibleRange(r);
        detView->setStartPos(r.startPos);
    }
}

#define SPLITTER_STATE_MAP_NAME "ADVSI_MAP"
#define PAN_REG_NAME            "PAN_REG"
#define DET_POS_NAME            "DET_POS"
#define OVERVIEW_VISIBLE        "OVERVIEW_VISIBLE"
#define PAN_VISIBLE             "PAN_VISIBLE"
#define DET_VISIBLE             "DET_VISIBLE"
#define MAIN_RULER_VISIBLE      "MAINR_VISIBLE"
#define CUSTOM_RULERS_VISIBLE   "CUSTOMR_VISIBLE"
#define CUSTOM_R_NAMES          "CUSTOMR_NAMES"
#define CUSTOM_R_COLORS         "CUSTOMR_COLORS"
#define CUSTOM_R_OFFSETS        "CUSTOMR_OFFSETS"
#define SEQUENCE_GRAPH_NAME     "GRAPH_NAME"
#define GRAPH_LABELS_POSITIONS  "LABELS_POSITIONS"

void ADVSingleSequenceWidget::updateState(const QVariantMap& m) {
    QVariantMap map = m.value(SPLITTER_STATE_MAP_NAME).toMap();
    QString sequenceInProjectId = getActiveSequenceContext()->getSequenceObject()->getGHints()->get(GObjectHint_InProjectId).toString();
    QVariantMap myData = map.value(sequenceInProjectId).toMap();
    U2Region panReg = myData.value(PAN_REG_NAME).value<U2Region>();
    int detPos = myData.value(DET_POS_NAME).toInt();

    U2Region seqRange(0, getActiveSequenceContext()->getSequenceLength());
    if (seqRange.contains(detPos)) {
        detView->setStartPos(detPos);
    }
    if (!panReg.isEmpty() && seqRange.contains(panReg)) {
        panView->setVisibleRange(panReg);
    }

    bool overIsVisible = myData.value(OVERVIEW_VISIBLE, true).toBool();
    setOverviewCollapsed(!overIsVisible);

    bool panIsVisible = myData.value(PAN_VISIBLE, true).toBool();
    setPanViewCollapsed(!panIsVisible);

    bool detIsVisible = myData.value(DET_VISIBLE, true).toBool();
    setDetViewCollapsed(!detIsVisible);

    bool mainRulerVisible = myData.value(MAIN_RULER_VISIBLE, true).toBool();
    panView->getToggleMainRulerAction()->setChecked(mainRulerVisible);

    bool customRulersVisible = myData.value(CUSTOM_RULERS_VISIBLE, true).toBool();
    panView->getToggleCustomRulersAction()->setChecked(customRulersVisible);

    QStringList rnames = myData[CUSTOM_R_NAMES].toStringList();
    QList<QVariant> rcolors = myData[CUSTOM_R_COLORS].toList();
    QList<QVariant> roffsets = myData[CUSTOM_R_OFFSETS].toList();
    if (rnames.count() == rcolors.count() && rnames.count() == roffsets.count()) {
        panView->removeAllCustomRulers();
        for (int i=0; i< rnames.count(); i++) {
            QString name = rnames[i];
            int offset = roffsets[i].toInt();
            QColor color = rcolors[i].value<QColor>();
            panView->addCustomRuler(RulerInfo(name, offset, color));
        }
    }
    QStringList graphNames = myData[SEQUENCE_GRAPH_NAME].toStringList();

    emit si_updateGraphView(graphNames, myData);
}

void ADVSingleSequenceWidget::saveState(QVariantMap& m) {
    QVariantMap map = m.value(SPLITTER_STATE_MAP_NAME).toMap();

    QVariantMap myData;
    myData[PAN_REG_NAME] = QVariant::fromValue<U2Region>(panView->getVisibleRange());
    myData[DET_POS_NAME] = QVariant::fromValue<int>(detView->getVisibleRange().startPos);
    myData[OVERVIEW_VISIBLE] = !isOverviewCollapsed();
    myData[PAN_VISIBLE] = !isPanViewCollapsed();
    myData[DET_VISIBLE] = !isDetViewCollapsed();
    myData[MAIN_RULER_VISIBLE] = panView->getToggleMainRulerAction()->isChecked();
    myData[CUSTOM_RULERS_VISIBLE] = panView->getToggleCustomRulersAction()->isChecked();

    QStringList rnames;
    QList<QVariant> roffsets;
    QList<QVariant> rcolors;
    foreach(const RulerInfo& ri, panView->getCustomRulers()) {
        rnames.append(ri.name);
        roffsets.append(ri.offset);
        rcolors.append(ri.color);
    }
    myData[CUSTOM_R_NAMES] = rnames;
    myData[CUSTOM_R_OFFSETS] = roffsets;
    myData[CUSTOM_R_COLORS] = rcolors;

    QStringList graphNames;
    QList<QVariant> positions;
    foreach(GSequenceLineView *view, lineViews) {
        QList<QVariant> positions;
        GSequenceGraphView *graphView = dynamic_cast<GSequenceGraphView *>(view);
        if(NULL != graphView) {
            graphNames.append(graphView->getGraphViewName());
            graphView->getLabelPositions(positions);
            myData[graphView->getGraphViewName()] = positions;
        }
    }
    myData[SEQUENCE_GRAPH_NAME] = graphNames;
    myData[GRAPH_LABELS_POSITIONS] = positions;


    QString sequenceInProjectId = getActiveSequenceContext()->getSequenceObject()->getGHints()->get(GObjectHint_InProjectId).toString();
    map[sequenceInProjectId] = myData;
    m[SPLITTER_STATE_MAP_NAME] = map;

}

// QT 4.5.0 bug workaround
void ADVSingleSequenceWidget::sl_closeView() {
    closeView();
}

void ADVSingleSequenceWidget::sl_saveScreenshot() {
    if (linesLayout->count() + linesSplitter->count() < 2) {
        return;
    }

    SingleSequenceImageExportController controller(this);

    QString fileName = GUrlUtils::fixFileName(getSequenceObject()->getGObjectName());
    QWidget *p = (QWidget*)AppContext::getMainWindow()->getQMainWindow();
    QObjectScopedPointer<ExportImageDialog> dialog = new ExportImageDialog(&controller, ExportImageDialog::SequenceView, fileName, ExportImageDialog::NoScaling, p);

    dialog->exec();
}

void ADVSingleSequenceWidget::closeView() {
    U2SequenceObject* dnaObj = getSequenceObject();
    AnnotatedDNAView* v = getAnnotatedDNAView();
    v->removeObject(dnaObj);
}

void ADVSingleSequenceWidget::sl_createCustomRuler() {
    QSet<QString> namesToFilter;
    foreach (const RulerInfo &ri, panView->getCustomRulers()) {
        namesToFilter.insert(ri.name);
    }

    int offset = panView->getVisibleRange().center();

    AnnotationSelection *annSelection = getDetGSLView()->getSequenceContext()->getAnnotationsSelection();
    U2SequenceObject *seqObj = getSequenceObject();
    int annOffset = INT_MAX;
    foreach (const Annotation* ann, annSelection->getAnnotations()) {
        AnnotationTableObject *annObj = ann->getGObject();
        if(!annObj->hasObjectRelation(seqObj, ObjectRole_Sequence)) {
            continue;
        }

        // find minimum of start positions of selected annotations
        foreach (const U2Region &region, ann->getRegions()) {
            annOffset = annOffset > region.startPos ? region.startPos : annOffset;
        }
    }
    if (annOffset != INT_MAX) {
        offset = annOffset;
    }

    QVector<U2Region> selection = getSequenceSelection()->getSelectedRegions();
    if(!selection.isEmpty()) {
        offset = selection.first().startPos;
    }

    QObjectScopedPointer<CreateRulerDialogController> d = new CreateRulerDialogController(namesToFilter, offset);
    const int rc = d->exec();
    CHECK(!d.isNull(), );

    if (rc != QDialog::Accepted) {
        return;
    }
    RulerInfo ri(d->name, d->offset, d->color);
    panView->addCustomRuler(ri);
}

void ADVSingleSequenceWidget::sl_removeCustomRuler() {
    QString rulerName = qobject_cast<QAction*>(sender())->data().toString();
    panView->removeCustomRuler(rulerName);
}

void ADVSingleSequenceWidget::sl_onAnnotationSelectionChanged(AnnotationSelection*, const QList<Annotation *>&, const QList<Annotation *>&) {
    updateSelectionActions();
}

void ADVSingleSequenceWidget::updateSelectionActions() {
    QVector<U2Region> selRegs = getSelectedAnnotationRegions(3);

    selectInAnnotationRangeAction->setEnabled(selRegs.size() == 2 && !selRegs[0].intersects(selRegs[1]));
    selectOutAnnotationRangeAction->setEnabled(!selRegs.isEmpty());
}

void ADVSingleSequenceWidget::sl_toggleAllSubViews(){
    setViewCollapsed(!isViewCollapsed());
}

void ADVSingleSequenceWidget::sl_togglePanView(bool checked) {
    setPanViewCollapsed(!checked);
}

void ADVSingleSequenceWidget::sl_toggleDetView(bool checked) {
    setDetViewCollapsed(!checked);
}

void ADVSingleSequenceWidget::sl_toggleOverview(bool checked) {
    setOverviewCollapsed(!checked);
}

void ADVSingleSequenceWidget::onSequenceObjectRenamed(const QString&) {
    headerWidget->updateTitle();
}

//////////////////////////////////////////////////////////////////////////
// header

ADVSingleSequenceHeaderWidget::ADVSingleSequenceHeaderWidget(ADVSingleSequenceWidget* p) : QWidget(p), ctx(p) {
    setFixedHeight(ADV_HEADER_HEIGHT);
    setBackgroundRole(QPalette::Window);
    setAutoFillBackground(true);

    connect(ctx->getAnnotatedDNAView(), SIGNAL(si_focusChanged(ADVSequenceWidget*, ADVSequenceWidget*)),
                                        SLOT(sl_advFocusChanged(ADVSequenceWidget*, ADVSequenceWidget*)));

    //TODO: track focus events (mouse clicks) on toolbar in disabled state and on disabled buttons !!!

    QHBoxLayout* l = new QHBoxLayout();
    l->setSpacing(4);
    l->setContentsMargins(5, 1, 0, 2);

    U2SequenceObject* seqObj = ctx->getSequenceObject();
    QString objName = seqObj->getGObjectName();
    pixLabel= new QLabel(this);
    QFont f = pixLabel->font();
    if (f.pixelSize() > ADV_HEADER_HEIGHT) {
        f.setPixelSize(ADV_HEADER_HEIGHT-8);
    }
    QIcon objIcon(":/core/images/gobject.png");
    QPixmap pix = objIcon.pixmap(QSize(16, 16), QIcon::Active);
    pixLabel->setPixmap(pix);
    pixLabel->setFont(f);
    QString objInfoTip = "<i>" + objName  + "</i>"
        + "<br>" + tr("Alphabet: <b>%1</b>").arg(seqObj->getAlphabet()->getName())
        + "<br>" + tr(" Sequence size: <b>%1</b>").arg(seqObj->getSequenceLength())
        + "<br>" + tr(" File:&nbsp;<b>%1</b>").arg(seqObj->getDocument()->getURLString());
    pixLabel->setToolTip(objInfoTip);
    pixLabel->installEventFilter(this);

    int labelWidth = 50;
    QFontMetrics fm(f, this);
    nameLabel = new QLabel("", this);
    updateTitle();
    nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    nameLabel->setMinimumWidth(labelWidth);
    nameLabel->setMaximumWidth(fm.width(nameLabel->text()));
    nameLabel->setFont(f);
    nameLabel->setToolTip(objInfoTip);
    nameLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    nameLabel->setObjectName("nameLabel");

    standardToolBar = new OrderedToolbar(this);
    standardToolBar->setObjectName("tool_bar_" + ctx->getSequenceObject()->getGObjectName());
    standardToolBar->setMovable(true);
    standardToolBar->setStyleSheet(QString("QToolBar {spacing: %1px; margin: 0px; }").arg(ADV_HEADER_TOOLBAR_SPACING));
    standardToolBar->setFixedHeight(ADV_HEADER_HEIGHT);

    viewsToolBar = new OrderedToolbar(this);
    viewsToolBar->setObjectName("views_tool_bar_" + ctx->getSequenceObject()->getGObjectName());
    viewsToolBar->setStyleSheet(QString("QToolBar {spacing: %1px; margin: 0px; }").arg(ADV_HEADER_TOOLBAR_SPACING));
    viewsToolBar->setFixedHeight(ADV_HEADER_HEIGHT);

    setLayout(l);

    l->addWidget(pixLabel);
    l->addWidget(nameLabel);
    l->addStretch();
    l->addWidget(standardToolBar);
    l->addWidget(viewsToolBar);

    connect(standardToolBar, SIGNAL(actionTriggered(QAction*)), SLOT(sl_actionTriggered(QAction*)));
    connect(viewsToolBar,    SIGNAL(actionTriggered(QAction*)), SLOT(sl_actionTriggered(QAction*)));

    updateActiveState();

}

void ADVSingleSequenceHeaderWidget::updateTitle() {
    U2SequenceObject* seqObj = ctx->getSequenceObject();
    QString newTitle = seqObj->getGObjectName() + " [" + getShortAlphabetName(seqObj->getAlphabet()) +"]";
    setTitle(newTitle);
}

void ADVSingleSequenceHeaderWidget::sl_actionTriggered(QAction* a){
    Q_UNUSED(a);
    ctx->getAnnotatedDNAView()->setFocusedSequenceWidget(ctx);
}

void ADVSingleSequenceHeaderWidget::sl_advFocusChanged(ADVSequenceWidget* prevFocus, ADVSequenceWidget* newFocus) {
    if (prevFocus == ctx || newFocus == ctx) {
        update();
        updateActiveState();
    }
}
void ADVSingleSequenceHeaderWidget::updateActiveState() {
    bool focused = ctx->getAnnotatedDNAView()->getSequenceWidgetInFocus() == ctx;
    nameLabel->setEnabled(focused);
    pixLabel->setEnabled(focused);
    ctx->getSelectRangeAction()->setShortcutContext(focused ? Qt::WindowShortcut : Qt::WidgetShortcut);
    //toolBar->setEnabled(focused); TODO: click on disabled buttons does not switch focus!
}

void ADVSingleSequenceHeaderWidget::mouseDoubleClickEvent(QMouseEvent *e) {
    ctx->toggleViewAction->trigger();
    QWidget::mouseDoubleClickEvent(e);
}

void ADVSingleSequenceHeaderWidget::paintEvent(QPaintEvent *e) {
    QWidget::paintEvent(e);

    QPainter p(this);
    p.setPen(QApplication::palette().color(QPalette::Dark));
    p.drawLine(0, height()-1, width(), height()-1);
}

QString ADVSingleSequenceHeaderWidget::getShortAlphabetName(const DNAAlphabet* al) {
    DNAAlphabetType type = al->getType();
    if (type == DNAAlphabet_RAW) {
        return tr("raw");
    }
    if (type == DNAAlphabet_AMINO) {
        if (al->getId() == BaseDNAAlphabetIds::AMINO_EXTENDED()) {
            return tr("amino ext");
        }
        return tr("amino");
    }
    assert(type == DNAAlphabet_NUCL);
    QString id = al->getId();
    if (id == BaseDNAAlphabetIds::NUCL_DNA_DEFAULT()) {
        return tr("dna");
    } else if (id == BaseDNAAlphabetIds::NUCL_DNA_EXTENDED()) {
        return tr("dna ext");
    } else if (id == BaseDNAAlphabetIds::NUCL_RNA_DEFAULT()) {
        return tr("rna");
    } else if (id == BaseDNAAlphabetIds::NUCL_RNA_EXTENDED()) {
        return tr("rna ext");
    }
    return "?";
}

}//namespace


