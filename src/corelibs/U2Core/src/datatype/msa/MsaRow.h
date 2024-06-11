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

#include <QSharedPointer>

#include <U2Core/Chromatogram.h>
#include <U2Core/DNASequence.h>
#include <U2Core/MsaRowUtils.h>
#include <U2Core/U2Msa.h>
#include <U2Core/U2OpStatus.h>

namespace U2 {

class Msa;
class MsaData;
class MsaRowData;

class U2CORE_EXPORT MsaRow {
    friend class Msa;
    friend class MsaData;
    friend class MsaRowData;

public:
    MsaRow(MsaRowData* maRowData);
    MsaRow(MsaData* maData = nullptr);

    MsaRow(const U2MsaRow& rowInDb,
           const DNASequence& sequence,
           const QVector<U2MsaGap>& gaps,
           const U2DataId& chromatogramId,
           const Chromatogram& chromatogram,
           MsaData* maData);

    MsaRow(const U2MsaRow& rowInDb,
           const DNASequence& sequence,
           const QVector<U2MsaGap>& gaps,
           MsaData* maData);

    MsaRow(const U2MsaRow& rowInDb,
           const QString& rowName,
           const QByteArray& rawSequenceData,
           MsaData* maData);

    MsaRow(const MsaRow& row, MsaData* mcaData);

    virtual ~MsaRow() = default;

    MsaRow clone() const;

    MsaRowData* data() const;

    MsaRowData& operator*();
    const MsaRowData& operator*() const;

    MsaRowData* operator->();
    const MsaRowData* operator->() const;

protected:
    QSharedPointer<MsaRowData> maRowData;
};

/**
 * A row in a multiple alignment structure.
 * The row consists of a sequence without gaps
 * and a gap model.
 * A row core is an obsolete concept. Currently,
 * it exactly equals to the row (offset always equals to zero).
 */
class U2CORE_EXPORT MsaRowData {
    friend class Msa;
    friend class MsaData;
    friend class MsaRow;

public:
    MsaRowData(MsaData* maData = nullptr);

    MsaRowData(const U2MsaRow& rowInDb,
               const DNASequence& sequence,
               const QVector<U2MsaGap>& gaps,
               const U2DataId& chromatogramId,
               const Chromatogram& chromatogram,
               MsaData* mcaData);

    MsaRowData(const U2MsaRow& rowInDb,
               const QString& rowName,
               const QByteArray& rawSequenceData,
               MsaData* maData);

    MsaRowData(const MsaRow& row,
               MsaData* maData);

    /** Length of the sequence without gaps */
    int getUngappedLength() const;

    /**
     * If character at 'pos' position is not a gap, returns the char position in sequence.
     * Otherwise returns '-1'.
     */
    int getUngappedPosition(int pos) const;
    U2Region getGapped(const U2Region& region);

    bool isTrailingOrLeadingGap(qint64 position) const;

    U2Region getCoreRegion() const;
    U2Region getUngappedRegion(const U2Region& gappedRegion) const;
    DNASequence getUngappedSequence() const;

    /** Returns the list of gaps for the row */
    const QVector<U2MsaGap>& getGaps() const;

    /**
     * Inserts 'count' gaps into the specified position, if possible.
     * If position is bigger than the row length or negative, does nothing.
     * Returns incorrect status if 'count' is negative.
     */
    void insertGaps(int pos, int count, U2OpStatus& os);

    /**
     * Removes up to 'count' characters starting from the specified position
     * If position is bigger than the row length, does nothing.
     * Returns incorrect status if 'pos' or 'count' is negative.
     */
    void removeChars(int pos, int count, U2OpStatus& os);

    /** Sets a new gap model. Warning: does not validate the new gap model. */
    void setGapModel(const QVector<U2MsaGap>& newGapModel);

    /** The row must not contain trailing gaps, this method is used to assure it after the row modification.  */
    void removeTrailingGaps();

    QString getName() const;

    void setName(const QString& name);

    /** Returns ID of the row in the database. */
    qint64 getRowId() const;

    void setRowId(qint64 rowId);

    void setSequenceId(const U2DataId& sequenceId);

    char charAt(qint64 position) const;

    bool isGap(qint64 position) const;

    bool isLeadingOrTrailingGap(qint64 position) const;

    /**
     * Returns base count located leftward to the 'before' position in the alignment.
     */
    qint64 getBaseCount(qint64 before) const;

