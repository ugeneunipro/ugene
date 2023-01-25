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

#include "TestRunnerTask.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

#include <U2Core/AppContext.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/Log.h>
#include <U2Core/Timer.h>

#include <U2Test/GTest.h>
#include <U2Test/GTestFrameworkComponents.h>

namespace U2 {

static Logger teamcityLog(ULOG_CAT_TEAMCITY);

TestRunnerTask::TestRunnerTask(const QList<GTestState*>& tests, const GTestEnvironment* _env, int testSizeToRun)
    : Task(tr("Test runner"), TaskFlag_NoRun), env(_env) {
    tpm = Task::Progress_Manual;

    setMaxParallelSubtasks(testSizeToRun);
    sizeToRun = testSizeToRun;
    finishedTests = 0;

    awaitingTests = tests;
    totalTests = tests.size();
    foreach (GTestState* t, awaitingTests) {
        t->clearState();
    }
    for (int i = 0; !awaitingTests.isEmpty() && i < sizeToRun; i++) {
        GTestState* t = awaitingTests.takeFirst();
        LoadTestTask* lt = new LoadTestTask(t);
        addSubTask(lt);
    }
}

QList<Task*> TestRunnerTask::onSubTaskFinished(Task* subTask) {
    // update progress info: progress is a % of finished tests

    stateInfo.progress = 100 * finishedTests / totalTests;

    QList<Task*> res;
    if (isCanceled()) {
        return res;
    }
    auto loadTestTask = qobject_cast<LoadTestTask*>(subTask);
    if (loadTestTask == nullptr) {
        auto test = qobject_cast<GTest*>(subTask);
        assert(test);
        test->cleanup();
        if (!test->hasError()) {
            test->removeTempDir();
        }
        GTestState* testState = stateByTest.value(test);
        assert(testState != nullptr);
        assert(testState->isNew());
        GTestRef* testRef = testState->getTestRef();
        GTestSuite* testSuite = testRef->getSuite();
        if (test->hasError()) {
            testState->setFailed(test->getStateInfo().getError());
            teamcityLog.info(QString("##teamcity[testStarted name='%1 : %2']").arg(testSuite->getName(), testRef->getShortName()));
            teamcityLog.info(QString("##teamcity[testFailed name='%1 : %2' message='%3' details='%3']").arg(testSuite->getName(), testRef->getShortName(), QString(testState->getErrorMessage()).replace("'", "|'").replace('\n', ' ')));
            teamcityLog.info(QString("##teamcity[testFinished name='%1 : %2']").arg(testSuite->getName(), testRef->getShortName()));
        } else {
            testState->setPassed();
            teamcityLog.info(QString("##teamcity[testStarted name='%1 : %2']").arg(testSuite->getName(), testRef->getShortName()));
            teamcityLog.info(QString("##teamcity[testFinished name='%1 : %2' duration='%3']").arg(testSuite->getName(), testRef->getShortName(), QString::number(GTimer::millisBetween(test->getTimeInfo().startTime, test->getTimeInfo().finishTime))));
        }
        if (!awaitingTests.isEmpty()) {
            GTestState* t = awaitingTests.takeFirst();
            res.append(new LoadTestTask(t));
        }
    } else {
        finishedTests++;
        GTestState* testState = loadTestTask->testState;
        GTestRef* testRef = testState->getTestRef();
        GTestSuite* suite = testRef->getSuite();
        if (loadTestTask->hasError()) {
            testState->setFailed(loadTestTask->getStateInfo().getError());
            teamcityLog.info(QString("##teamcity[testStarted name='%1 : %2']").arg(suite->getName(), testRef->getShortName()));
            teamcityLog.info(QString("##teamcity[testFailed name='%1 : %2' message='%3' details='%3']").arg(suite->getName(), testRef->getShortName(), QString(testState->getErrorMessage()).replace("'", "|'").replace('\n', ' ')));
            teamcityLog.info(QString("##teamcity[testFinished name='%1 : %2']").arg(suite->getName(), testRef->getShortName()));
        } else {
            GTestFormatId testFormatId = testRef->getFormatId();
            GTestFormat* testFormat = AppContext::getTestFramework()->getTestFormatRegistry()->findFormat(testFormatId);
            if (testFormat == nullptr) {
                testState->setFailed(tr("Test format not supported: %1").arg(testFormatId));
                teamcityLog.info(QString("##teamcity[testStarted name='%1 : %2']").arg(suite->getName(), testRef->getShortName()));
                teamcityLog.info(QString("##teamcity[testFailed name='%1 : %2' message='%3' details='%3']").arg(suite->getName(), testRef->getShortName(), QString(testState->getErrorMessage()).replace("'", "|'").replace('\n', ' ').replace('\n', ' ')));
                teamcityLog.info(QString("##teamcity[testFinished name='%1 : %2']").arg(suite->getName(), testRef->getShortName()));
            } else {
                auto testEnv = new GTestEnvironment();
                allTestEnvironments << testEnv;

                // Copy parent env values.
                GTestEnvironment* testParentEnv = suite->getEnv();
                QMap<QString, QString> parentVars = testParentEnv->getVars();
                QList<QString> parentVarsNames = parentVars.keys();
                for (const QString& parentVar : qAsConst(parentVarsNames)) {
                    testEnv->setVar(parentVar, parentVars[parentVar]);
                }

                // Override existing variables with the global ones.
                QMap<QString, QString> globalEnvVars = env->getVars();
                QList<QString> globalEnvKeys = globalEnvVars.keys();
                for (const QString& var : qAsConst(globalEnvKeys)) {
                    testEnv->setVar(var, globalEnvVars[var]);
                }

                // Set custom per-test environment variables.
                if (testEnv->getVars().contains("TEMP_DATA_DIR")) {
                    QString tempDir = testEnv->getVar("TEMP_DATA_DIR") + "/" +
                                      GUrlUtils::fixFileName(suite->getName()) + "/" +
                                      GUrlUtils::fixFileName(testRef->getShortName());
                    testEnv->setVar("TEMP_DATA_DIR", tempDir);
                }

                if (testEnv->getVar("WORKFLOW_OUTPUT_DIR").isEmpty()) {
                    testEnv->setVar("WORKFLOW_OUTPUT_DIR", testEnv->getVar("TEMP_DATA_DIR"));
                }

                QDir tmpDir(testEnv->getVar("TEMP_DATA_DIR"));
                if (!tmpDir.exists()) {
                    tmpDir.mkpath(tmpDir.absolutePath());
                } else {
                    taskLog.info(QString("Warning: the test temp dir already exists: %1").arg(tmpDir.path()));
                }

                QString ugeneDataPath = QFileInfo(QCoreApplication::applicationDirPath(), "data").absoluteFilePath();
                QString workflowSamplePath = ugeneDataPath + "/workflow_samples/";
                testEnv->setVar("WORKFLOW_SAMPLES_DIR", workflowSamplePath);

                QString ugeneSamplesPath = ugeneDataPath + "/samples/";
                testEnv->setVar("SAMPLE_DATA_DIR", ugeneSamplesPath);

                QString testCaseDir = QFileInfo(testRef->getURL()).absoluteDir().absolutePath();
                testEnv->setVar("LOCAL_DATA_DIR", testCaseDir + "/_input/");
                testEnv->setVar("EXPECTED_OUTPUT_DIR", testCaseDir + "/_expected/");

                // Create the test
                QString err;
                GTest* test = testFormat->createTest(testRef->getShortName(), nullptr, testEnv, loadTestTask->testData, err);
                if (test == nullptr) {
                    testState->setFailed(err);
                    teamcityLog.info(QString("##teamcity[testStarted name='%1 : %2']").arg(suite->getName(), testRef->getShortName()));
                    teamcityLog.info(QString("##teamcity[testFailed name='%1 : %2' message='%3' details='%3']").arg(suite->getName(), testRef->getShortName(), QString(testState->getErrorMessage()).replace("'", "|'").replace('\n', ' ')));
                    teamcityLog.info(QString("##teamcity[testFinished name='%1 : %2']").arg(suite->getName(), testRef->getShortName()));
                } else {
                    int suiteTimeoutSeconds = suite->getTestTimeout();
                    test->setTimeOut(suiteTimeoutSeconds);
                    stateByTest[test] = testState;
                    QString var = env->getVar(TIME_OUT_VAR);
                    int globalTestTimeoutSeconds = var.toInt();
                    if (globalTestTimeoutSeconds > 0) {
                        test->setTimeOut(globalTestTimeoutSeconds);
                    }
                    res.append(test);
                }
            }
        }
    }
    return res;
}

void TestRunnerTask::cleanup() {
    qDeleteAll(allTestEnvironments);
    allTestEnvironments.clear();
    Task::cleanup();
}

LoadTestTask::LoadTestTask(GTestState* _testState)
    : Task(tr("TestLoader for %1").arg(_testState->getTestRef()->getShortName()), TaskFlag_None), testState(_testState) {
}

void LoadTestTask::run() {
    const QString& url = testState->getTestRef()->getURL();
    QFile f(url);
    if (!f.open(QIODevice::ReadOnly)) {
        stateInfo.setError(tr("Cannot open file: %1").arg(url));
        return;
    }
    testData = f.readAll();
    f.close();
}

}  // namespace U2
