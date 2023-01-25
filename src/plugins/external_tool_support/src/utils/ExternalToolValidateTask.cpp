/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "ExternalToolValidateTask.h"

#include <QString>

#include <U2Core/AppContext.h>
#include <U2Core/CmdlineTaskRunner.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/Log.h>
#include <U2Core/ScriptingToolRegistry.h>
#include <U2Core/Timer.h>
#include <U2Core/U2SafePoints.h>

#ifdef Q_OS_DARWIN
#    include <U2Core/AppResources.h>
#endif

#include <U2Lang/WorkflowUtils.h>

#include "ExternalToolSearchTask.h"
#include "ExternalToolSupportSettings.h"

namespace U2 {

ExternalToolValidateTask::ExternalToolValidateTask(const QString& toolId, const QString& toolName, TaskFlags flags)
    : Task(tr("%1 validate task").arg(toolName), flags),
      toolId(toolId),
      toolName(toolName),
      isValid(false) {
}

ExternalToolJustValidateTask::ExternalToolJustValidateTask(const QString& toolId, const QString& toolName, const QString& path)
    : ExternalToolValidateTask(toolId, toolName, TaskFlag_None),
      externalToolProcess(nullptr),
      tool(nullptr) {
    toolPath = path;
    SAFE_POINT_EXT(!toolPath.isEmpty(), setError(tr("Tool's path is empty")), );

    ExternalToolRegistry* etRegistry = AppContext::getExternalToolRegistry();
    SAFE_POINT(etRegistry, "An external tool registry is NULL", );
    tool = etRegistry->getById(toolId);
    SAFE_POINT(tool, QString("External tool '%1' isn't found in the registry").arg(toolName), );
    QFileInfo toolPathInfo(toolPath);
    CHECK_EXT(toolPathInfo.exists(), setError(tr("External tool is not found: %1").arg(toolPath)), );

    bool isPathOnlyValidation = qgetenv("UGENE_EXTERNAL_TOOLS_VALIDATION_BY_PATH_ONLY") == "1";
    if (isPathOnlyValidation) {
        isValid = toolPathInfo.isFile();
        coreLog.info("Using path only validation for: " + toolName + ", path: " + toolPath);
        setFlag(U2::TaskFlag_NoRun, true);
    }
}

ExternalToolJustValidateTask::~ExternalToolJustValidateTask() {
    delete externalToolProcess;
    externalToolProcess = nullptr;
}

void ExternalToolJustValidateTask::run() {
    ExternalToolRegistry* etRegistry = AppContext::getExternalToolRegistry();
    SAFE_POINT(etRegistry, "An external tool registry is NULL", );
    tool = etRegistry->getById(toolId);
    SAFE_POINT(tool, QString("External tool '%1' isn't found in the registry").arg(toolName), );

    validations.append(tool->getToolAdditionalValidations());
    ExternalToolValidation originalValidation = tool->getToolValidation();
    originalValidation.executableFile = toolPath;

    if (!originalValidation.toolRunnerProgram.isEmpty()) {
        ScriptingToolRegistry* stRegistry = AppContext::getScriptingToolRegistry();
        SAFE_POINT_EXT(nullptr != stRegistry, setError(tr("Scripting tool registry is NULL")), );
        ScriptingTool* stool = stRegistry->getById(originalValidation.toolRunnerProgram);
        CHECK_EXT(nullptr != stool, setError(tr("Scripting tool '%1' isn't found in the registry").arg(originalValidation.toolRunnerProgram)), );

        if (stool->getPath().isEmpty()) {
            stateInfo.setError(QString("The tool %1 that runs %2 is not installed. "
                                       "Please set the path to the executable file of the"
                                       " tool in the External Tools settings. Some of the tools "
                                       "may be located in UGENE/Tools folder")
                                   .arg(originalValidation.toolRunnerProgram)
                                   .arg(toolName));
        } else {
            originalValidation.arguments.prepend(originalValidation.executableFile);
            for (int i = stool->getRunParameters().size() - 1; i >= 0; i--) {
                originalValidation.arguments.prepend(stool->getRunParameters().at(i));
            }
            originalValidation.executableFile = stool->getPath();
        }
    }

    validations.append(originalValidation);
    coreLog.trace("Creating validation task for: " + toolName);
    checkVersionRegExp = tool->getVersionRegExp();
    version = "unknown";

    coreLog.trace("Program executable: " + toolPath);
    SAFE_POINT(!validations.isEmpty(), "Tools' validations list is empty", );
    coreLog.trace("Program arguments: " + validations.last().arguments.join(" "));

    CHECK(!hasError(), );

    for (const ExternalToolValidation& validation : qAsConst(validations)) {
        if (externalToolProcess != nullptr) {
            delete externalToolProcess;
            externalToolProcess = nullptr;
        }

        checkArchitecture(validation.executableFile);
        CHECK_OP(stateInfo, );

        externalToolProcess = new QProcess();
        setEnvironment(tool);

        externalToolProcess->start(validation.executableFile, validation.arguments);
        bool started = externalToolProcess->waitForStarted(3000);

        if (!started) {
            errorMsg = validation.possibleErrorsDescr.value(ExternalToolValidation::DEFAULT_DESCR_KEY, "");
            if (!errorMsg.isEmpty()) {
                stateInfo.setError(errorMsg);
            } else {
                stateInfo.setError(tr("Tool does not start.<br>"
                                      "It is possible that the specified executable file "
                                      "<i>%1</i> for %2 tool is invalid. You can change "
                                      "the path to the executable file in the external "
                                      "tool settings in the global preferences.")
                                       .arg(toolPath)
                                       .arg(toolName));
            }
            isValid = false;
            return;
        }

        int elapsedTime = 0;
        const int timeout = tool->isCustom() ? 0 : TIMEOUT_MS;  // Custom tools can't be validated properly now, there is no need to wait them
        while (!externalToolProcess->waitForFinished(CHECK_PERIOD_MS)) {
            elapsedTime += CHECK_PERIOD_MS;
            if (isCanceled() || elapsedTime >= timeout) {
                cancelProcess();
            }
        }

        CHECK(parseLog(validation), );
        CHECK(isValid, );
    }

    performAdditionalChecks();
    CHECK_OP(stateInfo, );
}

Task::ReportResult ExternalToolJustValidateTask::report() {
    if (!isValid && !stateInfo.hasError() && !toolPath.isEmpty()) {
        if (errorMsg.isEmpty()) {
            stateInfo.setError(tr("Can not find expected message.<br>"
                                  "It is possible that the specified "
                                  "executable file <i>%1</i> for %2 tool "
                                  "is invalid. You can change the path to "
                                  "the executable file in the external tool "
                                  "settings in the global preferences.")
                                   .arg(toolPath)
                                   .arg(toolName));
        } else {
            stateInfo.setError(errorMsg);
        }
    }

    return ReportResult_Finished;
}

void ExternalToolJustValidateTask::cancelProcess() {
    CmdlineTaskRunner::killProcessTree(externalToolProcess);
}

void ExternalToolJustValidateTask::setEnvironment(ExternalTool* externalTool) {
    QStringList additionalPaths;
    for (const QString& toolId : qAsConst(externalTool->getDependencies())) {
        ExternalTool* masterTool = AppContext::getExternalToolRegistry()->getById(toolId);
        if (nullptr != masterTool) {
            additionalPaths << QFileInfo(masterTool->getPath()).dir().absolutePath();
        }
    }

#ifdef Q_OS_WIN
    const QString pathVariableSeparator = ";";
#else
    const QString pathVariableSeparator = ":";
#endif

    QProcessEnvironment processEnvironment = QProcessEnvironment::systemEnvironment();
    const QString path = additionalPaths.join(pathVariableSeparator) + pathVariableSeparator + processEnvironment.value("PATH");
    if (!additionalPaths.isEmpty()) {
        algoLog.trace(QString("PATH environment variable: '%1'").arg(path));
    }
    processEnvironment.insert("PATH", path);

    externalToolProcess->setProcessEnvironment(processEnvironment);
}

bool ExternalToolJustValidateTask::parseLog(const ExternalToolValidation& validation) {
    errorMsg = validation.possibleErrorsDescr.value(ExternalToolValidation::DEFAULT_DESCR_KEY, "");

    QString errLog = QString(externalToolProcess->readAllStandardError());
    if (!errLog.isEmpty()) {
        if (errLog.contains(QRegExp(validation.expectedMsg))) {
            isValid = true;
            checkVersion(errLog);
            tool->extractAdditionalParameters(errLog);
        } else {
            isValid = false;
            foreach (const QString& errStr, validation.possibleErrorsDescr.keys()) {
                if (errLog.contains(errStr)) {
                    errorMsg = validation.possibleErrorsDescr[errStr];
                    return false;
                }
            }
        }
    }

    QString log = QString(externalToolProcess->readAllStandardOutput());
    if (!log.isEmpty()) {
        if (log.contains(QRegExp(validation.expectedMsg))) {
            isValid = true;
            checkVersion(log);
            tool->extractAdditionalParameters(log);
        } else {
            isValid = false;
            foreach (const QString& errStr, validation.possibleErrorsDescr.keys()) {
                if (log.contains(errStr)) {
                    errorMsg = validation.possibleErrorsDescr[errStr];
                    return false;
                }
            }
        }
    }

    if (errLog.isEmpty() && log.isEmpty() && validation.expectedMsg.isEmpty()) {
        isValid = true;
    }

    return true;
}

void ExternalToolJustValidateTask::checkVersion(const QString& partOfLog) {
    if (checkVersionRegExp.isEmpty()) {
        version = tool->getPredefinedVersion();
    } else {
        QStringList lastPartOfLog = partOfLog.split(QRegExp("(\n|\r)"));
        foreach (QString buf, lastPartOfLog) {
            if (buf.contains(checkVersionRegExp)) {
                checkVersionRegExp.indexIn(buf);
                version = checkVersionRegExp.cap(1);
                return;
            }
        }
    }
}

void ExternalToolJustValidateTask::checkArchitecture(const QString& toolPath) {
    Q_UNUSED(toolPath);
#ifdef Q_OS_DARWIN
    QProcess archProcess;
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    archProcess.setProcessEnvironment(env);
    archProcess.start("file", QStringList() << toolPath);

    bool started = archProcess.waitForStarted(3000);
    CHECK(started, );

    int timeout = 10000;
    int period = 1000;
    int timer = 0;
    while (!archProcess.waitForFinished(period)) {
        timer += period;
        CHECK(timer <= timeout, );

        if (isCanceled()) {
            cancelProcess();
        }
    }

    const QString output = archProcess.readAllStandardOutput();
    archProcess.close();

    bool is_ppc = output.contains("Mach-O executable ppc");
    bool is_i386 = output.contains("Mach-O executable i386");
    bool is_x86_64 = output.contains("Mach-O 64-bit executable x86_64");
    if (is_ppc && !is_i386 && !is_x86_64) {
        setError("This external tool has unsupported architecture");
    }
#endif
}

void ExternalToolJustValidateTask::performAdditionalChecks() {
    tool->performAdditionalChecks(toolPath);

    if (tool->hasAdditionalErrorMessage()) {
        isValid = false;
        stateInfo.setError(tool->getAdditionalErrorMessage());
    }
}

ExternalToolSearchAndValidateTask::ExternalToolSearchAndValidateTask(const QString& _toolId, const QString& _toolName)
    : ExternalToolValidateTask(_toolId, _toolName, TaskFlags(TaskFlag_CancelOnSubtaskCancel | TaskFlag_NoRun)),
      toolIsFound(false),
      searchTask(nullptr),
      validateTask(nullptr) {
}

void ExternalToolSearchAndValidateTask::prepare() {
    searchTask = new ExternalToolSearchTask(toolId);
    addSubTask(searchTask);
}

QList<Task*> ExternalToolSearchAndValidateTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> subTasks;
    CHECK(!subTask->isCanceled(), subTasks);

