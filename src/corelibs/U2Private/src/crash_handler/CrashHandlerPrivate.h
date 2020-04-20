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

#ifndef _U2_CRASH_HANDLER_PRIVATE_H_
#define _U2_CRASH_HANDLER_PRIVATE_H_

#include <QString>

namespace google_breakpad {
class ExceptionHandler;
}

namespace U2 {

class CrashHandlerPrivate {
public:
    CrashHandlerPrivate();
    virtual ~CrashHandlerPrivate();

    virtual void setupHandler() = 0;
    virtual void shutdown() = 0;

    virtual void storeStackTrace();
    virtual QString getStackTrace() const;
    virtual QString getAdditionalInfo() const;

protected:
    static void handleException(const QString &exceptionType, const QString &dumpUrl);

    google_breakpad::ExceptionHandler *breakpadHandler;
};

}   // namespace U2

#endif // _U2_CRASH_HANDLER_PRIVATE_H_
