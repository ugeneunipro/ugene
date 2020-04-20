/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2017 UniPro <ugene@unipro.ru>
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

#ifndef _HI_GUI_GTSCROLLBAR_H_
#define _HI_GUI_GTSCROLLBAR_H_

#include "GTGlobals.h"

#include <QPoint>
#include <QScrollBar>
#include <QStyleOptionSlider>

namespace HI {

class HI_EXPORT GTScrollBar {
public:
    static QScrollBar *getScrollBar(GUITestOpStatus &os, const QString &scrollBarName);

    static void pageUp(GUITestOpStatus &os, QScrollBar *scrollbar, GTGlobals::UseMethod useMethod);
    static void pageDown(GUITestOpStatus &os, QScrollBar *scrollbar, GTGlobals::UseMethod useMethod);

    static void lineUp(GUITestOpStatus &os, QScrollBar *scrollbar, GTGlobals::UseMethod useMethod); //does not necessarily move one line up (for example, moves cursor in text editors)
    static void lineDown(GUITestOpStatus &os, QScrollBar *scrollbar, GTGlobals::UseMethod useMethod); //does not necessarily move one line down (for example, moves cursor in text editors)

    static void moveSliderWithMouseUp(GUITestOpStatus &os, QScrollBar *scrollbar, int nPix);
    static void moveSliderWithMouseDown(GUITestOpStatus &os, QScrollBar *scrollbar, int nPix);
    static void moveSliderWithMouseToValue(GUITestOpStatus &os, QScrollBar *scrollbar, int value);

    static void moveSliderWithMouseWheelUp(GUITestOpStatus &os, QScrollBar *scrollbar, int nScrolls); //first moves the cursor to the slider and clicks it, then starts scrolling
    static void moveSliderWithMouseWheelDown(GUITestOpStatus &os, QScrollBar *scrollbar, int nScrolls); //first moves the cursor to the slider and clicks it, then starts scrolling

    static QPoint getSliderPosition(GUITestOpStatus &os, QScrollBar *scrollbar);
    static QPoint getUpArrowPosition(GUITestOpStatus &os, QScrollBar *scrollbar);
    static QPoint getDownArrowPosition(GUITestOpStatus &os, QScrollBar *scrollbar);
    static QPoint getAreaUnderSliderPosition(GUITestOpStatus &os, QScrollBar *scrollbar);
    static QPoint getAreaOverSliderPosition(GUITestOpStatus &os, QScrollBar *scrollbar);

private:
    static QStyleOptionSlider initScrollbarOptions(GUITestOpStatus &os, QScrollBar *scrollbar);
};

} //namespace

#endif
