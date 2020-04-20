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

#include <U2Core/U2DbiPackUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SqlHelpers.h>

#include "MysqlModDbi.h"
#include "MysqlMsaDbi.h"
#include "MysqlSequenceDbi.h"
#include "util/MysqlDbiUtils.h"
#include "util/MysqlHelpers.h"

namespace U2 {

MysqlMsaDbi::MysqlMsaDbi(MysqlDbi* dbi) :
    U2MsaDbi(dbi),
    MysqlChildDbiCommon(dbi)
{
}

void MysqlMsaDbi::initSqlSchema(U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    // MSA object
    U2SqlQuery("CREATE TABLE Msa (object BIGINT PRIMARY KEY, length BIGINT NOT NULL, "
        "alphabet TEXT NOT NULL, numOfRows INTEGER NOT NULL, "
        "FOREIGN KEY(object) REFERENCES Object(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();
    CHECK_OP(os, );

     // MSA object row
     //   msa      - msa object id
     //   rowId    - id of the row in the msa
     //   sequence - sequence object id
     //   pos      - positional number of a row in the msa (initially, equals 'id', but can be changed, e.g. in GUI by moving rows)
     //   gstart   - offset of the first element in the sequence
     //   gend     - offset of the last element in the sequence (non-inclusive)
     //   length   - sequence and gaps length (trailing gap are not taken into account)
    U2SqlQuery("CREATE TABLE MsaRow (msa BIGINT NOT NULL, "
        "rowId BIGINT NOT NULL PRIMARY KEY AUTO_INCREMENT, "
        "sequence BIGINT NOT NULL, pos BIGINT NOT NULL, gstart BIGINT NOT NULL, "
        "gend BIGINT NOT NULL, length BIGINT NOT NULL, "
        "FOREIGN KEY(msa) REFERENCES Msa(object) ON DELETE CASCADE, "
        "FOREIGN KEY(sequence) REFERENCES Sequence(object) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();
    CHECK_OP(os, );

    U2SqlQuery("CREATE INDEX MsaRow_msa_rowId ON MsaRow(msa, rowId)", db, os).execute();
    CHECK_OP(os, );
    U2SqlQuery("CREATE INDEX MsaRow_length ON MsaRow(length)", db, os).execute();
    CHECK_OP(os, );

     // Gap info for a MSA row:
     //   msa       - msa object id
     //   rowId     - id of the row in the msa
     //   gapStart  - start of the gap, the coordinate is relative to the gstart coordinate of the row
     //   gapEnd    - end of the gap, the coordinate is relative to the gstart coordinate of the row (non-inclusive)
     // Note! there is invariant: gend - gstart (of the row) == gapEnd - gapStart
    U2SqlQuery("CREATE TABLE MsaRowGap (msa BIGINT NOT NULL, rowId BIGINT NOT NULL, "
        "gapStart BIGINT NOT NULL, gapEnd BIGINT NOT NULL, "
        "FOREIGN KEY(rowId) REFERENCES MsaRow(rowId) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();
    CHECK_OP(os, );

    U2SqlQuery("CREATE INDEX MsaRowGap_msa_rowId ON MsaRowGap(msa, rowId)", db, os).execute();
    CHECK_OP(os, );
}

U2Msa MysqlMsaDbi::getMsaObject(const U2DataId& msaId, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    U2Msa res;
    dbi->getMysqlObjectDbi()->getObject(res, msaId, os);
    CHECK_OP(os, res);

    static const QString queryString = "SELECT length, alphabet FROM Msa WHERE object = :object";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":object", msaId);
    if (q.step()) {
        res.length = q.getInt64(0);
        res.alphabet = q.getString(1);
        q.ensureDone();
    } else if (!os.hasError()) {
        os.setError(U2DbiL10n::tr("Msa object not found"));
    }

    return res;
}

qint64 MysqlMsaDbi::getNumOfRows(const U2DataId& msaId, U2OpStatus& os) {
    qint64 res = 0;

    static const QString queryString = "SELECT numOfRows FROM Msa WHERE object = :object";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":object", msaId);
    if (q.step())  {
        res = q.getInt64(0);
        q.ensureDone();
    } else if (!os.hasError()) {
        os.setError(U2DbiL10n::tr("Msa object not found"));
    }

    return res;
}

QList<U2MsaRow> MysqlMsaDbi::getRows(const U2DataId& msaId, U2OpStatus& os) {
    QList<U2MsaRow> res;
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString rowString = "SELECT rowId, sequence, gstart, gend, length FROM MsaRow WHERE msa = :msa ORDER BY pos";
    U2SqlQuery rowQuery(rowString, db, os);
    rowQuery.bindDataId(":msa", msaId);

    static const QString gapString = "SELECT gapStart, gapEnd FROM MsaRowGap WHERE msa = :msa AND rowId = :rowId ORDER BY gapStart";

    while (rowQuery.step()) {
        U2MsaRow row;
        row.rowId = rowQuery.getInt64(0);
        row.sequenceId = rowQuery.getDataId(1, U2Type::Sequence);
        row.gstart = rowQuery.getInt64(2);
        row.gend = rowQuery.getInt64(3);
        row.length = rowQuery.getInt64(4);

        U2SqlQuery gapQuery(gapString, db, os);
        gapQuery.bindDataId(":msa", msaId);
        gapQuery.bindInt64(":rowId", row.rowId);
        while (gapQuery.step()) {
            U2MsaGap gap;
            gap.offset = gapQuery.getInt64(0);
            gap.gap = gapQuery.getInt64(1) - gap.offset;
            SAFE_POINT_EXT(gap.isValid(), os.setError("An invalid gap is stored in the database"), res);
            row.gaps.append(gap);
        }
        CHECK_OP(os, res);

        res.append(row);
    }

    return res;
}

U2MsaRow MysqlMsaDbi::getRow(const U2DataId& msaId, qint64 rowId, U2OpStatus& os) {
    U2MsaRow res;
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString rowString = "SELECT sequence, gstart, gend, length FROM MsaRow WHERE msa = :msa AND rowId = :rowId";
    U2SqlQuery q(rowString, db, os);
    q.bindDataId(":msa", msaId);
    q.bindInt64(":rowId", rowId);
    if (q.step()) {
        res.rowId = rowId;
        res.sequenceId = q.getDataId(0, U2Type::Sequence);
        res.gstart = q.getInt64(1);
        res.gend = q.getInt64(2);
        res.length = q.getInt64(3);
        q.ensureDone();
    } else if (!os.hasError()) {
        os.setError(U2DbiL10n::tr("Msa row not found"));
        return res;
    }

    static const QString gapString = "SELECT gapStart, gapEnd FROM MsaRowGap WHERE msa = :msa AND rowId = :rowId ORDER BY gapStart";
    U2SqlQuery gapQ(gapString, db, os);
    gapQ.bindDataId(":msa", msaId);
    gapQ.bindInt64(":rowId", rowId);
    while (gapQ.step()) {
        U2MsaGap gap;
        gap.offset = gapQ.getInt64(0);
        gap.gap = gapQ.getInt64(1) - gap.offset;
        res.gaps.append(gap);
    }

    return res;
}

QList<qint64> MysqlMsaDbi::getRowsOrder(const U2DataId& msaId, U2OpStatus& os) {
    QList<qint64> res;

    static const QString queryString = "SELECT rowId FROM MsaRow WHERE msa = :msa ORDER BY pos";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":msa", msaId);
    while (q.step()) {
        res.append(q.getInt64(0));
    }

    return res;
}

U2AlphabetId MysqlMsaDbi::getMsaAlphabet(const U2DataId& msaId, U2OpStatus& os) {
    QString alphabetName;
    U2SqlQuery q("SELECT alphabet FROM Msa WHERE object = :object", db, os);
    q.bindDataId(":object", msaId);
    if (q.step()) {
        alphabetName = q.getString(0);
        q.ensureDone();
    } else if (!os.hasError()) {
        os.setError(U2DbiL10n::tr("Msa object not found"));
    }

    return U2AlphabetId(alphabetName);
}

U2DataId MysqlMsaDbi::createMsaObject(const QString& folder, const QString& name, const U2AlphabetId& alphabet, U2OpStatus& os) {
    return createMsaObject(folder, name, alphabet, 0, os);
}

U2DataId MysqlMsaDbi::createMsaObject(const QString& folder, const QString& name, const U2AlphabetId& alphabet, int length, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    U2Msa msa;
    msa.visualName = name;
    msa.alphabet = alphabet;
    msa.length = length;

    // Create the object
    dbi->getMysqlObjectDbi()->createObject(msa, folder, U2DbiObjectRank_TopLevel, os);
    CHECK_OP(os, U2DataId());

    // Create a record in the Msa table
    static const QString queryString = "INSERT INTO Msa(object, length, alphabet, numOfRows) VALUES(:object, :length, :alphabet, :numOfRows)";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":object", msa.id);
    q.bindInt64(":length", msa.length);
    q.bindString(":alphabet", msa.alphabet.id);
    q.bindInt64(":numOfRows", 0); // no rows
    q.insert();

