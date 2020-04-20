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

#ifndef _U2_GUI_TEST_SERVICE_H_
#define _U2_GUI_TEST_SERVICE_H_

#include <U2Core/MultiTask.h>
#include <U2Core/Task.h>
#include <U2Core/global.h>

#include <U2Gui/MainWindow.h>

#include <core/GUITest.h>
#include "GUITestLauncher.h"
//#include "MainThreadTimer.h"

namespace U2 {

class CMDLineRegistry;
class GUITestLauncher;
class GUITestService;
class MainThreadRunnable;

class U2TEST_EXPORT GUITestService : public Service {
    Q_OBJECT
public:
    enum LaunchOptions {NONE, RUN_ONE_TEST, RUN_ALL_TESTS, RUN_ALL_TESTS_BATCH, RUN_TEST_SUITE, RUN_CRAZY_USER_MODE, CREATE_GUI_TEST, RUN_ALL_TESTS_NO_IGNORED};

    GUITestService(QObject *parent = NULL);
    ~GUITestService();

    void runTest(HI::GUITests testsToRun);
//    qint64 getMainThreadTimerValue() const;

    static GUITestService * getGuiTestService();    // the service should be already created and registered

    static void setEnvVariablesForGuiTesting();
    static LaunchOptions getLaunchOptions(CMDLineRegistry* cmdLine);
    static bool isGuiTestServiceNeeded();

    static const QString GUITESTING_REPORT_PREFIX;
    static const qint64 TIMER_INTERVAL;

public slots:
     void runGUICrazyUserTest();
     void runGUITest();
     void runGUITest(HI::GUITest* t);

    void runAllGUITests();

private slots:
    void sl_allStartUpPluginsLoaded();
    void sl_registerService();
    void sl_serviceRegistered();
    void sl_registerTestLauncherTask();
    void sl_taskStateChanged(Task*);
    void sl_testThreadFinish();

private:
    virtual void serviceStateChangedCallback(ServiceState oldState, bool enabledStateChanged);

    static HI::GUITests preChecks();
    static HI::GUITests postChecks();
    static HI::GUITests postActions();

    static void clearSandbox();
    static void removeDir(QString dirName);

    void registerAllTestsTask();
    void registerAllTestsTaskNoIgnored();
    void registerTestSuiteTask();
    void registerServiceTask();

    Task* createTestLauncherTask(int suiteNumber = 0, bool noIgnored = false) const;
    Task* createTestSuiteLauncherTask() const;
    static void writeTestResult(const QString &result);

    QAction *runTestsAction;
    Task *testLauncher;
    HI::GUITests testsToRun;
    bool needTeamcityLog;
//    MainThreadTimer timer;
};

}   // namespace U2

#endif // _U2_GUI_TEST_SERVICE_H_
