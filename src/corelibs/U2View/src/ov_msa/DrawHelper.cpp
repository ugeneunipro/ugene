/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "DrawHelper.h"

#include "U2Core/U2SafePoints.h"

#include "BaseWidthController.h"
#include "RowHeightController.h"
#include "ScrollController.h"
#include "ov_msa/MaCollapseModel.h"
#include "ov_msa/MaEditor.h"
#include "ov_msa/MaEditorSelection.h"
#include "ov_msa/MaEditorWgt.h"

namespace U2 {

DrawHelper::DrawHelper(MaEditorWgt *_ui)
    : ui(_ui) {
}

U2Region DrawHelper::getVisibleBases(int widgetWidth, bool countFirstClippedBase, bool countLastClippedBase) const {
    auto scrollController = ui->getScrollController();
    const int firstVisibleBase = scrollController->getFirstVisibleBase(countFirstClippedBase);
    const int lastVisibleBase = scrollController->getLastVisibleBase(widgetWidth, countLastClippedBase);
    return U2Region(firstVisibleBase, lastVisibleBase - firstVisibleBase + 1);
}

U2Region DrawHelper::getVisibleViewRowsRegion(int widgetHeight, bool countFirstClippedRow, bool countLastClippedRow) const {
    auto scrollController = ui->getScrollController();
    const int firstVisibleRowNumber = scrollController->getFirstVisibleViewRowIndex(countFirstClippedRow);
    const int lastVisibleRowNumber = scrollController->getLastVisibleViewRowIndex(widgetHeight, countLastClippedRow);
    return U2Region(firstVisibleRowNumber, lastVisibleRowNumber - firstVisibleRowNumber + 1);
}

QList<int> DrawHelper::getVisibleMaRowIndexes(int widgetHeight, bool countFirstClippedRow, bool countLastClippedRow) const {
    auto scrollController = ui->getScrollController();
    int firstVisibleViewRow = scrollController->getFirstVisibleViewRowIndex(countFirstClippedRow);
    int lastVisibleViewRow = scrollController->getLastVisibleViewRowIndex(widgetHeight, countLastClippedRow);
    U2Region viewRowsRegion(firstVisibleViewRow, lastVisibleViewRow - firstVisibleViewRow + 1);
    return ui->getEditor()->getCollapseModel()->getMaRowIndexesByViewRowIndexes(viewRowsRegion);
}

int DrawHelper::getVisibleBasesCount(int widgetWidth, bool countFirstClippedBase, bool countLastClippedBase) const {
    return getVisibleBases(widgetWidth, countFirstClippedBase, countLastClippedBase).length;
}

QRect DrawHelper::getScreenRect(const QRect& columnsAndRowsRect) const {
    CHECK(!columnsAndRowsRect.isEmpty(), QRect());

    U2Region xRange = ui->getBaseWidthController()->getBasesScreenRange(U2Region::fromXRange(columnsAndRowsRect));
    U2Region yRange = ui->getRowHeightController()->getScreenYRegionByViewRowsRegion(U2Region::fromYRange(columnsAndRowsRect));
    return QRect(xRange.startPos, yRange.startPos, xRange.length, yRange.length);
}

}  // namespace U2
