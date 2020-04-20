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

#include "ADVSyncViewManager.h"
#include "AnnotatedDNAView.h"
#include "ADVSingleSequenceWidget.h"
#include "ADVSequenceObjectContext.h"
#include "AutoAnnotationUtils.h"
#include "PanView.h"

#include <U2Core/DNASequenceSelection.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/Counter.h>

namespace U2 {

ADVSyncViewManager::ADVSyncViewManager(AnnotatedDNAView* v) : QObject(v), adv(v)
{
    assert(v->getSequenceContexts().isEmpty());

    recursion = false;
    selectionRecursion = false;

    lockByStartPosAction = new QAction(tr("Lock scales: visible range start"), this);
    lockByStartPosAction->setObjectName("Lock scales: visible range start");
    connect(lockByStartPosAction, SIGNAL(triggered()), SLOT(sl_lock()));
    lockByStartPosAction->setCheckable(true);

    lockBySeqSelAction = new QAction(tr("Lock scales: selected sequence"), this);
    lockBySeqSelAction->setObjectName("Lock scales: selected sequence");
    connect(lockBySeqSelAction, SIGNAL(triggered()), SLOT(sl_lock()));
    lockBySeqSelAction->setCheckable(true);

    lockByAnnSelAction = new QAction(tr("Lock scales: selected annotation"), this);
    lockByAnnSelAction->setObjectName("Lock scales: selected annotation");
    connect(lockByAnnSelAction, SIGNAL(triggered()), SLOT(sl_lock()));
    lockByAnnSelAction->setCheckable(true);

    lockActionGroup = new QActionGroup(this);
    lockActionGroup->addAction(lockByStartPosAction);
    lockActionGroup->addAction(lockBySeqSelAction);
    lockActionGroup->addAction(lockByAnnSelAction);
    lockActionGroup->setExclusive(true);

    syncByStartPosAction = new QAction(tr("Adjust scales: visible range start"), this);
    syncByStartPosAction->setObjectName("Adjust scales: visible range start");
    connect(syncByStartPosAction, SIGNAL(triggered()), SLOT(sl_sync()));

    syncBySeqSelAction = new QAction(tr("Adjust scales: selected sequence"), this);
    syncBySeqSelAction->setObjectName("Adjust scales: selected sequence");
    connect(syncBySeqSelAction, SIGNAL(triggered()), SLOT(sl_sync()));

    syncByAnnSelAction = new QAction(tr("Adjust scales: selected annotation"), this);
    syncByAnnSelAction->setObjectName("Adjust scales: selected annotation");
    connect(syncByAnnSelAction, SIGNAL(triggered()), SLOT(sl_sync()));

    lockMenu = new QMenu(tr("Lock scales"));
    lockMenu->setIcon(QIcon(":core/images/lock_scales.png"));
    lockMenu->addActions(lockActionGroup->actions());

    syncMenu = new QMenu(tr("Adjust scales"));
    syncMenu->setIcon(QIcon(":core/images/sync_scales.png"));
    syncMenu->addAction(syncByStartPosAction);
    syncMenu->addAction(syncBySeqSelAction);
    syncMenu->addAction(syncByAnnSelAction);

    lockButton = new QToolButton();
    lockButton->setObjectName("Lock scales");
    lockButton->setCheckable(true);
    connect(lockButton, SIGNAL(clicked()), SLOT(sl_lock()));
    lockButton->setDefaultAction(lockMenu->menuAction());
    lockButton->setCheckable(true);

    syncButton = new QToolButton();
    syncButton->setObjectName("Adjust scales");
    connect(syncButton, SIGNAL(clicked()), SLOT(sl_sync()));
    syncButton->setDefaultAction(syncMenu->menuAction());

    lockButtonTBAction = NULL;
    syncButtonTBAction = NULL;

    // auto-annotations highlighting ops

    toggleAutoAnnotationsMenu = new QMenu("Global automatic annotation highlighting");
    toggleAutoAnnotationsMenu->setIcon(QIcon(":core/images/predefined_annotation_groups.png"));
    connect( toggleAutoAnnotationsMenu, SIGNAL(aboutToShow()), SLOT(sl_updateAutoAnnotationsMenu()) );

    toggleAutoAnnotationsButton = new QToolButton();
    toggleAutoAnnotationsButton->setObjectName("toggleAutoAnnotationsButton");
    toggleAutoAnnotationsButton->setDefaultAction(toggleAutoAnnotationsMenu->menuAction());
    toggleAutoAnnotationsButton->setPopupMode(QToolButton::InstantPopup);

    toggleAutoAnnotationsAction = NULL;

    // visual mode ops
    toggleAllAction = new QAction("Toggle All sequence views", this);
    toggleAllAction->setObjectName("toggleAllSequenceViews");
    connect(toggleAllAction, SIGNAL(triggered()), SLOT(sl_toggleVisualMode()));

    toggleOveAction = new QAction("Toggle Overview", this);
    toggleOveAction->setObjectName("toggleOverview");
    connect(toggleOveAction, SIGNAL(triggered()), SLOT(sl_toggleVisualMode()));

    togglePanAction = new QAction("Toggle Zoom view", this);
    togglePanAction->setObjectName("toggleZoomView");
    connect(togglePanAction, SIGNAL(triggered()), SLOT(sl_toggleVisualMode()));

    toggleDetAction = new QAction("Toggle Details view", this);
    toggleDetAction->setObjectName("toggleDetailsView");
    connect(toggleDetAction, SIGNAL(triggered()), SLOT(sl_toggleVisualMode()));

    toggleViewButtonAction = NULL;
    toggleViewButtonMenu =  new QMenu(tr("Toggle views"));
    toggleViewButtonMenu->setIcon(QIcon(":core/images/adv_widget_menu.png"));

    toggleViewButtonMenu->addAction(toggleAllAction); //-> behavior can be not clear to user
    toggleViewButtonMenu->addAction(toggleOveAction);
    toggleViewButtonMenu->addAction(togglePanAction);
    toggleViewButtonMenu->addAction(toggleDetAction);
    connect(toggleViewButtonMenu, SIGNAL(aboutToShow()), SLOT(sl_updateVisualMode()));

    toggleViewButton = new QToolButton();
    toggleViewButton->setObjectName("toggleViewButton");
    toggleViewButton->setDefaultAction(toggleViewButtonMenu->menuAction());
    toggleViewButton->setPopupMode(QToolButton::InstantPopup);

    updateEnabledState();

    connect(adv, SIGNAL(si_sequenceWidgetAdded(ADVSequenceWidget*)), SLOT(sl_sequenceWidgetAdded(ADVSequenceWidget*)));
    connect(adv, SIGNAL(si_sequenceWidgetRemoved(ADVSequenceWidget*)), SLOT(sl_sequenceWidgetRemoved(ADVSequenceWidget*)));
}

ADVSyncViewManager::~ADVSyncViewManager() {
    delete lockButton;
    delete syncButton;
    delete syncMenu;
    delete lockMenu;

    delete toggleAutoAnnotationsButton;
    delete toggleAutoAnnotationsMenu;

    delete toggleViewButton;
    delete toggleViewButtonMenu;
}

void ADVSyncViewManager::updateToolbar1(QToolBar* tb) {
    if (lockButtonTBAction == NULL) {
        lockButtonTBAction = tb->addWidget(lockButton);
        syncButtonTBAction = tb->addWidget(syncButton);
    } else {
        tb->addAction(lockButtonTBAction);
        tb->addAction(syncButtonTBAction);
    }
}

void ADVSyncViewManager::updateToolbar2(QToolBar* tb) {

    if (toggleAutoAnnotationsAction == NULL ) {
        updateAutoAnnotationActions();
        toggleAutoAnnotationsAction = tb->addWidget(toggleAutoAnnotationsButton);
    } else {
        tb->addAction(toggleAutoAnnotationsAction);
    }

    if (toggleViewButtonAction == NULL) {
        toggleViewButtonAction = tb->addWidget(toggleViewButton);
    } else {
        tb->addAction(toggleViewButtonAction);
    }
}

void ADVSyncViewManager::updateEnabledState() {
    bool enabled = getViewsFromADV().size() > 1;
    syncButton->setEnabled(enabled);
    lockButton->setEnabled(enabled);
}

void ADVSyncViewManager::sl_sequenceWidgetAdded(ADVSequenceWidget* w) {
    ADVSingleSequenceWidget* sw = qobject_cast<ADVSingleSequenceWidget*>(w);
    if (sw == NULL) {
        return;
    }
    unlock();
    if (toggleAutoAnnotationsAction != NULL) {
        updateAutoAnnotationActions();
    }
}

void ADVSyncViewManager::sl_sequenceWidgetRemoved(ADVSequenceWidget* w) {
    ADVSingleSequenceWidget* sw = qobject_cast<ADVSingleSequenceWidget*>(w);
    if (sw == NULL) {
        return;
    }
    unlock();
    updateAutoAnnotationActions();
}

void ADVSyncViewManager::unlock() {
    foreach(ADVSingleSequenceWidget* sw, views) {
        sw->getPanView()->disconnect(this);
        sw->getSequenceSelection()->disconnect(this);
    }
    views.clear();
    updateEnabledState();
}

QList<ADVSingleSequenceWidget*> ADVSyncViewManager::getViewsFromADV() const {
    QList<ADVSingleSequenceWidget*> res;
    foreach(ADVSequenceWidget* w, adv->getSequenceWidgets()) {
        ADVSingleSequenceWidget* sw = qobject_cast<ADVSingleSequenceWidget*>(w);
        if (sw != NULL) {
            res.append(sw);
        }
    }
    return res;
}

void ADVSyncViewManager::sl_rangeChanged() {
    if (recursion) {
        return;
    }
    recursion = true;

    PanView* activePan = qobject_cast<PanView*>(sender());
    const U2Region& activeRange = activePan->getVisibleRange();
    int activeOffset = activePan->getSyncOffset();
    foreach(ADVSingleSequenceWidget* sw, views) {
        PanView* pan = sw->getPanView();
        if (pan == activePan) {
            continue;
        }
        int panOffset = pan->getSyncOffset();
        int resultOffset = panOffset - activeOffset;
        qint64 seqLen = pan->getSequenceLength();
        qint64 newStart = qBound(qint64(0), activeRange.startPos + resultOffset, seqLen);
        qint64 nVisible = qMin(activeRange.length, seqLen);
        if (newStart + nVisible > seqLen) {
            newStart = seqLen - nVisible;
        }
        assert(newStart >= 0 && newStart + nVisible <= seqLen);
        pan->setVisibleRange(U2Region(newStart, nVisible));
    }

    recursion = false;
}

void ADVSyncViewManager::sl_lock() {
    GCOUNTER(tvar, cvar, "SequenceView::SyncViewManager::Lock scales");
    QObject* s = sender();
    bool buttonClicked = (s == lockButton);

    SyncMode m = SyncMode_Start;
    if (lockButton->isChecked()) {
        unlock();
    } else {
        if (s == lockBySeqSelAction) {
            m = SyncMode_SeqSel;
        } else if (s == lockByAnnSelAction) {
            m = SyncMode_AnnSel;
        } else if (s == lockButton) {
            m = detectSyncMode();
        }
        sync(true, m);
    }

    if (buttonClicked) {
        QAction* checkedAction = lockActionGroup->checkedAction();
        if (NULL == checkedAction) {
            toggleCheckedAction(m);
        } else {
            checkedAction->toggle();
        }
        lockButton->toggle();
    } else {
        lockButton->setChecked(lockActionGroup->checkedAction() != NULL);
    }

}

void ADVSyncViewManager::sl_sync() {
    GCOUNTER(tvar, cvar, "SequenceView::SyncViewManager::Adjust scales");
    QObject* s = sender();
    SyncMode m = SyncMode_Start;
    if (s == syncBySeqSelAction) {
        m = SyncMode_SeqSel;
    } else if (s == syncByAnnSelAction) {
        m = SyncMode_AnnSel;
    } else if (s == syncButton) {
        m = detectSyncMode();
    }
    sync(false, m);
}

void ADVSyncViewManager::sync(bool lock, SyncMode m) {
    ADVSingleSequenceWidget* focusedW  = qobject_cast<ADVSingleSequenceWidget*>(adv->getSequenceWidgetInFocus());
    if (focusedW == NULL) {
        return;
    }

    QList<ADVSingleSequenceWidget*> seqs = getViewsFromADV();
    QVector<int> offsets(seqs.size());

    //offset here ==> new panview start pos
    //dOffset is used to keep focused sequence unchanged
    U2Region focusedRange;
    int dOffset = 0;
    for (int i=0; i< seqs.size(); i++ ){
        int offset = 0;
        ADVSingleSequenceWidget* seqW = seqs[i];
        switch(m) {
            case SyncMode_Start:  offset = seqW->getVisibleRange().startPos; break;
            case SyncMode_SeqSel: offset = offsetBySeqSel(seqW);break;
            case SyncMode_AnnSel: offset = offsetByAnnSel(seqW);break;
        }
        offsets[i] = offset;
        if (seqW == focusedW) {
            focusedRange = focusedW->getVisibleRange();
            dOffset = offset - focusedRange.startPos;
        }
    }
    assert(!focusedRange.isEmpty());
    for (int i=0; i< seqs.size(); i++ ){
        ADVSingleSequenceWidget* seqW = seqs[i];
        int offset = offsets[i] - dOffset;
        PanView* pan = seqW->getPanView();
        if (seqW != focusedW) {
            pan->setNumBasesVisible(focusedRange.length);
            pan->setStartPos(offset);
        }
        if (lock) {
            DNASequenceSelection* selection = seqW->getSequenceContext()->getSequenceSelection();
            connect(selection,
                SIGNAL(si_selectionChanged(LRegionsSelection*, const QVector<U2Region>&, const QVector<U2Region>& )),
                SLOT(sl_onSelectionChanged(LRegionsSelection*, const QVector<U2Region>&, const QVector<U2Region>& )) );
            pan->setSyncOffset(offset);
            connect(pan, SIGNAL(si_visibleRangeChanged()), SLOT(sl_rangeChanged()));
            views.append(seqW);
        }
    }
}

int ADVSyncViewManager::offsetBySeqSel(ADVSingleSequenceWidget* w) const {
    DNASequenceSelection* seqSel = w->getSequenceContext()->getSequenceSelection();
    if (seqSel->isEmpty()) {
        return w->getVisibleRange().startPos;
    }
    return seqSel->getSelectedRegions().first().startPos;
}

int ADVSyncViewManager::offsetByAnnSel(ADVSingleSequenceWidget* w) const {
    int pos = findSelectedAnnotationPos(w);
    if (pos == -1) {
        return w->getVisibleRange().startPos;
    }
    return pos;
}

int ADVSyncViewManager::findSelectedAnnotationPos(ADVSingleSequenceWidget* w) const {
    AnnotationSelection* as = w->getSequenceContext()->getAnnotationsSelection();
    const QSet<AnnotationTableObject *> &objs = w->getSequenceContext()->getAnnotationObjects(true);
    foreach(const Annotation* annotation , as->getAnnotations()) {
        AnnotationTableObject *obj = annotation->getGObject();
        if (objs.contains(obj)) {
            return annotation->getStrand().isCompementary()
                ? annotation->getRegions().last().endPos() : annotation->getRegions().first().startPos;
        }
    }
    return -1;
}


ADVSyncViewManager::SyncMode ADVSyncViewManager::detectSyncMode() const {
    ADVSingleSequenceWidget* focusedW  = qobject_cast<ADVSingleSequenceWidget*>(adv->getSequenceWidgetInFocus());
    assert(focusedW != NULL);
    QList<ADVSingleSequenceWidget*> seqs = getViewsFromADV();

    //if current sequence + any other sequence have annotation selection -> sync by annotation
    if (findSelectedAnnotationPos(focusedW) != -1) {
        foreach(ADVSingleSequenceWidget* sw, seqs) {
            if (sw != focusedW && findSelectedAnnotationPos(sw) != -1) {
                return SyncMode_AnnSel;
            }
        }
    }

    //if current sequence + any other sequence have sequence selection -> sync by annotation
    if (!focusedW->getSequenceContext()->getSequenceSelection()->isEmpty()) {
        foreach(ADVSingleSequenceWidget* sw, seqs) {
            if (sw != focusedW && !sw->getSequenceContext()->getSequenceSelection()->isEmpty()) {
                return SyncMode_SeqSel;
            }
        }
    }
    // else sync by start pos
    return SyncMode_Start;

}

void ADVSyncViewManager::sl_updateVisualMode() {
    //if have at least 1 visible -> hide all
    bool haveVisiblePan = false;
    bool haveVisibleDet = false;
    bool haveVisibleView = false;
    bool haveVisibleOve = false;
    foreach(ADVSingleSequenceWidget* sw, getViewsFromADV()) {
        haveVisiblePan = haveVisiblePan || !sw->isPanViewCollapsed();
        haveVisibleDet = haveVisibleDet || !sw->isDetViewCollapsed();
        haveVisibleView = haveVisibleView || !sw->isViewCollapsed();
        haveVisibleOve = haveVisibleOve || !sw->isOverviewCollapsed();
    }
    toggleAllAction->setText(haveVisibleView ? tr("Hide all sequences") : tr("Show all sequences"));
    togglePanAction->setText(haveVisiblePan ? tr("Hide all zoom views") : tr("Show all zoom views"));
    toggleDetAction->setText(haveVisibleDet ? tr("Hide all details")  : tr("Show all details"));
    toggleOveAction->setText(haveVisibleOve ? tr("Hide all overviews") : tr("Show all overviews"));
}

void ADVSyncViewManager::sl_toggleVisualMode() {
    //if have at least 1 visible -> hide all
    bool haveVisibleNav= false;
    bool haveVisiblePan = false;
    bool haveVisibleDet = false;
    bool haveVisibleView = false;

    QList<ADVSingleSequenceWidget*> views = getViewsFromADV();
    foreach(ADVSingleSequenceWidget* sw, views) {
        haveVisibleDet = haveVisibleDet || !sw->isDetViewCollapsed();
        haveVisibleView = haveVisibleView || !sw->isViewCollapsed();
        haveVisiblePan = haveVisiblePan || !sw->isPanViewCollapsed();
        haveVisibleNav = haveVisibleNav || !sw->isOverviewCollapsed();
    }

    QObject* s = sender();
    foreach(ADVSingleSequenceWidget* sw, views) {
        if (s == toggleOveAction) {
            sw->setOverviewCollapsed(haveVisibleNav);
        } else if (s == togglePanAction) {
            sw->setPanViewCollapsed(haveVisiblePan);
        } else if (s == toggleDetAction) {
            sw->setDetViewCollapsed(haveVisibleDet);
        } else {
            sw->setViewCollapsed(haveVisibleView);
        }
    }
}

void ADVSyncViewManager::sl_onSelectionChanged( LRegionsSelection* sel, const QVector<U2Region>& added, const QVector<U2Region>& )
{
    Q_UNUSED(sel);
    if (selectionRecursion) {
        return;
    }

    selectionRecursion = true;

    ADVSingleSequenceWidget* focusedW  = qobject_cast<ADVSingleSequenceWidget*>(adv->getSequenceWidgetInFocus());
    if (focusedW == NULL) {
        return;
    }
    for( int i = 0; i < views.size(); ++i )   {
        ADVSingleSequenceWidget* w = views[i];
        if (w == focusedW) {
            continue;
        }

        int offset = focusedW->getVisibleRange().startPos - w->getVisibleRange().startPos;

        DNASequenceSelection* selection = w->getSequenceSelection();
        selection->clear();
        qint64 seqLen = w->getSequenceLength();
        foreach(U2Region r, added) {
            r.startPos -= offset;

            if (r.startPos < 0) {
                r.startPos = 0;
            }

            if (r.endPos() > seqLen) {
                r.length = seqLen - r.startPos;
            }
            if (r.length > 0) {
                selection->addRegion(r);
            }
        }

    }

    selectionRecursion = false;
}

void ADVSyncViewManager::toggleCheckedAction(SyncMode mode) {
    switch (mode) {
        case SyncMode_AnnSel:
            lockByAnnSelAction->toggle();
            break;
        case SyncMode_SeqSel:
            lockBySeqSelAction->toggle();
            break;
        default:
            lockByStartPosAction->toggle();
    }
}

void ADVSyncViewManager::updateAutoAnnotationActions() {
    aaActionMap.clear();
    toggleAutoAnnotationsMenu->clear();

    foreach (ADVSequenceWidget* w, adv->getSequenceWidgets()) {
        QList<ADVSequenceWidgetAction*> actions = w->getADVSequenceWidgetActions();
        bool active = false;
        foreach (ADVSequenceWidgetAction* action, actions) {
            AutoAnnotationsADVAction* aaAction = qobject_cast<AutoAnnotationsADVAction*>(action);
            if (aaAction != NULL) {
                QList<QAction*> aaToggleActions = aaAction->getToggleActions();
                foreach( QAction* toggleAction, aaToggleActions) {
                    if (toggleAction->isEnabled()) {
                        aaActionMap.insertMulti(toggleAction->text(), toggleAction);
                        active = true;
                    }
                }
                aaAction->setVisible(active);
            }
        }
    }

    toggleAutoAnnotationsButton->setEnabled(!aaActionMap.isEmpty());

    QSet<QString> actionNames = aaActionMap.keys().toSet();

    foreach (const QString& aName, actionNames) {
        QAction* action = new QAction(toggleAutoAnnotationsMenu);
        action->setObjectName(aName);
        connect(action, SIGNAL(triggered()), SLOT(sl_toggleAutoAnnotationHighlighting()));
        toggleAutoAnnotationsMenu->addAction(action);
    }
}

#define HAVE_ENABLED_AUTOANNOTATIONS "have_enabled_autoannotations"

void ADVSyncViewManager::sl_toggleAutoAnnotationHighlighting() {
    QAction* menuAction = qobject_cast<QAction*>( sender() );
    if (menuAction == NULL) {
        return;
    }
    QVariant val = menuAction->property(HAVE_ENABLED_AUTOANNOTATIONS);
    assert(val.isValid());
    bool haveEnabledAutoAnnotations = val.toBool();
    QList<QAction*> aaActions = aaActionMap.values(menuAction->objectName());
    foreach (QAction* aaAction, aaActions )
    {
        aaAction->setChecked(!haveEnabledAutoAnnotations);
    }

}

void ADVSyncViewManager::sl_updateAutoAnnotationsMenu() {
    QList<QAction*> menuActions = toggleAutoAnnotationsMenu->actions();

    foreach (QAction* menuAction, menuActions) {
        QString aName = menuAction->objectName();
        bool haveEnabledAutoAnnotations = false;
        //if have at least 1 checked  -> uncheck all
        QList<QAction*> aaActions = aaActionMap.values(aName);
        foreach(QAction* aaAction, aaActions ) {
            if (aaAction->isChecked()) {
                haveEnabledAutoAnnotations = true;
                break;
            }
        }

        if (haveEnabledAutoAnnotations) {
            menuAction->setText(tr("Hide %1").arg(aName));
        } else {
            menuAction->setText(tr("Show %1").arg(aName));
        }
        menuAction->setProperty(HAVE_ENABLED_AUTOANNOTATIONS,haveEnabledAutoAnnotations);
    }

}

}//namespace
