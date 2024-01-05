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

#pragma once

#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObject.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/MaModificationInfo.h>
#include <U2Core/Msa.h>

namespace U2 {

class MsaSavedState {
public:
    MsaSavedState();
    ~MsaSavedState();

    bool hasState() const;
    Msa takeState();
    void setState(const Msa& ma);

private:
    Msa* lastState;
};

class U2CORE_EXPORT MsaObject : public GObject {
    Q_OBJECT
public:
    static const QString REFERENCE_SEQUENCE_ID_FOR_ALIGNMENT;

    enum TrimEdge {
        Left = 1,
        Right = 2
    };

    MsaObject(const QString& name,
                            const U2EntityRef& maRef,
                            const QVariantMap& hintsMap = {},
                            const Msa& alignment = {},
                            const GObjectType& objectType = GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);

    ~MsaObject() override;

    /** Sets type of modifications tracking for the alignment */
    void setTrackMod(U2OpStatus& os, U2TrackModType trackMod);

    /** Returns cached in-memory msa object model. */
    const Msa& getAlignment() const;
    void setMultipleAlignment(const Msa& ma, MaModificationInfo mi = MaModificationInfo(), const QVariantMap& hints = QVariantMap());

    /** GObject methods */
    void setGObjectName(const QString& newName) override;

    /** Const getters */
    const DNAAlphabet* getAlphabet() const;
    qint64 getLength() const;
    qint64 getRowCount() const;
    const QVector<MsaRow>& getRows() const;
    const MsaRow& getRow(int row) const;
    int getRowPosById(qint64 rowId) const;
    char charAt(int seqNum, qint64 position) const;

    /** Keeps only given row ids and given column range in the alignment. */
    void crop(const QList<qint64>& rowIds, const U2Region& columnRange);

    /** Keeps only given column range in the alignment. */
    void crop(const U2Region& columnRange);

    // inserts column of gaps with newChar at rowIndex row
    void insertCharacter(int rowIndex, int pos, char newChar);

    QList<QVector<U2MsaGap>> getGapModel() const;

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

    /** Removes all rows with the given ids from the alignment. */
    void removeRowsById(const QList<qint64>& rowIds);

    /** Removes columns region from all rows in the list. */
    void removeRegion(const QList<int>& rowIndexes, int x, int width, bool removeEmptyRows);

    /** Method that affect the whole alignment, including sequences */
    void removeRegion(int startPos, int startRow, int nBases, int nRows, bool removeEmptyRows, bool track = true);

    /** Renames row with a given index. */
    void renameRow(int rowIdx, const QString& newName);

    void moveRowsBlock(int firstRow, int numRows, int delta);

    bool isRegionEmpty(int x, int y, int width, int height) const;

    bool isRegionEmpty(const QList<int>& rowIndexes, int x, int width) const;

    /** Returns list of ordered row ids. */
    QList<qint64> getRowIds() const;

    /**
     * Returns list of row ids for the given row indexes.
     * Returns empty list if some row index is out of range.
     */
    QList<qint64> getRowIdsByRowIndexes(const QList<int>& rowIndexes) const;

    /**
     * Updates the rows order.
     * There must be one-to-one correspondence between the specified rows IDs
     * and rows IDs of the alignment.
     */
    void updateRowsOrder(U2OpStatus& os, const QList<qint64>& rowIds);

    void changeLength(U2OpStatus& os, qint64 newLength);

    void updateCachedMultipleAlignment(const MaModificationInfo& mi = MaModificationInfo(), const QList<qint64>& removedRowIds = QList<qint64>());
    void sortRowsByList(const QStringList& order);

    /** Replaces character in the row and changes the alphabet, if the current one does not contain the character. */
    void replaceCharacter(int startPos, int rowIndex, char newChar);

    /** Replaces sequence of characters from startPos with the new char. Updates alignment alphabet if needed*/
    void replaceCharacters(const U2Region& columnRange, int rowIndex, char newChar);

    /**
     * Removes gap region that extends from the @pos column and is no longer than @maxGaps.
     * If the region starting from @pos and having width of @maxGaps includes any non-gap symbols
     * then its longest subset starting from @pos and containing gaps only is removed.
     *
     * If the given region is a subset of a trailing gaps area then nothing happens.
     */
    int deleteGap(U2OpStatus& os, const U2Region& rows, int pos, int maxGaps);

