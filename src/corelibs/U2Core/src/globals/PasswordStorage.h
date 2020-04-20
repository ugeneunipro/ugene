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

#ifndef _U2_PASSWORD_STORAGE_H_
#define _U2_PASSWORD_STORAGE_H_

#include <QMap>
#include <QString>

#include <U2Core/StrPackUtils.h>

namespace U2 {

class U2CORE_EXPORT PasswordStorage {
public:
    PasswordStorage();

    void addEntry(const QString& fullUrl, const QString& password, bool remember);
    void removeEntry(const QString& fullUrl);
    QString getEntry(const QString& fullUrl) const;
    bool contains(const QString& fullUrl) const;
    bool isRemembered(const QString& fullUrl) const;
    void setRemembered(const QString& fullUrl, bool remember);

private:
    static void remember(const QString& fullUrl, const QString& password);
    static void forget(const QString &fullUrl);
    static QByteArray serialize(const QString &password);
    static QString deserialize(const QByteArray &data);

    StrStrMap registry;

    static const QString SETTINGS_PATH;
};

}   // namespace U2

#endif // _U2_PASSWORD_STORAGE_H_
