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

#ifndef _U2_WORKFLOW_ENV_H_
#define _U2_WORKFLOW_ENV_H_

#include <assert.h>
#include <U2Lang/Datatype.h>
#include <U2Lang/ExternalToolCfg.h>
#include <U2Lang/SchemaActorsRegistry.h>
#include "Descriptor.h"

namespace U2 {

class DataTypeRegistry;

namespace Workflow {

class DomainFactoryRegistry;
class ActorPrototypeRegistry;
class ActorValidatorRegistry;
class WorkflowTasksRegistry;

/**
 * base class for workflow environment
 * has one instance
 * same as AppContext only for workflow
 *
 * see WorkflowEnvImpl for realization
 */
class U2LANG_EXPORT WorkflowEnv {
public:
    static bool init(WorkflowEnv* instance);
    static void shutdown() { delete instance; instance = NULL;}

    static DataTypeRegistry* getDataTypeRegistry() {return getInstance()->data;}
    static ActorPrototypeRegistry* getProtoRegistry() {return getInstance()->proto;}
    static DomainFactoryRegistry* getDomainRegistry() {return getInstance()->domain;}
    static DataTypeValueFactoryRegistry* getDataTypeValueFactoryRegistry() { return getInstance()->dvfReg; }
    static ExternalToolCfgRegistry* getExternalCfgRegistry() {return getInstance()->ecfgReg;}
    static SchemaActorsRegistry *getSchemaActorsRegistry() {return getInstance()->schemaActorsReg;}
    static WorkflowTasksRegistry *getWorkflowTasksRegistry() {return getInstance()->workflowTasksRegistry;}
    static ActorValidatorRegistry *getActorValidatorRegistry() {return getInstance()->actorValidatorRegistry;}

protected:
    static WorkflowEnv* instance;
    static WorkflowEnv* getInstance();

public:
    virtual ~WorkflowEnv() {}

protected:
    virtual DataTypeRegistry * initDataRegistry() = 0;
    virtual ActorPrototypeRegistry * initProtoRegistry() = 0;
    virtual DomainFactoryRegistry * initDomainRegistry() = 0;
    virtual DataTypeValueFactoryRegistry * initDataTypeValueFactoryRegistry() = 0;
    virtual ExternalToolCfgRegistry * initExternalToolCfgRegistry() = 0;
    virtual SchemaActorsRegistry * initSchemaActorsRegistry() = 0;
    virtual WorkflowTasksRegistry * initWorkflowTasksRegistry() = 0;
    virtual ActorValidatorRegistry * initActorValidatorRegistry() = 0;

protected:
    DataTypeRegistry *data;
    ActorPrototypeRegistry *proto;
    DomainFactoryRegistry *domain;
    DataTypeValueFactoryRegistry *dvfReg;
    ExternalToolCfgRegistry *ecfgReg;
    SchemaActorsRegistry *schemaActorsReg;
    WorkflowTasksRegistry *workflowTasksRegistry;
    ActorValidatorRegistry *actorValidatorRegistry;

}; // WorkflowEnv

} //namespace Workflow

} //namespace U2

#endif // _U2_WORKFLOW_ENV_H_