    if (subTask == searchTask) {
        CHECK(!searchTask->hasError(), subTasks);
        toolPaths = searchTask->getPaths();
        if (toolPaths.isEmpty()) {
            isValid = false;
            toolPath = "";
            toolIsFound = false;
        } else {
            toolIsFound = true;
            validateTask = new ExternalToolJustValidateTask(toolId, toolName, toolPaths.first());
            if (validateTask->isValidTool()) {  // in-place path-only validation. Used in GUI tests.
                isValid = true;
                toolPath = validateTask->getToolPath();
                version = validateTask->getToolVersion();
                delete validateTask;
                validateTask = nullptr;
                return subTasks;
            } else {
                subTasks << validateTask;
            }
        }
    } else if (subTask == validateTask) {
        if (validateTask->isValidTool()) {
            isValid = true;
            toolPath = validateTask->getToolPath();
            version = validateTask->getToolVersion();
        } else {
            errorMsg = validateTask->getError();
            toolPath = validateTask->getToolPath();
            SAFE_POINT(!toolPaths.isEmpty(), "Tool path's list is empty", subTasks);
            toolPaths.removeFirst();

            if (!toolPaths.isEmpty()) {
                validateTask = new ExternalToolJustValidateTask(toolId, toolName, toolPaths.first());
                subTasks << validateTask;
            }
        }
    }

