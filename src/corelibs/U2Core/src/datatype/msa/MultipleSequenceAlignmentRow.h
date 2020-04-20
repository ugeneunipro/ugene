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

#ifndef _U2_MULTIPLE_SEQUENCE_ALIGNMENT_ROW_H_
#define _U2_MULTIPLE_SEQUENCE_ALIGNMENT_ROW_H_

#include <U2Core/DNASequence.h>
#include <U2Core/MsaRowUtils.h>
#include <U2Core/U2Msa.h>

#include "MultipleAlignmentRow.h"

namespace U2 {

class DNASequence;
class MultipleSequenceAlignment;
class MultipleSequenceAlignmentData;
class MultipleSequenceAlignmentRowData;
class U2OpStatus;

class U2CORE_EXPORT MultipleSequenceAlignmentRow : public MultipleAlignmentRow {
public:
    MultipleSequenceAlignmentRow();
    MultipleSequenceAlignmentRow(const MultipleAlignmentRow &maRow);
    MultipleSequenceAlignmentRow(MultipleSequenceAlignmentData *msaData);
    MultipleSequenceAlignmentRow(MultipleSequenceAlignmentRowData *msaRowData);

    /** Creates a row in memory. */
    MultipleSequenceAlignmentRow(const U2MsaRow &rowInDb, const DNASequence &sequence, const U2MsaRowGapModel &gaps, MultipleSequenceAlignmentData *msaData);
    MultipleSequenceAlignmentRow(const U2MsaRow &rowInDb, const QString &rowName, const QByteArray &rawData, MultipleSequenceAlignmentData *msaData);
    MultipleSequenceAlignmentRow(const MultipleSequenceAlignmentRow &row, MultipleSequenceAlignmentData *msaData);

    MultipleSequenceAlignmentRowData * data() const;

    MultipleSequenceAlignmentRowData & operator*();
    const MultipleSequenceAlignmentRowData & operator*() const;

    MultipleSequenceAlignmentRowData * operator->();
    const MultipleSequenceAlignmentRowData * operator->() const;

    MultipleSequenceAlignmentRow clone() const;

private:
    QSharedPointer<MultipleSequenceAlignmentRowData> getMsaRowData() const;
};

/**
 * A row in a multiple alignment structure.
 * The row consists of a sequence without gaps
 * and a gap model.
 * A row core is an obsolete concept. Currently,
 * it exactly equals to the row (offset always equals to zero).
 */
class U2CORE_EXPORT MultipleSequenceAlignmentRowData : public MultipleAlignmentRowData {
    friend class MultipleSequenceAlignmentData;
    friend class MultipleSequenceAlignmentRow;

protected:
    /** Do NOT create a row without an alignment! */
    MultipleSequenceAlignmentRowData(MultipleSequenceAlignmentData *msaData = NULL);

    /** Creates a row in memory. */
    MultipleSequenceAlignmentRowData(const U2MsaRow &rowInDb, const DNASequence &sequence, const QList<U2MsaGap> &gaps, MultipleSequenceAlignmentData *msaData);
    MultipleSequenceAlignmentRowData(const U2MsaRow &rowInDb, const QString &rowName, const QByteArray &rawData, MultipleSequenceAlignmentData *msaData);
    MultipleSequenceAlignmentRowData(const MultipleSequenceAlignmentRow &row, MultipleSequenceAlignmentData *msaData);

public:
    virtual ~MultipleSequenceAlignmentRowData();

    /** Name of the row (equals to the sequence name), can be empty */
    QString getName() const;
    void setName(const QString& name);

    /** Returns the list of gaps for the row */
    inline const U2MsaRowGapModel & getGapModel() const;

    /** Careful, the new gap model is not validated! */
    void setGapModel(const QList<U2MsaGap> &newGapModel);

    /** Returns the row sequence (without gaps) */
    inline const DNASequence & getSequence() const;

    /** Returns ID of the row in the database. */
    qint64 getRowId() const;

    void setRowId(qint64 rowId);

    void setSequenceId(const U2DataId &sequenceId);

    /** Returns ID of the row sequence in the database. */
    U2MsaRow getRowDbInfo() const;

    /** Sets database IDs for row and sequence */
    void setRowDbInfo(const U2MsaRow &dbRow);

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
    virtual inline bool simplify();

    /** Adds anotherRow data to this row(ingores trailing gaps), "lengthBefore" must be greater than this row's length. */
    virtual void append(const MultipleSequenceAlignmentRow &anotherRow, int lengthBefore, U2OpStatus &os);
    virtual void append(const MultipleSequenceAlignmentRowData &anotherRow, int lengthBefore, U2OpStatus &os);

    /**
     * Sets new sequence and gap model.
     * If the sequence is empty, the offset is ignored (if any).
     */
    void setRowContent(const DNASequence &sequence, const U2MsaRowGapModel &gapModel, U2OpStatus &os);
    void setRowContent(const QByteArray &bytes, int offset, U2OpStatus &os);

    /**
     * Inserts 'count' gaps into the specified position, if possible.
     * If position is bigger than the row length or negative, does nothing.
     * Returns incorrect status if 'count' is negative.
     */
    virtual void insertGaps(int pos, int count, U2OpStatus &os);

