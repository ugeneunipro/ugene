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

#include <qglobal.h>

#ifdef Q_OS_WIN
#    include <tchar.h>
#    include <windows.h>
#endif  // Q_OS_WIN

#ifdef Q_OS_DARWIN
#    include "app_settings/ResetSettingsMac.h"
#endif

#include <QApplication>
#include <QDesktopWidget>
#include <QIcon>
#include <QMessageBox>
#include <QStyleFactory>
#include <QTranslator>

#include <U2Algorithm/AlignmentAlgorithmsRegistry.h>
#include <U2Algorithm/AssemblyConsensusAlgorithmRegistry.h>
#include <U2Algorithm/CDSearchTaskFactoryRegistry.h>
#include <U2Algorithm/DnaAssemblyAlgRegistry.h>
#include <U2Algorithm/GenomeAssemblyRegistry.h>
#include <U2Algorithm/MSAConsensusAlgorithmRegistry.h>
#include <U2Algorithm/MSADistanceAlgorithmRegistry.h>
#include <U2Algorithm/MolecularSurfaceFactoryRegistry.h>
#include <U2Algorithm/MsaColorScheme.h>
#include <U2Algorithm/MsaHighlightingScheme.h>
#include <U2Algorithm/PWMConversionAlgorithmRegistry.h>
#include <U2Algorithm/PhyTreeGeneratorRegistry.h>
#include <U2Algorithm/RepeatFinderTaskFactoryRegistry.h>
#include <U2Algorithm/SWMulAlignResultNamesTagsRegistry.h>
#include <U2Algorithm/SWResultFilterRegistry.h>
#include <U2Algorithm/SecStructPredictAlgRegistry.h>
#include <U2Algorithm/SequenceContentFilterTask.h>
#include <U2Algorithm/SmithWatermanTaskFactoryRegistry.h>
#include <U2Algorithm/SplicedAlignmentTaskRegistry.h>
#include <U2Algorithm/StructuralAlignmentAlgorithmRegistry.h>
#include <U2Algorithm/SubstMatrixRegistry.h>

#include <U2Core/AnnotationSettings.h>
#include <U2Core/AppFileStorage.h>
#include <U2Core/AutoAnnotationsSupport.h>
#include <U2Core/CMDLineCoreOptions.h>
#include <U2Core/CMDLineRegistry.h>
#include <U2Core/CMDLineUtils.h>
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
#include <U2Core/LogCache.h>
#include <U2Core/PasswordStorage.h>
#include <U2Core/ProjectFilterTaskRegistry.h>
#include <U2Core/ResourceTracker.h>
#include <U2Core/ScriptingToolRegistry.h>
#include <U2Core/TaskStarter.h>
#include <U2Core/Timer.h>
#include <U2Core/TmpDirChecker.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/UdrSchemaRegistry.h>
#include <U2Core/UserActionsWriter.h>
#include <U2Core/UserApplicationsSettings.h>
#include <U2Core/VirtualFileSystem.h>

#include <U2Designer/DashboardInfoRegistry.h>

#include <U2Formats/ConvertFileTask.h>
#include <U2Formats/DocumentFormatUtils.h>

#include <U2Gui/CredentialsAskerGui.h>
#include <U2Gui/FeatureKeyFilterTask.h>
#include <U2Gui/ImportWidgetsFactories.h>
#include <U2Gui/LogView.h>
#include <U2Gui/MsaContentFilterTask.h>
#include <U2Gui/MsaSeqNameFilterTask.h>
#include <U2Gui/OPWidgetFactoryRegistry.h>
#include <U2Gui/ObjectNameFilterTask.h>
#include <U2Gui/ObjectViewModel.h>
#include <U2Gui/PasteController.h>
#include <U2Gui/SequenceAccFilterTask.h>
#include <U2Gui/TextContentFilterTask.h>
#include <U2Gui/ToolsMenu.h>

#include <U2Lang/LocalDomain.h>
#include <U2Lang/QueryDesignerRegistry.h>
#include <U2Lang/WorkflowEnvImpl.h>

#include <U2Test/GTestFrameworkComponents.h>
#include <U2Test/XMLTestFormat.h>

#include <U2View/AnnotHighlightWidgetFactory.h>
#include <U2View/AssemblyInfoWidget.h>
#include <U2View/AssemblyNavigationWidget.h>
#include <U2View/AssemblySettingsWidget.h>
#include <U2View/ColorSchemaSettingsController.h>
#include <U2View/DnaAssemblyUtils.h>
#include <U2View/FindPatternMsaWidgetFactory.h>
#include <U2View/FindPatternWidgetFactory.h>
#include <U2View/MSAGeneralTabFactory.h>
#include <U2View/MSAHighlightingTabFactory.h>
#include <U2View/MaExportConsensusTabFactory.h>
#include <U2View/McaGeneralTabFactory.h>
#include <U2View/McaReadsTabFactory.h>
#include <U2View/PairAlignFactory.h>
#include <U2View/RefSeqCommonWidget.h>
#include <U2View/SeqStatisticsWidgetFactory.h>
#include <U2View/SequenceInfoFactory.h>
#include <U2View/TreeOptionsWidgetFactory.h>