    return subTasks;
}

Task::ReportResult ExternalToolSearchAndValidateTask::report() {
    if (qgetenv("UGENE_GUI_TEST") == "1") {  // dump external tool validation time in GUI tests mode.
        qint64 taskRunMillis = (GTimer::currentTimeMicros() - timeInfo.startTime) / 1000;
        coreLog.trace(QString("ExternalToolSearchAndValidateTask[%1] time: %2 millis").arg(toolId).arg(taskRunMillis));
    }
    ExternalToolRegistry* etRegistry = AppContext::getExternalToolRegistry();
    SAFE_POINT(etRegistry, "An external tool registry is NULL", ReportResult_Finished);
    ExternalTool* tool = etRegistry->getById(toolId);
    SAFE_POINT(tool, QString("An external tool '%1' isn't found in the registry").arg(toolName), ReportResult_Finished);

    if (!isValid && toolIsFound && !toolPath.isEmpty()) {
        if (errorMsg.isEmpty()) {
            setError(tr("Can not find expected message."
                        "<br>It is possible that the specified executable file "
                        "<i>%1</i> for %2 tool is invalid. "
                        "You can change the path to the executable file "
                        "in the external tool settings in the global "
                        "preferences.")
                         .arg(toolPath)
                         .arg(toolName));
        } else {
            setError(errorMsg);
        }
    }

    return ReportResult_Finished;
}

