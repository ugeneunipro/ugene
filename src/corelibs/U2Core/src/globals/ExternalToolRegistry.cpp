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

#include "ExternalToolRegistry.h"

#include <QRegularExpression>

#include <U2Core/AppContext.h>
#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/Settings.h>
#include <U2Core/Task.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

////////////////////////////////////////
// ExternalToolValidation
const QString ExternalToolValidation::DEFAULT_DESCR_KEY = "DEFAULT_DESCR";

////////////////////////////////////////
// ExternalTool
ExternalTool::ExternalTool(const QString& id, const QString& dirName, const QString& name, const QString& path)
    : id(id),
      dirName(dirName),
      name(name),
      path(path),
      isValidTool(false),
      isCheckedTool(false),
      toolKitName(name),
      muted(false),
      isModuleTool(false),
      isCustomTool(false),
      isRunnerTool(false) {
    if (AppContext::getMainWindow() != nullptr) {
        icon = QIcon(":external_tool_support/images/cmdline.png");
        grayIcon = QIcon(":external_tool_support/images/cmdline_gray.png");
        warnIcon = QIcon(":external_tool_support/images/cmdline_warn.png");
    }
}

const QString& ExternalTool::getId() const {
    return id;
}

const QString& ExternalTool::getName() const {
    return name;
}

const QString& ExternalTool::getPath() const {
    return path;
}

const QIcon& ExternalTool::getIcon() const {
    return icon;
}

const QIcon& ExternalTool::getGrayIcon() const {
    return grayIcon;
}

const QIcon& ExternalTool::getWarnIcon() const {
    return warnIcon;
}

const QString& ExternalTool::getDescription() const {
    return description;
}

const QString& ExternalTool::getToolRunnerProgramId() const {
    return toolRunnerProgram;
}

QStringList ExternalTool::getToolRunnerAdditionalOptions() const {
    return QStringList();
}

const QString& ExternalTool::getExecutableFileName() const {
    return executableFileName;
}

const QString& ExternalTool::getVersion() const {
    return version;
}

const QString& ExternalTool::getPredefinedVersion() const {
    return predefinedVersion;
}

const QRegExp& ExternalTool::getVersionRegExp() const {
    return versionRegExp;
}

const QString& ExternalTool::getToolKitName() const {
    return toolKitName;
}

const StrStrMap& ExternalTool::getErrorDescriptions() const {
    return errorDescriptions;
}

const StrStrMap& ExternalTool::getAdditionalInfo() const {
    return additionalInfo;
}

QStringList ExternalTool::getAdditionalPaths() const {
    return QStringList();
}

QStringList ExternalTool::getRunParameters() const {
    return QStringList();
}

void ExternalTool::extractAdditionalParameters(const QString& /*output*/) {
    // do nothing
}

void ExternalTool::performAdditionalChecks(const QString& /*toolPath*/) {
    // do nothing
}

void ExternalTool::checkArgsAndPaths(const QStringList& arguments, U2OpStatus& os) const {
    checkPaths(os);
    CHECK_OP(os, )
    checkArgs(arguments, os);
}

void ExternalTool::checkArgs(const QStringList& arguments, U2OpStatus& os) const {
    QString error;
    for (auto check : qAsConst(pathChecks)) {
        switch (check) {
            case ExternalTool::PathChecks::NonLatinArguments:
                error = ExternalToolSupportUtils::checkArgumentPathLatinSymbols(arguments);
                break;
            case ExternalTool::PathChecks::SpacesArguments:
                error = ExternalToolSupportUtils::checkArgumentPathSpaces(arguments);
                break;
            case ExternalTool::PathChecks::NonLatinTemporaryDirPath:
            case ExternalTool::PathChecks::NonLatinToolPath:
            case ExternalTool::PathChecks::NonLatinIndexPath:
            case ExternalTool::PathChecks::SpacesTemporaryDirPath:
            case ExternalTool::PathChecks::SpacesToolPath:
                continue;
            default:
                break;
        }
        if (!error.isEmpty()) {
            error = error.arg(getName());
            os.setError(error);
            break;
        }
    }
}

void ExternalTool::checkPaths(U2OpStatus& os) const {
    QString error;
    for (auto check : qAsConst(pathChecks)) {
        switch (check) {
            case ExternalTool::PathChecks::NonLatinTemporaryDirPath:
                error = ExternalToolSupportUtils::checkTemporaryDirLatinSymbols();
                break;
            case ExternalTool::PathChecks::NonLatinToolPath:
                error = ExternalToolSupportUtils::checkToolPathLatinSymbols(this);
                break;
            case ExternalTool::PathChecks::NonLatinIndexPath:
                error = ExternalToolSupportUtils::checkIndexDirLatinSymbols();
                break;
            case ExternalTool::PathChecks::SpacesTemporaryDirPath:
                error = ExternalToolSupportUtils::checkTemporaryDirSpaces();
                break;
            case ExternalTool::PathChecks::SpacesToolPath:
                error = ExternalToolSupportUtils::checkToolPathSpaces(this);
                break;
            case ExternalTool::PathChecks::NonLatinArguments:
            case ExternalTool::PathChecks::SpacesArguments:
                continue;
            default:
                break;
        }
        if (!error.isEmpty()) {
            error = error.arg(getName());
            os.setError(error);
            break;
        }
    }
}

ExternalToolValidation ExternalTool::getToolValidation() {
    ExternalToolValidation result(toolRunnerProgram, executableFileName, validationArguments, validMessage, errorDescriptions);
    return result;
}

