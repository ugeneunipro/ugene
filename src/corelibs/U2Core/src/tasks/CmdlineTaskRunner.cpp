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

#include <QCoreApplication>
#include <QMap>
#include <QRegularExpression>
#include <QSet>
#include <QTextCodec>
#include <QTimer>

#include <U2Core/AppContext.h>
#include <U2Core/CMDLineUtils.h>
#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>

#if (defined(Q_OS_WIN32) || defined(Q_OS_WINCE))
#    include <Windows.h>
#endif

#ifdef Q_OS_WIN
#    include <tlhelp32.h>
#else
#    include <unistd.h>
#endif

#include "CmdlineTaskRunner.h"

namespace U2 {

/************************************************************************/
/* CmdlineTaskRunner */
/************************************************************************/
namespace {
const QString OUTPUT_ERROR_ARG = "ugene-output-error";
const QString OUTPUT_PROGRESS_ARG = "ugene-output-progress-state";
const QString OUTPUT_PROGRESS_TAG = "task-progress=";
const QString ERROR_KEYWORD = "#%*ugene-finished-with-error#%*";

inline int getLogNameCandidate(const QString& line, QString& nameCandidate) {
    if ("" == line) {
        return -1;
    }

    if (!line.startsWith("[")) {
        return -1;
    }

    // maybe, @line is "[time][loglevel] log"
    int openPos = line.indexOf("[", 1);  // 1 because it is needed to skip first [time] substring
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
        case LogLevel_TRACE:
            return "TRACE";
        case LogLevel_DETAILS:
            return "DETAILS";
        case LogLevel_INFO:
            return "INFO";
        case LogLevel_ERROR:
            return "ERROR";
        default:
            assert(0);
    }
    return "";
}

bool containsPrefix(const QStringList& args, const QString& prefix) {
    foreach (const QString& arg, args) {
        if (arg.startsWith(prefix)) {
            return true;
        }
    }
    return false;
}
}  // namespace

#ifdef Q_OS_WIN
class WindowsJobProcess : public QProcess {
public:
    WindowsJobProcess(U2OpStatus& os, QObject* parent)
        : QProcess(parent) {
        const auto error = [] {
            const DWORD code = GetLastError();
            return CmdlineTaskRunner::tr("Cannot configure a Windows process job (error %1).").arg(code);
        };
        job = CreateJobObject(nullptr, nullptr);
        CHECK_EXT(job != nullptr, os.setError(error()), );
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits = {};
        limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
        CHECK_EXT(SetInformationJobObject(job, JobObjectExtendedLimitInformation, &limits, sizeof(limits)), os.setError(error()), );

        SIZE_T size = 0;
        InitializeProcThreadAttributeList(nullptr, 1, 0, &size);
        CHECK_EXT(size > 0, os.setError(error()), );
        attributeStorage.resize(size);
        auto attributes = reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(attributeStorage.data());
        CHECK_EXT(InitializeProcThreadAttributeList(attributes, 1, 0, &size), os.setError(error()), );
        startupInfo.lpAttributeList = attributes;
        CHECK_EXT(UpdateProcThreadAttribute(attributes, 0, PROC_THREAD_ATTRIBUTE_JOB_LIST, &job, sizeof(job), nullptr, nullptr), os.setError(error()), );
        setCreateProcessArgumentsModifier([this](CreateProcessArguments* args) {
            // Assign the job before the initial thread runs, preserving QProcess's standard handles.
            startupInfo.StartupInfo = *args->startupInfo;
            startupInfo.StartupInfo.cb = sizeof(startupInfo);
            args->startupInfo = &startupInfo.StartupInfo;
            args->flags |= EXTENDED_STARTUPINFO_PRESENT;
        });
    }

    ~WindowsJobProcess() override {
        if (job != nullptr) {
            CloseHandle(job);
            if (state() != NotRunning) {
                waitForFinished(5000);
            }
        }
        if (startupInfo.lpAttributeList != nullptr) {
            DeleteProcThreadAttributeList(startupInfo.lpAttributeList);
        }
    }

    int killProcessTree() {
        int result = TerminateJobObject(job, 1);
        if (result == 0) {
            const DWORD code = GetLastError();
            coreLog.error(CmdlineTaskRunner::tr("Cannot terminate a Windows process job (error %1).").arg(code));
        }
        return result;
    }

private:
    HANDLE job = nullptr;
    QByteArray attributeStorage;
    STARTUPINFOEXW startupInfo = {};
};

