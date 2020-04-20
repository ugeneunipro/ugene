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

#ifndef _U2_USER_APPLICATIONS_SETTINGS_H_
#define _U2_USER_APPLICATIONS_SETTINGS_H_

#include <QObject>

#include <U2Core/U2OpStatus.h>

namespace U2 {

class U2CORE_EXPORT UserAppsSettings : public QObject {
    Q_OBJECT
public:
    UserAppsSettings();
    ~UserAppsSettings();

    QString getWebBrowserURL() const;
    void setWebBrowserURL(const QString& url);

    bool useDefaultWebBrowser() const;
    void setUseDefaultWebBrowser(bool state);

    QString getTranslationFile() const;
    void setTranslationFile(const QString& fn);

    bool openLastProjectAtStartup() const;
    void setOpenLastProjectAtStartup(bool v);

    int getAskToSaveProject() const;
    void setAskToSaveProject(int v);

    //a style selected by user
    QString getVisualStyle() const;
    void setVisualStyle(const QString& newStyle);

    // specify dir for downloaded files
    QString getDownloadDirPath() const;
    void setDownloadDirPath(const QString& newPath) const;

    // storage dir for custom external tool config files
    QString getCustomToolsConfigsDirPath() const;
    void setCustomToolsConfigsDirPath(const QString& newPath) const;

    // recently downloaded file names
    QStringList getRecentlyDownloadedFileNames() const;
    void setRecentlyDownloadedFileNames(const QStringList& fileNames) const;

    // temporary dir path as specified by user
    QString getUserTemporaryDirPath() const;
    void setUserTemporaryDirPath(const QString& newPath);

    // temporary dir for the current process: userTemporaryDirPath + "ugene_tmp/p" + processId + "/domain"
    // domain is used here to avoid collision between different algorithms
    QString getCurrentProcessTemporaryDirPath(const QString& domain = QString()) const;

    // returns the temropary path: userTemporaryDirPath + "ugene_tmp/p" + processId + "/domain/" + tmpSubDir
    QString createCurrentProcessTemporarySubDir(U2OpStatus &os, const QString &domain = QString()) const;

    // default UGENE data dir
    QString getDefaultDataDirPath() const;
    void setDefaultDataDirPath(const QString& newPath);

    bool isStatisticsCollectionEnabled() const;
    void setEnableCollectingStatistics(bool b);

    bool tabbedWindowLayout() const;
    void setTabbedWindowLayout(bool b);

    bool resetSettings() const;
    void setResetSettings(bool b);

    QString getFileStorageDir() const;
    void setFileStorageDir(const QString &newPath);

    bool updatesEnabled() const;
    void setUpdatesEnabled(bool value);

    bool isUpdateSkipped(const QString &versionString) const;
    void skipUpdate(const QString &versionString);

signals:
    void si_temporaryPathChanged();
    void si_windowLayoutChanged();

private:
    bool cleanupTmpDir;
};

}//namespace

#endif
