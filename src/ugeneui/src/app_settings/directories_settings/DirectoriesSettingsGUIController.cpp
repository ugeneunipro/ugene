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
#include <U2Core/CleanupFileStorageTask.h>
#include <U2Core/Log.h>
#include <U2Core/TmpDirChecker.h>
#include <U2Core/UserApplicationsSettings.h>
#include <U2Core/Settings.h>

#include <U2Gui/U2FileDialog.h>

#include "DirectoriesSettingsGUIController.h"

namespace U2{
#define SETTINGS_ROOT   QString("/genome_aligner_settings/")
#define INDEX_DIR       QString("index_dir")

DirectoriesSettingsPageController::DirectoriesSettingsPageController(QObject* p)
: AppSettingsGUIPageController(tr("Directories"), APP_SETTINGS_DIRECTORIES, p)
{
}

QString DirectoriesSettingsPageUtils::getIndexDir() {
    QString defaultDir = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath("aligner");
    QString res = AppContext::getSettings()->getValue(SETTINGS_ROOT + INDEX_DIR, defaultDir, true).toString();

    return res;
}

void DirectoriesSettingsPageUtils::setIndexDir(const QString &indexDir) {
    QString defaultDir = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath("aligner");
    if (defaultDir != indexDir) {
        AppContext::getSettings()->setValue(SETTINGS_ROOT + INDEX_DIR, indexDir, true);
    }
}


AppSettingsGUIPageState* DirectoriesSettingsPageController::getSavedState() {
    DirectoriesSettingsPageState* state = new DirectoriesSettingsPageState();
    UserAppsSettings* s = AppContext::getAppSettings()->getUserAppsSettings();
    state->downloadsDirPath = s->getDownloadDirPath();
    state->documentsDirectory = s->getDefaultDataDirPath();
    state->temporaryDirPath = s->getUserTemporaryDirPath();
    state->fileStorageDirPath = s->getFileStorageDir();
    state->indexDirectory = DirectoriesSettingsPageUtils::getIndexDir();
    
    return state;
}

void DirectoriesSettingsPageController::saveState(AppSettingsGUIPageState* s) {
    DirectoriesSettingsPageState* state = qobject_cast<DirectoriesSettingsPageState*>(s);
    UserAppsSettings* st = AppContext::getAppSettings()->getUserAppsSettings();
    st->setDownloadDirPath(state->downloadsDirPath);
    st->setDefaultDataDirPath(state->documentsDirectory);
    DirectoriesSettingsPageUtils::setIndexDir(state->indexDirectory);
    TmpDirChecker tmpDirChecker;
    if (!tmpDirChecker.checkPath(state->temporaryDirPath)) {
        uiLog.error(tr("You do not have permission to write to \"%1\" folder\"").arg(state->temporaryDirPath));
    }else{
        st->setUserTemporaryDirPath(state->temporaryDirPath);
    }

    if (!tmpDirChecker.checkPath(state->fileStorageDirPath)) {
        uiLog.error(tr("You do not have permission to write to \"%1\" folder\"").arg(state->temporaryDirPath));
    }else{
        st->setFileStorageDir(state->fileStorageDirPath);
    }
}

AppSettingsGUIPageWidget* DirectoriesSettingsPageController::createWidget(AppSettingsGUIPageState* state) {
    DirectoriesSettingsPageWidget* r = new DirectoriesSettingsPageWidget(this);
    r->setState(state);
    return r;
}

const QString DirectoriesSettingsPageController::helpPageId = QString("24748741");

DirectoriesSettingsPageWidget::DirectoriesSettingsPageWidget(DirectoriesSettingsPageController* /*ctrl*/) {
    setupUi(this);
    connect(browseDownloadDirButton, SIGNAL(clicked()), SLOT(sl_browseDownloadsDirButtonClicked()));
    connect(documentsDirectoryButton, SIGNAL(clicked()), SLOT(sl_browseDocumentsDirButtonClicked()));
    connect(browseTmpDirButton,SIGNAL(clicked()),SLOT(sl_browseTmpDirButtonClicked()));
    connect(browseFileStorageButton,SIGNAL(clicked()),SLOT(sl_browseFileStorageButtonClicked()));
    connect(cleanupStorageButton,SIGNAL(clicked()),SLOT(sl_cleanupStorage()));
    connect(indexDirectoryButton, SIGNAL(clicked()), SLOT(sl_onIndexDirButton()));
}

void DirectoriesSettingsPageWidget::setState(AppSettingsGUIPageState* s) {
    DirectoriesSettingsPageState* state = qobject_cast<DirectoriesSettingsPageState*>(s);
    downloadsDirPathEdit->setText(state->downloadsDirPath);
    documentsDirectoryEdit->setText(state->documentsDirectory);
    tmpDirPathEdit->setText(state->temporaryDirPath);
    fileStorageDirPathEdit->setText(state->fileStorageDirPath);
    indexDirectoryEdit->setText(state->indexDirectory);

}

AppSettingsGUIPageState* DirectoriesSettingsPageWidget::getState(QString& err) const {
    Q_UNUSED(err)
    DirectoriesSettingsPageState* state = new DirectoriesSettingsPageState();
    state->downloadsDirPath = downloadsDirPathEdit->text();
    state->documentsDirectory = documentsDirectoryEdit->text();
    state->temporaryDirPath = tmpDirPathEdit->text();
    state->fileStorageDirPath = fileStorageDirPathEdit->text();
    state->indexDirectory = indexDirectoryEdit->text();

    return state;
}

void DirectoriesSettingsPageWidget::sl_browseDownloadsDirButtonClicked() {
    QString path = downloadsDirPathEdit->text();
    QString dir = U2FileDialog::getExistingDirectory(this, tr("Choose Folder"), path,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        downloadsDirPathEdit->setText(dir);
    }
    
}

void DirectoriesSettingsPageWidget::sl_browseDocumentsDirButtonClicked() {
    QString path = documentsDirectoryEdit->text();
    QString dir = U2FileDialog::getExistingDirectory(this, tr("Choose Folder"), path,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        documentsDirectoryEdit->setText(dir);
    }
    
}

void DirectoriesSettingsPageWidget::sl_browseTmpDirButtonClicked() {
    QString path = tmpDirPathEdit->text();
    QString dir = U2FileDialog::getExistingDirectory(this, tr("Choose Folder"), path,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        tmpDirPathEdit->setText(dir);
    }

}

void DirectoriesSettingsPageWidget::sl_browseFileStorageButtonClicked() {
    QString path = fileStorageDirPathEdit->text();
    QString dir = U2FileDialog::getExistingDirectory(this, tr("Choose Folder"), path,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        fileStorageDirPathEdit->setText(dir);
    }
}

void DirectoriesSettingsPageWidget::sl_cleanupStorage() {
    CleanupFileStorageTask *t = new CleanupFileStorageTask();
    AppContext::getTaskScheduler()->registerTopLevelTask(t);
}

void DirectoriesSettingsPageWidget::sl_onIndexDirButton() {
    QString path = indexDirectoryEdit->text();
    QString dir = U2FileDialog::getExistingDirectory(this, tr("Choose Folder"), path,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        indexDirectoryEdit->setText(dir);
    }
}

} //namespace