class WindowsProcessTree {
public:
    explicit WindowsProcessTree(qint64 rootPid) {
        CHECK(openProcess(rootPid).handle != nullptr, );
        FILETIME now;
        GetSystemTimeAsFileTime(&now);
        // This fallback is only for processes launched without a Job Object.
        snapshotTime = toUInt64(now);
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        CHECK(snapshot != INVALID_HANDLE_VALUE, );
        PROCESSENTRY32 entry = {};
        entry.dwSize = sizeof(entry);
        if (Process32First(snapshot, &entry)) {
            do {
                children[entry.th32ParentProcessID].append(entry.th32ProcessID);
            } while (Process32Next(snapshot, &entry));
        }
        CloseHandle(snapshot);
    }

    ~WindowsProcessTree() {
        for (const ProcessInfo& process : qAsConst(processes)) {
            CloseHandle(process.handle);
        }
    }

    QList<long> getDirectChildren(qint64 parentPid) {
        QList<long> result;
        const ProcessInfo parent = processes.value(parentPid);
        CHECK(parent.handle != nullptr, result);
        for (long childPid : children.value(parentPid)) {
            if (childPid <= 0 || childPid == QCoreApplication::applicationPid()) {
                continue;
            }
            const ProcessInfo child = openProcess(childPid);
            // A reused PID must not turn an older process or a post-snapshot process into a child.
            if (child.handle != nullptr && CmdlineTaskRunner::isProcessSnapshotChild(parent.created, child.created, snapshotTime)) {
                result.append(childPid);
            }
        }
        return result;
    }

    int killProcess(qint64 processId) const {
        CHECK(processId > 0 && processId != QCoreApplication::applicationPid(), 0);
        HANDLE handle = processes.value(processId).handle;
        return handle == nullptr ? 0 : TerminateProcess(handle, 1);
    }

private:
    Q_DISABLE_COPY(WindowsProcessTree)

    struct ProcessInfo {
        HANDLE handle = nullptr;
        quint64 created = 0;
    };

    static quint64 toUInt64(const FILETIME& time) {
        return (quint64(time.dwHighDateTime) << 32) | time.dwLowDateTime;
    }

    ProcessInfo openProcess(qint64 processId) {
        if (processes.contains(processId)) {
            return processes.value(processId);
        }
        HANDLE handle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_TERMINATE, FALSE, processId);
        CHECK(handle != nullptr, {});
        FILETIME created, exited, kernel, user;
        if (!GetProcessTimes(handle, &created, &exited, &kernel, &user)) {
            CloseHandle(handle);
            return {};
        }
        ProcessInfo process {handle, toUInt64(created)};
        // Retain the handle until termination so a later PID reuse cannot select a different process.
        processes.insert(processId, process);
        return process;
    }

    QMap<qint64, ProcessInfo> processes;
    QMap<qint64, QList<long>> children;
    quint64 snapshotTime = 0;
};
#endif

const QString CmdlineTaskRunner::REPORT_FILE_ARG = "ugene-write-task-report-to-file";

QProcess* CmdlineTaskRunner::createProcess(U2OpStatus& os, QObject* parent) {
    CHECK_OP(os, nullptr);
#ifdef Q_OS_WIN
    auto process = new WindowsJobProcess(os, parent);
    if (os.hasError()) {
        delete process;
        return nullptr;
    }
    return process;
#else
    return new QProcess(parent);
#endif
}

QList<long> CmdlineTaskRunner::getChildrenProcesses(qint64 processId, bool fullTree) {
#ifdef Q_OS_WIN
    WindowsProcessTree tree(processId);
    return getChildrenProcesses(processId, fullTree, [&](qint64 pid) { return tree.getDirectChildren(pid); });
#else
    return getChildrenProcesses(processId, fullTree, getDirectChildrenProcesses);
#endif
}

#ifndef Q_OS_WIN
QList<long> CmdlineTaskRunner::getDirectChildrenProcesses(qint64 processId) {
    QList<long> children;

#    if defined(Q_OS_LINUX) || defined(Q_OS_DARWIN)
    char* buff = nullptr;
    size_t len = 255;
    char command[256] = {0};

    sprintf(command, "ps -ef|awk '$3==%u {print $2}'", (unsigned)processId);
    FILE* fp = (FILE*)popen(command, "r");
    CHECK(fp != nullptr, children);
    while (getline(&buff, &len, fp) >= 0) {
        int child_process_id = QString(buff).toInt();
        if (child_process_id != 0) {
            children << child_process_id;
        }
    }
    free(buff);
    pclose(fp);
#    endif

    return children;
}
#endif

