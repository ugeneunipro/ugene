/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "AutoAnnotationsSupport.h"

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

namespace U2 {

const QString AutoAnnotationObject::AUTO_ANNOTATION_HINT("auto-annotation object");

AutoAnnotationsUpdater::AutoAnnotationsUpdater(const QString& name, const QString& groupName, bool isCantBeEnabledByDefault, bool translationDependent)
    : groupName(groupName), name(name), canBeEnabledByDefault(!isCantBeEnabledByDefault), dependsOnAminoTranslation(translationDependent) {
    isOnByDefault = canBeEnabledByDefault && AppContext::getSettings()->getValue(AUTO_ANNOTATION_SETTINGS + groupName, false, true, true).toBool();
}

AutoAnnotationsUpdater::~AutoAnnotationsUpdater() {
    AppContext::getSettings()->setValue(AUTO_ANNOTATION_SETTINGS + groupName, isOnByDefault, true, true);
}

AutoAnnotationsUpdater* AutoAnnotationsSupport::findUpdaterByGroupName(const QString& groupName) {
    foreach (AutoAnnotationsUpdater* updater, updaterList) {
        if (groupName == updater->getGroupName()) {
            return updater;
        }
    }
    return nullptr;
}

AutoAnnotationsUpdater* AutoAnnotationsSupport::findUpdaterByName(const QString& name) {
    foreach (AutoAnnotationsUpdater* updater, updaterList) {
        if (updater->getName() == name) {
            return updater;
        }
    }
    return nullptr;
}

//////////////////////////////////////////////////////////////////////////

void AutoAnnotationsSupport::registerAutoAnnotationsUpdater(AutoAnnotationsUpdater* updater) {
    updaterList.append(updater);
}

AutoAnnotationsSupport::~AutoAnnotationsSupport() {
    qDeleteAll(updaterList);
}

bool AutoAnnotationsSupport::isAutoAnnotationObject(const AnnotationTableObject* obj) {
    return obj->getGHintsMap().value(AutoAnnotationObject::AUTO_ANNOTATION_HINT).toBool();
}

bool AutoAnnotationsSupport::isAutoAnnotationObject(const GObject* obj) {
    bool isAnnotationObject = obj->getGObjectType() == GObjectTypes::ANNOTATION_TABLE;
    bool hasAutoAnnotationHint = obj->getGHintsMap().value(AutoAnnotationObject::AUTO_ANNOTATION_HINT).toBool();
    return (isAnnotationObject && hasAutoAnnotationHint);
}

//////////////////////////////////////////////////////////////////////////

AutoAnnotationObject::AutoAnnotationObject(U2SequenceObject* obj, DNATranslation* aminoTT, QObject* parent)
    : QObject(parent), sequenceObject(obj), aminoTT(aminoTT) {
    QVariantMap hints;
    hints.insert(AUTO_ANNOTATION_HINT, true);
    QString tableName = AutoAnnotationsSupport::tr("Auto-annotations [%1 | %2]")
                            .arg(obj->getDocument()->getName())
                            .arg(obj->getGObjectName());

    U2OpStatusImpl os;
    const U2DbiRef localDbiRef = AppContext::getDbiRegistry()->getSessionTmpDbiRef(os);
    SAFE_POINT_OP(os, );

    annotationTableObject = new AnnotationTableObject(tableName, localDbiRef, hints);
    annotationTableObject->addObjectRelation(sequenceObject, ObjectRole_Sequence);
    aaSupport = AppContext::getAutoAnnotationsSupport();
}

AutoAnnotationObject::~AutoAnnotationObject() {
    U2OpStatusImpl os;

    const U2EntityRef& entity = annotationTableObject->getEntityRef();

    DbiConnection con(entity.dbiRef, os);
    con.dbi->getObjectDbi()->removeObject(entity.entityId, os);
    delete annotationTableObject;
    SAFE_POINT_OP(os, );
}

void AutoAnnotationObject::updateAll() {
    QList<AutoAnnotationsUpdater*> aaUpdaters = aaSupport->getAutoAnnotationUpdaters();
    handleUpdate(aaUpdaters);
}

void AutoAnnotationObject::updateTranslationDependent(DNATranslation* newAminoTT) {
    aminoTT = newAminoTT;
    QList<AutoAnnotationsUpdater*> updaters;
    foreach (AutoAnnotationsUpdater* updater, aaSupport->getAutoAnnotationUpdaters()) {
        if (updater->isDependsOnAminoTranslation()) {
            updaters.append(updater);
        }
    }
    handleUpdate(updaters);
}

void AutoAnnotationObject::updateGroup(const QString& groupName) {
    AutoAnnotationsUpdater* updater = aaSupport->findUpdaterByGroupName(groupName);
    if (updater != nullptr) {
        QList<AutoAnnotationsUpdater*> updaters;
        updaters << updater;
        handleUpdate(updaters);
    }
}

void AutoAnnotationObject::addNewUpdateTask(AutoAnnotationsUpdater* updater, Task* updateTask) {
    SAFE_POINT(updater != nullptr, L10N::nullPointerError("Auto-annotation updater"), );
    SAFE_POINT(updateTask != nullptr, L10N::nullPointerError("Auto-annotation update task"), );

    connect(updateTask, SIGNAL(si_stateChanged()), SLOT(sl_updateTaskFinished()));
    newUpdateTasks[updater].append(updateTask);
}

void AutoAnnotationObject::addRunningUpdateTask(AutoAnnotationsUpdater* updater, Task* updateTask) {
    SAFE_POINT(updater != nullptr, L10N::nullPointerError("Auto-annotation updater"), );
    SAFE_POINT(updateTask != nullptr, L10N::nullPointerError("Auto-annotation update task"), );

    connect(updateTask, SIGNAL(si_stateChanged()), SLOT(sl_updateTaskFinished()));
    runningUpdateTasks[updater].append(updateTask);
}

bool AutoAnnotationObject::cancelRunningUpdateTasks(AutoAnnotationsUpdater* updater) {
    SAFE_POINT(updater != nullptr, L10N::nullPointerError("Auto-annotation updater"), false);

    bool result = !runningUpdateTasks[updater].isEmpty();
    foreach (Task* task, runningUpdateTasks[updater]) {
        task->cancel();
    }

    qDeleteAll(newUpdateTasks[updater]);
    newUpdateTasks[updater].clear();

    return result;
}

void AutoAnnotationObject::handleUpdate(const QList<AutoAnnotationsUpdater*>& updaters) {
    foreach (AutoAnnotationsUpdater* updater, updaters) {
        QList<Task*> subTasks;
        // check constraints
        AutoAnnotationConstraints cns;
        cns.alphabet = sequenceObject->getAlphabet();
        cns.hints = sequenceObject->getGHints();
        if (!updater->checkConstraints(cns)) {
            continue;
        }

        const bool isDeferredLaunch = cancelRunningUpdateTasks(updater);

        // cleanup
        AnnotationGroup* sub = annotationTableObject->getRootGroup()->getSubgroup(updater->getGroupName(), false);
        if (sub != nullptr) {
            auto removeAnnotationsTask = new RemoveAnnotationsTask(annotationTableObject, updater->getGroupName());
            if (isDeferredLaunch) {
                addNewUpdateTask(updater, removeAnnotationsTask);
            } else {
                addRunningUpdateTask(updater, removeAnnotationsTask);
                subTasks.append(removeAnnotationsTask);
            }
        }

        // update
        if (enabledGroups.contains(updater->getGroupName())) {
            // create update tasks
            auto createAutoAnnotationsUpdateTask = updater->createAutoAnnotationsUpdateTask(this);
            if (createAutoAnnotationsUpdateTask != nullptr) {
                if (isDeferredLaunch) {
                    addNewUpdateTask(updater, createAutoAnnotationsUpdateTask);
                } else {
                    addRunningUpdateTask(updater, createAutoAnnotationsUpdateTask);
                    subTasks.append(createAutoAnnotationsUpdateTask);
                }
            }
        }
        // envelope to unlock annotation object
        if (!subTasks.isEmpty()) {
            AppContext::getTaskScheduler()->registerTopLevelTask(new AutoAnnotationsUpdateTask(this, subTasks));
        }
    }
}

void AutoAnnotationObject::sl_updateTaskFinished() {
    auto task = qobject_cast<Task*>(sender());
    SAFE_POINT(task != nullptr, L10N::nullPointerError("Auto-annotation update task"), );

    CHECK(task->isFinished(), );

    foreach (AutoAnnotationsUpdater* updater, runningUpdateTasks.keys()) {
        QList<Task*>& updateTasks = runningUpdateTasks[updater];
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

void AutoAnnotationObject::setGroupEnabled(const QString& groupName, bool enabled) {
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
        emit si_updateFinished();
    }
}

DNATranslation* AutoAnnotationObject::getAminoTT() const {
    return aminoTT;
}

AutoAnnotationConstraints::AutoAnnotationConstraints()
    : alphabet(nullptr), hints(nullptr) {
}

//////////////////////////////////////////////////////////////////////////

const QString AutoAnnotationsUpdateTask::NAME("Auto-annotations update task");

AutoAnnotationsUpdateTask::AutoAnnotationsUpdateTask(AutoAnnotationObject* autoAnnotationObject,
                                                     QList<Task*> updateTasks)
    : Task(NAME, TaskFlags_NR_FOSE_COSC | TaskFlag_SilentCancelOnShutdown),
      autoAnnotationObject(autoAnnotationObject), sequenceObject(nullptr), lock(nullptr),
      subTasks(updateTasks) {
    isAutoAnnotationObjectInvalid = false;
    setMaxParallelSubtasks(1);
    connect(autoAnnotationObject, SIGNAL(destroyed(QObject*)), SLOT(sl_onSequenceDeleted()));
}

AutoAnnotationsUpdateTask::~AutoAnnotationsUpdateTask() {
    cleanup();
}

void AutoAnnotationsUpdateTask::prepare() {
    SAFE_POINT(autoAnnotationObject != nullptr, "Empty auto-annotation object", );

    lock = new StateLock("Auto-annotations update", StateLockFlag_LiveLock);
    sequenceObject = autoAnnotationObject->getSequenceObject();
    sequenceObject->lockState(lock);

    autoAnnotationObject->emitStateChange(true);
    for (Task* subtask : qAsConst(subTasks)) {
        addSubTask(subtask);
    }
}

void AutoAnnotationsUpdateTask::cleanup() {
    if (lock != nullptr) {
        CHECK_EXT(!autoAnnotationObject.isNull(), cancel(), );
        sequenceObject->unlockState(lock);
        delete lock;
    }
}

Task::ReportResult AutoAnnotationsUpdateTask::report() {
    if (isCanceled() && isAutoAnnotationObjectInvalid) {
        return ReportResult_Finished;
    }

    CHECK_EXT(!autoAnnotationObject.isNull(), cancel(), ReportResult_Finished);
    autoAnnotationObject->emitStateChange(false);

    return ReportResult_Finished;
}

void AutoAnnotationsUpdateTask::sl_onSequenceDeleted() {
    if (lock != nullptr) {
        sequenceObject->unlockState(lock);
        delete lock;
        lock = nullptr;
        cancel();
    }
}

}  // namespace U2
