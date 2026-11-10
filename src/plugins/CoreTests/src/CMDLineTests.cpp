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

#include "CMDLineTests.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDateTime>
#include <QDomElement>
#include <QFile>

#include <U2Core/AppContext.h>
#include <U2Core/CMDLineCoreOptions.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/Log.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>

#include <U2Lang/WorkflowSettings.h>
#include <U2Lang/WorkflowUtils.h>

#ifdef Q_OS_WIN
#    include <qt_windows.h>
#    include <psapi.h>
#    include <tlhelp32.h>
#endif

namespace U2 {

#define COMMON_DATA_DIR_ENV_ID "COMMON_DATA_DIR"
#define LOCAL_DATA_DIR_ENV_ID "LOCAL_DATA_DIR"
#define SAMPLE_DATA_DIR_ENV_ID "SAMPLE_DATA_DIR"
#define WORKFLOW_SAMPLES_ENV_ID "WORKFLOW_SAMPLES_DIR"
#define WORKFLOW_OUTPUT_ENV_ID "WORKFLOW_OUTPUT_DIR"
#define TEMP_DATA_DIR_ENV_ID "TEMP_DATA_DIR"
#define CONFIG_FILE_ENV_ID "CONFIG_FILE"
#define CONFIG_PROTOTYPE "PROTOTYPE"
#define WORKING_DIR_ATTR "working-dir"

/************************
 * GTest_RunCMDLine
 ************************/
void GTest_RunCMDLine::init(XMLTestFormat*, const QDomElement& el) {
    setUgeneclPath();
    setArgs(el);
    proc = new QProcess(this);
    if (el.hasAttribute(WORKING_DIR_ATTR)) {
        workingDir = el.attribute(WORKING_DIR_ATTR);
        XMLTestUtils::replacePrefix(env, workingDir);
        if (QUrl(workingDir).isRelative()) {
            workingDir = env->getVar(TEMP_DATA_DIR_ENV_ID) + "/" + workingDir;
        }
    }

    autoRemoveWorkingDir = false;
    if (workingDir.isEmpty()) {
        workingDir = GUrlUtils::rollFileName(env->getVar(TEMP_DATA_DIR_ENV_ID) + "/workingDir", "_");
        autoRemoveWorkingDir = true;
        taskLog.trace(QString("Working dir is not defined, the following dir will be used as working: %1").arg(workingDir));
    }

    QDir userScriptsDir(WorkflowSettings::getUserDirectory());
    QString protoPath = env->getVar(COMMON_DATA_DIR_ENV_ID) + "/" + env->getVar(CONFIG_PROTOTYPE);
    QDir protoDir(protoPath);
    protoDir.setNameFilters({"*.usa"});
    QFileInfoList protoScriptFiles = protoDir.entryInfoList();
    for (const QFileInfo& protoScriptFile : qAsConst(protoScriptFiles)) {
        QFileInfo targetScriptFile(userScriptsDir.path() + "/" + protoScriptFile.fileName());
        if (!targetScriptFile.exists() || protoScriptFile.size() != targetScriptFile.size()) {
            QFile::copy(protoScriptFile.absoluteFilePath(), targetScriptFile.absoluteFilePath());
        }
    }
}

void GTest_RunCMDLine::setArgs(const QDomElement& el) {
    args.append("--log-level-details");
    args.append("--lang=en");
    args.append("--log-no-task-progress");
    args.append("--" + CMDLineCoreOptions::INI_FILE + "=" + AppContext::getSettings()->fileName());

    QStringList argsFromXml;
    QDomNamedNodeMap attributeMap = el.attributes();
    int attributeCount = attributeMap.length();
    for (int i = 0; i < attributeCount; ++i) {
        QDomNode node = attributeMap.item(i);
        QString nodeName = node.nodeName();
        if (nodeName == "message") {
            expectedMessage = node.nodeValue();
            continue;
        }
        if (nodeName == "message2") {
            expectedMessage2 = node.nodeValue();
            continue;
        }
        if (nodeName == "nomessage") {
            unexpectedMessage = node.nodeValue();
            continue;
        }
        if (nodeName == WORKING_DIR_ATTR) {
            continue;
        }
        QString argument = "--" + nodeName + "=" + getVal(node.nodeValue());
        if (argument.startsWith("--task")) {
            argsFromXml.prepend(argument);  // '--task' attribute must go first.
        } else {
            argsFromXml.append(argument);
        }
    }
    args << argsFromXml;
    cmdLog.info(args.join(" "));
}

QString GTest_RunCMDLine::splitVal(const QString& val, const QString& prefValue, const QString& prefix, bool isTmp) {
    int midSize = prefValue.size();
    QStringList dsVals = WorkflowUtils::unpackListOfDatasets(val);
    QStringList result;
    foreach (const QString& dsVal, dsVals) {
        QStringList realVals = WorkflowUtils::unpackListOfUrls(dsVal);
        QStringList dsResult;
        for (QString s : qAsConst(realVals)) {
            if (s.startsWith(prefValue)) {
                s = s.mid(midSize);
            }
            QString filename = prefix + s;
            dsResult << filename;
            if (isTmp) {
                tmpFiles << filename;
            }
        }
        result << WorkflowUtils::packListOfUrls(dsResult);
    }
    return WorkflowUtils::packListOfDatasets(result);
}

QString GTest_RunCMDLine::getVal(const QString& val) {
    if (val.isEmpty()) {
        return val;
    }
    if (val.startsWith(XMLTestUtils::COMMON_DATA_DIR_PREFIX)) {
        return splitVal(val, XMLTestUtils::COMMON_DATA_DIR_PREFIX, env->getVar(COMMON_DATA_DIR_ENV_ID) + "/", false);
    }
    if (val.startsWith(XMLTestUtils::TMP_DATA_DIR_PREFIX)) {
        return splitVal(val, XMLTestUtils::TMP_DATA_DIR_PREFIX, env->getVar(TEMP_DATA_DIR_ENV_ID) + "/", true);
    }
    if (val == XMLTestUtils::CONFIG_FILE_PATH) {
        return env->getVar(COMMON_DATA_DIR_ENV_ID) + "/" + env->getVar(CONFIG_FILE_ENV_ID);
    }
    if (val.startsWith(XMLTestUtils::LOCAL_DATA_DIR_PREFIX)) {
        return splitVal(val, XMLTestUtils::LOCAL_DATA_DIR_PREFIX, env->getVar(LOCAL_DATA_DIR_ENV_ID), false);
    }
    if (val.startsWith(XMLTestUtils::SAMPLE_DATA_DIR_PREFIX)) {
        return splitVal(val, XMLTestUtils::SAMPLE_DATA_DIR_PREFIX, env->getVar(SAMPLE_DATA_DIR_ENV_ID), false);
    }
    if (val.startsWith(XMLTestUtils::WORKFLOW_SAMPLES_DIR_PREFIX)) {
        return splitVal(val, XMLTestUtils::WORKFLOW_SAMPLES_DIR_PREFIX, env->getVar(WORKFLOW_SAMPLES_ENV_ID), false);
    }
    if (val.startsWith(XMLTestUtils::WORKFLOW_OUTPUT_DIR_PREFIX)) {
        return splitVal(val, XMLTestUtils::WORKFLOW_OUTPUT_DIR_PREFIX, env->getVar(WORKFLOW_OUTPUT_ENV_ID) + "/", false);
    }
    return val;
}

void GTest_RunCMDLine::setUgeneclPath() {
    ugeneclPath = AppContext::getMainWindow() != nullptr ? QApplication::applicationDirPath() : QCoreApplication::applicationDirPath();
    SAFE_POINT(!ugeneclPath.isEmpty(), "FAILED to get applicationDirPath", );
#ifndef _DEBUG
    QString ugeneclName = "ugenecl";
#else
    QString ugeneclName = "ugenecld";
#endif
    ugeneclPath += "/" + ugeneclName;
}

#ifdef Q_OS_WIN
static void saveProcessSnapshot(qint64 processId, const QString& logPath);
#endif

void GTest_RunCMDLine::prepare() {
    if (!workingDir.isEmpty()) {
        QDir().mkpath(workingDir);
        proc->setWorkingDirectory(workingDir);
        args.prepend(QString("--%1=true").arg(CMDLineCoreOptions::XML_CUSTOM_WORKING_DIR));
    }

    // Create separate log file for subprocess to avoid TEST_LOG_LISTENER resource deadlock
    // when parent test needs to check log messages and subprocess also tries to write to log
    QString tempDir = env->getVar(TEMP_DATA_DIR_ENV_ID);
    separateLogFile = GUrlUtils::rollFileName(tempDir + "/cmdline_subprocess_log.txt", "_");
    tmpFiles << separateLogFile;  // Will be auto-removed in cleanup
    args.prepend("--log-file=" + separateLogFile);

#ifdef Q_OS_WIN
    tmpFiles << separateLogFile + ".memory.txt";
    saveProcessSnapshot(0, separateLogFile);
#endif
    processStartTime = GTimer::currentTimeMicros();
    lastMemorySampleTime = processStartTime;
    proc->start(ugeneclPath, args);
    proc->waitForStarted(5000);
}

static const QString ERROR_LABEL_TRY1 = "finished with error";
static QString getErrorMsg(const QString& str) {
    int ind = str.indexOf(ERROR_LABEL_TRY1);
    if (ind != -1) {
        return str.mid(ind + ERROR_LABEL_TRY1.size());
    }
    return QString();
}

#ifdef Q_OS_WIN
static void saveProcessSnapshot(qint64 processId, const QString& logPath) {
    QString summary = QString("Process memory snapshot %1: runner PID=%2, child PID=%3, log=%4\n")
                          .arg(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs))
                          .arg(GetCurrentProcessId())
                          .arg(processId)
                          .arg(logPath);
    PERFORMANCE_INFORMATION system = {};
    system.cb = sizeof(system);
    if (GetPerformanceInfo(&system, sizeof(system))) {
        summary += QString("System bytes: physical_total=%1 physical_available=%2 commit_total=%3 commit_limit=%4\n")
                       .arg(quint64(system.PhysicalTotal) * system.PageSize)
                       .arg(quint64(system.PhysicalAvailable) * system.PageSize)
                       .arg(quint64(system.CommitTotal) * system.PageSize)
                       .arg(quint64(system.CommitLimit) * system.PageSize);
    } else {
        summary += QString("GetPerformanceInfo failed: %1\n").arg(GetLastError());
    }
    summary += "PID\tPPID\tName\tWorkingSetBytes\tPrivateBytes\tPeakWorkingSetBytes\tPageFaultCount\tKernelTime100ns\tUserTime100ns\tCreationTime100ns\n";
    QString snapshot = summary;
    HANDLE processes = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (processes != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W entry = {};
        entry.dwSize = sizeof(entry);
        BOOL found = Process32FirstW(processes, &entry);
        while (found) {
            QString name = QString::fromWCharArray(entry.szExeFile);
            QString row = QString("%1\t%2\t%3\t").arg(entry.th32ProcessID).arg(entry.th32ParentProcessID).arg(name);
            HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, entry.th32ProcessID);
            PROCESS_MEMORY_COUNTERS_EX memory = {};
            if (process != nullptr && GetProcessMemoryInfo(process, reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&memory), sizeof(memory))) {
                row += QString("%1\t%2\t%3\t%4\t").arg(memory.WorkingSetSize).arg(memory.PrivateUsage).arg(memory.PeakWorkingSetSize).arg(memory.PageFaultCount);
                FILETIME created, exited, kernel, user;
                if (GetProcessTimes(process, &created, &exited, &kernel, &user)) {
                    auto ticks = [](const FILETIME& time) { return (quint64(time.dwHighDateTime) << 32) | time.dwLowDateTime; };
                    row += QString("%1\t%2\t%3").arg(ticks(kernel)).arg(ticks(user)).arg(ticks(created));
                } else {
                    row += QString("GetProcessTimes error=%1").arg(GetLastError());
                }
            } else {
                row += QString("Memory unavailable, Win32 error=%1").arg(GetLastError());
            }
            if (process != nullptr) {
                CloseHandle(process);
            }
            snapshot += row + '\n';
            // Keep successful-run logs small, but retain Java measurements for SnpEff comparisons.
            if (entry.th32ProcessID == GetCurrentProcessId() || (processId != 0 && (entry.th32ProcessID == processId || entry.th32ParentProcessID == processId)) || name.compare("java.exe", Qt::CaseInsensitive) == 0) {
                summary += row + '\n';
            }
            found = Process32NextW(processes, &entry);
        }
        if (GetLastError() != ERROR_NO_MORE_FILES) {
            QString error = QString("Process enumeration failed: %1\n").arg(GetLastError());
            summary += error;
            snapshot += error;
        }
        CloseHandle(processes);
    } else {
        QString error = QString("CreateToolhelp32Snapshot failed: %1\n").arg(GetLastError());
        summary += error;
        snapshot += error;
    }
    QFile file(logPath + ".memory.txt");
    QByteArray bytes = snapshot.toUtf8();
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append) || file.write(bytes) != bytes.size() || !file.flush()) {
        cmdLog.error(QString("Cannot save process snapshot: %1: %2").arg(file.fileName(), file.errorString()));
    }
    cmdLog.details(summary);
}

