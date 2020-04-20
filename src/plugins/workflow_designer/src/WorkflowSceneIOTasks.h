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

#ifndef _U2_WORKFLOW_SCENE_IO_TASK_H_
#define _U2_WORKFLOW_SCENE_IO_TASK_H_

#include <QPointer>

#include <U2Core/Task.h>
#include <U2Lang/Schema.h>
#include <U2Lang/WorkflowIOTasks.h>

namespace U2 {

class WorkflowScene;
using namespace Workflow;

class SaveWorkflowSceneTask : public Task {
    Q_OBJECT
public:
    static const QString SCHEMA_PATHS_SETTINGS_TAG;

public:
    SaveWorkflowSceneTask(Schema *schema, const Metadata& meta);
    virtual void run();

private:
    Schema *schema;
    Metadata meta;
};

class LoadWorkflowSceneTask : public Task {
    Q_OBJECT
public:
    LoadWorkflowSceneTask(Schema *schema, Metadata *meta, WorkflowScene *scene, const QString &url, bool noUrl = false, bool disableWizardAutorun = false);
    virtual void run();
    virtual Task::ReportResult report();

private:
    void resetSceneAndScheme( );

    Schema *schema;
    Metadata *meta;
    WorkflowScene *scene;
    QString url;
    QString rawData;
    LoadWorkflowTask::FileFormat format;
    bool noUrl;
    bool disableWizardAutorun;
};

} //namespace
#endif
