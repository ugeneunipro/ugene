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

#ifndef _U2_UGENE_TEST_FRAMEWORK_H_
#define _U2_UGENE_TEST_FRAMEWORK_H_


#include <U2Core/Log.h>
#include <U2Core/Task.h>

#include <QString>
#include <QList>
#include <QObject>
#include <QMap>

namespace U2 {

/** Tests that need to verify log uses this resource */
#define RESOURCE_LISTEN_LOG_IN_TESTS    1000001


class U2TEST_EXPORT GTestEnvironment {
public:
    void setVar(const QString& varName, const QString& val) {vars[varName] = val;}

    QString getVar(const QString& varName) const {return vars.value(varName);}

    QMap<QString, QString> getVars() const {return vars;}

    bool containsEmptyVars() const {return vars.values().contains(QString(""));}

private:
    QMap<QString, QString> vars;
};


class U2TEST_EXPORT GTest : public Task {
    Q_OBJECT
public:
    GTest(const QString& taskName, GTest* cp, const GTestEnvironment* env,
        TaskFlags flags, const QList<GTest*>& subtasks = QList<GTest*>());

    QObject* getContext(const QString& name) const;

    void addContext(const QString& name, QObject* v);

    void removeContext(const QString& name);

    template <class T>
    static T* getContext(const GTest* test, const QString& name) {
        QObject* o = test->getContext(name);
        T* res = qobject_cast<T*>(o);
        return res;
    }

    const GTestEnvironment* getEnv() const {return env;}

    void removeTempDir();

protected:
    GTest* getContextProvider() const {return contextProvider;}

    void failMissingValue(const QString &attribute);
    void wrongValue(const QString &attribute);
    void emptyValue(const QString &attribute);

    GTest* contextProvider;
    const GTestEnvironment* env;
    QMap<QString, QObject*> subtestsContext;
};


class U2TEST_EXPORT GTestFormat : public QObject {
public:
    GTestFormat(const QString& _formatId) : formatId(_formatId){};

    const QString getFormatId() const {return formatId;}

    virtual GTest* createTest(const QString& name, GTest* cp, const GTestEnvironment* env, const QByteArray& testData, QString& err) = 0;

protected:
    QString formatId;
};

class GTestSuite;
class U2TEST_EXPORT GTestRef {
public:
    GTestRef(const QString& _url, const QString& sName, const GTestFormatId f)
        : url(_url), shortName(sName), formatId(f), suite(NULL) {}

    const QString& getURL() const {return url;}

    const QString& getShortName() const {return shortName;}

    const GTestFormatId getFormatId() {return formatId;}

    void setSuite( GTestSuite * _suite ) { suite = _suite; }

    GTestSuite * getSuite() const {return suite;}

    bool operator ==( const GTestRef& other) const {return (url == other.url) && (formatId == other.formatId);}

private:
    QString             url;
    QString             shortName;
    GTestFormatId       formatId;
    GTestSuite *        suite;
};

class U2TEST_EXPORT GTestState : public QObject {
    Q_OBJECT
public:
    GTestState(GTestRef* ref) : testRef(ref), state(TriState_Unknown) {}

    GTestRef* getTestRef() const {return testRef;}

    bool isPassed() const {return state == TriState_Yes;}

    bool isFailed() const {return state == TriState_No;}

    bool isNew() const {return state == TriState_Unknown;}

    const QString& getErrorMessage() const {return errMessage;}

    void clearState();

    void setFailed(const QString& err);

    void setPassed();

signals:
    void si_stateChanged(GTestState* ts);

private:
    GTestRef*           testRef;
    TriState            state;
    QString             errMessage;
};


class U2TEST_EXPORT GTestSuite  : public QObject {
    Q_OBJECT
public:
    virtual ~GTestSuite();

    GTestEnvironment* getEnv() {return &env;}

    const QList<GTestRef*>& getTests() const {return tests;}
    const QMap<GTestRef*, QString>& getExcludedTests() const {return excluded;}

    const QString& getURL() const {return url;}

    const QString& getName() const {return name;}

    int getTestTimeout() const { return testTimeout; }

    static GTestSuite* readTestSuite(const QString& url, QString& err);
    static QList<GTestSuite*> readTestSuiteList(const QString& url, QStringList& err);

protected:
    GTestSuite()
        : url(), name(), tests(), excluded(), testTimeout(0), env()
    {}

    QString url;
    QString name;

    QList<GTestRef*> tests;
    QMap<GTestRef*, QString> excluded;

    /** Maximum run time for single test in sutie.
        timeout = 0 means this parameter unused */
    int testTimeout;

    GTestEnvironment env;
};



enum GTestLogHelperStatus {GTest_LogHelper_Invalid, GTest_LogHelper_Valid};

/**
 * Helps to verify if the log contains a message (or several messages),
 * or, visa versa, doesn't contain a message (or several messages).
 *
 * To use it, create a new instance of the class in a test init function.
 * Call "expectLogMessage" for each expected message.
 * Call "expectNoLogMessage" for messages that mustn't be in the log.
 * In the test's report() or similar function call verifyStatus().
 *
 * WARNING: All tests that listen to log need also to define attribute
 * "lockForLogListening" for "multi-test" tag! This is done to avoid mixing of log
 * between different tests.
 */
class U2TEST_EXPORT GTestLogHelper : public QObject, public LogListener {
    Q_OBJECT
public:
    GTestLogHelper();
    ~GTestLogHelper();

    void initMessages(const QStringList& expectedMessages, const QStringList& unexpectedMessages);

    GTestLogHelperStatus verifyStatus();

private:
    void onMessage(const LogMessage& logMessage);

    QMap<QString, bool> expectedMessages;
    QMap<QString, bool> unexpectedMessages; // Messages that mustn't be present in the log

    qint64      logHelperStartTime;
    qint64      logHelperEndTime;

    bool        statusWasVerified;
};

}//namespace

#endif
