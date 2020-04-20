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

#ifndef _U2_MYSQL_SEQUENCE_DBI_H_
#define _U2_MYSQL_SEQUENCE_DBI_H_

#include "MysqlDbi.h"
#include "MysqlObjectDbi.h"

#include <U2Core/U2SequenceDbi.h>

namespace U2 {

class MysqlModificationAction;

class U2FORMATS_EXPORT MysqlSequenceDbi : public U2SequenceDbi, public MysqlChildDbiCommon {

public:
    MysqlSequenceDbi(MysqlDbi* dbi);

    virtual void initSqlSchema(U2OpStatus& os);

    /** Reads sequence object from database */
    virtual U2Sequence getSequenceObject(const U2DataId& sequenceId, U2OpStatus& os);

    /**  Reads specified sequence data region from database */
    virtual QByteArray getSequenceData(const U2DataId& sequenceId, const U2Region& region, U2OpStatus& os);

    /**  Adds new (empty) sequence instance into database, sets the assigned id on the passed U2Sequence instance.
        The folder must exist in the database.
        Use 'updateSequenceData' method to supply data to the created sequence.
    */
    virtual void createSequenceObject(U2Sequence& sequence, const QString& folder, U2OpStatus& os, U2DbiObjectRank rank);

    /**
        Updates sequence object fields.
        Requires: U2DbiFeature_WriteSequence feature support.
    */
    virtual void updateSequenceObject(U2Sequence& sequence, U2OpStatus& os);

    /**
        Updates sequence region.
        The region must be valid region within sequence bounds.
        Note: regionToReplace length can differ from dataToInsert length, so the method can be used to add/remove sequence regions
    */
    virtual void updateSequenceData(const U2DataId &sequenceId, const U2Region &regionToReplace, const QByteArray &dataToInsert, const QVariantMap &hints, U2OpStatus &os);
    virtual void updateSequenceData(const U2DataId &masterId, const U2DataId &sequenceId,
                                    const U2Region &regionToReplace, const QByteArray &dataToInsert, const QVariantMap &hints, U2OpStatus &os);

    /** The same as above, except passed modification action is used */
    void updateSequenceData(MysqlModificationAction &updateAction, const U2DataId &sequenceId, const U2Region &regionToReplace, const QByteArray &dataToInsert, const QVariantMap &hints, U2OpStatus &os);

    /** Undo the operation for the sequence. */
    void undo(const U2DataId& msaId, qint64 modType, const QByteArray& modDetails, U2OpStatus& os);

    /** Redo the operation for the sequence. */
    void redo(const U2DataId& msaId, qint64 modType, const QByteArray& modDetails, U2OpStatus& os);

private:
    ///////////////////////////////////////////////////////////
    // Core methods
    void updateSequenceDataCore(const U2DataId& sequenceId, const U2Region& regionToReplace, const QByteArray& dataToInsert, const QVariantMap &hints, U2OpStatus& os);

    ///////////////////////////////////////////////////////////
    // Undo methods
    void undoUpdateSequenceData(const U2DataId& sequenceId, const QByteArray& modDetails, U2OpStatus& os);

    ///////////////////////////////////////////////////////////
    // Redo methods
    void redoUpdateSequenceData(const U2DataId& sequenceId, const QByteArray& modDetails, U2OpStatus& os);
};

}   // namespace U2

#endif // _U2_MYSQL_SEQUENCE_DBI_H_
