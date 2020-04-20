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

#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

#include "AbstractProjectFilterTask.h"

namespace U2 {

//////////////////////////////////////////////////////////////////////////
/// AbstractProjectFilterTask
//////////////////////////////////////////////////////////////////////////

AbstractProjectFilterTask::AbstractProjectFilterTask(const ProjectTreeControllerModeSettings &settings, const QString &filterGroupName,
    const QList<QPointer<Document> > &docs)
    : Task(tr("Filtering project content by the \"%1\" criterion").arg(filterGroupName), TaskFlag_None), settings(settings), docs(docs),
    filterGroupName(filterGroupName), filteredObjCountPerIteration(10), totalObjectCount(0), processedObjectCount(0)
{
    tpm = Task::Progress_Manual;

    SAFE_POINT(!filterGroupName.isEmpty(), "Project filter has empty name", );
    doStaticInitialization();

    foreach (const QPointer<Document> &doc, docs) {
        if (!doc.isNull()) {
            totalObjectCount += doc->getObjects().size();
        }
    }
}

void AbstractProjectFilterTask::run() {
    foreach (const QPointer<Document> &doc, docs) {
        filterDocument(doc.data());
    }
    const int filteredObjectCount = filteredObjs.size();
    if (!stateInfo.isCoR() && 0 != filteredObjectCount % filteredObjCountPerIteration && filteredObjCountPerIteration > 1) {
        emit si_objectsFiltered(filterGroupName, filteredObjs.mid(filteredObjectCount - filteredObjectCount % filteredObjCountPerIteration));
    }
}

void AbstractProjectFilterTask::filterDocument(const QPointer<Document> &doc) {
    CHECK(!doc.isNull(), );
    CHECK(doc->isLoaded(), );

    foreach (GObject *obj, doc->getObjects()) {
        CHECK(!doc.isNull(), );
        if (filterAcceptsObject(obj)) {
            filteredObjs.append(obj);
            if (0 == filteredObjs.size() % filteredObjCountPerIteration || 1 == filteredObjCountPerIteration) {
                emit si_objectsFiltered(filterGroupName, filteredObjs.mid(filteredObjs.size() - filteredObjCountPerIteration));
            }
        }
        stateInfo.setProgress(++processedObjectCount / totalObjectCount);
        if (stateInfo.isCoR()) {
            break;
        }
    }
}

bool AbstractProjectFilterTask::filterAcceptsObject(GObject * /*obj*/) {
    FAIL("AbstractProjectFilterTask::filterAcceptsObject is not implemented", false);
}

void AbstractProjectFilterTask::doStaticInitialization() {
    static bool staticInitCompleted = false;
    if (!staticInitCompleted) {
        qRegisterMetaType<SafeObjList>("SafeObjList");
        staticInitCompleted = true;
    }
}

//////////////////////////////////////////////////////////////////////////
/// ProjectFilterTaskFactory
//////////////////////////////////////////////////////////////////////////

ProjectFilterTaskFactory::~ProjectFilterTaskFactory() {

}

AbstractProjectFilterTask * ProjectFilterTaskFactory::registerNewTask(const ProjectTreeControllerModeSettings &settings,
    const QList<QPointer<Document> > &docs) const
{
    AbstractProjectFilterTask *task = createNewTask(settings, docs);
    SAFE_POINT(NULL != task, L10N::nullPointerError("project filter task"), NULL);
    AppContext::getTaskScheduler()->registerTopLevelTask(task);
    return task;
}

} // namespace U2