QList<long> CmdlineTaskRunner::getChildrenProcesses(qint64 processId, bool fullTree, const std::function<QList<long>(qint64)>& getDirectChildren) {
    QList<long> children;
    CHECK(processId > 0, children);
    QSet<qint64> visited {processId, QCoreApplication::applicationPid()};
    QList<qint64> pending {processId};
    for (int i = 0; i < pending.size(); i++) {
        for (long child : getDirectChildren(pending.at(i))) {
            if (child <= 0 || visited.contains(child)) {
                continue;
            }
            visited.insert(child);
            children.append(child);
            if (fullTree) {
                pending.append(child);
            }
        }
    }
    return children;
}

bool CmdlineTaskRunner::isProcessSnapshotChild(quint64 parentCreated, quint64 childCreated, quint64 snapshotTime) {
    return parentCreated <= childCreated && childCreated <= snapshotTime;
}

int CmdlineTaskRunner::killChildrenProcesses(qint64 processId, bool fullTree) {
    int result = 0;

#ifdef Q_OS_WIN
    WindowsProcessTree tree(processId);
    QList<long> children = getChildrenProcesses(processId, fullTree, [&](qint64 pid) { return tree.getDirectChildren(pid); });
#else
    QList<long> children = getChildrenProcesses(processId, fullTree);
#endif

    if (!children.isEmpty()) {
        uiLog.trace("kill all children of process: " + QString::number(processId));
    }
    while (!children.isEmpty()) {
        long child = children.takeLast();

        uiLog.trace("    kill process: " + QString::number(child));
#ifdef Q_OS_WIN
        result += tree.killProcess(child);
#else
        result += killProcess(child);
#endif

#if defined(Q_OS_LINUX) || defined(Q_OS_DARWIN)
        usleep(1000000);
#elif defined(Q_OS_WIN)
        Sleep(1000);
#endif
    }

    return result;
}

int CmdlineTaskRunner::killProcessTree(QProcess* process) {
    CHECK(process != nullptr, 0);
#ifdef Q_OS_WIN
    // The root may have exited already while its descendants still belong to the job.
    if (auto jobProcess = dynamic_cast<WindowsJobProcess*>(process)) {
        return jobProcess->killProcessTree();
    }
#endif
    qint64 processId = process->processId();
    return killProcessTree(processId);
}

int CmdlineTaskRunner::killProcessTree(qint64 processId) {
    CHECK(processId > 0 && processId != QCoreApplication::applicationPid(), 0);
#ifdef Q_OS_WIN
    HANDLE parent = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
    CHECK(parent != nullptr, 0);
#endif
    uiLog.trace("Killing full processes tree: " + QString::number(processId));
    uiLog.trace("Killing children processes: " + QString::number(processId));
    int result = killChildrenProcesses(processId);
    uiLog.trace("Killing parent process: " + QString::number(processId));
#ifdef Q_OS_WIN
    result += TerminateProcess(parent, 1);
    CloseHandle(parent);
#else
    result += killProcess(processId);
#endif
    return result;
}

int CmdlineTaskRunner::killProcess(qint64 processId) {
    CHECK(processId > 0 && processId != QCoreApplication::applicationPid(), 0);
    int result = 0;
#if defined(Q_OS_LINUX) || defined(Q_OS_DARWIN)
    int exists = QProcess::execute("kill", {"-0", QString::number(processId)});
    if (exists == 0) {
        result = QProcess::execute("kill", {"-9", QString::number(processId)});
    }
#elif defined(Q_OS_WIN)
    DWORD dwDesiredAccess = PROCESS_TERMINATE;
    BOOL bInheritHandle = FALSE;
    HANDLE hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, processId);
    if (hProcess != nullptr) {
        result = TerminateProcess(hProcess, 1);
        CloseHandle(hProcess);
    }
#endif
    return result;
}

