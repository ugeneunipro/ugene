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

#include <QTextStream>

#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>
#include <U2Core/Timer.h>
#include <U2Core/Version.h>

#include "CrashHandler.h"
#include "CrashHandlerArgsHelper.h"
#include "CrashHandlerPrivate.h"
#include "CrashLogCache.h"
#include "TaskSchedulerImpl.h"

#if defined(Q_OS_MAC)
#include "CrashHandlerPrivateMac.h"
#elif defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
#include "CrashHandlerPrivateUnixNotMac.h"
#elif defined(Q_OS_WIN)
#include "CrashHandlerPrivateWin.h"
#endif

const static char * SETTINGS_UGENE_UID            = "shtirlitz/uid";

namespace U2 {

CrashHandlerPrivate * CrashHandler::crashHandlerPrivate = NULL;
char* CrashHandler::buffer = NULL;
LogCache* CrashHandler::crashLogCache = NULL;
bool CrashHandler::sendCrashReports = true;

bool CrashHandler::isEnabled() {
#ifdef _DEBUG
    return false;
#endif

    static QString useCrashHandlerValue = qgetenv(ENV_USE_CRASH_HANDLER);

    if ("0" == useCrashHandlerValue) {
        return false;
    }

    if ("1" == useCrashHandlerValue) {
        return true;
    }

    return true;
}

bool CrashHandler::getSendCrashReports() {
    return sendCrashReports;
}

void CrashHandler::setSendCrashReports(bool sendReports) {
    sendCrashReports = sendReports;
}

void CrashHandler::handleException(const QString &exceptionType, const QString &dumpUrl) {
    if (!sendCrashReports) {
        QTextStream out(stderr);
        out << "Unrecognized error";
        out.flush();
        return;
    }

    if (NULL != crashHandlerPrivate) {
        crashHandlerPrivate->storeStackTrace();
    }

    CrashHandlerArgsHelper helper;
    helper.setReportData(generateReport(exceptionType, helper.getMaxReportSize()));
    helper.setDumpUrl(dumpUrl);

    runMonitorProcess(helper);
}

void CrashHandler::preallocateReservedSpace() {
    assert(buffer == NULL);
    buffer = new char[10 * 1024 * 1024];
}

void CrashHandler::releaseReserve() {
    delete[] buffer;
    buffer = NULL;
}

void CrashHandler::setupLogCache() {
    assert(crashLogCache == NULL);
    crashLogCache = new CrashLogCache();
    crashLogCache->filter.filters.append(LogFilterItem(ULOG_CAT_TASKS, LogLevel_TRACE));
    crashLogCache->filter.filters.append(LogFilterItem(ULOG_CAT_CORE_SERVICES, LogLevel_TRACE));
    crashLogCache->filter.filters.append(LogFilterItem(ULOG_CAT_IO, LogLevel_TRACE));
    crashLogCache->filter.filters.append(LogFilterItem(ULOG_CAT_USER_INTERFACE, LogLevel_TRACE));
    crashLogCache->filter.filters.append(LogFilterItem(ULOG_CAT_ALGORITHM, LogLevel_TRACE));
    crashLogCache->filter.filters.append(LogFilterItem(ULOG_CAT_CONSOLE, LogLevel_ERROR));
    crashLogCache->filter.filters.append(LogFilterItem(ULOG_CAT_CORE_SERVICES, LogLevel_DETAILS));
    crashLogCache->filter.filters.append(LogFilterItem(ULOG_CAT_TASKS, LogLevel_DETAILS));
    crashLogCache->filter.filters.append(LogFilterItem(ULOG_CAT_USER_ACTIONS, LogLevel_TRACE));
}

void CrashHandler::setupPrivateHandler() {
    assert(crashHandlerPrivate == NULL);
#if defined(Q_OS_MAC)
    crashHandlerPrivate = new CrashHandlerPrivateMac;
#elif defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
    crashHandlerPrivate = new CrashHandlerPrivateUnixNotMac;
#elif defined(Q_OS_WIN)
    crashHandlerPrivate = new CrashHandlerPrivateWin;
#endif
    SAFE_POINT(NULL != crashHandlerPrivate, "Crash handler was not initialized: an unrecognized OS", );
    crashHandlerPrivate->setupHandler();
}

QString CrashHandler::generateReport(const QString &exceptionType, int maxReportSize) {
    QString reportText = exceptionType + "|";

    reportText += Version::appVersion().text;
    reportText += "|";

    Settings* settings = AppContext::getSettings();
    if (settings != NULL){
        QVariant uuidQvar = settings->getValue( SETTINGS_UGENE_UID, "None" );
        reportText += uuidQvar.toString() + "|";
    } else {
        reportText += "Settings is null|";
    }

    QString activeWindow = AppContext::getActiveWindowName();
    if(activeWindow.isEmpty()) {
        reportText += "None|";
    } else {
        reportText += activeWindow + "|";
    }

    const QString handlerAdditionalInfo = (crashHandlerPrivate == NULL ? "" : crashHandlerPrivate->getAdditionalInfo());
    reportText += (handlerAdditionalInfo.isEmpty() ? "None" : handlerAdditionalInfo) + "|";

    QList<LogMessage*> logMessages = crashLogCache == NULL ? QList<LogMessage*>() : crashLogCache->messages;
    QString messageLog;
    if (!logMessages.isEmpty()) {
        QList<LogMessage*>::iterator it;
        int i;
        for(i = 0, it = --logMessages.end(); i <= maxReportSize && it!= logMessages.begin(); i++, it--) {
            LogMessage* msg = *it;
            messageLog.prepend("[" + GTimer::createDateTime(msg->time).toString("hh:mm:ss.zzz") + "] " + "[" + msg->categories.first() + "] " + msg->text + "\n");
        }
    } else {
        messageLog += "None";
    }

    AppResourcePool* pool = AppResourcePool::instance();
    if (pool) {
        size_t memoryBytes = pool->getCurrentAppMemory();
        QString memInfo = QString("AppMemory: %1Mb; ").arg(memoryBytes/(1000*1000));

        reportText += (memInfo + "\n");
    }
    reportText += messageLog + " | ";

    QString taskList;
    TaskScheduler *ts = AppContext::getTaskScheduler();
    QList<Task* > topTasks = ts != NULL ? ts->getTopLevelTasks() : QList<Task*>();
    foreach(Task *t, topTasks) {
        if(t->getState() != Task::State_Finished) {
            QString state;
            if (t->getState() == Task::State_Running) {
                state = "(Running)";
            } else if(t->getState() == Task::State_New) {
                state = "(New)";
            } else if(t->getState() == Task::State_Prepared) {
                state = "(Preparing)";
            }
            QString progress = QString::number(t->getStateInfo().progress);
            taskList.append(t->getTaskName() + "\t" + state + "\t" + progress  + "\n");
            foreach(const QPointer<Task> &tt, t->getSubtasks()) {
                getSubTasks(tt.data(), taskList, 1);
            }
        }
    }
    reportText += taskList;
    if (taskList.isEmpty()) {
        reportText += "None";
    }

    const QString stackTrace = (NULL != crashHandlerPrivate ? crashHandlerPrivate->getStackTrace() : "");
    reportText += "|" + (stackTrace.isEmpty() ? "None" : stackTrace);

    return reportText;
}

void CrashHandler::setupHandler() {
    preallocateReservedSpace();
    setupLogCache();
    setupPrivateHandler();
}

void CrashHandler::shutdown() {
    releaseReserve();
    delete crashHandlerPrivate;
}

void CrashHandler::runMonitorProcess(const CrashHandlerArgsHelper &helper) {
#ifndef Q_OS_WIN
    const QString path = AppContext::getWorkingDirectoryPath() + "/ugenem";
#else
    const QString path = AppContext::getWorkingDirectoryPath() + "/ugenem.exe";
#endif

    static QMutex mutex;
    QMutexLocker lock(&mutex);
    QProcess::startDetached(path, helper.getArguments());
}

void CrashHandler::getSubTasks(Task *t, QString& list, int lvl) {
    if(t->getState() != Task::State_Finished) {
        QString prefix;
        QString state;
        prefix.fill('-', lvl);
        if(t->getState() == Task::State_Running) {
            state = "(Running)";
        } else if(t->getState() == Task::State_New) {
            state = "(New)";
        } else if(t->getState() == Task::State_Prepared) {
            state = "(Preparing)";
        }
        QString progress = QString::number(t->getStateInfo().progress);
        list.append(prefix + t->getTaskName() + "\t" + state + "\t" + progress  + "\n");
        foreach(const QPointer<Task> &tt, t->getSubtasks()) {
            getSubTasks(tt.data(), list, lvl + 1);
        }
    }
}

}