    /**
     * The length must be greater or equal to the row length.
     * When the specified length is greater, an appropriate number of trailing gaps are appended to the end of the byte array.
     */
    QByteArray toByteArray(U2OpStatus& os, int length) const;

    /** Returns length of the sequence + number of gaps including trailing gaps (if any) */
    int getRowLength() const;

    /** Returns length of the sequence + number of gaps. Doesn't include trailing gaps. */
    int getRowLengthWithoutTrailing() const;

    /** Obsolete. Always return the row length (non-inclusive!) */
    int getCoreEnd() const;

    /** Obsolete. Always returns zero. */
    int getCoreStart() const;

    /** Obsolete. The length of the row core */
    qint64 getCoreLength() const;

    /** Packed version: returns the row without leading and trailing gaps */
    QByteArray getCore() const;

    /** Returns the row the way it is -- with leading and trailing gaps */
    QByteArray getData() const;

    /** Removes all gaps. Returns true if changed. */
    bool simplify();

    /**
     * Crops the row -> keeps only specified region in the row.
     * 'pos' and 'pos + count' can be greater than the row length.
     * Keeps trailing gaps.
     */
    void crop(U2OpStatus& os, int startPosition, int count);

    /** Adds anotherRow data to this row(ignores trailing gaps), "lengthBefore" must be greater than this row's length. */
    void append(const MsaRow& anotherRow, int lengthBefore, U2OpStatus& os);

    void append(const MsaRowData& anotherRow, int lengthBefore, U2OpStatus& os);

    bool isDefault() const;

    /** Returns database info the the row.*/
    U2MsaRow getRowDbInfo() const;

    /** Returns ID of the sequence object used by the row. */
    const U2DataId& getSequenceId() const;

    /** Returns ID of the chromatogram for the sequence. May be empty of no chromatogram is assigned. */
    const U2DataId& getChromatogramId() const;

    /** Sets database IDs for row and sequence */
    void setRowDbInfo(const U2MsaRow& dbRow);

    /**
     * Checks that the row is equal to 'other' rows.
     * For the equality method details see comments for the implementation.
     */
    bool isEqual(const MsaRowData& other) const;

    /** Calls isEqual() method. */
    bool operator==(const MsaRowData& other) const;

    /** Returns !isEqual() method result. */
    bool operator!=(const MsaRowData& other) const;

    /* Compares sequences of 2 rows ignoring gaps. */
    static bool isEqualIgnoreGaps(const MsaRowData* row1, const MsaRowData* row2);

    /** Compares whole gapped sequences (with inner gaps) but with leading gaps removed. */
    bool isEqualCore(const MsaRowData& other) const;

    /** Joins sequence chars and gaps into one byte array. */
    QByteArray getSequenceWithGaps(bool keepLeadingGaps, bool keepTrailingGaps) const;

    const Chromatogram& getChromatogram() const;

    Chromatogram getGappedChromatogram() const;

    /** Returns the row sequence (without gaps). */
    const DNASequence& getSequence() const;

    /** Returns the position of @pos, including gaps */
    qint64 getGappedPosition(int pos) const;

    /**
     * Sets new sequence and gap model.
     * If the sequence is empty, the offset is ignored (if any).
     */
    void setRowContent(const Chromatogram& chromatogram, const DNASequence& sequence, const QVector<U2MsaGap>& gapModel, U2OpStatus& os);

    /**
     * Sets new sequence and gap model.
     * If the sequence is empty, the offset is ignored (if any).
     */
    void setRowContent(const DNASequence& sequence, const QVector<U2MsaGap>& gapModel, U2OpStatus& os);

    void setRowContent(const QByteArray& bytes, int offset, U2OpStatus& os);

    /** Converts the row sequence to upper case */
    void toUpperCase();

    /**
     * Replaces all occurrences of 'origChar' by 'resultChar'.
     * The 'origChar' must be a non-gap character.
     * The 'resultChar' can be a gap, gaps model is recalculated in this case.
     */
    void replaceChars(char origChar, char resultChar, U2OpStatus& os);

    void setAdditionalInfo(const QVariantMap& additionalInfo);

    const QVariantMap& getAdditionalInfo() const;

    void reverse();
    void complement();
    void reverseComplement();

