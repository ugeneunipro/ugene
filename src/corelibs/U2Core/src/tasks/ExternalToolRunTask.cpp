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

#include "ExternalToolRunTask.h"

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/CmdlineTaskRunner.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/Log.h>
#include <U2Core/ScriptingToolRegistry.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <QDir>
#include <QRegularExpression>

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

ExternalToolRunTask::ExternalToolRunTask(const QString &_toolId, const QStringList &_arguments,
    ExternalToolLogParser *_logParser, const QString &_workingDirectory, const QStringList &_additionalPaths,
    const QString &_additionalProcessToKill, bool parseOutputFile)
    : Task(AppContext::getExternalToolRegistry()->getToolNameById(_toolId) + tr(" tool"), TaskFlag_None),
    arguments(_arguments),
    logParser(_logParser),
    toolId(_toolId),
    workingDirectory(_workingDirectory),
    additionalPaths(_additionalPaths),
    externalToolProcess(NULL),
    helper(NULL),
    listener(NULL),
    additionalProcessToKill(_additionalProcessToKill),
    parseOutputFile(parseOutputFile) {
    CHECK_EXT(AppContext::getExternalToolRegistry()->getById(toolId) != nullptr, stateInfo.setError(tr("External tool is absent")), );

    toolName = AppContext::getExternalToolRegistry()->getToolNameById(toolId);
    coreLog.trace("Creating run task for: " + toolName);
    if (NULL != logParser) {
        logParser->setParent(this);
    }
}

ExternalToolRunTask::~ExternalToolRunTask() {
    delete externalToolProcess;
}

void ExternalToolRunTask::run() {
    if (hasError() || isCanceled()) {
        return;
    }

    ProcessRun pRun = ExternalToolSupportUtils::prepareProcess(toolId, arguments, workingDirectory, additionalPaths, stateInfo, listener);
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
        foreach(const QString& envVarName, additionalEnvVariables.keys()) {
            processEnvironment.insert(envVarName, additionalEnvVariables.value(envVarName));
        }
        externalToolProcess->setProcessEnvironment(processEnvironment);
    }

    helper.reset(new ExternalToolRunTaskHelper(this));
    if (NULL != listener) {
        helper->addOutputListener(listener);
    }

    externalToolProcess->start(pRun.program, pRun.arguments);
    bool started = externalToolProcess->waitForStarted(START_WAIT_MSEC);

    if (!started) {
        ExternalTool* tool = AppContext::getExternalToolRegistry()->getById(toolId);
        if (tool->isValid()) {
            stateInfo.setError(tr("Can not run %1 tool.").arg(toolName));
        } else {
            stateInfo.setError(tr("Can not run %1 tool. May be tool path '%2' not valid?")
                .arg(toolName)
                .arg(AppContext::getExternalToolRegistry()->getById(toolId)->getPath()));
        }
        return;
    }
    while (!externalToolProcess->waitForFinished(1000)) {
        if (isCanceled()) {
            killProcess(externalToolProcess, additionalProcessToKill);
            algoLog.details(tr("Tool %1 is cancelled").arg(toolName));
            return;
        }
    }

    {

        QProcess::ExitStatus status = externalToolProcess->exitStatus();
        int exitCode = externalToolProcess->exitCode();
        if (status == QProcess::CrashExit && !hasError()) {
            QString error = parseStandartOutputFile();
            if (error.isEmpty()) {
                QString intendedError = tr("%1 tool exited with the following error: %2 (Code: %3)")
                                           .arg(toolName)
                                           .arg(externalToolProcess->errorString())
                                           .arg(externalToolProcess->exitCode());
                parseError(intendedError);
                error = logParser->getLastError();
            }

            setError(error);
        } else if (status == QProcess::NormalExit && exitCode != EXIT_SUCCESS && !hasError()) {
            QString error = parseStandartOutputFile();
            setError(error.isEmpty() ? tr("%1 tool exited with code %2").arg(toolName).arg(exitCode) : error);
        } else if (status == QProcess::NormalExit && exitCode == EXIT_SUCCESS && !hasError()) {
            algoLog.details(tr("Tool %1 finished successfully").arg(toolName));
        }
    }
}

void ExternalToolRunTask::killProcess(QProcess *process, QString childProcesses) {
    CmdlineTaskRunner::killProcessTree(process);
}

QList<long> ExternalToolRunTask::getChildPidsRecursive(long parentPid) {
    return CmdlineTaskRunner::getChildrenProcesses(parentPid);
}

void ExternalToolRunTask::addOutputListener(ExternalToolListener* outputListener) {
    if (helper) {
        helper->addOutputListener(outputListener);
    }
    listener = outputListener;
}

