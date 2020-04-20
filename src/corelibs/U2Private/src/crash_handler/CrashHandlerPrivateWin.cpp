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
#ifdef Q_OS_WIN

#include <QDir>

#include <client/windows/handler/exception_handler.h>

#include <U2Core/AppContext.h>
#include <U2Core/U2SafePoints.h>

#include "CrashHandler.h"
#include "CrashHandlerArgsHelper.h"
#include "CrashHandlerPrivateWin.h"

#if defined(Q_OS_WIN32)
#ifdef UGENE_X86_64 //see http://social.msdn.microsoft.com/Forums/en-US/vcgeneral/thread/4dc15026-884c-4f8a-8435-09d0111d708d/
extern "C"
{
void rollbackStack();
}
#endif
#endif

namespace U2 {

CrashHandlerPrivateWin::CrashHandlerPrivateWin()
    : CrashHandlerPrivate(),
      crashDirWasSucessfullyCreated(false),
      dumpWasSuccessfullySaved(false)
{

}

CrashHandlerPrivateWin::~CrashHandlerPrivateWin()  {
    shutdown();
}

void CrashHandlerPrivateWin::setupHandler() {
#ifndef _DEBUG
    const QString dumpDir = QDir::tempPath() + "/ugene_crashes";
    if (!QDir().exists(dumpDir)) {
        crashDirWasSucessfullyCreated = QDir().mkpath(dumpDir);
    }

    breakpadHandler = new google_breakpad::ExceptionHandler(dumpDir.toStdWString(), NULL, breakpadCallback, this, google_breakpad::ExceptionHandler::HANDLER_ALL);
#endif
}

void CrashHandlerPrivateWin::shutdown() {
    delete breakpadHandler;
    breakpadHandler = NULL;
}

QString CrashHandlerPrivateWin::getStackTrace() const {
    return st.getBuffer();
}

QString CrashHandlerPrivateWin::getAdditionalInfo() const {
    QString info;

    if (!crashDirWasSucessfullyCreated) {
        info += "Dir for storing crash dumps creation failed on the breakpad initialization\n";
    }

    if (!dumpWasSuccessfullySaved) {
        info += "Crash dump file saving failed on the crash handling\n";
    }

    return info;
}

bool CrashHandlerPrivateWin::breakpadCallback(const wchar_t *dump_path,
                                              const wchar_t *minidump_id,
                                              void *context,
                                              EXCEPTION_POINTERS *exinfo,
                                              MDRawAssertionInfo * /*assertion*/,
                                              bool succeeded) {
    QString dumpPath;
    if (succeeded) {
        dumpPath = QString::fromStdWString(dump_path) + "/" + QString::fromStdWString(minidump_id) + ".dmp";
    }

    CrashHandlerPrivateWin *privateHandler = static_cast<CrashHandlerPrivateWin *>(context);
#ifdef Q_OS_WIN64
    privateHandler->walkStack(exinfo);
#endif
    privateHandler->dumpWasSuccessfullySaved = succeeded;

    handleException(privateHandler->getExceptionText(exinfo), dumpPath);
    return true;
}

void CrashHandlerPrivateWin::walkStack(EXCEPTION_POINTERS *exinfo) {
    if (exinfo->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW) {
#if defined(Q_OS_WIN32)
#ifdef UGENE_X86 //see http://social.msdn.microsoft.com/Forums/en-US/vcgeneral/thread/4dc15026-884c-4f8a-8435-09d0111d708d/
        _asm add esp, 10240; //roll back stack and current frame pointer
#else
        rollbackStack();//TODO:need hack for x86_64
#endif
#endif
    }
    st.ShowCallstack(OpenThread(READ_CONTROL ,false , breakpadHandler->get_requesting_thread_id()), exinfo->ContextRecord);
}

QString CrashHandlerPrivateWin::getExceptionText(EXCEPTION_POINTERS *exinfo) {
    QString exceptionText = "Unhandled exception";
    CHECK(NULL != exinfo, "C++ exception|" + exceptionText);

    switch(exinfo->ExceptionRecord->ExceptionCode) {
    case EXCEPTION_ACCESS_VIOLATION:
        exceptionText = "Access violation";
        break;
    case EXCEPTION_DATATYPE_MISALIGNMENT:
        exceptionText = "Data type misalignment";
        break;
    case EXCEPTION_BREAKPOINT:
        exceptionText = "Break point";
        break;
    case EXCEPTION_SINGLE_STEP:
        exceptionText = "Single step";
        break;
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        exceptionText = "Array bounds exceeded";
        break;
    case EXCEPTION_FLT_DENORMAL_OPERAND:
        exceptionText = "Float denormal operand";
        break;
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        exceptionText = "Float divide by zero";
        break;
    case EXCEPTION_FLT_INEXACT_RESULT:
        exceptionText = "Float inexact result";
        break;
    case EXCEPTION_FLT_INVALID_OPERATION:
        exceptionText = "Float invalid operation";
        break;
    case EXCEPTION_FLT_OVERFLOW:
        exceptionText = "Float overflow";
        break;
    case EXCEPTION_FLT_STACK_CHECK:
        exceptionText = "Float check stack";
        break;
    case EXCEPTION_FLT_UNDERFLOW:
        exceptionText = "Float underflow";
        break;
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
        exceptionText = "Int divide by zero";
        break;
    case EXCEPTION_INT_OVERFLOW:
        exceptionText = "Int overflow";
        break;
    case EXCEPTION_PRIV_INSTRUCTION:
        exceptionText = "Privileged instruction";
        break;
    case EXCEPTION_IN_PAGE_ERROR:
        exceptionText = "In page error";
        break;
    case EXCEPTION_ILLEGAL_INSTRUCTION:
        exceptionText = "Illegal instruction";
        break;
    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
        exceptionText = "Noncontinuable exception";
        break;
    case EXCEPTION_STACK_OVERFLOW:
        exceptionText = "Stack overflow";
        break;
    case EXCEPTION_INVALID_DISPOSITION:
        exceptionText = "Invalid disposition";
        break;
    case EXCEPTION_GUARD_PAGE:
        exceptionText = "Guard page violation";
        break;
    case EXCEPTION_INVALID_HANDLE:
        exceptionText = "Invalid handle";
        break;
    case CONTROL_C_EXIT:
        exceptionText = "Control C exit";
        break;
    default:
        ;   // Do nothing
    }

    return "C++ exception|" + exceptionText;
}

}   // namespace U2

#endif
