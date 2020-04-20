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

#include <QEventLoop>
#include <QSet>

#include <U2Core/AppContext.h>
#include <U2Core/CustomExternalTool.h>
#include <U2Core/MultiTask.h>
#include <U2Core/PluginModel.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2SafePoints.h>

#include "ExternalToolManager.h"
#include "ExternalToolSupportSettings.h"
#include "custom_tools/LoadCustomExternalToolsTask.h"
#include "utils/ExternalToolSearchTask.h"
#include "utils/ExternalToolValidateTask.h"

namespace U2 {

ExternalToolManagerImpl::ExternalToolManagerImpl() :
    startupChecks(true)
{
    etRegistry = AppContext::getExternalToolRegistry();
}

void ExternalToolManagerImpl::start() {
    if (nullptr != etRegistry && !startupChecks) {
        connect(etRegistry, SIGNAL(si_toolAdded(const QString &)), SLOT(sl_customToolImported(const QString &)));
        connect(etRegistry, SIGNAL(si_toolIsAboutToBeRemoved(const QString &)), SLOT(sl_customToolRemoved(const QString &)));
    }

    if (AppContext::getPluginSupport()->isAllPluginsLoaded()) {
        sl_pluginsLoaded();
    } else {
        connect(AppContext::getPluginSupport(),
            SIGNAL(si_allStartUpPluginsLoaded()),
            SLOT(sl_pluginsLoaded()));
    }
}

void ExternalToolManagerImpl::innerStart() {
    SAFE_POINT(etRegistry, "The external tool registry is NULL", );

    dependencies.clear();
    validateList.clear();
    searchList.clear();
    toolStates.clear();

    loadCustomTools();
}

void ExternalToolManagerImpl::checkStartupTasksState() {
    CHECK(startupChecks, );
    CHECK(!toolStates.values().contains(ValidationIsInProcess) && !toolStates.values().contains(SearchingIsInProcess), );
    startupChecks = false;
    ExternalToolSupportSettings::setExternalTools();

    connect(etRegistry, SIGNAL(si_toolAdded(const QString &)), SLOT(sl_customToolImported(const QString &)));
    connect(etRegistry, SIGNAL(si_toolIsAboutToBeRemoved(const QString &)), SLOT(sl_customToolRemoved(const QString &)));

    emit si_startupChecksFinish();
}

void ExternalToolManagerImpl::stop() {
    CHECK(etRegistry, );
    foreach(ExternalTool* tool, etRegistry->getAllEntries()) {
        disconnect(tool, NULL, this, NULL);
    }
    disconnect(etRegistry, SIGNAL(si_toolAdded(const QString &)), this, SLOT(sl_customToolImported(const QString &)));
    disconnect(etRegistry, SIGNAL(si_toolIsAboutToBeRemoved(const QString &)), this, SLOT(sl_customToolRemoved(const QString &)));
}

void ExternalToolManagerImpl::check(const QString& toolId, const QString& toolPath, ExternalToolValidationListener* listener) {
    StrStrMap toolPaths;
    toolPaths.insert(toolId, toolPath);
    check(QStringList() << toolId, toolPaths, listener);
}

void ExternalToolManagerImpl::check(const QStringList& toolIds, const StrStrMap& toolPaths, ExternalToolValidationListener* listener) {
    SAFE_POINT(etRegistry, "The external tool registry is NULL", );
    SAFE_POINT(listener, "Listener is NULL", );

    QList<Task*> taskList;

    foreach(const QString& toolId, toolIds) {
        QString toolPath = toolPaths.value(toolId);
        if (dependenciesAreOk(toolId) && !toolPath.isEmpty()) {
            ExternalToolValidateTask* task = new ExternalToolJustValidateTask(toolId, AppContext::getExternalToolRegistry()->getToolNameById(toolId), toolPath);
            taskList << task;
        } else {
            listener->setToolState(toolId, false);
        }
    }

    if (taskList.isEmpty()) {
        listener->validationFinished();
    } else {
        ExternalToolsValidateTask* validationTask = new ExternalToolsValidateTask(taskList);
        connect(validationTask, SIGNAL(si_stateChanged()), SLOT(sl_checkTaskStateChanged()));
        listeners.insert(validationTask, listener);
        validationTask->setMaxParallelSubtasks(MAX_PARALLEL_SUBTASKS);
        TaskScheduler* scheduler = AppContext::getTaskScheduler();
        SAFE_POINT(scheduler, "Task scheduler is NULL", );
        scheduler->registerTopLevelTask(validationTask);
    }
}

void ExternalToolManagerImpl::validate(const QString& toolId, ExternalToolValidationListener* listener) {
    validate(QStringList() << toolId, listener);
}

void ExternalToolManagerImpl::validate(const QString& toolId, const QString& path, ExternalToolValidationListener* listener) {
    StrStrMap toolPaths;
    toolPaths.insert(toolId, path);
    validate(QStringList() << toolId, toolPaths, listener);
}

void ExternalToolManagerImpl::validate(const QStringList& toolIds, ExternalToolValidationListener* listener) {
    validate(toolIds, StrStrMap(), listener);
}

void ExternalToolManagerImpl::validate(const QStringList& toolIds, const StrStrMap& toolPaths, ExternalToolValidationListener* listener) {
    SAFE_POINT(etRegistry, "The external tool registry is NULL", );

    foreach(const QString& toolId, toolIds) {
        etRegistry->getById(toolId)->setAdditionalErrorMessage(QString());
        if (dependenciesAreOk(toolId)) {
            validateList << toolId;
        } else {
            toolStates.insert(toolId, NotValidByDependency);
            if (toolPaths.contains(toolId)) {
                setToolPath(toolId, toolPaths.value(toolId));
            }
        }
    }

    if (listener && validateList.isEmpty()) {
        listener->validationFinished();
    }

    validateTools(toolPaths, listener);
}

bool ExternalToolManagerImpl::isValid(const QString& toolId) const {
    SAFE_POINT(etRegistry, "The external tool registry is NULL", false);

    return (Valid == toolStates.value(toolId, NotDefined));
}

ExternalToolManager::ExternalToolState ExternalToolManagerImpl::getToolState(const QString& toolId) const {
    return toolStates.value(toolId, NotDefined);
}

QString ExternalToolManagerImpl::addTool(ExternalTool* tool) {
    SAFE_POINT(etRegistry, "The external tool registry is NULL", "");
    QString toolPath;

    if (tool->isValid()) {
        toolStates.insert(tool->getId(), Valid);
    } else {
        toolStates.insert(tool->getId(), NotDefined);
    }

    connect(tool,
        SIGNAL(si_toolValidationStatusChanged(bool)),
        SLOT(sl_toolValidationStatusChanged(bool)));

    QStringList toolDependencies = tool->getDependencies();
    if (!toolDependencies.isEmpty()) {
        foreach(const QString& dependency, toolDependencies) {
            dependencies.insertMulti(dependency, tool->getId());
        }

        if (dependenciesAreOk(tool->getId()) && !tool->isValid()) {
            if (tool->isModule()) {
                QString masterId = tool->getDependencies().first();
                ExternalTool* masterTool = etRegistry->getById(masterId);
                SAFE_POINT(masterTool, QString("An external tool '%1' isn't found in the registry").arg(masterId), "");

                toolPath = masterTool->getPath();
            } else {
                toolPath = tool->getPath();
            }
            validateList << tool->getId();
        }
    } else {
        if (!tool->isValid()) {
            validateList << tool->getId();
            toolPath = tool->getPath();
        }
    }

    if (!validateList.contains(tool->getId()) && !tool->isModule() && !tool->isValid()) {
        searchList << tool->getId();
    }

    return toolPath;
}

void ExternalToolManagerImpl::sl_checkTaskStateChanged() {
    ExternalToolsValidateTask* masterTask = qobject_cast<ExternalToolsValidateTask*>(sender());
    SAFE_POINT(masterTask, "Unexpected task", );

    if (masterTask->isFinished()) {
        ExternalToolValidationListener* listener = listeners.value(masterTask, NULL);
        if (listener) {
            listeners.remove(masterTask);

            foreach(const QPointer<Task> &subTask, masterTask->getSubtasks()) {
                ExternalToolValidateTask* task = qobject_cast<ExternalToolValidateTask*>(subTask.data());
                SAFE_POINT(task, "Unexpected task", );

                listener->setToolState(task->getToolId(), task->isValidTool());
            }
            listener->validationFinished();
        }
    }
}

void ExternalToolManagerImpl::sl_validationTaskStateChanged() {
    SAFE_POINT(etRegistry, "The external tool registry is NULL", );
    ExternalToolValidateTask* task = qobject_cast<ExternalToolValidateTask*>(sender());
    SAFE_POINT(task, "Unexpected task", );

    if (task->isFinished()) {
        if (task->isValidTool()) {
            toolStates.insert(task->getToolId(), Valid);
        } else {
            toolStates.insert(task->getToolId(), NotValid);
        }

        ExternalTool* tool = etRegistry->getById(task->getToolId());
        SAFE_POINT(tool, QString("An external tool '%1' isn't found in the registry").arg(task->getToolName()), );
        if (tool->isModule()) {
            QStringList toolDependencies = tool->getDependencies();
            SAFE_POINT(!toolDependencies.isEmpty(), QString("Tool's dependencies list is unexpectedly empty: "
                "a master tool for the module '%1' is not defined").arg(tool->getId()), );
            QString masterId = toolDependencies.first();
            ExternalTool* masterTool = etRegistry->getById(masterId);
            SAFE_POINT(tool, QString("An external tool '%1' isn't found in the registry").arg(masterId), );
            SAFE_POINT(masterTool->getPath() == task->getToolPath(), "Module tool should have the same path as it's master tool", );
        }

        tool->setVersion(task->getToolVersion());
        tool->setPath(task->getToolPath());
        tool->setValid(task->isValidTool());

        searchTools();
        ExternalToolSupportSettings::setExternalTools();
    }

    checkStartupTasksState();
}

void ExternalToolManagerImpl::sl_searchTaskStateChanged() {
    SAFE_POINT(etRegistry, "The external tool registry is NULL", );
    ExternalToolSearchTask* task = qobject_cast<ExternalToolSearchTask*>(sender());
    SAFE_POINT(task, "Unexpected task", );

    if (task->isFinished()) {
        QStringList toolPaths = task->getPaths();
        if (!toolPaths.isEmpty()) {
            setToolPath(task->getToolId(), toolPaths.first());
            toolStates.insert(task->getToolId(), dependenciesAreOk(task->getToolId()) ? NotValidByDependency : NotValid);
        } else {
            toolStates.insert(task->getToolId(), NotValid);
        }
    }

    checkStartupTasksState();
}

void ExternalToolManagerImpl::sl_toolValidationStatusChanged(bool isValid) {
    SAFE_POINT(etRegistry, "The external tool registry is NULL", );
    ExternalTool* tool = qobject_cast<ExternalTool*>(sender());
    SAFE_POINT(tool, "Unexpected message sender", );

    if (isValid) {
        toolStates.insert(tool->getId(), Valid);
    } else {
        toolStates.insert(tool->getId(), NotValid);
    }

    StrStrMap toolPaths;
    foreach(const QString& vassalId, dependencies.values(tool->getId())) {
        ExternalTool* vassalTool = etRegistry->getById(vassalId);
        SAFE_POINT(vassalTool, QString("An external tool '%1' isn't found in the registry").arg(vassalId), );

        if (vassalTool->isModule()) {
            toolPaths.insert(vassalId, tool->getPath());
            setToolPath(vassalId, tool->getPath());
        }

        if (isValid &&
            dependenciesAreOk(vassalId) &&
            ValidationIsInProcess != toolStates.value(vassalId, NotDefined)) {
            validateList << vassalId;
            searchList.removeAll(vassalId);
        } else if (ValidationIsInProcess != toolStates.value(vassalId, NotDefined)) {
            vassalTool->setValid(false);
            toolStates.insert(vassalId, NotValidByDependency);
        }
    }

    validateTools(toolPaths);
}

void ExternalToolManagerImpl::sl_pluginsLoaded() {
    innerStart();
}

void ExternalToolManagerImpl::sl_customToolsLoaded(Task *task) {
    LoadCustomExternalToolsTask *loadTask = qobject_cast<LoadCustomExternalToolsTask *>(task);
    SAFE_POINT(nullptr != loadTask, "Unexpected task, can't cast it to LoadCustomExternalToolsTask *", );

    ExternalToolSupportSettings::loadExternalTools();

    QList<ExternalTool*> toolsList = etRegistry->getAllEntries();
    StrStrMap toolPaths;
    foreach(ExternalTool * tool, toolsList) {
        SAFE_POINT(tool, "Tool is NULL", );
        QString toolPath = addTool(tool);
        if (!toolPath.isEmpty()) {
            toolPaths.insert(tool->getId(), toolPath);
        }
    }

    validateTools(toolPaths);
}

void ExternalToolManagerImpl::sl_customToolImported(const QString &toolId) {
    SAFE_POINT(nullptr != etRegistry, "The external tool registry is nullptr", );
    ExternalTool *tool = etRegistry->getById(toolId);
    SAFE_POINT(nullptr != tool, "Tool is nullptr", );

    StrStrMap toolPaths;
    const QString toolPath = addTool(tool);
    if (!toolPath.isEmpty()) {
        toolPaths.insert(tool->getId(), toolPath);
    }

    validateTools(toolPaths);
}

void ExternalToolManagerImpl::sl_customToolRemoved(const QString &toolId) {
    toolStates.remove(toolId);
    QMutableMapIterator<QString, QString> iterator(dependencies);
    while (iterator.hasNext()) {
        auto item = iterator.next();
        if (toolId == item.key() || toolId == item.value()) {
            iterator.remove();
        }
    }
}

bool ExternalToolManagerImpl::dependenciesAreOk(const QString& toolId) {
    bool result = true;
    QStringList dependencyList = dependencies.keys(toolId);
    foreach(const QString& masterId, dependencyList) {
        CHECK_OPERATIONS(toolStates.keys().contains(masterId),
                         coreLog.details(tr("A dependency tool isn't represented in the general tool list. Skip dependency \"%1\"").arg(masterId)),
                         continue);

        result &= (Valid == toolStates.value(masterId, NotDefined));
    }
    return result;
}

void ExternalToolManagerImpl::validateTools(const StrStrMap& toolPaths, ExternalToolValidationListener* listener) {
    QList<Task*> taskList;
    foreach(QString toolId, validateList) {
        validateList.removeAll(toolId);
        toolStates.insert(toolId, ValidationIsInProcess);

        QString toolPath;
        bool pathSpecified = toolPaths.contains(toolId);
        if (pathSpecified) {
            toolPath = toolPaths.value(toolId);
            if (toolPath.isEmpty()) {
                toolStates.insert(toolId, NotValid);
                setToolPath(toolId, toolPath);
                if (listener) {
                    listener->setToolState(toolId, false);
                }
                setToolValid(toolId, false);
                continue;
            }
        }

        ExternalToolValidateTask* task;
        if (pathSpecified) {
            task = new ExternalToolJustValidateTask(toolId, AppContext::getExternalToolRegistry()->getToolNameById(toolId), toolPath);
        } else {
            task = new ExternalToolSearchAndValidateTask(toolId, AppContext::getExternalToolRegistry()->getToolNameById(toolId));
        }
        connect(task,
            SIGNAL(si_stateChanged()),
            SLOT(sl_validationTaskStateChanged()));
        taskList << task;
    }

    if (!taskList.isEmpty()) {
        ExternalToolsValidateTask* validationTask = new ExternalToolsValidateTask(taskList);
        validationTask->setMaxParallelSubtasks(MAX_PARALLEL_SUBTASKS);
        if (listener) {
            connect(validationTask, SIGNAL(si_stateChanged()), SLOT(sl_checkTaskStateChanged()));
            listeners.insert(validationTask, listener);
        }
        TaskScheduler* scheduler = AppContext::getTaskScheduler();
        SAFE_POINT(scheduler, "Task scheduler is NULL", );
        scheduler->registerTopLevelTask(validationTask);
    } else {
        if (listener) {
            listener->validationFinished();
        }
    }

    checkStartupTasksState();
}

void ExternalToolManagerImpl::loadCustomTools() {
    LoadCustomExternalToolsTask *loadTask = new LoadCustomExternalToolsTask();
    connect(new TaskSignalMapper(loadTask), SIGNAL(si_taskFinished(Task *)), SLOT(sl_customToolsLoaded(Task *)));
    AppContext::getTaskScheduler()->registerTopLevelTask(loadTask);
}

void ExternalToolManagerImpl::searchTools() {
    QList<Task*> taskList;

    foreach(const QString& toolId, searchList) {
        searchList.removeAll(toolId);
        toolStates.insert(toolId, SearchingIsInProcess);
        ExternalToolSearchTask* task = new ExternalToolSearchTask(toolId);
        connect(task,
            SIGNAL(si_stateChanged()),
            SLOT(sl_searchTaskStateChanged()));
        taskList << task;
    }

    if (!taskList.isEmpty()) {
        ExternalToolsSearchTask* searchTask = new ExternalToolsSearchTask(taskList);
        TaskScheduler* scheduler = AppContext::getTaskScheduler();
        SAFE_POINT(scheduler, "Task scheduler is NULL", );
        scheduler->registerTopLevelTask(searchTask);
    }

    checkStartupTasksState();
}

void ExternalToolManagerImpl::setToolPath(const QString& toolId, const QString& toolPath) {
    SAFE_POINT(etRegistry, "The external tool registry is NULL", );
    ExternalTool* tool = etRegistry->getById(toolId);
    SAFE_POINT(tool, QString("An external tool '%1' isn't found in the registry").arg(toolId), );
    tool->setPath(toolPath);
}

void ExternalToolManagerImpl::setToolValid(const QString& toolId, bool isValid) {
    SAFE_POINT(etRegistry, "The external tool registry is NULL", );
    ExternalTool* tool = etRegistry->getById(toolId);
    SAFE_POINT(tool, QString("An external tool '%1' isn't found in the registry").arg(toolId), );
    tool->setValid(isValid);
}

}   // namespace
