/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include "ShutdownTask.h"

#include <QCoreApplication>

#include <U2Core/AppContext.h>
#include <U2Core/ProjectService.h>
#include <U2Core/Settings.h>

#include "MainWindowImpl.h"

namespace U2 {

ShutdownTask::ShutdownTask()
    : Task(tr("Shutdown"), TaskFlags(TaskFlag_NoRun)), docsToRemoveAreFetched(false) {
}

static bool isReadyToBeDisabled(Service* s, ServiceRegistry* sr) {
    ServiceType st = s->getType();
    int nServicesOfTheSameType = sr->findServices(st).size();
    assert(nServicesOfTheSameType >= 1);
    foreach (Service* child, sr->getServices()) {
        if (!child->getParentServiceTypes().contains(st) || !child->isEnabled()) {
            continue;
        }
        if (nServicesOfTheSameType == 1) {
            return false;
        }
    }
    return true;
}

static Service* findServiceToDisable(ServiceRegistry* sr) {
    int nEnabled = 0;
    foreach (Service* s, sr->getServices()) {
        nEnabled += s->isEnabled() ? 1 : 0;
        if (s->isEnabled() && isReadyToBeDisabled(s, sr)) {
            return s;
        }
    }
    assert(nEnabled == 0);
    return nullptr;
}

static bool closeViews() {
    MWMDIManager* wm = AppContext::getMainWindow()->getMDIManager();
    MWMDIWindow* w = nullptr;
    // close windows one by one, asking active window first
    // straightforward foreach() cycle appears not flexible enough,
    // as interdependent windows may close each other (happened with TestRunner and TestReporter)
    while ((w = wm->getActiveWindow())) {
        if (!wm->closeMDIWindow(w)) {
            return false;
        }
    }
    while (!wm->getWindows().isEmpty()) {
        if (!wm->closeMDIWindow(wm->getWindows().first())) {
            return false;
        }
    }
    return true;
}

void ShutdownTask::prepare() {
    Task* ct = new CloseWindowsTask();
    addSubTask(ct);

    QList<Task*> activeTopLevelTaskList = AppContext::getTaskScheduler()->getTopLevelTasks();
    activeTopLevelTaskList.removeOne(this);
    if (!activeTopLevelTaskList.isEmpty()) {
        // To be run before closing any views
        // otherwise it may cancel tasks produced by closing windows (e.g. SaveWorkflowTask)
        ct->addSubTask(new CancelAllTask());
    }
}

QList<Task*> ShutdownTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;

    stateInfo.cancelFlag = subTask->isCanceled();
    if (isCanceled() || subTask->hasError()) {
        cancelShutdown();
        return res;
    }

    ServiceRegistry* sr = AppContext::getServiceRegistry();
    Service* s = findServiceToDisable(sr);
    if (s != nullptr) {
        res.append(sr->disableServiceTask(s));
    }

    // fetch documents from project while it's not released
    if (!docsToRemoveAreFetched) {
        Project* proj = AppContext::getProject();
        if (proj != nullptr) {
            docsToRemove = proj->getDocuments();
        }
        docsToRemoveAreFetched = true;
    }

    if (res.isEmpty()) {  // all services has stopped
        qDeleteAll(docsToRemove);
    }

    return res;
}

void ShutdownTask::cancelShutdown() {
    cancel();
}

Task::ReportResult ShutdownTask::report() {
    if (propagateSubtaskError() || hasError() || isCanceled()) {
        setErrorNotificationSuppression(true);
        return Task::ReportResult_Finished;
    }

    if (AppContext::getTaskScheduler()->getTopLevelTasks().size() > 1) {  // some documents are being deleted
        return Task::ReportResult_CallMeAgain;
    }

#ifdef _DEBUG
    const QList<Service*>& services = AppContext::getServiceRegistry()->getServices();
    foreach (Service* s, services) {
        assert(s->isDisabled());
    }
#endif
    return Task::ReportResult_Finished;
}

CloseWindowsTask::CloseWindowsTask()
    : Task(QObject::tr("Close windows"), TaskFlags(TaskFlags_NR_FOSE_COSC)) {
}

void CloseWindowsTask::prepare() {
    Project* proj = AppContext::getProject();
    if (proj == nullptr) {
        return;
    }
}

QList<Task*> CloseWindowsTask::onSubTaskFinished(Task* subTask) {
    if (subTask->isCanceled()) {
        stateInfo.cancelFlag = true;
        return QList<Task*>();
    }
    return QList<Task*>();
}

Task::ReportResult CloseWindowsTask::report() {
    // wait for saving/closing tasks if any
    foreach (Task* t, AppContext::getTaskScheduler()->getTopLevelTasks()) {
        if (t != getTopLevelParentTask() && !t->isFinished()) {
            return ReportResult_CallMeAgain;
        }
    }
    return ReportResult_Finished;
}

CancelAllTask::CancelAllTask()
    : Task(ShutdownTask::tr("Cancel active tasks"), TaskFlag_NoRun) {
}

void CancelAllTask::prepare() {
    // cancel all tasks but ShutdownTask
    QList<Task*> activeTopTasks = AppContext::getTaskScheduler()->getTopLevelTasks();
    activeTopTasks.removeOne(getTopLevelParentTask());
    foreach (Task* t, activeTopTasks) {
        t->cancel();
    }
}

Task::ReportResult CancelAllTask::report() {
    foreach (Task* t, AppContext::getTaskScheduler()->getTopLevelTasks()) {
        if (t->isCanceled() && !t->isFinished()) {
            return ReportResult_CallMeAgain;
        }
    }
    return ReportResult_Finished;
}

}  // namespace U2
