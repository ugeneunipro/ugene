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

#include "U2Core/U2SafePoints.h"

#include "BaseWidthController.h"
#include "DrawHelper.h"
#include "RowHeightController.h"
#include "ScrollController.h"
#include "ov_msa/MaCollapseModel.h"
#include "ov_msa/view_rendering/MaEditorSelection.h"
#include "ov_msa/view_rendering/MaEditorWgt.h"

namespace U2 {

DrawHelper::DrawHelper(MaEditorWgt *maEditorWgt) :
    ui(maEditorWgt),
    scrollController(maEditorWgt->getScrollController()),
    collapsibleModel(maEditorWgt->getCollapseModel())
{

}

U2Region DrawHelper::getVisibleBases(int widgetWidth, bool countFirstClippedBase, bool countLastClippedBase) const {
    const int firstVisibleBase = scrollController->getFirstVisibleBase(countFirstClippedBase);
    const int lastVisibleBase = scrollController->getLastVisibleBase(widgetWidth, countLastClippedBase);
    return U2Region(firstVisibleBase, lastVisibleBase - firstVisibleBase + 1);
}

U2Region DrawHelper::getVisibleViewRowsRegion(int widgetHeight, bool countFirstClippedRow, bool countLastClippedRow) const {
    const int firstVisibleRowNumber = scrollController->getFirstVisibleViewRowIndex(countFirstClippedRow);
    const int lastVisibleRowNumber= scrollController->getLastVisibleViewRowIndex(widgetHeight, countLastClippedRow);
    return U2Region(firstVisibleRowNumber, lastVisibleRowNumber - firstVisibleRowNumber + 1);
}

QList<int> DrawHelper::getVisibleMaRowIndexes(int widgetHeight, bool countFirstClippedRow, bool countLastClippedRow) const {
    int firstVisibleViewRow = scrollController->getFirstVisibleViewRowIndex(countFirstClippedRow);
    int lastVisibleViewRow = scrollController->getLastVisibleViewRowIndex(widgetHeight, countLastClippedRow);
    U2Region viewRowsRegion(firstVisibleViewRow, lastVisibleViewRow - firstVisibleViewRow + 1);
    return collapsibleModel->getMaRowIndexesByViewRowIndexes(viewRowsRegion);
}

int DrawHelper::getVisibleBasesCount(int widgetWidth, bool countFirstClippedBase, bool countLastClippedBase) const {
    return getVisibleBases(widgetWidth, countFirstClippedBase,countLastClippedBase).length;
}

QRect DrawHelper::getSelectionScreenRect(const MaEditorSelection &selection) const {
    CHECK(!selection.isEmpty(), QRect());

    U2Region xRange = ui->getBaseWidthController()->getBasesScreenRange(selection.getXRegion());
    U2Region yRange = ui->getRowHeightController()->getScreenYRegionByViewRowsRegion(selection.getYRegion());
    return QRect(xRange.startPos, yRange.startPos, xRange.length, yRange.length);
}

}   // namespace U2
