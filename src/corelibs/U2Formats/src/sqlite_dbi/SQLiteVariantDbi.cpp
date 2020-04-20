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

#include "SQLiteVariantDbi.h"
#include "SQLiteObjectDbi.h"

#include <U2Core/U2SqlHelpers.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

SQLiteVariantDbi::SQLiteVariantDbi(SQLiteDbi* dbi) : U2VariantDbi(dbi), SQLiteChildDBICommon(dbi) {
}

void SQLiteVariantDbi::initSqlSchema(U2OpStatus& os) {
    if (os.hasError()) {
        return;
    }
    // Variant track object
    SQLiteWriteQuery(" CREATE TABLE VariantTrack (object INTEGER PRIMARY KEY, sequence INTEGER, "
        "sequenceName TEXT NOT NULL, trackType INTEGER DEFAULT 1, fileHeader TEXT, "
        "FOREIGN KEY(object) REFERENCES Object(id) ON DELETE CASCADE)", db, os).execute();

    // Variant element
    // track - Variant track object id
    // startPos - variation start position
    // endPos - variation end position
    // refData - reference sequence part
    // obsData - observed variation of the reference
    // comment - comment visible for user
    // publicId - identifier visible for user
    // additionalInfo - added for vcf4 support
    SQLiteWriteQuery("CREATE TABLE Variant(id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
        "track INTEGER, startPos INTEGER, endPos INTEGER, refData BLOB NOT NULL, "
        "obsData BLOB NOT NULL, publicId TEXT NOT NULL, additionalInfo TEXT, "
        "FOREIGN KEY(track) REFERENCES VariantTrack(object) ON DELETE CASCADE)", db, os).execute();
}

U2VariantTrack SQLiteVariantDbi::getVariantTrack(const U2DataId& variantTrackId, U2OpStatus& os) {
    U2VariantTrack res;

    DBI_TYPE_CHECK(variantTrackId, U2Type::VariantTrack, os, res);

    dbi->getSQLiteObjectDbi()->getObject(res, variantTrackId, os);
    CHECK_OP(os, res);

    SQLiteReadQuery q("SELECT sequence, sequenceName, trackType, fileHeader FROM VariantTrack WHERE object = ?1", db, os);
    q.bindDataId(1, variantTrackId);
    if (q.step())  {
        res.sequence = q.getDataId(0, U2Type::Sequence);
        res.sequenceName = q.getString(1);
        res.trackType = (VariantTrackType)q.getInt32(2);
        res.fileHeader = q.getString(3);
        q.ensureDone();
    }
    return res;
}
U2VariantTrack SQLiteVariantDbi::getVariantTrackofVariant( const U2DataId& variantId, U2OpStatus& os ){
    U2VariantTrack res;

    DBI_TYPE_CHECK(variantId, U2Type::VariantType, os, res);

    SQLiteReadQuery q("SELECT track FROM Variant WHERE id = ?1", db, os);
    q.bindDataId(1, variantId);

    if(q.step()){
        U2DataId trackId = q.getDataId(0, U2Type::VariantTrack);

        res = getVariantTrack(trackId, os);
    }

    return res;
}


void SQLiteVariantDbi::addVariantsToTrack(const U2VariantTrack& track, U2DbiIterator<U2Variant>* it, U2OpStatus& os){
    if (track.sequenceName.isEmpty()) {
        os.setError(U2DbiL10n::tr("Sequence name is not set!"));
        return;
    }

    SQLiteTransaction t(db, os);

    QSharedPointer<SQLiteQuery> q2 = t.getPreparedQuery("INSERT INTO Variant(track, startPos, endPos, refData, obsData, publicId, additionalInfo) \
        VALUES(?1, ?2, ?3, ?4, ?5, ?6, ?7)", db, os);
    while (it->hasNext() && !os.isCoR()) {
        U2Variant var = it->next();
        q2->reset();
        q2->bindDataId(1, track.id);
        q2->bindInt64(2, var.startPos);
        q2->bindInt64(3, var.endPos);
        q2->bindBlob(4, var.refData);
        q2->bindBlob(5, var.obsData);
        q2->bindString(6, var.publicId);
        q2->bindString(7, StrPackUtils::packMap(var.additionalInfo));

        var.id = q2->insert(U2Type::VariantType);
        SAFE_POINT_OP(os,);
    }

}


void SQLiteVariantDbi::createVariationsIndex( U2OpStatus& os ){
    SQLiteWriteQuery("CREATE INDEX IF NOT EXISTS VariantIndex ON Variant(track)" ,db, os).execute();
    SQLiteWriteQuery("CREATE INDEX IF NOT EXISTS VariantIndexstartPos ON Variant(startPos)" ,db, os).execute();
}