const QList<ExternalToolValidation>& ExternalTool::getToolAdditionalValidations() const {
    return additionalValidators;
}

const QStringList& ExternalTool::getDependencies() const {
    return dependencies;
}

const QString& ExternalTool::getAdditionalErrorMessage() const {
    return additionalErrorMesage;
}

void ExternalTool::setAdditionalErrorMessage(const QString& message) {
    additionalErrorMesage = message;
}

bool ExternalTool::hasAdditionalErrorMessage() const {
    return !additionalErrorMesage.isEmpty();
}

void ExternalTool::setPath(const QString& _path) {
    if (path != _path) {
        path = _path;
        emit si_pathChanged();
    }
}

void ExternalTool::setValid(bool isValid) {
    isValidTool = isValid;
    emit si_toolValidationStatusChanged(isValidTool);
}

void ExternalTool::setChecked(bool isChecked) {
    isCheckedTool = isChecked;
}

void ExternalTool::setVersion(const QString& _version) {
    version = _version;
}

void ExternalTool::setAdditionalInfo(const StrStrMap& newAdditionalInfo) {
    additionalInfo = newAdditionalInfo;
}

bool ExternalTool::isValid() const {
    return isValidTool;
}

bool ExternalTool::isChecked() const {
    return isCheckedTool;
}

bool ExternalTool::isMuted() const {
    return muted;
}

bool ExternalTool::isModule() const {
    return isModuleTool;
}

bool ExternalTool::isCustom() const {
    return isCustomTool;
}

bool ExternalTool::isRunner() const {
    return isRunnerTool;
}

////////////////////////////////////////
// ExternalToolValidationListener
ExternalToolValidationListener::ExternalToolValidationListener(const QString& toolId) {
    toolIds << toolId;
}

ExternalToolValidationListener::ExternalToolValidationListener(const QStringList& toolIds)
    : toolIds(toolIds) {
}

////////////////////////////////////////
// ExternalToolRegistry
ExternalToolRegistry::ExternalToolRegistry()
    : manager(nullptr) {
}

ExternalToolRegistry::~ExternalToolRegistry() {
    qDeleteAll(toolByLowerCaseIdMap.values());
}

ExternalTool* ExternalToolRegistry::getByName(const QString& name) const {
    const QList<ExternalTool*> toolList = toolByLowerCaseIdMap.values();
    for (ExternalTool* tool : qAsConst(toolList)) {
        if (tool->getName() == name) {
            return tool;
        }
    }
    return nullptr;
}

ExternalTool* ExternalToolRegistry::getById(const QString& id) const {
    return toolByLowerCaseIdMap.value(id.toLower(), nullptr);
}

QString ExternalToolRegistry::getToolNameById(const QString& id) const {
    ExternalTool* tool = getById(id);
    CHECK(tool != nullptr, QString());
    return tool->getName();
}

bool ExternalToolRegistry::registerEntry(ExternalTool* tool) {
    const QString& id = tool->getId();
    QString lowerCaseId = id.toLower();
    if (toolByLowerCaseIdMap.contains(lowerCaseId)) {
        return false;
    }
    toolByLowerCaseIdMap.insert(lowerCaseId, tool);
    emit si_toolAdded(id);
    return true;
}

void ExternalToolRegistry::unregisterEntry(const QString& id) {
    QString lowerCaseId = id.toLower();
    CHECK(toolByLowerCaseIdMap.contains(lowerCaseId), );
    emit si_toolIsAboutToBeRemoved(id);

    ExternalTool* et = toolByLowerCaseIdMap.take(lowerCaseId);
    delete et;
}

QList<ExternalTool*> ExternalToolRegistry::getAllEntries() const {
    return toolByLowerCaseIdMap.values();
}

QList<QList<ExternalTool*>> ExternalToolRegistry::getAllEntriesSortedByToolKits() const {
    QMap<QString, QList<ExternalTool*>> toolListByToolKitNameMap;
    const QList<ExternalTool*> toolList = toolByLowerCaseIdMap.values();
    for (ExternalTool* tool : qAsConst(toolList)) {
        const QString& toolKitName = tool->getToolKitName();
        if (!toolListByToolKitNameMap.contains(toolKitName)) {
            toolListByToolKitNameMap.insert(toolKitName, QList<ExternalTool*>() << tool);
        } else {
            QList<ExternalTool*>& toolKitTools = toolListByToolKitNameMap[toolKitName];
            toolKitTools << tool;
        }
    }
    // Sort tools inside every toolkit tools list by tool name.
    QList<QList<ExternalTool*>> sortedResultList;
    const QList<QList<ExternalTool*>> toolkitLists = toolListByToolKitNameMap.values();
    for (QList<ExternalTool*> toolsList : qAsConst(toolkitLists)) {
        std::sort(toolsList.begin(), toolsList.end(), [](ExternalTool* t1, ExternalTool* t2) {
            return t1->getName().compare(t2->getName(), Qt::CaseInsensitive) < 0;
        });
        sortedResultList << toolsList;
    }
    // Sort toolkits in the result list by toolkit name.
    std::sort(sortedResultList.begin(), sortedResultList.end(), [](QList<ExternalTool*>& t1, QList<ExternalTool*>& t2) {
        return t1[0]->getToolKitName().compare(t2[0]->getToolKitName(), Qt::CaseInsensitive) < 0;
    });
    return sortedResultList;
}

void ExternalToolRegistry::setManager(ExternalToolManager* _manager) {
    manager = _manager;
}

ExternalToolManager* ExternalToolRegistry::getManager() const {
    return manager;
}

}  // namespace U2
