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

#ifndef _U2_CMDLINE_IO_OUT_TASK_RUNNER_H_
#define _U2_CMDLINE_IO_OUT_TASK_RUNNER_H_

#include <U2Core/U2Type.h>
#include <U2Core/CmdlineTaskRunner.h>

namespace U2 {

class GObject;

class U2CORE_EXPORT CmdlineInOutTaskConfig : public CmdlineTaskConfig {
public:
    CmdlineInOutTaskConfig();

    QList<GObject*>     inputObjects;
    U2DbiRef            outDbiRef;
    bool                emptyOutputPossible;
};

class U2CORE_EXPORT CmdlineInOutTaskRunner : public CmdlineTaskRunner {
    Q_OBJECT
public:
    CmdlineInOutTaskRunner(const CmdlineInOutTaskConfig &config);

    ReportResult report();

    const QList<U2DataId> & getOutputObjects() const;

    static QString toString(const U2DbiRef &dbiRef);
    static U2DbiRef parseDbiRef(const QString &string, U2OpStatus &os);
    static QString toString(const U2DataId &id);
    static U2DataId parseDataId(const QString &string, const U2DbiRef &dbiRef, U2OpStatus &os);
    static void logOutputObject(const U2DataId &id);
    static const QString IN_DB_ARG;
    static const QString IN_ID_ARG;
    static const QString OUT_DB_ARG;

private:
    // CmdlineTaskRunner
    bool isCommandLogLine(const QString &logLine) const;
    bool parseCommandLogWord(const QString &logWord);

private:
    CmdlineInOutTaskConfig config;
    QList<U2DataId> outputObjects;
};

} // U2

#endif // _U2_CMDLINE_IO_OUT_TASK_RUNNER_H_