// U2Private imports
#include <AppContextImpl.h>
#include <AppSettingsImpl.h>
#include <DocumentFormatRegistryImpl.h>
#include <IOAdapterRegistryImpl.h>
#include <PluginSupportImpl.h>
#include <ServiceRegistryImpl.h>
#include <SettingsImpl.h>
#include <TaskSchedulerImpl.h>
#include <crash_handler/CrashHandler.h>

#include <U2Core/BundleInfo.h>

#include "app_settings/AppSettingsGUIImpl.h"
#include "app_settings/logview_settings/LogSettingsGUIController.h"
#include "main_window/CheckUpdatesTask.h"
#include "main_window/MainWindowImpl.h"
#include "main_window/ProxyStyle.h"
#include "main_window/SplashScreen.h"
#include "plugin_viewer/PluginViewerImpl.h"
#include "project_support/ProjectLoaderImpl.h"
#include "project_support/ProjectTasksGui.h"
#include "project_view/ProjectViewImpl.h"
#include "shtirlitz/Shtirlitz.h"
#include "task_view/TaskViewController.h"
#include "update/UgeneUpdater.h"
#include "welcome_page/WelcomePageMdiController.h"

using namespace U2;

#ifdef Q_OS_WIN
typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);

LPFN_ISWOW64PROCESS fnIsWow64Process;

BOOL IsWow64() {
    BOOL bIsWow64 = FALSE;

    // IsWow64Process is not available on all supported versions of Windows.
    // Use GetModuleHandle to get a handle to the DLL that contains the function
    // and GetProcAddress to get a pointer to the function if available.

    fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
        GetModuleHandle(TEXT("kernel32")), "IsWow64Process");

    if (nullptr != fnIsWow64Process) {
        if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64)) {
            // handle error
        }
    }
    return bIsWow64;
}
#endif  // Q_OS_WIN

static void registerCoreServices() {
    ServiceRegistry* sr = AppContext::getServiceRegistry();
    TaskScheduler* ts = AppContext::getTaskScheduler();
    ts->registerTopLevelTask(sr->registerServiceTask(new PluginViewerImpl()));
    ts->registerTopLevelTask(sr->registerServiceTask(new ProjectViewImpl()));
}

