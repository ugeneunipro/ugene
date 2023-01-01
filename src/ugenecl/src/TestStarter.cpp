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

#include "TestStarter.h"

#include <QProcess>

#include <U2Core/Log.h>
#include <U2Core/Settings.h>

#include <U2Test/GTest.h>
#include <U2Test/TestRunnerTask.h>

#define SETTINGS_ROOT QString("test_runner/")

namespace U2 {

TestStarter::TestStarter(const QStringList& urls)
    : Task(tr("Test loader"), TaskFlags_NR_FOSCOE), urls(urls) {
    env = new GTestEnvironment();
    readBuiltInVars();
}

TestStarter::~TestStarter() {
    cleanup();
    assert(suites.isEmpty());
}

void TestStarter::prepare() {
    if (urls.isEmpty()) {
        readSavedSuites();
    } else {
        foreach (const QString& url, urls) {
            if (url.endsWith(".list")) {
                QStringList errs;
                QList<GTestSuite*> tsl = GTestSuite::readTestSuiteList(url, errs);
                if (!errs.isEmpty()) {
                    ioLog.error("Error reading test suites: \n" + errs.join("\n"));
                } else {
                    for (GTestSuite* ts : qAsConst(tsl)) {
                        addTestSuite(ts);
                    }
                }
            } else {
                QString err;
                GTestSuite* ts = GTestSuite::readTestSuite(url, err);
                if (!err.isEmpty()) {
                    ioLog.error("Error reading test suites: \n" + err);
                } else {
                    addTestSuite(ts);
                }
            }
        }
    }

    ttask = createRunTask();
    addSubTask(ttask = createRunTask());
}

void TestStarter::cleanup() {
    // saveSuites();
    // saveEnv();
    deallocateSuites();
    delete env;
    env = nullptr;
    // Task::cleanup();
}

void TestStarter::addTestSuite(GTestSuite* ts) {
    // TO DO: check bug when test suites duplicated
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
    updateDefaultEnvValues(ts);

    emit si_testSuiteAdded(ts);
}

void TestStarter::updateDefaultEnvValues(GTestSuite*) {
    QMap<QString, QString> vars = env->getVars();
    if (vars.contains("COMMON_DATA_DIR") && vars.value("COMMON_DATA_DIR").isEmpty()) {
        QString commonDataDir = qgetenv("COMMON_DATA_DIR");
        env->setVar("COMMON_DATA_DIR", commonDataDir.isEmpty() ? "/_common_data" : commonDataDir);
    }

    if (vars.contains("TEMP_DATA_DIR") && vars.value("TEMP_DATA_DIR").isEmpty()) {
        QString tempDataDir = qgetenv("TEMP_DATA_DIR");
        env->setVar("TEMP_DATA_DIR", tempDataDir.isEmpty() ? "/_tmp" : tempDataDir);
    }
}

void TestStarter::removeTestSuite(GTestSuite* ts) {
    assert(suites.contains(ts));
    suites.removeOne(ts);

    // todo: cleanup vars, but leave built-in
    saveEnv();

    emit si_testSuiteRemoved(ts);
}

GTestSuite* TestStarter::findTestSuiteByURL(const QString& url) {
    foreach (GTestSuite* t, suites) {
        if (t->getURL() == url) {
            return t;
        }
    }
    return nullptr;
}

void TestStarter::readBuiltInVars() {
    QStringList biVars;
    biVars << NUM_THREADS_VAR;
    readEnvForKeys(biVars);

    QMap<QString, QString> vars = env->getVars();
    if (!vars.contains(NUM_THREADS_VAR) || vars.value(NUM_THREADS_VAR).isEmpty()) {
        env->setVar(NUM_THREADS_VAR, "5");
    }
    if (!vars.contains(TIME_OUT_VAR) || vars.value(TIME_OUT_VAR).isEmpty()) {
        env->setVar(TIME_OUT_VAR, "0");
    }
}

void TestStarter::readSavedSuites() {
    // TODO: do it in in service startup task!!!
    QStringList suiteUrls = AppContext::getSettings()->getValue(SETTINGS_ROOT + "suites", QStringList()).toStringList();
    for (const QString& url : qAsConst(suiteUrls)) {
        QString err;
        GTestSuite* ts = GTestSuite::readTestSuite(url, err);
        if (ts == nullptr) {
            ioLog.error(tr("error_reading_ts_%1_error_%2").arg(url).arg(err));
        } else {
            addTestSuite(ts);
        }
    }
}

void TestStarter::saveSuites() {
    QStringList list;
    foreach (GTestSuite* s, suites) {
        list.append(s->getURL());
    }
    AppContext::getSettings()->setValue(SETTINGS_ROOT + "suites", list);
}

void TestStarter::deallocateSuites() {
    foreach (GTestSuite* s, suites) {
        emit si_testSuiteRemoved(s);
        delete s;
    }
    suites.clear();
}

void TestStarter::readEnvForKeys(QStringList keys) {
    foreach (const QString& k, keys) {
        QString val = env->getVar(k);
        if (val.isEmpty()) {
            val = AppContext::getSettings()->getValue(SETTINGS_ROOT + "env/" + k, QString()).toString();
            env->setVar(k, val);
        }
    }
}

void TestStarter::saveEnv() {
    foreach (const QString& k, env->getVars().keys()) {
        QString val = env->getVar(k);
        if (!val.isEmpty()) {
            AppContext::getSettings()->setValue(SETTINGS_ROOT + "env/" + k, val);
        } else {
            AppContext::getSettings()->remove(SETTINGS_ROOT + "env/" + k);
        }
    }
}

void TestStarter::sl_refresh() {
    saveSuites();
    deallocateSuites();
    readSavedSuites();
}

TestRunnerTask* TestStarter::createRunTask() {
    if (env->containsEmptyVars()) {
        coreLog.error(tr("Not all environment variables set"));
        return nullptr;
    }
    QList<GTestState*> testsToRun;
    for (GTestSuite* ts : qAsConst(suites)) {
        foreach (GTestRef* tref, ts->getTests()) {
            testsToRun << new GTestState(tref);
        }
    }
    if (testsToRun.isEmpty()) {
        coreLog.error(tr("No tests found"));
    }

    bool ok;
    //     QTime* temp=new QTime;
    //     *temp=QTime::currentTime();
    //     startRunTime = temp;
    int numberTestsToRun = getEnv()->getVar(NUM_THREADS_VAR).toInt(&ok);
    if (!ok || numberTestsToRun <= 0) {
        numberTestsToRun = 5;
    }
    return new TestRunnerTask(testsToRun, getEnv(), numberTestsToRun);
}

Task::ReportResult TestStarter::report() {
    int numPassed = 0, numFailed = 0;
    uiLog.info(tr("Testing report:"));
    uiLog.info(tr("---------------"));
    if (ttask->isFinished() && !ttask->hasError()) {
        foreach (GTestState* t, ttask->getStateByTestMap()) {
            uiLog.info(tr("State: %1 - Test %2:%3 %4").arg(t->isPassed() ? "pass" : "FAIL").arg(t->getTestRef()->getSuite()->getName()).arg(t->getTestRef()->getShortName()).arg(t->isPassed() ? "" : "- Details: " + t->getErrorMessage()));
            if (t->isPassed()) {
                numPassed++;
            } else {
                numFailed++;
            }
            // printf("%s\n",tr("------------------Test %1 %2").arg(t->getTestRef()->getShortName()).arg(t->isPassed() ? "OK" : t->getErrorMessage()).toLocal8Bit());
        }
        uiLog.info(tr("---------------"));
        uiLog.info(tr("Summary - Number of tests: %1 | Tests passed: %2 | Tests failed: %3").arg(QString::number(numPassed + numFailed)).arg(QString::number(numPassed)).arg(QString::number(numFailed)));
    } else {
        uiLog.error("Test run problem: " + ttask->getError());
        // printf("%s\n",QString("--------- Test run problem: " + ttask->getError()).toLocal8Bit());
    }
    return ReportResult_Finished;
}

}  // namespace U2
