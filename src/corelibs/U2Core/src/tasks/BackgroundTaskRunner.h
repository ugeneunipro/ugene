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

#ifndef __BACKGROUND_TASK_RUNNER_H__
#define __BACKGROUND_TASK_RUNNER_H__

#include <QMap>

#include <U2Core/Task.h>
#include <U2Core/AppContext.h>

namespace U2 {

/**
 * Simple template task which allows to grab its result.
 * Intended to be used as a base class for tasks for BackgroundTaskRunner.
 */
template<class Result>
class BackgroundTask : public Task {
public:
    inline Result getResult() const {return result;}
protected:
    BackgroundTask(const QString& _name, TaskFlags f) : Task(_name, f){
        setVerboseOnTaskCancel(false); // do not add messages about the task canceling into the log
    }
    Result result;
};


/**
 * Stub containing Q_OBJECT macro, signals&slots. Classes with signal/slot related
 * stuff can't be templates, so everything needed for BackgroundTaskRunner is moved here
 */
class U2CORE_EXPORT BackgroundTaskRunner_base: public QObject {
    Q_OBJECT
public:
    virtual ~BackgroundTaskRunner_base();
    virtual void emitFinished();
signals:
    void si_finished();
private slots:
    virtual void sl_finished() = 0;
};

/**
 * Simple manager for background tasks.
 * Allows running only one background task at a time, canceling previous task
 * when the new one is queued with run(). Emits si_finished() (defined in the base)
 * when the queued task is finished. Cancels current task in destructor.
 */
template<class Result>
class BackgroundTaskRunner : public BackgroundTaskRunner_base {
public:
    BackgroundTaskRunner() : task(0), success(false) {}

    virtual ~BackgroundTaskRunner() {
        cancel();
    }

    void run(BackgroundTask<Result> * newTask)  {
        if(task) {
            task->cancel();
        }
        task = newTask;
        connect(task, SIGNAL(si_stateChanged()), SLOT(sl_finished()));
        AppContext::getTaskScheduler()->registerTopLevelTask(task);
    }

    void cancel() {
        if(task) {
            task->cancel();
            task = NULL;
        }
    }

    inline Result getResult() const {
        if(task) {
            return Result();
        }
        return result;
    }

    inline bool isIdle() const {
        return !task;
    }

    /**
     * Returns true if last finished task wasn't cancelled and finished without error
     */
    inline bool isSuccessful() const {
        return success;
    }

    inline QString getError() const {
        return lastError;
    }

private:
    virtual void sl_finished() {
        BackgroundTask<Result> * senderr = dynamic_cast<BackgroundTask<Result>*>(sender());
        assert(senderr);
        if(task != senderr) {
            return;
        }
        if(Task::State_Finished != senderr->getState()) {
            return;
        }
        result = task->getResult();
        success = ! task->getStateInfo().isCoR();
        lastError = task->getError();
        task = NULL;
        emitFinished();
    }

private:
    BackgroundTask<Result> * task;
    Result result;
    bool success;
    QString lastError;

private:
    BackgroundTaskRunner(const BackgroundTaskRunner &);
    BackgroundTaskRunner operator=(const BackgroundTaskRunner &);
};


}

#endif
