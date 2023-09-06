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

#include <core/GUITestOpStatus.h>

#include <QDir>
#include <QMainWindow>
#include <QScreen>

#include <U2Core/AppContext.h>
#include <U2Core/CMDLineCoreOptions.h>
#include <U2Core/CMDLineRegistry.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/Log.h>
#include <U2Core/Settings.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/TaskStarter.h>
#include <U2Core/Timer.h>
#include <U2Core/U2SafePoints.h>

#include "GUITestService.h"
#include "GUITestTeamcityLogger.h"
#include "GUITestThread.h"
#include "UGUITest.h"
#include "UGUITestBase.h"

namespace U2 {

#define ULOG_CAT_TEAMCITY "Teamcity Log"
static Logger log(ULOG_CAT_TEAMCITY);
const QString GUITestService::GUITESTING_REPORT_PREFIX = "GUITesting";

GUITestService::GUITestService(QObject*)
    : Service(Service_GUITesting, tr("GUI test viewer"), tr("Service to support UGENE GUI testing")),
      runTestsAction(nullptr),
      testLauncher(nullptr),
      isTeamcityLogOn(false) {
    connect(AppContext::getPluginSupport(), SIGNAL(si_allStartUpPluginsLoaded()), SLOT(sl_allStartUpPluginsLoaded()));
}

GUITestService::~GUITestService() {
    delete runTestsAction;
}

GUITestService* GUITestService::getGuiTestService() {
    QList<Service*> services = AppContext::getServiceRegistry()->findServices(Service_GUITesting);
    return services.isEmpty() ? nullptr : qobject_cast<GUITestService*>(services.first());
}

void GUITestService::sl_registerService() {
    registerServiceTask();
}

void GUITestService::sl_serviceRegistered() {
    const LaunchOptions launchedFor = getLaunchOptions(AppContext::getCMDLineRegistry());

    switch (launchedFor) {
        case RUN_ONE_TEST:
            QTimer::singleShot(isOsLinux() ? 100 : 1000, this, SLOT(runGUITest()));
            break;

        case RUN_ALL_TESTS:
            registerAllTestsTask();
            break;

        case RUN_TEST_SUITE:
            registerTestSuiteTask();
            break;

        case RUN_ALL_TESTS_BATCH:
            QTimer::singleShot(1000, this, SLOT(runAllGUITests()));
            break;

        case RUN_CRAZY_USER_MODE:
            QTimer::singleShot(1000, this, SLOT(runGUICrazyUserTest()));
            break;

        case RUN_ALL_TESTS_NO_IGNORED:
            registerAllTestsTaskNoIgnored();
            break;

        case NONE:
        default:
            break;
    }
}

void GUITestService::setEnvVariablesForGuiTesting() {
    qputenv(ENV_GUI_TEST, "1");
    qputenv(ENV_USE_NATIVE_DIALOGS, "0");
    qputenv(ENV_UGENE_DEV, "1");
}

GUITestService::LaunchOptions GUITestService::getLaunchOptions(CMDLineRegistry* cmdLine) {
    CHECK(cmdLine, NONE);

    LaunchOptions result = NONE;
    if (cmdLine->hasParameter(CMDLineCoreOptions::LAUNCH_GUI_TEST)) {
        QString paramValue = cmdLine->getParameterValue(CMDLineCoreOptions::LAUNCH_GUI_TEST);
        result = !paramValue.isEmpty() ? RUN_ONE_TEST : RUN_ALL_TESTS;
    } else if (cmdLine->hasParameter(CMDLineCoreOptions::LAUNCH_GUI_TEST_BATCH)) {
        result = RUN_ALL_TESTS_BATCH;
    } else if (cmdLine->hasParameter(CMDLineCoreOptions::LAUNCH_GUI_TEST_SUITE)) {
        result = RUN_TEST_SUITE;
    } else if (cmdLine->hasParameter(CMDLineCoreOptions::LAUNCH_GUI_TEST_NO_IGNORED)) {
        result = RUN_ALL_TESTS_NO_IGNORED;
    } else if (cmdLine->hasParameter(CMDLineCoreOptions::LAUNCH_GUI_TEST_CRAZY_USER)) {
        result = RUN_CRAZY_USER_MODE;
    }
    if (result != NONE) {
        setEnvVariablesForGuiTesting();
    }
    return result;
}

bool GUITestService::isGuiTestServiceNeeded() {
    return getLaunchOptions(AppContext::getCMDLineRegistry()) != NONE;
}

void GUITestService::registerAllTestsTask() {
    testLauncher = createTestLauncherTask();
    AppContext::getTaskScheduler()->registerTopLevelTask(testLauncher);

    connect(AppContext::getTaskScheduler(), SIGNAL(si_stateChanged(Task*)), SLOT(sl_taskStateChanged(Task*)));
}

void GUITestService::registerAllTestsTaskNoIgnored() {
    testLauncher = createTestLauncherTask(0, true);
    AppContext::getTaskScheduler()->registerTopLevelTask(testLauncher);

    connect(AppContext::getTaskScheduler(), SIGNAL(si_stateChanged(Task*)), SLOT(sl_taskStateChanged(Task*)));
}

Task* GUITestService::createTestLauncherTask(int suiteNumber, bool noIgnored) const {
    SAFE_POINT(testLauncher == nullptr, "", nullptr);

    Task* task = new GUITestLauncher(suiteNumber, noIgnored);
    return task;
}

void GUITestService::registerTestSuiteTask() {
    testLauncher = createTestSuiteLauncherTask();
    AppContext::getTaskScheduler()->registerTopLevelTask(testLauncher);

    connect(AppContext::getTaskScheduler(), SIGNAL(si_stateChanged(Task*)), this, SLOT(sl_taskStateChanged(Task*)));
}

Task* GUITestService::createTestSuiteLauncherTask() const {
    Q_ASSERT(!testLauncher);

    CMDLineRegistry* cmdLine = AppContext::getCMDLineRegistry();
    Q_ASSERT(cmdLine);

    bool ok;
    int suiteNumber = cmdLine->getParameterValue(CMDLineCoreOptions::LAUNCH_GUI_TEST_SUITE).toInt(&ok);
    bool useSameIni = cmdLine->hasParameter(CMDLineCoreOptions::USE_SAME_INI_FOR_TESTS);
    QString iniTemplate;

    if (useSameIni) {
        QString settingsFile = AppContext::getSettings()->fileName();
        QFileInfo iniFile(settingsFile);
        // check if file exists and if yes: Is it really a file and no directory?
        if (iniFile.exists() && iniFile.isFile()) {
            iniTemplate = settingsFile;
        } else {
            useSameIni = false;
        }
    }
    if (!ok) {
        QString pathToSuite = cmdLine->getParameterValue(CMDLineCoreOptions::LAUNCH_GUI_TEST_SUITE);
        Task* task = !useSameIni ? new GUITestLauncher(pathToSuite) : new GUITestLauncher(pathToSuite, false, iniTemplate);
        Q_ASSERT(task);
        return task;
    }

    Task* task = !useSameIni ? new GUITestLauncher(suiteNumber) : new GUITestLauncher(suiteNumber, false, iniTemplate);
    Q_ASSERT(task);

    return task;
}

void GUITestService::sl_allStartUpPluginsLoaded() {
    auto externalToolsManager = AppContext::getExternalToolRegistry()->getManager();
    if (externalToolsManager != nullptr && externalToolsManager->isInStartupValidationMode()) {
        // Wait until startup validation is finished.
        connect(externalToolsManager, SIGNAL(si_startupValidationFinished()), SLOT(sl_registerService()));
    } else {
        sl_registerService();
    }
}

void GUITestService::runAllGUITests() {
    UGUITestBase* db = UGUITestBase::getInstance();
    QList<GUITest*> preChecks = db->getTests(UGUITestBase::PreCheck);
    QList<GUITest*> postChecks = db->getTests(UGUITestBase::PostCheck);
    QList<GUITest*> postActions = db->getTests(UGUITestBase::PostAction);

    QList<GUITest*> tests = db->getTests(UGUITestBase::Normal);
    SAFE_POINT(!tests.isEmpty(), "", );

    QString runOneTestOnly = qgetenv("UGENE_GUI_TEST_NAME_RUN_ONLY");
    for (GUITest* test : qAsConst(tests)) {
        QString testName = test->getFullName();
        QString testNameForTeamCity = test->suite + "_" + test->name;

        if (!runOneTestOnly.isNull() && !runOneTestOnly.isEmpty() && runOneTestOnly != testName) {
            continue;
        }

        if (UGUITestLabels::hasIgnoredLabel(test)) {
            coreLog.details("Test has ignored label: " + test->getFullName());
            GUITestTeamcityLogger::testIgnored(testNameForTeamCity, test->getDescription());
            continue;
        }

        qint64 startTime = GTimer::currentTimeMicros();
        GUITestTeamcityLogger::testStarted(testNameForTeamCity);

        HI::GUITestOpStatus os;
        log.trace("GTRUNNER - runAllGUITests - going to run initial checks before " + testName);
        for (GUITest* initTest : qAsConst(preChecks)) {
            initTest->run();
        }

        clearSandbox();
        log.trace("GTRUNNER - runAllGUITests - going to run test " + testName);
        test->run();
        log.trace("GTRUNNER - runAllGUITests - finished running test " + testName);

        for (GUITest* postCheckTest : qAsConst(postChecks)) {
            postCheckTest->run();
        }

        QString testResult = GTGlobals::getOpStatus().getError();
        if (testResult.isEmpty()) {
            testResult = GUITestTeamcityLogger::successResult;
        }
        GTGlobals::resetOpStatus();
        for (GUITest* postActionTest : qAsConst(postActions)) {
            postActionTest->run();
        }

        qint64 finishTime = GTimer::currentTimeMicros();
        GUITestTeamcityLogger::teamCityLogResult(testNameForTeamCity, testResult, GTimer::millisBetween(startTime, finishTime));
    }

    log.trace("GTRUNNER - runAllGUITests - shutting down UGENE");
    AppContext::getTaskScheduler()->cancelAllTasks();
    AppContext::getMainWindow()->getQMainWindow()->close();
}

void GUITestService::runGUITest() {
    CMDLineRegistry* cmdLine = AppContext::getCMDLineRegistry();
    SAFE_POINT(cmdLine != nullptr, "", );
    QString fullTestName = cmdLine->getParameterValue(CMDLineCoreOptions::LAUNCH_GUI_TEST);
    isTeamcityLogOn = cmdLine->hasParameter(CMDLineCoreOptions::TEAMCITY_OUTPUT);

    QString suiteName = fullTestName.split(":").first();
    QString testName = fullTestName.split(":").last();
    UGUITestBase* testBase = UGUITestBase::getInstance();
    GUITest* test = testBase->getTest(suiteName, testName);

    if (test == nullptr) {
        // Search by teamcity test name.
        const QList<GUITest*> testList = testBase->getTests();
        for (GUITest* test2 : qAsConst(testList)) {
            QString teamcityTestName = UGUITest::getTeamcityTestName(test2->suite, test2->name);
            if (fullTestName == teamcityTestName) {
                test = test2;
                break;
            }
        }
    }
    SAFE_POINT(test != nullptr, QString("Test '%1' is not found. A wrong test name?").arg(testName), );
    runGUITest(test);
}

void GUITestService::runGUICrazyUserTest() {
    UGUITestBase* testBase = UGUITestBase::getInstance();
    GUITest* test = testBase->getTest("", "simple_crazy_user");
    runGUITest(test);
}

void GUITestService::runGUITest(GUITest* test) {
    SAFE_POINT(test != nullptr, "GUITest is NULL", );
    if (isTeamcityLogOn) {
        QString testNameForTeamCity = test->suite + "_" + test->name;
        GUITestTeamcityLogger::testStarted(testNameForTeamCity);
    }

    auto testThread = new GUITestThread(test);
    connect(testThread, SIGNAL(finished()), SLOT(sl_testThreadFinish()));
    testThread->start();
}

void GUITestService::registerServiceTask() {
    Task* registerServiceTask = AppContext::getServiceRegistry()->registerServiceTask(this);
    SAFE_POINT(registerServiceTask != nullptr, "registerServiceTask is NULL", );
    connect(new TaskSignalMapper(registerServiceTask), SIGNAL(si_taskFinished(Task*)), SLOT(sl_serviceRegistered()));

    AppContext::getTaskScheduler()->registerTopLevelTask(registerServiceTask);
}

void GUITestService::serviceStateChangedCallback(ServiceState, bool enabledStateChanged) {
    if (!enabledStateChanged) {
        return;
    }
}

void GUITestService::sl_taskStateChanged(Task* t) {
    if (t != testLauncher) {
        return;
    }
    if (!t->isFinished()) {
        return;
    }

    testLauncher = nullptr;
    AppContext::getTaskScheduler()->disconnect(this);

    LaunchOptions launchedFor = getLaunchOptions(AppContext::getCMDLineRegistry());
    if (launchedFor == RUN_ALL_TESTS || launchedFor == RUN_TEST_SUITE) {
        AppContext::getTaskScheduler()->cancelAllTasks();
        AppContext::getMainWindow()->getQMainWindow()->close();
    }
}

void GUITestService::writeTestResult(const QString& result) {
    printf("%s\n", (QString(GUITESTING_REPORT_PREFIX) + ": " + result).toUtf8().data());
}

void GUITestService::clearSandbox() {
    log.trace("GUITestService __ clearSandbox");

    QString pathToSandbox = UGUITest::testDir + "_common_data/scenarios/sandbox/";
    QDir sandbox(pathToSandbox);

    foreach (QString fileName, sandbox.entryList()) {
        if (fileName != "." && fileName != "..") {
            if (QFile::remove(pathToSandbox + fileName))
                continue;
            else {
                QDir dir(pathToSandbox + fileName);
                removeDir(dir.absolutePath());
            }
        }
    }
}

void GUITestService::removeDir(QString dirName) {
    QDir dir(dirName);

    foreach (QFileInfo fileInfo, dir.entryInfoList()) {
        QString fileName = fileInfo.fileName();
        QString filePath = fileInfo.filePath();
        if (fileName != "." && fileName != "..") {
            if (QFile::remove(filePath))
                continue;
            else {
                QDir childDir(filePath);
                if (childDir.rmdir(filePath))
                    continue;
                else
                    removeDir(filePath);
            }
        }
    }
    dir.rmdir(dir.absoluteFilePath(dirName));
}

void GUITestService::sl_testThreadFinish() {
    auto testThread = qobject_cast<GUITestThread*>(sender());
    SAFE_POINT(testThread != nullptr, "testThread is NULL", );
    HI::GUITest* test = testThread->getTest();
    SAFE_POINT(test != nullptr, "GUITest is NULL", );
    if (isTeamcityLogOn) {
        QString testNameForTeamCity = test->suite + "_" + test->name;
        GUITestTeamcityLogger::teamCityLogResult(testNameForTeamCity, testThread->getTestResult(), -1);
    }
    sender()->deleteLater();
    AppContext::getMainWindow()->getQMainWindow()->close();
}

}  // namespace U2
