/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "GTUtilsLog.h"

#include "U2Core/LogCache.h"
#include <U2Core/U2SafePoints.h>

namespace U2 {
using namespace HI;

GTLogTracer::GTLogTracer(const QString& expectedMessage)
    : isExpectedMessageFound(false), expectedMessage(expectedMessage) {
    LogServer::getInstance()->addListener(this);
}

GTLogTracer::~GTLogTracer() {
    LogServer::getInstance()->removeListener(this);
}

void GTLogTracer::onMessage(const LogMessage& msg) {
    if (msg.level == LogLevel_ERROR) {
        errorsList << msg.text;
    }

    if (!expectedMessage.isEmpty() && !msg.text.contains("] GT_") && msg.text.contains(expectedMessage)) {
        isExpectedMessageFound = true;
    }
}

QList<LogMessage*> GTLogTracer::getMessages() {
    return LogCache::getAppGlobalInstance()->messages;
}

bool GTLogTracer::checkMessage(const QString& s) {
    QList<LogMessage*> messages = getMessages();
    for (LogMessage* message : qAsConst(messages)) {
        if (message->text.contains(s, Qt::CaseInsensitive)) {
            return true;
        }
    }
    return false;
}

#define GT_CLASS_NAME "GTUtilsLog"
#define GT_METHOD_NAME "check"
void GTUtilsLog::check(HI::GUITestOpStatus& os, const GTLogTracer& logTracer) {
    Q_UNUSED(os);
    GTGlobals::sleep(500);
    GT_CHECK(!logTracer.hasErrors(), "There are errors in log: " + logTracer.errorsList.join("\n"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkContainsError"
void GTUtilsLog::checkContainsError(HI::GUITestOpStatus& os, const GTLogTracer& logTracer, const QString& messagePart) {
    Q_UNUSED(os);
    GTGlobals::sleep(500);
    bool isErrorFound = false;
    for (QString error : qAsConst(logTracer.errorsList)) {
        if (error.contains(messagePart)) {
            isErrorFound = true;
            break;
        }
    }
    GT_CHECK(isErrorFound, "The log doesn't contain error message: " + messagePart);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkContainsMessage"
void GTUtilsLog::checkContainsMessage(HI::GUITestOpStatus& os, const GTLogTracer& logTracer, bool expected) {
    GT_CHECK(!logTracer.expectedMessage.isEmpty(), "'Expected message' is required by logTracer");
    GTGlobals::sleep(500);
    if (expected) {
        GT_CHECK(logTracer.isExpectedMessageFound, "Expected message is not found: " + logTracer.expectedMessage);
    } else {
        GT_CHECK(!logTracer.isExpectedMessageFound, "Expected message is found, but should not: " + logTracer.expectedMessage);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getErrors"
QStringList GTUtilsLog::getErrors(HI::GUITestOpStatus& /*os*/, const GTLogTracer& logTracer) {
    QStringList result;
    foreach (LogMessage* message, logTracer.getMessages()) {
        if (message->level == LogLevel_ERROR) {
            result << message->text;
        }
    }
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkMessageWithWait"
void GTUtilsLog::checkMessageWithWait(HI::GUITestOpStatus& os, const GTLogTracer& logTracer, const QString& message, int timeoutMillis) {
    for (int time = 0; time < timeoutMillis; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        if (logTracer.checkMessage(message)) {
            return;
        }
    }
    GT_FAIL("Message was not found in log: " + message, );
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkMessageWithTextCount"
void GTUtilsLog::checkMessageWithTextCount(HI::GUITestOpStatus& os, const QString& messagePart, int expectedMessageCount, const QString& context) {
    int messageCount = 0;
    QList<LogMessage*> messages = GTLogTracer::getMessages();
    for (auto message : qAsConst(messages)) {
        if (message->text.contains("checkMessageWithTextCount: Unexpected message count for text: '")) {
            continue;  // A harness message from one of the previous GT_CHECK calls. Contains the check message part.
        }
        messageCount += message->text.contains(messagePart, Qt::CaseInsensitive) ? 1 : 0;
    }
    GT_CHECK(messageCount == expectedMessageCount,
             QString("checkMessageWithTextCount: Unexpected message count for text: '%1', expected: %2, got: %3%4")
                 .arg(messagePart)
                 .arg(expectedMessageCount)
                 .arg(messageCount)
                 .arg(context.isEmpty() ? "" : ", context: " + context));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkNoErrorsInLog"
void GTUtilsLog::checkNoErrorsInLog(HI::GUITestOpStatus& os) {
    QList<LogMessage*> messages = GTLogTracer::getMessages();
    for (LogMessage* message : qAsConst(messages)) {
        GT_CHECK(message->level != LogLevel_ERROR, "Found error message in log: " + message->text);
    }
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
