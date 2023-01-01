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

#include <QCoreApplication>
#include <QTranslator>

#include <U2Algorithm/AlignmentAlgorithmsRegistry.h>
#include <U2Algorithm/AssemblyConsensusAlgorithmRegistry.h>
#include <U2Algorithm/CDSearchTaskFactoryRegistry.h>
#include <U2Algorithm/DnaAssemblyAlgRegistry.h>
#include <U2Algorithm/GenomeAssemblyRegistry.h>
#include <U2Algorithm/MSAConsensusAlgorithmRegistry.h>
#include <U2Algorithm/MSADistanceAlgorithmRegistry.h>
#include <U2Algorithm/MolecularSurfaceFactoryRegistry.h>
#include <U2Algorithm/PWMConversionAlgorithmRegistry.h>
#include <U2Algorithm/PhyTreeGeneratorRegistry.h>
#include <U2Algorithm/RepeatFinderTaskFactoryRegistry.h>
#include <U2Algorithm/SWResultFilterRegistry.h>
#include <U2Algorithm/SecStructPredictAlgRegistry.h>
#include <U2Algorithm/SmithWatermanTaskFactoryRegistry.h>
#include <U2Algorithm/SplicedAlignmentTaskRegistry.h>
#include <U2Algorithm/StructuralAlignmentAlgorithmRegistry.h>
#include <U2Algorithm/SubstMatrixRegistry.h>

#include <U2Core/AnnotationSettings.h>
#include <U2Core/AppFileStorage.h>
#include <U2Core/CMDLineCoreOptions.h>
#include <U2Core/CMDLineRegistry.h>
#include <U2Core/CMDLineUtils.h>
#include <U2Core/ConsoleShutdownTask.h>
#include <U2Core/Counter.h>
#include <U2Core/DBXRefRegistry.h>
#include <U2Core/DNAAlphabetRegistryImpl.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/DataBaseRegistry.h>
#include <U2Core/DataPathRegistry.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/LoadRemoteDocumentTask.h>
#include <U2Core/Log.h>
#include <U2Core/PasswordStorage.h>
#include <U2Core/ResourceTracker.h>
#include <U2Core/ScriptingToolRegistry.h>
#include <U2Core/TaskStarter.h>
#include <U2Core/Timer.h>
#include <U2Core/TmpDirChecker.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/UdrSchemaRegistry.h>
#include <U2Core/UserApplicationsSettings.h>
#include <U2Core/Version.h>
#include <U2Core/VirtualFileSystem.h>

#include <U2Designer/DashboardInfoRegistry.h>

#include <U2Formats/ConvertFileTask.h>
#include <U2Formats/DocumentFormatUtils.h>

#include <U2Lang/LocalDomain.h>
#include <U2Lang/QueryDesignerRegistry.h>
#include <U2Lang/WorkflowEnvImpl.h>

#include <U2Test/GTestFrameworkComponents.h>
#include <U2Test/TestRunnerTask.h>

// U2Private
#include <AppContextImpl.h>
#include <AppSettingsImpl.h>
#include <ConsoleLogDriver.h>
#include <CredentialsAskerCli.h>
#include <DocumentFormatRegistryImpl.h>
#include <IOAdapterRegistryImpl.h>
#include <PluginSupportImpl.h>
#include <ServiceRegistryImpl.h>
#include <SettingsImpl.h>
#include <TaskSchedulerImpl.h>
#include <crash_handler/CrashHandler.h>

#include <U2Core/BundleInfo.h>

// local project imports
#include "DumpHelpTask.h"
#include "DumpLicenseTask.h"
#include "DumpVersionTask.h"
#include "TaskStatusBar.h"
#include "TestStarter.h"

#define TR_SETTINGS_ROOT QString("test_runner/")

using namespace U2;

static void registerCoreServices() {
    ServiceRegistry* sr = AppContext::getServiceRegistry();
    TaskScheduler* ts = AppContext::getTaskScheduler();
    Q_UNUSED(sr);
    Q_UNUSED(ts);
    // unlike ugene's UI Main.cpp we don't create PluginViewerImpl, ProjectViewImpl
}

