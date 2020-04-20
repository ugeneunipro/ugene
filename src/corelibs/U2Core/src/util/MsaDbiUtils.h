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

#ifndef _U2_MSA_DBI_UTILS_
#define _U2_MSA_DBI_UTILS_

#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/U2MsaDbi.h>

namespace U2 {

class U2CORE_EXPORT MaDbiUtils : public QObject {
    Q_OBJECT
public:
    /** Split 'input' bytes into sequence bytes (chars) and a gap model */
    static void splitBytesToCharsAndGaps(const QByteArray& input, QByteArray& seqBytes, QList<U2MsaGap>& gapModel);

    /**
    * Get the length of the alignment in the database.
    */
    static qint64 getMaLength(const U2EntityRef& maRef, U2OpStatus& os);

    /**
    * Update the length of the alignment in the database.
    */
    static void updateMaLength(const U2EntityRef& maRef, qint64 newLen, U2OpStatus& os);

    /**
    * Get the alphabet of the alignment in the database.
    */
    static U2AlphabetId getMaAlphabet(const U2EntityRef& maRef, U2OpStatus& os);

    /**
    * Update the alphabet of the alignment in the database.
    */
    static void updateMaAlphabet(const U2EntityRef& maRef, const U2AlphabetId& alphabet, U2OpStatus& os);

    /**
     * Renames an alignment.
     * Parameter 'newName' must be NOT empty!
     */
    static void renameMa(const U2EntityRef& maRef, const QString& newName, U2OpStatus& os);

    /**
     * Updates a gap model of the specified row in the database.
     * Parameter 'rowId' must contain a valid row ID in the database.
     */
    static void updateRowGapModel(const U2EntityRef& maRef, qint64 rowId, const QList<U2MsaGap>& gaps, U2OpStatus& os);

    static QList<qint64> getRowsOrder(const U2EntityRef& meRef, U2OpStatus& os);

    /**
     * Updates positions of the rows in the database according to the order in the list.
     * All IDs must exactly match IDs of the MSA!
     */
    static void updateRowsOrder(const U2EntityRef& maRef, const QList<qint64>& rowsOrder, U2OpStatus& os);

    /**
     * Renames a row of the alignment, i.e. the corresponding sequence.
     * Parameter 'rowId' must contain a valid row ID in the database.
     * Parameter 'newName' must be NOT empty!
     */
    static void renameRow(const U2EntityRef& msaRef, qint64 rowId, const QString& newName, U2OpStatus& os);

    /**
     * Updates positions of the rows in the database according to the delta.
     * If some rows can`t move, the other rows will continue to move until there is space.
     * rowsToMove must have the same relative order as rows in database have.
     * All IDs must exactly match IDs of the MSA!
     */
    static void moveRows(const U2EntityRef& msaRef, const QList<qint64>& rowsToMove, const int delta, U2OpStatus& os);

    /**
     * Calculates start and end position in the sequence,
     * depending on the start position in the row and the 'count' character from it
     */
    static void getStartAndEndSequencePositions(const QByteArray &seq, const QList<U2MsaGap> &gaps,
        qint64 pos, qint64 count,
        qint64& startPosInSeq, qint64& endPosInSeq);

    static DbiConnection * getCheckedConnection(const U2DbiRef &dbiRef, U2OpStatus &os);
    static bool validateRowIds(const MultipleSequenceAlignment& al, const QList<qint64>& rowIds);
    static void validateRowIds(U2MsaDbi *msaDbi, const U2DataId &msaId, const QList<qint64>& rowIds, U2OpStatus &os);

    static void calculateGapModelAfterReplaceChar(QList<U2MsaGap>& gapModel, qint64 pos);
};

class U2CORE_EXPORT MsaDbiUtils : public QObject {
    Q_OBJECT
        friend class McaDbiUtils;
public:

    /**
     * Updates the whole alignment in the database:
     * 1) Updates the MSA object
     * 2) If 'al' rows have IDs of the rows and sequences in the database, updates these data.
     *    Otherwise adds or removes the corresponding rows and sequences.
     * 3) Updates rows positions
     */
    static void updateMsa(const U2EntityRef& msaRef, const MultipleSequenceAlignment& al, U2OpStatus& os);

    /**
     * Inserts 'count' gaps to rows with specified IDs from 'pos' position.
     * Updates the alignment length.
     * Parameter 'rowIds' must contain valid IDs of the alignment rows in the database!
     * Parameter 'pos' must be >=0 and < the alignment length.
     * Parameter 'count' must be > 0.
     */
    static void insertGaps(const U2EntityRef& msaRef, const QList<qint64>& rowIds, qint64 pos, qint64 count, U2OpStatus& os, bool collapseTrailingGaps);

    /**
     * Removes a region from an alignment.
     * Does NOT trim the result alignment!
     * Parameter 'rowIds' must contain valid IDs of the alignment rows in the database!
     * Parameter 'pos' must be >=0.
     * Parameter 'count' must be > 0.
     * The sum 'pos' + 'count' must be <= the alignment length.
     */
    static void removeRegion(const U2EntityRef& msaRef, const QList<qint64>& rowIds, qint64 pos, qint64 count, U2OpStatus& os);

