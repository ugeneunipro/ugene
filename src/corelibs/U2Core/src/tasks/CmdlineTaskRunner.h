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

#ifndef _U2_CMDLINE_TASK_RUNNER_H_
#define _U2_CMDLINE_TASK_RUNNER_H_

#include <QProcess>
#include <U2Core/Task.h>

namespace U2 {

class U2CORE_EXPORT CmdlineTaskConfig {
public:
    CmdlineTaskConfig();

    QString             command;
    QStringList         arguments;
    LogLevel            logLevel;
    bool                withPluginList;
    QStringList         pluginList;
    QString             reportFile;
};

class U2CORE_EXPORT CmdlineTaskRunner : public Task {
    Q_OBJECT
public:
    static QList<long> getChildrenProcesses(qint64 processId, bool fullTree=true);
    static int killChildrenProcesses(qint64 processId, bool fullTree=true);
    static int killProcessTree(QProcess *process);
    static int killProcessTree(qint64 processId);
    static int killProcess(qint64 processId);

    CmdlineTaskRunner(const CmdlineTaskConfig &config);

    void prepare();
    ReportResult report();

    static const QString REPORT_FILE_ARG;

protected:
    virtual bool isCommandLogLine(const QString &logLine) const;
    virtual bool parseCommandLogWord(const QString &logWord);

private:
    void writeLog(QStringList &lines);
    QString readStdout();

private slots:
    void sl_onError(QProcess::ProcessError);
    void sl_onReadStandardOutput();
    void sl_onFinish(int exitCode, QProcess::ExitStatus exitStatus);

private:
    CmdlineTaskConfig       config;
    QProcess*               process;
    QString                 processLogPrefix;
};

class U2CORE_EXPORT CmdlineTask : public Task {
    Q_OBJECT
public:
    CmdlineTask(const QString &name, TaskFlags flags);
    ReportResult report();

protected:
    virtual QString getTaskError() const;

private slots:
    void sl_outputProgressAndState();
};

} // U2

#endif // _U2_CMDLINE_TASK_RUNNER_H_
