/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "GTUtilsQt.h"

namespace HI {

bool GTUtilsQt::compareVariants(const QVariant& variant1, const QVariant& variant2) {
    // If types differ, the QVariants cannot be equal.
    if (variant1.userType() != variant2.userType())
        return false;

    // Handle direct comparison for supported types (Qt handles many primitive types itself).
    if (variant1 == variant2)
        return true;

    // For containers, compare element-wise.
    switch (variant1.userType()) {
        case QMetaType::QVariantList: {
            const QVariantList list1 = variant1.toList();
            const QVariantList list2 = variant2.toList();
            if (list1.size() != list2.size()) {
                return false;
            }
            for (int i = 0; i < list1.size(); ++i) {
                if (!compareVariants(list1[i], list2[i]))
                    return false;
            }
            return true;
        }

        case QMetaType::QVariantMap: {
            const QVariantMap map1 = variant1.toMap();
            const QVariantMap map2 = variant2.toMap();
            if (map1.size() != map2.size()) {
                return false;
            }
            for (auto it = map1.constBegin(); it != map1.constEnd(); ++it) {
                if (!map2.contains(it.key()) || !compareVariants(it.value(), map2[it.key()]))
                    return false;
            }
            return true;
        }

        case QMetaType::QVariantHash: {
            const QVariantHash hash1 = variant1.toHash();
            const QVariantHash hash2 = variant2.toHash();
            if (hash1.size() != hash2.size()) {
                return false;
            }
            for (auto it = hash1.constBegin(); it != hash1.constEnd(); ++it) {
                if (!hash2.contains(it.key()) || !compareVariants(it.value(), hash2[it.key()]))
                    return false;
            }
            return true;
        }

        case QMetaType::QStringList: {
            const QStringList list1 = variant1.toStringList();
            const QStringList list2 = variant2.toStringList();
            return list1 == list2;
        }

        // Add cases for other container types when needed.
        default:
            if (variant1.canConvert<QList<QString>>() && variant2.canConvert<QList<QString>>()) {
                QList<QString> list1 = variant1.value<QList<QString>>();
                QList<QString> list2 = variant2.value<QList<QString>>();
                return list1 == list2;
            }
            GT_LOG(QString("Unsupported type for deep comparison: ") + variant1.typeName());
            return false;
    }
}

}  // namespace HI
