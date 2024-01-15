/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "TestRunnerPlugin.h"

#include <QMenu>

#include <U2Core/CMDLineCoreOptions.h>
#include <U2Core/CMDLineUtils.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/Log.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/MainWindow.h>

#include <U2Test/GTest.h>

#include "GTestScriptWrapper.h"
#include "TestViewController.h"

#define SETTINGS_ROOT QString("test_runner/")
/** Default number of threads to run XML tests. */
#define NUM_THREADS_VAR_VALUE "1"
#define TIME_OUT_VAR_VALUE "0"

namespace U2 {

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    if (AppContext::getMainWindow()) {
        TestRunnerPlugin* plug = new TestRunnerPlugin();
        return plug;
    }
    return nullptr;
}

TestRunnerPlugin::TestRunnerPlugin()
    : Plugin(tr("Test Runner"), tr("Support for running runs XML tests from GUI & console interfaces.")) {
    if (AppContext::getCMDLineRegistry()->hasParameter(CMDLineCoreOptions::SUITE_URLS)) {
        connect(AppContext::getPluginSupport(), SIGNAL(si_allStartUpPluginsLoaded()), SLOT(sl_startTestRunner()));
    } else {
        services.push_back(new TestRunnerService());
    }
}

void TestRunnerPlugin::sl_startTestRunner() {
    // Wait until external tools validation is finished if needed.
    ExternalToolManager* externalToolManager = AppContext::getExternalToolRegistry()->getManager();
    if (externalToolManager != nullptr && externalToolManager->isInStartupValidationMode()) {
        connect(externalToolManager, SIGNAL(si_startupValidationFinished()), SLOT(sl_startTestRunner()));
        return;
    }
    QStringList suiteUrls = CMDLineRegistryUtils::getParameterValuesByWords(CMDLineCoreOptions::SUITE_URLS);

    auto testRunnerService = new TestRunnerService();
    testRunnerService->setEnvironment();

    CMDLineRegistry* cmdReg = AppContext::getCMDLineRegistry();
    if (cmdReg->hasParameter(CMDLineCoreOptions::TEST_THREADS)) {
        QString val = cmdReg->getParameterValue(CMDLineCoreOptions::TEST_THREADS);
        bool isOk;
        val.toInt(&isOk);
        if (!isOk) {
            printf("Incorrect number of threads\n");
            AppContext::getTaskScheduler()->cancelAllTasks();
            AppContext::getMainWindow()->getQMainWindow()->close();
            return;
        }
        testRunnerService->setVar(NUM_THREADS_VAR, val);
    }

    foreach (const QString& param, suiteUrls) {
        QString dir;
        if (param.contains(":") || param[0] == '.' || param[0] == '/') {
            dir = param;
        } else {
            dir = "../../test/";
            dir.append(param);
        }

        if (param.split(".").last() == "xml") {
            QString error;
            GTestSuite* suite = GTestSuite::readTestSuite(dir, error);
            if (error != "") {
                printf("%s\n", tr("Can't load suite %1").arg(param).toLocal8Bit().constData());
                AppContext::getTaskScheduler()->cancelAllTasks();
                AppContext::getMainWindow()->getQMainWindow()->close();
                return;
            }
            testRunnerService->addTestSuite(suite);
        } else {
            if (param.split(".").last() == "list") {
                QStringList errorMessageList;
                QList<GTestSuite*> testSuiteList = GTestSuite::readTestSuiteList(dir, errorMessageList);
                if (!errorMessageList.isEmpty()) {
                    QString errorDetails = errorMessageList.join("\n");
                    printf("%s\n", tr("Can't load suite %1, errors: %2").arg(param).arg(errorDetails).toLocal8Bit().constData());
                    AppContext::getTaskScheduler()->cancelAllTasks();
                    AppContext::getMainWindow()->getQMainWindow()->close();

                    return;
                }
                for (GTestSuite* testSuite : qAsConst(testSuiteList)) {
                    QString testSuiteUrl = testSuite->getURL();
                    if (testRunnerService->findTestSuiteByURL(testSuiteUrl) != nullptr) {
                        delete testSuite;  // duplicate.
                        continue;
                    }
                    testRunnerService->addTestSuite(testSuite);
                }
            } else {
                printf("Not a test suite\n");
                AppContext::getTaskScheduler()->cancelAllTasks();
                AppContext::getMainWindow()->getQMainWindow()->close();
                return;
            }
        }
    }

    auto view = new TestViewController(testRunnerService, true);
    AppContext::getMainWindow()->getMDIManager()->addMDIWindow(view);
    view->sl_runAllSuitesAction();
}

