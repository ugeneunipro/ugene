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

#include <assert.h>

#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QMap>

#include <U2Core/AppContext.h>
#include <U2Core/Timer.h>
#include <U2Core/U2SafePoints.h>

#include "GTest.h"

namespace U2 {

/* TRANSLATOR U2::GTest */

//////////////////////////////////////////////////////////////////////////
// GTest

GTest::GTest(const QString& taskName, GTest* cp, const GTestEnvironment* _env, TaskFlags flags, const QList<GTest*>& subtasks)
    : Task(taskName, flags), contextProvider(cp), env(_env) {
    assert(env != nullptr);

    foreach (Task* t, subtasks) {
        addSubTask(t);
    }
}

QObject* GTest::getContext(const QString& name) const {
    assert(contextProvider != nullptr);
    return contextProvider->subtestsContext.value(name);
}

void GTest::addContext(const QString& name, QObject* v) {
    assert(contextProvider != nullptr);
    assert(!contextProvider->subtestsContext.contains(name));
    contextProvider->subtestsContext[name] = v;
}

void GTest::removeContext(const QString& name) {
    assert(contextProvider != nullptr);
    assert(contextProvider->subtestsContext.contains(name));
    contextProvider->subtestsContext.remove(name);
}

void GTest::removeTempDir() {
    QDir tempDir(env->getVar("TEMP_DATA_DIR"));
    taskLog.trace(QString("Removing test temporary dir: %1").arg(tempDir.path()));
    tempDir.removeRecursively();
}

void GTest::failMissingValue(const QString& name) {
    stateInfo.setError(QString("Mandatory attribute not set: %1").arg(name));
}

void GTest::wrongValue(const QString& name) {
    stateInfo.setError(QString("Wrong value for attribute: %1").arg(name));
}

void GTest::emptyValue(const QString& name) {
    stateInfo.setError(QString("Empty value for attribute: %1").arg(name));
}

//////////////////////////////////////////////////////////////////////////
// GTestSuite

GTestSuite::~GTestSuite() {
    qDeleteAll(tests);
    QMap<GTestRef*, QString>::iterator iter;
    for (iter = excluded.begin(); iter != excluded.end(); ++iter) {
        delete iter.key();
    }
}

static QStringList findAllFiles(const QString& dirPath, const QString& ext, bool recursive, int rec) {
    QStringList res;
    if (rec > 100) {  // symlink or other err
        // todo: report err?
        return res;
    }
    QDir dir(dirPath);

    // add files first
    QStringList files = ext.isEmpty() ? dir.entryList(QDir::Files) : dir.entryList(ext.split(":"), QDir::Files);
    foreach (const QString& file, files) {
        QFileInfo fi(dir.absolutePath() + "/" + file);
        res.append(fi.absoluteFilePath());
    }

    // process subdirs if needed
    if (recursive) {
        QStringList subDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        foreach (QString sub, subDirs) {
            QString subDirPath = dirPath + "/" + sub;
            QStringList subRes = findAllFiles(subDirPath, ext, true, rec++);
            res += subRes;
        }
    }
    return res;
}

static QString addExcludeTests(const QString& fullTestDirPath, const QString& str, QList<QRegExp>& xList) {
    QString err;

    if (str.isEmpty()) {
        return err;
    }

    foreach (const QString& s, str.split(",")) {
        QRegExp r(fullTestDirPath + "/" + s.trimmed(), Qt::CaseSensitive, QRegExp::Wildcard);
        if (!r.isValid()) {
            err = QString("Invalid exclude: %1").arg(s);
            break;
        }
        xList << r;
    }
    return err;
}

GTestSuite* GTestSuite::readTestSuite(const QString& url, QString& err) {
    QFile f(url);
    if (!f.open(QIODevice::ReadOnly)) {
        err = "Can't open file: " + url;
        return nullptr;
    }
    QByteArray xmlData = f.readAll();
    f.close();

    QDomDocument suiteDoc;
    bool res = suiteDoc.setContent(xmlData);
    if (!res) {
        err = "Not XML doc: " + url;
        return nullptr;
    }

    QDomElement suiteEl = suiteDoc.documentElement();
    if (suiteEl.tagName() != "suite") {
        err = "Top level element is not <suite>: " + url;
        return nullptr;
    }

    // Name
    QString suiteName = suiteEl.attribute("name");
    if (suiteName.isEmpty()) {
        err = "Suite name is empty: " + url;
        return nullptr;
    }

    // Test timeout
    QString testTimeout = suiteEl.attribute("test-timeout", "0");

    // Env
    GTestEnvironment suiteEnv;
    QDomNodeList envNodes = suiteEl.elementsByTagName("env-var");
    for (int i = 0; i < envNodes.size(); i++) {
        QDomNode n = envNodes.item(i);
        assert(n.isElement());
        if (!n.isElement()) {
            continue;
        }
        QDomElement varEl = n.toElement();
        QString varName = varEl.attribute("name");
        if (varName.isEmpty()) {
            err = "var_tag_without_name: " + url;
            return nullptr;
        }
        QString varVal = varEl.attribute("value");
        suiteEnv.setVar(varName, varVal);
    }

    // Tests
    QFileInfo suiteUrl(url);
    QString suiteDir = suiteUrl.absoluteDir().absolutePath();
    QList<GTestRef*> suiteTests;
    QMap<GTestRef*, QString> excluded;

    QString dirPath = "";
    QString testFormatName;
    QDomNodeList testDirEls = suiteEl.elementsByTagName("test-dir");

    for (int i = 0; i < testDirEls.size(); i++) {
        QDomNode n = testDirEls.item(i);
        assert(n.isElement());
        if (!n.isElement()) {
            continue;
        }
        QDomElement testDirEl = n.toElement();
        dirPath = testDirEl.attribute("path");
        if (dirPath.isEmpty()) {
            err = "path_attribute_not_found: " + url;
            break;
        }
        QString fullTestDirPath = suiteDir + "/" + dirPath;
        QFileInfo testDir(fullTestDirPath);
        if (!testDir.exists()) {
            err = "test_dir_not_exists:" + fullTestDirPath;
            break;
        }

        QList<QRegExp> excludeRexExList;
        err = addExcludeTests(fullTestDirPath, testDirEl.attribute("exclude"), excludeRexExList);
        if (!err.isEmpty()) {
            break;
        }

        if (sizeof(void*) == 8) {  // means that it is 64 bit system
            err = addExcludeTests(fullTestDirPath, testDirEl.attribute("exclude_64"), excludeRexExList);
            if (!err.isEmpty()) {
                break;
            }
        }

        if (sizeof(void*) == 4) {  // means that it is 32 bit system
            err = addExcludeTests(fullTestDirPath, testDirEl.attribute("exclude_32"), excludeRexExList);
            if (!err.isEmpty()) {
                break;
            }
        }

        testFormatName = testDirEl.attribute("test-format");
        bool recursive = testDirEl.attribute("recursive") != "false";
        QString testExt = testDirEl.attribute("test-ext");
        QStringList testUrls = findAllFiles(fullTestDirPath, testExt, recursive, 0);
        for (const QString& testUrl : qAsConst(testUrls)) {
            int shortNameLen = testUrl.length() - fullTestDirPath.length() - 1;  // minus '/' char
            assert(shortNameLen > 0);
            QString tShortName = testUrl.right(shortNameLen);
            // all tests appended
            suiteTests << new GTestRef(testUrl, tShortName, testFormatName);
        }
    }

    // excluded
    QDomNodeList excludedEls = suiteEl.elementsByTagName("excluded");
    for (int i = 0; i < excludedEls.size(); i++) {
        QDomNode n = excludedEls.item(i);
        assert(n.isElement());
        if (!n.isElement()) {
            continue;
        }
        QDomElement excludedEl = n.toElement();
        QString platform = excludedEl.attribute("platform");

        // Check if 'exclude' is per single OS only.
        if (platform == "windows" && !isOsWindows()) {
            continue;
        }
        if (platform == "linux" && !isOsLinux()) {
            continue;
        }
        if (platform == "mac" && !isOsMac()) {
            continue;
        }
        QString testName = excludedEl.attribute("test");

        QString fullTestPath = suiteDir + "/" + dirPath + "/" + testName;

        auto tref = new GTestRef(fullTestPath, testName, testFormatName);
        QString reason = excludedEl.attribute("reason");

        if (!excluded.contains(tref)) {
            excluded.insert(tref, reason);
        }
    }

    // take excluded from all tests
    foreach (GTestRef* test, suiteTests) {
        QMap<GTestRef*, QString>::iterator iter;
        for (iter = excluded.begin(); iter != excluded.end(); ++iter) {
            GTestRef* ref = dynamic_cast<GTestRef*>(iter.key());
            if (*test == *ref) {
                suiteTests.removeOne(test);
            }
        }
    }

    if (!err.isEmpty()) {
        qDeleteAll(suiteTests);
        QMap<GTestRef*, QString>::iterator iter;
        for (iter = excluded.begin(); iter != excluded.end(); ++iter) {
            delete iter.key();
        }
        return nullptr;
    }

    GTestSuite* suite = new GTestSuite();
    suite->url = suiteUrl.absoluteFilePath();
    suite->name = suiteName;
    suite->env = suiteEnv;
    suite->tests = suiteTests;
    suite->excluded = excluded;
    // testTimeout format is unchecked, but it will fail to 0 it's ok
    suite->testTimeout = testTimeout.toInt();
    suite->testTimeout = (suite->testTimeout == 0) ? -1 : suite->testTimeout;  // -1 means timeout check disabled

    for (GTestRef* r : qAsConst(suiteTests)) {
        r->setSuite(suite);
    }
    const QList<GTestRef*> excludeKeyList = excluded.keys();
    for (GTestRef* r : qAsConst(excludeKeyList)) {
        r->setSuite(suite);
    }

    return suite;
}

QList<GTestSuite*> GTestSuite::readTestSuiteList(const QString& url, QStringList& errs) {
    QList<GTestSuite*> result;
    QFile suitListFile(url);
    QString dir = QFileInfo(url).dir().absolutePath();
    if (!suitListFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        printf("%s\n", tr("Can't load suite list %1").arg(url).toLatin1().constData());
        errs << tr("Can't open suite list %1").arg(url);
        return result;
    }
    QString suiteFileContent = suitListFile.readAll();
    QStringList suiteNamesList = suiteFileContent.split(QRegExp("\\s+"));
    for (auto suiteName : qAsConst(suiteNamesList)) {
        if (suiteName.isEmpty()) {
            continue;
        }
        suiteName = suiteName.trimmed();
        if (suiteName.startsWith("#")) {  // this is a comment line
            continue;
        }
        QString absolutePath = QFileInfo(dir + "/" + suiteName).absoluteFilePath();
        QString error;
        GTestSuite* testSuite = GTestSuite::readTestSuite(absolutePath, error);
        if (testSuite == nullptr) {
            assert(!error.isEmpty());
            errs << error;
        } else {
            coreLog.info(QString("Loaded test suite '%1', tests: %2").arg(testSuite->getName()).arg(testSuite->tests.size()));
            result << testSuite;
        }
    }
    return result;
}

//////////////////////////////////////////////////////////////////////////
// GTestState

void GTestState::clearState() {
    if (state == TriState_Unknown) {
        assert(errMessage.isEmpty());
        return;
    }
    errMessage.clear();
    state = TriState_Unknown;
    emit si_stateChanged(this);
}

void GTestState::setFailed(const QString& err) {
    if (state == TriState_No) {
        assert(!errMessage.isEmpty());
        return;
    }
    assert(!err.isEmpty());
    assert(errMessage.isEmpty());

    errMessage = err;
    state = TriState_No;
    emit si_stateChanged(this);
}

void GTestState::setPassed() {
    if (state == TriState_Yes) {
        assert(errMessage.isEmpty());
        return;
    }
    errMessage.clear();
    state = TriState_Yes;
    emit si_stateChanged(this);
}

//////////////////////////////////////////////////////////////////////////
// GTestLogHelper
GTestLogHelper::GTestLogHelper()
    : statusWasVerified(false) {
}

GTestLogHelper::~GTestLogHelper() {
    if (!statusWasVerified && !expectedMessages.isEmpty()) {
        LogServer::getInstance()->removeListener(this);
    }
}

void GTestLogHelper::initMessages(const QStringList& expectedList, const QStringList& unexpectedList) {
    statusWasVerified = false;
    foreach (QString message, expectedList) {
        expectedMessages[message] = false;  // i.e. not detected yet
    }

    foreach (QString message, unexpectedList) {
        unexpectedMessages[message] = false;
    }

    logHelperStartTime = GTimer::currentTimeMicros();
    LogServer::getInstance()->addListener(this);
}

GTestLogHelperStatus GTestLogHelper::verifyStatus() {
    LogServer::getInstance()->removeListener(this);
    GTestLogHelperStatus status = GTest_LogHelper_Valid;

    foreach (const QString& str, expectedMessages.keys()) {
        if (false == expectedMessages[str]) {
            status = GTest_LogHelper_Invalid;
            coreLog.error(QString("GTestLogHelper: no expected message \"%1\" in the log!").arg(str));
        }
    }

    foreach (const QString& str, unexpectedMessages.keys()) {
        if (true == unexpectedMessages[str]) {
            status = GTest_LogHelper_Invalid;
            coreLog.error(QString("GTestLogHelper: message \"%1\" is present in the log unexpectedly!").arg(str));
        }
    }

    statusWasVerified = true;
    logHelperEndTime = GTimer::currentTimeMicros();

    return status;
}

void GTestLogHelper::onMessage(const LogMessage& logMessage) {
    qint64 currentTime = GTimer::currentTimeMicros();

    SAFE_POINT(logMessage.time >= logHelperStartTime,
               QString("Internal error in GTestLogHelper (incorrect start time): message '%1' with time '%2' appeared"
                       " in log at time '%3'. GTestLogHelper start time is '%4'.")
                   .arg(logMessage.text)
                   .arg(logMessage.time)
                   .arg(currentTime)
                   .arg(logHelperStartTime), );

    if (statusWasVerified) {
        FAIL(QString("Internal error in GTestLogHelper (got a message after verifying the status):"
                     " message '%1' with time '%2' appeared"
                     " in log at time '%3'. GTestLogHelper end time is '%4'.")
                 .arg(logMessage.text)
                 .arg(logMessage.time)
                 .arg(currentTime)
                 .arg(logHelperEndTime), );
    }

    foreach (const QString& str, expectedMessages.keys()) {
        if (logMessage.text.contains(str)) {
            expectedMessages[str] = true;
        }
    }

    foreach (const QString& str, unexpectedMessages.keys()) {
        if (logMessage.text.contains(str)) {
            unexpectedMessages[str] = true;
        }
    }
}

}  // namespace U2
