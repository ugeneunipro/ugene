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

#include "RowHeightController.h"
#include "ScrollController.h"
#include "ov_msa/MaEditor.h"
#include "ov_msa/MaCollapseModel.h"
#include "ov_msa/view_rendering/MaEditorWgt.h"

namespace U2 {

RowHeightController::RowHeightController(MaEditorWgt* maEditorWgt)
        : QObject(maEditorWgt),
          ui(maEditorWgt) {

}

int RowHeightController::getGlobalYPositionByMaRowIndex(int maRowIndex) const {
    const MaCollapseModel* collapseModel = ui->getCollapseModel();
    int viewRowIndex = collapseModel->getViewRowIndexByMaRowIndex(maRowIndex);
    int offset = 0;
    for (int viewRow = 0; viewRow < viewRowIndex; viewRow++) {
        int maRow = collapseModel->getMaRowIndexByViewRowIndex(viewRow);
        offset += getRowHeightByMaIndex(maRow);
    }
    return offset;
}

int RowHeightController::getGlobalYPositionByMaRowIndex(int maRowIndex, const QList<int>& maRowIndexes) const {
    int offset = 0;
            foreach (int currentIndex, maRowIndexes) {
            if (currentIndex == maRowIndex) {
                return offset;
            }
            offset += getRowHeightByMaIndex(currentIndex);
        }
    FAIL(false, 0);
}

int RowHeightController::getGlobalYPositionOfTheFirstVisibleRow(bool countClipped) const {
    return getGlobalYPositionByMaRowIndex(ui->getScrollController()->getFirstVisibleMaRowIndex(countClipped));
}

int RowHeightController::getScreenYPositionOfTheFirstVisibleRow(bool countClipped) const {
    const int globalYPositionOfTheFirstVisibleRow = getGlobalYPositionOfTheFirstVisibleRow(countClipped);
    return globalYPositionOfTheFirstVisibleRow - ui->getScrollController()->getScreenPosition().y();
}

int RowHeightController::getRowHeightByViewRowIndex(int viewRowIndex) const {
    int maRowIndex = ui->getCollapseModel()->getMaRowIndexByViewRowIndex(viewRowIndex);
    return getRowHeightByMaIndex(maRowIndex);
}

int RowHeightController::getSumOfRowHeightsByMaIndexes(const QList<int>& maRowIndexes) const {
    int sumHeight = 0;
            foreach (int maRowIndex, maRowIndexes) {
            sumHeight += getRowHeightByMaIndex(maRowIndex);
        }
    return sumHeight;
}

int RowHeightController::getTotalAlignmentHeight() const {
    int viewRowCount = ui->getCollapseModel()->getViewRowCount();
    U2Region globalYRegion = getGlobalYRegionByViewRowsRegion(U2Region(0, viewRowCount));
    return static_cast<int>(globalYRegion.length);
}

int RowHeightController::getSingleRowHeight() const {
    const int fontHeight = QFontMetrics(ui->getEditor()->getFont(), ui).height();
    const float zoomMult = ui->getEditor()->zoomMult;
    return qRound(fontHeight * zoomMult);
}

int RowHeightController::getMaRowIndexByGlobalYPosition(int y) const {
    int viewRowIndex = getViewRowIndexByGlobalYPosition(y);
    return ui->getCollapseModel()->getMaRowIndexByViewRowIndex(viewRowIndex);
}

int RowHeightController::getViewRowIndexByGlobalYPosition(int y) const {
    const int viewRowCount = ui->getCollapseModel()->getViewRowCount();
    int accumulatedHeight = 0;
    for (int viewRowIndex = 0; viewRowIndex < viewRowCount; viewRowIndex++) {
        const int rowHeight = getRowHeightByViewRowIndex(viewRowIndex);
        if (accumulatedHeight + rowHeight <= y) {
            accumulatedHeight += rowHeight;
        } else {
            return viewRowIndex;
        }
    }
    return -1;
}

int RowHeightController::getViewRowIndexByScreenYPosition(int y) const {
    return getViewRowIndexByGlobalYPosition(y + ui->getScrollController()->getScreenPosition().y());
}

U2Region RowHeightController::getGlobalYRegionByMaRowIndex(int maRowIndex) const {
    int globalYPosition = getGlobalYPositionByMaRowIndex(maRowIndex);
    int rowHeight = getRowHeightByMaIndex(maRowIndex);
    return U2Region(globalYPosition, rowHeight);
}

U2Region RowHeightController::getGlobalYRegionByMaRowIndex(int maRowIndex, const QList<int>& maRowIndexes) const {
    int globalYPosition = getGlobalYPositionByMaRowIndex(maRowIndex, maRowIndexes);
    int rowHeight = getRowHeightByMaIndex(maRowIndex);
    return U2Region(globalYPosition, rowHeight);
}

// The OUT_OF_RANGE_OFFSET used to build safe coordinates out of the view.
// We can't use 0 offset, because the value will overlap with the first or the last row.
//
// GUI tests notice: GUI tests call RowHeightController to compute initial mouse positioning for overflows.
//  In this case the offset must be big enough to skip rubber band.
#define OUT_OF_RANGE_OFFSET 5

U2Region RowHeightController::getGlobalYRegionByViewRowIndex(int viewRowIndex) const {
    if (ui->getCollapseModel()->getViewRowCount() == 0) { // empty alignment.
        return U2Region(- OUT_OF_RANGE_OFFSET, 0);
    }
    MaCollapseModel* collapseModel = ui->getCollapseModel();
    int viewRowCount = collapseModel->getViewRowCount();
    // Return an empty region after the view for viewRowIndexes > maxRows
    // and a region before the view for viewRowIndex < 0. Use OUT_OF_RANGE_OFFSET for the out of range regions.
    if (viewRowIndex < 0) {
        U2Region startOfView = getGlobalYRegionByViewRowIndex(0);
        return U2Region(startOfView.startPos - OUT_OF_RANGE_OFFSET, 0);
    } else if (viewRowIndex >= viewRowCount) {
        U2Region endOfView = getGlobalYRegionByViewRowIndex(viewRowCount - 1);
        return U2Region(endOfView.endPos() + OUT_OF_RANGE_OFFSET, 0);
    }
    int maRow = collapseModel->getMaRowIndexByViewRowIndex(viewRowIndex);
    return getGlobalYRegionByMaRowIndex(maRow);
}

U2Region RowHeightController::getGlobalYRegionByViewRowsRegion(const U2Region& viewRowsRegion) const {
    U2Region startPosRegion = getGlobalYRegionByViewRowIndex(viewRowsRegion.startPos);
    U2Region endPosRegion = getGlobalYRegionByViewRowIndex(viewRowsRegion.endPos() - 1);
    return U2Region::containingRegion(startPosRegion, endPosRegion);
}

U2Region RowHeightController::getScreenYRegionByViewRowsRegion(const U2Region& viewRowsRegion) const {
    U2Region startPosRegion = getScreenYRegionByViewRowIndex(viewRowsRegion.startPos);
    U2Region endPosRegion = getScreenYRegionByViewRowIndex(viewRowsRegion.endPos() - 1);
    return U2Region::containingRegion(startPosRegion, endPosRegion);
}

U2Region RowHeightController::getScreenYRegionByMaRowIndex(int maRowIndex) const {
    return getScreenYRegionByMaRowIndex(maRowIndex, ui->getScrollController()->getScreenPosition().y());
}

U2Region RowHeightController::getScreenYRegionByMaRowIndex(int maRowIndex, int screenYOrigin) const {
    U2Region rowRange = getGlobalYRegionByMaRowIndex(maRowIndex);
    return U2Region(rowRange.startPos - screenYOrigin, rowRange.length);
}

U2Region RowHeightController::mapGlobalToScreen(const U2Region& globalRegion) const {
    int screenYOrigin = ui->getScrollController()->getScreenPosition().y();
    return U2Region(globalRegion.startPos - screenYOrigin, globalRegion.length);
}

U2Region RowHeightController::getScreenYRegionByViewRowIndex(int viewRowIndex) const {
    return mapGlobalToScreen(getGlobalYRegionByViewRowIndex(viewRowIndex));
}

}   // namespace U2
