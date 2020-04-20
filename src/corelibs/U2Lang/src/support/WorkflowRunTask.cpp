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

#include <QCoreApplication>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/U2SafePoints.h>

#include <U2Lang/HRSchemaSerializer.h>
#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowDebugMessageParser.h>
#include <U2Lang/WorkflowEnv.h>
#include <U2Lang/WorkflowMonitor.h>

#include "WorkflowDebugStatus.h"
#include "WorkflowRunTask.h"

namespace U2 {

WorkflowAbstractRunner::WorkflowAbstractRunner(const QString &name, TaskFlags flags)
: CmdlineTask(name, flags)
{

}

const QList<WorkflowMonitor*> & WorkflowAbstractRunner::getMonitors() const {
    return monitors;
}

WorkflowAbstractIterationRunner::WorkflowAbstractIterationRunner(const QString &name, TaskFlags flags)
: Task(name, flags)
{

}

/*******************************************
 * WorkflowRunTask
 *******************************************/
WorkflowRunTask::WorkflowRunTask(const Schema& sh, const QMap<ActorId, ActorId>& remap, WorkflowDebugStatus *debugInfo)
: WorkflowAbstractRunner(tr("Execute workflow"),
    TaskFlags(TaskFlag_NoRun) | TaskFlag_ReportingIsSupported | TaskFlag_OnlyNotificationReport), rmap(remap), flows(sh.getFlows())
{
    GCOUNTER( cvar, tvar, "WorkflowRunTask" );
    if (NULL == debugInfo) {
        debugInfo = new WorkflowDebugStatus;
    }
    if (NULL == debugInfo->parent()) {
        debugInfo->setParent(this);
    }

    WorkflowIterationRunTask* t = new WorkflowIterationRunTask(sh, debugInfo);
    WorkflowMonitor *m = t->getMonitor();
    if (NULL != m) {
        monitors << m;
    }
    connect(t, SIGNAL(si_ticked()), SIGNAL(si_ticked()));
    addSubTask(t);

    setMaxParallelSubtasks(MAX_PARALLEL_SUBTASKS_AUTO);
}

inline bool isValidFile(const QString &link, const qint64 &processStartTime) {
    GUrl url(link);
    if (url.isLocalFile()) {
        if (QFile::exists(link)) {
            QFileInfo info(link);
            bool createdAtThisRun = (info.lastModified().toTime_t() >= processStartTime);
            return createdAtThisRun;
        }
    }
    return false;
}

QList<WorkerState> WorkflowRunTask::getState( Actor* actor) {
    QList<WorkerState> ret;
    foreach(const QPointer<Task> &t, getSubtasks()) {
        WorkflowIterationRunTask* rt = qobject_cast<WorkflowIterationRunTask*>(t.data());
        ret << rt->getState(actor->getId());
    }
    return ret;
}

int WorkflowRunTask::getMsgNum(const Link* l) {
    int ret = 0;
    foreach(const QPointer<Task> &t, getSubtasks()) {
        WorkflowIterationRunTask* rt = qobject_cast<WorkflowIterationRunTask*>(t.data());
        ret += rt->getMsgNum(l);
    }
    return ret;
}

int WorkflowRunTask::getMsgPassed(const Link* l) {
    int ret = 0;
    foreach(const QPointer<Task> &t, getSubtasks()) {
        ret += qobject_cast<WorkflowIterationRunTask*>(t.data())->getMsgPassed(l);
    }
    return ret;
}

QString WorkflowRunTask::generateReport() const {
    QString report;
    foreach (WorkflowMonitor *monitor, getMonitors()) {
        const QMap<QString, QMultiMap<QString, QString> > workersReports = monitor->getWorkersReports();
        foreach (const QString &worker, workersReports.keys()) {
            const QMultiMap<QString, QString> tasksReports = workersReports[worker];
            QString workerReport;
            foreach (const QString &taskName, tasksReports.uniqueKeys()) {
                foreach (const QString &taskReport, tasksReports.values(taskName)) {
                    if (!taskReport.isEmpty()) {
                        workerReport += QString("<div class=\"task\" id=\"%1\">%2</div>").arg(taskName).arg(QString(taskReport.toUtf8().toBase64()));
                    }
                }
            }
            report += QString("<div class=\"worker\" id=\"%1\">%2</div>").arg(worker).arg(workerReport);
        }
    }
    return report;
}

QString WorkflowRunTask::getTaskError() const {
    if (hasError()) {
        return getError();
    }

    foreach(WorkflowMonitor *monitor, monitors) {
        foreach(const WorkflowNotification &notification, monitor->getNotifications()) {
            if (WorkflowNotification::U2_ERROR == notification.type) {
                return notification.message;
            }
        }
    }
    return "";
}

/*******************************************
* WorkflowIterationRunTask
*******************************************/
namespace {
    const int UPDATE_PROGRESS_INTERVAL = 500;
}

WorkflowIterationRunTask::WorkflowIterationRunTask(const Schema& sh,
    WorkflowDebugStatus *initDebugInfo)
    : WorkflowAbstractIterationRunner(tr("Workflow run"),
    (getAdditionalFlags() | TaskFlag_CancelOnSubtaskCancel | TaskFlag_FailOnSubtaskError)),
    context(NULL), schema(new Schema()), scheduler(NULL), debugInfo(initDebugInfo),
    nextTickRestoring(false), contextInitialized(false)
{

    rmap = HRSchemaSerializer::deepCopy(sh, schema, stateInfo);
    SAFE_POINT_OP(stateInfo, );

    if(schema->getDomain().isEmpty()) {
        QList<DomainFactory*> factories = WorkflowEnv::getDomainRegistry()->getAllEntries();
        assert(!factories.isEmpty());
        schema->setDomain(factories.isEmpty() ? "" : factories.at(0)->getId());
    }
    DomainFactory* df = WorkflowEnv::getDomainRegistry()->getById(schema->getDomain());
    if (!df) {
        stateInfo.setError(  tr("Unknown domain %1").arg(schema->getDomain()) );
        return;
    }

    connect(debugInfo, SIGNAL(si_pauseStateChanged(bool)), SLOT(sl_pauseStateChanged(bool)));
    connect(debugInfo, SIGNAL(si_singleStepIsRequested(const ActorId &)), SLOT(sl_singleStepIsRequested(const ActorId &)));
    connect(debugInfo, SIGNAL(si_busInvestigationIsRequested(const Workflow::Link *, int)),
        SLOT(sl_busInvestigationIsRequested(const Workflow::Link *, int)));
    connect(debugInfo, SIGNAL(si_busCountOfMessagesIsRequested(const Workflow::Link *)),
        SLOT(sl_busCountOfMessagesRequested(const Workflow::Link *)));
    connect(debugInfo, SIGNAL(si_convertMessages2Documents(const Workflow::Link *,
        const QString &, int, const QString &)), SLOT(sl_convertMessages2Documents(
        const Workflow::Link *, const QString &, int, const QString &)));

    WorkflowMonitor *m = new WorkflowMonitor(this, schema);
    context = new WorkflowContext(schema->getProcesses(), m);

    QTimer * timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), SIGNAL(si_updateProducers()));
    timer->start(UPDATE_PROGRESS_INTERVAL);
}

