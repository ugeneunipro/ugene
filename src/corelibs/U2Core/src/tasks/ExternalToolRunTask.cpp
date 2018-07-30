/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include "ExternalToolRunTask.h"

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/UserApplicationsSettings.h>
#include <U2Core/ScriptingToolRegistry.h>
#include <U2Core/U2SafePoints.h>

#include <QDir>
#include <QCoreApplication>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#ifdef Q_OS_UNIX
#include <signal.h>
#include <unistd.h>
#endif


namespace U2 {

#define WIN_LAUNCH_CMD_COMMAND "cmd /C "
#define START_WAIT_MSEC 3000

ExternalToolRunTask::ExternalToolRunTask(const QString &_toolName, const QStringList &_arguments,
ExternalToolLogParser *_logParser, const QString &_workingDirectory, const QStringList &_additionalPaths,
const QString &_additionalProcessToKill, bool parseOutputFile)
: Task(_toolName + tr(" tool"), TaskFlag_None),
  arguments(_arguments),
  logParser(_logParser),
  toolName(_toolName),
  workingDirectory(_workingDirectory),
  additionalPaths(_additionalPaths),
  externalToolProcess(NULL),
  helper(NULL),
  listener(NULL),
  additionalProcessToKill(_additionalProcessToKill),
  parseOutputFile(parseOutputFile)
{
    coreLog.trace("Creating run task for: " + toolName);
    if (NULL != logParser) {
        logParser->setParent(this);
    }
}

ExternalToolRunTask::~ExternalToolRunTask(){
    delete externalToolProcess;
}

void ExternalToolRunTask::run(){
    if (hasError() || isCanceled()) {
        return;
    }

    ProcessRun pRun = ExternalToolSupportUtils::prepareProcess(toolName, arguments, workingDirectory, additionalPaths, stateInfo, listener);
    CHECK_OP(stateInfo, );
    externalToolProcess = pRun.process;

    if (!inputFile.isEmpty()) {
        externalToolProcess->setStandardInputFile(inputFile);
    }
    if (!outputFile.isEmpty()) {
        externalToolProcess->setStandardOutputFile(outputFile);
    }
    if (!additionalEnvVariables.isEmpty()) {
        QProcessEnvironment processEnvironment = externalToolProcess->processEnvironment();
        foreach (const QString& envVarName, additionalEnvVariables.keys()) {
            processEnvironment.insert(envVarName, additionalEnvVariables.value(envVarName));
        }
        externalToolProcess->setProcessEnvironment(processEnvironment);
    }

    helper.reset(new ExternalToolRunTaskHelper(this));
    if(NULL != listener) {
        helper->addOutputListener(listener);
    }

    externalToolProcess->start(pRun.program, pRun.arguments);
    bool started = externalToolProcess->waitForStarted(START_WAIT_MSEC);

    if (!started){
        ExternalTool* tool = AppContext::getExternalToolRegistry()->getByName(toolName);
        if (tool->isValid()){
            stateInfo.setError(tr("Can not run %1 tool.").arg(toolName));
        } else {
            stateInfo.setError(tr("Can not run %1 tool. May be tool path '%2' not valid?")
                               .arg(toolName)
                               .arg(AppContext::getExternalToolRegistry()->getByName(toolName)->getPath()));
        }
        return;
    }
    while(!externalToolProcess->waitForFinished(1000)){
        if (isCanceled()) {
            killProcess(externalToolProcess, additionalProcessToKill);
            algoLog.details(tr("Tool %1 is cancelled").arg(toolName));
            return;
        }
    }

    {
        int exitCode = externalToolProcess->exitCode();
        if(exitCode != EXIT_SUCCESS && !hasError()) {
            QString error;
            if (parseOutputFile) {
                parseStandartOutputFile(outputFile);
                error = logParser->getLastError();
            }
            setError(error.isEmpty() ? tr("%1 tool exited with code %2").arg(toolName).arg(exitCode) : error);
        } else {
            algoLog.details(tr("Tool %1 finished successfully").arg(toolName));
        }
    }
}

void ExternalToolRunTask::killProcess(QProcess *process, QString childProcesses) {
    Q_UNUSED(childProcesses)

#if (!defined(Q_OS_WIN32) && !defined(Q_OS_WINCE)) || defined(qdoc)
    long numPid = process->pid();
    Q_UNUSED(numPid);
#else
    Q_PID pid = process->pid();
    long numPid = pid->dwProcessId;
    Q_UNUSED(numPid);
#endif
#ifdef Q_OS_WIN
    QProcess::execute(QString("taskkill /PID %1 /T /F").arg(numPid));
    if (!childProcesses.isEmpty()) {
        QProcess::execute(QString("taskkill /IM %1 /T /F").arg(childProcesses));
    }
#endif
#ifdef Q_OS_UNIX
    QList<long> pids = getChildPidsRecursive(numPid);
    pids << numPid;
    foreach (const long pid, pids) {
        taskLog.trace(QString("Kill process: %1").arg(pid));
        kill(pid, SIGTERM);
    }
#endif
}

QList<long> ExternalToolRunTask::getChildPidsRecursive(long parentPid) {
    QList<long> res;

    QProcess p;
    p.start("ps", QStringList() << "-axo pid,ppid");
    p.waitForFinished();
    const QStringList lines = QString(p.readAllStandardOutput()).split('\n');
    p.close();

    foreach (const QString& line, lines) {
        CHECK_OPERATION(line.contains(QString::number(parentPid)), continue);

        const QStringList pidStrings = line.split(QRegExp("\\s"), QString::SkipEmptyParts);
        CHECK_OPERATION(2 == pidStrings.size(), continue);
        CHECK_OPERATION(pidStrings.last() == QString::number(parentPid), continue);

        bool ok = false;
        long pid = pidStrings.first().toLong(&ok);
        CHECK_OPERATIONS(ok,
                         taskLog.trace(QString("Fail to convert process' PID to number: '%1'").arg(pidStrings.first())),
                         continue);

        res << getChildPidsRecursive(pid);
        res << pid;
    }

    return res;
}

void ExternalToolRunTask::addOutputListener(ExternalToolListener* outputListener) {
    if(helper) {
        helper->addOutputListener(outputListener);
    }
    listener = outputListener;
}

void ExternalToolRunTask::parseStandartOutputFile(QString &filepath) {
    QFile f(filepath);
    if (!f.open(QIODevice::ReadOnly)) {
        return;
    }
    QString output;
    for (QByteArray line = f.readLine(); line.length() > 0; line = f.readLine()) {
        output += line;
    }
    f.close();
    logParser->parseOutput(output);
}

////////////////////////////////////////
//ExternalToolSupportTask
void ExternalToolSupportTask::setListenerForTask(ExternalToolRunTask* runTask, int listenerNumber) {
    CHECK(listeners.size() > listenerNumber, );
    runTask->addOutputListener(listeners.at(listenerNumber));
}

void ExternalToolSupportTask::setListenerForHelper(ExternalToolRunTaskHelper* helper, int listenerNumber) {
    CHECK(listeners.size() > listenerNumber, );
    helper->addOutputListener(listeners.at(listenerNumber));
}

ExternalToolListener* ExternalToolSupportTask::getListener(int listenerNumber) {
    CHECK(listeners.size() > listenerNumber, NULL);
    return listeners.at(listenerNumber);
}

////////////////////////////////////////
//ExternalToolRunTaskHelper
ExternalToolRunTaskHelper::ExternalToolRunTaskHelper(ExternalToolRunTask* t)
: process(t->externalToolProcess), logParser(t->logParser), os(t->stateInfo), listener(NULL)
{
    logData.resize(1000);
    connect(process, SIGNAL(readyReadStandardOutput()), SLOT(sl_onReadyToReadLog()));
    connect(process, SIGNAL(readyReadStandardError()), SLOT(sl_onReadyToReadErrLog()));
}

ExternalToolRunTaskHelper::ExternalToolRunTaskHelper(QProcess *_process, ExternalToolLogParser *_logParser, U2OpStatus &_os)
: process(_process), logParser(_logParser), os(_os), listener(NULL)
{
    logData.resize(1000);
    connect(process, SIGNAL(readyReadStandardOutput()), SLOT(sl_onReadyToReadLog()));
    connect(process, SIGNAL(readyReadStandardError()), SLOT(sl_onReadyToReadErrLog()));
}

void ExternalToolRunTaskHelper::sl_onReadyToReadLog(){
    QMutexLocker locker(&logMutex);

    CHECK(NULL != process, );
    if (process->readChannel() == QProcess::StandardError) {
        process->setReadChannel(QProcess::StandardOutput);
    }
    int numberReadChars = process->read(logData.data(), logData.size());
    while(numberReadChars > 0){
        //call log parser
        QString line = QString::fromLocal8Bit(logData.constData(), numberReadChars);
        logParser->parseOutput(line);
        if(NULL != listener) {
            listener->addNewLogMessage(line, ExternalToolListener::OUTPUT_LOG);
        }
        numberReadChars = process->read(logData.data(), logData.size());
    }
    os.setProgress(logParser->getProgress());
}

void ExternalToolRunTaskHelper::sl_onReadyToReadErrLog(){
    QMutexLocker locker(&logMutex);

    CHECK(NULL != process, );
    if (process->readChannel() == QProcess::StandardOutput) {
        process->setReadChannel(QProcess::StandardError);
    }
    int numberReadChars = process->read(logData.data(), logData.size());
    while(numberReadChars > 0){
        //call log parser
        QString line = QString::fromLocal8Bit(logData.constData(), numberReadChars);
        logParser->parseErrOutput(line);
        if(NULL != listener) {
            listener->addNewLogMessage(line, ExternalToolListener::ERROR_LOG);
        }
        numberReadChars = process->read(logData.data(), logData.size());
    }
    QString lastErr = logParser->getLastError();
    if (!lastErr.isEmpty()) {
        os.setError(lastErr);
    }
    os.setProgress(logParser->getProgress());
}

void ExternalToolRunTaskHelper::addOutputListener(ExternalToolListener* _listener) {
    listener = _listener;
}

////////////////////////////////////////
//ExternalToolLogParser
ExternalToolLogParser::ExternalToolLogParser() {
    progress=-1;
    lastLine="";
    lastErrLine="";
    lastError="";
}

void ExternalToolLogParser::parseOutput(const QString &partOfLog){
    lastPartOfLog = partOfLog.split(QChar('\n'));
    lastPartOfLog.first() = lastLine+lastPartOfLog.first();
    lastLine = lastPartOfLog.takeLast();
    foreach (const QString &buf, lastPartOfLog) {
        processLine(buf);
    }
}

void ExternalToolLogParser::parseErrOutput(const QString &partOfLog){
    lastPartOfLog = partOfLog.split(QChar('\n'));
    lastPartOfLog.first() = lastErrLine+lastPartOfLog.first();
    lastErrLine = lastPartOfLog.takeLast();
    foreach(const QString &buf, lastPartOfLog) {
        processErrLine(buf);
    }
}

void ExternalToolLogParser::processLine(const QString &line) {
    if (isError(line)) {
        setLastError(line);
    } else {
        ioLog.trace(line);
    }
}

void ExternalToolLogParser::processErrLine(const QString &line) {
    if (isError(line)) {
        setLastError(line);
    } else {
        ioLog.trace(line);
    }
}

bool ExternalToolLogParser::isError(const QString &line) const {
    return line.contains("error", Qt::CaseInsensitive);
}

void ExternalToolLogParser::setLastError(const QString &value) {
    if (!value.isEmpty()) {
        ioLog.error(value);
    }
    lastError = value;
}

////////////////////////////////////////
//ExternalToolSupportUtils
void ExternalToolSupportUtils::removeTmpDir( const QString& tmpDirUrl, U2OpStatus& os) {
    if (tmpDirUrl.isEmpty()) {
        os.setError(tr("Can not remove temporary folder: path is empty."));
        return;
    }
    QDir tmpDir(tmpDirUrl);
    foreach(const QString& file, tmpDir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries)){
        if (!tmpDir.remove(file)) {
            os.setError(tr("Can not remove files from temporary folder."));
            return;
        }
    }
    if (!tmpDir.rmdir(tmpDir.absolutePath())){
        os.setError(tr("Can not remove folder for temporary files."));
    }
}