//////////////////////////////////////////////////////////////////////////
// service
TestRunnerService::TestRunnerService()
    : Service(Service_TestRunner, tr("Test runner"), tr("Service to support UGENE embedded testing")), env(nullptr) {
    windowAction = nullptr;
    view = nullptr;
}

TestRunnerService::~TestRunnerService() {
    assert(suites.isEmpty());
}

void TestRunnerService::setVar(const QString& varName, const QString& val) {
    env->setVar(varName, val);
}

void TestRunnerService::setEnvironment() {
    env = new GTestEnvironment();
    readBuiltInVars();
}

void TestRunnerService::serviceStateChangedCallback(ServiceState oldState, bool enabledStateChanged) {
    Q_UNUSED(oldState);
    CHECK(enabledStateChanged, );
    if (isEnabled()) {
        SAFE_POINT(view == nullptr, "View must be null!", );
        SAFE_POINT(windowAction == nullptr, "windowAction must be null!", );

        env = new GTestEnvironment();
        readSavedSuites();
        readBuiltInVars();

        windowAction = new QAction(tr("XML Test runner"), this);
#ifdef _DEBUG
        windowAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_H));
#endif
        windowAction->setObjectName("action__testrunner");
        connect(windowAction, SIGNAL(triggered()), SLOT(sl_showWindow()));
        AppContext::getMainWindow()->getTopLevelMenu(MWMENU_TOOLS)->addAction(windowAction);
    } else {
        assert(windowAction != nullptr);
        delete windowAction;
        windowAction = nullptr;

        saveSuites();
        saveEnv();
        deallocateSuites();
        delete env;
        env = nullptr;

        if (view != nullptr) {
            AppContext::getMainWindow()->getMDIManager()->closeMDIWindow(view);
            assert(view == nullptr);
        }
    }
}

void TestRunnerService::sl_showWindow() {
    assert(isEnabled());
    if (view == nullptr) {
        view = new TestViewController(this);
        view->installEventFilter(this);
        AppContext::getMainWindow()->getMDIManager()->addMDIWindow(view);
    }
    AppContext::getMainWindow()->getMDIManager()->activateWindow(view);
}

bool TestRunnerService::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::Close && obj == view) {
        view = nullptr;
    }
    return QObject::eventFilter(obj, event);
}

void TestRunnerService::addTestSuite(GTestSuite* ts) {
    assert(!findTestSuiteByURL(ts->getURL()));
    assert(!suites.contains(ts));
    suites.append(ts);

    GTestEnvironment* tsEnv = ts->getEnv();
    const QStringList& tsEnvKeys = tsEnv->getVars().keys();
    QStringList tsEnvResultedKeys;
    // skipping non-empty variables
    foreach (const QString& key, tsEnvKeys) {
        if (tsEnv->getVar(key).isEmpty()) {
            tsEnvResultedKeys.push_back(key);
        }
    }
    readEnvForKeys(tsEnvResultedKeys);
    saveSuites();

    emit si_testSuiteAdded(ts);
}

void TestRunnerService::removeTestSuite(GTestSuite* ts) {
    assert(suites.contains(ts));
    suites.removeOne(ts);

    // todo: cleanup vars, but leave built-in
    saveEnv();
    saveSuites();

    emit si_testSuiteRemoved(ts);
}

