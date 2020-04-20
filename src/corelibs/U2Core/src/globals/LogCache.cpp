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

#include "LogCache.h"

#ifdef Q_OS_WIN32
#include "windows.h"
#endif

#include <stdio.h>

namespace U2 {

LogCache* LogCache::appGlobalCache = NULL;

LogFilterItem::LogFilterItem(const QString& _category , LogLevel _minLevel) {
    category = _category;
    minLevel = _minLevel;
}

QString LogFilter::selectEffectiveCategory(const LogMessage& msg) const {
    QString result;
    foreach (const LogFilterItem& f, filters) {
        if (msg.level >= f.minLevel && msg.categories.contains(f.category)) {
            result = f.category;
            break;
        }
    }
    return result;
}

bool LogFilter::matches(const LogMessage& msg) const {
    return !selectEffectiveCategory(msg).isEmpty();
}

//////////////////////////////////////////////////////////////////////////
// LogCache

LogCache::LogCache(int maxLogMessages) : maxLogMessages(maxLogMessages) {
    LogServer::getInstance()->addListener(this);
}

LogCache::~LogCache() {
    LogServer::getInstance()->removeListener(this);
    while (!messages.empty()) {
        LogMessage* m = messages.takeFirst();
        delete m;
    }
}

void LogCache::onMessage(const LogMessage& msg) {
    lock.lockForWrite();
    if (!filter.isEmpty() && !filter.matches(msg)) {
        lock.unlock();
        return;
    }

    messages.append(new LogMessage(msg.categories, msg.level, msg.text));
    updateSize();
    lock.unlock();
}

void LogCache::updateSize() {
    while (messages.size() > maxLogMessages) {
        LogMessage* m = messages.takeFirst();
        delete m;
    }
}

void LogCache::setAppGlobalInstance(LogCache* cache) {
    assert(appGlobalCache == NULL);
    appGlobalCache = cache;
}

QList<LogMessage> LogCache::getLastMessages(int count) {
    lock.lockForRead();
    int cacheSize = messages.size();
    if (count<0) {
        count = cacheSize;
    }
    int lastToAdd = qMax(0, cacheSize-count);

    QList<LogMessage> lastMessages;
    for (int i = cacheSize; --i>=lastToAdd;) {
        LogMessage* m = messages.at(i);
        if (m->categories.contains(ULOG_CAT_USER_ACTIONS))
            continue;
        lastMessages.prepend(*m);
    }
    lock.unlock();

    return lastMessages;
}

//////////////////////////////////////////////////////////////////////////
// Cache EXT

LogCacheExt::LogCacheExt() {
    consoleEnabled = false;
    fileEnabled = false;
}

bool LogCacheExt::setFileOutputEnabled(const QString& fileName) {
    if (fileName.isEmpty()) {
        file.close();
        fileEnabled = false;
        return true;
    }

    if (file.isOpen()) {
        file.close();
        fileEnabled = false;
    }

    file.setFileName(fileName);
    if (!file.open(QIODevice::WriteOnly  | QIODevice::Append)) {
        return false;
    }
    fileEnabled = true;
    return true;
}

void LogCacheExt::onMessage(const LogMessage& msg) {
    if (!filter.isEmpty() && !filter.matches(msg)) {
        return;
    }
    if (consoleEnabled){
        QByteArray ba = msg.text.toLocal8Bit();
        char* buf = ba.data();
#ifdef Q_OS_WIN32
        // a bit of magic to workaround Windows console encoding issues
        CharToOemA(buf,buf);
#endif
        printf("%s\n", buf);
    }

    if (fileEnabled) {
        QByteArray ba = msg.text.toLocal8Bit();
        char* buf = ba.data();
        file.write(buf, ba.length());
        file.write("\n", 1);
        file.flush();
    }

    LogCache::onMessage(msg);
}

void LogCacheExt::setFileOutputDisabled(){
    if (file.isOpen()) {
        file.close();
    }
    fileEnabled = false;
}


}//namespace

