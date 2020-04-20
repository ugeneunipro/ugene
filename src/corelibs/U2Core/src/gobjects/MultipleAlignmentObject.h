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

#ifndef _U2_MULTIPLE_ALIGNMENT_OBJECT_H_
#define _U2_MULTIPLE_ALIGNMENT_OBJECT_H_

#include <U2Core/GObject.h>
#include <U2Core/MaModificationInfo.h>
#include <U2Core/MultipleAlignment.h>

namespace U2 {

class MaSavedState {
public:
    MaSavedState();
    ~MaSavedState();

    bool hasState() const;
    const MultipleAlignment takeState();
    void setState(const MultipleAlignment &ma);

private:
    MultipleAlignment *lastState;
};

class U2CORE_EXPORT MultipleAlignmentObject : public GObject {
    Q_OBJECT
public:
    MultipleAlignmentObject(const QString &gobjectType,
                            const QString &name,
                            const U2EntityRef &maRef,
                            const QVariantMap &hintsMap,
                            const MultipleAlignment &alignment);
    ~MultipleAlignmentObject();

    /** Sets type of modifications tracking for the alignment */
    void setTrackMod(U2OpStatus &os, U2TrackModType trackMod);

    const MultipleAlignment& getMultipleAlignment() const;
    void setMultipleAlignment(const MultipleAlignment &ma, MaModificationInfo mi = MaModificationInfo(), const QVariantMap &hints = QVariantMap());

    const MultipleAlignment getMultipleAlignmentCopy() const;

    /** GObject methods */
    void setGObjectName(const QString &newName);

    /** Const getters */
    const DNAAlphabet * getAlphabet() const;
    qint64 getLength() const;
    qint64 getNumRows() const;
    const QList<MultipleAlignmentRow>& getRows() const;
    const MultipleAlignmentRow getRow(int row) const;
    int getRowPosById(qint64 rowId) const;
    virtual char charAt(int seqNum, qint64 position) const = 0;

    U2MsaListGapModel getGapModel() const;

    /**
     * Converts MA indexes into ids.
     * If index is out of range and 'excludeErrors' is false appends '-1' to the result list.
     */
    QList<qint64> convertMaRowIndexesToMaRowIds(const QList<int>& maRowIndexes, bool excludeErrors = true);

    /**
     * Converts MA ids into indexes.
     * If id is invalid and 'excludeErrors' is false appends '-1' to the result list.
     */
    QList<int> convertMaRowIdsToMaRowIndexes(const QList<qint64>& maRowIds, bool excludeErrors = true);

    /** Removes single row from the alignment by row index. */
    void removeRow(int rowIdx);

    /** Removes all rows from the list from the alignment by row indexes. */
    void removeRows(const QList<int>& rowIndexes);

    /** Removes columns region from all rows in the list. */
    void removeRegion(const QList<int>& rowIndexes, int x, int width, bool removeEmptyRows);

    /** Method that affect the whole alignment, including sequences */
    void removeRegion(int startPos, int startRow, int nBases, int nRows, bool removeEmptyRows, bool track = true);

    /** Renames row with a given index. */
    void renameRow(int rowIdx, const QString& newName);

    void moveRowsBlock(int firstRow, int numRows, int delta);

    bool isRegionEmpty(int x, int y, int width, int height) const;

    bool isRegionEmpty(const QList<int>& rowIndexes, int x, int width) const;

    QList<qint64> getRowsOrder(U2OpStatus& os) const;
    /**
     * Updates the rows order.
     * There must be one-to-one correspondence between the specified rows IDs
     * and rows IDs of the alignment.
     */
    void updateRowsOrder(U2OpStatus &os, const QList<qint64> &rowIds);

    void changeLength(U2OpStatus &os, qint64 newLength);

    void updateCachedMultipleAlignment(const MaModificationInfo &mi = MaModificationInfo(), const QList<qint64> &removedRowIds = QList<qint64>());
    void sortRowsByList(const QStringList &order);

    virtual void replaceCharacter(int startPos, int rowIndex, char newChar) = 0;

