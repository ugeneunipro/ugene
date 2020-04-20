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

#ifndef _U2_MULTIPLE_SEQUENCE_ALIGNMENT_H_
#define _U2_MULTIPLE_SEQUENCE_ALIGNMENT_H_

#include <U2Core/U2SafePoints.h>

#include "MultipleAlignment.h"
#include "MultipleSequenceAlignmentRow.h"

namespace U2 {

class MultipleSequenceAlignmentData;
class U2Region;

#define MA_OBJECT_NAME QString("Multiple alignment")

class U2CORE_EXPORT MultipleSequenceAlignment : public MultipleAlignment {
public:
    MultipleSequenceAlignment(const QString &name = QString(),
        const DNAAlphabet *alphabet = NULL,
        const QList<MultipleSequenceAlignmentRow> &rows = QList<MultipleSequenceAlignmentRow>());
    MultipleSequenceAlignment(const MultipleAlignment &ma);
    MultipleSequenceAlignment(MultipleSequenceAlignmentData *msaData);

    MultipleSequenceAlignmentData * data() const;

    MultipleSequenceAlignmentData & operator*();
    const MultipleSequenceAlignmentData & operator*() const;

    MultipleSequenceAlignmentData * operator->();
    const MultipleSequenceAlignmentData * operator->() const;

    MultipleSequenceAlignment clone() const;
    template <class Derived> inline Derived dynamicCast() const;

private:
    QSharedPointer<MultipleSequenceAlignmentData> getMsaData() const;
};

/**
 * Multiple sequence alignment
 * The length of the alignment is the maximum length of its rows.
 * There are minimal checks on the alignment's alphabet, but the client of the class
 * is expected to keep the conformance of the data and the alphabet.
 */
class U2CORE_EXPORT MultipleSequenceAlignmentData : public MultipleAlignmentData {
    friend class MultipleSequenceAlignment;

protected:
    /**
     * Creates a new alignment.
     * The name must be provided if this is not default alignment.
     */
    MultipleSequenceAlignmentData(const QString &name = QString(),
        const DNAAlphabet *alphabet = NULL,
        const QList<MultipleSequenceAlignmentRow> &rows = QList<MultipleSequenceAlignmentRow>());
    MultipleSequenceAlignmentData(const MultipleSequenceAlignmentData &msaData);

public:
    MultipleSequenceAlignmentData & operator=(const MultipleSequenceAlignment &msa);
    MultipleSequenceAlignmentData & operator=(const MultipleSequenceAlignmentData &msaData);

    /** Returns the number of rows in the alignment */
    int getNumRows() const;

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
     * Sorts rows by similarity making identical rows sequential. Sets MSA rows to the sorted rows.
     * Returns 'true' if the rows were resorted and MSA is changed, and 'false' otherwise.
     */
    bool sortRowsBySimilarity(QVector<U2Region> &united);

    /** Returns rows sorted by similarity. Does not update MSA. */
    QList<MultipleSequenceAlignmentRow> getRowsSortedBySimilarity(QVector<U2Region> &united) const;

    /** Returns row of the alignment */
    inline MultipleSequenceAlignmentRow getMsaRow(int row);
    inline const MultipleSequenceAlignmentRow getMsaRow(int row) const;
    const MultipleSequenceAlignmentRow getMsaRow(const QString &name) const;

    /** Returns all rows in the alignment */
    const QList<MultipleSequenceAlignmentRow> getMsaRows() const;

    MultipleSequenceAlignmentRow getMsaRowByRowId(qint64 rowId, U2OpStatus &os) const;

    /** Returns a character (a gap or a non-gap) in the specified row and position */
    char charAt(int rowNumber, int pos) const;
    bool isGap(int rowNumber, int pos) const;

    /**
     * Inserts 'count' gaps into the specified position.
     * Can increase the overall alignment length.
     */
    void insertGaps(int row, int pos, int count, U2OpStatus &os);

    /**
     * Removes a region from the alignment.
     * If "removeEmptyRows" is "true", removes all empty rows from the processed region.
     * The alignment is trimmed after removing the region.
     * Can decrease the overall alignment length.
     */
    void removeRegion(int startPos, int startRow, int nBases, int nRows, bool removeEmptyRows);

    /**
     * Renames the row with the specified index.
     * Assumes that the row index is valid and the name is not empty.
     */
    void renameRow(int row, const QString &name);

    /**
     * Sets the new content for the row with the specified index.
     * Assumes that the row index is valid.
     * Can modify the overall alignment length (increase or decrease).
     */
    void setRowContent(int rowNumber, const QByteArray &sequence, int offset = 0);

    /** Converts all rows' sequences to upper case */
    void toUpperCase();

