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

#ifndef _U2_MULTIPLE_CHROMATOGRAM_ALIGNMENT_ROW_H_
#define _U2_MULTIPLE_CHROMATOGRAM_ALIGNMENT_ROW_H_

#include <U2Core/DNAChromatogram.h>
#include <U2Core/DNASequence.h>
#include <U2Core/McaRowInnerData.h>
#include <U2Core/MsaRowUtils.h>
#include <U2Core/U2Msa.h>

#include "MultipleAlignmentRow.h"

namespace U2 {

class MultipleChromatogramAlignment;
class MultipleChromatogramAlignmentData;
class MultipleChromatogramAlignmentRowData;
class U2OpStatus;

class U2CORE_EXPORT MultipleChromatogramAlignmentRow : public MultipleAlignmentRow {
public:
    MultipleChromatogramAlignmentRow();
    MultipleChromatogramAlignmentRow(const MultipleAlignmentRow &maRow);
    MultipleChromatogramAlignmentRow(MultipleChromatogramAlignmentData *mcaData);
    MultipleChromatogramAlignmentRow(MultipleChromatogramAlignmentRowData *mcaRowData);

    /** Creates a row in memory. */
    MultipleChromatogramAlignmentRow(const U2McaRow &rowInDb, const DNAChromatogram &chromatogram, const DNASequence &sequence, const U2MsaRowGapModel &gaps, MultipleChromatogramAlignmentData *mcaData);
    MultipleChromatogramAlignmentRow(const U2McaRow &rowInDb, const QString &rowName, const DNAChromatogram &chromatogram, const QByteArray &rawData, MultipleChromatogramAlignmentData *mcaData);
    MultipleChromatogramAlignmentRow(const MultipleChromatogramAlignmentRow &row, MultipleChromatogramAlignmentData *mcaData);

    MultipleChromatogramAlignmentRowData *data() const;

    MultipleChromatogramAlignmentRowData &operator*();
    const MultipleChromatogramAlignmentRowData &operator*() const;

    MultipleChromatogramAlignmentRowData *operator->();
    const MultipleChromatogramAlignmentRowData *operator->() const;

    MultipleChromatogramAlignmentRow clone() const;

private:
    QSharedPointer<MultipleChromatogramAlignmentRowData> getMcaRowData() const;
};

/**
 * A row in a multiple alignment structure.
 * The row consists of a sequence without gaps
 * and a gap model.
 * A row core is an obsolete concept. Currently,
 * it exactly equals to the row (offset always equals to zero).
 */
class U2CORE_EXPORT MultipleChromatogramAlignmentRowData : public MultipleAlignmentRowData {
    friend class MultipleChromatogramAlignmentData;
    friend class MultipleChromatogramAlignmentRow;

protected:
    /** Do NOT create a row without an alignment! */
    MultipleChromatogramAlignmentRowData(MultipleChromatogramAlignmentData *mcaData = NULL);

    /** Creates a row in memory. */
    MultipleChromatogramAlignmentRowData(const U2McaRow &rowInDb, const DNAChromatogram &chromatogram, const DNASequence &sequence, const QList<U2MsaGap> &gaps, MultipleChromatogramAlignmentData *mcaData);
    MultipleChromatogramAlignmentRowData(const U2McaRow &rowInDb, const QString &rowName, const DNAChromatogram &chromatogram, const QByteArray &rawData, MultipleChromatogramAlignmentData *mcaData);
    MultipleChromatogramAlignmentRowData(const MultipleChromatogramAlignmentRow &row, MultipleChromatogramAlignmentData *mcaData);

public:
    /** Name of the row (equals to the sequence name), can be empty */
    QString getName() const;
    void setName(const QString &name);

    /** Returns the list of gaps for the row */
    inline const U2MsaRowGapModel &getGapModel() const;

    /** Careful, the new gap model is not validated! */
    void setGapModel(const QList<U2MsaGap> &newGapModel);

    /** Returns the row sequence (without gaps) */
    inline const DNASequence &getSequence() const;

    const DNAChromatogram &getChromatogram() const;
    DNAChromatogram getGappedChromatogram() const;

    /** Returns ID of the row in the database. */
    qint64 getRowId() const;

    void setRowId(qint64 rowId);

    void setSequenceId(const U2DataId &sequenceId);

    /** Returns ID of the row sequence in the database. */
    U2McaRow getRowDbInfo() const;

    /** Sets database IDs for row and sequence */
    void setRowDbInfo(const U2McaRow &dbRow);

