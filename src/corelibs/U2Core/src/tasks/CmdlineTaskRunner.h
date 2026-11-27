/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#pragma once

#include <functional>

#include <QProcess>

#include <U2Core/Task.h>

namespace U2 {

class U2CORE_EXPORT CmdlineTaskConfig {
public:
    QString command;
    QStringList arguments;
    LogLevel logLevel = LogLevel_DETAILS;
    bool withPluginList = false;
    QStringList pluginList;
    QString reportFile;
};

class U2CORE_EXPORT CmdlineTaskRunner : public Task {
    Q_OBJECT
public:
    /** Creates a process that owns its descendants through a Windows Job Object. Returns nullptr on error. */
    static QProcess* createProcess(U2OpStatus& os, QObject* parent = nullptr);
    static QList<long> getChildrenProcesses(qint64 processId, bool fullTree = true);
    static int killChildrenProcesses(qint64 processId, bool fullTree = true);
    static int killProcessTree(QProcess* process);
    static int killProcessTree(qint64 processId);
    static int killProcess(qint64 processId);

    CmdlineTaskRunner(const CmdlineTaskConfig& config);

    void prepare() override;
    ReportResult report() override;

    /*
     * Returns QString with [ERROR] level log messages separated by new line.
     */
    QString getProcessErrorsLog() const;

    static const QString REPORT_FILE_ARG;

protected:
    virtual bool isCommandLogLine(const QString& logLine) const;
    virtual bool parseCommandLogWord(const QString& logWord);

private:
    friend class CmdlineTaskRunnerUnitTests_processTree;
    friend class WindowsProcessTree;

#ifndef Q_OS_WIN
    static QList<long> getDirectChildrenProcesses(qint64 processId);
#endif
    static QList<long> getChildrenProcesses(qint64 processId, bool fullTree, const std::function<QList<long>(qint64)>& getDirectChildren);
    static bool isProcessSnapshotChild(quint64 parentCreated, quint64 childCreated, quint64 snapshotTime);

    void writeLog(QStringList& messages);
    QString readStdout();

private slots:
    void sl_onError(QProcess::ProcessError);
    void sl_onReadStandardOutput();
    void sl_onFinish(int exitCode, QProcess::ExitStatus exitStatus);

private:
    CmdlineTaskConfig config;
    QProcess* process;
    QString processLogPrefix;
    QString processErrorLog;
};

class U2CORE_EXPORT CmdlineTask : public Task {
    Q_OBJECT
public:
    CmdlineTask(const QString& name, TaskFlags flags);
    ReportResult report() override;

protected:
    virtual QString getTaskError() const;

private slots:
    void sl_outputProgressAndState();
};

}  // namespace U2
