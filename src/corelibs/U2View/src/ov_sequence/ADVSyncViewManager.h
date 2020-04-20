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

#ifndef _U2_ADV_SYNC_VIEW_MANAGER_H_
#define _U2_ADV_SYNC_VIEW_MANAGER_H_

#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QToolButton>

#include <U2Core/U2Region.h>

namespace U2 {

class AnnotatedDNAView;
class ADVSequenceWidget;
class ADVSingleSequenceWidget;
class LRegionsSelection;

//Support for synchronous operation for ADVSingleSequenceViews: sync& lock scale, toggle visibility
class ADVSyncViewManager : public QObject {
    Q_OBJECT
public:
    ADVSyncViewManager(AnnotatedDNAView* v);
    ~ADVSyncViewManager();

    void updateAutoAnnotationActions();
    void updateToolbar1(QToolBar* tb);
    void updateToolbar2(QToolBar* tb);
private slots:
    void sl_rangeChanged();
    void sl_onSelectionChanged(LRegionsSelection* thiz, const QVector<U2Region>& added, const QVector<U2Region>& removed);
    void sl_sequenceWidgetAdded(ADVSequenceWidget* w);
    void sl_sequenceWidgetRemoved(ADVSequenceWidget* w);

    void sl_lock();
    void sl_sync();

    void sl_toggleVisualMode();
    void sl_toggleAutoAnnotationHighlighting();
    void sl_updateAutoAnnotationsMenu();
    void sl_updateVisualMode();


private:
    enum SyncMode {
        SyncMode_Start,
        SyncMode_SeqSel,
        SyncMode_AnnSel
    };

    void sync(bool lock, SyncMode mode);

    int offsetBySeqSel(ADVSingleSequenceWidget* w) const;
    int offsetByAnnSel(ADVSingleSequenceWidget* w) const;

    SyncMode detectSyncMode() const;
    int findSelectedAnnotationPos(ADVSingleSequenceWidget* w) const;

    void unlock();
    void updateEnabledState();
    void toggleCheckedAction(SyncMode mode);


    QList<ADVSingleSequenceWidget*> getViewsFromADV() const;

    AnnotatedDNAView*                   adv;

    QAction*                            lockByStartPosAction;
    QAction*                            lockBySeqSelAction;
    QAction*                            lockByAnnSelAction;
    QAction*                            syncByStartPosAction;
    QAction*                            syncBySeqSelAction;
    QAction*                            syncByAnnSelAction;
    QActionGroup*                       lockActionGroup;

    QToolButton*                        syncButton;
    QToolButton*                        lockButton;
    QMenu*                              syncMenu;
    QMenu*                              lockMenu;

    QAction*                            syncButtonTBAction;
    QAction*                            lockButtonTBAction;

    bool                                selectionRecursion;
    bool                                recursion;
    QList<ADVSingleSequenceWidget*>     views;

    // auto annotation highlighting
    QMap<QString, QAction*>             aaActionMap;
    QAction*                            toggleAutoAnnotationsAction;
    QMenu*                              toggleAutoAnnotationsMenu;
    QToolButton*                        toggleAutoAnnotationsButton;

    // visual mode vars
    QAction*                            toggleViewButtonAction;
    QToolButton*                        toggleViewButton;
    QMenu*                              toggleViewButtonMenu;
    QAction*                            toggleAllAction;
    QAction*                            toggleOveAction;
    QAction*                            togglePanAction;
    QAction*                            toggleDetAction;
};

} //namespace
#endif
