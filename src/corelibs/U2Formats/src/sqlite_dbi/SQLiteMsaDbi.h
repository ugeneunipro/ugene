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

#ifndef _U2_SQLITE_MSA_DBI_H_
#define _U2_SQLITE_MSA_DBI_H_

#include <U2Core/U2AbstractDbi.h>

#include "SQLiteDbi.h"
#include "SQLiteObjectDbi.h"

namespace U2 {

class SQLiteMsaDbi : public U2MsaDbi, public SQLiteChildDBICommon {
public:
    SQLiteMsaDbi(SQLiteDbi* dbi);

    /** Creates all required tables */
    void initSqlSchema(U2OpStatus& os) override;

    /** Reads Msa objects by id */
    U2Msa getMsaObject(const U2DataId& id, U2OpStatus& os) override;

    /** Returns the number of rows of the MSA (value cached in Msa table) */
    int getNumOfRows(const U2DataId& msaId, U2OpStatus& os) override;

    /** Returns all rows of a MSA with the specified IDs */
    QList<U2MsaRow> getRows(const U2DataId& msaId, U2OpStatus& os) override;

    /** Returns a row with the specified ID */
    U2MsaRow getRow(const U2DataId& msaId, qint64 rowId, U2OpStatus& os) override;

    /** Returns the list of rows IDs in the database for the specified MSA (in increasing order) */
    QList<qint64> getOrderedRowIds(const U2DataId& msaId, U2OpStatus& os) override;

    /** Return the MSA alphabet */
    U2AlphabetId getMsaAlphabet(const U2DataId& msaId, U2OpStatus& os) override;

    /** Returns length stored in Msa table */
    qint64 getMsaLength(const U2DataId& msaId, U2OpStatus& os) override;

    /**
     * Creates a new empty multiple alignment in the database.
     * The folder must exist in the database.
     * The number of rows and the length of the alignment are set to 0.
     * Returns the assigned id.
     */
    U2DataId createMcaObject(const QString& folder, const QString& name, const U2AlphabetId& alphabet, U2OpStatus& os) override;
    U2DataId createMcaObject(const QString& folder, const QString& name, const U2AlphabetId& alphabet, int length, U2OpStatus& os) override;
    U2DataId createMsaObject(const QString& folder, const QString& name, const U2AlphabetId& alphabet, U2OpStatus& os) override;
    U2DataId createMsaObject(const QString& folder, const QString& name, const U2AlphabetId& alphabet, int length, U2OpStatus& os) override;

    /**
     * Updates the multiple alignment name.
     * Increments the alignment version.
     * Tracks modifications, if required.
     */
    void updateMsaName(const U2DataId& msaId, const QString& name, U2OpStatus& os) override;

    /**
     * Updates the multiple alignment alphabet.
     * Increments the alignment version.
     * Tracks modifications, if required.
     */
    void updateMsaAlphabet(const U2DataId& msaId, const U2AlphabetId& alphabet, U2OpStatus& os) override;

    void addRows(const U2DataId& msaId, QList<U2MsaRow>& rows, int insertRowIndex, U2OpStatus& os) override;

    /**
     * Creates a new row and gap model records in the database.
     * Enlarges msa length, if 'row.length' is greater than current msa length.
     * Ignores 'row.rowId'.
     * Recalculates 'row.length' and and 'row.rowId'. Sets valid to the passed U2MsaRow instance.
     * If 'posInMsa' equals to '-1' the row is appended to the end of the MSA,
     * otherwise it is inserted to the specified position and all positions are updated.
     * Assigns MSA as a parent for the sequence.
     * Updates the number of rows of the MSA.
     * Updates the alignment length.
     * Increments the alignment version.
     * Tracks modifications, if required.
     */
    void addRow(const U2DataId& msaId, int insertRowIndex, U2MsaRow& row, U2OpStatus& os) override;

    /**
     * Removes rows for the specified alignment and with the specified ids
     * from the database.
     * Updates the number of rows of the MSA.
     * Updates all rows positions for the alignment.
     * Updates the alignment length.
     * Tracks modifications, if required.
     */
    void removeRows(const U2DataId& msaId, const QList<qint64>& rowIds, U2OpStatus& os) override;

