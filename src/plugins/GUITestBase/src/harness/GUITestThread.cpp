/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include <core/GUITest.h>
#include <core/GUITestOpStatus.h>
#include <drivers/GTMouseDriver.h>

#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QGuiApplication>
#include <QScreen>

#include <U2Core/AppContext.h>
#include <U2Core/U2SafePoints.h>

#include "GTUtilsTaskTreeView.h"
#include "GUITestService.h"
#include "GUITestTeamcityLogger.h"
#include "GUITestThread.h"
#include "UGUITest.h"
#include "UGUITestBase.h"

namespace U2 {

GUITestThread::GUITestThread(GUITest* test, bool isRunPostActionsAndCleanup)
    : testToRun(test),
      isRunPostActionsAndCleanup(isRunPostActionsAndCleanup),
      testResult("Not run") {
    SAFE_POINT(test != nullptr, "GUITest is NULL", );
}

void GUITestThread::run() {
    SAFE_POINT(testToRun != nullptr, "GUITest is NULL", );

    clearSandbox();
    QString error = launchTest(testToRun);
    if (isRunPostActionsAndCleanup) {
        cleanup();
    }
    testResult = error.isEmpty() ? GUITestTeamcityLogger::successResult : error;
    writeTestResult();
    exit();
}

void GUITestThread::sl_testTimeOut() {
    GT_LOG("Test is timed out");
    saveScreenshot();
    cleanup();
    testResult = QString("test timed out");
    writeTestResult();
    exit();
}

QString GUITestThread::launchTest(GUITest* test) {
    GTGlobals::resetOpStatus();
    QTimer::singleShot(testToRun->timeout, this, SLOT(sl_testTimeOut()));

    // Start all tests with some common mouse position.
    GTMouseDriver::moveTo({400, 300});
    UGUITestBase* testBase = UGUITestBase::getInstance();

    try {
        QList<GUITest*> preCheckList = testBase->getTests(UGUITestBase::PreCheck);
        for (GUITest* preCheck : qAsConst(preCheckList)) {
            GT_LOG("running pre check: " + preCheck->getFullName());
            preCheck->run();
            GT_LOG("pre check is finished: " + preCheck->getFullName());
        }

        GT_LOG("test->run() started: " + test->getFullName());
        test->run();
        GT_LOG("test->run() finished with no errors: " + test->getFullName());

        QList<GUITest*> postCheckList = testBase->getTests(UGUITestBase::PostCheck);
        for (GUITest* postCheck : qAsConst(postCheckList)) {
            GT_LOG("running post check: " + postCheck->getFullName());
            postCheck->run();
            GT_LOG("post check is finished: " + postCheck->getFullName());
        }
    } catch (HI::GUITestOpStatus*) {
        GT_LOG("Got exception while running the test.");
    }
    QString error = GTGlobals::getOpStatus().getError();
    GT_LOG(QString("launchTest finished %1").arg(error.isEmpty() ? "with no error" : "with error: " + error));
    return error;
}

void GUITestThread::clearSandbox() {
    const QString pathToSandbox = UGUITest::testDir + "_common_data/scenarios/sandbox/";
    QDir sandbox(pathToSandbox);

    const QStringList entryList = sandbox.entryList();
    for (const QString& fileName : qAsConst(entryList)) {
        if (fileName != "." && fileName != "..") {
            if (QFile::remove(pathToSandbox + fileName)) {
                continue;
            } else {
                QDir dir(pathToSandbox + fileName);
                removeDir(dir.absolutePath());
            }
        }
    }
}

void GUITestThread::removeDir(const QString& dirName) {
    QDir dir(dirName);

    const QFileInfoList fileInfoList = dir.entryInfoList();
    for (const QFileInfo& fileInfo : qAsConst(fileInfoList)) {
        const QString fileName = fileInfo.fileName();
        const QString filePath = fileInfo.filePath();
        if (fileName != "." && fileName != "..") {
            if (QFile::remove(filePath)) {
                continue;
            } else {
                QDir subDir(filePath);
                if (subDir.rmdir(filePath)) {
                    continue;
                } else {
                    removeDir(filePath);
                }
            }
        }
    }
    dir.rmdir(dir.absoluteFilePath(dirName));
}

void GUITestThread::saveScreenshot() {
    HI::GUITestOpStatus os;
    QImage image = GTGlobals::takeScreenShot();
    image.save(HI::GUITest::screenshotDir + testToRun->getFullName() + ".jpg");
}

void GUITestThread::cleanup() {
    GT_LOG("Running cleanup after the test");
    testToRun->cleanup();
    UGUITestBase* testBase = UGUITestBase::getInstance();
    QList<GUITest*> postActionList = testBase->getTests(UGUITestBase::PostAction);
    for (HI::GUITest* postAction : qAsConst(postActionList)) {
        try {
            qDebug("Cleanup action is started: %s", postAction->getFullName().toLocal8Bit().constData());
            GTGlobals::resetOpStatus();
            postAction->run();
            qDebug("Cleanup action is finished: %s", postAction->getFullName().toLocal8Bit().constData());
        } catch (HI::GUITestOpStatus* opStatus) {
            coreLog.error(opStatus->getError());
        }
    }
    GT_LOG("Cleanup is finished");
}

void GUITestThread::writeTestResult() {
    QByteArray testOutput = (GUITestService::GUITESTING_REPORT_PREFIX + ": " + testResult).toUtf8();
    qDebug("writing test result for teamcity: '%s'", testOutput.constData());
    printf("%s\n", testOutput.constData());
    fflush(stdout);
}

}  // namespace U2
