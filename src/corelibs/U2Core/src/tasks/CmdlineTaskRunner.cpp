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

#include <QTextCodec>
#include <QTimer>
#include <U2Core/AppContext.h>
#include <U2Core/CMDLineUtils.h>
#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>

#if (defined(Q_OS_WIN32) || defined(Q_OS_WINCE))
#include <Windows.h>
#endif

#ifdef Q_OS_WIN
#include <tlhelp32.h>
#else
#include <unistd.h>
#endif

#include "CmdlineTaskRunner.h"

namespace U2 {


CmdlineTaskConfig::CmdlineTaskConfig()
    : logLevel(LogLevel_DETAILS), withPluginList(false) {}

/************************************************************************/
/* CmdlineTaskRunner */
/************************************************************************/
namespace {
const QString OUTPUT_ERROR_ARG = "ugene-output-error";
const QString OUTPUT_PROGRESS_ARG = "ugene-output-progress-state";
const QString OUTPUT_PROGRESS_TAG = "task-progress=";
const QString ERROR_KEYWORD = "#%*ugene-finished-with-error#%*";

inline int getLogNameCandidate(const QString &line, QString &nameCandidate) {
    if ("" == line) {
        return -1;
    }

    if (!line.startsWith("[")) {
        return -1;
    }

    // maybe, @line is "[time][loglevel] log"
    int openPos = line.indexOf("[", 1); // 1 because it is needed to skip first [time] substring
    if (-1 == openPos) {
        return -1;
    }
    int closePos = line.indexOf("]", openPos);
    if (-1 == closePos) {
        return -1;
    }
    nameCandidate = line.mid(openPos + 1, closePos - openPos - 1);
    return closePos;
}

QString getLogLevelName(LogLevel l) {
    switch (l) {
    case LogLevel_TRACE: return "TRACE";
    case LogLevel_DETAILS: return "DETAILS";
    case LogLevel_INFO: return "INFO";
    case LogLevel_ERROR: return "ERROR";
    default: assert(0);
    }
    return "";
}

bool containsPrefix(const QStringList &args, const QString &prefix) {
    foreach(const QString &arg, args) {
        if (arg.startsWith(prefix)) {
            return true;
        }
    }
    return false;
}
}

const QString CmdlineTaskRunner::REPORT_FILE_ARG = "ugene-write-task-report-to-file";

QList<long> CmdlineTaskRunner::getChildrenProcesses(qint64 processId, bool fullTree) {
    QList<long> children;

#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    char *buff = NULL;
    size_t len = 255;
    char command[256] = {0};

    sprintf(command,"ps -ef|awk '$3==%u {print $2}'", (unsigned)processId);
    FILE *fp = (FILE*) popen(command, "r");
    while (getline(&buff, &len, fp) >= 0) {
        int child_process_id = QString(buff).toInt();
        if (child_process_id != 0) {
            children << child_process_id;
        }
    }
    free(buff);
    fclose(fp);
#elif defined(Q_OS_WIN)
    HANDLE hProcessSnap;
    HANDLE hProcess;
    PROCESSENTRY32 pe32;

    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    CHECK(hProcessSnap != INVALID_HANDLE_VALUE, children);

    // Set the size of the structure before using it.
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Retrieve information about the first process,
    // and exit if unsuccessful
    CHECK_EXT(Process32First(hProcessSnap, &pe32), CloseHandle(hProcessSnap), children);

    // Now walk the snapshot of processes, and
    // display information about each process in turn
    do {
        if (pe32.th32ParentProcessID == processId) {
            hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
            if (hProcess != NULL) {
                CloseHandle(hProcess);
                children << pe32.th32ProcessID;
            }
        }
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
#endif

    if (fullTree && children.length() > 0) {
        foreach(long child, children) {
            QList<long> children2 = getChildrenProcesses(child, fullTree);
            children << children2;
        }
    }

    return children;
}

int CmdlineTaskRunner::killChildrenProcesses(qint64 processId, bool fullTree) {
    int result = 0;

    QList<long> children = getChildrenProcesses(processId, fullTree);

    if (!children.isEmpty()) {
        uiLog.trace("kill all children of process: " + QString::number(processId));
    }
    while (!children.isEmpty()) {
        long child = children.takeLast();

        uiLog.trace("    kill process: " + QString::number(child));
        result += killProcess(child);

#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
        usleep(1000000);
#elif defined(Q_OS_WIN)
        Sleep(1000);
#endif
    }

    return result;
}

int CmdlineTaskRunner::killProcessTree(QProcess *process) {
    qint64 processId = process->processId();
    return killProcessTree(processId);
}

int CmdlineTaskRunner::killProcessTree(qint64 processId) {
    uiLog.trace("Killing full processes tree: " + QString::number(processId));
    uiLog.trace("Killing children processes: " + QString::number(processId));
    int result = killChildrenProcesses(processId);
    uiLog.trace("Killing parent process: " + QString::number(processId));
    result += killProcess(processId);
    return result;
}

int CmdlineTaskRunner::killProcess(qint64 processId) {
    int result = 0;
#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    int exist = QProcess::execute("kill -0 " + QString::number(processId));
    if (exist == 0) {
        result = QProcess::execute("kill -9 " + QString::number(processId));
    }
#elif defined(Q_OS_WIN)
    DWORD dwDesiredAccess = PROCESS_TERMINATE;
    BOOL  bInheritHandle = FALSE;
    HANDLE hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, processId);
    if (hProcess != NULL) {
        result = TerminateProcess(hProcess, 1);
        CloseHandle(hProcess);
    }
#endif
    return result;
}

CmdlineTaskRunner::CmdlineTaskRunner(const CmdlineTaskConfig &config)
    : Task(tr("Run UGENE command line: %1").arg(config.command), TaskFlag_NoRun), config(config), process(NULL) {
    tpm = Progress_Manual;
}

void CmdlineTaskRunner::prepare() {
    QStringList args;
    args << config.command;
    args << "--log-no-task-progress";
    args << QString("--%1").arg(OUTPUT_PROGRESS_ARG);
    args << QString("--%1").arg(OUTPUT_ERROR_ARG);
    args << QString("--ini-file=\"%1\"").arg(AppContext::getSettings()->fileName());

    if (!config.reportFile.isEmpty()) {
        args << QString("--%1=\"%2\"").arg(REPORT_FILE_ARG).arg(config.reportFile);
    }

    args << config.arguments;
    if (config.withPluginList) {
        args << QString("--%1=\"%2\"").arg(CMDLineRegistry::PLUGINS_ARG).arg(config.pluginList.join(";"));
    }

    if (!containsPrefix(args, "--log-level")) {
        QString logLevel = getLogLevelName(config.logLevel).toLower();
        args << ("--log-level-" + logLevel);
    }

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert(ENV_SEND_CRASH_REPORTS, "0");

    process = new QProcess(this);
    process->setProcessEnvironment(env);
    connect(process, SIGNAL(error(QProcess::ProcessError)), SLOT(sl_onError(QProcess::ProcessError)));
    connect(process, SIGNAL(readyReadStandardOutput()), SLOT(sl_onReadStandardOutput()));
    connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &CmdlineTaskRunner::sl_onFinish);

