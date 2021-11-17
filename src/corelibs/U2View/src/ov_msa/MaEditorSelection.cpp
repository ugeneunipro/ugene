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

#include "MaEditorSelection.h"

#include <U2Core/U2SafePoints.h>

#include "ov_mca/McaEditor.h"
#include "ov_mca/McaEditorReferenceArea.h"
#include "ov_msa/MaCollapseModel.h"
#include "ov_msa/MaEditor.h"

namespace U2 {

/************************************************************************/
/* MaEditorSelection */
/************************************************************************/

MaEditorSelection::MaEditorSelection(const QList<QRect>& rects)
    : rectList(buildSafeSelectionRects(rects)) {
}

QList<QRect> MaEditorSelection::buildSafeSelectionRects(const QList<QRect>& rectList) {
    if (rectList.size() <= 1) {  // 0 or 1 result: no need to merge, validate only.
        return rectList.isEmpty() || rectList.first().isEmpty() ? QList<QRect>() : rectList;
    }
    int unifiedLeft = INT_MAX;
    int unifiedRight = INT_MIN;
    for (auto rect : qAsConst(rectList)) {
        unifiedLeft = qMin(unifiedLeft, rect.left());
        unifiedRight = qMax(unifiedRight, rect.right());
    }
    if (unifiedRight < unifiedLeft) {  // All rects are empty.
        return {};
    }
    // Sort & merge rects if needed. Assign unified left & right.
    QList<QRect> sortedRectList = rectList;
    std::sort(sortedRectList.begin(), sortedRectList.end(), [](const QRect& r1, const QRect& r2) {
        return r1.top() < r2.top();
    });
    QList<QRect> mergedAndSortedRectList;
    for (QRect rect : sortedRectList) {
        if (rect.height() == 0) {
            continue;
        }
        // Set unified left & right.
        rect.setLeft(unifiedLeft);
        rect.setRight(unifiedRight);

        // Add to the list of the first.
        if (mergedAndSortedRectList.isEmpty()) {
            mergedAndSortedRectList << rect;
        } else {
            // Or merge with a previous one if overlaps/touches.
            QRect prevRect = mergedAndSortedRectList.last();
            if (prevRect.intersects(rect) || prevRect.bottom() + 1 == rect.top()) {
                mergedAndSortedRectList.removeLast();
                mergedAndSortedRectList << prevRect.united(rect);
            } else {
                mergedAndSortedRectList << rect;
            }
        }
    }
    return mergedAndSortedRectList;
}

bool MaEditorSelection::isEmpty() const {
    return rectList.isEmpty();
}

bool MaEditorSelection::isMultiRegionSelection() const {
    return rectList.size() > 1;
}

bool MaEditorSelection::isSingleRegionSelection() const {
    return rectList.size() == 1;
}

bool MaEditorSelection::isSingleRowSelection() const {
    return rectList.size() == 1 && rectList[0].height() == 1;
}

bool MaEditorSelection::isSingleColumnSelection() const {
    return rectList.size() == 1 && rectList[0].width() == 1;
}

bool MaEditorSelection::isSingleBaseSelection() const {
    return isSingleRegionSelection() && rectList.first().width() == 1 && rectList.first().height() == 1;
}

int MaEditorSelection::getWidth() const {
    return isEmpty() ? 0 : rectList.first().width();
}

QRect MaEditorSelection::toRect() const {
    if (rectList.isEmpty()) {
        return {0, 0, 0, 0};
    }
    QRect boundingRect = rectList[0];
    for (int i = 1; i < rectList.length(); i++) {
        const QRect& rect = rectList[i];
        QPoint topLeft(qMin(rect.x(), boundingRect.x()), qMin(rect.y(), boundingRect.y()));
        QPoint bottomRight(qMax(rect.right(), boundingRect.right()), qMax(rect.bottom(), boundingRect.bottom()));
        boundingRect = QRect(topLeft, bottomRight);
    }
    return boundingRect;
}

const QList<QRect>& MaEditorSelection::getRectList() const {
    return rectList;
}

bool MaEditorSelection::operator==(const MaEditorSelection& other) const {
    return other.getRectList() == rectList;
}

bool MaEditorSelection::operator!=(const MaEditorSelection& other) const {
    return !(other == *this);
}

bool MaEditorSelection::contains(int columnIndex, int rowIndex) const {
    CHECK(!isEmpty(), false);
    const QRect& rect = rectList.first();
    return columnIndex >= rect.left() && columnIndex <= rect.right() && containsRow(rowIndex);
}

bool MaEditorSelection::contains(const QPoint& columnAndRowPoint) const {
    return contains(columnAndRowPoint.x(), columnAndRowPoint.y());
}

bool MaEditorSelection::containsRow(int rowIndex) const {
    for (const QRect& rect : qAsConst(rectList)) {
        if (rect.top() <= rowIndex && rect.bottom() >= rowIndex) {
            return true;
        }
    }
    return false;
}

QList<int> MaEditorSelection::getSelectedRowIndexes() const {
    QList<int> selectedRowIndexes;
    for (const QRect& rect : qAsConst(rectList)) {
        for (int rowIndex = rect.top(); rowIndex <= rect.bottom(); rowIndex++) {
            selectedRowIndexes << rowIndex;
        }
    }
    return selectedRowIndexes;
}

int MaEditorSelection::getFirstSelectedRowIndex() const {
    CHECK(!isEmpty(), -1);
    return getSelectedRowIndexes().first();
}

U2Region MaEditorSelection::getColumnRegion() const {
    if (isEmpty()) {
        return {};
    }
    return {rectList[0].x(), rectList[0].width()};
}

/************************************************************************/
/* MaEditorSelectionController */
/************************************************************************/

MaEditorSelectionController::MaEditorSelectionController(MaEditor* _editor)
    : QObject(_editor), editor(_editor) {
    SAFE_POINT(editor != nullptr, "MAEditor is null!", );
    connect(editor->getCollapseModel(), &MaCollapseModel::si_toggled, this, &MaEditorSelectionController::handleCollapseModelChange);
    connect(editor->getMaObject(), &MultipleAlignmentObject::si_alignmentChanged, this, &MaEditorSelectionController::handleAlignmentChange);
}

const MaEditorSelection& MaEditorSelectionController::getSelection() const {
    return selection;
}

void MaEditorSelectionController::clearSelection() {
    setSelection({});
}

void MaEditorSelectionController::setSelection(const MaEditorSelection& newSelection) {
    CHECK(!editor->isAlignmentEmpty() || newSelection.isEmpty(), );
    CHECK(newSelection != selection, );
    CHECK(validateSelectionGeometry(newSelection, editor->getAlignmentLen(), editor->getCollapseModel()->getViewRowCount()), );
    MaEditorSelection oldSelection = selection;
    selection = newSelection;
    selectedRowIdsSnapshot = getSelectedMaRowIds();
    emit si_selectionChanged(selection, oldSelection);
}

bool MaEditorSelectionController::validateSelectionGeometry(const MaEditorSelection& selection, int alignmentLength, int viewRowCount) {
    CHECK(!selection.isEmpty(), true);

    // Check column range.
    U2Region columnRegion = selection.getColumnRegion();
    bool hasValidColumnRange = columnRegion.startPos >= 0 && columnRegion.endPos() <= alignmentLength;
    SAFE_POINT(hasValidColumnRange, "Invalid column range in MSA selection", false);
    CHECK(hasValidColumnRange, false);

    // Check row range.
    U2Region rowRange = U2Region::fromStartAndEnd(selection.getRectList().first().top(), selection.getRectList().last().bottom() + 1);
    bool hasValidRowRange = rowRange.startPos >= 0 && rowRange.endPos() <= viewRowCount;
    SAFE_POINT(hasValidRowRange, "Invalid row range in MSA selection", false);
    return hasValidRowRange;
}

int MaEditorSelection::getCountOfSelectedRows() const {
    int count = 0;
    for (const QRect& rect : qAsConst(rectList)) {
        count += rect.height();
    }
    return count;
}

QList<int> MaEditorSelectionController::getSelectedMaRowIndexes() const {
    QList<int> maRowIndexes;
    QList<QRect> selectedRectList = editor->getSelection().getRectList();
    MaCollapseModel* collapseModel = editor->getCollapseModel();
    for (const QRect& rect : qAsConst(selectedRectList)) {
        U2Region rowRange = U2Region::fromYRange(rect);
        QList<int> maRowIndexesPerRect = collapseModel->getMaRowIndexesByViewRowIndexes(rowRange, true);
        maRowIndexes << maRowIndexesPerRect;
    }
    return maRowIndexes;
}

QList<qint64> MaEditorSelectionController::getSelectedMaRowIds() const {
    QList<int> selectedMaRowIndexes = getSelectedMaRowIndexes();
    QList<qint64> allMaRowIds = editor->getMaRowIds();
    QList<qint64> selectedMaRowIds;
    for (int maRowIndex : qAsConst(selectedMaRowIndexes)) {
        SAFE_POINT(maRowIndex >= 0 && maRowIndex < allMaRowIds.size(), "Invalid ma-row-index: " + QString::number(maRowIndex), {});
        selectedMaRowIds << allMaRowIds[maRowIndex];
    }
    return selectedMaRowIds;
}

void MaEditorSelectionController::handleAlignmentChange() {
    // Ensure the columns region is in range.
    U2Region columnsRegions = selection.getColumnRegion();
    columnsRegions.startPos = qMin(columnsRegions.startPos, (qint64)editor->getAlignmentLen() - 1);
    qint64 selectedColumnsEndPos = qMin(columnsRegions.endPos(), (qint64)editor->getAlignmentLen());
    columnsRegions.length = selectedColumnsEndPos - columnsRegions.startPos;

    // Select the longest continuous region for the new selection
    QList<int> selectedMaRowIndexes = editor->getMaObject()->convertMaRowIdsToMaRowIndexes(selectedRowIdsSnapshot);
    MaCollapseModel* collapseModel = editor->getCollapseModel();
    QList<QRect> newSelectedRects;
    for (int i = 0; i < selectedMaRowIndexes.size(); i++) {
        int viewRowIndex = collapseModel->getViewRowIndexByMaRowIndex(selectedMaRowIndexes[i]);
        if (viewRowIndex >= 0) {
            newSelectedRects << QRect(columnsRegions.startPos, viewRowIndex, columnsRegions.length, 1);
        }
    }
    setSelection(MaEditorSelection(newSelectedRects));
}

void MaEditorSelectionController::handleCollapseModelChange() {
    handleAlignmentChange();  // Using the same selection morph logic with alignment modification.
}

/************************************************************************/
/* McaEditorSelectionController */
/************************************************************************/

McaEditorSelectionController::McaEditorSelectionController(McaEditor* _editor)
    : MaEditorSelectionController(_editor), mcaEditor(_editor) {
}

void McaEditorSelectionController::clearSelection() {
    MaEditorSelectionController::clearSelection();
    mcaEditor->getMcaEditorWgtUI()->getReferenceArea()->clearSelection();
}

void McaEditorSelectionController::setSelection(const MaEditorSelection& newSelection) {
    if (newSelection.isEmpty()) {
        MaEditorSelectionController::setSelection({});
        mcaEditor->getMcaEditorWgtUI()->getReferenceArea()->clearSelection();
        return;
    }
    QList<QRect> selectedRects = newSelection.getRectList();
    if (newSelection.isSingleBaseSelection() && mcaEditor->getMaObject()->getMca()->isTrailingOrLeadingGap(selectedRects[0].y(), selectedRects[0].x())) {
        // Clear selection if gap is clicked.
        MaEditorSelectionController::setSelection({});
        mcaEditor->getMcaEditorWgtUI()->getReferenceArea()->clearSelection();
        return;
    }
    MaEditorSelectionController::setSelection(newSelection);
}

}  // namespace U2
