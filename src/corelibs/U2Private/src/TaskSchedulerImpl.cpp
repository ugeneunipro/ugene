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

#include "TaskSchedulerImpl.h"
#ifdef Q_OS_DARWIN
#    include "SleepPreventerMac.h"
#endif

#include <QCoreApplication>
#include <QVector>

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/Timer.h>
#include <U2Core/U2SafePoints.h>

/* TRANSLATOR U2::TaskSchedulerImpl */

#ifdef Q_CC_MSVC_NET
#    include <Windows.h>
const DWORD MS_VC_EXCEPTION = 0x406D1388;

#    pragma pack(push, 8)
typedef struct tagTHREADNAME_INFO {
    DWORD dwType;  // Must be 0x1000.
    LPCSTR szName;  // Pointer to name (in user addr space).
    DWORD dwThreadID;  // Thread ID (-1=caller thread).
    DWORD dwFlags;  // Reserved for future use, must be zero.
} THREADNAME_INFO;
#    pragma pack(pop)

void SetThreadName(DWORD dwThreadID, char* threadName) {
    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = threadName;
    info.dwThreadID = dwThreadID;
    info.dwFlags = 0;

    __try {
        RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
    }
}
#endif

const int GET_NEW_SUBTASKS_EVENT_TYPE = 10001;
const int TERMINATE_MESSAGE_LOOP_EVENT_TYPE = 10002;
const int PAUSE_THREAD_EVENT_TYPE = 10003;

