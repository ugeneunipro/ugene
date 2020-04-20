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
#include <U2Core/U2SequenceUtils.h>

#include "MysqlObjectDbi.h"
#include "MysqlSequenceDbi.h"
#include "util/MysqlHelpers.h"
#include "util/MysqlModificationAction.h"

namespace U2 {

MysqlSequenceDbi::MysqlSequenceDbi(MysqlDbi* dbi) :
    U2SequenceDbi(dbi),
    MysqlChildDbiCommon(dbi)
{
}

void MysqlSequenceDbi::initSqlSchema(U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    // sequence object
    U2SqlQuery("CREATE TABLE Sequence (object BIGINT PRIMARY KEY, length BIGINT NOT NULL DEFAULT 0, alphabet TEXT NOT NULL, "
                            "circular TINYINT NOT NULL DEFAULT 0, "
                             "FOREIGN KEY(object) REFERENCES Object(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();

    // part of the sequence, starting with 'sstart'(inclusive) and ending at 'send'(not inclusive)
    U2SqlQuery("CREATE TABLE SequenceData (sequence BIGINT, sstart BIGINT NOT NULL, send BIGINT NOT NULL, data LONGBLOB NOT NULL, "
        "FOREIGN KEY(sequence) REFERENCES Sequence(object) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();

    U2SqlQuery("CREATE INDEX SequenceData_sequence_region on SequenceData(sstart, send)", db, os).execute();
}

U2Sequence MysqlSequenceDbi::getSequenceObject(const U2DataId& sequenceId, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    U2Sequence res;
    DBI_TYPE_CHECK(sequenceId, U2Type::Sequence, os, res);
    dbi->getMysqlObjectDbi()->getObject(res, sequenceId, os);
    CHECK_OP(os, res);

    static const QString queryString = "SELECT length, alphabet, circular FROM Sequence WHERE object = :object";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":object", sequenceId);
    if (q.step()) {
        res.length = q.getInt64(0);
        res.alphabet = q.getString(1);
        res.circular = q.getBool(2);
        q.ensureDone();
    } else if (!os.hasError()) {
        os.setError(U2DbiL10n::tr("Sequence object not found"));
    }

    return res;
}

QByteArray MysqlSequenceDbi::getSequenceData(const U2DataId& sequenceId, const U2Region& region, U2OpStatus& os) {
    try {
        QByteArray res;

        if (0 == region.length) {
            return res;
        } else if (U2_REGION_MAX != region) {
            res.reserve(region.length);
        }

        // Get all chunks that intersect the region
        static const QString queryString = "SELECT sstart, send, data FROM SequenceData "
            "WHERE sequence = :id AND send >= :startPos AND sstart < :endPos ORDER BY sstart";
        U2SqlQuery q(queryString, db, os);

        q.bindDataId(":id", sequenceId);
        q.bindInt64(":startPos", region.startPos);
        q.bindInt64(":endPos", region.endPos());
        qint64 pos = region.startPos;
        qint64 regionLengthToRead = region.length;
        while (q.step()) {
            qint64 sstart = q.getInt64(0);
            qint64 send = q.getInt64(1);
            qint64 length = send - sstart;
            QByteArray data = q.getBlob(2);
            CHECK_OP(os, QByteArray());

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
        os.setError(U2DbiL10n::tr("Internal error occurred during the sequence processing"));
        coreLog.error(U2DbiL10n::tr("An exception was thrown during reading sequence data from dbi"));
        return QByteArray();
    }
}

void MysqlSequenceDbi::createSequenceObject(U2Sequence& sequence, const QString& folder, U2OpStatus& os, U2DbiObjectRank rank) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    dbi->getMysqlObjectDbi()->createObject(sequence, folder, rank, os);
    CHECK_OP(os, );

    static const QString queryString = "INSERT INTO Sequence(object, length, alphabet, circular) VALUES(:object, :length, :alphabet, :circular)";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":object", sequence.id);
    q.bindInt64(":length", sequence.length);
    q.bindString(":alphabet", sequence.alphabet.id);
    q.bindBool(":circular", sequence.circular);
    q.insert();
}

void MysqlSequenceDbi::updateSequenceObject(U2Sequence& sequence, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString = "UPDATE Sequence SET alphabet = :alphabet, circular = :circular WHERE object = :object";
    U2SqlQuery q(queryString, db, os);
    q.bindString(":alphabet", sequence.alphabet.id);
    q.bindBool(":circular", sequence.circular);
    q.bindDataId(":object", sequence.id);
    q.execute();
    CHECK_OP(os, );

    dbi->getMysqlObjectDbi()->updateObject(sequence, os);
    CHECK_OP(os, );

    MysqlObjectDbi::incrementVersion(sequence.id, db, os);
}

namespace {

#define SEQUENCE_CHUNK_SIZE (64 * 1024)

static QList<QByteArray> quantify(const QList<QByteArray>& input) {
    QList<QByteArray> res;
    QByteArray currentChunk;

    foreach (const QByteArray& i, input) {
        int bytes2Chunk = SEQUENCE_CHUNK_SIZE - currentChunk.length();
        if (i.length() <= bytes2Chunk) {
            // if 'i' fits into chunk - just add it
            currentChunk.append(i);
        } else {
            // 'i' does not fit into chunk -> split it into separate chunks
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

}   // unnamed namespace

void MysqlSequenceDbi::updateSequenceData(const U2DataId& sequenceId, const U2Region& regionToReplace, const QByteArray& dataToInsert, const QVariantMap &hints, U2OpStatus& os) {
    updateSequenceData(sequenceId, sequenceId, regionToReplace, dataToInsert, hints, os);
}

void MysqlSequenceDbi::updateSequenceData(const U2DataId &masterId, const U2DataId &sequenceId, const U2Region &regionToReplace, const QByteArray &dataToInsert, const QVariantMap &hints, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    MysqlModificationAction updateAction(dbi, masterId);
    updateAction.prepare(os);
    CHECK_OP(os, );

    updateSequenceData(updateAction, sequenceId, regionToReplace, dataToInsert, hints, os);
    CHECK_OP(os, );

    updateAction.complete(os);
}

void MysqlSequenceDbi::updateSequenceData(MysqlModificationAction& updateAction, const U2DataId& sequenceId, const U2Region& regionToReplace,
    const QByteArray& dataToInsert, const QVariantMap &hints, U2OpStatus& os)
{
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    QByteArray modDetails;
    if (TrackOnUpdate == updateAction.getTrackModType()) {
        QByteArray oldSeq = dbi->getSequenceDbi()->getSequenceData(sequenceId, regionToReplace, os);
        CHECK_OP(os, );
        modDetails = U2DbiPackUtils::packSequenceDataDetails(regionToReplace, oldSeq, dataToInsert, hints);
    }

    updateSequenceDataCore(sequenceId, regionToReplace, dataToInsert, hints, os);
    CHECK_OP(os, );

    // Track the modification, if required; add the object to the list (versions of the objects will be incremented on the updateAction completion)
    updateAction.addModification(sequenceId, U2ModType::sequenceUpdatedData, modDetails, os);
}

void MysqlSequenceDbi::undo(const U2DataId& seqId, qint64 modType, const QByteArray& modDetails, U2OpStatus& os) {
    if (U2ModType::sequenceUpdatedData == modType) {
        undoUpdateSequenceData(seqId, modDetails, os);
    } else {
        os.setError(QString("Unexpected modification type '%1'!").arg(QString::number(modType)));
        return;
    }
}

void MysqlSequenceDbi::redo(const U2DataId& seqId, qint64 modType, const QByteArray& modDetails, U2OpStatus& os) {
    if (U2ModType::sequenceUpdatedData == modType) {
        redoUpdateSequenceData(seqId, modDetails, os);
    } else {
        os.setError(QString("Unexpected modification type '%1'!").arg(QString::number(modType)));
        return;
    }
}

/************************************************************************/
/* Core methods */
/************************************************************************/
void MysqlSequenceDbi::updateSequenceDataCore(const U2DataId& sequenceId, const U2Region& regionToReplace, const QByteArray& dataToInsert, const QVariantMap &hints, U2OpStatus& os) {
    bool updateLenght = hints.value(U2SequenceDbiHints::UPDATE_SEQUENCE_LENGTH, true).toBool();
    bool emptySequence = hints.value(U2SequenceDbiHints::EMPTY_SEQUENCE, false).toBool();
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    //algorithm:
        // find all regions affected -> remove them
        // construct new regions from cuts from old regions and new dataToInsert
        // remove affected annotations or adjust their locations if possible

    // find cropped parts
    QByteArray leftCrop;
    QByteArray rightCrop;
    qint64 cropLeftPos = -1;
    if (!emptySequence) {
        static const QString leftString = "SELECT sstart FROM SequenceData WHERE sequence = :id AND sstart <= :startPos1 AND :startPos2 <= send";
        U2SqlQuery leftQ(leftString, db, os);
        leftQ.bindDataId(":id", sequenceId);
        leftQ.bindInt64(":startPos1", regionToReplace.startPos);
        leftQ.bindInt64(":startPos2", regionToReplace.startPos);
        cropLeftPos = leftQ.selectInt64(-1);
        CHECK_OP(os, );
    }

    emptySequence = (-1 == cropLeftPos);
    if (emptySequence) {
        cropLeftPos = 0;
    } else {
        if (cropLeftPos >= 0) {
            leftCrop = getSequenceData(sequenceId, U2Region(cropLeftPos, regionToReplace.startPos - cropLeftPos), os);
            CHECK_OP(os, );
        } else {
            cropLeftPos = 0;
        }

        static const QString rightString = "SELECT send FROM SequenceData WHERE sequence = :sequence AND sstart <= :endPos1 AND :endPos2 <= send";
        U2SqlQuery rightQ(rightString, db, os);
        rightQ.bindDataId(":sequence", sequenceId);
        rightQ.bindInt64(":endPos1", regionToReplace.endPos());
        rightQ.bindInt64(":endPos2", regionToReplace.endPos());
        qint64 cropRightPos = rightQ.selectInt64(-1);
        CHECK_OP(os, );

        if (cropRightPos > 0) {
            rightCrop = getSequenceData(sequenceId, U2Region(regionToReplace.endPos(), cropRightPos - regionToReplace.endPos()), os);
            CHECK_OP(os, );
        }

        // remove all affected regions
        static const QString removeString = "DELETE FROM SequenceData WHERE sequence = :sequence AND ((:startPos1 <= sstart AND sstart < :endPos) OR (sstart <= :startPos2 AND :startPos3 <= send))";
        U2SqlQuery removeQ(removeString, db, os);
        removeQ.bindDataId(":sequence", sequenceId);
        removeQ.bindInt64(":startPos1", regionToReplace.startPos);
        removeQ.bindInt64(":startPos2", regionToReplace.startPos);
        removeQ.bindInt64(":startPos3", regionToReplace.startPos);
        removeQ.bindInt64(":endPos", regionToReplace.endPos());
        removeQ.execute();
        CHECK_OP(os, );

        // now adjust 'sstart' & 'send' for all sequence regions on the right side of the insert
        if (dataToInsert.length() != regionToReplace.length) {
            qint64 d = dataToInsert.length() - regionToReplace.length;

            static const QString updateString = "UPDATE SequenceData SET sstart = sstart + :dataLength1, send = send + :dataLength2 WHERE sequence = :sequence AND sstart >= :endPos";
            U2SqlQuery updateQ(updateString, db, os);
            updateQ.bindInt64(":dataLength1", d);
            updateQ.bindInt64(":dataLength2", d);
            updateQ.bindDataId(":sequence", sequenceId);
            updateQ.bindInt64(":endPos", regionToReplace.endPos());
            updateQ.execute();
            CHECK_OP(os, );
        }
    }

    // insert new regions
    QList<QByteArray> newDataToInsert = quantify(QList<QByteArray>() << leftCrop << dataToInsert << rightCrop);
    qint64 startPos = cropLeftPos;

    static const QString insertString = "INSERT INTO SequenceData(sequence, sstart, send, data) VALUES(:sequence, :sstart, :send, :data)";
    U2SqlQuery insertQ(insertString, db, os);
    foreach (const QByteArray& d, newDataToInsert) {
        insertQ.bindDataId(":sequence", sequenceId);
        insertQ.bindInt64(":sstart", startPos);
        insertQ.bindInt64(":send", startPos + d.length());
        insertQ.bindBlob(":data", d);
        insertQ.execute();
        CHECK_OP(os, );

        startPos += d.length();
    }

    // Update sequence object length;
    qint64 newLength;
    if (emptySequence) {
        newLength = dataToInsert.length();
    } else {
        static const QString lengthString = "SELECT MAX(send) FROM SequenceData WHERE sequence = :sequence";
        U2SqlQuery lengthQ(lengthString, db, os);
        lengthQ.bindDataId(":sequence", sequenceId);
        newLength = lengthQ.selectInt64();
        CHECK_OP(os, );
    }

    if (updateLenght) {
        static const QString updateLengthString = "UPDATE Sequence SET length = :length WHERE object = :object";
        U2SqlQuery updateLengthQuery(updateLengthString, db, os);
        updateLengthQuery.bindInt64(":length", newLength);
        updateLengthQuery.bindDataId(":object", sequenceId);
        updateLengthQuery.update();
        CHECK_OP(os, );
    }
}

/************************************************************************/
/* Undo/redo methods */
/************************************************************************/
void MysqlSequenceDbi::undoUpdateSequenceData(const U2DataId& sequenceId, const QByteArray& modDetails, U2OpStatus& os) {
    U2Region replacedRegion;
    U2Region replacedByRegion;
    QByteArray oldData;
    QByteArray newData;
    QVariantMap hints;

    bool ok = U2DbiPackUtils::unpackSequenceDataDetails(modDetails, replacedRegion, oldData, newData, hints);
    CHECK_EXT(ok, os.setError(U2DbiL10n::tr("An error occurred during reverting replacing sequence data")), );

    hints.remove(U2SequenceDbiHints::EMPTY_SEQUENCE);
    replacedByRegion = U2Region(replacedRegion.startPos, newData.length());
    updateSequenceDataCore(sequenceId, replacedByRegion, oldData, hints, os);
}

void MysqlSequenceDbi::redoUpdateSequenceData(const U2DataId& sequenceId, const QByteArray& modDetails, U2OpStatus& os) {
    U2Region replacedRegion;
    U2Region replacedByRegion;
    QByteArray oldData;
    QByteArray newData;
    QVariantMap hints;

    bool ok = U2DbiPackUtils::unpackSequenceDataDetails(modDetails, replacedRegion, oldData, newData, hints);
    CHECK_EXT(ok, os.setError(U2DbiL10n::tr("An error occurred during replacing sequence data")), );

    replacedByRegion = U2Region(replacedRegion.startPos, newData.length());
    updateSequenceDataCore(sequenceId, replacedRegion, newData, hints, os);
}

}   // namespace U2
