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

#ifndef _U2_TASK_H_
#define _U2_TASK_H_

#include <U2Core/global.h>
#include <U2Core/Log.h>
#include <U2Core/U2OpStatus.h>

#include <QDateTime>
#include <QPointer>
#include <QReadWriteLock>
#include <QStringList>
#include <QVarLengthArray>

namespace U2 {

struct U2CORE_EXPORT TaskResourceUsage {

    TaskResourceUsage(int id = 0, int use = 0, bool prepareStage = false)
        : resourceId(id), resourceUse(use), prepareStageLock(prepareStage), locked(false)
    {}

    enum UseType {
        Read,
        Write
    };

    TaskResourceUsage(int id, UseType use, bool prepareStage = false)
        : resourceId(id), resourceUse(use), prepareStageLock(prepareStage), locked(false)
    {}

    int resourceId;
    int resourceUse;
    bool prepareStageLock;
    bool locked;
    /* Leave it empty for default message */
    QString errorMessage;
};

class TaskScheduler;

/**
    Holds task state info about current error and progress
    Error variable is protected by RW lock to ensure safe multi-threaded updates
*/
class U2CORE_EXPORT TaskStateInfo : public U2OpStatus {
public:
    TaskStateInfo() : progress(-1), cancelFlag(false), hasErr(false), lock(QReadWriteLock::NonRecursive) {}

    /* Percent values in range 0..100, negative if unknown. */
    int    progress;
    int    cancelFlag;


    virtual bool hasError() const {return hasErr;}
    virtual QString getError() const {QReadLocker r(&lock); return error;}
    virtual void setError(const QString& err) {QWriteLocker w(&lock); error =  err; hasErr = !error.isEmpty();}

    virtual bool isCanceled() const {return cancelFlag;}
    virtual void setCanceled(bool v)  {cancelFlag = v;}

    virtual int getProgress() const {return progress;}
    virtual void setProgress(int v)  {progress = v;}

    virtual QString getDescription() const {QReadLocker r(&lock); return desc;}
    virtual void setDescription(const QString& _desc) {QWriteLocker w(&lock); desc = _desc;}

    virtual bool hasWarnings() const {QReadLocker r(&lock); return !warnings.isEmpty(); }
    virtual QStringList getWarnings() const {QReadLocker r(&lock); return warnings; }
    virtual void addWarning(const QString &warning);
    virtual void addWarnings(const QStringList &wList);

    /* The same as addWarnings() but it does not write to log. Used by TaskScheduler. */
    void insertWarnings(const QStringList &wList);

private:
    bool hasErr;
    QString desc;
    QString error;
    QStringList warnings;

private:
    mutable QReadWriteLock lock; //the lock is used because error & stateDesc can be assigned from any thread
};

class U2CORE_EXPORT TaskTimeInfo {
public:
    TaskTimeInfo() : startTime(0), finishTime(0), timeOut(-1) {}

    //time in microseconds from Unix Epoch (UTC). See Timer.h
    qint64  startTime;  //the time task is promoted to 'running' state
    qint64  finishTime; //the time task is promoted to 'finished' state

    int     timeOut;  //number of seconds to be passed before tasks is timed out, -1 -> timeout function is disabled

};

#define MAX_PARALLEL_SUBTASKS_AUTO   0
#define MAX_PARALLEL_SUBTASKS_SERIAL 1

enum TaskFlag {

    // Base flags
    TaskFlag_None = 0,

    TaskFlag_NoRun = 1 << 1,

    TaskFlag_RunBeforeSubtasksFinished = 1 << 2, //subtask can be run before its subtasks finished

    TaskFlag_NoAutoDelete = 1 << 3, //for top level tasks only: task is not deleted by scheduler after task is finished

    TaskFlag_RunMessageLoopOnly = 1 << 4, // for tasks that shouldn't run but should conduct processing of their subtasks in a separate thread

    TaskFlag_RunInMainThread = 1 << 5, // for tasks that need access to GUI, they will be run in main thread

    // Behavior based on subtasks
    TaskFlag_FailOnSubtaskError = 1 << 10, //subtask error is propagated automatically

    TaskFlag_FailOnSubtaskCancel = 1 << 11, // error and cancel flag are set if subtask was canceled and parent is neither canceled nor have errors

