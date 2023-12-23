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

#include "MultipleAlignment.h"
#include "MultipleAlignmentRow.h"

namespace U2 {

class McaRowMemoryData;
class MultipleChromatogramAlignmentData;

class U2CORE_EXPORT MultipleChromatogramAlignment : public MultipleAlignment {
public:
    MultipleChromatogramAlignment();
    MultipleChromatogramAlignment(const MultipleAlignment& ma);
    MultipleChromatogramAlignment(MultipleChromatogramAlignmentData* mcaData);
    MultipleChromatogramAlignment(const QString& name,
                                  const DNAAlphabet* alphabet = nullptr,
                                  const QList<MultipleAlignmentRow>& rows = {});

    MultipleChromatogramAlignmentData* data() const;

    MultipleChromatogramAlignmentData& operator*();
    const MultipleChromatogramAlignmentData& operator*() const;

    MultipleChromatogramAlignmentData* operator->();
    const MultipleChromatogramAlignmentData* operator->() const;

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
    MultipleChromatogramAlignmentData(const QString& name = QString(),
                                      const DNAAlphabet* alphabet = nullptr,
                                      const QList<MultipleAlignmentRow>& rows = QList<MultipleAlignmentRow>());
    MultipleChromatogramAlignmentData(const MultipleChromatogramAlignmentData& mcaData);

public:
    MultipleChromatogramAlignmentData& operator=(const MultipleChromatogramAlignment& mca);
    MultipleChromatogramAlignmentData& operator=(const MultipleChromatogramAlignmentData& mcaData);

    /**
     * Creates a new alignment from the sub-alignment. Do not trims the result.
     * Assumes that 'start' >= 0, and 'start + len' is less or equal than the alignment length.
     */
    MultipleChromatogramAlignment mid(int start, int len) const;

    /**
     * Joins two alignments. Alignments must have the same size and alphabet.
     * Increases the alignment length.
     */
    MultipleChromatogramAlignmentData& operator+=(const MultipleChromatogramAlignmentData& mcaData);

    /** Compares two alignments: calls isEqual() method. */
    bool operator==(const MultipleChromatogramAlignmentData& other) const;
    bool operator!=(const MultipleChromatogramAlignmentData& other) const;

    MultipleAlignment getCopy() const override;
    MultipleChromatogramAlignment getExplicitCopy() const;

private:
    void copy(const MultipleAlignmentData& other);
    void copy(const MultipleChromatogramAlignmentData& other);
};

inline bool operator!=(const MultipleChromatogramAlignment& ptr1, const MultipleChromatogramAlignment& ptr2) {
    return *ptr1 != *ptr2;
}
inline bool operator!=(const MultipleChromatogramAlignment& ptr1, const MultipleChromatogramAlignmentData* ptr2) {
    return *ptr1 != *ptr2;
}
inline bool operator!=(const MultipleChromatogramAlignmentData* ptr1, const MultipleChromatogramAlignment& ptr2) {
    return *ptr1 != *ptr2;
}
inline bool operator==(const MultipleChromatogramAlignment& ptr1, const MultipleChromatogramAlignment& ptr2) {
    return *ptr1 == *ptr2;
}
inline bool operator==(const MultipleChromatogramAlignment& ptr1, const MultipleChromatogramAlignmentData* ptr2) {
    return *ptr1 == *ptr2;
}
inline bool operator==(const MultipleChromatogramAlignmentData* ptr1, const MultipleChromatogramAlignment& ptr2) {
    return *ptr1 == *ptr2;
}

}  // namespace U2
