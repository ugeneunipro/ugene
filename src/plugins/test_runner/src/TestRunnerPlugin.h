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

#ifndef _U2_TEST_RUNNER_PLUGIN_H_
#define _U2_TEST_RUNNER_PLUGIN_H_

#include <U2Core/PluginModel.h>
#include <U2Core/AppContext.h>
#include <U2Core/ServiceModel.h>
#include <U2Test/TestRunnerTask.h>
#include <QAction>

namespace U2 {

class GTestSuite;
class GTestFormat;
class TestViewController;
class GTestEnvironment;

#define NUM_THREADS_VAR "NUM_THREADS"

class TestRunnerPlugin : public Plugin {
    Q_OBJECT
public:
    TestRunnerPlugin();
private slots:
    void sl_startTestRunner();
};

class TestRunnerService : public Service {
    Q_OBJECT
public:
    TestRunnerService();
    ~TestRunnerService();

    const QList<GTestSuite*> getTestSuites() const {return suites;}
    void addTestSuite(GTestSuite*);
    void removeTestSuite(GTestSuite*);
    GTestSuite* findTestSuiteByURL(const QString& url);

    GTestEnvironment* getEnv() { return env;}
    void setVar(const QString& varName, const QString& val);
    void setEnvironment();

signals:
    void si_testSuiteAdded(GTestSuite* ts);
    void si_testSuiteRemoved(GTestSuite* ts);

protected:
    virtual void serviceStateChangedCallback(ServiceState oldState, bool enabledStateChanged);
    virtual bool eventFilter(QObject *obj, QEvent *event);

public slots:
    void sl_refresh();
private slots:
    void sl_showWindow();

private:
    void readSavedSuites();
    void saveSuites();
    void readBuiltInVars();
    void deallocateSuites();

    void readEnvForKeys(QStringList keys);
    void saveEnv();
    void updateDefaultEnvValues(GTestSuite* ts);


    TestViewController*         view;
    QAction*                    windowAction;
    QList<GTestSuite*>          suites;
    GTestEnvironment*           env;
};
/*class TestRunnerScriptModule : public GScriptModule
{
    virtual void setup(QScriptEngine *engine) const;
};*/
} //namespace

#endif
