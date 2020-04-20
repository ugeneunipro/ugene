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

#include "TestRunnerPlugin.h"
#include "TestViewController.h"
#include "GTestScriptWrapper.h"

#include <U2Gui/MainWindow.h>
#include <U2Core/Log.h>
#include <U2Core/Settings.h>
#include <U2Core/CMDLineCoreOptions.h>
#include <U2Core/CMDLineUtils.h>

#include <U2Test/GTest.h>

#include <QMenu>
#include <QProcess>

#define SETTINGS_ROOT QString("test_runner/")
#define NUM_THREADS_VAR_VALUE "10"
#define TIME_OUT_VAR_VALUE "0"

namespace U2 {

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    if (AppContext::getMainWindow()) {
        TestRunnerPlugin* plug = new TestRunnerPlugin();
        return plug;
    }
    return NULL;
}


TestRunnerPlugin::TestRunnerPlugin() : Plugin(tr("test_runner_plug_name"), tr("test_runner_desc")) {
    if (AppContext::getCMDLineRegistry()->hasParameter( CMDLineCoreOptions::SUITE_URLS )) {
        connect(  AppContext::getPluginSupport(), SIGNAL( si_allStartUpPluginsLoaded() ), SLOT(sl_startTestRunner()));
    }
    else {
        services.push_back(new TestRunnerService());
    }
}

void TestRunnerPlugin::sl_startTestRunner() {
    CMDLineRegistry *cmdReg = AppContext::getCMDLineRegistry();
    //QString param = cmdReg->getParameterValue(CMDLineCoreOptions::SUITE_URLS);
    QStringList suiteUrls = CMDLineRegistryUtils::getParameterValuesByWords( CMDLineCoreOptions::SUITE_URLS );

    TestRunnerService *srv = new TestRunnerService();
    srv->setEnvironment();

    if(cmdReg->hasParameter(CMDLineCoreOptions::TEST_THREADS)) {
        QString val = cmdReg->getParameterValue(CMDLineCoreOptions::TEST_THREADS);
        bool isOk;
        val.toInt(&isOk);
        if(!isOk) {
            printf("Incorrect number of threads\n");
            AppContext::getTaskScheduler()->cancelAllTasks();
            AppContext::getMainWindow()->getQMainWindow()->close();
            return;
        }
        srv->setVar(NUM_THREADS_VAR, val);
    }

    foreach(const QString &param, suiteUrls) {
        QString dir;
        if(param.contains(":")||param[0]=='.'||param[0]=='/') {
            dir = param;
        }
        else {
            dir = "../../test/";
            dir.append(param);
        }

        if (param.split(".").last()=="xml") {
            QString error;
            GTestSuite *suite = GTestSuite::readTestSuite(dir,error);
            if(error!="") {
                printf("%s\n",tr("Can't load suite %1").arg(param).toLatin1().constData());
                AppContext::getTaskScheduler()->cancelAllTasks();
                AppContext::getMainWindow()->getQMainWindow()->close();
                return;
            }
            srv->addTestSuite(suite);
        } else {
            if(param.split(".").last()=="list") {
                QStringList errs;
                QList<GTestSuite*> lst = GTestSuite::readTestSuiteList(dir, errs);
                if (!errs.isEmpty()) {
                    printf("%s\n",tr("Can't load suite %1").arg(param).toLatin1().constData());
                    AppContext::getTaskScheduler()->cancelAllTasks();
                    AppContext::getMainWindow()->getQMainWindow()->close();

                    return;
                }
                foreach(GTestSuite* ts, lst){
                    QString urlfs = ts->getURL();
                    if (srv->findTestSuiteByURL(urlfs)!=NULL) {
                        delete ts;
                    }else{
                        srv->addTestSuite(ts);
                    }
                }
            }
            else {
                printf("Not a test suite\n");
                AppContext::getTaskScheduler()->cancelAllTasks();
                AppContext::getMainWindow()->getQMainWindow()->close();
                return;
            }
        }
    }

    TestViewController *view = new TestViewController(srv,true);
    AppContext::getMainWindow()->getMDIManager()->addMDIWindow(view);
    view->sl_runAllSuitesAction();
}

//////////////////////////////////////////////////////////////////////////
// service
TestRunnerService::TestRunnerService()
: Service(Service_TestRunner, tr("Test runner"), tr("Service to support UGENE embedded testing")), env(NULL)
{
    windowAction = NULL;
    view = NULL;
}

TestRunnerService::~TestRunnerService() {
    assert(suites.isEmpty());
}

void TestRunnerService::setVar(const QString& varName, const QString& val) {
    env->setVar(varName,val);
}

void TestRunnerService::setEnvironment() {
    env = new GTestEnvironment();
    readBuiltInVars();
}

void TestRunnerService::serviceStateChangedCallback(ServiceState oldState, bool enabledStateChanged) {
    Q_UNUSED(oldState);

    if (!enabledStateChanged) {
        return;
    }
    //QStringList ugene_env = QProcess::systemEnvironment();
    //bool test_runner_enabled = -1 != ugene_env.indexOf( QRegExp(QString(ENV_UGENE_DEV)+"*", Qt::CaseInsensitive, QRegExp::Wildcard) );

    if (isEnabled()) {
        assert(view==NULL);
        assert(windowAction == NULL);

        env = new GTestEnvironment();
        readSavedSuites();
        readBuiltInVars();

        windowAction = new QAction(tr("Test runner"), this);
#ifdef _DEBUG
        windowAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_H));