    /**
     * Modifies the alignment by keeping data from the specified region and rows only.
     * Assumes that the region start is not negative, but it can be greater than a row length.
     */
    bool crop(const U2Region &region, const QSet<QString> &rowNames, U2OpStatus &os);
    bool crop(const U2Region &region, U2OpStatus &os);
    bool crop(int start, int count, U2OpStatus &os);

    /**
     * Creates a new alignment from the sub-alignment. Do not trims the result.
     * Assumes that 'start' >= 0, and 'start + len' is less or equal than the alignment length.
     */
    MultipleSequenceAlignment mid(int start, int len) const;

    virtual void setRowGapModel(int rowNumber, const QList<U2MsaGap> &gapModel);

    void setSequenceId(int rowIndex, const U2DataId &sequenceId);

    /**
     * Adds a new row to the alignment.
     * If rowIndex == -1 -> appends the row to the alignment.
     * Otherwise, if rowIndex is incorrect, the closer bound is used (the first or the last row).
     * Does not trim the original alignment.
     * Can increase the overall alignment length.
     */
    void addRow(const QString &name, const QByteArray &bytes);
    void addRow(const QString &name, const QByteArray &bytes, int rowIndex);
    void addRow(const U2MsaRow &rowInDb, const DNASequence &sequence, U2OpStatus &os);
    void addRow(const QString &name, const DNASequence &sequence, const QList<U2MsaGap> &gaps, U2OpStatus &os);

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
    void appendChars(int row, const char *str, int len);

    void appendChars(int row, qint64 afterPos, const char *str, int len);

    void appendRow(int rowNumber, const MultipleSequenceAlignmentRow &rowIdx, bool ignoreTrailingGaps, U2OpStatus &os);

    void appendRow(int rowNumber, qint64 afterPos, const MultipleSequenceAlignmentRow &rowIdx, U2OpStatus &os);

    /** returns "True" if there are no gaps in the alignment */
    bool hasEmptyGapModel() const;

    /**  returns "True" if all sequences in the alignment have equal lengths */
    bool hasEqualLength() const;

    /**
     * Joins two alignments. Alignments must have the same size and alphabet.
     * Increases the alignment length.
     */
    MultipleSequenceAlignmentData & operator+=(const MultipleSequenceAlignmentData &ma);

    /**
     * Compares two alignments: lengths, alphabets, rows and infos (that include names).
     */
    bool operator==(const MultipleSequenceAlignmentData &msaData) const;
    bool operator!=(const MultipleSequenceAlignmentData &msaData) const;

    MultipleAlignment getCopy() const;
    MultipleSequenceAlignment getExplicitCopy() const;

private:
    void copy(const MultipleAlignmentData &other);
    void copy(const MultipleSequenceAlignmentData &other);
    MultipleAlignmentRow getEmptyRow() const;

    /** Create a new row (sequence + gap model) from the bytes */
    MultipleSequenceAlignmentRow createRow(const QString &name, const QByteArray &bytes);

    /**
     * Sequence must not contain gaps.
     * All gaps in the gaps model (in 'rowInDb') must be valid and have an offset within the bound of the sequence.
     */
    MultipleSequenceAlignmentRow createRow(const U2MsaRow &rowInDb, const DNASequence &sequence, const QList<U2MsaGap> &gaps, U2OpStatus &os);

    MultipleSequenceAlignmentRow createRow(const MultipleSequenceAlignmentRow &row);

    void setRows(const QList<MultipleSequenceAlignmentRow> &msaRows);
};

inline MultipleSequenceAlignmentRow MultipleSequenceAlignmentData::getMsaRow(int rowIndex) {
    return getRow(rowIndex).dynamicCast<MultipleSequenceAlignmentRow>();
}

inline const MultipleSequenceAlignmentRow MultipleSequenceAlignmentData::getMsaRow(int rowIndex) const {
    return getRow(rowIndex).dynamicCast<const MultipleSequenceAlignmentRow>();
}

inline bool	operator!=(const MultipleSequenceAlignment &ptr1, const MultipleSequenceAlignment &ptr2) { return *ptr1 != *ptr2; }
inline bool	operator!=(const MultipleSequenceAlignment &ptr1, const MultipleSequenceAlignmentData *ptr2) { return *ptr1 != *ptr2; }
inline bool	operator!=(const MultipleSequenceAlignmentData *ptr1, const MultipleSequenceAlignment &ptr2) { return *ptr1 != *ptr2; }
inline bool	operator==(const MultipleSequenceAlignment &ptr1, const MultipleSequenceAlignment &ptr2) { return *ptr1 == *ptr2; }
inline bool	operator==(const MultipleSequenceAlignment &ptr1, const MultipleSequenceAlignmentData *ptr2) { return *ptr1 == *ptr2; }
inline bool	operator==(const MultipleSequenceAlignmentData *ptr1, const MultipleSequenceAlignment &ptr2) { return *ptr1 == *ptr2; }

}   // namespace U2

#endif
