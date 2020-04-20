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

#include <QAction>
#include <QCheckBox>
#include <QGraphicsScene>
#include <QGroupBox>
#include <QMenu>
#include <QToolBar>
#include <QTreeWidget>
#include <QVBoxLayout>

#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/BreakpointHitCountDialog.h>
#include <U2Designer/DelegateEditors.h>
#include <U2Designer/EditBreakpointLabelsDialog.h>
#include <U2Designer/NewBreakpointDialog.h>

#include <U2Lang/WorkflowDebugStatus.h>
#include <U2Lang/WorkflowSettings.h>

#include "BreakpointManagerView.h"
#include "WorkflowViewItems.h"

const char *VIEW_ELEMENT_NAME_COLUMN_TITLE = "Element Name";
const char *VIEW_LABELS_COLUMN_TITLE = "Labels";
const char *VIEW_BREAKPOINT_STATE_COLUMN_NAME = "State";
const char *VIEW_BREAKPOINT_CONDITION_COLUMN_NAME = "Condition";
const char *VIEW_BREAKPOINT_HIT_COUNT_COLUMN_NAME = "Hit Count";

const bool DEFAULT_BREAKPOINT_STATE = true;
const int BREAKPOINT_STATE_COLUMN_NUMBER = 0;
const int BREAKPOINT_LABELS_COLUMN_NUMBER = 2;
const int BREAKPOINT_CONDITION_COLUMN_NUMBER = 3;
const int BREAKPOINT_HIT_COUNT_COLUMN_NUMBER = 4;
const char *DEFAULT_BREAKPOINT_LABEL = "";
const char *DEFAULT_BREAKPOINT_CONDITION = "(no condition)";
const char *CURRENT_HIT_COUNT_BEGINNING = " (currently ";
const char *CURRENT_HIT_COUNT_ENDING = ")";

const QString BREAKPOINT_LABELS_SEPARATOR = ";";

const char *CONDITION_LABEL_BEGIN = "when '";
const char *CONDITION_LABEL_END_IS_TRUE = "' is true";
const char *CONDITION_LABEL_END_HAS_CHANGED = "' has changed";

const QColor BREAKPOINT_HIGHLIGHTING_COLOR = Qt::yellow;
const QColor BREAKPOINT_DEFAULT_COLOR = Qt::white;

