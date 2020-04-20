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

#ifndef _U2_LOG_H_
#define _U2_LOG_H_

#include <U2Core/global.h>

#include <QMetaType>
#include <QMutex>
#include <QTime>
#include <QStringList>

namespace U2 {

enum LogLevel { LogLevel_TRACE, LogLevel_DETAILS, LogLevel_INFO, LogLevel_ERROR, LogLevel_NumLevels};

class U2CORE_EXPORT LogMessage {
public:
    LogMessage() {}
    LogMessage(const QStringList& cat, LogLevel l, const QString& m);

    QStringList categories;
    LogLevel    level;
    QString     text;
    qint64      time; //time in microseconds from Unix Epoch (UTC). See Timer.h
};


class U2CORE_EXPORT Logger {
public:
    Logger(const QString& category1);
    Logger(const QString& category1, const QString& category2);
    Logger(const QString& category1, const QString& category2, const QString& category3);
    Logger(const QString& category1, const QString& category2, const QString& category3, const QString& category4);
    Logger(const QStringList& categoryNames);

    virtual ~Logger();

    static void log(LogLevel level, const QString& message, const QString& category);

    static void log(LogLevel level, const QString& message, const QStringList& categoryies);

    virtual void message(LogLevel level, const QString& msg);

    void message(LogLevel level, const QString& msg, const QString& extraCategory);

    void message(LogLevel level, const QString& msg, const QStringList& extraCategories);

    void trace(const QString& msg)  { message(LogLevel_TRACE, msg);}

    void details(const QString& msg)  { message(LogLevel_DETAILS, msg);}

    void info(const QString& msg)  { message(LogLevel_INFO, msg);}

    void error(const QString& msg)  { message(LogLevel_ERROR, msg);}

    const QStringList& getCategories() const {return categoryNames;}

protected:
    void init();
    QStringList categoryNames;
};

class U2CORE_EXPORT LogListener {
public:
    virtual void onMessage(const LogMessage& m) = 0;
};


class U2CORE_EXPORT LogServer : public QObject {
    Q_OBJECT
    friend class Logger;
    friend class UiLogger;
public:
    LogServer();
    static LogServer* getInstance();
    const QList<Logger*>& getLoggers() const {return loggers;}
    QStringList getCategories() const;

    void addListener(LogListener* listner);
    void removeListener(LogListener* listener);

private:
    void message(const LogMessage& m);
    void message(const LogMessage& m, LogListener* listener);

    QList<Logger*> loggers;
    QList<LogListener*> listeners;
    QMutex listenerMutex;
};


//TODO: support log category translation + use log category ids instead of the names in code

// Log category for algorithms and computing details
#define ULOG_CAT_ALGORITHM          "Algorithms"
// Log category for console output
#define ULOG_CAT_CONSOLE            "Console"
// Log category for core service (internal model events)
#define ULOG_CAT_CORE_SERVICES      "Core Services"
// Log category for IO events
#define ULOG_CAT_IO                 "Input/Output"
// Log category for performance tracking
#define ULOG_CAT_PERFORMANCE        "Performance"
// Log category for scripting related logs
#define ULOG_CAT_SCRIPTS            "Scripts"
// Log category for task machinery related logs
#define ULOG_CAT_TASKS              "Tasks"
// Log category for UI related events
#define ULOG_CAT_USER_INTERFACE     "User Interface"
// Log category for user's actions
#define ULOG_CAT_USER_ACTIONS    "User Actions"


static Logger algoLog(ULOG_CAT_ALGORITHM);
static Logger cmdLog(ULOG_CAT_CONSOLE);
static Logger coreLog(ULOG_CAT_CORE_SERVICES);
static Logger ioLog(ULOG_CAT_IO);
static Logger perfLog(ULOG_CAT_PERFORMANCE);
static Logger scriptLog(ULOG_CAT_SCRIPTS);
static Logger taskLog(ULOG_CAT_TASKS);
static Logger uiLog(ULOG_CAT_USER_INTERFACE);
static Logger userActLog(ULOG_CAT_USER_ACTIONS);


}//namespace

Q_DECLARE_METATYPE( U2::LogMessage )


#endif

