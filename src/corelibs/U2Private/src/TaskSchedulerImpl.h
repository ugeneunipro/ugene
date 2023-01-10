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

#ifndef _U2_TASK_SCHEDULER_IMPL_H_
#define _U2_TASK_SCHEDULER_IMPL_H_

#include <QMap>
#include <QMutex>
#include <QThread>
#include <QTimer>
#include <QWaitCondition>

#include <U2Core/Task.h>
#include <U2Core/global.h>

namespace U2 {

class TaskInfo;
class AppResourcePool;
class AppResource;

/**
 * Some systems can go to hibernation during UGENE work.
 * This class prevents this behavior if top-level tasks exist.
 */
class SleepPreventer {
public:
    virtual ~SleepPreventer() {
    }
    virtual void capture() {
    }
    virtual void release() {
    }
};

class TaskThread : public QThread {
    Q_OBJECT
public:
    TaskThread(TaskInfo* ti);

    void run() override;

    void resume();

    QList<Task*> getProcessedSubtasks() const;

    void appendProcessedSubtask(Task*);

    TaskInfo* ti = nullptr;

    QMutex subtasksLocker;

    QList<Task*> unconsideredNewSubtasks;

    volatile bool newSubtasksObtained = false;

    QWaitCondition pauser;

    volatile bool isPaused = false;

    QMutex pauseLocker;

signals:
    /** Signal emitted by TaskFlag_RunMessageLoopOnly tasks only when new sub-tasks are added during the 'run' method execution. */
    void si_processMySubtasks();

protected:
    bool event(QEvent* event) override;

private:
    void getNewSubtasks();
    void terminateMessageLoop();
    void pause();

    QList<Task*> processedSubtasks;
};

class TaskInfo {
public:
    TaskInfo(Task* task, TaskInfo* parentTaskInfo);

    virtual ~TaskInfo();

    Task* task = nullptr;
    TaskInfo* parentTaskInfo = nullptr;
    QList<Task*> newSubtasks;

    bool wasPrepared = false;  // 'true' if prepare() was called for the task
    bool subtasksWereCanceled = false;  // 'true' if canceled task has called cancel() on its subtasks
    bool selfRunFinished = false;  // indicates that the 'run' method of this task was finished
    bool hasLockedPrepareResources = false;  // true if there were resource locks for 'prepare' stage
    bool hasLockedRunResources = false;  // true if there were resource locks for 'run' stage

    int prevProgress = 0;  // used for TaskProgress_Manual
    QString prevDesc;

    int numPreparedSubtasks = 0;
    int numRunningSubtasks = 0;
    int numFinishedSubtasks = 0;

    TaskThread* thread = nullptr;

    int numActiveSubtasks() const;
};

class U2PRIVATE_EXPORT TaskSchedulerImpl : public TaskScheduler {
    Q_OBJECT
public:
    using TaskScheduler::onSubTaskFinished;

    TaskSchedulerImpl(AppResourcePool* rp);

    ~TaskSchedulerImpl();

    void registerTopLevelTask(Task* t) override;

    void unregisterTopLevelTask(Task* t) override;

    const QList<Task*>& getTopLevelTasks() const override;

    void cancelAllTasks() override;

    QString getStateName(Task* t) const override;

    void pauseThreadWithTask(const Task* task) override;

    void resumeThreadWithTask(const Task* task) override;

    void cancelTask(Task* t);

    void addThreadId(qint64 taskId, Qt::HANDLE id) override;

    void removeThreadId(qint64 taskId) override;

    void onSubTaskFinished(TaskThread* thread, Task* subtask);

    bool isCallerInsideTaskSchedulerCallback() const override;

private slots:
    void update();
    void sl_threadFinished();
    void sl_processSubtasks();

private:
    bool processFinishedTasks();
    void unregisterFinishedTopLevelTasks();
    void processNewSubtasks();
    void prepareNewTasks();
    void runReady();

    bool readyToFinish(TaskInfo* ti);
    bool addToPriorityQueue(Task* t, TaskInfo* parentInfo);  // return true if added. Failure can be caused if a task requires resources
    void runThread(TaskInfo* pi);
    void stopTask(Task* t);
    void updateTaskProgressAndDesc(TaskInfo* ti);
    void promoteTask(TaskInfo* ti, Task::State newState);
    void deleteTask(Task* t);
    void finishSubtasks(TaskInfo* pti);

    QString tryLockResources(Task* task, bool prepareStage, bool& hasLockedResourcesAfterCall);  // returns error message
    void releaseResources(TaskInfo* ti, bool prepareStage);

    void propagateStateToParent(Task* t);
    void updateOldTasksPriority();
    void checkSerialPromotion(TaskInfo* pti, Task* subtask);
    void createSleepPreventer();

private:
    QTimer timer;
    QList<Task*> topLevelTasks;
    QList<TaskInfo*> priorityQueue;
    QList<TaskInfo*> tasksWithNewSubtasks;
    QList<Task*> newTasks;
    QStringList stateNames;
    QMap<quint64, Qt::HANDLE> threadIds;

    AppResourcePool* resourcePool = nullptr;
    AppResource* threadsResource = nullptr;
    bool stateChangesObserved = false;
    SleepPreventer* sleepPreventer = nullptr;

    /** Set to 'true' for the time of any 'emit' method call. */
    bool isInsideSchedulingUpdate = false;
};

}  // namespace U2
#endif
