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

#ifndef _U2_SERVICE_MODEL_H_
#define _U2_SERVICE_MODEL_H_

#include <U2Core/global.h>

#include <QString>
#include <QList>
#include <QPointer>

namespace U2 {

class Task;
class Plugin;
class Service;

enum ServiceState {
    ServiceState_Enabled,
    ServiceState_Disabled_New,
    ServiceState_Disabled_Manually,
    ServiceState_Disabled_ParentDisabled,
    ServiceState_Disabled_CircularDependency,
    ServiceState_Disabled_FailedToStart
};

enum ServiceFlag {
    ServiceFlag_None        = 0, //no flags
    ServiceFlag_Singleton   = 1, //only one service with this id can be registered
};

typedef QFlags<ServiceFlag> ServiceFlags;

class U2CORE_EXPORT Service : public QObject {
    friend class ServiceRegistry;
    friend class ServiceLock;
    Q_OBJECT
public:
    Service(ServiceType t, const QString& _name, const QString& _desc,
        const QList<ServiceType>& parentServices = QList<ServiceType>(),
        ServiceFlags flags = ServiceFlag_None);

    const QList<ServiceType>& getParentServiceTypes() const {return parentServices;}

    ServiceType getType() const {return type;}

    const QString& getName() const {return name;}

    const QString& getDescription() const {return description;}

    ServiceState getState() const {return state;}

    bool isDisabled() const {return state!=ServiceState_Enabled;}

    bool isEnabled() const {return !isDisabled();}

    ServiceFlags getFlags() const {return flags;}

protected:
    /// returns NULL if no actions are required to enable service
    virtual Task* createServiceEnablingTask() {return NULL;}

    /// returns NULL if no actions are required to disable service
    virtual Task* createServiceDisablingTask() {return NULL;}

    virtual void serviceStateChangedCallback(ServiceState oldState, bool enabledStateChanged) { Q_UNUSED(oldState); Q_UNUSED(enabledStateChanged);}

private:
    ServiceType         type;
    QString             name;
    QString             description;
    QList<ServiceType>  parentServices;
    ServiceState        state;
    ServiceFlags        flags;

};

//////////////////////////////////////////////////////////////////////////
// service registry

class U2CORE_EXPORT ServiceRegistry : public QObject {
    Q_OBJECT
public:

    /// Returns list of all registered services
    virtual const QList<Service*>& getServices() const = 0;

    /// Finds service with the specified ServiceType
    virtual QList<Service*> findServices(ServiceType t) const = 0;

    /// registers the service in a registry and initiates enabling task if all parent services are enabled
    /// if service started successfully - checks all child services if new services can be started
    virtual Task* registerServiceTask(Service* s, bool lockServiceResource = true) = 0;

    /// unregisters the service in a registry.
    /// if the service is enabled -> initiates service disabling task first
    virtual Task* unregisterServiceTask(Service* s, bool lockServiceResource = true) = 0;

    virtual Task* enableServiceTask(Service* s, bool lockServiceResource = true) = 0;

    virtual Task* disableServiceTask(Service* s, bool lockServiceResource = true) = 0;

protected:
    void _setServiceState(Service* s, ServiceState state);
    Task* _createServiceEnablingTask(Service* s) {return s->createServiceEnablingTask();}
    Task* _createServiceDisablingTask(Service* s) {return s->createServiceDisablingTask();}


signals:
    void si_serviceUnregistered(Service* s);
    void si_serviceStateChanged(Service* s, ServiceState oldState);
};


}//namespace

#endif
