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

#include "ExternalToolSupportSettings.h"

#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QSettings>
#include <QStyle>
#include <QStyleFactory>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Settings.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/AppSettingsGUI.h>

namespace U2 {

#define NUMBER_EXTERNAL_TOOL SETTINGS + "numberExternalTools"
#define PREFIX_EXTERNAL_TOOL_ID SETTINGS + "exToolId"
#define PREFIX_EXTERNAL_TOOL_PATH SETTINGS + "exToolPath"
#define PREFIX_EXTERNAL_TOOL_IS_VALID SETTINGS + "exToolIsValid"
// If true the external tool was already checked and the current isValid state should be preserved on startup and no additional check should be made.
#define PREFIX_EXTERNAL_TOOL_IS_CHECKED SETTINGS + "exToolIsChecked"
#define PREFIX_EXTERNAL_TOOL_VERSION SETTINGS + "exToolVersion"
#define PREFIX_EXTERNAL_TOOL_ADDITIONAL_INFO SETTINGS + "exToolAdditionalInfo"

Watcher* const ExternalToolSupportSettings::watcher = new Watcher;

int ExternalToolSupportSettings::prevNumberExternalTools = 0;

int ExternalToolSupportSettings::getNumberExternalTools() {
    return AppContext::getSettings()->getValue(NUMBER_EXTERNAL_TOOL, 0, true).toInt();
}

void ExternalToolSupportSettings::setNumberExternalTools(int v) {
    AppContext::getSettings()->setValue(NUMBER_EXTERNAL_TOOL, v, true);
    emit watcher->changed();
}

void ExternalToolSupportSettings::loadExternalToolsFromAppConfig() {
    int numberExternalTools = getNumberExternalTools();
    Settings* settings = AppContext::getSettings();
    for (int i = 0; i < numberExternalTools; i++) {
        QString toolIndex = QString::number(i);
        QString id = settings->getValue(PREFIX_EXTERNAL_TOOL_ID + toolIndex, QVariant(""), true).toString();
        QString path = settings->getValue(PREFIX_EXTERNAL_TOOL_PATH + toolIndex, QVariant(""), true, true).toString();
        bool isValid = settings->getValue(PREFIX_EXTERNAL_TOOL_IS_VALID + toolIndex, QVariant(false), true, true).toBool();
        bool isChecked = settings->getValue(PREFIX_EXTERNAL_TOOL_IS_CHECKED + toolIndex, QVariant(false), true, true).toBool();
        QString version = settings->getValue(PREFIX_EXTERNAL_TOOL_VERSION + toolIndex, QVariant("unknown"), true, true).toString();
        StrStrMap additionalInfo = settings->getValue(PREFIX_EXTERNAL_TOOL_ADDITIONAL_INFO + toolIndex, QVariant::fromValue<StrStrMap>(StrStrMap()), true, true).value<StrStrMap>();
        ExternalTool* tool = AppContext::getExternalToolRegistry()->getById(id);
        if (tool != nullptr) {
            tool->setPath(path);
            tool->setVersion(version);
            tool->setValid(isValid);
            tool->setChecked(isChecked);
            tool->setAdditionalInfo(additionalInfo);
        }
    }
    prevNumberExternalTools = numberExternalTools;
    ExternalToolSupportSettings::saveExternalToolsToAppConfig();
}

void ExternalToolSupportSettings::saveExternalToolsToAppConfig() {
    QList<ExternalTool*> externalToolList = AppContext::getExternalToolRegistry()->getAllEntries();
    setNumberExternalTools(externalToolList.length());
    int numberOfIterations = prevNumberExternalTools <= externalToolList.length() ? externalToolList.length() : prevNumberExternalTools;
    Settings* settings = AppContext::getSettings();
    for (int i = 0; i < numberOfIterations; i++) {
        QString toolIndex = QString::number(i);
        if (i < externalToolList.length()) {
            ExternalTool* tool = externalToolList[i];
            settings->setValue(PREFIX_EXTERNAL_TOOL_ID + toolIndex, tool->getId(), true);
            settings->setValue(PREFIX_EXTERNAL_TOOL_PATH + toolIndex, tool->getPath(), true, true);
            settings->setValue(PREFIX_EXTERNAL_TOOL_IS_VALID + toolIndex, tool->isValid(), true, true);
            settings->setValue(PREFIX_EXTERNAL_TOOL_IS_CHECKED + toolIndex, tool->isChecked(), true, true);
            settings->setValue(PREFIX_EXTERNAL_TOOL_VERSION + toolIndex, tool->getVersion(), true, true);
            StrStrMap additionalInfo = tool->getAdditionalInfo();
            if (!additionalInfo.isEmpty()) {
                settings->setValue(PREFIX_EXTERNAL_TOOL_ADDITIONAL_INFO + toolIndex, QVariant::fromValue<StrStrMap>(additionalInfo), true, true);
            }
        } else {
            settings->remove(PREFIX_EXTERNAL_TOOL_ID + toolIndex);
            settings->remove(PREFIX_EXTERNAL_TOOL_PATH + toolIndex);
            settings->remove(PREFIX_EXTERNAL_TOOL_IS_VALID + toolIndex);
            settings->remove(PREFIX_EXTERNAL_TOOL_IS_CHECKED + toolIndex);
            settings->remove(PREFIX_EXTERNAL_TOOL_VERSION + toolIndex);
            settings->remove(PREFIX_EXTERNAL_TOOL_ADDITIONAL_INFO + toolIndex);
        }
    }
    prevNumberExternalTools = externalToolList.length();
}

void ExternalToolSupportSettings::checkTemporaryDir(U2OpStatus& os) {
    if (AppContext::getAppSettings()->getUserAppsSettings()->getUserTemporaryDirPath().isEmpty()) {
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox;
        msgBox->setWindowTitle(QObject::tr("Path for temporary files"));
        msgBox->setText(QObject::tr("Path for temporary files not selected."));
        msgBox->setInformativeText(QObject::tr("Do you want to select it now?"));
        msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox->setDefaultButton(QMessageBox::Yes);
        const int ret = msgBox->exec();
        CHECK(!msgBox.isNull(), );

        if (ret == QMessageBox::Yes) {
            AppContext::getAppSettingsGUI()->showSettingsDialog(APP_SETTINGS_USER_APPS);
        }
    }
    if (AppContext::getAppSettings()->getUserAppsSettings()->getUserTemporaryDirPath().isEmpty()) {
        os.setError(UserAppsSettings::tr("Temporary UGENE dir is empty"));
    }
}

bool ExternalToolSupportSettings::checkTemporaryDir(const LogLevel& logLevel) {
    U2OpStatus2Log os(logLevel);
    ExternalToolSupportSettings::checkTemporaryDir(os);
    return !os.hasError();
}
//////////////////////////////////////////////////////////////////////////
// LimitedDirIterator
LimitedDirIterator::LimitedDirIterator(const QDir& dir, int deepLevels)
    : deepLevel(deepLevels), curPath("") {
    if (deepLevel < 0) {
        deepLevel = 0;
    }
    data.enqueue(qMakePair(dir.absolutePath(), 0));
}

bool LimitedDirIterator::hasNext() {
    return !data.isEmpty();
}

QString LimitedDirIterator::next() {
    QString res = curPath;

    fetchNext();

    return res;
}

QString LimitedDirIterator::filePath() {
    return curPath;
}

void LimitedDirIterator::fetchNext() {
    if (!data.isEmpty()) {
        QPair<QString, int> nextPath = data.dequeue();
        curPath = nextPath.first;
        if (deepLevel > nextPath.second) {
            QDir curDir(curPath);
            QStringList subdirs = curDir.entryList(QDir::NoDotAndDotDot | QDir::Dirs);
            foreach (const QString& subdir, subdirs) {
                data.enqueue(qMakePair(curPath + "/" + subdir, nextPath.second + 1));
            }
        }
    }
}

}  // namespace U2
