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

#include <QThread>

#include <U2Core/AppContext.h>
#include <U2Core/Task.h>

#include "TaskTests.h"

namespace U2 {

#define VALUE_ATTR      "value"
#define DOC_ATTR        "doc"
#define OBJ_ATTR        "obj"
#define INDEX_ATTR      "index"
#define NAME_ATTR       "name"
#define FLAGS_ATTR      "flags"
#define SUBTASK_ATTR    "subtask"
#define STATE_ATTR      "state"
#define STATEINFO_PROGRESS_ATTR "progress"
#define STATEINFO_CANCEL_FLAG_ATTR "cancelflag"
#define TASK_TYPE_ATTR  "type"
#define DELETE_ATTR     "delete"
#define SERIAL_FLAG_ATTR "serial"
#define SUBTASK_NUM_ATTR "subtask_num"
#define CANCEL_FLAG_ATTR "cancel"
#define RUN_AFTER_ALL_SUBS_FINISHED_FLAG_ATTR "run_after_all_subs"
#define DELAY_ATTR       "ms"
#define CONDITION_ATTR   "cond"
#define BASE_TASK                 "base_task"
#define INFINITE_TASK             "infinite_task"
#define DESTRUCTOR_CLEANUP_TASK   "destructor_cleanup_task"

class SThread : public QThread {
public:
    static void msleep(unsigned long msecs){ QThread::msleep(msecs);}
};

static TaskFlags flagsFromString(QString str, bool *ok = NULL) {
    TaskFlags taskFlags = TaskFlags(TaskFlag_None);
    if(ok!=NULL) *ok = false;
    if(!str.isEmpty()) {
        QRegExp rx("([^\\|]+)");
        int pos = 0;
        QHash<QString, TaskFlag> hash;
        hash["TaskFlag_None"] = TaskFlag_None;
        hash["TaskFlag_NoRun"] = TaskFlag_NoRun;
        hash["TaskFlag_NoAutoDelete"] = TaskFlag_NoAutoDelete;
        hash["TaskFlag_RunBeforeSubtasksFinished"] = TaskFlag_RunBeforeSubtasksFinished;
        hash["TaskFlag_FailOnSubtaskError"] = TaskFlag_FailOnSubtaskError;
        hash["TaskFlag_FailOnSubtaskCancel"] = TaskFlag_FailOnSubtaskCancel;
        while ((pos = rx.indexIn(str, pos)) != -1) {
            pos += rx.matchedLength();
            const TaskFlag flag = hash.value(rx.cap(1), static_cast<TaskFlag>(-1));
            if (flag == static_cast<TaskFlag>(-1)) {
                return taskFlags;
            }
            taskFlags |= flag;
        }
        if (ok != NULL) {
            *ok = true;
        }
    }
    return taskFlags;
}

static Task::State stateFromString(QString str, bool *ok = NULL) {
    Task::State taskState = Task::State_New;
    if(ok!=NULL) *ok = false;
    if(!str.isEmpty()) {
        QHash<QString, Task::State> hash;
        hash["State_New"] = Task::State_New;
        hash["State_Prepared"] = Task::State_Prepared;
        hash["State_Running"] = Task::State_Running;
        hash["State_Finished"] = Task::State_Finished;
        taskState = hash.value(str, static_cast<Task::State>(-1));
        if (taskState == static_cast<Task::State>(-1)) {
            return taskState;
        }
        if (ok != NULL) {
            *ok = true;
        }
    }
    return taskState;
}

/*****************************************/
/*InfiniteTestTask*/
/*****************************************/

void InfiniteTestTask::run() {
    while(!stateInfo.cancelFlag) {
        SThread::msleep(100);
    }
}

/*****************************************/
/*DestructorCleanupTask*/
/*****************************************/

DestructorCleanupTask::DestructorCleanupTask(QString taskName, TaskFlags f) :Task(taskName, f) {}

DestructorCleanupTask::~DestructorCleanupTask() {
    cleanup();
}

StateOrderTestTask::StateOrderTestTask(StateOrderTestTaskCallback *ptr, TaskFlags _f)
: Task("calback_test_task",_f)
{
    callback = ptr;
    step = 0;
}
void StateOrderTestTask::prepare() {
    callback->func(this,StateOrder_Prepare);
}
void StateOrderTestTask ::run() {
    callback->func(this,StateOrder_Run);
}
Task::ReportResult StateOrderTestTask::report() {
    callback->func(this,StateOrder_Report);
    return ReportResult_Finished;
}

StateOrderTestTask::~StateOrderTestTask() {
    callback->func(this,StateOrder_Done);
}

void GTest_TaskCreateTest::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    deleteTask = false;

