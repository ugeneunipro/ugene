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

#include "ExternalToolManager.h"

#include <QEventLoop>
#include <QSet>
#include <QTimer>

#include <U2Core/AppContext.h>
#include <U2Core/CustomExternalTool.h>
#include <U2Core/PluginModel.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2SafePoints.h>

#include "ExternalToolSupportSettings.h"
#include "custom_tools/LoadCustomExternalToolsTask.h"
#include "utils/ExternalToolSearchTask.h"
#include "utils/ExternalToolValidateTask.h"

namespace U2 {

ExternalToolManagerImpl::ExternalToolManagerImpl()
    : isStartupValidation(true) {
    etRegistry = AppContext::getExternalToolRegistry();
    SAFE_POINT(etRegistry != nullptr, "ExternalToolRegistry is null!", );

    if (AppContext::getPluginSupport()->isAllPluginsLoaded()) {
        // Call sl_initialize asynchronously, so the plugin will set ExternalToolManager instance to the AppContext first.
        QTimer::singleShot(0, this, SLOT(sl_initialize()));
    } else {
        connect(AppContext::getPluginSupport(), SIGNAL(si_allStartUpPluginsLoaded()), SLOT(sl_initialize()));
    }
}

void ExternalToolManagerImpl::sl_initialize() {
    // Run LoadCustomExternalToolsTask. Once it is finished all tools will be present in the registry and validation will start.
    auto loadTask = new LoadCustomExternalToolsTask();
    connect(new TaskSignalMapper(loadTask), SIGNAL(si_taskFinished(Task*)), SLOT(sl_onRegistryHasToolsListingLoaded(Task*)));
    AppContext::getTaskScheduler()->registerTopLevelTask(loadTask);
}

void ExternalToolManagerImpl::sl_onRegistryHasToolsListingLoaded(Task* /*task*/) {
    ExternalToolSupportSettings::loadExternalToolsFromAppConfig();
    QList<ExternalTool*> toolList = etRegistry->getAllEntries();
    for (ExternalTool* tool : qAsConst(toolList)) {
        registerTool(tool);
    }
    StrStrMap toolPathByIdMap;
    for (ExternalTool* tool : qAsConst(toolList)) {
        QString toolPath = addToolToPendingListsAndReturnToolPath(tool);
        if (!toolPath.isEmpty()) {
            toolPathByIdMap.insert(tool->getId(), toolPath);
        }
    }
    runPendingValidationTasks(toolPathByIdMap);
}

void ExternalToolManagerImpl::checkStartupValidationState() {
    CHECK(isStartupValidation, );
    QList<ExternalToolState> toolStates = toolStateMap.values();
    if (!pendingValidationToolSet.isEmpty() || toolStates.contains(ValidationIsInProcess)) {
        return;
    }
    isStartupValidation = false;
    ExternalToolSupportSettings::saveExternalToolsToAppConfig();

    connect(etRegistry, SIGNAL(si_toolAdded(const QString&)), SLOT(sl_onToolAddedToRegistry(const QString&)));
    connect(etRegistry, SIGNAL(si_toolIsAboutToBeRemoved(const QString&)), SLOT(sl_onToolRemovedFromRegistry(const QString&)));

    emit si_startupValidationFinished();
}

void ExternalToolManagerImpl::validate(const QStringList& toolIds, const StrStrMap& toolPaths, ExternalToolValidationListener* listener) {
    for (const QString& toolId : qAsConst(toolIds)) {
        ExternalTool* tool = etRegistry->getById(toolId);
        if (tool == nullptr) {
            continue;
        }
        tool->setAdditionalErrorMessage(QString());
        if (checkAllDependenciesAreValid(tool)) {
            pendingValidationToolSet << toolId;
        } else {
            toolStateMap.insert(toolId, NotValidByDependency);
            if (toolPaths.contains(toolId)) {
                tool->setPath(toolPaths.value(toolId));
            }
        }
    }
    runPendingValidationTasks(toolPaths, listener);
}

bool ExternalToolManagerImpl::isValid(const QString& toolId) const {
    return toolStateMap.value(toolId, Unprocessed) == Valid;
}

ExternalToolManager::ExternalToolState ExternalToolManagerImpl::getToolState(const QString& toolId) const {
    return toolStateMap.value(toolId, Unprocessed);
}

void ExternalToolManagerImpl::registerTool(ExternalTool* tool) {
    ExternalToolState toolState = tool->isValid() ? Valid : (tool->isChecked() ? NotValid : Unprocessed);
    toolStateMap.insert(tool->getId(), toolState);

    connect(tool, SIGNAL(si_toolValidationStatusChanged(bool)), SLOT(sl_onToolStatusChanged(bool)));

    QStringList masterToolList = tool->getDependencies();
    if (!masterToolList.isEmpty()) {
        for (const QString& masterToolId : qAsConst(masterToolList)) {
            childToolsMultiMap.insertMulti(masterToolId, tool->getId());
        }
    }
}

QString ExternalToolManagerImpl::addToolToPendingListsAndReturnToolPath(ExternalTool* tool) {
    QString toolPath = tool->getPath();
    if (tool->isValid() || tool->isChecked()) {
        return toolPath;
    }
    QStringList toolDependencies = tool->getDependencies();
    if (!toolDependencies.isEmpty()) {
        if (checkAllDependenciesAreValid(tool)) {
            if (tool->isModule()) {
                QString masterId = tool->getDependencies().first();
                ExternalTool* masterTool = etRegistry->getById(masterId);
                SAFE_POINT(masterTool, QString("An external tool '%1' isn't found in the registry").arg(masterId), "");
                toolPath = masterTool->getPath();
            }
            pendingValidationToolSet << tool->getId();
        }
    } else {
        pendingValidationToolSet << tool->getId();
    }
    return toolPath;
}

void ExternalToolManagerImpl::sl_onToolValidationTaskFinished(Task* task) {
    auto validationTask = qobject_cast<ExternalToolValidateTask*>(task);
    SAFE_POINT(validationTask, "Unexpected task", );
    ExternalTool* tool = etRegistry->getById(validationTask->getToolId());
    CHECK(tool != nullptr, );  // A tool may be removed from the registry while validation task is running.

    toolStateMap.insert(validationTask->getToolId(), validationTask->isValidTool() ? Valid : NotValid);

    SAFE_POINT(tool, QString("An external tool '%1' isn't found in the registry").arg(validationTask->getToolName()), );
    if (tool->isModule()) {
        QStringList toolDependencies = tool->getDependencies();
        SAFE_POINT(!toolDependencies.isEmpty(), QString("Tool's dependencies list is unexpectedly empty: "
                                                        "a master tool for the module '%1' is not defined")
                                                    .arg(tool->getId()), );
        QString masterId = toolDependencies.first();
        ExternalTool* masterTool = etRegistry->getById(masterId);
        SAFE_POINT(masterTool, QString("An external tool '%1' isn't found in the registry").arg(masterId), );
        SAFE_POINT(masterTool->getPath() == validationTask->getToolPath(), "Module tool should have the same path as it's master tool", );
    }

    tool->setVersion(validationTask->getToolVersion());
    tool->setPath(validationTask->getToolPath());
    tool->setValid(validationTask->isValidTool());
    tool->setChecked(true);

    // Saving on every tool update may be too much but it may help to avoid startup failures:
    //  if startup is failed because of the external tools validation next time it will continue from the state it stopped.
    ExternalToolSupportSettings::saveExternalToolsToAppConfig();
    checkStartupValidationState();
}

void ExternalToolManagerImpl::sl_onToolStatusChanged(bool isValid) {
    ExternalTool* tool = qobject_cast<ExternalTool*>(sender());
    SAFE_POINT(tool != nullptr, "Unexpected message sender", );

    toolStateMap.insert(tool->getId(), isValid ? Valid : NotValid);

    // Process all child tools.
    StrStrMap childToolPathMap;
    const QList<QString> childToolList = childToolsMultiMap.values(tool->getId());
    for (const QString& childToolId : qAsConst(childToolList)) {
        ExternalTool* childTool = etRegistry->getById(childToolId);
        SAFE_POINT(childTool, QString("An external tool '%1' isn't found in the registry").arg(childToolId), );

        if (childTool->isModule()) {
            childToolPathMap.insert(childToolId, tool->getPath());
            childTool->setPath(tool->getPath());
        }

        ExternalToolState childToolState = toolStateMap.value(childToolId, Unprocessed);
        if (childToolState != ValidationIsInProcess) {
            if (isValid && checkAllDependenciesAreValid(childTool)) {
                pendingValidationToolSet << childToolId;
            } else if (!isValid && !pendingValidationToolSet.contains(childToolId)) {
                childTool->setValid(false);
                toolStateMap.insert(childToolId, NotValidByDependency);
            }
        }
    }

    runPendingValidationTasks(childToolPathMap);
}

void ExternalToolManagerImpl::sl_onToolAddedToRegistry(const QString& toolId) {
    ExternalTool* tool = etRegistry->getById(toolId);
    SAFE_POINT(tool != nullptr, "Tool is nullptr", );

    StrStrMap toolPathByIdMap;
    QString toolPath = addToolToPendingListsAndReturnToolPath(tool);
    if (!toolPath.isEmpty()) {
        toolPathByIdMap.insert(tool->getId(), toolPath);
    }
    runPendingValidationTasks(toolPathByIdMap);
}

void ExternalToolManagerImpl::sl_onToolRemovedFromRegistry(const QString& toolId) {
    toolStateMap.remove(toolId);
    pendingValidationToolSet.remove(toolId);
    childToolsMultiMap.remove(toolId);

    QMutableMapIterator<QString, QString> iterator(childToolsMultiMap);
    while (iterator.hasNext()) {
        auto mapEntry = iterator.next();
        if (toolId == mapEntry.value()) {
            iterator.remove();
        }
    }
}

bool ExternalToolManagerImpl::checkAllDependenciesAreValid(ExternalTool* tool) {
    bool isAllValid = true;
    const QStringList dependencyList = tool->getDependencies();
    for (const QString& masterId : qAsConst(dependencyList)) {
        if (!toolStateMap.contains(masterId)) {
            coreLog.details(tr("A dependency tool isn't represented in the general tool list. Skip dependency \"%1\"").arg(masterId));
            isAllValid = false;
            continue;
        }
        isAllValid = isAllValid && toolStateMap.value(masterId, Unprocessed) == Valid;
    }
    return isAllValid;
}

void ExternalToolManagerImpl::runPendingValidationTasks(const StrStrMap& predefinedToolPathById, ExternalToolValidationListener* listener) {
    QList<Task*> validationTaskList;
    QStringList validationToolList = pendingValidationToolSet.values();
    pendingValidationToolSet.clear();
    for (const QString& toolId : qAsConst(validationToolList)) {
        QString predefinedToolPath;
        bool hasPredefinedPath = predefinedToolPathById.contains(toolId);
        if (hasPredefinedPath) {
            // Path is already provided -> if it is empty the tool must be marked as invalid and not further validation made.
            predefinedToolPath = predefinedToolPathById.value(toolId);
            if (predefinedToolPath.isEmpty()) {
                toolStateMap.insert(toolId, NotValid);
                ExternalTool* tool = etRegistry->getById(toolId);
                if (tool != nullptr) {
                    tool->setPath(predefinedToolPath);
                    tool->setValid(false);
                    if (listener != nullptr) {
                        listener->setToolState(toolId, false);
                    }
                }
                continue;
            }
        }
        toolStateMap.insert(toolId, ValidationIsInProcess);
        ExternalToolValidateTask* task;
        ExternalTool* tool = AppContext::getExternalToolRegistry()->getById(toolId);
        if (tool == nullptr) {  // a tool may be asynchronously removed from registry.
            continue;
        }
        if (hasPredefinedPath) {
            task = new ExternalToolJustValidateTask(toolId, tool->getName(), predefinedToolPath);
        } else {
            task = new ExternalToolSearchAndValidateTask(toolId, tool->getName());
        }
        connect(new TaskSignalMapper(task), SIGNAL(si_taskFinished(Task*)), SLOT(sl_onToolValidationTaskFinished(Task*)));
        validationTaskList << task;
    }

    if (!validationTaskList.isEmpty()) {
        auto masterTask = new ExternalToolsValidationMasterTask(validationTaskList, listener);
        AppContext::getTaskScheduler()->registerTopLevelTask(masterTask);
    } else if (listener != nullptr) {
        listener->validationFinished();
    }
    checkStartupValidationState();
}

}  // namespace U2