GTestSuite* TestRunnerService::findTestSuiteByURL(const QString& url) {
    foreach (GTestSuite* t, suites) {
        if (t->getURL() == url) {
            return t;
        }
    }
    return nullptr;
}

void TestRunnerService::readBuiltInVars() {
    QStringList biVars;
    biVars << NUM_THREADS_VAR;
    biVars << "COMMON_DATA_DIR";
    biVars << "TEMP_DATA_DIR";
    readEnvForKeys(biVars);

    QMap<QString, QString> vars = env->getVars();
    if (!vars.contains(NUM_THREADS_VAR) || vars.value(NUM_THREADS_VAR).isEmpty()) {
        env->setVar(NUM_THREADS_VAR, NUM_THREADS_VAR_VALUE);
    }
    if (!vars.contains(TIME_OUT_VAR) || vars.value(TIME_OUT_VAR).isEmpty()) {
        env->setVar(TIME_OUT_VAR, TIME_OUT_VAR_VALUE);
    }
    if (!vars.contains("COMMON_DATA_DIR") || vars.value("COMMON_DATA_DIR").isEmpty()) {
        QString commonDataDir = qgetenv("COMMON_DATA_DIR");
        env->setVar("COMMON_DATA_DIR", commonDataDir.isEmpty() ? "/_common_data" : commonDataDir);
    }
    if (!vars.contains("TEMP_DATA_DIR") || vars.value("TEMP_DATA_DIR").isEmpty()) {
        QString tempDataDir = qgetenv("TEMP_DATA_DIR");
        env->setVar("TEMP_DATA_DIR", tempDataDir.isEmpty() ? "/_tmp" : tempDataDir);
    }
}

void TestRunnerService::readSavedSuites() {
    // TODO: do it in in service startup task!!!

    QStringList suiteUrls = AppContext::getSettings()->getValue(SETTINGS_ROOT + "suites", QStringList()).toStringList();
    for (const QString& suiteUrl : qAsConst(suiteUrls)) {
        QString err;
        GTestSuite* ts = GTestSuite::readTestSuite(suiteUrl, err);
        if (ts == nullptr) {
            ioLog.error(tr("Error reading test suite from %1. Error: %2").arg(suiteUrl).arg(err));
        } else {
            addTestSuite(ts);
        }
    }
}

void TestRunnerService::saveSuites() {
    QStringList list;
    foreach (GTestSuite* s, suites) {
        list.append(s->getURL());
    }
    AppContext::getSettings()->setValue(SETTINGS_ROOT + "suites", list);
}

void TestRunnerService::deallocateSuites() {
    foreach (GTestSuite* s, suites) {
        emit si_testSuiteRemoved(s);
        delete s;
    }
    suites.clear();
}

void TestRunnerService::readEnvForKeys(QStringList keys) {
    foreach (const QString& k, keys) {
        QString val = env->getVar(k);
        if (val.isEmpty()) {
            val = AppContext::getSettings()->getValue(SETTINGS_ROOT + "env/" + k, QString()).toString();
            env->setVar(k, val);
        }
    }
}

void TestRunnerService::saveEnv() {
    foreach (const QString& k, env->getVars().keys()) {
        QString val = env->getVar(k);
        if (!val.isEmpty()) {
            AppContext::getSettings()->setValue(SETTINGS_ROOT + "env/" + k, val);
        } else {
            AppContext::getSettings()->remove(SETTINGS_ROOT + "env/" + k);
        }
    }
}

void TestRunnerService::sl_refresh() {
    saveSuites();
    deallocateSuites();
    readSavedSuites();
}
//////////////////////////////////////////////////////////////////////////
// Script Module
/*void TestRunnerScriptModule::setup(QScriptEngine *engine) const{
    GTestScriptWrapper::setQTest(engine);
}*/
}  // namespace U2
