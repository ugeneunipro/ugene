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
struct U2CORE_EXPORT IconParameters {
    IconParameters() = default;
    IconParameters(const QString& _iconCathegory,
               const QString& _iconName,
               bool _hasColorCathegory = true);

    // True if nothing is set
    bool isEmpty() const;

    // Cathegory of the image. This is the prefix, defined in .qrc file
    QString iconCathegory;
    // Icon name
    QString iconName;
    // True, if icon has two implementations (dark and light)
    // False, if there is only one implementation for both color schemes
    bool hasColorCathegory = false;
};

}  // namespace U2

Q_DECLARE_METATYPE(U2::IconParameters)