    /**
     * The length must be greater or equal to the row length.
     * When the specified length is greater, an appropriate number of
     * trailing gaps are appended to the end of the byte array.
     */
    QByteArray toByteArray(U2OpStatus &os, qint64 length) const;

    /** Returns length of the sequence + number of gaps including trailing gaps (if any) */
    int getRowLength() const;

    /** Returns length of the sequence + number of gaps. Doesn't include trailing gaps. */
    inline qint64 getRowLengthWithoutTrailing() const;

    /** Packed version: returns the row without leading and trailing gaps */
    QByteArray getCore() const;

    /** Returns the row the way it is -- with leading and trailing gaps */
    QByteArray getData() const;

    /** Obsolete. Always return the row length (non-inclusive!) */
    inline int getCoreEnd() const;

    /** Obsolete. Always returns zero. */
    int getCoreStart() const;

    /** Obsolete. The length of the row core */
    qint64 getCoreLength() const;

    /** Removes all gaps. Returns true if changed. */
    inline bool simplify();

    /** Adds anotherRow data to this row(ingores trailing gaps), "lengthBefore" must be greater than this row's length. */
    void append(const MultipleChromatogramAlignmentRow &anotherRow, int lengthBefore, U2OpStatus &os);
    void append(const MultipleChromatogramAlignmentRowData &anotherRow, int lengthBefore, U2OpStatus &os);

    /**
     * Sets new sequence and gap model.
     * If the sequence is empty, the offset is ignored (if any).
     */
    void setRowContent(const DNAChromatogram &chromatogram, const DNASequence &sequence, const U2MsaRowGapModel &gapModel, U2OpStatus &os);

    /**
     * Inserts 'count' gaps into the specified position, if possible.
     * If position is bigger than the row length or negative, does nothing.
     * Returns incorrect status if 'count' is negative.
     */
    void insertGaps(int pos, int count, U2OpStatus &os);

    /**
     * Removes up to 'count' characters starting from the specified position
     * If position is bigger than the row length, does nothing.
     * Returns incorrect status if 'pos' or 'count' is negative.
     */
    void removeChars(int pos, int count, U2OpStatus &os);

    /**
     * Returns a character in row at the specified position.
     * If the specified position is outside the row bounds, returns a gap.
     */
    char charAt(qint64 position) const;
    bool isGap(qint64 position) const;
    bool isLeadingOrTrailingGap(qint64 position) const;


    /** Length of the sequence without gaps */
    inline int getUngappedLength() const;

    /**
     * If character at 'pos' position is not a gap, returns the char position in sequence.
     * Otherwise returns '-1'.
     */
    int getUngappedPosition(int pos) const;

    /**
     * Returns base count located leftward to the 'before' position in the alignment.
     */
    qint64 getBaseCount(qint64 before) const;

    /**
     * Exactly compares the rows. Sequences and gap models must match.
     * However, the rows are considered equal if they differ by trailing gaps only.
     */
    bool isRowContentEqual(const MultipleChromatogramAlignmentRow &row) const;
    bool isRowContentEqual(const MultipleChromatogramAlignmentRowData &rowData) const;

    bool isDefault() const override;

    /** Compares 2 rows. Rows are equal if their contents and names are equal. */
    bool operator!=(const MultipleChromatogramAlignmentRowData &mcaRowData) const;
    bool operator!=(const MultipleAlignmentRowData &maRowData) const override;
    bool operator==(const MultipleChromatogramAlignmentRowData &mcaRowData) const;
    bool operator==(const MultipleAlignmentRowData &maRowData) const override;

    /**
     * Crops the row -> keeps only specified region in the row.
     * 'pos' and 'pos + count' can be greater than the row length.
     * Keeps trailing gaps.
     */
    void crop(U2OpStatus &os, qint64 startPosition, qint64 count);

    /**
     * Returns new row of the specified 'count' length, started from 'pos'.
     * 'pos' and 'pos + count' can be greater than the row length.
     * Keeps trailing gaps.
     */
    MultipleChromatogramAlignmentRow mid(int pos, int count, U2OpStatus &os) const;

    /** Converts the row sequence to upper case */
    void toUpperCase();

    /**
     * Replaces all occurrences of 'origChar' by 'resultChar'.
     * The 'origChar' must be a non-gap character.
     * The 'resultChar' can be a gap, gaps model is recalculated in this case.
     */
    void replaceChars(char origChar, char resultChar, U2OpStatus &os);

