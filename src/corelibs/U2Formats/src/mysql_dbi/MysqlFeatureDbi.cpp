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

#include <QQueue>

#include <U2Core/U2SafePoints.h>

#include "MysqlFeatureDbi.h"
#include "MysqlObjectDbi.h"
#include "util/MysqlDbiUtils.h"
#include "util/MysqlHelpers.h"

namespace U2 {

MysqlFeatureDbi::MysqlFeatureDbi(MysqlDbi* dbi)
    : U2FeatureDbi(dbi), MysqlChildDbiCommon(dbi)
{

}

void MysqlFeatureDbi::initSqlSchema(U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    //nameHash is used for better indexing
    U2SqlQuery("CREATE TABLE Feature (id BIGINT NOT NULL PRIMARY KEY AUTO_INCREMENT, "
        "class INTEGER NOT NULL, type INTEGER NOT NULL, parent BIGINT, root BIGINT, nameHash INTEGER, name TEXT, "
        "sequence BIGINT, strand INTEGER NOT NULL DEFAULT 0, start BIGINT NOT NULL DEFAULT 0, "
        "len BIGINT NOT NULL DEFAULT 0, end BIGINT NOT NULL DEFAULT 0) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();

    U2SqlQuery("CREATE TABLE FeatureKey (id BIGINT NOT NULL PRIMARY KEY AUTO_INCREMENT, feature BIGINT NOT NULL, "
        " name TEXT NOT NULL, value TEXT, "
        " FOREIGN KEY(feature) REFERENCES Feature(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();

    U2SqlQuery("CREATE TABLE AnnotationTable (object BIGINT PRIMARY KEY, rootId BIGINT NOT NULL, "
        "FOREIGN KEY(object) REFERENCES Object(id) ON DELETE CASCADE, "
        "FOREIGN KEY(rootId) REFERENCES Feature(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();

    U2SqlQuery("CREATE INDEX FeatureRootIndex ON Feature(root, class)", db, os).execute();
    U2SqlQuery("CREATE INDEX FeatureParentIndex ON Feature(parent)", db, os).execute();
    U2SqlQuery("CREATE INDEX FeatureLocationIndex ON Feature(start, end)", db, os).execute();
    U2SqlQuery("CREATE INDEX FeatureNameIndex ON Feature(root, nameHash)", db, os).execute();

    //FeatureKey index
    U2SqlQuery("CREATE INDEX FeatureKeyIndex ON FeatureKey(feature)", db, os).execute();
}

namespace {

inline QString getFeatureFields(const QString &featureAlias = "f") {
    return QString("%1.id, %1.class, %1.type, %1.parent, %1.root, %1.name, %1.sequence, "
        "%1.strand, %1.start, %1.len ").arg(featureAlias);
}

}

class MysqlFeatureRSLoader : public MysqlRSLoader<U2Feature> {
public:
    U2Feature load(U2SqlQuery* q) {
        return loadStatic(q);
    }

    static U2Feature loadStatic(U2SqlQuery* q) {
        U2Feature res;
        //class, type, parent, root, name, sequence, strand, start, len
        res.id = q->getDataId(0, U2Type::Feature);
        res.featureClass = static_cast<U2Feature::FeatureClass>(q->getInt32(1));
        res.featureType= static_cast<U2FeatureType>(q->getInt32(2));
        res.parentFeatureId = q->getDataId(3, U2Type::Feature);
        res.rootFeatureId = q->getDataId(4, U2Type::Feature);
        res.name = q->getString(5);
        res.sequenceId = q->getDataId(6, U2Type::Sequence);
        res.location.strand = U2Strand(U2Strand::Direction(q->getInt32(7)));
        res.location.region.startPos = q->getInt64(8);
        res.location.region.length= q->getInt64(9);
        return res;
    }
};

class MysqlFeatureFilter: public MysqlRSFilter<U2Feature>{
public:
    MysqlFeatureFilter(const QString& name, const U2DataId& _seqId){
        nameToFilter = name;
        seqId = _seqId;
    }
    bool filter(const U2Feature& f){
        if ((nameToFilter.isEmpty() || f.name == nameToFilter) && (seqId.isEmpty() || seqId == f.sequenceId)){
            return true;
        }
        return false;
    }

private:
    QString nameToFilter;
    U2DataId seqId;

};

void MysqlFeatureDbi::createAnnotationTableObject(U2AnnotationTable &table,
    const QString &folder, U2OpStatus &os)
{
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    dbi->getMysqlObjectDbi()->createObject(table, folder, U2DbiObjectRank_TopLevel, os);
    CHECK_OP(os,);

    static const QString queryString("INSERT INTO AnnotationTable (object, rootId) VALUES(:id, :rootId)");
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":id", table.id);
    q.bindDataId(":rootId", table.rootFeature);
    q.insert();
}

U2AnnotationTable MysqlFeatureDbi::getAnnotationTableObject(const U2DataId &tableId,
    U2OpStatus &os)
{
    U2AnnotationTable result;
    DBI_TYPE_CHECK(tableId, U2Type::AnnotationTable, os, result);

    static const QString queryString = "SELECT rootId, name FROM AnnotationTable, Object WHERE object = :id1 AND id = :id2";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":id1", tableId);
    q.bindDataId(":id2", tableId);
    if (q.step()) {
        result.rootFeature = q.getDataId(0, U2Type::Feature);
        result.visualName = q.getString(1);
        q.ensureDone();
    } else if (!os.hasError()) {
        os.setError(U2DbiL10n::tr("Annotation table object is not found."));
    }
    result.id = tableId;

    return result;
}

void MysqlFeatureDbi::removeAnnotationTableData(const U2DataId &tableId, U2OpStatus &os) {
    DBI_TYPE_CHECK(tableId, U2Type::AnnotationTable, os,);

    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    U2SqlQuery removeFeaturesQuery("DELETE F.* FROM Feature AS F INNER JOIN AnnotationTable AS A "
        "ON A.rootId = F.root OR A.rootId = F.id WHERE A.object = :object", db, os);
    removeFeaturesQuery.bindDataId(":object", tableId);
    removeFeaturesQuery.execute();
}

U2Feature MysqlFeatureDbi::getFeature(const U2DataId& featureId, U2OpStatus& os) {
    U2Feature res;
    DBI_TYPE_CHECK(featureId, U2Type::Feature, os, res);

    const QString queryString("SELECT " + getFeatureFields() + " FROM Feature AS f WHERE id = :id");
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":id", featureId);
    if (q.step()) {
        res = MysqlFeatureRSLoader::loadStatic(&q);
    } else if (!os.hasError()) {
        os.setError(U2DbiL10n::tr("Feature is not found."));
    }

    return res;
}

namespace {

void add (QString& buf, const QString& str, const QString& op, const QString& placeholder, int& n) {
    if (!buf.isEmpty()) {
        buf += " AND ";
    }
    buf += str;
    if (!op.isEmpty()) {
        n++;
        buf += op + ":" + placeholder + QString::number(n);
    }
}

QString toSqlCompareOp(ComparisonOp op) {
    QString res;
    switch(op) {
    case ComparisonOp_EQ  :
        res = "=";
        break;
    case ComparisonOp_NEQ :
        res = "!=";
        break;
    case ComparisonOp_GT  :
        res = ">";
        break;
    case ComparisonOp_GET :
        res = ">=";
        break;
    case ComparisonOp_LT  :
        res = "<";
        break;
    case ComparisonOp_LET :
        res = "<=";
        break;
    default:
        res = "?";
        break;
    }
    return res;
}

QString toSqlOrderOp(OrderOp op) {
    QString res;
    switch(op) {
    case OrderOp_Asc:
        res = "ASC";
        break;
    case OrderOp_Desc :
        res = "DESC";
        break;
    default:
        break;
    }
    return res;
}

QString toSqlOrderOpFromCompareOp(ComparisonOp op) {
    QString res;
    switch(op) {
    case ComparisonOp_EQ  :
        break;
    case ComparisonOp_NEQ :
        break;
    case ComparisonOp_GT  :
        res = "ASC";
        break;
    case ComparisonOp_GET :
        res = "ASC";
        break;
    case ComparisonOp_LT  :
        res = "DESC";
        break;
    case ComparisonOp_LET :
        res = "DESC";
        break;
    default:
        break;
    }
    return res;
}

QString getWhereQueryPartFromType(const QString &featurePlaceholder, const FeatureFlags &types) {
    QString result;
    if (types.testFlag(U2Feature::Annotation)) {
        result += featurePlaceholder + ".class = " + QString::number(U2Feature::Annotation);
    }
    if (types.testFlag(U2Feature::Group)) {
        if (!result.isEmpty()) {
            result += " OR ";
        }
        result += featurePlaceholder + ".class = " + QString::number(U2Feature::Group);
    }

    if (!result.isEmpty()) {
        result = " AND (" + result + ") ";
    }
    return result;
}

}   // unnamed namespace


QSharedPointer<U2SqlQuery> MysqlFeatureDbi::createFeatureQuery(const QString &selectPart, const FeatureQuery &fq, bool useOrder, U2OpStatus &os) {
    QString wherePart;
    int n = 0;

    bool useParent = !fq.parentFeatureId.isEmpty();
    if (useParent) {
        DBI_TYPE_CHECK(fq.parentFeatureId, U2Type::Feature, os, QSharedPointer<U2SqlQuery>());
        add(wherePart, "f.parent", "=", "parent", n);
    } else if (fq.topLevelOnly) {
        add(wherePart, "f.parent", "=", "parent", n);
    }

    bool useRoot = !fq.rootFeatureId.isEmpty();
    if (useRoot) {
        DBI_TYPE_CHECK(fq.rootFeatureId, U2Type::Feature, os, QSharedPointer<U2SqlQuery>());
        add(wherePart, "f.root", "=", "root", n);
    }

    bool useClass = (U2Feature::Invalid != fq.featureClass);
    if (useClass) {
        add(wherePart, "f.class", "=", "class", n);
    }

    bool useType = (U2FeatureTypes::Invalid != fq.featureType);
    if (useType) {
        add(wherePart, "f.type", "=", "type", n);
    }

    bool useName = !fq.featureName.isEmpty();
    if (useName) {
        add(wherePart, "f.nameHash", "=", "nameHash", n);
    }

    bool useSequence = !fq.sequenceId.isEmpty();
    if (useSequence) {
        DBI_TYPE_CHECK(fq.sequenceId, U2Type::Sequence, os, QSharedPointer<U2SqlQuery>());
        add(wherePart, "f.sequence", "=", "sequence", n);
    }

    bool useRegion = (0 < fq.intersectRegion.length);
    bool oneClosestFeature = (ComparisonOp_Invalid != fq.closestFeature);
    if (useRegion) {
        if (!oneClosestFeature) { //check if intersects
            if (1 != fq.intersectRegion.length) {
                add(wherePart, QString("f.start < :endPos%2 AND f.end > :startPos%1")
                     .arg(n + 1).arg(n + 2), "", "", n);
            } else {
                add(wherePart, QString("f.start < :endPos%2 AND f.start + f.len > :startPos%1")
                     .arg(n + 1).arg(n + 2), "", "", n);
            }
            n += 2;
        } else { //check if close
            add(wherePart, QString("f.start %1 :startPos%2")
                 .arg(toSqlCompareOp(fq.closestFeature)).arg(n + 1), "" ,"", n);
            n++;
        }
    }

    bool useStrand = (Strand_Both != fq.strandQuery);
    if (useStrand) {
        add(wherePart, "f.strand", "=", "strand", n);
    }

    bool useKeyName = !fq.keyName.isEmpty();
    if (useKeyName) {
        add(wherePart, "fk.name", "=" , "name", n);
    }

    bool useKeyValue = !fq.keyValue.isEmpty();
    if (useKeyValue) {
        add(wherePart, "fk.value", toSqlCompareOp(fq.keyValueCompareOp), "value", n);
    }

    bool useKeyTable = (useKeyName || useKeyValue);
    if (useKeyTable) {
        wherePart += " AND fk.feature = f.id ";
    }

    if (useOrder) {
        if (OrderOp_None != fq.keyNameOrderOp) {
            wherePart += " ORDER BY fk.name " + toSqlOrderOp(fq.keyNameOrderOp);
        } else if (OrderOp_None != fq.keyValueOrderOp) {
            wherePart += " ORDER BY fk.value";
        } else if (OrderOp_None != fq.startPosOrderOp && useRegion) {
            if (!oneClosestFeature) {
                wherePart += " ORDER BY f.start ";
            } else {
                wherePart += QString(" ORDER BY f.start %1 ")
                    .arg(toSqlOrderOpFromCompareOp(fq.closestFeature));
            }
        }
    }

    QString tablesPart = "Feature AS f";
    if (useKeyTable) {
        tablesPart += ", FeatureKey AS fk";
    }

    QString fullQuery = selectPart + " FROM " + tablesPart;
    if (!wherePart.isEmpty()) {
        fullQuery += " WHERE " + wherePart;
    }
    if (oneClosestFeature) {
        fullQuery += " LIMIT 1 ";
    }

    QSharedPointer<U2SqlQuery> q;
    q = QSharedPointer<U2SqlQuery>(new U2SqlQuery(fullQuery, db, os));
    int m = 0;

    if (useParent) {
        q->bindDataId(":parent" + QString::number(++m), fq.parentFeatureId);
    } else if (fq.topLevelOnly) {
        q->bindDataId(":parent" + QString::number(++m), U2DataId());
    }
    if (useRoot) {
        q->bindDataId(":root" + QString::number(++m), fq.rootFeatureId);
    }
    if (useClass) {
        q->bindInt32(":class" + QString::number(++m), fq.featureClass);
    }
    if (useType) {
        q->bindInt32(":type" + QString::number(++m), fq.featureType);
    }
    if (useName) {
        q->bindInt32(":nameHash" + QString::number(++m), qHash(fq.featureName));
    }
    if (useSequence) {
        q->bindDataId(":sequence" + QString::number(++m), fq.sequenceId);
    }
    if (useRegion) {
        if (!oneClosestFeature) {
            q->bindInt64(":startPos" + QString::number(++m), fq.intersectRegion.startPos);
            q->bindInt64(":endPos" + QString::number(++m), fq.intersectRegion.endPos());
        } else {
            q->bindInt64(":startPos" + QString::number(++m), fq.intersectRegion.startPos);
        }
    }
    if (useStrand) {
        U2Strand::Direction direction = U2Strand::Direct;
        if(Strand_Direct == fq.strandQuery) {
            direction = U2Strand::Direct;
        } else if (Strand_Compl == fq.strandQuery) {
            direction = U2Strand::Complementary;
        }
        q->bindInt32(":strand" + QString::number(++m), direction);
    }

    if (useKeyName) {
        q->bindString(":name" + QString::number(++m), fq.keyName);
    }
    if (useKeyValue) {
        q->bindString(":value" + QString::number(++m), fq.keyValue);
    }

    return q;
}

qint64 MysqlFeatureDbi::countFeatures(const FeatureQuery &fq, U2OpStatus &os) {
    QSharedPointer<U2SqlQuery> q(createFeatureQuery("SELECT COUNT(*)", fq, false, os));
    CHECK_OP(os, -1);

    return q->selectInt64();
}

U2DbiIterator<U2Feature>* MysqlFeatureDbi::getFeatures(const FeatureQuery& fq, U2OpStatus& os) {
    QSharedPointer<U2SqlQuery> q = createFeatureQuery("SELECT " + getFeatureFields(), fq, true, os);
    CHECK_OP(os, NULL);

    return new MysqlRSIterator<U2Feature>(q, new MysqlFeatureRSLoader(), NULL, U2Feature(), os);
}

QList<U2FeatureKey> MysqlFeatureDbi::getFeatureKeys(const U2DataId& featureId, U2OpStatus& os) {
    static const QString queryString("SELECT name, value FROM FeatureKey WHERE feature = :id");
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":id", featureId);

