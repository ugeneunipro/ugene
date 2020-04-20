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

#ifndef _U2_MULTIPLE_CHROMATOGRAM_ALIGNMENT_H_
#define _U2_MULTIPLE_CHROMATOGRAM_ALIGNMENT_H_

#include "MultipleAlignment.h"
#include "MultipleChromatogramAlignmentRow.h"

namespace U2 {

class McaRowMemoryData;
class MultipleChromatogramAlignmentData;

class U2CORE_EXPORT MultipleChromatogramAlignment : public MultipleAlignment {
public:
    MultipleChromatogramAlignment();
    MultipleChromatogramAlignment(const MultipleAlignment &ma);
    MultipleChromatogramAlignment(MultipleChromatogramAlignmentData *mcaData);
    MultipleChromatogramAlignment(const QString &name,
                                  const DNAAlphabet *alphabet = NULL,
                                  const QList<MultipleChromatogramAlignmentRow> &rows = QList<MultipleChromatogramAlignmentRow>());

    MultipleChromatogramAlignmentData *data() const;

    MultipleChromatogramAlignmentData &operator*();
    const MultipleChromatogramAlignmentData &operator*() const;

    MultipleChromatogramAlignmentData *operator->();
    const MultipleChromatogramAlignmentData *operator->() const;

    MultipleChromatogramAlignment clone() const;

private:
    QSharedPointer<MultipleChromatogramAlignmentData> getMcaData() const;
};

/**
 * Multiple chromatogram alignment
 * The length of the alignment is the maximum length of its rows.
 * There are minimal checks on the alignment's alphabet, but the client of the class
 * is expected to keep the conformance of the data and the alphabet.
 */
class U2CORE_EXPORT MultipleChromatogramAlignmentData : public MultipleAlignmentData {
    friend class MultipleChromatogramAlignment;

protected:
    /**
     * Creates a new alignment.
     * The name must be provided if this is not default alignment.
     */
    MultipleChromatogramAlignmentData(const QString &name = QString(),
                                      const DNAAlphabet *alphabet = NULL,
                                      const QList<MultipleChromatogramAlignmentRow> &rows = QList<MultipleChromatogramAlignmentRow>());
    MultipleChromatogramAlignmentData(const MultipleChromatogramAlignmentData &mcaData);

public:
    MultipleChromatogramAlignmentData &operator=(const MultipleChromatogramAlignment &mca);
    MultipleChromatogramAlignmentData &operator=(const MultipleChromatogramAlignmentData &mcaData);

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
     * Sorts rows by similarity making identical rows sequential.
     * Returns 'true' if the rows were resorted, and 'false' otherwise.
     */
    bool sortRowsBySimilarity(QVector<U2Region> &united);

    /** Returns row of the alignment */
    inline MultipleChromatogramAlignmentRow getMcaRow(int row);
    inline const MultipleChromatogramAlignmentRow getMcaRow(int row) const;
    const MultipleChromatogramAlignmentRow getMcaRow(const QString &name) const;

    /** Returns all rows in the alignment */
    const QList<MultipleChromatogramAlignmentRow> getMcaRows() const;

    MultipleChromatogramAlignmentRow getMcaRowByRowId(qint64 rowId, U2OpStatus &os) const;

    /** Returns a character (a gap or a non-gap) in the specified row and position */
    char charAt(int rowNumber, int pos) const;
    bool isGap(int rowNumber, int pos) const;
    bool isTrailingOrLeadingGap(int rowNumber, int pos) const;

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
    void setRowContent(int rowNumber, const DNAChromatogram &chromatogram, const QByteArray &sequence, int offset = 0);
    void setRowContent(int rowNumber, const DNAChromatogram &chromatogram, const DNASequence &sequence, const U2MsaRowGapModel &gapModel);
    void setRowContent(int rowNumber, const McaRowMemoryData &mcaRowMemoryData);

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
    MultipleChromatogramAlignment mid(int start, int len) const;

    void setRowGapModel(int rowNumber, const QList<U2MsaGap> &gapModel);

    void setSequenceId(int rowIndex, const U2DataId &sequenceId);