    return msa.id;
}

void MysqlMsaDbi::updateMsaName(const U2DataId& msaId, const QString& name, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    U2Object msaObj;
    dbi->getMysqlObjectDbi()->getObject(msaObj, msaId, os);
    CHECK_OP(os, );

    MysqlDbiUtils::renameObject(dbi, msaObj, name, os);
}

void MysqlMsaDbi::updateMsaAlphabet(const U2DataId& msaId, const U2AlphabetId& alphabet, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    MysqlModificationAction updateAction(dbi, msaId);
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
    static const QString queryString = "UPDATE Msa SET alphabet = :alphabet WHERE object = :object";
    U2SqlQuery q(queryString, db, os);
    q.bindString(":alphabet", alphabet.id);
    q.bindDataId(":object", msaId);
    q.update();
    CHECK_OP(os, );

    // Increment version; track the modification, if required
    updateAction.addModification(msaId, U2ModType::msaUpdatedAlphabet, modDetails, os);
    CHECK_OP(os, );

    updateAction.complete(os);
}

void MysqlMsaDbi::addRows(const U2DataId& msaId, QList<U2MsaRow>& rows, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    MysqlModificationAction updateAction(dbi, msaId);
    U2TrackModType trackMod = updateAction.prepare(os);
    CHECK_OP(os, );

    // Add the rows
    qint64 numOfRows = getNumOfRows(msaId, os);
    CHECK_OP(os, );

    QList<qint64> posInMsa;
    for (int i = 0; i < rows.count(); i++) {
        posInMsa << i + numOfRows;
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
    if (maxLength > getMsaLength(msaId, os)) {
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
    CHECK_OP(os, );

    updateAction.complete(os);
}

void MysqlMsaDbi::addRow(const U2DataId& msaId, qint64 posInMsa, U2MsaRow& row, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    MysqlModificationAction updateAction(dbi, msaId);
    U2TrackModType trackMod = updateAction.prepare(os);
    CHECK_OP(os, );

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
    CHECK_OP(os, );

    updateAction.complete(os);
}

void MysqlMsaDbi::removeRows(const U2DataId& msaId, const QList<qint64>& rowIds, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    MysqlModificationAction updateAction(dbi, msaId);
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
    CHECK_OP(os, );

    if (numOfRows == rowIds.count()) {
        updateMsaLength(updateAction, msaId, 0, os);
    }

    // Increment version; track the modification, if required
    updateAction.addModification(msaId, U2ModType::msaRemovedRows, modDetails, os);
    CHECK_OP(os, );

    updateAction.complete(os);
}

void MysqlMsaDbi::removeRow(const U2DataId& msaId, qint64 rowId, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    MysqlModificationAction updateAction(dbi, msaId);
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
    CHECK_OP(os, );

    updateAction.complete(os);
}

void MysqlMsaDbi::deleteRowsData(const U2DataId& msaId, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString = "DELETE O.* FROM Object AS O INNER JOIN MsaRow AS MR ON O.id = MR.sequence WHERE MR.msa = :msa";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":msa", msaId);
    q.execute();
}

void MysqlMsaDbi::updateRowName(const U2DataId& msaId, qint64 rowId, const QString& newName, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    MysqlModificationAction updateAction(dbi, msaId);
    updateAction.prepare(os);
    CHECK_OP(os, );

    U2DataId sequenceId = getSequenceIdByRowId(msaId, rowId, os);
    CHECK_OP(os, );

    U2Sequence seqObject = dbi->getSequenceDbi()->getSequenceObject(sequenceId, os);
    CHECK_OP(os, );

    MysqlDbiUtils::renameObject(updateAction, dbi, seqObject, newName, os);
    CHECK_OP(os, );

    updateAction.complete(os);
}

void MysqlMsaDbi::updateRowContent(const U2DataId& msaId, qint64 rowId, const QByteArray& seqBytes, const QList<U2MsaGap>& gaps, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    MysqlModificationAction updateAction(dbi, msaId);
    updateAction.prepare(os);
    CHECK_OP(os, );

    // Get the row object
    U2MsaRow row = getRow(msaId, rowId, os);
    CHECK_OP(os, );

    // Update the sequence data
    QVariantMap hints;
    dbi->getMysqlSequenceDbi()->updateSequenceData(updateAction, row.sequenceId, U2_REGION_MAX, seqBytes, hints, os);
    CHECK_OP(os, );

    // Update the row object
    U2MsaRow newRow(row);
    qint64 seqLength = seqBytes.length();
    newRow.gstart = 0;
    newRow.gend = seqLength;
    newRow.length = calculateRowLength(seqLength, gaps);
    updateRowInfo(updateAction, msaId, newRow, os);
    CHECK_OP(os, );

    // Update the gap model
    // WARNING: this update must go after the row info update to recalculate the msa length properly
    updateGapModel(updateAction, msaId, rowId, gaps, os);
    CHECK_OP(os, );

    // Save tracks, if required; increment versions
    updateAction.complete(os);
}

void MysqlMsaDbi::updateGapModel(const U2DataId& msaId, qint64 msaRowId, const QList<U2MsaGap>& gapModel, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    MysqlModificationAction updateAction(dbi, msaId);
    updateAction.prepare(os);
    CHECK_OP(os, );

    updateGapModel(updateAction, msaId, msaRowId, gapModel, os);
    CHECK_OP(os, );

    updateAction.complete(os);
}

void MysqlMsaDbi::updateMsaLength(const U2DataId& msaId, qint64 length, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    MysqlModificationAction updateAction(dbi, msaId);
    updateAction.prepare(os);
        CHECK_OP(os, );

    updateMsaLength(updateAction, msaId, length, os);

    updateAction.complete(os);
}

void MysqlMsaDbi::updateMsaLength(MysqlModificationAction &updateAction, const U2DataId &msaId, qint64 length, U2OpStatus &os) {
    QByteArray modDetails;
    if (TrackOnUpdate == updateAction.getTrackModType()) {
        const qint64 oldMsaLen = getMsaLength(msaId, os);
        CHECK_OP(os, );
        modDetails = U2DbiPackUtils::packAlignmentLength(oldMsaLen, length);
    }

    updateMsaLengthCore(msaId, length, os);

    updateAction.addModification(msaId, U2ModType::msaLengthChanged, modDetails, os);
}

void MysqlMsaDbi::setNewRowsOrder(const U2DataId& msaId, const QList<qint64>& rowIds, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    MysqlModificationAction updateAction(dbi, msaId);
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
    SAFE_POINT(numOfRows == rowIds.count(), "Incorrect number of row IDs", );

    // Set the new order
    setNewRowsOrderCore(msaId, rowIds, os);
    CHECK_OP(os, );

    // Increment version; track the modification, if required
    updateAction.addModification(msaId, U2ModType::msaSetNewRowsOrder, modDetails, os);
    CHECK_OP(os, );

    updateAction.complete(os);
}

void MysqlMsaDbi::undo(const U2DataId& msaId, qint64 modType, const QByteArray& modDetails, U2OpStatus& os) {
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
        os.setError(U2DbiL10n::tr("Unexpected modification type '%1'").arg(QString::number(modType)));
        return;
    }
}

