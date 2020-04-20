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
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/L10n.h>
#include <U2Core/RemoveAnnotationsTask.h>
#include <U2Core/Settings.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "AutoAnnotationsSupport.h"

namespace U2 {

const QString AutoAnnotationObject::AUTO_ANNOTATION_HINT("auto-annotation object");

AutoAnnotationsUpdater::AutoAnnotationsUpdater(const QString &nm, const QString &gName, bool alwaysOff, bool translationDependent)
    : groupName(gName), name(nm), alwaysOffByDefault(alwaysOff), translationDependent(translationDependent) {
    checkedByDefault = alwaysOffByDefault ? false : AppContext::getSettings()
        ->getValue(AUTO_ANNOTATION_SETTINGS + groupName, false, true).toBool();
}

AutoAnnotationsUpdater::~AutoAnnotationsUpdater() {
    AppContext::getSettings()->setValue(AUTO_ANNOTATION_SETTINGS + groupName, checkedByDefault, true);
}

bool AutoAnnotationsUpdater::isTranslationDependent() {
    return translationDependent;
}

QList<AutoAnnotationsUpdater *> AutoAnnotationsSupport::getAutoAnnotationUpdaters() {
    return aaUpdaters;
}

AutoAnnotationsUpdater * AutoAnnotationsSupport::findUpdaterByGroupName(const QString& groupName) {
    foreach(AutoAnnotationsUpdater *updater, aaUpdaters) {
        if (groupName == updater->getGroupName()) {
            return updater;
        }
    }
    return NULL;
}

AutoAnnotationsUpdater * AutoAnnotationsSupport::findUpdaterByName(const QString &name) {
    foreach(AutoAnnotationsUpdater *updater, aaUpdaters) {
        if (updater->getName() == name) {
            return updater;
        }
    }
    return NULL;
}

//////////////////////////////////////////////////////////////////////////

void AutoAnnotationsSupport::registerAutoAnnotationsUpdater(AutoAnnotationsUpdater *updater) {
    aaUpdaters.append(updater);
}

void AutoAnnotationsSupport::unregisterAutoAnnotationsUpdater(AutoAnnotationsUpdater *updater) {
    aaUpdaters.removeOne(updater);
}

AutoAnnotationsSupport::~AutoAnnotationsSupport() {
    qDeleteAll(aaUpdaters);
}

bool AutoAnnotationsSupport::isAutoAnnotation(const AnnotationTableObject *obj) {
    return obj->getGHintsMap().value(AutoAnnotationObject::AUTO_ANNOTATION_HINT).toBool();
}

bool AutoAnnotationsSupport::isAutoAnnotation(const GObject* obj) {
    bool isAnnotationObject = obj->getGObjectType() == GObjectTypes::ANNOTATION_TABLE;
    bool hasAutoAnnotationHint = obj->getGHintsMap().value(AutoAnnotationObject::AUTO_ANNOTATION_HINT).toBool();
    return (isAnnotationObject && hasAutoAnnotationHint);
}

//////////////////////////////////////////////////////////////////////////

AutoAnnotationObject::AutoAnnotationObject(U2SequenceObject *obj, DNATranslation *aminoTT, QObject *parent)
    : QObject(parent), dnaObj(obj), aminoTT(aminoTT) {
    QVariantMap hints;
    hints.insert(AUTO_ANNOTATION_HINT, true);
    QString tableName = AutoAnnotationsSupport::tr("Auto-annotations [%1 | %2]")
        .arg(obj->getDocument()->getName()).arg(obj->getGObjectName());

    U2OpStatusImpl os;
    const U2DbiRef localDbiRef = AppContext::getDbiRegistry()->getSessionTmpDbiRef(os);
    SAFE_POINT_OP(os, );

    aobj = new AnnotationTableObject(tableName, localDbiRef, hints);
    aobj->addObjectRelation(dnaObj, ObjectRole_Sequence);
    aaSupport = AppContext::getAutoAnnotationsSupport();
}

AutoAnnotationObject::~AutoAnnotationObject() {
    U2OpStatusImpl os;

    const U2EntityRef &entity = aobj->getEntityRef();

    DbiConnection con(entity.dbiRef, os);
    con.dbi->getObjectDbi()->removeObject(entity.entityId, os);
    delete aobj;
    SAFE_POINT_OP(os, );
}

void AutoAnnotationObject::updateAll() {
    QList<AutoAnnotationsUpdater *> aaUpdaters = aaSupport->getAutoAnnotationUpdaters();
    handleUpdate(aaUpdaters);
}

void AutoAnnotationObject::updateTranslationDependent(DNATranslation *newAminoTT) {
    aminoTT = newAminoTT;
    QList<AutoAnnotationsUpdater *> aaUpdaters;
    foreach(AutoAnnotationsUpdater *updater, aaSupport->getAutoAnnotationUpdaters()) {
        if (updater->isTranslationDependent()) {
            aaUpdaters.append(updater);
        }
    }
    handleUpdate(aaUpdaters);
}

void AutoAnnotationObject::updateGroup(const QString &groupName) {
    AutoAnnotationsUpdater *updater = aaSupport->findUpdaterByGroupName(groupName);
    if (NULL != updater) {
        QList<AutoAnnotationsUpdater *> updaters;
        updaters << updater;
        handleUpdate(updaters);
    }
}

void AutoAnnotationObject::addNewUpdateTask(AutoAnnotationsUpdater *updater, Task *updateTask) {
    SAFE_POINT(NULL != updater, L10N::nullPointerError("Auto-annotation updater"), );
    SAFE_POINT(NULL != updateTask, L10N::nullPointerError("Auto-annotation update task"), );

    connect(updateTask, SIGNAL(si_stateChanged()), SLOT(sl_updateTaskFinished()));
    newUpdateTasks[updater].append(updateTask);
}

void AutoAnnotationObject::addRunningUpdateTask(AutoAnnotationsUpdater *updater, Task *updateTask) {
    SAFE_POINT(NULL != updater, L10N::nullPointerError("Auto-annotation updater"), );
    SAFE_POINT(NULL != updateTask, L10N::nullPointerError("Auto-annotation update task"), );

    connect(updateTask, SIGNAL(si_stateChanged()), SLOT(sl_updateTaskFinished()));
    runningUpdateTasks[updater].append(updateTask);
}

bool AutoAnnotationObject::cancelRunningUpdateTasks(AutoAnnotationsUpdater *updater) {
    SAFE_POINT(NULL != updater, L10N::nullPointerError("Auto-annotation updater"), false);

    const bool result = !runningUpdateTasks[updater].isEmpty();
    foreach(Task *task, runningUpdateTasks[updater]) {
        task->cancel();
    }

    qDeleteAll(newUpdateTasks[updater]);
    newUpdateTasks[updater].clear();

    return result;
}

void AutoAnnotationObject::handleUpdate(const QList<AutoAnnotationsUpdater *> &updaters) {
    QList<Task*> subTasks;
    QStringList groupNames;

    foreach(AutoAnnotationsUpdater *updater, updaters) {
        // check constraints
        AutoAnnotationConstraints cns;
        cns.alphabet = dnaObj->getAlphabet();
        cns.hints = dnaObj->getGHints();
        if (!updater->checkConstraints(cns)) {
            continue;
        }

        const bool deferredLaunch = cancelRunningUpdateTasks(updater);

        // cleanup
        AnnotationGroup *sub = aobj->getRootGroup()->getSubgroup(updater->getGroupName(), false);
        if (NULL != sub) {
            Task *t = new RemoveAnnotationsTask(aobj, updater->getGroupName());
            if (deferredLaunch) {
                addNewUpdateTask(updater, t);
            } else {
                addRunningUpdateTask(updater, t);
                subTasks.append(t);
            }
        }

        // update
        if (enabledGroups.contains(updater->getGroupName())) {
            // create update tasks
            Task *t = updater->createAutoAnnotationsUpdateTask(this);
            if (NULL != t) {
                if (deferredLaunch) {
                    addNewUpdateTask(updater, t);
                } else {
                    addRunningUpdateTask(updater, t);
                    subTasks.append(t);
                }
            }
        }
        groupNames.append(updater->getGroupName());

        // envelope to unlock annotation object
        if (!subTasks.isEmpty()) {
            AppContext::getTaskScheduler()->registerTopLevelTask(new AutoAnnotationsUpdateTask(this, subTasks));
            subTasks.clear();
        }
    }
}

void AutoAnnotationObject::sl_updateTaskFinished() {
    Task *task = qobject_cast<Task *>(sender());
    SAFE_POINT(NULL != task, L10N::nullPointerError("Auto-annotation update task"), );

    CHECK(task->isFinished(), );

    foreach(AutoAnnotationsUpdater *updater, runningUpdateTasks.keys()) {
        QList<Task *> &updateTasks = runningUpdateTasks[updater];
        if (updateTasks.contains(task)) {
            updateTasks.removeAll(task);

            if (updateTasks.isEmpty()) {
                runningUpdateTasks[updater] = newUpdateTasks[updater];
                newUpdateTasks[updater].clear();
                AppContext::getTaskScheduler()->registerTopLevelTask(new AutoAnnotationsUpdateTask(this, runningUpdateTasks[updater]));
            }
        }
    }
}

void AutoAnnotationObject::setGroupEnabled(const QString &groupName, bool enabled) {
    if (enabled) {
        enabledGroups.insert(groupName);
    } else {
        enabledGroups.remove(groupName);
    }
}

void AutoAnnotationObject::emitStateChange(bool started) {
    if (started) {
        emit si_updateStarted();
    } else {
        emit si_updateFinshed();
    }
}

DNATranslation* AutoAnnotationObject::getAminoTT() const {
    return aminoTT;
}

AutoAnnotationConstraints::AutoAnnotationConstraints()
    : alphabet(NULL), hints(NULL) {

}

//////////////////////////////////////////////////////////////////////////

const QString AutoAnnotationsUpdateTask::NAME("Auto-annotations update task");

AutoAnnotationsUpdateTask::AutoAnnotationsUpdateTask(AutoAnnotationObject *aaObj,
    QList<Task *> updateTasks)
    : Task(NAME, TaskFlags_NR_FOSE_COSC), aa(aaObj), aaSeqObj(NULL), lock(NULL),
    subTasks(updateTasks) {
    aaObjectInvalid = false;
    setMaxParallelSubtasks(1);
    connect(aaObj, SIGNAL(destroyed(QObject*)), SLOT(sl_onSequenceDeleted()));
}

AutoAnnotationsUpdateTask::~AutoAnnotationsUpdateTask() {
    cleanup();
}

void AutoAnnotationsUpdateTask::prepare() {
    SAFE_POINT(NULL != aa, tr("Empty auto-annotation object"), );

    lock = new StateLock("Auto-annotations update", StateLockFlag_LiveLock);
    aaSeqObj = aa->getSeqObject();
    aaSeqObj->lockState(lock);

    aa->emitStateChange(true);
    foreach(Task *subtask, subTasks) {
        addSubTask(subtask);
    }
}

void AutoAnnotationsUpdateTask::cleanup() {
    if (NULL != lock) {
        CHECK_EXT(!aa.isNull(), cancel(), );
        aaSeqObj->unlockState(lock);
        delete lock;
    }
}

Task::ReportResult AutoAnnotationsUpdateTask::report() {
    if (isCanceled() && aaObjectInvalid) {
        return ReportResult_Finished;
    }

    CHECK_EXT(!aa.isNull(), cancel(), ReportResult_Finished);
    aa->emitStateChange(false);

    return ReportResult_Finished;
}

U2SequenceObject* AutoAnnotationsUpdateTask::getSequenceObject() const {
    return aaSeqObj;
}

void AutoAnnotationsUpdateTask::sl_onSequenceDeleted() {
    if (NULL != lock) {
        aaSeqObj->unlockState(lock);
        delete lock;
        lock = NULL;
        cancel();
    }
}

} //namespace U2
