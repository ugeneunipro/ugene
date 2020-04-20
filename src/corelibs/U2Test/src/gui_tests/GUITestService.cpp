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

#include <QDir>
#include <QMainWindow>
#include <QScreen>

#include <U2Core/AppContext.h>
#include <U2Core/CMDLineCoreOptions.h>
#include <U2Core/CMDLineRegistry.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/GObject.h>
#include <U2Core/Log.h>
#include <U2Core/Settings.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/TaskStarter.h>
#include <U2Core/Timer.h>
#include <U2Core/U2SafePoints.h>

#include <core/MainThreadRunnable.h>
#include "UGUITestBase.h"
#include <core/GUITestOpStatus.h>
#include "GUITestService.h"
#include "GUITestTeamcityLogger.h"
#include "GUITestThread.h"
#include "GUITestWindow.h"
#include "UGUITest.h"

namespace U2 {

#define ULOG_CAT_TEAMCITY "Teamcity Log"
static Logger log(ULOG_CAT_TEAMCITY);
const QString GUITestService::GUITESTING_REPORT_PREFIX = "GUITesting";
const qint64 GUITestService::TIMER_INTERVAL = 100;

GUITestService::GUITestService(QObject *) :
    Service(Service_GUITesting, tr("GUI test viewer"), tr("Service to support UGENE GUI testing")),
    runTestsAction(NULL),
    testLauncher(NULL),
    needTeamcityLog(false)
{
    connect(AppContext::getPluginSupport(), SIGNAL(si_allStartUpPluginsLoaded()), SLOT(sl_allStartUpPluginsLoaded()));
}

GUITestService::~GUITestService() {
    delete runTestsAction;
}

GUITestService *GUITestService::getGuiTestService() {
    QList<Service *> services = AppContext::getServiceRegistry()->findServices(Service_GUITesting);
    return services.isEmpty() ? NULL : qobject_cast<GUITestService *>(services.first());
}

void GUITestService::sl_registerService() {
    registerServiceTask();
}

void GUITestService::sl_serviceRegistered() {
    const LaunchOptions launchedFor = getLaunchOptions(AppContext::getCMDLineRegistry());

    switch (launchedFor) {
        case RUN_ONE_TEST:
            QTimer::singleShot(1000, this, SLOT(runGUITest()));
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

        case CREATE_GUI_TEST:
            new GUITestingWindow();
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

    if(cmdLine->hasParameter(CMDLineCoreOptions::CREATE_GUI_TEST)){
        result = CREATE_GUI_TEST;
    } else if (cmdLine->hasParameter(CMDLineCoreOptions::LAUNCH_GUI_TEST)) {
        QString paramValue = cmdLine->getParameterValue(CMDLineCoreOptions::LAUNCH_GUI_TEST);
        if (!paramValue.isEmpty()) {
            result = RUN_ONE_TEST;
        } else {
            result = RUN_ALL_TESTS;
        }
    } else if (cmdLine->hasParameter(CMDLineCoreOptions::LAUNCH_GUI_TEST_BATCH)) {
        result = RUN_ALL_TESTS_BATCH;
    } else if (cmdLine->hasParameter(CMDLineCoreOptions::LAUNCH_GUI_TEST_SUITE)) {
        result = RUN_TEST_SUITE;
    } else if (cmdLine->hasParameter(CMDLineCoreOptions::LAUNCH_GUI_TEST_NO_IGNORED)) {
        result = RUN_ALL_TESTS_NO_IGNORED;
    } else if (cmdLine->hasParameter(CMDLineCoreOptions::LAUNCH_GUI_TEST_CRAZY_USER)) {
        result = RUN_CRAZY_USER_MODE;
    }
    if (result !=  NONE) {
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
    SAFE_POINT(NULL == testLauncher,"",NULL);

    Task *task = new GUITestLauncher(suiteNumber, noIgnored);
    return task;
}

void GUITestService::registerTestSuiteTask(){
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
    if(!ok){
        QString pathToSuite = cmdLine->getParameterValue(CMDLineCoreOptions::LAUNCH_GUI_TEST_SUITE);
        Task *task = !useSameIni ?
                     new GUITestLauncher(pathToSuite) :
                     new GUITestLauncher(pathToSuite, false, iniTemplate);
        Q_ASSERT(task);
        return task;
    }

    Task *task = !useSameIni ?
                 new GUITestLauncher(suiteNumber) :
                 new GUITestLauncher(suiteNumber, false, iniTemplate);
    Q_ASSERT(task);

    return task;
}

GUITests GUITestService::preChecks() {

    UGUITestBase* tb = AppContext::getGUITestBase();
    SAFE_POINT(NULL != tb,"",GUITests());

    GUITests additionalChecks = tb->takeTests(UGUITestBase::PreAdditional);
    SAFE_POINT(additionalChecks.size()>0,"",GUITests());

    return additionalChecks;
}

GUITests GUITestService::postChecks() {

    UGUITestBase* tb = AppContext::getGUITestBase();
    SAFE_POINT(NULL != tb,"",GUITests());

    GUITests additionalChecks = tb->takeTests(UGUITestBase::PostAdditionalChecks);
    SAFE_POINT(additionalChecks.size()>0,"",GUITests());

    return additionalChecks;
}

GUITests GUITestService::postActions() {

    UGUITestBase* tb = AppContext::getGUITestBase();
    SAFE_POINT(NULL != tb,"",GUITests());

    GUITests additionalChecks = tb->takeTests(UGUITestBase::PostAdditionalActions);
    SAFE_POINT(additionalChecks.size()>0,"",GUITests());

    return additionalChecks;
}

void GUITestService::sl_allStartUpPluginsLoaded() {
    if (!connect(AppContext::getExternalToolRegistry()->getManager(), SIGNAL(si_startupChecksFinish()), SLOT(sl_registerService()))) {
        coreLog.error(tr("Can't connect external tool manager signal"));
        sl_registerService();
    }
}

void GUITestService::runAllGUITests() {

    GUITests initTests = preChecks();
    GUITests postChecksTests = postChecks();
    GUITests postActiosTests = postActions();

    GUITests tests = AppContext::getGUITestBase()->takeTests();
    SAFE_POINT(false == tests.isEmpty(),"",);

    foreach(HI::GUITest* t, tests) {
        SAFE_POINT(NULL != t,"",);
        if (!t) {
            continue;
        }
        QString testName = t->getFullName();
        QString testNameForTeamCity = t->getSuite() +"_"+ t->getName();

        if (t->isIgnored()) {
            GUITestTeamcityLogger::testIgnored(testNameForTeamCity, t->getIgnoreMessage());
            continue;
        }

        qint64 startTime = GTimer::currentTimeMicros();
        GUITestTeamcityLogger::testStarted(testNameForTeamCity);

        HI::GUITestOpStatus os;
        log.trace("GTRUNNER - runAllGUITests - going to run initial checks before " + testName);
        foreach(HI::GUITest* t, initTests) {
            if (t) {
                t->run(os);
            }
        }

        clearSandbox();
        log.trace("GTRUNNER - runAllGUITests - going to run test " + testName);
        t->run(os);
        log.trace("GTRUNNER - runAllGUITests - finished running test " + testName);

        foreach(HI::GUITest* t, postChecksTests) {
            if (t) {
                t->run(os);
            }
        }

        HI::GUITestOpStatus os2;
        foreach(HI::GUITest* t, postActiosTests) {
            if (t) {
                t->run(os2);
            }
        }

        QString testResult = os.hasError() ? os.getError() : GUITestTeamcityLogger::successResult;

        qint64 finishTime = GTimer::currentTimeMicros();
        GUITestTeamcityLogger::teamCityLogResult(testNameForTeamCity, testResult, GTimer::millisBetween(startTime, finishTime));
    }

    log.trace("GTRUNNER - runAllGUITests - shutting down UGENE");
    AppContext::getTaskScheduler()->cancelAllTasks();
    AppContext::getMainWindow()->getQMainWindow()->close();
}

void GUITestService::runGUITest() {

    CMDLineRegistry* cmdLine = AppContext::getCMDLineRegistry();
    SAFE_POINT(NULL != cmdLine,"",);
    QString testName = cmdLine->getParameterValue(CMDLineCoreOptions::LAUNCH_GUI_TEST);
    needTeamcityLog = cmdLine->hasParameter(CMDLineCoreOptions::TEAMCITY_OUTPUT);

    UGUITestBase *tb = AppContext::getGUITestBase();
    SAFE_POINT(NULL != tb, "Test base is NULL", );
    HI::GUITest *t = tb->takeTest(testName.split(":").first(), testName.split(":").last());
    SAFE_POINT(NULL != t, QString("Test '%1' is NULL. A wrong test name?").arg(testName), );

    runGUITest(t);
}

void GUITestService::runGUICrazyUserTest() {
    UGUITestBase *tb = AppContext::getGUITestBase();
    SAFE_POINT(tb,"",);
    HI::GUITest *t = tb->takeTest("","simple_crazy_user");

    runGUITest(t);
}

void GUITestService::runGUITest(HI::GUITest *test) {
    SAFE_POINT(NULL != test, "GUITest is NULL", );
    if(needTeamcityLog){
        QString testNameForTeamCity = test->getSuite() +"_"+ test->getName();
        GUITestTeamcityLogger::testStarted(testNameForTeamCity);
    }

    GUITestThread *testThread = new GUITestThread(test, log);
    connect(testThread, SIGNAL(finished()), SLOT(sl_testThreadFinish()));
    testThread->start();
}

void GUITestService::registerServiceTask() {
    Task *registerServiceTask = AppContext::getServiceRegistry()->registerServiceTask(this);
    SAFE_POINT(NULL != registerServiceTask, "registerServiceTask is NULL", );
    connect(new TaskSignalMapper(registerServiceTask), SIGNAL(si_taskFinished(Task *)), SLOT(sl_serviceRegistered()));

    AppContext::getTaskScheduler()->registerTopLevelTask(registerServiceTask);
}

void GUITestService::serviceStateChangedCallback(ServiceState, bool enabledStateChanged) {

    if (!enabledStateChanged) {
        return;
    }
}

void GUITestService::sl_registerTestLauncherTask() {

    registerAllTestsTask();
}

void GUITestService::sl_taskStateChanged(Task* t) {

    if (t != testLauncher) {
        return;
    }
    if (!t->isFinished()) {
        return;
    }

    testLauncher = NULL;
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

void GUITestService::clearSandbox()
{
    log.trace("GUITestService __ clearSandbox");

    QString pathToSandbox = UGUITest::testDir + "_common_data/scenarios/sandbox/";
    QDir sandbox(pathToSandbox);

    foreach (QString fileName, sandbox.entryList()) {
        if (fileName != "." && fileName != "..") {
            if(QFile::remove(pathToSandbox + fileName))
                continue;
            else{
                QDir dir(pathToSandbox + fileName);
                removeDir(dir.absolutePath());
            }
        }
    }
}

void GUITestService::removeDir(QString dirName)
{
    QDir dir(dirName);


    foreach (QFileInfo fileInfo, dir.entryInfoList()) {
        QString fileName = fileInfo.fileName();
        QString filePath = fileInfo.filePath();
        if (fileName != "." && fileName != "..") {
            if(QFile::remove(filePath))
                continue;
            else{
                QDir dir(filePath);
                if(dir.rmdir(filePath))
                    continue;
                else
                    removeDir(filePath);
            }

        }
    }dir.rmdir(dir.absoluteFilePath(dirName));
}

void GUITestService::sl_testThreadFinish() {
    GUITestThread* testThread = qobject_cast<GUITestThread*>(sender());
    SAFE_POINT(NULL != testThread, "testThread is NULL", );
    HI::GUITest* test = testThread->getTest();
    SAFE_POINT(NULL != test, "GUITest is NULL", );
    if(needTeamcityLog){
        QString testNameForTeamCity = test->getSuite() +"_"+ test->getName();
        GUITestTeamcityLogger::teamCityLogResult(testNameForTeamCity, testThread->getTestResult(), -1);
    }
    sender()->deleteLater();
    AppContext::getMainWindow()->getQMainWindow()->close();
}

}
