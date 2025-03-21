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

#include "ADVSyncViewManager.h"

#include <U2Core/CollectionUtils.h>
#include <U2Core/Counter.h>
#include <U2Core/DNASequenceSelection.h>

#include "ADVSequenceObjectContext.h"
#include "ADVSingleSequenceWidget.h"
#include "AnnotatedDNAView.h"
#include "AutoAnnotationUtils.h"
#include "PanView.h"

namespace U2 {

ADVSyncViewManager::ADVSyncViewManager(AnnotatedDNAView* v)
    : QObject(v), adv(v) {
    assert(v->getSequenceContexts().isEmpty());

    recursion = false;
    selectionRecursion = false;

    lockByStartPosAction = new QAction(tr("Lock scales: visible range start"), this);
    lockByStartPosAction->setObjectName("Lock scales: visible range start");
    connect(lockByStartPosAction, &QAction::triggered, this, &ADVSyncViewManager::sl_lock);
    lockByStartPosAction->setCheckable(true);

    lockBySeqSelAction = new QAction(tr("Lock scales: selected sequence"), this);
    lockBySeqSelAction->setObjectName("Lock scales: selected sequence");
    connect(lockBySeqSelAction, &QAction::triggered, this, &ADVSyncViewManager::sl_lock);
    lockBySeqSelAction->setCheckable(true);

    lockByAnnSelAction = new QAction(tr("Lock scales: selected annotation"), this);
    lockByAnnSelAction->setObjectName("Lock scales: selected annotation");
    connect(lockByAnnSelAction, &QAction::triggered, this, &ADVSyncViewManager::sl_lock);
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
    connect(lockMenu, &QMenu::aboutToShow, this, &ADVSyncViewManager::sl_setUpLockMenuActions);

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

    lockButtonTBAction = nullptr;
    syncButtonTBAction = nullptr;

    // auto-annotations highlighting ops

    toggleAutoAnnotationsMenu = new QMenu("Global automatic annotation highlighting");
    toggleAutoAnnotationsMenu->setIcon(QIcon(":core/images/predefined_annotation_groups.png"));
    connect(toggleAutoAnnotationsMenu, SIGNAL(aboutToShow()), SLOT(sl_updateAutoAnnotationsMenu()));

    toggleAutoAnnotationsButton = new QToolButton();
    toggleAutoAnnotationsButton->setObjectName("toggleAutoAnnotationsButton");
    toggleAutoAnnotationsButton->setDefaultAction(toggleAutoAnnotationsMenu->menuAction());
    toggleAutoAnnotationsButton->setPopupMode(QToolButton::InstantPopup);

    toggleAutoAnnotationsAction = nullptr;

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

    toggleViewButtonAction = nullptr;
    toggleViewButtonMenu = new QMenu(tr("Toggle views"));
    toggleViewButtonMenu->setIcon(QIcon(":core/images/adv_widget_menu.png"));

    toggleViewButtonMenu->addAction(toggleAllAction);  //-> behavior can be not clear to user
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
    if (lockButtonTBAction == nullptr) {
        lockButtonTBAction = tb->addWidget(lockButton);
        syncButtonTBAction = tb->addWidget(syncButton);
    } else {
        tb->addAction(lockButtonTBAction);
        tb->addAction(syncButtonTBAction);
    }
}

void ADVSyncViewManager::updateToolbar2(QToolBar* tb) {
    if (toggleAutoAnnotationsAction == nullptr) {
        updateAutoAnnotationActions();
        toggleAutoAnnotationsAction = tb->addWidget(toggleAutoAnnotationsButton);
    } else {
        tb->addAction(toggleAutoAnnotationsAction);
    }

    if (toggleViewButtonAction == nullptr) {
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
    auto sw = qobject_cast<ADVSingleSequenceWidget*>(w);
    if (sw == nullptr) {
        return;
    }
    unlock();
    if (toggleAutoAnnotationsAction != nullptr) {
        updateAutoAnnotationActions();
    }
}

void ADVSyncViewManager::sl_sequenceWidgetRemoved(ADVSequenceWidget* w) {
    auto sw = qobject_cast<ADVSingleSequenceWidget*>(w);
    if (sw == nullptr) {
        return;
    }
    unlock();
    updateAutoAnnotationActions();
}

void ADVSyncViewManager::unlock() {
    foreach (ADVSingleSequenceWidget* sw, views) {
        sw->getPanView()->disconnect(this);
        sw->getSequenceSelection()->disconnect(this);
    }
    views.clear();
    updateEnabledState();
}

QList<ADVSingleSequenceWidget*> ADVSyncViewManager::getViewsFromADV() const {
    QList<ADVSingleSequenceWidget*> res;
    foreach (ADVSequenceWidget* w, adv->getSequenceWidgets()) {
        auto sw = qobject_cast<ADVSingleSequenceWidget*>(w);
        if (sw != nullptr) {
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

    auto activePan = qobject_cast<PanView*>(sender());
    const U2Region& activeRange = activePan->getVisibleRange();
    int activeOffset = activePan->getSyncOffset();
    foreach (ADVSingleSequenceWidget* sw, views) {
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
    GCOUNTER(tvar, "SequenceView::SyncViewManager::Lock scales");
    QObject* s = sender();
    bool buttonClicked = (s == lockButton);

    SyncMode m = SyncMode_None;
    if (s == lockByStartPosAction) {
        m = SyncMode_Start;
    } else if (s == lockBySeqSelAction) {
        m = SyncMode_SeqSel;
    } else if (s == lockByAnnSelAction) {
        m = SyncMode_AnnSel;
    } else if (buttonClicked && !lockButton->isChecked()) {
        m = detectSyncMode();
    }
    if (lockButton->isChecked()) {
        unlock();
    } else {
        sync(true, m);
    }

    if (buttonClicked) {
        toggleCheckedAction(m);
        lockButton->toggle();
    } else {
        if (syncModeState == m) {
            lockButton->setChecked(false);
            m = SyncMode_None;
            toggleCheckedAction(m);
        }
        lockButton->setChecked(lockActionGroup->checkedAction() != nullptr);
    }
    syncModeState = m;
}

void ADVSyncViewManager::sl_sync() {
    GCOUNTER(tvar, "SequenceView::SyncViewManager::Adjust scales");
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
    auto focusedW = qobject_cast<ADVSingleSequenceWidget*>(adv->getActiveSequenceWidget());
    if (focusedW == nullptr) {
        return;
    }

    QList<ADVSingleSequenceWidget*> seqs = getViewsFromADV();
    QVector<int> offsets(seqs.size());

    // offset here ==> new panview start pos
    // dOffset is used to keep focused sequence unchanged
    U2Region focusedRange;
    int dOffset = 0;
    for (int i = 0; i < seqs.size(); i++) {
        int offset = 0;
        ADVSingleSequenceWidget* seqW = seqs[i];
        switch (m) {
            case SyncMode_Start:
                offset = seqW->getVisibleRange().startPos;
                break;
            case SyncMode_SeqSel:
                offset = offsetBySeqSel(seqW);
                break;
            case SyncMode_AnnSel:
                offset = offsetByAnnSel(seqW);
                break;
            case SyncMode_None:
                return;
        }
        offsets[i] = offset;
        if (seqW == focusedW) {
            focusedRange = focusedW->getVisibleRange();
            dOffset = offset - focusedRange.startPos;
        }
    }
    assert(!focusedRange.isEmpty());
    for (int i = 0; i < seqs.size(); i++) {
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
                    SIGNAL(si_selectionChanged(LRegionsSelection*, const QVector<U2Region>&, const QVector<U2Region>&)),
                    SLOT(sl_onSelectionChanged(LRegionsSelection*, const QVector<U2Region>&, const QVector<U2Region>&)));
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
    const QSet<AnnotationTableObject*>& objs = w->getSequenceContext()->getAnnotationObjects(true);
    foreach (const Annotation* annotation, as->getAnnotations()) {
        AnnotationTableObject* obj = annotation->getGObject();
        if (objs.contains(obj)) {
            return annotation->getStrand().isComplementary() ? annotation->getRegions().last().endPos() : annotation->getRegions().first().startPos;
        }
    }
    return -1;
}

ADVSyncViewManager::SyncMode ADVSyncViewManager::detectSyncMode() const {
    auto focusedW = qobject_cast<ADVSingleSequenceWidget*>(adv->getActiveSequenceWidget());
    assert(focusedW != nullptr);
    QList<ADVSingleSequenceWidget*> seqs = getViewsFromADV();

    // if current sequence + any other sequence have annotation selection -> sync by annotation
    if (findSelectedAnnotationPos(focusedW) != -1) {
        foreach (ADVSingleSequenceWidget* sw, seqs) {
            if (sw != focusedW && findSelectedAnnotationPos(sw) != -1) {
                return SyncMode_AnnSel;
            }
        }
    }

    // if current sequence + any other sequence have sequence selection -> sync by annotation
    if (!focusedW->getSequenceContext()->getSequenceSelection()->isEmpty()) {
        foreach (ADVSingleSequenceWidget* sw, seqs) {
            if (sw != focusedW && !sw->getSequenceContext()->getSequenceSelection()->isEmpty()) {
                return SyncMode_SeqSel;
            }
        }
    }
    // else sync by start pos
    return SyncMode_Start;
}

void ADVSyncViewManager::sl_updateVisualMode() {
    // if have at least 1 visible -> hide all
    bool haveVisiblePan = false;
    bool haveVisibleDet = false;
    bool haveVisibleView = false;
    bool haveVisibleOve = false;
    foreach (ADVSingleSequenceWidget* sw, getViewsFromADV()) {
        haveVisiblePan = haveVisiblePan || !sw->isPanViewCollapsed();
        haveVisibleDet = haveVisibleDet || !sw->isDetViewCollapsed();
        haveVisibleView = haveVisibleView || !sw->isViewCollapsed();
        haveVisibleOve = haveVisibleOve || !sw->isOverviewCollapsed();
    }
    toggleAllAction->setText(haveVisibleView ? tr("Hide all sequences") : tr("Show all sequences"));
    togglePanAction->setText(haveVisiblePan ? tr("Hide all zoom views") : tr("Show all zoom views"));
    toggleDetAction->setText(haveVisibleDet ? tr("Hide all details") : tr("Show all details"));
    toggleOveAction->setText(haveVisibleOve ? tr("Hide all overviews") : tr("Show all overviews"));
}

void ADVSyncViewManager::sl_setUpLockMenuActions() {
    auto focusedW = qobject_cast<ADVSingleSequenceWidget*>(adv->getActiveSequenceWidget());
    if (focusedW == nullptr) {
        lockByAnnSelAction->setEnabled(false);
        lockBySeqSelAction->setEnabled(false);
        return;
    }

    lockByAnnSelAction->setEnabled(findSelectedAnnotationPos(focusedW) != -1);
    lockBySeqSelAction->setEnabled(!focusedW->getSequenceContext()->getSequenceSelection()->isEmpty());
}

void ADVSyncViewManager::sl_toggleVisualMode() {
    // if have at least 1 visible -> hide all
    bool haveVisibleNav = false;
    bool haveVisiblePan = false;
    bool haveVisibleDet = false;
    bool haveVisibleView = false;

    QList<ADVSingleSequenceWidget*> views = getViewsFromADV();
    foreach (ADVSingleSequenceWidget* sw, views) {
        haveVisibleDet = haveVisibleDet || !sw->isDetViewCollapsed();
        haveVisibleView = haveVisibleView || !sw->isViewCollapsed();
        haveVisiblePan = haveVisiblePan || !sw->isPanViewCollapsed();
        haveVisibleNav = haveVisibleNav || !sw->isOverviewCollapsed();
    }

    QObject* s = sender();
    foreach (ADVSingleSequenceWidget* sw, views) {
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

void ADVSyncViewManager::sl_onSelectionChanged(LRegionsSelection* sel, const QVector<U2Region>& added, const QVector<U2Region>&) {
    Q_UNUSED(sel);
    if (selectionRecursion) {
        return;
    }

    selectionRecursion = true;

    auto focusedW = qobject_cast<ADVSingleSequenceWidget*>(adv->getActiveSequenceWidget());
    if (focusedW == nullptr) {
        return;
    }
    for (int i = 0; i < views.size(); ++i) {
        ADVSingleSequenceWidget* w = views[i];
        if (w == focusedW) {
            continue;
        }

        int offset = focusedW->getVisibleRange().startPos - w->getVisibleRange().startPos;

        DNASequenceSelection* selection = w->getSequenceSelection();
        selection->clear();
        qint64 seqLen = w->getSequenceLength();
        foreach (U2Region r, added) {
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
        case SyncMode_Start:
            lockByStartPosAction->toggle();
            break;
        case SyncMode_None:
            lockByStartPosAction->setChecked(false);
            lockBySeqSelAction->setChecked(false);
            lockByAnnSelAction->setChecked(false);
            break;
    }
}

void ADVSyncViewManager::updateAutoAnnotationActions() {
    aaActionMap.clear();
    toggleAutoAnnotationsMenu->clear();

    foreach (ADVSequenceWidget* w, adv->getSequenceWidgets()) {
        QList<ADVSequenceWidgetAction*> actions = w->getADVSequenceWidgetActions();
        bool active = false;
        for (ADVSequenceWidgetAction* action : qAsConst(actions)) {
            auto aaAction = qobject_cast<AutoAnnotationsADVAction*>(action);
            if (aaAction != nullptr) {
                QList<QAction*> aaToggleActions = aaAction->getToggleActions();
                for (QAction* toggleAction : qAsConst(aaToggleActions)) {
                    if (toggleAction->isEnabled()) {
                        aaActionMap.insert(toggleAction->text(), toggleAction);
                        active = true;
                    }
                }
                aaAction->setVisible(active);
            }
        }
    }

    toggleAutoAnnotationsButton->setEnabled(!aaActionMap.isEmpty());

    QSet<QString> actionNames = toSet(aaActionMap.keys());

    foreach (const QString& aName, actionNames) {
        auto action = new QAction(toggleAutoAnnotationsMenu);
        action->setObjectName(aName);
        connect(action, SIGNAL(triggered()), SLOT(sl_toggleAutoAnnotationHighlighting()));
        toggleAutoAnnotationsMenu->addAction(action);
    }
}

#define HAVE_ENABLED_AUTOANNOTATIONS "have_enabled_autoannotations"

void ADVSyncViewManager::sl_toggleAutoAnnotationHighlighting() {
    auto menuAction = qobject_cast<QAction*>(sender());
    if (menuAction == nullptr) {
        return;
    }
    QVariant val = menuAction->property(HAVE_ENABLED_AUTOANNOTATIONS);
    assert(val.isValid());
    bool haveEnabledAutoAnnotations = val.toBool();
    QList<QAction*> aaActions = {aaActionMap[menuAction->objectName()]};
    foreach (QAction* aaAction, aaActions) {
        aaAction->setChecked(!haveEnabledAutoAnnotations);
    }
}

void ADVSyncViewManager::sl_updateAutoAnnotationsMenu() {
    QList<QAction*> menuActions = toggleAutoAnnotationsMenu->actions();

    for (QAction* menuAction : qAsConst(menuActions)) {
        QString aName = menuAction->objectName();
        bool haveEnabledAutoAnnotations = false;
        // if have at least 1 checked  -> uncheck all
        QList<QAction*> aaActions = {aaActionMap[aName]};
        foreach (QAction* aaAction, aaActions) {
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
        menuAction->setProperty(HAVE_ENABLED_AUTOANNOTATIONS, haveEnabledAutoAnnotations);
    }
}

}  // namespace U2
