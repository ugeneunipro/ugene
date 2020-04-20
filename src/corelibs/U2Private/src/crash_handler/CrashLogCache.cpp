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

#include <U2Core/AppResources.h>
#include <U2Core/U2SqlHelpers.h>

#include "CrashLogCache.h"

namespace U2 {

void CrashLogCache::onMessage(const LogMessage &msg) {
    static int count = 0;
    if (!(count++ % logMemoryInfoEvery)) {
        cmdLog.trace(formMemInfo());
    }

    LogCache::onMessage(msg);
}

QString CrashLogCache::formMemInfo() {
    AppResourcePool* pool = AppResourcePool::instance();
    CHECK(pool, QString());

    size_t memoryBytes = pool->getCurrentAppMemory();
    QString memInfo = QString("AppMemory: %1Mb").arg(memoryBytes/(1000*1000));
    AppResource *mem = pool->getResource(RESOURCE_MEMORY);
    if (mem) {
        memInfo += QString("; Locked memory AppResource: %1/%2").arg(mem->maxUse() - mem->available()).arg(mem->maxUse());
    }

    int currentMemory=0, maxMemory=0;
    if (SQLiteUtils::getMemoryHint(currentMemory, maxMemory, 0)) {
        memInfo += QString("; SQLite memory %1Mb, max %2Mb").arg(currentMemory/(1000*1000)).arg(maxMemory/(1000*1000));
    }

    return memInfo;
}

}   // namespace U2