void MysqlMsaDbi::redo(const U2DataId& msaId, qint64 modType, const QByteArray& modDetails, U2OpStatus& os) {
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
    }else if (U2ModType::msaLengthChanged == modType) {
        redoMsaLengthChange(msaId, modDetails, os);
    }
    else {
        os.setError(U2DbiL10n::tr("Unexpected modification type '%1'").arg(QString::number(modType)));
        return;
    }
}

qint64 MysqlMsaDbi::getMsaLength(const U2DataId& msaId, U2OpStatus& os) {
    qint64 res = 0;

    U2SqlQuery q("SELECT length FROM Msa WHERE object = :object", db, os);
    q.bindDataId(":object", msaId);
    if (q.step()) {
        res = q.getInt64(0);
        q.ensureDone();
    }
    else if (!os.hasError()) {
        os.setError(U2DbiL10n::tr("Msa object not found"));
    }

    return res;
}

U2DataId MysqlMsaDbi::createMcaObject(const QString &folder, const QString &name, const U2AlphabetId &alphabet, U2OpStatus &os) {
    return createMcaObject(folder, name, alphabet, 0, os);
}

U2DataId MysqlMsaDbi::createMcaObject(const QString &folder, const QString &name, const U2AlphabetId &alphabet, int length, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    U2Mca mca;
    mca.visualName = name;
    mca.alphabet = alphabet;
    mca.length = length;

    // Create the object
    dbi->getMysqlObjectDbi()->createObject(mca, folder, U2DbiObjectRank_TopLevel, os);
    CHECK_OP(os, U2DataId());

    // Create a record in the Msa table
    static const QString queryString = "INSERT INTO Msa(object, length, alphabet, numOfRows) VALUES(:object, :length, :alphabet, :numOfRows)";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":object", mca.id);
    q.bindInt64(":length", mca.length);
    q.bindString(":alphabet", mca.alphabet.id);
    q.bindInt64(":numOfRows", 0); // no rows
    q.insert();

    return mca.id;
}