    bool isReversed() const;
    /**

     * Returns true if the row sequence should be read in the reversed direction
     * Such rows only exist for MCA alignments with a reverse-complementary read direction.
     */
    bool isComplemented() const;

    /** Returns pair of the first and the second peaks, wich chromatogram traces have the highest values of the current @position.
      * The word "peak" mean, that this trace should have the highest value in the middle of the base and lowest values on its edges.
      * If the central point does not contain smaller values to the left and right of itself, then it is not a peak and, regardless
      * of its height, does not participate in the calculation. Both peaks must have the maximum trail value for
      * a given position - otherwise, hasTwoPeaks will be set to false.
      * If there are no two higest peaks, @hasTwoPeaks is set to false.  */
    QPair<ChromatogramData::TraceAndValue, ChromatogramData::TraceAndValue> getTwoHighestPeaks(int position, bool& hasTwoPeaks) const;

    /**
     * Returns new row of the specified 'count' length, started from 'pos'.
     * 'pos' and 'pos + count' can be greater than the row length.
     * Keeps trailing gaps.
     */
    MsaRow mid(int pos, int count, U2OpStatus& os) const;

    MsaRow getExplicitCopy() const;

protected:
    void setParentAlignment(MsaData* alignment);

    /** Invalidates gapped sequence cache. */
    void invalidateGappedCache() const;

    /** If there are consecutive gaps in the gaps model, merges them into one gap */
    void mergeConsecutiveGaps();

    /**
     * Add "offset" of gaps to the beginning of the row
     * Warning: it is not verified that the row sequence is not empty.
     */
    static void addOffsetToGapModel(QVector<U2MsaGap>& gapModel, int offset);

    /**
     * Calculates start and end position in the sequence,
     * depending on the start position in the row and the 'count' character from it
     */
    void getStartAndEndSequencePositions(int pos, int count, int& startPosInSeq, int& endPosInSeq) const;

    /** Removing gaps from the row between position 'pos' and 'pos + count' */
    void removeGapsFromGapModel(U2OpStatus& os, int pos, int count);

    /** Gets char from the gapped sequence cache. Updates the cache if needed. */
    char getCharFromCache(int gappedPosition) const;

protected:
    /** The sequence of the row without gaps (cached) */
    DNASequence sequence;

    /**
     * Gaps model of the row
     * There should be no trailing gaps!
     * Trailing gaps are 'Virtual': they are stored 'inside' the alignment length
     */
    QVector<U2MsaGap> gaps;

    /** Id of the chromatogram associated with the sequence. Can be empty. */
    U2DataId chromatogramId;

    /** Optional chromatogram. Defined only for MCA objects today. */
    Chromatogram chromatogram;

    /** The row in the database */
    U2MsaRow initialRowInDb;

    /** Gapped cache offset in the row position.*/
    mutable int gappedCacheOffset = 0;

    /**
     * Cached segment of the gapped sequence.
     * The reason why this cache is efficient:
     *  Most of the algorithms access the row data sequentially: charAt(i), charAt(i+1), charAt(i+2).
     *  This access may be very slow for rows with a large gap models: to compute every character the gap model must be re-applied form the very first gap.
     *  This cache helps to avoid this gaps re-computation on sequential reads.
     */
    mutable QByteArray gappedSequenceCache;

    // TODO: this field is not used in comparison algorithm. Check its usage and fix or remove the field completely.1
    QVariantMap additionalInfo;

    MsaData* alignment = nullptr;
};

inline bool operator==(const MsaRow& ptr1, const MsaRow& ptr2) {
    return *ptr1 == *ptr2;
}
inline bool operator==(const MsaRow& ptr1, const MsaRowData* ptr2) {
    return ptr2 == nullptr ? ptr1->isDefault() : (*ptr1 == *ptr2);
}
inline bool operator==(const MsaRowData* ptr1, const MsaRow& ptr2) {
    return ptr1 == nullptr ? ptr2->isDefault() : (*ptr1 == *ptr2);
}
inline bool operator!=(const MsaRow& ptr1, const MsaRow& ptr2) {
    return !(ptr1 == ptr2);
}
inline bool operator!=(const MsaRow& ptr1, const MsaRowData* ptr2) {
    return !(ptr1 == ptr2);
}
inline bool operator!=(const MsaRowData* ptr1, const MsaRow& ptr2) {
    return !(ptr1 == ptr2);
}

}  // namespace U2