    resultContextName = el.attribute(INDEX_ATTR);
    QString taskName_str = el.attribute(NAME_ATTR);
    if (taskName_str.isEmpty()) {
        failMissingValue(NAME_ATTR);
        return;
    }

    QString taskFlags_str = el.attribute(FLAGS_ATTR);
    TaskFlags taskFlags = TaskFlags(TaskFlag_None);
    if(!taskFlags_str.isEmpty()) {
        bool ok = false;
        taskFlags = flagsFromString(taskFlags_str,&ok);
        if(!ok) {
            failMissingValue(FLAGS_ATTR);
            return;
        }
    }

    QString taskType_str = el.attribute(TASK_TYPE_ATTR);
    if(taskType_str.isEmpty()) {
        failMissingValue(TASK_TYPE_ATTR);
        return;
    }

    QString deleteTask_str = el.attribute(DELETE_ATTR);
    if(!deleteTask_str.isEmpty()) {
        bool ok = false;
        deleteTask = deleteTask_str.toInt(&ok);
        if(!ok) {
            failMissingValue(DELETE_ATTR);
            return;
        }
    }

    if (taskType_str == BASE_TASK) {
        task = new Task(taskName_str, taskFlags|TaskFlags(TaskFlag_NoRun));
    } else if (taskType_str == INFINITE_TASK) {
        task = new InfiniteTestTask(taskName_str, taskFlags);
    } else if (taskType_str == DESTRUCTOR_CLEANUP_TASK) {
        task = new DestructorCleanupTask(taskName_str, taskFlags | TaskFlags(TaskFlag_NoRun));
    }  else {
        failMissingValue(TASK_TYPE_ATTR);
        return;
    }
}

Task::ReportResult GTest_TaskCreateTest::report() {
    if (!resultContextName.isEmpty()) {
        addContext(resultContextName, task);
    }
    return ReportResult_Finished;
}

void GTest_TaskCreateTest::cleanup() {
    if(!resultContextName.isEmpty()) {
        removeContext(resultContextName);
    }
    if (deleteTask /*|| task->hasFlags(TaskFlag_NoAutoDelete)*/) {
        delete task;
    }

    XmlTest::cleanup();
}

void GTest_TaskAddSubtaskTest::init(U2::XMLTestFormat *tf, const QDomElement &el){
    Q_UNUSED(tf);
    taskContextName = el.attribute(OBJ_ATTR);
    if (taskContextName .isEmpty()) {
        failMissingValue(OBJ_ATTR);
        return;
    }
    subtaskContextName = el.attribute(SUBTASK_ATTR);
    if (subtaskContextName.isEmpty()) {
        failMissingValue(SUBTASK_ATTR);
        return;
    }
}

Task::ReportResult GTest_TaskAddSubtaskTest::report() {
    Task *task= getContext<Task>(this, taskContextName);
    if(task==NULL) {
        stateInfo.setError(QString("invalid context: %1").arg(taskContextName));
        return ReportResult_Finished;
    }

    Task* subtask = getContext<Task>(this, subtaskContextName);
    if(subtask==NULL) {
        stateInfo.setError(QString("invalid context: %1").arg(subtaskContextName));
        return ReportResult_Finished;
    }
    task->addSubTask(subtask);

    if(!task->getPureSubtasks().contains(subtask)) {
        stateInfo.setError(QString("subtask not add"));
        return ReportResult_Finished;
    }
    if(subtask->getParentTask()!=task) {
        stateInfo.setError(QString("subtask parent not set"));
        return ReportResult_Finished;
    }

    return ReportResult_Finished;
}


void GTest_TaskCancelTest::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    objContextName = el.attribute(OBJ_ATTR);
    if (objContextName.isEmpty()) {
        failMissingValue(OBJ_ATTR);
        return;
    }
}

