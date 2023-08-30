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
    threadsResource = resourcePool->getResource(UGENE_RESOURCE_ID_THREAD);
    SAFE_POINT(threadsResource != nullptr, "Thread resource is not defined", );

    createSleepPreventer();
}

TaskSchedulerImpl::~TaskSchedulerImpl() {
    SAFE_POINT(topLevelTasks.empty(), "Top level task list is not empty.", );
    SAFE_POINT(priorityQueue.isEmpty(), "Priority queue is not empty.", );
    delete sleepPreventer;
}

void TaskSchedulerImpl::cancelTask(Task* task) {
    CHECK(!task->isCanceled(), );
    CHECK(task->getState() < Task::State_Finished, );
    taskLog.trace(tr("Canceling task: %1").arg(task->getTaskName()));
    TaskStateInfo& ti = getTaskStateInfo(task);
    ti.cancelFlag = true;
    resumeThreadWithTask(task);  // for the case when task's thread is paused. it should be resumed and terminated
    QList<QPointer<Task>> subtasks = task->getSubtasks();
    for (const QPointer<Task>& subtask : qAsConst(subtasks)) {
        cancelTask(subtask);
    }
}

void TaskSchedulerImpl::cancelAllTasks() {
    QList<Task*> tasks = topLevelTasks;
    for (Task* task : qAsConst(tasks)) {
        cancelTask(task);
    }
}

static void onBadAlloc(Task* task) {
    task->setError(TaskSchedulerImpl::tr("There is not enough memory to finish the task."));
}

static bool isSlowCheckMode() {
#ifdef _DEBUG
    return true;
#else
    static bool isTestMode = qgetenv(ENV_GUI_TEST) == "1";
    return isTestMode;
#endif
}

static void checkPriorityQueueState(const QList<TaskInfo*>& priorityQueue) {
    if (isSlowCheckMode()) {
        for (const auto& ti : qAsConst(priorityQueue)) {
            SAFE_POINT(ti->task->getState() != Task::State_Finished,
                       QString("Found finished task priorityQueue: %1.").arg(ti->task->getTaskName()), );
        }
    }
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
    checkPriorityQueueState(priorityQueue);
    bool hasFinished = false;
    for (int i = priorityQueue.size(); --i >= 0;) {
        TaskInfo* ti = priorityQueue[i];
        Task::State state = ti->task->getState();

        // Once thread is finished set 'selfRunFinished' to true.
        // Important: update selfRunFinished from the main thread only to avoid concurrency issues.
        if (state == Task::State_Running && ti->thread != nullptr && ti->thread->isUserRunFinished) {
            ti->selfRunFinished = true;
        }

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
            QList<QPointer<Task>> subtasksCopy = ti->task->getSubtasks();
            for (const QPointer<Task>& subtask : qAsConst(subtasksCopy)) {
                cancelTask(subtask);
            }
        }

        if (state != Task::State_Running) {
            continue;
        }

        if (ti->selfRunFinished) {
            releaseResources(ti, TaskResourceStage::Run);  // release resources for RUN stage
        }

        // update state desc
        updateTaskProgressAndDesc(ti);
        if (!isReadyToFinish(ti)) {  // the task can be finished only after all its subtasks finished and its run finished
            continue;
        }

        try {
            Task::ReportResult res = ti->task->report();
            if (res == Task::ReportResult_CallMeAgain) {
                continue;
            }
        } catch (const std::bad_alloc&) {
            onBadAlloc(ti->task);
        }

        TaskInfo* pti = ti->parentTaskInfo;
        if (pti != nullptr && pti->task->hasFlags(TaskFlag_RunMessageLoopOnly) && pti->thread != nullptr && pti->thread->isPaused) {
            continue;
        }

        hasFinished = true;
        promoteTask(ti, Task::State_Finished);

        if (!isOsMac()) {  // TODO: why Mac is special? Document why do we need 'processEvents' here.
            QCoreApplication::processEvents();
        }
        releaseResources(ti, TaskResourceStage::Prepare);  // release resources for PREPARE stage

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
                } else {
                    // Event was not delivered to the thread because pti->thread->isRunning() was false.
                    // This may happen because the parent see 'cancel' or 'error' flag, see TaskThread::event().
                    // In this case we anyway must deliver onSubTaskFinished() events to the parent task.
                    newSubTasks = onSubTaskFinished(parentTask, task);
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
        }
    }
    checkPriorityQueueState(priorityQueue);
    return hasFinished;
}

