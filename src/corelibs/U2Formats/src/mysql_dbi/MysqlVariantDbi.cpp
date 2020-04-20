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

#include <U2Core/U2SqlHelpers.h>
#include <U2Core/U2SafePoints.h>

#include "MysqlObjectDbi.h"
#include "MysqlVariantDbi.h"
#include "util/MysqlHelpers.h"

namespace U2 {

/********************************************************************/
/* Support classes */
/********************************************************************/

class MysqlVariantLoader: public MysqlRSLoader<U2Variant> {
public:
    U2Variant load(U2SqlQuery* q) {
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

class SimpleVariantTrackLoader : public MysqlRSLoader<U2VariantTrack> {
    U2VariantTrack load(U2SqlQuery* q) {
        U2VariantTrack track;
        SAFE_POINT(NULL != q, "Query pointer is NULL", track);

        track.id = q->getDataId(0, U2Type::VariantTrack);
        track.sequence = q->getDataId(1,U2Type::Sequence);
        track.sequenceName = q->getString(2);
        track.trackType = (VariantTrackType)q->getInt32(3);
        track.fileHeader = q->getString(4);

        return track;
    }
};

class SimpleVariantTrackFilter : public MysqlRSFilter<U2VariantTrack> {
public:
    SimpleVariantTrackFilter(VariantTrackType trackType) :
        trackType(trackType) {}

    bool filter(const U2VariantTrack& track) {
        return (trackType == TrackType_All || trackType == track.trackType);
    }

private:
    VariantTrackType trackType;
};


/********************************************************************/
/* MysqlVariantDbi */
/********************************************************************/

MysqlVariantDbi::MysqlVariantDbi(MysqlDbi* dbi) :
    U2VariantDbi(dbi),
    MysqlChildDbiCommon(dbi)
{
}

void MysqlVariantDbi::initSqlSchema(U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    // Variant track object
    U2SqlQuery(" CREATE TABLE VariantTrack (object BIGINT PRIMARY KEY, sequence BIGINT, sequenceName TEXT NOT NULL,"
        " trackType INTEGER DEFAULT 1, fileHeader LONGTEXT,"
        " FOREIGN KEY(object) REFERENCES Object(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();

    // Variant element
    // track - Variant track object id
    // startPos - variation start position
    // endPos - variation end position
    // refData - reference sequence part
    // obsData - observed variation of the reference
    // comment - comment visible for user
    // publicId - identifier visible for user
    // additionalInfo - added for vcf4 support
    U2SqlQuery("CREATE TABLE Variant(id BIGINT NOT NULL PRIMARY KEY AUTO_INCREMENT, track BIGINT, startPos BIGINT, endPos BIGINT, "
        " refData BLOB NOT NULL, obsData BLOB NOT NULL, publicId TEXT NOT NULL, additionalInfo TEXT,"
        " FOREIGN KEY(track) REFERENCES VariantTrack(object) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();
}

U2DbiIterator<U2VariantTrack>* MysqlVariantDbi::getVariantTracks(VariantTrackType trackType, U2OpStatus& os) {
    static const QString queryString = "SELECT object, sequence, sequenceName, trackType, fileHeader FROM VariantTrack";
    QSharedPointer<U2SqlQuery> q(new U2SqlQuery(queryString, db, os));

    return new MysqlRSIterator<U2VariantTrack>(q, new SimpleVariantTrackLoader(), new SimpleVariantTrackFilter(trackType), U2VariantTrack(), os);
}

U2DbiIterator<U2VariantTrack>* MysqlVariantDbi::getVariantTracks(const U2DataId& seqId, U2OpStatus& os) {
    static const QString queryString = "SELECT object, sequence, sequenceName, trackType, fileHeader FROM VariantTrack WHERE sequence = :sequence";
    QSharedPointer<U2SqlQuery> q(new U2SqlQuery(queryString, db, os));
    q->bindDataId(":sequence", seqId);

    return new MysqlRSIterator<U2VariantTrack>(q, new SimpleVariantTrackLoader(), NULL, U2VariantTrack(), os);
}

U2DbiIterator<U2VariantTrack>* MysqlVariantDbi::getVariantTracks( const U2DataId& seqId, VariantTrackType trackType, U2OpStatus& os ) {
    if (trackType == TrackType_All) {
        return getVariantTracks(seqId, os);
    }

    static const QString queryString = "SELECT object, sequence, sequenceName FROM VariantTrack WHERE sequence = :sequence";
    QSharedPointer<U2SqlQuery> q(new U2SqlQuery(queryString, db, os));
    q->bindDataId(":sequence", seqId);

    return new MysqlRSIterator<U2VariantTrack>(q, new SimpleVariantTrackLoader(), new SimpleVariantTrackFilter(trackType), U2VariantTrack(), os);
}

U2VariantTrack MysqlVariantDbi::getVariantTrack(const U2DataId& variantTrackId, U2OpStatus& os) {
    U2VariantTrack res;
    DBI_TYPE_CHECK(variantTrackId, U2Type::VariantTrack, os, res);
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    dbi->getMysqlObjectDbi()->getObject(res, variantTrackId, os);
    CHECK_OP(os, res);

    static const QString queryString = "SELECT sequence, sequenceName, trackType, fileHeader FROM VariantTrack WHERE object = :object";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":object", variantTrackId);

    if (q.step()) {
        res.sequence = q.getDataId(0, U2Type::Sequence);
        res.sequenceName = q.getString(1);
        int trackType = q.getInt32(2);
        CHECK_EXT(TrackType_FIRST <= trackType && trackType <= TrackType_LAST,
                  os.setError(U2DbiL10n::tr("Invalid variant track type: %1").arg(trackType)), res);
        res.trackType = static_cast<VariantTrackType>(trackType);
        res.fileHeader = q.getString(3);
        q.ensureDone();
    }

    return res;
}

U2VariantTrack MysqlVariantDbi::getVariantTrackofVariant( const U2DataId& variantId, U2OpStatus& os ){
    U2VariantTrack res;
    DBI_TYPE_CHECK(variantId, U2Type::VariantType, os, res);
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString = "SELECT track FROM Variant WHERE id = :id";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":id", variantId);

    if (q.step()) {
        U2DataId trackId = q.getDataId(0, U2Type::VariantTrack);
        res = getVariantTrack(trackId, os);
    }

    return res;
}

void MysqlVariantDbi::addVariantsToTrack(const U2VariantTrack& track, U2DbiIterator<U2Variant>* it, U2OpStatus& os) {
    CHECK_EXT(!track.sequenceName.isEmpty(), os.setError(U2DbiL10n::tr("Sequence name is not set")), );
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString = "INSERT INTO Variant(track, startPos, endPos, refData, obsData, publicId, additionalInfo) "
            "VALUES(:track, :startPos, :endPos, :refData, :obsData, :publicId, :additionalInfo)";
    U2SqlQuery q(queryString, db, os);

    while (it->hasNext()) {
        U2Variant var = it->next();
        q.bindDataId(":track", track.id);
        q.bindInt64(":startPos", var.startPos);
        q.bindInt64(":endPos", var.endPos);
        q.bindBlob(":refData", var.refData);
        q.bindBlob(":obsData", var.obsData);
        q.bindString(":publicId", var.publicId);
        q.bindString(":additionalInfo", StrPackUtils::packMap(var.additionalInfo));

        var.id = q.insert(U2Type::VariantType);
        CHECK_OP(os, );
    }
}

void MysqlVariantDbi::createVariationsIndex( U2OpStatus& os ) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    U2SqlQuery("CREATE INDEX IF NOT EXISTS VariantIndex ON Variant(track)", db, os).execute();
    CHECK_OP(os, );
    U2SqlQuery("CREATE INDEX IF NOT EXISTS VariantIndexstartPos ON Variant(startPos)", db, os).execute();
}

void MysqlVariantDbi::createVariantTrack(U2VariantTrack& track, VariantTrackType trackType, const QString& folder, U2OpStatus& os) {
    CHECK_EXT(!track.sequenceName.isEmpty(), os.setError(U2DbiL10n::tr("Sequence name is not set")), );
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    dbi->getMysqlObjectDbi()->createObject(track, folder, U2DbiObjectRank_TopLevel, os);
    CHECK_OP(os,);

    track.trackType = trackType;

    static const QString queryString = "INSERT INTO VariantTrack(object, sequence, sequenceName, trackType, fileHeader) "
            "VALUES(:object, :sequence, :sequenceName, :trackType, :fileHeader)";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":object", track.id);
    q.bindDataId(":sequence", track.sequence);
    q.bindString(":sequenceName", track.sequenceName);
    q.bindInt32(":trackType", track.trackType);
    q.bindString(":fileHeader", track.fileHeader);
    q.execute();
}

void MysqlVariantDbi::updateVariantTrack(U2VariantTrack &track, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString = "UPDATE VariantTrack SET sequence = :sequence, sequenceName = :sequenceName, trackType = :trackType, fileHeader = :fileHeader WHERE object = :object";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":sequence", track.sequence);
    q.bindString(":sequenceName", track.sequenceName);
    q.bindInt32(":trackType", track.trackType);
    q.bindString(":fileHeader", track.fileHeader);
    q.bindDataId(":object", track.id);
    q.execute();
    CHECK_OP(os, );

