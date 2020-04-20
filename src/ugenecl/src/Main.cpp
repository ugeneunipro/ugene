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

#include <QCoreApplication>
#include <QTranslator>

#include <U2Algorithm/AlignmentAlgorithmsRegistry.h>
#include <U2Algorithm/AssemblyConsensusAlgorithmRegistry.h>
#include <U2Algorithm/CDSearchTaskFactoryRegistry.h>
#include <U2Algorithm/CudaGpuRegistry.h>
#include <U2Algorithm/DnaAssemblyAlgRegistry.h>
#include <U2Algorithm/GenomeAssemblyRegistry.h>
#include <U2Algorithm/MSAConsensusAlgorithmRegistry.h>
#include <U2Algorithm/MSADistanceAlgorithmRegistry.h>
#include <U2Algorithm/MolecularSurfaceFactoryRegistry.h>
#include <U2Algorithm/OpenCLGpuRegistry.h>
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
#include <U2Core/GObjectTypes.h>
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
#include <U2Lang/WorkflowRunTask.h>

#include <U2Test/GTestFrameworkComponents.h>
#include <U2Test/TestRunnerTask.h>

//U2Private
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

// local project imports
#include "DumpHelpTask.h"
#include "DumpLicenseTask.h"
#include "DumpVersionTask.h"
#include "ForeverTask.h"
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
    QStringList suiteUrls = CMDLineRegistryUtils::getParameterValuesByWords( CMDLineCoreOptions::SUITE_URLS );
    if( suiteUrls.size() > 0 ) {
        TestStarter* ts=new TestStarter( suiteUrls );

        GTestEnvironment* envs=ts->getEnv();
        envs->setVar(TIME_OUT_VAR, AppContext::getSettings()->getValue(TR_SETTINGS_ROOT + TIME_OUT_VAR,QString("0")).toString());
        envs->setVar(NUM_THREADS_VAR, AppContext::getSettings()->getValue(TR_SETTINGS_ROOT + NUM_THREADS_VAR,QString("5")).toString());

        QObject::connect(AppContext::getPluginSupport(), SIGNAL(si_allStartUpPluginsLoaded()), new TaskStarter(ts), SLOT(registerTask()));
        ret = true;
    }
    return ret;
}

static void updateStaticTranslations() {
    GObjectTypes::initTypeTranslations();
}


static void setScriptsSearchPath() {
    QStringList scriptsSearchPath;
    const static char * RELATIVE_SCRIPTS_DIR = "/scripts";
    const static char * RELATIVE_DEV_SCRIPTS_DIR = "/../../scripts";

    QString appDirPath = AppContext::getWorkingDirectoryPath();
    if( QDir(appDirPath+RELATIVE_SCRIPTS_DIR).exists() ) {
        scriptsSearchPath.push_back( appDirPath+RELATIVE_SCRIPTS_DIR );
    } else if( QDir(appDirPath+RELATIVE_DEV_SCRIPTS_DIR).exists() ) {
        scriptsSearchPath.push_back( appDirPath+RELATIVE_DEV_SCRIPTS_DIR );
    }
    if( scriptsSearchPath.empty() ) {
        scriptsSearchPath.push_back("/");
    }
    QDir::setSearchPaths( PATH_PREFIX_SCRIPTS, scriptsSearchPath );
}

static void setDataSearchPaths() {
    //set search paths for data files
    QStringList dataSearchPaths;
    const static char * RELATIVE_DATA_DIR = "/data";
    const static char * RELATIVE_DEV_DATA_DIR = "/../../data";
    //on windows data is normally located in the application folder
    QString appDirPath = AppContext::getWorkingDirectoryPath();

    if( QDir(appDirPath+RELATIVE_DATA_DIR).exists() ) {
        dataSearchPaths.push_back( appDirPath+RELATIVE_DATA_DIR );
    } else if( QDir(appDirPath+RELATIVE_DEV_DATA_DIR).exists() ) {          //data location for developers
        printf("ADDED PATH %s \n", qPrintable(appDirPath+RELATIVE_DEV_DATA_DIR) );
        dataSearchPaths.push_back( appDirPath+RELATIVE_DEV_DATA_DIR );
    }

#if (defined(Q_OS_UNIX)) && defined( UGENE_DATA_DIR )
    //using folder which is set during installation process on linux
    QString ugene_data_dir( UGENE_DATA_DIR );
    if( QDir(ugene_data_dir).exists() ) {
        dataSearchPaths.push_back( QString(UGENE_DATA_DIR) );
    }
#endif

    if( dataSearchPaths.empty() ) {
        dataSearchPaths.push_back("/");
    }

    QDir::setSearchPaths( PATH_PREFIX_DATA, dataSearchPaths );
    //now data files may be opened using QFile( "data:some_data_file" )
}

