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

#include "GUITestLauncher.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QMap>
#include <QTextStream>
#include <QThread>

#include <U2Core/CMDLineCoreOptions.h>
#include <U2Core/CmdlineTaskRunner.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/Timer.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "GUITestService.h"
#include "GUITestTeamcityLogger.h"
#include "UGUITestBase.h"

namespace U2 {

GUITestLauncher::GUITestLauncher(int suiteNumber, bool noIgnored, const QString& iniFileTemplate)
    : Task("gui_test_launcher", TaskFlags(TaskFlag_ReportingIsSupported) | TaskFlag_ReportingIsEnabled),
      suiteNumber(suiteNumber), noIgnored(noIgnored), pathToSuite(""), iniFileTemplate(iniFileTemplate) {
    tpm = Task::Progress_Manual;
    testOutputDir = findAvailableTestOutputDir();

    QWidget* splashScreen = QApplication::activeWindow();
    if (splashScreen != nullptr) {
        splashScreen->hide();
    }
}

GUITestLauncher::GUITestLauncher(const QString& _pathToSuite, bool _noIgnored, const QString& _iniFileTemplate)
    : Task("gui_test_launcher", TaskFlags(TaskFlag_ReportingIsSupported) | TaskFlag_ReportingIsEnabled),
      suiteNumber(0), noIgnored(_noIgnored), pathToSuite(_pathToSuite), iniFileTemplate(_iniFileTemplate) {
    tpm = Task::Progress_Manual;
    testOutputDir = findAvailableTestOutputDir();
}

bool GUITestLauncher::renameTestLog(const QString& testName, int testRunIteration) {
    QString outFileName = getTestOutputFileName(testName, testRunIteration);
    QString outFilePath = testOutputDir + QString("/logs/");

    QFile outLog(outFilePath + outFileName);
    return outLog.rename(outFilePath + "failed_" + outFileName);
}

void GUITestLauncher::run() {
    if (!initTestList()) {
        // FIXME: if test suite can't run for some reason UGENE runs shutdown task that asserts that startup is in progress.
        //  Workaround: wait 3 seconds to ensure that startup is complete & GUI test base error message is printed.
        QThread::sleep(3);
        return;
    }
    if (testList.isEmpty()) {
        setError(tr("No tests to run"));
        return;
    }

    qint64 suiteStartMicros = GTimer::currentTimeMicros();

    int finishedCount = 0;
    for (GUITest* test : qAsConst(testList)) {
        if (isCanceled()) {
            return;
        }
        if (test == nullptr) {
            updateProgress(finishedCount++);
            continue;
        }
        QString fullTestName = test->getFullName();
        QString teamcityTestName = UGUITest::getTeamcityTestName(test->suite, test->name);
        testResultByFullTestNameMap[fullTestName] = "";

        firstTestRunCheck(fullTestName);

        bool isValidPlatform = UGUITestLabels::hasPlatformLabel(test);
        bool isIgnored = UGUITestLabels::hasIgnoredLabel(test);
        bool isSkipIgnoredCheck = noIgnored;
        if (isValidPlatform && (!isIgnored || isSkipIgnoredCheck)) {
            qint64 startTime = GTimer::currentTimeMicros();
            GUITestTeamcityLogger::testStarted(teamcityTestName);

            try {
                QString testResult = runTest(fullTestName, test->timeout);
                testResultByFullTestNameMap[fullTestName] = testResult;

                qint64 finishTime = GTimer::currentTimeMicros();
                GUITestTeamcityLogger::teamCityLogResult(teamcityTestName, testResult, GTimer::millisBetween(startTime, finishTime));
            } catch (const std::exception& exc) {
                coreLog.error("Got exception while running test: " + fullTestName);
                coreLog.error("Exception text: " + QString(exc.what()));
            }
        } else if (isValidPlatform) {
            // If the test should run on the current platform but has ignored label -> report it to the teamcity.
            GUITestTeamcityLogger::testIgnored(teamcityTestName, test->getDescription());
        }

        updateProgress(finishedCount++);
    }
    qint64 suiteEndMicros = GTimer::currentTimeMicros();
    qint64 suiteTimeMinutes = ((suiteEndMicros - suiteStartMicros) / 1000000) / 60;
    coreLog.info(QString("Suite %1 finished in %2 minutes").arg(suiteNumber).arg(suiteTimeMinutes));
}

void GUITestLauncher::firstTestRunCheck(const QString& testName) {
    QString testResult = testResultByFullTestNameMap[testName];
    Q_ASSERT(testResult.isEmpty());
}

/** Returns ideal tests list for the given suite or an empty list if there is no ideal configuration is found. */
QList<GUITest*> getIdealNightlyTestsSplit(int suiteIndex, int suiteCount, const QList<GUITest*>& allTests) {
    QList<int> testsPerSuite;
    if (suiteCount == 3) {  // Windows & Mac.
        testsPerSuite << 950 << 910 << -1;
    } else if (suiteCount == 4) {
        testsPerSuite << 640 << 680 << 640 << -1;
    } else if (suiteCount == 5) {
        testsPerSuite << 540 << 565 << 490 << 560 << -1;  // Linux.
    }
    CHECK(suiteCount == testsPerSuite.size(), {});  // Check that we know the distribution. Return an empty list if we do not.
    QList<GUITest*> tests;
    int offset = 0;
    for (int i = 0; i < suiteIndex; i++) {
        offset += testsPerSuite[i];
    }
    int testCount = testsPerSuite[suiteIndex];  // last index is -1 => list.mid(x, -1) returns a tail.
    tests << allTests.mid(offset, testCount);
    return tests;
}

bool GUITestLauncher::initTestList() {
    testList.clear();

    UGUITestBase* guiTestBase = UGUITestBase::getInstance();

    // Label set to build a run-time test set is passed via environment variable.
    QString labelEnvVar = qgetenv("UGENE_GUI_TEST_LABEL");
    QStringList labelList = labelEnvVar.isEmpty() ? QStringList() : labelEnvVar.split(",");

    if (suiteNumber != 0) {
        // If no label is provided 'Nightly' (UGUITestLabels::Nightly) label is used by default.
        if (labelList.isEmpty()) {
            labelList << UGUITestLabels::Nightly;
        }
        int testSuiteCount = 1;
        if (labelList.contains(UGUITestLabels::Nightly)) {
            // TODO: make configurable via ENV.
            testSuiteCount = isOsWindows() ? 3
                             : isOsMac()   ? 3
                             : isOsLinux() ? 5
                                           : 1;
        }

        if (suiteNumber < 1 || suiteNumber > testSuiteCount) {
            setError(QString("Invalid suite number: %1. There are %2 suites").arg(suiteNumber).arg(testSuiteCount));
            return false;
        }

        QList<GUITest*> labeledTestList = guiTestBase->getTests(UGUITestBase::Normal, labelList);
        if (labelList.contains(UGUITestLabels::Nightly)) {
            testList = getIdealNightlyTestsSplit(suiteNumber - 1, testSuiteCount, labeledTestList);
        }
        if (testList.isEmpty()) {
            // If there is no ideal test split for the given number -> distribute tests between suites evenly.
            for (int i = suiteNumber - 1; i < labeledTestList.length(); i += testSuiteCount) {
                testList << labeledTestList[i];
            }
        }
        coreLog.info(QString("Running suite %1-%2, Tests in the suite: %3, total tests: %4").arg(labelList.join(",")).arg(suiteNumber).arg(testList.size()).arg(labeledTestList.length()));
    } else if (!pathToSuite.isEmpty()) {
        // If a file with tests is specified we ignore labels and look-up in the complete tests set.
        QList<GUITest*> allTestList = guiTestBase->getTests(UGUITestBase::Normal);
        QString absPath = QDir().absoluteFilePath(pathToSuite);
        QFile suite(absPath);
        if (!suite.open(QFile::ReadOnly)) {
            setError("Can't open suite file: " + absPath);
            return false;
        }
        char buf[1024];
        while (suite.readLine(buf, sizeof(buf)) != -1) {
            QString testName = QString(buf).remove('\n').remove('\r').remove('\t').remove(' ');
            if (testName.startsWith("#") || testName.isEmpty()) {
                continue;  // comment line or empty line.
            }
            bool added = false;
            for (GUITest* test : qAsConst(allTestList)) {
                QString fullTestName = test->getFullName();
                QString teamcityTestName = UGUITest::getTeamcityTestName(test->suite, test->name);
                if (testName == fullTestName || testName == teamcityTestName) {
                    testList << test;
                    added = true;
                    break;
                }
                if (testName == test->suite) {
                    testList << test;
                    added = true;
                }
            }
            if (!added) {
                setError("Test not found: " + testName);
                return false;
            }
        }
    } else {
        // Run all tests with the given list of labels as a single suite.
        // If the list of labels is empty all tests are selected.
        testList = guiTestBase->getTests(UGUITestBase::Normal, labelList);
    }

    // Apply dynamic ignore list. Ignored tests are reported to Teamcity as 'ignored' and are not run at all.
    QString ignoreListFilePath = qgetenv("UGENE_GUI_TEST_IGNORE_LIST_FILE");
    if (!ignoreListFilePath.isEmpty()) {
        QString ignoreListFileContent = IOAdapterUtils::readTextFile(ignoreListFilePath);
        coreLog.details("Applying ignore list:\n" + ignoreListFileContent);
        int nIgnoredTests = 0;
        QStringList ignoreListEntries = ignoreListFileContent.split("\n");
        for (const QString& entry : qAsConst(ignoreListEntries)) {
            if (entry.isEmpty() || entry.startsWith("#")) {
                continue;
            }
            for (auto test : qAsConst(testList)) {
                QString teamcityTestName = UGUITest::getTeamcityTestName(test->suite, test->name);
                if (test->getFullName().startsWith(entry) || teamcityTestName.startsWith(entry)) {
                    test->labelSet.insert(UGUITestLabels::Ignored);
                    coreLog.details(QString("Adding Ignore label to test '%1', entry: '%2'").arg(teamcityTestName).arg(entry));
                    nIgnoredTests++;
                }
            }
        }
        coreLog.details(QString("Matched %1 tests to ignore").arg(nIgnoredTests));
    }
    return true;
}

void GUITestLauncher::updateProgress(int finishedCount) {
    int testsSize = testList.size();
    if (testsSize) {
        stateInfo.progress = finishedCount * 100 / testsSize;
    }
}

QString GUITestLauncher::getTestOutputFileName(const QString& testName, int testRunIteration) {
    return QString("ugene_" + testName + "_r_" + QString::number(testRunIteration) + ".out").replace(':', '_');
}

QString GUITestLauncher::findAvailableTestOutputDir() {
    QString date = QDate::currentDate().toString("dd.MM.yyyy");
    QString guiTestOutputDirectory = qgetenv("GUI_TESTING_OUTPUT");
    QString initPath;
    if (guiTestOutputDirectory.isEmpty()) {
        initPath = QDir::homePath() + "/gui_testing_output/" + date;
    } else {
        initPath = guiTestOutputDirectory + "/gui_testing_output/" + date;
    }
    QDir d(initPath);
    int i = 1;
    while (d.exists()) {
        d = QDir(initPath + QString("_%1").arg(i));
        i++;
    }
    return d.absolutePath();
}

static bool restoreTestDirWithExternalScript(const QString& pathToShellScript, const QString& iniFilePath) {
    QDir testsDir(qgetenv("UGENE_TESTS_PATH"));
    if (!testsDir.exists()) {
        coreLog.error("UGENE_TESTS_PATH is not set!");
        return false;
    }
    QDir dataDir(qgetenv("UGENE_DATA_PATH"));
    if (!dataDir.exists()) {
        coreLog.error("UGENE_DATA_PATH is not set!");
        return false;
    }

    QProcessEnvironment processEnv = QProcessEnvironment::systemEnvironment();
    processEnv.insert("UGENE_TESTS_DIR_NAME", testsDir.dirName());
    processEnv.insert("UGENE_DATA_DIR_NAME", dataDir.dirName());
    processEnv.insert(U2_USER_INI, iniFilePath);
    qint64 startTimeMicros = GTimer::currentTimeMicros();
    QProcess process;
    process.setProcessEnvironment(processEnv);
    QString restoreProcessWorkDir = QFileInfo(testsDir.absolutePath() + "/../").absolutePath();
    process.setWorkingDirectory(restoreProcessWorkDir);  // Parent dir of the test dir.
    //    coreLog.info("Running restore process, work dir: " + restoreProcessWorkDir +
    //                 ", tests dir: " + testsDir.dirName() +
    //                 ", data dir: " + dataDir.dirName() +
    //                 ", script: " + pathToShellScript);
    if (isOsWindows()) {
        process.start("cmd /C " + pathToShellScript);
    } else {
        process.start("/bin/bash", {pathToShellScript});
    }
    qint64 processId = process.processId();
    bool isStarted = process.waitForStarted();
    if (!isStarted) {
        coreLog.error("An error occurred while running restore script: " + process.errorString());
        return false;
    } else {
    }
    bool isFinished = process.waitForFinished(5000);

    qint64 endTimeMicros = GTimer::currentTimeMicros();
    qint64 runTimeMillis = (endTimeMicros - startTimeMicros) / 1000;
    coreLog.info("Backup and restore run time (millis): " + QString::number(runTimeMillis));

    QProcess::ExitStatus exitStatus = process.exitStatus();
    if (!isFinished || exitStatus != QProcess::NormalExit) {
        CmdlineTaskRunner::killChildrenProcesses(processId);
        coreLog.error("Backup restore script was killed/exited with bad status: " + QString::number(exitStatus));
        return false;
    }
    return true;
}

QProcessEnvironment GUITestLauncher::prepareTestRunEnvironment(const QString& testName, int testRunIteration) {
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    QDir().mkpath(testOutputDir + "/logs");
    env.insert(ENV_UGENE_DEV, "1");
    env.insert(ENV_GUI_TEST, "1");
    env.insert(ENV_USE_NATIVE_DIALOGS, "0");
    env.insert(U2_PRINT_TO_FILE, testOutputDir + "/logs/" + getTestOutputFileName(testName, testRunIteration));

    // Some variables in UGENE are computed relative to the ini file folder (like MSA color schema dir).
    // Use different folder for different runs.
    QString iniFileDir = testOutputDir + "/inis/";
    if (testRunIteration > 0) {
        iniFileDir += QString::number(testRunIteration) + "/";
    }
    QString iniFilePath = iniFileDir + QString(testName).replace(':', '_') + "_r_" + QString::number(testRunIteration) + "_UGENE.ini";
    if (!iniFileTemplate.isEmpty() && QFile::exists(iniFileTemplate)) {
        QFile::copy(iniFileTemplate, iniFilePath);
    }
    env.insert(U2_USER_INI, iniFilePath);

    QString externalScriptToRestore = qgetenv("UGENE_TEST_EXTERNAL_SCRIPT_TO_RESTORE");
    if (!externalScriptToRestore.isEmpty()) {
        restoreTestDirWithExternalScript(externalScriptToRestore, iniFilePath);
    }

    return env;
}

QString GUITestLauncher::runTest(const QString& testName, int timeoutMillis) {
    int maxReruns = qMax(qgetenv("UGENE_TEST_NUMBER_RERUN_FAILED_TEST").toInt(), 0);
    QString testOutput;
    bool isVideoRecordingOn = qgetenv("UGENE_TEST_ENABLE_VIDEO_RECORDING") == "1";
    bool isVideoRecordingAlwaysOn = isVideoRecordingOn && qgetenv("UGENE_TEST_ENABLE_VIDEO_RECORDING_ALL_ITERATIONS") == "1";
    for (int iteration = 0; iteration < 1 + maxReruns; iteration++) {
        if (iteration >= 1) {
            coreLog.error(QString("Re-running the test. Current re-run: %1, max re-runs: %2, check logs in: %3")
                              .arg(iteration)
                              .arg(maxReruns)
                              .arg(testOutputDir));
        }
        U2OpStatusImpl os;
        testOutput = runTestOnce(os, testName, iteration, timeoutMillis, isVideoRecordingOn && (isVideoRecordingAlwaysOn || iteration > 0));
        bool isFailed = os.hasError() || GUITestTeamcityLogger::isTestFailed(testOutput);
        if (!isFailed) {
            break;
        }
        coreLog.error(QString("Test failed with error: '%1'. Test output is '%2'.").arg(os.getError()).arg(testOutput));
        renameTestLog(testName, iteration);
    }
    return testOutput;
}

QString GUITestLauncher::runTestOnce(U2OpStatus& os, const QString& testName, int iteration, const int timeout, bool enableVideoRecording) {
    QProcessEnvironment environment = prepareTestRunEnvironment(testName, iteration);

    QString ugeneUiPath = QCoreApplication::applicationFilePath();
    QStringList arguments = getTestProcessArguments(testName);

    // ~QProcess is killing the process, will not return until the process is terminated.
    QProcess process;
    process.setProcessEnvironment(environment);
    QString ugeneTestRunWrapper = qgetenv("UGENE_TEST_RUN_WRAPPER");
    if (ugeneTestRunWrapper.isEmpty()) {
        process.start(ugeneUiPath, arguments);
    } else {
        arguments.push_front(ugeneUiPath);
        process.start(ugeneTestRunWrapper, arguments);
    }
    qint64 processId = process.processId();

    QProcess screenRecorderProcess;
    if (enableVideoRecording) {
        screenRecorderProcess.start(getScreenRecorderString(testName));
    }

    bool isStarted = process.waitForStarted();
    if (!isStarted) {
        QString error = QString("An error occurred while starting UGENE: %1").arg(process.errorString());
        os.setError(error);
        return error;
    }
    bool isFinished = process.waitForFinished(timeout);
    QProcess::ExitStatus exitStatus = process.exitStatus();

    if (!isFinished || exitStatus != QProcess::NormalExit) {
        CmdlineTaskRunner::killChildrenProcesses(processId);
    }

    if (isOsWindows()) {
        QProcess::execute("closeErrorReport.exe");  // this exe file, compiled Autoit script
    }

    QString testResult = readTestResult(process.readAllStandardOutput());

    if (enableVideoRecording) {
        screenRecorderProcess.close();
        bool isScreenRecorderFinished = screenRecorderProcess.waitForFinished(2000);
        if (!isScreenRecorderFinished) {
            screenRecorderProcess.kill();
            screenRecorderProcess.waitForFinished(2000);
        }
        bool keepVideoFile = qgetenv("UGENE_TEST_KEEP_VIDEOS") == "1";
        if (!keepVideoFile && !GUITestTeamcityLogger::isTestFailed(testResult)) {
            QFile(getVideoPath(testName)).remove();
        }
    }

    if (isFinished && exitStatus == QProcess::NormalExit) {
        return testResult;
    }
    if (isOsWindows()) {
        CmdlineTaskRunner::killProcessTree(process.processId());
        process.kill();  // to avoid QProcess: Destroyed while process is still running.
        process.waitForFinished(2000);
    }
    QString error = isFinished ? QString("An error occurred while finishing UGENE: %1\n%2").arg(process.errorString()).arg(testResult) : QString("Test fails because of timeout.");
    os.setError(error);
    return error;
}

QStringList GUITestLauncher::getTestProcessArguments(const QString& testName) {
    QString guiTestOption = "--" + CMDLineCoreOptions::LAUNCH_GUI_TEST + "=" + testName;
    return {guiTestOption};
}

QString GUITestLauncher::readTestResult(const QByteArray& output) {
    QString msg;
    QTextStream stream(output, QIODevice::ReadOnly);

    while (!stream.atEnd()) {
        QString str = stream.readLine();

        if (str.contains(GUITestService::GUITESTING_REPORT_PREFIX)) {
            msg = str.remove(0, str.indexOf(':') + 1);
            if (!msg.isEmpty()) {
                break;
            }
        }
    }

    return msg;
}

QString GUITestLauncher::generateReport() const {
    QString res;
    res += "<table width=\"100%\">";
    res += QString("<tr><th>%1</th><th>%2</th></tr>").arg(tr("Test name")).arg(tr("Status"));

    QMap<QString, QString>::const_iterator i;
    for (i = testResultByFullTestNameMap.begin(); i != testResultByFullTestNameMap.end(); ++i) {
        QString color = "green";
        if (GUITestTeamcityLogger::isTestFailed(i.value())) {
            color = "red";
        }
        res += QString("<tr><th><font color='%3'>%1</font></th><th><font color='%3'>%2</font></th></tr>").arg(i.key()).arg(i.value()).arg(color);
    }
    res += "</table>";

    return res;
}

QString GUITestLauncher::getScreenRecorderString(const QString& testName) {
    QString result;
    QString videoFilePath = getVideoPath(testName);
    if (isOsLinux()) {
        QRect rec = QApplication::desktop()->screenGeometry();
        int height = rec.height();
        int width = rec.width();
        QString display = qgetenv("DISPLAY");
        result = QString("ffmpeg -video_size %1x%2 -framerate 5 -f x11grab -i %3.0 %4").arg(width).arg(height).arg(display).arg(videoFilePath);
    } else if (isOsMac()) {
        result = QString("ffmpeg -f avfoundation -r 5 -i \"1:none\" \"%1\"").arg(videoFilePath);
    } else if (isOsWindows()) {
        result = QString("ffmpeg -f dshow -i video=\"UScreenCapture\" -r 5 %1").arg(videoFilePath);
    }
    uiLog.trace("going to record video: " + result);
    return result;
}

QString GUITestLauncher::getVideoPath(const QString& testName) {
    QString dirPath = qgetenv("UGENE_GUI_TEST_VIDEO_DIR_PATH");
    if (dirPath.isEmpty()) {
        dirPath = QDir::currentPath() + "/videos";
    }
    if (!QDir(dirPath).exists()) {
        QDir().mkpath(dirPath);
    }
    return dirPath + "/" + QString(testName).replace(":", "_") + ".avi";
}

}  // namespace U2
