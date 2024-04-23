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

#pragma once

#include <QMutex>
#include <QSettings>

#include "U2Core/global.h"
#include <U2Core/Settings.h>

namespace U2 {

class U2PRIVATE_EXPORT SettingsImpl : public Settings {
public:
    SettingsImpl(QSettings::Scope scope);
    ~SettingsImpl();

    QStringList getAllKeys(const QString& path) const override;
    QStringList getChildGroups(const QString& path) const override;
    void cleanSection(const QString& path) override;

    bool contains(const QString& key) const override;
    void remove(const QString& key) override;

    QVariant getValue(const QString& key, const QVariant& defaultValue = QVariant(), bool versionedValue = false, bool pathValue = false) const override;
    void setValue(const QString& key, const QVariant& value, bool versionedValue = false, bool pathValue = false) override;

    QString toVersionKey(const QString& key) const override;
    QString toMinorVersionKey(const QString& key) const override;

    /* Add current ugene working directory path to key */
    QString toPathKey(const QString& key) const override;

    void sync() override;

    QString fileName() const override;

private:
    mutable QMutex threadSafityLock;
    QSettings* settings;
};
}  // namespace U2
