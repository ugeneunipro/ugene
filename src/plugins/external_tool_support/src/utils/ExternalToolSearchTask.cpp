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

#include <QCoreApplication>
#include <QDir>
#include <QProcessEnvironment>

#include <U2Core/AppContext.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/U2SafePoints.h>

#include "ExternalToolSearchTask.h"
#include "ExternalToolSupportSettings.h"

namespace U2 {

const QString ExternalToolSearchTask::TOOLS = "tools";

ExternalToolSearchTask::ExternalToolSearchTask(const QString& _toolId) :
    Task(tr("'%1' external tool search task").arg(AppContext::getExternalToolRegistry()->getToolNameById(_toolId)), TaskFlag_None),
    toolId(_toolId) {
}

void ExternalToolSearchTask::run() {
    ExternalToolRegistry* etRegistry = AppContext::getExternalToolRegistry();
    SAFE_POINT(etRegistry, "External tool registry is NULL", );
    ExternalTool* tool = etRegistry->getById(toolId);
    CHECK_EXT(tool, setError(tr("An external tool '%1' isn't found in the registry").arg(toolId)), );

    // 1. Search for the tool in the tools folder
    QDir appDir(QCoreApplication::applicationDirPath());
    QStringList entryList = appDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    QString toolsDir;

    foreach (const QString& dirName, entryList) {
        if (dirName == TOOLS) {
            toolsDir = appDir.absolutePath() + QDir::separator() + dirName;
            break;
        }
    }

    if (!toolsDir.isEmpty()) {
        QString exeName = getExeName(tool);
        CHECK(!exeName.isEmpty(), );
        bool fileNotFound = true;
        LimitedDirIterator it (toolsDir);

        while (it.hasNext() && fileNotFound) {
            it.next();
            QString toolPath(it.filePath() + QDir::separator() + exeName);
            QFileInfo info(toolPath);
            if (info.exists() && info.isFile()) {
                toolPaths << QDir::toNativeSeparators(toolPath);
                fileNotFound = false;
            }
        }
    }

    // 2. Search for the tool in the PATH variable
    QStringList envList = QProcessEnvironment::systemEnvironment().toStringList();
    int pathIndex = envList.indexOf(QRegExp("PATH=.*", Qt::CaseInsensitive));

    if (pathIndex >= 0) {
        QString pathEnv = envList.at(pathIndex);
#if defined(Q_OS_UNIX)
        QStringList paths = pathEnv.split("=").at(1).split(":");
#else
#ifdef Q_OS_WIN
        QStringList paths = pathEnv.split("=").at(1).split(";");
#else
        Q_UNUSED(pathEnv);
        QStringList paths;
#endif
#endif
        QString exeName = getExeName(tool);
        CHECK(!exeName.isEmpty(), );

        foreach (const QString& curPath, paths) {
            QString exePath = curPath + QDir::separator() + exeName;
            QFileInfo info(exePath);
            if (info.exists()) {
                toolPaths << exePath;
            }
        }
    }

    // 3. Check current tool's path
    if (!tool->getPath().isEmpty()) {
        QFileInfo info(tool->getPath());
        if (info.exists()) {
            toolPaths << tool->getPath();
        }
    }
}

QString ExternalToolSearchTask::getExeName(ExternalTool* tool) {
    SAFE_POINT_EXT(tool, setError(tr("Tool pointer is NULL")), "");

    if (!tool->getExecutableFileName().isEmpty()) {
        return tool->getExecutableFileName();
    }

    if (tool->isModule()) {
        QStringList dependencies = tool->getDependencies();
        SAFE_POINT_EXT(!dependencies.isEmpty(), setError(tr("External tool module hasn't any dependencies: it hasn't master tool")), "");
        ExternalToolRegistry* etRegistry = AppContext::getExternalToolRegistry();
        SAFE_POINT_EXT(etRegistry, setError(tr("External tool registry is NULL")), "");
        ExternalTool* masterTool = etRegistry->getById(dependencies.first());
        SAFE_POINT_EXT(masterTool, setError(tr("An external tool '%1' isn't found in the registry").arg(dependencies.first())), "");
        return masterTool->getExecutableFileName();
    }

    setError(tr("Tool's executable name is not set"));
    return "";
}

ExternalToolsSearchTask::ExternalToolsSearchTask(const QList<Task*> &_tasks) :
    SequentialMultiTask(tr("Searching external tools"), _tasks, TaskFlags(TaskFlag_NoRun | TaskFlag_CancelOnSubtaskCancel)) {
}

QList<Task*> ExternalToolsSearchTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> subTasks;
    ExternalToolSearchTask* searchTask = qobject_cast<ExternalToolSearchTask*>(subTask);
    bool muted = false;

    if (searchTask) {
        ExternalToolRegistry* etRegistry = AppContext::getExternalToolRegistry();
        SAFE_POINT(etRegistry, "External tool registry is NULL", subTasks);
        ExternalTool* tool = etRegistry->getById(searchTask->getToolId());
        SAFE_POINT(tool, QString("An external tool '%1' isn't found in the registry").arg(searchTask->getToolId()), subTasks);
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

}   // namespace U2