namespace U2 {

QMap<BreakpointConditionParameter, HitCondition>
    BreakpointManagerView::conditionParametertranslations
    = QMap<BreakpointConditionParameter, HitCondition>();

BreakpointManagerView::BreakpointManagerView(WorkflowDebugStatus *initDebugInfo, Schema *initScheme,
                                             QGraphicsScene *scene, QWidget *parent)
    : QWidget(parent),
      debugInfo(initDebugInfo),
      scene(scene),
      scheme(initScheme),
      breakpointsList(NULL),
      actorConnections(),
      breakpointStateControls(),
      allExistingLabels(),
      lastReachedBreakpoint(NULL),
      newBreakpointAction(NULL),
      deleteSelectedBreakpointAction(NULL),
      deleteAllBreakpointsAction(NULL),
      disableAllBreakpointsAction(NULL),
      highlightItemWithBreakpoint(NULL),
      hitCountAction(NULL),
      editLabelsAction(NULL),
      setConditionAction(NULL)
{
    Q_ASSERT(NULL != debugInfo && NULL != scheme);

    createActions();
    Q_ASSERT(NULL != newBreakpointAction && NULL != deleteAllBreakpointsAction
        && NULL != deleteSelectedBreakpointAction && NULL != disableAllBreakpointsAction
        && NULL != hitCountAction && NULL != editLabelsAction && NULL != setConditionAction);

    initBreakpointsList();
    Q_ASSERT(NULL != breakpointsList);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QVBoxLayout *contentLayout = new QVBoxLayout(this);
    contentLayout->setSpacing(0);
    contentLayout->setMargin(0);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->addWidget(initToolBar());
    contentLayout->addWidget(breakpointsList);

    connect(debugInfo, SIGNAL(si_pauseStateChanged(bool)), SLOT(sl_pauseStateChanged(bool)));
    connect(debugInfo, SIGNAL(si_breakpointAdded(const ActorId &)),
        SLOT(sl_breakpointAdded(const ActorId &)));
    connect(debugInfo, SIGNAL(si_breakpointEnabled(const ActorId &)),
        SLOT(sl_breakpointEnabled(const ActorId &)));
    connect(debugInfo, SIGNAL(si_breakpointRemoved(const ActorId &)),
        SLOT(sl_breakpointRemoved(const ActorId &)));
    connect(debugInfo, SIGNAL(si_breakpointDisabled(const ActorId &)),
        SLOT(sl_breakpointDisabled(const ActorId &)));

    connect(breakpointsList, SIGNAL(itemSelectionChanged()),
        SLOT(sl_breakpointsSelectionChanged()));
    connect(breakpointsList, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
        SLOT(sl_breakpointDoubleClicked(QTreeWidgetItem *, int)));

    if (conditionParametertranslations.isEmpty()) {
        conditionParametertranslations[IS_TRUE] = CONDITION_IS_TRUE;
        conditionParametertranslations[HAS_CHANGED] = CONDITION_HAS_CHANGED;
    }

    hide();

    installEventFilter(this);
}

void BreakpointManagerView::createActions() {
    newBreakpointAction = new QAction(tr("&Break at element..."), this);
    newBreakpointAction->setIcon(QIcon(":workflow_designer/images/breakpoint.png"));
    newBreakpointAction->setShortcut(QKeySequence("Ctrl+B"));
    connect(newBreakpointAction, SIGNAL(triggered()), SLOT(sl_newBreakpoint()));
    connect(newBreakpointAction, SIGNAL(triggered()), scene, SLOT(update()));
    newBreakpointAction->setEnabled(true);

    deleteAllBreakpointsAction = new QAction(tr("Delete &all breakpoints"), this);
    deleteAllBreakpointsAction->setIcon(QIcon(":workflow_designer/images/delete_all_breakpoints.png"));
    deleteAllBreakpointsAction->setShortcut(QKeySequence("Shift+Del"));
    deleteAllBreakpointsAction->setShortcutContext(Qt::WidgetShortcut);
    connect(deleteAllBreakpointsAction, SIGNAL(triggered()), SLOT(sl_deleteAllBreakpoints()));
    deleteAllBreakpointsAction->setEnabled(false);

    deleteSelectedBreakpointAction = new QAction(tr("&Delete"), this);
    deleteSelectedBreakpointAction->setIcon(QIcon(":workflow_designer/images/delete_selected_breakpoints.png"));
    deleteSelectedBreakpointAction->setShortcut(QKeySequence("Del"));
    deleteSelectedBreakpointAction->setShortcutContext(Qt::WidgetShortcut);

    connect(deleteSelectedBreakpointAction, SIGNAL(triggered()), SLOT(sl_deleteSelectedBreakpoint()));
    deleteSelectedBreakpointAction->setEnabled(false);
    deleteSelectedBreakpointAction->setToolTip(tr("Delete the selected breakpoints"));

    disableAllBreakpointsAction = new QAction(tr("&Enable or disable all breakpoints"), this);
    disableAllBreakpointsAction->setIcon(QIcon(":workflow_designer/images/disable_all_breakpoints.png"));
    disableAllBreakpointsAction->setShortcut(QKeySequence("Ctrl+D"));
    connect(disableAllBreakpointsAction, SIGNAL(triggered()), SLOT(sl_disableAllBreakpoints()));
    disableAllBreakpointsAction->setEnabled(false);

    highlightItemWithBreakpoint = new QAction(tr("H&ighlight selected item"), this);
    highlightItemWithBreakpoint->setIcon(QIcon(":workflow_designer/images/highlight_item.png"));
    highlightItemWithBreakpoint->setShortcut(QKeySequence("Ctrl+H"));
    connect(highlightItemWithBreakpoint, SIGNAL(triggered()), SLOT(sl_highlightItem()));
    highlightItemWithBreakpoint->setEnabled(false);

    hitCountAction = new QAction(tr("&Hit Count..."), this);
    connect(hitCountAction, SIGNAL(triggered()), SLOT(sl_hitCount()));

    editLabelsAction = new QAction(tr("Edit &labels..."), this);
    connect(editLabelsAction, SIGNAL(triggered()), SLOT(sl_editLabels()));

    setConditionAction = new QAction(tr("&Condition..."), this);
    connect(setConditionAction, SIGNAL(triggered()), SLOT(sl_setCondition()));
}

QToolBar *BreakpointManagerView::initToolBar() {
    QToolBar *mainToolBar = new QToolBar(this);

    mainToolBar->addAction(newBreakpointAction);
    mainToolBar->addAction(deleteSelectedBreakpointAction);
    mainToolBar->addAction(deleteAllBreakpointsAction);
    mainToolBar->addAction(disableAllBreakpointsAction);
    mainToolBar->addAction(highlightItemWithBreakpoint);

    mainToolBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    return mainToolBar;
}

void BreakpointManagerView::initBreakpointsList() {
    Q_ASSERT(NULL == breakpointsList);

    breakpointsList = new QTreeWidget(this);
    breakpointsList->setObjectName("breakpoints list");
    QStringList headerLabels = QStringList() << tr(VIEW_BREAKPOINT_STATE_COLUMN_NAME)
        << tr(VIEW_ELEMENT_NAME_COLUMN_TITLE) << tr(VIEW_LABELS_COLUMN_TITLE)
        << tr(VIEW_BREAKPOINT_CONDITION_COLUMN_NAME) << tr(VIEW_BREAKPOINT_HIT_COUNT_COLUMN_NAME);
    breakpointsList->setColumnCount(headerLabels.size());
    breakpointsList->setHeaderLabels(headerLabels);
    breakpointsList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    breakpointsList->setSortingEnabled(true);
    breakpointsList->resizeColumnToContents(BREAKPOINT_STATE_COLUMN_NUMBER);
    breakpointsList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(breakpointsList, SIGNAL(customContextMenuRequested(const QPoint &)),
        SLOT(sl_contextMenuForBreakpointListRequested(const QPoint &)));
}

void BreakpointManagerView::sl_breakpointAdded(const ActorId &actorId) {
    if(!actorConnections.values().contains(actorId)) {
        QCheckBox *stateCheckBox = new QCheckBox();
        stateCheckBox->setChecked(DEFAULT_BREAKPOINT_STATE);
        stateCheckBox->setAutoFillBackground(true);

        Actor *actor = scheme->actorById(actorId);
        QTreeWidgetItem *item = new QTreeWidgetItem(breakpointsList, QStringList() << QString()
            << actor->getLabel() << tr(DEFAULT_BREAKPOINT_LABEL) << tr(DEFAULT_BREAKPOINT_CONDITION)
            << getNamesOfHitCounters()[ALWAYS]);

        actorConnections[item] = actorId;
        breakpointsList->setItemWidget(item, BREAKPOINT_STATE_COLUMN_NUMBER, stateCheckBox);
        breakpointStateControls[stateCheckBox] = item;
        connect(stateCheckBox, SIGNAL(stateChanged(int)), SLOT(sl_breakpointStateChanged(int)));

        if(!deleteAllBreakpointsAction->isEnabled()) {
            deleteAllBreakpointsAction->setEnabled(true);
        }
        if(!disableAllBreakpointsAction->isEnabled()) {
            disableAllBreakpointsAction->setEnabled(true);
        }
    } else {
        QCheckBox *stateController = dynamic_cast<QCheckBox *>(
            breakpointsList->itemWidget(actorConnections.key(actorId),
            BREAKPOINT_STATE_COLUMN_NUMBER));
        if(!stateController->isChecked()) {
            stateController->setChecked(true);
        }
    }
}

void BreakpointManagerView::clear() {
    foreach (const ActorId &id, actorConnections.values()) {
        sl_breakpointRemoved(id);
    }
}

void BreakpointManagerView::sl_breakpointRemoved(const ActorId &actorId) {
    removeBreakpointFromList(actorConnections.key(actorId));
}

void BreakpointManagerView::sl_newBreakpoint() {
    // if there is any selected items - add breakpoints automatically
    if (scene->selectedItems().size() != 0) {
        foreach(QGraphicsItem *item, scene->selectedItems()) {
            if(WorkflowProcessItemType == item->type()) {
                WorkflowProcessItem *processItem = qgraphicsitem_cast<WorkflowProcessItem*>(item);
                SAFE_POINT(NULL != processItem, "WorkflowProcessItem is NULL!",);
                if(processItem->isBreakpointInserted() && !processItem->isBreakpointEnabled()) {
                    processItem->toggleBreakpointState();
                    debugInfo->setBreakpointEnabled(processItem->getProcess()->getId(), true);
                } else {
                    processItem->toggleBreakpoint();
                }
                if(processItem->isBreakpointInserted()) {
                    debugInfo->addBreakpointToActor(processItem->getProcess()->getId());
                } else {
                    debugInfo->removeBreakpointFromActor(processItem->getProcess()->getId());
                }
            }
        }
    } else {
        // no items selected - launch new breakpoint dialog
        QStringList actorsLabels;
        foreach(Actor *actor, scheme->getProcesses()) {
            actorsLabels << actor->getLabel();
        }

        QObjectScopedPointer<NewBreakpointDialog> dialog = new NewBreakpointDialog(actorsLabels, this);

        connect(dialog.data(), SIGNAL(si_newBreakpointCreated(const QString &)), SLOT(sl_addBreakpoint(const QString &)));
        dialog->exec();
        CHECK(!dialog.isNull(), );
    }
}

void BreakpointManagerView::sl_addBreakpoint(const QString &elementName) {
    foreach(Actor *actor, scheme->getProcesses()) {
        if(elementName == actor->getLabel()) {
            debugInfo->addBreakpointToActor(actor->getId());
            return;
        }
    }
    Q_ASSERT(false);
}

void BreakpointManagerView::sl_deleteSelectedBreakpoint() {
    removeBreakpointsFromList(breakpointsList->selectedItems());
}

void BreakpointManagerView::sl_deleteAllBreakpoints() {
    removeBreakpointsFromList(breakpointsList->findItems("", Qt::MatchContains));
}

void BreakpointManagerView::sl_disableAllBreakpoints() {
    QCheckBox *firstController = qobject_cast<QCheckBox *>(breakpointStateControls.begin().key());
    Q_ASSERT(NULL != firstController);
    const bool currentState = (Qt::Checked == firstController->checkState());
    foreach(QWidget *breakpointStateController, breakpointStateControls.keys()) {
        QCheckBox *checkBox = qobject_cast<QCheckBox *>(breakpointStateController);
        Q_ASSERT(NULL != checkBox);
        checkBox->setChecked(!currentState);
    }
}

void BreakpointManagerView::removeBreakpointFromList(QTreeWidgetItem *item) {
    const int removedControlsCount = breakpointStateControls.remove(breakpointStateControls.key(item));
    const int removedActorsCount = actorConnections.remove(item);
    Q_ASSERT(1 == removedActorsCount || 0 == removedActorsCount || removedControlsCount
        != removedActorsCount);
    Q_UNUSED(removedControlsCount);

    if(1 == removedActorsCount) {
        if(lastReachedBreakpoint == item) {
            lastReachedBreakpoint = NULL;
        }
        delete item;
    }
    disableGenericActionsIfNoItemsExist();
}

void BreakpointManagerView::removeBreakpointsFromList(QList<QTreeWidgetItem *> items) {
    foreach(QTreeWidgetItem *item, items) {
        debugInfo->removeBreakpointFromActor(actorConnections[item]);
    }
}

void BreakpointManagerView::sl_breakpointsSelectionChanged() {
    const int countOfSelectedBreakpoints = breakpointsList->selectedItems().size();
    deleteSelectedBreakpointAction->setEnabled(0 < countOfSelectedBreakpoints);
    highlightItemWithBreakpoint->setEnabled(1 == countOfSelectedBreakpoints);
}

void BreakpointManagerView::sl_breakpointStateChanged(int state) {
    QObject *activator = sender();
    QWidget *stateAssigner = qobject_cast<QWidget *>(activator);
    Q_ASSERT(NULL != stateAssigner);
    QTreeWidgetItem *breakpointItem = breakpointStateControls[stateAssigner];

    bool isBreakpointBeingEnabled = (Qt::Checked == state);
    debugInfo->setBreakpointEnabled(actorConnections[breakpointItem], isBreakpointBeingEnabled);
}

void BreakpointManagerView::disableGenericActionsIfNoItemsExist() {
    const int countOfItems = breakpointsList->findItems("", Qt::MatchContains).size();
    deleteAllBreakpointsAction->setEnabled(countOfItems > 0);
    disableAllBreakpointsAction->setEnabled(countOfItems > 0);
}

void BreakpointManagerView::sl_highlightItem() {
    const QList<QTreeWidgetItem *> selectedItems = breakpointsList->selectedItems();
    Q_ASSERT(1 == selectedItems.size());
    emit si_highlightingRequested(actorConnections[selectedItems.first()]);
}

void BreakpointManagerView::sl_breakpointDoubleClicked(QTreeWidgetItem *item, int column) {
    if(NULL != item) {
        switch (column) {
        case BREAKPOINT_LABELS_COLUMN_NUMBER:
            sl_editLabels();
            break;
        case BREAKPOINT_CONDITION_COLUMN_NUMBER:
            sl_setCondition();
            break;
        case BREAKPOINT_HIT_COUNT_COLUMN_NUMBER:
            sl_hitCount();
            break;
        }
    }
}

void BreakpointManagerView::sl_labelsCreated(QStringList newLabels) {
    allExistingLabels += newLabels;
    debugInfo->addNewAvailableBreakpointLabels(newLabels);
}

void BreakpointManagerView::sl_labelAddedToCurrentBreakpoint(QStringList newLabels) {
    QTreeWidgetItem *currentItem = breakpointsList->currentItem();
    currentItem->setText(BREAKPOINT_LABELS_COLUMN_NUMBER,
        newLabels.join(BREAKPOINT_LABELS_SEPARATOR));
    debugInfo->setBreakpointLabels(actorConnections[currentItem], newLabels);
}

void BreakpointManagerView::sl_breakpointEnabled(const ActorId &actor) {
    QCheckBox *stateAssigner = dynamic_cast<QCheckBox *>(getBreakpointStateController(actor));
    Q_ASSERT(NULL != stateAssigner);
    stateAssigner->setChecked(true);
}

void BreakpointManagerView::sl_breakpointDisabled(const ActorId &actor) {
    QCheckBox *stateAssigner = dynamic_cast<QCheckBox *>(getBreakpointStateController(actor));
    Q_ASSERT(NULL != stateAssigner);
    stateAssigner->setChecked(false);
}

QWidget * BreakpointManagerView::getBreakpointStateController(const ActorId &actor) {
    QTreeWidgetItem *actorViewItem = actorConnections.key(actor, NULL);
    Q_ASSERT(NULL != actorViewItem);
    QCheckBox *stateController = dynamic_cast<QCheckBox *>(breakpointStateControls.key(actorViewItem,
        NULL));
    Q_ASSERT(NULL != stateController);
    return stateController;
}

void BreakpointManagerView::sl_resetHitCount() {
    QTreeWidgetItem *currentItem = breakpointsList->currentItem();
    Q_ASSERT(NULL != currentItem);
    debugInfo->resetHitCounterForActor(actorConnections[currentItem]);
    updateCurrentHitCountLabels(true);
}

void BreakpointManagerView::sl_hitCounterAssigned(const QString &hitCounterCondition, quint32
    parameter)
{
    QTreeWidgetItem *currentItem = breakpointsList->currentItem();
    Q_ASSERT(NULL != currentItem);
    if(hitCounterCondition != currentItem->text(BREAKPOINT_HIT_COUNT_COLUMN_NUMBER)) {
        QString hitCounterLabel = hitCounterCondition;
        if (hitCounterCondition != getNamesOfHitCounters()[ALWAYS]) {
            hitCounterLabel.append(QString::number(parameter));
        }
        currentItem->setText(BREAKPOINT_HIT_COUNT_COLUMN_NUMBER, hitCounterLabel);
    }
    debugInfo->setHitCounterForActor(actorConnections[currentItem],
        getNamesOfHitCounters().key(hitCounterCondition), parameter);
}

void BreakpointManagerView::sl_contextMenuForBreakpointListRequested(const QPoint &pos) {
    if(NULL != breakpointsList->currentItem() && NULL != breakpointsList->itemAt(pos)) {
        QMenu contextMenu;
        contextMenu.addAction(deleteSelectedBreakpointAction);
        contextMenu.addSeparator();
        contextMenu.addAction(setConditionAction);
        contextMenu.addAction(hitCountAction);
        contextMenu.addAction(editLabelsAction);

        contextMenu.exec(breakpointsList->viewport()->mapToGlobal(pos));
    }
}

void BreakpointManagerView::sl_hitCount() {
    QTreeWidgetItem *item = breakpointsList->currentItem();
    const QMap<BreakpointHitCountCondition, QString> &hitCountersNames
        = getNamesOfHitCounters();
    const BreakpointHitCounterDump hitCounterInfo
        = debugInfo->getHitCounterDumpForActor(actorConnections[item]);
    QObjectScopedPointer<BreakpointHitCountDialog> hitCountDialog = new BreakpointHitCountDialog(QStringList(hitCountersNames.values()),
        hitCountersNames[hitCounterInfo.typeOfCondition], hitCounterInfo.hitCounterParameter,
        hitCounterInfo.hitCount, QStringList(hitCountersNames[ALWAYS]), this);

    connect(hitCountDialog.data(), SIGNAL(si_resetHitCount()), SLOT(sl_resetHitCount()));
    connect(hitCountDialog.data(), SIGNAL(si_hitCounterAssigned(const QString &, quint32)),
        SLOT(sl_hitCounterAssigned(const QString &, quint32)));

    hitCountDialog->exec();
}

void BreakpointManagerView::sl_editLabels() {
    QTreeWidgetItem *item = breakpointsList->currentItem();
    QObjectScopedPointer<EditBreakpointLabelsDialog> labelsDialog = new EditBreakpointLabelsDialog(debugInfo->getAvailableBreakpointLabels(),
        debugInfo->getBreakpointLabels(actorConnections[item]), this);

    connect(labelsDialog.data(), SIGNAL(si_labelsCreated(QStringList)), SLOT(sl_labelsCreated(QStringList)));
    connect(labelsDialog.data(), SIGNAL(si_labelAddedToCallingBreakpoint(QStringList)),
        SLOT(sl_labelAddedToCurrentBreakpoint(QStringList)));

    labelsDialog->exec();
}

void BreakpointManagerView::sl_setCondition() {
    QTreeWidgetItem *item = breakpointsList->currentItem();
    const ActorId actorId = actorConnections[item];
    BreakpointConditionDump conditionInfo = debugInfo->getConditionDumpForActor(actorId);
    HitCondition conditionParameter
        = conditionParametertranslations[conditionInfo.conditionParameter];

    QObjectScopedPointer<BreakpointConditionEditDialog> conditionDialog = new BreakpointConditionEditDialog(this,
        AttributeScriptDelegate::createScriptHeader(*(scheme->actorById(actorId)->getCondition())),
        (conditionInfo.isEnabled | conditionInfo.condition.isEmpty()), conditionInfo.condition,
        conditionParameter);

    connect(conditionDialog.data(), SIGNAL(si_conditionTextChanged(const QString &)),
        SLOT(sl_conditionTextChanged(const QString &)));
    connect(conditionDialog.data(), SIGNAL(si_conditionParameterChanged(HitCondition)),
        SLOT(sl_conditionParameterChanged(HitCondition)));
    connect(conditionDialog.data(), SIGNAL(si_conditionSwitched(bool)),
        SLOT(sl_conditionSwitched(bool)));

    conditionDialog->exec();
}

void BreakpointManagerView::sl_conditionTextChanged(const QString &text) {
    QTreeWidgetItem *item = breakpointsList->currentItem();
    const ActorId actorId = actorConnections[item];

    debugInfo->setConditionTextForActor(actorId, text);
    BreakpointConditionDump conditionInfo = debugInfo->getConditionDumpForActor(actorId);
    if (conditionInfo.isEnabled && !text.isEmpty()) {
        QString conditionLabel = tr(CONDITION_LABEL_BEGIN) + text;
        QString conditionLabelEnd;
        switch (conditionInfo.conditionParameter) {
        case IS_TRUE:
            conditionLabelEnd = tr(CONDITION_LABEL_END_IS_TRUE);
            break;
        case HAS_CHANGED:
            conditionLabelEnd = tr(CONDITION_LABEL_END_HAS_CHANGED);
            break;
        default:
            Q_ASSERT(false);
        }
        conditionLabel.append(conditionLabelEnd);
        item->setText(BREAKPOINT_CONDITION_COLUMN_NUMBER, conditionLabel);
    } else {
        item->setText(BREAKPOINT_CONDITION_COLUMN_NUMBER, tr(DEFAULT_BREAKPOINT_CONDITION));
    }
}

void BreakpointManagerView::sl_conditionSwitched(bool enabled) {
    QTreeWidgetItem *item = breakpointsList->currentItem();
    debugInfo->setConditionEnabledForActor(actorConnections[item], enabled);
}

void BreakpointManagerView::sl_conditionParameterChanged(HitCondition newParameter) {
    QTreeWidgetItem *item = breakpointsList->currentItem();
    BreakpointConditionParameter translatedParameter = conditionParametertranslations.key(
        newParameter);
    debugInfo->setConditionParameterForActor(actorConnections[item], translatedParameter);
}

void BreakpointManagerView::sl_pauseStateChanged(bool paused) {
    updateCurrentHitCountLabels(paused);
    if(!paused && NULL != lastReachedBreakpoint) {
        setBreakpointBackgroundColor(lastReachedBreakpoint, BREAKPOINT_DEFAULT_COLOR);
        lastReachedBreakpoint = NULL;
    }
}

void BreakpointManagerView::updateCurrentHitCountLabels(bool show) const {
    foreach(QTreeWidgetItem *item, actorConnections.keys()) {
        QString hitCountLabel = item->text(BREAKPOINT_HIT_COUNT_COLUMN_NUMBER);
        const int currentHitCountBegining = hitCountLabel.indexOf(tr(CURRENT_HIT_COUNT_BEGINNING));
        if(-1 != currentHitCountBegining) {
            hitCountLabel = hitCountLabel.left(currentHitCountBegining);
        }
        if(show) {
            hitCountLabel += tr(CURRENT_HIT_COUNT_BEGINNING)
                + QString::number(debugInfo->getHitCountForActor(actorConnections[item]))
                + tr(CURRENT_HIT_COUNT_ENDING);
        }
        item->setText(BREAKPOINT_HIT_COUNT_COLUMN_NUMBER, hitCountLabel);
    }
}

void BreakpointManagerView::onBreakpointReached(ActorId actor) {
    QTreeWidgetItem *item = actorConnections.key(actor, NULL);
    Q_ASSERT(NULL != item);
    if(NULL != lastReachedBreakpoint) {
        setBreakpointBackgroundColor(lastReachedBreakpoint, BREAKPOINT_DEFAULT_COLOR);
    }
    setBreakpointBackgroundColor(item, BREAKPOINT_HIGHLIGHTING_COLOR);
    lastReachedBreakpoint = item;
}

bool BreakpointManagerView::eventFilter(QObject * /*object*/, QEvent *event) {
    CHECK(NULL != event, false);
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        CHECK(NULL != keyEvent, false);

        bool shiftPressed = keyEvent->modifiers().testFlag(Qt::ShiftModifier);
        if (shiftPressed && keyEvent->key() == Qt::Key_Delete) {
            sl_deleteAllBreakpoints();
        }
        if (keyEvent->matches(QKeySequence::Delete)) {
            sl_deleteSelectedBreakpoint();
        }
    }
    return true;
}

void BreakpointManagerView::setBreakpointBackgroundColor(QTreeWidgetItem *breakpoint,
    const QColor &newBackground)
{
    for(int columnNumber = 0; columnNumber < breakpointsList->columnCount(); ++columnNumber) {
        breakpoint->setBackgroundColor(columnNumber, newBackground);
    }
}

void BreakpointManagerView::paintEvent(QPaintEvent * /*event*/) {
    static const QColor nonActiveColor = breakpointsList->palette().window().color();
    if(!this->isEnabled()) {
        foreach(QTreeWidgetItem *item, breakpointsList->findItems(QString(), Qt::MatchContains)) {
            if(BREAKPOINT_DEFAULT_COLOR
                == item->backgroundColor(BREAKPOINT_LABELS_COLUMN_NUMBER))
            {
                setBreakpointBackgroundColor(item, nonActiveColor);
            }
        }
    } else {
        foreach(QTreeWidgetItem *item, breakpointsList->findItems(QString(), Qt::MatchContains)) {
            if(nonActiveColor == item->backgroundColor(BREAKPOINT_LABELS_COLUMN_NUMBER)) {
                setBreakpointBackgroundColor(item, BREAKPOINT_DEFAULT_COLOR);
            }
        }
    }
}

} // namespace U2
