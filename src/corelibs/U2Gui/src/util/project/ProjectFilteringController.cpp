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
#include <U2Core/Counter.h>
#include <U2Core/L10n.h>
#include <U2Core/ProjectFilterTaskRegistry.h>
#include <U2Core/U2SafePoints.h>

#include "ProjectFilteringController.h"

namespace U2 {

const int ProjectFilteringController::FILTER_START_INTERVAL = 2000;

ProjectFilteringController::ProjectFilteringController(QObject *p)
    : QObject(p)
{
    filterStarter.setSingleShot(true);
    connect(&filterStarter, SIGNAL(timeout()), SLOT(sl_startFiltering()));
}

void ProjectFilteringController::startFiltering(const ProjectTreeControllerModeSettings &settings,
    const QList<QPointer<Document> > &docs)
{
    stopFiltering();
    emit si_filteringStarted();

    lastSettings = settings;
    lastDocs = docs;

    filterStarter.start(FILTER_START_INTERVAL);
}

void ProjectFilteringController::stopFiltering() {
    foreach (AbstractProjectFilterTask *task, activeFilteringTasks) {
        task->cancel();
    }
    emit si_filteringFinished();
}

void ProjectFilteringController::sl_startFiltering() {
    CHECK(!lastDocs.isEmpty(), );
    if (!activeFilteringTasks.isEmpty()) {
        filterStarter.start(FILTER_START_INTERVAL);
        return;
    }

    ProjectFilterTaskRegistry *registry = AppContext::getProjectFilterTaskRegistry();
    foreach(AbstractProjectFilterTask *task, registry->createFilterTasks(lastSettings, lastDocs)) {
        addNewActiveTask(task);
    }
    emit si_filteringStarted();
    GCOUNTER(cvar, tvar, "Project filtering launch");
}

void ProjectFilteringController::sl_objectsFiltered(const QString &groupName, const SafeObjList &objs) {
    AbstractProjectFilterTask *filterTask = qobject_cast<AbstractProjectFilterTask *>(sender());
    SAFE_POINT(NULL != filterTask, L10N::nullPointerError("project filter task"), );
    SAFE_POINT(!groupName.isEmpty(), "Invalid project filter group name", );

    if (!filterTask->isCanceled() && !filterTask->hasError()) {
        emit si_objectsFiltered(groupName, objs);
    }
}

void ProjectFilteringController::sl_filteringFinished() {
    AbstractProjectFilterTask *task = qobject_cast<AbstractProjectFilterTask *>(sender());
    SAFE_POINT(NULL != task, L10N::nullPointerError("project filter task"), );

    if (task->isFinished()) {
        activeFilteringTasks.remove(task);

        if (activeFilteringTasks.isEmpty() && !filterStarter.isActive()) {
            emit si_filteringFinished();
        }
    }
}

void ProjectFilteringController::addNewActiveTask(AbstractProjectFilterTask *task) {
    SAFE_POINT(NULL != task, L10N::nullPointerError("Project filtering task"), );

    connectNewTask(task);
    SAFE_POINT(!activeFilteringTasks.contains(task), "Unexpected project filter task", );
    activeFilteringTasks.insert(task);
}

void ProjectFilteringController::connectNewTask(AbstractProjectFilterTask *task) {
    connect(task, SIGNAL(si_objectsFiltered(const QString &, const SafeObjList &)), SLOT(sl_objectsFiltered(const QString &, const SafeObjList &)));
    connect(task, SIGNAL(si_stateChanged()), SLOT(sl_filteringFinished()));
}

} // namespace U2