    /**
     * Removes a row with the specified ID for the specified alignment.
     * Updates the number of rows of the MSA.
     * Updates all rows positions for the alignment.
     * Updates the alignment length.
     * Increments the alignment version.
     * Tracks modifications, if required.
     */
    void removeRow(const U2DataId& msaId, qint64 rowId, U2OpStatus& os) override;

    /**
     * Removes all rows from the alignment with the specified id.
     * Does not update the alignment length.
     * Does not increment the alignment version.
     * USE this method only on MSA deletion
     */
    void deleteRowsData(const U2DataId& msaId, U2OpStatus& os);

    /**
     * Updates name of the sequence of the row.
     * Increments the alignment version.
     * Tracks modifications, if required.
     */
    void updateRowName(const U2DataId& msaId, qint64 rowId, const QString& newName, U2OpStatus& os) override;

    /**
     * Updates sequence data and information about the row.
     * Note that the row 'gstart' and 'gend' are set to the sequence bounds.
     * Updates the alignment length.
     * Increments the alignment version.
     */
    void updateRowContent(const U2DataId& msaId, qint64 rowId, const QByteArray& seqBytes, const QVector<U2MsaGap>& gaps, U2OpStatus& os) override;

    /**
     * Removes all previous values and sets a new gap model for a row in a MSA.
     * Updates the alignment length.
     * Increments the alignment version.
     */
    void updateGapModel(const U2DataId& msaId, qint64 msaRowId, const QVector<U2MsaGap>& gapModel, U2OpStatus& os) override;

    /** Updates a part of the Msa object info - the length */
    void updateMsaLength(const U2DataId& msaId, qint64 length, U2OpStatus& os) override;

    /**
     * Updates positions of the rows in the database according to the order in the list
     * Be careful, all IDs must exactly match IDs of the MSA!
     * Increments the alignment version.
     */
    void setNewRowsOrder(const U2DataId& msaId, const QList<qint64>& rowIds, U2OpStatus& os) override;

    /** Undo the operation for the MSA. */
    void undo(const U2DataId& msaId, qint64 modType, const QByteArray& modDetails, U2OpStatus& os);

    /** Redo the operation for the MSA. */
    void redo(const U2DataId& msaId, qint64 modType, const QByteArray& modDetails, U2OpStatus& os);

private:
    /**
     * Creates new records in MsaRow and MsaRowGap tables for the added row, and
     * sets the parent of the sequence object to the MSA object.
     * Sets the assigned ID to the passed U2MaRow instance.
     */
    void addMsaRowAndGaps(const U2DataId& msaId, qint64 posInMsa, U2MsaRow& row, U2OpStatus& os);

    /**
     * Adds a new MSA row into database.
     * To add a gap for the row, use the "createMsaRowGap" method.
     */
    void createMsaRow(const U2DataId& msaId, qint64 posInMsa, U2MsaRow& msa, U2OpStatus& os);

    /** Adds a new gap for a MSA row into database. */
    void createMsaRowGap(const U2DataId& msaId, qint64 msaRowId, const U2MsaGap& msaGap, U2OpStatus& os);

    /** Removes records from MsaRow and MsaRowGap tables for the row. */
    void removeMsaRowAndGaps(const U2DataId& msaId, qint64 rowId, bool removeSequence, U2OpStatus& os);

    /** Removes all records about the row gaps from the database. */
    void removeRecordsFromMsaRowGap(const U2DataId& msaId, qint64 rowId, U2OpStatus& os);

    /** Removes a record about the row from the database. */
    void removeRecordFromMsaRow(const U2DataId& msaId, qint64 rowId, U2OpStatus& os);

    /** Updates "numOfRows" in the "Msa" table */
    void updateNumOfRows(const U2DataId& msaId, qint64 numOfRows, U2OpStatus& os);