QString ExternalToolSupportUtils::createTmpDir(const QString &prePath, const QString &domain, U2OpStatus &os) {
    int i = 0;
    while (true) {
        QString tmpDirName = QString("d_%1").arg(i);
        QString tmpDirPath = prePath + "/" + domain + "/" + tmpDirName;
        QDir tmpDir(tmpDirPath);

        if (!tmpDir.exists()) {
            if (!QDir().mkpath(tmpDirPath)) {
                os.setError(tr("Can not create folder for temporary files: %1").arg(tmpDirPath));
            }
            return tmpDir.absolutePath();
        }
        i++;
    }
}

QString ExternalToolSupportUtils::createTmpDir(const QString& domain, U2OpStatus& os) {
    QString tmpDirPath = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath();
    return createTmpDir(tmpDirPath, domain, os);
}

void ExternalToolSupportUtils::appendExistingFile(const QString &path, QStringList &files) {
    GUrl url(path);
    if (QFile::exists(url.getURLString())) {
        files << url.getURLString();
    }
}

bool ExternalToolSupportUtils::startExternalProcess(QProcess *process, const QString &program, const QStringList &arguments) {
    process->start(program, arguments);
    bool started = process->waitForStarted(START_WAIT_MSEC);

#ifdef Q_OS_WIN32
    if(!started) {
        QString execStr = WIN_LAUNCH_CMD_COMMAND + program;
        foreach (const QString arg, arguments) {
            execStr += " " + arg;
        }
        process->start(execStr);
        coreLog.trace(tr("Can't run an executable file \"%1\" as it is. Try to run it as a cmd line command: \"%2\"")
            .arg(program).arg(execStr));
        started = process->waitForStarted(START_WAIT_MSEC);
    }
#endif

    return started;
}

