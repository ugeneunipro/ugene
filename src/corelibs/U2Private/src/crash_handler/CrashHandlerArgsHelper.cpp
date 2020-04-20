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

#include <U2Core/AppContext.h>
#include <U2Core/CMDLineCoreOptions.h>
#include <U2Core/CMDLineRegistry.h>
#include <U2Core/TmpDirChecker.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "CrashHandlerArgsHelper.h"

namespace U2 {

const QString CrashHandlerArgsHelper::SESSION_DB_FILE_ARG = "-d";
const QString CrashHandlerArgsHelper::DUMP_FILE_ARG = "-dump";
const QString CrashHandlerArgsHelper::REPORT_FILE_ARG = "-f";
const QString CrashHandlerArgsHelper::SILENT_SEND_FILE_ARG = "--silent-sending";
const QString CrashHandlerArgsHelper::FAILED_TEST_FILE_ARG = "--failed-test";

CrashHandlerArgsHelper::CrashHandlerArgsHelper()
    : useFile(false)
{
    U2OpStatusImpl os;
    reportUrl = findFilePathToWrite(os);
    CHECK_OP(os, );

    file.setFileName(reportUrl);
    useFile = file.open(QIODevice::WriteOnly);

    shutdownSessionDatabase();
}

CrashHandlerArgsHelper::~CrashHandlerArgsHelper() {
    if (file.isOpen()) {
        file.close();
    }
}

int CrashHandlerArgsHelper::getMaxReportSize() const {
    if (useFile) {
        return MAX_FILE_LOG;
    }
    return MAX_PLAIN_LOG;
}

QStringList CrashHandlerArgsHelper::getArguments() const {
    QStringList args;
    if (!databaseUrl.isEmpty()) {
        args << SESSION_DB_FILE_ARG << databaseUrl;
    }
    if (!dumpUrl.isEmpty()) {
        args << DUMP_FILE_ARG << dumpUrl;
    }
    if (useFile) {
        args << REPORT_FILE_ARG << reportUrl;
    } else {
        args << report.toUtf8().toBase64();
    }

    if (qgetenv(ENV_GUI_TEST).toInt() == 1) {
        CMDLineRegistry* cmdLine = AppContext::getCMDLineRegistry();
        if (NULL != cmdLine) {
            QString testName = cmdLine->getParameterValue(CMDLineCoreOptions::LAUNCH_GUI_TEST);
            args << SILENT_SEND_FILE_ARG;
            args << FAILED_TEST_FILE_ARG << testName;
        }
    }

    return args;
}

void CrashHandlerArgsHelper::setReportData(const QString &data) {
    if (useFile) {
        QByteArray bytes = data.toUtf8();
        file.write(bytes);
        file.close();
    } else {
        report = data;
    }
}

void CrashHandlerArgsHelper::setDumpUrl(const QString &url) {
    dumpUrl = url;
}

QString CrashHandlerArgsHelper::findTempDir(U2OpStatus &os) {
    if (TmpDirChecker::checkWritePermissions(QDir::tempPath())) {
        return QDir::tempPath();
    }
    if (TmpDirChecker::checkWritePermissions(QDir::homePath())) {
        return QDir::homePath();
    }
    os.setError("No accessible dir");
    return "";
}

QString CrashHandlerArgsHelper::findFilePathToWrite(U2OpStatus &os) {
    QString dirPath = findTempDir(os);
    CHECK_OP(os, "");

    return TmpDirChecker::getNewFilePath(dirPath, "crash_report");
}

void CrashHandlerArgsHelper::shutdownSessionDatabase() {
    U2DbiRegistry *dbiReg = AppContext::getDbiRegistry();
    CHECK(NULL != dbiReg, );

    U2OpStatusImpl os;
    const QString url = dbiReg->shutdownSessionDbi(os);
    if (!os.hasError()) {
        databaseUrl = url;
    }
}

}   // namespace U2
