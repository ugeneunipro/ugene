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

#include "Log.h"
#include <U2Core/Timer.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/LogCache.h>

#include <QSet>

namespace U2 {


LogServer::LogServer() : listenerMutex(QMutex::Recursive) {
    qRegisterMetaType<LogMessage>("LogMessage");
}

LogServer* LogServer::getInstance() {
    static LogServer instance;
    return &instance;
}

QStringList LogServer::getCategories() const {
    QSet<QString> uniqueNames;
    QStringList result;
    foreach(const Logger* l, loggers) {
        foreach(const QString& category, l->getCategories()) {
            if (uniqueNames.contains(category)) {
                continue;
            }
            result.append(category);
            uniqueNames.insert(category);
        }

    }
    return result;
}

void LogServer::addListener(LogListener* listner)
{
    QMutexLocker l(&listenerMutex);

    SAFE_POINT(NULL != listner, "Internal error during adding a log listner: NULL listner!",);
    SAFE_POINT(!listeners.contains(listner),
        "Internal error during adding a log listner: the listener is already added!",);
    listeners.append(listner);
}

void LogServer::removeListener(LogListener* listener)
{
    QMutexLocker l(&listenerMutex);

    int numOfListenersRemoved = listeners.removeAll(listener);
    SAFE_POINT(1 == numOfListenersRemoved, QString("Internal error during removing a log listener:"
        " unexpected number '%1' of listeners!").arg(numOfListenersRemoved),);
}

void LogServer::message(const LogMessage& m)
{
    QMutexLocker l(&listenerMutex);
    foreach (LogListener* listner, listeners)
    {
        listner->onMessage(m);
    }
}

void LogServer::message(const LogMessage& m, LogListener* listener)
{
    QMutexLocker l(&listenerMutex);
    listener->onMessage(m);
}


Logger::Logger(const QString& category1) {
    static int test = 0;
    test++;
    categoryNames << category1;
    init();
}

Logger::Logger(const QString& category1, const QString& category2) {
    categoryNames << category1 << category2;
    init();
}

Logger::Logger(const QString& category1, const QString& category2, const QString& category3) {
    categoryNames << category1 << category2 << category3;
    init();
}

Logger::Logger(const QString& category1, const QString& category2, const QString& category3, const QString& category4) {
    categoryNames << category1 << category2 << category3 << category4;
    init();
}

Logger::Logger(const QStringList& _categoryNames) {
    categoryNames = _categoryNames;
    init();
}

void Logger::init() {
    LogServer* s = LogServer::getInstance();
    s->loggers.append(this);
}

Logger::~Logger() {
// TODO possible race condition at shutdown -> log service could already be destroyed
//    LogServer* s = LogServer::getInstance();
//    s->categories.removeOne(this);
//    categoryNames.clear();
}

LogMessage::LogMessage(const QStringList& cat, LogLevel l, const QString& m)
: categories(cat), level(l), text(m), time(GTimer::currentTimeMicros())
{
}

void Logger::message(LogLevel level, const QString& msg)  {
    LogMessage m(categoryNames, level, msg);
    LogServer::getInstance()->message(m);
}

void Logger::message(LogLevel level, const QString& msg, const QString& extraCategory)  {
    message(level, msg, QStringList(extraCategory));
}

void Logger::message(LogLevel level, const QString& msg, const QStringList& extraCategories)  {
    QStringList resultCategories = categoryNames;
    resultCategories << extraCategories;
    LogMessage m(resultCategories, level, msg);
    LogServer::getInstance()->message(m);
}


void Logger::log(LogLevel level, const QString& message, const QString& category) {
    log(level, message, QStringList(category));
}

void Logger::log(LogLevel level, const QString& message, const QStringList& categoryies) {
    LogMessage m(categoryies, level, message);
    LogServer::getInstance()->message(m);
}


}//namespace
