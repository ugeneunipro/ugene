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

#include "PluginSupportImpl.h"
#include "ServiceRegistryImpl.h"

#include <U2Core/AppContext.h>
#include <U2Core/CMDLineRegistry.h>
#include <U2Core/CmdlineTaskRunner.h>
#include <U2Core/Settings.h>
#include <U2Core/Log.h>
#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

#include <QCoreApplication>
#include <QLibrary>
#include <QDir>
#include <QSet>

#include <U2Gui/MainWindow.h>

#include <algorithm>

#ifdef Q_OS_WIN
#include <windows.h>
#endif


namespace U2 {

/* TRANSLATOR U2::PluginSupportImpl */
#define SKIP_LIST_SETTINGS QString("plugin_support/skip_list/")
#define PLUGINS_ACCEPTED_LICENSE_LIST QString("plugin_support/accepted_list/")
#define PLUGIN_VERIFICATION QString("plugin_support/verification/")

static QStringList findAllPluginsInDefaultPluginsDir();


PluginRef::PluginRef(Plugin* _plugin, QLibrary* _library, const PluginDesc& desc)
: plugin(_plugin), library(_library), pluginDesc(desc), removeFlag(false)
{
}

PluginSupportImpl::PluginSupportImpl(): allLoaded(false) {
    connect(this, SIGNAL(si_allStartUpPluginsLoaded()), SLOT(sl_registerServices()));

    Task* loadStartUpPlugins = new LoadAllPluginsTask(this, findAllPluginsInDefaultPluginsDir());
    AppContext::getTaskScheduler()->registerTopLevelTask(loadStartUpPlugins);
}


PluginSupportImpl::~PluginSupportImpl() {
    foreach(PluginRef* ref, plugRefs) {
        delete ref;
    }
}

bool PluginSupportImpl::isAllPluginsLoaded() const {
    return allLoaded;
}

LoadAllPluginsTask::LoadAllPluginsTask(PluginSupportImpl* _ps, const QStringList& _pluginFiles)
    : Task(tr("Loading start up plugins"), TaskFlag_NoRun),
      ps(_ps),
      pluginFiles(_pluginFiles)
{
    coreLog.trace("List of the plugins to be loaded:");
    foreach(const QString& path, pluginFiles) {
        coreLog.trace(path);
    }
    coreLog.trace("End of the list");

}
void LoadAllPluginsTask::prepare() {
    foreach (const QString& url, pluginFiles) {
        addToOrderingQueue(url);
    }

    QString err;
    orderedPlugins = PluginDescriptorHelper::orderPlugins(orderedPlugins, err);

    if (!err.isEmpty()) {
        setError(err);
        return;
    }

    foreach(const PluginDesc& desc, orderedPlugins) {
        addSubTask(new AddPluginTask(ps, desc));
    }
}

void LoadAllPluginsTask::addToOrderingQueue(const QString& url) {
    QFileInfo descFile(url);
    if (!descFile.exists()) {
        coreLog.trace(  tr("File not found: %1").arg(url) );
        return;
    }

    if (!descFile.isFile()) {
        coreLog.trace(  tr("Invalid file format: %1").arg(url) );
        return;
    }

    QString err;
    PluginDesc desc = PluginDescriptorHelper::readPluginDescriptor(url, err);
    if (!desc.isValid()) {
        assert(!err.isEmpty());
        coreLog.trace(err);
        return;
    }

    // now check plugin compatibility
    bool isUIMode = AppContext::getMainWindow() != NULL || AppContext::isGUIMode(); // isGUIMode - for pluginChecker!
    bool modeIsOk = false;
    if (isUIMode) {
        modeIsOk = desc.mode.testFlag(PluginMode_UI);
    } else {
        modeIsOk = desc.mode.testFlag(PluginMode_Console);
    }
    if (!modeIsOk) {
        coreLog.trace(QString("Plugin is inactive in the current mode: %1, skipping load").arg(desc.id));
        return;
    }

    // check version
    Version ugeneVersion = Version::appVersion();
    Version qtVersion = Version::qtVersion();
    if (ugeneVersion.debug != desc.pluginVersion.debug) {
        coreLog.trace(QString("Plugin debug/release mode is not matched with UGENE binaries: %1").arg(desc.id));
        return;
    }
    if (qtVersion < desc.qtVersion) {
        coreLog.trace(QString("Plugin was built with higher QT version: %1").arg(desc.id));
        return;
    }
    if (ugeneVersion != desc.ugeneVersion) {
        coreLog.trace(QString("Plugin was built with another UGENE version: %1, %2 vs %3").arg(desc.id).arg(desc.ugeneVersion.text).arg(ugeneVersion.text));
        return;
    }

    //check platform

    if (desc.platform.arch == PlatformArch_Unknown) {
        coreLog.trace(QString("Plugin platform arch is unknown: %1").arg(desc.id));
        return;
    }
    if (desc.platform.arch == PlatformArch_32 && QT_POINTER_SIZE != 4) {
        coreLog.trace(QString("Plugin was built on 32-bit platform: %1").arg(desc.id));
        return;
    }
    if (desc.platform.arch == PlatformArch_64 && QT_POINTER_SIZE != 8) {
        coreLog.trace(QString("Plugin was built on 64-bit platform: %1").arg(desc.id));
        return;
    }

    if (desc.platform.name == PlatformName_Unknown) {
        coreLog.trace(QString("Plugin platform name is unknown: %1").arg(desc.id));
        return;
    }

#if defined(Q_OS_WIN)
    if (desc.platform.name != PlatformName_Win) {
        coreLog.trace(QString("Plugin platform is not Windows: %1").arg(desc.id));
        return;
    }
#elif defined(Q_OS_MAC)
    if (desc.platform.name != PlatformName_Mac) {
        coreLog.trace(QString("Plugin platform is not Mac: %1").arg(desc.id));
        return;
    }
#else
    if (desc.platform.name != PlatformName_UnixNotMac) {
        coreLog.trace(QString("Plugin platform is not Unix/Linux: %1").arg(desc.id));
        return;
    }
#endif

    orderedPlugins.append(desc);
}





Task::ReportResult LoadAllPluginsTask::report()
{
    ps->allLoaded = true;
    emit ps->si_allStartUpPluginsLoaded();
    return ReportResult_Finished;
}

namespace {
    QStringList getCmdlinePlugins() {
        CMDLineRegistry *reg = AppContext::getCMDLineRegistry();
        if (reg->hasParameter(CMDLineRegistry::PLUGINS_ARG)) {
            QString pluginsToLoad = reg->getParameterValue(CMDLineRegistry::PLUGINS_ARG);
            return pluginsToLoad.split(";");
        }
        return QStringList();
    }
}

static QStringList findAllPluginsInDefaultPluginsDir() {
    QDir d = PluginSupportImpl::getDefaultPluginsDir();
    QStringList filter; filter << QString("*.") + PLUGIN_FILE_EXT;
    QStringList fileNames = d.entryList(filter, QDir::Readable | QDir::Files, QDir::NoSort);
    QStringList res;
    bool hasCmdlinePlugins = AppContext::getCMDLineRegistry()->hasParameter(CMDLineRegistry::PLUGINS_ARG);
    QStringList cmdlinePlugins = getCmdlinePlugins();
    foreach(const QString& name, fileNames) {
        GUrl filePath(d.absolutePath() + "/" + name);
        if (!hasCmdlinePlugins || cmdlinePlugins.contains(filePath.baseFileName())) {
            QString path = filePath.getURLString();
            res.append(path);
            coreLog.trace(QString("Found plugin candidate in default dir: %1").arg(path));
        }
    }
    return res;
}


PluginRef::~PluginRef() {
    assert(plugin!=NULL);
    delete plugin;
    plugin = NULL;
}

void PluginSupportImpl::sl_registerServices() {
    ServiceRegistry* sr = AppContext::getServiceRegistry();
    foreach(PluginRef* ref, plugRefs) {
        foreach(Service* s, ref->plugin->getServices()) {
            AppContext::getTaskScheduler()->registerTopLevelTask(sr->registerServiceTask(s));
        }
    }
}

void PluginSupportImpl::registerPlugin(PluginRef* ref) {
    plugRefs.push_back(ref);
    plugins.push_back(ref->plugin);
    updateSavedState(ref);
}


QString PluginSupportImpl::getPluginFileURL(Plugin* p) const {
    assert(plugins.size() == plugRefs.size());

    foreach(PluginRef* ref, plugRefs) {
        if (ref->plugin == p) {
            if (ref->library == NULL) {
                return "";
            }
            return ref->library->fileName();

        }
    }
    return QString::null;
}

PluginRef* PluginSupportImpl::findRef(Plugin* p) const {
    foreach(PluginRef* r, plugRefs) {
        if (r->plugin == p) {
            return r;
        }
    }
    return NULL;
}

PluginRef* PluginSupportImpl::findRefById(const QString& pluginId) const {
    foreach(PluginRef* r, plugRefs) {
        if (r->pluginDesc.id == pluginId) {
            return r;
        }
    }
    return NULL;
}

void PluginSupportImpl::setLicenseAccepted(Plugin *p){
    p->acceptLicense();
    PluginRef* r = findRef(p);
    assert(r!=NULL);
    updateSavedState(r);
}
void PluginSupportImpl::updateSavedState(PluginRef* ref) {
    if (ref->library == NULL) {
        // skip core plugin
        return;
    }
    Settings* settings = AppContext::getSettings();
    QString skipListSettingsDir = settings->toVersionKey(SKIP_LIST_SETTINGS);
    QString pluginAcceptedLicenseSettingsDir = settings->toVersionKey(PLUGINS_ACCEPTED_LICENSE_LIST);
    QString descUrl = ref->pluginDesc.descriptorUrl.getURLString();
    QString pluginId = ref->pluginDesc.id;
    if (ref->removeFlag) {
        //add to skip-list if auto-loaded
        if (isDefaultPluginsDir(descUrl)) {
            QStringList skipFiles = settings->getValue(skipListSettingsDir, QStringList()).toStringList();
            if (!skipFiles.contains(descUrl)) {
                skipFiles.append(descUrl);
                settings->setValue(skipListSettingsDir, skipFiles);
            }
        }
    } else {
        //remove from skip-list if present
        if (isDefaultPluginsDir(descUrl)) {
            QStringList skipFiles = settings->getValue(skipListSettingsDir, QStringList()).toStringList();
            if (skipFiles.removeOne(descUrl)) {
                settings->setValue(skipListSettingsDir, skipFiles);
            }
        }
    }

    if (!ref->plugin->isFree()){
        settings->setValue(pluginAcceptedLicenseSettingsDir + pluginId + "license",ref->plugin->isLicenseAccepted());
    }
}

QDir PluginSupportImpl::getDefaultPluginsDir() {
    return QDir(AppContext::getWorkingDirectoryPath() + "/plugins");
}

bool PluginSupportImpl::isDefaultPluginsDir(const QString& url) {
    QDir urlAbsDir = QFileInfo(url).absoluteDir();
    QDir plugsDir = getDefaultPluginsDir();
    return  urlAbsDir == plugsDir;
}

//////////////////////////////////////////////////////////////////////////
/// Tasks
AddPluginTask:: AddPluginTask(PluginSupportImpl* _ps, const PluginDesc& _desc, bool forceVerification)
    : Task(tr("Add plugin task: %1").arg(_desc.id), TaskFlag_NoRun),
      lib(NULL),
      ps(_ps),
      desc(_desc),
      forceVerification(forceVerification),
      verificationMode(false),
      verifyTask(NULL)
{
    CMDLineRegistry *reg = AppContext::getCMDLineRegistry();
    verificationMode = reg->hasParameter(CMDLineRegistry::VERIFY_ARG);
}

void AddPluginTask::prepare() {
    PluginRef* ref = ps->findRefById(desc.id);
    if (ref != NULL) {
        stateInfo.setError(  tr("Plugin is already loaded: %1").arg(desc.id) );
        return;
    }

    //check that plugin we depends on is already loaded
    foreach (const DependsInfo& di, desc.dependsList) {
        PluginRef* ref = ps->findRefById(di.id);
        if (ref == NULL) {
            stateInfo.setError(  tr("Plugin %1 depends on %2 which is not loaded").arg(desc.id).arg(di.id) );
            return;
        }
        if (ref->pluginDesc.pluginVersion < di.version) {
            stateInfo.setError(  tr("Plugin %1 depends on %2 which is available, but the version is too old").arg(desc.id).arg(di.id) );
            return;
        }
    }

    //load library
    QString libUrl = desc.libraryUrl.getURLString();
    lib.reset(new QLibrary(libUrl));
    bool loadOk = lib->load();

    if (!loadOk) {
        stateInfo.setError(  tr("Plugin loading error: %1, Error string %2").arg(libUrl).arg(lib->errorString()) );
        coreLog.error(stateInfo.getError());
        return;
    }

    Settings* settings = AppContext::getSettings();
    SAFE_POINT(settings != NULL, tr("Settings is NULL"), );
    QString checkVersion = settings->getValue(PLUGIN_VERIFICATION + desc.id, "").toString();

    bool verificationIsEnabled = true;
#ifdef Q_OS_MAC
    if (qgetenv(ENV_GUI_TEST).toInt() == 1) {
        verificationIsEnabled = false;
    }
#endif

    if (verificationIsEnabled) {
        PLUG_VERIFY_FUNC verify_func = PLUG_VERIFY_FUNC(lib->resolve(U2_PLUGIN_VERIFY_NAME));
        if (verify_func && !verificationMode && (checkVersion != Version::appVersion().text || forceVerification)) {
            verifyTask = new VerifyPluginTask(ps, desc);
            addSubTask(verifyTask);
        }
    }
}

Task::ReportResult AddPluginTask::report() {
    CHECK_OP(stateInfo, ReportResult_Finished);

    if (verifyPlugin()) {
        return ReportResult_Finished;
    }

    Settings* settings = AppContext::getSettings();
    settings->sync();
    QString skipFile = settings->getValue(settings->toVersionKey(SKIP_LIST_SETTINGS)+ desc.id, QString()).toString();
    if (skipFile == desc.descriptorUrl.getURLString()) {
        return ReportResult_Finished;
    }

    instantiatePlugin();
    return ReportResult_Finished;
}

bool AddPluginTask::verifyPlugin() {
    // verify plugin
    PLUG_VERIFY_FUNC verify_func = PLUG_VERIFY_FUNC(lib->resolve(U2_PLUGIN_VERIFY_NAME));
    if (verify_func && verificationMode) {
        if (!verify_func()) {
            // verification mode is exclusively for crash check!
            FAIL("Plugin is not verified!", true);
        }
    }

    // check if verification failed
    Settings* settings = AppContext::getSettings();
    QString libUrl = desc.libraryUrl.getURLString();
    PLUG_FAIL_MESSAGE_FUNC message_func = PLUG_FAIL_MESSAGE_FUNC(lib->resolve(U2_PLUGIN_FAIL_MASSAGE_NAME));
    if (!verificationMode && verifyTask != NULL) {
        settings->setValue(PLUGIN_VERIFICATION + desc.id, Version::appVersion().text);
        if (!verifyTask->isCorrectPlugin()) {
            settings->setValue(settings->toVersionKey(SKIP_LIST_SETTINGS) + desc.id, desc.descriptorUrl.getURLString());
            QString message = message_func ? *(QScopedPointer<QString>(message_func())) : tr("Plugin loading error: %1. Verification failed.").arg(libUrl);
            stateInfo.setError(message);
            MainWindow* mw = AppContext::getMainWindow();
            CHECK(mw != NULL, ReportResult_Finished);
            mw->addNotification(message, Warning_Not);
            return true;
        } else {
            QString skipFile = settings->getValue(settings->toVersionKey(SKIP_LIST_SETTINGS) + desc.id, QString()).toString();
            if (skipFile == desc.descriptorUrl.getURLString()) {
                settings->remove(settings->toVersionKey(SKIP_LIST_SETTINGS) + desc.id);
            }
        }
    }
    return false;
}

void AddPluginTask::instantiatePlugin() {
    PLUG_INIT_FUNC init_fn = PLUG_INIT_FUNC(lib->resolve(U2_PLUGIN_INIT_FUNC_NAME));
    QString libUrl = desc.libraryUrl.getURLString();
    if (!init_fn) {
        stateInfo.setError(tr("Plugin initialization routine was not found: %1").arg(libUrl));
        return;
    }

    Plugin* p = init_fn();
    if (p == NULL) {
        stateInfo.setError(tr("Plugin initialization failed: %1").arg(libUrl));
        return;
    }

    p->setId(desc.id);
    p->setLicensePath(desc.licenseUrl.getURLString());

    if (!p->isFree()) {
        QString versionAppendix = Version::buildDate;
        if (!Version::appVersion().isDevVersion) {
            versionAppendix.clear();
        } else {
            versionAppendix.replace(" ", ".");
            versionAppendix.append("-");
        }
        Settings* settings = AppContext::getSettings();
        QString pluginAcceptedLicenseSettingsDir = settings->toVersionKey(PLUGINS_ACCEPTED_LICENSE_LIST);
        if (settings->getValue(pluginAcceptedLicenseSettingsDir + versionAppendix + desc.id + "license", false).toBool()) {
            p->acceptLicense();
        }
    }

    PluginRef* ref = new PluginRef(p, lib.take(), desc);
    ps->registerPlugin(ref);
}

VerifyPluginTask::VerifyPluginTask(PluginSupportImpl* ps, const PluginDesc& desc)
: Task(tr("Verify plugin task: %1").arg(desc.id), TaskFlags(TaskFlag_ReportingIsSupported) | TaskFlag_ReportingIsEnabled), ps(ps), desc(desc), timeOut(100000), proc(NULL), pluginIsCorrect(false)
{
}
void VerifyPluginTask::run() {
    QString executableDir = AppContext::getWorkingDirectoryPath();
    QString pluginCheckerPath = executableDir + "/plugins_checker";
    if(Version::appVersion().debug) {
        pluginCheckerPath += 'd';
    }
    #ifdef Q_OS_WIN
        pluginCheckerPath += ".exe";
    #endif

    if(!QFileInfo(pluginCheckerPath).exists()) {
        coreLog.error(QString("Can not find file: \"%1\"").arg(pluginCheckerPath));
        return;
    }
    proc = new QProcess();
    proc->start(pluginCheckerPath, QStringList()
                << QString("--%1=%2").arg(CMDLineRegistry::PLUGINS_ARG).arg(desc.id)
                << "--" + CMDLineRegistry::VERIFY_ARG
                << QString("--ini-file=\"%1\"").arg(AppContext::getSettings()->fileName()));

    int elapsedTime = 0;
    while(!proc->waitForFinished(1000) && elapsedTime < timeOut) {
        if(isCanceled()) {
            CmdlineTaskRunner::killProcessTree(proc);
        }
        elapsedTime += 1000;
    }
    QString errorMessage = proc->readAllStandardError();
    // In the following check we removed ` && errorMessage.isEmpty()` check,
    // as we don't check the actual message
    // Moreover, there is a non-empy string on Windows 10 (printed by Qt):
    //     `Untested OS version Windows 10!`
    // See UTI-325:
    //     https://ugene.dev/tracker/browse/UTI-325?filter=14051
    // The issue was initiated by UGENE blog user.
    // See blog thread:
    //     http://ugene.net/forum/YaBB.pl?num=1569395370/2#2
    if (proc->exitStatus() == QProcess::NormalExit) {
        pluginIsCorrect = true;
    }
}
}//namespace