ProcessRun ExternalToolSupportUtils::prepareProcess(const QString &toolName, const QStringList &arguments, const QString &workingDirectory, const QStringList &additionalPaths, U2OpStatus &os, ExternalToolListener* listener) {
    ProcessRun result;
    result.process = NULL;
    result.arguments = arguments;

    ExternalTool *tool = AppContext::getExternalToolRegistry()->getByName(toolName);
    if (NULL == tool) {
        os.setError(tr("Undefined tool: '%1'").arg(toolName));
        return result;
    }
    if (tool->getPath().isEmpty()) {
        os.setError(tr("Path for '%1' tool not set").arg(toolName));
        return result;
    }
    result.program = tool->getPath();
    QString toolRunnerProgram = tool->getToolRunnerProgram();
    QString listenerProgramMessage = result.program;

    if (!toolRunnerProgram.isEmpty()){
        ScriptingToolRegistry *stregister = AppContext::getScriptingToolRegistry();
        SAFE_POINT_EXT(NULL != stregister, os.setError("No scripting tool registry"), result);
        ScriptingTool *stool = stregister->getByName(toolRunnerProgram);
        if (NULL == stool || stool->getPath().isEmpty()){
            os.setError(QString("The tool %1 that runs %2 is not installed. Please set the path of the tool in the External Tools settings").arg(toolRunnerProgram).arg(toolName));
            return result;
        }
        result.arguments.prepend(result.program);

        for (int i = stool->getRunParameters().size() - 1; i >= 0; i--){
            result.arguments.prepend(stool->getRunParameters().at(i));
        }
        foreach(const QString &param, tool->getToolRunnerAdditionalOptions()) {
            result.arguments.prepend(param);
        }
        result.program = stool->getPath();
        listenerProgramMessage.prepend(result.program + " ");
    }

#ifdef Q_OS_WIN
    const QString pathVariableSeparator = ";";
#else
    const QString pathVariableSeparator = ":";
#endif

    QProcessEnvironment processEnvironment = QProcessEnvironment::systemEnvironment();
    QString path = additionalPaths.join(pathVariableSeparator) + pathVariableSeparator +
                   tool->getAdditionalPaths().join(pathVariableSeparator) + pathVariableSeparator +
                   processEnvironment.value("PATH");
    if (!additionalPaths.isEmpty()) {
        algoLog.trace(QString("PATH environment variable: '%1'").arg(path));
    }
    processEnvironment.insert("PATH", path);

    result.process = new QProcess();
    result.process->setProcessEnvironment(processEnvironment);
    if (!workingDirectory.isEmpty()){
        result.process->setWorkingDirectory(workingDirectory);
        algoLog.details(tr("Working folder is \"%1\"").arg(result.process->workingDirectory()));
    }

    // QProcess wraps arguments that contain spaces in quotes automatically.
    // But launched line should look correctly in the log.
    QStringList argumentListForDisplay;
    foreach (const QString& arg, result.arguments) {
        argumentListForDisplay << GUrlUtils::getQuotedString(arg);
    }
    algoLog.details(tr("Launching %1 tool: %2 %3").arg(toolName).arg(result.program).arg(argumentListForDisplay.join(" ")));

    if(NULL != listener) {
        listener->setToolName(toolName);
        listener->addNewLogMessage(listenerProgramMessage, ExternalToolListener::PROGRAM_PATH);
        QString argumentsLine = ExternalToolSupportUtils::prepareArgumentsForCmdLine(arguments);
        argumentsLine.replace(" -", "\n-");

        listener->addNewLogMessage(argumentsLine, ExternalToolListener::ARGUMENTS);
    }
    return result;
}

