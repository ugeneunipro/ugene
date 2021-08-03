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

#include "MaEditorSelection.h"

#include <U2Core/U2SafePoints.h>

#include "ov_msa/MaEditor.h"
#include "ov_msa/McaEditor.h"
#include "ov_msa/McaEditorReferenceArea.h"

namespace U2 {

/************************************************************************/
/* MaEditorSelection */
/************************************************************************/

MaEditorSelection::MaEditorSelection(const QList<QRect> &rects)
    : rectList(buildSafeSelectionRects(rects)) {
}

QList<QRect> MaEditorSelection::buildSafeSelectionRects(const QList<QRect> &rectList) {
    if (rectList.size() <= 1) {    // 0 or 1 result: no need to merge, validate only.
        return rectList.isEmpty() || rectList.first().isEmpty() ? QList<QRect>() : rectList;
    }
    int unifiedLeft = INT_MAX;
    int unifiedRight = INT_MIN;
    for (auto rect : qAsConst(rectList)) {
        unifiedLeft = qMin(unifiedLeft, rect.left());
        unifiedRight = qMax(unifiedRight, rect.right());
    }
    if (unifiedRight < unifiedLeft) {    // All rects are empty.
        return {};
    }
    // Sort & merge rects if needed. Assign unified left & right.
    QList<QRect> sortedRectList = rectList;
    std::sort(sortedRectList.begin(), sortedRectList.end(), [](const QRect &r1, const QRect &r2) {
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
        const QRect &rect = rectList[i];
        QPoint topLeft(qMin(rect.x(), boundingRect.x()), qMin(rect.y(), boundingRect.y()));
        QPoint bottomRight(qMax(rect.right(), boundingRect.right()), qMax(rect.bottom(), boundingRect.bottom()));
        boundingRect = QRect(topLeft, bottomRight);
    }
    return boundingRect;
}

const QList<QRect> &MaEditorSelection::getRectList() const {
    return rectList;
}

bool MaEditorSelection::operator==(const MaEditorSelection &other) const {
    return other.getRectList() == rectList;
}

bool MaEditorSelection::operator!=(const MaEditorSelection &other) const {
    return !(other == *this);
}

bool MaEditorSelection::contains(int columnIndex, int rowIndex) const {
    CHECK(!isEmpty(), false);
    const QRect &rect = rectList.first();
    return columnIndex >= rect.left() && columnIndex <= rect.right() && containsRow(rowIndex);
}

bool MaEditorSelection::contains(const QPoint &columnAndRowPoint) const {
    return contains(columnAndRowPoint.x(), columnAndRowPoint.y());
}

bool MaEditorSelection::containsRow(int rowIndex) const {
    for (const QRect &rect : qAsConst(rectList)) {
        if (rect.top() <= rowIndex && rect.bottom() >= rowIndex) {
            return true;
        }
    }
    return false;
}

QList<int> MaEditorSelection::getSelectedRowIndexes() const {
    QList<int> selectedRowIndexes;
    for (const QRect &rect : qAsConst(rectList)) {
        for (int rowIndex = rect.top(); rowIndex <= rect.bottom(); rowIndex++) {
            selectedRowIndexes << rowIndex;
        }
    }
    return selectedRowIndexes;
}

/************************************************************************/
/* MaEditorSelectionController */
/************************************************************************/

MaEditorSelectionController::MaEditorSelectionController(MaEditor *_editor)
    : QObject(_editor), editor(_editor) {
    SAFE_POINT(editor != nullptr, "MAEditor is null!", );
}

const MaEditorSelection &MaEditorSelectionController::getSelection() const {
    return selection;
}

void MaEditorSelectionController::clearSelection() {
    setSelection({});
}

void MaEditorSelectionController::setSelection(const MaEditorSelection &newSelection) {
    CHECK(!editor->isAlignmentEmpty() || newSelection.isEmpty(), );
    CHECK(newSelection != selection, );
    MaEditorSelection oldSelection = selection;
    selection = newSelection;
    emit si_selectionChanged(selection, oldSelection);
}

int MaEditorSelection::getCountOfSelectedRows() const {
    int count = 0;
    for (const QRect &rect : qAsConst(rectList)) {
        count += rect.height();
    }
    return count;
}

/************************************************************************/
/* McaEditorSelectionController */
/************************************************************************/

McaEditorSelectionController::McaEditorSelectionController(McaEditor *_editor)
    : MaEditorSelectionController(_editor), mcaEditor(_editor) {
}

void McaEditorSelectionController::clearSelection() {
    MaEditorSelectionController::clearSelection();
    mcaEditor->getUI()->getReferenceArea()->clearSelection();
}

void McaEditorSelectionController::setSelection(const MaEditorSelection &newSelection) {
    QRect selectionRect = newSelection.toRect();
    if (selectionRect.isEmpty()) {
        MaEditorSelectionController::setSelection({});
        mcaEditor->getUI()->getReferenceArea()->clearSelection();
        return;
    }
    if (selectionRect.width() == 1 && mcaEditor->getMaObject()->getMca()->isTrailingOrLeadingGap(selectionRect.y(), selectionRect.x())) {
        // Clear selection if gap is clicked.
        MaEditorSelectionController::setSelection({});
        mcaEditor->getUI()->getReferenceArea()->clearSelection();
        return;
    }
    MaEditorSelectionController::setSelection(newSelection);
}

}    // namespace U2
