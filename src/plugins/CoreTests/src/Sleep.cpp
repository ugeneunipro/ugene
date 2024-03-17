/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "Sleep.h"

#include <QDomElement>
#include <QTest>

#include <U2Core/AppContext.h>
#include <U2Core/CMDLineCoreOptions.h>
#include <U2Core/CMDLineRegistry.h>

#include <U2Test/GTestFrameworkComponents.h>

#define SLEEP_TIME_DURATION "duration-ms"

namespace U2 {

void GTest_Sleep::init(XMLTestFormat*, const QDomElement& el) {
    QString v = el.attribute(SLEEP_TIME_DURATION);
    if (v.isEmpty()) {
        return;
    }
    bool ok = false;
    sleepTimer = v.toInt(&ok);
    if (!ok) {
        failMissingValue(SLEEP_TIME_DURATION);
    }
}

Task::ReportResult GTest_Sleep::report() {
    CMDLineRegistry* cmdLineRegistry = AppContext::getCMDLineRegistry();
    bool ok = false;
    int numberTestsToRun = getEnv()->getVar("NUM_THREADS").toInt(&ok);
    if (!ok) {
        failMissingValue("NUM_THREADS");
    }else if (numberTestsToRun == 1) {
        int testTimeout = cmdLineRegistry->getParameterValue(TestFramework::TEST_TIMEOUT_CMD_OPTION).toInt();
        if (sleepTimer >= MAX_SLEEP_TIMER) {
            stateInfo.setError("Sleep timer greater than max sleep timer value!");
        } else if (testTimeout > 0 && sleepTimer >= testTimeout) {
            stateInfo.setError("Sleep timer greater than test timeout value!");
        } else {
            QTest::qSleep(sleepTimer);
        }
    } else {
        stateInfo.setError("Tests with 'sleep' should be run in one thread!");
    }
    return ReportResult_Finished;
}

QList<XMLTestFactory*> SleepTests::createTestFactories() {
    return {GTest_Sleep::createFactory()};
}

}  // namespace U2
