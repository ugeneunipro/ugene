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

#ifndef _U2_SEQUENCE_DBI_H_
#define _U2_SEQUENCE_DBI_H_

#include <U2Core/U2Type.h>
#include <U2Core/U2Dbi.h>
#include <U2Core/U2Sequence.h>

namespace U2 {

/**
    An interface to access to sequence objects
*/
class U2SequenceDbi : public U2ChildDbi {
protected:
    U2SequenceDbi(U2Dbi* rootDbi) : U2ChildDbi(rootDbi){}

public:
    /**
        Reads sequence object from database by its id.
        If there is no sequence object with the specified id returns a default constructed value.
    */
    virtual U2Sequence getSequenceObject(const U2DataId& sequenceId, U2OpStatus& os) = 0;

    /**
        Reads specified sequence data region from database.
        The region must be valid region within sequence bounds.
        If there is no sequence with the specified id returns an empty QByteArray.
    */
    virtual QByteArray getSequenceData(const U2DataId& sequenceId, const U2Region& region, U2OpStatus& os) = 0;

    /**
        Adds a new (empty) sequence instance into database.
        Sets the assigned id to the passed U2Sequence instance.
        The folder must exist in the database.

        Note: Use 'updateSequenceData' method to supply data to the created sequence.

        Requires: U2DbiFeature_WriteSequence feature support
    */
    virtual void createSequenceObject(U2Sequence& sequence, const QString& folder, U2OpStatus& os,
        U2DbiObjectRank rank = U2DbiObjectRank_TopLevel) = 0;

    /**
        Updates sequence object fields.

        Requires: U2DbiFeature_WriteSequence feature support.
    */
    virtual void updateSequenceObject(U2Sequence& sequence, U2OpStatus& os) = 0;

    /**
        Updates sequence region:
        replaces 'regionToReplace' sequence region with 'dataToInsert'.

        Note: 'regionToReplace' length can differ from dataToInsert length, so the method
        can be used to insert/remove sequence regions.

        NOTE: The passed region 'startPos' has to be between 0 and sequence length inclusive.
        If the sequence is empty 'regionToReplace.startPos' has to be 0.

        HINT: use U2_REGION_MAX to replace or clear whole the sequence

        Requires: U2DbiFeature_WriteSequence feature support

        Note: if @updateLength is set then it will update length of the sequence object

        Hint: if @emptySequence is set then the start position of the inserting data is not calculated
    */
    virtual void updateSequenceData(const U2DataId& sequenceId, const U2Region& regionToReplace, const QByteArray& dataToInsert, const QVariantMap &hints, U2OpStatus& os) = 0;
    virtual void updateSequenceData(const U2DataId &masterId, const U2DataId &sequenceId,
                                    const U2Region &regionToReplace, const QByteArray &dataToInsert, const QVariantMap &hints, U2OpStatus &os) = 0;

};

} //namespace

#endif
