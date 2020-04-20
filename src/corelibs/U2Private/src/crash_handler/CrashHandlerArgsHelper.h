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

#ifndef _U2_CRASH_HANDLER_ARGS_HELPER_H_
#define _U2_CRASH_HANDLER_ARGS_HELPER_H_

#include <QFile>

namespace U2 {

class U2OpStatus;

class CrashHandlerArgsHelper {
public:
    CrashHandlerArgsHelper();
    ~CrashHandlerArgsHelper();

    int getMaxReportSize() const;
    QStringList getArguments() const;

    void setReportData(const QString &data);
    void setDumpUrl(const QString &url);

private:
    void shutdownSessionDatabase();

    QString reportUrl;
    QString databaseUrl;
    QString dumpUrl;
    bool useFile;
    QFile file;
    QString report;

    static QString findTempDir(U2OpStatus &os);
    static QString findFilePathToWrite(U2OpStatus &os);

    static const QString SESSION_DB_FILE_ARG;
    static const QString DUMP_FILE_ARG;
    static const QString REPORT_FILE_ARG;
    static const int MAX_PLAIN_LOG = 70;
    static const int MAX_FILE_LOG = 500;
    static const QString SILENT_SEND_FILE_ARG;
    static const QString FAILED_TEST_FILE_ARG;
};

}   // namespace U2

#endif // _U2_CRASH_HANDLER_ARGS_HELPER_H_