    /**
    * Replace a character in an alignment.
    * Parameter 'rowId' must be valid ID of the alignment row in the database!
    * Parameter 'pos' must be >= 0.
    */
    static void replaceCharacterInRow(const U2EntityRef& msaRef, qint64 rowId, qint64 pos, char newChar, U2OpStatus& os);

    /**
     * Keeps only the specified rows in the alignment - 'count' characters from position 'pos'.
     * If a row length is less than 'pos', the sequence and gap model becomes empty.
     * Updates the alignment length.
     * Parameter 'rowIds' must contain valid IDs of the alignment rows in the database!
     * Parameter 'pos' must be >= 0 and < the alignment length.
     * Parameter 'count' must be > 0.
     */
    static void crop(const U2EntityRef& msaRef, const QList<qint64> rowIds, qint64 pos, qint64 count, U2OpStatus& os);

    /**
     * Removes leading and trailing gaps, if required.
     * Updates the alignment length in this case.
     * Gap mode should be correct, else some gaps may be not trimmed.
     * Returns list of modified rows.
     */
    static QList<qint64> trim(const U2EntityRef& msaRef, U2OpStatus& os);

    /**
     * Adds a row to the alignment and updates 'row'.
     * Updates the alignment length.
     * Parameter 'posInMsa' must be within bounds [0, numRows] or '-1' (row is appended).
     * Parameter 'row' must contain valid values!
     */
    static void addRow(const U2EntityRef& msaRef, qint64 posInMsa, U2MsaRow& row, U2OpStatus& os);

    /**
     * Removes a row from the alignment.
     * Parameter 'rowId' must contain a valid row ID in the database.
     */
    static void removeRow(const U2EntityRef& msaRef, qint64 rowId, U2OpStatus& os);

    /**
     * Updates the row sequence data and the row info (gaps, etc.) in the database.
     * Parameter 'rowId' must contain a valid row ID in the database.
     */
    static void updateRowContent(const U2EntityRef& msaRef, qint64 rowId,
        const QByteArray& seqBytes, const QList<U2MsaGap>& gaps,
        U2OpStatus& os);

    /**
     * If some of specified rows is empty it will be removed.
     * Returns list of removed row ids.
     */
    static QList<qint64> removeEmptyRows(const U2EntityRef& msaRef, const QList<qint64>& rowIds, U2OpStatus &os);

    /** Calculates a new gap model when 'count' gaps are inserted to 'pos' position */
    static void calculateGapModelAfterInsert(QList<U2MsaGap>& gapModel, qint64 pos, qint64 count);

private:
    /**
     * Verifies if the alignment contains columns of gaps at the beginning.
     * Delete this columns of gaps from each row.
     * Gaps should be ordered and should not stick together.
     * Returns a list containing modified rows
     */
    static QList<U2MsaRow> cutOffLeadingGaps(QList<U2MsaRow>& rows);

    /**
     * Delete all gaps from gapModel after msaLength.
     * If gap begins before the end of alignment and end after it, it will be modified:
     * the gap`s end will be equal to the alignment`s end.
     * Returns a list containing modified rows
     */
    static QList<U2MsaRow> cutOffTrailingGaps(QList<U2MsaRow>& rows, const qint64 msaLength);

    /**
     * Removes gaps from the row between position 'pos' and 'pos + count'.
     * Shifts the remaining gaps, if required.
     */
    static void calculateGapModelAfterRemove(QList<U2MsaGap>& gapModel, qint64 pos, qint64 count);

    /** Length of all gaps in the gap model */
    static qint64 calculateGapsLength(const QList<U2MsaGap>& gapModel);

    /** Length of the sequence and gap model for the row */
    static qint64 calculateRowLength(const U2MsaRow& row);

    /** If there are consecutive gaps in the gaps model, merges them into one gap */
    static void mergeConsecutiveGaps(QList<U2MsaGap>& gapModel);

    /** Removes chars/gaps from the row */
    static void removeCharsFromRow(QByteArray &seq, QList<U2MsaGap> &gaps, qint64 pos, qint64 count);

    /** Replace chars in the row */
    static void replaceCharInRow(QByteArray &seq, QList<U2MsaGap> &gaps, qint64 pos, char newChar);

    /**
     * Crops a row to region from 'pos' to 'pos' + 'count',
     * or a lesser region, if the length of the row is smaller.
     * Parameter 'pos' can even be greater than the length of the row.
     * The row sequence and gap model are set to empty values in this case.
     */
    static void cropCharsFromRow(MultipleSequenceAlignmentRow& alRow, qint64 pos, qint64 count);

    /** Returns "true" if there is a gap on position "pos" */
    static bool gapInPosition(const QList<U2MsaGap>& gapModel, qint64 pos);
};

} // namespace

#endif