    QList<U2FeatureKey> result;
    while(q.step()) {
        U2FeatureKey key;
        key.name = q.getCString(0);
        key.value = q.getCString(1);
        result.append(key);
    }

    return result;
}

namespace {

void addKeyCommon(U2SqlQuery& qk, const U2DataId& featureId, const U2FeatureKey& key) {
    qk.bindDataId(":feature", featureId);
    qk.bindString(":name", key.name);
    qk.bindString(":value", key.value);
    qk.insert();
}

QString getFeatureKeyInsertQuery(int keyCount) {
    SAFE_POINT(keyCount > 0, "Unexpected feature keys number", QString());

    QString queryStringk("INSERT INTO FeatureKey(feature, name, value) VALUES");
    for (int i = 1, n = 3 * keyCount; i <= n; i += 3) {
        queryStringk += QString("(:%1, :%2, :%3),").arg(i).arg(i + 1).arg(i + 2);
    }
    queryStringk.chop(1); //remove last comma
    return queryStringk;
}

void addFeatureKeys(const QList<U2FeatureKey> &keys, const U2DataId &featureId, MysqlDbRef *db, U2OpStatus& os) {
    CHECK(!keys.isEmpty(), );

    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    const QString insertQueryStr = getFeatureKeyInsertQuery(keys.size());

    U2SqlQuery query(insertQueryStr, db, os);

    for (int i = 1, n = 3 * keys.size(); i <= n && !os.isCoR(); i += 3) {
        const U2FeatureKey &key = keys[(i - 1) / 3];
        query.bindDataId(QString(":%1").arg(i), featureId);
        query.bindString(QString(":%1").arg(i + 1), key.name);
        query.bindString(QString(":%1").arg(i + 2), key.value);
    }
    query.insert();
}

} // unnamed namespace

