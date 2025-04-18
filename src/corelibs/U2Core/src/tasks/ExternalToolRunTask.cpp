/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include <QDir>
#include <QRegularExpression>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/CmdlineTaskRunner.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/Log.h>
#include <U2Core/ScriptingToolRegistry.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

namespace U2 {

#define SETTINGS_ROOT QString("/genome_aligner_settings/")
#define INDEX_DIR QString("index_dir")

#define START_WAIT_MSEC 3000

ExternalToolRunTask::ExternalToolRunTask(const QString& _toolId, const QStringList& _arguments, ExternalToolLogParser* _logParser, const QString& _workingDirectory, const QStringList& _additionalPaths, bool parseOutputFile)
    : Task(AppContext::getExternalToolRegistry()->getToolNameById(_toolId) + tr(" tool"), TaskFlag_None),
      arguments(_arguments),
      logParser(_logParser),
      toolId(_toolId),
      workingDirectory(_workingDirectory),
      additionalPaths(_additionalPaths),
      parseOutputFile(parseOutputFile) {
}

ExternalToolRunTask::~ExternalToolRunTask() {
    delete externalToolProcess;
}

void ExternalToolRunTask::prepare() {
    ExternalTool* tool = AppContext::getExternalToolRegistry()->getById(toolId);
    CHECK_EXT(tool != nullptr, stateInfo.setError(tr("External tool \"%1\" is absent").arg(toolId)), );
    toolName = tool->getName();

    CHECK_EXT(QFile::exists(tool->getPath()), stateInfo.setError(tr("External tool '%1' doesn't exist").arg(tool->getPath())), )
    tool->checkArgsAndPaths(arguments, stateInfo);

    if (!stateInfo.hasError() && logParser != nullptr) {
        logParser->setParent(this);
    }
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
        foreach (const QString& envVarName, additionalEnvVariables.keys()) {
            processEnvironment.insert(envVarName, additionalEnvVariables.value(envVarName));
        }
        externalToolProcess->setProcessEnvironment(processEnvironment);
    }