Task::ReportResult GTest_TaskCancelTest::report() {
    QObject *obj = getContext(objContextName);
    if(obj==NULL) {
        stateInfo.setError(QString("invalid object context"));
        return ReportResult_Finished;
    }
    assert(obj!=NULL);
    Task *task = qobject_cast<Task*>(obj);
    task->cancel();
    if(!task->getStateInfo().cancelFlag) {
        stateInfo.setError(QString("task state flag not matched: %1, expected %2 ").arg(task->getStateInfo().cancelFlag).arg(true));
        return ReportResult_Finished;
    }
    return ReportResult_Finished;
}

void GTest_TaskCheckFlag::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);
    taskContextName = el.attribute(OBJ_ATTR);
    if (taskContextName.isEmpty()) {
        failMissingValue(OBJ_ATTR);
        return;
    }
    QString taskFlags_str = el.attribute(FLAGS_ATTR);
    if (taskFlags_str.isEmpty()) {
        failMissingValue(FLAGS_ATTR);
        return;
    }
    bool ok = false;
    /*TaskFlags taskFlags = */flagsFromString(taskFlags_str,&ok);
    if(!ok) {
        failMissingValue(FLAGS_ATTR);
        return;
    }
}

Task::ReportResult GTest_TaskCheckFlag::report() {
    QObject *obj = getContext(taskContextName);
    if(obj==NULL) {
        stateInfo.setError(QString("invalid object context"));
        return ReportResult_Finished;
    }
    assert(obj!=NULL);
    Task *task = qobject_cast<Task*>(obj);
    if(task->getFlags().operator &(flag) == 0) {
        stateInfo.setError(QString("task flags not matched %1, expected %2").arg(task->getFlags()).arg(flag));
        return ReportResult_Finished;
    }
    return ReportResult_Finished;
}

void GTest_TaskCheckState::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    checkState = false;
    checkProgress = false;
    checkCancelFlag = false;
    checkError = false;
    checkStateDesc = false;

    taskContextName = el.attribute(OBJ_ATTR);
    if (taskContextName.isEmpty()) {
        failMissingValue(OBJ_ATTR);
        return;
    }
    QString taskState_str = el.attribute(STATE_ATTR);
    if (!taskState_str.isEmpty()) {
        bool ok = false;
        taskState = stateFromString(taskState_str,&ok);
        if (!ok) {
            failMissingValue(STATE_ATTR);
            return;
        }
        checkState = true;
    }
    QString progress_str = el.attribute(STATEINFO_PROGRESS_ATTR);
    if (!progress_str.isEmpty()) {
        bool ok = false;
        taskStateInfo.progress = progress_str.toInt(&ok);
        if (!ok) {
            failMissingValue(STATEINFO_PROGRESS_ATTR);
            return;
        }
        checkProgress = true;
    }

    QString cancelFlag_str = el.attribute(STATEINFO_CANCEL_FLAG_ATTR);
    if (!cancelFlag_str.isEmpty()) {
        bool ok = false;
        taskStateInfo.cancelFlag = cancelFlag_str.toInt(&ok);
        if(!ok) {
            failMissingValue(STATEINFO_CANCEL_FLAG_ATTR);
            return;
        }
        checkCancelFlag = true;
    }
    //TODO: stateDesc, error
}

Task::ReportResult GTest_TaskCheckState::report() {
    Task *task = getContext<Task>(this, taskContextName);
    if(task == NULL) {
        stateInfo.setError(QString("invalid context %1").arg(taskContextName));
        return ReportResult_Finished;
    }
    if(checkState) {
        if(task->getState() != taskState) {
            stateInfo.setError(QString("task state not matched %1, expected %2").arg(task->getState()).arg(taskState));
            return ReportResult_Finished;
        }
    }
    if(checkProgress) {
        if(taskStateInfo.progress != task->getStateInfo().progress) {
            stateInfo.setError(QString("task stateInfo.progress not matched %1, expected %2").arg(task->getStateInfo().progress).arg(taskStateInfo.progress));
            return ReportResult_Finished;
        }
    }
    if(checkCancelFlag) {
        if(taskStateInfo.cancelFlag!= task->getStateInfo().cancelFlag) {
            stateInfo.setError(QString("task stateInfo.cancelFlag not matched %1, expected %2").arg(task->getStateInfo().cancelFlag).arg(taskStateInfo.cancelFlag));
            return ReportResult_Finished;
        }
    }
    return ReportResult_Finished;
}

