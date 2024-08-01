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

#include <QColor>

#include <U2Core/global.h>

namespace U2 {

class U2VIEW_EXPORT McaColors {
public:
    McaColors() = delete;

    static const QColor getChromatogramColorByBase(char base);
    static const QColor getChromatogramColorByBase(bool isDark, char base);
    static const QColor getChromatogramColorById(bool isDark, int id);
    static const QColor getArrowColor(bool isReversed);

private:
    static constexpr int COLORS_NUM = 4;
    static const QColor COLOR_FOR_IDS_LIGHT[COLORS_NUM];
    static const QColor COLOR_FOR_IDS_DARK[COLORS_NUM];

    static const QColor ARROW_DIRECT_COLOR_LIGHT;
    static const QColor ARROW_DIRECT_COLOR_DARK;
    static const QColor ARROW_REVERSE_COLOR_LIGHT;
    static const QColor ARROW_REVERSE_COLOR_DARK;


};

}
