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

#include "GTMouseUtils.h"
#include "GTGlobals.h"
#include "drivers/GTMouseDriver.h"
#include <QtGui/QCursor>

namespace HI {

#define GT_CLASS_NAME "GTMouse"

#define GT_METHOD_NAME "moveCursorToWidget"
void GTMouseUtils::moveCursorToWidget(GUITestOpStatus &os, QWidget *widget) {
    GT_CHECK(widget != NULL, "Provided widget is null");
    QPoint widgetCenter = widget->rect().center();
    GTMouseDriver::moveTo(widgetCenter);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "moveCursorOutOfWidget"
void GTMouseUtils::moveCursorOutOfWidget(GUITestOpStatus &os, QWidget *widget) {
    GT_CHECK(widget != NULL, "Provided widget is null");
    QPoint currentPosition = QCursor::pos();
    GT_CHECK(widget->rect().contains(currentPosition, false), "Cursor not over widget");
    QPoint finalPosition = widget->rect().topLeft() + QPoint(1, 1); //top left + offset
    GTMouseDriver::moveTo(finalPosition);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}
