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

#include "CustomExternalTool.h"

namespace U2 {

CustomExternalTool::CustomExternalTool()
    : ExternalTool(QString(), QString(), QString())
{
    isCustomTool = true;
}

void CustomExternalTool::setId(const QString& _id) {
    id = _id;
}

void CustomExternalTool::setName(const QString &_name) {
    name = _name;
}

void CustomExternalTool::setIcon(const QIcon &_icon) {
    icon = _icon;
}

void CustomExternalTool::setGrayIcon(const QIcon &_icon) {
    grayIcon = _icon;
}

void CustomExternalTool::setWarnIcon(const QIcon &_icon) {
    warnIcon = _icon;
}

void CustomExternalTool::setDescription(const QString &_description) {
    description = _description;
}

void CustomExternalTool::setLauncher(const QString &launcherId) {
    toolRunnerProgram = launcherId;
}

void CustomExternalTool::setBinaryName(const QString &binaryName) {
    executableFileName = binaryName;
}

void CustomExternalTool::setValidationArguments(const QStringList &arguments) {
    validationArguments = arguments;
}

void CustomExternalTool::setValidationExpectedText(const QString &text) {
    validMessage = text;
}

void CustomExternalTool::setPredefinedVersion(const QString &version) {
    predefinedVersion = version;
}

void CustomExternalTool::setVersionRegExp(const QRegExp &_versionRegExp) {
    versionRegExp = _versionRegExp;
}

void CustomExternalTool::setToolkitName(const QString &_toolkitName) {
    toolKitName = _toolkitName;
}

void CustomExternalTool::setDependencies(const QStringList &_dependencies) {
    dependencies = _dependencies;
}

void CustomExternalTool::setConfigFilePath(const QString &_configFilePath) {
    configFilePath = _configFilePath;
}

const QString &CustomExternalTool::getConfigFilePath() const {
    return configFilePath;
}

}   // namespace U2