    /**
     * Adds a new row to the alignment.
     * If rowIndex == -1 -> appends the row to the alignment.
     * Otherwise, if rowIndex is incorrect, the closer bound is used (the first or the last row).
     * Does not trim the original alignment.
     * Can increase the overall alignment length.
     */
    void addRow(const QString &name, const DNAChromatogram &chromatogram, const QByteArray &bytes);
    void addRow(const QString &name, const DNAChromatogram &chromatogram, const QByteArray &bytes, int rowIndex);
    void addRow(const U2MsaRow &rowInDb, const DNAChromatogram &chromatogram, const DNASequence &sequence, U2OpStatus &os);
    void addRow(const QString &name, const DNAChromatogram &chromatogram, const DNASequence &sequence, const U2MsaRowGapModel &gaps, U2OpStatus &os);
    void addRow(const U2MsaRow &rowInDb, const McaRowMemoryData &mcaRowMemoryData, U2OpStatus &os);

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

    /** returns "True" if there are no gaps in the alignment */
    bool hasEmptyGapModel() const;

    /**  returns "True" if all sequences in the alignment have equal lengths */
    bool hasEqualLength() const;

    /**
     * Joins two alignments. Alignments must have the same size and alphabet.
     * Increases the alignment length.
     */
    MultipleChromatogramAlignmentData &operator+=(const MultipleChromatogramAlignmentData &mcaData);

    /**
     * Compares two alignments: lengths, alphabets, rows and infos (that include names).
     */
    bool operator==(const MultipleChromatogramAlignmentData &mcaData) const;
    bool operator!=(const MultipleChromatogramAlignmentData &mcaData) const;

    MultipleAlignment getCopy() const;
    MultipleChromatogramAlignment getExplicitCopy() const;

private:
    void copy(const MultipleAlignmentData &other);
    void copy(const MultipleChromatogramAlignmentData &other);
    MultipleAlignmentRow getEmptyRow() const;

    /** Create a new row (sequence + gap model) from the bytes */
    MultipleChromatogramAlignmentRow createRow(const QString &name, const DNAChromatogram &chromatogram, const QByteArray &bytes);

    /**
     * Sequence must not contain gaps.
     * All gaps in the gaps model (in 'rowInDb') must be valid and have an offset within the bound of the sequence.
     */
    MultipleChromatogramAlignmentRow createRow(const U2MsaRow &rowInDb, const DNAChromatogram &chromatogram, const DNASequence &sequence, const U2MsaRowGapModel &gaps, U2OpStatus &os);

    MultipleChromatogramAlignmentRow createRow(const MultipleChromatogramAlignmentRow &row);

    void setRows(const QList<MultipleChromatogramAlignmentRow> &mcaRows);
};

inline MultipleChromatogramAlignmentRow MultipleChromatogramAlignmentData::getMcaRow(int rowIndex) {
    return getRow(rowIndex).dynamicCast<MultipleChromatogramAlignmentRow>();
}

inline const MultipleChromatogramAlignmentRow MultipleChromatogramAlignmentData::getMcaRow(int rowIndex) const {
    return getRow(rowIndex).dynamicCast<const MultipleChromatogramAlignmentRow>();
}

inline bool	operator!=(const MultipleChromatogramAlignment &ptr1, const MultipleChromatogramAlignment &ptr2) { return *ptr1 != *ptr2; }
inline bool	operator!=(const MultipleChromatogramAlignment &ptr1, const MultipleChromatogramAlignmentData *ptr2) { return *ptr1 != *ptr2; }
inline bool	operator!=(const MultipleChromatogramAlignmentData *ptr1, const MultipleChromatogramAlignment &ptr2) { return *ptr1 != *ptr2; }
inline bool	operator==(const MultipleChromatogramAlignment &ptr1, const MultipleChromatogramAlignment &ptr2) { return *ptr1 == *ptr2; }
inline bool	operator==(const MultipleChromatogramAlignment &ptr1, const MultipleChromatogramAlignmentData *ptr2) { return *ptr1 == *ptr2; }
inline bool	operator==(const MultipleChromatogramAlignmentData *ptr1, const MultipleChromatogramAlignment &ptr2) { return *ptr1 == *ptr2; }

}   // namespace U2

#endif // _U2_MULTIPLE_CHROMATOGRAM_ALIGNMENT_H_
