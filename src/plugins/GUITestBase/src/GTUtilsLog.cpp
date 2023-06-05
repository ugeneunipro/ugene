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

#include "GTUtilsLog.h"
#include <utils/GTThread.h>

#include "U2Core/LogCache.h"
#include <U2Core/U2SafePoints.h>

namespace U2 {
using namespace HI;

GTLogTracer::GTLogTracer() {
    LogServer::getInstance()->addListener(this);
}

GTLogTracer::~GTLogTracer() {
    LogServer::getInstance()->removeListener(this);
}

/** Returns true if the message was produced by the GUI testing framework. */
static bool isGuiTestSystemMessage(const QString& message) {
    return message.contains("] GT_");
}

void GTLogTracer::onMessage(const LogMessage& msg) {
    if (msg.level == LogLevel_ERROR) {
        if (isOsMac()) {
            // Ignore QT errors on new MacOS. These errors can only be fixed by migration to the latest Qt.
            // Test to reproduce: GUITest_assembly_extract_consensus_test_0001_single_input.
            if (msg.text.contains("has active key-value observers (KVO)! These will stop working now that the window is recreated, and will result in exceptions when the observers are removed")) {
                return;
            }
        }
        errorMessages << msg.text;
    }
    allMessages << msg.text;
}

bool GTLogTracer::hasErrors() const {
    return !errorMessages.isEmpty();
}

QString GTLogTracer::getJoinedErrorString() const {
    return errorMessages.isEmpty() ? "" : errorMessages.join("\n");
}

void GTLogTracer::clear() {
    errorMessages.clear();
    allMessages.clear();
}

static bool findMessage(const QString& substring, const QStringList& messages) {
    for (const QString& message : qAsConst(messages)) {
        if (!isGuiTestSystemMessage(message) && message.contains(substring, Qt::CaseInsensitive)) {
            return true;
        }
    }
    return false;
}
bool GTLogTracer::hasMessage(const QString& substring) const {
    return findMessage(substring, allMessages);
}

bool GTLogTracer::hasError(const QString& substring) const {
    return findMessage(substring, errorMessages);
}

#define GT_CLASS_NAME "GTUtilsLog"

#define GT_METHOD_NAME "checkMessageWithWait"
void GTUtilsLog::checkMessageWithWait(HI::GUITestOpStatus& os, const GTLogTracer& lt, const QString& message, int timeoutMillis) {
    for (int time = 0; time < timeoutMillis; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        if (lt.hasMessage(message)) {
            return;
        }
    }
    GT_FAIL("Message was not found in log: " + message, );
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkMessageWithTextCount"
void GTUtilsLog::checkMessageWithTextCount(HI::GUITestOpStatus& os, const QString& messagePart, int expectedMessageCount, const QString& context) {
    int messageCount = 0;
    const QList<LogMessage*>& messages = LogCache::getAppGlobalInstance()->messages;
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

#undef GT_CLASS_NAME

}  // namespace U2