static void saveTimeoutDiagnostics(qint64 processId, const QString& logPath) {
    const QString diagnosticsPath = logPath + ".timeout.txt";
    const QString dumpPath = logPath + ".timeout.dmp";
    cmdLog.error(QString("Collecting minidump for PID %1: %2").arg(processId).arg(dumpPath));
    QProcess collector;
    collector.setProcessChannelMode(QProcess::MergedChannels);
    collector.setStandardOutputFile(diagnosticsPath);
    collector.start(QCoreApplication::applicationDirPath() + "/plugins/CoreTestsDump.exe", {QString::number(processId), dumpPath});
    if (!collector.waitForStarted(5000)) {
        cmdLog.error(QString("Cannot start timeout diagnostics: %1").arg(collector.errorString()));
    } else if (!collector.waitForFinished(60000)) {
        collector.kill();
        if (!collector.waitForFinished(5000)) {
            cmdLog.error("Timeout dump collector did not stop within 5 seconds after kill");
        }
        cmdLog.error("Timeout diagnostics exceeded 60 seconds; the dump may be incomplete");
    } else if (collector.exitStatus() != QProcess::NormalExit || collector.exitCode() != 0) {
        cmdLog.error(QString("Timeout diagnostics failed, exit code: %1").arg(collector.exitCode()));
    }
    QFile diagnostics(diagnosticsPath);
    if (diagnostics.open(QIODevice::ReadOnly)) {
        cmdLog.error(QString("Timeout diagnostics (%1):\n%2").arg(diagnosticsPath, QString::fromUtf8(diagnostics.readAll())));
    } else {
        cmdLog.error(QString("Cannot read timeout diagnostics: %1: %2").arg(diagnosticsPath, diagnostics.errorString()));
    }
}
#endif

