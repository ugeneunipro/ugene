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
    MultipleChromatogramAlignmentRow(const MultipleAlignmentRow& maRow);
    MultipleChromatogramAlignmentRow(MultipleChromatogramAlignmentData* mcaData);
    MultipleChromatogramAlignmentRow(MultipleChromatogramAlignmentRowData* mcaRowData);

    /** Creates a row in memory. */
    MultipleChromatogramAlignmentRow(const U2MsaRow& rowInDb, const DNAChromatogram& chromatogram, const DNASequence& sequence, const QVector<U2MsaGap>& gaps, MultipleChromatogramAlignmentData* mcaData);
    MultipleChromatogramAlignmentRow(const U2MsaRow& rowInDb, const QString& rowName, const DNAChromatogram& chromatogram, const QByteArray& rawData, MultipleChromatogramAlignmentData* mcaData);
    MultipleChromatogramAlignmentRow(const MultipleChromatogramAlignmentRow& row, MultipleChromatogramAlignmentData* mcaData);

    MultipleChromatogramAlignmentRowData* data() const;

    MultipleChromatogramAlignmentRowData& operator*();
    const MultipleChromatogramAlignmentRowData& operator*() const;

    MultipleChromatogramAlignmentRowData* operator->();
    const MultipleChromatogramAlignmentRowData* operator->() const;

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
    MultipleChromatogramAlignmentRowData(MultipleChromatogramAlignmentData* mcaData = nullptr);

    /** Creates a row in memory. */
    MultipleChromatogramAlignmentRowData(const U2MsaRow& rowInDb, const DNAChromatogram& chromatogram, const DNASequence& sequence, const QVector<U2MsaGap>& gaps, MultipleChromatogramAlignmentData* mcaData);
    MultipleChromatogramAlignmentRowData(const U2MsaRow& rowInDb, const QString& rowName, const DNAChromatogram& chromatogram, const QByteArray& rawData, MultipleChromatogramAlignmentData* mcaData);
    MultipleChromatogramAlignmentRowData(const MultipleChromatogramAlignmentRow& row, MultipleChromatogramAlignmentData* mcaData);

public:
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

    /** Returns pair of the first and the second (by peak height) chromatogram trace characted in the @pos position */
    QPair<DNAChromatogram::ChromatogramTraceAndValue, DNAChromatogram::ChromatogramTraceAndValue> getTwoHighestPeaks(qint64 position, bool& hasTwoPeaks) const;

    /** Adds sequence chromatogram equality check to the base MultipleAlignmentRowData::isEqualCore method. */
    bool isEqualCore(const MultipleAlignmentRowData& other) const override;

    bool isDefault() const override;

    /** Checks that 'other' is MultipleChromatogramAlignmentRowData and calls the MCA version of the method. */
    bool isEqual(const MultipleAlignmentRowData& other) const override;

    /** Compares 2 rows. Rows are equal if their names, sequences, no-leading-gap models and chromatograms are equal. */
    bool isEqual(const MultipleChromatogramAlignmentRowData& other) const;

    /**
     * Crops the row -> keeps only specified region in the row.
     * 'pos' and 'pos + count' can be greater than the row length.
     * Keeps trailing gaps.
     */
    void crop(U2OpStatus& os, qint64 startPosition, qint64 count) override;

    /**
     * Returns new row of the specified 'count' length, started from 'pos'.
     * 'pos' and 'pos + count' can be greater than the row length.
     * Keeps trailing gaps.
     */
    MultipleChromatogramAlignmentRow mid(int pos, int count, U2OpStatus& os) const;

    /** Converts the row sequence to upper case */
    void toUpperCase();

    /**
     * Replaces all occurrences of 'origChar' by 'resultChar'.
     * The 'origChar' must be a non-gap character.
     * The 'resultChar' can be a gap, gaps model is recalculated in this case.
     */
    void replaceChars(char origChar, char resultChar, U2OpStatus& os);

    MultipleChromatogramAlignmentRow getExplicitCopy() const;

    MultipleAlignmentData* getMultipleAlignmentData() const override;

    void setAdditionalInfo(const QVariantMap& additionalInfo);
    QVariantMap getAdditionalInfo() const;

    McaRowMemoryData getRowMemoryData() const;

    void reverse();
    void complement();
    void reverseComplement();

    bool isReversed() const;
    bool isComplemented() const override;

private:
    /** Gets the length of all gaps */
    inline int getGapsLength() const;

    /**
     * Calculates start and end position in the sequence,
     * depending on the start position in the row and the 'count' character from it
     */
    void getStartAndEndSequencePositions(int pos, int count, int& startPosInSeq, int& endPosInSeq);

    /** Removing gaps from the row between position 'pos' and 'pos + count' */
    void removeGapsFromGapModel(U2OpStatus& os, int pos, int count);

    void setParentAlignment(const MultipleChromatogramAlignment& msa);
    void setParentAlignment(MultipleChromatogramAlignmentData* mcaData);

    MultipleChromatogramAlignmentData* alignment;

    QVariantMap additionalInfo;
};

inline int MultipleChromatogramAlignmentRowData::getUngappedLength() const {
    return sequence.length();
}

inline int MultipleChromatogramAlignmentRowData::getGapsLength() const {
    return MsaRowUtils::getGapsLength(gaps);
}

inline bool operator==(const MultipleChromatogramAlignmentRow& ptr1, const MultipleChromatogramAlignmentRow& ptr2) {
    return *ptr1 == *ptr2;
}
inline bool operator==(const MultipleChromatogramAlignmentRow& ptr1, const MultipleChromatogramAlignmentRowData* ptr2) {
    return ptr2 == nullptr ? ptr1->isDefault() : (*ptr1 == *ptr2);
}
inline bool operator==(const MultipleChromatogramAlignmentRowData* ptr1, const MultipleChromatogramAlignmentRow& ptr2) {
    return ptr1 == nullptr ? ptr2->isDefault() : (*ptr1 == *ptr2);
}
inline bool operator!=(const MultipleChromatogramAlignmentRow& ptr1, const MultipleChromatogramAlignmentRow& ptr2) {
    return !(ptr1 == ptr2);
}
inline bool operator!=(const MultipleChromatogramAlignmentRow& ptr1, const MultipleChromatogramAlignmentRowData* ptr2) {
    return !(ptr1 == ptr2);
}
inline bool operator!=(const MultipleChromatogramAlignmentRowData* ptr1, const MultipleChromatogramAlignmentRow& ptr2) {
    return !(ptr1 == ptr2);
}

}  // namespace U2