    TaskFlag_PropagateSubtaskDesc = 1 << 12, // task use description from the subtask (last changed)

    TaskFlag_CancelOnSubtaskCancel = 1 << 13, // only cancel flag is set if subtask was canceled and parent is neither canceled nor have errors


    // Reporting options
    TaskFlag_ReportingIsSupported = 1 << 20, // task supports reporting

    TaskFlag_ReportingIsEnabled = 1 << 21, // task is asked to generate report

    TaskFlag_VerboseStateLog = 1 << 22, //tasks prepared/finished state is dumped to the 'info' log category. Effective for top-level tasks only

    TaskFlag_MinimizeSubtaskErrorText = 1 << 23, // for TaskFlag_FailOnSubtaskError task minimizes the error text
                                                 // excluding task-names info from the text
                                                 // applies this behaviour for the current task and all children of the current task

    TaskFlag_SuppressErrorNotification = 1 << 24, //for top level tasks only: if task fails, tells if notification is shown

    TaskFlag_VerboseOnTaskCancel = 1 << 25, // when a task is cancelled, it is dumped to the log ('info' category)

    TaskFlag_OnlyNotificationReport = 1 << 26, // task is asked to generate report

    TaskFlag_CollectChildrenWarnings = 1 << 27,

    TaskFlag_ConcatenateChildrenErrors = 1 << 28 // task collects errors from all children and unites them into one report
};

#define TaskFlags_FOSCOE                (TaskFlags(TaskFlag_FailOnSubtaskError) | TaskFlag_FailOnSubtaskCancel)
#define TaskFlags_NR_FOSCOE             (TaskFlags_FOSCOE | TaskFlag_NoRun)
#define TaskFlags_RBSF_FOSCOE           (TaskFlags_FOSCOE | TaskFlag_RunBeforeSubtasksFinished)

// TODO: use this new alternative to FOSCOE, more logical: fail on error, cancel on cancel
#define TaskFlags_FOSE_COSC             (TaskFlags(TaskFlag_FailOnSubtaskError) | TaskFlag_CancelOnSubtaskCancel)
#define TaskFlags_NR_FOSE_COSC          (TaskFlags_FOSE_COSC | TaskFlag_NoRun)
#define TaskFlags_RBSF_FOSE_COSC        (TaskFlags_FOSE_COSC | TaskFlag_RunBeforeSubtasksFinished)

Q_DECLARE_FLAGS(TaskFlags, TaskFlag)
typedef QVarLengthArray<TaskResourceUsage, 1> TaskResources;

class U2CORE_EXPORT Task : public QObject {
    Q_OBJECT
    friend class TaskScheduler;
public:

    enum State {
        State_New,
        State_Prepared,
        State_Running,
        State_Finished
    };

    enum ProgressManagement {
        Progress_Manual,
        Progress_SubTasksBased
    };

    enum ReportResult {
        ReportResult_Finished,
        ReportResult_CallMeAgain
    };


    //Creates new task with State_New state
    Task(const QString& _name, TaskFlags f);

    //Prepares Task to run
    //Task must request/prepare all resources it needs, create subtasks and define progress management type
    //This method called after Task is added to Scheduler from the main thread
    //After calling this method task gets State_Prepared state
    virtual void prepare(){}

    // Called by Scheduler from the separate thread. No updates to Project/Document model can be done from this method
    // Task gets State_Running state when its first of its subtasks is run
    virtual void run() {assert(0);} // assertion is added to find all tasks with RUN declared in flags but not implemented

    // Called from the main thread after run() is finished
    // Task must report all of it results if needed.
    // If task can't report right now (for example a model is state-locked)
    // task can return ReportResult_CallMeAgain.
    // If task locks some resources, it's a good place to release them
    // After task reporting succeeds, it gets State_Finished state
    virtual ReportResult report() {return ReportResult_Finished;}

    bool isCanceled() const {return stateInfo.cancelFlag;}

    // Returns subtasks of the task. Task must prepare it's subtask on prepare() call and can't change them latter.
    const QList<QPointer<Task> > &getSubtasks() const;
    QList<Task *> getPureSubtasks() const;

    QString getTaskName() const {return taskName;}

    State getState() const {return state;}

