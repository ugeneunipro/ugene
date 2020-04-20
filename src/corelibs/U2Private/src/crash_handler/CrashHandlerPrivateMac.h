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

#ifndef _U2_CRASH_HANDLER_PRIVATE_MAC_H_
#define _U2_CRASH_HANDLER_PRIVATE_MAC_H_

#include <qglobal.h>
#ifdef Q_OS_MAC

#include "CrashHandlerPrivate.h"

namespace U2 {


class CrashHandlerPrivateMac : public CrashHandlerPrivate {
public:
    CrashHandlerPrivateMac();
    ~CrashHandlerPrivateMac();

    void setupHandler();
    void shutdown();

    void storeStackTrace();
    QString getAdditionalInfo() const;

private:
    static bool breakpadCallback(const char *dump_dir,
                                 const char *minidump_id,
                                 void *context,
                                 bool succeeded);

    QString stacktraceFilePath;

    bool legacyStacktraceFileWasSucessfullyRemoved;
    bool stacktraceFileWasSucessfullyRemoved;
    bool stacktraceFileSucessfullyCreated;
    bool stacktraceFileWasSucessfullyClosed;
    bool crashDirWasSucessfullyCreated;
    bool dumpWasSuccessfullySaved;

    static const QString LEGACY_STACKTRACE_FILE_PATH;
};

}   // namespace U2

#endif

#endif // _U2_CRASH_HANDLER_PRIVATE_MAC_H_
