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

#include "SQLiteObjectDbi.h"
#include "SQLiteFeatureDbi.h"

static const QString FDBI_FIELDS("f.id, f.class, f.type, f.parent, f.root, f.name, f.sequence, f.strand, f.start, f.len ");

namespace U2 {

SQLiteFeatureDbi::SQLiteFeatureDbi(SQLiteDbi* dbi)
    : U2FeatureDbi(dbi), SQLiteChildDBICommon(dbi)
{

}

static QString getQueryForFeatureDeletionTrigger() {
    return "CREATE TRIGGER FeatureDeletion BEFORE DELETE ON Feature "
               "FOR EACH ROW "
               "BEGIN "
                   "DELETE FROM FeatureLocationRTreeIndex WHERE id = OLD.id;"
                   "DELETE FROM Feature WHERE parent = OLD.id;"
               "END";
}

void SQLiteFeatureDbi::initSqlSchema(U2OpStatus& os) {
    //nameHash is used for better indexing
    SQLiteWriteQuery("CREATE TABLE Feature (id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
        "class INTEGER NOT NULL, type INTEGER NOT NULL, parent INTEGER, root INTEGER, nameHash INTEGER, name TEXT, "
        "sequence INTEGER, strand INTEGER NOT NULL, "
        "start INTEGER NOT NULL DEFAULT 0, len INTEGER NOT NULL DEFAULT 0)", db, os).execute();

    SQLiteWriteQuery("CREATE TABLE FeatureKey (id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
        "feature INTEGER NOT NULL, name TEXT NOT NULL, value TEXT NOT NULL, "
        "FOREIGN KEY(feature) REFERENCES Feature(id) ON DELETE CASCADE)", db, os).execute();

    // annotation table object
    SQLiteWriteQuery("CREATE TABLE AnnotationTable (object INTEGER PRIMARY KEY, rootId INTEGER NOT NULL, "
        "FOREIGN KEY(object) REFERENCES Object(id) ON DELETE CASCADE, "
        "FOREIGN KEY(rootId) REFERENCES Feature(id) ON DELETE CASCADE)", db, os).execute();

    //Feature index
    SQLiteWriteQuery("CREATE VIRTUAL TABLE FeatureLocationRTreeIndex USING rtree_i32(id, start, end)",
        db, os).execute();

    SQLiteWriteQuery("CREATE INDEX IF NOT EXISTS FeatureRootIndex ON Feature(root, class)" ,db, os).execute();
    SQLiteWriteQuery("CREATE INDEX IF NOT EXISTS FeatureParentIndex ON Feature(parent)", db, os).execute();
    SQLiteWriteQuery("CREATE INDEX IF NOT EXISTS FeatureNameIndex ON Feature(root, nameHash)", db, os).execute();

    //FeatureKey index
    SQLiteWriteQuery("CREATE INDEX IF NOT EXISTS FeatureKeyIndex ON FeatureKey(feature)", db, os).execute();

    //Deletion triggers
    SQLiteWriteQuery(getQueryForFeatureDeletionTrigger(), db, os).execute();
}

class SqlFeatureRSLoader : public SQLiteResultSetLoader<U2Feature> {
public:
    U2Feature load(SQLiteQuery* q) {
        return loadStatic(q);
    }

