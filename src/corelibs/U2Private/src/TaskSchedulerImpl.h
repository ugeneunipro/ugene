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

#ifndef _U2_TASK_SCHEDULER_IMPL_H_
#define _U2_TASK_SCHEDULER_IMPL_H_

#include <U2Core/global.h>
#include <U2Core/Task.h>

#include <QThread>
#include <QTimer>
#include <QMap>
#include <QMutex>
#include <QWaitCondition>

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
    virtual ~SleepPreventer() {}
    virtual void capture() {}
    virtual void release() {}
};

class TaskThread : public QThread {
    Q_OBJECT
public:
    TaskThread(TaskInfo* _ti);
    void run();
    void resume();
    QList<Task*> getProcessedSubtasks() const;
    void appendProcessedSubtask(Task *);

    TaskInfo* ti;
    QObject*  finishEventListener;
    QMutex subtasksLocker;
    QList<Task *> unconsideredNewSubtasks;
    volatile bool newSubtasksObtained;

    QWaitCondition pauser;
    volatile bool isPaused;
    QMutex pauseLocker;
signals:
    void si_processMySubtasks();

protected:
    bool event(QEvent *event);

private:
    void getNewSubtasks();
    void terminateMessageLoop();
    void pause();

    QList<Task *> processedSubtasks;
};


class TaskInfo {
public:
    TaskInfo(Task* t, TaskInfo* p)
        : task(t), parentTaskInfo(p), wasPrepared(false), subtasksWereCanceled(false), selfRunFinished(false),
        hasLockedPrepareResources(false), hasLockedRunResources(false),
        prevProgress(0), numPreparedSubtasks(0), numRunningSubtasks(0), numFinishedSubtasks(0),  thread(NULL) {}

    virtual ~TaskInfo();

    //true if task state >= RUN && thread is finished or not used at all

    Task*           task;
    TaskInfo*       parentTaskInfo;
    QList<Task*>    newSubtasks;

    bool            wasPrepared;            // 'true' if prepare() was called for the task
    bool            subtasksWereCanceled;   // 'true' if canceled task has called cancel() on its subtasks
    bool            selfRunFinished;        // indicates that the 'run' method of this task was finished
    bool            hasLockedPrepareResources;  //true if there were resource locks for 'prepare' stage
    bool            hasLockedRunResources;      //true if there were resource locks for 'run' stage


    int             prevProgress;   //used for TaskProgress_Manual
    QString         prevDesc;

    int             numPreparedSubtasks;
    int             numRunningSubtasks;
    int             numFinishedSubtasks;

    TaskThread*     thread;

    inline int numActiveSubtasks() const {
        return numPreparedSubtasks+numRunningSubtasks;
    }


};

class U2PRIVATE_EXPORT TaskSchedulerImpl : public TaskScheduler {
    Q_OBJECT
public:
    using TaskScheduler::onSubTaskFinished;

    TaskSchedulerImpl(AppResourcePool* rp);
    ~TaskSchedulerImpl();

    virtual void registerTopLevelTask(Task* t);

    virtual void unregisterTopLevelTask(Task* t);

    const QList<Task*>& getTopLevelTasks() const {return topLevelTasks;}

    Task * getTopLevelTaskById( qint64 id ) const;

    QDateTime estimatedFinishTime(Task*) const;

    virtual void cancelTask(Task* t);

    virtual void cancelAllTasks();

    virtual QString getStateName(Task* t) const;

    void addThreadId(qint64 taskId, Qt::HANDLE id) {/*threadIds.insert(taskId, id);*/threadIds[taskId] = id;}
    void removeThreadId(qint64 taskId) {threadIds.remove(taskId);}
    qint64 getNameByThreadId(Qt::HANDLE id) const{return threadIds.key(id);}
    void pauseThreadWithTask(const Task *task);
    void resumeThreadWithTask(const Task *task);
    void onSubTaskFinished(TaskThread *thread, Task *subtask);

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
    bool addToPriorityQueue(Task* t, TaskInfo* parentInfo); //return true if added. Failure can be caused if a task requires resources
    void runThread(TaskInfo* pi);
    void stopTask(Task* t);
    void updateTaskProgressAndDesc(TaskInfo* ti);
    void promoteTask(TaskInfo* ti, Task::State newState);
    void deleteTask(Task* t);
    void finishSubtasks(TaskInfo *pti);

    QString tryLockResources(Task* task, bool prepareStage, bool& hasLockedResourcesAfterCall); //returns error message
    void releaseResources(TaskInfo* ti, bool prepareStage);

    void propagateStateToParent(Task* t);
    void updateOldTasksPriority();
    void checkSerialPromotion(TaskInfo* pti, Task* subtask);
    void createSleepPreventer();

private:
    QTimer                  timer;
    QList<Task*>            topLevelTasks;
    QList<TaskInfo*>        priorityQueue;
    QList<TaskInfo*>        tasksWithNewSubtasks;
    QList<Task*>            newTasks;
    QList<Task*>            loadingTasks;
    QStringList             stateNames;
    QMap<quint64, Qt::HANDLE>    threadIds;

    AppResourcePool*        resourcePool;
    AppResource*            threadsResource;
    bool                    stateChangesObserved;
    bool                    stateIsLoaded;
    SleepPreventer*         sleepPreventer;
};

} //namespace
#endif