    MultipleChromatogramAlignmentRow getExplicitCopy() const;

    void setAdditionalInfo(const QVariantMap &additionalInfo);
    QVariantMap getAdditionalInfo() const;

    McaRowMemoryData getRowMemoryData() const;

    void reverse();
    void complement();
    void reverseComplement();

    bool isReversed() const;
    bool isComplemented() const;

private:
    /** Splits input to sequence bytes and gaps model */
    static void splitBytesToCharsAndGaps(const QByteArray &input, QByteArray &seqBytes, QList<U2MsaGap> &gapModel);

    /**
     * Add "offset" of gaps to the beginning of the row
     * Warning: it is not verified that the row sequence is not empty.
     */
    static void addOffsetToGapModel(QList<U2MsaGap> &gapModel, int offset);

    /**
     * Joins sequence chars and gaps into one byte array.
     * "keepOffset" specifies to take into account gaps at the beginning of the row.
     */
    QByteArray joinCharsAndGaps(bool keepOffset, bool keepTrailingGaps) const;

    /** Gets the length of all gaps */
    inline int getGapsLength() const;

    /** If there are consecutive gaps in the gaps model, merges them into one gap */
    void mergeConsecutiveGaps();

    /** The row must not contain trailing gaps, this method is used to assure it after the row modification */
    void removeTrailingGaps();

    void syncLengths();

    /**
     * Calculates start and end position in the sequence,
     * depending on the start position in the row and the 'count' character from it
     */
    void getStartAndEndSequencePositions(int pos, int count, int &startPosInSeq, int &endPosInSeq);

    /** Removing gaps from the row between position 'pos' and 'pos + count' */
    void removeGapsFromGapModel(U2OpStatus &os, int pos, int count);

    void setParentAlignment(const MultipleChromatogramAlignment &msa);
    void setParentAlignment(MultipleChromatogramAlignmentData *mcaData);

    MultipleChromatogramAlignmentData *alignment;

    DNAChromatogram chromatogram;

    /** The row in the database */
    U2McaRow initialRowInDb;

    QVariantMap additionalInfo;
};

inline const U2MsaRowGapModel &MultipleChromatogramAlignmentRowData::getGapModel() const {
    return gaps;
}

inline const DNASequence &MultipleChromatogramAlignmentRowData::getSequence() const {
    return sequence;
}

inline qint64 MultipleChromatogramAlignmentRowData::getRowLengthWithoutTrailing() const {
    return MsaRowUtils::getRowLength(sequence.seq, gaps);
}

inline int MultipleChromatogramAlignmentRowData::getCoreEnd() const {
    return getRowLengthWithoutTrailing();
}

inline bool MultipleChromatogramAlignmentRowData::simplify() {
    if (gaps.count() > 0) {
        gaps.clear();
        return true;
    }
    return false;
}

inline int MultipleChromatogramAlignmentRowData::getUngappedLength() const {
    return sequence.length();
}

inline int MultipleChromatogramAlignmentRowData::getGapsLength() const {
    return MsaRowUtils::getGapsLength(gaps);
}

inline bool operator==(const MultipleChromatogramAlignmentRow &ptr1, const MultipleChromatogramAlignmentRow &ptr2) {
    return *ptr1 == *ptr2;
}
inline bool operator==(const MultipleChromatogramAlignmentRow &ptr1, const MultipleChromatogramAlignmentRowData *ptr2) {
    return nullptr == ptr2 ? ptr1->isDefault() : (*ptr1 == *ptr2);
}
inline bool operator==(const MultipleChromatogramAlignmentRowData *ptr1, const MultipleChromatogramAlignmentRow &ptr2) {
    return nullptr == ptr1 ? ptr2->isDefault() : (*ptr1 == *ptr2);
}
inline bool operator!=(const MultipleChromatogramAlignmentRow &ptr1, const MultipleChromatogramAlignmentRow &ptr2) {
    return !(ptr1 == ptr2);
}
inline bool operator!=(const MultipleChromatogramAlignmentRow &ptr1, const MultipleChromatogramAlignmentRowData *ptr2) {
    return !(ptr1 == ptr2);
}
inline bool operator!=(const MultipleChromatogramAlignmentRowData *ptr1, const MultipleChromatogramAlignmentRow &ptr2) {
    return !(ptr1 == ptr2);
}

}    // namespace U2

#endif    // _U2_MULTIPLE_CHROMATOGRAM_ALIGNMENT_ROW_H_
