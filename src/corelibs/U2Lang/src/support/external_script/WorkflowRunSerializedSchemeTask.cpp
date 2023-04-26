/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "WorkflowRunSerializedSchemeTask.h"

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>

#include <U2Lang/Schema.h>
#include <U2Lang/ScriptContext.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowIOTasks.h>
#include <U2Lang/WorkflowMonitor.h>
#include <U2Lang/WorkflowRunTask.h>
#include <U2Lang/WorkflowUtils.h>

namespace U2 {

/////// BaseSerializedSchemeRunner implementation /////////////////////////////////////////////////

BaseSerializedSchemeRunner::BaseSerializedSchemeRunner(const QString& _pathToScheme,
                                                       ScriptableScheduler* _scheduler,
                                                       QStringList& outputFiles)
    : Task(tr("Workflow run from serialized scheme"), TaskFlag_NoRun),
      workflowResultFiles(outputFiles), pathToScheme(_pathToScheme), scheduler(_scheduler) {
}

/////// WorkflowRunSerializedSchemeTask implementation ////////////////////////////////////////////

WorkflowRunSerializedSchemeTask::WorkflowRunSerializedSchemeTask(const QString& _pathToScheme,
                                                                 ScriptableScheduler* _scheduler,
                                                                 QStringList& outputFiles)
    : BaseSerializedSchemeRunner(_pathToScheme, _scheduler, outputFiles), scheme(nullptr),
      loadTask(nullptr), runTask(nullptr) {
    GCOUNTER(cvar, "workflow_run_from_script");

    loadTask = createLoadSchemeTask();
    if (loadTask != nullptr) {
        addSubTask(loadTask);
    }
}

QList<Task*> WorkflowRunSerializedSchemeTask::onSubTaskFinished(Task* subtask) {
    Q_ASSERT(subtask != nullptr);
    QList<Task*> res;

    propagateSubtaskError();
    if (hasError() || isCanceled()) {
        return res;
    }

    if (subtask == loadTask) {
        Q_ASSERT(scheme == loadTask->getSchema());
        QMap<ActorId, ActorId> remapping = loadTask->getRemapping();

        if (scheme->getDomain().isEmpty()) {
            QList<QString> domainsId = WorkflowEnv::getDomainRegistry()->getAllIds();
            Q_ASSERT(!domainsId.isEmpty());
            if (!domainsId.isEmpty()) {
                scheme->setDomain(domainsId.first());
            }
        }

        QStringList errorList;
        bool good = WorkflowUtils::validate(*scheme, errorList);
        if (!good) {
            setError("\n\n" + errorList.join("\n\n"));
            return res;
        }
        // AppContext::getScriptContext( )->setWorkflowScheduler( scheduler );
        runTask = new WorkflowRunTask(*scheme, remapping);
        res.append(runTask);
    } else if (subtask == runTask) {
        const QList<Workflow::WorkflowMonitor*> workflowMonitors = runTask->getMonitors();
        for (Workflow::WorkflowMonitor* monitor : qAsConst(workflowMonitors)) {
            foreach (Workflow::Monitor::FileInfo file, monitor->getOutputFiles()) {
                workflowResultFiles.append(file.url);
            }
        }
    }
    return res;
}

LoadWorkflowTask* WorkflowRunSerializedSchemeTask::createLoadSchemeTask() {
    const QString approvedPath = WorkflowUtils::findPathToSchemaFile(pathToScheme);
    if (approvedPath.isEmpty()) {
        setError(tr("Cannot find workflow: %1").arg(pathToScheme));
        return nullptr;
    }

    scheme = QSharedPointer<Schema>::create();
    scheme->setDeepCopyFlag(true);
    return new LoadWorkflowTask(scheme, nullptr, approvedPath);
}

}  // namespace U2