static void setDataSearchPaths() {
    // set search paths for data files
    QStringList dataSearchPaths;
    const static char* RELATIVE_DATA_DIR = "/data";
    const static char* RELATIVE_DEV_DATA_DIR = "/../../data";
    // on windows data is normally located in the application folder
    QString appDirPath = AppContext::getWorkingDirectoryPath();
    if (QDir(appDirPath + RELATIVE_DATA_DIR).exists()) {
        dataSearchPaths.push_back(appDirPath + RELATIVE_DATA_DIR);
    } else if (QDir(appDirPath + RELATIVE_DEV_DATA_DIR).exists()) {  // data location for developers
        dataSearchPaths.push_back(appDirPath + RELATIVE_DEV_DATA_DIR);
#ifdef Q_OS_DARWIN
    } else {
        QString dataDir = BundleInfo::getDataSearchPath();
        if (!dataDir.isEmpty()) {
            dataSearchPaths.push_back(dataDir);
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
}

static void initLogsCache(LogCacheExt& logsCache, const QStringList&) {
#ifdef _DEBUG
    logsCache.setConsoleOutputEnabled(true);
#else
    QString consoleOutVal = qgetenv("UGENE_PRINT_TO_CONSOLE");
    logsCache.setConsoleOutputEnabled(consoleOutVal == "true" || consoleOutVal == "1");
#endif
    QString file = qgetenv(U2_PRINT_TO_FILE);
    if (!file.isEmpty()) {
        logsCache.setFileOutputEnabled(file);
        return;
    }
    LogSettings ls;
    ls.reinitAll();

#ifdef UGENE_FORCE_WRITE_LOG_TO_FILE
    // It should be defined during the UGENE building.
    // It is to force log writing for user's personal packages to avoid explaining how to write the log to file
    // Add to DEFINES when you execute qmake for the project
    ls.toFile = true;
    const QString logsDir = AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath() + "/logs";
    QDir().mkpath(logsDir);
    ls.outputFile = logsDir + "/ugene_log_" + QDateTime::currentDateTime().toString("yyyy.MM.dd_hh-mm") + ".txt";
#endif

    if (ls.toFile) {
        logsCache.setFileOutputEnabled(ls.outputFile);
    }
}
void guiTestMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    Q_UNUSED(context)
    switch (type) {
        case QtDebugMsg:
            uiLog.trace(msg);
            break;
        case QtWarningMsg:
            uiLog.details(msg);
            break;
        case QtCriticalMsg:
            uiLog.error(msg);
            break;
        case QtFatalMsg:
            uiLog.error(msg);
            abort();
        default:
            uiLog.trace(msg);
            break;
    }
}
static void initOptionsPanels() {
    OPWidgetFactoryRegistry* opWidgetFactoryRegistry = AppContext::getOPWidgetFactoryRegistry();
    OPCommonWidgetFactoryRegistry* opCommonWidgetFactoryRegistry = AppContext::getOPCommonWidgetFactoryRegistry();

    // Sequence View groups
    opWidgetFactoryRegistry->registerFactory(new FindPatternWidgetFactory());
    opWidgetFactoryRegistry->registerFactory(new AnnotHighlightWidgetFactory());
    opWidgetFactoryRegistry->registerFactory(new SequenceInfoFactory());

    // Assembly Browser groups
    opWidgetFactoryRegistry->registerFactory(new AssemblyNavigationWidgetFactory());
    opWidgetFactoryRegistry->registerFactory(new AssemblyInfoWidgetFactory());
    opWidgetFactoryRegistry->registerFactory(new AssemblySettingsWidgetFactory());

    // MSA groups
    auto msaGeneralTabFactory = new MSAGeneralTabFactory();
    QString msaGeneralId = msaGeneralTabFactory->getOPGroupParameters().getGroupId();
    opWidgetFactoryRegistry->registerFactory(msaGeneralTabFactory);
    opWidgetFactoryRegistry->registerFactory(new FindPatternMsaWidgetFactory());

    auto msaHighlightingFactory = new MSAHighlightingFactory();
    QString msaHighlightingId = msaHighlightingFactory->getOPGroupParameters().getGroupId();
    opWidgetFactoryRegistry->registerFactory(msaHighlightingFactory);

    opWidgetFactoryRegistry->registerFactory(new PairAlignFactory());
    opWidgetFactoryRegistry->registerFactory(new MSATreeOptionsWidgetFactory());
    opWidgetFactoryRegistry->registerFactory(new AddTreeWidgetFactory());
    opWidgetFactoryRegistry->registerFactory(new MsaExportConsensusTabFactory());

    auto msaSeqStatisticvsFactory = new SeqStatisticsWidgetFactory();
    QString msaSeqStatisticsId = msaSeqStatisticvsFactory->getOPGroupParameters().getGroupId();
    opWidgetFactoryRegistry->registerFactory(msaSeqStatisticvsFactory);

    // MSA common widgets
    QList<QString> groupIds;
    groupIds << msaHighlightingId << msaSeqStatisticsId << msaGeneralId;
    auto refSeqCommonWidget = new RefSeqCommonWidgetFactory(groupIds);
    opCommonWidgetFactoryRegistry->registerFactory(refSeqCommonWidget);

    // Tree View groups
    opWidgetFactoryRegistry->registerFactory(new TreeOptionsWidgetFactory());

    // MCA groups
    opWidgetFactoryRegistry->registerFactory(new McaGeneralTabFactory());
    opWidgetFactoryRegistry->registerFactory(new McaExportConsensusTabFactory());
    opWidgetFactoryRegistry->registerFactory(new McaReadsTabFactory());
}

static void initProjectFilterTaskRegistry() {
    ProjectFilterTaskRegistry* registry = AppContext::getProjectFilterTaskRegistry();

    registry->registerTaskFactory(new ObjectNameFilterTaskFactory);
    registry->registerTaskFactory(new MsaSeqNameFilterTaskFactory);
    registry->registerTaskFactory(new MsaContentFilterTaskFactory);
    registry->registerTaskFactory(new McaReadNameFilterTaskFactory);
    registry->registerTaskFactory(new McaReferenceNameFilterTaskFactory);
    registry->registerTaskFactory(new McaReadContentFilterTaskFactory);
    registry->registerTaskFactory(new McaReferenceContentFilterTaskFactory);
    registry->registerTaskFactory(new SequenceContentFilterTaskFactory);
    registry->registerTaskFactory(new TextContentFilterTaskFactory);
    registry->registerTaskFactory(new SequenceAccFilterTaskFactory);
    registry->registerTaskFactory(new FeatureKeyFilterTaskFactory);
}

class GApplication : public QApplication {
public:
    GApplication(int& argc, char** argv)
        : QApplication(argc, argv) {
    }

    bool event(QEvent* event) override {
        if (event->type() == QEvent::FileOpen) {
            QStringList urls(dynamic_cast<QFileOpenEvent*>(event)->file());
            openAfterPluginsLoaded(urls);
            return true;
        }
        return QApplication::event(event);
    }

    void openAfterPluginsLoaded(const QStringList& urls, TaskStarter::StartCondition condition = TaskStarter::NoCondition) {
        auto task = new OpenWithProjectTask(urls);

        auto taskStarter = new TaskStarter(task, condition);
        if (AppContext::getMainWindow()->getQMainWindow()->isVisible()) {
            taskStarter->registerTask();
        } else {
            connect(AppContext::getMainWindow(), SIGNAL(si_show()), taskStarter, SLOT(registerTask()));
        }
    }
};

static QString findKey(const QStringList& envList, const QString& key) {
    QString prefix = key + "=";
    QString result;
    foreach (const QString& var, envList) {
        if (var.startsWith(prefix)) {
            result = var.mid(prefix.length());
            break;
        }
    }
    return result;
}

#ifdef Q_OS_DARWIN
static void fixMacFonts() {
    if (QSysInfo::MacintoshVersion > QSysInfo::MV_10_8) {
        // fix Mac OS X 10.9 (mavericks) font issue
        // https://bugreports.qt-project.org/browse/QTBUG-32789
        // the solution is taken from http://successfulsoftware.net/2013/10/23/fixing-qt-4-for-mac-os-x-10-9-mavericks/
        QFont::insertSubstitution(".Lucida Grande UI", "Lucida Grande");
    }
}
#endif

int main(int argc, char** argv) {
    if (CrashHandler::isEnabled()) {
        CrashHandler::setupHandler();
    }

    if (qgetenv(ENV_SEND_CRASH_REPORTS) == "0") {
        CrashHandler::setSendCrashReports(false);
    }

    GTIMER(c1, t1, "main()->QApp::exec");

#ifdef Q_OS_DARWIN
    fixMacFonts();

    // A workaround for https://bugreports.qt.io/browse/QTBUG-87014: "Qt application gets stuck trying to open main window under Big Sur"
    qputenv("QT_MAC_WANTS_LAYER", "1");
#endif

    // QApplication app(argc, argv);
    GApplication app(argc, argv);

#ifdef Q_OS_LINUX
    QPixmap pixmap(":/ugene/images/originals/ugene_128.png");
    GApplication::setWindowIcon(pixmap);
#endif

    QMainWindow window;
    auto splashScreen = new SplashScreen(&window);
    splashScreen->adjustSize();
    splashScreen->setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            splashScreen->size(),
            qApp->desktop()->availableGeometry(QApplication::desktop()->primaryScreen())));
    splashScreen->show();

    AppContextImpl* appContext = AppContextImpl::getApplicationContext();
    appContext->setGUIMode(true);
    appContext->setWorkingDirectoryPath(QCoreApplication::applicationDirPath());

    QCoreApplication::addLibraryPath(AppContext::getWorkingDirectoryPath());

    // add some extra paths used during development