void GTest_TaskExec::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);
    taskContextName = el.attribute(OBJ_ATTR);
    if (taskContextName.isEmpty()) {
        failMissingValue(OBJ_ATTR);
        return;
    }
}

void GTest_TaskExec::prepare() {
    Task *task = getContext<Task>(this, taskContextName);
    if (task==NULL) {
        stateInfo.setError(QString("invalid context %1").arg(taskContextName));
        return;
    }
    AppContext::getTaskScheduler()->registerTopLevelTask(task);
}

Task::ReportResult GTest_TaskExec::report() {
    return ReportResult_Finished;
}

void GTest_TaskStateOrder::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    serial_flag = true;
    subtask_num = 0;
    cancel_flag = false;
    run_after_all_subs_flag = false;
    done_flag = false;

    QString subtask_num_str = el.attribute(SUBTASK_NUM_ATTR);
    if (!subtask_num_str.isEmpty()) {
        bool ok = false;
        subtask_num = subtask_num_str.toInt(&ok);
        if(!ok && subtask_num >= 0) {
            failMissingValue(SUBTASK_NUM_ATTR);
            return;
        }
    }
    QString serial_flag_str = el.attribute(SERIAL_FLAG_ATTR);
    if (serial_flag_str.isEmpty()) {
        failMissingValue(SERIAL_FLAG_ATTR);
        return;
    }
    bool ok = false;
    serial_flag = serial_flag_str.toInt(&ok);
    if(!ok) {
        failMissingValue(SERIAL_FLAG_ATTR);
        return;
    }

    QString cancel_flag_str = el.attribute(CANCEL_FLAG_ATTR);
    if (!cancel_flag_str.isEmpty()) {
        ok = false;
        cancel_flag = cancel_flag_str.toInt(&ok);
        if(!ok) {
            failMissingValue(CANCEL_FLAG_ATTR);
            return;
        }
    }
    QString run_after_all_subs_flag_str = el.attribute(RUN_AFTER_ALL_SUBS_FINISHED_FLAG_ATTR );
    if (!run_after_all_subs_flag_str.isEmpty()) {
        ok = false;
        run_after_all_subs_flag = run_after_all_subs_flag_str.toInt(&ok);
        if(!ok) {
            stateInfo.setError(QString("Mandatory attribute not set: %1").arg(RUN_AFTER_ALL_SUBS_FINISHED_FLAG_ATTR ));
            return;
        }
    }
    task = new StateOrderTestTask(this, (run_after_all_subs_flag ? TaskFlag_None : TaskFlag_RunBeforeSubtasksFinished));

    for (int i=0; i<subtask_num; i++) {
        StateOrderTestTask *sub = new StateOrderTestTask(this, TaskFlag_None);
        subs.append(sub);
        task->addSubTask(sub);
    }
    AppContext::getTaskScheduler()->registerTopLevelTask(task);
}


void GTest_TaskStateOrder::run() {
    while (!done_flag || (!subs.isEmpty() && !isCanceled())) {
        SThread::msleep(100);
    }
}

void GTest_TaskStateOrder::func(StateOrderTestTask *_task, StateOrderType st) {

    int ind = -1;
    if(task == _task) {
        if(st == StateOrder_Done) {
            done_flag = true;
            return;
        }
    } else {
        ind = subs.indexOf(_task);
        if(ind < 0) {
            stateInfo.setError(QString("task test internal error: can't find subtask in list").arg(1));
            return;
        }
        if(st == StateOrder_Done) {
            subs.removeAt(ind);
            return;
        }
    }
    int &step = _task->step;
    State newState = _task->getState();

    if(step == 0) {
        if(cancel_flag) {
            _task->cancel();
        }
        if(serial_flag) {
            for(int i=0; i<ind; i++) {
                if(!subs[i]->isFinished()) {
                    stateInfo.setError(QString("task serial subtask promoting error"));
                    return;
                }
            }
        }
        step++;
        if(newState != State_New) {
            stateInfo.setError(QString("task promoting error: state value not matched %1, expected %2").arg(newState).arg(State_New));
            return;
        }
        return;
    } else
        if(step == 1) {
            step++;
            if(task == _task) {
                if(run_after_all_subs_flag) {
                    for(int i=0; i<subs.count(); i++) {
                        if(!subs[i]->isFinished()) {
                            stateInfo.setError(QString("task promoting error: run after all subtasks finished"));
                            return;
                        }
                    }
                }
            }
            if(task->getStateInfo().cancelFlag) {
                if(newState == State_Running && st == StateOrder_Run) {
                    stateInfo.setError(QString("task promoting error: run canceled task"));
                    return;
                }
            } else
                if(newState != State_Running) {
                    stateInfo.setError(QString("task promoting error: state value not matched %1, expected %2").arg(newState).arg(State_Running));
                    return;
                }
        } else
            if(step == 2) {
                if (st != StateOrder_Report) {
                    stateInfo.setError(QString("task promoting error"));
                    return;
                }
            }
}

