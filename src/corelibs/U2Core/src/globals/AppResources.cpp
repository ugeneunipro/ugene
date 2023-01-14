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

#include "AppResources.h"

#include <QProcess>
#include <QThread>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>

#include <U2Test/GTest.h>

#if defined(Q_OS_LINUX) || defined(Q_OS_FREEBSD)
#    include <unistd.h>  //for sysconf(3)
#endif
#if defined(Q_OS_LINUX)
#    include <fstream>
#endif

#ifdef Q_OS_WIN
// clang-format off
#    include <windows.h>
#    include <Psapi.h>
// clang-format on
#endif

namespace U2 {

#if defined(Q_OS_LINUX)
void process_mem_usage(size_t& vm_usage) {
    using std::ifstream;
    using std::ios_base;
    using std::string;

    vm_usage = 0;

    // 'file' stat seems to give the most reliable results
    ifstream stat_stream("/proc/self/stat");
    CHECK(stat_stream.good(), );

    // dummy vars for leading entries in stat that we don't care about
    string pid, comm, state, ppid, pgrp, session, tty_nr;
    string tpgid, flags, minflt, cminflt, majflt, cmajflt;
    string utime, stime, cutime, cstime, priority, nice;
    string O, itrealvalue, starttime;

    // the two fields we want
    unsigned long vsize;

    stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt >> utime >> stime >> cutime >> cstime >> priority >> nice >> O >> itrealvalue >> starttime >> vsize;  // don't care about the rest

    stat_stream.close();

    vm_usage = vsize;
}
#endif

#define SETTINGS_ROOT QString("app_resource/")

AppResourcePool::AppResourcePool() {
    Settings* s = AppContext::getSettings();
    idealThreadCount = s->getValue(SETTINGS_ROOT + "idealThreadCount", QThread::idealThreadCount()).toInt();

    int maxThreadCount = s->getValue(SETTINGS_ROOT + "maxThreadCount", 1000).toInt();
    threadResource = new AppResourceSemaphore(RESOURCE_THREAD, maxThreadCount, tr("Threads"));
    registerResource(threadResource);

    int totalPhysicalMemory = getTotalPhysicalMemory();
    int maxMem = s->getValue(SETTINGS_ROOT + "maxMem", totalPhysicalMemory).toInt();
    maxMem = maxMem > x64MaxMemoryLimitMb ? x64MaxMemoryLimitMb : maxMem;

    memResource = new AppResourceSemaphore(RESOURCE_MEMORY, maxMem, tr("Memory"), tr("Mb"));
    registerResource(memResource);

    projectResource = new AppResourceSemaphore(RESOURCE_PROJECT, 1, tr("Project"));
    registerResource(projectResource);

    listenLogInGTest = new AppResourceReadWriteLock(RESOURCE_LISTEN_LOG_IN_TESTS, "LogInTests");
    registerResource(listenLogInGTest);
}

AppResourcePool::~AppResourcePool() {
    qDeleteAll(resources.values());
}

int AppResourcePool::getTotalPhysicalMemory() {
#if defined(Q_OS_WIN32)
    int totalPhysicalMemory = defaultMemoryLimitMb;
    MEMORYSTATUSEX memory_status;
    ZeroMemory(&memory_status, sizeof(MEMORYSTATUSEX));
    memory_status.dwLength = sizeof(memory_status);
    if (GlobalMemoryStatusEx(&memory_status)) {
        totalPhysicalMemory = memory_status.ullTotalPhys / (1024 * 1024);
    } else {
        coreLog.error("Total physical memory: getting info error");
    }

#elif defined(Q_OS_LINUX) || defined(Q_OS_FREEBSD)
    long pagesize = sysconf(_SC_PAGESIZE);
    long numpages = sysconf(_SC_PHYS_PAGES);

    // Assume that page size is always a multiple of 1024, so it can be
    // divided without losing any precision.  On the other hand, number
    // of pages would hardly overflow 'long' when multiplied by a small
    // number (number of pages / 1024), so we should be safe here.
    int totalPhysicalMemory = (int)(numpages * (pagesize / 1024) / 1024);

#elif defined(Q_OS_DARWIN)
    int totalPhysicalMemory = defaultMemoryLimitMb;
    QProcess p;
    p.start("sysctl", QStringList() << "-n"
                                    << "hw.memsize");
    p.waitForFinished();
    QString system_info = p.readAllStandardOutput();
    p.close();
    bool ok = false;
    qlonglong output_mem = system_info.toLongLong(&ok);
    if (ok) {
        totalPhysicalMemory = output_mem / (1024 * 1024);
    }
#else
    coreLog.error("Total physical memory: Unsupported OS");
#endif

    return totalPhysicalMemory;
}

void AppResourcePool::setIdealThreadCount(int n) {
    SAFE_POINT(n > 0 && n <= threadResource->getMaximumUsage(), QString("Invalid ideal threads count: %1").arg(n), );

    n = qBound(1, n, threadResource->getMaximumUsage());
    idealThreadCount = n;
    AppContext::getSettings()->setValue(SETTINGS_ROOT + "idealThreadCount", idealThreadCount);
}

void AppResourcePool::setMaxThreadCount(int n) {
    SAFE_POINT(n >= 1, QString("Invalid max threads count: %1").arg(n), );

    threadResource->setMaximumUsage(qMax(idealThreadCount, n));
    AppContext::getSettings()->setValue(SETTINGS_ROOT + "maxThreadCount", threadResource->getMaximumUsage());
}

static constexpr int MIN_MAXIMUM_MEMORY_SIZE_MB = 200;

void AppResourcePool::setMaxMemorySizeInMB(int n) {
    int maxMemorySize = qMax(n, MIN_MAXIMUM_MEMORY_SIZE_MB);
    memResource->setMaximumUsage(maxMemorySize);
    AppContext::getSettings()->setValue(SETTINGS_ROOT + "maxMem", maxMemorySize);
    SAFE_POINT(n >= MIN_MAXIMUM_MEMORY_SIZE_MB, "Invalid max memory size: " + QString::number(n), );
}

size_t AppResourcePool::getCurrentAppMemory() {
#ifdef Q_OS_WIN
    PROCESS_MEMORY_COUNTERS memCounter;
    bool result = GetProcessMemoryInfo(GetCurrentProcess(), &memCounter, sizeof(memCounter));
    return result ? memCounter.WorkingSetSize : 0;
#elif defined(Q_OS_LINUX)
    size_t vm = 0;
    process_mem_usage(vm);
    return vm;
#elif defined(Q_OS_FREEBSD)
    QProcess p;
    p.start("ps", QStringList() << "-o"
                                << "vsize="
                                << "-p" << QString("%1").arg(getpid()));
    p.waitForFinished();
    QString ps_vsize = p.readAllStandardOutput();
    p.close();
    bool ok = false;
    qlonglong output_mem = ps_vsize.toLongLong(&ok);
    return ok ? output_mem : 0;
// #elif defined(Q_OS_DARWIN)
//    qint64 pid = QCoreApplication::applicationPid();

//    QProcess p;
//    // Virtual private memory size in megabytes
//    p.start("sh", QStringList() << "-c" << "top -l 1 -pid " + QString::number(pid) + " -e -stats vprvt | tail -1 | sed 's/M+//g'");
//    p.waitForFinished();
//    const QString outputString = p.readAllStandardOutput();
//    p.close();
//    bool ok = false;
//    size_t output_mem = outputString.toULong(&ok);
//    if (ok) {
//        return output_mem * 1024 * 1024;
//    }
#else
    return 0;
#endif
}

void AppResourcePool::registerResource(AppResource* r) {
    SAFE_POINT(nullptr != r, "", );
    SAFE_POINT(!resources.contains(r->getResourceId()), QString("Duplicate resource: %1").arg(r->getResourceId()), );

    resources[r->getResourceId()] = r;
}

void AppResourcePool::unregisterResource(int id) {
    CHECK(resources.contains(id), );
    delete resources.take(id);
}

AppResource* AppResourcePool::getResource(int id) const {
    return resources.value(id, nullptr);
}

AppResourcePool* AppResourcePool::instance() {
    return AppContext::getAppSettings() ? AppContext::getAppSettings()->getAppResourcePool() : nullptr;
}

////////////////////////////////////////////////
///////////// AppResource

AppResource::AppResource(int id, int _maxUse, const QString& _name, const QString& _suffix)
    : name(_name), suffix(_suffix), resourceId(id), maximumUsage(_maxUse) {
}

int AppResource::getMaximumUsage() const {
    return maximumUsage;
}

int AppResource::getResourceId() const {
    return resourceId;
}

////////////////////////////////////////////////
///////////// AppResourceReadWriteLock

AppResourceReadWriteLock::AppResourceReadWriteLock(int id, const QString& _name, const QString& _suffix)
    : AppResource(id, Write, _name, _suffix) {
    resource = new QReadWriteLock();
}

AppResourceReadWriteLock::~AppResourceReadWriteLock() {
    delete resource;
}

void AppResourceReadWriteLock::acquire(int type) {
    SAFE_POINT(type == UseType::Read || type == UseType::Write, "AppResourceReadWriteLock::acquire. Invalid lock type: " + QString::number(type), );
    if (type == UseType::Write) {
        resource->lockForWrite();
    } else {
        resource->lockForRead();
    }
}

bool AppResourceReadWriteLock::tryAcquire(int type) {
    SAFE_POINT(type == UseType::Read || type == UseType::Write, "AppResourceReadWriteLock::tryAcquire. Invalid lock type: " + QString::number(type), false);
    return type == UseType::Write ? resource->tryLockForWrite() : resource->tryLockForRead();
}

bool AppResourceReadWriteLock::tryAcquire(int type, int timeout) {
    SAFE_POINT(type == UseType::Read || type == UseType::Write, "AppResourceReadWriteLock::tryAcquire(timeout). Invalid lock type: " + QString::number(type), false);
    return type == UseType::Write ? resource->tryLockForWrite(timeout) : resource->tryLockForRead(timeout);
}

void AppResourceReadWriteLock::release(int) {
    resource->unlock();
}

int AppResourceReadWriteLock::available() const {
    return -1;
}

////////////////////////////////////////////////
///////////// AppResourceReadWriteLock

AppResourceSemaphore::AppResourceSemaphore(int id, int _maxUse, const QString& _name, const QString& _suffix)
    : AppResource(id, _maxUse, _name, _suffix) {
    resource = new QSemaphore(_maxUse);
}

AppResourceSemaphore::~AppResourceSemaphore() {
    delete resource;
}

void AppResourceSemaphore::acquire(int n) {
    LOG_TRACE(acquire);
    resource->acquire(n);
}

bool AppResourceSemaphore::tryAcquire(int n) {
    LOG_TRACE(tryAcquire);
    return resource->tryAcquire(n);
}

bool AppResourceSemaphore::tryAcquire(int n, int timeout) {
    LOG_TRACE(tryAcquire_timeout);
    return resource->tryAcquire(n, timeout);
}

void AppResourceSemaphore::release(int n) {
    LOG_TRACE(release);
    SAFE_POINT(n >= 0, QString("AppResource %1 release %2 < 0 called").arg(name).arg(n), );
    resource->release(n);
    // QSemaphore allow to create resources by releasing, we do not want to get such behavior
    int avail = resource->available();
    SAFE_POINT(avail <= maximumUsage, "Invalid result available resource value: " + QString::number(avail), );
}

int AppResourceSemaphore::available() const {
    return resource->available();
}

void AppResourceSemaphore::setMaximumUsage(int n) {
    LOG_TRACE(setMaximumUsage);
    int diff = n - maximumUsage;
    if (diff > 0) {
        // adding resources
        resource->release(diff);
        maximumUsage += diff;
    } else {
        diff = -diff;
        // safely remove resources
        for (int i = diff; i > 0; i--) {
            bool ok = resource->tryAcquire(i, 0);
            if (ok) {
                // successfully acquired i resources
                maximumUsage -= i;
                break;
            }
        }
    }
}

////////////////////////////////////////////////
///////////// MemoryLocker

MemoryLocker::MemoryLocker(U2OpStatus& os, int preLockMB)
    : os(&os),
      preLockMB(preLockMB > 0 ? preLockMB : 0) {
    resource = AppResourcePool::instance()->getResource(RESOURCE_MEMORY);
    tryAcquire(0);
}

MemoryLocker::MemoryLocker(int preLockMB)
    : preLockMB(preLockMB > 0 ? preLockMB : 0) {
    resource = AppResourcePool::instance()->getResource(RESOURCE_MEMORY);
    tryAcquire(0);
}

MemoryLocker::~MemoryLocker() {
    release();
}

bool MemoryLocker::tryAcquire(qint64 bytes) {
    needBytes += bytes;

    int needMB = needBytes / (1000 * 1000) + preLockMB;
    if (needMB > lockedMB) {
        int diff = needMB - lockedMB;
        CHECK_EXT(resource != nullptr, if (os) os->setError("MemoryLocker - Resource error"), false);
        bool ok = resource->tryAcquire(diff);
        if (ok) {
            lockedMB = needMB;
        } else {
            errorMessage = QString("MemoryLocker - Not enough memory error, %1 megabytes are required").arg(needMB);
            if (nullptr != os) {
                os->setError(errorMessage);
            }
        }
        return ok;
    }
    return true;
}

void MemoryLocker::release() {
    CHECK_EXT(resource != nullptr, if (os) os->setError("MemoryLocker - Resource error"), );
    if (lockedMB > 0) {
        resource->release(lockedMB);
    }
    lockedMB = 0;
    needBytes = 0;
}

bool MemoryLocker::hasError() const {
    return !errorMessage.isEmpty();
}

const QString& MemoryLocker::getError() const {
    return errorMessage;
}

}  // namespace U2