TaskFlags WorkflowIterationRunTask::getAdditionalFlags() {
    TaskFlags result = (AppContext::isGUIMode())
        ? (TaskFlags(TaskFlag_RunMessageLoopOnly) | TaskFlag_RunBeforeSubtasksFinished)
        : TaskFlag_NoRun;
    return result;
}

WorkflowIterationRunTask::~WorkflowIterationRunTask() {
    lmap.clear();
    DomainFactory* df = WorkflowEnv::getDomainRegistry()->getById(schema->getDomain());
    if (df) {
        df->destroy(scheduler, schema);
    }
    scheduler = NULL;
// make all signals to be delivered to GUI before the scheme is destroyed
    QCoreApplication::processEvents();
    delete schema;
    delete context;
}

void WorkflowIterationRunTask::prepare() {
    if( hasError() || isCanceled() ) {
        return;
    }

    bool res = schema->expand();
    if (!res) {
        stateInfo.setError(tr("Failed to preprocess the workflow. Some of included files are broken"));
        return;
    }
    DomainFactory* df = WorkflowEnv::getDomainRegistry()->getById(schema->getDomain());
    assert( df != NULL ); // checked in constructor
    foreach(Actor* a, schema->getProcesses()) {
        Worker* w = df->createWorker(a);
        if (!w) {
            stateInfo.setError( tr("Failed to create worker %1 %2 in domain %3")\
                .arg(a->getProto()->getId()).arg(a->getId()).arg(schema->getDomain()) );
            return;
        }
    }
    foreach(Link* l, schema->getFlows()) {
        CommunicationChannel* cc = df->createConnection(l);
        if (!cc) {
            stateInfo.setError(  tr("Failed to create connection %1 %2 in domain %3") ); //fixme
            return;
        }
        QStringList lst;
        lst << rmap.key(l->source()->owner()->getId());
        lst << (l->source()->getId());
        lst << rmap.key(l->destination()->owner()->getId());
        lst << (l->destination()->getId());
        QString key = lst.join("|");
        lmap.insert(key, cc);
    }

    contextInitialized = context->init();
    if (!contextInitialized) {
        stateInfo.setError(tr("Failed to create a workflow context"));
        return;
    }
    debugInfo->setContext(context);
    scheduler = df->createScheduler(schema);
    if (!scheduler) {
        stateInfo.setError(  tr("Failed to create scheduler in domain %1").arg(df->getDisplayName()) );
        return;
    }
    scheduler->setContext(context);
    scheduler->init();
    scheduler->setDebugInfo(debugInfo);
    context->getMonitor()->start();
    while(scheduler->isReady() && !isCanceled()) {
        Task* t = scheduler->tick();
        if (t) {
            addSubTask(t);
            break;
        }
    }
}