void MysqlFeatureDbi::createFeature(U2Feature& feature, const QList<U2FeatureKey>& keys, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryStringf("INSERT INTO Feature(class, type, parent, root, name, sequence, strand, start, len, end, nameHash) "
                                      "VALUES(:class, :type, :parent, :root, :name, :sequence, :strand, :start, :len, :end, :nameHash)");
    U2SqlQuery qf(queryStringf, db, os);
    qf.bindInt32(":class", feature.featureClass);
    qf.bindInt32(":type", feature.featureType);
    qf.bindDataId(":parent", feature.parentFeatureId);
    qf.bindDataId(":root", feature.rootFeatureId);
    qf.bindString(":name", feature.name);
    qf.bindDataId(":sequence", feature.sequenceId);
    qf.bindInt32(":strand", feature.location.strand.getDirectionValue());
    qf.bindInt64(":start", feature.location.region.startPos);
    qf.bindInt64(":len", feature.location.region.length);
    qf.bindInt64(":end", feature.location.region.endPos());
    qf.bindInt32(":nameHash", qHash(feature.name));
    feature.id = qf.insert(U2Type::Feature);
    CHECK_OP(os,);

    addFeatureKeys(keys, feature.id, db, os);
}

void MysqlFeatureDbi::addKey(const U2DataId& featureId, const U2FeatureKey& key, U2OpStatus& os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString = "INSERT INTO FeatureKey(feature, name, value) VALUES(:feature, :name, :value)";
    U2SqlQuery qk(queryString, db, os);
    addKeyCommon(qk, featureId, key);
}

