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

#include "McaColors.h"

#include <U2Core/AppContext.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/MainWindow.h>

#include <QPalette>

namespace U2 {

const QColor McaColors::COLOR_FOR_IDS_LIGHT[4] = {Qt::darkGreen, Qt::blue, Qt::black, Qt::red};
const QColor McaColors::COLOR_FOR_IDS_DARK[4] = {Qt::green, Qt::cyan, Qt::white, QColor(255, 127, 127)};

const QColor McaColors::ARROW_DIRECT_COLOR_LIGHT = "blue";
const QColor McaColors::ARROW_DIRECT_COLOR_DARK = "#4EADE1";
const QColor McaColors::ARROW_REVERSE_COLOR_LIGHT = "green";
const QColor McaColors::ARROW_REVERSE_COLOR_DARK = "#03c03c";

const QColor McaColors::getChromatogramColorByBase(char base) {
    auto mw = AppContext::getMainWindow();
    SAFE_POINT_NN(mw, QPalette().text().color());

    return getChromatogramColorByBase(mw->isDarkMode(), base);
}

const QColor McaColors::getChromatogramColorByBase(bool isDark, char base) {
    switch (base) {
        case 'A':
            return isDark ? COLOR_FOR_IDS_DARK[0] : COLOR_FOR_IDS_LIGHT[0];
        case 'C':
            return isDark ? COLOR_FOR_IDS_DARK[1] : COLOR_FOR_IDS_LIGHT[1];
        case 'G':
            return isDark ? COLOR_FOR_IDS_DARK[2] : COLOR_FOR_IDS_LIGHT[2];
        case 'T':
            return isDark ? COLOR_FOR_IDS_DARK[3] : COLOR_FOR_IDS_LIGHT[3];
        default:
            return QPalette().text().color();
    }
}

const QColor McaColors::getChromatogramColorById(bool isDark, int id) {
    SAFE_POINT(0 <= id && id < COLORS_NUM, "Incorrect color id", QPalette().text().color());

    return isDark ? COLOR_FOR_IDS_DARK[id] : COLOR_FOR_IDS_LIGHT[id];
}

const QColor McaColors::getArrowColor(bool isReversed) {
    QColor result;
    if (AppContext::getMainWindow()->isDarkMode()) {
        result = isReversed ? ARROW_REVERSE_COLOR_DARK : ARROW_DIRECT_COLOR_DARK;
    } else {
        result = isReversed ? ARROW_REVERSE_COLOR_LIGHT : ARROW_DIRECT_COLOR_LIGHT;
    }

    return result;
}

}