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

#ifndef _U2_EXTERNAL_TOOL_VALIDATION_MANAGER_H_
#define _U2_EXTERNAL_TOOL_VALIDATION_MANAGER_H_

#include <QList>
#include <QObject>
#include <QSet>

#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/global.h>

namespace U2 {

class Task;

/**
 * ExternalToolManagerImpl is responsible for external tools validation and is a source of the current tool status (ExternalToolState)
 * for all external tools in the registry.
 */
class ExternalToolManagerImpl : public ExternalToolManager {
    Q_OBJECT
public:
    ExternalToolManagerImpl();

    void validate(const QStringList& toolIds, const StrStrMap& toolPaths, ExternalToolValidationListener* listener = nullptr) override;

    /** Checks that tool is valid. */
    bool isValid(const QString& toolId) const override;

    /** Returns true if startup validation is in progress. */
    bool isInStartupValidationMode() const override {
        return isStartupValidation;
    }

    /** Returns current validation state of the external tool. */
    ExternalToolState getToolState(const QString& toolId) const override;

private slots:
    /** Initializes custom external tools loading and results to sl_onRegistryHasToolsListingLoaded call. */
    void sl_initialize();

    /** Starts validation of all tasks in the registry. */
    void sl_onRegistryHasToolsListingLoaded(Task*);

    /** Callback of the single tool validation task. */
    void sl_onToolValidationTaskFinished(Task* task);

    /** Called on external tool status change in the registry. */
    void sl_onToolStatusChanged(bool isValid);

    /** Called every time a new tool is added to the registry. */
    void sl_onToolAddedToRegistry(const QString& toolId);

    /** Called every time an existing tool is about to be removed from the registry. */
    void sl_onToolRemovedFromRegistry(const QString& toolId);

private:
    /** Checks if all startup tasks are finished and starts the startup validation if it was not started before. */
    void checkStartupValidationState();

    /** Registers tool & dependencies in the state map. */
    void registerTool(ExternalTool* tool);

    QString addToolToPendingListsAndReturnToolPath(ExternalTool* tool);

    /** Returns true if all tool dependencies can are valid. */
    bool checkAllDependenciesAreValid(ExternalTool* tool);

    void runPendingValidationTasks(const StrStrMap& predefinedToolPathById = StrStrMap(), ExternalToolValidationListener* listener = nullptr);

    ExternalToolRegistry* etRegistry;

    /** Tools waiting for validation. */
    QSet<QString> pendingValidationToolSet;

    /** Inversed map of tool dependencies: master tool id -> list of child tool ids.*/
    StrStrMap childToolsMultiMap;

    /** State of the tool by tool id. */
    QMap<QString, ExternalToolState> toolStateMap;

    /** 'True' during start-up validation phase. */
    bool isStartupValidation;
};

}  // namespace U2

#endif  // _U2_EXTERNAL_TOOL_VALIDATION_MANAGER_H_
