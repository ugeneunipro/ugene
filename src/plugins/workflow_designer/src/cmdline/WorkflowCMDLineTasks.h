/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include <U2Lang/WorkflowIOTasks.h>

namespace U2 {

class WorkflowRunFromCMDLineBase : public Task {
    Q_OBJECT
public:
    WorkflowRunFromCMDLineBase();
    ~WorkflowRunFromCMDLineBase() override = default;
    QList<Task*> onSubTaskFinished(Task* subTask) override;
    Task::ReportResult report() override;

protected:
    virtual Task* getWorkflowRunTask() const = 0;

private:
    LoadWorkflowTask* prepareLoadSchemaTask(const QString& schemaName);
    void processLoadSchemaTask(const QString& schemaName, int optionIdx);
    QByteArray getReportFromError() const;

protected:
    QSharedPointer<Schema> schema;
    int optionsStartAt;
    LoadWorkflowTask* loadTask;
    Task* workflowRunTask;
    QString schemaName;
    QMap<ActorId, ActorId> remapping;

};  // WorkflowRunFromCMDLineBase

class WorkflowRunFromCMDLineTask : public WorkflowRunFromCMDLineBase {
    Q_OBJECT
public:
    Task* getWorkflowRunTask() const override;
    QString generateReport() const override;
};

}  // namespace U2
