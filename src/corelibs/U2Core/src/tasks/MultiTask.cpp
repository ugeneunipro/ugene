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

#include <U2Core/AppContext.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/U2SafePoints.h>

#include "MultiTask.h"

namespace U2 {

MultiTask::MultiTask(const QString &name, const QList<Task *> &taskz, bool withLock, TaskFlags f)
    : Task(name, f), l(nullptr), tasks(taskz) {
    setMaxParallelSubtasks(1);
    SAFE_POINT(!taskz.empty(), "No tasks provided to multitask", );

    foreach( Task * t, taskz ) {
        addSubTask(t);
    }
    if (withLock) {
        SAFE_POINT(AppContext::getProject() != NULL, "MultiTask::no project", );
        l = new StateLock(getTaskName(), StateLockFlag_LiveLock);
        AppContext::getProject()->lockState(l);
    }
}

QList<Task *> MultiTask::getTasks() const {
    return tasks;
}

Task::ReportResult MultiTask::report() {
    Project *p = AppContext::getProject();
    if (l != nullptr && p != nullptr) {
        p->unlockState(l);
        delete l;
        l = nullptr;
    }
    foreach(Task* t, tasks) {
        CHECK_CONTINUE(t->isConcatenateChildrenErrors());

        setReportingSupported(true);
        setReportingEnabled(true);
        break;
    }
    return Task::ReportResult_Finished;
}

QString MultiTask::generateReport() const {
    QString report = "<hr><br>";
    foreach (Task *subtask, tasks) {
        if (subtask->hasFlags(TaskFlags(TaskFlag_ReportingIsSupported | TaskFlag_ReportingIsEnabled))) {
            report += QString("Subtask <b>'%1'</b>:<br><br>").arg(subtask->getTaskName());
            report += subtask->generateReport();
            report += "<br><hr><br>";
        }
    }
    return report;
}

//////////////////////////////////////////////////////////////////////////
//SequentialMultiTask

SequentialMultiTask::SequentialMultiTask(const QString &name, const QList<Task *> &taskz, TaskFlags f)
    : Task(name, f), tasks(taskz)
{
    setMaxParallelSubtasks(1);
}

void SequentialMultiTask::prepare() {
    //run the first task
    if (tasks.size() > 0) {
        addSubTask(tasks.first());
    }

}

QList<Task *> SequentialMultiTask::onSubTaskFinished(Task *subTask) {
    QList<Task *> res;

    int idx = tasks.indexOf(subTask);
    if ((idx != -1) && (idx + 1 < tasks.size())) {
        res.append(tasks.at(idx+1));
    }

    return res;
}

QList<Task *> SequentialMultiTask::getTasks() const {
    return tasks;
}

} //namespace
