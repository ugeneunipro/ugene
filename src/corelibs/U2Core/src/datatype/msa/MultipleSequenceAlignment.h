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

    /**
     * Joins two alignments. Alignments must have the same size and alphabet.
     * Increases the alignment length.
     */
    MultipleSequenceAlignmentData& operator+=(const MultipleSequenceAlignmentData& ma);

    /** Compares two alignments: calls isEqual() method. */
    bool operator==(const MultipleSequenceAlignmentData& msaData) const;
    bool operator!=(const MultipleSequenceAlignmentData& msaData) const;

    MultipleAlignment getCopy() const override;
    MultipleSequenceAlignment getExplicitCopy() const;

private:
    void copy(const MultipleAlignmentData& other);
    void copy(const MultipleSequenceAlignmentData& other);
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
