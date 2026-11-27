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

#include "CmdlineTaskRunnerUnitTests.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QMap>
#include <QTemporaryDir>

#include <U2Core/CmdlineTaskRunner.h>
#include <U2Core/U2OpStatusUtils.h>

#ifdef Q_OS_WIN
#    include <Windows.h>
#endif

namespace U2 {

IMPLEMENT_TEST(CmdlineTaskRunnerUnitTests, processTree) {
    const qint64 ownPid = QCoreApplication::applicationPid();
    const qint64 rootPid = ownPid + 1;
    const long childPid = ownPid + 2;
    const long grandchildPid = ownPid + 3;
    const long siblingPid = ownPid + 4;
    QMap<qint64, QList<long>> children;
    QMap<qint64, int> visits;
    bool repeatedVisit = false;
    auto getDirectChildren = [&](qint64 pid) {
        if (++visits[pid] > 1) {
            // Bound the broken recursive implementation so a regression fails without crashing the runner.
            repeatedVisit = true;
            return QList<long>();
        }
        return children.value(pid);
    };

    children = {{rootPid, {childPid, siblingPid}}, {childPid, {grandchildPid}}};
    QList<long> result = CmdlineTaskRunner::getChildrenProcesses(rootPid, true, getDirectChildren);
    CHECK_TRUE((result == QList<long> {childPid, siblingPid, grandchildPid}), "Missing descendants or incorrect termination order");

    visits.clear();
    children = {{rootPid, {childPid}}, {childPid, {long(rootPid), grandchildPid}}, {grandchildPid, {childPid}}};
    result = CmdlineTaskRunner::getChildrenProcesses(rootPid, true, getDirectChildren);
    CHECK_FALSE(repeatedVisit, "PID cycle caused repeated traversal");
    CHECK_TRUE((result == QList<long> {childPid, grandchildPid}), "Cycle included the root or duplicate PIDs");

    visits.clear();
    // Reproduce the dump's external process -> UGENE -> external process cycle, plus an unrelated UGENE child.
    children = {{rootPid, {long(ownPid), childPid}}, {ownPid, {long(rootPid), siblingPid}}, {childPid, {grandchildPid}}};
    result = CmdlineTaskRunner::getChildrenProcesses(rootPid, true, getDirectChildren);
    CHECK_TRUE((result == QList<long> {childPid, grandchildPid}), "UGENE or its unrelated children were included");
    CHECK_FALSE(visits.contains(ownPid), "Traversed UGENE's children through a stale parent PID");

    visits.clear();
    result = CmdlineTaskRunner::getChildrenProcesses(rootPid, false, getDirectChildren);
    CHECK_TRUE((result == QList<long> {childPid}), "Direct children mode included UGENE or grandchildren");
    CHECK_EQUAL(1, visits.size(), "number of enumerated parents in direct children mode");

    visits.clear();
    children = {{rootPid, {long(rootPid), childPid, childPid, 0, -1}}};
    result = CmdlineTaskRunner::getChildrenProcesses(rootPid, true, getDirectChildren);
    CHECK_TRUE((result == QList<long> {childPid}), "Self-loop, duplicate or invalid PID was included");
    CHECK_FALSE(repeatedVisit, "Repeated a PID while traversing a self-loop");

    visits.clear();
    CHECK_TRUE(CmdlineTaskRunner::getChildrenProcesses(0, true, getDirectChildren).isEmpty(), "Traversed PID zero");
    CHECK_TRUE(CmdlineTaskRunner::getChildrenProcesses(-1, true, getDirectChildren).isEmpty(), "Traversed a negative PID");
    CHECK_TRUE(visits.isEmpty(), "Enumerated children for an invalid root");

    children.clear();
    const int depth = 10000;
    for (int i = 0; i < depth; i++) {
        children[rootPid + i] = {long(rootPid + i + 1)};
    }
    result = CmdlineTaskRunner::getChildrenProcesses(rootPid, true, getDirectChildren);
    CHECK_EQUAL(depth, result.size(), "deep tree size");
    CHECK_EQUAL(rootPid + depth, qint64(result.last()), "deepest descendant");
    CHECK_FALSE(repeatedVisit, "Visited a process more than once");

    CHECK_TRUE(CmdlineTaskRunner::isProcessSnapshotChild(100, 200, 300), "Rejected a child present in the snapshot");
    CHECK_FALSE(CmdlineTaskRunner::isProcessSnapshotChild(200, 100, 300), "Accepted a stale parent PID reused by a newer process");
    CHECK_FALSE(CmdlineTaskRunner::isProcessSnapshotChild(100, 400, 300), "Accepted a PID reused after the snapshot");
}

IMPLEMENT_TEST(CmdlineTaskRunnerUnitTests, protectOwnProcess) {
    QProcess child;
#ifdef Q_OS_WIN
    child.start("more.com");
#else
    child.start("cat");
#endif
    CHECK_TRUE(child.waitForStarted(), "Failed to start the controlled child process");
    CHECK_FALSE(child.waitForFinished(100), "Controlled child exited before the test");
    const qint64 childPid = child.processId();
    const qint64 ownPid = QCoreApplication::applicationPid();
    CHECK_TRUE(CmdlineTaskRunner::getChildrenProcesses(ownPid, false).contains(childPid), "Did not find the controlled child in the OS snapshot");
    CHECK_EQUAL(0, CmdlineTaskRunner::killProcess(ownPid), "attempt to terminate UGENE");
    CHECK_EQUAL(0, CmdlineTaskRunner::killProcessTree(ownPid), "attempt to terminate UGENE's process tree");
    CHECK_FALSE(child.waitForFinished(100), "Self-protection terminated UGENE's child process");
    QProcess notRunning;
    CHECK_EQUAL(0, CmdlineTaskRunner::killProcessTree(&notRunning), "attempt to terminate an unstarted process tree");
    CmdlineTaskRunner::killChildrenProcesses(ownPid);
    CHECK_TRUE(child.waitForFinished(5000), "Failed to terminate the controlled child through its retained handle");

    child.start();
    CHECK_TRUE(child.waitForStarted(), "Failed to restart the controlled child process");
    CmdlineTaskRunner::killProcessTree(&child);
    CHECK_TRUE(child.waitForFinished(5000), "Failed to terminate the controlled process tree");
}

IMPLEMENT_TEST(CmdlineTaskRunnerUnitTests, jobProcessTree) {
#ifdef Q_OS_WIN
    struct ChildHandle {
        HANDLE handle;
        ~ChildHandle() {
            if (handle != nullptr) {
                TerminateProcess(handle, 1);
                WaitForSingleObject(handle, 5000);
                CloseHandle(handle);
            }
        }
    };
    QTemporaryDir dir;
    CHECK_TRUE(dir.isValid(), "Failed to create the job test directory");
    QFile script(dir.filePath("spawn-child.ps1"));
    CHECK_TRUE(script.open(QIODevice::WriteOnly), "Failed to create the child launcher script");
    const QByteArray scriptText = R"(
[Console]::Out.WriteLine('ready')
while (!(Test-Path (Join-Path $PSScriptRoot 'spawn'))) { Start-Sleep -Milliseconds 20 }
$info = New-Object System.Diagnostics.ProcessStartInfo
$info.FileName = Join-Path $PSHOME 'powershell.exe'
$info.Arguments = '-NoProfile -NonInteractive -Command "Start-Sleep -Seconds 60"'
$info.UseShellExecute = $false
$info.CreateNoWindow = $true
$child = [System.Diagnostics.Process]::Start($info)
[Console]::Out.WriteLine($child.Id)
[Console]::Out.Flush()
$child.Dispose()
[Environment]::Exit(0)
)";
    CHECK_EQUAL(scriptText.size(), script.write(scriptText), "launcher script size");
    script.close();
    const QString powershell = QDir::fromNativeSeparators(qEnvironmentVariable("SystemRoot")) + "/System32/WindowsPowerShell/v1.0/powershell.exe";

