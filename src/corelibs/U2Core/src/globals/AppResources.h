/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <U2Core/global.h>
#include <U2Core/U2SafePoints.h>
#include <QHash>
#include <QSemaphore>
#include <QReadWriteLock>
#include <U2Core/U2OpStatus.h>

namespace U2 {

/** Thread resource - number of threads */
#define RESOURCE_THREAD     1

/** Memory resource - amount of memory in megabytes */
#define RESOURCE_MEMORY     2

/** CUDA GPU resource - ensures that device is busy or not*/
#define RESOURCE_CUDA_GPU   3

/** OPENCL GPU resource - ensures that device is busy or not*/
#define RESOURCE_OPENCL_GPU 4

/**
    Project resource. There is 1 project active in the system and if the resource is locked
    no project load/unload operation is possible
*/
#define RESOURCE_PROJECT    5

#define LOG_TRACE(METHOD) \
    coreLog.trace(QString("AppResource %1 ::" #METHOD " %2, available %3").arg(name).arg(n).arg(available()));

class U2CORE_EXPORT AppResource {
public:
    // TaskMemory will be released after Task is finished,
    // SystemMemory will be released only on UGENE shutdown. Example: SQLite buffer memory lock
    enum MemoryLockType {
        TaskMemory,
        SystemMemory
    };

    AppResource(int id, int _maxUse, const QString& _name, const QString& _suffix = QString())
        : name(_name), suffix(_suffix), resourceId(id), _maxUse(_maxUse), systemUse(0) {}

    virtual ~AppResource(){}

    virtual void acquire(int n = 1, MemoryLockType lt = TaskMemory) = 0;
    virtual bool tryAcquire(int n = 1, MemoryLockType lt = TaskMemory) = 0;

    virtual bool tryAcquire(int n, int timeout, MemoryLockType lt = TaskMemory) = 0;

    virtual void release(int n = 1, MemoryLockType lt = TaskMemory) = 0;

    virtual int available() const = 0;

    // total maximum usage of resource
    int maxUse() const { return _maxUse; }

    // total maximum usage of resource without system memory
    int maxTaskUse() const {
        SAFE_POINT_EXT(systemUse >= 0,,_maxUse);
        return _maxUse - systemUse;
    }
    int getResourceId() const {
        return resourceId;
    }

    QString name;
    QString suffix;

protected:
    int resourceId;
    int _maxUse;
    int systemUse;

private:
    AppResource( const AppResource& other );
    AppResource& operator= (const AppResource& other);
};

class U2CORE_EXPORT AppResourceReadWriteLock : public AppResource {
public:
    AppResourceReadWriteLock(int id, const QString& _name, const QString& _suffix = QString())
        : AppResource(id, Write, _name, _suffix), resource(NULL) {
        resource = new QReadWriteLock;
    }

    virtual ~AppResourceReadWriteLock() {
        delete resource; resource = NULL;
    }

    enum UseType {
        Read,
        Write
    };

    // for TaskScheduler
    virtual void acquire(int n, MemoryLockType lt = TaskMemory) {
        Q_UNUSED(lt);
        switch (n) {
            case Read:
                resource->lockForRead();
                break;
            case Write:
                resource->lockForWrite();
                break;
            default:
                break;
        }
    }

    virtual bool tryAcquire(int n, MemoryLockType lt = TaskMemory) {
        Q_UNUSED(lt);
        switch (n) {
            case Read:
                return resource->tryLockForRead();
            case Write:
                return resource->tryLockForWrite();
            default:
                return false;
        }
    }
    virtual bool tryAcquire(int n, int timeout, MemoryLockType lt = TaskMemory) {
        Q_UNUSED(lt);
        switch (n) {
            case Read:
                return resource->tryLockForRead(timeout);
            case Write:
                return resource->tryLockForWrite(timeout);
            default:
                return false;
        }
    }

    virtual void release(int n, MemoryLockType lt = TaskMemory) {
        Q_UNUSED(n);
        Q_UNUSED(lt);
        resource->unlock();
    }

    virtual int available() const {
        return -1;
    }

private:
    QReadWriteLock* resource;
};

class U2CORE_EXPORT AppResourceSemaphore : public AppResource {
public:
    AppResourceSemaphore(int id, int _maxUse, const QString& _name, const QString& _suffix = QString())
        : AppResource(id, _maxUse, _name, _suffix), resource(NULL) {
            resource = new QSemaphore(_maxUse);
    }
    virtual ~AppResourceSemaphore(){
        delete resource; resource = NULL;
    }

    void acquire(int n = 1, MemoryLockType lt = TaskMemory) {
        LOG_TRACE(acquire);
        resource->acquire(n);
        if (lt == AppResource::SystemMemory) {
            systemUse += n;
        }
    }

    bool tryAcquire(int n = 1, MemoryLockType lt = TaskMemory) {
        LOG_TRACE(tryAcquire);
        bool acquired = resource->tryAcquire(n);
        if (acquired && (lt == AppResource::SystemMemory)) {
            systemUse += n;
        }
        return acquired;
    }

    bool tryAcquire(int n, int timeout, MemoryLockType lt = TaskMemory) {
        LOG_TRACE(tryAcquire_timeout);
        bool acquired = resource->tryAcquire(n, timeout);
        if (acquired && (lt == AppResource::SystemMemory)) {
            systemUse += n;
        }
        return acquired;
    }

    void release(int n = 1, MemoryLockType lt = TaskMemory) {
        LOG_TRACE(release);
        SAFE_POINT(n>=0, QString("AppResource %1 release %2 < 0 called").arg(name).arg(n), );
        resource->release(n);
        if (lt == AppResource::SystemMemory) {
            systemUse -= n;
            SAFE_POINT_EXT(systemUse >= 0,,);
        }

        // QSemaphore allow to create resources by releasing, we do not want to get such behavior
        int avail = resource->available();
        SAFE_POINT_EXT(avail <= _maxUse,,);
    }

    int available() const {
        return resource->available();
    }

    void setMaxUse (int n) {
        LOG_TRACE(setMaxUse);
        int diff = n - _maxUse;
        if (diff > 0) {
            // adding resources
            resource->release(diff);
            _maxUse += diff;
        } else {
            diff = -diff;
            // safely remove resources
            for (int i=diff; i>0; i--) {
                bool ok = resource->tryAcquire(i, 0);
                if (ok) {
                    // successfully acquired i resources
                    _maxUse -= i;
                    if (_maxUse < systemUse) {
                        resource->release(systemUse - _maxUse);
                        _maxUse = systemUse;
                    }
                    break;
                }
            }
        }
    }

private:
    QSemaphore *resource;
};

#define MIN_MEMORY_SIZE 200

class U2CORE_EXPORT AppResourcePool : public QObject {
    Q_OBJECT
public:
    AppResourcePool();
    virtual ~AppResourcePool();

    int getIdealThreadCount() const {return idealThreadCount;}
    void setIdealThreadCount(int n);

    int getMaxThreadCount() const {return threadResource->maxUse();}
    void setMaxThreadCount(int n);

    int getMaxMemorySizeInMB() const {return memResource->maxUse();}
    void setMaxMemorySizeInMB(int m);

    static size_t getCurrentAppMemory();

    static bool isSSE2Enabled();

    void registerResource(AppResource* r);
    void unregisterResource(int id);

    AppResource* getResource(int id) const;

    static AppResourcePool* instance();

    static int getTotalPhysicalMemory();

    static bool isSystem64bit();

    static const int x32MaxMemoryLimitMb = 3*512;       // 1536Mb
    static const int x64MaxMemoryLimitMb = 2*1024*1024; // 2Tb
private:
    static const int defaultMemoryLimitMb = 8*1024;

    QHash<int, AppResource*> resources;

    int idealThreadCount;

    AppResourceSemaphore* threadResource;
    AppResourceSemaphore* memResource;
    AppResourceSemaphore* projectResouce;
    AppResourceReadWriteLock* listenLogInGTest;
};


class MemoryLocker {
public:
    MemoryLocker(U2OpStatus& os, int preLockMB = 10, AppResource::MemoryLockType memoryLockType = AppResource::TaskMemory)
        : os(&os),
          preLockMB(preLockMB > 0 ? preLockMB : 0),
          lockedMB(0),
          needBytes(0),
          resource(NULL),
          memoryLockType(memoryLockType)
    {
        resource = AppResourcePool::instance()->getResource(RESOURCE_MEMORY);
        tryAcquire(0);
    }

    MemoryLocker(int preLockMB = 10, AppResource::MemoryLockType memoryLockType = AppResource::TaskMemory)
        : os(NULL),
          preLockMB(preLockMB > 0 ? preLockMB : 0),
          lockedMB(0),
          needBytes(0),
          resource(NULL),
          memoryLockType(memoryLockType)
    {
        resource = AppResourcePool::instance()->getResource(RESOURCE_MEMORY);
        tryAcquire(0);
    }

    MemoryLocker(MemoryLocker& other) {
        resource = other.resource;
        memoryLockType = other.memoryLockType;
        os = NULL;
        preLockMB = other.preLockMB;
        lockedMB = other.lockedMB;
        other.lockedMB = 0;
        needBytes = other.needBytes;
        other.needBytes = 0;
        resource = NULL;
        errorMessage = "";
    }

    MemoryLocker& operator=(MemoryLocker& other);

    virtual ~MemoryLocker() {
        release();
    }

    bool tryAcquire(qint64 bytes) {
        needBytes += bytes;

        int needMB = needBytes/(1000*1000) + preLockMB;
        if (needMB > lockedMB) {
            int diff = needMB - lockedMB;
            CHECK_EXT(NULL != resource, if (os) os->setError("MemoryLocker - Resource error"), false);
            bool ok = resource->tryAcquire(diff, memoryLockType);
            if (ok) {
                lockedMB = needMB;
            } else {
                errorMessage = QString("MemoryLocker - Not enough memory error, %1 megabytes are required").arg(needMB);
                if(NULL != os) {
                    os->setError(errorMessage);
                }
            }
            return ok;
        }
        return true;
    }

    void release() {
        CHECK_EXT(NULL != resource, if (os) os->setError("MemoryLocker - Resource error"), );
        if (lockedMB > 0) {
            resource->release(lockedMB, memoryLockType);
        }
        lockedMB = 0;
        needBytes = 0;
    }

    bool hasError(){return !errorMessage.isEmpty();}

    const QString& getError(){return errorMessage;}

private:
    U2OpStatus* os;
    int preLockMB;
    int lockedMB;
    qint64 needBytes;
    AppResource* resource;
    AppResource::MemoryLockType memoryLockType;
    QString errorMessage;
};


}//namespace
#endif