void MysqlMsaDbi::addMsaRowAndGaps(const U2DataId& msaId, qint64 posInMsa, U2MsaRow& row, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    createMsaRow(msaId, posInMsa, row, os);
    CHECK_OP(os, );

    foreach (const U2MsaGap& gap, row.gaps) {
        createMsaRowGap(msaId, row.rowId, gap, os);
        CHECK_OP(os, );
    }

    dbi->getMysqlObjectDbi()->setParent(msaId, row.sequenceId, os);
}

void MysqlMsaDbi::createMsaRow(const U2DataId& msaId, qint64 posInMsa, U2MsaRow& msaRow, U2OpStatus& os) {
    SAFE_POINT(posInMsa >= 0, U2DbiL10n::tr("Invalid row position: %1").arg(QString::number(posInMsa)), );

    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    // Calculate the row length
    qint64 rowLength = calculateRowLength(msaRow.gend - msaRow.gstart, msaRow.gaps);

    // Insert the data
    static const QString queryString = "INSERT INTO MsaRow(msa, sequence, pos, gstart, gend, length) VALUES(:msa, :sequence, :pos, :gstart, :gend, :length)";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":msa", msaId);
    q.bindDataId(":sequence", msaRow.sequenceId);
    q.bindInt64(":pos", posInMsa);
    q.bindInt64(":gstart", msaRow.gstart);
    q.bindInt64(":gend", msaRow.gend);
    q.bindInt64(":length", rowLength);
    msaRow.rowId = q.insert();
}

void MysqlMsaDbi::createMsaRowGap(const U2DataId& msaId, qint64 msaRowId, const U2MsaGap& msaGap, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString = "INSERT INTO MsaRowGap(msa, rowId, gapStart, gapEnd) VALUES(:msa, :rowId, :gapStart, :gapEnd)";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":msa", msaId);
    q.bindInt64(":rowId", msaRowId);
    q.bindInt64(":gapStart", msaGap.offset);
    q.bindInt64(":gapEnd", msaGap.offset + msaGap.gap);
    q.insert();
}