#ifdef Q_OS_WIN
#    ifdef _DEBUG
    QString devPluginsPath = QDir(AppContext::getWorkingDirectoryPath() + "/../../extras/windows/dotnet_style/_debug").absolutePath();
#    else
    QString devPluginsPath = QDir(AppContext::getWorkingDirectoryPath() + "/../../extras/windows/dotnet_style/_release").absolutePath();
#    endif
    QCoreApplication::addLibraryPath(devPluginsPath);  // dev version
#endif

    setSearchPaths();

    // parse all cmdline arguments
    auto cmdLineRegistry = new CMDLineRegistry(GApplication::arguments());
    appContext->setCMDLineRegistry(cmdLineRegistry);

    // 1 create settings
    auto globalSettings = new SettingsImpl(QSettings::SystemScope);
    appContext->setGlobalSettings(globalSettings);

    auto settings = new SettingsImpl(QSettings::UserScope);
    appContext->setSettings(settings);

    AppSettings* appSettings = new AppSettingsImpl();
    appContext->setAppSettings(appSettings);

    UserAppsSettings* userAppSettings = AppContext::getAppSettings()->getUserAppsSettings();

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

    // Set translations if needed: use value in the settings or environment variables to override.
    // The default case 'en' does not need any files: the values for this locale are hardcoded in the code.
    QTranslator translator;
    QStringList failedToLoadTranslatorFiles;  // List of translators file names tried but failed to load/not found.

    // The file specified by user has the highest priority in the translations lookup order.
    QStringList envList = QProcess::systemEnvironment();
    QString envTranslationFile = findKey(envList, "UGENE_TRANSLATION_FILE");
    if (envTranslationFile.isEmpty() || !translator.load(envTranslationFile)) {
        if (!envTranslationFile.isEmpty()) {
            failedToLoadTranslatorFiles << envTranslationFile;
        }
        QStringList translationFileList = {
            "transl_" + findKey(envList, "UGENE_TRANSLATION"),
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
    }
    if (!translator.isEmpty()) {
        QCoreApplication::installTranslator(&translator);
        GObjectTypes::initTypeTranslations();
    }
    GObjectTypes::initTypeIcons();

    ToolsMenu::init();

    // 3 create functional components of ugene
    LogCacheExt logsCache;
    initLogsCache(logsCache, envList);
    LogCache::setAppGlobalInstance(&logsCache);
    app.installEventFilter(new UserActionsWriter());
    coreLog.details(UserAppsSettings::tr("UGENE initialization started"));
    for (const QString& fileName : failedToLoadTranslatorFiles) {
        coreLog.trace(QObject::tr("Translation file not found: %1").arg(fileName));
    }

    int ugeneArch = getUgeneBinaryArch();
    QString ugeneArchCounterSuffix = ugeneArch == UGENE_ARCH_X86_64   ? "Ugene 64-bit"
                                     : ugeneArch == UGENE_ARCH_X86_32 ? "Ugene 32-bit"
                                                                      : "Undetected architecture";
    GCounter::increment("UGENE architecture", ugeneArchCounterSuffix);

    QString osArchCounterSuffix = isOsWindows() ? "Windows"
                                  : isOsMac()   ? "MacOS"
                                  : isOsLinux() ? "Linux"
                                  : isOsUnix()  ? "Unix"
                                                : "Undetected OS";
#if defined Q_OS_WIN
    if (ugeneArch == UGENE_ARCH_X86_64 || IsWow64()) {
        osArchCounterSuffix += " 64-bit";
    } else {
        osArchCounterSuffix += " 32-bit";
    }
#elif defined Q_OS_DARWIN
    osArchCounterSuffix += " 64-bit";
#else
    QString currentCpuArchitecture = QSysInfo::currentCpuArchitecture();
    if (currentCpuArchitecture.contains("64")) {
        osArchCounterSuffix += " 64-bit";
    } else if (currentCpuArchitecture.contains("32")) {
        osArchCounterSuffix += " 32-bit";
    }
#endif
    GCounter::increment("OS architecture", osArchCounterSuffix);

    coreLog.trace(QString("UGENE run at dir %1 with parameters %2").arg(AppContext::getWorkingDirectoryPath()).arg(GApplication::arguments().join(" ")));

    // print some settings info, can't do it earlier than logging is initialized
    coreLog.trace(QString("Active UGENE.ini file : %1").arg(AppContext::getSettings()->fileName()));

    qInstallMessageHandler(guiTestMessageOutput);

#if defined(Q_OS_UNIX)
    if (envList.indexOf("UGENE_GUI_TEST=1") >= 0) {
        QApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);
    }