ExternalToolsValidationMasterTask::ExternalToolsValidationMasterTask(const QList<Task*>& tasks, ExternalToolValidationListener* listener)
    : SequentialMultiTask(tr("Validate external tools"),
                          tasks,
                          TaskFlags(TaskFlag_NoRun | TaskFlag_CancelOnSubtaskCancel)),
      listener(listener) {
    setMaxParallelSubtasks(5);
}

QList<Task*> ExternalToolsValidationMasterTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> subTasks;
    ExternalToolValidateTask* validateTask = qobject_cast<ExternalToolValidateTask*>(subTask);
    bool muted = false;

    if (validateTask) {
        ExternalToolRegistry* etRegistry = AppContext::getExternalToolRegistry();
        SAFE_POINT(etRegistry, "An external tool registry is NULL", subTasks);
        ExternalTool* tool = etRegistry->getById(validateTask->getToolId());
        SAFE_POINT(tool, QString("An external tool '%1' isn't found in the registry").arg(validateTask->getToolName()), subTasks);
        muted = tool->isMuted();
    }

    if (subTask->hasError()) {
        if (muted) {
            taskLog.details(subTask->getTaskName() + tr(" failed: ") + subTask->getError());
        } else {
            taskLog.error(subTask->getTaskName() + tr(" failed: ") + subTask->getError());
        }
    }

    subTasks = SequentialMultiTask::onSubTaskFinished(subTask);
    return subTasks;
}

Task::ReportResult ExternalToolsValidationMasterTask::report() {
    if (listener != nullptr) {
        for (const QPointer<Task>& subTask : qAsConst(getSubtasks())) {
            ExternalToolValidateTask* task = qobject_cast<ExternalToolValidateTask*>(subTask.data());
            SAFE_POINT(task, "Unexpected ExternalToolValidateTask subtask", ReportResult_Finished);
            listener->setToolState(task->getToolId(), task->isValidTool());
        }
        listener->validationFinished();
    }
    return ReportResult_Finished;
}

ExternalToolsInstallTask::ExternalToolsInstallTask(const QList<Task*>& _tasks)
    : SequentialMultiTask(tr("Installing external tools"), _tasks, TaskFlags(TaskFlag_NoRun | TaskFlag_CancelOnSubtaskCancel)) {
}

QList<Task*> ExternalToolsInstallTask::onSubTaskFinished(Task* subTask) {
    if (subTask->hasError()) {
        taskLog.error(subTask->getTaskName() + tr(" failed: ") + subTask->getError());
    }
    return SequentialMultiTask::onSubTaskFinished(subTask);
}

}  // namespace U2
