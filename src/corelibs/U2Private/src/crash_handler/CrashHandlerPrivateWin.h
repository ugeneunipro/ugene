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

#ifndef _U2_CRASH_HANDLER_PRIVATE_WIN_H_
#define _U2_CRASH_HANDLER_PRIVATE_WIN_H_

#include <qglobal.h>
#ifdef Q_OS_WIN

#include <windows.h>

#include <google_breakpad/common/minidump_format.h>

#include "CrashHandlerPrivate.h"
#include "StackWalker.h"

namespace U2 {

class CrashHandlerPrivateWin : public CrashHandlerPrivate {
public:
    CrashHandlerPrivateWin();
    ~CrashHandlerPrivateWin();

    void setupHandler();
    void shutdown();

    QString getStackTrace() const;
    QString getAdditionalInfo() const;

private:
    static bool breakpadCallback(const wchar_t *dump_path,
                                 const wchar_t *minidump_id,
                                 void *context,
                                 EXCEPTION_POINTERS *exinfo,
                                 MDRawAssertionInfo *assertion,
                                 bool succeeded);

    void walkStack(EXCEPTION_POINTERS *exinfo);
    QString getExceptionText(EXCEPTION_POINTERS *exinfo);

    StackWalker st;
    bool crashDirWasSucessfullyCreated;
    bool dumpWasSuccessfullySaved;
};


}   // namespace U2

#endif

#endif // _U2_CRASH_HANDLER_PRIVATE_WIN_H_
