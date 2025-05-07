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

#include <QStyleFactory>

namespace U2 {

// Creates a style by given parameters
class StyleFactory {
public:
    // Get all possible styles
    static QStringList keys();

    // Supported color modes
    enum class ColorMode {
        Light,
        Dark,
        Auto
    };
    // Create style by style name and color mode
    static QStyle* create(const QString& styleName, ColorMode colorMode);
    // Create style by style name and color mode
    static QStyle* create(const QString& styleName, int colorMode);

    // True if dark style avaliable
    // Not avaliable on macOS early than 10.14 and
    // Windows early than 10 1809 10.0.17763
    static bool isDarkStyleAvaliable();
    // True if dark style enabled
    static bool isDarkStyleEnabled();


};

}
