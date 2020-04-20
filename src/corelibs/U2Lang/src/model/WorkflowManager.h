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

#ifndef _U2_WORKFLOW_MNGR_H_
#define _U2_WORKFLOW_MNGR_H_

#include <assert.h>

#include <QList>
#include <QMap>
#include <QObject>
#include <QString>
#include <QVariant>

#include <U2Lang/Schema.h>
#include <U2Lang/WorkflowContext.h>
#include <U2Lang/WorkflowTransport.h>

#include <U2Core/IdRegistry.h>
#include <U2Core/Task.h>

namespace U2 {

class WorkflowDebugStatus;

namespace Workflow {

/**
 * Worker represents actor at runtime
 * takes input data from and puts output data to CommunicationChannels
 *
 * base class
 */
class U2LANG_EXPORT Worker {
public:
    Worker() : context(NULL) {}
    virtual ~Worker() {}

    // initialize input and output ports
    virtual void init() = 0;
    // if data in the ports is ready
    virtual bool isReady() const = 0;
    // get data from actor and return task
    // if you want your worker support scripting:
    // use BaseWorker::getMessageAndSetupScriptValues to take data from port
    virtual Task* tick() = 0;
    // nothing to do
    virtual bool isDone() const = 0;
    // opened files, etc...
    virtual void cleanup() = 0;

    void setContext(WorkflowContext *newContext) {context = newContext;}

protected:
    WorkflowContext *context;

}; // Worker

/**
* runtime state of worker
*/
enum WorkerState {
    WorkerWaiting,
    WorkerReady,
    WorkerRunning,
    WorkerDone,
    WorkerPaused
}; // WorkerState

/**
 * Worker for whole schema
 * can decide if workers can run in parallel or any other analysis
 */
class U2LANG_EXPORT Scheduler : public Worker {
public:
    Scheduler(Schema *sch) : schema(sch), lastTask(NULL) {}
    virtual WorkerState getWorkerState(const ActorId &) = 0;
    virtual Task * replayLastWorkerTick() = 0;
    // returning value indicates if current task was canceled
    virtual bool cancelCurrentTaskIfAllowed() = 0;
    virtual void makeOneTick(const ActorId &) = 0;
    virtual void setDebugInfo(WorkflowDebugStatus *newDebugInfo) {
        Q_ASSERT(NULL != newDebugInfo);
        debugInfo = newDebugInfo;
    }

protected:
    Schema *schema;
    Task *lastTask;
    WorkflowDebugStatus *debugInfo;

    virtual WorkerState getWorkerState(const Actor*) = 0;
}; // Scheduler

/**
 * represents controller between actor and runtime realizations of actor (Worker)
 * is a container of such controllers (inherits IdRegistry<DomainFactory>)
 * represents set of tasks of some computational domain
 *
 * says how to execute your worker (Scheduler and CommunicationChannel)
 * Usage: create execution domain factory (e.g. LocalDomainFactory) and register there your tasks factories
 */
class U2LANG_EXPORT DomainFactory : public IdRegistry<DomainFactory>, public Descriptor {
public:
    DomainFactory(const Descriptor& d) : Descriptor(d) {}
    DomainFactory(const QString& id) : Descriptor(id) {}
    virtual ~DomainFactory(){}

    // computational tasks domain
    virtual Worker* createWorker(Actor*) = 0;

    // execution domain
    virtual CommunicationChannel* createConnection(Link*) {return NULL;}
    virtual Scheduler* createScheduler(Schema*) {return NULL;}
    virtual void destroy(Scheduler*, Schema*) {}
    static void addParametersSetToMap(QVariantMap& map, const QString& attrValue, const QStringList& parametersList){
        QVariantMap parametersMap;
        foreach(const QString& curStr, parametersList) {
            parametersMap[curStr] = curStr;
        }
        map.insert(attrValue, parametersMap);
    }

}; // DomainFactory

/**
 * standard registry of factories
 * Usage: register here execution domain factories (see usage of DomainFactory)
 */
class U2LANG_EXPORT DomainFactoryRegistry : public IdRegistry<DomainFactory> {
}; // DomainFactoryRegistry

}//Workflow namespace

}//GB2 namespace


#endif
