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

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/AutoAnnotationsSupport.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/MainWindow.h>
#include <U2Gui/ObjectViewModel.h>

#include <U2View/ADVAnnotationCreation.h>

#include "ADVSequenceObjectContext.h"
#include "AnnotatedDNAView.h"
#include "AutoAnnotationUtils.h"

namespace U2 {

const QString AutoAnnotationsADVAction::ACTION_NAME("AutoAnnotationUpdateAction");

#define AUTO_ANNOTATION_GROUP_NAME "AutoAnnotatationGroupName"

AutoAnnotationsADVAction::AutoAnnotationsADVAction(ADVSequenceWidget* v,
                                                   AutoAnnotationObject* obj)
: ADVSequenceWidgetAction(ACTION_NAME, tr("Automatic annotations highlighting")), aaObj(obj), updatesCount(0)
{
    seqWidget = v;
    addToBar = true;

    menu = new QMenu();
    setIcon(QIcon(":core/images/predefined_annotation_groups.png"));
    setMenu(menu);

    connect(aaObj, SIGNAL(si_updateStarted()), SLOT(sl_autoAnnotationUpdateStarted()));
    connect(aaObj, SIGNAL(si_updateFinshed()), SLOT(sl_autoAnnotationUpdateFinished()));

    selectAllAction= new QAction(tr("Select all"),this);
    connect(selectAllAction, SIGNAL(triggered()), SLOT(sl_onSelectAll()));

    deselectAllAction = new QAction(tr("Deselect all"),this);
    connect(deselectAllAction, SIGNAL(triggered()), SLOT(sl_onDeselectAll()));

    updateMenu();

    aaObj->updateAll();
}

#define MAX_SEQ_SIZE_TO_ENABLE_AUTO_ANNOTATIONS 10000

void AutoAnnotationsADVAction::updateMenu()
{
    AutoAnnotationConstraints constraints;
    if (seqWidget->getSequenceContexts().count() > 0) {
        constraints.alphabet = seqWidget->getSequenceContexts().first()->getAlphabet();
    }

    // Auto annotations should not be enabled by default for very large sequences
    // or when there are many small sequences. This flag controls such behavior.
    bool largeSequence = false;

    if(seqWidget->getSequenceObjects().count() > 0){
        constraints.hints = seqWidget->getSequenceObjects().first()->getGHints();

        int totalLen = 0;

        AnnotatedDNAView* view = seqWidget->getAnnotatedDNAView();
        if (view != NULL) {
            const QList<ADVSequenceObjectContext*>& ctxList = view->getSequenceContexts();
            foreach (const ADVSequenceObjectContext* ctx, ctxList) {
                totalLen += ctx->getSequenceLength();
            }
        }
        largeSequence = totalLen > MAX_SEQ_SIZE_TO_ENABLE_AUTO_ANNOTATIONS;
    }

    QList<AutoAnnotationsUpdater*> updaters = AppContext::getAutoAnnotationsSupport()->getAutoAnnotationUpdaters();
    if (updaters.count() == 0) {
        setEnabled(false);
        return;
    }
    foreach (AutoAnnotationsUpdater* updater, updaters) {
        QAction* toggleAction = new QAction(updater->getName(), this);
        toggleAction->setObjectName(updater->getName());
        toggleAction->setProperty(AUTO_ANNOTATION_GROUP_NAME, updater->getGroupName());
        bool enabled = updater->checkConstraints(constraints);
        toggleAction->setEnabled(enabled);
        toggleAction->setCheckable(true);
        bool checked = updater->isCheckedByDefault() && !largeSequence;
        toggleAction->setChecked(checked);
        aaObj->setGroupEnabled(updater->getGroupName(), checked);
        connect(toggleAction, SIGNAL(toggled(bool)), SLOT(sl_toggle(bool)));
        menu->addAction(toggleAction);
    }

    menu->update();
}

void AutoAnnotationsADVAction::sl_toggle(bool toggled)
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action == NULL) {
        return;
    }
    AutoAnnotationsUpdater* updater = AppContext::getAutoAnnotationsSupport()->findUpdaterByName(action->text());
    if (updater != NULL) {
        QString groupName = updater->getGroupName();
        aaObj->setGroupEnabled(groupName, toggled);
        aaObj->updateGroup(groupName);
        updater->setCheckedByDefault(toggled);
    }
}

