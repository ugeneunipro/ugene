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

#ifndef _U2_MULTIPLE_ALIGNMENT_H_
#define _U2_MULTIPLE_ALIGNMENT_H_

#include <QVariantMap>

#include "MultipleAlignmentRow.h"

namespace U2 {

class DNAAlphabet;

/** Default name for a multiple alignment */
#define MA_OBJECT_NAME QString("Multiple alignment")

#define MAlignment_TailedGapsPattern "\\-+$"

class MultipleAlignmentData;

class U2CORE_EXPORT MultipleAlignment {
protected:
    MultipleAlignment(MultipleAlignmentData *maData);

public:
    enum Order {
        Ascending,
        Descending
    };

    virtual ~MultipleAlignment();

    MultipleAlignmentData * data() const;
    template <class Derived> inline Derived dynamicCast() const;

    MultipleAlignmentData & operator*();
    const MultipleAlignmentData & operator*() const;

    MultipleAlignmentData * operator->();
    const MultipleAlignmentData * operator->() const;

protected:
    QSharedPointer<MultipleAlignmentData> maData;
};

template <class Derived>
Derived MultipleAlignment::dynamicCast() const {
    return Derived(*this);
}

/**
 * Multiple alignment
 * The length of the alignment is the maximum length of its rows.
 * There are minimal checks on the alignment's alphabet, but the client of the class
 * is expected to keep the conformance of the data and the alphabet.
 */
class U2CORE_EXPORT MultipleAlignmentData {
protected:
    /**
     * Creates a new alignment.
     * The name must be provided if this is not default alignment.
     */
    MultipleAlignmentData(const QString &name = QString(),
        const DNAAlphabet *alphabet = NULL,
        const QList<MultipleAlignmentRow> &rows = QList<MultipleAlignmentRow>());
    MultipleAlignmentData(const MultipleAlignmentData &multipleAlignment);

public:
    virtual ~MultipleAlignmentData();

    // TODO: marked to remove (if it is not used)
    //    const MultipleAlignmentData & operator=(const MultipleAlignmentData &other);

    /**
     * Clears the alignment. Makes alignment length == 0.
     * Doesn't change alphabet or name
     */
    void clear();

    /** Returns  the name of the alignment */
    QString getName() const;

    /** Sets the name of the alignment */
    void setName(const QString &newName);

    /** Returns the alphabet of the alignment */
    const DNAAlphabet * getAlphabet() const;

    /**
     * Sets the alphabet of the alignment, the value can't be NULL.
     * Warning: rows already present in the alignment are not verified to correspond to this alphabet
     */
    void setAlphabet(const DNAAlphabet *alphabet);

    /** Returns the alignment info */
    QVariantMap getInfo() const;

    /** Sets the alignment info */
    void setInfo(const QVariantMap &info);

    /** Returns true if the length of the alignment is 0 */
    bool isEmpty() const;

    /** Returns the length of the alignment */
    int getLength() const;

    /** Sets the length of the alignment. The length must be >= 0. */
    void setLength(int length);

    /** Returns the number of rows in the alignment */
    int getNumRows() const;

    U2MsaListGapModel getGapModel() const;

    /** Sorts rows by name */
    void sortRowsByName(MultipleAlignment::Order order = MultipleAlignment::Ascending);

    /** Returns row of the alignment */
    MultipleAlignmentRow getRow(int row);
    const MultipleAlignmentRow & getRow(int row) const;
    const MultipleAlignmentRow & getRow(const QString &name) const;

    /** Returns all rows in the alignment */
    const QList<MultipleAlignmentRow> & getRows() const;

    /** Returns IDs of the alignment rows in the database */
    QList<qint64> getRowsIds() const;

    /** Returns row ids by row indexes. */
    QList<qint64> getRowIdsByRowIndexes(const QList<int>& rowIndexes) const;

    MultipleAlignmentRow getRowByRowId(qint64 rowId, U2OpStatus &os) const;

    char charAt(int rowNumber, qint64 position) const;
    bool isGap(int rowNumber, qint64 pos) const;

    /** Returns all rows' names in the alignment */
    QStringList getRowNames() const;

    int getRowIndexByRowId(qint64 rowId, U2OpStatus &os) const;

    /**
     * Renames the row with the specified index.
     * Assumes that the row index is valid and the name is not empty.
     */
    void renameRow(int rowIndex, const QString &name);

    /** Updates row ID of the row at 'rowIndex' position */
    void setRowId(int rowIndex, qint64 rowId);

    /**
     * Removes a row from alignment.
     * The alignment is changed only (to zero) if the alignment becomes empty.
     */
    void removeRow(int rowIndex, U2OpStatus &os);

    /**
     * Removes up to n characters starting from the specified position.
     * Can decrease the overall alignment length.
     */
    void removeChars(int row, int pos, int n, U2OpStatus &os);

    /**
     * Shifts a selection of consequent rows.
     * 'delta' can be positive or negative.
     * It is assumed that indexes of shifted rows are within the bounds of the [0, number of rows).
     */
    void moveRowsBlock(int startRow, int numRows, int delta);

    /**
     * Compares two alignments: lengths, alphabets, rows and infos (that include names).
     */
    bool operator==(const MultipleAlignmentData &ma) const;
    bool operator!=(const MultipleAlignmentData &ma) const;

    /** Checks model consistency */
    virtual void check() const;

    /** Arranges rows in lists order*/
    bool sortRowsByList(const QStringList &order);

    virtual MultipleAlignment getCopy() const = 0;

protected:
    virtual MultipleAlignmentRow getEmptyRow() const = 0;

    /** Helper-method for adding a row to the alignment */
    void addRowPrivate(const MultipleAlignmentRow &row, qint64 rowLenWithTrailingGaps, int rowIndex);

    /** Alphabet for all sequences in the alignment */
    const DNAAlphabet *alphabet;

    /** Alignment rows (each row = sequence + gap model) */
    QList<MultipleAlignmentRow> rows;

    /** The length of the longest row in the alignment */
    qint64 length;

    /** Additional alignment info */
    QVariantMap info;
};

inline bool	operator!=(const MultipleAlignment &ptr1, const MultipleAlignment &ptr2) { return *ptr1 != *ptr2; }
inline bool	operator!=(const MultipleAlignment &ptr1, const MultipleAlignmentData *ptr2) { return *ptr1 != *ptr2; }
inline bool	operator!=(const MultipleAlignmentData *ptr1, const MultipleAlignment &ptr2) { return *ptr1 != *ptr2; }
inline bool	operator==(const MultipleAlignment &ptr1, const MultipleAlignment &ptr2) { return *ptr1 == *ptr2; }
inline bool	operator==(const MultipleAlignment &ptr1, const MultipleAlignmentData *ptr2) { return *ptr1 == *ptr2; }
inline bool	operator==(const MultipleAlignmentData *ptr1, const MultipleAlignment &ptr2) { return *ptr1 == *ptr2; }

}   // namespace U2

#endif // _U2_MULTIPLE_ALIGNMENT_H_