void SQLiteVariantDbi::createVariantTrack(U2VariantTrack& track, VariantTrackType trackType, const QString& folder, U2OpStatus& os){
    if (track.sequenceName.isEmpty()) {
        os.setError(U2DbiL10n::tr("Sequence name is not set!"));
        return;
    }
    //SQLiteTransaction t(db, os);

    dbi->getSQLiteObjectDbi()->createObject(track, folder, U2DbiObjectRank_TopLevel, os);
    SAFE_POINT_OP(os,);

    track.trackType = trackType;

    SQLiteWriteQuery q1("INSERT INTO VariantTrack(object, sequence, sequenceName, trackType, fileHeader) VALUES(?1, ?2, ?3, ?4, ?5)", db, os);
    q1.bindDataId(1, track.id);
    q1.bindDataId(2, track.sequence);
    q1.bindString(3, track.sequenceName);
    q1.bindInt32(4, track.trackType);
    q1.bindString(5, track.fileHeader);

    q1.execute();
    SAFE_POINT_OP(os,);
}

void SQLiteVariantDbi::updateVariantTrack(U2VariantTrack& track, U2OpStatus& os) {
    SQLiteWriteQuery q("UPDATE VariantTrack SET sequence = ?1, sequenceName = ?2, trackType = ?3, fileHeader = ?4 WHERE object = ?5", db, os);
    q.bindDataId(1, track.sequence);
    q.bindString(2, track.sequenceName);
    q.bindInt32(3, track.trackType);
    q.bindString(4, track.fileHeader);
    q.bindDataId(5, track.id);
    q.execute();
    CHECK_OP(os, );

    dbi->getSQLiteObjectDbi()->updateObject(track, os);
    CHECK_OP(os, );

    SQLiteObjectDbi::incrementVersion(track.id, db, os);
    CHECK_OP(os, );
}

class SqliteVariantLoader: public SQLiteResultSetLoader<U2Variant> {
public:
    U2Variant load(SQLiteQuery* q) {
        U2Variant res;
        res.id = q->getDataId(0, U2Type::VariantType);
        res.startPos = q->getInt64(1);
        res.endPos =  q->getInt64(2);
        res.refData = q->getBlob(3);
        res.obsData = q->getBlob(4);
        res.publicId = q->getString(5);
        res.additionalInfo = StrPackUtils::unpackMap(q->getString(6));
        return res;
    }
};

U2DbiIterator<U2Variant>* SQLiteVariantDbi::getVariants(const U2DataId& trackId, const U2Region& region, U2OpStatus& os) {
    if (region == U2_REGION_MAX) {
        static QString queryString ("SELECT id, startPos, endPos, refData, obsData, publicId, additionalInfo FROM Variant WHERE track = ?1 ORDER BY startPos");
        QSharedPointer<SQLiteReadQuery> q (new SQLiteReadQuery(queryString, db, os));
        q->bindDataId(1, trackId);
        return new SQLiteResultSetIterator<U2Variant>(q, new SqliteVariantLoader(), NULL, U2Variant(), os);
    }

    QSharedPointer<SQLiteReadQuery> q (new SQLiteReadQuery("SELECT id, startPos, endPos, refData, obsData, publicId, additionalInfo FROM Variant \
                                                                                            WHERE track = ?1 AND startPos >= ?2 AND startPos <?3", db, os));
    q->bindDataId(1, trackId);
    q->bindInt64(2, region.startPos);
    q->bindInt64(3, region.endPos());
    return new SQLiteResultSetIterator<U2Variant>(q, new SqliteVariantLoader(), NULL, U2Variant(), os);
}

class SimpleVariantTrackLoader : public SQLiteResultSetLoader<U2VariantTrack> {
    U2VariantTrack load(SQLiteQuery* q) {
        U2VariantTrack track;
        track.id = q->getDataId(0, U2Type::VariantTrack);
        track.sequence = q->getDataId(1,U2Type::Sequence);
        track.sequenceName = q->getString(2);
        track.trackType = (VariantTrackType)q->getInt32(3);
        track.fileHeader = q->getString(4);
        return track;
    }
};
class SimpleVariantTrackFilter : public SQLiteResultSetFilter<U2VariantTrack> {
public:
    SimpleVariantTrackFilter(VariantTrackType _trackType){
        trackType = _trackType;
    }
    bool filter(const U2VariantTrack& track){
        if (trackType == TrackType_All || trackType == track.trackType){
            return true;
        }
        return false;
    }

private:
    VariantTrackType trackType;
};

