/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#include "MultilineBaseWidthController.h"

#include "MultilineScrollController.h"
#include "ov_msa/MaEditor.h"
#include "ov_msa/view_rendering/MaEditorMultilineWgt.h"

namespace U2 {

MultilineBaseWidthController::MultilineBaseWidthController(MaEditorMultilineWgt *maEditorMultilineWgt)
    : QObject(maEditorMultilineWgt),
      maEditor(maEditorMultilineWgt->getEditor()),
      ui(maEditorMultilineWgt) {
}

int MultilineBaseWidthController::getFirstVisibleBaseGlobalOffset(bool countClipped) const {
    return getBaseGlobalOffset(ui->getScrollController()->getFirstVisibleBase(countClipped));
}

int MultilineBaseWidthController::getFirstVisibleBaseScreenOffset(bool countClipped) const {
    const int firstVisibleBaseGlobalOffset = getFirstVisibleBaseGlobalOffset(countClipped);
    return firstVisibleBaseGlobalOffset - ui->getScrollController()->getScreenPosition().x();
}

int MultilineBaseWidthController::getBaseGlobalOffset(int position) const {
    return getBaseWidth() * position;
}

int MultilineBaseWidthController::getBaseScreenOffset(int position) const {
    return getBaseGlobalOffset(position) - ui->getScrollController()->getScreenPosition().x();
}

int MultilineBaseWidthController::getBaseScreenCenter(int position) const {
    return getBaseScreenOffset(position) + getBaseWidth() / 2;
}

int MultilineBaseWidthController::getBaseWidth() const {
    return maEditor->getColumnWidth();
}

int MultilineBaseWidthController::getBasesWidth(int count) const {
    return count * getBaseWidth();
}

int MultilineBaseWidthController::getBasesWidth(const U2Region &region) const {
    return getBasesWidth(static_cast<int>(region.length));
}

U2Region MultilineBaseWidthController::getBaseGlobalRange(int position) const {
    return getBasesGlobalRange(position, 1);
}

U2Region MultilineBaseWidthController::getBasesGlobalRange(int startPosition, int count) const {
    return U2Region(getBaseGlobalOffset(startPosition), getBasesWidth(count));
}

U2Region MultilineBaseWidthController::getBasesGlobalRange(const U2Region &region) const {
    return getBasesGlobalRange(static_cast<int>(region.startPos), static_cast<int>(region.length));
}

U2Region MultilineBaseWidthController::getBaseScreenRange(int position) const {
    return getBasesScreenRange(position, 1, ui->getScrollController()->getScreenPosition().x());
}

U2Region MultilineBaseWidthController::getBasesScreenRange(const U2Region &region) const {
    return getBasesScreenRange(static_cast<int>(region.startPos), static_cast<int>(region.length), ui->getScrollController()->getScreenPosition().x());
}

U2Region MultilineBaseWidthController::getBaseScreenRange(int position, int screenXOrigin) const {
    return getBasesScreenRange(position, 1, screenXOrigin);
}

U2Region MultilineBaseWidthController::getBasesScreenRange(int startPosition, int count, int screenXOrigin) const {
    const U2Region globalRange = getBasesGlobalRange(startPosition, count);
    return U2Region(globalRange.startPos - screenXOrigin, globalRange.length);
}

U2Region MultilineBaseWidthController::getBasesScreenRange(const U2Region &region, int screenXOrigin) const {
    return getBasesScreenRange(static_cast<int>(region.startPos), static_cast<int>(region.length), screenXOrigin);
}

int MultilineBaseWidthController::getTotalAlignmentWidth() const {
    return maEditor->getAlignmentLen() * getBaseWidth();
}

int MultilineBaseWidthController::globalXPositionToColumn(int x) const {
    return x / getBaseWidth();
}

int MultilineBaseWidthController::screenXPositionToColumn(int x) const {
    return globalXPositionToColumn(ui->getScrollController()->getScreenPosition().x() + x);
}

int MultilineBaseWidthController::screenXPositionToBase(int x) const {
    const int column = screenXPositionToColumn(x);
    return 0 <= column && column < maEditor->getAlignmentLen() ? column : -1;
}

}  // namespace U2