void MysqlFeatureDbi::removeAllKeys(const U2DataId& featureId, const QString& keyName, U2OpStatus& os) {
    DBI_TYPE_CHECK(featureId, U2Type::Feature, os,);

    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString = "DELETE FROM FeatureKey WHERE feature = :feature AND name = :name";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":feature", featureId);
    q.bindString(":name", keyName);
    q.execute();
}

void MysqlFeatureDbi::removeKey(const U2DataId &featureId, const U2FeatureKey &key, U2OpStatus &os) {
    DBI_TYPE_CHECK(featureId, U2Type::Feature, os,);

    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString = "DELETE FROM FeatureKey WHERE feature = :feature AND name = :name AND value = :value LIMIT 1";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":feature", featureId);
    q.bindString(":name", key.name);
    q.bindString(":value", key.value);
    q.execute();
}

void MysqlFeatureDbi::updateName(const U2DataId& featureId, const QString& newName, U2OpStatus& os){
    DBI_TYPE_CHECK(featureId, U2Type::Feature, os,);

    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString = "UPDATE Feature SET name = :name, nameHash = :nameHash WHERE id = :id";
    U2SqlQuery qf(queryString, db, os);
    qf.bindDataId(":id", featureId);
    qf.bindInt32(":nameHash", qHash(newName));
    qf.bindString(":name", newName);
    qf.execute();
}

