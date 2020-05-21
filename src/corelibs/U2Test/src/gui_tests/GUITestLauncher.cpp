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

#include "GUITestLauncher.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QMap>
#include <QTextStream>
#include <QThread>

#include <U2Core/AppContext.h>
#include <U2Core/CMDLineCoreOptions.h>
#include <U2Core/CmdlineTaskRunner.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/Timer.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "GUITestTeamcityLogger.h"
#include "UGUITestBase.h"

#ifdef Q_OS_LINUX
#    define GUI_TEST_TIMEOUT_MILLIS 300000
#else
#    define GUI_TEST_TIMEOUT_MILLIS 480000
#endif

#ifdef Q_OS_MAC
#    define NUMBER_OF_TEST_SUITES 4
#elif defined(Q_OS_LINUX)
#    define NUMBER_OF_TEST_SUITES 5
#elif defined(Q_OS_WIN)
#    define NUMBER_OF_TEST_SUITES 3
#else
#    define NUMBER_OF_TEST_SUITES 1
#endif

#define GUITESTING_REPORT_PREFIX "GUITesting"

namespace U2 {

GUITestLauncher::GUITestLauncher(int _suiteNumber, bool _noIgnored, QString _iniFileTemplate)
    : Task("gui_test_launcher", TaskFlags(TaskFlag_ReportingIsSupported) | TaskFlag_ReportingIsEnabled),
      suiteNumber(_suiteNumber), noIgnored(_noIgnored), pathToSuite(""), iniFileTemplate(_iniFileTemplate) {
    tpm = Task::Progress_Manual;
    testOutDir = getTestOutDir();

    QWidget *splashScreen = QApplication::activeWindow();
    if (splashScreen != nullptr) {
        splashScreen->hide();
    }
}

GUITestLauncher::GUITestLauncher(QString _pathToSuite, bool _noIgnored, QString _iniFileTemplate)
    : Task("gui_test_launcher", TaskFlags(TaskFlag_ReportingIsSupported) | TaskFlag_ReportingIsEnabled),
      suiteNumber(0), noIgnored(_noIgnored), pathToSuite(_pathToSuite), iniFileTemplate(_iniFileTemplate) {
    tpm = Task::Progress_Manual;
    testOutDir = getTestOutDir();
}

bool GUITestLauncher::renameTestLog(const QString &testName) {
    QString outFileName = testOutFile(testName);
    QString outFilePath = testOutDir + QString("/logs/");

    QFile outLog(outFilePath + outFileName);
    return outLog.rename(outFilePath + "failed_" + outFileName);
}

void GUITestLauncher::run() {
    if (!initGUITestBase()) {
        // FIXME: if test suite can't run for some reason UGENE runs shutdown task that asserts that startup is in progress.
        //  Workaround: wait 3 seconds to ensure that startup is complete & GUI test base error message is printed.
        QThread::currentThread()->sleep(3);
        return;
    }

    int finishedCount = 0;
    foreach (HI::GUITest *test, tests) {
        if (isCanceled()) {
            return;
        }
        if (test == nullptr) {
            updateProgress(finishedCount++);
            continue;
        }
        QString testName = test->getFullName();
        QString testNameForTeamCity = test->getSuite() + "_" + test->getName();
        results[testName] = "";

        firstTestRunCheck(testName);

        if (!test->isIgnored()) {
            qint64 startTime = GTimer::currentTimeMicros();
            GUITestTeamcityLogger::testStarted(testNameForTeamCity);

            QString testResult = runTest(testName);
            results[testName] = testResult;
            if (GUITestTeamcityLogger::testFailed(testResult)) {
                renameTestLog(testName);
            }

            qint64 finishTime = GTimer::currentTimeMicros();
            GUITestTeamcityLogger::teamCityLogResult(testNameForTeamCity, testResult, GTimer::millisBetween(startTime, finishTime));
        } else if (test->getReason() == HI::GUITest::Bug) {
            GUITestTeamcityLogger::testIgnored(testNameForTeamCity, test->getIgnoreMessage());
        }

        updateProgress(finishedCount++);
    }
}

void GUITestLauncher::firstTestRunCheck(const QString &testName) {
    QString testResult = results[testName];
    Q_ASSERT(testResult.isEmpty());
}

bool GUITestLauncher::initGUITestBase() {
    UGUITestBase *b = AppContext::getGUITestBase();
    SAFE_POINT(b != nullptr, "Test base is NULL", false);
    QString label = qgetenv("UGENE_GUI_TEST_LABEL");
    QList<HI::GUITest *> allTestList = b->getTests(UGUITestBase::Normal, label);
    if (allTestList.isEmpty()) {
        setError(tr("No tests to run"));
        return false;
    }

    if (suiteNumber != 0) {
        if (suiteNumber < 1 || suiteNumber > NUMBER_OF_TEST_SUITES) {
            setError(QString("Invalid suite number: %1. There are %2 suites").arg(suiteNumber).arg(NUMBER_OF_TEST_SUITES));
            return false;
        }
        int numberAllOfTests = allTestList.length();
        int numberOfTestsPerSuite = qRound(numberAllOfTests / (double)NUMBER_OF_TEST_SUITES);
        bool isLastSuite = suiteNumber == NUMBER_OF_TEST_SUITES;
        int suiteIndex = suiteNumber - 1;
        int firstTestIndex = suiteIndex * numberOfTestsPerSuite;
        int testsInSuite = isLastSuite ? numberAllOfTests - firstTestIndex : numberOfTestsPerSuite;
        tests = allTestList.mid(firstTestIndex, testsInSuite);
        coreLog.info(QString("Running suite %1, tests from: %2 to %3. Tests in the suite: %4, total tests: %5")
                         .arg(suiteNumber)
                         .arg(firstTestIndex)
                         .arg(firstTestIndex + testsInSuite)
                         .arg(testsInSuite)
                         .arg(numberAllOfTests));
    } else if (!pathToSuite.isEmpty()) {
        QString absPath = QDir().absoluteFilePath(pathToSuite);
        QFile suite(absPath);
        if (!suite.open(QFile::ReadOnly)) {
            setError("Can't open suite file: " + absPath);
            return false;
        }
        char buf[1024];
        while (suite.readLine(buf, sizeof(buf)) != -1) {
            QString testName = QString(buf).remove('\n').remove('\t').remove(' ');
            if (testName.startsWith("#") || testName.isEmpty()) {
                continue;    // comment line or empty line.
            }
            bool added = false;
            foreach (HI::GUITest *test, allTestList) {
                QString fullTestName = test->getFullName();
                QString fullTestNameInTeamcityFormat = fullTestName.replace(':', '_');
                if (fullTestName == testName || fullTestNameInTeamcityFormat == testName) {
                    tests << test;
                    added = true;
                    break;
                }
            }
            if (!added) {
                setError("Test not found: " + testName);
                return false;
            }
        }
    } else {
        tests = allTestList;
    }

    if (noIgnored) {
        foreach (HI::GUITest *test, tests) {
            test->setIgnored(false);
        }
    }
    return true;
}

void GUITestLauncher::updateProgress(int finishedCount) {
    int testsSize = tests.size();
    if (testsSize) {
        stateInfo.progress = finishedCount * 100 / testsSize;
    }
}

QString GUITestLauncher::testOutFile(const QString &testName) {
    return QString("ugene_" + testName + ".out").replace(':', '_');
}

QString GUITestLauncher::getTestOutDir() {
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

QProcessEnvironment GUITestLauncher::getProcessEnvironment(QString testName) {
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    QDir().mkpath(testOutDir + "/logs");
    env.insert(ENV_UGENE_DEV, "1");
    env.insert(ENV_GUI_TEST, "1");
    env.insert(ENV_USE_NATIVE_DIALOGS, "0");
    env.insert(U2_PRINT_TO_FILE, testOutDir + "/logs/" + testOutFile(testName));

    QString iniFileName = testOutDir + "/inis/" + testName.replace(':', '_') + "_UGENE.ini";
    if (!iniFileTemplate.isEmpty() && QFile::exists(iniFileTemplate)) {
        QFile::copy(iniFileTemplate, iniFileName);
    }
    env.insert(U2_USER_INI, iniFileName);

    return env;
}

static bool isVideoRecordingOn() {
    return qgetenv("UGENE_TEST_ENABLE_VIDEO_RECORDING") == "1";
}

static bool restoreTestDirWithExternalScript(const QString &pathToShellScript) {
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
    qint64 startTimeMicros = GTimer::currentTimeMicros();
    QProcess process;
    process.setProcessEnvironment(processEnv);
    QString restoreProcessWorkDir = QFileInfo(testsDir.absolutePath() + "/../").absolutePath();
    process.setWorkingDirectory(restoreProcessWorkDir);    // Parent dir of the test dir.
    coreLog.info("Running restore process, work dir: " + restoreProcessWorkDir +
                 ", tests dir: " + testsDir.dirName() +
                 ", data dir: " + dataDir.dirName() +
                 ", script: " + pathToShellScript);
    process.start("/bin/bash", QStringList() << pathToShellScript);
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

QString GUITestLauncher::runTest(const QString &testName) {
    int repeatCount = qMax(qgetenv("UGENE_TEST_NUMBER_RERUN_FAILED_TEST").toInt(), 0);
    QString testOutput;
    for (int i = 0; i < 1 + repeatCount; i++) {
        if (i >= 1) {
            coreLog.error(QString("Re-running the test. Current re-run: %1, max re-runs: %2").arg(i).arg(repeatCount));
        }
        U2OpStatusImpl os;
        testOutput = runTestOnce(os, testName, isVideoRecordingOn());
        if (!os.hasError()) {
            break;
        }
        coreLog.error(QString("Test failed with error: '%1'. Test output is '%2'.").arg(os.getError()).arg(testOutput));
    }
    return testOutput;
}

QString GUITestLauncher::runTestOnce(U2OpStatus &os, const QString &testName, bool enableVideoRecording) {
    QProcessEnvironment environment = getProcessEnvironment(testName);

    QString externalScriptToRestore = qgetenv("UGENE_TEST_EXTERNAL_SCRIPT_TO_RESTORE");
    if (!externalScriptToRestore.isEmpty()) {
        if (restoreTestDirWithExternalScript(externalScriptToRestore)) {
            environment.insert("UGENE_TEST_SKIP_BACKUP_AND_RESTORE", "1");
        }
    }

    QString path = QCoreApplication::applicationFilePath();
    QStringList arguments = getTestProcessArguments(testName);

    // ~QProcess is killing the process, will not return until the process is terminated.
    QProcess process;
    process.setProcessEnvironment(environment);
    process.start(path, arguments);
    qint64 processId = process.processId();

    QProcess screenRecorder;
    if (enableVideoRecording) {
        screenRecorder.start(getScreenRecorderString(testName));
    }

    bool isStarted = process.waitForStarted();
    if (!isStarted) {
        QString error = QString("An error occurred while starting UGENE: %1").arg(process.errorString());
        os.setError(error);
        return error;
    }
    bool isFinished = process.waitForFinished(GUI_TEST_TIMEOUT_MILLIS);
    QProcess::ExitStatus exitStatus = process.exitStatus();

    if (!isFinished || exitStatus != QProcess::NormalExit) {
        CmdlineTaskRunner::killChildrenProcesses(processId);
    }

#ifdef Q_OS_WIN
    QProcess::execute("closeErrorReport.exe");    //this exe file, compiled Autoit script
#endif

    QString testResult = readTestResult(process.readAllStandardOutput());

    if (enableVideoRecording) {
        screenRecorder.close();
        screenRecorder.waitForFinished(2000);
        if (!GUITestTeamcityLogger::testFailed(testResult)) {
            QFile(getVideoPath(testName)).remove();
        }
    }

    if (isFinished && exitStatus == QProcess::NormalExit) {
        return testResult;
    }
#ifdef Q_OS_WIN
    CmdlineTaskRunner::killProcessTree(process.processId());
#endif
    QString error = isFinished ? QString("An error occurred while finishing UGENE: %1\n%2").arg(process.errorString()).arg(testResult) :
                                 QString("Test fails because of timeout.");
    os.setError(error);
    return error;
}

QStringList GUITestLauncher::getTestProcessArguments(const QString &testName) {
    return QStringList() << QString("--") + CMDLineCoreOptions::LAUNCH_GUI_TEST + "=" + testName;
}

QString GUITestLauncher::readTestResult(const QByteArray &output) {
    QString msg;
    QTextStream stream(output, QIODevice::ReadOnly);

    while (!stream.atEnd()) {
        QString str = stream.readLine();

        if (str.contains(GUITESTING_REPORT_PREFIX)) {
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
    for (i = results.begin(); i != results.end(); ++i) {
        QString color = "green";
        if (GUITestTeamcityLogger::testFailed(i.value())) {
            color = "red";
        }
        res += QString("<tr><th><font color='%3'>%1</font></th><th><font color='%3'>%2</font></th></tr>").arg(i.key()).arg(i.value()).arg(color);
    }
    res += "</table>";

    return res;
}

QString GUITestLauncher::getScreenRecorderString(QString testName) {
    QString result;
#ifdef Q_OS_LINUX
    QRect rec = QApplication::desktop()->screenGeometry();
    int height = rec.height();
    int width = rec.width();
    QString display = qgetenv("DISPLAY");
    result = QString("ffmpeg -video_size %1x%2 -framerate 5 -f x11grab -i %3.0 %4").arg(width).arg(height).arg(display).arg(getVideoPath(testName));
#elif defined Q_OS_MAC
    result = QString("ffmpeg -f avfoundation -r 5 -i \"1:none\" \"%1\"").arg(getVideoPath(testName));
#elif defined Q_OS_WIN
    result = QString("ffmpeg -f dshow -i video=\"UScreenCapture\" -r 5 %1").arg(getVideoPath(testName.replace(':', '_')));
#endif
    uiLog.trace("going to record video: " + result);
    return result;
}

QString GUITestLauncher::getVideoPath(const QString &testName) {
    QDir().mkpath(QDir::currentPath() + "/videos");
    QString result = QDir::currentPath() + "/videos/" + testName + ".avi";
    return result;
}
}    // namespace U2