void MysqlMsaDbi::removeMsaRowAndGaps(const U2DataId& msaId, qint64 rowId, bool removeSequence, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    U2DataId sequenceId = getSequenceIdByRowId(msaId, rowId, os);
    CHECK_OP(os, );

    removeRecordsFromMsaRowGap(msaId, rowId, os);
    CHECK_OP(os, );
    removeRecordFromMsaRow(msaId, rowId, os);
    CHECK_OP(os, );

    dbi->getMysqlObjectDbi()->removeParent(msaId, sequenceId, removeSequence, os);
}

void MysqlMsaDbi::removeRecordsFromMsaRowGap(const U2DataId& msaId, qint64 rowId, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString = "DELETE FROM MsaRowGap WHERE msa = :msa AND rowId = :rowId";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":msa", msaId);
    q.bindInt64(":rowId", rowId);
    q.update();
}

void MysqlMsaDbi::removeRecordFromMsaRow(const U2DataId& msaId, qint64 rowId, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString = "DELETE FROM MsaRow WHERE msa = :msa AND rowId = :rowId";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":msa", msaId);
    q.bindInt64(":rowId", rowId);
    q.update();
}

void MysqlMsaDbi::updateNumOfRows(const U2DataId& msaId, qint64 numOfRows, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString = "UPDATE Msa SET numOfRows = :numOfRows WHERE object = :object";
    U2SqlQuery q(queryString, db, os);
    q.bindInt64(":numOfRows", numOfRows);
    q.bindDataId(":object", msaId);
    q.update();
}

void MysqlMsaDbi::recalculateRowsPositions(const U2DataId& msaId, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    QList<U2MsaRow> rows = getRows(msaId, os);
    CHECK_OP(os, );

    static const QString queryString = "UPDATE MsaRow SET pos = :pos WHERE msa = :msa AND rowId = :rowId";
    for (int i = 0, n = rows.count(); i < n; ++i) {
        qint64 rowId = rows[i].rowId;
        U2SqlQuery q(queryString, db, os);
        q.bindInt64(":pos", i);
        q.bindDataId(":msa", msaId);
        q.bindInt64(":rowId", rowId);
        q.execute();
        CHECK_OP(os, );
    }
}

qint64 MysqlMsaDbi::calculateRowLength(qint64 seqLength, const QList<U2MsaGap>& gaps) {
    qint64 res = seqLength;
    foreach (const U2MsaGap& gap, gaps) {
        // ignore trailing gaps
        if (gap.offset < res) {
            res += gap.gap;
        }
    }

    return res;
}

qint64 MysqlMsaDbi::getRowSequenceLength(const U2DataId& msaId, qint64 rowId, U2OpStatus& os) {
    qint64 res = 0;

    static const QString queryString = "SELECT gstart, gend FROM MsaRow WHERE msa = :msa AND rowId = :rowId";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":msa", msaId);
    q.bindInt64(":rowId", rowId);
    if (q.step()) {
        qint64 startInSeq = q.getInt64(0);
        qint64 endInSeq = q.getInt64(1);
        res = endInSeq - startInSeq;
        q.ensureDone();
    } else if (!os.hasError()) {
        os.setError(U2DbiL10n::tr("Msa row not found"));
    }

    return res;
}

void MysqlMsaDbi::updateRowLength(const U2DataId& msaId, qint64 rowId, qint64 newLength, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString = "UPDATE MsaRow SET length = :length WHERE msa = :msa AND rowId = :rowId";
    U2SqlQuery q(queryString, db, os);
    q.bindInt64(":length", newLength);
    q.bindDataId(":msa", msaId);
    q.bindInt64(":rowId", rowId);
    q.update();
}

void MysqlMsaDbi::updateMsaLengthCore(const U2DataId& msaId, qint64 length, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString = "UPDATE Msa SET length = :length WHERE object = :object";
    U2SqlQuery q(queryString, db, os);
    q.bindInt64(":length", length);
    q.bindDataId(":object", msaId);
    q.execute();
}

U2DataId MysqlMsaDbi::getSequenceIdByRowId(const U2DataId& msaId, qint64 rowId, U2OpStatus& os) {
    U2DataId res;

    static const QString queryString = "SELECT sequence FROM MsaRow WHERE msa = :msa AND rowId = :rowId";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":msa", msaId);
    q.bindInt64(":rowId", rowId);
    if (q.step()) {
        res = q.getDataId(0, U2Type::Sequence);
        q.ensureDone();
    } else if (!os.hasError()) {
        os.setError(U2DbiL10n::tr("Msa row not found"));
    }

    return res;
}

qint64 MysqlMsaDbi::getPosInMsa(const U2DataId &msaId, qint64 rowId, U2OpStatus &os) {
    qint64 result = -1;

    static const QString queryString = "SELECT pos FROM MsaRow WHERE msa = :msa AND rowId = :rowId";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":msa", msaId);
    q.bindInt64(":rowId", rowId);
    if (q.step()) {
        result = q.getInt64(0);
        q.ensureDone();
    } else if (!os.hasError()) {
        os.setError(QString("No row with id '%1' in msa '%2'").arg(QString::number(rowId)).arg(msaId.data()));
    }

    return result;
}

