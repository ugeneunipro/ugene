/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include <QVariantMap>

#include "MsaRow.h"

namespace U2 {

class DNAAlphabet;

/** Default name for a multiple alignment */
#define MA_OBJECT_NAME QString("Multiple alignment")

class MsaData;

/** Cached, in-memory MSA/MCA object model. */
class U2CORE_EXPORT Msa {
public:
    Msa(MsaData* maData);
    Msa(const QString& name = "", const DNAAlphabet* alphabet = nullptr);

    enum Order {
        Ascending,
        Descending
    };

    /** Type of the rows sorting algorithm. */
    enum SortType {
        /** Sorts rows by locale-insensitive name. */
        SortByName = 1,

        /** Sorts rows by the row sequence length (with no gaps). */
        SortByLength = 2,

        /** Sorts rows by the leading gap size. */
        SortByLeadingGap = 3,
    };

    virtual ~Msa() = default;

    MsaData* data() const;

    MsaData& operator*();
    const MsaData& operator*() const;

    MsaData* operator->();
    const MsaData* operator->() const;

protected:
    QSharedPointer<MsaData> maData;
};

/**
 * Multiple alignment
 * The length of the alignment is the maximum length of its rows.
 * There are minimal checks on the alignment's alphabet, but the client of the class
 * is expected to keep the conformance of the data and the alphabet.
 */
class U2CORE_EXPORT MsaData {
public:
    /**
     * Creates a new alignment.
     * The name must be provided if this is not default alignment.
     */
    MsaData(const QString& name = "",
            const DNAAlphabet* alphabet = nullptr,
            const QVector<MsaRow>& rows = {});

    MsaData(const MsaData& data);

    virtual ~MsaData() = default;

    /**
     * Clears the alignment. Makes alignment length == 0.
     * Doesn't change alphabet or name
     */
    void clear();

    /**
     * Recomputes the length of the alignment and makes it as minimal
     * as possible. All leading gaps columns are removed by default.
     * Returns "true" if the alignment has been modified.
     */
    bool trim(bool removeLeadingGaps = true);

    /**
     * Removes all gaps from all columns in the alignment.
     * Returns "true" if the alignment has been changed.
     */
    bool simplify();

    /**
     * Inserts 'count' gaps into the specified position.
     * Can increase the overall alignment length.
     */
    void insertGaps(int row, int pos, int count, U2OpStatus& os);

    /** Returns  the name of the alignment */
    QString getName() const;

    /** Sets the name of the alignment */
    void setName(const QString& newName);

    /** Returns the alphabet of the alignment */
    const DNAAlphabet* getAlphabet() const;

    /**
     * Sets the alphabet of the alignment, the value can't be NULL.
     * Warning: rows already present in the alignment are not verified to correspond to this alphabet
     */
    void setAlphabet(const DNAAlphabet* alphabet);

    /** Returns the alignment info */
    QVariantMap getInfo() const;

    /** Sets the alignment info */
    void setInfo(const QVariantMap& info);

    /** Returns true if the length of the alignment is 0 */
    bool isEmpty() const;

    /** Returns the length of the alignment */
    int getLength() const;

    /** Sets the length of the alignment. The length must be >= 0. */
    void setLength(int length);

    /** Returns the number of rows in the alignment */
    int getRowCount() const;

    QList<QVector<U2MsaGap>> getGapModel() const;

    /** Sorts rows. If range is provided and is not empty sorts only given range. */
    void sortRows(const Msa::SortType& sortType,
                  Msa::Order order = Msa::Ascending,
                  const U2Region& range = {});

    /** Returns row of the alignment */
    MsaRow getRow(int row);
    const MsaRow& getRow(int row) const;
    const MsaRow& getRow(const QString& name) const;

    /** Returns all rows in the alignment */
    const QVector<MsaRow>& getRows() const;

    /** Returns IDs of the alignment rows in the database */
    QList<qint64> getRowsIds() const;

    /** Returns row ids by row indexes. */
    QList<qint64> getRowIdsByRowIndexes(const QList<int>& rowIndexes) const;

    MsaRow getRowByRowId(qint64 rowId, U2OpStatus& os) const;

