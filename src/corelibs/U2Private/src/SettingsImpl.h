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

#ifndef _U2_SETTINGS_IMPL
#define _U2_SETTINGS_IMPL

#include "U2Core/global.h"
#include <U2Core/Settings.h>

#include <QSettings>
#include <QMutex>

namespace U2 {

class U2PRIVATE_EXPORT SettingsImpl : public Settings {
public:
    SettingsImpl(QSettings::Scope scope);
    ~SettingsImpl();

    QStringList getAllKeys(const QString& path) const;
    QStringList getChildGroups(const QString& path) const;
    void cleanSection(const QString& path);

    virtual bool contains(const QString& key) const;
    virtual void remove(const QString& key);

    virtual QVariant getValue(const QString& key, const QVariant& defaultValue = QVariant(), bool versionedValue = false) const;
    virtual void setValue(const QString& key, const QVariant& value, bool versionedValue = false);

    virtual QString toVersionKey(const QString& key) const;
    virtual QString toMinorVersionKey(const QString& key) const;

    virtual void sync();

    virtual QString fileName() const;

private:
    mutable QMutex  threadSafityLock;
    QSettings* settings;
};
}//namespace
#endif
