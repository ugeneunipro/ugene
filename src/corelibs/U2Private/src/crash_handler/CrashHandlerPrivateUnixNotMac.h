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

#ifndef _U2_CRASH_HANDLER_PRIVATE_UNIX_NOT_MAC_H_
#define _U2_CRASH_HANDLER_PRIVATE_UNIX_NOT_MAC_H_

#include <qglobal.h>
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)

#include <client/linux/handler/exception_handler.h>

#include "CrashHandlerPrivate.h"

namespace U2 {

class CrashHandlerPrivateUnixNotMac : public CrashHandlerPrivate {
public:
    CrashHandlerPrivateUnixNotMac();
    ~CrashHandlerPrivateUnixNotMac();

    void setupHandler();
    void shutdown();

    void storeStackTrace();
    QString getAdditionalInfo() const;

private:
    static bool breakpadCallback(const google_breakpad::MinidumpDescriptor &descriptor,
                                 void *context,
                                 bool succeeded);
    static bool crashContextCallback(const void *crash_context,
                                     size_t crash_context_size,
                                     void *context);
    static QString getExceptionText(const google_breakpad::ExceptionHandler::CrashContext *crashContext);

    QString lastExceptionText;
    bool stacktraceFileWasSucessfullyRemoved;
    bool stacktraceFileSucessfullyCreated;
    bool stacktraceFileWasSucessfullyClosed;
    bool crashDirWasSucessfullyCreated;
    bool dumpWasSuccessfullySaved;

    static const QString STACKTRACE_FILE_PATH;
};

}   // namespace U2

#endif

#endif // _U2_CRASH_HANDLER_PRIVATE_UNIX_NOT_MAC_H_
