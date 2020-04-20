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

#ifndef _U2_PLUGINMODEL_H_
#define _U2_PLUGINMODEL_H_

#include <QList>
#include <QObject>
#include <QString>

#include <U2Core/GUrl.h>

namespace U2 {
//BUG:417: add API version check

class Service;
class ServiceRegistry;
class Plugin;
class PluginSupport;
class Task;

#define U2_PLUGIN_INIT_FUNC ugene_plugin_init
#define U2_PLUGIN_INIT_FUNC_NAME "ugene_plugin_init"

#define U2_PLUGIN_VERIFY_FUNC ugene_plugin_verify
#define U2_PLUGIN_VERIFY_NAME "ugene_plugin_verify"

#define U2_PLUGIN_FAIL_MASSAGE_FUNC ugene_plugin_fail_message
#define U2_PLUGIN_FAIL_MASSAGE_NAME "ugene_plugin_fail_message"

typedef Plugin* (*PLUG_INIT_FUNC) ();
typedef bool(*PLUG_VERIFY_FUNC) ();
typedef QString * (*PLUG_FAIL_MESSAGE_FUNC) ();

enum PluginState {
    PluginState_Loaded,
    PluginState_FailedToLoad
};

class U2CORE_EXPORT Plugin : public QObject {
    Q_OBJECT
public:
    Plugin(const QString & _name, const QString& _desc, const bool _isFree = true, PluginState _state = PluginState_Loaded);

    //plugin is deallocated by plugin_support service when it's removed or on application shutting down
    virtual ~Plugin() {}

    const QString & getId() const;
    void setId(const QString &value);

    const QString& getName() const { return name; }

    const QString& getDescription() const { return description; }

    const GUrl& getLicensePath() const { return licensePath; }
    void setLicensePath(const QString& licensePath);

    PluginState getState() const { return state; }

    bool isFree() const { return isFreeValue; }
    bool isLicenseAccepted() const { return isLicenseAcceptedValue; }
    void acceptLicense();

    // returns list of services provided by the plugin
    // after plugin is loaded all services from this list are automatically registered
    const QList<Service*>& getServices() const { return services; }

protected:
    QString         id;
    QString         name, description;
    QList<Service*> services;
    bool            isFreeValue;
    bool            isLicenseAcceptedValue;
    PluginState     state;
    GUrl            licensePath;
};


class U2CORE_EXPORT PluginSupport : public QObject {
    friend class LoadAllPluginsTask;
    Q_OBJECT

public:
    virtual const QList<Plugin*>&   getPlugins() = 0;
    virtual void setLicenseAccepted(Plugin* p) = 0;
    virtual bool isAllPluginsLoaded() const = 0;

signals:
    void si_allStartUpPluginsLoaded();

};

}//namespace

#endif
