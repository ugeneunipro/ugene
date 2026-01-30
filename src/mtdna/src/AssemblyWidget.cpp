#include "AssemblyWidget.h"

#include <qglobal.h>

#ifdef Q_OS_WIN
#    include <tchar.h>
#    include <windows.h>
#endif  // Q_OS_WIN

#ifdef Q_OS_DARWIN
#    include <QOperatingSystemVersion>

#    include <U2Core/BundleInfo.h>

#    include "app_settings/ResetSettingsMac.h"
#endif

#include <QApplication>
#include <QProcess>
#include <QScreen>
#include <QTranslator>

#include <U2Algorithm/AssemblyConsensusAlgorithmRegistry.h>

#include <U2Core/CMDLineRegistry.h>
#include <U2Core/DNAAlphabetRegistryImpl.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/PasswordStorage.h>
#include <U2Core/ResourceTracker.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/UdrSchemaRegistry.h>
#include <U2Core/UserApplicationsSettings.h>
#include <U2Core/Version.h>

#include <U2Gui/OPWidgetFactoryRegistry.h>
#include <U2Gui/U2FileDialogPolicy.h>

#include <U2View/AssemblyBrowser.h>
#include <U2View/AssemblyInfoWidget.h>
#include <U2View/AssemblyNavigationWidget.h>
#include <U2View/AssemblySettingsWidget.h>

// U2Private imports
#include <AppContextImpl.h>
#include <AppSettingsImpl.h>
#include <DocumentFormatRegistryImpl.h>
#include <IOAdapterRegistryImpl.h>
#include <ServiceRegistryImpl.h>
#include <SettingsImpl.h>
#include <TaskSchedulerImpl.h>

#include "main_window/MainWindowImpl.h"
#include "main_window/ShutdownTask.h"
#include "main_window/TmpDirChangeDialogController.h"
#include "project_support/ProjectServiceImpl.h"
#include "project_support/ProjectImpl.h"

using namespace U2;

