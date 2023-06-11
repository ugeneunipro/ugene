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
#include <QPoint>
#include <QScrollBar>
#include <QStyleOptionSlider>

#include "GTGlobals.h"

namespace HI {

class HI_EXPORT GTScrollBar {
public:
    static QScrollBar* getScrollBar(const QString& scrollBarName);

    /** Returns current value of the scrollbar. */
    static int getValue(QScrollBar* scrollbar);

    static void pageUp(QScrollBar* scrollbar, GTGlobals::UseMethod useMethod);
    static void pageDown(QScrollBar* scrollbar, GTGlobals::UseMethod useMethod);

    static void lineUp(QScrollBar* scrollbar, GTGlobals::UseMethod useMethod);  // does not necessarily move one line up (for example, moves cursor in text editors)
    static void lineDown(QScrollBar* scrollbar, GTGlobals::UseMethod useMethod);  // does not necessarily move one line down (for example, moves cursor in text editors)

    static void moveSliderWithMouseUp(QScrollBar* scrollbar, int nPix);
    static void moveSliderWithMouseDown(QScrollBar* scrollbar, int nPix);
    static void moveSliderWithMouseToValue(QScrollBar* scrollbar, int value);

    static void moveSliderWithMouseWheelUp(QScrollBar* scrollbar, int nScrolls);  // first moves the cursor to the slider and clicks it, then starts scrolling
    static void moveSliderWithMouseWheelDown(QScrollBar* scrollbar, int nScrolls);  // first moves the cursor to the slider and clicks it, then starts scrolling

    static QPoint getSliderPosition(QScrollBar* scrollbar);
    static QPoint getUpArrowPosition(QScrollBar* scrollbar);
    static QPoint getDownArrowPosition(QScrollBar* scrollbar);
    static QPoint getAreaUnderSliderPosition(QScrollBar* scrollbar);
    static QPoint getAreaOverSliderPosition(QScrollBar* scrollbar);

private:
    static QStyleOptionSlider initScrollbarOptions(QScrollBar* scrollbar);
};

}  // namespace HI
