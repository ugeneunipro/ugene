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

#include <QFile>
#include <QDialogButtonBox>
#include <QStyleFactory>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/Log.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/U2FileDialog.h>

#include "UserApplicationsSettingsGUIController.h"

namespace U2
{
#define TRANSMAP_FILE_NAME "translations.txt"

UserApplicationsSettingsPageController::UserApplicationsSettingsPageController(QObject* p) 
: AppSettingsGUIPageController(tr("General"), APP_SETTINGS_USER_APPS, p)
{
    translations.insert(tr("Autodetection"), "");
    QFile file( QString(PATH_PREFIX_DATA)+ ":" + TRANSMAP_FILE_NAME );

    if(!file.exists() || !file.open(QIODevice::ReadOnly)){
        coreLog.error(tr("File with translations is not found: %1").arg(TRANSMAP_FILE_NAME));
        translations.insert("English", "transl_en");
        return;
    } 
    QTextStream in(&file);
    in.setCodec("UTF-8");
    in.setAutoDetectUnicode(true);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if(line.length()==0 || line.startsWith("#")) {
            continue;
        }
        QStringList fields = line.split("|");
        if (fields.size()!=2) {
            coreLog.error(tr("Illegal entry in translations file: %1").arg(line));
            continue;
        }
        translations.insert(fields[0], fields[1].trimmed());
    }
    file.close();
}


AppSettingsGUIPageState* UserApplicationsSettingsPageController::getSavedState() {
    UserApplicationsSettingsPageState* state = new UserApplicationsSettingsPageState();
    UserAppsSettings* s = AppContext::getAppSettings()->getUserAppsSettings();
    state->translFile = s->getTranslationFile();
    state->openLastProjectFlag = s->openLastProjectAtStartup();
    state->askToSaveProject = s->getAskToSaveProject();
    state->style = s->getVisualStyle();
    state->enableStatistics = s->isStatisticsCollectionEnabled();
    state->tabbedWindowLayout = s->tabbedWindowLayout();
    state->resetSettings = s->resetSettings();
    state->updatesEnabled = s->updatesEnabled();
    return state;
}

void UserApplicationsSettingsPageController::saveState(AppSettingsGUIPageState* s) {
    UserApplicationsSettingsPageState* state = qobject_cast<UserApplicationsSettingsPageState*>(s);
    UserAppsSettings* st = AppContext::getAppSettings()->getUserAppsSettings();
    st->setTranslationFile(state->translFile);
    st->setOpenLastProjectAtStartup(state->openLastProjectFlag);
    st->setAskToSaveProject(state->askToSaveProject);
    st->setEnableCollectingStatistics(state->enableStatistics);
    //st->setTabbedWindowLayout(state->tabbedWindowLayout);
    st->setResetSettings(state->resetSettings);
    st->setUpdatesEnabled(state->updatesEnabled);

    if (0 != state->style.compare(st->getVisualStyle(), Qt::CaseInsensitive)) {
        QStyle* style = QStyleFactory::create(state->style);
        QApplication::setStyle(style);
        st->setVisualStyle(state->style);
    }
}

AppSettingsGUIPageWidget* UserApplicationsSettingsPageController::createWidget(AppSettingsGUIPageState* state) {
    UserApplicationsSettingsPageWidget* r = new UserApplicationsSettingsPageWidget(this);
    r->setState(state);
    return r;
}

const QString UserApplicationsSettingsPageController::helpPageId = QString("24748730");

UserApplicationsSettingsPageWidget::UserApplicationsSettingsPageWidget(UserApplicationsSettingsPageController* ctrl) {
    setupUi(this);
    connect(langButton, SIGNAL(clicked()), SLOT(sl_transFileClicked()));
    QMapIterator<QString, QString> it(ctrl->translations);
    while (it.hasNext()) {
        it.next();
        langCombo->addItem(it.key(), it.value());
    }
    styleCombo->addItems(QStyleFactory::keys());
}

void UserApplicationsSettingsPageWidget::setState(AppSettingsGUIPageState* s) {
    UserApplicationsSettingsPageState* state = qobject_cast<UserApplicationsSettingsPageState*>(s);
    enableStatisticsEdit->setChecked(state->enableStatistics);

    int idx = langCombo->findData(state->translFile);
    if (idx < 0) {
        idx = langCombo->count();
        langCombo->addItem(tr("Custom translation [%1]").arg(state->translFile), state->translFile);
    }
    langCombo->setCurrentIndex(idx);
    
    int styleIdx = styleCombo->findText(state->style, Qt::MatchFixedString); //case insensitive
    if (styleIdx!=-1) {
        styleCombo->setCurrentIndex(styleIdx);
    }

    autoOpenProjectBox->setChecked(state->openLastProjectFlag);
    askToSaveProject->addItem(tr("Ask to save new project on exit"), QDialogButtonBox::NoButton);
    askToSaveProject->addItem(tr("Don't save new project on exit"), QDialogButtonBox::No);
    askToSaveProject->addItem(tr("Always save new project on exit"), QDialogButtonBox::Yes);
    askToSaveProject->setCurrentIndex(askToSaveProject->findData(state->askToSaveProject));

    resetSettingsBox->setChecked(state->resetSettings);
    updatesCheckBox->setChecked(state->updatesEnabled);
}

AppSettingsGUIPageState* UserApplicationsSettingsPageWidget::getState(QString& /*err*/) const {
    UserApplicationsSettingsPageState* state = new UserApplicationsSettingsPageState();
    state->translFile = langCombo->itemData(langCombo->currentIndex()).toString();
    state->openLastProjectFlag = autoOpenProjectBox->isChecked();
    state->askToSaveProject = askToSaveProject->itemData(askToSaveProject->currentIndex()).toInt();
    state->style = styleCombo->currentText();
    state->enableStatistics = enableStatisticsEdit->isChecked();
    state->resetSettings = resetSettingsBox->isChecked();
    state->updatesEnabled = updatesCheckBox->isChecked();

    return state;
}

void UserApplicationsSettingsPageWidget::sl_transFileClicked() {
    QString file = U2FileDialog::getOpenFileName(this, tr("Select translation file"), QString(), 0);
    if (!file.isEmpty()) {
        QFileInfo fi(file);
        int idx = langCombo->findData(fi.baseName());
        if (idx < 0) {
            idx = langCombo->findData(fi.fileName());
        }
        if (idx < 0) {
            idx = langCombo->findData(fi.absoluteFilePath());
        }
        if (idx < 0) {
            idx = langCombo->count();
            langCombo->addItem(tr("Custom translation [%1]").arg(file), fi.absoluteFilePath());
        }
        langCombo->setCurrentIndex(idx);
    }
}

} //namespace