namespace biopipe {

AssemblyWidget::AssemblyWidget(QWidget* parent, const QString& assemblyPath, const QString& referencePath)
    : QWidget(parent) {

    AssemblyWidget::init();

    AssemblyWidget::createWidget(parent, assemblyPath, referencePath);

    auto rootLayout = new QHBoxLayout();
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->addWidget(assemblyWidget);
    setLayout(rootLayout);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

AssemblyWidget::~AssemblyWidget() {
    delete assemblyWidget;
    AssemblyWidget::free();
}

void AssemblyWidget::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    assemblyWidget->showMaximized();
}

void AssemblyWidget::init() {
    AppContextImpl* appContext = AppContextImpl::getApplicationContext();
    appContext->setGUIMode(true);

    auto cmdLineRegistry = new CMDLineRegistry({});
    appContext->setCMDLineRegistry(cmdLineRegistry);

    auto globalSettings = new SettingsImpl(QSettings::SystemScope);
    appContext->setGlobalSettings(globalSettings);

    auto settings = new SettingsImpl(QSettings::UserScope);
    appContext->setSettings(settings);

    auto appSettings = new AppSettingsImpl();
    appContext->setAppSettings(appSettings);

    UserAppsSettings* userAppSettings = AppContext::getAppSettings()->getUserAppsSettings();

#ifdef Q_OS_DARWIN
    fixMacFonts();

    // A workaround for https://bugreports.qt.io/browse/QTBUG-87014: "Qt application gets stuck trying to open main window under Big Sur"
    qputenv("QT_MAC_WANTS_LAYER", "1");
#endif

    // QApplication app(argc, argv);
    appContext->setWorkingDirectoryPath(QCoreApplication::applicationDirPath());

    if (isOsWindows()) {
        QFont appFont = QGuiApplication::font();
        if (appFont.pointSize() < 8) {
            appFont.setPointSize(8);
            QGuiApplication::setFont(appFont);
        }
    }

    QCoreApplication::addLibraryPath(AppContext::getWorkingDirectoryPath());

    setSearchPaths();

    // Set translations if needed: use value in the settings or environment variables to override.
    // The default case 'en' does not need any files: the values for this locale are hardcoded in the code.
    QTranslator translator;

    // The file specified by user has the highest priority in the translations lookup order.
    QStringList envList = QProcess::systemEnvironment();
    QStringList translationFileList = {
        "transl_" + findKey(envList, "ASSEMBLY_LANGUAGE"),
        userAppSettings->getTranslationFile(),
        "transl_" + QLocale::system().name().left(2).toLower()};

    // Keep only valid entries.
    translationFileList.removeAll("");
    translationFileList.removeAll("transl_");
    translationFileList.removeDuplicates();
    // Use the first translation from the list that works.
    for (const QString& translationFile : qAsConst(translationFileList)) {
        if (translationFile == "transl_en" || translator.load(translationFile, AppContext::getWorkingDirectoryPath())) {
            break;
        }
    }
    if (!translator.isEmpty()) {
        QCoreApplication::installTranslator(&translator);
        GObjectTypes::initTypeTranslations();
    }

    U2FileDialogPolicy::checkIfShouldUseNonNativeDialog();

    auto resTrack = new ResourceTracker();
    appContext->setResourceTracker(resTrack);

    auto ts = new TaskSchedulerImpl(appSettings->getAppResourcePool());
    appContext->setTaskScheduler(ts);

    auto sreg = new ServiceRegistryImpl();
    appContext->setServiceRegistry(sreg);

    auto mw = new MainWindowImpl;
    appContext->setMainWindow(mw);

    QMainWindow window;

    auto ovfr = new GObjectViewFactoryRegistry();
    appContext->setObjectViewFactoryRegistry(ovfr);

    auto schemaRegistry = new UdrSchemaRegistry();
    appContext->setUdrSchemaRegistry(schemaRegistry);

    auto dbiRegistry = new U2DbiRegistry();
    appContext->setDbiRegistry(dbiRegistry);

    auto dfr = new DocumentFormatRegistryImpl();
    appContext->setDocumentFormatRegistry(dfr);

    auto io = new IOAdapterRegistryImpl();
    appContext->setIOAdapterRegistry(io);

    auto dtr = new DNATranslationRegistry();
    appContext->setDNATranslationRegistry(dtr);

    auto dal = new DNAAlphabetRegistryImpl(dtr);
    appContext->setDNAAlphabetRegistry(dal);

    auto assemblyConsReg = new AssemblyConsensusAlgorithmRegistry();
    appContext->setAssemblyConsensusAlgorithmRegistry(assemblyConsReg);

    auto opWidgetFactoryRegistry = new OPWidgetFactoryRegistry();
    appContext->setOPWidgetFactoryRegistry(opWidgetFactoryRegistry);

    auto opCommonWidgetFactoryRegistry = new OPCommonWidgetFactoryRegistry();
    appContext->setOPCommonWidgetFactoryRegistry(opCommonWidgetFactoryRegistry);

    auto passwordStorage = new PasswordStorage();
    appContext->setPasswordStorage(passwordStorage);
    AppSettingsImpl::addPublicDbCredentials2Settings();

    // Register all Options Panel groups on the required GObjectViews
    initOptionsPanels();

    // Check for tmp dir is valid
    // Move outside?
    while (true) {
        QString ugeneTempDirPath = userAppSettings->getUserTemporaryDirPath() + "/ugene_tmp";
        QDir dir;
        dir.mkpath(ugeneTempDirPath);
        CHECK_BREAK(!FileAndDirectoryUtils::isDirectoryWritable(ugeneTempDirPath));

        TmpDirChangeDialogController tmpDirChangeDialogController(ugeneTempDirPath, nullptr);
        tmpDirChangeDialogController.exec();

        if (tmpDirChangeDialogController.result() == QDialog::Accepted) {
            userAppSettings->setUserTemporaryDirPath(tmpDirChangeDialogController.getTmpDirPath());
        } else {
            throw std::exception("Tmd dir is not writtable");
        }
    }
}

void AssemblyWidget::free() {
    auto shutdownTask = new ShutdownTask();
    AppContext::getTaskScheduler()->registerTopLevelTask(shutdownTask);
    loopUntilTaskIsFinished(shutdownTask);

    AppContextImpl* appContext = AppContextImpl::getApplicationContext();
    auto sreg = appContext->getServiceRegistry();
    appContext->setServiceRegistry(nullptr);
    delete sreg;

    auto passwordStorage = appContext->getPasswordStorage();
    appContext->setPasswordStorage(nullptr);
    delete passwordStorage;

    auto opCommonWidgetFactoryRegistry = appContext->getOPCommonWidgetFactoryRegistry();
    appContext->setOPCommonWidgetFactoryRegistry(nullptr);
    delete opCommonWidgetFactoryRegistry;

    auto opWidgetFactoryRegistry = appContext->getOPWidgetFactoryRegistry();
    appContext->setOPWidgetFactoryRegistry(nullptr);
    delete opWidgetFactoryRegistry;

    auto assemblyConsReg = appContext->getAssemblyConsensusAlgorithmRegistry();
    appContext->setAssemblyConsensusAlgorithmRegistry(nullptr);
    delete assemblyConsReg;

    auto dal = appContext->getDNAAlphabetRegistry();
    appContext->setDNAAlphabetRegistry(nullptr);
    delete dal;

    auto dtr = appContext->getDNATranslationRegistry();
    appContext->setDNATranslationRegistry(nullptr);
    delete dtr;

    auto io = appContext->getIOAdapterRegistry();
    appContext->setIOAdapterRegistry(nullptr);
    delete io;

    auto dfr = appContext->getDocumentFormatRegistry();
    appContext->setDocumentFormatRegistry(nullptr);
    delete dfr;

    auto dbiRegistry = appContext->getDbiRegistry();
    delete dbiRegistry;
    appContext->setDbiRegistry(nullptr);

    auto schemaRegistry = appContext->getUdrSchemaRegistry();
    appContext->setUdrSchemaRegistry(nullptr);
    delete schemaRegistry;

    auto ovfr = appContext->getObjectViewFactoryRegistry();
    appContext->setObjectViewFactoryRegistry(nullptr);
    delete ovfr;

    auto cmdLineRegistry = appContext->getCMDLineRegistry();
    appContext->setCMDLineRegistry(nullptr);
    delete cmdLineRegistry;

    auto mw = appContext->getMainWindow();
    appContext->setMainWindow(nullptr);
    delete mw;

    auto ts = appContext->getTaskScheduler();
    appContext->setTaskScheduler(nullptr);
    delete ts;

    auto resTrack = appContext->getResourceTracker();
    appContext->setResourceTracker(nullptr);
    delete resTrack;

    UserAppsSettings* userAppSettings = AppContext::getAppSettings()->getUserAppsSettings();
    bool deleteSettingsFile = userAppSettings->resetSettings();
    QString iniFile = AppContext::getSettings()->fileName();

    auto appSettings = appContext->getAppSettings();
    appContext->setAppSettings(nullptr);
    delete appSettings;

    auto settings = appContext->getSettings();
    appContext->setSettings(nullptr);
    delete settings;

    auto globalSettings = appContext->getGlobalSettings();
    appContext->setGlobalSettings(nullptr);
    delete globalSettings;

    if (deleteSettingsFile) {
#ifndef Q_OS_DARWIN
        QFile::remove(iniFile);
#else
        ResetSettingsMac::reset();
#endif  // !Q_OS_DARWIN
    }
}

void AssemblyWidget::createWidget(QWidget* parent, const QString& assemblyPath, const QString& referencePath) {
    U2OpStatusImpl os;
    auto ldt = LoadDocumentTask::getDefaultLoadDocTask(os, assemblyPath);
    AppContext::getTaskScheduler()->registerTopLevelTask(ldt);

    loopUntilTaskIsFinished(ldt);

    auto assemblyDoc = ldt->takeDocument();
    if (assemblyDoc == nullptr) {
        throw std::exception("Assembly doc is missed");
    }

    auto assemblyObjects = assemblyDoc->getObjects();
    if (assemblyObjects.isEmpty()) {
        throw std::exception("Assembly objects list is empty");
    }
    auto obj = assemblyObjects.first();
    if (obj == nullptr) {
        throw std::exception("Assembly obj is missed");
    }

    auto ao = qobject_cast<AssemblyObject*>(obj);
    auto proj = new ProjectImpl("", "");
    auto ps = new ProjectServiceImpl(proj);
    auto plt = AppContext::getServiceRegistry()->registerServiceTask(ps);
    AppContext::getTaskScheduler()->registerTopLevelTask(plt);

    loopUntilTaskIsFinished(plt);

    auto v = new AssemblyBrowser("mtDNA Assembly Browser", ao);
    if (!v->checkValid(os)) {
        delete v;
        throw std::exception(os.getError().toStdString().c_str());
    }

    assemblyWidget = new GObjectViewWindow(v, "mtDNA Assembly Browser123", false);

    auto loadRefTask = LoadDocumentTask::getDefaultLoadDocTask(os, referencePath);
    if (os.hasError()) {
        delete assemblyWidget;
        throw std::exception(os.getError().toStdString().c_str());
    }
    AppContext::getTaskScheduler()->registerTopLevelTask(loadRefTask);
    loopUntilTaskIsFinished(loadRefTask);

    auto referenceDoc = loadRefTask->takeDocument();
    if (referenceDoc == nullptr) {
        throw std::exception("Reference doc is missed");
    }

    v->setReference(referenceDoc);

    qputenv(AssemblyBrowser::ONLY_ASSEMBLY_BROWSER, "true");
}

void AssemblyWidget::loopUntilTaskIsFinished(Task* task) {
    QEventLoop loop;
    QObject::connect(task, &Task::si_stateChanged, [&loop, task]() {
        if (task->isFinished()) {
            loop.quit();
        }
    });
    loop.exec();

    if (task->hasError()) {
        throw std::exception(QString("LoadDocumentTask: %1").arg(task->getError()).toStdString().c_str());
    }
}

void AssemblyWidget::setDataSearchPaths() {
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

void AssemblyWidget::setSearchPaths() {
    setDataSearchPaths();
}

void AssemblyWidget::initOptionsPanels() {
    OPWidgetFactoryRegistry* opWidgetFactoryRegistry = AppContext::getOPWidgetFactoryRegistry();
    OPCommonWidgetFactoryRegistry* opCommonWidgetFactoryRegistry = AppContext::getOPCommonWidgetFactoryRegistry();

    // Assembly Browser groups
    opWidgetFactoryRegistry->registerFactory(new AssemblyNavigationWidgetFactory());
    opWidgetFactoryRegistry->registerFactory(new AssemblyInfoWidgetFactory());
    opWidgetFactoryRegistry->registerFactory(new AssemblySettingsWidgetFactory());
}

QString AssemblyWidget::findKey(const QStringList& envList, const QString& key) {
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
void AssemblyWidget::fixMacFonts() {
    QOperatingSystemVersion osVersion = QOperatingSystemVersion::current();
    if (osVersion.majorVersion() >= 10 && osVersion.minorVersion() >= 8) {
        // fix Mac OS X 10.9 (mavericks) font issue
        // https://bugreports.qt-project.org/browse/QTBUG-32789
        // the solution is taken from http://successfulsoftware.net/2013/10/23/fixing-qt-4-for-mac-os-x-10-9-mavericks/
        QFont::insertSubstitution(".Lucida Grande UI", "Lucida Grande");
    }
}
#endif

QStringList AssemblyWidget::getArguments(int argc, char** argv) {
    QStringList args;
    for (int i = 0; i < argc; ++i) {
        args << QString::fromLocal8Bit(argv[i]);
    }
    return args;
}



}  // namespace biopipe
