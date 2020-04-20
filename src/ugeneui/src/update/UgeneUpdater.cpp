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

#include <QCoreApplication>
#include <QFile>
#include <QProcess>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/Settings.h>
#include <U2Core/UserApplicationsSettings.h>
#include <U2Core/Version.h>
#include <U2Gui/GUIUtils.h>

#include "UgeneUpdater.h"

namespace U2 {

QScopedPointer<UgeneUpdater> UgeneUpdater::instance(NULL);
QMutex UgeneUpdater::mutex;

UgeneUpdater * UgeneUpdater::getInstance() {
    QMutexLocker lock(&mutex);
    if (NULL != instance.data()) {
        return instance.data();
    }
    instance.reset(new UgeneUpdater());
    return instance.data();
}

void UgeneUpdater::release() {
    QMutexLocker lock(&mutex);
    delete instance.take();
}

void UgeneUpdater::onClose() {
    if (instance->isUpdateOnClose()) {
        instance->startMaintenanceTool();
    }
    release();
}

UgeneUpdater::UgeneUpdater()
: updateOnClose(false)
{

}

void UgeneUpdater::update() {
    if (hasMaintenanceTool()) {
        setUpdateOnClose(true);
        emit si_update();
    } else {
        GUIUtils::runWebBrowser("http://ugene.net/download.html");
    }
}

void UgeneUpdater::setUpdateOnClose(bool value) {
    updateOnClose = value;
}

bool UgeneUpdater::isUpdateOnClose() const {
    return updateOnClose;
}

bool UgeneUpdater::isUpdateSkipped(const Version &version) {
    UserAppsSettings *settings = AppContext::getAppSettings()->getUserAppsSettings();
    return settings->isUpdateSkipped(version.text);
}

void UgeneUpdater::skipUpdate(const Version &version) {
    UserAppsSettings *settings = AppContext::getAppSettings()->getUserAppsSettings();
    settings->skipUpdate(version.text);
}

bool UgeneUpdater::isEnabled() {
    UserAppsSettings *settings = AppContext::getAppSettings()->getUserAppsSettings();
    return settings->updatesEnabled();
}

void UgeneUpdater::setEnabled(bool value) {
    UserAppsSettings *settings = AppContext::getAppSettings()->getUserAppsSettings();
    settings->setUpdatesEnabled(value);
}

QString UgeneUpdater::getMaintenanceToolPath() const {
#ifdef Q_OS_MAC
    QString path = QCoreApplication::applicationDirPath() + "/../../setup.app/Contents/MacOS/setup";
#else
    QString path = QCoreApplication::applicationDirPath() + "/setup";
#endif
#ifdef Q_OS_WIN
    path += ".exe";
#endif
    return path;
}

bool UgeneUpdater::hasMaintenanceTool() const {
    return QFile::exists(getMaintenanceToolPath());
}

void UgeneUpdater::startMaintenanceTool() const {
    QStringList arguments;
    arguments << "--updater";
    QProcess::startDetached(getMaintenanceToolPath(), arguments);
}

} // U2