    int deleteGapByRowIndexList(U2OpStatus& os, const QList<int>& rowIndexes, int pos, int maxGaps);

    void trimRow(int rowIndex, int currentPos, U2OpStatus& os, TrimEdge edge);

    /**
     * Updates the corresponding alternative mutations.
     * Set the second strongest character, if it's peak height percentage is more then @threshold and @showAlternativeMutations is true.
     */
    void updateAlternativeMutations(bool showAlternativeMutations, int threshold, U2OpStatus& os);

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
    /**
     * Returns true if there are any gaps except trailing.
     */
    bool hasNonTrailingGap() const;

    /**
     * Updates a gap model of the alignment.
     * The map must contain valid row IDs and corresponding gap models.
     */
    void updateGapModel(U2OpStatus& os, const QMap<qint64, QVector<U2MsaGap>>& rowsGapModel);
    void updateGapModel(const QList<MsaRow>& sourceRows);

    /** Methods to work with rows */
    void updateRow(U2OpStatus& os, int rowIdx, const QString& name, const QByteArray& seqBytes, const QVector<U2MsaGap>& gapModel);

    /** Replaces all characters in the alignment and updates alphabet if provided.*/
    void replaceAllCharacters(char oldChar, char newChar, const DNAAlphabet* newAlphabet = nullptr);

    /**
     * Updates MSA alphabet to the 'newAlphabet'.
     * Keeps only valid chars for the new alphabet, all invalid are replaced with the default symbol.
     * 'replacementMap' can be used to adjust characters conversion:
     *  - first a character is mapped using 'replacementMap'
     *  - next this character is checked that it is valid for the given alphabet.
     *
     * The 'replacementMap' can be either empty of should contain mapping for all possible 256 Latin1 chars.
     */
    void morphAlphabet(const DNAAlphabet* newAlphabet, const QByteArray& replacementMap = QByteArray());

    void insertGap(const U2Region& rows, int pos, int nGaps);

    void insertGapByRowIndexList(const QList<int>& rowIndexes, int pos, int nGaps);

    void insertGapByRowIdList(const QList<qint64>& rowIds, int pos, int nGaps);

    U2SequenceObject* getReferenceObj() const;

    void deleteColumnsWithGaps(U2OpStatus& os, int requiredGapsCount = -1);

    MsaObject* clone(const U2DbiRef& dstDbiRef, U2OpStatus& os, const QVariantMap& hints = {}) const override;

signals:
    void si_startMaUpdating();
    void si_alignmentChanged(const Msa& maBefore, const MaModificationInfo& modInfo);
    void si_alignmentBecomesEmpty(bool isEmpty);
    void si_completeStateChanged(bool complete);
    void si_rowsRemoved(const QList<qint64>& rowIds);
    void si_invalidateAlignmentObject();
    void si_alphabetChanged(const MaModificationInfo& mi, const DNAAlphabet* prevAlphabet);

protected:
    void loadAlignment(U2OpStatus& os);
    void updateCachedRows(U2OpStatus& os, const QList<qint64>& rowIds);
    void updateDatabase(U2OpStatus& os, const Msa& ma);
    void removeRowPrivate(U2OpStatus& os, const U2EntityRef& maRef, qint64 rowId);
    void removeRegionPrivate(U2OpStatus& os, const U2EntityRef& maRef, const QList<qint64>& rows, int startPos, int nBases);

    void loadDataCore(U2OpStatus& os) override;

    /**
     * Returns maximum count of subsequent gap columns in the region that starts from column
     * with @pos number, has width of @maxGaps and includes the rows specified by @rows.
     * @maxGaps, @pos are to be non-negative numbers. Gap columns should finish in column
     * having @pos + @maxGaps number, otherwise 0 is returned. If the region is located
     * in the MSA trailing gaps area, then 0 is returned.
     */
    int getMaxWidthOfGapRegion(U2OpStatus& os, const QList<int>& rowIndexes, int pos, int maxGaps);

    Msa cachedMa;

    MsaSavedState savedState;

    /** Lazily loaded reference sequence object. Present only for Mca objects. */
    mutable U2SequenceObject* referenceObj = nullptr;
};

}  // namespace U2