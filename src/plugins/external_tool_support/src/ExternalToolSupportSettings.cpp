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

#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QSettings>
#include <QStyle>
#include <QStyleFactory>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/Settings.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/AppSettingsGUI.h>
#include <U2Core/QObjectScopedPointer.h>

#include "ExternalToolSupportSettings.h"

namespace U2 {

#define NUMBER_EXTERNAL_TOOL SETTINGS + "numberExternalTools"
#define PREFIX_EXTERNAL_TOOL_ID SETTINGS + "exToolId"
#define PREFIX_EXTERNAL_TOOL_NAME_OBSOLETE SETTINGS + "exToolName"
#define PREFIX_EXTERNAL_TOOL_PATH SETTINGS + "exToolPath"
#define PREFIX_EXTERNAL_TOOL_IS_VALID SETTINGS + "exToolIsValid"
#define PREFIX_EXTERNAL_TOOL_VERSION SETTINGS + "exToolVersion"
#define PREFIX_EXTERNAL_TOOL_ADDITIONAL_INFO SETTINGS + "exToolAdditionalInfo"
#define TEMPORY_DIRECTORY SETTINGS + "temporyDirectory"

Watcher* const ExternalToolSupportSettings::watcher = new Watcher;

int ExternalToolSupportSettings::prevNumberExternalTools = 0;

int ExternalToolSupportSettings::getNumberExternalTools() {
    return AppContext::getSettings()->getValue(NUMBER_EXTERNAL_TOOL, 0, true).toInt();
}

void ExternalToolSupportSettings::setNumberExternalTools( int v ) {
    AppContext::getSettings()->setValue(NUMBER_EXTERNAL_TOOL, v, true);
    emit watcher->changed();
}

void ExternalToolSupportSettings::loadExternalTools() {
    int numberExternalTools = getNumberExternalTools();
    QString id;
    QString name;
    QString path;
    bool isValid = false;
    QString version;
    StrStrMap additionalInfo;
    for (int i = 0; i < numberExternalTools; i++) {
        id = AppContext::getSettings()->getValue(PREFIX_EXTERNAL_TOOL_ID + QString::number(i), QVariant(""), true).toString();
        if (id.isEmpty()) {
            name = AppContext::getSettings()->getValue(PREFIX_EXTERNAL_TOOL_NAME_OBSOLETE + QString::number(i), QVariant(""), true).toString();
        }
        path = AppContext::getSettings()->getValue(PREFIX_EXTERNAL_TOOL_PATH + QString::number(i), QVariant(""), true).toString();
        if (!QFile::exists(path)) {
            // executable is not found -> leave this tool alone
            continue;
        }
        isValid = AppContext::getSettings()->getValue(PREFIX_EXTERNAL_TOOL_IS_VALID + QString::number(i), QVariant(false), true).toBool();
        version = AppContext::getSettings()->getValue(PREFIX_EXTERNAL_TOOL_VERSION + QString::number(i), QVariant("unknown"), true).toString();
        additionalInfo = AppContext::getSettings()->getValue(PREFIX_EXTERNAL_TOOL_ADDITIONAL_INFO + QString::number(i), QVariant::fromValue<StrStrMap>(StrStrMap()), true).value<StrStrMap>();
        ExternalTool* tool = !id.isEmpty() ? AppContext::getExternalToolRegistry()->getById(id) : AppContext::getExternalToolRegistry()->getByName(name);
        if (tool != nullptr) {
            tool->setPath(path);
            tool->setVersion(version);
            tool->setValid(isValid);
            tool->setAdditionalInfo(additionalInfo);
        }
    }
    prevNumberExternalTools = numberExternalTools;
    ExternalToolSupportSettings::setExternalTools();
}

void ExternalToolSupportSettings::setExternalTools() {
    QList<ExternalTool*> ExternalToolList = AppContext::getExternalToolRegistry()->getAllEntries();
    int numberExternalTools = ExternalToolList.length();
    setNumberExternalTools(numberExternalTools);
    QString id;
    QString path;
    bool isValid = false;
    QString version;
    StrStrMap additionalInfo;
    int numberIterations = numberExternalTools >= prevNumberExternalTools ? numberExternalTools : prevNumberExternalTools;
    for (int i = 0; i < numberIterations; i++) {
        if (i < numberExternalTools) {
            id = ExternalToolList.at(i)->getId();
            path = ExternalToolList.at(i)->getPath();
            isValid = ExternalToolList.at(i)->isValid();
            version = ExternalToolList.at(i)->getVersion();
            additionalInfo = ExternalToolList.at(i)->getAdditionalInfo();
            AppContext::getSettings()->setValue(PREFIX_EXTERNAL_TOOL_ID + QString::number(i), id, true);
            AppContext::getSettings()->setValue(PREFIX_EXTERNAL_TOOL_PATH + QString::number(i), path, true);
            AppContext::getSettings()->setValue(PREFIX_EXTERNAL_TOOL_IS_VALID + QString::number(i), isValid, true);
            AppContext::getSettings()->setValue(PREFIX_EXTERNAL_TOOL_VERSION + QString::number(i), version, true);
            if (!additionalInfo.isEmpty()) {
                AppContext::getSettings()->setValue(PREFIX_EXTERNAL_TOOL_ADDITIONAL_INFO + QString::number(i), QVariant::fromValue<StrStrMap>(additionalInfo), true);
            }
        } else {
            AppContext::getSettings()->remove(PREFIX_EXTERNAL_TOOL_ID + QString::number(i));
            AppContext::getSettings()->remove(PREFIX_EXTERNAL_TOOL_PATH + QString::number(i));
            AppContext::getSettings()->remove(PREFIX_EXTERNAL_TOOL_IS_VALID + QString::number(i));
            AppContext::getSettings()->remove(PREFIX_EXTERNAL_TOOL_VERSION + QString::number(i));
            AppContext::getSettings()->remove(PREFIX_EXTERNAL_TOOL_ADDITIONAL_INFO + QString::number(i));
        }
    }
    prevNumberExternalTools = numberExternalTools;
}

void ExternalToolSupportSettings::checkTemporaryDir(U2OpStatus& os){
    if (AppContext::getAppSettings()->getUserAppsSettings()->getUserTemporaryDirPath().isEmpty()){
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

//////////////////////////////////////////////////////////////////////////
//LimitedDirIterator
LimitedDirIterator::LimitedDirIterator( const QDir &dir, int deepLevels )
:deepLevel(deepLevels)
,curPath("")
{
    if (deepLevel < 0){
        deepLevel = 0;
    }
    data.enqueue(qMakePair(dir.absolutePath(), 0));
}

bool LimitedDirIterator::hasNext(){
    return !data.isEmpty();
}

QString LimitedDirIterator::next(){
    QString res = curPath;

    fetchNext();

    return res;
}

QString LimitedDirIterator::filePath(){
    return curPath;
}

void LimitedDirIterator::fetchNext(){
    if (!data.isEmpty()){
        QPair<QString, int> nextPath = data.dequeue();
        curPath = nextPath.first;
        if (deepLevel > nextPath.second){
            QDir curDir(curPath);
            QStringList subdirs = curDir.entryList(QDir::NoDotAndDotDot | QDir::Dirs);
            foreach(const QString& subdir, subdirs){
                data.enqueue(qMakePair(curPath+ "/" + subdir, nextPath.second + 1));
            }
        }
    }
}

}//namespace
