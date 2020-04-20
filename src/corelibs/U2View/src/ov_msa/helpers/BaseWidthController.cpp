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

#include "BaseWidthController.h"
#include "ScrollController.h"
#include "ov_msa/MaEditor.h"
#include "ov_msa/view_rendering/MaEditorWgt.h"

namespace U2 {

BaseWidthController::BaseWidthController(MaEditorWgt *maEditorWgt) :
    QObject(maEditorWgt),
    maEditor(maEditorWgt->getEditor()),
    ui(maEditorWgt)
{

}

int BaseWidthController::getFirstVisibleBaseGlobalOffset(bool countClipped) const {
    return getBaseGlobalOffset(ui->getScrollController()->getFirstVisibleBase(countClipped));
}

int BaseWidthController::getFirstVisibleBaseScreenOffset(bool countClipped) const {
    const int firstVisibleBaseGlobalOffset = getFirstVisibleBaseGlobalOffset(countClipped);
    return firstVisibleBaseGlobalOffset - ui->getScrollController()->getScreenPosition().x();
}

int BaseWidthController::getBaseGlobalOffset(int position) const {
    return getBaseWidth() * position;
}

int BaseWidthController::getBaseScreenOffset(int position) const {
    return getBaseGlobalOffset(position) - ui->getScrollController()->getScreenPosition().x();
}

int BaseWidthController::getBaseScreenCenter(int position) const {
    return getBaseScreenOffset(position) + getBaseWidth() / 2;
}

int BaseWidthController::getBaseWidth() const {
    return maEditor->getColumnWidth();
}

int BaseWidthController::getBasesWidth(int count) const {
    return count * getBaseWidth();
}

int BaseWidthController::getBasesWidth(const U2Region &region) const {
    return getBasesWidth(static_cast<int>(region.length));
}

U2Region BaseWidthController::getBaseGlobalRange(int position) const {
    return getBasesGlobalRange(position, 1);
}

U2Region BaseWidthController::getBasesGlobalRange(int startPosition, int count) const {
    return U2Region(getBaseGlobalOffset(startPosition), getBasesWidth(count));
}

U2Region BaseWidthController::getBasesGlobalRange(const U2Region &region) const {
    return getBasesGlobalRange(static_cast<int>(region.startPos), static_cast<int>(region.length));
}

U2Region BaseWidthController::getBaseScreenRange(int position) const {
    return getBasesScreenRange(position, 1, ui->getScrollController()->getScreenPosition().x());
}

U2Region BaseWidthController::getBasesScreenRange(const U2Region &region) const {
    return getBasesScreenRange(static_cast<int>(region.startPos), static_cast<int>(region.length), ui->getScrollController()->getScreenPosition().x());
}

U2Region BaseWidthController::getBaseScreenRange(int position, int screenXOrigin) const {
    return getBasesScreenRange(position, 1, screenXOrigin);
}

U2Region BaseWidthController::getBasesScreenRange(int startPosition, int count, int screenXOrigin) const {
    const U2Region globalRange = getBasesGlobalRange(startPosition, count);
    return U2Region(globalRange.startPos - screenXOrigin, globalRange.length);
}

U2Region BaseWidthController::getBasesScreenRange(const U2Region &region, int screenXOrigin) const {
    return getBasesScreenRange(static_cast<int>(region.startPos), static_cast<int>(region.length), screenXOrigin);
}

int BaseWidthController::getTotalAlignmentWidth() const {
    return maEditor->getAlignmentLen() * getBaseWidth();
}

int BaseWidthController::globalXPositionToColumn(int x) const {
    return x / getBaseWidth();
}

int BaseWidthController::screenXPositionToColumn(int x) const {
    return globalXPositionToColumn(ui->getScrollController()->getScreenPosition().x() + x);
}

int BaseWidthController::screenXPositionToBase(int x) const {
    const int column = screenXPositionToColumn(x);
    return 0 <= column && column < maEditor->getAlignmentLen() ? column : -1;
}

}   // namespace U2
