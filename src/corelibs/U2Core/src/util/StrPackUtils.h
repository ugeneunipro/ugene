/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#ifndef _U2_STR_PACK_UTILS_H_
#define _U2_STR_PACK_UTILS_H_

#include <QBitArray>
#include <QCoreApplication>
#include <QMap>
#include <QRegExp>
#include <QStringList>
#include <QVariant>

#include <U2Core/global.h>

typedef QMap<QString, QString> StrStrMap;
typedef QPair<QString, QString> StrStrPair;

namespace U2 {

class U2CORE_EXPORT StrPackUtils {
    Q_DECLARE_TR_FUNCTIONS(StrPackUtils)
public:
    enum Options {
        SingleQuotes,
        DoubleQuotes
    };

    static QString packStringList(const QStringList &list, Options options = DoubleQuotes);
    static QStringList unpackStringList(const QString &string, Options options = DoubleQuotes);

    static QString packMap(const QVariantMap &map, Options options = DoubleQuotes);
    static QString packMap(const StrStrMap &map, Options options = DoubleQuotes);
    static StrStrMap unpackMap(const QString &string, Options options = DoubleQuotes);

private:
    static QBitArray initCharactersToEscape();

    static QString escapeCharacters(QString string);
    static QString unescapeCharacters(QString string);

    static QString wrapString(const QString &string, Options options = DoubleQuotes);

    static const QBitArray charactersToEscape;
    static const QString LIST_SEPARATOR;
    static const QString MAP_SEPARATOR;
    static const QString PAIR_CONNECTOR;

    static const QString listSeparatorPattern;
    static const QRegExp listSingleQuoteSeparatorRegExp;
    static const QRegExp listDoubleQuoteSeparatorRegExp;

    static const QString mapSeparatorPattern;
    static const QRegExp mapSingleQuoteSeparatorRegExp;
    static const QRegExp mapDoubleQuoteSeparatorRegExp;

    static const QString pairSeparatorPattern;
    static const QRegExp pairSingleQuoteSeparatorRegExp;
    static const QRegExp pairDoubleQuoteSeparatorRegExp;
};

}   // namespace U2

template <>
inline QVariant qVariantFromValue<StrStrMap>(const StrStrMap &map) {
    return qVariantFromValue(U2::StrPackUtils::packMap(map));
}

template<>
inline StrStrMap qvariant_cast<StrStrMap>(const QVariant &variant) {
    return U2::StrPackUtils::unpackMap(qvariant_cast<QString>(variant));
}

#endif // _U2_STR_PACK_UTILS_H_