#endif
    QString styleName = userAppSettings->getVisualStyle();
    QStyle* qtStyle = nullptr;
    if (!styleName.isEmpty()) {
        qtStyle = QStyleFactory::create(styleName);
        if (qtStyle == nullptr) {
            uiLog.details(AppContextImpl::tr("Style not available %1").arg(styleName));
        }
    }
    auto proxyStyle = new ProxyStyle(qtStyle);
    // Re-use the original style object name, because it is saved in the settings as a part of 'User preferences'.
    if (qtStyle != nullptr) {
        proxyStyle->setObjectName(qtStyle->objectName());
    }
    QApplication::setStyle(proxyStyle);

    auto resTrack = new ResourceTracker();
    appContext->setResourceTracker(resTrack);

    auto ts = new TaskSchedulerImpl(appSettings->getAppResourcePool());
    appContext->setTaskScheduler(ts);

    auto asr = new AnnotationSettingsRegistry(DocumentFormatUtils::predefinedSettings());
    appContext->setAnnotationSettingsRegistry(asr);

    auto tf = new TestFramework();
    appContext->setTestFramework(tf);

    GTestFormatRegistry* tfr = AppContext::getTestFramework()->getTestFormatRegistry();
    auto xmlTestFormat = qobject_cast<XMLTestFormat*>(tfr->findFormat("XML"));
    QList<XMLTestFactory*> fs = ProjectTests::createTestFactories();
    foreach (XMLTestFactory* f, fs) {
        bool res = xmlTestFormat->registerTestFactory(f);
        assert(res);
        Q_UNUSED(res);
    }

    auto mw = new MainWindowImpl();
    appContext->setMainWindow(mw);
    mw->prepare();
#ifdef Q_OS_DARWIN
    // TODO: need to check for other OS and remove #ifdef
    if (cmdLineRegistry->hasParameter(CMDLineCoreOptions::LAUNCH_GUI_TEST) || cmdLineRegistry->hasParameter(CMDLineCoreOptions::LAUNCH_GUI_TEST_BATCH)) {
        mw->getQMainWindow()->menuBar()->setNativeMenuBar(false);
    }
    if (cmdLineRegistry->hasParameter(CMDLineCoreOptions::DONT_USE_NATIVE_MENUBAR)) {
        if (cmdLineRegistry->getParameterValue(CMDLineCoreOptions::DONT_USE_NATIVE_MENUBAR) == "0") {
            mw->getQMainWindow()->menuBar()->setNativeMenuBar(true);
        } else {
            mw->getQMainWindow()->menuBar()->setNativeMenuBar(false);
        }
    }