void TaskSchedulerImpl::unregisterFinishedTopLevelTasks() {
    QList<Task*> tasksToDelete;
    for (Task* task : qAsConst(topLevelTasks)) {
        if (task->getState() == Task::State_Finished) {
            tasksToDelete.append(task);
        }
    }
    for (Task* task : qAsConst(tasksToDelete)) {
        unregisterTopLevelTask(task);
    }
}

void TaskSchedulerImpl::processNewTasks() {
    checkPriorityQueueState(priorityQueue);
    QList<TaskInfo*> priorityQueueCopy = priorityQueue;
    for (TaskInfo* ti : qAsConst(priorityQueueCopy)) {
        if (ti->task->isCanceled() || ti->task->hasError()) {
            for (auto subtask : qAsConst(ti->newSubtasks)) {
                subtask->cancel();
            }
            ti->newSubtasks.clear();
            continue;  // Do not run new subtasks if the task is canceled.
        }
        int maxToNewSubtasksToCheck = ti->newSubtasks.length();
        int maxParallel = ti->task->getNumParallelSubtasks();
        int maxToPrepare = maxParallel - ti->countActiveSubtasks();
        for (int i = 0, nPrepared = 0; nPrepared < maxToPrepare && i < maxToNewSubtasksToCheck; i++) {
            Task* newSubtask = ti->newSubtasks[i];
            SAFE_POINT(newSubtask->getState() == Task::State_New, "Subtask has not New: " + newSubtask->getTaskName(), );
            bool isCoR = newSubtask->isCanceled() || newSubtask->hasError();
            if (!isCoR && tryPrepare(newSubtask, ti)) {
                ti->newSubtasks[i] = nullptr;
                nPrepared++;
            } else {
                SAFE_POINT(newSubtask->getState() == Task::State_New, "Task changed state inside failed tryPrepare: " + newSubtask->getTaskName(), );
                if (newSubtask->hasError() || newSubtask->isCanceled()) {
                    ti->newSubtasks[i] = nullptr;
                    setTaskState(newSubtask, Task::State_Finished);
                    propagateStateToParent(newSubtask);
                    ti->numFinishedSubtasks++;
                }
            }
        }
        ti->newSubtasks.removeAll(nullptr);
    }

    checkPriorityQueueState(priorityQueue);

    QList<Task*> newTopLevelTasksCopy = newTopLevelTasks;
    newTopLevelTasks.clear();
    for (Task* task : qAsConst(newTopLevelTasksCopy)) {
        if (task->hasError() || task->isCanceled()) {  // The task may be cancelled before it is scheduled.
            setTaskState(task, Task::State_Finished);
            unregisterTopLevelTask(task);
            continue;
        }
        if (tryPrepare(task, nullptr)) {
            newTopLevelTasks.removeOne(task);
        } else {
            if (task->hasError() || task->isCanceled()) {
                newTopLevelTasks.removeOne(task);
                setTaskState(task, Task::State_Finished);
            }
        }
    }

    checkPriorityQueueState(priorityQueue);
}

