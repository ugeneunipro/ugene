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

#include "AppResources.h"

#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>
#include <U2Core/AppSettings.h>
#include <U2Core/U2SafePoints.h>
#include <U2Test/GTest.h>

#include <QThread>
#include <QProcess>

#if defined(Q_OS_LINUX) || defined(Q_OS_FREEBSD)
#    include <stdio.h>
#    include <unistd.h>    //for sysconf(3)
#endif
#if defined(Q_OS_LINUX)
#    include <fstream>
#endif

#ifdef Q_OS_WIN
#include <windows.h>
#include <Psapi.h>
#include <Winbase.h> //for IsProcessorFeaturePresent
#endif

namespace U2 {

#if defined(Q_OS_LINUX)
void process_mem_usage(size_t &vm_usage) {
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

    stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt >> utime >> stime >> cutime >> cstime >> priority >> nice >> O >> itrealvalue >> starttime >> vsize;    // don't care about the rest

    stat_stream.close();

    vm_usage = vsize;
}
#endif

#define SETTINGS_ROOT QString("app_resource/")

AppResourcePool::AppResourcePool() {
    Settings *s = AppContext::getSettings();
    idealThreadCount = s->getValue(SETTINGS_ROOT + "idealThreadCount", QThread::idealThreadCount()).toInt();

    int maxThreadCount = s->getValue(SETTINGS_ROOT + "maxThreadCount", 1000).toInt();
    threadResource = new AppResourceSemaphore(RESOURCE_THREAD, maxThreadCount, tr("Threads"));
    registerResource(threadResource);

    int totalPhysicalMemory = getTotalPhysicalMemory();
    int maxMem = s->getValue(SETTINGS_ROOT + "maxMem", totalPhysicalMemory).toInt();
#if defined(Q_OS_MAC64) || defined(Q_OS_WIN64) || defined(UGENE_X86_64) || defined(__amd64__) || defined(__AMD64__) || defined(__x86_64__) || defined(_M_X64)
    maxMem = maxMem > x64MaxMemoryLimitMb ? x64MaxMemoryLimitMb : maxMem;
#else
    maxMem = maxMem > x32MaxMemoryLimitMb ? x32MaxMemoryLimitMb : maxMem;
#endif

    memResource = new AppResourceSemaphore(RESOURCE_MEMORY, maxMem, tr("Memory"), tr("Mb"));
    registerResource(memResource);

    projectResouce = new AppResourceSemaphore(RESOURCE_PROJECT, 1, tr("Project"));
    registerResource(projectResouce);

    listenLogInGTest = new AppResourceReadWriteLock(RESOURCE_LISTEN_LOG_IN_TESTS, "LogInTests");
    registerResource(listenLogInGTest);
}

AppResourcePool::~AppResourcePool() {
    qDeleteAll(resources.values());
}

int AppResourcePool::getTotalPhysicalMemory() {
    int totalPhysicalMemory = defaultMemoryLimitMb;

#if defined(Q_OS_WIN32)
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
    // of pages would hardly overflow `long' when multiplied by a small
    // number (number of pages / 1024), so we should be safe here.
    totalPhysicalMemory = (int)(numpages * (pagesize / 1024) / 1024);

#elif defined(Q_OS_MAC)
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

bool AppResourcePool::is32BitBuild() {
    bool result = false;
#ifdef Q_PROCESSOR_X86_32
    result = true;
#endif
    return result;
}

bool AppResourcePool::isSystem64bit() {
#ifdef Q_OS_MAC
    QProcess p;
    p.start("sysctl", QStringList() << "-n"
                                    << "hw.optional.x86_64");
    p.waitForFinished();
    const QString system_info = p.readAllStandardOutput();
    p.close();
    bool ok = false;
    int is64bit = system_info.toInt(&ok);
    return is64bit != 0;
#endif

    return false;
}

void AppResourcePool::setIdealThreadCount(int n) {
    SAFE_POINT(n > 0 && n <= threadResource->maxUse(), QString("Invalid ideal threads count: %1").arg(n), );

    n = qBound(1, n, threadResource->maxUse());
    idealThreadCount = n;
    AppContext::getSettings()->setValue(SETTINGS_ROOT + "idealThreadCount", idealThreadCount);
}

void AppResourcePool::setMaxThreadCount(int n) {
    SAFE_POINT(n >= 1, QString("Invalid max threads count: %1").arg(n), );

    threadResource->setMaxUse(qMax(idealThreadCount, n));
    AppContext::getSettings()->setValue(SETTINGS_ROOT + "maxThreadCount", threadResource->maxUse());
}

void AppResourcePool::setMaxMemorySizeInMB(int n) {
    SAFE_POINT(n >= MIN_MEMORY_SIZE, QString("Invalid max memory size: %1").arg(n), );

    memResource->setMaxUse(qMax(n, MIN_MEMORY_SIZE));
    AppContext::getSettings()->setValue(SETTINGS_ROOT + "maxMem", memResource->maxUse());
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
    if (ok) {
        return output_mem;
    }
#elif defined(Q_OS_MAC)
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
#endif
    return -1;
}

bool AppResourcePool::isSSE2Enabled() {
    bool answer = false;
#if defined(Q_OS_WIN)
    //Using WinAPI call on Windows.

    //Return Value
    //If the feature is supported, the return value is a nonzero value.
    //If the feature is not supported, the return value is zero.
    //
    //If the HAL does not support detection of the feature,
    //whether or not the hardware supports the feature, the return value is also zero.
    //
    //Windows 2000: This feature is not supported.
    //
    //Header:  Winbase.h (include Windows.h)
    //Library: Kernel32.lib
    //DLL:     Kernel32.dll

    answer = (bool)IsProcessorFeaturePresent(PF_XMMI64_INSTRUCTIONS_AVAILABLE);
#elif defined(__amd64__) || defined(__AMD64__) || defined(__x86_64__) || defined(_M_X64)
    answer = true;
#elif defined(__i386__) || defined(__X86__) || defined(_M_IX86)
    //cpuid instruction:
    //- takes 0x1 on eax,
    //- returns standard features flags in edx, bit 26 is SSE2 flag
    //- clobbers ebx, ecx
    unsigned int fflags = 0;
    unsigned int stub = 0;
    __asm__ __volatile__(
        "push %%ebx; cpuid; mov %%ebx, %%edi; pop %%ebx"
        : "=a"(stub),
          "=D"(stub),
          "=c"(stub),
          "=d"(fflags)
        : "a"(0x1));
    answer = ((fflags & (1 << 26)) != 0);
#endif
    return answer;
}

void AppResourcePool::registerResource(AppResource *r) {
    SAFE_POINT(NULL != r, "", );
    SAFE_POINT(!resources.contains(r->getResourceId()), QString("Duplicate resource: %1").arg(r->getResourceId()), );

    resources[r->getResourceId()] = r;
}

void AppResourcePool::unregisterResource(int id) {
    CHECK(resources.contains(id), );
    delete resources.take(id);
}

AppResource *AppResourcePool::getResource(int id) const {
    return resources.value(id, NULL);
}

AppResourcePool *AppResourcePool::instance() {
    return AppContext::getAppSettings() ? AppContext::getAppSettings()->getAppResourcePool() : NULL;
}

MemoryLocker &MemoryLocker::operator=(MemoryLocker &other) {
    MemoryLocker tmp(other);
    qSwap(os, tmp.os);
    qSwap(preLockMB, tmp.preLockMB);
    qSwap(lockedMB, tmp.lockedMB);
    qSwap(needBytes, tmp.needBytes);
    qSwap(memoryLockType, tmp.memoryLockType);
    qSwap(resource, tmp.resource);
    qSwap(errorMessage, tmp.errorMessage);

    return *this;
}

}    // namespace U2
