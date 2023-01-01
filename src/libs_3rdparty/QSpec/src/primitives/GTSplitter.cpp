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

#include "GTSplitter.h"

#include "drivers/GTMouseDriver.h"

namespace HI {

#define GT_CLASS_NAME "GTSplitter"

#define GT_METHOD_NAME "getHandleRect"
QRect GTSplitter::getHandleRect(GUITestOpStatus& os, QSplitter* splitter, int handleNumber) {
    GT_CHECK_RESULT(splitter != nullptr, "splitter is nullptr", QRect());

    int handlesCount = splitter->count();
    GT_CHECK_RESULT(handleNumber >= 0 && handleNumber < handlesCount,
                    QString("Invalid handle number: %1. There are %2 handles in the splitter").arg(handleNumber).arg(handlesCount),
                    QRect());

    QWidget* handle = splitter->handle(handleNumber);
    QRect handleRect = handle->rect();
    return QRect(handle->mapToGlobal(handleRect.topLeft()), handle->mapToGlobal(handleRect.bottomRight()));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "moveHandle"
void GTSplitter::moveHandle(GUITestOpStatus& os, QSplitter* splitter, int pixels, int handleNumber) {
    QRect handleRect = getHandleRect(os, splitter, handleNumber);
    QPoint mouseOffset(0, 0);
    if (splitter->orientation() == Qt::Vertical) {
        mouseOffset.setY(pixels);
    } else {
        mouseOffset.setX(pixels);
    }
    GTMouseDriver::dragAndDrop(handleRect.center(), handleRect.center() + mouseOffset);
}
#undef GT_METHOD_NAME

}  // namespace HI
