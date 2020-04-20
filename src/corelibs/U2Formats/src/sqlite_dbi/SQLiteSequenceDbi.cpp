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

#include "SQLiteSequenceDbi.h"
#include "SQLiteObjectDbi.h"

#include <U2Core/U2DbiPackUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceUtils.h>
#include <U2Core/U2SqlHelpers.h>

namespace U2 {

SQLiteSequenceDbi::SQLiteSequenceDbi(SQLiteDbi* dbi) : U2SequenceDbi(dbi), SQLiteChildDBICommon(dbi) {
}

void SQLiteSequenceDbi::initSqlSchema(U2OpStatus& os) {
    // sequence object
    SQLiteWriteQuery("CREATE TABLE Sequence (object INTEGER PRIMARY KEY, length INTEGER NOT NULL DEFAULT 0, alphabet TEXT NOT NULL, circular INTEGER NOT NULL DEFAULT 0, "
                "FOREIGN KEY(object) REFERENCES Object(id) ON DELETE CASCADE)", db, os).execute();

    // part of the sequence, starting with 'sstart'(inclusive) and ending at 'send'(not inclusive)
    SQLiteWriteQuery("CREATE TABLE SequenceData (sequence INTEGER, sstart INTEGER NOT NULL, send INTEGER NOT NULL, data BLOB NOT NULL, "
                "PRIMARY KEY (sequence, sstart, send), "
                "FOREIGN KEY(sequence) REFERENCES Sequence(object) ON DELETE CASCADE)", db, os).execute();

    SQLiteWriteQuery("CREATE INDEX SequenceData_sequence_send on SequenceData(sequence, send)", db, os).execute();
}

U2Sequence SQLiteSequenceDbi::getSequenceObject(const U2DataId& sequenceId, U2OpStatus& os) {
    U2Sequence res;

    DBI_TYPE_CHECK(sequenceId, U2Type::Sequence, os, res);
    dbi->getSQLiteObjectDbi()->getObject(res, sequenceId, os);

    CHECK_OP(os, res);

    static const QString queryString("SELECT Sequence.length, Sequence.alphabet, Sequence.circular FROM Sequence WHERE Sequence.object = ?1");
    SQLiteReadQuery q(queryString, db, os);
    q.bindDataId(1, sequenceId);
    if (q.step()) {
        res.length = q.getInt64(0);
        res.alphabet = q.getString(1);
        res.circular = q.getBool(2);
        q.ensureDone();
    } else if (!os.hasError()) {
        os.setError(U2DbiL10n::tr("Sequence object not found."));
    }
    return res;
}

QByteArray SQLiteSequenceDbi::getSequenceData(const U2DataId& sequenceId, const U2Region& region, U2OpStatus& os) {
    try {
        QByteArray res;
        if (0 == region.length) {
            return res;
        } else if (U2_REGION_MAX != region) {
            res.reserve(region.length);
        }
        // Get all chunks that intersect the region
        SQLiteReadQuery q("SELECT sstart, send, data FROM SequenceData WHERE sequence = ?1 "
            "AND  (send >= ?2 AND sstart < ?3) ORDER BY sstart", db, os);

        q.bindDataId(1, sequenceId);
        q.bindInt64(2, region.startPos);
        q.bindInt64(3, region.endPos());
        qint64 pos = region.startPos;
        qint64 regionLengthToRead = region.length;
        while (q.step()) {
            qint64 sstart = q.getInt64(0);
            qint64 send = q.getInt64(1);
            qint64 length = send - sstart;
            QByteArray data = q.getBlob(2);

            int copyStart = pos - sstart;
            int copyLength = static_cast<int>(qMin(regionLengthToRead, length - copyStart));
            res.append(data.constData() + copyStart, copyLength);
            pos += copyLength;
            regionLengthToRead -= copyLength;

            SAFE_POINT_EXT(regionLengthToRead >= 0,
                os.setError("An error occurred during reading sequence data from dbi."),
                QByteArray());
        }
        return res;
    } catch (const std::bad_alloc &) {
#ifdef UGENE_X86
        os.setError("UGENE ran out of memory during the sequence processing. "
            "The 32-bit UGENE version has a restriction on its memory consumption. Try using the 64-bit version instead.");
#else
        os.setError("Out of memory during the sequence processing.");
#endif
        return QByteArray();
    } catch (...) {
        os.setError("Internal error occurred during the sequence processing.");
        coreLog.error("An exception was thrown during reading sequence data from dbi.");
        return QByteArray();
    }
}

void SQLiteSequenceDbi::createSequenceObject(U2Sequence& sequence, const QString& folder, U2OpStatus& os, U2DbiObjectRank rank) {
    SQLiteTransaction t(db, os);
    dbi->getSQLiteObjectDbi()->createObject(sequence, folder, rank, os);
    CHECK_OP(os, );

    static const QString queryString("INSERT INTO Sequence(object, length, alphabet, circular) VALUES(?1, ?2, ?3, ?4)");
    QSharedPointer<SQLiteQuery> q = t.getPreparedQuery(queryString, db, os);
    CHECK_OP(os, );
    q->bindDataId(1, sequence.id);
    q->bindInt64(2, sequence.length);
    q->bindString(3, sequence.alphabet.id);
    q->bindBool(4, sequence.circular);
    q->insert();
    assert(!os.hasError());
}


void SQLiteSequenceDbi::updateSequenceObject(U2Sequence& sequence, U2OpStatus& os) {
    SQLiteTransaction t(db, os);

    static const QString queryString("UPDATE Sequence SET alphabet = ?1, circular = ?2 WHERE object = ?3");
    QSharedPointer<SQLiteQuery> q = t.getPreparedQuery(queryString, db, os);
    CHECK_OP(os, );
    q->bindString(1, sequence.alphabet.id);
    q->bindBool(2, sequence.circular);
    q->bindDataId(3, sequence.id);
    q->execute();

    SAFE_POINT_OP(os, );

    dbi->getSQLiteObjectDbi()->updateObject(sequence, os);
    SAFE_POINT_OP(os, );

    SQLiteObjectDbi::incrementVersion(sequence.id, db, os);
    SAFE_POINT_OP(os, );
}


#define SEQUENCE_CHUNK_SIZE (1024*1024)

static QList<QByteArray> quantify(const QList<QByteArray>& input) {
    QList<QByteArray> res;
    QByteArray currentChunk;
    foreach (const QByteArray& i, input) {
        int bytes2Chunk = SEQUENCE_CHUNK_SIZE - currentChunk.length();
        if (i.length() <= bytes2Chunk) { //if 'i' fits into chunk - just add it
            currentChunk.append(i);
        } else  { // 'i' does not fit into chunk -> split it into separate chunks
            for (int j = 0; j < i.length(); j += bytes2Chunk) {
                if (j > 0) {
                    bytes2Chunk = qMin(SEQUENCE_CHUNK_SIZE, i.length() - j);
                }
                currentChunk.append(i.constData() + j, bytes2Chunk);
                if (currentChunk.length() == SEQUENCE_CHUNK_SIZE) {
                    res.append(currentChunk);
                    currentChunk.clear();
                }
            }
        }
        if (currentChunk.length() == SEQUENCE_CHUNK_SIZE) {
            res.append(currentChunk);
            currentChunk.clear();
        }
    }
    if (!currentChunk.isEmpty())  {
        res.append(currentChunk);
    }
    return res;
}

void SQLiteSequenceDbi::updateSequenceData(const U2DataId& sequenceId, const U2Region& regionToReplace, const QByteArray& dataToInsert, const QVariantMap &hints, U2OpStatus& os) {
    updateSequenceData(sequenceId, sequenceId, regionToReplace, dataToInsert, hints, os);
}

void SQLiteSequenceDbi::updateSequenceData(const U2DataId &masterId, const U2DataId &sequenceId, const U2Region &regionToReplace, const QByteArray &dataToInsert, const QVariantMap &hints, U2OpStatus &os) {
    SQLiteTransaction t(db, os);

    SQLiteModificationAction updateAction(dbi, masterId);
    updateAction.prepare(os);
    SAFE_POINT_OP(os, );

    updateSequenceData(updateAction, sequenceId, regionToReplace, dataToInsert, hints, os);
    SAFE_POINT_OP(os, );

    updateAction.complete(os);
    SAFE_POINT_OP(os, );
}

void SQLiteSequenceDbi::updateSequenceData(SQLiteModificationAction& updateAction, const U2DataId& sequenceId, const U2Region& regionToReplace,
    const QByteArray& dataToInsert, const QVariantMap &hints, U2OpStatus& os)
{
    QByteArray modDetails;
    if (TrackOnUpdate == updateAction.getTrackModType()) {
        QByteArray oldSeq = dbi->getSequenceDbi()->getSequenceData(sequenceId, regionToReplace, os);
        SAFE_POINT_OP(os, );
        modDetails = U2DbiPackUtils::packSequenceDataDetails(regionToReplace, oldSeq, dataToInsert, hints);
    }

    updateSequenceDataCore(sequenceId, regionToReplace, dataToInsert, hints, os);
    SAFE_POINT_OP(os, );

    // Track the modification, if required; add the object to the list (versions of the objects will be incremented on the updateAction completion)
    updateAction.addModification(sequenceId, U2ModType::sequenceUpdatedData, modDetails, os);
    SAFE_POINT_OP(os, );
}

void SQLiteSequenceDbi::undo(const U2DataId& seqId, qint64 modType, const QByteArray& modDetails, U2OpStatus& os) {
    if (U2ModType::sequenceUpdatedData == modType) {
        undoUpdateSequenceData(seqId, modDetails, os);
    }
    else {
        os.setError(QString("Unexpected modification type '%1'!").arg(QString::number(modType)));
        return;
    }
}

void SQLiteSequenceDbi::redo(const U2DataId& seqId, qint64 modType, const QByteArray& modDetails, U2OpStatus& os) {
    if (U2ModType::sequenceUpdatedData == modType) {
        redoUpdateSequenceData(seqId, modDetails, os);
    }
    else {
        os.setError(QString("Unexpected modification type '%1'!").arg(QString::number(modType)));
        return;
    }
}

/************************************************************************/
/* Core methods */
/************************************************************************/
void SQLiteSequenceDbi::updateSequenceDataCore(const U2DataId& sequenceId, const U2Region& regionToReplace, const QByteArray& dataToInsert, const QVariantMap &hints, U2OpStatus& os) {
    bool updateLenght = hints.value(U2SequenceDbiHints::UPDATE_SEQUENCE_LENGTH, true).toBool();
    bool emptySequence = hints.value(U2SequenceDbiHints::EMPTY_SEQUENCE, false).toBool();
    SQLiteTransaction t(db, os);

    //algorithm:
        // find all regions affected -> remove them
        // construct new regions from cuts from old regions and new dataToInsert
        // remove affected annotations or adjust their locations if possible

    // find cropped parts
    QByteArray leftCrop, rightCrop;
    qint64 cropLeftPos = -1;
    if (!emptySequence) {
        static const QString leftString("SELECT sstart FROM SequenceData WHERE sequence = ?1 AND send >= ?2 AND sstart <= ?2");
        QSharedPointer<SQLiteQuery> leftQ = t.getPreparedQuery(leftString, db, os);
        CHECK_OP(os, );
        leftQ->bindDataId(1, sequenceId);
        leftQ->bindInt64(2, regionToReplace.startPos);
        cropLeftPos = leftQ->selectInt64(-1);
        if (os.hasError()) {
            return;
        }
    }
    emptySequence = (-1 == cropLeftPos);
    if (emptySequence) {
        cropLeftPos = 0;
    } else {
        if (cropLeftPos >= 0) {
            leftCrop = getSequenceData(sequenceId, U2Region(cropLeftPos, regionToReplace.startPos - cropLeftPos), os);
        } else {
            cropLeftPos = 0;
        }

        static const QString rightString("SELECT send FROM SequenceData WHERE sequence = ?1 AND send >= ?2 AND sstart <= ?2");
        QSharedPointer<SQLiteQuery> rightQ = t.getPreparedQuery(rightString, db, os);
        CHECK_OP(os, );
        rightQ->bindDataId(1, sequenceId);
        rightQ->bindInt64(2, regionToReplace.endPos());
        qint64 cropRightPos = rightQ->selectInt64(-1);
        if (os.hasError()) {
            return;
        }
        if (cropRightPos > 0) {
            rightCrop = getSequenceData(sequenceId, U2Region(regionToReplace.endPos(), cropRightPos - regionToReplace.endPos()), os);
        }

        // remove all affected regions
        static const QString removeString("DELETE FROM SequenceData WHERE sequence = ?1 "
            " AND ((sstart >= ?2 AND sstart < ?3) OR (?2 >= sstart  AND send >= ?2))");
        QSharedPointer<SQLiteQuery> removeQ = t.getPreparedQuery(removeString, db, os);
        CHECK_OP(os, );
        removeQ->bindDataId(1, sequenceId);
        removeQ->bindInt64(2, regionToReplace.startPos);
        removeQ->bindInt64(3, regionToReplace.endPos());
        removeQ->execute();
        if (os.hasError()) {
            return;
        }

        // now adjust 'sstart' & 'send' for all sequence regions on the right side of the insert
        if (dataToInsert.length() != regionToReplace.length) {
            qint64 d = dataToInsert.length() - regionToReplace.length;
            static const QString updateString("UPDATE SequenceData SET sstart = sstart + ?1, send = send + ?1 WHERE sequence = ?2 AND sstart >= ?3");
            QSharedPointer<SQLiteQuery> updateQ = t.getPreparedQuery(updateString, db, os);
            CHECK_OP(os, );
            updateQ->bindInt64(1, d);
            updateQ->bindDataId(2, sequenceId);
            updateQ->bindInt64(3, regionToReplace.endPos());
            updateQ->execute();
            if (os.hasError()) {
                return;
            }
        }
    }
    // insert new regions
    QList<QByteArray> newDataToInsert = quantify(QList<QByteArray>() << leftCrop << dataToInsert << rightCrop);
    static const QString insertString("INSERT INTO SequenceData(sequence, sstart, send, data) VALUES(?1, ?2, ?3, ?4)");
    QSharedPointer<SQLiteQuery> insertQ = t.getPreparedQuery(insertString, db, os);
    CHECK_OP(os, );
    qint64 startPos = cropLeftPos;
    foreach(const QByteArray& d, newDataToInsert) {
        insertQ->reset();
        insertQ->bindDataId(1, sequenceId);
        insertQ->bindInt64(2, startPos);
        insertQ->bindInt64(3, startPos + d.length());
        insertQ->bindBlob(4, d);
        insertQ->execute();
        if (os.hasError()) {
            return;
        }
        startPos += d.length();
    }
    // Update sequence object length;
    qint64 newLength;
    if (emptySequence) {
        newLength = dataToInsert.length();
    } else {
        static const QString lengthString("SELECT MAX(send) FROM SequenceData WHERE sequence = ?1");
        QSharedPointer<SQLiteQuery> lengthQ = t.getPreparedQuery(lengthString, db, os);
        CHECK_OP(os, );
        lengthQ->bindDataId(1, sequenceId);
        newLength = lengthQ->selectInt64();
        if (os.hasError()) {
            return;
        }
    }

    if (updateLenght) {
        static const QString updateLengthString("UPDATE Sequence SET length = ?1 WHERE object = ?2");
        QSharedPointer<SQLiteQuery> updateLengthQuery = t.getPreparedQuery(updateLengthString, db, os);
        CHECK_OP(os, );
        updateLengthQuery->bindInt64(1, newLength);
        updateLengthQuery->bindDataId(2, sequenceId);
        updateLengthQuery->update(1);
        if (os.hasError()) {
            return;
        }
    }
}

/************************************************************************/
/* Undo/redo methods */
/************************************************************************/
void SQLiteSequenceDbi::undoUpdateSequenceData(const U2DataId& sequenceId, const QByteArray& modDetails, U2OpStatus& os) {
    U2Region replacedRegion;
    U2Region replacedByRegion;
    QByteArray oldData;
    QByteArray newData;
    QVariantMap hints;
    bool ok = U2DbiPackUtils::unpackSequenceDataDetails(modDetails, replacedRegion, oldData, newData, hints);
    if (!ok) {
        os.setError("An error occurred during reverting replacing sequence data!");
        return;
    }
    hints.remove(U2SequenceDbiHints::EMPTY_SEQUENCE);

    replacedByRegion = U2Region(replacedRegion.startPos, newData.length());

    updateSequenceDataCore(sequenceId, replacedByRegion, oldData, hints, os);
}

void SQLiteSequenceDbi::redoUpdateSequenceData(const U2DataId& sequenceId, const QByteArray& modDetails, U2OpStatus& os) {
    U2Region replacedRegion;
    U2Region replacedByRegion;
    QByteArray oldData;
    QByteArray newData;
    QVariantMap hints;
    bool ok = U2DbiPackUtils::unpackSequenceDataDetails(modDetails, replacedRegion, oldData, newData, hints);
    if (!ok) {
        os.setError("An error occurred during replacing sequence data!");
        return;
    }

    replacedByRegion = U2Region(replacedRegion.startPos, newData.length());

    updateSequenceDataCore(sequenceId, replacedRegion, newData, hints, os);
}
} //namespace
