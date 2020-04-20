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

#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QGuiApplication>
#include <QScreen>

#include <core/GUITest.h>
#include <core/GUITestOpStatus.h>
#include <core/MainThreadRunnable.h>

#include <U2Core/AppContext.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "UGUITestBase.h"
#include "GUITestService.h"
#include "GUITestTeamcityLogger.h"
#include "GUITestThread.h"
#include "UGUITest.h"

namespace U2 {

GUITestThread::GUITestThread(HI::GUITest *test, Logger &log, bool _needCleanup) :
    test(test),
    log(log),
    needCleanup(_needCleanup),
    testResult("Not run")
{
    SAFE_POINT(NULL != test, "GUITest is NULL", );
}

void GUITestThread::run() {
    SAFE_POINT(NULL != test, "GUITest is NULL", );

    GUITests tests;
    tests << preChecks();
    tests << test;
    tests << postChecks();

    clearSandbox();

    QTimer timer;
    connect(&timer, SIGNAL(timeout()), this, SLOT(sl_getMemory()), Qt::DirectConnection);
    timer.start(1000);

    const QString error = launchTest(tests);

    if (needCleanup) {
        cleanup();
    }

    timer.stop();

    saveMemoryInfo();

    testResult = error.isEmpty() ? GUITestTeamcityLogger::successResult : error;
    writeTestResult();

    exit();
}

void GUITestThread::sl_testTimeOut() {
    saveScreenshot();
    cleanup();
    testResult = QString("test timed out");
    writeTestResult();
    exit();
}

QString GUITestThread::launchTest(const GUITests &tests) {
    QTimer::singleShot(test->getTimeout(), this, SLOT(sl_testTimeOut()));

    HI::GUITestOpStatus os;
    try {
        foreach (HI::GUITest *t, tests) {
            if (NULL != t) {
                t->run(os);
            }
        }
    } catch(HI::GUITestOpStatus *) {

    }
    QString result = os.getError();

    //Run post checks if has error
    if (!result.isEmpty()){
        try {
            foreach (HI::GUITest *t, postChecks()) {
                if (NULL != t) {
                    t->run(os);
                }
            }
        } catch(HI::GUITestOpStatus *) {

        }
    }

    return result;
}

GUITests GUITestThread::preChecks() {
    UGUITestBase *tb = AppContext::getGUITestBase();
    SAFE_POINT(NULL != tb, "GUITestBase is NULL", GUITests());

//    GUITests additionalChecks = tb->takeTests(GUITestBase::PreAdditional);
    GUITests additionalChecks = tb->getTests(UGUITestBase::PreAdditional);
    SAFE_POINT(!additionalChecks.isEmpty(), "additionalChecks is empty", GUITests());

    return additionalChecks;
}

GUITests GUITestThread::postChecks() {
    UGUITestBase *tb = AppContext::getGUITestBase();
    SAFE_POINT(NULL != tb, "GUITestBase is NULL", GUITests());

//    GUITests additionalChecks = tb->takeTests(GUITestBase::PostAdditionalChecks);
    GUITests additionalChecks = tb->getTests(UGUITestBase::PostAdditionalChecks);
    SAFE_POINT(!additionalChecks.isEmpty(), "additionalChecks is empty", GUITests());

    return additionalChecks;
}

GUITests GUITestThread::postActions() {
    UGUITestBase *tb = AppContext::getGUITestBase();
    SAFE_POINT(NULL != tb, "GUITestBase is NULL", GUITests());

//    GUITests additionalChecks = tb->takeTests(GUITestBase::PostAdditionalActions);
    GUITests additionalChecks = tb->getTests(UGUITestBase::PostAdditionalActions);
    SAFE_POINT(!additionalChecks.isEmpty(), "additionalChecks is empty", GUITests());

    return additionalChecks;
}

void GUITestThread::clearSandbox() {
    log.trace("GUITestThread __ clearSandbox");

    const QString pathToSandbox = UGUITest::testDir + "_common_data/scenarios/sandbox/";
    QDir sandbox(pathToSandbox);

    foreach (const QString &fileName, sandbox.entryList()) {
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

void GUITestThread::removeDir(const QString &dirName) {
    QDir dir(dirName);

    foreach (const QFileInfo &fileInfo, dir.entryInfoList()) {
        const QString fileName = fileInfo.fileName();
        const QString filePath = fileInfo.filePath();
        if (fileName != "." && fileName != "..") {
            if (QFile::remove(filePath)) {
                continue;
            } else {
                QDir dir(filePath);
                if (dir.rmdir(filePath)) {
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
    class Scenario : public HI::CustomScenario {
    public:
        Scenario(HI::GUITest *test) :
            test(test)
        {

        }

        void run(HI::GUITestOpStatus &) {
            const QPixmap originalPixmap = QGuiApplication::primaryScreen()->grabWindow(QApplication::desktop()->winId());
            originalPixmap.save(HI::GUITest::screenshotDir + test->getFullName() + ".jpg");
        }

    private:
        HI::GUITest *test;
    };

    HI::GUITestOpStatus os;
    HI::MainThreadRunnable::runInMainThread(os, new Scenario(test));
}

void GUITestThread::cleanup() {
    test->cleanup();
    foreach (HI::GUITest *postAction, postActions()) {
        HI::GUITestOpStatus os;
        try {
            postAction->run(os);
        } catch (HI::GUITestOpStatus *opStatus) {
            coreLog.error(opStatus->getError());
        }
    }
}

void GUITestThread::writeTestResult() {
    printf("%s\n", (GUITestService::GUITESTING_REPORT_PREFIX + ": " + testResult).toUtf8().data());
}

void GUITestThread::sl_getMemory(){
#ifdef Q_OS_LINUX
    qint64 appPid = QApplication::applicationPid();

    int memValue = countMemForProcessTree(appPid);

    memoryList << memValue;
#endif
}

int GUITestThread::countMemForProcessTree(int pid){
#ifdef Q_OS_LINUX
    int result = 0;
    //getting child processes
    QProcess pgrep;
    pgrep.start(QString("pgrep -P %1").arg(pid/*appPid*/));
    pgrep.waitForFinished();
    QByteArray pgrepOut = pgrep.readAllStandardOutput();
    QStringList childPidsStringList = QString(pgrepOut).split("\n");
    QList<int> childPids;
    foreach (QString s, childPidsStringList) {
        bool ok;
        int childPid = s.toInt(&ok);
        if(ok){
            childPids<<childPid;
        }
    }

    //getting memory of process by pid
    QProcess memTracer;
    memTracer.start("bash", QStringList() << "-c" << QString("top -b -p%1 -n 1 | grep %1").arg(pid));
    memTracer.waitForFinished();
    QByteArray memOutput = memTracer.readAllStandardOutput();
    QString s = QString(memOutput);
    uiLog.trace("top outpup: |" + s + "|");
    QStringList splitted = s.split(' ');
    splitted.removeAll("");

    int memValue;
    if(splitted.size()>=5){
        QString memString = splitted.at(5);
        if(memString.at(memString.length()-1) == 'g'){
            //section for gigabytes
            memString.chop(1);
            bool ok;
            memString.toDouble(&ok);
            if(ok){
                memValue = memString.toDouble()*1000*1000;
            }else{
                coreLog.trace("String " + memString + " could not be converted to double");
            }
        }else{
            bool ok;
            memString.toInt(&ok);
            if(ok){
                memValue = memString.toInt();
            }else{
                coreLog.trace("String " + memString + "could not be converted to int");
            }
        }
    }else{
        memValue = 0;
    }
    result+=memValue;

    foreach (int childPid, childPids) {
        result+=countMemForProcessTree(childPid);
    }
    return result;
#else
    return -1;
#endif
}

void GUITestThread::saveMemoryInfo(){
#ifdef Q_OS_LINUX
    int max = *std::max_element(memoryList.begin(), memoryList.end());
    QString filename="memFolder/memory.txt";
    QDir().mkpath("memFolder");
    QFile file( filename );
    if ( file.open(QIODevice::ReadWrite | QIODevice::Append) ){
        file.write(QString("%1_%2 %3\n").arg(test->getSuite()).arg(test->getName()).arg(max).toUtf8());
        file.close();
    }
#endif
}

}   // namespace U2
