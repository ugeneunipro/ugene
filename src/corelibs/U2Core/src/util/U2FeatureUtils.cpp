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

#include <U2Core/Annotation.h>
#include <U2Core/AnnotationGroup.h>
#include <U2Core/L10n.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2FeatureKeys.h>
#include <U2Core/U2Location.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "U2FeatureUtils.h"

namespace U2 {

U2AnnotationTable U2FeatureUtils::createAnnotationTable(const QString &tableName, const U2DbiRef &dbiRef, const QString& folder, U2OpStatus &os) {
    U2AnnotationTable result;

    const U2Feature rootFeature = U2FeatureUtils::exportAnnotationGroupToFeature(AnnotationGroup::ROOT_GROUP_NAME, U2DataId(), U2DataId(), dbiRef, os);
    CHECK_OP(os, result);

    DbiConnection connection(dbiRef, os);
    CHECK_OP(os, result);

    U2FeatureDbi *dbi = connection.dbi->getFeatureDbi();
    SAFE_POINT(NULL != dbi, "Feature DBI is not initialized!", result);

    result.visualName = tableName;
    result.rootFeature = rootFeature.id;
    dbi->createAnnotationTableObject(result, folder, os);

    return result;
}

U2AnnotationTable U2FeatureUtils::getAnnotationTable(const U2EntityRef &tableRef, U2OpStatus &os) {
    DbiConnection con(tableRef.dbiRef, os);
    CHECK_OP(os, U2AnnotationTable());
    U2FeatureDbi *featureDbi = con.dbi->getFeatureDbi();
    SAFE_POINT(NULL != featureDbi, "Feature DBI is not initialized!", U2AnnotationTable());

    return featureDbi->getAnnotationTableObject(tableRef.entityId, os);
}

U2Feature U2FeatureUtils::exportAnnotationDataToFeatures(const SharedAnnotationData &a, const U2DataId &rootFeatureId,
    const U2DataId &parentFeatureId, const U2DbiRef &dbiRef, U2OpStatus &os) {
    U2Feature feature;
    QList<U2FeatureKey> fKeys;
    SAFE_POINT(!parentFeatureId.isEmpty(), "Invalid feature ID detected!", feature);
    SAFE_POINT(dbiRef.isValid(), "Invalid DBI reference detected!", feature);
    SAFE_POINT(!a->location->regions.isEmpty(), "Invalid annotation location!", feature);

    createFeatureEntityFromAnnotationData(a, rootFeatureId, parentFeatureId, feature, fKeys);
    // when the feature is group it has to have no regions and vice versa
    const bool isMultyRegion = a->location->isMultiRegion();

    DbiConnection connection(dbiRef, os);
    CHECK_OP(os, feature);

    U2FeatureDbi *dbi = connection.dbi->getFeatureDbi();
    SAFE_POINT(NULL != dbi, "Feature DBI is not initialized!", feature);

    //store to db to get ID
    dbi->createFeature(feature, fKeys, os);
    CHECK_OP(os, feature);

    //add subfeatures
    if (isMultyRegion) {
        U2FeatureUtils::addSubFeatures(a->location->regions, a->location->strand, feature.id, rootFeatureId, dbiRef, os);
    }
    return feature;
}

U2Feature U2FeatureUtils::exportAnnotationGroupToFeature(const QString &name, const U2DataId &rootFeatureId,
    const U2DataId &parentFeatureId, const U2DbiRef &dbiRef, U2OpStatus &os) {
    U2Feature result;
    SAFE_POINT(!name.isEmpty() && (!name.contains(AnnotationGroup::GROUP_PATH_SEPARATOR) || name == AnnotationGroup::ROOT_GROUP_NAME),
        "Invalid annotation group detected!", result);
    // @parentFeatureId is not checked because it may be empty for top level features
    SAFE_POINT(dbiRef.isValid(), "Invalid DBI reference detected!", result);

    DbiConnection connection(dbiRef, os);
    CHECK_OP(os, result);

    U2FeatureDbi *dbi = connection.dbi->getFeatureDbi();
    SAFE_POINT(NULL != dbi, "Feature DBI is not initialized!", result);

    result.featureClass = U2Feature::Group;
    //store to db to get ID
    result.name = name;
    result.parentFeatureId = parentFeatureId;
    result.rootFeatureId = rootFeatureId;
    dbi->createFeature(result, QList<U2FeatureKey>(), os);

    return result;
}

namespace {

void addFeatureKeyToAnnotation(const U2FeatureKey &key, SharedAnnotationData &aData, U2OpStatus &os) {
    CHECK(key.isValid(), );

    if (Q_UNLIKELY(U2FeatureKeyOperation == key.name)) {
        if (U2FeatureKeyOperationJoin == key.value) {
            aData->setLocationOperator(U2LocationOperator_Join);
        } else if (U2FeatureKeyOperationOrder == key.value) {
            aData->setLocationOperator(U2LocationOperator_Order);
        } else if (U2FeatureKeyOperationBond == key.value) {
            aData->setLocationOperator(U2LocationOperator_Bond);
        } else {
            CHECK_EXT(false, os.setError(QObject::tr("Unexpected feature operator value detected.")), );
        }
    } else if (Q_UNLIKELY(U2FeatureKeyCase == key.name)) {
        aData->caseAnnotation = true;
    } else {
        aData->qualifiers << U2Qualifier(key.name, key.value);
    }
}

}

void U2FeatureUtils::removeFeature(const U2DataId &featureId, const U2DbiRef &dbiRef, U2OpStatus &os) {
    SAFE_POINT(!featureId.isEmpty(), "Invalid feature detected!", );
    SAFE_POINT(dbiRef.isValid(), "Invalid DBI reference detected!", );

    DbiConnection connection(dbiRef, os);
    CHECK_OP(os, );
    U2FeatureDbi *dbi = connection.dbi->getFeatureDbi();
    SAFE_POINT(NULL != dbi, "Invalid DBI pointer encountered!", );

    dbi->removeFeaturesByParent(featureId, os, SelectParentFeature);
}

void U2FeatureUtils::removeFeatures(const QList<U2DataId> &featureIds, const U2DbiRef &dbiRef, U2OpStatus &os) {
    foreach(const U2DataId &featureId, featureIds) {
        SAFE_POINT(!featureId.isEmpty(), "Invalid feature detected!", );
    }
    SAFE_POINT(dbiRef.isValid(), "Invalid DBI reference detected!", );

    DbiConnection connection(dbiRef, os);
    CHECK_OP(os, );
    U2FeatureDbi *dbi = connection.dbi->getFeatureDbi();
    SAFE_POINT(NULL != dbi, "Invalid DBI pointer encountered!", );

    dbi->removeFeaturesByParents(featureIds, os);
}

void U2FeatureUtils::addSubFeatures(const QVector<U2Region> &regions, const U2Strand &strand, const U2DataId &parentFeatureId,
    const U2DataId &rootFeatureId, const U2DbiRef &dbiRef, U2OpStatus &os) {
    SAFE_POINT(!parentFeatureId.isEmpty(), "Invalid feature ID detected!", );
    SAFE_POINT(dbiRef.isValid(), "Invalid DBI reference detected!", );
    CHECK(!regions.isEmpty(), );

    DbiConnection connection(dbiRef, os);
    CHECK_OP(os, );
    U2FeatureDbi *dbi = connection.dbi->getFeatureDbi();
    SAFE_POINT(NULL != dbi, "Invalid DBI pointer encountered!", );

    foreach(const U2Region &reg, regions) {
        SAFE_POINT(!reg.isEmpty(), "Attempting to assign annotation to an empty region!", );
        U2Feature sub;
        sub.featureClass = U2Feature::Annotation;
        sub.location.region = reg;
        sub.location.strand = strand;
        sub.parentFeatureId = parentFeatureId;
        sub.rootFeatureId = rootFeatureId;
        dbi->createFeature(sub, QList<U2FeatureKey>(), os);
        CHECK_OP(os, );
    }
}

QList<FeatureAndKey> U2FeatureUtils::getSortedSubgroups(QList<FeatureAndKey> &fkList, const U2DataId &parentId) {
    QList<FeatureAndKey> result;
    if (fkList.size() == 0) {
        return result;
    }
    int i = 0;
    FeatureAndKey fk = fkList[i];
    while (fk.feature.featureClass == U2Feature::Group) {
        if (fk.feature.parentFeatureId == parentId) {
            result.append(fk);
            fkList.removeAt(i);
        } else {
            i++;
        }
        fk = fkList[i];
    }
    foreach(const FeatureAndKey &fk, result) {
        result.append(getSortedSubgroups(fkList, fk.feature.id));
    }
    return result;
}

AnnotationGroup * U2FeatureUtils::loadAnnotationTable(const U2DataId &rootFeatureId, const U2DbiRef &dbiRef,
    AnnotationTableObject *parentObj, U2OpStatus &os) {
    SAFE_POINT(!rootFeatureId.isEmpty(), "Invalid feature ID detected!", NULL);
    SAFE_POINT(NULL != parentObj, L10N::nullPointerError("Annotation table object"), NULL);
    SAFE_POINT(dbiRef.isValid(), "Invalid DBI reference detected!", NULL);

    DbiConnection connection(dbiRef, os);
    CHECK_OP(os, NULL);
    U2FeatureDbi *dbi = connection.dbi->getFeatureDbi();
    SAFE_POINT(NULL != dbi, L10N::nullPointerError("Feature DBI"), NULL);

    AnnotationGroup *rootGroup = new AnnotationGroup(rootFeatureId, AnnotationGroup::ROOT_GROUP_NAME, NULL, parentObj);

    Annotation *currentAnnotation = NULL;
    QList<FeatureAndKey> rawData = dbi->getFeatureTable(rootFeatureId, os);
    QList<FeatureAndKey> groups = getSortedSubgroups(rawData, rootFeatureId);
    rawData = groups + rawData;

    foreach(const FeatureAndKey &fnk, rawData) {
        if (U2Feature::Group == fnk.feature.featureClass) {
            rootGroup->addSubgroup(fnk.feature);
        } else if (NULL != (currentAnnotation = rootGroup->findAnnotationById(fnk.feature.parentFeatureId))) {
            // add a region to an already fetched annotation
            SAFE_POINT(!fnk.feature.location.region.isEmpty() && !fnk.key.isValid(), "Unexpected feature data fetched from DB", NULL);
            const_cast<SharedAnnotationData &>(currentAnnotation->getData())->location->regions.append(fnk.feature.location.region);
        } else if (NULL != (currentAnnotation = rootGroup->findAnnotationById(fnk.feature.id))) {
            // add a qualifier to an already fetched annotation
            SAFE_POINT(fnk.key.isValid(), "Valid feature key expected", NULL);
            addFeatureKeyToAnnotation(fnk.key, const_cast<SharedAnnotationData &>(currentAnnotation->getData()), os);
        } else {
            // fetch annotation
            SharedAnnotationData aData(new AnnotationData);
            aData->type = fnk.feature.featureType;
            aData->name = fnk.feature.name;
            aData->location->strand = fnk.feature.location.strand;
            if (!fnk.feature.location.region.isEmpty()) {
                aData->location->regions.append(fnk.feature.location.region);
            }
            addFeatureKeyToAnnotation(fnk.key, aData, os);
            CHECK_OP(os, NULL);

            AnnotationGroup *parentGroup = rootGroup->findSubgroupById(fnk.feature.parentFeatureId);
            SAFE_POINT(NULL != parentGroup, L10N::nullPointerError("annotation group"), NULL);
            parentGroup->addShallowAnnotations(QList<Annotation *>() << new Annotation(fnk.feature.id, aData, parentGroup, parentObj), false);
        }
    }
    return rootGroup;
}

void U2FeatureUtils::updateFeatureParent(const U2DataId &featureId, const U2DataId &newParentId, const U2DbiRef &dbiRef, U2OpStatus &os) {
    SAFE_POINT(!featureId.isEmpty() && !newParentId.isEmpty(), "Invalid feature ID detected!", );
    SAFE_POINT(dbiRef.isValid(), "Invalid DBI reference detected!", );

    DbiConnection connection(dbiRef, os);
    CHECK_OP(os, );
    U2FeatureDbi *fDbi = connection.dbi->getFeatureDbi();
    SAFE_POINT(NULL != fDbi, "Invalid DBI pointer encountered!", );

    fDbi->updateParentId(featureId, newParentId, os);
    CHECK_OP(os, );
}

void U2FeatureUtils::updateFeatureType(const U2DataId &featureId, U2FeatureType newType, const U2DbiRef &dbiRef, U2OpStatus &os) {
    SAFE_POINT(!featureId.isEmpty(), "Invalid feature ID detected!", );

    DbiConnection connection(dbiRef, os);
    CHECK_OP(os, );
    U2FeatureDbi *fDbi = connection.dbi->getFeatureDbi();
    SAFE_POINT(NULL != fDbi, "Invalid DBI pointer encountered!", );

    fDbi->updateType(featureId, newType, os);
}

void U2FeatureUtils::updateFeatureName(const U2DataId &featureId, const QString &newName, const U2DbiRef &dbiRef, U2OpStatus &os) {
    SAFE_POINT(!featureId.isEmpty(), "Invalid feature ID detected!", );
    SAFE_POINT(!newName.isEmpty(), "Attempting to set an empty name for a feature!", );

    DbiConnection connection;
    connection.open(dbiRef, os);
    CHECK_OP(os, );
    U2FeatureDbi *fDbi = connection.dbi->getFeatureDbi();
    SAFE_POINT(NULL != fDbi, "Invalid DBI pointer encountered!", );

    fDbi->updateName(featureId, newName, os);
}

void U2FeatureUtils::updateFeatureLocation(const U2DataId &featureId, const U2DataId &rootFeatureId, const U2Location &location,
    const U2DbiRef &dbiRef, U2OpStatus &os) {
    SAFE_POINT(!featureId.isEmpty(), "Invalid feature ID detected!", );
    SAFE_POINT(dbiRef.isValid(), "Invalid DBI reference detected!", );

    DbiConnection connection;
    connection.open(dbiRef, os);
    CHECK_OP(os, );
    U2FeatureDbi *dbi = connection.dbi->getFeatureDbi();
    SAFE_POINT(NULL != dbi, "Invalid DBI pointer encountered!", );

    dbi->removeFeaturesByParent(featureId, os, NotSelectParentFeature);

    const bool isMultyRegion = location->isMultiRegion();
    if (isMultyRegion) {
        U2FeatureLocation newLocation(location->strand, U2Region());
        dbi->updateLocation(featureId, newLocation, os);

        CHECK_OP(os, );
        U2FeatureUtils::addSubFeatures(location->regions, location->strand, featureId, rootFeatureId, dbiRef, os);
    } else {
        U2FeatureLocation newLocation(location->strand, location->regions.first());
        dbi->updateLocation(featureId, newLocation, os);
    }
    // update location operator
    const QList<U2FeatureKey> keys = dbi->getFeatureKeys(featureId, os);
    CHECK_OP(os, );
    U2FeatureKey locationOpKey(U2FeatureKeyOperation, QString());
    const bool valueFound = dbi->getKeyValue(featureId, locationOpKey, os);
    SAFE_POINT(valueFound && !locationOpKey.value.isEmpty(), "Invalid annotation's location operator value!", );

    const U2FeatureKey newOpKey = createFeatureKeyLocationOperator(location->op);
    if (newOpKey.value != locationOpKey.value) {
        dbi->updateKeyValue(featureId, newOpKey, os);
        CHECK_OP(os, );
    }
}

void U2FeatureUtils::addFeatureKey(const U2DataId &featureId, const U2FeatureKey& key, const U2DbiRef &dbiRef, U2OpStatus &os) {
    SAFE_POINT(!featureId.isEmpty(), "Invalid feature ID detected!", );
    SAFE_POINT(dbiRef.isValid(), "Invalid DBI reference detected!", );
    SAFE_POINT(!key.name.isEmpty(), "Qualifier with an empty name detected!", );

    DbiConnection connection(dbiRef, os);
    CHECK_OP(os, );
    U2FeatureDbi *dbi = connection.dbi->getFeatureDbi();
    SAFE_POINT(NULL != dbi, "Invalid DBI pointer encountered!", );

    dbi->addKey(featureId, key, os);
}

void U2FeatureUtils::removeFeatureKey(const U2DataId &featureId, const U2FeatureKey& key, const U2DbiRef &dbiRef, U2OpStatus &os) {
    SAFE_POINT(!featureId.isEmpty(), "Invalid feature ID detected!", );
    SAFE_POINT(dbiRef.isValid(), "Invalid DBI reference detected!", );
    SAFE_POINT(!key.name.isEmpty(), "Qualifier with an empty name detected!", );

    DbiConnection connection(dbiRef, os);
    CHECK_OP(os, );
    U2FeatureDbi *dbi = connection.dbi->getFeatureDbi();
    SAFE_POINT(NULL != dbi, "Invalid DBI pointer encountered!", );

    dbi->removeKey(featureId, key, os);
}

SharedAnnotationData U2FeatureUtils::getAnnotationDataFromFeature(const U2DataId &featureId, const U2DbiRef &dbiRef, U2OpStatus &op) {
    SharedAnnotationData result(new AnnotationData);
    SAFE_POINT(!featureId.isEmpty(), "Invalid feature detected!", result);
    SAFE_POINT(dbiRef.isValid(), "Invalid DBI reference detected!", result);

    DbiConnection connection;
    connection.open(dbiRef, op);
    CHECK_OP(op, result);
    U2FeatureDbi *fDbi = connection.dbi->getFeatureDbi();
    SAFE_POINT(NULL != fDbi, "Invalid DBI pointer encountered!", result);

    // extract feature keys first to check if the feature actually represents an annotation group
    QList<U2FeatureKey> fKeys = fDbi->getFeatureKeys(featureId, op);
    CHECK_OP(op, result);
    foreach(const U2FeatureKey &key, fKeys) {
        addFeatureKeyToAnnotation(key, result, op);
        CHECK_OP(op, result);
    }

    const QList<U2Feature> features = U2FeatureUtils::getFeaturesByParent(featureId, dbiRef, op, Nonrecursive, U2Feature::Annotation, SelectParentFeature);
    CHECK_OP(op, result);

    U2Feature annotatingFeature;
    QVector<U2Region> regions;
    foreach(const U2Feature &f, features) {
        SAFE_POINT_EXT(U2Feature::Annotation == f.featureClass, op.setError("Invalid feature type detected!"), result);
        if (Q_UNLIKELY(f.id == featureId)) {
            CHECK_EXT(!annotatingFeature.hasValidId(), op.setError("Invalid feature selection occurred!"), result);
            annotatingFeature = f;
        }
        if (Q_LIKELY(U2Region() != f.location.region)) {
            regions << f.location.region;
        }
    }

    //copy data
    result->location->strand = annotatingFeature.location.strand;

    result->location->regions = regions;
    result->type = annotatingFeature.featureType;
    result->name = annotatingFeature.name;

    return result;
}

QList<U2Feature> U2FeatureUtils::getSubAnnotations(const U2DataId &parentFeatureId, const U2DbiRef &dbiRef, U2OpStatus &os,
    OperationScope resursive, ParentFeatureStatus parent) {
    return (Root == parent)
        ? getFeaturesByRoot(parentFeatureId, dbiRef, os, resursive, U2Feature::Annotation)
        : getFeaturesByParent(parentFeatureId, dbiRef, os, resursive, U2Feature::Annotation);
}

QList<U2Feature> U2FeatureUtils::getSubGroups(const U2DataId &parentFeatureId, const U2DbiRef &dbiRef, U2OpStatus &os,
    OperationScope resursive, ParentFeatureStatus parent) {
    return (Root == parent)
        ? getFeaturesByRoot(parentFeatureId, dbiRef, os, resursive, U2Feature::Group)
        : getFeaturesByParent(parentFeatureId, dbiRef, os, resursive, U2Feature::Group);
}

QList<U2Feature> U2FeatureUtils::getFeaturesByParent(const U2DataId &parentFeatureId, const U2DbiRef &dbiRef, U2OpStatus &os,
    OperationScope scope, const FeatureFlags &featureClass, SubfeatureSelectionMode mode) {
    QList<U2Feature> result;
    SAFE_POINT(!parentFeatureId.isEmpty(), "Invalid feature detected!", result);
    SAFE_POINT(dbiRef.isValid(), "Invalid DBI reference detected!", result);

    DbiConnection connection;
    connection.open(dbiRef, os);
    CHECK_OP(os, result);
    U2FeatureDbi *dbi = connection.dbi->getFeatureDbi();
    SAFE_POINT(NULL != dbi, "Invalid DBI pointer encountered!", result);

    QScopedPointer<U2DbiIterator<U2Feature> > featureIter(dbi->getFeaturesByParent(parentFeatureId, QString(), U2DataId(), os, mode));
    CHECK_OP(os, result);

    QList<U2Feature> subfeatures;
    while (featureIter->hasNext()) {
        const U2Feature feature = featureIter->next();
        if (featureClass.testFlag(feature.featureClass)) {
            result << feature;
        }
        CHECK_OP(os, result);
        subfeatures << feature;
    }
    if (Recursive == scope) {
        foreach(const U2Feature &feature, subfeatures) {
            result << getFeaturesByParent(feature.id, dbiRef, os, Recursive, featureClass, NotSelectParentFeature);
            CHECK_OP(os, result);
        }
    }

    return result;
}

QList<U2Feature> U2FeatureUtils::getFeaturesByRoot(const U2DataId &rootFeatureId, const U2DbiRef &dbiRef,
    U2OpStatus &os, OperationScope scope, const FeatureFlags &featureClass) {
    QList<U2Feature> result;
    SAFE_POINT(!rootFeatureId.isEmpty(), "Invalid feature detected!", result);
    SAFE_POINT(dbiRef.isValid(), "Invalid DBI reference detected!", result);

    DbiConnection connection;
    connection.open(dbiRef, os);
    CHECK_OP(os, result);
    U2FeatureDbi *dbi = connection.dbi->getFeatureDbi();
    SAFE_POINT(NULL != dbi, "Invalid DBI pointer encountered!", result);

    QScopedPointer<U2DbiIterator<U2Feature> > featureIter(dbi->getFeaturesByRoot(rootFeatureId, featureClass, os));
    CHECK_OP(os, result);

    QList<U2Feature> subfeatures;
    while (featureIter->hasNext()) {
        const U2Feature feature = featureIter->next();
        if (featureClass.testFlag(feature.featureClass)) {
            if (!feature.name.isEmpty() && (Recursive == scope || feature.parentFeatureId == rootFeatureId)) {
                result << feature;
            }
            CHECK_OP(os, result);
        } else {
            assert(false);
            coreLog.error("Unexpected feature type is fetched from the DB");
        }
    }

    return result;
}

U2Feature U2FeatureUtils::getFeatureById(const U2DataId &id, const U2DbiRef &dbiRef, U2OpStatus &op) {
    U2Feature result;
    SAFE_POINT(!id.isEmpty(), "Invalid feature ID detected!", result);
    SAFE_POINT(dbiRef.isValid(), "Invalid DBI reference detected!", result);

    DbiConnection connection;
    connection.open(dbiRef, op);
    CHECK_OP(op, result);

    U2FeatureDbi *dbi = connection.dbi->getFeatureDbi();
    SAFE_POINT(NULL != dbi, "Feature DBI is not initialized!", result);

    result = dbi->getFeature(id, op);
    return result;
}

void U2FeatureUtils::createFeatureEntityFromAnnotationData(const SharedAnnotationData &annotation, const U2DataId &rootFeatureId,
    const U2DataId &parentFeatureId, U2Feature &resFeature, QList<U2FeatureKey> &resFeatureKeys) {
    resFeature.featureClass = U2Feature::Annotation;
    //copy data
    resFeature.name = annotation->name;
    resFeature.featureType = annotation->type;
    resFeature.parentFeatureId = parentFeatureId;
    resFeature.rootFeatureId = rootFeatureId;
    resFeature.location.strand = annotation->location->strand;
    if (annotation->location->isSingleRegion()) {
        resFeature.location.region = annotation->location->regions.first();
    }

    //add qualifiers
    foreach(const U2Qualifier &qual, annotation->qualifiers) {
        resFeatureKeys.append(U2FeatureKey(qual.name, qual.value));
    }

    //add operation
    const U2FeatureKey locationOpKey = createFeatureKeyLocationOperator(annotation->location->op);
    if (locationOpKey.isValid()) {
        resFeatureKeys.append(locationOpKey);
    }
    // add case info
    if (annotation->caseAnnotation) {
        resFeatureKeys.append(U2FeatureKey(U2FeatureKeyCase, QString()));
    }
}

U2FeatureKey U2FeatureUtils::createFeatureKeyLocationOperator(U2LocationOperator value) {
    U2FeatureKey result;
    switch (value) {
    case U2LocationOperator_Join:
        result = U2FeatureKey(U2FeatureKeyOperation, U2FeatureKeyOperationJoin);
        break;
    case U2LocationOperator_Order:
        result = U2FeatureKey(U2FeatureKeyOperation, U2FeatureKeyOperationOrder);
        break;
    case U2LocationOperator_Bond:
        result = U2FeatureKey(U2FeatureKeyOperation, U2FeatureKeyOperationBond);
        break;
    default:
        SAFE_POINT(false, "Unexpected location operator!", result);
    }
    return result;
}

} // namespace U2
