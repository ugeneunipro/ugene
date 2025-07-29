/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include <U2Core/global.h>

namespace U2 {

// Parameters, which define an image
struct U2CORE_EXPORT IconRef {
    IconRef() = default;
    IconRef(const QString& _iconModule,
               const QString& _iconName,
               const QString& _innerDirName = QString());

    // Convert IconRef to QVariant
    QVariant toVariant() const;

    // True if nothing is set
    bool isEmpty() const;

    // Module of the image. This is the prefix, defined in .qrc file
    QString iconModule;
    // Icon name
    QString iconName;
    // Inner directory name - if image is in some inner directory (e.g. welcome_page in ugeneui)
    QString innerDirName;
};

}  // namespace U2

// Metatype for IconRef to serialize it in QVariant
// Search for ICON_DATA_REF in the codebase to find where it is used
Q_DECLARE_METATYPE(U2::IconRef)

