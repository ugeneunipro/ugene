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

#ifndef _U2_CRASH_HANDLER_H_
#define _U2_CRASH_HANDLER_H_

#include <QCoreApplication>
#include <QMutex>
#include <QProcess>
#include <QString>
#include <QStringList>

#include <U2Core/AppResources.h>
#include <U2Core/LogCache.h>
#include <U2Core/U2SqlHelpers.h>
#include <U2Core/global.h>

namespace google_breakpad {
class ExceptionHandler;
}

namespace U2 {

struct ExceptionInfo {
    QString errorType;
};

class CrashHandlerArgsHelper;
class CrashHandlerPrivate;
class LogCache;
class LogMessage;
class Task;

class U2PRIVATE_EXPORT CrashHandler {
    friend class CrashHandlerPrivate;

public:
    static void setupHandler();
    static void shutdown();

    static bool isEnabled();
    static bool getSendCrashReports();
    static void setSendCrashReports(bool sendReports);

    static void releaseReserve();
    static void handleException(const QString &exceptionType, const QString &dumpUrl = "");

private:
    static void preallocateReservedSpace();
    static void setupLogCache();
    static void setupPrivateHandler();

    static void getSubTasks(Task *t, QString & list, int lvl);
    static void runMonitorProcess(const CrashHandlerArgsHelper &helper);
    static QString generateReport(const QString &exceptionType, int maxReportSize);

    static CrashHandlerPrivate *crashHandlerPrivate;
    static char *               buffer;
    static LogCache *           crashLogCache;
    static bool                 sendCrashReports;
};

}   // namespace U2

#endif // _U2_CRASH_HANDLER_H_
