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

#include "DotPlotSplitter.h"
#include "DotPlotWidget.h"
#include "DotPlotFilterDialog.h"

#include <U2Gui/WidgetWithLocalToolbar.h>

#include <U2View/AnnotatedDNAView.h>

#include <U2Core/U2SafePoints.h>

#include <QString>
#include <QPair>
#include <QSet>

#include <QAction>
#include <QVBoxLayout>
#include <QMenu>
#include <QToolBar>


namespace U2 {

DotPlotSplitter::DotPlotSplitter(AnnotatedDNAView* a)
    : ADVSplitWidget(a),
      locked(false)
{
    syncLockAction =        createAction(":core/images/sync_lock.png",      tr("Multiple view synchronization lock"),   SLOT(sl_toggleSyncLock(bool)));
    filterAction =          createAction(":dotplot/images/filter.png",      tr("Filter results"),                       SLOT(sl_toggleFilter()),            false);
    zoomInAction =          createAction(":core/images/zoom_in.png",        tr("Zoom in (<b> + </b>)"),                 SLOT(sl_toggleZoomIn()),            false);
    zoomOutAction =         createAction(":core/images/zoom_out.png",       tr("Zoom out (<b> - </b>)"),                SLOT(sl_toggleZoomOut()),           false);
    resetZoomingAction =    createAction(":core/images/zoom_whole.png",     tr("Reset zooming (<b>0</b>)"),             SLOT(sl_toggleZoomReset()),         false);
    selAction =             createAction(":dotplot/images/cursor.png",      tr("Select tool (<b>S</b>)"),               SLOT(sl_toggleSel()));
    handAction =            createAction(":dotplot/images/hand_icon.png",   tr("Hand tool (<b>H</b>)"),                 SLOT(sl_toggleHand()));

    splitter = new QSplitter(Qt::Horizontal);

    WidgetWithLocalToolbar* wgt = new WidgetWithLocalToolbar(this);
    QLayout* l = new QVBoxLayout();
    l->setMargin(0);
    l->setSpacing(0);
    l->addWidget(splitter);
    wgt->setContentLayout(l);

    wgt->addActionToLocalToolbar(filterAction);
    wgt->addActionToLocalToolbar(syncLockAction);
    wgt->addActionToLocalToolbar(zoomInAction);
    wgt->addActionToLocalToolbar(zoomOutAction);
    wgt->addActionToLocalToolbar(resetZoomingAction);
    wgt->addActionToLocalToolbar(selAction);
    wgt->addActionToLocalToolbar(handAction);

    QLayout* mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    mainLayout->addWidget(wgt);
    setLayout(mainLayout);

    setAcceptDrops(false);

    setFocus();
}

bool DotPlotSplitter::onCloseEvent() {

    foreach (DotPlotWidget* w, dotPlotList) {
        bool canClose = w->onCloseEvent();
        if (!canClose) {
            return false;
        }
    }

    return true;
}

QAction *DotPlotSplitter::createAction(const QIcon& ic, const QString& toolTip, const char *slot, bool checkable) {

    QAction *a = new QAction(this);
    if (a != NULL) {
        a->setIcon(ic);
        a->setToolTip(toolTip);
        a->setCheckable(checkable);
        if (checkable) {
            connect(a, SIGNAL(toggled(bool)), this, slot);
        }
        else {
            connect(a, SIGNAL(triggered()), this, slot);
        }
    }

    return a;
}

QAction *DotPlotSplitter::createAction(const QString& iconPath, const QString& toolTip, const char *slot, bool checkable) {

    return createAction(QIcon(iconPath), toolTip, slot, checkable);
}

void DotPlotSplitter::addView(DotPlotWidget* view) {

    dotPlotList.append(view);
    SAFE_POINT(splitter, "splitter is NULL", );
    splitter->addWidget(view);

    connect(view,
        SIGNAL(si_dotPlotChanged(ADVSequenceObjectContext*, ADVSequenceObjectContext*, float, float, QPointF)),
        SLOT(sl_dotPlotChanged(ADVSequenceObjectContext*, ADVSequenceObjectContext*, float, float, QPointF)));

    connect(view,
        SIGNAL(si_dotPlotSelecting()),
        SLOT(sl_dotPlotSelecting()));

    checkLockButtonState();
    view->setFocus();
    updateButtonState();
}

void DotPlotSplitter::removeView(DotPlotWidget* view) {

    dotPlotList.removeAll(view);
    checkLockButtonState();
}

typedef QPair<QString,QString> QStrStrPair;

void DotPlotSplitter::checkLockButtonState() {

    QSet<QStrStrPair> dotPlots;

    bool enableLockButton = false;

    int dotPlotCount = dotPlotList.count();
    foreach (DotPlotWidget *dpWidget, dotPlotList) {

        QString nameX = dpWidget->getXSequenceName();
        QString nameY = dpWidget->getYSequenceName();

        if (!nameX.isEmpty() && !nameY.isEmpty()) {

            QStrStrPair p;
            p.first = nameX;
            p.second = nameY;

            if (dotPlots.contains(p)) {
                enableLockButton = true;
            }
            else {
                dotPlots.insert(p);
            }

            dpWidget->setIgnorePanView(dotPlotCount > 1);
        }
    }

    if (syncLockAction) {
        syncLockAction->setEnabled(enableLockButton);
        if (!enableLockButton) { // disabled button should not be checked
            syncLockAction->setChecked(false);
        }
    }
}

bool DotPlotSplitter::isEmpty() const{

    return dotPlotList.isEmpty();
}

// build popup menu for all DotPlotWidgets
// each DotPlotWidget defines if it should add menu
void DotPlotSplitter::buildPopupMenu(QMenu *m) {
    foreach (DotPlotWidget *w, dotPlotList) {
        SAFE_POINT(w, "w is NULL", );
        w->buildPopupMenu(m);
    }
}

void DotPlotSplitter::sl_toggleSyncLock(bool l) {

    locked = l;
}

void DotPlotSplitter::sl_toggleFilter(){
    foreach (DotPlotWidget *w, dotPlotList) {
        SAFE_POINT(w, "w is NULL", );
        w->sl_filter();
        break; //todo: support several widgets
    }
}

void DotPlotSplitter::sl_toggleAspectRatio(bool aspectRatio) {
    bool noFocus = true;
    foreach (DotPlotWidget *dpWidget, dotPlotList) {
        if(dpWidget->hasFocus()){
            dpWidget->setKeepAspectRatio(aspectRatio);
            noFocus = false;
            break;
        }
    }
    if(noFocus){
        foreach (DotPlotWidget *dpWidget, dotPlotList) {
            dpWidget->setKeepAspectRatio(aspectRatio);
        }
    }
}

void DotPlotSplitter::sl_toggleZoomIn() {
    bool noFocus = true;
    foreach (DotPlotWidget *dpWidget, dotPlotList) {
        if(dpWidget->hasFocus()){
            dpWidget->zoomIn();
            noFocus = false;
            break;
        }
    }
    if(noFocus){
        foreach (DotPlotWidget *dpWidget, dotPlotList) {
            dpWidget->zoomIn();
        }
    }
    updateButtonState();
}

void DotPlotSplitter::sl_toggleZoomOut() {
    bool noFocus = true;
    foreach (DotPlotWidget *dpWidget, dotPlotList) {
        if(dpWidget->hasFocus()){
            dpWidget->zoomOut();
            noFocus = false;
            break;
        }
    }
    if(noFocus){
        foreach (DotPlotWidget *dpWidget, dotPlotList) {
            dpWidget->zoomOut();
        }
    }
    updateButtonState();
}

void DotPlotSplitter::sl_toggleZoomReset() {
    bool noFocus = true;
    foreach (DotPlotWidget *dpWidget, dotPlotList) {
        if(dpWidget->hasFocus()){
            dpWidget->zoomReset();
            noFocus = false;
            break;
        }
    }
    if(noFocus){
        foreach (DotPlotWidget *dpWidget, dotPlotList) {
            dpWidget->zoomReset();
        }
    }
    updateButtonState();
}

void DotPlotSplitter::sl_toggleSel(){
    handAction->setChecked(!selAction->isChecked());

    foreach (DotPlotWidget *dpWidget, dotPlotList) {
        dpWidget->setSelActive(selAction->isChecked());
    }
}
void DotPlotSplitter::sl_toggleHand(){
    selAction->setChecked(!handAction->isChecked());

    foreach (DotPlotWidget *dpWidget, dotPlotList) {
        dpWidget->setSelActive(selAction->isChecked());
    }
}

void DotPlotSplitter::sl_dotPlotChanged(ADVSequenceObjectContext* sequenceX, ADVSequenceObjectContext* sequenceY, float shiftX, float shiftY, QPointF zoom) {
    SAFE_POINT(sequenceX != NULL && sequenceY != NULL, tr("One of the sequences in dotplot is NULL"), );

    checkLockButtonState();

    if (locked) {
        foreach (DotPlotWidget* w, dotPlotList) {
            w->setShiftZoom(sequenceX, sequenceY, shiftX, shiftY, zoom);
        }
        update();

    }
    updateButtonState();

}

void DotPlotSplitter::sl_dotPlotSelecting() {
    foreach (DotPlotWidget* w, dotPlotList) {
        if (w->hasSelection()) {
            break;
        }
    }
}

void DotPlotSplitter::updateButtonState(){
    bool noFocus = true;
    foreach (DotPlotWidget *dpWidget, dotPlotList) {
        if (dpWidget->hasFocus()){
            zoomInAction->setEnabled(dpWidget->canZoomIn());
            zoomOutAction->setEnabled(dpWidget->canZoomOut());
            resetZoomingAction->setEnabled(dpWidget->canZoomOut());
            noFocus = false;
            break;
        }
    }
    if (noFocus && !dotPlotList.isEmpty()){
        DotPlotWidget *dpWidget = dotPlotList.first();
        zoomInAction->setEnabled(dpWidget->canZoomIn());
        zoomOutAction->setEnabled(dpWidget->canZoomOut());
        resetZoomingAction->setEnabled(dpWidget->canZoomOut());
        handAction->setShortcut(QKeySequence());
        selAction->setShortcut(QKeySequence());
        zoomInAction->setShortcut(QKeySequence());
        zoomOutAction->setShortcut(QKeySequence());
        resetZoomingAction->setShortcut(QKeySequence());
    } else {
        handAction->setShortcut('H');
        selAction->setShortcut('S');
        zoomInAction->setShortcut('+');
        zoomOutAction->setShortcut('-');
        resetZoomingAction->setShortcut('0');
    }
}

} // namespace
