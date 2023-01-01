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

#ifndef _U2_EXTERNAL_TOOL_VALIDATE_TASK_H
#define _U2_EXTERNAL_TOOL_VALIDATE_TASK_H

#include <QProcess>

#include <U2Core/MultiTask.h>
#include <U2Core/Task.h>

namespace U2 {

class ExternalTool;
class ExternalToolLogParser;
class ExternalToolValidation;
class ExternalToolSearchTask;
class ExternalToolValidationListener;

class ExternalToolValidateTask : public Task {
    Q_OBJECT
public:
    ExternalToolValidateTask(const QString& toolId, const QString& toolName, TaskFlags flags = TaskFlag_None);
    virtual ~ExternalToolValidateTask() {
    }

    virtual Task::ReportResult report() = 0;

    bool isValidTool() const {
        return isValid;
    }
    const QString& getToolId() const {
        return toolId;
    }
    const QString& getToolName() const {
        return toolName;
    }
    const QString& getToolPath() const {
        return toolPath;
    }
    const QString& getToolVersion() const {
        return version;
    }

protected:
    QString toolId;
    QString toolName;
    QString toolPath;
    QString version;
    bool isValid;
};

class ExternalToolJustValidateTask : public ExternalToolValidateTask {
    Q_OBJECT
    Q_DISABLE_COPY(ExternalToolJustValidateTask)
public:
    ExternalToolJustValidateTask(const QString& toolId, const QString& toolName, const QString& path);
    virtual ~ExternalToolJustValidateTask();

    virtual void run();
    virtual Task::ReportResult report();

    void cancelProcess();

private:
    void setEnvironment(ExternalTool* externalTool);
    bool parseLog(const ExternalToolValidation& validation);
    void checkVersion(const QString& partOfLog);
    void checkArchitecture(const QString& toolPath);
    void performAdditionalChecks();

    QString errorMsg;

    QList<ExternalToolValidation> validations;  // original tool validation is the last one

    QRegExp checkVersionRegExp;

    QString lastErrLine;
    QString lastOutLine;

    QProcess* externalToolProcess;

    ExternalTool* tool;

    static const int TIMEOUT_MS = 30000;
    static const int CHECK_PERIOD_MS = 1000;
};

class ExternalToolSearchAndValidateTask : public ExternalToolValidateTask {
    Q_OBJECT
public:
    ExternalToolSearchAndValidateTask(const QString& toolId, const QString& toolName);

    void prepare() override;
    QList<Task*> onSubTaskFinished(Task* subTask) override;
    Task::ReportResult report() override;

private:
    QStringList toolPaths;
    QString errorMsg;
    bool toolIsFound;
    ExternalToolSearchTask* searchTask;
    ExternalToolJustValidateTask* validateTask;
};

class ExternalToolsValidationMasterTask : public SequentialMultiTask {
    Q_OBJECT
public:
    ExternalToolsValidationMasterTask(const QList<Task*>& tasks, ExternalToolValidationListener* listener);

    QList<Task*> onSubTaskFinished(Task* subTask) override;

    ReportResult report() override;

private:
    ExternalToolValidationListener* listener;
};

class ExternalToolsInstallTask : public SequentialMultiTask {
    Q_OBJECT
public:
    ExternalToolsInstallTask(const QList<Task*>& _tasks);

    QList<Task*> onSubTaskFinished(Task* subTask) override;
};

}  // namespace U2

#endif  // _U2_EXTERNAL_TOOL_VALIDATE_TASK_H