QString ExternalToolSupportUtils::prepareArgumentsForCmdLine(const QStringList &arguments) {
    QString argumentsLine;
    foreach(QString argumentStr, arguments) {
        //Find start of the parameter value
        int startIndex = argumentStr.indexOf('=') + 1;
        //Add quotes if parameter contains whitespace characters
        QString valueStr = argumentStr.mid(startIndex);
        if (valueStr.contains(' ') || valueStr.contains('\t')) {
            argumentStr.append('"');
            argumentStr.insert(startIndex, '"');
        }
        argumentsLine += ' ' + argumentStr;
    }
    return argumentsLine;
}

QVariantMap ExternalToolSupportUtils::getScoresGapDependencyMap() {
    QVariantMap map;
    QVariantMap gaps;
    gaps["2 2"] = "2 2";
    gaps["1 2"] = "1 2";
    gaps["0 2"] = "0 2";
    gaps["2 1"] = "2 1";
    gaps["1 1"] = "1 1";
    map.insert("1 -4", gaps);
    map.insert("1 -3", gaps);

    gaps.clear();
    gaps["2 2"] = "2 2";
    gaps["1 2"] = "1 2";
    gaps["0 2"] = "0 2";
    gaps["3 1"] = "3 1";
    gaps["2 1"] = "2 1";
    gaps["1 1"] = "1 1";
    map.insert("1 -2", gaps);

    gaps.clear();
    gaps["4 2"] = "4 2";
    gaps["3 2"] = "3 2";
    gaps["2 2"] = "2 2";
    gaps["1 2"] = "1 2";
    gaps["0 2"] = "0 2";
    gaps["4 1"] = "4 1";
    gaps["3 1"] = "3 1";
    gaps["2 1"] = "2 1";
    map.insert("1 -1", gaps);

    gaps.clear();
    gaps["4 4"] = "4 4";
    gaps["2 4"] = "2 4";
    gaps["0 4"] = "0 4";
    gaps["4 2"] = "4 2";
    gaps["2 2"] = "2 2";
    map.insert("2 -7", gaps);
    map.insert("2 -5", gaps);

    gaps.clear();
    gaps["6 4"] = "6 4";
    gaps["4 4"] = "4 4";
    gaps["2 4"] = "2 4";
    gaps["0 4"] = "0 4";
    gaps["3 3"] = "3 3";
    gaps["6 2"] = "6 2";
    gaps["5 2"] = "5 2";
    gaps["4 2"] = "4 2";
    gaps["2 2"] = "2 2";
    map.insert("2 -3", gaps);

    gaps.clear();
    gaps["12 8"] = "12 8";
    gaps["6 5"] = "6 5";
    gaps["5 5"] = "5 5";
    gaps["4 5"] = "4 5";
    gaps["3 5"] = "3 5";
    map.insert("4 -5", gaps);
    map.insert("5 -4", gaps);

    return map;
}

ExternalToolLogProcessor::~ExternalToolLogProcessor() {

}

ExternalToolListener::ExternalToolListener(ExternalToolLogProcessor *logProcessor) :
    logProcessor(logProcessor)
{

}

ExternalToolListener::~ExternalToolListener() {
    delete logProcessor;
}

void ExternalToolListener::setToolName(const QString &_toolName) {
    toolName = _toolName;
}

void ExternalToolListener::setLogProcessor(ExternalToolLogProcessor *newLogProcessor) {
    delete logProcessor;
    logProcessor = newLogProcessor;
}

const QString &ExternalToolListener::getToolName() const {
    return toolName;
}

}//namespace
