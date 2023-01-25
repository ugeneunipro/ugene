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

#include "ExternalToolSearchTask.h"

#include <QCoreApplication>
#include <QDir>
#include <QProcessEnvironment>

#include <U2Core/AppContext.h>
#include <U2Core/BundleInfo.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/U2SafePoints.h>

#include "ExternalToolSupportSettings.h"

namespace U2 {

#define DEFAULT_TOOLS_DIR_NAME "tools"

ExternalToolSearchTask::ExternalToolSearchTask(const QString& toolId)
    : Task(tr("'%1' external tool search task").arg(AppContext::getExternalToolRegistry()->getToolNameById(toolId)), TaskFlag_None),
      toolId(toolId) {
}

void ExternalToolSearchTask::run() {
    ExternalToolRegistry* etRegistry = AppContext::getExternalToolRegistry();
    SAFE_POINT(etRegistry, "External tool registry is NULL", );

    ExternalTool* tool = etRegistry->getById(toolId);
    CHECK_EXT(tool, setError(tr("External tool '%1' is not registered").arg(toolId)), );

    // Search for pre-installed external tools first.
    QDir appDir(QCoreApplication::applicationDirPath());
    QString toolsDir = qgetenv("UGENE_TOOLS_DIR");

    if (toolsDir.isEmpty() && QFileInfo(appDir.absoluteFilePath(DEFAULT_TOOLS_DIR_NAME)).isDir()) {
        toolsDir = appDir.absoluteFilePath(DEFAULT_TOOLS_DIR_NAME);
#ifdef Q_OS_DARWIN
    } else if (toolsDir.isEmpty()) {
        QString dir = BundleInfo::getToolsSearchPath();
        if (!dir.isEmpty()) {
            toolsDir = dir;
        }
#endif
    }

    if (!toolsDir.isEmpty()) {
        QString executableFileName = getExecutableFileName(tool);
        CHECK(!executableFileName.isEmpty(), );
        QString searchDir = toolsDir;
        if (!tool->getDirName().isEmpty()) {
            searchDir += "/" + tool->getDirName();
        }
        LimitedDirIterator it(searchDir);
        while (it.hasNext()) {
            it.next();
            QString executableFilePath = it.filePath() + "/" + executableFileName;
            if (QFileInfo(executableFilePath).isFile()) {
                toolPaths << QDir::toNativeSeparators(executableFilePath);
                break;
            }
        }
        if (!toolPaths.isEmpty()) {
            return;
        }
    }

    // If there is no external tools dir search to for tools in PATH variable.
    QStringList envList = QProcessEnvironment::systemEnvironment().toStringList();
    int pathIndex = envList.indexOf(QRegExp("PATH=.*", Qt::CaseInsensitive));

    if (pathIndex >= 0) {
        QString pathEnv = envList.at(pathIndex);
#if defined(Q_OS_UNIX)
        QStringList paths = pathEnv.split("=").at(1).split(":");
#else
#    ifdef Q_OS_WIN
        QStringList paths = pathEnv.split("=").at(1).split(";");
#    else
        Q_UNUSED(pathEnv);
        QStringList paths;
#    endif
#endif
        QString exeName = getExecutableFileName(tool);
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

QString ExternalToolSearchTask::getExecutableFileName(ExternalTool* tool) {
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

ExternalToolsSearchTask::ExternalToolsSearchTask(const QList<Task*>& _tasks)
    : SequentialMultiTask(tr("Searching external tools"), _tasks, TaskFlags(TaskFlag_NoRun | TaskFlag_CancelOnSubtaskCancel)) {
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

}  // namespace U2
