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

#ifndef _U2_APPRESOURCES_H_
#define _U2_APPRESOURCES_H_

#include <QHash>
#include <QReadWriteLock>
#include <QSemaphore>

#include <U2Core/Log.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/global.h>

namespace U2 {

/** Thread resource - number of threads */
#define UGENE_RESOURCE_ID_THREAD "Threads"

/** Memory resource - amount of memory in megabytes */
#define UGENE_RESOURCE_ID_MEMORY "Memory"

/**
    Project resource. There is 1 project active in the system and if the resource is locked
    no project load/unload operation is possible
*/
#define UGENE_RESOURCE_ID_PROJECT "Project"

/** Abstraction of a unique logical resource with max capacity. */
class U2CORE_EXPORT AppResource {
public:
    AppResource(const QString& resourceId, int capacity, const QString& units = "");
    virtual ~AppResource() = default;

    AppResource(const AppResource& other) = delete;
    AppResource& operator=(const AppResource& other) = delete;

    virtual void acquire(int n) = 0;

    virtual bool tryAcquire(int n) = 0;

    virtual bool tryAcquire(int n, int timeout) = 0;

    virtual void release(int n) = 0;

    /** Returns available amount of the resource. If returns -1 the method is not supported. */
    virtual int available() const = 0;

    int getCapacity() const;

    /** Unique resource id. */
    const QString id;

    /** Visual resource units: Mb, Kb, threads, etc…. */
    const QString units;

protected:
    /** Maximum limit available for acquire for the resource. */
    int capacity = -1;
};

/** Resource based on QReadWriteLock. The resource must be acquire with either UseType::Read or UseType::Write. */
class U2CORE_EXPORT AppResourceReadWriteLock : public AppResource {
public:
    AppResourceReadWriteLock(const QString& resourceId);

    ~AppResourceReadWriteLock() override;

    enum UseType {
        Read = 0,
        Write = 1,
    };

    void acquire(int n) override;

    bool tryAcquire(int n) override;

    bool tryAcquire(int n, int timeout) override;

    void release(int) override;

    int available() const override;

private:
    QReadWriteLock* resource = nullptr;
};

/** Resource based on QSemaphore (counter). The resource may be acquired up to 'capacity' times. */
class U2CORE_EXPORT AppResourceSemaphore : public AppResource {
public:
    AppResourceSemaphore(const QString& resourceId, int capacity = 1, const QString& units = "");

    ~AppResourceSemaphore() override;

    void acquire(int n) override;

    bool tryAcquire(int n) override;

    bool tryAcquire(int n, int timeout) override;

    void release(int n) override;

    int available() const override;

    void setCapacity(int n);

private:
    QSemaphore* resource = nullptr;
};

class U2CORE_EXPORT AppResourcePool : public QObject {
    Q_OBJECT
public:
    AppResourcePool();
    ~AppResourcePool() override;

    int getIdealThreadCount() const;

    void setIdealThreadCount(int n);

    int getMaxThreadCount() const;

    void setMaxThreadCount(int n) const;

    int getMaxMemorySizeInMB() const;

    void setMaxMemorySizeInMB(int m);

    static size_t getCurrentAppMemory();

    void registerResource(AppResource* r);

    void unregisterResource(const QString& id);

    /** Returns registered resource with the given id or nullptr if resource is not found. */
    AppResource* getResource(const QString& id) const;

    static AppResourcePool* instance();

    static int getTotalPhysicalMemory();

    static const int x64MaxMemoryLimitMb = 2 * 1024 * 1024;  // 2Tb

private:
    static const int defaultMemoryLimitMb = 8 * 1024;

    QHash<QString, AppResource*> resources;

    int idealThreadCount = 0;

    AppResourceSemaphore* threadResource = nullptr;
    AppResourceSemaphore* memResource = nullptr;
    AppResourceSemaphore* projectResource = nullptr;
    AppResourceReadWriteLock* listenLogInGTest = nullptr;
};

class U2CORE_EXPORT MemoryLocker {
public:
    MemoryLocker(U2OpStatus& os, int preLockMB = 10);

    MemoryLocker(int preLockMB = 10);

    virtual ~MemoryLocker();

    MemoryLocker(const MemoryLocker& other) = delete;

    MemoryLocker& operator=(const MemoryLocker& other) = delete;

    bool tryAcquire(qint64 bytes);

    void release();

    bool hasError() const;

    const QString& getError() const;

private:
    U2OpStatus* os = nullptr;
    int preLockMB = 0;
    int lockedMB = 0;
    qint64 needBytes = 0;
    AppResource* resource = nullptr;
    QString errorMessage;
};

}  // namespace U2
#endif
