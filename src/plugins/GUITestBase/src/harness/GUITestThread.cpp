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

#include <core/GUITest.h>
#include <core/GUITestOpStatus.h>
#include <core/MainThreadRunnable.h>
#include <drivers/GTMouseDriver.h>

#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QGuiApplication>
#include <QScreen>

#include <U2Core/AppContext.h>
#include <U2Core/U2SafePoints.h>

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

    UGUITestBase* db = UGUITestBase::getInstance();

    QList<GUITest*> testList;
    testList << db->getTests(UGUITestBase::PreAdditional);
    testList << testToRun;
    testList << db->getTests(UGUITestBase::PostAdditionalChecks);

    clearSandbox();

    QString error = launchTest(testList);
    if (isRunPostActionsAndCleanup) {
        cleanup();
    }

    testResult = error.isEmpty() ? GUITestTeamcityLogger::successResult : error;
    writeTestResult();

    exit();
}

void GUITestThread::sl_testTimeOut() {
    qDebug("Test is timed out");
    saveScreenshot();
    cleanup();
    testResult = QString("test timed out");
    writeTestResult();
    exit();
}

QString GUITestThread::launchTest(const QList<GUITest*>& tests) {
    QTimer::singleShot(testToRun->timeout, this, SLOT(sl_testTimeOut()));

    // Start all tests with some common mouse position.
    GTMouseDriver::moveTo({400, 300});

    HI::GUITestOpStatus os;
    try {
        for (GUITest* test : qAsConst(tests)) {
            qDebug("launchTest started: %s", test->getFullName().toLocal8Bit().constData());
            test->run(os);
            qDebug("launchTest finished: %s", test->getFullName().toLocal8Bit().constData());
        }
    } catch (HI::GUITestOpStatus*) {
    }
    // Run post checks if there is an error.
    QString error = os.getError();
    if (!error.isEmpty()) {
        try {
            UGUITestBase* testBase = UGUITestBase::getInstance();
            const QList<GUITest*> postCheckList = testBase->getTests(UGUITestBase::PostAdditionalChecks);
            for (GUITest* test : qAsConst(postCheckList)) {
                qDebug("launchTest running additional post check: %s", test->getFullName().toLocal8Bit().constData());
                test->run(os);
                qDebug("launchTest additional post check is finished: %s", test->getFullName().toLocal8Bit().constData());
            }
        } catch (HI::GUITestOpStatus*) {
        }
    }
    qDebug("launchTest for all tests/checks is finished, error: '%s', isEmpty: %d", error.toLocal8Bit().constData(), error.isEmpty());
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
    QImage image = GTGlobals::takeScreenShot(os);
    image.save(HI::GUITest::screenshotDir + testToRun->getFullName() + ".jpg");
}

void GUITestThread::cleanup() {
    qDebug("Running cleanup after the test");
    testToRun->cleanup();
    UGUITestBase* testBase = UGUITestBase::getInstance();
    const QList<GUITest*> postActionList = testBase->getTests(UGUITestBase::PostAdditionalActions);
    for (HI::GUITest* postAction : qAsConst(postActionList)) {
        HI::GUITestOpStatus os;
        try {
            qDebug("Cleanup action is started: %s", postAction->getFullName().toLocal8Bit().constData());
            postAction->run(os);
            qDebug("Cleanup action is finished: %s", postAction->getFullName().toLocal8Bit().constData());
        } catch (HI::GUITestOpStatus* opStatus) {
            coreLog.error(opStatus->getError());
        }
    }
    qDebug("Cleanup is finished");
}

void GUITestThread::writeTestResult() {
    QByteArray testOutput = (GUITestService::GUITESTING_REPORT_PREFIX + ": " + testResult).toUtf8();
    qDebug("writing test result for teamcity: '%s'", testOutput.constData());

    printf("%s\n", testOutput.constData());
    fflush(stdout);
}

}  // namespace U2