Task::ReportResult GTest_RunCMDLine::report() {
    if (hasError() || isCanceled()) {
        return ReportResult_Finished;
    }

    // Check subprocess timeout
    if (proc->state() != QProcess::NotRunning) {
        qint64 elapsedSeconds = GTimer::secsBetween(processStartTime, GTimer::currentTimeMicros());
#ifdef Q_OS_WIN
        if (GTimer::secsBetween(lastMemorySampleTime, GTimer::currentTimeMicros()) >= 60 || elapsedSeconds > SUBPROCESS_TIMEOUT_SECONDS) {
            saveProcessSnapshot(proc->processId(), separateLogFile);
            lastMemorySampleTime = GTimer::currentTimeMicros();
        }
#endif
        if (elapsedSeconds > SUBPROCESS_TIMEOUT_SECONDS) {
#ifdef Q_OS_WIN
            saveTimeoutDiagnostics(proc->processId(), separateLogFile);
#endif
            proc->kill();
            proc->waitForFinished(5000);  // Wait up to 5 seconds for process to die
            stateInfo.setError(QString("Subprocess timeout: process did not finish in %1 seconds").arg(SUBPROCESS_TIMEOUT_SECONDS));
        } else {
            return ReportResult_CallMeAgain;
        }
    }

    // Read output from separate log file instead of stdout to avoid TEST_LOG_LISTENER deadlock
    QString output;
    if (!separateLogFile.isEmpty() && QFile::exists(separateLogFile)) {
        QFile logFile(separateLogFile);
        if (logFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            output = QString::fromUtf8(logFile.readAll());
            logFile.close();
        }
    }

    // Also read stdout as fallback (for compatibility with old behavior)
    QString stdOutput(proc->readAllStandardOutput());
    if (!stdOutput.isEmpty()) {
        if (!output.isEmpty()) {
            output += "\n=== STDOUT ===\n";
        }
        output += stdOutput;
    }

    if (hasError()) {
        cmdLog.error(QString("Subprocess output at timeout (log: %1):\n%2\n=== STDERR ===\n%3")
                         .arg(separateLogFile, output, QString::fromLocal8Bit(proc->readAllStandardError())));
        return ReportResult_Finished;
    }

    cmdLog.trace(output);

    if (!expectedMessage.isEmpty()) {
        cmdLog.error(output);
        if (!output.contains(expectedMessage, Qt::CaseSensitive)) {
            stateInfo.setError(QString("Expected message not found in output"));
        }
    }
    if (!expectedMessage2.isEmpty()) {
        cmdLog.error(output);
        if (!output.contains(expectedMessage2, Qt::CaseSensitive)) {
            stateInfo.setError(QString("Expected message not found in output"));
        }
    }
    if (!unexpectedMessage.isEmpty()) {
        if (output.contains(unexpectedMessage, Qt::CaseSensitive)) {
            stateInfo.setError(QString("Unexpected message is found in output"));
        }
    }
    if (!expectedMessage.isEmpty() || !unexpectedMessage.isEmpty()) {
        return ReportResult_Finished;
    }

    QString err = getErrorMsg(output);
    if (!err.isEmpty()) {
        int eofIdx = err.indexOf("\n");
        if (eofIdx > 0) {
            err = err.left(eofIdx - 1);
        }
        setError("Process finished with error" + err);
    }

    if (proc->exitStatus() == QProcess::CrashExit) {
        setError("Process is crashed!");
    }

    return ReportResult_Finished;
}

void GTest_RunCMDLine::cleanup() {
    if (!XMLTestUtils::parentTasksHaveError(this)) {
        foreach (const QString& file, tmpFiles) {
            taskLog.trace(QString("Temporary file removed: %1").arg(file));
            QFile::remove(file);
        }

        if (autoRemoveWorkingDir) {
            taskLog.trace(QString("Temporary working dir auto-removed: %1").arg(workingDir));
            QDir(workingDir).removeRecursively();
        }
    }

    XmlTest::cleanup();
}

/************************
 * GTest_RunCMDLine
 ************************/
QList<XMLTestFactory*> CMDLineTests::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(GTest_RunCMDLine::createFactory());
    return res;
}

}  // namespace U2
