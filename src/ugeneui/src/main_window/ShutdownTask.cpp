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

#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/Log.h>
#include <U2Core/PluginModel.h>
#include <U2Core/ProjectService.h>
#include <U2Core/ServiceModel.h>
#include <U2Core/Settings.h>

#include "MainWindowImpl.h"
#include "ShutdownTask.h"
#include "project_support/ProjectLoaderImpl.h"

namespace U2 {

ShutdownTask::ShutdownTask(MainWindowImpl* _mw)
    : Task(tr("Shutdown"), TaskFlags(TaskFlag_NoRun)), mw(_mw), docsToRemoveAreFetched(false)
{

}

static bool isReadyToBeDisabled(Service* s, ServiceRegistry* sr) {
    ServiceType st = s->getType();
    int nServicesOfTheSameType = sr->findServices(st).size();
    assert(nServicesOfTheSameType >= 1);
    foreach(Service* child, sr->getServices()) {
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
    foreach(Service* s, sr->getServices()) {
        nEnabled+= s->isEnabled() ? 1 : 0;
        if (s->isEnabled() && isReadyToBeDisabled(s, sr)) {
            return s;
        }
    }
    assert(nEnabled == 0);
return NULL;
}

static bool closeViews() {
    MWMDIManager* wm = AppContext::getMainWindow()->getMDIManager();
    MWMDIWindow* w = NULL;
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

// This function prepends empty string to RecentProjects in UGENE SETTINGS in order
// to prevent project auto loading on next UGENE launch
static void cancelProjectAutoLoad() {
    QStringList recentFiles = AppContext::getSettings()->getValue(SETTINGS_DIR + RECENT_PROJECTS_SETTINGS_NAME, QStringList(),  true).toStringList();
    QString emptyUrl;
    recentFiles.prepend(emptyUrl);
    AppContext::getSettings()->setValue(SETTINGS_DIR + RECENT_PROJECTS_SETTINGS_NAME, recentFiles, true);
}

void ShutdownTask::prepare() {
    coreLog.info(tr("Starting shutdown process..."));
    mw->setShutDownInProcess(true);

    Project* currProject = AppContext::getProject();
    if (currProject == NULL) {
        cancelProjectAutoLoad();
    }

    Task* ct = new CloseWindowsTask();
    addSubTask(ct);

    QList<Task*> activeTopTasks = AppContext::getTaskScheduler()->getTopLevelTasks();
    activeTopTasks.removeOne(this);
    if (!activeTopTasks.isEmpty()) {
        QStringList sl;
        foreach(Task* t, activeTopTasks) {
            sl.append(t->getTaskName());
        }

        QMessageBox::StandardButton ret = QMessageBox::question(mw->getQMainWindow(),
            tr("Shutdown confirmation"),
            tr("There are active tasks. Stop them now?")+QString("\n\n - %1").arg(sl.join("\n - ")),
            QMessageBox::Ok|QMessageBox::Cancel);
        if (ret != QMessageBox::Ok) {
            cancel();
            return;
        }
        // To be run before closing any views
        // otherwise it may cancel tasks produced by closing windows (e.g. SaveWorkflowTask)
        ct->addSubTask(new CancelAllTask());
    }
}

QList<Task*> ShutdownTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;

    stateInfo.cancelFlag = subTask->isCanceled();
    if (isCanceled() || subTask->hasError() ) {
        cancelShutdown();
        return res;
    }

    ServiceRegistry* sr = AppContext::getServiceRegistry();
    Service* s = findServiceToDisable(sr);
    if (s!=NULL) {
        res.append(sr->disableServiceTask(s));
    }

    // fetch documents from project while it's not released
    if (!docsToRemoveAreFetched) {
        Project *proj = AppContext::getProject();
        if (NULL != proj) {
            docsToRemove = proj->getDocuments();
        }
        docsToRemoveAreFetched = true;
    }

    if (res.isEmpty()) { // all services has stopped
        qDeleteAll(docsToRemove);
    }

    return res;
}

void ShutdownTask::cancelShutdown() {
    mw->setShutDownInProcess(false);
    cancel();
}

Task::ReportResult ShutdownTask::report() {
    if (propagateSubtaskError() || hasError() || isCanceled()) {
        setErrorNotificationSuppression(true);
        mw->setShutDownInProcess(false);
        return Task::ReportResult_Finished;
    }

    if (AppContext::getTaskScheduler()->getTopLevelTasks().size() > 1) { // some documents are being deleted
        return Task::ReportResult_CallMeAgain;
    }

#ifdef _DEBUG
    const QList<Service*>& services = AppContext::getServiceRegistry()->getServices();
    foreach(Service* s, services) {
        assert(s->isDisabled());
    }
#endif
    mw->close();
    QCoreApplication::exit(0);
    return Task::ReportResult_Finished;
}

CloseWindowsTask::CloseWindowsTask()
    : Task(QObject::tr("Close windows"), TaskFlags(TaskFlag_NoRun))
{

}

void CloseWindowsTask::prepare() {
    Project* proj = AppContext::getProject();
    if (proj == NULL) {
        return;
    }
    if ( proj->isTreeItemModified() || proj->getProjectURL().isEmpty() ) {
        addSubTask(AppContext::getProjectService()->saveProjectTask(SaveProjectTaskKind_SaveProjectAndDocumentsAskEach));
    }
}

QList<Task *> CloseWindowsTask::onSubTaskFinished(Task *subTask) {
    if (subTask->isCanceled()) {
        stateInfo.cancelFlag = true;
        return QList<Task*>();
    }
    coreLog.trace(tr("Closing views"));
    if (!closeViews()) {
        getTopLevelParentTask()->cancel();
    }
    return QList<Task*>();
}

Task::ReportResult CloseWindowsTask::report() {
    // wait for saving/closing tasks if any
    foreach(Task* t, AppContext::getTaskScheduler()->getTopLevelTasks()) {
        if (t != getTopLevelParentTask() && !t->isFinished()) {
            return ReportResult_CallMeAgain;
        }
    }
    return ReportResult_Finished;
}

CancelAllTask::CancelAllTask()
    : Task(ShutdownTask::tr("Cancel active tasks"), TaskFlag_NoRun)
{

}

void CancelAllTask::prepare() {
    // cancel all tasks but ShutdownTask
    QList<Task*> activeTopTasks = AppContext::getTaskScheduler()->getTopLevelTasks();
    activeTopTasks.removeOne(getTopLevelParentTask());
    foreach(Task* t, activeTopTasks) {
        coreLog.trace(tr("Canceling: %1").arg(t->getTaskName()));
        t->cancel();
    }
}

Task::ReportResult CancelAllTask::report() {
    foreach(Task* t, AppContext::getTaskScheduler()->getTopLevelTasks()) {
        if (t->isCanceled() && !t->isFinished()) {
            return ReportResult_CallMeAgain;
        }
    }
    return ReportResult_Finished;
}

}   // namespace U2