static bool openDocs() {
    bool ret = false;
    QStringList suiteUrls = CMDLineRegistryUtils::getParameterValuesByWords(CMDLineCoreOptions::SUITE_URLS);
    if (!suiteUrls.empty()) {
        auto ts = new TestStarter(suiteUrls);

        GTestEnvironment* envs = ts->getEnv();
        envs->setVar(TIME_OUT_VAR, AppContext::getSettings()->getValue(TR_SETTINGS_ROOT + TIME_OUT_VAR, QString("0")).toString());
        envs->setVar(NUM_THREADS_VAR, AppContext::getSettings()->getValue(TR_SETTINGS_ROOT + NUM_THREADS_VAR, QString("5")).toString());

        QObject::connect(AppContext::getPluginSupport(), SIGNAL(si_allStartUpPluginsLoaded()), new TaskStarter(ts), SLOT(registerTask()));
        ret = true;
    }
    return ret;
}

static void setScriptsSearchPath() {
    QStringList scriptsSearchPath;
    const static char* RELATIVE_SCRIPTS_DIR = "/scripts";
    const static char* RELATIVE_DEV_SCRIPTS_DIR = "/../../scripts";

    QString appDirPath = AppContext::getWorkingDirectoryPath();
    if (QDir(appDirPath + RELATIVE_SCRIPTS_DIR).exists()) {
        scriptsSearchPath.push_back(appDirPath + RELATIVE_SCRIPTS_DIR);
    } else if (QDir(appDirPath + RELATIVE_DEV_SCRIPTS_DIR).exists()) {
        scriptsSearchPath.push_back(appDirPath + RELATIVE_DEV_SCRIPTS_DIR);
    }
    if (scriptsSearchPath.empty()) {
        scriptsSearchPath.push_back("/");
    }
    QDir::setSearchPaths(PATH_PREFIX_SCRIPTS, scriptsSearchPath);
}

static void setDataSearchPaths() {
    // set search paths for data files
    QStringList dataSearchPaths;
    const static char* RELATIVE_DATA_DIR = "/data";
    const static char* RELATIVE_DEV_DATA_DIR = "/../../data";
    // on Windows data is normally located in the application folder
    QString appDirPath = AppContext::getWorkingDirectoryPath();

    if (QDir(appDirPath + RELATIVE_DATA_DIR).exists()) {
        dataSearchPaths.push_back(appDirPath + RELATIVE_DATA_DIR);
    } else if (QDir(appDirPath + RELATIVE_DEV_DATA_DIR).exists()) {  // data location for developers
        dataSearchPaths.push_back(appDirPath + RELATIVE_DEV_DATA_DIR);
#ifdef Q_OS_DARWIN
    } else {
        QString dir = BundleInfo::getDataSearchPath();
        if (!dir.isEmpty()) {
            dataSearchPaths.push_back(dir);
        }
#endif
    }

    if (dataSearchPaths.empty()) {
        dataSearchPaths.push_back("/");
    }

    QDir::setSearchPaths(PATH_PREFIX_DATA, dataSearchPaths);
    // now data files may be opened using QFile( "data:some_data_file" )
}

static void setSearchPaths() {
    setDataSearchPaths();
    setScriptsSearchPath();
}