    for (bool destroyProcess : {false, true}) {
        QFile::remove(dir.filePath("spawn"));
        U2OpStatusImpl os;
        QScopedPointer<QProcess> process(CmdlineTaskRunner::createProcess(os));
        CHECK_NO_ERROR(os);
        CHECK_TRUE(process != nullptr, "Failed to create the managed process");
        process->start(dir.filePath("missing-tool.exe"));
        CHECK_FALSE(process->waitForStarted(), "Unexpectedly started a missing executable");
        process->start(powershell, {"-NoProfile", "-NonInteractive", "-ExecutionPolicy", "Bypass", "-File", script.fileName()});
        CHECK_TRUE(process->waitForStarted(), process->errorString());
        while (!process->canReadLine() && process->waitForReadyRead(10000)) {
        }
        CHECK_TRUE(process->readLine().trimmed() == "ready", "Launcher did not reach the synchronization point");
        const qint64 parentPid = process->processId();
        ChildHandle parent {OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, FALSE, parentPid)};
        CHECK_TRUE(parent.handle != nullptr, "Could not open the launcher process");
        const QList<long> snapshot = CmdlineTaskRunner::getChildrenProcesses(parentPid);
        QFile gate(dir.filePath("spawn"));
        CHECK_TRUE(gate.open(QIODevice::WriteOnly), "Failed to signal the launcher");
        gate.close();
        CHECK_TRUE(process->waitForFinished(10000), QString("Launcher failed to exit: native state %1; stdout %2; stderr %3").arg(WaitForSingleObject(parent.handle, 0)).arg(QString::fromLocal8Bit(process->readAllStandardOutput())).arg(QString::fromLocal8Bit(process->readAllStandardError())));
        CHECK_EQUAL(0, process->exitCode(), "launcher exit code");
        bool parsed = false;
        const long childPid = process->readAllStandardOutput().trimmed().toLong(&parsed);
        CHECK_TRUE(parsed && childPid > 0 && childPid != QCoreApplication::applicationPid(), "Invalid child PID");
        ChildHandle child {OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, FALSE, childPid)};
        CHECK_TRUE(child.handle != nullptr, "Could not open the late child process");
        CHECK_FALSE(snapshot.contains(childPid), "Child already existed when the snapshot was taken");
        CHECK_EQUAL(DWORD(WAIT_TIMEOUT), WaitForSingleObject(child.handle, 0), "child state before cancellation");
        CHECK_EQUAL(qint64(0), process->processId(), "exited launcher PID");

        if (destroyProcess) {
            process.reset();
        } else {
            CmdlineTaskRunner::killProcessTree(process.data());
        }
        CHECK_EQUAL(DWORD(WAIT_OBJECT_0), WaitForSingleObject(child.handle, 5000), "late child state after cancellation or destruction");
    }
#endif
}

}  // namespace U2