void MysqlFeatureDbi::updateParentId(const U2DataId& featureId, const U2DataId& parentId, U2OpStatus& os){
    DBI_TYPE_CHECK(featureId, U2Type::Feature, os,);
    DBI_TYPE_CHECK(parentId, U2Type::Feature, os,);

    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString = "UPDATE Feature SET parent = :parent WHERE id = :id";
    U2SqlQuery qf(queryString, db, os);
    qf.bindDataId(":parent", parentId);
    qf.bindDataId(":id", featureId);
    qf.execute();
}

void MysqlFeatureDbi::updateSequenceId(const U2DataId &featureId, const U2DataId &seqId,
    U2OpStatus &os)
{
    DBI_TYPE_CHECK(featureId, U2Type::Feature, os,);
    DBI_TYPE_CHECK(seqId, U2Type::Sequence, os,);

    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString = "UPDATE Feature SET sequence = :sequence WHERE id = :id";
    U2SqlQuery qf(queryString, db, os);
    qf.bindDataId(":sequence", seqId);
    qf.bindDataId(":id", featureId);
    qf.execute();
}

void MysqlFeatureDbi::updateKeyValue(const U2DataId& featureId, const U2FeatureKey& key, U2OpStatus& os){
    DBI_TYPE_CHECK(featureId, U2Type::Feature, os,);

    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString = "UPDATE FeatureKey SET value = :value WHERE feature = :feature AND name = :name";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":feature", featureId);
    q.bindString(":name", key.name);
    q.bindString(":value", key.value);
    q.execute();
}