    QString cmdlineUgenePath(CMDLineRegistryUtils::getCmdlineUgenePath());
    coreLog.details("Starting UGENE command line: " + cmdlineUgenePath + " " + args.join(" "));
    process->start(cmdlineUgenePath, args);
#if (defined(Q_OS_WIN32) || defined(Q_OS_WINCE))
    QString processId = NULL != process->pid() ? QString::number(process->pid()->dwProcessId) : "unknown";
    processLogPrefix = QString("process:%1>").arg(processId);
#else
    processLogPrefix = QString("process:%1>").arg(process->pid());
#endif
    bool startedSuccessfully = process->waitForStarted();
    CHECK_EXT(startedSuccessfully, setError(tr("Cannot start process '%1'").arg(cmdlineUgenePath)), );
}

Task::ReportResult CmdlineTaskRunner::report() {
    CHECK(NULL != process, ReportResult_Finished);
    if (hasError()) {
        return ReportResult_Finished;
    }
    if (isCanceled()) {
        ExternalToolRunTask::killProcess(process);
        return ReportResult_Finished;
    }
    QProcess::ProcessState state = process->state();
    if (state == QProcess::Running) {
        return ReportResult_CallMeAgain;
    }
    return ReportResult_Finished;
}

bool CmdlineTaskRunner::isCommandLogLine(const QString &/*logLine*/) const {
    return false;
}

bool CmdlineTaskRunner::parseCommandLogWord(const QString &/*logWord*/) {
    return false;
}