    /**
     * Removes up to 'count' characters starting from the specified position
     * If position is bigger than the row length, does nothing.
     * Returns incorrect status if 'pos' or 'count' is negative.
     */
    virtual void removeChars(int pos, int count, U2OpStatus& os);

    /**
     * Returns a character in row at the specified position.
     * If the specified position is outside the row bounds, returns a gap.
     */
    char charAt(qint64 position) const;
    bool isGap(qint64 pos) const;

    /**
     * Returns base count located leftward to the 'before' position in the alignment.
     */
    qint64 getBaseCount(qint64 before) const;

    /**
     * Exactly compares the rows. Sequences and gap models must match.
     * However, the rows are considered equal if they differ by trailing gaps only.
     */
    virtual bool isRowContentEqual(const MultipleSequenceAlignmentRow &row) const;
    virtual bool isRowContentEqual(const MultipleSequenceAlignmentRowData &rowData) const;

    bool isDefault() const override;

    /** Compares 2 rows. Rows are equal if their contents and names are equal. */
    bool operator!=(const MultipleSequenceAlignmentRowData &msaRowData) const;
    bool operator!=(const MultipleAlignmentRowData &maRowData) const;
    bool operator==(const MultipleSequenceAlignmentRowData &msaRowData) const;
    bool operator==(const MultipleAlignmentRowData &maRowData) const;

    /**
     * Crops the row -> keeps only specified region in the row.
     * 'pos' and 'pos + count' can be greater than the row length.
     * Keeps trailing gaps.
     */
    virtual void crop(U2OpStatus &os, qint64 startPosition, qint64 count);

    /**
     * Returns new row of the specified 'count' length, started from 'pos'.
     * 'pos' and 'pos + count' can be greater than the row length.
     * Keeps trailing gaps.
     */
    MultipleSequenceAlignmentRow mid(int pos, int count, U2OpStatus &os) const;

    /** Converts the row sequence to upper case */
    virtual void toUpperCase();

    /**
     * Replaces all occurrences of 'origChar' by 'resultChar'.
     * The 'origChar' must be a non-gap character.
     * The 'resultChar' can be a gap, gaps model is recalculated in this case.
     */
    virtual void replaceChars(char origChar, char resultChar, U2OpStatus &os);

    MultipleSequenceAlignmentRow getExplicitCopy() const;

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

    /**
     * Calculates start and end position in the sequence,
     * depending on the start position in the row and the 'count' character from it
     */
    void getStartAndEndSequencePositions(int pos, int count, int &startPosInSeq, int &endPosInSeq);

    /** Removing gaps from the row between position 'pos' and 'pos + count' */
    void removeGapsFromGapModel(U2OpStatus &os, int pos, int count);

    void setParentAlignment(const MultipleSequenceAlignment &msa);
    void setParentAlignment(MultipleSequenceAlignmentData *msaData);

    MultipleSequenceAlignmentData *alignment;

    /** The row in the database */
    U2MsaRow initialRowInDb;
};

inline const U2MsaRowGapModel & MultipleSequenceAlignmentRowData::getGapModel() const {
    return gaps;
}

inline const DNASequence & MultipleSequenceAlignmentRowData::getSequence() const {
    return sequence;
}

inline qint64 MultipleSequenceAlignmentRowData::getRowLengthWithoutTrailing() const {
    return MsaRowUtils::getRowLength(sequence.seq, gaps);
}

inline int MultipleSequenceAlignmentRowData::getCoreEnd() const {
    return getRowLengthWithoutTrailing();
}

inline bool MultipleSequenceAlignmentRowData::simplify() {
    if (gaps.count() > 0) {
        gaps.clear();
        return true;
    }
    return false;
}

inline int MultipleSequenceAlignmentRowData::getGapsLength() const {
    return MsaRowUtils::getGapsLength(gaps);
}

inline bool	operator==(const MultipleSequenceAlignmentRow &ptr1, const MultipleSequenceAlignmentRow &ptr2) {
    return *ptr1 == *ptr2;
}
inline bool	operator==(const MultipleSequenceAlignmentRow &ptr1, const MultipleSequenceAlignmentRowData *ptr2) {
    return nullptr == ptr2 ? ptr1->isDefault() : (*ptr1 == *ptr2);
}
inline bool	operator==(const MultipleSequenceAlignmentRowData *ptr1, const MultipleSequenceAlignmentRow &ptr2) {
    return nullptr == ptr1 ? ptr2->isDefault() : (*ptr1 == *ptr2);
}
inline bool operator!=(const MultipleSequenceAlignmentRow &ptr1, const MultipleSequenceAlignmentRow &ptr2) {
    return !(ptr1 == ptr2);
}
inline bool operator!=(const MultipleSequenceAlignmentRow &ptr1, const MultipleSequenceAlignmentRowData *ptr2) {
    return !(ptr1 == ptr2);
}
inline bool operator!=(const MultipleSequenceAlignmentRowData *ptr1, const MultipleSequenceAlignmentRow &ptr2) {
    return !(ptr1 == ptr2);
}


}   // namespace U2

#endif // _U2_MULTIPLE_SEQUENCE_ALIGNMENT_ROW_H_
