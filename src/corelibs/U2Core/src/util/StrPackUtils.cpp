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

#include <math.h>

#include <U2Core/U2SafePoints.h>

#include "StrPackUtils.h"

namespace U2 {

const QBitArray StrPackUtils::charactersToEscape = StrPackUtils::initCharactersToEscape();
const QString StrPackUtils::LIST_SEPARATOR = ",";
const QString StrPackUtils::MAP_SEPARATOR = ";";
const QString StrPackUtils::PAIR_CONNECTOR = "=";

const QString StrPackUtils::listSeparatorPattern = QString("^\\%2|(?!\\\\)\\%2%1\\%2|\\%2$").arg(LIST_SEPARATOR);
const QRegExp StrPackUtils::listSingleQuoteSeparatorRegExp(listSeparatorPattern.arg("\'"));
const QRegExp StrPackUtils::listDoubleQuoteSeparatorRegExp(listSeparatorPattern.arg("\""));

const QString StrPackUtils::mapSeparatorPattern = QString("(?!\\\\)\\%2%1\\%2").arg(MAP_SEPARATOR);
const QRegExp StrPackUtils::mapSingleQuoteSeparatorRegExp(mapSeparatorPattern.arg("\'"));
const QRegExp StrPackUtils::mapDoubleQuoteSeparatorRegExp(mapSeparatorPattern.arg("\""));

const QString StrPackUtils::pairSeparatorPattern = QString("^\\%2|(?!\\\\)\\%2%1\\%2|\\%2$").arg(PAIR_CONNECTOR);
const QRegExp StrPackUtils::pairSingleQuoteSeparatorRegExp(pairSeparatorPattern.arg("\'"));
const QRegExp StrPackUtils::pairDoubleQuoteSeparatorRegExp(pairSeparatorPattern.arg("\""));

QString StrPackUtils::packStringList(const QStringList& list, Options options) {
    QString packedList;
    foreach (const QString& string, list) {
        packedList += wrapString(escapeCharacters(string), options) + LIST_SEPARATOR;
    }
    packedList.chop(LIST_SEPARATOR.size());
    return packedList;
}

QStringList StrPackUtils::unpackStringList(const QString& string, Options options) {
    QStringList unpackedList;
    const QRegExp separator = (options == SingleQuotes ? listSingleQuoteSeparatorRegExp : listDoubleQuoteSeparatorRegExp);
    foreach (const QString& escapedString, string.split(separator, QString::SkipEmptyParts)) {
        unpackedList << unescapeCharacters(escapedString);
    }
    return unpackedList;
}

QString StrPackUtils::packMap(const QVariantMap& map, Options options) {
    StrStrMap newMap;
    foreach (const QString& key, map.keys()) {
        QVariant mapValue = map.value(key);
        SAFE_POINT(mapValue.canConvert<QString>(), "QVariant value can't be converted to string", QString());

        newMap.insert(key, map.value(key).toString());
    }
    return packMap(newMap, options);
}

QString StrPackUtils::packMap(const StrStrMap& map, Options options) {
    QString string;
    foreach (const QString& key, map.keys()) {
        string += wrapString(escapeCharacters(key), options) +
                  PAIR_CONNECTOR +
                  wrapString(escapeCharacters(map[key]), options) +
                  MAP_SEPARATOR;
    }
    string.chop(MAP_SEPARATOR.size());
    return string;
}

StrStrMap StrPackUtils::unpackMap(const QString& string, Options options) {
    StrStrMap map;
    QRegExp elementsSeparator = options == SingleQuotes ? mapSingleQuoteSeparatorRegExp : mapDoubleQuoteSeparatorRegExp;
    foreach (const QString& pair, string.split(elementsSeparator, QString::SkipEmptyParts)) {
        QRegExp keyValueSeparator = options == SingleQuotes ? pairSingleQuoteSeparatorRegExp : pairDoubleQuoteSeparatorRegExp;
        QStringList splitPair = pair.split(keyValueSeparator, QString::SkipEmptyParts);
        Q_ASSERT(splitPair.size() <= 2);
        if (!splitPair.empty()) {  // splitPair can be empty if both key and value are empty strings.
            map.insert(splitPair.first(), splitPair.size() > 1 ? splitPair[1] : "");
        }
    }
    return map;
}

QBitArray StrPackUtils::initCharactersToEscape() {
    QBitArray map(pow(2, 8 * sizeof(char)));
    map[(int)'\\'] = true;
    map[(int)'\"'] = true;
    map[(int)'\''] = true;
    return map;
}

QString StrPackUtils::escapeCharacters(QString string) {
    string.replace('\\', QString("\\\\"));  // escape '\' first
    for (int i = 0; i < charactersToEscape.size(); i++) {
        if (charactersToEscape[i] && (char)i != '\\') {
            const char c = (char)i;
            string.replace(c, QString("\\") + c);
        }
    }

    return string;
}

QString StrPackUtils::unescapeCharacters(QString string) {
    for (int i = 0; i < charactersToEscape.size(); i++) {
        if (charactersToEscape[i]) {
            const char c = (char)i;
            string.replace(QString("\\") + c, QString(1, c));
        }
    }

    return string;
}

QString StrPackUtils::wrapString(const QString& string, Options options) {
    const QString wrapChar = (options == SingleQuotes ? "\'" : "\"");
    return wrapChar + string + wrapChar;
}

}  // namespace U2
