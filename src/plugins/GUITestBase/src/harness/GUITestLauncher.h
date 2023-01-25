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

#ifndef _U2_GUI_TEST_LAUNCHER_H_
#define _U2_GUI_TEST_LAUNCHER_H_

#include <core/GUITest.h>

#include <QProcessEnvironment>

#include <U2Core/MultiTask.h>
#include <U2Core/Task.h>
#include <U2Core/global.h>

#include <U2Gui/MainWindow.h>

namespace U2 {

using namespace HI;

class U2OpStatus;

class GUITestLauncher : public Task {
    Q_OBJECT
public:
    GUITestLauncher(int suiteNumber, bool noIgnored = false, const QString& iniFileTemplate = "");
    GUITestLauncher(const QString& pathToSuite = "", bool noIgnored = false, const QString& iniFileTemplate = "");

    void run() override;
    QString generateReport() const override;

private:
    /** List of tests to run. */
    QList<GUITest*> testList;

    /** Result status per test. */
    QMap<QString, QString> testResultByFullTestNameMap;

    int suiteNumber;
    bool noIgnored;
    QString pathToSuite;
    QString testOutputDir;
    QString iniFileTemplate;

    static QStringList getTestProcessArguments(const QString& testName);
    /**
     * Prepares ini file, logs dir and process environment for a single test run.
     * Returns system environment for the test process.
     */
    QProcessEnvironment prepareTestRunEnvironment(const QString& testName, int testRunIteration);
    static QString getTestOutputFileName(const QString& testName, int testRunIteration);
    static QString findAvailableTestOutputDir();

    void firstTestRunCheck(const QString& testName);

    /** Runs test multiple times (UGENE_TEST_NUMBER_RERUN_FAILED_TEST) and returns test output of the last run. */
    QString runTest(const QString& testName, int timeoutMillis);

    /** Runs test once and returns test output. */
    QString runTestOnce(U2OpStatus& os, const QString& testName, int iteration, const int timeout, bool enableVideoRecording);

    static QString readTestResult(const QByteArray& output);
    bool renameTestLog(const QString& testName, int testRunIteration);

    bool initTestList();
    void updateProgress(int finishedCount);

    static QString getScreenRecorderString(const QString& testName);

    /**
     * Returns full video file path for the given test.
     *
     * By default the dir for the tests is the current QDir::currentDir() + '/videos' but
     * it can be changed with UGENE_GUI_TEST_VIDEO_DIR_PATH environment variable.
     */
    static QString getVideoPath(const QString& testName);
};

}  // namespace U2

#endif