void AutoAnnotationsADVAction::sl_onSelectAll() {
    QList<QAction*> actions = getToggleActions();
    foreach(QAction* action, actions) {
        if (!action->isChecked()) {
            action->trigger();
        }
    }
}

void AutoAnnotationsADVAction::sl_onDeselectAll()
{
    QList<QAction*> actions = getToggleActions();
    foreach(QAction* action, actions) {
        if (action->isChecked()) {
            action->trigger();
        }
    }
}

AutoAnnotationsADVAction::~AutoAnnotationsADVAction()
{
    menu->clear();
    delete menu;
    menu = NULL;
}

QList<QAction*> AutoAnnotationsADVAction::getToggleActions()
{
    return menu->actions();
}

QAction* AutoAnnotationsADVAction::findToggleAction(const QString& groupName)
{
    QList<QAction*> toggleActions = menu->actions();
    foreach(QAction* tAction, toggleActions) {
        if (tAction->property(AUTO_ANNOTATION_GROUP_NAME) == groupName) {
            return tAction;
        }
    }
    return NULL;
}

void AutoAnnotationsADVAction::addUpdaterToMenu(AutoAnnotationsUpdater* updater){
    AutoAnnotationConstraints constraints;
    if (seqWidget->getSequenceContexts().count() > 0) {
            constraints.alphabet = seqWidget->getSequenceContexts().first()->getAlphabet();
    }
    if(seqWidget->getSequenceObjects().count() > 0){
        constraints.hints = seqWidget->getSequenceObjects().first()->getGHints();
    }

    QAction* toggleAction = new QAction(updater->getName(), this);
    toggleAction->setProperty(AUTO_ANNOTATION_GROUP_NAME, updater->getGroupName());
    bool enabled = updater->checkConstraints(constraints);
    toggleAction->setEnabled(enabled);
    toggleAction->setCheckable(true);
    bool checked = updater->isCheckedByDefault();
    toggleAction->setChecked(checked);
    aaObj->setGroupEnabled(updater->getGroupName(), checked);
    connect(toggleAction, SIGNAL(toggled(bool)), SLOT(sl_toggle(bool)));
    menu->addAction(toggleAction);

    menu->update();
}

void AutoAnnotationsADVAction::sl_autoAnnotationUpdateStarted()
{
    setEnabled(false);
    updatesCount++;
}

void AutoAnnotationsADVAction::sl_autoAnnotationUpdateFinished()
{
    updatesCount--;
    if (updatesCount == 0) {
        setEnabled(true);
    }
}

//////////////////////////////////////////////////////////////////////////

QAction* AutoAnnotationUtils::findAutoAnnotationsToggleAction(ADVSequenceObjectContext* ctx, const QString& groupName)
{
    foreach(ADVSequenceWidget* w, ctx->getSequenceWidgets()) {
         ADVSequenceWidgetAction* advAction = w->getADVSequenceWidgetAction(AutoAnnotationsADVAction::ACTION_NAME);
         if (advAction == NULL) {
             continue;
         }
         AutoAnnotationsADVAction* aaAction = qobject_cast<AutoAnnotationsADVAction*> (advAction);
         assert(aaAction != NULL);
         QList<QAction*> toggleActions = aaAction->getToggleActions();
         foreach(QAction* tAction, toggleActions) {
             if (tAction->property(AUTO_ANNOTATION_GROUP_NAME) == groupName) {
                 return tAction;
             }
         }
    }

    return NULL;

}