    /**
     * Re-calculates positions of the MSA rows to go from 0 to "numOfRows - 1",
     * the original position must be in increasing order.
     */
    void recalculateRowsPositions(const U2DataId& msaId, U2OpStatus& os);

    /** Calculates length of the row (characters + gaps), does NOT take into account trailing gaps. */
    qint64 calculateRowLength(qint64 seqLength, const QVector<U2MsaGap>& gaps);

    /** Gets length of the sequence in the row (without gaps) */
    qint64 getRowSequenceLength(const U2DataId& msaId, qint64 rowId, U2OpStatus& os);

    /** Updates 'length' field in MsaRow for specified */
    void updateRowLength(const U2DataId& msaId, qint64 rowId, qint64 newLength, U2OpStatus& os);

    /** Gets a sequence ID for the row */
    U2DataId getSequenceIdByRowId(const U2DataId& msaId, qint64 rowId, U2OpStatus& os);

    /** Required for store modification track of removed rows  */
    QByteArray getRemovedRowDetails(const U2MsaRow& row);

    /** Gets position of the row in the msa */
    qint64 getPosInMsa(const U2DataId& msaId, qint64 rowId, U2OpStatus& os);

    qint64 getMaximumRowId(const U2DataId& msaId, U2OpStatus& os);

    ///////////////////////////////////////////////////////////
    // Core methods
    void updateGapModelCore(const U2DataId& msaId, qint64 msaRowId, const QVector<U2MsaGap>& gapModel, U2OpStatus& os);
    void addRowSubcore(const U2DataId& msaId, qint64 numOfRows, const QList<qint64>& rowsOrder, U2OpStatus& os);
    void addRowCore(const U2DataId& msaId, int insertRowIndex, U2MsaRow& row, U2OpStatus& os);
    void addRowsCore(const U2DataId& msaId, const QList<int>& insertRowIndexes, QList<U2MsaRow>& rows, U2OpStatus& os);
    void removeRowSubcore(const U2DataId& msaId, qint64 numOfRows, U2OpStatus& os);
    void removeRowCore(const U2DataId& msaId, qint64 rowId, bool removeSequence, U2OpStatus& os);
    void removeRowsCore(const U2DataId& msaId, const QList<qint64>& rowIds, bool removeSequence, U2OpStatus& os);
    void setNewRowsOrderCore(const U2DataId& msaId, const QList<qint64> rowIds, U2OpStatus& os);
    void updateRowInfoCore(const U2DataId& msaId, const U2MsaRow& row, U2OpStatus& os);
    void updateMsaLengthCore(const U2DataId& msaId, qint64 length, U2OpStatus& os);

    ///////////////////////////////////////////////////////////
    // Undo methods
    void undoUpdateMsaAlphabet(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os);
    void undoAddRows(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os);
    void undoAddRow(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os);
    void undoRemoveRows(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os);
    void undoRemoveRow(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os);
    void undoUpdateGapModel(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os);
    void undoSetNewRowsOrder(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os);
    void undoUpdateRowInfo(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os);
    void undoMsaLengthChange(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os);

    ///////////////////////////////////////////////////////////
    // Redo methods
    void redoUpdateMsaAlphabet(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os);
    void redoAddRows(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os);
    void redoAddRow(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os);
    void redoRemoveRows(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os);
    void redoRemoveRow(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os);
    void redoUpdateGapModel(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os);
    void redoSetNewRowsOrder(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os);
    void redoUpdateRowInfo(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os);
    void redoMsaLengthChange(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os);

    ///////////////////////////////////////////////////////////
    // Methods included into a multi-action
    void updateRowInfo(SQLiteModificationAction& updateAction, const U2DataId& msaId, const U2MsaRow& row, U2OpStatus& os);
    void updateGapModel(SQLiteModificationAction& updateAction, const U2DataId& msaId, qint64 msaRowId, const QVector<U2MsaGap>& gapModel, U2OpStatus& os);
    void updateMsaLength(SQLiteModificationAction& updateAction, const U2DataId& msaId, qint64 length, U2OpStatus& os);
};

}  // namespace U2

#endif