int main(int argc, char** argv) {
    if (CrashHandler::isEnabled()) {
        CrashHandler::setupHandler();
    }

    if (qgetenv(ENV_SEND_CRASH_REPORTS) == "0") {
        CrashHandler::setSendCrashReports(false);
    }

    if (!Version::checkBuildAndRuntimeVersions()) {
        return -1;
    }

    if (argc == 1) {
        Version version = Version::appVersion();
        QByteArray versionText = version.toString().toUtf8();
        fprintf(stderr, "Console interface for Unipro UGENE v%s\n"
#ifdef Q_OS_WIN
                        "Hint: Use 'ugeneui.exe' command to run GUI version of UGENE\n"
                        "Hint: Run 'ugene --usage', 'ugene.exe --help' or 'ugene.exe --help=<sectionName>' for more information\n"
#else
                        "Hint: Use 'ugene -ui' command to run GUI version of UGENE\n"
                        "Hint: Run 'ugene --usage', 'ugene --help' or 'ugene --help=<sectionName>' for more information\n"
#endif
                ,
                versionText.constData());
        return -1;
    }

    GTIMER(c1, t1, "main()->QApp::exec");

    QCoreApplication app(argc, argv);

    AppContextImpl* appContext = AppContextImpl::getApplicationContext();
    appContext->setWorkingDirectoryPath(QCoreApplication::applicationDirPath());

    QCoreApplication::addLibraryPath(AppContext::getWorkingDirectoryPath());
    setSearchPaths();

    // parse all cmdline arguments
    auto cmdLineRegistry = new CMDLineRegistry(QCoreApplication::arguments());
    appContext->setCMDLineRegistry(cmdLineRegistry);

    // 1 create settings
    auto globalSettings = new SettingsImpl(QSettings::SystemScope);
    appContext->setGlobalSettings(globalSettings);

    auto settings = new SettingsImpl(QSettings::UserScope);
    appContext->setSettings(settings);

    AppSettings* appSettings = new AppSettingsImpl();
    appContext->setAppSettings(appSettings);

    UserAppsSettings* userAppSettings = AppContext::getAppSettings()->getUserAppsSettings();

    // Set translations if needed: use value in the settings or cmd-line parameter override.
    // The default case 'en' does not need any files: the values for this locale are hardcoded in the code.
    QTranslator translator;
    QStringList failedToLoadTranslatorFiles;  // List of translators file names tried but failed to load/not found.
    QStringList translationFileList = {
        "transl_" + cmdLineRegistry->getParameterValue(CMDLineCoreOptions::TRANSLATION),
        userAppSettings->getTranslationFile(),
        "transl_" + QLocale::system().name().left(2).toLower(),
        BundleInfo::getExtraTranslationSearchPath(cmdLineRegistry)};
    // Keep only valid entries.
    translationFileList.removeAll("");
    translationFileList.removeAll("transl_");
    translationFileList.removeDuplicates();
    // Use the first translation from the list that works.
    for (const QString& translationFile : qAsConst(translationFileList)) {
        if (translationFile == "transl_en" || translator.load(translationFile, AppContext::getWorkingDirectoryPath())) {
            break;
        }
        failedToLoadTranslatorFiles << translationFile;
    }
    if (!translator.isEmpty()) {
        QCoreApplication::installTranslator(&translator);
        GObjectTypes::initTypeTranslations();
    }

    // 2 create functional components of congene
    ConsoleLogDriver logs;
    Q_UNUSED(logs);
    coreLog.details(AppContextImpl::tr("UGENE initialization started"));
    for (const QString& fileName : failedToLoadTranslatorFiles) {
        coreLog.trace(QObject::tr("Translation file not found: %1").arg(fileName));
    }

    auto resTrack = new ResourceTracker();
    appContext->setResourceTracker(resTrack);

    auto ts = new TaskSchedulerImpl(appSettings->getAppResourcePool());
    appContext->setTaskScheduler(ts);

    auto asr = new AnnotationSettingsRegistry(DocumentFormatUtils::predefinedSettings());
    appContext->setAnnotationSettingsRegistry(asr);

    auto tf = new TestFramework();
    appContext->setTestFramework(tf);

    auto rfr = new RepeatFinderTaskFactoryRegistry();
    appContext->setRepeatFinderTaskFactoryRegistry(rfr);

    auto qpr = new QDActorPrototypeRegistry();
    appContext->setQDActorFactoryRegistry(qpr);

    CMDLineUtils::init();
    DumpLicenseTask::initHelp();
    DumpVersionTask::initHelp();

    auto phyreg = new PhyTreeGeneratorRegistry();
    appContext->setPhyTreeGeneratorRegistry(phyreg);

    // unlike ugene's main.cpp we don't create MainWindowImpl, AppSettingsGUI and GObjectViewFactoryRegistry

    auto str = new ScriptingToolRegistry();
    appContext->setScriptingToolRegistry(str);

    auto etr = new ExternalToolRegistry();
    appContext->setExternalToolRegistry(etr);

    auto schemaRegistry = new UdrSchemaRegistry();
    appContext->setUdrSchemaRegistry(schemaRegistry);

    auto dbiRegistry = new U2DbiRegistry();
    appContext->setDbiRegistry(dbiRegistry);

    auto dfr = new DocumentFormatRegistryImpl();
    appContext->setDocumentFormatRegistry(dfr);

    auto psp = new PluginSupportImpl();
    appContext->setPluginSupport(psp);

    auto sreg = new ServiceRegistryImpl();
    appContext->setServiceRegistry(sreg);

    auto io = new IOAdapterRegistryImpl();
    appContext->setIOAdapterRegistry(io);

    auto dtr = new DNATranslationRegistry();
    appContext->setDNATranslationRegistry(dtr);

    DNAAlphabetRegistry* dal = new DNAAlphabetRegistryImpl(dtr);
    appContext->setDNAAlphabetRegistry(dal);

    // unlike ugene's main.cpp we don't create ScriptManagerView, MsaColorSchemeRegistry
    auto dbxrr = new DBXRefRegistry();
    appContext->setDBXRefRegistry(dbxrr);

    auto msaConsReg = new MSAConsensusAlgorithmRegistry();
    appContext->setMSAConsensusAlgorithmRegistry(msaConsReg);

    auto msaDistReg = new MSADistanceAlgorithmRegistry();
    appContext->setMSADistanceAlgorithmRegistry(msaDistReg);

    auto assemblyConsReg = new AssemblyConsensusAlgorithmRegistry();
    appContext->setAssemblyConsensusAlgorithmRegistry(assemblyConsReg);

    auto pwmConvReg = new PWMConversionAlgorithmRegistry();
    appContext->setPWMConversionAlgorithmRegistry(pwmConvReg);

    auto smr = new SubstMatrixRegistry();
    appContext->setSubstMatrixRegistry(smr);

    auto swar = new SmithWatermanTaskFactoryRegistry();
    appContext->setSmithWatermanTaskFactoryRegistry(swar);

    auto msfr = new MolecularSurfaceFactoryRegistry();
    appContext->setMolecularSurfaceFactoryRegistry(msfr);

    auto swrfr = new SWResultFilterRegistry();
    appContext->setSWResultFilterRegistry(swrfr);

    auto sspar = new SecStructPredictAlgRegistry();
    appContext->setSecStructPedictAlgRegistry(sspar);

    auto pwr = new AlignmentAlgorithmsRegistry();
    appContext->setAlignmentAlgorithmsRegistry(pwr);

    auto rdc = new RecentlyDownloadedCache();
    appContext->setRecentlyDownloadedCache(rdc);

    auto vfsReg = new VirtualFileSystemRegistry();
    appContext->setVirtualFileSystemRegistry(vfsReg);

    auto dashboardInfoRegistry = new DashboardInfoRegistry;
    appContext->setDashboardInfoRegistry(dashboardInfoRegistry);

    Workflow::WorkflowEnv::init(new Workflow::WorkflowEnvImpl());
    Workflow::WorkflowEnv::getDomainRegistry()->registerEntry(new LocalWorkflow::LocalDomainFactory());

    auto assemblyReg = new DnaAssemblyAlgRegistry();
    appContext->setDnaAssemblyAlgRegistry(assemblyReg);

    auto genomeAssemblyReg = new GenomeAssemblyAlgRegistry();
    appContext->setGenomeAssemblyAlgRegistry(genomeAssemblyReg);

    auto dbr = new DataBaseRegistry();
    appContext->setDataBaseRegistry(dbr);

    auto cdsfr = new CDSearchFactoryRegistry();
    appContext->setCDSearchFactoryRegistry(cdsfr);

    auto saar = new StructuralAlignmentAlgorithmRegistry();
    appContext->setStructuralAlignmentAlgorithmRegistry(saar);

    auto convertFactoryRegistry = new ConvertFactoryRegistry();
    appContext->setConvertFactoryRegistry(convertFactoryRegistry);

    auto splicedAlignmentTaskRegistry = new SplicedAlignmentTaskRegistry();
    appContext->setSplicedAlignmentTaskRegistry(splicedAlignmentTaskRegistry);

    auto workflowScriptRegistry = new WorkflowScriptRegistry();
    appContext->setWorkflowScriptRegistry(workflowScriptRegistry);

    auto passwordStorage = new PasswordStorage();
    appContext->setPasswordStorage(passwordStorage);
    AppSettingsImpl::addPublicDbCredentials2Settings();

    CredentialsAsker* credentialsAsker = new CredentialsAskerCli();
    appContext->setCredentialsAsker(credentialsAsker);

    auto appFileStorage = new AppFileStorage();
    U2OpStatusImpl os;
    appFileStorage->init(os);
    if (os.hasError()) {
        coreLog.error(os.getError());
        delete appFileStorage;
    } else {
        appContext->setAppFileStorage(appFileStorage);
    }

    auto dpr = new U2DataPathRegistry();
    appContext->setDataPathRegistry(dpr);

    auto tsbc = new TaskStatusBarCon();

    // show help if needed
    bool showHelp = cmdLineRegistry->hasParameter(CMDLineCoreOptions::HELP) ||
                    cmdLineRegistry->hasParameter(CMDLineCoreOptions::USAGE) ||
                    cmdLineRegistry->hasParameter(CMDLineCoreOptions::HELP_SHORT);

    if (showHelp) {
        QObject::connect(psp, SIGNAL(si_allStartUpPluginsLoaded()), new TaskStarter(new DumpHelpTask()), SLOT(registerTask()));
    }

    bool showLicense = cmdLineRegistry->hasParameter(DumpLicenseTask::LICENSE_CMDLINE_OPTION);
    if (showLicense) {
        QObject::connect(psp, SIGNAL(si_allStartUpPluginsLoaded()), new TaskStarter(new DumpLicenseTask()), SLOT(registerTask()));
    }

    bool showVersion = cmdLineRegistry->hasParameter(DumpVersionTask::VERSION_CMDLINE_OPTION);
    showVersion = showVersion || cmdLineRegistry->hasParameter(DumpVersionTask::VERSION_CMDLINE_OPTION_SHORT);
    if (showVersion) {
        QObject::connect(psp, SIGNAL(si_allStartUpPluginsLoaded()), new TaskStarter(new DumpVersionTask()), SLOT(registerTask()));
    }

    if (cmdLineRegistry->hasParameter(CMDLineCoreOptions::DOWNLOAD_DIR)) {
        userAppSettings->setDownloadDirPath(FileAndDirectoryUtils::getAbsolutePath(cmdLineRegistry->getParameterValue(CMDLineCoreOptions::DOWNLOAD_DIR)));
    }
    if (cmdLineRegistry->hasParameter(CMDLineCoreOptions::CUSTOM_TOOLS_CONFIG_DIR)) {
        userAppSettings->setCustomToolsConfigsDirPath(FileAndDirectoryUtils::getAbsolutePath(cmdLineRegistry->getParameterValue(CMDLineCoreOptions::CUSTOM_TOOLS_CONFIG_DIR)));
    }
    if (cmdLineRegistry->hasParameter(CMDLineCoreOptions::TMP_DIR)) {
        userAppSettings->setUserTemporaryDirPath(FileAndDirectoryUtils::getAbsolutePath(cmdLineRegistry->getParameterValue(CMDLineCoreOptions::TMP_DIR)));
    }
    if (cmdLineRegistry->hasParameter(CMDLineCoreOptions::DEFAULT_DATA_DIR)) {
        userAppSettings->setDefaultDataDirPath(FileAndDirectoryUtils::getAbsolutePath(cmdLineRegistry->getParameterValue(CMDLineCoreOptions::DEFAULT_DATA_DIR)));
    }
    if (cmdLineRegistry->hasParameter(CMDLineCoreOptions::FILE_STORAGE_DIR)) {
        userAppSettings->setFileStorageDir(FileAndDirectoryUtils::getAbsolutePath(cmdLineRegistry->getParameterValue(CMDLineCoreOptions::FILE_STORAGE_DIR)));
    }

    if (!showHelp && !showLicense && !showVersion) {
        QObject::connect(psp, SIGNAL(si_allStartUpPluginsLoaded()), new TaskStarter(new TmpDirChecker()), SLOT(registerTask()));
    }

    openDocs();
    registerCoreServices();

    GCOUNTER(cvar, "ugenecl launch");

    // 3 run QT
    t1.stop();
    coreLog.info(AppContextImpl::tr("%1-bit version of UGENE started").arg(Version::appArchitecture));
    ConsoleShutdownTask watchQuit(&app);
    Q_UNUSED(watchQuit);
    int rc = QCoreApplication::exec();

    // 4 deallocate resources
    Workflow::WorkflowEnv::shutdown();

    delete tsbc;

    appContext->setCredentialsAsker(nullptr);
    delete credentialsAsker;

    appContext->setPasswordStorage(nullptr);
    delete passwordStorage;

    appContext->setDataPathRegistry(nullptr);
    delete dpr;

    appContext->setDashboardInfoRegistry(nullptr);
    delete dashboardInfoRegistry;

    appContext->setVirtualFileSystemRegistry(nullptr);
    delete vfsReg;

    appContext->setRecentlyDownloadedCache(nullptr);
    delete rdc;

    delete swrfr;
    appContext->setSWResultFilterRegistry(nullptr);

    delete swar;
    appContext->setSmithWatermanTaskFactoryRegistry(nullptr);

    delete msfr;
    appContext->setMolecularSurfaceFactoryRegistry(nullptr);

    delete smr;
    appContext->setSubstMatrixRegistry(nullptr);

    delete sreg;
    appContext->setServiceRegistry(nullptr);

    delete psp;
    appContext->setPluginSupport(nullptr);

    delete tf;
    appContext->setTestFramework(nullptr);

    delete dal;
    appContext->setDNAAlphabetRegistry(nullptr);

    delete dbxrr;
    appContext->setDBXRefRegistry(nullptr);

    delete dtr;
    appContext->setDNATranslationRegistry(nullptr);

    delete io;
    appContext->setIOAdapterRegistry(nullptr);

    delete dfr;
    appContext->setDocumentFormatRegistry(nullptr);

    delete dbiRegistry;
    appContext->setDbiRegistry(nullptr);

    delete schemaRegistry;
    appContext->setUdrSchemaRegistry(nullptr);

    delete ts;
    appContext->setTaskScheduler(nullptr);

    delete asr;
    appContext->setAnnotationSettingsRegistry(nullptr);

    delete resTrack;
    appContext->setResourceTracker(nullptr);

    appContext->setAppSettings(nullptr);
    delete appSettings;

    delete settings;
    appContext->setSettings(nullptr);

    delete globalSettings;
    appContext->setGlobalSettings(nullptr);

    delete cmdLineRegistry;
    appContext->setCMDLineRegistry(nullptr);

    delete sspar;
    appContext->setSecStructPedictAlgRegistry(nullptr);

    appContext->setExternalToolRegistry(nullptr);
    delete etr;

    appContext->setScriptingToolRegistry(nullptr);
    delete str;

    delete msaConsReg;
    appContext->setMSAConsensusAlgorithmRegistry(nullptr);

    delete msaDistReg;
    appContext->setMSADistanceAlgorithmRegistry(nullptr);

    delete assemblyConsReg;
    appContext->setAssemblyConsensusAlgorithmRegistry(nullptr);

    appContext->setPWMConversionAlgorithmRegistry(nullptr);
    delete pwmConvReg;

    delete dbr;
    appContext->setDataBaseRegistry(nullptr);

    appContext->setQDActorFactoryRegistry(nullptr);
    delete qpr;

    delete cdsfr;
    appContext->setCDSearchFactoryRegistry(nullptr);

    appContext->setStructuralAlignmentAlgorithmRegistry(nullptr);
    delete saar;

    appContext->setConvertFactoryRegistry(nullptr);
    delete convertFactoryRegistry;

    delete splicedAlignmentTaskRegistry;
    appContext->setSplicedAlignmentTaskRegistry(nullptr);

    delete pwr;
    appContext->setAlignmentAlgorithmsRegistry(nullptr);

    CrashHandler::shutdown();

    return rc;
}