QList<Task*> WorkflowIterationRunTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> tasks;
    // handle the situation when pause signal was not delivered to the current thread
    while(debugInfo->isPaused() && !isCanceled()) {
        QCoreApplication::processEvents();
    }
    if(scheduler->isReady() && nextTickRestoring) {
        Task *replayingTask = scheduler->replayLastWorkerTick();
        nextTickRestoring = false;
        if(NULL != replayingTask) {
            tasks << replayingTask;
            emit si_ticked();
            return tasks;
        }
    }

    if (subTask->hasError()) {
        getMonitor()->addTaskError(subTask);
    }
    if (subTask->hasWarning()) {
        getMonitor()->addTaskWarning(subTask);
    }
    while(scheduler->isReady() && !isCanceled()) {
        Task* t = scheduler->tick();
        if (t) {
            tasks << t;
            break;
        }
    }
    emit si_ticked();

    return tasks;
}

Task::ReportResult WorkflowIterationRunTask::report() {
    if (!contextInitialized) {
        return ReportResult_Finished;
    }
    context->getMonitor()->pause();
    if (scheduler) {
        scheduler->cleanup();
        if (!scheduler->isDone()) {
            if(!hasError() && !isCanceled()) {
                setError(tr("No workers are ready, while not all workers are done. Workflow is broken?"));
                algoLog.error(stateInfo.getError());
            }
        }
    }

    // add unregistered output files
    qint64 startTimeSec = getTimeInfo().startTime/1000000;
    foreach(Actor *a, schema->getProcesses()) {
        LocalWorkflow::BaseWorker *bw = a->castPeer<LocalWorkflow::BaseWorker>();
        QStringList urls = bw->getOutputFiles();
        foreach (const QString &url, urls) {
            QString absUrl = context->absolutePath(url);
            if (isValidFile(absUrl, startTimeSec)) {
                context->getMonitor()->addOutputFile(absUrl, a->getId());
            }
        }
    }

    emit si_updateProducers();
    return ReportResult_Finished;
}

WorkerState WorkflowIterationRunTask::getState(const ActorId& id)
{
    if (scheduler) {
        const WorkerState currentState = scheduler->getWorkerState(rmap.value(id));
        return (debugInfo->isPaused() && Workflow::WorkerRunning == currentState) ?
            Workflow::WorkerPaused : currentState;
    }
    return WorkerWaiting;
}

