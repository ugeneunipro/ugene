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

#ifndef _U2_MULTIPLE_ALIGNMENT_ROW_H_
#define _U2_MULTIPLE_ALIGNMENT_ROW_H_

#include <QSharedPointer>

#include <U2Core/DNASequence.h>
#include <U2Core/MsaRowUtils.h>
#include <U2Core/U2Msa.h>
#include <U2Core/U2OpStatus.h>

namespace U2 {

class MultipleAlignment;
class MultipleAlignmentData;
class MultipleAlignmentRowData;
class U2OpStatus;

enum class U2CORE_EXPORT MultipleAlignmentDataType {
    MSA,
    MCA,
};

class U2CORE_EXPORT MultipleAlignmentRow {
    friend class MultipleAlignmentData;

protected:
    MultipleAlignmentRow(MultipleAlignmentRowData* maRowData);

public:
    virtual ~MultipleAlignmentRow() = default;

    MultipleAlignmentRowData* data() const;
    template<class Derived>
    inline Derived dynamicCast() const;
    template<class Derived>
    inline Derived dynamicCast(U2OpStatus& os) const;

    MultipleAlignmentRowData& operator*();
    const MultipleAlignmentRowData& operator*() const;

    MultipleAlignmentRowData* operator->();
    const MultipleAlignmentRowData* operator->() const;

protected:
    QSharedPointer<MultipleAlignmentRowData> maRowData;
};

template<class Derived>
Derived MultipleAlignmentRow::dynamicCast() const {
    return Derived(*this);
}

template<class Derived>
Derived MultipleAlignmentRow::dynamicCast(U2OpStatus& os) const {
    Derived derived(*this);
    if (nullptr == derived.maRowData) {
        assert(false);
        os.setError("Can't cast MultipleAlignmentRow to a derived class");
    }
    return derived;
}

/**
 * A row in a multiple alignment structure.
 * The row consists of a sequence without gaps
 * and a gap model.
 * A row core is an obsolete concept. Currently,
 * it exactly equals to the row (offset always equals to zero).
 */
class U2CORE_EXPORT MultipleAlignmentRowData {
public:
    MultipleAlignmentRowData(const MultipleAlignmentDataType& type);
    MultipleAlignmentRowData(const MultipleAlignmentDataType& type, const DNASequence& sequence, const QVector<U2MsaGap>& gaps);
    virtual ~MultipleAlignmentRowData() = default;

    /** Length of the sequence without gaps */
    inline int getUngappedLength() const;

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
    virtual const QVector<U2MsaGap>& getGaps() const = 0;
    virtual void removeChars(int pos, int count, U2OpStatus& os) = 0;

    /** Name of the row, can be empty */
    virtual QString getName() const = 0;
    virtual void setName(const QString& name) = 0;

    /** Returns ID of the row in the database. */
    virtual qint64 getRowId() const = 0;
    virtual void setRowId(qint64 rowId) = 0;

    virtual char charAt(qint64 position) const = 0;
    virtual bool isGap(qint64 position) const = 0;
    virtual bool isLeadingOrTrailingGap(qint64 position) const = 0;

    virtual QByteArray toByteArray(U2OpStatus& os, qint64 length) const = 0;

    virtual qint64 getRowLengthWithoutTrailing() const = 0;
    virtual int getCoreStart() const = 0;
    virtual int getCoreEnd() const = 0;
    virtual qint64 getCoreLength() const = 0;
    virtual qint64 getBaseCount(qint64 beforePosition) const = 0;

    virtual void crop(U2OpStatus& os, qint64 startPosition, qint64 count) = 0;

    virtual bool isDefault() const = 0;

    /**
     * Checks that the row is equal to 'other' rows.
     * Only rows of the same 'MultipleAlignmentDataType' can be equal.
     * For the equality method details see comments for the implementation.
     */
    virtual bool isEqual(const MultipleAlignmentRowData& other) const = 0;

    /** Calls isEqual() method. */
    bool operator==(const MultipleAlignmentRowData& other) const;

    /** Returns !isEqual() method result. */
    bool operator!=(const MultipleAlignmentRowData& other) const;

    /* Compares sequences of 2 rows ignoring gaps. */
    static bool isEqualIgnoreGaps(const MultipleAlignmentRowData* row1, const MultipleAlignmentRowData* row2);

    /**
     * Compares whole gapped sequences (with inner gaps) but with leading gaps removed.
     * This method method may be overriden to compare other sequence related properties (like chromatogram in MCA).
     */
    virtual bool isEqualCore(const MultipleAlignmentRowData& other) const;

    /** Joins sequence chars and gaps into one byte array. */
    QByteArray getSequenceWithGaps(bool keepLeadingGaps, bool keepTrailingGaps) const;

    /** Returns whole alignment data. */
    virtual MultipleAlignmentData* getMultipleAlignmentData() const = 0;

    /**
     * Returns true if the row sequence should be read in the reversed direction
     * Such rows only exist for MCA alignments with a reverse-complementary read direction.
     */
    virtual bool isComplemented() const;

public:
    const MultipleAlignmentDataType type;

protected:
    /** The sequence of the row without gaps (cached) */
    DNASequence sequence;

    /**
     * Gaps model of the row
     * There should be no trailing gaps!
     * Trailing gaps are 'Virtual': they are stored 'inside' the alignment length
     */
    QVector<U2MsaGap> gaps;
};

inline int MultipleAlignmentRowData::getUngappedLength() const {
    return sequence.length();
}

inline bool operator==(const MultipleAlignmentRow& ptr1, const MultipleAlignmentRow& ptr2) {
    return *ptr1 == *ptr2;
}
inline bool operator==(const MultipleAlignmentRow& ptr1, const MultipleAlignmentRowData* ptr2) {
    return nullptr == ptr2 ? ptr1->isDefault() : (*ptr1 == *ptr2);
}
inline bool operator==(const MultipleAlignmentRowData* ptr1, const MultipleAlignmentRow& ptr2) {
    return nullptr == ptr1 ? ptr2->isDefault() : (*ptr1 == *ptr2);
}
inline bool operator!=(const MultipleAlignmentRow& ptr1, const MultipleAlignmentRow& ptr2) {
    return !(ptr1 == ptr2);
}
inline bool operator!=(const MultipleAlignmentRow& ptr1, const MultipleAlignmentRowData* ptr2) {
    return !(ptr1 == ptr2);
}
inline bool operator!=(const MultipleAlignmentRowData* ptr1, const MultipleAlignmentRow& ptr2) {
    return !(ptr1 == ptr2);
}

}  // namespace U2

#endif  // _U2_MULTIPLE_ALIGNMENT_ROW_H_
