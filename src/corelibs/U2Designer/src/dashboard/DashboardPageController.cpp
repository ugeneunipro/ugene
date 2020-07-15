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

#include "DashboardPageController.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaMethod>

#include "Dashboard.h"
#include "DashboardJsAgent.h"
#include "webview/WebViewController.h"

namespace U2 {

DashboardPageController::DashboardPageController(Dashboard *_dashboard, U2WebView *webView)
    : QObject(webView),
      progress(0),
      state(Dashboard::STATE_RUNNING),
      dashboard(_dashboard),
      monitor(_dashboard->getMonitor()) {
    agent = new DashboardJsAgent(_dashboard);
    webViewController = new WebViewController(webView, agent);
    connect(webViewController, SIGNAL(si_pageIsAboutToBeInitialized()), SLOT(sl_pageIsAboutToBeInitialized()));
    connect(webViewController, SIGNAL(si_pageReady()), SLOT(sl_pageInitialized()));

    if (monitor != nullptr) {
        connect(monitor, SIGNAL(si_progressChanged(int)), SLOT(sl_progressChanged(int)));
        connect(monitor, SIGNAL(si_taskStateChanged(Monitor::TaskState)), SLOT(sl_taskStateChanged(Monitor::TaskState)));
        connect(monitor, SIGNAL(si_newNotification(WorkflowNotification, int)), SLOT(sl_newNotification(WorkflowNotification, int)), Qt::UniqueConnection);
        connect(monitor, SIGNAL(si_workerInfoChanged(const QString &, const Monitor::WorkerInfo &)), SLOT(sl_workerInfoChanged(const QString &, const Monitor::WorkerInfo &)));
        connect(monitor, SIGNAL(si_updateProducers()), SLOT(sl_workerStatsUpdate()));
        connect(monitor, SIGNAL(si_newOutputFile(const Monitor::FileInfo &)), SLOT(sl_newOutputFile(const Monitor::FileInfo &)));
//        connect(monitor, SIGNAL(si_logChanged(Monitor::LogEntry)), SLOT(sl_onLogChanged(Monitor::LogEntry)));

        for (const WorkflowNotification &notification : monitor->getNotifications()) {
            sl_newNotification(notification, 0);    // TODO: fix count of notifications
        }
    }
}

void DashboardPageController::loadPage(const QString &pageUrl) {
    webViewController->loadPage(pageUrl);
}

void DashboardPageController::savePage(const QString &pageUrl) {
    webViewController->savePage(pageUrl);
}

void DashboardPageController::runJavaScript(const QString &script) {
    webViewController->runJavaScript(script);
}

DashboardJsAgent *DashboardPageController::getAgent() {
    return qobject_cast<DashboardJsAgent *>(agent);
}

void DashboardPageController::sl_pageIsAboutToBeInitialized() {
    if (monitor != nullptr) {
        runJavaScript("setNeedCreateWidgets(true);");
    }
}

void DashboardPageController::sl_pageInitialized() {
    pageReady = true;
    initData();
    emit si_pageReady();
}

void DashboardPageController::sl_progressChanged(int newProgress) {
    progress = newProgress;
    if (pageReady) {
        emit getAgent()->si_progressChanged(progress);
    }
}

namespace {

QString state2String(Monitor::TaskState state) {
    switch (state) {
    case Monitor::RUNNING:
        return Dashboard::STATE_RUNNING;
    case Monitor::RUNNING_WITH_PROBLEMS:
        return Dashboard::STATE_RUNNING_WITH_PROBLEMS;
    case Monitor::FINISHED_WITH_PROBLEMS:
        return Dashboard::STATE_FINISHED_WITH_PROBLEMS;
    case Monitor::FAILED:
        return Dashboard::STATE_FAILED;
    case Monitor::SUCCESS:
        return Dashboard::STATE_SUCCESS;
    default:
        return Dashboard::STATE_CANCELED;
    }
}

}    // namespace

void DashboardPageController::sl_taskStateChanged(Monitor::TaskState newState) {
    state = state2String(newState);
    if (pageReady) {
        emit getAgent()->si_taskStateChanged(state);
    }
}

void DashboardPageController::sl_newNotification(const WorkflowNotification &notification, int count) {
    const QString serializedNotification = serializeNotification(notification, count);
    if (pageReady) {
        emit getAgent()->si_newProblem(serializedNotification);
    } else {
        problems << serializedNotification;
    }
}

void DashboardPageController::sl_workerInfoChanged(const QString &actorId, const Monitor::WorkerInfo &info) {
    const QString serializedInfo = serializeWorkerInfo(actorId, info);
    if (pageReady) {
        emit getAgent()->si_workerStatsInfoChanged(serializedInfo);
    } else {
        workerInfos << serializedInfo;
    }
}

void DashboardPageController::sl_workerStatsUpdate() {
    SAFE_POINT(NULL != monitor, "WorkflowMonitor is NULL", );
    const QString serializedStatistics = serializeWorkerStatistics(monitor->getWorkersInfo());
    if (pageReady) {
        emit getAgent()->si_workerStatsUpdate(serializedStatistics);
    } else {
        workersStatisticsInfos << serializedStatistics;
    }
}

void DashboardPageController::sl_onLogChanged(const Monitor::LogEntry &entry) {
    const QString serializedLogEntry = serializeLogEntry(entry);
    if (pageReady) {
        emit getAgent()->si_onLogChanged(serializedLogEntry);
    } else {
        logEntries << serializedLogEntry;
    }
}

void DashboardPageController::sl_newOutputFile(const Monitor::FileInfo &info) {
    const QString serializedFileInfo = serializeFileInfo(info);
    if (pageReady) {
        emit getAgent()->si_newOutputFile(serializedFileInfo);
    } else {
        fileInfos << serializedFileInfo;
    }
}

void DashboardPageController::initData() {
    emit getAgent()->si_progressChanged(progress);
    emit getAgent()->si_taskStateChanged(state);

    for (const QString &problem : problems) {
        emit getAgent()->si_newProblem(problem);
    }

    for (const QString &workerInfo : workerInfos) {
        emit getAgent()->si_workerStatsInfoChanged(workerInfo);
    }

    for (const QString &workersStatisticsInfo : workersStatisticsInfos) {
        emit getAgent()->si_workerStatsUpdate(workersStatisticsInfo);
    }

    for (const QString &fileInfo : fileInfos) {
        emit getAgent()->si_newOutputFile(fileInfo);
    }

    for (const QString &entry : logEntries) {
        emit getAgent()->si_onLogChanged(entry);
    }
}

QString DashboardPageController::serializeNotification(const WorkflowNotification &problem, int count) const {
    SAFE_POINT(NULL != monitor, "WorkflowMonitor is NULL", "");
    QJsonObject infoJS;
    infoJS["actorId"] = problem.actorId;
    infoJS["actorName"] = monitor->actorName(problem.actorId);
    infoJS["type"] = problem.type;
    infoJS["message"] = problem.message;
    infoJS["count"] = count;
    const QJsonDocument doc(infoJS);
    return QString(doc.toJson(QJsonDocument::Compact));
}

QString DashboardPageController::serializeWorkerInfo(const QString &actorId, const Monitor::WorkerInfo &info) const {
    SAFE_POINT(NULL != monitor, "WorkflowMonitor is NULL", "");
    QJsonObject infoJS;
    infoJS["actorId"] = actorId;
    infoJS["actor"] = monitor->actorName(actorId);
    infoJS["timeMks"] = info.timeMks;
    infoJS["countOfProducedData"] = monitor->getDataProduced(actorId);
    QJsonDocument doc(infoJS);
    return QString(doc.toJson(QJsonDocument::Compact));
}

QString DashboardPageController::serializeWorkerStatistics(const QMap<QString, Monitor::WorkerInfo> &workersStatistics) const {
    SAFE_POINT(NULL != monitor, "WorkflowMonitor is NULL", "");
    QJsonArray workersStatisticsInfo;
    foreach (const QString &actorId, workersStatistics.keys()) {
        Monitor::WorkerInfo info = workersStatistics[actorId];
        QJsonObject infoJS;
        infoJS["actorId"] = actorId;
        infoJS["actor"] = monitor->actorName(actorId);
        infoJS["timeMks"] = info.timeMks;
        infoJS["countOfProducedData"] = monitor->getDataProduced(actorId);
        workersStatisticsInfo.append(infoJS);
    }

    QJsonDocument doc(workersStatisticsInfo);
    return QString(doc.toJson(QJsonDocument::Compact));
}

QString DashboardPageController::serializeLogEntry(const Monitor::LogEntry &entry) const {
    QJsonObject entryJS;
    entryJS["toolName"] = entry.toolName;
    entryJS["actorId"] = entry.actorId;
    entryJS["actorName"] = entry.actorName;
    entryJS["actorRunNumber"] = entry.actorRunNumber;
    entryJS["toolRunNumber"] = entry.toolRunNumber;
    entryJS["contentType"] = entry.contentType;
    entryJS["lastLine"] = entry.lastLine;
    QJsonDocument doc(entryJS);
    return QString(doc.toJson(QJsonDocument::Compact));
}

QString DashboardPageController::serializeFileInfo(const Monitor::FileInfo &info) const {
    SAFE_POINT(NULL != monitor, "WorkflowMonitor is NULL", "");
    QJsonObject infoJS;
    infoJS["actor"] = monitor->actorName(info.actor);
    infoJS["url"] = info.url;
    infoJS["openBySystem"] = info.openBySystem;
    infoJS["isDir"] = info.isDir;
    QJsonDocument doc(infoJS);
    return QString(doc.toJson(QJsonDocument::Compact));
}

}    // namespace U2