CmdlineTaskRunner::CmdlineTaskRunner(const CmdlineTaskConfig& config)
    : Task(tr("Run UGENE command line: %1").arg(config.command), TaskFlag_NoRun), config(config), process(nullptr) {
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

    if (config.withPluginList) {
        args << QString("--%1=\"%2\"").arg(CMDLineRegistry::PLUGINS_ARG).arg(config.pluginList.join(";"));
    }

    if (!containsPrefix(args, "--log-level")) {
        QString logLevel = getLogLevelName(config.logLevel).toLower();
        args << ("--log-level-" + logLevel);
    }

    args << config.arguments;

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert(ENV_SEND_CRASH_REPORTS, "0");

    process = createProcess(stateInfo, this);
    CHECK_OP(stateInfo, );
    process->setProcessEnvironment(env);
    connect(process, SIGNAL(errorOccurred(QProcess::ProcessError)), SLOT(sl_onError(QProcess::ProcessError)));
    connect(process, SIGNAL(readyReadStandardOutput()), SLOT(sl_onReadStandardOutput()));
    connect(process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &CmdlineTaskRunner::sl_onFinish);

    QString cmdlineUgenePath(CMDLineRegistryUtils::getCmdlineUgenePath());
    coreLog.details("Starting UGENE command line: " + cmdlineUgenePath + " " + args.join(" "));
    process->start(cmdlineUgenePath, args);
    processLogPrefix = QString("process:%1>").arg(process->processId());
    bool startedSuccessfully = process->waitForStarted();
    CHECK_EXT(startedSuccessfully, setError(tr("Cannot start process '%1'").arg(cmdlineUgenePath)), );
}

Task::ReportResult CmdlineTaskRunner::report() {
    CHECK(process != nullptr, ReportResult_Finished);
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

QString CmdlineTaskRunner::getProcessErrorsLog() const {
    return processErrorLog;
}

bool CmdlineTaskRunner::isCommandLogLine(const QString& /*logLine*/) const {
    return false;
}

bool CmdlineTaskRunner::parseCommandLogWord(const QString& /*logWord*/) {
    return false;
}

void CmdlineTaskRunner::writeLog(QStringList& messages) {
    QStringList::Iterator it = messages.begin();
    for (; it != messages.end(); it++) {
        QString& line = *it;
        line = line.trimmed();
        QString nameCandidate;
        int closePos = getLogNameCandidate(line, nameCandidate);
        if (-1 == closePos) {
            continue;
        }

        QString logMessage = line.mid(closePos + 1);
        static const QString logLevelErrorName = getLogLevelName(LogLevel_ERROR);
        if (logLevelErrorName == nameCandidate) {
            processErrorLog += logMessage + "\n";
        }

        for (int i = config.logLevel; i < LogLevel_NumLevels; i++) {
            QString logLevelName = getLogLevelName((LogLevel)i);
            if (logLevelName != nameCandidate) {
                continue;
            }

            logMessage = logMessage.trimmed();
            bool commandToken = logMessage.startsWith(OUTPUT_PROGRESS_TAG) || logMessage.startsWith(ERROR_KEYWORD) || isCommandLogLine(logMessage);
            if (commandToken) {
                continue;
            }
            taskLog.message((LogLevel)i, processLogPrefix + logMessage);
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
    QTextCodec* codec = QTextCodec::codecForName(charSet);
    return codec->toUnicode(process->readAllStandardOutput());
}

void CmdlineTaskRunner::sl_onError(QProcess::ProcessError error) {
    QString msg;
    switch (error) {
        case QProcess::FailedToStart:
            msg = tr("The process '%1' failed to start. Either the invoked program is missing, "
                     "or you may have insufficient permissions to invoke the program")
                      .arg(CMDLineRegistryUtils::getCmdlineUgenePath());
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
    const QStringList lines = data.split(QChar('\n'));
    QStringList combineMultiLines;
    for (const QString& line : qAsConst(lines)) {
        if (line.startsWith("[") || combineMultiLines.isEmpty()) {
            combineMultiLines.append(line);
        } else {
            combineMultiLines.last() += "\n" + line;
        }
    }
    writeLog(combineMultiLines);

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

    for (const QString& line : qAsConst(lines)) {
        QStringList words = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        for (const QString& word : qAsConst(words)) {
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

void logError(const QString& error) {
    coreLog.info(QString("%1%2%1").arg(ERROR_KEYWORD).arg(error));
}
}  // namespace

CmdlineTask::CmdlineTask(const QString& name, TaskFlags flags)
    : Task(name, flags) {
    if (AppContext::getCMDLineRegistry()->hasParameter(OUTPUT_PROGRESS_ARG)) {
        auto timer = new QTimer(this);
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
}  // namespace U2
