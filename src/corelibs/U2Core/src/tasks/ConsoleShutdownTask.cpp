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

#include "ConsoleShutdownTask.h"

#include <U2Core/Log.h>
#include <U2Core/ServiceModel.h>
#include <U2Core/PluginModel.h>
#include <U2Core/AppContext.h>
#include <U2Core/ProjectModel.h>

namespace U2 {

ConsoleShutdownTask::ConsoleShutdownTask(QCoreApplication* app)
: Task(tr("Shutdown"), TaskFlags_NR_FOSCOE | TaskFlag_NoAutoDelete), app(app)
{
    connect(AppContext::getTaskScheduler(), SIGNAL(si_topLevelTaskUnregistered(Task*)), SLOT(startShutdown()));
    connect(app, SIGNAL(aboutToQuit()), SLOT(startShutdown()));
}

void ConsoleShutdownTask::startShutdown() {

    if (sender() == app) {
        coreLog.info("Shutdown initiated by user");
    } else {
        if (AppContext::getTaskScheduler()->getTopLevelTasks().size() != 0) {
            return;
        }
        coreLog.info("All tasks finished, shutting down");
    }
    app->disconnect(this, SLOT(startShutdown()));
    AppContext::getTaskScheduler()->disconnect(this, SLOT(startShutdown()));
    AppContext::getTaskScheduler()->registerTopLevelTask(this);
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

class CancelAllTask : public Task {
public:
    CancelAllTask() : Task(ConsoleShutdownTask::tr("Cancel active tasks"), TaskFlag_NoRun) {}
    void prepare() {
        // cancel all tasks but ShutdownTask
        QList<Task*> activeTopTasks = AppContext::getTaskScheduler()->getTopLevelTasks();
        activeTopTasks.removeOne(getTopLevelParentTask());
        foreach(Task* t, activeTopTasks) {
            coreLog.trace(QString("Canceling: %1").arg(t->getTaskName()));
            t->cancel();
        }
    }

    ReportResult report() {
        foreach(Task* t, AppContext::getTaskScheduler()->getTopLevelTasks()) {
            if (t->isCanceled() && !t->isFinished()) {
                return ReportResult_CallMeAgain;
            }
        }
        return ReportResult_Finished;
    }
};


void ConsoleShutdownTask::prepare() {
    coreLog.info(tr("Starting shutdown process..."));

    addSubTask(new CancelAllTask());
}

QList<Task*> ConsoleShutdownTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;

    if (isCanceled() || subTask->hasError()) {
        return res; //stop shutdown process
    }

    ServiceRegistry* sr = AppContext::getServiceRegistry();
    Service* s = findServiceToDisable(sr);
    if (s!=NULL) {
        res.append(sr->disableServiceTask(s));
    }
    return res;
}

Task::ReportResult ConsoleShutdownTask::report() {
    if (stateInfo.cancelFlag) {
        coreLog.info(tr("Shutdown was canceled"));
        return Task::ReportResult_Finished;
    }
    if (propagateSubtaskError()) {
        coreLog.error(tr("Shutdown failed, error: %1").arg(stateInfo.getError()));
        return Task::ReportResult_Finished;
    }

#ifdef _DEBUG
    const QList<Service*>& services = AppContext::getServiceRegistry()->getServices();
    foreach(Service* s, services) {
        assert(s->isDisabled());
    }
#endif
    app->quit();
    return Task::ReportResult_Finished;
}

}//namespace
