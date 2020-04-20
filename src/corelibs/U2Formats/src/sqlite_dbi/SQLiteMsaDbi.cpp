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

#include "SQLiteMsaDbi.h"
#include "SQLiteModDbi.h"
#include "SQLiteSequenceDbi.h"

#include <U2Core/U2DbiPackUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SqlHelpers.h>

namespace U2 {

SQLiteMsaDbi::SQLiteMsaDbi(SQLiteDbi* dbi) : U2MsaDbi(dbi), SQLiteChildDBICommon(dbi) {
}

void SQLiteMsaDbi::initSqlSchema(U2OpStatus& os) {
    if (os.hasError()) {
        return;
    }

    // MSA object
    SQLiteWriteQuery("CREATE TABLE Msa (object INTEGER PRIMARY KEY, length INTEGER NOT NULL, "
        "alphabet TEXT NOT NULL, numOfRows INTEGER NOT NULL, "
        "FOREIGN KEY(object) REFERENCES Object(id) ON DELETE CASCADE)", db, os).execute();

     // MSA object row
     //   msa      - msa object id
     //   rowId    - id of the row in the msa
     //   sequence - sequence object id
     //   pos      - positional number of a row in the msa (initially, equals 'id', but can be changed, e.g. in GUI by moving rows)
     //   gstart   - offset of the first element in the sequence
     //   gend     - offset of the last element in the sequence (non-inclusive)
     //   length   - sequence and gaps length (trailing gap are not taken into account)
    SQLiteWriteQuery("CREATE TABLE MsaRow (msa INTEGER NOT NULL, rowId INTEGER NOT NULL, sequence INTEGER NOT NULL,"
        " pos INTEGER NOT NULL, gstart INTEGER NOT NULL, gend INTEGER NOT NULL, length INTEGER NOT NULL,"
        " PRIMARY KEY(msa, rowId),"
        " FOREIGN KEY(msa) REFERENCES Msa(object) ON DELETE CASCADE, "
        " FOREIGN KEY(sequence) REFERENCES Sequence(object) ON DELETE CASCADE)", db, os).execute();
    SQLiteWriteQuery("CREATE INDEX MsaRow_msa_rowId ON MsaRow(msa, rowId)", db, os).execute();
    SQLiteWriteQuery("CREATE INDEX MsaRow_length ON MsaRow(length)", db, os).execute();
    SQLiteWriteQuery("CREATE INDEX MsaRow_sequence ON MsaRow(sequence)", db, os).execute();

     // Gap info for a MSA row:
     //   msa       - msa object id
     //   rowId     - id of the row in the msa
     //   gapStart  - start of the gap, the coordinate is relative to the gstart coordinate of the row
     //   gapEnd    - end of the gap, the coordinate is relative to the gstart coordinate of the row (non-inclusive)
     // Note! there is invariant: gend - gstart (of the row) == gapEnd - gapStart
    SQLiteWriteQuery("CREATE TABLE MsaRowGap (msa INTEGER NOT NULL, rowId INTEGER NOT NULL, "
        "gapStart INTEGER NOT NULL, gapEnd INTEGER NOT NULL, "
        "FOREIGN KEY(msa, rowId) REFERENCES MsaRow(msa, rowId) ON DELETE CASCADE)",
        db, os).execute();
    SQLiteWriteQuery("CREATE INDEX MsaRowGap_msa_rowId ON MsaRowGap(msa, rowId)", db, os).execute();
}

U2DataId SQLiteMsaDbi::createMsaObject(const QString& folder, const QString& name, const U2AlphabetId& alphabet, U2OpStatus& os) {
    return createMsaObject(folder, name, alphabet, 0, os);
}

U2DataId SQLiteMsaDbi::createMsaObject(const QString &folder, const QString &name, const U2AlphabetId &alphabet, int length, U2OpStatus &os) {
    SQLiteTransaction t(db, os);
    U2Msa msa;
    msa.visualName = name;
    msa.alphabet = alphabet;
    msa.length = length;

    // Create the object
    dbi->getSQLiteObjectDbi()->createObject(msa, folder, U2DbiObjectRank_TopLevel, os);
    CHECK_OP(os, U2DataId());

    // Create a record in the Msa table
    SQLiteWriteQuery q("INSERT INTO Msa(object, length, alphabet, numOfRows) VALUES(?1, ?2, ?3, ?4)", db, os);
    CHECK_OP(os, U2DataId());

    q.bindDataId(1, msa.id);
    q.bindInt64(2, msa.length);
    q.bindString(3, msa.alphabet.id);
    q.bindInt64(4, 0); // no rows
    q.insert();

    return msa.id;
}

void SQLiteMsaDbi::updateMsaName(const U2DataId& msaId, const QString& name, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    U2Object msaObj;
    dbi->getSQLiteObjectDbi()->getObject(msaObj, msaId, os);
    CHECK_OP(os, );

    SQLiteObjectDbiUtils::renameObject(dbi, msaObj, name, os);
}

void SQLiteMsaDbi::updateMsaAlphabet(const U2DataId& msaId, const U2AlphabetId& alphabet, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    SQLiteModificationAction updateAction(dbi, msaId);
    U2TrackModType trackMod = updateAction.prepare(os);
    CHECK_OP(os, );

    // Get modDetails, if required
    QByteArray modDetails;
    if (TrackOnUpdate == trackMod) {
        U2Msa msaObj = getMsaObject(msaId, os);
        CHECK_OP(os, );
        modDetails = U2DbiPackUtils::packAlphabetDetails(msaObj.alphabet, alphabet);
    }

    // Update the alphabet
    SQLiteWriteQuery q("UPDATE Msa SET alphabet = ?1 WHERE object = ?2", db, os);
    CHECK_OP(os, );

    q.bindString(1, alphabet.id);
    q.bindDataId(2, msaId);
    q.update(1);

    // Increment version; track the modification, if required
    updateAction.addModification(msaId, U2ModType::msaUpdatedAlphabet, modDetails, os);
    SAFE_POINT_OP(os, );

    updateAction.complete(os);
    SAFE_POINT_OP(os, );
}

void SQLiteMsaDbi::createMsaRow(const U2DataId& msaId, qint64 posInMsa, U2MsaRow& msaRow, U2OpStatus& os) {
    assert(posInMsa >= 0);

    // Calculate the row length
    qint64 rowLength = calculateRowLength(msaRow.gend - msaRow.gstart, msaRow.gaps);

    // Insert the data
    SQLiteWriteQuery q("INSERT INTO MsaRow(msa, rowId, sequence, pos, gstart, gend, length)"
        " VALUES(?1, ?2, ?3, ?4, ?5, ?6, ?7)", db, os);
    CHECK_OP(os, );

    q.bindDataId(1, msaId);
    q.bindInt64(2, msaRow.rowId);
    q.bindDataId(3, msaRow.sequenceId);
    q.bindInt64(4, posInMsa);
    q.bindInt64(5, msaRow.gstart);
    q.bindInt64(6, msaRow.gend);
    q.bindInt64(7, rowLength);
    q.insert();
}

void SQLiteMsaDbi::createMsaRowGap(const U2DataId& msaId, qint64 msaRowId, const U2MsaGap& msaGap, U2OpStatus& os) {
    SQLiteTransaction t(db, os);

    static const QString queryString("INSERT INTO MsaRowGap(msa, rowId, gapStart, gapEnd) VALUES(?1, ?2, ?3, ?4)");
    QSharedPointer<SQLiteQuery> q = t.getPreparedQuery(queryString, db, os);
    CHECK_OP(os, );

    q->bindDataId(1, msaId);
    q->bindInt64(2, msaRowId);
    q->bindInt64(3, msaGap.offset);
    q->bindInt64(4, msaGap.offset + msaGap.gap);
    q->insert();
}

void SQLiteMsaDbi::addMsaRowAndGaps(const U2DataId& msaId, qint64 posInMsa, U2MsaRow& row, U2OpStatus& os) {
    createMsaRow(msaId, posInMsa, row, os);
    CHECK_OP(os, );

    foreach (const U2MsaGap& gap, row.gaps) {
        createMsaRowGap(msaId, row.rowId, gap, os);
        CHECK_OP(os, );
    }

    dbi->getSQLiteObjectDbi()->setParent(msaId, row.sequenceId, os);
}

void SQLiteMsaDbi::addRow(const U2DataId& msaId, qint64 posInMsa, U2MsaRow& row, U2OpStatus& os) {
    SQLiteModificationAction updateAction(dbi, msaId);
    U2TrackModType trackMod = updateAction.prepare(os);
    CHECK_OP(os, );

    row.rowId = getMaximumRowId(msaId, os) + 1;
    CHECK_OP(os, );

    // TODO: allow core mothod to change posInMsa value to store the correct value on tracking
    addRowCore(msaId, posInMsa, row, os);
    CHECK_OP(os, );

    QByteArray modDetails;
    if (TrackOnUpdate == trackMod) {
        modDetails = U2DbiPackUtils::packRow(posInMsa, row);
    }
    if (row.length > getMsaLength(msaId, os)) {
        updateMsaLength(updateAction, msaId, row.length, os);
    }

    // Update track mod type for child sequence object
    if (TrackOnUpdate == trackMod) {
        dbi->getObjectDbi()->setTrackModType(row.sequenceId, TrackOnUpdate, os);
        CHECK_OP(os, );
    }

    // Increment version; track the modification, if required
    updateAction.addModification(msaId, U2ModType::msaAddedRow, modDetails, os);
    SAFE_POINT_OP(os, );

    updateAction.complete(os);
    SAFE_POINT_OP(os, );
}

void SQLiteMsaDbi::addRows(const U2DataId& msaId, QList<U2MsaRow>& rows, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    Q_UNUSED(t);
    SQLiteModificationAction updateAction(dbi, msaId);
    U2TrackModType trackMod = updateAction.prepare(os);
    CHECK_OP(os, );

    // Add the rows
    qint64 numOfRows = getNumOfRows(msaId, os);
    CHECK_OP(os, );

    QList<qint64> posInMsa;
    for (int i=0; i<rows.count(); i++) {
        posInMsa << i + numOfRows;
    }

    qint64 maxRowId = getMaximumRowId(msaId, os);
    for (int i = 0; i < rows.count(); ++i) {
        rows[i].rowId = maxRowId + i + 1;
    }

    QByteArray modDetails;
    if (TrackOnUpdate == trackMod) {
        modDetails = U2DbiPackUtils::packRows(posInMsa, rows);
    }

    addRowsCore(msaId, posInMsa, rows, os);
    CHECK_OP(os, );

    // Update msa length
    qint64 maxLength = 0;
    foreach (const U2MsaRow &row, rows) {
        maxLength = qMax(maxLength, row.length);
    }
    qint64 currentMsaLength = getMsaLength(msaId, os);
    if (maxLength > currentMsaLength) {
        updateMsaLength(updateAction, msaId, maxLength, os);
        CHECK_OP(os, );
    }

    // Update track mod type for child sequence object
    if (TrackOnUpdate == trackMod) {
        foreach (const U2MsaRow& row, rows) {
            dbi->getObjectDbi()->setTrackModType(row.sequenceId, TrackOnUpdate, os);
            CHECK_OP(os, );
        }
    }

    // Increment version; track the modification, if required
    updateAction.addModification(msaId, U2ModType::msaAddedRows, modDetails, os);
    SAFE_POINT_OP(os, );

    updateAction.complete(os);
    SAFE_POINT_OP(os, );
}

void SQLiteMsaDbi::updateRowName(const U2DataId& msaId, qint64 rowId, const QString& newName, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    Q_UNUSED(t);
    SQLiteModificationAction updateAction(dbi, msaId);
    updateAction.prepare(os);
    SAFE_POINT_OP(os, );

    U2DataId sequenceId = getSequenceIdByRowId(msaId, rowId, os);
    SAFE_POINT_OP(os, );

    U2Sequence seqObject = dbi->getSequenceDbi()->getSequenceObject(sequenceId, os);
    SAFE_POINT_OP(os, );

    SQLiteObjectDbiUtils::renameObject(updateAction, dbi, seqObject, newName, os);
    SAFE_POINT_OP(os, );

    updateAction.complete(os);
    SAFE_POINT_OP(os, );
}

void SQLiteMsaDbi::updateRowContent(const U2DataId& msaId, qint64 rowId, const QByteArray& seqBytes, const QList<U2MsaGap>& gaps, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    Q_UNUSED(t);

    SQLiteModificationAction updateAction(dbi, msaId);
    U2TrackModType trackMod = updateAction.prepare(os);
    SAFE_POINT_OP(os, );
    Q_UNUSED(trackMod);

    // Get the row object
    U2MsaRow row = getRow(msaId, rowId, os);
    SAFE_POINT_OP(os, );

    // Update the sequence data
    QVariantMap hints;
    dbi->getSQLiteSequenceDbi()->updateSequenceData(updateAction,
        row.sequenceId, U2_REGION_MAX, seqBytes, hints, os);
    SAFE_POINT_OP(os, );

    // Update the row object
    U2MsaRow newRow(row);
    qint64 seqLength = seqBytes.length();
    newRow.gstart = 0;
    newRow.gend = seqLength;
    newRow.length = calculateRowLength(seqLength, gaps);
    updateRowInfo(updateAction, msaId, newRow, os);
    SAFE_POINT_OP(os, );

    // Update the gap model
    // WARNING: this update must go after the row info update to recalculate the msa length properly
    updateGapModel(updateAction, msaId, rowId, gaps, os);
    SAFE_POINT_OP(os, );

    // Save tracks, if required; increment versions
    updateAction.complete(os);
    SAFE_POINT_OP(os, );
}

void SQLiteMsaDbi::updateRowInfo(SQLiteModificationAction &updateAction, const U2DataId &msaId, const U2MsaRow &row, U2OpStatus &os) {
    QByteArray modDetails;
    if (TrackOnUpdate == updateAction.getTrackModType()) {
        U2MsaRow oldRow = getRow(msaId, row.rowId, os);
        SAFE_POINT_OP(os, );

        modDetails = U2DbiPackUtils::packRowInfoDetails(oldRow, row);
    }

    updateRowInfoCore(msaId, row, os);
    SAFE_POINT_OP(os, );

    // Track the modification, if required; add the object to the list (versions of the objects will be incremented on the updateAction completion)
    updateAction.addModification(msaId, U2ModType::msaUpdatedRowInfo, modDetails, os);
    SAFE_POINT_OP(os, );
}

QList<qint64> SQLiteMsaDbi::getRowsOrder(const U2DataId& msaId, U2OpStatus& os) {
    QList<qint64> res;
    SQLiteReadQuery q("SELECT rowId FROM MsaRow WHERE msa = ?1 ORDER BY pos", db, os);
    q.bindDataId(1, msaId);
    qint64 rowId;
    while (q.step()) {
        rowId = q.getInt64(0);
        res.append(rowId);
    }
    return res;
}

U2AlphabetId SQLiteMsaDbi::getMsaAlphabet(const U2DataId& msaId, U2OpStatus& os) {
    QString alphabetName;
    SQLiteReadQuery q("SELECT alphabet FROM Msa WHERE object = ?1", db, os);
    q.bindDataId(1, msaId);
    if (q.step()) {
        alphabetName = q.getString(0);
        q.ensureDone();
    } else if (!os.hasError()) {
        os.setError(U2DbiL10n::tr("Msa object not found"));
    }

    return U2AlphabetId(alphabetName);
}

void SQLiteMsaDbi::setNewRowsOrder(const U2DataId& msaId, const QList<qint64>& rowIds, U2OpStatus& os) {
    // Init track info
    SQLiteTransaction t(db, os);
    SQLiteModificationAction updateAction(dbi, msaId);
    U2TrackModType trackMod = updateAction.prepare(os);
    CHECK_OP(os, );

    QByteArray modDetails;
    if (TrackOnUpdate == trackMod) {
        QList<qint64> oldOrder = getRowsOrder(msaId, os);
        CHECK_OP(os, );
        modDetails = U2DbiPackUtils::packRowOrderDetails(oldOrder, rowIds);
    }

    // Check that row IDs number is correct (if required, can be later removed for efficiency)
    qint64 numOfRows = getNumOfRows(msaId, os);
    CHECK_OP(os, );
    SAFE_POINT(numOfRows == rowIds.count(), "Incorrect number of row IDs!", );

    // Set the new order
    setNewRowsOrderCore(msaId, rowIds, os);
    CHECK_OP(os, );

    // Increment version; track the modification, if required
    updateAction.addModification(msaId, U2ModType::msaSetNewRowsOrder, modDetails, os);
    SAFE_POINT_OP(os, );

    updateAction.complete(os);
    SAFE_POINT_OP(os, );
}

void SQLiteMsaDbi::removeRecordFromMsaRow(const U2DataId& msaId, qint64 rowId, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    static const QString queryString("DELETE FROM MsaRow WHERE msa = ?1 AND rowId = ?2");
    QSharedPointer<SQLiteQuery> q = t.getPreparedQuery(queryString, db, os);
    CHECK_OP(os, );

    q->bindDataId(1, msaId);
    q->bindInt64(2, rowId);
    q->update(1);
}

void SQLiteMsaDbi::removeRecordsFromMsaRowGap(const U2DataId& msaId, qint64 rowId, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    static const QString queryString("DELETE FROM MsaRowGap WHERE msa = ?1 AND rowId = ?2");
    QSharedPointer<SQLiteQuery> q = t.getPreparedQuery(queryString, db, os);
    CHECK_OP(os, );

    q->bindDataId(1, msaId);
    q->bindInt64(2, rowId);
    q->update();
}

void SQLiteMsaDbi::removeRow(const U2DataId& msaId, qint64 rowId, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    SQLiteModificationAction updateAction(dbi, msaId);
    U2TrackModType trackMod = updateAction.prepare(os);
    CHECK_OP(os, );

    QByteArray modDetails;
    if (TrackOnUpdate == trackMod) {
        U2MsaRow removedRow = getRow(msaId, rowId, os);
        CHECK_OP(os, );
        qint64 posInMsa = getPosInMsa(msaId, rowId, os);
        CHECK_OP(os, );
        modDetails = U2DbiPackUtils::packRow(posInMsa, removedRow);
    }

    bool removeSequence = (TrackOnUpdate != trackMod);
    removeRowCore(msaId, rowId, removeSequence, os);
    CHECK_OP(os, );

    // Increment version; track the modification, if required
    updateAction.addModification(msaId, U2ModType::msaRemovedRow, modDetails, os);
    SAFE_POINT_OP(os, );

    updateAction.complete(os);
    SAFE_POINT_OP(os, );
}

void SQLiteMsaDbi::removeRows(const U2DataId& msaId, const QList<qint64>& rowIds, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    Q_UNUSED(t);
    SQLiteModificationAction updateAction(dbi, msaId);
    U2TrackModType trackMod = updateAction.prepare(os);
    CHECK_OP(os, );

    QByteArray modDetails;
    int numOfRows = getNumOfRows(msaId, os);
    if (TrackOnUpdate == trackMod) {
        QList<qint64> posInMsa;
        QList<U2MsaRow> rows;
        foreach (qint64 rowId, rowIds) {
            posInMsa << getPosInMsa(msaId, rowId, os);
            CHECK_OP(os, );
            rows << getRow(msaId, rowId, os);
            CHECK_OP(os, );
        }
        modDetails = U2DbiPackUtils::packRows(posInMsa, rows);
    }

    bool removeSequence = (TrackOnUpdate != trackMod);
    removeRowsCore(msaId, rowIds, removeSequence, os);

    if (numOfRows == rowIds.count()) {
        updateMsaLength(updateAction, msaId, 0, os);
    }

    // Increment version; track the modification, if required
    updateAction.addModification(msaId, U2ModType::msaRemovedRows, modDetails, os);
    SAFE_POINT_OP(os, );

    updateAction.complete(os);
    SAFE_POINT_OP(os, );
}

void SQLiteMsaDbi::removeMsaRowAndGaps(const U2DataId& msaId, qint64 rowId, bool removeSequence, U2OpStatus& os) {
    U2DataId sequenceId = getSequenceIdByRowId(msaId, rowId, os);
    CHECK_OP(os, );

    removeRecordsFromMsaRowGap(msaId, rowId, os);
    removeRecordFromMsaRow(msaId, rowId, os);

    dbi->getSQLiteObjectDbi()->removeParent(msaId, sequenceId, removeSequence, os);
}

void SQLiteMsaDbi::deleteRowsData(const U2DataId& msaId, U2OpStatus& os) {
    static const QString deleteObjStr = "DELETE FROM Object WHERE id IN (SELECT sequence FROM MsaRow WHERE msa = ?1)";
    SQLiteWriteQuery deleteObjQeury(deleteObjStr, db, os);
    deleteObjQeury.bindDataId(1, msaId);
    deleteObjQeury.execute();
}

U2Msa SQLiteMsaDbi::getMsaObject(const U2DataId& msaId, U2OpStatus& os) {
    U2Msa res;
    dbi->getSQLiteObjectDbi()->getObject(res, msaId, os);

    SAFE_POINT_OP(os, res);

    SQLiteReadQuery q("SELECT length, alphabet FROM Msa WHERE object = ?1", db, os);
    q.bindDataId(1, msaId);
    if (q.step())  {
        res.length = q.getInt64(0);
        res.alphabet = q.getString(1);
        q.ensureDone();
    } else if (!os.hasError()) {
        os.setError(U2DbiL10n::tr("Msa object not found!"));
    }
    return res;
}

qint64 SQLiteMsaDbi::getNumOfRows(const U2DataId& msaId, U2OpStatus& os) {
    qint64 res = 0;
    SQLiteReadQuery q("SELECT numOfRows FROM Msa WHERE object = ?1", db, os);
    CHECK_OP(os, res);

    q.bindDataId(1, msaId);
    if (q.step())  {
        res = q.getInt64(0);
        q.ensureDone();
    } else if (!os.hasError()) {
        os.setError(U2DbiL10n::tr("Msa object not found!"));
    }
    return res;
}

void SQLiteMsaDbi::recalculateRowsPositions(const U2DataId& msaId, U2OpStatus& os) {
    QList<U2MsaRow> rows = getRows(msaId, os);
    CHECK_OP(os, );

    SQLiteTransaction t(db, os);
    SQLiteWriteQuery q("UPDATE MsaRow SET pos = ?1 WHERE msa = ?2 AND rowId = ?3", db, os);
    CHECK_OP(os, );

    for (int i = 0, n = rows.count(); i < n; ++i) {
        qint64 rowId = rows[i].rowId;
        q.reset();
        q.bindInt64(1, i);
        q.bindDataId(2, msaId);
        q.bindInt64(3, rowId);
        q.execute();
    }
}

qint64 SQLiteMsaDbi::getMaximumRowId(const U2DataId& msaId, U2OpStatus& os) {
    qint64 maxRowId = 0;
    SQLiteReadQuery q("SELECT MAX(rowId) FROM MsaRow WHERE msa = ?1", db, os);
    SAFE_POINT_OP(os, 0);

    q.bindDataId(1, msaId);
    q.getInt64(1);
    if (q.step()) {
        maxRowId = q.getInt64(0);
    }

    return maxRowId;
}

QList<U2MsaRow> SQLiteMsaDbi::getRows(const U2DataId& msaId, U2OpStatus& os) {
    QList<U2MsaRow> res;
    SQLiteReadQuery q("SELECT rowId, sequence, gstart, gend, length FROM MsaRow WHERE msa = ?1 ORDER BY pos", db, os);
    q.bindDataId(1, msaId);

    SQLiteReadQuery gapQ("SELECT gapStart, gapEnd FROM MsaRowGap WHERE msa = ?1 AND rowId = ?2 ORDER BY gapStart", db, os);
    while (q.step()) {
        U2MsaRow row;
        row.rowId = q.getInt64(0);
        row.sequenceId = q.getDataId(1, U2Type::Sequence);
        row.gstart = q.getInt64(2);
        row.gend = q.getInt64(3);
        row.length = q.getInt64(4);

        gapQ.reset();
        gapQ.bindDataId(1, msaId);
        gapQ.bindInt64(2, row.rowId);
        while (gapQ.step()) {
            U2MsaGap gap;
            gap.offset = gapQ.getInt64(0);
            gap.gap = gapQ.getInt64(1) - gap.offset;
            SAFE_POINT_EXT(gap.isValid(), os.setError("An invalid gap is stored in the database"), res);
            row.gaps.append(gap);
        }

        SAFE_POINT_OP(os, res);
        res.append(row);
    }
    return res;
}

U2MsaRow SQLiteMsaDbi::getRow(const U2DataId& msaId, qint64 rowId, U2OpStatus& os) {
    U2MsaRow res;
    SQLiteReadQuery q("SELECT sequence, gstart, gend, length FROM MsaRow WHERE msa = ?1 AND rowId = ?2", db, os);
    SAFE_POINT_OP(os, res);

    q.bindDataId(1, msaId);
    q.bindInt64(2, rowId);
    if (q.step()) {
        res.rowId = rowId;
        res.sequenceId = q.getDataId(0, U2Type::Sequence);
        res.gstart = q.getInt64(1);
        res.gend = q.getInt64(2);
        res.length = q.getInt64(3);
        q.ensureDone();
    } else if (!os.hasError()) {
        os.setError(U2DbiL10n::tr("Msa row not found!"));
        SAFE_POINT_OP(os, res);
    }

    SQLiteReadQuery gapQ("SELECT gapStart, gapEnd FROM MsaRowGap WHERE msa = ?1 AND rowId = ?2 ORDER BY gapStart", db, os);
    SAFE_POINT_OP(os, res);

    gapQ.bindDataId(1, msaId);
    gapQ.bindInt64(2, rowId);
    while (gapQ.step()) {
        U2MsaGap gap;
        gap.offset = gapQ.getInt64(0);
        gap.gap = gapQ.getInt64(1) - gap.offset;
        res.gaps.append(gap);
    }

    return res;
}

void SQLiteMsaDbi::updateNumOfRows(const U2DataId& msaId, qint64 numOfRows, U2OpStatus& os) {
    SQLiteWriteQuery q("UPDATE Msa SET numOfRows = ?1 WHERE object = ?2", db, os);
    SAFE_POINT_OP(os, );

    q.bindInt64(1, numOfRows);
    q.bindDataId(2, msaId);
    q.update(1);
}

void SQLiteMsaDbi::updateGapModel(const U2DataId& msaId, qint64 msaRowId, const QList<U2MsaGap>& gapModel, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    Q_UNUSED(t);

    SQLiteModificationAction updateAction(dbi, msaId);
    updateAction.prepare(os);
    SAFE_POINT_OP(os, );

    updateGapModel(updateAction, msaId, msaRowId, gapModel, os);
    SAFE_POINT_OP(os, );

    updateAction.complete(os);
    SAFE_POINT_OP(os, );
}

void SQLiteMsaDbi::updateGapModel(SQLiteModificationAction &updateAction, const U2DataId& msaId, qint64 msaRowId, const QList<U2MsaGap>& gapModel, U2OpStatus& os) {
    QByteArray gapsDetails;
    if (TrackOnUpdate == updateAction.getTrackModType()) {
        U2MsaRow row = getRow(msaId, msaRowId, os);
        SAFE_POINT_OP(os, );
        gapsDetails = U2DbiPackUtils::packGapDetails(msaRowId, row.gaps, gapModel);
    }

    updateGapModelCore(msaId, msaRowId, gapModel, os);
    SAFE_POINT_OP(os, );

    qint64 len = 0;
    foreach(const U2MsaGap& gap, gapModel) {
        len += gap.gap;
    }
    len += getRowSequenceLength(msaId, msaRowId, os);
    SAFE_POINT_OP(os, );
    if (len > getMsaLength(msaId, os)) {
        updateMsaLength(updateAction, msaId, len, os);
    }
    SAFE_POINT_OP(os, );

    // Track the modification, if required; add the object to the list (versions of the objects will be incremented on the updateAction completion)
    updateAction.addModification(msaId, U2ModType::msaUpdatedGapModel, gapsDetails, os);
    SAFE_POINT_OP(os, );
}

void SQLiteMsaDbi::updateMsaLength(const U2DataId& msaId, qint64 length, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    Q_UNUSED(t);

    SQLiteModificationAction updateAction(dbi, msaId);
    updateAction.prepare(os);
    SAFE_POINT_OP(os, );

    updateMsaLength(updateAction, msaId, length, os);

    updateAction.complete(os);
    SAFE_POINT_OP(os, );
}

void SQLiteMsaDbi::updateMsaLength(SQLiteModificationAction &updateAction, const U2DataId &msaId, qint64 length, U2OpStatus &os) {
    QByteArray modDetails;
    if (TrackOnUpdate == updateAction.getTrackModType()) {
        qint64 oldMsaLen = getMsaLength(msaId, os);
        CHECK_OP(os, );
        modDetails = U2DbiPackUtils::packAlignmentLength(oldMsaLen, length);
    }

    updateMsaLengthCore(msaId, length, os);
    SAFE_POINT_OP(os, )

    updateAction.addModification(msaId, U2ModType::msaLengthChanged, modDetails, os);
    SAFE_POINT_OP(os, );
}

qint64 SQLiteMsaDbi::getMsaLength(const U2DataId& msaId, U2OpStatus& os) {
    qint64 res = 0;
    SQLiteReadQuery q("SELECT length FROM Msa WHERE object = ?1", db, os);
    CHECK_OP(os, res);

    q.bindDataId(1, msaId);
    if (q.step()) {
        res = q.getInt64(0);
        q.ensureDone();
    }
    else if (!os.hasError()) {
        os.setError(U2DbiL10n::tr("Msa object not found!"));
    }

    return res;
}

U2DataId SQLiteMsaDbi::createMcaObject(const QString &folder, const QString &name, const U2AlphabetId &alphabet, U2OpStatus &os) {
    return createMcaObject(folder, name, alphabet, 0, os);
}

U2DataId SQLiteMsaDbi::createMcaObject(const QString &folder, const QString &name, const U2AlphabetId &alphabet, int length, U2OpStatus &os) {
    SQLiteTransaction t(db, os);
    Q_UNUSED(t);

    U2Mca mca;
    mca.visualName = name;
    mca.alphabet = alphabet;
    mca.length = length;

    // Create the object
    dbi->getSQLiteObjectDbi()->createObject(mca, folder, U2DbiObjectRank_TopLevel, os);
    CHECK_OP(os, U2DataId());

    // Create a record in the Msa table
    SQLiteWriteQuery q("INSERT INTO Msa(object, length, alphabet, numOfRows) VALUES(?1, ?2, ?3, ?4)", db, os);
    CHECK_OP(os, U2DataId());

    q.bindDataId(1, mca.id);
    q.bindInt64(2, mca.length);
    q.bindString(3, mca.alphabet.id);
    q.bindInt64(4, 0); // no rows
    q.insert();

    return mca.id;
}

qint64 SQLiteMsaDbi::calculateRowLength(qint64 seqLength, const QList<U2MsaGap>& gaps) {
    qint64 res = seqLength;
    foreach (const U2MsaGap& gap, gaps) {
        if (gap.offset < res) { // ignore trailing gaps
            res += gap.gap;
        }
    }
    return res;
}

qint64 SQLiteMsaDbi::getRowSequenceLength(const U2DataId& msaId, qint64 rowId, U2OpStatus& os) {
    qint64 res = 0;
    SQLiteReadQuery q("SELECT gstart, gend FROM MsaRow WHERE msa = ?1 AND rowId = ?2", db, os);
    CHECK_OP(os, res);

    q.bindDataId(1, msaId);
    q.bindInt64(2, rowId);
    if (q.step()) {
        qint64 startInSeq = q.getInt64(0);
        qint64 endInSeq = q.getInt64(1);
        res = endInSeq - startInSeq;
        q.ensureDone();
    }
    else if (!os.hasError()) {
        os.setError(U2DbiL10n::tr("Msa row not found!"));
    }

    return res;
}

void SQLiteMsaDbi::updateRowLength(const U2DataId& msaId, qint64 rowId, qint64 newLength, U2OpStatus& os) {
    SQLiteWriteQuery q("UPDATE MsaRow SET length = ?1 WHERE msa = ?2 AND rowId = ?3", db, os);
    CHECK_OP(os, );

    q.bindInt64(1, newLength);
    q.bindDataId(2, msaId);
    q.bindInt64(3, rowId);
    q.update(1);
}

void SQLiteMsaDbi::updateMsaLengthCore(const U2DataId &msaId, qint64 length, U2OpStatus &os) {
    SQLiteTransaction t(db, os);
    SQLiteWriteQuery q("UPDATE Msa SET length = ?1 WHERE object = ?2", db, os);
    CHECK_OP(os, );

    q.bindInt64(1, length);
    q.bindDataId(2, msaId);
    q.execute();
}

U2DataId SQLiteMsaDbi::getSequenceIdByRowId(const U2DataId& msaId, qint64 rowId, U2OpStatus& os) {
    U2DataId res;
    SQLiteReadQuery q("SELECT sequence FROM MsaRow WHERE msa = ?1 AND rowId = ?2", db, os);
    CHECK_OP(os, res);

    q.bindDataId(1, msaId);
    q.bindInt64(2, rowId);
    if (q.step()) {
        res = q.getDataId(0, U2Type::Sequence);
        q.ensureDone();
    }
    else if (!os.hasError()) {
        os.setError(U2DbiL10n::tr("Msa row not found!"));
    }

    return res;
}

QByteArray SQLiteMsaDbi::getRemovedRowDetails(const U2MsaRow& row) {
    QByteArray res;

    // Info about gaps
    QByteArray gapsInfo;
    for (int i = 0, n = row.gaps.count(); i < n; ++i) {
        const U2MsaGap& gap = row.gaps[i];
        gapsInfo += "offset=";
        gapsInfo += QByteArray::number(gap.offset);
        gapsInfo += "&gap=";
        gapsInfo += QByteArray::number(gap.gap);

        if (i > 0 && i < n - 1) {
            gapsInfo += "&";
        }
    }

    res = QByteArray("rowId=") + QByteArray::number(row.rowId) +
        QByteArray("&sequenceId=") + row.sequenceId.toHex() +
        QByteArray("&gstart=") + QByteArray::number(row.gstart) +
        QByteArray("&gend=") + QByteArray::number(row.gend) +
        QByteArray("&gaps=\"") + gapsInfo + QByteArray("\"") +
        QByteArray("&length=") + QByteArray::number(row.length);

    return res;
}

qint64 SQLiteMsaDbi::getPosInMsa(const U2DataId &msaId, qint64 rowId, U2OpStatus &os) {
    SQLiteReadQuery q("SELECT pos FROM MsaRow WHERE msa = ?1 AND rowId = ?2", db, os);
    CHECK_OP(os, -1);
    q.bindDataId(1, msaId);
    q.bindInt64(2, rowId);
    if (q.step()) {
        qint64 result = q.getInt64(0);
        q.ensureDone();
        return result;
    }
    os.setError(QString("No row with id '%1' in msa '%2'").arg(QString::number(rowId)).arg(msaId.data()));
    return -1;
}

void SQLiteMsaDbi::undo(const U2DataId& msaId, qint64 modType, const QByteArray& modDetails, U2OpStatus& os) {
    if (U2ModType::msaUpdatedAlphabet == modType) {
        undoUpdateMsaAlphabet(msaId, modDetails, os);
    }
    else if (U2ModType::msaAddedRows == modType) {
        undoAddRows(msaId, modDetails, os);
    }
    else if (U2ModType::msaAddedRow == modType) {
        undoAddRow(msaId, modDetails, os);
    }
    else if (U2ModType::msaRemovedRows == modType) {
        undoRemoveRows(msaId, modDetails, os);
    }
    else if (U2ModType::msaRemovedRow == modType) {
        undoRemoveRow(msaId, modDetails, os);
    }
    else if (U2ModType::msaUpdatedRowInfo == modType) {
        undoUpdateRowInfo(msaId, modDetails, os);
    }
    else if (U2ModType::msaUpdatedGapModel == modType) {
        undoUpdateGapModel(msaId, modDetails, os);
    }
    else if (U2ModType::msaSetNewRowsOrder == modType) {
        undoSetNewRowsOrder(msaId, modDetails, os);
    } else if (U2ModType::msaLengthChanged == modType) {
        undoMsaLengthChange(msaId, modDetails, os);
    }
    else {
        os.setError(QString("Unexpected modification type '%1'!").arg(QString::number(modType)));
        return;
    }
}

void SQLiteMsaDbi::redo(const U2DataId& msaId, qint64 modType, const QByteArray& modDetails, U2OpStatus& os) {
    if (U2ModType::msaUpdatedAlphabet == modType) {
        redoUpdateMsaAlphabet(msaId, modDetails, os);
    }
    else if (U2ModType::msaAddedRows == modType) {
        redoAddRows(msaId, modDetails, os);
    }
    else if (U2ModType::msaAddedRow == modType) {
        redoAddRow(msaId, modDetails, os);
    }
    else if (U2ModType::msaRemovedRows == modType) {
        redoRemoveRows(msaId, modDetails, os);
    }
    else if (U2ModType::msaRemovedRow == modType) {
        redoRemoveRow(msaId, modDetails, os);
    }
    else if (U2ModType::msaUpdatedRowInfo == modType) {
        redoUpdateRowInfo(msaId, modDetails, os);
    }
    else if (U2ModType::msaUpdatedGapModel == modType) {
        redoUpdateGapModel(msaId, modDetails, os);
    }
    else if (U2ModType::msaSetNewRowsOrder == modType) {
        redoSetNewRowsOrder(msaId, modDetails, os);
    } else if (U2ModType::msaLengthChanged == modType) {
        redoMsaLengthChange(msaId, modDetails, os);
    }
    else {
        os.setError(QString("Unexpected modification type '%1'!").arg(QString::number(modType)));
        return;
    }
}

/************************************************************************/
/* Core methods                                                         */
/************************************************************************/
void SQLiteMsaDbi::updateGapModelCore(const U2DataId &msaId, qint64 msaRowId, const QList<U2MsaGap> &gapModel, U2OpStatus &os) {
    SQLiteTransaction t(db, os);
    // Remove obsolete gaps of the row
    removeRecordsFromMsaRowGap(msaId, msaRowId, os);
    CHECK_OP(os, );

    // Store the new gap model
    foreach (const U2MsaGap& gap, gapModel) {
        createMsaRowGap(msaId, msaRowId, gap, os);
        CHECK_OP(os, );
    }

    // Update the row length (without trailing gaps)
    qint64 rowSequenceLength = getRowSequenceLength(msaId, msaRowId, os);
    CHECK_OP(os, );

    qint64 newRowLength = calculateRowLength(rowSequenceLength, gapModel);
    updateRowLength(msaId, msaRowId, newRowLength, os);
    CHECK_OP(os, );
}

void SQLiteMsaDbi::addRowSubcore(const U2DataId &msaId, qint64 numOfRows, const QList<qint64> &rowsOrder, U2OpStatus &os) {
    // Re-calculate position, if needed
    setNewRowsOrderCore(msaId, rowsOrder, os);
    CHECK_OP(os, );

    // Update the number of rows of the MSA
    updateNumOfRows(msaId, numOfRows, os);
}

void SQLiteMsaDbi::addRowCore(const U2DataId& msaId, qint64 posInMsa, U2MsaRow& row, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    Q_UNUSED(t);

    // Append the row to the end, if "-1"
    qint64 numOfRows = getNumOfRows(msaId, os);
    CHECK_OP(os, );

    QList<qint64> rowsOrder;
    if (-1 == posInMsa) {
        posInMsa = numOfRows;
    }
    else {
        rowsOrder = getRowsOrder(msaId, os);
        CHECK_OP(os, );
        SAFE_POINT(rowsOrder.count() == numOfRows, "Incorrect number of rows!", );
    }
    SAFE_POINT(posInMsa >= 0 && posInMsa <= numOfRows, "Incorrect input position!", );

    // Create the row
    addMsaRowAndGaps(msaId, posInMsa, row, os);
    CHECK_OP(os, );

    // Update the alignment length
    row.length = calculateRowLength(row.gend - row.gstart, row.gaps);
    if (posInMsa != numOfRows) {
        rowsOrder.insert(posInMsa, row.rowId);
    }
    addRowSubcore(msaId, numOfRows+1, rowsOrder, os);
}

void SQLiteMsaDbi::addRowsCore(const U2DataId &msaId, const QList<qint64> &posInMsa, QList<U2MsaRow> &rows, U2OpStatus &os) {
    SQLiteTransaction t(db, os);
    Q_UNUSED(t);
    qint64 numOfRows = getNumOfRows(msaId, os);
    CHECK_OP(os, );

    QList<qint64> rowsOrder = getRowsOrder(msaId, os);
    CHECK_OP(os, );
    SAFE_POINT(rowsOrder.count() == numOfRows, "Incorrect number of rows!", );

    // Add new rows
    QList<qint64>::ConstIterator pi = posInMsa.begin();
    QList<U2MsaRow>::Iterator ri = rows.begin();
    for (; ri != rows.end(); ri++, pi++) {
        qint64 pos = *pi;
        if (-1 == pos) {
            pos = numOfRows;
        }
        SAFE_POINT(pos >= 0 && pos <= numOfRows, "Incorrect input position!", );
        addMsaRowAndGaps(msaId, pos, *ri, os);
        CHECK_OP(os, );
        ri->length = calculateRowLength(ri->gend - ri->gstart, ri->gaps);
        numOfRows++;
        rowsOrder.insert(pos, ri->rowId);
    }

    addRowSubcore(msaId, numOfRows, rowsOrder, os);
}

void SQLiteMsaDbi::removeRowSubcore(const U2DataId &msaId, qint64 numOfRows, U2OpStatus &os) {
    // Update the number of rows
    updateNumOfRows(msaId, numOfRows, os);
    CHECK_OP(os, );

    // Re-calculate the rows positions
    recalculateRowsPositions(msaId, os);
}

void SQLiteMsaDbi::removeRowCore(const U2DataId& msaId, qint64 rowId, bool removeSequence, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    // Get and verify the number of rows
    qint64 numOfRows = getNumOfRows(msaId, os);
    CHECK_OP(os, );
    SAFE_POINT(numOfRows > 0, "Empty MSA!", );

    // Remove the row
    removeMsaRowAndGaps(msaId, rowId, removeSequence, os);
    CHECK_OP(os, );

    removeRowSubcore(msaId, numOfRows-1, os);
}

void SQLiteMsaDbi::removeRowsCore(const U2DataId &msaId, const QList<qint64> &rowIds, bool removeSequence, U2OpStatus &os) {
    SQLiteTransaction t(db, os);
    qint64 numOfRows = getNumOfRows(msaId, os);
    CHECK_OP(os, );
    SAFE_POINT(numOfRows >= rowIds.count() , "Incorrect rows to remove!", );

    for (int i = 0; i < rowIds.count(); ++i) {
        removeMsaRowAndGaps(msaId, rowIds[i], removeSequence, os);
        CHECK_OP(os, );
    }

    removeRowSubcore(msaId, numOfRows-rowIds.size(), os);
}

void SQLiteMsaDbi::setNewRowsOrderCore(const U2DataId &msaId, const QList<qint64> rowIds, U2OpStatus &os) {
    SQLiteTransaction t(db, os);
    SQLiteWriteQuery q("UPDATE MsaRow SET pos = ?1 WHERE msa = ?2 AND rowId = ?3", db, os);
    CHECK_OP(os, );

    for (int i = 0, n = rowIds.count(); i < n; ++i) {
        qint64 rowId = rowIds[i];
        q.reset();
        q.bindInt64(1, i);
        q.bindDataId(2, msaId);
        q.bindInt64(3, rowId);
        q.execute();
    }
}

void SQLiteMsaDbi::updateRowInfoCore(const U2DataId& msaId, const U2MsaRow& row, U2OpStatus& os) {
    SQLiteWriteQuery q("UPDATE MsaRow SET sequence = ?1, gstart = ?2, gend = ?3 WHERE msa = ?4 AND rowId = ?5", db, os);
    SAFE_POINT_OP(os, );

    q.bindDataId(1, row.sequenceId);
    q.bindInt64(2, row.gstart);
    q.bindInt64(3, row.gend);
    q.bindDataId(4, msaId);
    q.bindInt64(5, row.rowId);
    q.update(1);
}

/************************************************************************/
/* Undo/redo methods */
/************************************************************************/
void SQLiteMsaDbi::undoUpdateMsaAlphabet(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    U2AlphabetId oldAlphabet;
    U2AlphabetId newAlphabet;
    bool ok = U2DbiPackUtils::unpackAlphabetDetails(modDetails, oldAlphabet, newAlphabet);
    if (!ok) {
        os.setError("An error occurred during updating an alignment alphabet!");
        return;
    }

    // Update the value
    SQLiteWriteQuery q("UPDATE Msa SET alphabet = ?1 WHERE object = ?2", db, os);
    CHECK_OP(os, );

    q.bindString(1, oldAlphabet.id);
    q.bindDataId(2, msaId);
    q.update(1);
}

void SQLiteMsaDbi::redoUpdateMsaAlphabet(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    U2AlphabetId oldAlphabet;
    U2AlphabetId newAlphabet;
    bool ok = U2DbiPackUtils::unpackAlphabetDetails(modDetails, oldAlphabet, newAlphabet);
    if (!ok) {
        os.setError("An error occurred during updating an alignment alphabet!");
        return;
    }

    // Redo the updating
    SQLiteWriteQuery q("UPDATE Msa SET alphabet = ?1 WHERE object = ?2", db, os);
    CHECK_OP(os, );

    q.bindString(1, newAlphabet.id);
    q.bindDataId(2, msaId);
    q.update(1);
}

void SQLiteMsaDbi::undoAddRows(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    QList<qint64> posInMsa;
    QList<U2MsaRow> rows;
    bool ok = U2DbiPackUtils::unpackRows(modDetails, posInMsa, rows);
    if (!ok) {
        os.setError("An error occurred during reverting adding of rows!");
        return;
    }
    QList<qint64> rowIds;
    foreach (const U2MsaRow &row, rows) {
        rowIds << row.rowId;
    }
    removeRowsCore(msaId, rowIds, false, os);
}

void SQLiteMsaDbi::redoAddRows(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    QList<qint64> posInMsa;
    QList<U2MsaRow> rows;
    bool ok = U2DbiPackUtils::unpackRows(modDetails, posInMsa, rows);
    if (!ok) {
        os.setError("An error occurred during reverting adding of rows!");
        return;
    }

    addRowsCore(msaId, posInMsa, rows, os);
}

void SQLiteMsaDbi::undoAddRow(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    U2MsaRow row;
    qint64 posInMsa;
    bool ok = U2DbiPackUtils::unpackRow(modDetails, posInMsa, row);
    if (!ok) {
        os.setError("An error occurred during reverting addition of a row!");
        return;
    }

    removeRowCore(msaId, row.rowId, false, os);
}

void SQLiteMsaDbi::redoAddRow(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    U2MsaRow row;
    qint64 posInMsa;
    bool ok = U2DbiPackUtils::unpackRow(modDetails, posInMsa, row);
    if (!ok) {
        os.setError("An error occurred during addition of a row!");
        return;
    }

    addRowCore(msaId, posInMsa, row, os);
}

void SQLiteMsaDbi::undoRemoveRows(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    QList<qint64> posInMsa;
    QList<U2MsaRow> rows;
    bool ok = U2DbiPackUtils::unpackRows(modDetails, posInMsa, rows);
    if (!ok) {
        os.setError("An error occurred during reverting removing of rows!");
        return;
    }

    addRowsCore(msaId, posInMsa, rows, os);
}

void SQLiteMsaDbi::redoRemoveRows(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    QList<qint64> posInMsa;
    QList<U2MsaRow> rows;
    bool ok = U2DbiPackUtils::unpackRows(modDetails, posInMsa, rows);
    if (!ok) {
        os.setError("An error occurred during reverting removing of rows!");
        return;
    }
    QList<qint64> rowIds;
    foreach (const U2MsaRow &row, rows) {
        rowIds << row.rowId;
    }
    removeRowsCore(msaId, rowIds, false, os);
}

void SQLiteMsaDbi::undoRemoveRow(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    U2MsaRow row;
    qint64 posInMsa;
    bool ok = U2DbiPackUtils::unpackRow(modDetails, posInMsa, row);
    if (!ok) {
        os.setError("An error occurred during reverting removing of a row!");
        return;
    }

    addRowCore(msaId, posInMsa, row, os);
}

void SQLiteMsaDbi::redoRemoveRow(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    U2MsaRow row;
    qint64 posInMsa;
    bool ok = U2DbiPackUtils::unpackRow(modDetails, posInMsa, row);
    if (!ok) {
        os.setError("An error occurred during reverting removing of a row!");
        return;
    }

    removeRowCore(msaId, row.rowId, false, os);
}

void SQLiteMsaDbi::undoUpdateGapModel(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    qint64 rowId = 0;
    QList<U2MsaGap> oldGaps;
    QList<U2MsaGap> newGaps;
    bool ok = U2DbiPackUtils::unpackGapDetails(modDetails, rowId, oldGaps, newGaps);
    if (!ok) {
        os.setError("An error occurred during updating an alignment gaps!");
        return;
    }

    updateGapModelCore(msaId, rowId, oldGaps, os);
}

void SQLiteMsaDbi::redoUpdateGapModel(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    qint64 rowId = 0;
    QList<U2MsaGap> oldGaps;
    QList<U2MsaGap> newGaps;
    bool ok = U2DbiPackUtils::unpackGapDetails(modDetails, rowId, oldGaps, newGaps);
    if (!ok) {
        os.setError("An error occurred during updating an alignment gaps!");
        return;
    }

    updateGapModelCore(msaId, rowId, newGaps, os);
}

void SQLiteMsaDbi::undoSetNewRowsOrder(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    QList<qint64> oldOrder;
    QList<qint64> newOrder;
    bool ok = U2DbiPackUtils::unpackRowOrderDetails(modDetails, oldOrder, newOrder);
    if (!ok) {
        os.setError("An error occurred during updating an alignment row order!");
        return;
    }

    // Set the old order
    setNewRowsOrderCore(msaId, oldOrder, os);
}

void SQLiteMsaDbi::redoSetNewRowsOrder(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    QList<qint64> oldOrder;
    QList<qint64> newOrder;
    bool ok = U2DbiPackUtils::unpackRowOrderDetails(modDetails, oldOrder, newOrder);
    if (!ok) {
        os.setError("An error occurred during updating an alignment row order!");
        return;
    }

    setNewRowsOrderCore(msaId, newOrder, os);
}

void SQLiteMsaDbi::undoUpdateRowInfo(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    U2MsaRow oldRow;
    U2MsaRow newRow;
    bool ok = U2DbiPackUtils::unpackRowInfoDetails(modDetails, oldRow, newRow);
    if (!ok) {
        os.setError("An error occurred during updating a row info!");
        return;
    }
    SAFE_POINT(oldRow.rowId == newRow.rowId, "Incorrect rowId!", );
    SAFE_POINT(oldRow.sequenceId == newRow.sequenceId, "Incorrect sequenceId!", );

    updateRowInfoCore(msaId, oldRow, os);
}

void SQLiteMsaDbi::redoUpdateRowInfo(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    U2MsaRow oldRow;
    U2MsaRow newRow;
    bool ok = U2DbiPackUtils::unpackRowInfoDetails(modDetails, oldRow, newRow);
    if (!ok) {
        os.setError("An error occurred during updating a row info!");
        return;
    }
    SAFE_POINT(oldRow.rowId == newRow.rowId, "Incorrect rowId!", );
    SAFE_POINT(oldRow.sequenceId == newRow.sequenceId, "Incorrect sequenceId!", );

    updateRowInfoCore(msaId, newRow, os);
}

void SQLiteMsaDbi::undoMsaLengthChange(const U2DataId &msaId, const QByteArray &modDetails, U2OpStatus &os) {
    qint64 oldLen;
    qint64 newLen;

    bool ok = U2DbiPackUtils::unpackAlignmentLength(modDetails, oldLen, newLen);
    CHECK_EXT(ok, os.setError(U2DbiL10n::tr("An error occurred during updating an msa length")), );

    updateMsaLengthCore(msaId, oldLen, os);
}

void SQLiteMsaDbi::redoMsaLengthChange(const U2DataId &msaId, const QByteArray &modDetails, U2OpStatus &os) {
    qint64 oldLen;
    qint64 newLen;

    bool ok = U2DbiPackUtils::unpackAlignmentLength(modDetails, oldLen, newLen);
    CHECK_EXT(ok, os.setError(U2DbiL10n::tr("An error occurred during updating an msa length")), );

    updateMsaLengthCore(msaId, newLen, os);
}


} //namespace