void AutoAnnotationUtils::triggerAutoAnnotationsUpdate(ADVSequenceObjectContext* ctx, const QString& aaGroupName)
{
    AutoAnnotationsADVAction* aaAction = findAutoAnnotationADVAction(ctx);

    if (aaAction != NULL && !aaAction->isEnabled()) {
        return;
    }

    assert(aaAction != NULL);
    if (aaAction) {

        QAction* updateAction = aaAction->findToggleAction(aaGroupName);
        assert (updateAction != NULL);

        if (!updateAction) {
            return;
        }

        if (!updateAction->isChecked()) {
            updateAction->trigger();
        } else {
            AutoAnnotationsUpdater* updater = AppContext::getAutoAnnotationsSupport()->findUpdaterByGroupName(aaGroupName);
            if (updater != NULL) {
                aaAction->getAAObj()->updateGroup(aaGroupName);
            }
        }
    }
}

AutoAnnotationsADVAction* AutoAnnotationUtils::findAutoAnnotationADVAction(ADVSequenceObjectContext* ctx)
{
    foreach(ADVSequenceWidget* w, ctx->getSequenceWidgets()) {
        ADVSequenceWidgetAction* advAction = w->getADVSequenceWidgetAction(AutoAnnotationsADVAction::ACTION_NAME);
        if (advAction == NULL) {
            continue;
        } else {
            return qobject_cast<AutoAnnotationsADVAction*>(advAction);
        }
    }

    return NULL;
}

QList<QAction*> AutoAnnotationUtils::getAutoAnnotationToggleActions(ADVSequenceObjectContext* ctx)
{
    QList<QAction*> res;

    foreach(ADVSequenceWidget* w, ctx->getSequenceWidgets()) {
        ADVSequenceWidgetAction* advAction = w->getADVSequenceWidgetAction(AutoAnnotationsADVAction::ACTION_NAME);
        if (advAction == NULL) {
            continue;
        }
        AutoAnnotationsADVAction* aaAction = qobject_cast<AutoAnnotationsADVAction*> (advAction);
        assert(aaAction != NULL);
        res = aaAction->getToggleActions();


        int selectedCount = 0;
        foreach (QAction* a, res) {
            if (a->isChecked()) {
                selectedCount += 1;
            }
        }

        if (selectedCount == res.size()) {
            res.append(aaAction->getDeselectAllAction());
        } else {
            res.append(aaAction->getSelectAllAction());
        }
    }

    return res;
}

//////////////////////////////////////////////////////////////////////////

ExportAutoAnnotationsGroupTask::ExportAutoAnnotationsGroupTask(AnnotationGroup *ag,
    GObjectReference& ref, ADVSequenceObjectContext* ctx, const QString &annDescription)
    : Task("ExportAutoAnnotationsGroupTask", TaskFlags_NR_FOSCOE),
      aGroup(ag),
      aRef(ref),
      seqCtx(ctx),
      createTask(NULL),
      annDescription(annDescription)
{
    SAFE_POINT_EXT(NULL != ag, stateInfo.setError(tr("Invalid annotation group provided")), );
}

void ExportAutoAnnotationsGroupTask::prepare() {
    QList<Annotation *> annsToExport;
    aGroup->findAllAnnotationsInGroupSubTree(annsToExport);

    QList<SharedAnnotationData> aData;
    foreach (Annotation *a, annsToExport) {
        SharedAnnotationData data(new AnnotationData(*(a->getData())));
        U1AnnotationUtils::addDescriptionQualifier(data, annDescription);
        aData.append(data);
    }

    CHECK(!aData.isEmpty(), );

    createTask = new ADVCreateAnnotationsTask(seqCtx->getAnnotatedDNAView(), aRef, aGroup->getName(), aData);
    addSubTask(createTask);
}

QList<Task *> ExportAutoAnnotationsGroupTask::onSubTaskFinished(Task *subTask) {
    QList<Task *> res;

    if (!subTask->isFinished() || subTask->hasError() || subTask->isCanceled()) {
        return res;
    }

    if (subTask == createTask) {
        QAction *toggleAction = AutoAnnotationUtils::findAutoAnnotationsToggleAction(seqCtx, aGroup->getName());
        if (toggleAction != NULL && toggleAction->isChecked()) {
            toggleAction->trigger();
        }
    }

    return res;
}

} //namespace
