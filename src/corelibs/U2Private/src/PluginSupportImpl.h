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

#ifndef _U2_PLUGINSUPPORT_IMPL_
#define _U2_PLUGINSUPPORT_IMPL_

#include <U2Core/global.h>
#include <U2Core/PluginModel.h>
#include <U2Core/Task.h>

#include <QLibrary>
#include <QDir>

#include "PluginDescriptor.h"
#include <QProcess>

namespace U2 {

class ServiceRegistryImpl;
class AddPluginTask;
class RemovePluginTask;

class PluginRef {
public:
    PluginRef(Plugin* _plugin, QLibrary* _library, const PluginDesc& desc);

    ~PluginRef();

    Plugin*     plugin;
    QLibrary*   library;
    PluginDesc  pluginDesc;
    bool        removeFlag;
};

class U2PRIVATE_EXPORT PluginSupportImpl : public PluginSupport {
    Q_OBJECT

    friend class AddPluginTask;

public:
    PluginSupportImpl();
    ~PluginSupportImpl();

    virtual const QList<Plugin*>& getPlugins() {return plugins;}

    virtual Task* addPluginTask(const QString& pathToPlugin);

    //plugin will not be removed from the plugin list during the next app run
    virtual void setRemoveFlag(Plugin* p, bool v);
    virtual bool getRemoveFlag(Plugin* p) const;

    virtual void setLicenseAccepted(Plugin* p);


    static bool isDefaultPluginsDir(const QString& url);
    static QDir getDefaultPluginsDir();

    PluginRef* findRef(Plugin* p) const;
    PluginRef* findRefById(const QString& pluginId) const;

    virtual bool isAllPluginsLoaded() const;

    bool allLoaded;

private slots:
    void sl_registerServices();

protected:
    void registerPlugin(PluginRef* ref);
    QString getPluginFileURL(Plugin* p) const;

    void updateSavedState(PluginRef* ref);
    static QSet<QString> getPluginPaths();

private:
    QList<PluginRef*>    plugRefs;
    QList<Plugin*>       plugins;
};

class VerifyPluginTask;
class AddPluginTask : public Task {
    Q_OBJECT
public:
    AddPluginTask(PluginSupportImpl* ps, const PluginDesc& desc, bool forceVerificatoin = false);
    void prepare();
    ReportResult report();
private:
    bool verifyPlugin();
    void instantiatePlugin();

    QScopedPointer<QLibrary> lib;
    PluginSupportImpl*  ps;
    PluginDesc          desc;
    bool                forceVerification;
    bool                verificationMode;
    VerifyPluginTask*   verifyTask;
};

class VerifyPluginTask : public Task {
    Q_OBJECT
public:
    VerifyPluginTask(PluginSupportImpl* ps, const PluginDesc& desc);
    void run();
    bool isCorrectPlugin() const{return pluginIsCorrect;}
    const PluginDesc& getPluginDescriptor() const{return desc;}
private:
    PluginSupportImpl*  ps;
    PluginDesc          desc;
    int                 timeOut;
    QProcess*           proc;
    bool                pluginIsCorrect;
};

class LoadAllPluginsTask : public Task {
    Q_OBJECT
public:
    LoadAllPluginsTask(PluginSupportImpl* ps,const QStringList& pluginFiles);
    void prepare();
    ReportResult report();


private:
    void addToOrderingQueue(const QString& url);

    PluginSupportImpl*  ps;
    QStringList         pluginFiles;
    QList<PluginDesc>   orderedPlugins; // plugins ordered by desc
};



}//namespace

#endif
