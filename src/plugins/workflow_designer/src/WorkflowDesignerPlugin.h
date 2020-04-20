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

#ifndef _U2_FLOWCHART_H_
#define _U2_FLOWCHART_H_

#include <U2Core/PluginModel.h>
#include <U2Core/ServiceModel.h>
#include <U2Core/Task.h>

#include <U2Gui/WelcomePageAction.h>

#include "SampleActionsManager.h"

class QAction;
class QEvent;

namespace U2 {

class WorkflowDesignerPlugin : public Plugin {
    Q_OBJECT
public:
    static const QString RUN_WORKFLOW;
    static const QString REMOTE_MACHINE;
    static const QString PRINT;
    static const QString CUSTOM_EL_WITH_SCRIPTS_DIR;
    static const QString CUSTOM_EXTERNAL_TOOL_DIR;
    static const QString INCLUDED_ELEMENTS_DIR;
    static const QString WORKFLOW_OUTPUT_DIR;

public:
    WorkflowDesignerPlugin ();
    ~WorkflowDesignerPlugin ();
private:
    void registerCMDLineHelp();
    void registerWorkflowTasks();
    void processCMDLineOptions();

private slots:
    void sl_saveSchemaImageTaskFinished();
    void sl_initWorkers();
};

class WorkflowDesignerService : public Service {
    Q_OBJECT
public:
    WorkflowDesignerService();
    bool closeViews();
protected:
    virtual Task* createServiceEnablingTask();

    virtual Task* createServiceDisablingTask();

    virtual void serviceStateChangedCallback(ServiceState oldState, bool enabledStateChanged);

public slots:
    void sl_showDesignerWindow();
    void sl_sampleActionClicked(const SampleAction &action);

private slots:
    void sl_showManagerWindow();
    void sl_startWorkflowPlugin();

private:
    bool checkServiceState() const;
    void initDesignerAction();
    void initNewWorkflowAction();
    void initSampleActions();

private:
    QAction*        designerAction;
    QAction*        managerAction;
    QAction*        newWorkflowAction;
};


class WorkflowWelcomePageAction : public WelcomePageAction {
public:
    WorkflowWelcomePageAction(WorkflowDesignerService *service);
    void perform();

private:
    QPointer<WorkflowDesignerService> service;
};

} //namespace

#endif