bool MysqlFeatureDbi::getKeyValue(const U2DataId &featureId, U2FeatureKey &key, U2OpStatus &os) {
    DBI_TYPE_CHECK(featureId, U2Type::Feature, os, false);

    static const QString queryString("SELECT value FROM FeatureKey WHERE feature = :feature AND name = :name");
    U2SqlQuery q(queryString, db, os);
    CHECK_OP(os, false);

    q.bindDataId(":feature", featureId);
    q.bindString(":name", key.name);

    if (q.step()) {
        key.value = q.getCString(0);
        return true;
    } else {
        return false;
    }
}

void MysqlFeatureDbi::updateLocation(const U2DataId& featureId, const U2FeatureLocation& location, U2OpStatus& os) {
    DBI_TYPE_CHECK(featureId, U2Type::Feature, os,);
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString updFeatureString = "UPDATE Feature SET strand = :strand, start = :start, len = :len, end = :end WHERE id = :id";
    U2SqlQuery qf(updFeatureString, db, os);
    qf.bindInt32(":strand", location.strand.getDirectionValue());
    qf.bindInt64(":start", location.region.startPos);
    qf.bindInt64(":len", location.region.length);
    qf.bindInt64(":end", location.region.endPos());
    qf.bindDataId(":id", featureId);
    qf.execute();
}

void MysqlFeatureDbi::updateType(const U2DataId &featureId, U2FeatureType newType, U2OpStatus &os) {
    DBI_TYPE_CHECK(featureId, U2Type::Feature, os,);

    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString = "UPDATE Feature SET type = :type WHERE id = :id";
    U2SqlQuery qf(queryString, db, os);
    qf.bindDataId(":id", featureId);
    qf.bindInt32(":type", newType);
    qf.execute();
}

void MysqlFeatureDbi::removeFeature(const U2DataId& featureId, U2OpStatus& os) {
    DBI_TYPE_CHECK(featureId, U2Type::Feature, os,);
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    QSharedPointer<U2DbiIterator<U2Feature> > subfeaturesIter(getFeaturesByParent(featureId,
        QString(), U2DataId(), os, NotSelectParentFeature));
    while (subfeaturesIter->hasNext()) {
        removeFeature(subfeaturesIter->next().id, os);
        CHECK_OP(os,);
    }

    static const QString featureQueryString = "DELETE FROM Feature WHERE id = :id";
    U2SqlQuery qf(featureQueryString, db, os);
    qf.bindDataId(":id", featureId);
    qf.execute();
}

void MysqlFeatureDbi::removeFeaturesByParent(const U2DataId &parentId, U2OpStatus &os,
    SubfeatureSelectionMode mode)
{
    DBI_TYPE_CHECK(parentId, U2Type::Feature, os,);

    const bool includeParent = SelectParentFeature == mode;

    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    U2SqlQuery qf("DELETE FROM Feature WHERE parent = :parent"
        + (includeParent ? QString(" OR id = :id") : ""), db, os);
    qf.bindDataId(":parent", parentId);
    if (includeParent) {
        qf.bindDataId(":id", parentId);
    }
    qf.execute();
}