void TaskSchedulerImpl::processPreparedTasks() {
    checkPriorityQueueState(priorityQueue);

    for (TaskInfo* ti : qAsConst(priorityQueue)) {
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
        QString failedToLockStateMessage = tryLockResources(ti, TaskResourceStage::Run);
        if (!failedToLockStateMessage.isEmpty()) {
            setTaskStateDesc(ti->task, failedToLockStateMessage);
            continue;  // The task will be processed later, if the task has no error.
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
    checkPriorityQueueState(priorityQueue);
}

void TaskSchedulerImpl::runThread(TaskInfo* ti) const {
    SAFE_POINT(ti->task->getState() == Task::State_Running, QString("Task %1 state is not 'running'.").arg(ti->task->getTaskName()), );
    SAFE_POINT(!ti->task->getFlags().testFlag(TaskFlag_NoRun), QString("Task %1 with flag 'NoRun'.").arg(ti->task->getTaskName()), );
    SAFE_POINT(ti->task->hasFlags(TaskFlag_RunBeforeSubtasksFinished) || ti->numFinishedSubtasks == ti->task->getSubtasks().size(),
               QString("There are unfinished subtasks but task %1 have flag 'RunBeforeSubtasksFinished'.").arg(ti->task->getTaskName()), );
    SAFE_POINT(!ti->task->isCanceled(), QString("Task %1 is cancelled.").arg(ti->task->getTaskName()), );
    SAFE_POINT(!ti->task->hasError(), QString("Task %1 has errors.").arg(ti->task->getTaskName()), );
    SAFE_POINT(!ti->selfRunFinished, QString("Task %1 already run.").arg(ti->task->getTaskName()), );
    SAFE_POINT(ti->hasLockedThreadResource || ti->task->hasFlags(TaskFlag_RunMessageLoopOnly), QString("Task %1 has no locked thread resource.").arg(ti->task->getTaskName()), );
    const auto& resourceList = ti->task->getTaskResources();
    if (!resourceList.isEmpty()) {
        for (const auto& resource : qAsConst(resourceList)) {
            SAFE_POINT(resource.locked, QString("Running task with unlocked resource: %1, %2.").arg(ti->task->getTaskName(), resource.resourceId), );
        }
    }

    ti->thread = new TaskThread(ti);
    connect(ti->thread, &TaskThread::si_processMySubtasks, this, &TaskSchedulerImpl::sl_processSubtasks, Qt::BlockingQueuedConnection);
    connect(ti->thread, &TaskThread::finished, this, &TaskSchedulerImpl::sl_threadFinished);
    ti->thread->start();
}

QString TaskSchedulerImpl::tryLockResources(TaskInfo* ti, const TaskResourceStage& stage) {
    SAFE_POINT(!ti->task->hasError(), "Can't lock resources for task with an error", ti->task->getError());
    QString stateMessage;
    Task* task = ti->task;
    bool isPrepareStage = stage == TaskResourceStage::Prepare;
    bool isRunStage = stage == TaskResourceStage::Run;
    if (isPrepareStage) {
        // This method can only be called for a 'New' task for the prepare-stage lock.
        if (task->getState() != Task::State_New) {
            stateMessage = L10N::internalError("Attempt to lock prepare-stage resources for non-NEW task!");
            task->setError(stateMessage);
            FAIL(stateMessage, stateMessage);
        }
    } else {  // task must be Prepared or Running. Task can be 'Running' if it has subtasks
        bool isPreparedOrRunning = task->getState() == Task::State_Prepared || task->getState() == Task::State_Running;
        if (!isPreparedOrRunning) {
            stateMessage = L10N::internalError(QString("Attempt to lock run-stage for task in state: %1!").arg(task->getState()));
            task->setError(stateMessage);
            FAIL(stateMessage, stateMessage);
        }
        if (ti->hasLockedThreadResource) {
            stateMessage = L10N::internalError(QString("Task has thread resource before tryLockResources: %1!").arg(task->getTaskName()));
            task->setError(stateMessage);
            FAIL(stateMessage, stateMessage);
        }
    }

    // Perform some validation checks first before making any locks.
    QVector<TaskResourceUsage>& tres = getTaskResources(task);
    for (auto& taskRes : tres) {
        if (taskRes.stage == stage) {
            if (taskRes.locked) {
                stateMessage = L10N::internalError(QString("Task %1 resource %2 is already locked.").arg(task->getTaskName()).arg(taskRes.resourceId));
                task->setError(stateMessage);
                FAIL(stateMessage, stateMessage);
            }
        } else {
            // Make some resource state validation check: 'prepare' stage resources must be locked during 'run' stage and
            // 'run' stage resources must be unlocked during 'prepare' stage.
            bool isPrepareStageAndRunResourceIsLocked = isPrepareStage && taskRes.stage == TaskResourceStage::Run && taskRes.locked;
            bool isRunStageAndPrepareResourceIsUnlocked = isRunStage && taskRes.stage == TaskResourceStage::Prepare && !taskRes.locked;
            if (isPrepareStageAndRunResourceIsLocked || isRunStageAndPrepareResourceIsUnlocked) {
                stateMessage = L10N::internalError(QString("Task %1 resource %2 lock state is not correct.").arg(task->getTaskName()).arg(taskRes.resourceId));
                task->setError(stateMessage);
                FAIL(stateMessage, stateMessage);
            }
        }
    }
    // Start making real locks below.

    // TaskFlag_RunMessageLoopOnly is not a computational task but a message loop (WD scheduling) task.
    // We can't reserve threads for TaskFlag_RunMessageLoopOnly because it may lead to deadlocks when no thread is available for the WD scheduler
    // to work but there are child tasks with locked threads waiting for instructions from the WD scheduler.
    bool isThreadResourceNeeded = isRunStage && !task->hasFlags(TaskFlag_RunMessageLoopOnly) && !ti->hasLockedThreadResource;
    if (isThreadResourceNeeded) {
        ti->hasLockedThreadResource = threadsResource->tryAcquire(1);
        if (!ti->hasLockedThreadResource) {
            return tr("Waiting for resource '%1', count: 1").arg(threadsResource->id);
        }
    }

    bool allLocksAreSuccessful = true;
    for (auto& taskRes : tres) {
        if (taskRes.stage != stage) {
            continue;
        }
        AppResource* appRes = resourcePool->getResource(taskRes.resourceId);
        if (appRes == nullptr) {
            if (AppResource::isDynamicResourceId(taskRes.resourceId)) {
                // TODO: support ReadWrite type of dynamic resources.
                //  Currently it is unsafe because with the RW resource & read-locks the resource ownership must be propagated to the last active task.
                appRes = new AppResourceSemaphore(taskRes.resourceId, taskRes.resourceUse);
                resourcePool->registerResource(appRes);
                ti->dynamicAppResourceIds << taskRes.resourceId;
            } else {
                stateMessage = tr("Task resource not found: '%1'").arg(taskRes.resourceId);
                allLocksAreSuccessful = false;
                break;
            }
        }

        if (!appRes->tryAcquire(taskRes.resourceUse)) {
            if (appRes->getCapacity() < taskRes.resourceUse) {
                QString detailedErrorMessage = tr("Not enough resources for the task, resource: '%1' max: %2%3 requested: %4%5")
                                                   .arg(appRes->id)
                                                   .arg(appRes->getCapacity())
                                                   .arg(appRes->units)
                                                   .arg(taskRes.resourceUse)
                                                   .arg(appRes->units);
                stateMessage = taskRes.errorMessage.isEmpty() ? detailedErrorMessage : taskRes.errorMessage;
                task->setError(stateMessage);
                coreLog.error(detailedErrorMessage);
            } else {
                stateMessage = tr("Waiting for the resource: %1").arg(appRes->id);
            }
            allLocksAreSuccessful = false;
            break;
        }
        taskRes.locked = true;
    }

    if (!allLocksAreSuccessful) {
        SAFE_POINT(!stateMessage.isEmpty(), "Unsuccessful lock with no stateMessage", L10N::internalError());
        taskLog.trace("Can't allocate all required resources for the task. Releasing locked resources: " + task->getTaskName());
        // There was an error during locking: unlock everything and return the error.
        for (auto& taskRes : tres) {
            AppResource* appRes = resourcePool->getResource(taskRes.resourceId);
            if (appRes != nullptr && taskRes.locked) {
                appRes->release(taskRes.resourceUse);
            }
            taskRes.locked = false;
            if (ti->dynamicAppResourceIds.removeOne(taskRes.resourceId)) {
                resourcePool->unregisterResource(taskRes.resourceId);
            }
        }
        if (ti->hasLockedThreadResource) {
            threadsResource->release(1);
            ti->hasLockedThreadResource = false;
        }
    }
    bool isValidThreadResourceFlagState = stateMessage.isEmpty()
                                              ? isThreadResourceNeeded == ti->hasLockedThreadResource
                                              : !ti->hasLockedThreadResource;
    SAFE_POINT(isValidThreadResourceFlagState, "Invalid isValidThreadResourceFlagState", L10N::internalError());
    return stateMessage;
}

void TaskSchedulerImpl::releaseResources(TaskInfo* ti, const TaskResourceStage& stage) {
    bool isPrepareStage = stage == TaskResourceStage::Prepare;
    SAFE_POINT(ti->task->getState() == (isPrepareStage ? Task::State_Finished : Task::State_Running), "Releasing task resources in illegal state!", );
    if (stage == TaskResourceStage::Run && ti->hasLockedThreadResource) {
        threadsResource->release(1);
        ti->hasLockedThreadResource = false;
    }
    QVector<TaskResourceUsage>& taskResources = getTaskResources(ti->task);
    for (auto& res : taskResources) {
        if (res.stage != stage) {
            // Make extra state validation to detect errors earlier.
            SAFE_POINT(!isPrepareStage || !res.locked,
                       QString("Task %1 lock state is not correct. Run stage resource is not unlocked: %2, selfRunFinished: %3")
                           .arg(ti->task->getTaskName())
                           .arg(res.resourceId)
                           .arg(ti->selfRunFinished), );
            continue;
        }
        if (res.locked) {
            AppResource* appRes = resourcePool->getResource(res.resourceId);
            appRes->release(res.resourceUse);
            res.locked = false;
        }
        if (ti->dynamicAppResourceIds.removeOne(res.resourceId)) {
            resourcePool->unregisterResource(res.resourceId);
        }
    }
}

void TaskSchedulerImpl::update() {
    if (isInsideSchedulingUpdate) {
        return;
    }
    isInsideSchedulingUpdate = true;
    stateChangesObserved = false;

    bool hasFinishedTasks = processFinishedTasks();
    if (hasFinishedTasks) {
        unregisterFinishedTopLevelTasks();
    }
    processNewTasks();

    processPreparedTasks();

    updateOldTasksPriority();

    if (stateChangesObserved) {
        stateChangesObserved = false;
        timer.setInterval(0);
    } else if (timer.interval() != UPDATE_TIMEOUT) {
        timer.setInterval(UPDATE_TIMEOUT);
    }

    isInsideSchedulingUpdate = false;
}

void TaskSchedulerImpl::registerTopLevelTask(Task* task) {
    SAFE_POINT(task, QString("Trying to register NULL task"), );
    SAFE_POINT(task->getState() == Task::State_New, QString("Trying to register task in not NEW state. State: %1").arg(task->getState()), );
    SAFE_POINT(!topLevelTasks.contains(task), QString("Task is already registered: %1").arg(task->getTaskName()), );

    QThread* appThread = QCoreApplication::instance()->thread();
    QThread* thisThread = QThread::currentThread();
    QThread* taskThread = task->thread();
    SAFE_POINT(thisThread == appThread, "Current task %1 thread is not app thread.", );
    SAFE_POINT(taskThread == appThread, QString("Task %1 thread is app not app thread.").arg(task->getTaskName()), );

    taskLog.details(tr("Registering new task: %1").arg(task->getTaskName()));
    topLevelTasks.append(task);
    newTopLevelTasks.append(task);
    emit si_topLevelTaskRegistered(task);
    sleepPreventer->capture();
}

bool TaskSchedulerImpl::tryPrepare(Task* task, TaskInfo* pti) {
    SAFE_POINT((pti == nullptr || !pti->task->isCanceled()) && !task->isCanceled(), "Trying to register cancelled task: " + task->getTaskName(), false);
    SAFE_POINT((pti == nullptr || !pti->task->hasError()) && !task->hasError(), "Trying to register errored task: " + task->getTaskName(), false);
    SAFE_POINT(task->getState() == Task::State_New, "Only new task can be prepared: " + task->getTaskName(), false);
    auto ti = new TaskInfo(task, pti);

    QString lockError = tryLockResources(ti, TaskResourceStage::Prepare);
    if (!lockError.isEmpty()) {
        setTaskStateDesc(task, lockError);
        delete ti;
        return false;
    }
    SAFE_POINT(task->getState() == Task::State_New, "Task changed state inside lock() method: " + task->getTaskName(), false);

    priorityQueue.append(ti);
    setTaskInsidePrepare(task, true);
    try {
        task->prepare();
    } catch (const std::bad_alloc&) {
        onBadAlloc(task);
    }
    setTaskInsidePrepare(task, false);
    promoteTask(ti, Task::State_Prepared);

    const QList<QPointer<Task>>& subtasks = task->getSubtasks();
    for (const auto& subtask : qAsConst(subtasks)) {
        ti->newSubtasks.append(subtask);
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
            SAFE_POINT(isReadyToFinish(ti), QString("Task %1 is not ready to finish.").arg(ti->task->getTaskName()), );
            break;
        }
    }
}

bool TaskSchedulerImpl::isReadyToFinish(TaskInfo* ti) {
    Task::State state = ti->task->getState();
    SAFE_POINT(state != Task::State_Finished, "Task is already finished: " + ti->task->getTaskName(), true);
    CHECK(state == Task::State_Running, false);
    CHECK(ti->selfRunFinished, false);
    CHECK(ti->newSubtasks.isEmpty(), false);
    CHECK(ti->numActiveRunningSubtasks == 0, false);
    CHECK(ti->numActivePreparedSubtasks == 0, false);
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
            FAIL("Unexpected task state.", L10N::internalError());
    }
}

