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

#include <U2Core/U2SafePoints.h>

#include "MultipleAlignment.h"
#include "MultipleAlignmentRow.h"

namespace U2 {

class MultipleSequenceAlignmentData;
class U2Region;

#define MA_OBJECT_NAME QString("Multiple alignment")

class U2CORE_EXPORT MultipleSequenceAlignment : public MultipleAlignment {
public:
    MultipleSequenceAlignment(const QString& name = QString(),
                              const DNAAlphabet* alphabet = nullptr,
                              const QList<MultipleAlignmentRow>& rows = {});
    MultipleSequenceAlignment(const MultipleAlignment& ma);
    MultipleSequenceAlignment(MultipleSequenceAlignmentData* msaData);

    MultipleSequenceAlignmentData* data() const;

    MultipleSequenceAlignmentData& operator*();
    const MultipleSequenceAlignmentData& operator*() const;

    MultipleSequenceAlignmentData* operator->();
    const MultipleSequenceAlignmentData* operator->() const;

    MultipleSequenceAlignment clone() const;
    template<class Derived>
    inline Derived dynamicCast() const;

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
    MultipleSequenceAlignmentData(const QString& name = QString(),
                                  const DNAAlphabet* alphabet = nullptr,
                                  const QList<MultipleAlignmentRow>& rows = QList<MultipleAlignmentRow>());
    MultipleSequenceAlignmentData(const MultipleSequenceAlignmentData& msaData);

public:
    MultipleSequenceAlignmentData& operator=(const MultipleSequenceAlignment& msa);
    MultipleSequenceAlignmentData& operator=(const MultipleSequenceAlignmentData& msaData);

    /**
     * Creates a new alignment from the sub-alignment. Do not trims the result.
     * Assumes that 'start' >= 0, and 'start + len' is less or equal than the alignment length.
     */
    MultipleSequenceAlignment mid(int start, int len) const;

    virtual void setRowGapModel(int rowNumber, const QVector<U2MsaGap>& gapModel);

    void setSequenceId(int rowIndex, const U2DataId& sequenceId);

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

    void appendRow(int rowNumber, qint64 afterPos, const MultipleAlignmentRow& rowIdx, U2OpStatus& os);

    /** returns "True" if there are no gaps in the alignment */
    bool hasEmptyGapModel() const;

    /**  returns "True" if all sequences in the alignment have equal lengths */
    bool hasEqualLength() const;

    /**
     * Joins two alignments. Alignments must have the same size and alphabet.
     * Increases the alignment length.
     */
    MultipleSequenceAlignmentData& operator+=(const MultipleSequenceAlignmentData& ma);

    /** Compares two alignments: calls isEqual() method. */
    bool operator==(const MultipleSequenceAlignmentData& msaData) const;
    bool operator!=(const MultipleSequenceAlignmentData& msaData) const;

    MultipleAlignment getCopy() const;
    MultipleSequenceAlignment getExplicitCopy() const;

private:
    void copy(const MultipleAlignmentData& other);
    void copy(const MultipleSequenceAlignmentData& other);
    MultipleAlignmentRow getEmptyRow() const;

    /** Create a new row (sequence + gap model) from the bytes */
    MultipleAlignmentRow createRow(const QString& name, const QByteArray& bytes);

    /**
     * Sequence must not contain gaps.
     * All gaps in the gaps model (in 'rowInDb') must be valid and have an offset within the bound of the sequence.
     */
    MultipleAlignmentRow createRow(const U2MsaRow& rowInDb, const DNASequence& sequence, const QVector<U2MsaGap>& gaps, U2OpStatus& os);

    MultipleAlignmentRow createRow(const MultipleAlignmentRow& row);
};

inline bool operator!=(const MultipleSequenceAlignment& ptr1, const MultipleSequenceAlignment& ptr2) {
    return *ptr1 != *ptr2;
}
inline bool operator!=(const MultipleSequenceAlignment& ptr1, const MultipleSequenceAlignmentData* ptr2) {
    return *ptr1 != *ptr2;
}
inline bool operator!=(const MultipleSequenceAlignmentData* ptr1, const MultipleSequenceAlignment& ptr2) {
    return *ptr1 != *ptr2;
}
inline bool operator==(const MultipleSequenceAlignment& ptr1, const MultipleSequenceAlignment& ptr2) {
    return *ptr1 == *ptr2;
}
inline bool operator==(const MultipleSequenceAlignment& ptr1, const MultipleSequenceAlignmentData* ptr2) {
    return *ptr1 == *ptr2;
}
inline bool operator==(const MultipleSequenceAlignmentData* ptr1, const MultipleSequenceAlignment& ptr2) {
    return *ptr1 == *ptr2;
}

}  // namespace U2
