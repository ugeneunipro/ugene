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

#include <QFile>

#include <U2Core/AppContext.h>
#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/Settings.h>
#include <U2Core/Counter.h>
#include <U2Core/CMDLineRegistry.h>
#include <U2Core/CMDLineUtils.h>
#include <U2Core/CMDLineCoreOptions.h>

#include <U2Lang/URLAttribute.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowUtils.h>
#include <U2Lang/WorkflowManager.h>
#include <U2Lang/WorkflowRunTask.h>

#include "WorkflowDesignerPlugin.h"
#include "WorkflowCMDLineTasks.h"

#define WORKFLOW_CMDLINE_TASK_LOG_CAT "Workflow cmdline tasks"

namespace U2 {

/*******************************************
* WorkflowRunFromCMDLineBase
*******************************************/
WorkflowRunFromCMDLineBase::WorkflowRunFromCMDLineBase()
    : Task(tr("Workflow run from cmdline"), TaskFlag_None),
      schema(NULL),
      optionsStartAt(-1),
      loadTask(NULL),
      workflowRunTask(NULL)
{
    GCOUNTER(cvar,tvar,"workflow_run_from_cmdline");

    CMDLineRegistry * cmdLineRegistry = AppContext::getCMDLineRegistry();

    // try to process schema without 'task' option (it can only be the first one)
    QStringList pureValues = CMDLineRegistryUtils::getPureValues();
    if( !pureValues.isEmpty() ) {
        QString schemaName = pureValues.first();
        processLoadSchemaTask( schemaName, 1 ); // because after program name
    }
    if( loadTask != NULL ) {
        addSubTask( loadTask );
        return;
    }

    // process schema with 'task' option
    int taskOptionIdx = CMDLineRegistryUtils::getParameterIndex( WorkflowDesignerPlugin::RUN_WORKFLOW );
    if(taskOptionIdx != -1) {
        processLoadSchemaTask( cmdLineRegistry->getParameterValue( WorkflowDesignerPlugin::RUN_WORKFLOW, taskOptionIdx ), taskOptionIdx );
    }
    if( loadTask == NULL ) {
        setError( tr( "no task to run" ) );
        return;
    }
    addSubTask( loadTask );
}

void WorkflowRunFromCMDLineBase::processLoadSchemaTask( const QString & schemaNameCandidate, int optionIdx ) {
    loadTask = prepareLoadSchemaTask( schemaNameCandidate );
    if( loadTask != NULL ) {
        schemaName = schemaNameCandidate;
        optionsStartAt = optionIdx + 1;
    }
}

LoadWorkflowTask * WorkflowRunFromCMDLineBase::prepareLoadSchemaTask( const QString & schemaName ) {
    QString pathToSchema = WorkflowUtils::findPathToSchemaFile( schemaName );
    if( pathToSchema.isEmpty() ) {
        coreLog.error( tr( "Cannot find workflow: %1" ).arg( schemaName ) );
        return NULL;
    }

    schema = new Schema();
    schema->setDeepCopyFlag(true);
    return new LoadWorkflowTask( schema, NULL, pathToSchema );
}

WorkflowRunFromCMDLineBase::~WorkflowRunFromCMDLineBase() {
    delete schema;
}

static void setSchemaCMDLineOptions( Schema * schema, int optionsStartAtIdx ) {
    assert( schema != NULL && optionsStartAtIdx > 0 );

    QList<StrStrPair> parameters = AppContext::getCMDLineRegistry()->getParameters();
    int sz = parameters.size();
    for( int i = optionsStartAtIdx; i < sz; ++i ) {
        const StrStrPair & param = parameters.at(i);
        if( param.first.isEmpty() ) { // TODO: unnamed parameters not supported yet
            continue;
        }

        QString paramAlias = param.first;
        QString paramName;
        Actor * actor = WorkflowUtils::findActorByParamAlias( schema->getProcesses(), paramAlias, paramName );
        if( actor == NULL ) {
            assert( paramName.isEmpty() );
            coreLog.details( WorkflowRunFromCMDLineBase::tr( "alias '%1' not set in workflow" ).arg( paramAlias ) );
            continue;
        }

        Attribute * attr = actor->getParameter( paramName );
        if( attr == NULL ) {
            coreLog.error( WorkflowRunFromCMDLineBase::tr( "actor parameter '%1' not found" ).arg( paramName ) );
            continue;
        }

        DataTypeValueFactory * valueFactory = WorkflowEnv::getDataTypeValueFactoryRegistry()->
            getById( attr->getAttributeType()->getId() );
        if( valueFactory == NULL ) {
            coreLog.error( WorkflowRunFromCMDLineBase::tr( "cannot parse value from '%1'" ).arg( param.second ) );
            continue;
        }

        ActorId id = actor->getId();
        bool isOk;
        QVariant value = valueFactory->getValueFromString( param.second, &isOk );
        if(!isOk){
            coreLog.error( WorkflowRunFromCMDLineBase::tr( "Incorrect value for '%1', null or default value passed to workflow" ).
                arg( param.first ));
            continue;
        }
        attr->setAttributeValue(value);
    }
}

QList<Task*> WorkflowRunFromCMDLineBase::onSubTaskFinished( Task* subTask ) {
    assert( subTask != NULL );
    QList<Task*> res;

    propagateSubtaskError();
    if( hasError() || isCanceled() ) {
        return res;
    }
    assert( !hasError() ); // if error, we won't be here

    if( loadTask == subTask ) {
        Schema * schema = loadTask->getSchema();
        assert( schema != NULL );
        remapping = loadTask->getRemapping();

        setSchemaCMDLineOptions( schema, optionsStartAt );
        if( schema->getDomain().isEmpty() ) {
            QList<QString> domainsId = WorkflowEnv::getDomainRegistry()->getAllIds();
            assert(!domainsId.isEmpty());
            if(!domainsId.isEmpty()) { schema->setDomain(domainsId.first()); }
        }

        QStringList l;
        bool good = WorkflowUtils::validate(*schema, l);
        if(!good) {
            QString schemaHelpStr = QString("\n\nsee 'ugene --help=%1' for details").arg(schemaName);
            setError("\n\n" + l.join("\n\n") + schemaHelpStr);
            return res;
        }

        workflowRunTask = getWorkflowRunTask();
        res << workflowRunTask;
    }
    return res;
}

void WorkflowRunFromCMDLineBase::run() {
    CMDLineRegistry *cmdLineRegistry = AppContext::getCMDLineRegistry();
    SAFE_POINT(NULL != cmdLineRegistry, "CMDLineRegistry is NULL", );
    CHECK(NULL != workflowRunTask, );

    const QString reportFilePath = cmdLineRegistry->getParameterValue(CmdlineTaskRunner::REPORT_FILE_ARG);
    CHECK(!reportFilePath.isEmpty(), );

    QFile reportFile(reportFilePath);
    const bool opened = reportFile.open(QIODevice::WriteOnly);
    CHECK_EXT(opened, setError(L10N::errorOpeningFileWrite(reportFilePath)), );

    reportFile.write(workflowRunTask->generateReport().toLocal8Bit());
}

/*******************************************
* WorkflowRunFromCMDLineTask
*******************************************/
Task * WorkflowRunFromCMDLineTask::getWorkflowRunTask() const {
    return new WorkflowRunTask(*schema, remapping);
}

} // U2