void CmdlineTaskRunner::writeLog(QStringList &lines) {
    QStringList::Iterator it = lines.begin();
    for (; it != lines.end(); it++) {
        QString &line = *it;
        line = line.trimmed();
        QString nameCandidate;
        int closePos = getLogNameCandidate(line, nameCandidate);
        if (-1 == closePos) {
            continue;
        }

        for (int i = config.logLevel; i < LogLevel_NumLevels; i++) {
            QString logLevelName = getLogLevelName((LogLevel)i);
            if (logLevelName != nameCandidate) {
                continue;
            }

            QString logLine = line.mid(closePos + 1);
            logLine = logLine.trimmed();
            bool commandToken = logLine.startsWith(OUTPUT_PROGRESS_TAG) || logLine.startsWith(ERROR_KEYWORD) || isCommandLogLine(logLine);
            if (commandToken) {
                continue;
            }
            taskLog.message((LogLevel)i, processLogPrefix + logLine);
        }
    }
}

QString CmdlineTaskRunner::readStdout() {
    QByteArray charSet;
#ifdef Q_OS_WIN32
    charSet = "CP866";
#else
    charSet = "UTF-8";
#endif
    QTextCodec *codec = QTextCodec::codecForName(charSet);
    return codec->toUnicode(process->readAllStandardOutput());
}

void CmdlineTaskRunner::sl_onError(QProcess::ProcessError error) {
    QString msg;
    switch (error) {
    case QProcess::FailedToStart:
        msg = tr("The process '%1' failed to start. Either the invoked program is missing, "
            "or you may have insufficient permissions to invoke the program").arg(CMDLineRegistryUtils::getCmdlineUgenePath());
        break;
    case QProcess::Crashed:
        msg = tr("The process '%1' crashed some time after starting successfully").arg(CMDLineRegistryUtils::getCmdlineUgenePath());
        break;
    case QProcess::WriteError:
    case QProcess::ReadError:
        msg = tr("Error occurred while reading from or writing to channel");
        break;
    default:
        msg = tr("Unknown error occurred");
    }
    setError(msg);
}

void CmdlineTaskRunner::sl_onReadStandardOutput() {
    QString data = readStdout();
    QStringList lines = data.split(QChar('\n'));
    writeLog(lines);

    int errInd = data.indexOf(ERROR_KEYWORD);
    if (errInd >= 0) {
        int errIndEnd = data.indexOf(ERROR_KEYWORD, errInd + 1);
        if (errIndEnd > errInd) {
            setError(data.mid(errInd + ERROR_KEYWORD.size(), errIndEnd - errInd - ERROR_KEYWORD.size()));
        } else {
            setError(data.mid(errInd + ERROR_KEYWORD.size() + 1));
        }
        return;
    }

    foreach(const QString &line, lines) {
        QStringList words = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        foreach(const QString &word, words) {
            if (word.startsWith(OUTPUT_PROGRESS_TAG)) {
                QString numStr = word.mid(OUTPUT_PROGRESS_TAG.size());
                bool ok = false;
                int num = numStr.toInt(&ok);
                if (ok && num >= 0) {
                    stateInfo.progress = qMin(num, 100);
                }
                break;
            } else if (parseCommandLogWord(word)) {
                break;
            }
        }
    }
}

void CmdlineTaskRunner::sl_onFinish(int exitCode, QProcess::ExitStatus exitStatus) {
    CHECK(!hasError(), ); // !do not overwrite previous error!

    // On Windows, if the process was terminated with TerminateProcess() from another application,
    // this function will still return NormalExit unless the exit code is less than 0.
    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        setError(tr("An error occurred. Process is not finished successfully."));
    }
}

/************************************************************************/
/* CmdlineTask */
/************************************************************************/
namespace {
const int UPDATE_PROGRESS_INTERVAL = 500;

void logError(const QString &error) {
    coreLog.info(QString("%1%2%1").arg(ERROR_KEYWORD).arg(error));
}
}

CmdlineTask::CmdlineTask(const QString &name, TaskFlags flags)
    : Task(name, flags) {
    if (AppContext::getCMDLineRegistry()->hasParameter(OUTPUT_PROGRESS_ARG)) {
        QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), SLOT(sl_outputProgressAndState()));
        timer->start(UPDATE_PROGRESS_INTERVAL);
    }
}

Task::ReportResult CmdlineTask::report() {
    if (AppContext::getCMDLineRegistry()->hasParameter(OUTPUT_ERROR_ARG)) {
        QString error = getTaskError();
        if (!error.isEmpty()) {
            logError(error);
        }
    }
    if (AppContext::getCMDLineRegistry()->hasParameter(OUTPUT_PROGRESS_ARG)) {
        sl_outputProgressAndState();
    }
    return ReportResult_Finished;
}

QString CmdlineTask::getTaskError() const {
    return getError();
}

void CmdlineTask::sl_outputProgressAndState() {
    coreLog.info(QString("%1%2").arg(OUTPUT_PROGRESS_TAG).arg(getProgress()));
}
} // U2