    /** Inserts gap into 'pos' for the given rows. */
    virtual void insertGap(const U2Region &rows, int pos, int nGaps) = 0;

    /** Inserts gap into 'pos' for the given rows. */
    virtual void insertGapByRowIndexList(const QList<int>& rowIndexes, int pos, int nGaps) = 0;

    /**
     * Removes gap region that extends from the @pos column and is no longer than @maxGaps.
     * If the region starting from @pos and having width of @maxGaps includes any non-gap symbols
     * then its longest subset starting from @pos and containing gaps only is removed.
     *
     * If the given region is a subset of a trailing gaps area then nothing happens.
     */
    int deleteGap(U2OpStatus& os, const U2Region& rows, int pos, int maxGaps);

    int deleteGapByRowIndexList(U2OpStatus& os, const QList<int>& rowIndexes, int pos, int maxGaps);

    virtual void deleteColumnsWithGaps(U2OpStatus &os, int requiredGapsCount = -1) = 0;

    /**
     * Performs shift of the region specified by parameters @startPos (leftmost column number),
     * @startRow (top row number), @nBases (region width), @nRows (region height) in no more
     * than @shift bases.
     *
     * @startPos and @startRow must be non-negative numbers, @nBases and @nRows - strictly
     * positive. The sign of @shift parameter specifies the direction of shifting: positive
     * for right direction, negative for left one. If 0 == @shift nothing happens.
     *
     * Shifting to the left may be performed only if a region preceding the selection
     * and having the same height consists of gaps only. In this case selected region
     * is moved to the left in the width of the preceding gap region but no more
     * than |@shift| bases.
     *
     * Returns shift size, besides sign of the returning value specifies direction of the shift
     */
    int shiftRegion(int startPos, int startRow, int nBases, int nRows, int shift);

    void saveState();
    void releaseState();

signals:
    void si_startMaUpdating();
    void si_alignmentChanged(const MultipleAlignment &maBefore, const MaModificationInfo &modInfo);
    void si_alignmentBecomesEmpty(bool isEmpty);
    void si_completeStateChanged(bool complete);
    void si_rowsRemoved(const QList<qint64> &rowIds);
    void si_invalidateAlignmentObject();
    void si_alphabetChanged(const MaModificationInfo &mi, const DNAAlphabet *prevAlphabet);

protected:
    virtual void loadAlignment(U2OpStatus &os) = 0;
    virtual void updateCachedRows(U2OpStatus &os, const QList<qint64> &rowIds) = 0;
    virtual void updateDatabase(U2OpStatus &os, const MultipleAlignment &ma) = 0;
    virtual void removeRowPrivate(U2OpStatus &os, const U2EntityRef &maRef, qint64 rowId) = 0;
    virtual void removeRegionPrivate(U2OpStatus &os, const U2EntityRef &maRef, const QList<qint64> &rows,
                                     int startPos, int nBases) = 0;
    void insertGap(const U2Region &rows, int pos, int nGaps, bool collapseTrailingGaps);

    void insertGapByRowIndexList(const QList<int>& rowIndexes, int pos, int nGaps, bool collapseTrailingGaps);

    void insertGapByRowIdList(const QList<qint64>& rowIds, int pos, int nGaps, bool collapseTrailingGaps);

    MultipleAlignment cachedMa;

private:
    void loadDataCore(U2OpStatus &os);

    /**
     * Returns maximum count of subsequent gap columns in the region that starts from column
     * with @pos number, has width of @maxGaps and includes the rows specified by @rows.
     * @maxGaps, @pos are to be non-negative numbers. Gap columns should finish in column
     * having @pos + @maxGaps number, otherwise 0 is returned. If the region is located
     * in the MSA trailing gaps area, then 0 is returned.
     */
    int getMaxWidthOfGapRegion(U2OpStatus &os, const QList<int>& rowIndexes, int pos, int maxGaps);

    MaSavedState savedState;
};

}   // namespace U2

#endif // _U2_MULTIPLE_ALIGNMENT_OBJECT_H_
