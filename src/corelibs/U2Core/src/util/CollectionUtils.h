/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include <QList>
#include <QMap>
#include <QSet>

namespace U2 {

template<typename T>
QSet<T> toSet(const QList<T>& list) {
    return QSet<T>(list.begin(), list.end());
}

template<typename T>
QList<T> toList(const QSet<T>& set) {
    return QList<T>(set.begin(), set.end());
}

template<typename Key, typename Value>
void unite(QMap<Key, Value>& target, const QMap<Key, Value>& other) {
    for (auto it = other.begin(); it != other.end(); ++it) {
        target.insert(it.key(), it.value());
    }
}

template<typename Key, typename Value>
void unite(QHash<Key, Value>& target, const QHash<Key, Value>& other) {
    for (auto it = other.begin(); it != other.end(); ++it) {
        target.insert(it.key(), it.value());
    }
}

}  // namespace U2
