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

#ifndef _U2_LOG_CACHE_H_
#define _U2_LOG_CACHE_H_

#include <U2Core/Log.h>

#include <QFileInfo>
#include <QReadWriteLock>

namespace U2 {

#define MAX_CACHE_SIZE 5000

class U2CORE_EXPORT LogFilterItem {
public:
    QString category;
    LogLevel minLevel;
    LogFilterItem(const QString& category = QString(), LogLevel minLevel = LogLevel_INFO);
};

class U2CORE_EXPORT LogFilter {
public:
    LogFilter() {}
    QList<LogFilterItem> filters;
    bool isEmpty() const { return filters.isEmpty(); }
    bool matches(const LogMessage& msg) const;
    QString selectEffectiveCategory(const LogMessage& msg) const;

};


class U2CORE_EXPORT LogCache : public QObject, public LogListener {
    Q_OBJECT
public:
    LogCache(int maxLogMessages = MAX_CACHE_SIZE);
    virtual ~LogCache();

    static void setAppGlobalInstance(LogCache* cache);
    static LogCache* getAppGlobalInstance() { return appGlobalCache; }

    virtual void onMessage(const LogMessage& msg);
    QList<LogMessage> getLastMessages(int count = -1);

private:
    static LogCache* appGlobalCache;
    void updateSize();

    QReadWriteLock lock;
    int maxLogMessages;
public:
    QList<LogMessage*> messages;
    LogFilter filter;
};


/** Extended cache version that dumps messages to file or console */
class U2CORE_EXPORT LogCacheExt : public LogCache {
    Q_OBJECT
public:
    LogCacheExt();

    void setConsoleOutputEnabled(bool enabled) { consoleEnabled = enabled; }
    bool isConsoleOutputEnabled() const { return consoleEnabled; }

    bool setFileOutputEnabled(const QString& file);
    void setFileOutputDisabled();
    bool isFileOutputEnabled() const { return fileEnabled; }
    QString getFileOutputPath() const { return QFileInfo(file).canonicalFilePath(); }

    virtual void onMessage(const LogMessage& msg);


private:
    bool consoleEnabled;
    bool fileEnabled;
    QFile file;

};

} //namespace

#endif
