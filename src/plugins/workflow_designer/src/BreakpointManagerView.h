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

#ifndef _U2_BREAKPOINT_MANAGER_VIEW_H_
#define _U2_BREAKPOINT_MANAGER_VIEW_H_

#include <QWidget>

#include <U2Lang/ActorModel.h>
#include <U2Lang/Schema.h>
#include <U2Lang/WorkflowBreakpointSharedInfo.h>

#include <U2Gui/BreakpointConditionEditDialog.h>

class QTreeWidget;
class QTreeWidgetItem;
class QToolBar;
class QAction;
class QGraphicsScene;

namespace U2 {

using namespace Workflow;
class WorkflowDebugStatus;

class BreakpointManagerView : public QWidget {
    Q_OBJECT
public:
    BreakpointManagerView(WorkflowDebugStatus *initDebugInfo, Schema *initScheme, QGraphicsScene *scene, QWidget *parent = NULL);
    void onBreakpointReached(ActorId actor);

    QAction* getNewBreakpointAction() { return newBreakpointAction; }

    bool eventFilter(QObject *object, QEvent *event);
    void clear();

signals:
    void si_highlightingRequested(const ActorId &actor);

protected:
    virtual void paintEvent(QPaintEvent *event);

public slots:
    void sl_breakpointRemoved(const ActorId &actorId);
    void sl_deleteAllBreakpoints();

private slots:
    void sl_breakpointAdded(const ActorId &actorId);
    void sl_newBreakpoint();
    void sl_deleteSelectedBreakpoint();
    void sl_disableAllBreakpoints();
    void sl_breakpointsSelectionChanged();
    void sl_breakpointStateChanged(int state);
    void sl_highlightItem();
    void sl_breakpointDoubleClicked(QTreeWidgetItem *item, int column);
    void sl_labelsCreated(QStringList newLabels);
    void sl_labelAddedToCurrentBreakpoint(QStringList newLabels);
    void sl_resetHitCount();
    void sl_hitCounterAssigned(const QString &hitCounterCondition, quint32 parameter);
    void sl_breakpointEnabled(const ActorId &actor);
    void sl_breakpointDisabled(const ActorId &actor);
    void sl_addBreakpoint(const QString &elementName);
    void sl_contextMenuForBreakpointListRequested(const QPoint &pos);
    void sl_hitCount();
    void sl_editLabels();
    void sl_setCondition();
    void sl_conditionTextChanged(const QString &text);
    void sl_conditionSwitched(bool enabled);
    void sl_conditionParameterChanged(HitCondition newParameter);
    void sl_pauseStateChanged(bool paused);

private:
    void createActions();
    QToolBar *initToolBar();
    void initBreakpointsList();
    void removeBreakpointFromList(QTreeWidgetItem *item);
    void removeBreakpointsFromList(QList<QTreeWidgetItem *> items);
    void disableGenericActionsIfNoItemsExist();
    QWidget *getBreakpointStateController(const ActorId &actor);
    void updateCurrentHitCountLabels(bool show) const;
    void setBreakpointBackgroundColor(QTreeWidgetItem *breakpoint, const QColor &newBackground);

    WorkflowDebugStatus *debugInfo;
    QGraphicsScene *scene;
    Schema *scheme;

    QTreeWidget *breakpointsList;
    QMap<QTreeWidgetItem *, ActorId> actorConnections;
    QMap<QWidget *, QTreeWidgetItem *> breakpointStateControls;
    QStringList allExistingLabels;
    QTreeWidgetItem *lastReachedBreakpoint;

    QAction *newBreakpointAction;
    QAction *deleteSelectedBreakpointAction;
    QAction *deleteAllBreakpointsAction;
    QAction *disableAllBreakpointsAction;
    QAction *highlightItemWithBreakpoint;
    QAction *hitCountAction;
    QAction *editLabelsAction;
    QAction *setConditionAction;

    static QMap<BreakpointConditionParameter, HitCondition> conditionParametertranslations;
};

} // namespace U2

#endif // _U2_BREAKPOINT_MANAGER_VIEW_H_
