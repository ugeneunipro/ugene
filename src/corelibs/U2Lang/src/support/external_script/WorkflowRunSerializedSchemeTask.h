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

#pragma once

#include <U2Core/Task.h>

namespace U2 {

namespace Workflow {
class Schema;
}

class LoadWorkflowTask;
class ScriptableScheduler;
class WorkflowRunTask;

class U2LANG_EXPORT BaseSerializedSchemeRunner : public Task {
    Q_OBJECT
public:
    BaseSerializedSchemeRunner(const QString& pathToScheme,
                               ScriptableScheduler* scheduler,
                               QStringList& outputFiles);

protected:
    QStringList& workflowResultFiles;
    const QString pathToScheme;
    ScriptableScheduler* scheduler;
};

class U2LANG_EXPORT WorkflowRunSerializedSchemeTask : public BaseSerializedSchemeRunner {
    Q_OBJECT
public:
    WorkflowRunSerializedSchemeTask(const QString& pathToScheme,
                                    ScriptableScheduler* scheduler,
                                    QStringList& outputFiles);
    QList<Task*> onSubTaskFinished(Task* subTask) override;
    QStringList getWorkflowOutputFiles() const;

private:
    LoadWorkflowTask* createLoadSchemeTask();

    QSharedPointer<Workflow::Schema> scheme;
    LoadWorkflowTask* loadTask;
    WorkflowRunTask* runTask;
};

}  // namespace U2
