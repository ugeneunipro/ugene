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

#ifndef _U2_WORKFLOW_CMDLINE_TASKS_H_
#define _U2_WORKFLOW_CMDLINE_TASKS_H_

#include <U2Core/Task.h>
#include <U2Lang/WorkflowIOTasks.h>

namespace U2 {

class WorkflowRunFromCMDLineBase : public Task {
    Q_OBJECT
public:
    WorkflowRunFromCMDLineBase();
    virtual ~WorkflowRunFromCMDLineBase();
    QList<Task*> onSubTaskFinished( Task* subTask );
    void run();

protected:
    virtual Task * getWorkflowRunTask() const = 0;

private:
    LoadWorkflowTask * prepareLoadSchemaTask( const QString & schemaName );
    void processLoadSchemaTask( const QString & schemaName, int optionIdx );

protected:
    Schema*             schema;
    int                 optionsStartAt;
    LoadWorkflowTask *  loadTask;
    Task *              workflowRunTask;
    QString             schemaName;
    QMap<ActorId, ActorId> remapping;

}; // WorkflowRunFromCMDLineBase

class WorkflowRunFromCMDLineTask : public WorkflowRunFromCMDLineBase {
    Q_OBJECT
public:
    virtual Task * getWorkflowRunTask() const;
}; // WorkflowRunFromCMDLineTask

} // U2

#endif // _U2_WORKFLOW_CMDLINE_TASKS_H_
