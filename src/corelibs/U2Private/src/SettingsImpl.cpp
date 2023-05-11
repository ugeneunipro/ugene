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

#include "SettingsImpl.h"

#include <QCryptographicHash>
#include <QDir>
#include <QProcess>

#include <U2Core/AppContext.h>
#include <U2Core/CMDLineCoreOptions.h>
#include <U2Core/CMDLineRegistry.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/Version.h>

namespace U2 {

static QString findKey(const QStringList& envList, const QString& key) {
    QString prefix = key + "=";
    QString result;
    foreach (const QString& var, envList) {
        if (var.startsWith(prefix)) {
            result = var.mid(prefix.length());
            break;
        }
    }
    return result;
}

static QString preparePath(const QString& pathName) {
    QString result = pathName.trimmed().replace("//", "/").replace("\\", "/");
    if (result.endsWith("/")) {
        result = result.left(result.length() - 1);
    }
    return result;
}

SettingsImpl::SettingsImpl(QSettings::Scope scope) {
    QString fileName;
    QStringList envList = QProcess::systemEnvironment();
    static const QString configFileName = QString("%1.ini").arg(U2_PRODUCT_NAME);

    if (scope == QSettings::UserScope) {
        // check for local cfg file -> portable APP
        bool localCfg = false;
        QDir dir(QDir::current());
        QFileInfoList dirEntries = dir.entryInfoList();
        foreach (const QFileInfo& entry, dirEntries) {
            if (entry.fileName() == configFileName) {
                fileName = entry.filePath();
                localCfg = true;
                break;
            }
        }
        if (!localCfg) {
            fileName = AppContext::getCMDLineRegistry()->getParameterValue(CMDLineCoreOptions::INI_FILE);
            if (fileName.isEmpty()) {
                fileName = findKey(envList, U2_USER_INI);
            }
        }
    } else {
        fileName = findKey(envList, U2_SYSTEM_INI);
    }
    bool isInTestMode = qgetenv("UGENE_GUI_TEST") == "1";
    QSettings::Format format = isOsMac() && !isInTestMode ? QSettings::NativeFormat : QSettings::IniFormat;
    if (fileName.isEmpty()) {
        settings = new QSettings(format, scope, U2_ORGANIZATION_NAME, U2_PRODUCT_NAME, this);
    } else {
        settings = new QSettings(fileName, format, this);
    }
}

SettingsImpl::~SettingsImpl() {
    settings->sync();
}

bool SettingsImpl::contains(const QString& pathName) const {
    QMutexLocker lock(&threadSafityLock);

    QString key = preparePath(pathName);
    return settings->contains(key);
}

void SettingsImpl::remove(const QString& pathName) {
    QMutexLocker lock(&threadSafityLock);

    QString key = preparePath(pathName);
    settings->remove(key);
}

QVariant SettingsImpl::getValue(const QString& pathName, const QVariant& defaultValue, bool versionedValue, bool pathValue) const {
    SAFE_POINT(!pathValue || (pathValue && versionedValue), "'pathValue' must be 'versionedValue'!", defaultValue);
    QMutexLocker lock(&threadSafityLock);

    QString key = preparePath(pathName);

    if (versionedValue) {
        QString keyWithExtras = pathValue ? toPathKey(key) : toVersionKey(key);

        settings->beginGroup(key);
        QStringList allKeys = settings->allKeys();
        settings->endGroup();

        foreach (const QString& settingsKey, allKeys) {
            if (QString(key + "/" + settingsKey) == keyWithExtras) {
                return settings->value(keyWithExtras, defaultValue);
            }
        }
        return defaultValue;
    }
    return settings->value(key, defaultValue);
}

void SettingsImpl::setValue(const QString& pathName, const QVariant& value, bool versionedValue, bool pathValue) {
    SAFE_POINT(!pathValue || (pathValue && versionedValue), "'pathValue' must be 'versionedValue'!", );
    QMutexLocker lock(&threadSafityLock);

    QString key = preparePath(pathName);

    if (pathValue && versionedValue) {
        key = toPathKey(key);
    } else if (versionedValue) {
        // TODO: delete versioned keys?

        // create versioned key
        key = toVersionKey(key);
    }

    settings->setValue(key, value);
}

QString SettingsImpl::toVersionKey(const QString& key) const {
    static QString VERSION_KEY_SUFFIX = "/" + Version::appVersion().toString();
    return key + VERSION_KEY_SUFFIX + (key.endsWith("/") ? "/" : "");
}

QString SettingsImpl::toMinorVersionKey(const QString& key) const {
    static QString VERSION_KEY_SUFFIX = "/" + QString::number(Version::appVersion().major) + "." + QString::number(Version::appVersion().minor);
    return key + VERSION_KEY_SUFFIX + (key.endsWith("/") ? "/" : "");
}

QString SettingsImpl::toPathKey(const QString& key) const {
    static QString PATH_KEY_SUFFIX = "/" + QString(QCryptographicHash::hash(QCoreApplication::applicationDirPath().toUtf8(), QCryptographicHash::Md4).toHex());
    return toVersionKey(key) + PATH_KEY_SUFFIX + (key.endsWith("/") ? "/" : "");
}

QStringList SettingsImpl::getAllKeys(const QString& path) const {
    QMutexLocker lock(&threadSafityLock);

    QString key = preparePath(path);
    settings->beginGroup(key);
    QStringList allKeys = settings->allKeys();
    settings->endGroup();
    return allKeys;
}

QStringList SettingsImpl::getChildGroups(const QString& path) const {
    QMutexLocker lock(&threadSafityLock);

    QString key = preparePath(path);
    settings->beginGroup(key);
    QStringList allKeys = settings->childGroups();
    settings->endGroup();
    return allKeys;
}

void SettingsImpl::cleanSection(const QString& path) {
    QStringList keyList = getAllKeys(path);
    foreach (QString key, keyList) {
        remove(key);
    }
}

void SettingsImpl::sync() {
    QMutexLocker lock(&threadSafityLock);

    settings->sync();
}

QString SettingsImpl::fileName() const {
    return settings->fileName();
}

}  // namespace U2
