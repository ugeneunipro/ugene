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

#ifndef _U2_TEST_RUNNER_TASK_H_
#define _U2_TEST_RUNNER_TASK_H_

#include <QList>
#include <QMap>

#include <U2Core/Task.h>

namespace U2 {

class GTestEnvironment;
class GTestState;
class GTest;
#define TIME_OUT_VAR "TIME_OUT_VAR"
#define ULOG_CAT_TEAMCITY "Teamcity Integration"

class U2TEST_EXPORT TestRunnerTask : public Task {
    Q_OBJECT
public:
    TestRunnerTask(const QList<GTestState*>& tests, const GTestEnvironment* env, int testSizeToRun = 5);

    virtual void cleanup();

    const QMap<GTest*, GTestState*>& getStateByTestMap() const {
        return stateByTest;
    }

protected:
    virtual QList<Task*> onSubTaskFinished(Task* subTask);

    QMap<GTest*, GTestState*> stateByTest;
    const GTestEnvironment* env;
    int sizeToRun;
    int finishedTests;
    int totalTests;
    QList<GTestState*> awaitingTests;

    /** All tests environments created during the run. Will be deleted with the test. */
    QList<GTestEnvironment*> allTestEnvironments;
};

class U2TEST_EXPORT LoadTestTask : public Task {
    Q_OBJECT
public:
    LoadTestTask(GTestState* test);

    void run();

    GTestState* testState;

    QByteArray testData;
};

}  // namespace U2

#endif