void TaskSchedulerImpl::checkSerialPromotion(TaskInfo* pti, Task* subtask) {
    SAFE_POINT(!subtask->isNew(), QString("Subtask %1 is not promoted to 'new' state.").arg(subtask->getTaskName()), );
    Task* task = pti == nullptr ? nullptr : pti->task;
    CHECK(task != nullptr, );

    if (isSlowCheckMode()) {
        int nParallel = task->getNumParallelSubtasks();
        bool before = true;
        int numActive = 0;
        const QList<QPointer<Task>>& subtasks = task->getSubtasks();
        for (const auto& sub : subtasks) {
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
                if (sub->getTaskResources().empty()) {
                    SAFE_POINT(subState != Task::State_New || sub->hasError(), QString("Subtask %1 has not 'new' state, or has errors.").arg(sub->getTaskName()), );
                }
            }
        }
    }
}

void TaskSchedulerImpl::createSleepPreventer() {
#ifndef Q_OS_DARWIN
    sleepPreventer = new SleepPreventer();
#else
    sleepPreventer = new SleepPreventerMac();
#endif
}

static void checkFinishedState(TaskInfo* ti) {
    SAFE_POINT(ti->numActivePreparedSubtasks == 0, QString("Can't finish task with prepared subtasks: %1 task.").arg(ti->task->getTaskName()), );
    SAFE_POINT(ti->numActiveRunningSubtasks == 0, QString("Can't finish task with running subtasks: %1 task.").arg(ti->task->getTaskName()), );
}

