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

#ifndef _U2_GUI_TEST_LAUNCHER_H_
#define _U2_GUI_TEST_LAUNCHER_H_

#include <core/GUITest.h>

#include <QProcessEnvironment>

#include <U2Core/MultiTask.h>
#include <U2Core/Task.h>
#include <U2Core/global.h>

#include <U2Gui/MainWindow.h>

namespace U2 {

class U2OpStatus;

class GUITestLauncher : public Task {
    Q_OBJECT
public:
    GUITestLauncher(int _suiteNumber, bool _noIgnored = false, QString _iniFileTemplate = "");
    GUITestLauncher(QString _pathToSuite = "", bool _noIgnored = false, QString _iniFileTemplate = "");

    virtual void run();
    virtual QString generateReport() const;

private:
    QList<HI::GUITest *> tests;
    QMap<QString, QString> results;
    int suiteNumber;
    bool noIgnored;
    QString pathToSuite;
    QString testOutDir;
    QString iniFileTemplate;

    static QStringList getTestProcessArguments(const QString &testName);
    /**
     * Prepares ini file, logs dir and process environment for a single test run.
     * Returns system environment for the test process.
     */
    QProcessEnvironment prepareTestRunEnvironment(const QString &testName, int testRunIteration);
    static QString testOutFile(const QString &testName);
    static QString getTestOutDir();

    void firstTestRunCheck(const QString &testName);

    /** Runs test and returns test output. */
    QString runTest(const QString &testName);

    /** Runs test once and returns test output. */
    QString runTestOnce(U2OpStatus &os, const QString &testName, int iteration, bool enableVideoRecording);

    static QString readTestResult(const QByteArray &output);
    bool renameTestLog(const QString &testName);

    bool initGUITestBase();
    void updateProgress(int finishedCount);

    QString getScreenRecorderString(QString testName);
    QString getVideoPath(const QString &testName);
};

}    // namespace U2

#endif