    char charAt(int rowNumber, qint64 position) const;
    bool isGap(int rowNumber, qint64 pos) const;
    bool isLeadingOrTrailingGap(int rowNumber, qint64 pos) const;

    /** Returns all rows' names in the alignment */
    QStringList getRowNames() const;

    int getRowIndexByRowId(qint64 rowId, U2OpStatus& os) const;

    /**
     * Renames the row with the specified index.
     * Assumes that the row index is valid and the name is not empty.
     */
    void renameRow(int rowIndex, const QString& name);

    /** Keep only only column-range of given rows in the alignment. */
    bool crop(const QList<qint64>& rowIds, const U2Region& columnRange, U2OpStatus& os);

    /** Keeps only 'columnRange' region in the alignment */
    bool crop(const U2Region& columnRange, U2OpStatus& os);

    /** Updates row ID of the row at 'rowIndex' position */
    void setRowId(int rowIndex, qint64 rowId);

    /**
     * Removes a row from alignment.
     * The alignment is changed only (to zero) if the alignment becomes empty.
     */
    void removeRow(int rowIndex, U2OpStatus& os);

    /**
     * Removes a region from the alignment.
     * If "removeEmptyRows" is "true", removes all empty rows from the processed region.
     * The alignment is trimmed after removing the region.
     * Can decrease the overall alignment length.
     */
    void removeRegion(int startPos, int startRow, int nBases, int nRows, bool removeEmptyRows);

    /**
     * Removes up to n characters starting from the specified position.
     * Can decrease the overall alignment length.
     */
    void removeChars(int row, int pos, int n, U2OpStatus& os);

    /**
     * Shifts a selection of consequent rows.
     * 'delta' can be positive or negative.
     * It is assumed that indexes of shifted rows are within the bounds of the [0, number of rows).
     */
    void moveRowsBlock(int startRow, int numRows, int delta);

    /**
     * Returns true if 2 alignments have the same alphabets, dimensions and the same ordered list of rows.
     * To compare rows the method calls row.isEqual(otherRow) method.
     */
    bool isEqual(const MsaData& other) const;

    /** Calls isEqual() method. */
    bool operator==(const MsaData& other) const;

    /** Returns !isEqual() method result. */
    bool operator!=(const MsaData& ma) const;

    /** Checks model consistency */
    virtual void check() const;

    /** Arranges rows in lists order*/
    bool sortRowsByList(const QStringList& order);

    Msa getCopy() const;

    /**
     * Sorts rows by similarity making identical rows sequential. Sets MSA rows to the sorted rows.
     * Returns 'true' if the rows were resorted and MSA is changed, and 'false' otherwise.
     */
    bool sortRowsBySimilarity(QVector<U2Region>& united);

    /** Returns rows sorted by similarity. Does not update MSA. */
    QVector<MsaRow> getRowsSortedBySimilarity(QVector<U2Region>& united) const;

    /**
     * Sets the new content for the row with the specified index.
     * Assumes that the row index is valid.
     * Can modify the overall alignment length (increase or decrease).
     */
    void setRowContent(int rowNumber, const QByteArray& sequence, int offset = 0);
    void setRowContent(int rowNumber, const Chromatogram& chromatogram, const DNASequence& sequence, const QVector<U2MsaGap>& gapModel);

    /** Converts all rows' sequences to upper case */
    void toUpperCase();

    void setSequenceId(int rowIndex, const U2DataId& sequenceId);

    void setRowGapModel(int rowNumber, const QVector<U2MsaGap>& gapModel);

    /**
     * Adds a new row to the alignment.
     * If rowIndex == -1 -> appends the row to the alignment.
     * Otherwise, if rowIndex is incorrect, the closer bound is used (the first or the last row).
     * Does not trim the original alignment.
     * Can increase the overall alignment length.
     */
    void addRow(const QString& name, const QByteArray& bytes);
    void addRow(const QString& name, const QByteArray& bytes, int rowIndex);
    void addRow(const U2MsaRow& rowInDb, const DNASequence& sequence, U2OpStatus& os);
    void addRow(const QString& name, const DNASequence& sequence, const QVector<U2MsaGap>& gaps, U2OpStatus& os);