#endif
        windowAction->setObjectName("action__testrunner");
        connect(windowAction, SIGNAL(triggered()), SLOT(sl_showWindow()));
        AppContext::getMainWindow()->getTopLevelMenu(MWMENU_TOOLS)->addAction(windowAction);
    } else {
        assert(windowAction!=NULL);
        delete windowAction;
        windowAction = NULL;

        saveSuites();
        saveEnv();
        deallocateSuites();
        delete env;
        env = NULL;


        if (view!=NULL) {
            AppContext::getMainWindow()->getMDIManager()->closeMDIWindow(view);
            assert(view == NULL);
        }
    }
}


void TestRunnerService::sl_showWindow() {
    assert(isEnabled());
    if (view==NULL) {
        view = new TestViewController(this);
        view->installEventFilter(this);
        AppContext::getMainWindow()->getMDIManager()->addMDIWindow(view);
    }
    AppContext::getMainWindow()->getMDIManager()->activateWindow(view);
}


bool TestRunnerService::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::Close && obj == view) {
        view = NULL;
    }
    return QObject::eventFilter(obj, event);
}


void TestRunnerService::addTestSuite(GTestSuite *ts) {
    assert(!findTestSuiteByURL(ts->getURL()));
    assert(!suites.contains(ts));
    suites.append(ts);

    GTestEnvironment * tsEnv = ts->getEnv();
    const QStringList & tsEnvKeys = tsEnv->getVars().keys();
    QStringList tsEnvResultedKeys;
    //skipping non-empty variables
    foreach( const QString & key, tsEnvKeys ) {
        if( tsEnv->getVar(key).isEmpty() ) {
            tsEnvResultedKeys.push_back( key );
        }
    }
    readEnvForKeys(tsEnvResultedKeys);
    saveSuites();

    emit si_testSuiteAdded(ts);
}

void TestRunnerService::removeTestSuite(GTestSuite* ts) {
    assert(suites.contains(ts));
    suites.removeOne(ts);

    //todo: cleanup vars, but leave built-in
    saveEnv();
    saveSuites();

    emit si_testSuiteRemoved(ts);
}

GTestSuite* TestRunnerService::findTestSuiteByURL(const QString& url) {
    foreach(GTestSuite* t, suites) {
        if (t->getURL() == url) {
            return t;
        }
    }
    return NULL;
}

void TestRunnerService::readBuiltInVars() {
    QStringList biVars;
    biVars<<NUM_THREADS_VAR;
    readEnvForKeys(biVars);

    QMap<QString, QString> vars = env->getVars();
    if (!vars.contains(NUM_THREADS_VAR) || vars.value(NUM_THREADS_VAR).isEmpty()) {
        env->setVar(NUM_THREADS_VAR, NUM_THREADS_VAR_VALUE);
    }
    if (!vars.contains(TIME_OUT_VAR) || vars.value(TIME_OUT_VAR).isEmpty()) {
        env->setVar(TIME_OUT_VAR, TIME_OUT_VAR_VALUE);
    }
    if (!vars.contains("COMMON_DATA_DIR") || vars.value("COMMON_DATA_DIR").isEmpty()) {
        env->setVar("COMMON_DATA_DIR", "/_common_data");
    }
    if (!vars.contains("TEMP_DATA_DIR") || vars.value("TEMP_DATA_DIR").isEmpty()) {
        env->setVar("TEMP_DATA_DIR", "/_tmp");
    }
}

void TestRunnerService::readSavedSuites() {
    //TODO: do it in in service startup task!!!

    QStringList suiteUrls = AppContext::getSettings()->getValue(SETTINGS_ROOT + "suites", QStringList()).toStringList();
    QString err;
    QMap<QString, QString> env;
    QString url;
    foreach(const QString& url, suiteUrls) {
        GTestSuite* ts = GTestSuite::readTestSuite(url, err);
        if (ts == NULL) {
            ioLog.error(tr("Error reading test suite from %1. Error: %2").arg(url).arg(err));
        } else {
            addTestSuite(ts);
        }
    }
}

void TestRunnerService::saveSuites() {
    QStringList list;
    foreach(GTestSuite* s, suites) {
        list.append(s->getURL());
    }
    AppContext::getSettings()->setValue(SETTINGS_ROOT + "suites", list);
}

void TestRunnerService::deallocateSuites() {
    foreach(GTestSuite* s, suites) {
        emit si_testSuiteRemoved(s);
        delete s;
    }
    suites.clear();
}

void TestRunnerService::readEnvForKeys(QStringList keys) {
    foreach(const QString& k, keys) {
        QString val = env->getVar(k);
        if (val.isEmpty()) {
            val = AppContext::getSettings()->getValue(SETTINGS_ROOT + "env/"+ k, QString()).toString();
            env->setVar(k, val);
        }
    }
}

void TestRunnerService::saveEnv() {
    foreach(const QString& k, env->getVars().keys()) {
        QString val = env->getVar(k);
        if (!val.isEmpty()) {
            AppContext::getSettings()->setValue(SETTINGS_ROOT + "env/"+ k, val);
        } else {
            AppContext::getSettings()->remove(SETTINGS_ROOT + "env/"+ k);
        }
    }
}

void TestRunnerService::sl_refresh()
{
    saveSuites();
    deallocateSuites();
    readSavedSuites();
}
//////////////////////////////////////////////////////////////////////////
// Script Module
/*void TestRunnerScriptModule::setup(QScriptEngine *engine) const{
    GTestScriptWrapper::setQTest(engine);
}*/
}//namespace