namespace {

void executeDeleteFeaturesByParentsQuery(const QList<U2DataId> &parentIds, MysqlDbRef *db, U2OpStatus &os) {
    SAFE_POINT(NULL != db, "Invalid database handler", );

    QString idsList = "(";
    for (int i = 1, n = parentIds.count(); i <= n; i++) {
        idsList += QString(":%1,").arg(i);
    }
    idsList.chop(1);
    idsList += ")";

    U2SqlQuery qf(QString("DELETE FROM Feature WHERE parent IN %1 OR id IN %1").arg(idsList), db, os);
    for (int i = 1, n = parentIds.count(); i <= n; i++) {
        qf.bindDataId(QString(":%1").arg(i), parentIds.at(i - 1));
    }
    qf.execute();
}

}

void MysqlFeatureDbi::removeFeaturesByParents(const QList<U2DataId> &parentIds, U2OpStatus &os) {
    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    int parentsNumber = parentIds.count();
    if(parentsNumber <= MysqlDbi::BIND_PARAMETERS_LIMIT) {
        executeDeleteFeaturesByParentsQuery(parentIds, db, os);
    } else {
        int deletedFeaturesNumber = 0;
        while(parentsNumber - deletedFeaturesNumber > 0) {
            int numDeletions = parentsNumber - deletedFeaturesNumber >= MysqlDbi::BIND_PARAMETERS_LIMIT ? MysqlDbi::BIND_PARAMETERS_LIMIT : -1;
            QList<U2DataId> copiedAnnotations = parentIds.mid(deletedFeaturesNumber, numDeletions);
            executeDeleteFeaturesByParentsQuery(copiedAnnotations, db, os);
            deletedFeaturesNumber += MysqlDbi::BIND_PARAMETERS_LIMIT;
        }
    }
}

void MysqlFeatureDbi::removeFeaturesByRoot(const U2DataId &rootId, U2OpStatus &os,
    SubfeatureSelectionMode mode)
{
    DBI_TYPE_CHECK(rootId, U2Type::Feature, os,);

    const bool includeParent = SelectParentFeature == mode;

    MysqlTransaction t(db, os);
    Q_UNUSED(t);

    U2SqlQuery qf("DELETE FROM Feature WHERE root = :root"
        + (includeParent ? QString(" OR id = :id") : ""), db, os);
    qf.bindDataId(":root", rootId);
    if (includeParent) {
        qf.bindDataId(":id", rootId);
    }
    qf.execute();
}

U2DbiIterator<U2Feature>* MysqlFeatureDbi::getFeaturesByRegion(const U2Region& reg,
    const U2DataId& rootId, const QString& featureName, const U2DataId& seqId, U2OpStatus& os,
    bool contains)
{
    const bool selectByRoot = !rootId.isEmpty();
    const QString queryByRegion = "SELECT " + getFeatureFields() + " FROM Feature AS f WHERE "
        + (selectByRoot ? QString("f.root = :root AND ") : QString())
        + (contains ? QString("f.start >= %1 AND f.end <= %2").arg(reg.startPos).arg(reg.endPos() - 1)
        : QString("f.start <= %1 AND f.end >= %2").arg(reg.endPos() - 1).arg(reg.startPos));

    QSharedPointer<U2SqlQuery> q(new U2SqlQuery(queryByRegion, db, os));

    if (selectByRoot) {
        q->bindDataId(":root", rootId);
    }

    return new MysqlRSIterator<U2Feature>(q, new MysqlFeatureRSLoader(),
        new MysqlFeatureFilter(featureName, seqId), U2Feature(), os);
}

U2DbiIterator<U2Feature> * MysqlFeatureDbi::getFeaturesBySequence(const QString &featureName,
    const U2DataId &seqId, U2OpStatus &os)
{
    static const QString queryStringk("SELECT " + getFeatureFields() + " FROM Feature AS f "
        "WHERE f.sequence = :sequence and f.name = :name ORDER BY f.start");
    QSharedPointer<U2SqlQuery> q(new U2SqlQuery(queryStringk, db, os));

    q->bindDataId(":sequence", seqId);
    q->bindString(":name", featureName);

    return new MysqlRSIterator<U2Feature>(q, new MysqlFeatureRSLoader(), NULL, U2Feature(), os);
}

