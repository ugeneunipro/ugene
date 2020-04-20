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

#ifndef _U2_MSA_DBI_H_
#define _U2_MSA_DBI_H_

#include <U2Core/U2Type.h>
#include <U2Core/U2Dbi.h>
#include <U2Core/U2Msa.h>

namespace U2 {

/**
    An interface to obtain access to multiple sequence alignment
*/
class U2MsaDbi : public U2ChildDbi {
protected:
    U2MsaDbi(U2Dbi* rootDbi) : U2ChildDbi(rootDbi) {}

public:
    /** Reads Msa objects by id */
    virtual U2Msa getMsaObject(const U2DataId& id, U2OpStatus& os) = 0;

    /** Returns the number of rows in the MSA */
    virtual qint64 getNumOfRows(const U2DataId& msaId, U2OpStatus& os) = 0;

    /** Returns all MSA rows */
    virtual QList<U2MsaRow> getRows(const U2DataId& msaId, U2OpStatus& os) = 0;

    /** Return a row with the specified ID */
    virtual U2MsaRow getRow(const U2DataId& msaId, qint64 rowId, U2OpStatus& os) = 0;

    /** Returns the list of rows IDs in the database for the specified MSA (in increasing order) */
    virtual QList<qint64> getRowsOrder(const U2DataId& msaId, U2OpStatus& os) = 0;

    /** Return the MSA alphabet */
    virtual U2AlphabetId getMsaAlphabet(const U2DataId& msaId, U2OpStatus& os) = 0;

    /** Return the MSA length */
    virtual qint64 getMsaLength(const U2DataId& msaId, U2OpStatus& os) = 0;

    /**
     * Creates a new empty MSA object
     * Requires: U2DbiFeature_WriteMsa feature support
     */
    virtual U2DataId createMcaObject(const QString& folder, const QString& name, const U2AlphabetId& alphabet, U2OpStatus& os) = 0;
    virtual U2DataId createMcaObject(const QString& folder, const QString& name, const U2AlphabetId& alphabet, int length, U2OpStatus& os) = 0;
    virtual U2DataId createMsaObject(const QString& folder, const QString& name, const U2AlphabetId& alphabet, U2OpStatus& os) = 0;
    virtual U2DataId createMsaObject(const QString& folder, const QString& name, const U2AlphabetId& alphabet, int length, U2OpStatus& os) = 0;

    /**
     * Updates the multiple alignment name
     * Requires: U2DbiFeature_WriteMsa feature support
     */
    virtual void updateMsaName(const U2DataId& msaId, const QString& name, U2OpStatus& os) = 0;

    /**
     * Updates the multiple alignment alphabet
     * Requires: U2DbiFeature_WriteMsa feature support
     */
    virtual void updateMsaAlphabet(const U2DataId& msaId, const U2AlphabetId& alphabet, U2OpStatus& os) = 0;

    /**
     * Adds rows to the MSA
     * Requires: U2DbiFeature_WriteMsa feature support
     */
    virtual void addRows(const U2DataId& msaId, QList<U2MsaRow>& rows, U2OpStatus& os) = 0;

    /**
     * Adds a row to the MSA
     * If 'posInMsa' equals to '-1' the row is appended to the end of the MSA,
     * otherwise it is inserted to the specified position and all positions are updated.
     * Requires: U2DbiFeature_WriteMsa feature support
     */
    virtual void addRow(const U2DataId& msaId, qint64 posInMsa, U2MsaRow& row, U2OpStatus& os) = 0;

    /**
     * Removes rows from MSA
     * Automatically removes affected sequences that are not anymore located in some folder nor Msa object
     * Requires: U2DbiFeature_WriteMsa feature support
     */
    virtual void removeRows(const U2DataId& msaId, const QList<qint64>& rowIds, U2OpStatus& os) = 0;

    /**
     * Removes the row gaps, the row.
     * Also removes the record that the msa is a parent of the row sequence
     * and attempts to remove the sequence after it.
     * Requires: U2DbiFeature_WriteMsa feature support
     */
    virtual void removeRow(const U2DataId& msaId, qint64 rowId, U2OpStatus& os) = 0;

    /** Updates name of the sequence of the row. */
    virtual void updateRowName(const U2DataId& msaId, qint64 rowId, const QString& newName, U2OpStatus& os) = 0;

    /**
     * Updates a row with the specified ID and its sequence.
     * Requires: U2DbiFeature_WriteMsa feature support
     */
    virtual void updateRowContent(const U2DataId& msaId, qint64 rowId, const QByteArray& seqBytes, const QList<U2MsaGap>& gaps, U2OpStatus& os) = 0;

    /**
     * Removes all previous values and sets a new gap model for a row in a MSA
     * Requires: U2DbiFeature_WriteMsa feature support
     */
    virtual void updateGapModel(const U2DataId& msaId, qint64 msaRowId, const QList<U2MsaGap>& gapModel, U2OpStatus& os) = 0;

    /**
     * Updates positions of the rows in the database according to the order in the list
     * Be careful, all IDs must exactly match IDs of the MSA!
     * Requires: U2DbiFeature_WriteMsa feature support
     */
    virtual void setNewRowsOrder(const U2DataId& msaId, const QList<qint64>& rowIds, U2OpStatus& os) = 0;

    /** Updates a part of the Msa object info - the length */
    virtual void updateMsaLength(const U2DataId& msaId, qint64 length, U2OpStatus& os) = 0;
};


} //namespace

#endif