/************************************************************************/
/* Core methods                                                         */
/************************************************************************/
void MysqlMsaDbi::updateGapModelCore(const U2DataId &msaId, qint64 msaRowId, const QList<U2MsaGap> &gapModel, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

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

void MysqlMsaDbi::addRowSubcore(const U2DataId &msaId, qint64 numOfRows, const QList<qint64> &rowsOrder, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    // Re-calculate position, if needed
    setNewRowsOrderCore(msaId, rowsOrder, os);
    CHECK_OP(os, );

    // Update the number of rows of the MSA
    updateNumOfRows(msaId, numOfRows, os);
}

void MysqlMsaDbi::addRowCore(const U2DataId& msaId, qint64 posInMsa, U2MsaRow& row, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    // Append the row to the end, if "-1"
    qint64 numOfRows = getNumOfRows(msaId, os);
    CHECK_OP(os, );

    QList<qint64> rowsOrder;
    if (-1 == posInMsa) {
        posInMsa = numOfRows;
    } else {
        rowsOrder = getRowsOrder(msaId, os);
        CHECK_OP(os, );
        SAFE_POINT(rowsOrder.count() == numOfRows, "Incorrect number of rows", );
    }
    SAFE_POINT(0 <= posInMsa && posInMsa <= numOfRows, "Incorrect input position", );

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

void MysqlMsaDbi::addRowsCore(const U2DataId &msaId, const QList<qint64> &posInMsa, QList<U2MsaRow> &rows, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    qint64 numOfRows = getNumOfRows(msaId, os);
    CHECK_OP(os, );

    QList<qint64> rowsOrder = getRowsOrder(msaId, os);
    CHECK_OP(os, );
    SAFE_POINT(rowsOrder.count() == numOfRows, "Incorrect number of rows", );

    // Add new rows
    QList<qint64>::ConstIterator pi = posInMsa.begin();
    QList<U2MsaRow>::Iterator ri = rows.begin();
    for (; ri != rows.end(); ri++, pi++) {
        qint64 pos = *pi;
        if (-1 == pos) {
            pos = numOfRows;
        }
        SAFE_POINT(0 <= pos && pos <= numOfRows, "Incorrect input position", );

        addMsaRowAndGaps(msaId, pos, *ri, os);
        CHECK_OP(os, );

        ri->length = calculateRowLength(ri->gend - ri->gstart, ri->gaps);
        numOfRows++;
        rowsOrder.insert(pos, ri->rowId);
    }

    addRowSubcore(msaId, numOfRows, rowsOrder, os);
}

void MysqlMsaDbi::removeRowSubcore(const U2DataId &msaId, qint64 numOfRows, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    // Update the number of rows
    updateNumOfRows(msaId, numOfRows, os);
    CHECK_OP(os, );

    // Re-calculate the rows positions
    recalculateRowsPositions(msaId, os);
}

void MysqlMsaDbi::removeRowCore(const U2DataId& msaId, qint64 rowId, bool removeSequence, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    // Get and verify the number of rows
    qint64 numOfRows = getNumOfRows(msaId, os);
    CHECK_OP(os, );
    SAFE_POINT(numOfRows > 0, "Empty MSA", );

    // Remove the row
    removeMsaRowAndGaps(msaId, rowId, removeSequence, os);
    CHECK_OP(os, );

    removeRowSubcore(msaId, numOfRows - 1, os);
}

void MysqlMsaDbi::removeRowsCore(const U2DataId &msaId, const QList<qint64> &rowIds, bool removeSequence, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    qint64 numOfRows = getNumOfRows(msaId, os);
    CHECK_OP(os, );
    SAFE_POINT(numOfRows >= rowIds.count() , "Incorrect rows to remove", );

    for (int i = 0; i < rowIds.count(); ++i) {
        removeMsaRowAndGaps(msaId, rowIds[i], removeSequence, os);
        CHECK_OP(os, );
    }

    removeRowSubcore(msaId, numOfRows-rowIds.size(), os);
}

void MysqlMsaDbi::setNewRowsOrderCore(const U2DataId &msaId, const QList<qint64> rowIds, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString = "UPDATE MsaRow SET pos = :pos WHERE msa = :msa AND rowId = :rowId";
    for (int i = 0, n = rowIds.count(); i < n; ++i) {
        qint64 rowId = rowIds[i];
        U2SqlQuery q(queryString, db, os);
        q.bindInt64(":pos", i);
        q.bindDataId(":msa", msaId);
        q.bindInt64(":rowId", rowId);
        q.execute();
        CHECK_OP(os, );
    }
}

void MysqlMsaDbi::updateRowInfoCore(const U2DataId& msaId, const U2MsaRow& row, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString = "UPDATE MsaRow SET sequence = :sequence, gstart = :gstart, gend = :gend WHERE msa = :msa AND rowId = :rowId";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":sequence", row.sequenceId);
    q.bindInt64(":gstart", row.gstart);
    q.bindInt64(":gend", row.gend);
    q.bindDataId(":msa", msaId);
    q.bindInt64(":rowId", row.rowId);
    q.update();
}

/************************************************************************/
/* Undo/redo methods */
/************************************************************************/
void MysqlMsaDbi::undoUpdateMsaAlphabet(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    U2AlphabetId oldAlphabet;
    U2AlphabetId newAlphabet;

    bool ok = U2DbiPackUtils::unpackAlphabetDetails(modDetails, oldAlphabet, newAlphabet);
    CHECK_EXT(ok, os.setError(U2DbiL10n::tr("An error occurred during updating an alignment alphabet")), );

    // Update the value
    static const QString queryString = "UPDATE Msa SET alphabet = :alphabet WHERE object = :object";
    U2SqlQuery q(queryString, db, os);
    q.bindString(":alphabet", oldAlphabet.id);
    q.bindDataId(":object", msaId);
    q.update();
}

void MysqlMsaDbi::undoAddRows(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    QList<qint64> posInMsa;
    QList<U2MsaRow> rows;

    bool ok = U2DbiPackUtils::unpackRows(modDetails, posInMsa, rows);
    CHECK_EXT(ok, os.setError(U2DbiL10n::tr("An error occurred during reverting adding of rows")), );

    QList<qint64> rowIds;
    foreach (const U2MsaRow &row, rows) {
        rowIds << row.rowId;
    }

    removeRowsCore(msaId, rowIds, false, os);
}

void MysqlMsaDbi::undoAddRow(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    U2MsaRow row;
    qint64 posInMsa;

    bool ok = U2DbiPackUtils::unpackRow(modDetails, posInMsa, row);
    CHECK_EXT(ok, os.setError(U2DbiL10n::tr("An error occurred during reverting addition of a row")), );

    removeRowCore(msaId, row.rowId, false, os);
}

void MysqlMsaDbi::undoRemoveRows(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    QList<qint64> posInMsa;
    QList<U2MsaRow> rows;

    bool ok = U2DbiPackUtils::unpackRows(modDetails, posInMsa, rows);
    CHECK_EXT(ok, os.setError(U2DbiL10n::tr("An error occurred during reverting removing of rows")), );

    addRowsCore(msaId, posInMsa, rows, os);
}

void MysqlMsaDbi::undoRemoveRow(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    U2MsaRow row;
    qint64 posInMsa;

    bool ok = U2DbiPackUtils::unpackRow(modDetails, posInMsa, row);
    CHECK_EXT(ok, os.setError(U2DbiL10n::tr("An error occurred during reverting removing of a row")), );

    addRowCore(msaId, posInMsa, row, os);
}

void MysqlMsaDbi::undoUpdateGapModel(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    qint64 rowId = 0;
    QList<U2MsaGap> oldGaps;
    QList<U2MsaGap> newGaps;

    bool ok = U2DbiPackUtils::unpackGapDetails(modDetails, rowId, oldGaps, newGaps);
    CHECK_EXT(ok, os.setError(U2DbiL10n::tr("An error occurred during updating an alignment gaps")), );

    updateGapModelCore(msaId, rowId, oldGaps, os);
}

void MysqlMsaDbi::undoSetNewRowsOrder(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    QList<qint64> oldOrder;
    QList<qint64> newOrder;

    bool ok = U2DbiPackUtils::unpackRowOrderDetails(modDetails, oldOrder, newOrder);
    CHECK_EXT(ok, os.setError(U2DbiL10n::tr("An error occurred during updating an alignment row order")), );

    // Set the old order
    setNewRowsOrderCore(msaId, oldOrder, os);
}

void MysqlMsaDbi::undoUpdateRowInfo(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    U2MsaRow oldRow;
    U2MsaRow newRow;

    bool ok = U2DbiPackUtils::unpackRowInfoDetails(modDetails, oldRow, newRow);
    CHECK_EXT(ok, os.setError(U2DbiL10n::tr("An error occurred during updating a row info")), );
    SAFE_POINT(oldRow.rowId == newRow.rowId, "Incorrect rowId", );
    SAFE_POINT(oldRow.sequenceId == newRow.sequenceId, "Incorrect sequenceId", );

    updateRowInfoCore(msaId, oldRow, os);
}

void MysqlMsaDbi::undoMsaLengthChange(const U2DataId &msaId, const QByteArray &modDetails, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    qint64 oldLen;
    qint64 newLen;

    bool ok = U2DbiPackUtils::unpackAlignmentLength(modDetails, oldLen, newLen);
    CHECK_EXT(ok, os.setError(U2DbiL10n::tr("An error occurred during updating an msa length")), );

    updateMsaLengthCore(msaId, oldLen, os);
}

void MysqlMsaDbi::redoUpdateMsaAlphabet(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    U2AlphabetId oldAlphabet;
    U2AlphabetId newAlphabet;

    bool ok = U2DbiPackUtils::unpackAlphabetDetails(modDetails, oldAlphabet, newAlphabet);
    CHECK_EXT(ok, os.setError(U2DbiL10n::tr("An error occurred during updating an alignment alphabet")), );

    // Redo the updating
    static const QString queryString = "UPDATE Msa SET alphabet = :alphabet WHERE object = :object";
    U2SqlQuery q(queryString, db, os);
    q.bindString(":alphabet", newAlphabet.id);
    q.bindDataId(":object", msaId);
    q.update();
}

void MysqlMsaDbi::redoAddRows(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    QList<qint64> posInMsa;
    QList<U2MsaRow> rows;

    bool ok = U2DbiPackUtils::unpackRows(modDetails, posInMsa, rows);
    CHECK_EXT(ok, os.setError(U2DbiL10n::tr("An error occurred during reverting adding of rows")), );

    addRowsCore(msaId, posInMsa, rows, os);
}

void MysqlMsaDbi::redoAddRow(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    U2MsaRow row;
    qint64 posInMsa;

    bool ok = U2DbiPackUtils::unpackRow(modDetails, posInMsa, row);
    CHECK_EXT(ok, os.setError(U2DbiL10n::tr("An error occurred during addition of a row")), );

    addRowCore(msaId, posInMsa, row, os);
}

void MysqlMsaDbi::redoRemoveRows(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    QList<qint64> posInMsa;
    QList<U2MsaRow> rows;

    bool ok = U2DbiPackUtils::unpackRows(modDetails, posInMsa, rows);
    CHECK_EXT(ok, os.setError(U2DbiL10n::tr("An error occurred during reverting removing of rows")), );

    QList<qint64> rowIds;
    foreach (const U2MsaRow &row, rows) {
        rowIds << row.rowId;
    }

    removeRowsCore(msaId, rowIds, false, os);
}

void MysqlMsaDbi::redoRemoveRow(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    U2MsaRow row;
    qint64 posInMsa;

    bool ok = U2DbiPackUtils::unpackRow(modDetails, posInMsa, row);
    CHECK_EXT(ok, os.setError(U2DbiL10n::tr("An error occurred during reverting removing of a row")), );

    removeRowCore(msaId, row.rowId, false, os);
}

void MysqlMsaDbi::redoUpdateGapModel(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    qint64 rowId = 0;
    QList<U2MsaGap> oldGaps;
    QList<U2MsaGap> newGaps;

    bool ok = U2DbiPackUtils::unpackGapDetails(modDetails, rowId, oldGaps, newGaps);
    CHECK_EXT(ok, os.setError(U2DbiL10n::tr("An error occurred during updating an alignment gaps")), );

    updateGapModelCore(msaId, rowId, newGaps, os);
}

void MysqlMsaDbi::redoSetNewRowsOrder(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    QList<qint64> oldOrder;
    QList<qint64> newOrder;

    bool ok = U2DbiPackUtils::unpackRowOrderDetails(modDetails, oldOrder, newOrder);
    CHECK_EXT(ok, os.setError(U2DbiL10n::tr("An error occurred during updating an alignment row order")), );

    setNewRowsOrderCore(msaId, newOrder, os);
}

void MysqlMsaDbi::redoUpdateRowInfo(const U2DataId& msaId, const QByteArray& modDetails, U2OpStatus& os) {
    U2MsaRow oldRow;
    U2MsaRow newRow;

    bool ok = U2DbiPackUtils::unpackRowInfoDetails(modDetails, oldRow, newRow);
    CHECK_EXT(ok, os.setError(U2DbiL10n::tr("An error occurred during updating a row info")), );
    SAFE_POINT(oldRow.rowId == newRow.rowId, "Incorrect rowId", );
    SAFE_POINT(oldRow.sequenceId == newRow.sequenceId, "Incorrect sequenceId", );

    updateRowInfoCore(msaId, newRow, os);
}

void MysqlMsaDbi::redoMsaLengthChange(const U2DataId &msaId, const QByteArray &modDetails, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    qint64 oldLen;
    qint64 newLen;

    bool ok = U2DbiPackUtils::unpackAlignmentLength(modDetails, oldLen, newLen);
    CHECK_EXT(ok, os.setError(U2DbiL10n::tr("An error occurred during updating an msa length")), );

    updateMsaLengthCore(msaId, newLen, os);
}

/************************************************************************/
/* Methods included into a multi-action                                 */
/************************************************************************/

void MysqlMsaDbi::updateRowInfo(MysqlModificationAction &updateAction, const U2DataId &msaId, const U2MsaRow &row, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    QByteArray modDetails;
    if (TrackOnUpdate == updateAction.getTrackModType()) {
        U2MsaRow oldRow = getRow(msaId, row.rowId, os);
        CHECK_OP(os, );
        modDetails = U2DbiPackUtils::packRowInfoDetails(oldRow, row);
    }

    updateRowInfoCore(msaId, row, os);
    CHECK_OP(os, );

    // Track the modification, if required; add the object to the list (versions of the objects will be incremented on the updateAction completion)
    updateAction.addModification(msaId, U2ModType::msaUpdatedRowInfo, modDetails, os);
}

void MysqlMsaDbi::updateGapModel(MysqlModificationAction &updateAction, const U2DataId& msaId, qint64 msaRowId, const QList<U2MsaGap>& gapModel, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    QByteArray gapsDetails;
    if (TrackOnUpdate == updateAction.getTrackModType()) {
        U2MsaRow row = getRow(msaId, msaRowId, os);
        CHECK_OP(os, );
        gapsDetails = U2DbiPackUtils::packGapDetails(msaRowId, row.gaps, gapModel);
    }

    updateGapModelCore(msaId, msaRowId, gapModel, os);
    CHECK_OP(os, );

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
}

}   // namespace U2