U2DbiIterator<U2Feature> * MysqlFeatureDbi::getFeaturesByParent(const U2DataId &parentId,
    const QString &featureName, const U2DataId &seqId, U2OpStatus &os,
    SubfeatureSelectionMode mode)
{
    const bool includeParent = SelectParentFeature == mode;
    const QString queryStringk("SELECT " + getFeatureFields() + " FROM Feature AS f "
        "WHERE f.parent = :parent" + (includeParent ? " OR f.id = :id" : "") + " ORDER BY f.start");
    QSharedPointer<U2SqlQuery> q(new U2SqlQuery(queryStringk, db, os));

    q->bindDataId(":parent", parentId);
    if (includeParent) {
        q->bindDataId(":id", parentId);
    }

    return new MysqlRSIterator<U2Feature>(q, new MysqlFeatureRSLoader(),
        new MysqlFeatureFilter(featureName, seqId), U2Feature(), os);
}

U2DbiIterator<U2Feature> * MysqlFeatureDbi::getFeaturesByRoot(const U2DataId &rootId, const FeatureFlags &types, U2OpStatus &os) {
    const QString queryStringk("SELECT " + getFeatureFields() + " FROM Feature AS f "
        "WHERE f.root = :root" + getWhereQueryPartFromType("f", types) +  "ORDER BY f.start");
    QSharedPointer<U2SqlQuery> q(new U2SqlQuery(queryStringk, db, os));

    q->bindDataId(":root", rootId);
    return new MysqlRSIterator<U2Feature>(q, new MysqlFeatureRSLoader(),
        new MysqlFeatureFilter(QString(), U2DataId()), U2Feature(), os);
}

U2DbiIterator<U2Feature> * MysqlFeatureDbi::getFeaturesByName(const U2DataId &rootId, const QString &name, const FeatureFlags &types, U2OpStatus &os) {
    const QString queryStringk("SELECT " + getFeatureFields() + " FROM Feature AS f "
        "WHERE f.root = :root" + getWhereQueryPartFromType("f", types) +  " AND nameHash = :nameHash ORDER BY f.start");
    QSharedPointer<U2SqlQuery> q(new U2SqlQuery(queryStringk, db, os));

    q->bindDataId(":root", rootId);
    q->bindInt32(":nameHash", qHash(name));
    CHECK_OP(os, NULL);
    return new MysqlRSIterator<U2Feature>(q, new MysqlFeatureRSLoader(), new MysqlFeatureFilter(QString(), U2DataId()), U2Feature(), os);
}

QList<FeatureAndKey> MysqlFeatureDbi::getFeatureTable(const U2DataId &rootFeatureId, U2OpStatus &os) {
    static const QString queryStringk("SELECT " + getFeatureFields() + ", fk.name, fk.value FROM Feature AS f "
        "LEFT OUTER JOIN FeatureKey AS fk ON f.id = fk.feature WHERE f.root = :root ORDER BY f.class DESC, f.start, f.len");
    U2SqlQuery q(queryStringk, db, os);

    q.bindDataId(":root", rootFeatureId);

    QList<FeatureAndKey> result;
    while (q.step()) {
        FeatureAndKey fnk;
        fnk.feature = MysqlFeatureRSLoader::loadStatic(&q);
        fnk.key.name = q.getCString(10);
        fnk.key.value = q.getCString(11);
        result.append(fnk);
    }

    return result;
}

QMap<U2DataId, QStringList> MysqlFeatureDbi::getAnnotationTablesByFeatureKey(const QStringList &values, U2OpStatus &os) {
    QMap<U2DataId, QStringList> result;
    CHECK(!values.isEmpty(), result);
    // Pay attention here if there is the need of processing more search terms
    CHECK_EXT(values.size() < MysqlDbi::BIND_PARAMETERS_LIMIT, os.setError("Too many search terms provided"), result);

    QString queryStringk("SELECT DISTINCT A.object, F.name FROM AnnotationTable AS A, Feature AS F, FeatureKey AS FK "
        "WHERE A.rootId = F.root AND F.id = FK.feature");

    for (int i = 1, n = values.size(); i <= n; ++i) {
        queryStringk.append(QString(" AND FK.value LIKE :%1").arg(i));
    }

    U2SqlQuery q(queryStringk, db, os);

    for (int i = 1, n = values.size(); i <= n; ++i) {
        q.bindString(QString(":%1").arg(i), QString("%%1%").arg(values[i - 1]));
        CHECK_OP(os, result);
    }

    while (q.step()) {
        result[q.getDataId(0, U2Type::AnnotationTable)].append(q.getString(1));
    }

    return result;
}

}   // namespace U2
