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

#include "UserApplicationsSettings.h"

#include <QApplication>
#include <QDialogButtonBox>
#include <QDir>
#include <QProcessEnvironment>
#include <QStandardPaths>
#include <QStyle>

#ifdef Q_OS_WIN
#    include <QStyleFactory>
#endif

#include <U2Core/AppContext.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/Settings.h>
#include <U2Core/U2OpStatusUtils.h>

namespace U2 {

#define SETTINGS_ROOT QString("/user_apps/")
#define WEB_BROWSER QString("web_browser")
#define USE_DEFAULT_WEB_BROWSER QString("use_default_web_browser")
#define TRANSLATION QString("translation_file")
#define LAST_PROJECT_FLAG QString("open_last_project")
#define SAVE_PROJECT_STATE QString("save_project")
#define VISUAL_STYLE QString("style")
#define DOWNLOAD_DIR QString("download_file")
#define CUSTOM_EXTERNAL_TOOL_CONFIGS_DIR QString("custom_external_tool_configs_dir")
#define RECENTLY_DOWNLOADED QString("recently_downloaded")
#define TEMPORARY_DIR QString("temporary_dir")
#define DATA_DIR QString("data_dir")
#define COLLECTING_STATISTICS QString("collecting_statistics")
#define WINDOW_LAYOUT QString("tabbed_windows")
#define RESET_SETTINGS_FLAG QString("reset_settings")
#define FILE_STORAGE_DIR QString("file_storage_dir")
#define UPDATES_ENABLED QString("check_updates")
#define SKIP_UPDATE_PREFIX QString("skip_update_")
#define ENABLE_EXPERIMENTAL_MODE QString("enable_experiments")

// TODO: create a special ENV header to keep all env-vars ugene depends
#define UGENE_SKIP_TMP_DIR_CLEANUP "UGENE_SKIP_TMP_DIR_CLEANUP"
#define UGENE_ENABLE_EXPERIMENTAL_FEATURES "UGENE_ENABLE_EXPERIMENTAL_FEATURES"

UserAppsSettings::UserAppsSettings() {
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    cleanupTmpDir = !env.contains(UGENE_SKIP_TMP_DIR_CLEANUP);
    if (env.contains(UGENE_ENABLE_EXPERIMENTAL_FEATURES)) {
        setExperimentalFeaturesModeEnabled(true);
    }
}

UserAppsSettings::~UserAppsSettings() {
    if (cleanupTmpDir) {
        QString path = getCurrentProcessTemporaryDirPath();
        coreLog.trace(tr("Cleaning temp dir: %1").arg(path));
        U2OpStatus2Log os;
        GUrlUtils::removeDir(path, os);
    }
}

QString UserAppsSettings::getWebBrowserURL() const {
    return AppContext::getSettings()->getValue(SETTINGS_ROOT + WEB_BROWSER, QString("")).toString();
}

bool UserAppsSettings::useDefaultWebBrowser() const {
    return AppContext::getSettings()->getValue(SETTINGS_ROOT + USE_DEFAULT_WEB_BROWSER, true).toBool();
}

void UserAppsSettings::setWebBrowserURL(const QString& url) {
    return AppContext::getSettings()->setValue(SETTINGS_ROOT + WEB_BROWSER, url);
}
void UserAppsSettings::setUseDefaultWebBrowser(bool state) {
    return AppContext::getSettings()->setValue(SETTINGS_ROOT + USE_DEFAULT_WEB_BROWSER, state);
}

QString UserAppsSettings::getTranslationFile() const {
    return AppContext::getSettings()->getValue(SETTINGS_ROOT + TRANSLATION, QString("")).toString();
}
void UserAppsSettings::setTranslationFile(const QString& fn) {
    return AppContext::getSettings()->setValue(SETTINGS_ROOT + TRANSLATION, fn);
}

bool UserAppsSettings::openLastProjectAtStartup() const {
    return AppContext::getSettings()->getValue(SETTINGS_ROOT + LAST_PROJECT_FLAG, true).toBool();
}

void UserAppsSettings::setOpenLastProjectAtStartup(bool v) {
    return AppContext::getSettings()->setValue(SETTINGS_ROOT + LAST_PROJECT_FLAG, v);
}

int UserAppsSettings::getAskToSaveProject() const {
    return AppContext::getSettings()->getValue(SETTINGS_ROOT + SAVE_PROJECT_STATE, QDialogButtonBox::NoButton).toInt();
}

void UserAppsSettings::setAskToSaveProject(int v) {
    AppContext::getSettings()->setValue(SETTINGS_ROOT + SAVE_PROJECT_STATE, v);
}

bool UserAppsSettings::resetSettings() const {
    return AppContext::getSettings()->getValue(SETTINGS_ROOT + RESET_SETTINGS_FLAG, false).toBool();
}

void UserAppsSettings::setResetSettings(bool b) {
    AppContext::getSettings()->setValue(SETTINGS_ROOT + RESET_SETTINGS_FLAG, b);
}

QString UserAppsSettings::getVisualStyle() const {
    QString defaultStyle = QApplication::style()->objectName();

#ifdef Q_OS_WIN
#    define DEFAULT_STYLE_NAME ".NET"
    const char* version = qVersion();
    if (QString("4.4.0") != version) {
        if (QStyleFactory::keys().contains(DEFAULT_STYLE_NAME)) {
            defaultStyle = DEFAULT_STYLE_NAME;
        }
    }
#endif

    return AppContext::getSettings()->getValue(SETTINGS_ROOT + VISUAL_STYLE, defaultStyle).toString();
}

void UserAppsSettings::setVisualStyle(const QString& newStyle) {
    return AppContext::getSettings()->setValue(SETTINGS_ROOT + VISUAL_STYLE, newStyle.toLower());
}

QString UserAppsSettings::getDownloadDirPath() const {
    return AppContext::getSettings()->getValue(SETTINGS_ROOT + DOWNLOAD_DIR, QDir::homePath() + "/.UGENE_downloaded").toString();
}

void UserAppsSettings::setDownloadDirPath(const QString& newPath) const {
    AppContext::getSettings()->setValue(SETTINGS_ROOT + DOWNLOAD_DIR, newPath);
}

QString UserAppsSettings::getCustomToolsConfigsDirPath() const {
    const QString defaultDir = GUrl(AppContext::getSettings()->fileName()).dirPath() + "/CustomExternalToolConfig";
    return AppContext::getSettings()->getValue(SETTINGS_ROOT + CUSTOM_EXTERNAL_TOOL_CONFIGS_DIR, defaultDir).toString();
}

void UserAppsSettings::setCustomToolsConfigsDirPath(const QString& newPath) const {
    const QString oldPath = getCustomToolsConfigsDirPath();

    Settings* s = AppContext::getSettings();
    s->setValue(SETTINGS_ROOT + CUSTOM_EXTERNAL_TOOL_CONFIGS_DIR, newPath);

    if (oldPath != newPath) {
        QDir dir(oldPath);
        if (!dir.exists()) {
            return;
        }

        dir.setNameFilters(QStringList() << "*.xml");
        QFileInfoList fileList = dir.entryInfoList();
        foreach (const QFileInfo& fileInfo, fileList) {
            const QString newFileUrl = newPath + "/" + fileInfo.fileName();
            QFile::copy(fileInfo.filePath(), newFileUrl);
        }
    }
}

QStringList UserAppsSettings::getRecentlyDownloadedFileNames() const {
    QStringList empty;
    return AppContext::getSettings()->getValue(SETTINGS_ROOT + RECENTLY_DOWNLOADED, empty).toStringList();
}

void UserAppsSettings::setRecentlyDownloadedFileNames(const QStringList& fileNames) const {
    AppContext::getSettings()->setValue(SETTINGS_ROOT + RECENTLY_DOWNLOADED, fileNames);
}

QString UserAppsSettings::getUserTemporaryDirPath() const {
    return AppContext::getSettings()->getValue(SETTINGS_ROOT + TEMPORARY_DIR, QStandardPaths::writableLocation(QStandardPaths::TempLocation)).toString();
}

void UserAppsSettings::setUserTemporaryDirPath(const QString& newPath) {
    AppContext::getSettings()->setValue(SETTINGS_ROOT + TEMPORARY_DIR, newPath);
    emit si_temporaryPathChanged();
}

// TODO: merge with getFileStorageDir.
QString UserAppsSettings::getDefaultDataDirPath() const {
    QString defaultDataDirPath = qgetenv("UGENE_SAVE_DATA_DIR");
    if (defaultDataDirPath.isEmpty()) {
        defaultDataDirPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/" + "UGENE_Data";
    }
    QString dataDirPath = AppContext::getSettings()->getValue(SETTINGS_ROOT + DATA_DIR, defaultDataDirPath).toString();
    QDir dir(dataDirPath);
    if (!dir.exists(dataDirPath)) {
        dir.mkpath(dataDirPath);
    }
    return dataDirPath;
}

void UserAppsSettings::setDefaultDataDirPath(const QString& newPath) {
    AppContext::getSettings()->setValue(SETTINGS_ROOT + DATA_DIR, newPath);
}

bool UserAppsSettings::isStatisticsCollectionEnabled() const {
    return AppContext::getSettings()->getValue(SETTINGS_ROOT + COLLECTING_STATISTICS, true).toBool();
}

void UserAppsSettings::setEnableCollectingStatistics(bool b) {
    AppContext::getSettings()->setValue(SETTINGS_ROOT + COLLECTING_STATISTICS, b);
}

bool UserAppsSettings::tabbedWindowLayout() const {
    return AppContext::getSettings()->getValue(SETTINGS_ROOT + WINDOW_LAYOUT, true, true).toBool();
}

void UserAppsSettings::setTabbedWindowLayout(bool b) {
    AppContext::getSettings()->setValue(SETTINGS_ROOT + WINDOW_LAYOUT, b, true);
    emit si_windowLayoutChanged();
}

QString UserAppsSettings::getCurrentProcessTemporaryDirPath(const QString& domain) const {
    qint64 pid = QCoreApplication::applicationPid();
    QString tmpDirPath = getUserTemporaryDirPath() + "/" + QString("ugene_tmp/p%1").arg(pid);
    if (!domain.isEmpty()) {
        tmpDirPath += "/" + domain;
    }
    return tmpDirPath;
}

static const int MAX_ATTEMPTS = 500;

QString UserAppsSettings::createCurrentProcessTemporarySubDir(U2OpStatus& os, const QString& domain) const {
    QDir baseDir(getCurrentProcessTemporaryDirPath(domain));
    if (!baseDir.exists()) {
        bool created = baseDir.mkpath(baseDir.absolutePath());
        if (!created) {
            os.setError(QString("Can not create the folder: %1").arg(baseDir.absolutePath()));
            return "";
        }
    }

    qint64 time = QDateTime::currentDateTime().toSecsSinceEpoch() / 1000;
    QString baseDirName = QByteArray::number(time);

    // create sub dir
    int idx = 0;
    QString result;
    bool created;
    do {
        result = baseDirName + "_" + QByteArray::number(idx);
        created = baseDir.mkdir(result);
        idx++;

        if (idx > MAX_ATTEMPTS) {
            os.setError(QString("Can not create a sub-folder in: %1").arg(baseDir.absolutePath()));
            return "";
        }
    } while (!created);

    return baseDir.absolutePath() + "/" + result;
}

QString UserAppsSettings::getFileStorageDir() const {
    QString defaultSaveFilesDir = qgetenv("UGENE_SAVE_FILES_DIR");
    if (defaultSaveFilesDir.isEmpty()) {
        defaultSaveFilesDir = QDir::homePath() + "/.UGENE_files";
    }
    return AppContext::getSettings()->getValue(SETTINGS_ROOT + FILE_STORAGE_DIR, defaultSaveFilesDir).toString();
}

void UserAppsSettings::setFileStorageDir(const QString& newPath) {
    AppContext::getSettings()->setValue(SETTINGS_ROOT + FILE_STORAGE_DIR, newPath);
}

bool UserAppsSettings::updatesEnabled() const {
    return AppContext::getSettings()->getValue(SETTINGS_ROOT + UPDATES_ENABLED, true).toBool();
}

void UserAppsSettings::setUpdatesEnabled(bool value) {
    AppContext::getSettings()->setValue(SETTINGS_ROOT + UPDATES_ENABLED, value);
}

bool UserAppsSettings::isUpdateSkipped(const QString& versionString) const {
    return AppContext::getSettings()->getValue(SETTINGS_ROOT + SKIP_UPDATE_PREFIX + versionString, false).toBool();
}

void UserAppsSettings::skipUpdate(const QString& versionString) {
    AppContext::getSettings()->setValue(SETTINGS_ROOT + SKIP_UPDATE_PREFIX + versionString, true);
}

bool UserAppsSettings::isExperimentalFeaturesModeEnabled() const {
    return AppContext::getSettings()->getValue(SETTINGS_ROOT + ENABLE_EXPERIMENTAL_MODE, false).toBool();
}

void UserAppsSettings::setExperimentalFeaturesModeEnabled(bool flag) {
    AppContext::getSettings()->setValue(SETTINGS_ROOT + ENABLE_EXPERIMENTAL_MODE, flag);
}

}  // namespace U2
