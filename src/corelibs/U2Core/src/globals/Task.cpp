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

#include "Task.h"

#include <U2Core/AppResources.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

TaskResourceUsage::TaskResourceUsage(const QString& _resourceId, int _usage, const TaskResourceStage& _stage)
    : resourceId(_resourceId), resourceUse(_usage), stage(_stage) {
    SAFE_POINT(resourceId != UGENE_RESOURCE_ID_THREAD || stage == TaskResourceStage::Run, "Thread resource can be reserved only for Run stage", );
}

void TaskStateInfo::addWarning(const QString& warning) {
    QWriteLocker w(&lock);
    warnings << warning;
    taskLog.error(warning);
}

void TaskStateInfo::addWarnings(const QStringList& wList) {
    QWriteLocker w(&lock);
    warnings << wList;
    foreach (const QString& warning, wList) {
        taskLog.error(warning);
    }
}

void TaskStateInfo::insertWarnings(const QStringList& wList) {
    QWriteLocker w(&lock);
    warnings << wList;
}

static qint64 genTaskId() {
    // todo: add main-thread model check
    static qint64 id = 0;
    qint64 res = ++id;
    return res;
}

Task::Task(const QString& _name, TaskFlags f) {
    taskName = _name;
    state = State_New;
    tpm = Progress_SubTasksBased;
    flags = f;
    setVerboseOnTaskCancel(true);
    taskId = genTaskId();
    parentTask = nullptr;
    progressWeightAsSubtask = 1;
    maxParallelSubtasks = MAX_PARALLEL_SUBTASKS_SERIAL;
    insidePrepare = false;
}

void Task::setMaxParallelSubtasks(int n) {
    SAFE_POINT(n >= 0, QString("max parallel subtasks must be >=0, value passed: %1").arg(n), );
    maxParallelSubtasks = n;
}

void Task::setTaskName(const QString& _taskName) {
    SAFE_POINT(isNew(), "Can only change name for new tasks!", );
    taskName = _taskName;
}

void Task::cancel() {
    CHECK(!isFinished(), );
    foreach (Task* t, subtasks) {
        if (!t->isFinished()) {
            t->cancel();
        }
    }
    stateInfo.cancelFlag = true;
}

const QList<QPointer<Task>>& Task::getSubtasks() const {
    return subtasks;
}

QList<Task*> Task::getPureSubtasks() const {
    QList<Task*> subtasksPointers;
    subtasksPointers.reserve(subtasks.size());
    foreach (const QPointer<Task>& subtask, subtasks) {
        subtasksPointers << subtask.data();
    }
    return subtasksPointers;
}

void Task::addSubTask(Task* sub) {
    SAFE_POINT(sub != nullptr, "Trying to add NULL subtask", );
    SAFE_POINT(sub->parentTask == nullptr, "Task already has a parent!", );
    SAFE_POINT(state == State_New, "Parents can be assigned to tasks in NEW state only!", );

    sub->parentTask = this;
    subtasks.append(sub);
    emit si_subtaskAdded(sub);
}

void Task::cleanup() {
    SAFE_POINT(isFinished() || (isNew() && (isCanceled() || hasError())),
               QString("Cleanup can only be called for finished tasks. Task: %1, ,state: %2").arg(getTaskName()).arg(getState()), );
    foreach (const QPointer<Task>& sub, getSubtasks()) {
        CHECK_CONTINUE(!sub.isNull());
        sub->cleanup();
    }
}

bool Task::propagateSubtaskError() {
    if (hasError()) {
        return true;
    }
    Task* badChild = getSubtaskWithErrors();
    if (badChild != nullptr) {
        stateInfo.setError(badChild->getError());
    }
    return stateInfo.hasError();
}

Task* Task::getSubtaskWithErrors() const {
    foreach (const QPointer<Task>& sub, getSubtasks()) {
        if (sub->hasError()) {
            return sub.data();
        }
    }
    return nullptr;
}

QList<Task*> Task::onSubTaskFinished(Task*) {
    static QList<Task*> stub;
    return stub;
}

int Task::getNumParallelSubtasks() const {
    int nParallel = maxParallelSubtasks;
    assert(nParallel >= 0);
    if (nParallel == MAX_PARALLEL_SUBTASKS_AUTO) {
        nParallel = AppResourcePool::instance()->getIdealThreadCount();
    }
    assert(nParallel >= 1);
    return nParallel;
}

void Task::setMinimizeSubtaskErrorText(bool v) {
    assert(flags.testFlag(TaskFlag_FailOnSubtaskError));
    setFlag(TaskFlag_MinimizeSubtaskErrorText, v);
}

void Task::addTaskResource(const TaskResourceUsage& resource) {
    SAFE_POINT((resource.stage == TaskResourceStage::Prepare && state == Task::State_New && !insidePrepare) ||
                   (resource.stage == TaskResourceStage::Run && state < Task::State_Running),
               QString("Can't add task resource in current state: %1)").arg(getState()), );
    SAFE_POINT(!resource.locked, QString("Resource is already locked, resource id: %1").arg(resource.resourceId), );
    taskResources.append(resource);
}

bool Task::isMinimizeSubtaskErrorText() const {
    return getFlags().testFlag(TaskFlag_MinimizeSubtaskErrorText) || (parentTask != nullptr && parentTask->isMinimizeSubtaskErrorText());
}

void Task::setCollectChildrensWarningsFlag(bool v) {
    setFlag(TaskFlag_CollectChildrenWarnings, v);
    if (v) {
        foreach (Task* sub, subtasks) {
            sub->setCollectChildrensWarningsFlag(v);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// task scheduler

void TaskScheduler::addSubTask(Task* t, Task* sub) {
    SAFE_POINT(t != nullptr, "When adding subtask to TaskScheduler, the parent task is NULL", );
    SAFE_POINT(sub != nullptr, "When adding subtask to TaskScheduler, the subtask is NULL", );
    SAFE_POINT(sub->getParentTask() == nullptr, "Task already has a parent!", );

    if (t->hasFlags(TaskFlag_CollectChildrenWarnings)) {
        sub->setCollectChildrensWarningsFlag(true);
    }

    sub->parentTask = t;
    t->subtasks.append(sub);
    emit t->si_subtaskAdded(sub);
}

void TaskScheduler::setTaskState(Task* task, Task::State newState) {
    SAFE_POINT(task->getState() < newState,
               QString("Illegal task state change! Current state: %1, new state: %2, Task: %3")
                   .arg(task->getState())
                   .arg(newState)
                   .arg(task->getTaskName()), );

    task->state = newState;

    emit task->si_stateChanged();
    emit si_stateChanged(task);
}

void TaskScheduler::setTaskStateDesc(Task* t, const QString& desc) {
    t->stateInfo.setDescription(desc);
}

void TaskScheduler::setTaskInsidePrepare(Task* t, bool val) {
    t->insidePrepare = val;
}

}  // namespace U2
