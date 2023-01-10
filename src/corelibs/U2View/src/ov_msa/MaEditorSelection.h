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

#ifndef _U2_MA_EDITOR_SELECTION_
#define _U2_MA_EDITOR_SELECTION_

#include <QList>
#include <QRect>

#include <U2Core/U2Region.h>

namespace U2 {

class MaEditor;
class McaEditor;

/** Data model for selection in MSA editor: list of visual non-overlapping rectangles. */
class U2VIEW_EXPORT MaEditorSelection {
public:
    /** Creates a new empty MSA editor selection first and calls buildSafeSelectionRects() on the given rect list. */
    MaEditorSelection(const QList<QRect>& rectList = QList<QRect>());

    /**
     * Builds 'safe' selection rects list.
     * The result contains non-empty non-intersecting/non-touching each other rects with a unified left/right coordinates (bounding) sorted by top position. */
    static QList<QRect> buildSafeSelectionRects(const QList<QRect>& rectList);

    /** Returns true if the selection contains no rects. */
    bool isEmpty() const;

    /** Returns true if selection contains multiple rectangles. */
    bool isMultiRegionSelection() const;

    /** Returns true if selection contains exactly 1 rect. The rect can be of any size. */
    bool isSingleRegionSelection() const;

    /** Returns true if selection contains only 1 row (any column range). */
    bool isSingleRowSelection() const;

    /** Returns true if selection contains only 1 column (any rows range). */
    bool isSingleColumnSelection() const;

    /** Returns true if selection contains 1 rect with 1x1 dimension. */
    bool isSingleBaseSelection() const;

    /** Returns sum of height of all selected rects. */
    int getCountOfSelectedRows() const;

    /**
     * Returns selection state as a rect.
     * The returned rect is a bounding rect for all rects in the 'rectList'.
     * If 'rectList' is empty the method returns an empty (0, 0, 0, 0) rect.
     * Warning: this method is unsafe and will be removed. Use getRectList() to deal with each individual selection rect correctly.
     */
    QRect toRect() const;

    /** Returns width of the selection. Note: all rects in the selection have unified width (left & right coordinates). */
    int getWidth() const;

    /** Returns selected X region or an empty region if there is no active selection. */
    U2Region getColumnRegion() const;

    /** Returns list of selected rects. */
    const QList<QRect>& getRectList() const;

    /** Returns true if the given point addressed by column/row is in the selection. */
    bool contains(int columnIndex, int rowIndex) const;

    /** Returns true if the given point addressed by column/row is in the selection. */
    bool contains(const QPoint& columnAndRowPoint) const;

    /** Returns true if the given row (any its part) is in the selection. */
    bool containsRow(int rowIndex) const;

    /** Returns view row indexes of all rows present in the selection. */
    QList<int> getSelectedRowIndexes() const;

    /** Returns the first selected view row index of -1 if the selection is empty. */
    int getFirstSelectedRowIndex() const;

    /** Compares 2 selection. Two selections are equal if they have equal list of rects with the same order. */
    bool operator==(const MaEditorSelection& other) const;

    /** Compares 2 selection. Two selections are equal if they have equal list of rects with the same order. */
    bool operator!=(const MaEditorSelection& other) const;

private:
    /**
     * Unsorted list of visual non-overlapping on-screen rectangles.
     * The list may contain only valid rectangles with x>=0, y>=0, height>0 and width>=0.
     * Width equal to 0 is allowed by historical reasons (see MCA editor) and is processed as a 'whole row' selection.
     */
    QList<QRect> rectList;
};

/** MSA/MCA editor selection controller. */
class U2VIEW_EXPORT MaEditorSelectionController : public QObject {
    Q_OBJECT
public:
    explicit MaEditorSelectionController(MaEditor* editor);

    /** Returns current selection state. */
    const MaEditorSelection& getSelection() const;

    /** Sets new selection instance. Emits si_selectionChanged signal. */
    virtual void setSelection(const MaEditorSelection& selection);

    /**
     * Checks thats selection geometry is correct: all rects are within the alignment.
     * Returns 'true' if the selection is correct. Returns false and calls SAFE_POINT if the selection is not safe.
     * */
    static bool validateSelectionGeometry(const MaEditorSelection& selection, int alignmentLength, int viewRowCount);

    /** Returns list of currently selected MA row indexes ordered using view row index order. */
    QList<int> getSelectedMaRowIndexes() const;

    /** Returns list of currently selected MA row ids ordered using view row index order. */
    QList<qint64> getSelectedMaRowIds() const;

signals:

    /** Signal emitted every time selection is changed. */
    void si_selectionChanged(const MaEditorSelection& current, const MaEditorSelection& prev);

public slots:
    /** Sets selection to empty selection. Emits signal that selection is changed. */
    virtual void clearSelection();

    /** Updates selection state as a side effect on the alignment change. */
    virtual void handleAlignmentChange();

    /** Updates selection state as a side effect on the collapse model change. */
    virtual void handleCollapseModelChange();

protected:
    /** Current selection with view rows/column coordinates. */
    MaEditorSelection selection;

    /** List of selected row ids from the current selection. Used to restore selection state on MSA or collapse state updates. */
    QList<qint64> selectedRowIdsSnapshot;

private:
    /** MSA/MCA editor instance. Never null. */
    MaEditor* const editor;
};

/**
 * Selection controller for MCA editor.
 * TODO: move out of ov_msa to ov_mca together with other mca specific classes.
 */
class U2VIEW_EXPORT McaEditorSelectionController : public MaEditorSelectionController {
    Q_OBJECT
public:
    explicit McaEditorSelectionController(McaEditor* editor);

    /** Sets new selection instance. Emits si_selectionChanged signal. */
    void setSelection(const MaEditorSelection& selection) override;

public slots:
    /** Clears both MA & reference sequence selections. */
    void clearSelection() override;

private:
    McaEditor* const mcaEditor;
};

}  // namespace U2

#endif  // _U2_MA_EDITOR_SELECTION_
