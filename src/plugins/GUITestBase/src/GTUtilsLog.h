/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#pragma once

#include <QStringList>

#include <U2Core/Log.h>

#include "GTGlobals.h"

namespace U2 {

class GTLogTracer : public QObject, public LogListener {
    Q_OBJECT
public:
    GTLogTracer();
    ~GTLogTracer();

    void onMessage(const LogMessage& msg) override;

    bool hasErrors() const;

    QString getJoinedErrorString() const;

    /** Checks if there was any log message with the given substring (case insensitive). */
    bool hasMessage(const QString& substring) const;

    /** Checks if there was error log message with the given substring (case insensitive). */
    bool hasError(const QString& substring) const;

    /** Clears all cached messages. */
    void clear();

    QStringList allMessages;
    QStringList errorMessages;
};

class GTUtilsLog {
public:
    /** Waits for the message to appear in the log with the given timeout. */
    static void checkMessageWithWait(HI::GUITestOpStatus& os, const GTLogTracer& logTracer, const QString& message, int timeoutMillis = 30000);

    /**
     * Checks that there are exactly 'expectedMessageCount' in the log with 'messagePart' text token inside.
     * 'context' is a message marker added to the failure string. Used to identify individual method calls.
     */
    static void checkMessageWithTextCount(HI::GUITestOpStatus& os, const QString& messagePart, int expectedMessageCount, const QString& context = "");
};

}  // namespace U2