    const TaskStateInfo& getStateInfo() const { return stateInfo; }

    const TaskTimeInfo&  getTimeInfo() const { return timeInfo; }

    int getProgress() const {return stateInfo.progress;}

    ProgressManagement getProgressManagementType() const {return tpm;}

    TaskFlags getFlags() const {return flags;}

    bool hasFlags(TaskFlags f) const {return flags & f;}

    void addSubTask(Task* sub);

    bool hasError() const {return stateInfo.hasError();}

    QString getError() const {return stateInfo.getError();}

    virtual bool hasWarning() const { return stateInfo.hasWarnings(); }

    virtual QStringList getWarnings() const { return stateInfo.getWarnings(); }

    bool isFinished() const {return state == Task::State_Finished;}

    bool isRunning() const {return state == Task::State_Running;}

    bool isPrepared() const {return state == Task::State_Prepared;}

    bool isNew() const {return state == Task::State_New;}

    // When called for a finished task it must deallocate all resources it keeps.
    // ATTENTION: this method WILL NOT be called by Task Scheduler automatically.
    // It is guaranteed that only tests run by TestRunnerTask will be cleaned up.
    // So, if any task provides 'cleanup' method, it still MUST correctly clean up
    // its resources in destructor.
    virtual void cleanup();

    virtual bool hasSubtasksWithErrors() const  { return getSubtaskWithErrors() != NULL; }

    virtual bool propagateSubtaskError();

    virtual Task* getSubtaskWithErrors() const;

    virtual qint64 getTaskId() const {return taskId;}

    virtual bool isTopLevelTask() const {return getParentTask() == 0;}

    virtual Task* getParentTask() const {return parentTask;}

    virtual Task* getTopLevelParentTask() {return isTopLevelTask() ? this : parentTask->getTopLevelParentTask();}

    virtual bool isReportingSupported() const {return flags.testFlag(TaskFlag_ReportingIsSupported);}

    virtual bool isReportingEnabled() const {return flags.testFlag(TaskFlag_ReportingIsEnabled);}

    virtual bool isNotificationReport() const {return flags.testFlag(TaskFlag_OnlyNotificationReport);}

    virtual void setReportingSupported(bool v) { setFlag(TaskFlag_ReportingIsSupported, v); }

    virtual void setReportingEnabled(bool v) {assert(isReportingSupported()); setFlag(TaskFlag_ReportingIsEnabled, v);}

    virtual void setNotificationReport(bool v) {assert(isReportingSupported()); setFlag(TaskFlag_ReportingIsEnabled, v);}

    virtual void setCollectChildrensWarningsFlag(bool v);

    virtual void setNoAutoDelete( bool v ) { setFlag( TaskFlag_NoAutoDelete, v ); }

    virtual QString generateReport() const {assert(0); return QString();}

    float getSubtaskProgressWeight() const {return progressWeightAsSubtask;}

    void setSubtaskProgressWeight(float v) {progressWeightAsSubtask = v;}

    bool useDescriptionFromSubtask() const {return flags.testFlag(TaskFlag_PropagateSubtaskDesc);}

    void setUseDescriptionFromSubtask(bool v) { setFlag(TaskFlag_PropagateSubtaskDesc, v);}

    bool isVerboseLogMode() const {return flags.testFlag(TaskFlag_VerboseStateLog);}

    void setVerboseLogMode(bool v) { setFlag(TaskFlag_VerboseStateLog, v); }

    bool isErrorNotificationSuppressed() const { return flags.testFlag(TaskFlag_SuppressErrorNotification); }

    void setErrorNotificationSuppression(bool v) { setFlag(TaskFlag_SuppressErrorNotification, v); }

    bool isVerboseOnTaskCancel() const {return flags.testFlag(TaskFlag_VerboseOnTaskCancel); }

    void setVerboseOnTaskCancel(bool v) { setFlag(TaskFlag_VerboseOnTaskCancel, v); }

    bool isConcatenateChildrenErrors() const { return flags.testFlag(TaskFlag_ConcatenateChildrenErrors); }

    void setConcatenateChildrenErrors(bool v) { setFlag(TaskFlag_ConcatenateChildrenErrors, v); }

    const TaskResources& getTaskResources() {return taskResources;}

