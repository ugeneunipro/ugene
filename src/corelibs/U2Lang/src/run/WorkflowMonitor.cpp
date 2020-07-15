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

#include "WorkflowMonitor.h"

#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Lang/ActorModel.h>
#include <U2Lang/HRSchemaSerializer.h>
#include <U2Lang/WorkflowRunTask.h>
#include <U2Lang/WorkflowUtils.h>

namespace U2 {
namespace Workflow {
using namespace Monitor;

const QString WorkflowMonitor::WORKFLOW_FILE_NAME("workflow.uwl");

WorkflowMonitor::WorkflowMonitor(WorkflowAbstractIterationRunner *_task, Schema *_schema)
    : QObject(), schema(_schema), task(_task), saveSchema(false), started(false), externalTools(false) {
    foreach (Actor *p, schema->getProcesses()) {
        procMap[p->getId()] = p;
        processNames[p->getId()] = p->getLabel();
        addTime(0, p->getId());
    }

    foreach (Actor *p, schema->getProcesses()) {
        WorkerParamsInfo info;
        info.actor = p;
        info.workerName = p->getLabel();
        QMap<QString, Attribute *> params = p->getParameters();
        QMapIterator<QString, Attribute *> paramsIter(params);
        while (paramsIter.hasNext()) {
            paramsIter.next();
            Attribute *attr = paramsIter.value();
            SAFE_POINT(NULL != attr, "NULL attribute in params!", );

            info.parameters << attr;
        }
        workersParamsInfo << info;
        if (p->getProto()->isExternalTool()) {
            externalTools = true;
        }
    }

    connect(task.data(), SIGNAL(si_updateProducers()), SIGNAL(si_updateProducers()));
    connect(task.data(), SIGNAL(si_progressChanged()), SLOT(sl_progressChanged()));
    connect(task.data(), SIGNAL(si_stateChanged()), SLOT(sl_taskStateChanged()));
}

const QList<FileInfo> &WorkflowMonitor::getOutputFiles() const {
    return outputFiles;
}

const QList<WorkflowNotification> &WorkflowMonitor::getNotifications() const {
    return notifications;
}

const QMap<QString, WorkerInfo> &WorkflowMonitor::getWorkersInfo() const {
    return workers;
}

const QList<WorkerParamsInfo> &WorkflowMonitor::getWorkersParameters() const {
    return workersParamsInfo;
}

const QMap<QString, Monitor::WorkerLogInfo> &WorkflowMonitor::getWorkersLog() const {
    return workersLog;
}

const QMap<QString, QMultiMap<QString, QString>> &WorkflowMonitor::getWorkersReports() const {
    return workersReports;
}

QString WorkflowMonitor::actorName(const QString &id) const {
    SAFE_POINT(processNames.contains(id), QString("Unknown actor id: '%1'").arg(id), "");
    return processNames[id];
}

void WorkflowMonitor::addOutputFile(const QString &url, const QString &producer, bool openBySystem) {
    CHECK(!url.isEmpty(), );
    FileInfo info(MonitorUtils::toSlashedUrl(url), producer, openBySystem);

    CHECK(!outputFiles.contains(info), );

    outputFiles << info;
    emit si_newOutputFile(info);
}

void WorkflowMonitor::addOutputFolder(const QString &url, const QString &producer) {
    addOutputFile(url, producer, true);
}

void WorkflowMonitor::addInfo(const QString &message, const QString &actor, const QString &type) {
    addNotification(WorkflowNotification(message, actor, type));
}

void WorkflowMonitor::addError(const QString &message, const QString &actor, const QString &type) {
    addNotification(WorkflowNotification(message, actor, type));
    coreLog.error(message);
}

void WorkflowMonitor::addTaskError(Task *task, const QString &message) {
    SAFE_POINT(taskMap.contains(task), "Unregistered task", );
    CHECK(!errorTasks.contains(task), );
    QString error = message.isEmpty() ? task->getError() : message;
    addError(error, taskMap[task]->getId());
    errorTasks << task;
}

void WorkflowMonitor::addTaskWarning(Task *task, const QString &message) {
    SAFE_POINT(taskMap.contains(task), "Unregistered task", );
    ActorId id = taskMap[task]->getId();
    if (!message.isEmpty()) {
        addError(message, id, WorkflowNotification::U2_WARNING);
    } else {
        foreach (const QString &warning, task->getWarnings()) {
            addError(warning, id, WorkflowNotification::U2_WARNING);
        }
    }
}

void WorkflowMonitor::addTime(qint64 timeMks, const QString &actor) {
    WorkerInfo &info = workers[actor];
    info.timeMks += timeMks;
    emit si_workerInfoChanged(actor, info);
}

void WorkflowMonitor::addTick(qint64 timeMks, const QString &actor) {
    workers[actor].ticks += 1;
    addTime(timeMks, actor);
}

void WorkflowMonitor::start() {
    SAFE_POINT(!started, "The workflow is already started", );
    started = true;
    setRunState(false);
}

void WorkflowMonitor::pause() {
    SAFE_POINT(started, "The workflow is not started yet", );
    setRunState(true);
}

void WorkflowMonitor::resume() {
    SAFE_POINT(started, "The workflow is not started yet", );
    setRunState(false);
}

bool WorkflowMonitor::isExternalToolScheme() const {
    return externalTools;
}

void WorkflowMonitor::registerTask(Task *task, const QString &actor) {
    SAFE_POINT(procMap.contains(actor), "Unknown actor id", );
    taskMap[task] = procMap[actor];
    connect(new TaskSignalMapper(task), SIGNAL(si_taskFinished(Task *)), SLOT(sl_workerTaskFinished(Task *)));
}

void WorkflowMonitor::setOutputDir(const QString &dir) {
    _outputDir = dir;
    emit si_dirSet(outputDir());

    if (saveSchema) {
        QString url = outputDir() + "report/" + WORKFLOW_FILE_NAME;
        U2OpStatus2Log os;
        HRSchemaSerializer::saveSchema(schema, meta.data(), url, os);
    }
}

QString WorkflowMonitor::outputDir() const {
    return _outputDir;
}

QString WorkflowMonitor::getLogsDir() const {
    return outputDir() + "logs";
}

QString WorkflowMonitor::getLogUrl(const QString &actorId, int actorRunNumber, const QString &toolName, int toolRunNumber, int contentType) const {
    WDListener *listener = getListener(actorId, actorRunNumber, toolName, toolRunNumber);
    switch (contentType) {
    case ExternalToolListener::OUTPUT_LOG:
        return listener->getStdoutLogFileUrl();
    case ExternalToolListener::ERROR_LOG:
        return listener->getStderrLogFileUrl();
    default:
        FAIL(tr("An unexpected contentType: %1").arg(contentType), QString());
    }
}

void WorkflowMonitor::sl_progressChanged() {
    CHECK(!task.isNull(), );
    emit si_progressChanged(task->getProgress());
}

void WorkflowMonitor::sl_taskStateChanged() {
    CHECK(!task.isNull(), );
    if (task->isFinished()) {
        TaskState state = SUCCESS;
        if (task->isCanceled()) {
            state = CANCELLED;
        } else if (task->hasError()) {
            state = FAILED;
        } else if (!notifications.isEmpty()) {
            if (hasErrors()) {
                state = FAILED;
            } else if (hasWarnings()) {
                state = FINISHED_WITH_PROBLEMS;
            } else {
                state = SUCCESS;
            }
        }

        for (QMap<QString, Monitor::WorkerLogInfo>::iterator i = workersLog.begin(); i != workersLog.end(); ++i) {
            qDeleteAll(i.value().logs);
            i.value().logs.clear();
        }

        emit si_taskStateChanged(state);
        emit si_report();
    }
}

void WorkflowMonitor::sl_workerTaskFinished(Task *workerTask) {
    Actor *actor = taskMap.value(workerTask, NULL);
    SAFE_POINT(NULL != actor, QString("An unknown task finished: %1").arg(workerTask->getTaskName()), );
    CHECK(workerTask->isReportingEnabled(), );
    workersReports[actor->getId()].insert(workerTask->getTaskName(), workerTask->generateReport());
}

void WorkflowMonitor::setWorkerInfo(const QString &actorId, const WorkerInfo &info) {
    workers[actorId] = info;
    emit si_workerInfoChanged(actorId, info);
}

void WorkflowMonitor::setRunState(bool paused) {
    emit si_runStateChanged(paused);
}

int WorkflowMonitor::getDataProduced(const QString &actor) const {
    CHECK(!task.isNull(), 0);
    return task->getDataProduced(actor);
}

bool WorkflowMonitor::containsOutputFile(const QString &url) const {
    foreach (const FileInfo &info, outputFiles) {
        if (info.url == MonitorUtils::toSlashedUrl(url)) {
            return true;
        }
    }
    return false;
}

void WorkflowMonitor::addNotification(const WorkflowNotification &notification) {
    const bool firstNotification = notifications.isEmpty();
    notifications << notification;

    if (firstNotification) {
        emit si_firstNotification();
    }
    foreach (const WorkflowNotification &notification, notifications) {
        if (WorkflowNotification::U2_ERROR == notification.type || WorkflowNotification::U2_WARNING == notification.type) {
            emit si_taskStateChanged(RUNNING_WITH_PROBLEMS);
            break;
        }
    }
    int count = 0;
    foreach (const WorkflowNotification &info, notifications) {
        if (notification == info) {
            count++;
        }
    }
    emit si_newNotification(notification, count);
}

bool WorkflowMonitor::hasWarnings() const {
    foreach (WorkflowNotification notification, notifications) {
        CHECK(notification.type != WorkflowNotification::U2_WARNING, true);
    }
    return false;
}

bool WorkflowMonitor::hasErrors() const {
    foreach (WorkflowNotification notification, notifications) {
        CHECK(notification.type != WorkflowNotification::U2_ERROR, true);
    }
    return false;
}

void WorkflowMonitor::setSaveSchema(const Metadata &_meta) {
    meta.reset(new Metadata(_meta));
    saveSchema = true;
}

QList<ExternalToolListener *> WorkflowMonitor::createWorkflowListeners(const QString &workerId, const QString &workerName, int listenersNumber) {
    QList<ExternalToolListener *> listeners;
    WorkerLogInfo &logInfo = workersLog[workerId];
    logInfo.workerRunNumber++;
    for (int i = 0; i < listenersNumber; i++) {
        WDListener *newListener = new WDListener(this, workerId, workerName, logInfo.workerRunNumber);
        listeners.append(newListener);
    }
    logInfo.logs.append(listeners);
    return listeners;
}

WDListener *WorkflowMonitor::getListener(const QString &actorId, int actorRunNumber, const QString &toolName, int toolRunNumber) const {
    foreach (ExternalToolListener *listener, workersLog[actorId].logs) {
        WDListener *wdListener = dynamic_cast<WDListener *>(listener);
        SAFE_POINT(nullptr != wdListener, "Can't cast ExternalToolListener to WDListener", nullptr);
        if (actorRunNumber == wdListener->getActorRunNumber() &&
            actorId == wdListener->getActorId() &&
            toolName == wdListener->getToolName() &&
            toolRunNumber == wdListener->getToolRunNumber()) {
            return wdListener;
        }
    }
    return nullptr;
}

int WorkflowMonitor::getNewToolRunNumber(const QString &actorId, int actorRunNumber, const QString &toolName) {
    int toolRunNumber = 1;
    foreach (ExternalToolListener *listener, workersLog[actorId].logs) {
        WDListener *wdListener = dynamic_cast<WDListener *>(listener);
        SAFE_POINT(nullptr != wdListener, "Can't cast ExternalToolListener to WDListener", 0);
        if (toolName == wdListener->getToolName() && actorRunNumber == wdListener->getActorRunNumber()) {
            toolRunNumber++;
        }
    }
    return toolRunNumber;
}

void WorkflowMonitor::onLogChanged(const WDListener *listener, int messageType, const QString &message) {
    U2::Workflow::Monitor::LogEntry entry;
    entry.toolName = listener->getToolName();
    entry.actorId = listener->getActorId();
    entry.actorName = listener->getActorName();
    entry.actorRunNumber = listener->getActorRunNumber();
    entry.toolRunNumber = listener->getToolRunNumber();
    entry.contentType = messageType;
    entry.lastLine = message;
    emit si_logChanged(entry);
}

/************************************************************************/
/* FileInfo */
/************************************************************************/
FileInfo::FileInfo()
    : url(), actor(), openBySystem(false), isDir(false) {
}

FileInfo::FileInfo(const QString &_url, const QString &_producer, bool _openBySystem)
    : url(_url),
      actor(_producer),
      openBySystem(_openBySystem),
      isDir(QFileInfo(url).isDir()) {
    if (isDir) {
        openBySystem = true;
        if (url.endsWith("/")) {
            url.chop(1);
        }
    }
}

bool FileInfo::operator==(const FileInfo &other) const {
    return url == other.url;
}

/************************************************************************/
/* WorkerInfo */
/************************************************************************/
WorkerInfo::WorkerInfo()
    : ticks(0), timeMks(0) {
}

/************************************************************************/
/* WorkerParamsInfo */
/************************************************************************/
WorkerParamsInfo::WorkerParamsInfo() {
}

/************************************************************************/
/* MonitorUtils */
/************************************************************************/
QMap<QString, QList<FileInfo>> MonitorUtils::filesByActor(const WorkflowMonitor *m) {
    QMap<QString, QList<FileInfo>> result;
    foreach (const FileInfo &info, m->getOutputFiles()) {
        result[info.actor] << info;
    }
    return result;
}

QStringList MonitorUtils::sortedByAppearanceActorIds(const WorkflowMonitor *m) {
    QStringList result;
    foreach (const FileInfo &info, m->getOutputFiles()) {
        if (!result.contains(info.actor)) {
            result << info.actor;
        }
    }
    return result;
}

QString MonitorUtils::toSlashedUrl(const QString &url) {
    QString result = url;
    result.replace("\\", "/");
    return result;
}

namespace {

class Registrator {
    static const bool isMetaRegistered;
};

static bool registerMeta() {
    qRegisterMetaType<Monitor::TaskState>("Monitor::TaskState");
    return true;
}

const bool Registrator::isMetaRegistered = registerMeta();

}    // namespace

/************************************************************************/
/* WDListener */
/************************************************************************/
WDListener::WDListener(WorkflowMonitor *_monitor, const QString &_actorId, const QString &_actorName, int _actorRunNumber)
    : monitor(_monitor),
      actorId(_actorId),
      actorName(_actorName),
      actorRunNumber(_actorRunNumber),
      toolRunNumber(0),
      outputHasMessages(false),
      errorHasMessages(false) {
    FileAndDirectoryUtils::createWorkingDir("", FileAndDirectoryUtils::CUSTOM, monitor->getLogsDir(), "");
}

void WDListener::addNewLogMessage(const QString &message, int messageType) {
    if (NULL != logProcessor) {
        logProcessor->processLogMessage(message);
    }
    writeToFile(messageType, message);
    monitor->onLogChanged(this, messageType, message);
}

void WDListener::setToolName(const QString &toolName) {
    toolRunNumber = monitor->getNewToolRunNumber(actorId, actorRunNumber, toolName);
    ExternalToolListener::setToolName(toolName);
}

QString WDListener::getStdoutLogFileUrl() {
    if (!outputLogFile.isOpen()) {
        initLogFile(OUTPUT_LOG);
    }
    return outputLogFile.fileName();
}

QString WDListener::getStderrLogFileUrl() {
    if (!errorLogFile.isOpen()) {
        initLogFile(ERROR_LOG);
    }
    return errorLogFile.fileName();
}

QString WDListener::getStdoutLogFileUrl(const QString &actorId, int runNumber, const QString &toolName, int toolRunNumber) {
    return actorId + "_run_" + QString::number(runNumber) + "_" +
           toolName + "_run_" + QString::number(toolRunNumber) +
           "_stdout_log.txt";
}

QString WDListener::getStderrLogFileUrl(const QString &actorId, int runNumber, const QString &toolName, int toolRunNumber) {
    return actorId + "_run_" + QString::number(runNumber) + "_" +
           toolName + "_run_" + QString::number(toolRunNumber) +
           "_stderr_log.txt";
}

void WDListener::initLogFile(int contentType) {
    const QString logsDir = monitor->getLogsDir();
    switch (contentType) {
    case OUTPUT_LOG:
        CHECK(!outputLogFile.isOpen(), );
        outputLogFile.setFileName(GUrlUtils::rollFileName(logsDir + "/" + getStdoutLogFileUrl(actorName, actorRunNumber, getToolName(), toolRunNumber), "_"));
        outputLogFile.open(QIODevice::WriteOnly);
        outputLogStream.setDevice(&outputLogFile);
        break;
    case ERROR_LOG:
        CHECK(!errorLogFile.isOpen(), );
        errorLogFile.setFileName(GUrlUtils::rollFileName(logsDir + "/" + getStderrLogFileUrl(actorName, actorRunNumber, getToolName(), toolRunNumber), "_"));
        errorLogFile.open(QIODevice::WriteOnly);
        errorLogStream.setDevice(&errorLogFile);
        break;
    default:
        FAIL(QString("An unexpected contentType: %1").arg(contentType), );
    }
}

void WDListener::writeToFile(int messageType, const QString &message) {
    switch (messageType) {
    case OUTPUT_LOG:
        if (!outputLogFile.isOpen()) {
            initLogFile(OUTPUT_LOG);
        }
        writeToFile(outputLogStream, message);
        if (!outputHasMessages) {
            outputLogStream.flush();
            outputHasMessages = true;
        }
        break;
    case ERROR_LOG:
        if (!errorLogFile.isOpen()) {
            initLogFile(ERROR_LOG);
        }
        writeToFile(errorLogStream, message);
        if (!errorHasMessages) {
            errorLogStream.flush();
            errorHasMessages = true;
        }
        break;
    default:;    // Do not write to file
    }
}

void WDListener::writeToFile(QTextStream &logStream, const QString &message) {
    CHECK(logStream.device()->isOpen(), );
    logStream << message;
}

}    // namespace Workflow
}    // namespace U2
