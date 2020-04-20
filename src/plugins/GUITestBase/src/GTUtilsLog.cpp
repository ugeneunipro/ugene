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

#include "GTUtilsLog.h"
#include "U2Core/LogCache.h"
#include <U2Core/U2SafePoints.h>

namespace U2 {
using namespace HI;

GTLogTracer::GTLogTracer(QString _expectedMessage)
: wasError(false), wasMessage(false), expectedMessage(_expectedMessage) {
    LogServer::getInstance()->addListener(this);
}

GTLogTracer::~GTLogTracer(){
    LogServer::getInstance()->removeListener(this);
}

void GTLogTracer::onMessage(const LogMessage &msg) {

    if (msg.level == LogLevel_ERROR) {
        wasError = true;
        error = msg.text;
    }

    if(expectedMessage != ""){
        if (msg.text.contains(expectedMessage)){
            wasMessage = true;
        }
    }


}

QList<LogMessage*> GTLogTracer::getMessages(){
    return LogCache::getAppGlobalInstance()->messages;
}

bool GTLogTracer::checkMessage(QString s){
    QList<LogMessage*> messages = getMessages();
    QList<QString> textMessages;
    foreach(LogMessage* message, messages){
        textMessages.append(message->text);
    }

    foreach(QString message, textMessages){
        if (message.contains(s, Qt::CaseInsensitive)){
            return true;
        }
    }
    return false;
}

#define GT_CLASS_NAME "GTUtilsLog"
#define GT_METHOD_NAME "checkMessageStartsWith"
bool GTLogTracer::checkMessageStartsWith(QString s) {
    QList<LogMessage*> messages = getMessages();
    QList<QString> textMessages;
    foreach(LogMessage* message, messages) {
        if (message->text.startsWith(s, Qt::CaseInsensitive)) {
            return true;
        }
    }
    return false;
}
#undef GT_METHOD_NAME

#define GT_CLASS_NAME "GTUtilsLog"
#define GT_METHOD_NAME "check"
void GTUtilsLog::check(HI::GUITestOpStatus &os, const GTLogTracer& logTracer) {
    Q_UNUSED(os);
    GTGlobals::sleep(500);
    GT_CHECK(!logTracer.hasError(), "There is an error in log: " + logTracer.getError());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkContainsError"
void GTUtilsLog::checkContainsError(HI::GUITestOpStatus &os, const GTLogTracer &logTracer, const QString &messagePart) {
    Q_UNUSED(os);
    GTGlobals::sleep(500);
    GT_CHECK(logTracer.hasError(), "There is no errors in the log");
    GT_CHECK(logTracer.getError().contains(messagePart), "The log doesn't contains error message");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkContainsMessage"
void GTUtilsLog::checkContainsMessage(HI::GUITestOpStatus &os, const GTLogTracer &logTracer, bool expected) {
    Q_UNUSED(os);
    GT_CHECK(logTracer.getExpectedMessage() != "", "Expected message shoul be specyfied on creating GTLogtracer");
    GTGlobals::sleep(500);
    if(expected){
        GT_CHECK(logTracer.messageFound(), "message not found");
    }else{
        GT_CHECK(!logTracer.messageFound(), "message unexpectidly found");
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getErrors"
QStringList GTUtilsLog::getErrors(HI::GUITestOpStatus & /*os*/, const GTLogTracer &logTracer) {
    QStringList result;
    foreach (LogMessage *message, logTracer.getMessages()) {
        if (LogLevel_ERROR == message->level) {
            result << message->text;
        }
    }
    return result;
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

} // namespace