U2DbiIterator<U2VariantTrack>* SQLiteVariantDbi::getVariantTracks(VariantTrackType trackType, U2OpStatus& os )
{
    QSharedPointer<SQLiteReadQuery> q (new SQLiteReadQuery("SELECT object, sequence, sequenceName, trackType, fileHeader FROM VariantTrack", db, os));
    return new SQLiteResultSetIterator<U2VariantTrack>(q, new SimpleVariantTrackLoader(), new SimpleVariantTrackFilter(trackType), U2VariantTrack(), os);
}

U2DbiIterator<U2VariantTrack>* SQLiteVariantDbi::getVariantTracks(const U2DataId& seqId, U2OpStatus& os){
    QSharedPointer<SQLiteReadQuery> q (new SQLiteReadQuery("SELECT object, sequence, sequenceName, trackType, fileHeader FROM VariantTrack WHERE sequence = ?1 ", db, os));

    q->bindDataId(1, seqId);

    return new SQLiteResultSetIterator<U2VariantTrack>(q, new SimpleVariantTrackLoader(), NULL, U2VariantTrack(), os);
}

U2DbiIterator<U2VariantTrack>* SQLiteVariantDbi::getVariantTracks( const U2DataId& seqId, VariantTrackType trackType, U2OpStatus& os ){
    if(trackType == TrackType_All){
        return getVariantTracks(seqId, os);
    }

    QSharedPointer<SQLiteReadQuery> q (new SQLiteReadQuery("SELECT object, sequence, sequenceName FROM VariantTrack WHERE sequence = ?1 ", db, os));

    q->bindDataId(1, seqId);

    return new SQLiteResultSetIterator<U2VariantTrack>(q, new SimpleVariantTrackLoader(), new SimpleVariantTrackFilter(trackType), U2VariantTrack(), os);
}


U2DbiIterator<U2Variant>* SQLiteVariantDbi::getVariantsRange(const U2DataId& track, int offset, int limit, U2OpStatus& os )
{
    QSharedPointer<SQLiteReadQuery> q  (new SQLiteReadQuery("SELECT id, startPos, endPos, refData, obsData, publicId, additionalInfo FROM Variant \
                                                                          WHERE track = ?1 LIMIT ?2 OFFSET ?3" , db, os));
    q->bindDataId(1, track);
    q->bindInt64(2, limit);
    q->bindInt64(3, offset);
    return new SQLiteResultSetIterator<U2Variant>(q, new SqliteVariantLoader(), NULL, U2Variant(), os);
}

int SQLiteVariantDbi::getVariantCount( const U2DataId& trackId, U2OpStatus& os )
{
    SQLiteReadQuery q("SELECT COUNT(*) FROM Variant WHERE track = ?1 " , db, os);
    q.bindDataId(1, trackId);
    if (!q.step()) {
        return -1;
    }

    return q.getInt32(0);
}

void SQLiteVariantDbi::removeTrack(const U2DataId& trackId, U2OpStatus& os){
    SQLiteWriteQuery q1("DELETE FROM Variant WHERE track = ?1", db, os);
    q1.bindDataId(1, trackId);
    q1.execute();
    SAFE_POINT_OP(os,);

    SQLiteWriteQuery q2("DELETE FROM VariantTrack WHERE object = ?1", db, os);
    q2.bindDataId(1, trackId);
    q2.execute();
    SAFE_POINT_OP(os,);
}

void SQLiteVariantDbi::updateVariantPublicId( const U2DataId& track, const U2DataId& variant, const QString& newId, U2OpStatus& os ){
    SQLiteTransaction t(db, os);
    DBI_TYPE_CHECK(track, U2Type::VariantTrack, os, );
    DBI_TYPE_CHECK(variant, U2Type::VariantType, os, );
    if (newId.isEmpty()){
        return;
    }

    static QString qvString("UPDATE Variant SET publicId = ?1 WHERE track = ?2 AND id = ?3");
    QSharedPointer<SQLiteQuery> qv(t.getPreparedQuery(qvString, db, os));
    qv->bindString(1, newId);
    qv->bindDataId(2, track);
    qv->bindDataId(3, variant);
    qv->execute();
    CHECK_OP(os, );

}

void SQLiteVariantDbi::updateTrackIDofVariant( const U2DataId& variant, const U2DataId& newTrackId, U2OpStatus& os ){
    SQLiteTransaction t(db, os);
    DBI_TYPE_CHECK(newTrackId, U2Type::VariantTrack, os, );
    DBI_TYPE_CHECK(variant, U2Type::VariantType, os, );
    if (newTrackId.isEmpty()){
        return;
    }

    static QString qvString("UPDATE Variant SET track = ?2 WHERE id = ?1");
    QSharedPointer<SQLiteQuery> qv(t.getPreparedQuery(qvString, db, os));

    qv->bindDataId(1, variant);
    qv->bindDataId(2, newTrackId);
    qv->execute();
    CHECK_OP(os, );
}

} //namespace