Task::ReportResult GTest_TaskStateOrder::report() {
    subs.clear();
    return ReportResult_Finished;
}

void GTest_Wait::init(U2::XMLTestFormat *tf, const QDomElement &el) {
    Q_UNUSED(tf);
    waitOk = false;
    condition = WaitCond_None;
    QString ms_str = el.attribute(DELAY_ATTR);
    if (ms_str.isEmpty()) {
        failMissingValue(DELAY_ATTR);
        return;
    }
    bool ok = false;
    ms = ms_str.toInt(&ok);
    if(!ok) {
        failMissingValue(DELAY_ATTR);
        return;
    }
    objContextName = el.attribute(OBJ_ATTR);
    if (!objContextName.isEmpty()) {
        waitCondString = el.attribute(CONDITION_ATTR);
        if(waitCondString.isEmpty()) {
            failMissingValue(CONDITION_ATTR);
            return;
        }
        if (waitCondString == "WaitCond_StateChanged") {
            condition = WaitCond_StateChanged;
            waitStateString = el.attribute(STATE_ATTR);
            if(waitStateString.isEmpty()) {
                failMissingValue(STATE_ATTR);
                return;
            }
            ok = false;
            waitForState = stateFromString(waitStateString, &ok);
            if (!ok) {
                failMissingValue(STATE_ATTR);
                return;
            }
        }
        else {
            //do nothing;
        }
    }
}

void GTest_Wait::prepare() {
    if(!objContextName.isEmpty()) {
        Task *task = getContext<Task>(this, objContextName);
        if (task==NULL) {
            stateInfo.setError(QString("invalid context %1").arg(objContextName));
            return;
        }
        this->connect(task,SIGNAL(si_stateChanged()),SLOT(sl_WaitCond_StateChanged()));
    }
}

void GTest_Wait::sl_WaitCond_StateChanged() {
    Task *task = getContext<Task>(this, objContextName);
    if (task==NULL) {
        stateInfo.setError(QString("invalid context %1").arg(objContextName));
        return;
    }
    if(task->getState() == waitForState) {
        waitOk = true;
        ms = 0;
    }
}

void GTest_Wait::run() {
    if(!objContextName.isEmpty()) {
        Task *task = getContext<Task>(this, objContextName);
        if (task==NULL) {
            stateInfo.setError(QString("invalid context %1").arg(objContextName));
            return;
        }
        QTime timer;
        timer.start();
        while(timer.elapsed() < ms && !isCanceled()) {
            if(task->getState() == waitForState) {
                break;
            }
            SThread::msleep(100);
        }
    } else {
        QTime timer;
        timer.start();
        while(timer.elapsed() < ms && !isCanceled()) {
            SThread::msleep(100);
        }
    }
}

Task::ReportResult GTest_Wait::report() {
    if (!hasError() && (condition!=WaitCond_None && !waitOk)) {
        setError(QString("Wait condition was not met: cond: %1, state: %2").arg(waitCondString).arg(waitStateString));
    }
    return ReportResult_Finished;
}

QList<XMLTestFactory*> TaskTests::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(GTest_TaskStateOrder::createFactory());
    res.append(GTest_TaskCreateTest::createFactory());
    res.append(GTest_TaskAddSubtaskTest::createFactory());
    res.append(GTest_TaskCancelTest::createFactory());
    res.append(GTest_TaskCheckState::createFactory());
    res.append(GTest_TaskCheckFlag::createFactory());
    res.append(GTest_TaskExec::createFactory());
    res.append(GTest_Wait::createFactory());
    return res;
}

}//namespace