    /**
     * Adds a new row to the alignment.
     * Does not trim the original alignment.
     * Can increase the overall alignment length.
     */
    void addRow(const U2MsaRow& rowInDb,
                const DNASequence& sequence,
                const U2DataId& chromatogramId,
                const Chromatogram& chromatogram,
                U2OpStatus& os);

    void addRow(const QString& name,
                const DNASequence& sequence,
                const QVector<U2MsaGap>& gaps,
                const U2DataId& chromatogramId,
                const Chromatogram& chromatogram,
                U2OpStatus& os);

    /**
     * Replaces all occurrences of 'origChar' by 'resultChar' in the row with the specified index.
     * The 'origChar' must be a non-gap character.
     * The 'resultChar' can be a gap, gaps model is recalculated in this case.
     * The index must be valid as well.
     */
    void replaceChars(int row, char origChar, char resultChar);

    /**
     * Appends chars to the row with the specified index.
     * The chars are appended to the alignment end, not to the row end
     * (i.e. the alignment length is taken into account).
     * Does NOT recalculate the alignment length!
     * The index must be valid.
     */
    void appendChars(int row, const char* str, int len);

    void appendChars(int row, qint64 afterPos, const char* str, int len);

    void appendRow(int rowNumber, qint64 afterPos, const MsaRow& rowIdx, U2OpStatus& os);

    /** returns "True" if there are no gaps in the alignment */
    bool hasEmptyGapModel() const;

    /**  returns "True" if all sequences in the alignment have equal lengths */
    bool hasEqualLength() const;

    /**
     * Creates a new alignment from the sub-alignment. Do not trims the result.
     * Assumes that 'start' >= 0, and 'start + len' is less or equal than the alignment length.
     */
    Msa mid(int start, int len) const;

    /**
     * Joins two alignments. Alignments must have the same size and alphabet.
     * Increases the alignment length.
     */
    MsaData& operator+=(const MsaData& ma);

private:
    /** Helper-method for adding a row to the alignment */
    void addRowPrivate(const MsaRow& row, qint64 rowLenWithTrailingGaps, int rowIndex);

    /** Create a new row (sequence + gap model) from the bytes */
    MsaRow createRow(const QString& name, const QByteArray& bytes);

    /**
     * Sequence must not contain gaps.
     * All gaps in the gaps model (in 'rowInDb') must be valid and have an offset within the bound of the sequence.
     */
    MsaRow createRow(const U2MsaRow& rowInDb, const DNASequence& sequence, const QVector<U2MsaGap>& gaps, U2OpStatus& os);

    MsaRow createRow(const MsaRow& row);

    /**
     * Sequence must not contain gaps.
     * All gaps in the gaps model (in 'rowInDb') must be valid and have an offset within the bound of the sequence.
     */
    MsaRow createRow(const U2MsaRow& rowInDb,
                     const DNASequence& sequence,
                     const QVector<U2MsaGap>& gaps,
                     const U2DataId& chromatogramId,
                     const Chromatogram& chromatogram,
                     U2OpStatus& os);

    void copyFrom(const MsaData& other);

protected:
    /** Alphabet for all sequences in the alignment */
    const DNAAlphabet* alphabet = nullptr;

    /** Alignment rows (each row = sequence + gap model) */
    QVector<MsaRow> rows;

    /** The length of the longest row in the alignment */
    qint64 length = 0;

    /** Additional alignment info */
    QVariantMap info;
};

inline bool operator!=(const Msa& ptr1, const Msa& ptr2) {
    return *ptr1 != *ptr2;
}
inline bool operator!=(const Msa& ptr1, const MsaData* ptr2) {
    return *ptr1 != *ptr2;
}
inline bool operator!=(const MsaData* ptr1, const Msa& ptr2) {
    return *ptr1 != *ptr2;
}
inline bool operator==(const Msa& ptr1, const Msa& ptr2) {
    return *ptr1 == *ptr2;
}
inline bool operator==(const Msa& ptr1, const MsaData* ptr2) {
    return *ptr1 == *ptr2;
}
inline bool operator==(const MsaData* ptr1, const Msa& ptr2) {
    return *ptr1 == *ptr2;
}

}  // namespace U2