    //WARN: if set to MAX_PARALLEL_SUBTASKS_AUTO, returns unprocessed value (MAX_PARALLEL_SUBTASKS_AUTO = 0)
    int getMaxParallelSubtasks() const {return maxParallelSubtasks;}

    // the difference from getMaxParralelSubtasks is that this method
    // will process MAX_PARALLEL_SUBTASKS_AUTO and will never return 0
    virtual int getNumParallelSubtasks() const;

    void setMaxParallelSubtasks(int n);

    void setError(const QString& err) {stateInfo.setError(err);}

    void setMinimizeSubtaskErrorText(bool v);

    /** Number of seconds to be passed to mark task as failed by timeout */
    void setTimeOut(int sec) {timeInfo.timeOut = sec;}

    /** Number of seconds to be passed to mark task as failed by timeout */
    int getTimeOut() const { return timeInfo.timeOut;}

    void addTaskResource(const TaskResourceUsage& r);

    bool isMinimizeSubtaskErrorText() const;

public slots:
    // Set's cancelFlag to true. Does not wait for task to be stopped
    void cancel();

signals:
    void si_subtaskAdded(Task* sub);

    void si_progressChanged();
    void si_descriptionChanged();
    void si_stateChanged();

protected:
    /// Called by scheduler when subtask is finished.
    virtual QList<Task*> onSubTaskFinished(Task* subTask);
    void setRunResources(const TaskResources& taskR) {assert(state <= State_Prepared); taskResources = taskR;}

    void setTaskName(const QString& taskName);

    void setFlag(TaskFlag f, bool v) {
        flags = v ? (flags | f) : flags & (~f);
    }

    TaskStateInfo           stateInfo;
    TaskTimeInfo            timeInfo;
    ProgressManagement      tpm;

    float                   progressWeightAsSubtask;
    int                     maxParallelSubtasks;

private:
    TaskFlags               flags;
    QString                 taskName;
    State                   state;
    Task*                   parentTask;
    QList<QPointer<Task> >  subtasks;
    qint64                  taskId;
    TaskResources           taskResources;
    bool                    insidePrepare;
};


class U2CORE_EXPORT TaskScheduler : public QObject {
    Q_OBJECT
public:

    virtual void registerTopLevelTask(Task* t) = 0;

    virtual void unregisterTopLevelTask(Task* t) = 0;

    virtual const QList<Task*>& getTopLevelTasks() const = 0;

    virtual Task * getTopLevelTaskById( qint64 id ) const = 0;

    virtual QDateTime estimatedFinishTime(Task*) const = 0;

    virtual void cancelAllTasks() = 0;

    virtual QString getStateName(Task* t) const = 0;

    virtual void addThreadId(qint64 taskId, Qt::HANDLE id)  = 0;

    virtual void removeThreadId(qint64 taskId) = 0;

    virtual qint64 getNameByThreadId(Qt::HANDLE id) const = 0;

    virtual void pauseThreadWithTask(const Task *task) = 0;

    virtual void resumeThreadWithTask(const Task *task) = 0;

signals:
    void si_noTasksInScheduler();
    void si_ugeneIsReadyToWork();

protected:

    TaskResources& getTaskResources(Task* t) {return t->taskResources;}

    TaskStateInfo&  getTaskStateInfo(Task* t) {return t->stateInfo;}

    TaskTimeInfo&   getTaskTimeInfo(Task* t) {return t->timeInfo;}

    void emit_taskProgressChanged(Task* t) {emit t->si_progressChanged();}

    void emit_taskDescriptionChanged(Task* t) {emit t->si_descriptionChanged();}

    QList<Task*> onSubTaskFinished(Task* parentTask, Task* subTask) {return parentTask->onSubTaskFinished(subTask);}

    void addSubTask(Task* t, Task* sub);

    void setTaskState(Task* t, Task::State newState);

    void setTaskStateDesc(Task* t, const QString& desc);

    void setTaskInsidePrepare(Task* t, bool val);

signals:
    void si_topLevelTaskRegistered(Task*);

    void si_topLevelTaskUnregistered(Task*);

    void si_stateChanged(Task* task);
};

} //namespace

Q_DECLARE_OPERATORS_FOR_FLAGS(U2::TaskFlags)

#endif