void TaskSchedulerImpl::promoteTask(TaskInfo* ti, Task::State newState) {
    Task* task = ti->task;

    stateChangesObserved = true;

    SAFE_POINT(newState > task->getState(), QString("Task %1 new state is not following task's lifecycle order.").arg(ti->task->getTaskName()), );

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
                pti->totalPreparedSubtasks++;
                pti->numActivePreparedSubtasks++;
            }
            if (ti->task->isTopLevelTask() && ti->task->isVerboseLogMode()) {
                taskLog.info(tr("Starting {%1} task").arg(ti->task->getTaskName()));
            }
            break;
        case Task::State_Running:
            tti.startTime = GTimer::currentTimeMicros();
            if (pti != nullptr) {
                pti->numActivePreparedSubtasks--;
                pti->numActiveRunningSubtasks++;
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
                    pti->numActiveRunningSubtasks--;
                }
                SAFE_POINT(pti->numActiveRunningSubtasks >= 0, QString("Parent %1 task has negative value of running subtasks.").arg(pti->task->getTaskName()), );
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
    if (isSlowCheckMode()) {
        Task* parentTask = task->getParentTask();
        if (parentTask != nullptr) {
            int nPrepared = 0;
            int nRunning = 0;
            int nFinished = 0;
            int nNew = 0;
            const QList<QPointer<Task>>& subtasks = parentTask->getSubtasks();
            for (const QPointer<Task>& sub : qAsConst(subtasks)) {
                switch (sub->getState()) {
                    case Task::State_New:
                        nNew++;
                        break;
                    case Task::State_Prepared:
                        nPrepared++;
                        break;
                    case Task::State_Running:
                        nRunning++;
                        break;
                    case Task::State_Finished:
                        nFinished++;
                        break;
                }
            }
            SAFE_POINT(nPrepared == pti->numActivePreparedSubtasks, QString("Prepared subtask count mismatch: %1.").arg(pti->task->getTaskName()), );
            SAFE_POINT(nRunning == pti->numActiveRunningSubtasks, QString("Running subtask count mismatch: %1.").arg(pti->task->getTaskName()), );
            SAFE_POINT(nFinished == pti->numFinishedSubtasks, QString("Finished subtask count mismatch %1.").arg(pti->task->getTaskName()), );
            SAFE_POINT(subtasks.length() == nNew + nPrepared + nRunning + nFinished, QString("Total subtask count mismatch: %1.").arg(pti->task->getTaskName()), );
        }
    }
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
    auto taskThread = qobject_cast<TaskThread*>(sender());
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
    CHECK(task->getState() >= Task::State_Prepared && task->getState() < Task::State_Finished, );
    foreach (TaskInfo* ti, priorityQueue) {
        if (task == ti->task && ti->thread != nullptr && ti->thread->isPaused) {
            ti->thread->resume();
        }
    }
}

void TaskSchedulerImpl::onSubTaskFinished(TaskThread* thread, Task* subtask) {
    if (thread->ti->task->hasFlags(TaskFlag_RunMessageLoopOnly) && subtask != nullptr && !thread->newSubtasksObtained) {
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
    : ti(_ti) {
    if (ti->task->hasFlags(TaskFlag_RunMessageLoopOnly)) {
        moveToThread(this);
    }
}

void TaskThread::run() {
    static QMutex lock;

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
    isUserRunFinished = true;
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

int TaskInfo::countActiveSubtasks() const {
    return numActivePreparedSubtasks + numActiveRunningSubtasks;
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