    helper.reset(new ExternalToolRunTaskHelper(this));
    if (listener != nullptr) {
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
    externalToolProcess->closeWriteChannel();
    while (!externalToolProcess->waitForFinished(1000)) {
        if (isCanceled()) {
            killProcess(externalToolProcess);
            if (!externalToolProcess->waitForFinished(2000)) {  // wait 2 seconds to let OS kill process in previous method
                externalToolProcess->kill();
            }
            if (externalToolProcess->state() != QProcess::NotRunning && !externalToolProcess->waitForFinished(10000)) {
                algoLog.info(tr("Unable to cancel tool %1 for 10 seconds. Stop \"%2\" process manually by your OS task manager.").arg(toolName).arg(AppContext::getExternalToolRegistry()->getById(toolId)->getExecutableFileName()));
            } else {
                algoLog.details(tr("Tool %1 is cancelled").arg(toolName));
            }
            return;
        }
    }

    {
        QProcess::ExitStatus status = externalToolProcess->exitStatus();
        int exitCode = externalToolProcess->exitCode();
        if (status == QProcess::CrashExit && !hasError()) {
            QString error = parseStandardOutputFile();
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
            QString error = parseStandardOutputFile();
            setError(error.isEmpty() ? tr("%1 tool exited with code %2").arg(toolName).arg(exitCode) : error);
        } else if (status == QProcess::NormalExit && exitCode == EXIT_SUCCESS && !hasError()) {
            algoLog.details(tr("Tool %1 finished successfully").arg(toolName));
        }
    }
}

void ExternalToolRunTask::killProcess(QProcess* process) {
    CmdlineTaskRunner::killProcessTree(process);
}

void ExternalToolRunTask::addOutputListener(ExternalToolListener* outputListener) {
    if (helper) {
        helper->addOutputListener(outputListener);
    }
    listener = outputListener;
}

QString ExternalToolRunTask::parseStandardOutputFile() const {
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
// ExternalToolSupportTask
void ExternalToolSupportTask::setListenerForTask(ExternalToolRunTask* runTask, int listenerNumber) {
    CHECK(listeners.size() > listenerNumber, );
    runTask->addOutputListener(listeners.at(listenerNumber));
}

void ExternalToolSupportTask::setListenerForHelper(ExternalToolRunTaskHelper* helper, int listenerNumber) {
    CHECK(listeners.size() > listenerNumber, );
    helper->addOutputListener(listeners.at(listenerNumber));
}

ExternalToolListener* ExternalToolSupportTask::getListener(int listenerNumber) {
    CHECK(listeners.size() > listenerNumber, nullptr);
    return listeners.at(listenerNumber);
}

////////////////////////////////////////
// ExternalToolRunTaskHelper
ExternalToolRunTaskHelper::ExternalToolRunTaskHelper(ExternalToolRunTask* t)
    : os(t->stateInfo), logParser(t->logParser), process(t->externalToolProcess), listener(nullptr) {
    logData.resize(1000);
    connect(process, SIGNAL(readyReadStandardOutput()), SLOT(sl_onReadyToReadLog()));
    connect(process, SIGNAL(readyReadStandardError()), SLOT(sl_onReadyToReadErrLog()));
}

ExternalToolRunTaskHelper::ExternalToolRunTaskHelper(QProcess* _process, ExternalToolLogParser* _logParser, U2OpStatus& _os)
    : os(_os), logParser(_logParser), process(_process), listener(nullptr) {
    logData.resize(1000);
    connect(process, SIGNAL(readyReadStandardOutput()), SLOT(sl_onReadyToReadLog()));
    connect(process, SIGNAL(readyReadStandardError()), SLOT(sl_onReadyToReadErrLog()));
}

void ExternalToolRunTaskHelper::sl_onReadyToReadLog() {
    QMutexLocker locker(&logMutex);

    CHECK(process != nullptr, );
    if (process->readChannel() == QProcess::StandardError) {
        process->setReadChannel(QProcess::StandardOutput);
    }
    int numberReadChars = static_cast<int>(process->read(logData.data(), logData.size()));
    while (numberReadChars > 0) {
        // call log parser
        QString line = QString::fromLocal8Bit(logData.constData(), numberReadChars);
        logParser->parseOutput(line);
        if (listener != nullptr) {
            listener->addNewLogMessage(line, ExternalToolListener::OUTPUT_LOG);
        }
        numberReadChars = static_cast<int>(process->read(logData.data(), logData.size()));
    }
    os.setProgress(logParser->getProgress());
}

void ExternalToolRunTaskHelper::sl_onReadyToReadErrLog() {
    QMutexLocker locker(&logMutex);

    CHECK(process != nullptr, );
    if (process->readChannel() == QProcess::StandardOutput) {
        process->setReadChannel(QProcess::StandardError);
    }
    int numberReadChars = static_cast<int>(process->read(logData.data(), logData.size()));
    while (numberReadChars > 0) {
        // call log parser
        QString line = QString::fromLocal8Bit(logData.constData(), numberReadChars);
        logParser->parseErrOutput(line);
        if (listener != nullptr) {
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
// ExternalToolLogParser
ExternalToolLogParser::ExternalToolLogParser(bool _writeErrorsToLog) {
    progress = -1;
    lastLine = "";
    lastErrLine = "";
    lastError = "";
    writeErrorsToLog = _writeErrorsToLog;
}

void ExternalToolLogParser::parseOutput(const QString& partOfLog) {
    lastPartOfLog = partOfLog.split(QRegularExpression("\\r?\\n"));
    lastPartOfLog.first() = lastLine + lastPartOfLog.first();
    // It's a possible situation, that one message will be processed twice
    lastLine = lastPartOfLog.last();
    foreach (const QString& buf, lastPartOfLog) {
        processLine(buf);
    }
}

void ExternalToolLogParser::parseErrOutput(const QString& partOfLog) {
    lastPartOfLog = partOfLog.split(QRegularExpression("\\r?\\n"));
    lastPartOfLog.first() = lastErrLine + lastPartOfLog.first();
    // It's a possible situation, that one message will be processed twice
    lastErrLine = lastPartOfLog.last();
    foreach (const QString& buf, lastPartOfLog) {
        processErrLine(buf);
    }
}

void ExternalToolLogParser::processLine(const QString& line) {
    if (isError(line)) {
        setLastError(line);
    } else {
        ioLog.trace(line);
    }
}

void ExternalToolLogParser::processErrLine(const QString& line) {
    if (isError(line)) {
        setLastError(line);
    } else {
        ioLog.trace(line);
    }
}

bool ExternalToolLogParser::isError(const QString& line) const {
    return line.contains("error", Qt::CaseInsensitive);
}

void ExternalToolLogParser::setLastError(const QString& value) {
    if (!value.isEmpty() && writeErrorsToLog) {
        ioLog.error(value);
    }
    lastError = value;
}

////////////////////////////////////////
// ExternalToolSupportUtils
void ExternalToolSupportUtils::removeTmpDir(const QString& absolutePath, U2OpStatus& os) {
    if (absolutePath.isEmpty()) {
        os.setError(tr("Can not remove temporary folder: path is empty."));
        return;
    }
    QDir tmpDir(absolutePath);
    if (!tmpDir.removeRecursively()) {
        os.setError(tr("Can not remove folder for temporary files, folder \"%1\".").arg(tmpDir.absolutePath()));
    }
}

QString ExternalToolSupportUtils::createTmpDir(const QString& prePath, const QString& domain, U2OpStatus& os) {
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

void ExternalToolSupportUtils::appendExistingFile(const QString& path, QStringList& files) {
    GUrl url(path);
    if (QFile::exists(url.getURLString())) {
        files << url.getURLString();
    }
}

bool ExternalToolSupportUtils::startExternalProcess(QProcess* process, const QString& program, const QStringList& arguments) {
    process->start(program, arguments);
    bool started = process->waitForStarted(START_WAIT_MSEC);

    if (isOsWindows()) {
        if (!started) {
            QStringList args = {"/C"};
            args << arguments;
            process->start("cmd", args);
            coreLog.trace(tr(R"(Can't run the executable file "%1" directly. Trying to run it as a command line command: "%2")")
                              .arg(program)
                              .arg("cmd " + args.join(" ")));
            started = process->waitForStarted(START_WAIT_MSEC);
        }
    }
    return started;
}

ProcessRun ExternalToolSupportUtils::prepareProcess(const QString& toolId, const QStringList& arguments, const QString& workingDirectory, const QStringList& additionalPaths, U2OpStatus& os, ExternalToolListener* listener) {
    ProcessRun result;
    result.process = nullptr;
    result.arguments = arguments;

    ExternalTool* tool = AppContext::getExternalToolRegistry()->getById(toolId);
    tool->checkArgsAndPaths(arguments, os);
    if (os.hasError()) {
        return result;
    }
    CHECK_EXT(tool != nullptr, os.setError(tr("A tool with the ID %1 is absent").arg(toolId)), result);

    const QString toolName = tool->getName();
    if (tool->getPath().isEmpty()) {
        os.setError(tr("Path for '%1' tool not set").arg(toolName));
        return result;
    }
    result.program = tool->getPath();
    QString toolRunnerProgram = tool->getToolRunnerProgramId();

    if (!toolRunnerProgram.isEmpty()) {
        ScriptingToolRegistry* stregister = AppContext::getScriptingToolRegistry();
        SAFE_POINT_EXT(stregister != nullptr, os.setError("No scripting tool registry"), result);
        ScriptingTool* stool = stregister->getById(toolRunnerProgram);
        if (stool == nullptr || stool->getPath().isEmpty()) {
            os.setError(QString("The tool %1 that runs %2 is not installed. Please set the path of the tool in the External Tools settings").arg(toolRunnerProgram).arg(toolName));
            return result;
        }
        result.arguments.prepend(result.program);

        for (int i = stool->getRunParameters().size() - 1; i >= 0; i--) {
            result.arguments.prepend(stool->getRunParameters().at(i));
        }
        foreach (const QString& param, tool->getToolRunnerAdditionalOptions()) {
            result.arguments.prepend(param);
        }
        result.program = stool->getPath();
    }

    QString pathVariableSeparator = isOsWindows() ? ";" : ":";
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
    QString commandWithArguments = GUrlUtils::getQuotedString(result.program) + ExternalToolSupportUtils::prepareArgumentsForCmdLine(result.arguments);
    algoLog.details(tr("Launching %1 tool: %2").arg(toolName).arg(commandWithArguments));

    if (listener != nullptr) {
        listener->setToolName(toolName);
        listener->addNewLogMessage(commandWithArguments, ExternalToolListener::PROGRAM_WITH_ARGUMENTS);
    }
    return result;
}

QString ExternalToolSupportUtils::prepareArgumentsForCmdLine(const QStringList& arguments) {
    QString argumentsLine;
    foreach (QString argumentStr, arguments) {
        // Find start of the parameter value
        int startIndex = argumentStr.indexOf('=') + 1;
        // Add quotes if parameter contains whitespace characters
        QString valueStr = argumentStr.mid(startIndex);
        if (valueStr.contains(' ') || valueStr.contains('\t')) {
            argumentStr.append('"');
            argumentStr.insert(startIndex, '"');
        }
        argumentsLine += ' ' + argumentStr;
    }
    return argumentsLine;
}

QStringList ExternalToolSupportUtils::splitCmdLineArguments(const QString& program) {
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

bool checkHasNonLatin1Symbols(const QString& str) {
    QByteArray tolatin1(str.toLatin1());
    return QString::fromLatin1(tolatin1.constData(), tolatin1.size()) != str;
}

QString ExternalToolSupportUtils::checkArgumentPathLatinSymbols(const QStringList& args) {
    for (const QString& path : qAsConst(args)) {
        if (!path.isEmpty() && checkHasNonLatin1Symbols(path)) {
            return tr("One of the arguments passed to \"%1\" external tool is not in Latin alphabet."
                      " Make sure that the input and output files and folders"
                      " are located in the paths which contain only Latin characters. Current problem path is: \"") +
                   path + "\"";
        }
    }
    return "";
}

QString ExternalToolSupportUtils::checkTemporaryDirLatinSymbols() {
    QString path = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath();
    if (checkHasNonLatin1Symbols(path)) {
        return tr("Your \"Temporary files\" directory contains non-latin symbols, \"%1\" external tool can't correct process it."
                  " Please change it in Preferences on the Directories page, restart UGENE and try again. Current problem path is: ") +
               path;
    }
    return "";
}

QString ExternalToolSupportUtils::checkToolPathLatinSymbols(const ExternalTool* tool) {
    const QString& path = tool->getPath();
    QByteArray tolatin1(path.toLatin1());
    if (QString::fromLatin1(tolatin1.constData(), tolatin1.size()) != path) {
        return tr("\"%1\" external tool located in path which contains non-latin symbols."
                  " Please change it location to path which contains only latin symbols, set the new path in"
                  " Preferences on the External tools and try again. Current problem path is: ") +
               path;
    }
    return "";
}

QString ExternalToolSupportUtils::checkIndexDirLatinSymbols() {
    QString path = AppContext::getSettings()->getValue(SETTINGS_ROOT + INDEX_DIR, "", true).toString();
    if (checkHasNonLatin1Symbols(path)) {
        return tr("Your \"Build indexes\" directory contains non-latin symbols, \"%1\" external tool can't correct process it."
                  " Please change it in Preferences on the Directories page, restart UGENE and try again. Current problem path is: ") +
               path;
    }
    return "";
}

QString ExternalToolSupportUtils::checkSpacesArgumentsLikeMakeblastdb(const QStringList& args) {
    QStringList agrsNoJoinedLines;
    for (const QString& arg : qAsConst(args)) {
        if (arg.startsWith("\"") && arg.endsWith("\"")) {
            QStringList argsSplited = arg.mid(1, arg.size() - 2).split("\" \"");
            for (const QString& splittedArg : qAsConst(argsSplited)) {
                if (splittedArg.isEmpty()) {
                    return tr("One of the arguments passed to \"%1\" external tool contains empty arguments.");
                } else if (splittedArg.contains("\"") && splittedArg.contains("'") && splittedArg.contains("`")) {
                    return tr("One of the arguments passed to \"%1\" external tool contains unexpected quotes. Current problem argument is: ") + splittedArg;
                } else {
                    agrsNoJoinedLines << splittedArg;
                }
            }
        } else {
            agrsNoJoinedLines << arg;
        }
    }
    return checkArgumentPathSpaces(agrsNoJoinedLines);
}

QString ExternalToolSupportUtils::checkArgumentPathSpaces(const QStringList& args) {
    for (const QString& path : qAsConst(args)) {
        if (path.contains(" ")) {
            return tr("One of the arguments passed to \"%1\" external tool contains spaces."
                      " Make sure that the input and output files and folders"
                      " are located in the paths which contain no spaces. Current problem path is: ") +
                   path;
        }
    }
    return "";
}

QString ExternalToolSupportUtils::checkTemporaryDirSpaces() {
    QString path = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath();
    if (path.contains(" ")) {
        return tr("Your \"Temporary files\" directory contains spaces, \"%1\" external tool can't correct process it."
                  " Please change it in Preferences on the Directories page, restart UGENE and try again. Current problem path is: ") +
               path;
    }
    return "";
}

QString ExternalToolSupportUtils::checkToolPathSpaces(const ExternalTool* tool) {
    const QString& path = tool->getPath();
    if (path.contains(" ")) {
        return tr("\"%1\" external tool located in path which contains spaces symbols."
                  " Please change it location to path which contains no spaces,  set the new path in"
                  " Preferences on the External tools and try again. Current problem path is: ") +
               path;
    }
    return "";
}

ExternalToolLogProcessor::~ExternalToolLogProcessor() {
}

ExternalToolListener::ExternalToolListener(ExternalToolLogProcessor* logProcessor)
    : logProcessor(logProcessor) {
}

ExternalToolListener::~ExternalToolListener() {
    delete logProcessor;
}

void ExternalToolListener::setToolName(const QString& _toolName) {
    toolName = _toolName;
}

void ExternalToolListener::setLogProcessor(ExternalToolLogProcessor* newLogProcessor) {
    delete logProcessor;
    logProcessor = newLogProcessor;
}

const QString& ExternalToolListener::getToolName() const {
    return toolName;
}

}  // namespace U2
