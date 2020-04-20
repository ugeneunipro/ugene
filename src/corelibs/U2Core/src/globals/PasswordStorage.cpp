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

#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>

#include "PasswordStorage.h"

namespace U2 {

const QString PasswordStorage::SETTINGS_PATH = "/user_credentials/";

PasswordStorage::PasswordStorage() {
    const QStringList keys = AppContext::getSettings()->getAllKeys(SETTINGS_PATH);
    foreach(const QString& fullUrl, keys) {
        const QString password = deserialize(AppContext::getSettings()->getValue(SETTINGS_PATH + fullUrl).toByteArray());
        registry.insert(fullUrl, password);
    }
}

void PasswordStorage::addEntry(const QString& fullUrl, const QString &password, bool rememberEntry) {
    registry.insert(fullUrl, password);
    if (rememberEntry) {
        remember(fullUrl, password);
    } else {
        forget(fullUrl);
    }
}

void PasswordStorage::removeEntry(const QString& fullUrl) {
    registry.remove(fullUrl);
    forget(fullUrl);
}

QString PasswordStorage::getEntry(const QString& fullUrl) const {
    return registry.value(fullUrl);
}

bool PasswordStorage::contains(const QString& fullUrl) const {
    return registry.contains(fullUrl);
}

bool PasswordStorage::isRemembered(const QString& fullUrl) const {
    return AppContext::getSettings()->contains(SETTINGS_PATH + fullUrl);
}

void PasswordStorage::setRemembered(const QString &fullUrl, bool rememberValue) {
    if (rememberValue) {
        remember(fullUrl, registry.value(fullUrl));
    } else {
        forget(fullUrl);
    }
}

void PasswordStorage::remember(const QString& fullUrl, const QString &password) {
    AppContext::getSettings()->setValue(SETTINGS_PATH + fullUrl, serialize(password));
}

void PasswordStorage::forget(const QString &fullUrl) {
    AppContext::getSettings()->remove(SETTINGS_PATH + fullUrl);
}

QByteArray PasswordStorage::serialize(const QString &password) {
    return password.toLatin1().toBase64();
}

QString PasswordStorage::deserialize(const QByteArray &data) {
    return QByteArray::fromBase64(data);
}

}   // namespce U2
