/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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
QRect GTSplitter::getHandleRect(GUITestOpStatus &os, QSplitter *splitter, int handleNumber) {
    Q_UNUSED(os)
    GT_CHECK_RESULT(nullptr != splitter, "splitter is nullptr", QRect());

    const int handlesCount = splitter->count() - 1;
    GT_CHECK_RESULT(0 <= handleNumber && handleNumber < handlesCount,
                    QString("Invalid handle number: %1. There are %2 handles in the splitter").arg(handleNumber).arg(handlesCount),
                    QRect());

    const QRect handleRect = splitter->handle(handleNumber)->rect();
    return QRect(splitter->mapToGlobal(handleRect.topLeft()), splitter->mapToGlobal(handleRect.bottomRight()));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "moveHandle"
void GTSplitter::moveHandle(GUITestOpStatus &os, QSplitter *splitter, int pixels, int handleNumber) {
    const QRect handleRect = getHandleRect(os, splitter, handleNumber);
    QPoint mouseOffset(0, 0);
    if (Qt::Vertical == splitter->orientation()) {
        mouseOffset.setY(pixels);
    } else {
        mouseOffset.setX(pixels);
    }
    GTMouseDriver::moveTo(handleRect.center());
    GTMouseDriver::click();
    GTMouseDriver::moveTo(handleRect.center() + mouseOffset);
    GTMouseDriver::release();
}
#undef GT_METHOD_NAME

}   // namespace HI
