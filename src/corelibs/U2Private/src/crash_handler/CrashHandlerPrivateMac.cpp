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

#include <qglobal.h>
#ifdef Q_OS_MAC

#include <execinfo.h>
#include <stdio.h>

#include <QDir>

#include <client/mac/handler/exception_handler.h>

#include <U2Core/AppContext.h>

#include "CrashHandler.h"
#include "CrashHandlerArgsHelper.h"
#include "CrashHandlerPrivateMac.h"

namespace U2 {

const QString CrashHandlerPrivateMac::LEGACY_STACKTRACE_FILE_PATH = "/tmp/UGENEstacktrace.txt";

CrashHandlerPrivateMac::CrashHandlerPrivateMac() :
    CrashHandlerPrivate(),
    legacyStacktraceFileWasSucessfullyRemoved(true),
    stacktraceFileWasSucessfullyRemoved(true),
    stacktraceFileSucessfullyCreated(true),
    stacktraceFileWasSucessfullyClosed(true),
    crashDirWasSucessfullyCreated(true),
    dumpWasSuccessfullySaved(true)
{

}

CrashHandlerPrivateMac::~CrashHandlerPrivateMac()  {
    shutdown();
}

void CrashHandlerPrivateMac::setupHandler() {
#ifndef _DEBUG      // debugger fails to launch if exception handler is installed
    if (QFile::exists(LEGACY_STACKTRACE_FILE_PATH)) {
        legacyStacktraceFileWasSucessfullyRemoved = QFile(LEGACY_STACKTRACE_FILE_PATH).remove();
    }

    const QString dumpDir = QDir::tempPath() + "/ugene_crashes";
    if (!QDir().exists(dumpDir)) {
        crashDirWasSucessfullyCreated = QDir().mkpath(dumpDir);
    }

    stacktraceFilePath = dumpDir + "/UGENEstacktrace.txt";
    if (QFile::exists(stacktraceFilePath)) {
        stacktraceFileWasSucessfullyRemoved = QFile(stacktraceFilePath).remove();
    }

    breakpadHandler = new google_breakpad::ExceptionHandler(dumpDir.toStdString(), NULL, breakpadCallback, this, true, NULL);
#endif
}

void CrashHandlerPrivateMac::shutdown() {
    delete breakpadHandler;
    breakpadHandler = NULL;
}

void CrashHandlerPrivateMac::storeStackTrace() {
    const QString path = AppContext::getWorkingDirectoryPath() + "/ugenem";

    char pid_buf[30];
    sprintf(pid_buf, "%d", getpid());
    char name_buf[512];
    name_buf[readlink(path.toLatin1().constData(), name_buf, 511)] = 0;
    FILE *fp = fopen(stacktraceFilePath.toLocal8Bit().constData(), "w+");
    stacktraceFileSucessfullyCreated = (NULL != fp);
    void * stackTrace[1024];
    int frames = backtrace(stackTrace, 1024);
    backtrace_symbols_fd(stackTrace, frames, fileno(fp));
    const int closed = fclose(fp);
    stacktraceFileWasSucessfullyClosed = (closed == 0);
}

QString CrashHandlerPrivateMac::getAdditionalInfo() const {
    QString info;

    if (!legacyStacktraceFileWasSucessfullyRemoved) {
        info += "Legacy stacktrace file removing failed on the breakpad initialization\n";
    }

    if (!stacktraceFileWasSucessfullyRemoved) {
        info += "Stacktrace file removing failed on the breakpad initialization\n";
    }

    if (!crashDirWasSucessfullyCreated) {
        info += "Crash dumps storing dir creation failed on the breakpad initialization\n";
    }

    if (!stacktraceFileSucessfullyCreated) {
        info += "Stacktrace file creation failed on the crash handling\n";
    }

    if (!stacktraceFileWasSucessfullyClosed) {
        info += "Stacktrace file closing failed on the crash handling\n";
    }

    if (!dumpWasSuccessfullySaved) {
        info += "Crash dump file saving failed on the crash handling\n";
    }

    return info;
}

bool CrashHandlerPrivateMac::breakpadCallback(const char *dump_dir, const char *minidump_id, void *context, bool succeeded) {
    QString dumpUrl;
    if (succeeded) {
        dumpUrl = QString::fromLocal8Bit(dump_dir) + "/" + QString::fromLocal8Bit(minidump_id) + ".dmp";
    }

    CrashHandlerPrivateMac *privateHandler = static_cast<CrashHandlerPrivateMac *>(context);
    privateHandler->dumpWasSuccessfullySaved = succeeded;

    handleException("C++ exception|Unhandled exception", dumpUrl);

    return true;
}

}   // namespace U2

#endif