    static U2Feature loadStatic(SQLiteQuery* q) {
        U2Feature res;
        //class, type, parent, root, name, sequence, strand, start, len
        res.id = q->getDataId(0, U2Type::Feature);
        res.featureClass = static_cast<U2Feature::FeatureClass>(q->getInt32(1));
        res.featureType = static_cast<U2FeatureType>(q->getInt32(2));
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

class SqlFeatureFilter: public SQLiteResultSetFilter<U2Feature>{
public:
    SqlFeatureFilter(const QString& name, const U2DataId& _seqId){
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

void SQLiteFeatureDbi::createAnnotationTableObject(U2AnnotationTable &table, const QString &folder, U2OpStatus &os) {
    dbi->getSQLiteObjectDbi()->createObject(table, folder, U2DbiObjectRank_TopLevel, os);
    CHECK_OP(os,);

    static const QString queryString("INSERT INTO AnnotationTable (object, rootId) VALUES(?1, ?2)");
    SQLiteWriteQuery q(queryString, db, os);
    CHECK_OP(os,);
    q.bindDataId(1, table.id);
    q.bindDataId(2, table.rootFeature);
    q.insert();
}

U2AnnotationTable SQLiteFeatureDbi::getAnnotationTableObject(const U2DataId &tableId, U2OpStatus &os) {
    U2AnnotationTable result;

    DBI_TYPE_CHECK(tableId, U2Type::AnnotationTable, os, result);

    SQLiteReadQuery q("SELECT rootId, name FROM AnnotationTable, Object WHERE object = ?1 AND id = ?1", db, os);
    q.bindDataId(1, tableId);
    if (q.step()) {
        result.rootFeature = q.getDataId(0, U2Type::Feature);
        result.visualName = q.getString(1);
        q.ensureDone();
    } else if (!os.hasError()) {
        os.setError(U2DbiL10n::tr("Annotation table object not found."));
    }
    result.id = tableId;

    return result;
}

void SQLiteFeatureDbi::removeAnnotationTableData(const U2DataId &tableId, U2OpStatus &os) {
    DBI_TYPE_CHECK(tableId, U2Type::AnnotationTable, os,);
    static const QString rootQueryStr = "(SELECT rootId FROM AnnotationTable WHERE object = ?1)";

    SQLiteWriteQuery removeAnnTableQuery(QString("DELETE FROM Feature WHERE root IN %1 OR id IN %1").arg(rootQueryStr), db, os);
    removeAnnTableQuery.bindDataId(1, tableId);
    removeAnnTableQuery.execute();
}

U2Feature SQLiteFeatureDbi::getFeature(const U2DataId& featureId, U2OpStatus& os) {
    U2Feature res;
    DBI_TYPE_CHECK(featureId, U2Type::Feature, os, res);

    const QString queryString("SELECT " + FDBI_FIELDS + " FROM Feature AS f WHERE id = ?1");
    SQLiteReadQuery q(queryString, db, os);
    q.bindDataId(1, featureId);
    q.execute();
    CHECK_OP(os, res);

    res = SqlFeatureRSLoader::loadStatic(&q);
    return res;
}

static void add(QString& buf, const QString& str, const QString& op, int& n) {
    if (!buf.isEmpty()) {
        buf += " AND ";
    }
    buf += str;
    if (!op.isEmpty()) {
        n++;
        buf += op + "?" + QString::number(n);
    }
}

static QString toSqlCompareOp(ComparisonOp op) {
    QString res;
    switch(op) {
        case ComparisonOp_EQ  : res = "=";  break;
        case ComparisonOp_NEQ : res = "!="; break;
        case ComparisonOp_GT  : res = ">";  break;
        case ComparisonOp_GET : res = ">="; break;
        case ComparisonOp_LT  : res = "<";  break;
        case ComparisonOp_LET : res = "<="; break;
        default: res = "?"; break;
    }
    return res;
}

static QString toSqlOrderOp(OrderOp op) {
    QString res;
    switch(op) {
        case OrderOp_Asc: res = "ASC";  break;
        case OrderOp_Desc : res = "DESC"; break;
        default: break;
    }
    return res;
}

static QString toSqlOrderOpFromCompareOp(ComparisonOp op) {
    QString res;
    switch(op) {
        case ComparisonOp_EQ  : break;
        case ComparisonOp_NEQ : break;
        case ComparisonOp_GT  : res = "ASC";  break;
        case ComparisonOp_GET : res = "ASC"; break;
        case ComparisonOp_LT  : res = "DESC";  break;
        case ComparisonOp_LET : res = "DESC"; break;
        default: break;
    }
    return res;
}

static QString getWhereQueryPartFromType(const QString &featurePlaceholder, const FeatureFlags &types) {
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

QSharedPointer<SQLiteQuery> SQLiteFeatureDbi::createFeatureQuery(const QString &selectPart, const FeatureQuery &fq, bool useOrder,
    U2OpStatus &os, SQLiteTransaction *trans)
{
    QString wherePart;
    int n = 0;

    bool useParent = !fq.parentFeatureId.isEmpty();
    if (useParent) {
        DBI_TYPE_CHECK(fq.parentFeatureId, U2Type::Feature, os, QSharedPointer<SQLiteReadQuery>());
        add(wherePart, "f.parent", "=", n);
    } else if (fq.topLevelOnly) {
        add(wherePart, "f.parent", "=", n);
    }

    bool useRoot = !fq.rootFeatureId.isEmpty();
    if (useRoot) {
        DBI_TYPE_CHECK(fq.rootFeatureId, U2Type::Feature, os, QSharedPointer<SQLiteReadQuery>());
        add(wherePart, "f.root", "=", n);
    }

    bool useClass = (U2Feature::Invalid != fq.featureClass);
    if (useClass) {
        add(wherePart, "f.class", "=", n);
    }

    bool useType = (U2FeatureTypes::Invalid != fq.featureType);
    if (useType) {
        add(wherePart, "f.type", "=", n);
    }

    bool useName = !fq.featureName.isEmpty();
    if (useName) {
        add(wherePart, "f.nameHash", "=", n);
    }

    bool useSequence = !fq.sequenceId.isEmpty();
    if (useSequence) {
        DBI_TYPE_CHECK(fq.sequenceId, U2Type::Sequence, os, QSharedPointer<SQLiteReadQuery>());
        add(wherePart, "f.sequence", "=", n);
    }

    bool useRegion = (0 < fq.intersectRegion.length);
    bool oneClosestFeature = (ComparisonOp_Invalid != fq.closestFeature);
    if (useRegion) {
        if (!oneClosestFeature) { //check if intersects
            if (1 != fq.intersectRegion.length) {
                add(wherePart, QString("fr.id = f.id AND fr.start < ?%2 AND fr.end > ?%1")
                    .arg(n + 1).arg(n + 2), "", n);
            } else {
                add(wherePart, QString("f.start < ?%2 AND f.start + f.len > ?%1")
                    .arg(n + 1).arg(n + 2), "", n);
            }
            n += 2;
        } else { //check if close
            add(wherePart, QString("fr.id = f.id AND fr.start %1 ?%2")
                .arg(toSqlCompareOp(fq.closestFeature)).arg(n + 1), "", n);
            n++;
        }
    }

    bool useStrand = (Strand_Both != fq.strandQuery);
    if (useStrand) {
        add(wherePart, "f.strand", "=", n);
    }

    bool useKeyName = !fq.keyName.isEmpty();
    if (useKeyName) {
        add(wherePart, "fk.name", "=" , n);
    }

    bool useKeyValue = !fq.keyValue.isEmpty();
    if (useKeyValue) {
        add(wherePart, "fk.value", toSqlCompareOp(fq.keyValueCompareOp), n);
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
                wherePart += " ORDER BY fr.start ";
            } else {
                wherePart += QString(" ORDER BY fr.start %1 ")
                    .arg(toSqlOrderOpFromCompareOp(fq.closestFeature));
            }
        }
    }

    QString tablesPart = "Feature AS f";
    if (useKeyTable) {
        tablesPart += ", FeatureKey AS fk";
    }
    if (useRegion && (1 != fq.intersectRegion.length || oneClosestFeature)) {
        tablesPart += ", FeatureLocationRTreeIndex AS fr";
    }

    QString fullQuery = selectPart + " FROM " + tablesPart;
    if (!wherePart.isEmpty()) {
        fullQuery += " WHERE " + wherePart;
    }
    if (oneClosestFeature) {
        fullQuery += " LIMIT 1 ";
    }

    QSharedPointer<SQLiteQuery> q;
    if (NULL == trans) {
        q = QSharedPointer<SQLiteReadQuery>(new SQLiteReadQuery(fullQuery, db, os));
    } else {
        q = trans->getPreparedQuery(fullQuery, db, os);
        CHECK_OP(os, QSharedPointer<SQLiteReadQuery>());
    }
    int m = 0;

    if (useParent) {
        q->bindDataId(++m, fq.parentFeatureId);
    } else if (fq.topLevelOnly) {
        q->bindDataId(++m, U2DataId());
    }
    if (useRoot) {
        q->bindDataId(++m, fq.rootFeatureId);
    }
    if (useClass) {
        q->bindInt32(++m, fq.featureClass);
    }
    if (useType) {
        q->bindInt32(++m, fq.featureType);
    }
    if (useName) {
        q->bindInt32(++m, qHash(fq.featureName));
    }
    if (useSequence) {
        q->bindDataId(++m, fq.sequenceId);
    }
    if (useRegion) {
        if (!oneClosestFeature) {
            q->bindInt64(++m, fq.intersectRegion.startPos);
            q->bindInt64(++m, fq.intersectRegion.endPos());
        } else {
            q->bindInt64(++m, fq.intersectRegion.startPos);
        }
    }
    if (useStrand) {
        U2Strand::Direction direction = U2Strand::Direct;
        if(Strand_Direct == fq.strandQuery) {
            direction = U2Strand::Direct;
        } else if (Strand_Compl == fq.strandQuery) {
            direction = U2Strand::Complementary;
        }
        q->bindInt32(++m, direction);
    }

    if (useKeyName) {
        q->bindString(++m, fq.keyName);
    }
    if (useKeyValue) {
        q->bindString(++m, fq.keyValue);
    }

    return q;
}

qint64 SQLiteFeatureDbi::countFeatures(const FeatureQuery &fq, U2OpStatus &os) {
    QSharedPointer<SQLiteQuery> q(createFeatureQuery("SELECT COUNT(*)", fq, false, os));
    CHECK_OP(os, -1);
    return q->selectInt64();
}

U2DbiIterator<U2Feature>* SQLiteFeatureDbi::getFeatures(const FeatureQuery& fq, U2OpStatus& os) {
    QSharedPointer<SQLiteQuery> q = createFeatureQuery("SELECT " + FDBI_FIELDS, fq, true, os);
    CHECK_OP(os, NULL);
    return new SQLiteResultSetIterator<U2Feature>(q, new SqlFeatureRSLoader(), NULL, U2Feature(), os);
}

QList<U2FeatureKey> SQLiteFeatureDbi::getFeatureKeys(const U2DataId& featureId, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    static const QString queryString("SELECT name, value FROM FeatureKey WHERE feature = ?1");
    SQLiteReadQuery q(queryString, db, os);

    q.bindDataId(1, featureId);
    CHECK_OP(os, QList<U2FeatureKey>());
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

void addKeyCommon(SQLiteWriteQuery& qk, const U2DataId& featureId, const U2FeatureKey& key) {
    qk.reset();
    qk.bindDataId(1, featureId);
    qk.bindString(2, key.name);
    qk.bindString(3, key.value);
    qk.insert();
}

QString getFeatureKeyInsertQuery(int keyCount) {
    SAFE_POINT(keyCount > 0, "Unexpected feature keys number", QString());

    QString queryStringk("INSERT INTO FeatureKey(feature, name, value) VALUES");
    for (int i = 1, n = 3 * keyCount; i <= n; i += 3) {
        queryStringk += QString("(?%1, ?%2, ?%3),").arg(i).arg(i + 1).arg(i + 2);
    }
    queryStringk.chop(1); //remove last comma
    return queryStringk;
}

void addFeatureKeys(const QList<U2FeatureKey> &keys, const U2DataId &featureId, DbRef *db, U2OpStatus& os) {
    SQLiteTransaction t(db, os);
    Q_UNUSED(t);

    const int keyCount = keys.count();
    CHECK(keyCount > 0, );

    const int maximumBoundKeysNumber = SQLiteDbi::BIND_PARAMETERS_LIMIT / 3;
    const int residualBindQueryCount = keyCount % maximumBoundKeysNumber; // 3 is the number of FeatureKey table attributes
    const int fullBindQueryCount = keyCount / maximumBoundKeysNumber;
    const bool fullQueryPresents = fullBindQueryCount > 0;

    const QString fullQueryStr = fullQueryPresents ? getFeatureKeyInsertQuery(maximumBoundKeysNumber) : QString();
    const QString residualQueryStr = getFeatureKeyInsertQuery(residualBindQueryCount);

    QSharedPointer<SQLiteQuery> residualQuery(t.getPreparedQuery(residualQueryStr, db, os));
    QSharedPointer<SQLiteQuery> fullQuery;
    if (fullQueryPresents) {
        fullQuery = t.getPreparedQuery(fullQueryStr, db, os);
    }

    for (int i = 1; i <= 3 * residualBindQueryCount; i += 3) {
        const U2FeatureKey &key = keys[(i - 1) / 3];
        residualQuery->bindDataId(i, featureId);
        residualQuery->bindString(i + 1, key.name);
        residualQuery->bindString(i + 2, key.value);
    }
    residualQuery->insert();
    CHECK_OP(os, );

    if (fullQueryPresents) {
        SAFE_POINT(NULL != fullQuery.data(), "Invalid database query detected", );
        for (int currentFullQuery = 0; currentFullQuery < fullBindQueryCount && !os.isCoR(); ++currentFullQuery) {
            const int firstBindingPos = residualBindQueryCount + currentFullQuery * maximumBoundKeysNumber;
            const int lastBindingPos = residualBindQueryCount + (currentFullQuery + 1) * maximumBoundKeysNumber;
            for (int keyNum = firstBindingPos, paramNum = 1; keyNum < lastBindingPos; ++keyNum, paramNum += 3) {
                const U2FeatureKey &key = keys[keyNum];
                fullQuery->bindDataId(paramNum, featureId);
                fullQuery->bindString(paramNum + 1, key.name);
                fullQuery->bindString(paramNum + 2, key.value);
            }
            fullQuery->insert();
            CHECK_OP(os, );
            fullQuery->reset();
        }
    }
}

}

void SQLiteFeatureDbi::createFeature(U2Feature& feature, const QList<U2FeatureKey>& keys, U2OpStatus& os) {
    SQLiteTransaction t(db, os);

    static const QString queryStringf("INSERT INTO Feature(class, type, parent, root, name, sequence, strand, start, len, nameHash) "
                                                   "VALUES(?1,    ?2,   ?3,     ?4,   ?5,   ?6,       ?7,     ?8,    ?9,   ?10)");
    QSharedPointer<SQLiteQuery> qf = t.getPreparedQuery(queryStringf, db, os);

    static const QString queryStringr("INSERT INTO FeatureLocationRTreeIndex(id, start, end) VALUES(?1, ?2, ?3)");
    QSharedPointer<SQLiteQuery> qr = t.getPreparedQuery(queryStringr, db, os);

    CHECK_OP(os,);
    qf->bindInt32(1, feature.featureClass);
    qf->bindInt32(2, feature.featureType);
    qf->bindDataId(3, feature.parentFeatureId);
    qf->bindDataId(4, feature.rootFeatureId);
    qf->bindString(5, feature.name);
    qf->bindDataId(6, feature.sequenceId);
    qf->bindInt32(7, feature.location.strand.getDirectionValue());
    qf->bindInt64(8, feature.location.region.startPos);
    qf->bindInt64(9, feature.location.region.length);
    qf->bindInt32(10, qHash(feature.name));
    feature.id = qf->insert(U2Type::Feature);
    CHECK_OP(os, );

    qr->bindDataId(1, feature.id);
    qr->bindInt64(2, feature.location.region.startPos);
    qr->bindInt64(3, feature.location.region.endPos());
    qr->execute();
    CHECK_OP(os, );

    addFeatureKeys(keys, feature.id, db, os);
}

void SQLiteFeatureDbi::addKey(const U2DataId& featureId, const U2FeatureKey& key, U2OpStatus& os) {
    DBI_TYPE_CHECK(featureId, U2Type::Feature, os,);

    SQLiteWriteQuery qk("INSERT INTO FeatureKey(feature, name, value) VALUES(?1, ?2, ?3)"  , db, os);
    addKeyCommon(qk, featureId, key);
}

void SQLiteFeatureDbi::removeAllKeys(const U2DataId& featureId, const QString& keyName, U2OpStatus& os) {
    DBI_TYPE_CHECK(featureId, U2Type::Feature, os,);

    SQLiteWriteQuery q("DELETE FROM FeatureKey WHERE feature = ?1 AND name = ?2" , db, os);
    q.bindDataId(1, featureId);
    q.bindString(2, keyName);
    q.execute();
}

void SQLiteFeatureDbi::removeKey(const U2DataId& featureId, const U2FeatureKey& key, U2OpStatus& os) {
    DBI_TYPE_CHECK(featureId, U2Type::Feature, os,);

    SQLiteWriteQuery q("DELETE FROM FeatureKey WHERE feature = ?1 AND name = ?2 AND value = ?3" , db, os);
    q.bindDataId(1, featureId);
    q.bindString(2, key.name);
    q.bindString(3, key.value);
    q.execute();
}

void SQLiteFeatureDbi::updateName(const U2DataId& featureId, const QString& newName, U2OpStatus& os){
    DBI_TYPE_CHECK(featureId, U2Type::Feature, os,);

    SQLiteWriteQuery qf("UPDATE Feature SET name = ?3, nameHash = ?2 WHERE id = ?1" , db, os);
    qf.bindDataId(1, featureId);
    qf.bindInt32(2, qHash(newName));
    qf.bindString(3, newName);
    qf.execute();
}

void SQLiteFeatureDbi::updateParentId(const U2DataId& featureId, const U2DataId& parentId, U2OpStatus& os){
    DBI_TYPE_CHECK(featureId, U2Type::Feature, os,);
    DBI_TYPE_CHECK(parentId, U2Type::Feature, os,);

    SQLiteWriteQuery qf("UPDATE Feature SET parent = ?1 WHERE id = ?2" , db, os);
    qf.bindDataId(1, parentId);
    qf.bindDataId(2, featureId);
    qf.execute();
}

void SQLiteFeatureDbi::updateSequenceId(const U2DataId &featureId, const U2DataId &seqId, U2OpStatus &os) {
    DBI_TYPE_CHECK(featureId, U2Type::Feature, os,);
    DBI_TYPE_CHECK(seqId, U2Type::Sequence, os,);

    SQLiteWriteQuery qf("UPDATE Feature SET sequence = ?1 WHERE id = ?2" , db, os);
    qf.bindDataId(1, seqId);
    qf.bindDataId(2, featureId);
    qf.execute();
}

void SQLiteFeatureDbi::updateKeyValue(const U2DataId& featureId, const U2FeatureKey& key, U2OpStatus& os) {
    DBI_TYPE_CHECK(featureId, U2Type::Feature, os,);

    SQLiteWriteQuery q("UPDATE FeatureKey SET value = ?3 WHERE feature = ?1 AND name = ?2" , db, os);
    q.bindDataId(1, featureId);
    q.bindString(2, key.name);
    q.bindString(3, key.value);
    q.execute();
}

bool SQLiteFeatureDbi::getKeyValue(const U2DataId &featureId, U2FeatureKey &key, U2OpStatus &os) {
    DBI_TYPE_CHECK(featureId, U2Type::Feature, os, false);

    static const QString queryString("SELECT value FROM FeatureKey WHERE feature = ?1 AND name = ?2");
    SQLiteReadQuery q(queryString, db, os);
    CHECK_OP(os, false);

    q.bindDataId(1, featureId);
    q.bindString(2, key.name);

    if (q.step()) {
        key.value = q.getCString(0);
        return true;
    } else {
        return false;
    }
}

void SQLiteFeatureDbi::updateLocation(const U2DataId& featureId, const U2FeatureLocation& location, U2OpStatus& os) {
    DBI_TYPE_CHECK(featureId, U2Type::Feature, os,);

    SQLiteTransaction t(db, os);

    SQLiteWriteQuery qf("UPDATE Feature SET strand = ?1, start = ?2, len = ?3 WHERE id = ?4" , db, os);
    qf.bindInt32(1, location.strand.getDirectionValue());
    qf.bindInt64(2, location.region.startPos);
    qf.bindInt64(3, location.region.length);
    qf.bindDataId(4, featureId);
    qf.execute();
    CHECK_OP(os,);

    SQLiteWriteQuery qr("UPDATE FeatureLocationRTreeIndex SET start = ?1, end = ?2 WHERE id = ?3" , db, os);
    qr.bindInt64(1, location.region.startPos);
    qr.bindInt64(2, location.region.endPos());
    qr.bindDataId(3, featureId);
    qr.execute();
}

void SQLiteFeatureDbi::updateType(const U2DataId &featureId, U2FeatureType newType, U2OpStatus &os) {
    DBI_TYPE_CHECK(featureId, U2Type::Feature, os,);

    SQLiteWriteQuery qf("UPDATE Feature SET type = ?2 WHERE id = ?1" , db, os);
    qf.bindDataId(1, featureId);
    qf.bindInt32(2, newType);
    qf.execute();
}

void SQLiteFeatureDbi::removeFeature(const U2DataId& featureId, U2OpStatus& os) {
    DBI_TYPE_CHECK(featureId, U2Type::Feature, os,);

    SQLiteTransaction t(db, os);

    SQLiteWriteQuery qk("DELETE FROM Feature WHERE id = ?1" , db, os);
    qk.bindDataId(1, featureId);
    qk.execute();
}

void SQLiteFeatureDbi::removeFeaturesByParent(const U2DataId &parentId, U2OpStatus &os, SubfeatureSelectionMode mode) {
    DBI_TYPE_CHECK(parentId, U2Type::Feature, os,);

    const bool includeParent = SelectParentFeature == mode;

    SQLiteTransaction t(db, os);

    SQLiteWriteQuery qf("DELETE FROM Feature WHERE parent = ?1"
        + (includeParent ? QString(" OR id = ?2") : ""), db, os);
    qf.bindDataId(1, parentId);
    if (includeParent) {
        qf.bindDataId(2, parentId);
    }
    qf.execute();
}

namespace {

void executeDeleteFeaturesByParentsQuery(const QList<U2DataId> &parentIds, DbRef *db, U2OpStatus &os) {
    SAFE_POINT(NULL != db, "Invalid database handler", );

    QString idsList = "(";
    for (int i = 1, n = parentIds.count(); i <= n; i++) {
        idsList += QString("?%1,").arg(i);
    }
    idsList.chop(1); // remove last comma
    idsList += ")";

    SQLiteWriteQuery qf(QString("DELETE FROM Feature WHERE parent IN %1 OR id IN %1").arg(idsList), db, os);
    for (int i = 1, n = parentIds.count(); i <= n; i++) {
        qf.bindDataId(i, parentIds.at(i - 1));
    }
    qf.execute();
}

}

void SQLiteFeatureDbi::removeFeaturesByParents(const QList<U2DataId> &parentIds, U2OpStatus &os) {
    SQLiteTransaction t(db, os);
    Q_UNUSED(t);

    int parentsNumber = parentIds.count();
    if (parentsNumber <= SQLiteDbi::BIND_PARAMETERS_LIMIT) {
        executeDeleteFeaturesByParentsQuery(parentIds, db, os);
    } else {
        int deletedFeaturesNumber = 0;
        while (parentsNumber - deletedFeaturesNumber > 0) {
            int numDeletions = parentsNumber - deletedFeaturesNumber >= SQLiteDbi::BIND_PARAMETERS_LIMIT ? SQLiteDbi::BIND_PARAMETERS_LIMIT : -1;
            const QList<U2DataId> copiedAnnotations = parentIds.mid(deletedFeaturesNumber, numDeletions);
            executeDeleteFeaturesByParentsQuery(copiedAnnotations, db, os);
            deletedFeaturesNumber += SQLiteDbi::BIND_PARAMETERS_LIMIT;
        }
    }
}

void SQLiteFeatureDbi::removeFeaturesByRoot(const U2DataId &rootId, U2OpStatus &os, SubfeatureSelectionMode mode) {
    DBI_TYPE_CHECK(rootId, U2Type::Feature, os,);

    const bool includeParent = SelectParentFeature == mode;

    SQLiteTransaction t(db, os);

    SQLiteWriteQuery("DROP TRIGGER FeatureDeletion", db, os).execute();
    CHECK_OP(os,);

    SQLiteWriteQuery qf("DELETE FROM Feature WHERE root = ?1"
        + (includeParent ? QString(" OR id = ?2") : ""), db, os);
    qf.bindDataId(1, rootId);
    if (includeParent) {
        qf.bindDataId(2, rootId);
    }
    qf.execute();
    CHECK_OP(os,);

    SQLiteWriteQuery(getQueryForFeatureDeletionTrigger(), db, os).execute();
}

U2DbiIterator<U2Feature> * SQLiteFeatureDbi::getFeaturesByRegion(const U2Region& reg, const U2DataId& rootId, const QString& featureName,
    const U2DataId& seqId, U2OpStatus& os, bool contains)
{
    SQLiteTransaction t(db, os);

    const bool selectByRoot = !rootId.isEmpty();
    const QString queryByRegion = "SELECT " + FDBI_FIELDS + " FROM Feature AS f "
        "INNER JOIN FeatureLocationRTreeIndex AS fr ON f.id = fr.id WHERE "
        + (selectByRoot ? QString("f.root = ?3 AND ") : QString())
        + (contains ? "fr.start >= ?1 AND fr.end <= ?2" : "fr.start <= ?2 AND fr.end >= ?1");

    QSharedPointer<SQLiteQuery> q = t.getPreparedQuery(queryByRegion, db, os);

    q->bindInt64(1, reg.startPos);
    q->bindInt64(2, reg.endPos() - 1);
    if (selectByRoot) {
        q->bindDataId(3, rootId);
    }

    CHECK_OP(os, NULL);
    return new SQLiteResultSetIterator<U2Feature>(q, new SqlFeatureRSLoader(),
        new SqlFeatureFilter(featureName, seqId), U2Feature(), os);
}

U2DbiIterator<U2Feature> * SQLiteFeatureDbi::getFeaturesBySequence(const QString &featureName, const U2DataId &seqId, U2OpStatus &os) {
    SQLiteTransaction t(db, os);
    static const QString queryStringk("SELECT " + FDBI_FIELDS + " FROM Feature AS f "
        "WHERE f.sequence = ?1 and f.name = ?2 ORDER BY f.start");
    QSharedPointer<SQLiteQuery> q =  t.getPreparedQuery(queryStringk, db, os);

    q->bindDataId(1, seqId);
    q->bindString(2, featureName);
    CHECK_OP(os, NULL);
    return new SQLiteResultSetIterator<U2Feature>(q, new SqlFeatureRSLoader(), NULL, U2Feature(), os);
}

U2DbiIterator<U2Feature> * SQLiteFeatureDbi::getFeaturesByParent(const U2DataId &parentId, const QString &featureName, const U2DataId &seqId,
    U2OpStatus &os, SubfeatureSelectionMode mode)
{
    SQLiteTransaction t(db, os);
    const bool includeParent = SelectParentFeature == mode;
    const QString queryStringk("SELECT " + FDBI_FIELDS + " FROM Feature AS f "
        "WHERE f.parent = ?1" + (includeParent ? " OR f.id = ?2" : "") + " ORDER BY f.start");
    QSharedPointer<SQLiteQuery> q =  t.getPreparedQuery(queryStringk, db, os);

    q->bindDataId(1, parentId);
    if (includeParent) {
        q->bindDataId(2, parentId);
    }
    CHECK_OP(os, NULL);
    return new SQLiteResultSetIterator<U2Feature>(q, new SqlFeatureRSLoader(),
        new SqlFeatureFilter(featureName, seqId), U2Feature(), os);
}

U2DbiIterator<U2Feature> * SQLiteFeatureDbi::getFeaturesByRoot(const U2DataId &rootId, const FeatureFlags &types, U2OpStatus &os) {
    SQLiteTransaction t(db, os);
    const QString queryStringk("SELECT " + FDBI_FIELDS + " FROM Feature AS f "
        "WHERE f.root = ?1" + getWhereQueryPartFromType("f", types) +  "ORDER BY f.start");
    QSharedPointer<SQLiteQuery> q =  t.getPreparedQuery(queryStringk, db, os);

    q->bindDataId(1, rootId);
    CHECK_OP(os, NULL);
    return new SQLiteResultSetIterator<U2Feature>(q, new SqlFeatureRSLoader(),
        new SqlFeatureFilter(QString(), U2DataId()), U2Feature(), os);
}

U2DbiIterator<U2Feature> * SQLiteFeatureDbi::getFeaturesByName(const U2DataId &rootId, const QString &name, const FeatureFlags &types, U2OpStatus &os) {
    SQLiteTransaction t(db, os);
    const QString queryStringk("SELECT " + FDBI_FIELDS + " FROM Feature AS f "
        "WHERE f.root = ?1" + getWhereQueryPartFromType("f", types) +  " AND nameHash = ?2 ORDER BY f.start");
    QSharedPointer<SQLiteQuery> q =  t.getPreparedQuery(queryStringk, db, os);

    q->bindDataId(1, rootId);
    q->bindInt32(2, qHash(name));
    CHECK_OP(os, NULL);
    return new SQLiteResultSetIterator<U2Feature>(q, new SqlFeatureRSLoader(), new SqlFeatureFilter(QString(), U2DataId()), U2Feature(), os);
}

QList<FeatureAndKey> SQLiteFeatureDbi::getFeatureTable(const U2DataId &rootFeatureId, U2OpStatus &os) {
    SQLiteTransaction t(db, os);
    static const QString queryStringk("SELECT " + FDBI_FIELDS + ", fk.name, fk.value FROM Feature AS f "
        "LEFT OUTER JOIN FeatureKey AS fk ON f.id = fk.feature WHERE f.root = ?1 ORDER BY f.class DESC, f.start, f.len");
    QSharedPointer<SQLiteQuery> q =  t.getPreparedQuery(queryStringk, db, os);

    q->bindDataId(1, rootFeatureId);
    QList<FeatureAndKey> result;
    CHECK_OP(os, result);

    while (q->step()) {
        FeatureAndKey fnk;
        fnk.feature = SqlFeatureRSLoader::loadStatic(q.data());
        fnk.key.name = q->getCString(10);
        fnk.key.value = q->getCString(11);
        result.append(fnk);
    }
    return result;
}

QMap<U2DataId, QStringList> SQLiteFeatureDbi::getAnnotationTablesByFeatureKey(const QStringList &values, U2OpStatus &os) {
    SQLiteTransaction t(db, os);
    QMap<U2DataId, QStringList> result;
    CHECK(!values.isEmpty(), result);
    // Pay attention here if there is the need of processing more search terms
    CHECK_EXT(values.size() < SQLiteDbi::BIND_PARAMETERS_LIMIT, os.setError("Too many search terms provided"), result);

    QString queryStringk("SELECT DISTINCT A.object, F.name FROM AnnotationTable AS A, Feature AS F, FeatureKey AS FK "
        "WHERE A.rootId = F.root AND F.id = FK.feature ");

    for (int i = 1, n = values.size(); i <= n; ++i) {
        queryStringk.append(QString("AND FK.value LIKE ?%1 ").arg(i));
    }

    queryStringk.append("COLLATE NOCASE");

    QSharedPointer<SQLiteQuery> q = t.getPreparedQuery(queryStringk, db, os);

    for (int i = 1, n = values.size(); i <= n; ++i) {
        q->bindString(i, QString("%%1%").arg(values[i - 1]));
        CHECK_OP(os, result);
    }

    while (q->step()) {
        result[q->getDataId(0, U2Type::AnnotationTable)].append(q->getString(1));
    }

    return result;
}

} //namespace