namespace U2 {

#define UPDATE_TIMEOUT 100

TaskSchedulerImpl::TaskSchedulerImpl(AppResourcePool* rp) {
    resourcePool = rp;

    stateNames << tr("New") << tr("Prepared") << tr("Running") << tr("Finished");
    connect(&timer, &QTimer::timeout, this, &TaskSchedulerImpl::update);
    timer.start(UPDATE_TIMEOUT);

    stateChangesObserved = false;
    threadsResource = resourcePool->getResource(RESOURCE_THREAD);

    createSleepPreventer();
}

TaskSchedulerImpl::~TaskSchedulerImpl() {
    SAFE_POINT(topLevelTasks.empty(), "Top level task list is not empty.", );
    SAFE_POINT(priorityQueue.isEmpty(), "Priority queue is not empty.", );
    delete sleepPreventer;
}

void TaskSchedulerImpl::cancelTask(Task* task) {
    if (task->getState() < Task::State_Finished) {
        taskLog.trace(tr("Canceling task: %1").arg(task->getTaskName()));
        getTaskStateInfo(task).cancelFlag = true;
        resumeThreadWithTask(task);  // for the case when task's thread is paused. it should be resumed and terminated
        foreach (const QPointer<Task>& t, task->getSubtasks()) {
            cancelTask(t.data());
        }
    }
}

void TaskSchedulerImpl::cancelAllTasks() {
    foreach (Task* t, topLevelTasks) {
        cancelTask(t);
    }
}

static void onBadAlloc(Task* task) {
    task->setError(TaskSchedulerImpl::tr("There is not enough memory to finish the task."));
}

void TaskSchedulerImpl::propagateStateToParent(Task* task) {
    Task* parentTask = task->getParentTask();
    if (parentTask == nullptr) {
        return;
    }
    if (parentTask->hasError() || parentTask->isCanceled()) {
        return;
    }
    if (task->isCanceled() && parentTask->getFlags().testFlag(TaskFlag_FailOnSubtaskCancel)) {
        TaskStateInfo& tsi = getTaskStateInfo(parentTask);
        tsi.setError(tr("Subtask {%1} is canceled %2").arg(task->getTaskName()).arg(task->getError()));
    } else if (task->isCanceled() && parentTask->getFlags().testFlag(TaskFlag_CancelOnSubtaskCancel)) {
        cancelTask(parentTask);
    } else if (task->hasError() && parentTask->getFlags().testFlag(TaskFlag_FailOnSubtaskError)) {
        TaskStateInfo& tsi = getTaskStateInfo(parentTask);
        if (parentTask->isMinimizeSubtaskErrorText()) {
            tsi.setError(task->getError());
        } else {
            tsi.setError(tr("Subtask {%1} is failed: %2").arg(task->getTaskName()).arg(task->getError()));
        }
    }

    if (task->isFinished() && parentTask->hasFlags(TaskFlag_CollectChildrenWarnings)) {
        if (task->hasWarning()) {
            TaskStateInfo& tsi = getTaskStateInfo(parentTask);
            tsi.insertWarnings(task->getWarnings());
        }
    }
}

bool TaskSchedulerImpl::processFinishedTasks() {
    bool hasFinished = false;
    for (int i = priorityQueue.size(); --i >= 0;) {
        TaskInfo* ti = priorityQueue[i];
        TaskInfo* pti = ti->parentTaskInfo;
        Task::State state = ti->task->getState();
        SAFE_POINT(state != Task::State_Finished, QString("Task %1 state is not 'finished'.").arg(ti->task->getTaskName()), hasFinished);

        if (ti->task->getTimeOut() > 0) {
            int secsPassed = GTimer::secsBetween(ti->task->getTimeInfo().startTime, GTimer::currentTimeMicros());
            if (ti->task->getTimeOut() < secsPassed) {
                QString msg = QString("Timeout error, running %1 sec expected %2 sec.").arg(secsPassed).arg(ti->task->getTimeOut());
                taskLog.error(QString("Task {%1} %2 Cancelling task...").arg(ti->task->getTaskName()).arg(msg));

                ti->task->setError(msg);
                ti->task->cancel();
            }
        }
        // if the task was canceled -> cancel subtasks too
        if (ti->task->isCanceled() && !ti->subtasksWereCanceled) {
            ti->subtasksWereCanceled = true;
            foreach (const QPointer<Task>& t, ti->task->getSubtasks()) {
                cancelTask(t.data());
            }
        }

        if (state != Task::State_Running) {
            continue;
        }

        if (ti->selfRunFinished && ti->hasLockedRunResources) {
            releaseResources(ti, false);  // release resources for RUN stage
        }

        // update state desc
        updateTaskProgressAndDesc(ti);
        if (!readyToFinish(ti)) {  // the task can be finished only after all its subtasks finished and its run finished
            continue;
        }

        if (ti->wasPrepared) {
            try {
                Task::ReportResult res = ti->task->report();
                if (res == Task::ReportResult_CallMeAgain) {
                    continue;
                }
            } catch (const std::bad_alloc&) {
                onBadAlloc(ti->task);
            }
        }

        if (pti != nullptr && pti->task->hasFlags(TaskFlag_RunMessageLoopOnly) && pti->thread != nullptr && pti->thread->isPaused) {
            continue;
        }

        hasFinished = true;
        promoteTask(ti, Task::State_Finished);

#ifndef Q_OS_DARWIN
        QCoreApplication::processEvents();
#endif

        releaseResources(ti, true);  // release resources for PREPARE stage

        Task* task = ti->task;
        priorityQueue.removeAt(i);

        // Send TERMINATE_MESSAGE_LOOP_EVENT_TYPE to TaskFlag_RunMessageLoopOnly task with allocated thread.
        if (task->hasFlags(TaskFlag_RunMessageLoopOnly) && ti->thread != nullptr) {
            QCoreApplication::postEvent(ti->thread,
                                        new QEvent(static_cast<QEvent::Type>(TERMINATE_MESSAGE_LOOP_EVENT_TYPE)));
        }
        delete ti;  // task is removed from priority queue

        // notify parent that subtask finished, check if there are new subtasks from parent
        if (pti != nullptr) {
            Task* parentTask = pti->task;
            SAFE_POINT(parentTask != nullptr, "When notifying parentTask about finished task: parentTask is NULL", hasFinished);
            SAFE_POINT(task != nullptr, "When notifying parentTask about finished task: task is NULL", hasFinished);
            propagateStateToParent(task);
            QList<Task*> newSubTasks;
            if (parentTask->hasFlags(TaskFlag_RunMessageLoopOnly) && pti->thread != nullptr) {
                QCoreApplication::postEvent(pti->thread,
                                            new QEvent(static_cast<QEvent::Type>(GET_NEW_SUBTASKS_EVENT_TYPE)));
                while (!pti->thread->newSubtasksObtained && pti->thread->isRunning()) {
                    QCoreApplication::processEvents();
                }
                if (pti->thread->newSubtasksObtained) {
                    pti->thread->subtasksLocker.lock();
                    newSubTasks = pti->thread->unconsideredNewSubtasks;
                    pti->thread->newSubtasksObtained = false;
                    pti->thread->subtasksLocker.unlock();
                }
            } else {
                newSubTasks = onSubTaskFinished(parentTask, task);
            }
            foreach (Task* newSub, newSubTasks) {
                if (newSub != nullptr) {
                    pti->newSubtasks.append(newSub);
                    addSubTask(parentTask, newSub);
                } else {
                    taskLog.error(QString("Calling onSubTaskFinished from task {%1} with subtask {%2} returned list containing NULL, skipping").arg(parentTask->getTaskName()).arg(task->getTaskName()));
                }
            }
            if (!pti->newSubtasks.isEmpty()) {
                if (!tasksWithNewSubtasks.contains(pti)) {
                    tasksWithNewSubtasks.append(pti);
                }
            }
        }
    }
    return hasFinished;
}

void TaskSchedulerImpl::unregisterFinishedTopLevelTasks() {
    QList<Task*> tasksToDelete;
    foreach (Task* task, topLevelTasks) {
        if (task->getState() == Task::State_Finished) {
            tasksToDelete.append(task);
        }
    }
    foreach (Task* task, tasksToDelete) {
        unregisterTopLevelTask(task);
    }
}

void TaskSchedulerImpl::processNewSubtasks() {
    for (int i = 0, n = tasksWithNewSubtasks.size(); i < n; i++) {
        TaskInfo* ti = tasksWithNewSubtasks[i];
        SAFE_POINT(ti->newSubtasks.size() > 0, QString("No new subtasks to process for %1.").arg(ti->task->getTaskName()), );

        int nParallel = ti->task->getNumParallelSubtasks();
        int nNew = ti->newSubtasks.size();
        int nToRun = qMin(nParallel - ti->numActiveSubtasks(), nNew);
        int nRun = 0;
        for (int j = 0; j < nToRun; j++) {
            Task* newSub = ti->newSubtasks[j];
            bool ok = addToPriorityQueue(newSub, ti);
            if (ok) {
                ti->newSubtasks[j] = nullptr;
                nRun++;
            }
        }
        if (nRun == nNew) {
            ti->newSubtasks.clear();
            tasksWithNewSubtasks[i] = nullptr;
        } else if (nRun > 0) {
            ti->newSubtasks.removeAll(nullptr);
        }
    }
    tasksWithNewSubtasks.removeAll(nullptr);
}

void TaskSchedulerImpl::runReady() {
    foreach (TaskInfo* ti, priorityQueue) {
        Task* task = ti->task;
        Task::State state = task->getState();
        // Note: task is running if any of its subtasks is running
        SAFE_POINT(state == Task::State_Prepared || state == Task::State_Running, QString("Task %1 state is not 'prepared' or 'running'.").arg(task->getTaskName()), );
        if (task->getFlags().testFlag(TaskFlag_NoRun) || task->isCanceled() || task->hasError()) {
            if (state == Task::State_Prepared) {
                promoteTask(ti, Task::State_Running);
            }
            if (ti->thread == nullptr) {
                ti->selfRunFinished = true;
            }
            continue;
        }
        if (ti->thread != nullptr) {  // task is already running in a separate thread
            SAFE_POINT(state == Task::State_Running, QString("Task %1 state is not 'running'.").arg(ti->task->getTaskName()), );
            continue;
        }
        bool ready = task->hasFlags(TaskFlag_RunBeforeSubtasksFinished) || ti->numFinishedSubtasks == task->getSubtasks().size();
        if (!ready) {
            continue;
        }
        QString noResMessage = tryLockResources(ti->task, false, ti->hasLockedRunResources);
        if (!noResMessage.isEmpty()) {
            setTaskStateDesc(ti->task, noResMessage);
            continue;
        }
        if (state == Task::State_Prepared) {
            promoteTask(ti, Task::State_Running);
        }
        setTaskStateDesc(ti->task, "");
        if (ti->task->hasFlags(TaskFlag_RunInMainThread)) {
            try {
                ti->task->run();
            } catch (const std::bad_alloc&) {
                onBadAlloc(ti->task);
            }
            SAFE_POINT(ti->task->getState() == Task::State_Running, QString("Task %1 state is not 'running'.").arg(ti->task->getTaskName()), );
            ti->selfRunFinished = true;
        } else {
            runThread(ti);
        }
    }
}

void TaskSchedulerImpl::runThread(TaskInfo* ti) {
    SAFE_POINT(ti->task->getState() == Task::State_Running, QString("Task %1 state is not 'running'.").arg(ti->task->getTaskName()), );
    SAFE_POINT(!ti->task->getFlags().testFlag(TaskFlag_NoRun), QString("Task %1 with flag 'NoRun'.").arg(ti->task->getTaskName()), );
    SAFE_POINT(ti->task->hasFlags(TaskFlag_RunBeforeSubtasksFinished) || ti->numFinishedSubtasks == ti->task->getSubtasks().size(), 
                QString("There are unfinishd subtasks but task %1 have flag 'RunBeforeSubtasksFinished'.").arg(ti->task->getTaskName()), );
    SAFE_POINT(!ti->task->isCanceled(), QString("Task %1 is cancelled.").arg(ti->task->getTaskName()), );
    SAFE_POINT(!ti->task->hasError(), QString("Task %1 has errors.").arg(ti->task->getTaskName()), );
    SAFE_POINT(!ti->selfRunFinished, QString("Task %1 already run.").arg(ti->task->getTaskName()), );

    ti->thread = new TaskThread(ti);
    connect(ti->thread, &TaskThread::si_processMySubtasks, this, &TaskSchedulerImpl::sl_processSubtasks, Qt::BlockingQueuedConnection);
    connect(ti->thread, &TaskThread::finished, this, &TaskSchedulerImpl::sl_threadFinished);
    ti->thread->start();
}

QString TaskSchedulerImpl::tryLockResources(Task* task, bool prepareStage, bool& hasLockedResourcesAfterCall) {
    QString errorString = QString();

    if (prepareStage) {  // task must be New
        SAFE_POINT(task->getState() == Task::State_New, "Attempt to lock prepare-stage resources for non-NEW task!", L10N::internalError());
    } else {  // task must be Prepared or Running. Task can be 'Running' if it has subtasks
        SAFE_POINT(task->getState() == Task::State_Running || task->getState() == Task::State_Prepared,
                   QString("Attempt to lock run-stage for task in state: %1!").arg(task->getState()),
                   L10N::internalError());
    }
    bool isThreadResourceAcquired = false;
    // TaskFlag_RunMessageLoopOnly is not a computational task but a message loop (WD scheduling) task.
    // We can't reserve threads for TaskFlag_RunMessageLoopOnly because it may lead to deadlocks when no thread is available for the WD scheduler
    // to work but there are child tasks with locked threads waiting for instructions from the WD scheduler.
    bool isThreadResourceNeeded = !prepareStage && !task->hasFlags(TaskFlag_RunMessageLoopOnly);
    if (isThreadResourceNeeded) {
        if (!threadsResource->tryAcquire()) {
            return tr("Waiting for resource '%1', count: %2").arg(threadsResource->name).arg(1);
        }
        isThreadResourceAcquired = true;
    }

    TaskResources& tres = getTaskResources(task);
    QVector<int> lockedResourceCount(tres.size());

    for (int i = 0, n = tres.size(); i < n; i++) {
        TaskResourceUsage& taskRes = tres[i];
        if (taskRes.prepareStageLock != prepareStage) {
            SAFE_POINT(prepareStage ? !taskRes.locked : taskRes.locked, QString("Task %1 lock state is not correct.").arg(task->getTaskName()), L10N::internalError());
            continue;
        }
        SAFE_POINT(!prepareStage || taskRes.resourceId != RESOURCE_THREAD, QString("Task %1 resouce id belongs to wrong thread.").arg(task->getTaskName()), L10N::internalError());
        AppResource* appRes = resourcePool->getResource(taskRes.resourceId);
        if (!appRes) {
            task->setError(tr("No required resources for the task, resource id: '%1'").arg(taskRes.resourceId));
            errorString = tr("Unable to run test because required resource not found");
            break;
        }

        bool resourceAcquired = appRes->tryAcquire(taskRes.resourceUse);
        if (!resourceAcquired) {
            if (appRes->maxTaskUse() < taskRes.resourceUse) {
                QString error = tr("Not enough resources for the task, resource name: '%1' max: %2%3 requested: %4%5")
                                    .arg(appRes->name)
                                    .arg(appRes->maxTaskUse())
                                    .arg(appRes->suffix)
                                    .arg(taskRes.resourceUse)
                                    .arg(appRes->suffix);
                if (!taskRes.errorMessage.isEmpty()) {
                    coreLog.error(error);
                    error = taskRes.errorMessage;
                }
                task->setError(error);
            }
            errorString = tr("Waiting for resource '%1', count: %2%3").arg(appRes->name).arg(taskRes.resourceUse).arg(appRes->suffix);
            break;
        } else {
            taskRes.locked = true;
            lockedResourceCount[i] = taskRes.resourceUse;
        }
    }

    if (errorString.isNull()) {
        hasLockedResourcesAfterCall = true;
        return errorString;
    }

    // releasing all locked resources
    for (int i = 0; i < tres.size(); i++) {
        TaskResourceUsage& taskRes = tres[i];

        AppResource* appRes = resourcePool->getResource(taskRes.resourceId);
        if (appRes && taskRes.locked) {
            appRes->release(lockedResourceCount[i]);
        }
        taskRes.locked = false;
    }
    if (isThreadResourceAcquired) {
        threadsResource->release();
    }

    hasLockedResourcesAfterCall = false;
    return errorString;
}

void TaskSchedulerImpl::releaseResources(TaskInfo* ti, bool prepareStage) {
    SAFE_POINT(ti->task->getState() == (prepareStage ? Task::State_Finished : Task::State_Running), "Releasing task resources in illegal state!", );
    if (!(prepareStage ? ti->hasLockedPrepareResources : ti->hasLockedRunResources)) {
        return;
    }
    bool isThreadResourceUsed = !prepareStage && !ti->task->hasFlags(TaskFlag_RunMessageLoopOnly);
    if (isThreadResourceUsed) {
        threadsResource->release();
    }
    TaskResources& tres = getTaskResources(ti->task);
    for (int i = 0, n = tres.size(); i < n; i++) {
        TaskResourceUsage& taskRes = tres[i];
        if (taskRes.prepareStageLock != prepareStage) {
            SAFE_POINT(prepareStage ? !taskRes.locked : taskRes.locked, QString("Task %1 lock state is not correct.").arg(ti->task->getTaskName()), );
            continue;
        }
        AppResource* appRes = resourcePool->getResource(taskRes.resourceId);
        appRes->release(taskRes.resourceUse);
        taskRes.locked = false;
    }
    if (prepareStage) {
        ti->hasLockedPrepareResources = false;
    } else {
        ti->hasLockedRunResources = false;
    }
}

void TaskSchedulerImpl::update() {
    if (isInsideSchedulingUpdate) {
        return;
    }
    isInsideSchedulingUpdate = true;
    stateChangesObserved = false;

    bool finishedFound = processFinishedTasks();
    if (finishedFound) {
        unregisterFinishedTopLevelTasks();
    }
    processNewSubtasks();

    prepareNewTasks();

    runReady();

    updateOldTasksPriority();

    if (stateChangesObserved) {
        stateChangesObserved = false;
        timer.setInterval(0);
    } else if (timer.interval() != UPDATE_TIMEOUT) {
        timer.setInterval(UPDATE_TIMEOUT);
    }

    isInsideSchedulingUpdate = false;
}

void TaskSchedulerImpl::prepareNewTasks() {
    if (newTasks.empty()) {
        return;
    }
    QList<Task*> newCopy = newTasks;
    newTasks.clear();
    foreach (Task* task, newCopy) {
        if (task->hasError() || task->isCanceled()) {  // check if its canceled or has errors
            propagateStateToParent(task);

            // forget about this task
            TaskInfo pti(task, 0);
            finishSubtasks(&pti);
            promoteTask(&pti, Task::State_Finished);

            if (task->isTopLevelTask()) {
                unregisterTopLevelTask(task);
            }
            continue;
        }
        bool ok = addToPriorityQueue(task, nullptr);
        if (!ok) {
            newTasks.append(task);
        }
    }
}

void TaskSchedulerImpl::registerTopLevelTask(Task* task) {
    SAFE_POINT(task, QString("Trying to register NULL task"), );

    QThread* appThread = QCoreApplication::instance()->thread();
    QThread* thisThread = QThread::currentThread();
    QThread* taskThread = task->thread();
    SAFE_POINT(thisThread == appThread, "Current task %1 thread is not app thread.", );
    SAFE_POINT(taskThread == appThread, QString("Task %1 thread is app not app thread.").arg(task->getTaskName()), );

    SAFE_POINT(task->getState() == Task::State_New, QString("Trying to register task in not NEW state. State: %1").arg(task->getState()), );
    SAFE_POINT(!topLevelTasks.contains(task), QString("Task is already registered: %1").arg(task->getTaskName()), );

    taskLog.details(tr("Registering new task: %1").arg(task->getTaskName()));
    topLevelTasks.append(task);
    emit si_topLevelTaskRegistered(task);
    newTasks.append(task);
    sleepPreventer->capture();
}

bool TaskSchedulerImpl::addToPriorityQueue(Task* task, TaskInfo* pti) {
    if (pti != nullptr && (pti->task->isCanceled() || pti->task->hasError())) {  // canceled tasks are not processed
        task->cancel();
    }

    // check if there are enough resources;
    bool runPrepare = !task->isCanceled() && !task->hasError();
    bool lr = false;
    if (runPrepare) {
        QString noResMessage = tryLockResources(task, true, lr);
        if (!noResMessage.isEmpty()) {
            setTaskStateDesc(task, noResMessage);
            if (!task->hasError()) {
                return false;  // call again
            } else {
                runPrepare = false;  // resource lock error
            }
        }
    }

    TaskInfo* ti = new TaskInfo(task, pti);
    ti->hasLockedPrepareResources = lr;
    priorityQueue.append(ti);
    if (runPrepare) {
        setTaskInsidePrepare(task, true);
        try {
            task->prepare();
        } catch (const std::bad_alloc&) {
            onBadAlloc(task);
        }
        setTaskInsidePrepare(task, false);
        ti->wasPrepared = true;
    }
    promoteTask(ti, Task::State_Prepared);

    int nParallel = task->getNumParallelSubtasks();
    const QList<QPointer<Task>>& subtasks = task->getSubtasks();
    for (int i = 0, n = subtasks.size(); i < n; i++) {
        Task* sub = subtasks[i].data();
        bool ok = i < nParallel && addToPriorityQueue(sub, ti);
        if (!ok && (!sub->hasError() || sub->getTaskResources().count() == 0)) {  // if task got err on resource allocation -> its not new now, but failed
            ti->newSubtasks.append(sub);
            if (!tasksWithNewSubtasks.contains(ti)) {
                tasksWithNewSubtasks.append(ti);
            }
        }
    }
    return true;
}

void TaskSchedulerImpl::unregisterTopLevelTask(Task* task) {
    SAFE_POINT(task != nullptr, "Trying to unregister NULL task", );
    SAFE_POINT(topLevelTasks.contains(task), QString("Trying to unregister task that is not top-level"), );

    taskLog.trace(tr("Unregistering task: %1").arg(task->getTaskName()));
    stopTask(task);
    topLevelTasks.removeOne(task);

    emit si_topLevelTaskUnregistered(task);

    if (!task->hasFlags(TaskFlag_NoAutoDelete)) {
        deleteTask(task);
    }

    sleepPreventer->release();
}

void TaskSchedulerImpl::stopTask(Task* task) {
    foreach (const QPointer<Task>& sub, task->getSubtasks()) {
        stopTask(sub.data());
    }

    foreach (TaskInfo* ti, priorityQueue) {  // stop task if its running
        if (ti->task == task) {
            cancelTask(task);
            if (ti->thread != nullptr && !ti->thread->isFinished()) {
                ti->thread->wait();  // TODO: try avoid blocking here
            }
            SAFE_POINT(readyToFinish(ti), QString("Task %1 is not ready to finish.").arg(ti->task->getTaskName()), );
            break;
        }
    }
}

bool TaskSchedulerImpl::readyToFinish(TaskInfo* ti) {
    if (ti->task->getState() == Task::State_Finished) {
        return true;
    }
    if (ti->task->getState() != Task::State_Running) {
        return false;
    }
    if (ti->numFinishedSubtasks < ti->task->getSubtasks().size()) {
        return false;
    }
    if (!ti->selfRunFinished) {
        return false;
    }
#ifdef _DEBUG
    foreach (const QPointer<Task>& sub, ti->task->getSubtasks()) {  // must be true because of 'numFinishedSubtasks' check above
        SAFE_POINT(sub->getState() == Task::State_Finished, QString("Subtask %1 state is not 'finished'.").arg(sub->getTaskName()), false);
    }
#endif
    SAFE_POINT(ti->newSubtasks.isEmpty(), QString("Task %1 have subtasks to start.").arg(ti->task->getTaskName()), false);
    return true;
}

QString TaskSchedulerImpl::getStateName(Task* t) const {
    Task::State s = t->getState();
    return stateNames[s];
}

static QString state2String(Task::State state) {
    switch (state) {
        case Task::State_New:
            return TaskSchedulerImpl::tr("New");
        case Task::State_Prepared:
            return TaskSchedulerImpl::tr("Prepared");
        case Task::State_Running:
            return TaskSchedulerImpl::tr("Running");
        case Task::State_Finished:
            return TaskSchedulerImpl::tr("Finished");
        default:
            SAFE_POINT(false, "Unexpected task state.", L10N::internalError());
    }
    return TaskSchedulerImpl::tr("Invalid name");
}

void TaskSchedulerImpl::checkSerialPromotion(TaskInfo* pti, Task* subtask) {
    //must be promoted at this point -> check algorithm depends requirement
    SAFE_POINT(!subtask->isNew(), QString("Subtask %1 is not promoted to 'new' state.").arg(subtask->getTaskName()), );  
    Task* task = pti == nullptr ? nullptr : pti->task;
    CHECK(task != nullptr, );

#ifdef _DEBUG
    int nParallel = task->getNumParallelSubtasks();
    bool before = true;
    int numActive = 0;
    const QList<QPointer<Task>>& subs = task->getSubtasks();
    for (int i = 0, n = subs.size(); i < n; i++) {
        const QPointer<Task>& sub = subs[i];
        if (!sub->isNew() && !sub->isFinished()) {
            numActive++;
            SAFE_POINT(numActive <= nParallel, "More parallel tasks active than allowed.", );
        }
        if (sub.data() == subtask) {
            before = false;
        } else if (before) {
            Task::State subState = sub->getState();
            // There may be "locked" subtasks (requires some resources) before
            // the current task (that is not "locked"). In this case their
            // state would be "New"
            if (sub->getTaskResources().size() == 0) {
                SAFE_POINT(subState != Task::State_New || sub->hasError(), QString("Subtask %1 has not 'new' state, or has errors.").arg(sub->getTaskName()), );
            }
        }
    }
#endif
}

void TaskSchedulerImpl::createSleepPreventer() {
#ifndef Q_OS_DARWIN
    sleepPreventer = new SleepPreventer;
#else
    sleepPreventer = new SleepPreventerMac;
#endif
}

static void checkFinishedState(TaskInfo* ti) {
#ifdef _DEBUG
    foreach (const QPointer<Task>& sub, ti->task->getSubtasks()) {
        SAFE_POINT(sub->getState() == Task::State_Finished, QString("Subtask %1 state is not 'finished'.").arg(sub->getTaskName()), );
    }
#endif
    SAFE_POINT(ti->newSubtasks.empty(), QString("There are new subtasks for %1 task.").arg(ti->task->getTaskName()), );
    SAFE_POINT(ti->numFinishedSubtasks == ti->task->getSubtasks().size(), 
        QString("Number of finished subtasks for %1 task is not equal subtasks count.").arg(ti->task->getTaskName()), );
    SAFE_POINT(ti->numRunningSubtasks == 0, QString("There are running subtasks for %1 task.").arg(ti->task->getTaskName()), );
    SAFE_POINT(ti->numPreparedSubtasks == 0, QString("There are prepared subtasks for %1 task.").arg(ti->task->getTaskName()), );
}

void TaskSchedulerImpl::promoteTask(TaskInfo* ti, Task::State newState) {
    stateChangesObserved = true;

    Task* task = ti->task;
    SAFE_POINT(newState > task->getState(), QString("Task %1 new state is not folowing task's lifecycle order.").arg(ti->task->getTaskName()), );

    setTaskState(task, newState);  // emits signals

    TaskStateInfo& tsi = getTaskStateInfo(task);
    TaskTimeInfo& tti = getTaskTimeInfo(task);
    TaskInfo* pti = ti->parentTaskInfo;

    if (!tsi.hasError()) {
        taskLog.trace(tr("Promoting task {%1} to '%2'").arg(task->getTaskName()).arg(state2String(newState)));
    } else {
        taskLog.trace(tr("Promoting task {%1} to '%2', error '%3'").arg(task->getTaskName()).arg(state2String(newState)).arg(tsi.getError()));
    }

    checkSerialPromotion(pti, ti->task);
    switch (newState) {
        case Task::State_Prepared:
            if (pti != nullptr) {
                pti->numPreparedSubtasks++;
            }
            if (ti->task->isTopLevelTask() && ti->task->isVerboseLogMode()) {
                taskLog.info(tr("Starting {%1} task").arg(ti->task->getTaskName()));
            }
            break;
        case Task::State_Running:
            tti.startTime = GTimer::currentTimeMicros();
            if (pti != nullptr) {
                pti->numPreparedSubtasks--;
                pti->numRunningSubtasks++;
                if (pti->task->getState() < Task::State_Running) {
                    SAFE_POINT(pti->task->getState() == Task::State_Prepared, QString("Parent task %1 is not in 'prepared' state.").arg(pti->task->getTaskName()), );
                    promoteTask(pti, Task::State_Running);
                }
            }
            break;
        case Task::State_Finished:
            checkFinishedState(ti);
            tti.finishTime = GTimer::currentTimeMicros();
            tsi.setDescription(QString());
            if (pti != nullptr) {
                if (ti->selfRunFinished) {
                    pti->numRunningSubtasks--;
                }
                SAFE_POINT(pti->numRunningSubtasks >= 0, QString("Parent %1 task has negative value of running subtasks.").arg(pti->task->getTaskName()), );
                pti->numFinishedSubtasks++;
                SAFE_POINT(pti->numFinishedSubtasks <= pti->task->getSubtasks().size(), QString("Parent %1 finished subtasks count more than it subtasks.").arg(pti->task->getTaskName()), );
            }
            if (ti->task->isTopLevelTask()) {
                if (tsi.hasError() && !tsi.cancelFlag) {
                    taskLog.error(tr("Task {%1} finished with error: %2").arg(task->getTaskName()).arg(tsi.getError()));
                } else if (tsi.cancelFlag) {
                    if (ti->task->isVerboseOnTaskCancel()) {
                        taskLog.info(tr("Task {%1} canceled").arg(ti->task->getTaskName()));
                    }
                } else if (ti->task->isVerboseLogMode()) {
                    taskLog.info(tr("Task {%1} finished").arg(ti->task->getTaskName()));
                }
            }
            break;
        default:
            SAFE_POINT(false, "Unexpected task state.", );
    }
#ifdef _DEBUG
    Task* parentTask = task->getParentTask();
    if (parentTask != nullptr) {
        int localPreparedSubs = 0;
        int localRunningSubs = 0;
        int localFinishedSubs = 0;
        int localNewSubs = 0;
        int localTotalSubs = 0;
        foreach (const QPointer<Task>& sub, parentTask->getSubtasks()) {
            switch (sub->getState()) {
                case Task::State_New:
                    localNewSubs++;
                    break;
                case Task::State_Prepared:
                    localPreparedSubs++;
                    break;
                case Task::State_Running:
                    localRunningSubs++;
                    break;
                case Task::State_Finished:
                    localFinishedSubs++;
                    break;
            }
            localTotalSubs++;
        }
        SAFE_POINT(localPreparedSubs == pti->numPreparedSubtasks, 
            QString("Local prepared subtask count for %1 task is not equal parents prepared subtasks.").arg(pti->task->getTaskName()), );
        SAFE_POINT(localRunningSubs == pti->numRunningSubtasks, 
            QString("Local running subtask count for %1 task is not equal parents running subtasks.").arg(pti->task->getTaskName()), );
        SAFE_POINT(localPreparedSubs + localRunningSubs == pti->numActiveSubtasks(), 
            QString("Local active (running and prepared) subtask count for %1 task is not equal parents active subtasks.").arg(pti->task->getTaskName()), );
        SAFE_POINT(localFinishedSubs == pti->numFinishedSubtasks, 
            QString("Local finished subtask count for %1 task is not equal parents finished subtasks.").arg(pti->task->getTaskName()), );
        SAFE_POINT(localTotalSubs == localNewSubs + localPreparedSubs + localRunningSubs + localFinishedSubs, 
            QString("Local total subtask count for %1 task is not equal sum of counters.").arg(pti->task->getTaskName()), );
    }
#endif
    updateTaskProgressAndDesc(ti);
}

void TaskSchedulerImpl::updateTaskProgressAndDesc(TaskInfo* ti) {
    Task* task = ti->task;
    TaskStateInfo& tsi = getTaskStateInfo(task);

    // update desc
    if (ti->task->useDescriptionFromSubtask()) {
        const QList<QPointer<Task>>& subs = task->getSubtasks();
        if (!subs.isEmpty()) {
            const QPointer<Task>& sub = subs.last();
            tsi.setDescription(sub->getStateInfo().getDescription());
        }
    }
    QString currentDesc = tsi.getDescription();
    if (currentDesc != ti->prevDesc) {
        ti->prevDesc = currentDesc;
        emit_taskDescriptionChanged(task);
    }

    // update progress
    int newProgress = tsi.progress;
    bool updateProgress = false;
    if (task->isFinished()) {
        newProgress = 100;
        updateProgress = tsi.progress != newProgress;
    } else if (task->getProgressManagementType() == Task::Progress_Manual) {
        int prevProgress = ti->prevProgress;
        if (tsi.progress != prevProgress) {
            ti->prevProgress = tsi.progress;
            emit_taskProgressChanged(task);
        }
    } else {
        SAFE_POINT(task->getProgressManagementType() == Task::Progress_SubTasksBased, QString("Progress managment type for %1 task is not 'subtask based'.").arg(task->getTaskName()), );
        const QList<QPointer<Task>>& subs = task->getSubtasks();
        int nsubs = subs.size();
        if (nsubs > 0 && !task->isCanceled()) {
            float sum = 0;
            float maxSum = 0.001F;
            foreach (const QPointer<Task>& sub, subs) {
                float w = sub->getSubtaskProgressWeight();
                sum += sub->getProgress() * w;
                maxSum += w;
            }
            if (maxSum > 1.0f) {
                newProgress = qRound(sum / maxSum);
            } else {
                newProgress = qRound(sum);
            }
        }
        updateProgress = tsi.progress != newProgress;
    }

    if (updateProgress) {
        tsi.progress = newProgress;
        emit_taskProgressChanged(task);
    }
}

void TaskSchedulerImpl::deleteTask(Task* task) {
    SAFE_POINT(task != nullptr, "Trying to delete NULL task", );
    foreach (const QPointer<Task>& sub, task->getSubtasks()) {
        // todo: check subtask autodelete ??
        deleteTask(sub.data());
    }
    taskLog.trace(tr("Deleting task: %1").arg(task->getTaskName()));
    task->deleteLater();
}

void TaskSchedulerImpl::finishSubtasks(TaskInfo* pti) {
    foreach (const QPointer<Task>& sub, pti->task->getSubtasks()) {
        TaskInfo ti(sub.data(), pti);
        finishSubtasks(&ti);
        promoteTask(&ti, Task::State_Finished);
    }
}

#define MAX_SECS_TO_LOWER_PRIORITY 60
static QThread::Priority getThreadPriority(Task* t) {
    SAFE_POINT(t->isTopLevelTask(), QString("Task %1 is not top level.").arg(t->getTaskName()), QThread::LowestPriority);
    SAFE_POINT(t->isRunning(), QString("Task %1 is not running.").arg(t->getTaskName()), QThread::LowestPriority);
    int secsPassed = GTimer::secsBetween(t->getTopLevelParentTask()->getTimeInfo().startTime, GTimer::currentTimeMicros());
    if (secsPassed > MAX_SECS_TO_LOWER_PRIORITY) {
        return QThread::LowestPriority;
    }
    return QThread::LowPriority;
}

static void updateThreadPriority(TaskInfo* ti) {
    SAFE_POINT(ti->thread != nullptr, QString("Task %1 thread is null.").arg(ti->task->getTaskName()), );
    QThread::Priority tp = getThreadPriority(ti->task->getTopLevelParentTask());
    if (ti->thread->priority() != tp && ti->thread->isRunning()) {
        ti->thread->setPriority(tp);
    }
}

void TaskSchedulerImpl::updateOldTasksPriority() {
// work every N-th tick
#define UPDATE_GRAN 10
    static int n = UPDATE_GRAN;
    if (--n != 0) {
        return;
    }
    n = UPDATE_GRAN;

    foreach (TaskInfo* ti, priorityQueue) {
        if (!ti->task->isRunning() || ti->thread == nullptr || !ti->thread->isRunning()) {
            continue;
        }
        updateThreadPriority(ti);
    }
}

void TaskSchedulerImpl::sl_threadFinished() {
    timer.setInterval(0);
}

void TaskSchedulerImpl::sl_processSubtasks() {
    TaskThread* taskThread = qobject_cast<TaskThread*>(sender());
    foreach (const QPointer<Task>& subtask, taskThread->ti->task->getSubtasks()) {
        if (subtask->isFinished() && !taskThread->getProcessedSubtasks().contains(subtask)) {
            onSubTaskFinished(taskThread, subtask.data());
            taskThread->appendProcessedSubtask(subtask.data());
            break;
        }
    }
}

void TaskSchedulerImpl::pauseThreadWithTask(const Task* task) {
    foreach (TaskInfo* ti, priorityQueue) {
        if (task == ti->task) {
            QCoreApplication::postEvent(ti->thread,
                                        new QEvent(static_cast<QEvent::Type>(PAUSE_THREAD_EVENT_TYPE)));
        }
    }
}

void TaskSchedulerImpl::resumeThreadWithTask(const Task* task) {
    foreach (TaskInfo* ti, priorityQueue) {
        if (task == ti->task && nullptr != ti->thread && ti->thread->isPaused) {
            ti->thread->resume();
        }
    }
}

void TaskSchedulerImpl::onSubTaskFinished(TaskThread* thread, Task* subtask) {
    if (thread->ti->task->hasFlags(TaskFlag_RunMessageLoopOnly) && nullptr != subtask && !thread->newSubtasksObtained) {
        thread->subtasksLocker.lock();
        try {
            thread->unconsideredNewSubtasks = onSubTaskFinished(thread->ti->task, subtask);
        } catch (const std::bad_alloc&) {
            onBadAlloc(thread->ti->task);
        }
        thread->newSubtasksObtained = true;
        thread->subtasksLocker.unlock();
    }
}

bool TaskSchedulerImpl::isCallerInsideTaskSchedulerCallback() const {
    return isInsideSchedulingUpdate;
}

const QList<Task*>& TaskSchedulerImpl::getTopLevelTasks() const {
    return topLevelTasks;
}

void TaskSchedulerImpl::addThreadId(qint64 taskId, Qt::HANDLE id) { /*threadIds.insert(taskId, id);*/
    threadIds[taskId] = id;
}

void TaskSchedulerImpl::removeThreadId(qint64 taskId) {
    threadIds.remove(taskId);
}

TaskThread::TaskThread(TaskInfo* _ti)
    : ti(_ti),
      subtasksLocker(),
      unconsideredNewSubtasks(),
      newSubtasksObtained(false),
      pauser(),
      isPaused(false),
      pauseLocker() {
    if (ti->task->hasFlags(TaskFlag_RunMessageLoopOnly)) {
        moveToThread(this);
    }
}

static QMutex lock;

void TaskThread::run() {
#ifdef Q_CC_MSVC_NET
    DWORD threadId = GetCurrentThreadId();
    QByteArray threadName = ti->task->getTaskName().toLocal8Bit();
    SetThreadName(threadId, threadName.data());
#endif
    Qt::HANDLE handle = QThread::currentThreadId();
    lock.lock();
    AppContext::getTaskScheduler()->addThreadId(ti->task->getTaskId(), handle);
    lock.unlock();

    SAFE_POINT(!ti->selfRunFinished, QString("Task %1 is already finished.").arg(ti->task->getTaskName()), );
    SAFE_POINT(ti->task->getState() == Task::State_Running, QString("Task is not in 'running' state.").arg(ti->task->getTaskName()), );

    updateThreadPriority(ti);
    if (!ti->task->hasFlags(TaskFlag_RunMessageLoopOnly)) {
        try {
            ti->task->run();
            SAFE_POINT(ti->task->getState() == Task::State_Running, QString("Task %1 is not in 'running' state.").arg(ti->task->getTaskName()), );
        } catch (const std::bad_alloc&) {
            onBadAlloc(ti->task);
        }
    }
    ti->selfRunFinished = true;
    if (ti->task->hasFlags(TaskFlag_RunMessageLoopOnly)) {
        int timerId = startTimer(1);
        exec();
        killTimer(timerId);
    }
    lock.lock();
    AppContext::getTaskScheduler()->removeThreadId(ti->task->getTaskId());
    lock.unlock();
}

bool TaskThread::event(QEvent* event) {
    int type = event->type();
    switch (type) {
        case GET_NEW_SUBTASKS_EVENT_TYPE:
            getNewSubtasks();
            break;
        case TERMINATE_MESSAGE_LOOP_EVENT_TYPE:
            terminateMessageLoop();
            break;
        case PAUSE_THREAD_EVENT_TYPE:
            pause();
            break;
        case QEvent::Timer:
            SAFE_POINT(dynamic_cast<QTimerEvent*>(event) != nullptr, "Cant cast to 'QTimerEvent*'.", false);
            if (ti->task->hasFlags(TaskFlag_RunMessageLoopOnly) && (ti->task->isCanceled() || ti->task->hasError())) {
                exit();
            }
            break;
        default:
            return false;
    }
    return true;
}

void TaskThread::getNewSubtasks() {
    if (ti->task->hasFlags(TaskFlag_RunMessageLoopOnly) && !newSubtasksObtained) {
        emit si_processMySubtasks();
    }
}

void TaskThread::terminateMessageLoop() {
    if (ti->task->hasFlags(TaskFlag_RunMessageLoopOnly) && isRunning()) {
        exit();
    }
}

void TaskThread::pause() {
    CHECK(!isPaused, );
    pauseLocker.lock();
    isPaused = true;
    pauser.wait(&pauseLocker);
    pauseLocker.unlock();
}

void TaskThread::resume() {
    CHECK(isPaused, );
    pauseLocker.lock();
    isPaused = false;
    pauseLocker.unlock();
    pauser.wakeAll();
}

QList<Task*> TaskThread::getProcessedSubtasks() const {
    return processedSubtasks;
}

void TaskThread::appendProcessedSubtask(Task* subtask) {
    processedSubtasks << subtask;
}

TaskInfo::TaskInfo(Task* _task, TaskInfo* _parentTaskInfo)
    : task(_task), parentTaskInfo(_parentTaskInfo) {
}

int TaskInfo::numActiveSubtasks() const {
    return numPreparedSubtasks + numRunningSubtasks;
}

TaskInfo::~TaskInfo() {
    if (thread != nullptr) {
        if (!thread->isFinished()) {
            taskLog.trace("TaskScheduler: Waiting for the thread before delete");
            if (thread->isPaused) {
                thread->resume();
            }
            thread->wait();
            taskLog.trace("TaskScheduler: Wait finished");
        }
        delete thread;
    }
}

}  // namespace U2
