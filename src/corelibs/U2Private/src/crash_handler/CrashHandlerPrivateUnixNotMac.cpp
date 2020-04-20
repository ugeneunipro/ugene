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
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)

#include <execinfo.h>

#include <QDir>

#include <U2Core/AppContext.h>
#include <U2Core/U2SafePoints.h>

#include "CrashHandler.h"
#include "CrashHandlerPrivateUnixNotMac.h"

namespace U2 {

const QString CrashHandlerPrivateUnixNotMac::STACKTRACE_FILE_PATH = "/tmp/UGENEstacktrace.txt";

CrashHandlerPrivateUnixNotMac::CrashHandlerPrivateUnixNotMac() :
    CrashHandlerPrivate(),
    stacktraceFileWasSucessfullyRemoved(true),
    stacktraceFileSucessfullyCreated(true),
    stacktraceFileWasSucessfullyClosed(true),
    crashDirWasSucessfullyCreated(true),
    dumpWasSuccessfullySaved(true)
{

}

CrashHandlerPrivateUnixNotMac::~CrashHandlerPrivateUnixNotMac()  {
    shutdown();
}

void CrashHandlerPrivateUnixNotMac::setupHandler() {
#ifndef _DEBUG
    if (QFile::exists(STACKTRACE_FILE_PATH)) {
        stacktraceFileWasSucessfullyRemoved = QFile(STACKTRACE_FILE_PATH).remove();
    }

    const QString dumpDir = QDir::tempPath() + "/ugene_crashes";
    if (!QDir().exists(dumpDir)) {
        crashDirWasSucessfullyCreated = QDir().mkpath(dumpDir);
    }

    const google_breakpad::MinidumpDescriptor destDirDescriptor(dumpDir.toStdString());
    breakpadHandler = new google_breakpad::ExceptionHandler(destDirDescriptor, NULL, breakpadCallback, this, true, -1);

    breakpadHandler->set_crash_handler(crashContextCallback);
#endif
}

void CrashHandlerPrivateUnixNotMac::shutdown() {
    delete breakpadHandler;
    breakpadHandler = NULL;
}

void CrashHandlerPrivateUnixNotMac::storeStackTrace() {
    const QString path = AppContext::getWorkingDirectoryPath() + "/ugenem";

    char pid_buf[30];
    sprintf(pid_buf, "%d", getpid());
    char name_buf[512];
    name_buf[readlink(path.toLatin1().data(), name_buf, 511)]=0;
    FILE *fp = fopen(STACKTRACE_FILE_PATH.toLocal8Bit().constData(), "w+");
    stacktraceFileSucessfullyCreated = (NULL != fp);
    void * stackTrace[1024];
    int frames = backtrace(stackTrace, 1024);
    backtrace_symbols_fd(stackTrace, frames, fileno(fp));
    const int closed = fclose(fp);
    stacktraceFileWasSucessfullyClosed = (closed == 0);
}

QString CrashHandlerPrivateUnixNotMac::getAdditionalInfo() const {
    QString info;

    if (!stacktraceFileWasSucessfullyRemoved) {
        info += "Stacktrace file removing failed on the breakpad initialization\n";
    }

    if (!crashDirWasSucessfullyCreated) {
        info += "Dir for storing crash dumps creation failed on the breakpad initialization\n";
    }

    if (!stacktraceFileSucessfullyCreated) {
        info += "Stacktrace file creating failed on the crash handling\n";
    }

    if (!stacktraceFileWasSucessfullyClosed) {
        info += "Stacktrace file closing failed on the crash handling\n";
    }

    if (!dumpWasSuccessfullySaved) {
        info += "Crash dump file saving failed on the crash handling\n";
    }

    return info;
}

bool CrashHandlerPrivateUnixNotMac::breakpadCallback(const google_breakpad::MinidumpDescriptor &descriptor,
                                                     void *context,
                                                     bool succeeded) {
    QString dumpPath;
    if (succeeded) {
        dumpPath = QString::fromLocal8Bit(descriptor.path());
    }

    CrashHandlerPrivateUnixNotMac *privateHandler = static_cast<CrashHandlerPrivateUnixNotMac *>(context);
    privateHandler->dumpWasSuccessfullySaved = succeeded;

    handleException(privateHandler->lastExceptionText, dumpPath);

    return true;
}

bool CrashHandlerPrivateUnixNotMac::crashContextCallback(const void *crash_context,
                                                        size_t /*crash_context_size*/,
                                                        void *context) {
    CrashHandlerPrivateUnixNotMac *privateHandler = static_cast<CrashHandlerPrivateUnixNotMac *>(context);
    const google_breakpad::ExceptionHandler::CrashContext *crashContext = static_cast<const google_breakpad::ExceptionHandler::CrashContext *>(crash_context);
    privateHandler->lastExceptionText = getExceptionText(crashContext);
    return false;
}

QString CrashHandlerPrivateUnixNotMac::getExceptionText(const google_breakpad::ExceptionHandler::CrashContext *crashContext) {
    QString exceptionText = "Unhandled exception";
    CHECK(NULL != crashContext, "C++ exception|" + exceptionText);

    switch(crashContext->siginfo.si_signo) {
    case SIGBUS:
        exceptionText = "Access to undefined portion of memory object";
        switch (crashContext->siginfo.si_code) {
        case BUS_ADRALN:
            exceptionText += ": invalid address alignment.";
            break;
        case BUS_ADRERR:
            exceptionText += ": non-existent physical address.";
            break;
        case BUS_OBJERR:
            exceptionText += ": object-specific hardware error.";
            break;
        }
        break;

    case SIGFPE:
        exceptionText = "Erroneous arithmetic operation";
        switch (crashContext->siginfo.si_code) {
        case FPE_INTDIV:
            exceptionText += ": integer divide-by-zero.";
            break;
        case FPE_INTOVF:
            exceptionText += ": integer overflow.";
            break;
        case FPE_FLTDIV:
            exceptionText += ": floating point divide-by-zero.";
            break;
        case FPE_FLTOVF:
            exceptionText += ": floating point overflow.";
            break;
        case FPE_FLTUND:
            exceptionText += ": floating point underflow.";
            break;
        case FPE_FLTRES:
            exceptionText += ": floating point inexact result.";
            break;
        case FPE_FLTINV:
            exceptionText += ": invalid floating point operation.";
            break;
        case FPE_FLTSUB:
            exceptionText += ": subscript out of range.";
            break;
        }
        break;

    case SIGILL:
        exceptionText = "Illegal instruction";
        switch (crashContext->siginfo.si_code) {
        case ILL_ILLOPC:
            exceptionText += ": illegal opcode.";
            break;
        case ILL_ILLOPN:
            exceptionText += ": illegal operand.";
            break;
        case ILL_ILLADR:
            exceptionText += ": illegal addressing mode.";
            break;
        case ILL_ILLTRP:
            exceptionText += ": illegal trap.";
            break;
        case ILL_PRVOPC:
            exceptionText += ": privileged opcode.";
            break;
        case ILL_PRVREG:
            exceptionText += ": privileged register.";
            break;
        case ILL_COPROC:
            exceptionText += ": coprocessor error.";
            break;
        case ILL_BADSTK:
            exceptionText += ": internal stack error.";
            break;
        }
        break;

    case SIGSEGV:
        exceptionText = "Segmentation fault";
        switch (crashContext->siginfo.si_code) {
        case SEGV_MAPERR:
            exceptionText += ": address not mapped.";
            break;
        case SEGV_ACCERR:
            exceptionText += ": invalid permissions.";
            break;
        }
        break;

    case SIGSYS:
        exceptionText = "Bad syscall";
        break;

    case SIGXCPU:
        exceptionText = "CPU time limit exceeded";
        break;

    case SIGXFSZ:
        exceptionText = "File size limit exceeded";
        break;

    case SIGABRT:
        exceptionText = "Program has been aborted";
        break;

    default:
        ; // Do nothing
    }

    return "C++ exception|" + exceptionText;
}

}   // namespace U2

#endif
