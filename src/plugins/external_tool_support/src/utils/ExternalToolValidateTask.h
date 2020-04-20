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

#ifndef _U2_EXTERNAL_TOOL_VALIDATE_TASK_H
#define _U2_EXTERNAL_TOOL_VALIDATE_TASK_H

#include <U2Core/Task.h>
#include <U2Core/MultiTask.h>
#include <QProcess>

namespace U2 {

class ExternalTool;
class ExternalToolLogParser;
class ExternalToolValidation;
class ExternalToolSearchTask;

class ExternalToolValidateTask : public Task {
    Q_OBJECT
public:
    ExternalToolValidateTask(const QString& toolId, const QString& toolName, TaskFlags flags = TaskFlag_None);
    virtual ~ExternalToolValidateTask() {}

    virtual Task::ReportResult report() = 0;

    bool isValidTool()  { return isValid; }
    QString getToolId()  { return toolId; }
    QString getToolName() { return toolName; }
    QString getToolPath()  { return toolPath; }
    QString getToolVersion()  { return version; }

protected:
    QString toolId;
    QString toolName;
    QString toolPath;
    QString version;
    bool isValid;
};

class ExternalToolJustValidateTask: public ExternalToolValidateTask {
    Q_OBJECT
    Q_DISABLE_COPY(ExternalToolJustValidateTask)
public:
    ExternalToolJustValidateTask(const QString& toolId, const QString& toolName, const QString& path);
    virtual ~ExternalToolJustValidateTask();

    virtual void run();
    virtual Task::ReportResult report();

    void cancelProcess();

private:
    void setEnvironment(ExternalTool *tool);
    bool parseLog(const ExternalToolValidation& validation);
    void checkVersion(const QString& partOfLog);
    void checkArchitecture(const QString &toolPath);
    void performAdditionalChecks();

    QString     errorMsg;

    QList<ExternalToolValidation> validations; //original tool validation is the last one

    QRegExp     checkVersionRegExp;

    QString     lastErrLine;
    QString     lastOutLine;

    QProcess*   externalToolProcess;

    ExternalTool* tool;
    static const int CHECK_PERIOD_MS = 1000;
    static const int TIMEOUT_MS = 30000;
};

class ExternalToolSearchAndValidateTask : public ExternalToolValidateTask {
    Q_OBJECT
public:
    ExternalToolSearchAndValidateTask(const QString& toolId, const QString& toolName);

    void prepare();
    virtual QList<Task*> onSubTaskFinished(Task *subTask);
    virtual Task::ReportResult report();

private:
    QStringList toolPaths;
    QString     errorMsg;
    bool        toolIsFound;
    ExternalToolSearchTask*     searchTask;
    ExternalToolJustValidateTask*   validateTask;
};

class ExternalToolsValidateTask : public SequentialMultiTask {
    Q_OBJECT
public:
    ExternalToolsValidateTask(const QList<Task*> &_tasks);

    virtual QList<Task*> onSubTaskFinished(Task* subTask);
};

class ExternalToolsInstallTask : public SequentialMultiTask {
    Q_OBJECT
public:
    ExternalToolsInstallTask(const QList<Task*> &_tasks);

    virtual QList<Task*> onSubTaskFinished(Task* subTask);
};

}   // namespace

#endif // _U2_EXTERNAL_TOOL_VALIDATE_TASK_H