#endif
    QObject::connect(UgeneUpdater::getInstance(), SIGNAL(si_update()), mw, SLOT(sl_exitAction()));

    AppSettingsGUI* appSettingsGUI = new AppSettingsGUIImpl();
    appContext->setAppSettingsGUI(appSettingsGUI);

    AppContext::getMainWindow()->getDockManager()->registerDock(MWDockArea_Bottom, new TaskViewDockWidget(), QKeySequence(Qt::ALT | Qt::Key_2));

    // Initialize logged log view
    auto logView = new LogViewWidget(&logsCache);
    logView->setObjectName(DOCK_LOG_VIEW);
    AppContext::getAppSettingsGUI()->registerPage(new LogSettingsPageController(logView));
    AppContext::getMainWindow()->getDockManager()->registerDock(MWDockArea_Bottom, logView, QKeySequence(Qt::ALT | Qt::Key_3));

    auto ovfr = new GObjectViewFactoryRegistry();
    appContext->setObjectViewFactoryRegistry(ovfr);

    auto schemaRegistry = new UdrSchemaRegistry();
    appContext->setUdrSchemaRegistry(schemaRegistry);

    auto dbiRegistry = new U2DbiRegistry();
    appContext->setDbiRegistry(dbiRegistry);

    auto dfr = new DocumentFormatRegistryImpl();
    appContext->setDocumentFormatRegistry(dfr);
    ImportWidgetsFactories::registerFactories();

    auto io = new IOAdapterRegistryImpl();
    appContext->setIOAdapterRegistry(io);

    auto dtr = new DNATranslationRegistry();
    appContext->setDNATranslationRegistry(dtr);

    DNAAlphabetRegistry* dal = new DNAAlphabetRegistryImpl(dtr);
    appContext->setDNAAlphabetRegistry(dal);

    auto dbxr = new DBXRefRegistry();
    appContext->setDBXRefRegistry(dbxr);

    auto mcsr = new MsaColorSchemeRegistry();
    appContext->setMsaColorSchemeRegistry(mcsr);

    AppContext::getAppSettingsGUI()->registerPage(new ColorSchemaSettingsPageController(mcsr));

    auto mhsr = new MsaHighlightingSchemeRegistry();
    appContext->setMsaHighlightingSchemeRegistry(mhsr);

    auto msaConsReg = new MSAConsensusAlgorithmRegistry();
    appContext->setMSAConsensusAlgorithmRegistry(msaConsReg);

    auto assemblyConsReg = new AssemblyConsensusAlgorithmRegistry();
    appContext->setAssemblyConsensusAlgorithmRegistry(assemblyConsReg);

    auto msaDistReg = new MSADistanceAlgorithmRegistry();
    appContext->setMSADistanceAlgorithmRegistry(msaDistReg);

    auto pwmConvReg = new PWMConversionAlgorithmRegistry();
    appContext->setPWMConversionAlgorithmRegistry(pwmConvReg);

    auto smr = new SubstMatrixRegistry();
    appContext->setSubstMatrixRegistry(smr);

    auto swar = new SmithWatermanTaskFactoryRegistry();
    appContext->setSmithWatermanTaskFactoryRegistry(swar);

    auto swmarntr = new SWMulAlignResultNamesTagsRegistry();
    appContext->setSWMulAlignResultNamesTagsRegistry(swmarntr);

    auto rfr = new RepeatFinderTaskFactoryRegistry();
    appContext->setRepeatFinderTaskFactoryRegistry(rfr);

    auto str = new ScriptingToolRegistry();
    appContext->setScriptingToolRegistry(str);

    auto etr = new ExternalToolRegistry();
    appContext->setExternalToolRegistry(etr);

    auto qpr = new QDActorPrototypeRegistry();
    appContext->setQDActorFactoryRegistry(qpr);

    auto cdsfr = new CDSearchFactoryRegistry();
    appContext->setCDSearchFactoryRegistry(cdsfr);

    auto saar = new StructuralAlignmentAlgorithmRegistry();
    appContext->setStructuralAlignmentAlgorithmRegistry(saar);

    auto genRegistry = new PhyTreeGeneratorRegistry();
    appContext->setPhyTreeGeneratorRegistry(genRegistry);

    auto msfr = new MolecularSurfaceFactoryRegistry();
    appContext->setMolecularSurfaceFactoryRegistry(msfr);

    auto swrfr = new SWResultFilterRegistry();
    appContext->setSWResultFilterRegistry(swrfr);

    auto sspar = new SecStructPredictAlgRegistry();
    appContext->setSecStructPedictAlgRegistry(sspar);

    auto vfsReg = new VirtualFileSystemRegistry();
    appContext->setVirtualFileSystemRegistry(vfsReg);

    auto assemblyReg = new DnaAssemblyAlgRegistry();
    appContext->setDnaAssemblyAlgRegistry(assemblyReg);

    auto genomeAssemblyReg = new GenomeAssemblyAlgRegistry();
    appContext->setGenomeAssemblyAlgRegistry(genomeAssemblyReg);

    DnaAssemblySupport assemblySupport;
    Q_UNUSED(assemblySupport);

    auto dbr = new DataBaseRegistry();
    appContext->setDataBaseRegistry(dbr);

    auto splicedAlignmentTaskRegistiry = new SplicedAlignmentTaskRegistry();
    appContext->setSplicedAlignmentTaskRegistry(splicedAlignmentTaskRegistiry);

    auto opWidgetFactoryRegistry = new OPWidgetFactoryRegistry();
    appContext->setOPWidgetFactoryRegistry(opWidgetFactoryRegistry);

    auto opCommonWidgetFactoryRegistry = new OPCommonWidgetFactoryRegistry();
    appContext->setOPCommonWidgetFactoryRegistry(opCommonWidgetFactoryRegistry);

    auto workflowScriptRegistry = new WorkflowScriptRegistry();
    appContext->setWorkflowScriptRegistry(workflowScriptRegistry);

    auto alignmentAlgorithmsRegistry = new AlignmentAlgorithmsRegistry();
    appContext->setAlignmentAlgorithmsRegistry(alignmentAlgorithmsRegistry);

    auto dpr = new U2DataPathRegistry();
    appContext->setDataPathRegistry(dpr);

    CredentialsAsker* credentialsAsker = new CredentialsAskerGui();
    appContext->setCredentialsAsker(credentialsAsker);

    auto passwordStorage = new PasswordStorage();
    appContext->setPasswordStorage(passwordStorage);
    AppSettingsImpl::addPublicDbCredentials2Settings();

    auto convertFactoryRegistry = new ConvertFactoryRegistry();
    appContext->setConvertFactoryRegistry(convertFactoryRegistry);

    auto welcomePageActions = new IdRegistry<WelcomePageAction>();
    appContext->setWelcomePageActionRegistry(welcomePageActions);

    auto projectFilterTaskRegistry = new ProjectFilterTaskRegistry;
    appContext->setProjectFilterTaskRegistry(projectFilterTaskRegistry);
    initProjectFilterTaskRegistry();

    PasteFactory* pasteFactory = new PasteFactoryImpl;
    appContext->setPasteFactory(pasteFactory);

    auto dashboardInfoRegistry = new DashboardInfoRegistry;
    appContext->setDashboardInfoRegistry(dashboardInfoRegistry);

    Workflow::WorkflowEnv::init(new Workflow::WorkflowEnvImpl());
    Workflow::WorkflowEnv::getDomainRegistry()->registerEntry(new LocalWorkflow::LocalDomainFactory());

    auto sreg = new ServiceRegistryImpl();
    appContext->setServiceRegistry(sreg);

    auto psp = new PluginSupportImpl();
    appContext->setPluginSupport(psp);

    auto pli = new ProjectLoaderImpl();
    appContext->setProjectLoader(pli);
    QObject::connect(mw, SIGNAL(si_paste()), pli, SLOT(sl_paste()));

    CMDLineUtils::init();

    auto rdc = new RecentlyDownloadedCache();
    appContext->setRecentlyDownloadedCache(rdc);

    auto aaSupport = new AutoAnnotationsSupport();
    appContext->setAutoAnnotationsSupport(aaSupport);

    auto appFileStorage = new AppFileStorage();
    U2OpStatusImpl os;
    appFileStorage->init(os);
    if (os.hasError()) {
        coreLog.error(os.getError());
        delete appFileStorage;
    } else {
        appContext->setAppFileStorage(appFileStorage);
    }

    // Register all Options Panel groups on the required GObjectViews
    initOptionsPanels();

    QStringList urls = CMDLineRegistryUtils::getPureValues();

    if (urls.isEmpty() && AppContext::getAppSettings()->getUserAppsSettings()->openLastProjectAtStartup()) {
        QString lastProject = ProjectLoaderImpl::getLastProjectURL();
        if (!lastProject.isEmpty()) {
            urls << lastProject;
        }
    }

    if (!urls.isEmpty()) {
        // defer loading until all plugins/services loaded
        app.openAfterPluginsLoaded(urls, TaskStarter::NoProject);
    }

    registerCoreServices();

    GCOUNTER(cvar, "ugeneui launch");

    // 3 run QT GUI
    t1.stop();

    // coreLog.info(AppContextImpl::tr("%1-bit version of UGENE started").arg(Version::appArchitecture));
    Version ugeneVersion = Version::appVersion();
    coreLog.info(QObject::tr("UGENE started"));
    coreLog.info(QObject::tr("UGENE version: %1 %2-bit").arg(ugeneVersion.toString()).arg(Version::appArchitecture));

    QObject::connect(psp, &PluginSupport::si_allStartUpPluginsLoaded, [splashScreen, mw]() {
        auto externalToolManager = AppContext::getExternalToolRegistry()->getManager();
        if (externalToolManager == nullptr) {  // External tools plugin is not loaded.
            splashScreen->sl_close();
            mw->sl_show();
        } else {
            QObject::connect(externalToolManager, &ExternalToolManager::si_startupValidationFinished, splashScreen, &SplashScreen::sl_close);
            QObject::connect(externalToolManager, &ExternalToolManager::si_startupValidationFinished, mw, &MainWindowImpl::sl_show);
        }
    });

    auto welcomePageController = new WelcomePageMdiController();
    QObject::connect(mw, &MainWindowImpl::si_show, welcomePageController, &WelcomePageMdiController::sl_showPage);
    QObject::connect(mw, SIGNAL(si_showWelcomePage()), welcomePageController, SLOT(sl_showPage()));
    QObject::connect(pli, SIGNAL(si_recentListChanged()), welcomePageController, SLOT(sl_onRecentChanged()));

    QList<Task*> tasks;

    if (UgeneUpdater::isEnabled() && qgetenv("UGENE_GUI_TEST").toInt() != 1) {
        tasks << new CheckUpdatesTask(true);

        auto tempDirChecker = new TmpDirChecker;
        tasks << tempDirChecker;
        QObject::connect(tempDirChecker, SIGNAL(si_checkFailed(QString)), mw, SLOT(sl_tempDirPathCheckFailed(QString)));
    }
    if (!envList.contains(ENV_UGENE_DEV + QString("=1"))) {
        tasks << new ShtirlitzStartupTask();
    }

    mw->registerStartupChecks(tasks);

    MemoryLocker l(160, AppResource::SystemMemory);  // 100Mb on UGENE start, ~60Mb SQLite cache
    int rc = GApplication::exec();
    l.release();

    // 4 deallocate resources
    if (!envList.contains(ENV_UGENE_DEV + QString("=1"))) {
        Shtirlitz::saveGatheredInfo();
    }

    delete welcomePageController;

    appContext->setDashboardInfoRegistry(nullptr);
    delete dashboardInfoRegistry;

    appContext->setPasteFactory(nullptr);
    delete pasteFactory;

    appContext->setProjectFilterTaskRegistry(nullptr);
    delete projectFilterTaskRegistry;

    appContext->setRecentlyDownloadedCache(nullptr);
    delete rdc;

    appContext->setProjectLoader(nullptr);
    delete pli;

    appContext->setPluginSupport(nullptr);
    delete psp;

    appContext->setServiceRegistry(nullptr);
    delete sreg;

    Workflow::WorkflowEnv::shutdown();

    appContext->setCredentialsAsker(nullptr);
    delete credentialsAsker;

    appContext->setPasswordStorage(nullptr);
    delete passwordStorage;

    appContext->setDataPathRegistry(nullptr);
    delete dpr;

    appContext->setAlignmentAlgorithmsRegistry(nullptr);
    delete alignmentAlgorithmsRegistry;

    appContext->setWorkflowScriptRegistry(nullptr);
    delete workflowScriptRegistry;

    appContext->setOPCommonWidgetFactoryRegistry(nullptr);
    delete opCommonWidgetFactoryRegistry;

    appContext->setOPWidgetFactoryRegistry(nullptr);
    delete opWidgetFactoryRegistry;

    appContext->setSplicedAlignmentTaskRegistry(nullptr);
    delete splicedAlignmentTaskRegistiry;

    appContext->setDataBaseRegistry(nullptr);
    delete dbr;

    appContext->setDnaAssemblyAlgRegistry(nullptr);
    delete assemblyReg;

    appContext->setVirtualFileSystemRegistry(nullptr);
    delete vfsReg;

    appContext->setSecStructPedictAlgRegistry(nullptr);
    delete sspar;

    appContext->setWelcomePageActionRegistry(nullptr);
    delete welcomePageActions;

    appContext->setConvertFactoryRegistry(nullptr);
    delete convertFactoryRegistry;

    appContext->setSWResultFilterRegistry(nullptr);
    delete swrfr;

    appContext->setMolecularSurfaceFactoryRegistry(nullptr);
    delete msfr;

    appContext->setPhyTreeGeneratorRegistry(nullptr);
    delete genRegistry;

    appContext->setStructuralAlignmentAlgorithmRegistry(nullptr);
    delete saar;

    appContext->setCDSearchFactoryRegistry(nullptr);
    delete cdsfr;

    appContext->setQDActorFactoryRegistry(nullptr);
    delete qpr;

    appContext->setExternalToolRegistry(nullptr);
    delete etr;

    appContext->setScriptingToolRegistry(nullptr);
    delete str;

    appContext->setRepeatFinderTaskFactoryRegistry(nullptr);
    delete rfr;

    appContext->setSWMulAlignResultNamesTagsRegistry(nullptr);
    delete swmarntr;

    appContext->setSmithWatermanTaskFactoryRegistry(nullptr);
    delete swar;

    appContext->setSubstMatrixRegistry(nullptr);
    delete smr;

    appContext->setPWMConversionAlgorithmRegistry(nullptr);
    delete pwmConvReg;

    appContext->setMSADistanceAlgorithmRegistry(nullptr);
    delete msaDistReg;

    appContext->setAssemblyConsensusAlgorithmRegistry(nullptr);
    delete assemblyConsReg;

    appContext->setMSAConsensusAlgorithmRegistry(nullptr);
    delete msaConsReg;

    appContext->setMsaHighlightingSchemeRegistry(nullptr);
    delete mhsr;

    appContext->setMsaColorSchemeRegistry(nullptr);
    delete mcsr;

    appContext->setDBXRefRegistry(nullptr);
    delete dbxr;

    appContext->setDNAAlphabetRegistry(nullptr);
    delete dal;

    appContext->setDNATranslationRegistry(nullptr);
    delete dtr;

    appContext->setIOAdapterRegistry(nullptr);
    delete io;

    appContext->setDocumentFormatRegistry(nullptr);
    delete dfr;

    delete dbiRegistry;
    appContext->setDbiRegistry(nullptr);

    appContext->setUdrSchemaRegistry(nullptr);
    delete schemaRegistry;

    appContext->setObjectViewFactoryRegistry(nullptr);
    delete ovfr;

    appContext->setAnnotationSettingsRegistry(nullptr);
    delete asr;

    appContext->setCMDLineRegistry(nullptr);
    delete cmdLineRegistry;

    appContext->setMainWindow(nullptr);
    delete mw;

    appContext->setTestFramework(nullptr);
    delete tf;

    appContext->setTaskScheduler(nullptr);
    delete ts;

    appContext->setResourceTracker(nullptr);
    delete resTrack;

    appContext->setAutoAnnotationsSupport(nullptr);
    delete aaSupport;

    bool deleteSettingsFile = userAppSettings->resetSettings();
    QString iniFile = AppContext::getSettings()->fileName();

    appContext->setAppSettingsGUI(nullptr);
    delete appSettingsGUI;

    appContext->setAppSettings(nullptr);
    delete appSettings;

    appContext->setSettings(nullptr);
    delete settings;

    appContext->setGlobalSettings(nullptr);
    delete globalSettings;

    if (deleteSettingsFile) {
#ifndef Q_OS_DARWIN
        QFile::remove(iniFile);
#else
        ResetSettingsMac::reset();
#endif  // !Q_OS_DARWIN
    }

    UgeneUpdater::onClose();
    CrashHandler::shutdown();

    return rc;
}