static QString getKey(const Link * l) {
    QStringList lst;
    lst << (l->source()->owner()->getId());
    lst << (l->source()->getId());
    lst << (l->destination()->owner()->getId());
    lst << (l->destination()->getId());
    return lst.join("|");
}

inline static bool isSourceActor(const QString &actor, const QString &key) {
    QStringList lst = key.split("|");
    CHECK(4 == lst.size(), false);
    return lst.first() == actor;
}

WorkflowMonitor * WorkflowIterationRunTask::getMonitor() const {
    CHECK(NULL != context, NULL);
    return context->getMonitor();
}

int WorkflowIterationRunTask::getMsgNum(const Link *l) {
    CommunicationChannel* cc = lmap.value(getKey(l));
    if (cc) {
        return cc->hasMessage();
    }
    return 0;
}

int WorkflowIterationRunTask::getMsgPassed(const Link* l) {
    CommunicationChannel * cc = lmap.value(getKey(l));
    if(cc != NULL) {
        return cc->takenMessages();
    }
    return 0;
}

QList<CommunicationChannel*> WorkflowIterationRunTask::getActorLinks(const QString &actor) {
    QList<CommunicationChannel*> result;

    QMap<QString, CommunicationChannel*>::ConstIterator i = lmap.constBegin();
    for (; i!= lmap.constEnd(); i++) {
        if (isSourceActor(actor, i.key())) {
            result << i.value();
        }
    }
    return result;
}

int WorkflowIterationRunTask::getDataProduced(const ActorId &actor) {
    int result = 0;
    foreach (CommunicationChannel *cc, getActorLinks(actor)) {
        result += cc->hasMessage();
        result += cc->takenMessages();
        break;
    }
    return result;
}

void WorkflowIterationRunTask::sl_pauseStateChanged(bool isPaused) {
    if (isPaused) {
        if (!debugInfo->isCurrentStepIsolated()) {
            nextTickRestoring = scheduler->cancelCurrentTaskIfAllowed();
        }
        if (AppContext::isGUIMode()) {
            AppContext::getTaskScheduler()->pauseThreadWithTask(this);
        }
    } else if (AppContext::isGUIMode()) {
        AppContext::getTaskScheduler()->resumeThreadWithTask(this);
    }
}

void WorkflowIterationRunTask::sl_busInvestigationIsRequested( const Workflow::Link *bus,
    int messageNumber )
{
    CommunicationChannel *channel = lmap.value( getKey( bus ) );
    if ( NULL != channel && debugInfo->isPaused( ) ) {
        QQueue<Message> messages = channel->getMessages( messageNumber, messageNumber );
        WorkflowDebugMessageParser *parser = debugInfo->getMessageParser( );
        SAFE_POINT( NULL != parser, "Invalid debug message parser!", );
        parser->setSourceData( messages );
        WorkflowInvestigationData data = parser->getAllMessageValues( );
        debugInfo->respondToInvestigator( data, bus );
    }
}

void WorkflowIterationRunTask::sl_busCountOfMessagesRequested(const Workflow::Link *bus) {
    debugInfo->respondMessagesCount(bus, getMsgNum(bus));
}

void WorkflowIterationRunTask::sl_singleStepIsRequested(const ActorId &actor) {
    if(debugInfo->isPaused()) {
        scheduler->makeOneTick(actor);
    }
}

void WorkflowIterationRunTask::sl_convertMessages2Documents( const Workflow::Link *bus,
    const QString &messageType, int messageNumber, const QString &schemeName )
{
    CommunicationChannel *channel = lmap.value( getKey( bus ) );
    if ( NULL != channel && debugInfo->isPaused( ) ) {
        QQueue<Message> messages = channel->getMessages( messageNumber, messageNumber );
        if ( !messages.isEmpty( ) ) {
            WorkflowDebugMessageParser *parser = debugInfo->getMessageParser( );
            SAFE_POINT( NULL != parser, "Invalid debug message parser!", );
            parser->setSourceData( messages );
            parser->convertMessagesToDocuments( messageType, schemeName, messageNumber );
        }
    }
}

}//namespace