QString ExternalToolRunTask::parseStandartOutputFile() const {
    CHECK(parseOutputFile, QString());

    QFile f(outputFile);
    CHECK(f.open(QIODevice::ReadOnly), QString());

    QString output;
    for (QByteArray line = f.readLine(); line.length() > 0; line = f.readLine()) {
        output += line;
    }
    f.close();
    logParser->parseOutput(output);

    return logParser->getLastError();
}

void ExternalToolRunTask::parseError(const QString& error) const {
    logParser->parseErrOutput(error);
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
    : os(t->stateInfo), logParser(t->logParser), process(t->externalToolProcess), listener(NULL) {
    logData.resize(1000);
    connect(process, SIGNAL(readyReadStandardOutput()), SLOT(sl_onReadyToReadLog()));
    connect(process, SIGNAL(readyReadStandardError()), SLOT(sl_onReadyToReadErrLog()));
}

ExternalToolRunTaskHelper::ExternalToolRunTaskHelper(QProcess *_process, ExternalToolLogParser *_logParser, U2OpStatus &_os)
    : os(_os), logParser(_logParser), process(_process), listener(NULL) {
    logData.resize(1000);
    connect(process, SIGNAL(readyReadStandardOutput()), SLOT(sl_onReadyToReadLog()));
    connect(process, SIGNAL(readyReadStandardError()), SLOT(sl_onReadyToReadErrLog()));
}

void ExternalToolRunTaskHelper::sl_onReadyToReadLog() {
    QMutexLocker locker(&logMutex);

    CHECK(NULL != process, );
    if (process->readChannel() == QProcess::StandardError) {
        process->setReadChannel(QProcess::StandardOutput);
    }
    int numberReadChars = static_cast<int>(process->read(logData.data(), logData.size()));
    while (numberReadChars > 0) {
        //call log parser
        QString line = QString::fromLocal8Bit(logData.constData(), numberReadChars);
        logParser->parseOutput(line);
        if (NULL != listener) {
            listener->addNewLogMessage(line, ExternalToolListener::OUTPUT_LOG);
        }
        numberReadChars = static_cast<int>(process->read(logData.data(), logData.size()));
    }
    os.setProgress(logParser->getProgress());
}

void ExternalToolRunTaskHelper::sl_onReadyToReadErrLog() {
    QMutexLocker locker(&logMutex);

    CHECK(NULL != process, );
    if (process->readChannel() == QProcess::StandardOutput) {
        process->setReadChannel(QProcess::StandardError);
    }
    int numberReadChars = static_cast<int>(process->read(logData.data(), logData.size()));
    while (numberReadChars > 0) {
        //call log parser
        QString line = QString::fromLocal8Bit(logData.constData(), numberReadChars);
        logParser->parseErrOutput(line);
        if (NULL != listener) {
            listener->addNewLogMessage(line, ExternalToolListener::ERROR_LOG);
        }
        numberReadChars = static_cast<int>(process->read(logData.data(), logData.size()));
    }
    processErrorToLog();
    os.setProgress(logParser->getProgress());
}

void ExternalToolRunTaskHelper::addOutputListener(ExternalToolListener* _listener) {
    listener = _listener;
}

void ExternalToolRunTaskHelper::processErrorToLog() {
    QString lastErr = logParser->getLastError();
    if (!lastErr.isEmpty()) {
        os.setError(lastErr);
    }
}

////////////////////////////////////////
//ExternalToolLogParser
ExternalToolLogParser::ExternalToolLogParser(bool _writeErrorsToLog) {
    progress = -1;
    lastLine = "";
    lastErrLine = "";
    lastError = "";
    writeErrorsToLog = _writeErrorsToLog;
}

void ExternalToolLogParser::parseOutput(const QString &partOfLog) {
    lastPartOfLog = partOfLog.split(QRegularExpression("\\r?\\n"));
    lastPartOfLog.first() = lastLine + lastPartOfLog.first();
    //It's a possible situation, that one message will be processed twice
    lastLine = lastPartOfLog.last();
    foreach (const QString &buf, lastPartOfLog) {
        processLine(buf);
    }
}

void ExternalToolLogParser::parseErrOutput(const QString &partOfLog) {
    lastPartOfLog = partOfLog.split(QRegularExpression("\\r?\\n"));
    lastPartOfLog.first() = lastErrLine + lastPartOfLog.first();
    //It's a possible situation, that one message will be processed twice
    lastErrLine = lastPartOfLog.last();
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
    if (!value.isEmpty() && writeErrorsToLog) {
        ioLog.error(value);
    }
    lastError = value;
}

////////////////////////////////////////
//ExternalToolSupportUtils
void ExternalToolSupportUtils::removeTmpDir(const QString& tmpDirUrl, U2OpStatus& os) {
    if (tmpDirUrl.isEmpty()) {
        os.setError(tr("Can not remove temporary folder: path is empty."));
        return;
    }
    QDir tmpDir(tmpDirUrl);
    foreach(const QString& file, tmpDir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries)) {
        if (!tmpDir.remove(file)) {
            os.setError(tr("Can not remove files from temporary folder."));
            return;
        }
    }
    if (!tmpDir.rmdir(tmpDir.absolutePath())) {
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
    if (!started) {
        QString execStr = WIN_LAUNCH_CMD_COMMAND + program;
        foreach(const QString arg, arguments) {
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

ProcessRun ExternalToolSupportUtils::prepareProcess(const QString &toolId, const QStringList &arguments, const QString &workingDirectory, const QStringList &additionalPaths, U2OpStatus &os, ExternalToolListener* listener) {
    ProcessRun result;
    result.process = NULL;
    result.arguments = arguments;

    ExternalTool *tool = AppContext::getExternalToolRegistry()->getById(toolId);
    CHECK_EXT(nullptr != tool, os.setError(tr("A tool with the ID %1 is absent").arg(toolId)), result);

    const QString toolName = tool->getName();
    if (tool->getPath().isEmpty()) {
        os.setError(tr("Path for '%1' tool not set").arg(toolName));
        return result;
    }
    result.program = tool->getPath();
    QString toolRunnerProgram = tool->getToolRunnerProgramId();

    if (!toolRunnerProgram.isEmpty()) {
        ScriptingToolRegistry *stregister = AppContext::getScriptingToolRegistry();
        SAFE_POINT_EXT(NULL != stregister, os.setError("No scripting tool registry"), result);
        ScriptingTool *stool = stregister->getById(toolRunnerProgram);
        if (NULL == stool || stool->getPath().isEmpty()) {
            os.setError(QString("The tool %1 that runs %2 is not installed. Please set the path of the tool in the External Tools settings").arg(toolRunnerProgram).arg(toolName));
            return result;
        }
        result.arguments.prepend(result.program);

        for (int i = stool->getRunParameters().size() - 1; i >= 0; i--) {
            result.arguments.prepend(stool->getRunParameters().at(i));
        }
        foreach(const QString &param, tool->getToolRunnerAdditionalOptions()) {
            result.arguments.prepend(param);
        }
        result.program = stool->getPath();
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
    if (!workingDirectory.isEmpty()) {
        result.process->setWorkingDirectory(workingDirectory);
        algoLog.details(tr("Working folder is \"%1\"").arg(result.process->workingDirectory()));
    }

    // QProcess wraps arguments that contain spaces in quotes automatically.
    // But launched line should look correctly in the log.
    const QString commandWithArguments = GUrlUtils::getQuotedString(result.program) + ExternalToolSupportUtils::prepareArgumentsForCmdLine(result.arguments);
    algoLog.details(tr("Launching %1 tool: %2").arg(toolName).arg(commandWithArguments));

    if (NULL != listener) {
        listener->setToolName(toolName);
        listener->addNewLogMessage(commandWithArguments, ExternalToolListener::PROGRAM_WITH_ARGUMENTS);
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

QStringList ExternalToolSupportUtils::splitCmdLineArguments(const QString &program) {
    // a function body from "qprocess.cpp"

    QStringList args;
    QString tmp;
    int quoteCount = 0;
    bool inQuote = false;

    // handle quoting. tokens can be surrounded by double quotes
    // "hello world". three consecutive double quotes represent
    // the quote character itself.
    for (int i = 0; i < program.size(); ++i) {
        if (program.at(i) == QLatin1Char('"') || program.at(i) == QLatin1Char('\'')) {
            ++quoteCount;
            if (quoteCount == 3) {
                // third consecutive quote
                quoteCount = 0;
                tmp += program.at(i);
            }
            continue;
        }
        if (quoteCount) {
            if (quoteCount == 1)
                inQuote = !inQuote;
            quoteCount = 0;
        }
        if (!inQuote && program.at(i).isSpace()) {
            if (!tmp.isEmpty()) {
                args += tmp;
                tmp.clear();
            }
        } else {
            tmp += program.at(i);
        }
    }
    if (!tmp.isEmpty())
        args += tmp;

    return args;
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
logProcessor(logProcessor) {

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