    dbi->getMysqlObjectDbi()->updateObject(track, os);
    CHECK_OP(os, );

    MysqlObjectDbi::incrementVersion(track.id, db, os);
}

U2DbiIterator<U2Variant>* MysqlVariantDbi::getVariants(const U2DataId& trackId, const U2Region& region, U2OpStatus& os) {
    if (region == U2_REGION_MAX) {
        static QString wholeRegionString ("SELECT id, startPos, endPos, refData, obsData, publicId, additionalInfo FROM Variant WHERE track = :track ORDER BY startPos");
        QSharedPointer<U2SqlQuery> q(new U2SqlQuery(wholeRegionString, db, os));
        q->bindDataId(":track", trackId);

        return new MysqlRSIterator<U2Variant>(q, new MysqlVariantLoader(), NULL, U2Variant(), os);
    }

    static const QString localRegionString = "SELECT id, startPos, endPos, refData, obsData, publicId, additionalInfo FROM Variant "
            "WHERE track = :track AND startPos >= :regionStart AND startPos < :regionEnd";
    QSharedPointer<U2SqlQuery> q(new U2SqlQuery(localRegionString, db, os));
    q->bindDataId(":track", trackId);
    q->bindInt64(":regionStart", region.startPos);
    q->bindInt64(":regionEnd", region.endPos());

    return new MysqlRSIterator<U2Variant>(q, new MysqlVariantLoader(), NULL, U2Variant(), os);
}

