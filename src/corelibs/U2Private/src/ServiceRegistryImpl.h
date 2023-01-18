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

#ifndef _U2_SERVICE_REGISTRY_IMPL_
#define _U2_SERVICE_REGISTRY_IMPL_

#include <QMap>

#include <U2Core/ServiceModel.h>
#include <U2Core/Task.h>
#include <U2Core/global.h>

namespace U2 {

class RegisterServiceTask;
class EnableServiceTask;
class UnregisterServiceTask;
class DisableServiceTask;

class U2PRIVATE_EXPORT ServiceRegistryImpl : public ServiceRegistry {
    Q_OBJECT

    friend class AbstractServiceTask;
    friend class RegisterServiceTask;
    friend class EnableServiceTask;
    friend class UnregisterServiceTask;
    friend class DisableServiceTask;

public:
    ServiceRegistryImpl() {
        timerIsActive = false;
    }
    ~ServiceRegistryImpl();

    /// Returns list of all registered services
    virtual const QList<Service*>& getServices() const {
        return services;
    }

    /// Finds service with the specified ServiceType
    QList<Service*> findServices(ServiceType t) const override;

    Task* registerServiceTask(Service* s) override;

    Task* unregisterServiceTask(Service* s) override;

    Task* enableServiceTask(Service* s) override;

    Task* disableServiceTask(Service* ) override;

    void unregisterPluginServices(Plugin* p);

private:
    void initiateServicesCheckTask();

    Service* findServiceReadyToEnable() const;

    void setServiceState(Service* s, ServiceState state);
    Task* createServiceEnablingTask(Service* s) {
        return _createServiceEnablingTask(s);
    }
    Task* createServiceDisablingTask(Service* s) {
        return _createServiceDisablingTask(s);
    }

protected:
    void timerEvent(QTimerEvent* event) override;

private:
    QList<Service*> services;
    bool timerIsActive;
    QList<Task*> activeServiceTasks;
};

class AbstractServiceTask : public Task {
    Q_OBJECT
protected:
    AbstractServiceTask(const QString& taskName, TaskFlags flag, ServiceRegistryImpl* _sr, Service* _s, bool lockServiceResource);

    ServiceRegistryImpl* sr;
    Service* s;
};

class RegisterServiceTask : public AbstractServiceTask {
    Q_OBJECT
public:
    RegisterServiceTask(ServiceRegistryImpl* sr, Service* s);

    void prepare() override;

    ReportResult report() override;
};

class EnableServiceTask : public AbstractServiceTask {
    Q_OBJECT
public:
    EnableServiceTask(ServiceRegistryImpl* sr, Service* s, bool lockServiceResource = true);

    void prepare() override;

    ReportResult report() override;
};

class UnregisterServiceTask : public AbstractServiceTask {
    Q_OBJECT
public:
    UnregisterServiceTask(ServiceRegistryImpl* sr, Service* s);

    ReportResult report() override;

    void prepare() override;
};

class DisableServiceTask : public AbstractServiceTask {
    Q_OBJECT
public:
    DisableServiceTask(ServiceRegistryImpl* sr, Service* s, bool manual, bool lockServiceResource = true);

    ReportResult report() override;

    void prepare() override;

private:
    bool manual;

    bool isGUITesting() const;
};

}  // namespace U2

#endif
