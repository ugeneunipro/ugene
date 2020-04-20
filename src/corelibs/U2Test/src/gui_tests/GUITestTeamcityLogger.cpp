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

#include "GUITestTeamcityLogger.h"

#include <U2Core/Log.h>


#define ULOG_CAT_TEAMCITY "Teamcity Integration"

namespace U2 {
static Logger teamcityLog(ULOG_CAT_TEAMCITY);

const QString GUITestTeamcityLogger::successResult = "Success";

void GUITestTeamcityLogger::testStarted(const QString& testName) {
    teamcityLog.trace(QString("##teamcity[testStarted name='%1']").arg(escaped(testName)));
}

void GUITestTeamcityLogger::testIgnored(const QString& testName, const QString& ignoreReason) {
    teamcityLog.trace(QString("##teamcity[testIgnored name='%1' message='%2']").arg(escaped(testName), escaped(ignoreReason)));
}

void GUITestTeamcityLogger::teamCityLogResult(const QString &testName, const QString &testResult, qint64 testTimeMicros) {

    if (testFailed(testResult)) {
        teamcityLog.trace(QString("##teamcity[testFailed name='%1' message='%2' details='%2' duration='%3']").arg(escaped(testName), escaped(testResult), QString::number(testTimeMicros)));
    }

    teamcityLog.trace(QString("##teamcity[testFinished name='%1' duration='%2']").arg(escaped(testName), QString::number(testTimeMicros)));
}

QString GUITestTeamcityLogger::escaped(const QString &s) {

    QString esc = s;

    esc = esc.replace("|", "||");
    esc = esc.replace("]", "|]");
    esc = esc.replace("\r", "|r");
    esc = esc.replace("\n", "|n");
    esc = esc.replace("'", "|'");
    return esc;
}

bool GUITestTeamcityLogger::testFailed(const QString &testResult) {

    if (!testResult.contains(successResult)) {
        return true;
    }

    return false;
}

} // namespace