U2DbiIterator<U2Variant>* MysqlVariantDbi::getVariantsRange(const U2DataId& track, int offset, int limit, U2OpStatus& os ) {
    CHECK_OP(os, NULL);

    static const QString queryString = "SELECT id, startPos, endPos, refData, obsData, publicId, additionalInfo FROM Variant "
            "WHERE track = :track LIMIT :limit OFFSET :offset";
    QSharedPointer<U2SqlQuery> q(new U2SqlQuery(queryString, db, os));
    q->bindDataId(":track", track);
    q->bindInt64(":limit", limit);
    q->bindInt64(":offset", offset);

    return new MysqlRSIterator<U2Variant>(q, new MysqlVariantLoader(), NULL, U2Variant(), os);
}

int MysqlVariantDbi::getVariantCount( const U2DataId& trackId, U2OpStatus& os ) {
    static const QString sueryString = "SELECT COUNT(*) FROM Variant WHERE track = :track";
    U2SqlQuery q(sueryString, db, os);
    q.bindDataId(":track", trackId);
    if (!q.step()) {
        return -1;
    }

    return q.getInt32(0);
}

void MysqlVariantDbi::removeTrack(const U2DataId& trackId, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString variantString = "DELETE FROM Variant WHERE track = :track";
    U2SqlQuery variantQuery(variantString, db, os);
    variantQuery.bindDataId(":track", trackId);
    variantQuery.execute();
    CHECK_OP(os,);

    static const QString trackString = "DELETE FROM VariantTrack WHERE object = :object";
    U2SqlQuery trackQuery(trackString, db, os);
    trackQuery.bindDataId(":object", trackId);
    trackQuery.execute();
}

void MysqlVariantDbi::updateVariantPublicId( const U2DataId& track, const U2DataId& variant, const QString& newId, U2OpStatus& os ) {
    DBI_TYPE_CHECK(track, U2Type::VariantTrack, os, );
    DBI_TYPE_CHECK(variant, U2Type::VariantType, os, );
    CHECK_EXT(!newId.isEmpty(), os.setError(U2DbiL10n::tr("New variant public ID is empty")), );

    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static QString qvString("UPDATE Variant SET publicId = :publicId WHERE track = :track AND id = :id");
    U2SqlQuery qv(qvString, db, os);
    qv.bindString(":publicId", newId);
    qv.bindDataId(":track", track);
    qv.bindDataId(":id", variant);
    qv.execute();
}

void MysqlVariantDbi::updateTrackIDofVariant( const U2DataId& variant, const U2DataId& newTrackId, U2OpStatus& os ) {
    DBI_TYPE_CHECK(newTrackId, U2Type::VariantTrack, os, );
    DBI_TYPE_CHECK(variant, U2Type::VariantType, os, );
    CHECK_EXT(!newTrackId.isEmpty(), os.setError(U2DbiL10n::tr("New variant track ID is empty")), );

    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static QString qvString("UPDATE Variant SET track = :track WHERE id = :id");
    U2SqlQuery qv(qvString, db, os);
    qv.bindDataId(":track", variant);
    qv.bindDataId(":id", newTrackId);
    qv.execute();
}

}   // namespace U2
