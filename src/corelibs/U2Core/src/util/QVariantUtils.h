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

#ifndef _U2_QVARUTILS_H_
#define _U2_QVARUTILS_H_

#include <QDataStream>
#include <QList>
#include <QMap>
#include <QString>
#include <QVariant>

#include <U2Core/U2SafePoints.h>

namespace U2 {

class U2CORE_EXPORT QVariantUtils {
public:
    static QVariant String2Var(const QString& string) {
        QDataStream s(QByteArray::fromBase64(string.toLatin1()));
        return QVariant(s);
    }

    static QVariantMap string2Map(const QString& string, bool emptyMapIfError) {
        QDataStream s(QByteArray::fromBase64(string.toLatin1()));
        QVariant res(QVariant::Map);
        s >> res;
        if (res.type() == QVariant::Map) {
            return res.toMap();
        }
        SAFE_POINT(emptyMapIfError, "Empty variant map detected!", QVariantMap());
        return QVariantMap();
    }

    static QString var2String(const QVariant& v) {
        QByteArray a;
        QDataStream s(&a, QIODevice::WriteOnly);
        s << v;
        QString res(a.toBase64());
        return res;
    }

    static QString map2String(const QVariantMap& map) {
        return var2String(QVariant(map));
    }

    static QVariant addStr2List(const QVariant& v, const QString& s) {
        if (v.canConvert(QVariant::StringList)) {
            QStringList l = v.toStringList();
            return (l << s);
        } else {
            SAFE_POINT(v.type() == QVariant::Invalid, "Unexpected variant type!", QVariant(s));
        }
        return QVariant(s);
    }

    static QVariant addStr2List(const QVariant& v, const QStringList& sl) {
        if (v.canConvert(QVariant::StringList)) {
            QStringList l = v.toStringList();
            return QVariant(l += sl);
        } else {
            SAFE_POINT(v.type() == QVariant::Invalid, "Unexpected variant type!", QVariant(sl));
        }
        return QVariant(sl);
    }
};

} //namespace
#endif