static void setSearchPaths() {
    setDataSearchPaths();
    setScriptsSearchPath();
}

int main(int argc, char **argv)
{
    if (CrashHandler::isEnabled()) {
        CrashHandler::setupHandler();
    }

    if (qgetenv(ENV_SEND_CRASH_REPORTS) == "0") {
        CrashHandler::setSendCrashReports(false);
    }

    const char* build = QT_VERSION_STR, *runtime = qVersion();
    if (strcmp(build, runtime) > 0){
        printf("Installed Qt version must be %s or greater \r\n", QT_VERSION_STR);
        return -1;
    }

    if (argc == 1) {
        Version version = Version::appVersion();
        fprintf(stderr, "Console interface for Unipro UGENE v%s\n"
#ifdef Q_OS_WIN
            "Hint: Use 'ugeneui.exe' command to run GUI version of UGENE\n"
            "Hint: Run 'ugene --usage', 'ugene.exe --help' or 'ugene.exe --help=<sectionName>' for more information\n"
#else
            "Hint: Use 'ugene -ui' command to run GUI version of UGENE\n"
            "Hint: Run 'ugene --usage', 'ugene --help' or 'ugene --help=<sectionName>' for more information\n"
#endif
            , version.text.toLatin1().constData());
        return -1;
    }

    GTIMER(c1, t1, "main()->QApp::exec");

    QCoreApplication app(argc, argv);

    AppContextImpl* appContext = AppContextImpl::getApplicationContext();
    appContext->setWorkingDirectoryPath(QCoreApplication::applicationDirPath());

    QCoreApplication::addLibraryPath(AppContext::getWorkingDirectoryPath());
    QString devPluginsPath = QDir(AppContext::getWorkingDirectoryPath()+"/../../installer/windows").absolutePath();
    QCoreApplication::addLibraryPath(devPluginsPath); //dev version

    setSearchPaths();

    // parse all cmdline arguments
    CMDLineRegistry* cmdLineRegistry = new CMDLineRegistry(app.arguments());
    appContext->setCMDLineRegistry(cmdLineRegistry);

    //1 create settings
    SettingsImpl* globalSettings = new SettingsImpl(QSettings::SystemScope);
    appContext->setGlobalSettings(globalSettings);

    SettingsImpl * settings = new SettingsImpl( QSettings::UserScope );
    appContext->setSettings( settings );


    AppSettings* appSettings = new AppSettingsImpl();
    appContext->setAppSettings(appSettings);

    UserAppsSettings* userAppSettings = AppContext::getAppSettings()->getUserAppsSettings();

    // set translations
    QTranslator translator;
    QString cmdlineTransl = cmdLineRegistry->getParameterValue(CMDLineCoreOptions::TRANSLATION);
    cmdlineTransl = cmdlineTransl.isEmpty() ? cmdlineTransl : "transl_" + cmdlineTransl;
    QString transFile[] = {
        cmdlineTransl,
        userAppSettings->getTranslationFile(),
        "transl_" + QLocale::system().name().left(2),
        "transl_en"
    };
    bool trOK = false;
    bool skipFirst = transFile[0].isEmpty();
    bool skipSecond = transFile[1].isEmpty();
    for (int i = 0; i < 4; ++i) {
        if((i == 0 && skipFirst) || (i == 1 && skipSecond)) {
            continue;
        }
        if (!translator.load(transFile[i], AppContext::getWorkingDirectoryPath())) {
            fprintf(stderr, "Translation not found: %s\n", transFile[i].toLatin1().constData());
        } else {
            trOK = true;
            break;
        }
    }
    if (!trOK) {
        fprintf(stderr, "No translations found, exiting\n");
        return 1;
    }

    app.installTranslator(&translator);
    updateStaticTranslations();

    // 2 create functional components of congene
    ConsoleLogDriver logs;
    Q_UNUSED(logs);
    coreLog.details(AppContextImpl::tr("UGENE initialization started"));

    ResourceTracker* resTrack = new ResourceTracker();
    appContext->setResourceTracker(resTrack);

    TaskSchedulerImpl* ts = new TaskSchedulerImpl(appSettings->getAppResourcePool());
    appContext->setTaskScheduler(ts);

    AnnotationSettingsRegistry* asr = new AnnotationSettingsRegistry(DocumentFormatUtils::predefinedSettings());
    appContext->setAnnotationSettingsRegistry(asr);

    TestFramework* tf = new TestFramework();
    appContext->setTestFramework(tf);

    RepeatFinderTaskFactoryRegistry* rfr = new RepeatFinderTaskFactoryRegistry();
    appContext->setRepeatFinderTaskFactoryRegistry(rfr);

    QDActorPrototypeRegistry* qpr = new QDActorPrototypeRegistry();
    appContext->setQDActorFactoryRegistry(qpr);

    CMDLineUtils::init();
    DumpLicenseTask::initHelp();
    DumpVersionTask::initHelp();

    PhyTreeGeneratorRegistry* phyreg = new PhyTreeGeneratorRegistry();
    appContext->setPhyTreeGeneratorRegistry(phyreg);

    // unlike ugene's main.cpp we don't create MainWindowImpl, AppSettingsGUI and GObjectViewFactoryRegistry

    ScriptingToolRegistry* str = new ScriptingToolRegistry();
    appContext->setScriptingToolRegistry(str);

    ExternalToolRegistry* etr = new ExternalToolRegistry();
    appContext->setExternalToolRegistry(etr);

    UdrSchemaRegistry *schemaRegistry = new UdrSchemaRegistry();
    appContext->setUdrSchemaRegistry(schemaRegistry);

    U2DbiRegistry *dbiRegistry = new U2DbiRegistry();
    appContext->setDbiRegistry(dbiRegistry);

    DocumentFormatRegistryImpl* dfr = new DocumentFormatRegistryImpl();
    appContext->setDocumentFormatRegistry(dfr);

    PluginSupportImpl* psp = new PluginSupportImpl();
    appContext->setPluginSupport(psp);

    ServiceRegistryImpl* sreg = new ServiceRegistryImpl() ;
    appContext->setServiceRegistry(sreg);

    IOAdapterRegistryImpl* io = new IOAdapterRegistryImpl();
    appContext->setIOAdapterRegistry(io);

    DNATranslationRegistry* dtr = new DNATranslationRegistry();
    appContext->setDNATranslationRegistry(dtr);

    DNAAlphabetRegistry* dal = new DNAAlphabetRegistryImpl(dtr);
    appContext->setDNAAlphabetRegistry(dal);

    // unlike ugene's main.cpp we don't create ScriptManagerView, MsaColorSchemeRegistry
    DBXRefRegistry *dbxrr = new DBXRefRegistry();
    appContext->setDBXRefRegistry(dbxrr);

    MSAConsensusAlgorithmRegistry* msaConsReg = new MSAConsensusAlgorithmRegistry();
    appContext->setMSAConsensusAlgorithmRegistry(msaConsReg);

    MSADistanceAlgorithmRegistry* msaDistReg = new MSADistanceAlgorithmRegistry();
    appContext->setMSADistanceAlgorithmRegistry(msaDistReg);

    AssemblyConsensusAlgorithmRegistry* assemblyConsReg = new AssemblyConsensusAlgorithmRegistry();
    appContext->setAssemblyConsensusAlgorithmRegistry(assemblyConsReg);

    PWMConversionAlgorithmRegistry* pwmConvReg = new PWMConversionAlgorithmRegistry();
    appContext->setPWMConversionAlgorithmRegistry(pwmConvReg);

    SubstMatrixRegistry* smr = new SubstMatrixRegistry();
    appContext->setSubstMatrixRegistry(smr);

    SmithWatermanTaskFactoryRegistry* swar = new SmithWatermanTaskFactoryRegistry();
    appContext->setSmithWatermanTaskFactoryRegistry(swar);

    MolecularSurfaceFactoryRegistry* msfr = new MolecularSurfaceFactoryRegistry();
    appContext->setMolecularSurfaceFactoryRegistry(msfr);

    SWResultFilterRegistry* swrfr = new SWResultFilterRegistry();
    appContext->setSWResultFilterRegistry(swrfr);

    SecStructPredictAlgRegistry* sspar = new SecStructPredictAlgRegistry();
    appContext->setSecStructPedictAlgRegistry(sspar);

    CudaGpuRegistry * cgr = new CudaGpuRegistry();
    appContext->setCudaGpuRegistry( cgr );

    AlignmentAlgorithmsRegistry *pwr = new AlignmentAlgorithmsRegistry();
    appContext->setAlignmentAlgorithmsRegistry(pwr);

#ifdef OPENCL_SUPPORT
    OpenCLGpuRegistry * oclgr = new OpenCLGpuRegistry();
    appContext->setOpenCLGpuRegistry( oclgr );
#endif

    RecentlyDownloadedCache* rdc = new RecentlyDownloadedCache();
    appContext->setRecentlyDownloadedCache(rdc);

    VirtualFileSystemRegistry * vfsReg = new VirtualFileSystemRegistry();
    appContext->setVirtualFileSystemRegistry( vfsReg );

    DashboardInfoRegistry *dashboardInfoRegistry = new DashboardInfoRegistry;
    appContext->setDashboardInfoRegistry(dashboardInfoRegistry);

    Workflow::WorkflowEnv::init(new Workflow::WorkflowEnvImpl());
    Workflow::WorkflowEnv::getDomainRegistry()->registerEntry(new LocalWorkflow::LocalDomainFactory());

    DnaAssemblyAlgRegistry* assemblyReg = new DnaAssemblyAlgRegistry();
    appContext->setDnaAssemblyAlgRegistry(assemblyReg);

    GenomeAssemblyAlgRegistry* genomeAssemblyReg = new GenomeAssemblyAlgRegistry();
    appContext->setGenomeAssemblyAlgRegistry(genomeAssemblyReg);

    DataBaseRegistry* dbr = new DataBaseRegistry();
    appContext->setDataBaseRegistry(dbr);

    CDSearchFactoryRegistry* cdsfr = new CDSearchFactoryRegistry();
    appContext->setCDSearchFactoryRegistry(cdsfr);

    StructuralAlignmentAlgorithmRegistry *saar = new StructuralAlignmentAlgorithmRegistry();
    appContext->setStructuralAlignmentAlgorithmRegistry(saar);

    ConvertFactoryRegistry* convertFactoryRegistry = new ConvertFactoryRegistry();
    appContext->setConvertFactoryRegistry(convertFactoryRegistry);

    SplicedAlignmentTaskRegistry* splicedAlignmentTaskRegistry = new SplicedAlignmentTaskRegistry();
    appContext->setSplicedAlignmentTaskRegistry(splicedAlignmentTaskRegistry);

    WorkflowScriptRegistry* workflowScriptRegistry = new WorkflowScriptRegistry();
    appContext->setWorkflowScriptRegistry(workflowScriptRegistry);

    PasswordStorage* passwordStorage = new PasswordStorage();
    appContext->setPasswordStorage(passwordStorage);
    AppSettingsImpl::addPublicDbCredentials2Settings();

    CredentialsAsker* credentialsAsker = new CredentialsAskerCli();
    appContext->setCredentialsAsker(credentialsAsker);

    AppFileStorage *appFileStorage = new AppFileStorage();
    U2OpStatusImpl os;
    appFileStorage->init(os);
    if (os.hasError()) {
        coreLog.error(os.getError());
        delete appFileStorage;
    } else {
        appContext->setAppFileStorage(appFileStorage);
    }

    U2DataPathRegistry* dpr = new U2DataPathRegistry();
    appContext->setDataPathRegistry(dpr);

    TaskStatusBarCon* tsbc=new TaskStatusBarCon();

    // show help if need
    bool showHelp = cmdLineRegistry->hasParameter(CMDLineCoreOptions::HELP) ||
        cmdLineRegistry->hasParameter(CMDLineCoreOptions::USAGE) ||
        cmdLineRegistry->hasParameter(CMDLineCoreOptions::HELP_SHORT);

    if( showHelp ) {
        QObject::connect( psp, SIGNAL( si_allStartUpPluginsLoaded()), new TaskStarter(new DumpHelpTask()), SLOT(registerTask()));
    }

    bool showLicense = cmdLineRegistry->hasParameter(DumpLicenseTask::LICENSE_CMDLINE_OPTION);
    if( showLicense ) {
        QObject::connect(psp, SIGNAL(si_allStartUpPluginsLoaded()), new TaskStarter(new DumpLicenseTask()), SLOT(registerTask()));
    }

    bool showVersion = cmdLineRegistry->hasParameter(DumpVersionTask::VERSION_CMDLINE_OPTION);
    showVersion = showVersion || cmdLineRegistry->hasParameter(DumpVersionTask::VERSION_CMDLINE_OPTION_SHORT);
    if(showVersion) {
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

    GReportableCounter launchCounter("ugenecl launch", "", 1);
    ++launchCounter.totalCount;

    //3 run QT
    t1.stop();
    coreLog.info(AppContextImpl::tr("%1-bit version of UGENE started").arg(Version::appArchitecture));
    ConsoleShutdownTask watchQuit(&app);
    Q_UNUSED(watchQuit);
    int rc = app.exec();

    //4 deallocate resources
    Workflow::WorkflowEnv::shutdown();

    delete tsbc;

    appContext->setCredentialsAsker(NULL);
    delete credentialsAsker;

    appContext->setPasswordStorage(NULL);
    delete passwordStorage;

    appContext->setDataPathRegistry(NULL);
    delete dpr;

    appContext->setDashboardInfoRegistry(nullptr);
    delete dashboardInfoRegistry;

    appContext->setVirtualFileSystemRegistry( NULL );
    delete vfsReg;

    appContext->setRecentlyDownloadedCache(NULL);
    delete rdc;

    delete swrfr;
    appContext->setSWResultFilterRegistry(NULL);

    delete swar;
    appContext->setSmithWatermanTaskFactoryRegistry(NULL);

    delete msfr;
    appContext->setMolecularSurfaceFactoryRegistry(NULL);

    delete smr;
    appContext->setSubstMatrixRegistry(NULL);

    delete sreg;
    appContext->setServiceRegistry(NULL);

    delete psp;
    appContext->setPluginSupport(NULL);

    delete tf;
    appContext->setTestFramework(0);

    delete dal;
    appContext->setDNAAlphabetRegistry(NULL);

    delete dbxrr;
    appContext->setDBXRefRegistry(0);

    delete dtr;
    appContext->setDNATranslationRegistry(NULL);

    delete io;
    appContext->setIOAdapterRegistry(NULL);

    delete dfr;
    appContext->setDocumentFormatRegistry(NULL);

    delete dbiRegistry;
    appContext->setDbiRegistry(NULL);

    delete schemaRegistry;
    appContext->setUdrSchemaRegistry(NULL);

    delete ts;
    appContext->setTaskScheduler(NULL);

    delete asr;
    appContext->setAnnotationSettingsRegistry(NULL);

    delete resTrack;
    appContext->setResourceTracker(NULL);

    delete cgr;
    appContext->setCudaGpuRegistry(NULL);

#ifdef OPENCL_SUPPORT
    delete oclgr;
#endif
    appContext->setOpenCLGpuRegistry(NULL);

    appContext->setAppSettings(NULL);
    delete appSettings;

    delete settings;
    appContext->setSettings(NULL);

    delete globalSettings;
    appContext->setGlobalSettings(NULL);

    delete cmdLineRegistry;
    appContext->setCMDLineRegistry(NULL);

    delete sspar;
    appContext->setSecStructPedictAlgRegistry(NULL);

    appContext->setExternalToolRegistry(NULL);
    delete etr;

    appContext->setScriptingToolRegistry(NULL);
    delete str;

    delete msaConsReg;
    appContext->setMSAConsensusAlgorithmRegistry(NULL);

    delete msaDistReg;
    appContext->setMSADistanceAlgorithmRegistry(NULL);

    delete assemblyConsReg;
    appContext->setAssemblyConsensusAlgorithmRegistry(NULL);

    appContext->setPWMConversionAlgorithmRegistry(NULL);
    delete pwmConvReg;

    delete dbr;
    appContext->setDataBaseRegistry(NULL);

    appContext->setQDActorFactoryRegistry(NULL);
    delete qpr;

    delete cdsfr;
    appContext->setCDSearchFactoryRegistry(NULL);

    appContext->setStructuralAlignmentAlgorithmRegistry(NULL);
    delete saar;

    appContext->setConvertFactoryRegistry(NULL);
    delete convertFactoryRegistry;

    delete splicedAlignmentTaskRegistry;
    appContext->setSplicedAlignmentTaskRegistry(NULL);

    delete pwr;
    appContext->setAlignmentAlgorithmsRegistry(NULL);

    CrashHandler::shutdown();

    return rc;
}
