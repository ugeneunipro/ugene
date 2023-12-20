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
    MultipleSequenceAlignmentRow(const MultipleAlignmentRow& maRow);
    MultipleSequenceAlignmentRow(MultipleSequenceAlignmentData* msaData);
    MultipleSequenceAlignmentRow(MultipleSequenceAlignmentRowData* msaRowData);

    /** Creates a row in memory. */
    MultipleSequenceAlignmentRow(const U2MsaRow& rowInDb, const DNASequence& sequence, const QVector<U2MsaGap>& gaps, MultipleSequenceAlignmentData* msaData);
    MultipleSequenceAlignmentRow(const U2MsaRow& rowInDb, const QString& rowName, const QByteArray& rawData, MultipleSequenceAlignmentData* msaData);
    MultipleSequenceAlignmentRow(const MultipleSequenceAlignmentRow& row, MultipleSequenceAlignmentData* msaData);

    MultipleSequenceAlignmentRowData* data() const;

    MultipleSequenceAlignmentRowData& operator*();
    const MultipleSequenceAlignmentRowData& operator*() const;

    MultipleSequenceAlignmentRowData* operator->();
    const MultipleSequenceAlignmentRowData* operator->() const;

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
    MultipleSequenceAlignmentRowData(MultipleSequenceAlignmentData* msaData = nullptr);

    /** Creates a row in memory. */
    MultipleSequenceAlignmentRowData(const U2MsaRow& rowInDb, const DNASequence& sequence, const QVector<U2MsaGap>& gaps, MultipleSequenceAlignmentData* msaData);
    MultipleSequenceAlignmentRowData(const U2MsaRow& rowInDb, const QString& rowName, const QByteArray& rawData, MultipleSequenceAlignmentData* msaData);
    MultipleSequenceAlignmentRowData(const MultipleSequenceAlignmentRow& row, MultipleSequenceAlignmentData* msaData);

public:

    /**
     * Returns new row of the specified 'count' length, started from 'pos'.
     * 'pos' and 'pos + count' can be greater than the row length.
     * Keeps trailing gaps.
     */
    MultipleSequenceAlignmentRow mid(int pos, int count, U2OpStatus& os) const;

    /** Converts the row sequence to upper case */
    void toUpperCase();

    /**
     * Replaces all occurrences of 'origChar' by 'resultChar'.
     * The 'origChar' must be a non-gap character.
     * The 'resultChar' can be a gap, gaps model is recalculated in this case.
     */
    void replaceChars(char origChar, char resultChar, U2OpStatus& os);

    MultipleSequenceAlignmentRow getExplicitCopy() const;

    MultipleAlignmentData* getMultipleAlignmentData() const override;

private:
    /** Gets the length of all gaps */
    int getGapsLength() const;

    void setParentAlignment(const MultipleSequenceAlignment& msa);
    void setParentAlignment(MultipleSequenceAlignmentData* msaData);

    MultipleSequenceAlignmentData* alignment = nullptr;
};

inline bool operator==(const MultipleSequenceAlignmentRow& ptr1, const MultipleSequenceAlignmentRow& ptr2) {
    return *ptr1 == *ptr2;
}
inline bool operator==(const MultipleSequenceAlignmentRow& ptr1, const MultipleSequenceAlignmentRowData* ptr2) {
    return ptr2 == nullptr ? ptr1->isDefault() : (*ptr1 == *ptr2);
}
inline bool operator==(const MultipleSequenceAlignmentRowData* ptr1, const MultipleSequenceAlignmentRow& ptr2) {
    return ptr1 == nullptr ? ptr2->isDefault() : (*ptr1 == *ptr2);
}
inline bool operator!=(const MultipleSequenceAlignmentRow& ptr1, const MultipleSequenceAlignmentRow& ptr2) {
    return !(ptr1 == ptr2);
}
inline bool operator!=(const MultipleSequenceAlignmentRow& ptr1, const MultipleSequenceAlignmentRowData* ptr2) {
    return !(ptr1 == ptr2);
}
inline bool operator!=(const MultipleSequenceAlignmentRowData* ptr1, const MultipleSequenceAlignmentRow& ptr2) {
    return !(ptr1 == ptr2);
}

}  // namespace U2
