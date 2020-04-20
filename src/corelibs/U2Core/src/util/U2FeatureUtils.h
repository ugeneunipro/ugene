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

#ifndef _U2_FEATURE_UTILS_H_
#define _U2_FEATURE_UTILS_H_

#include <U2Core/AnnotationData.h>
#include <U2Core/U2FeatureDbi.h>

namespace U2 {

class AnnotationGroup;
class AnnotationTableObject;
class U2Location;
class U2OpStatus;

enum ParentFeatureStatus {
    Root,
    Nonroot
};

enum OperationScope {
    Recursive,
    Nonrecursive
};

class U2CORE_EXPORT U2FeatureUtils {
public:
    /**
     * Creates DB representation of AnnotationTableObject
     */
    static U2AnnotationTable        createAnnotationTable(const QString &tableName, const U2DbiRef &dbiRef, const QString &folder, U2OpStatus &os);
    /**
     * Returns DB representation of AnnotationTableObject associated with @tableRef
     */
    static U2AnnotationTable        getAnnotationTable(const U2EntityRef &tableRef, U2OpStatus &os);
    /**
     * In case of multiple regions, feature contains all the names and qualifiers
     * and subfeatures contain only regions, in case of single regions features
     * contains all the information from the annotation
     */
    static U2Feature                exportAnnotationDataToFeatures(const SharedAnnotationData &a, const U2DataId &rootFeatureId,
                                        const U2DataId &parentFeatureId, const U2DbiRef &dbiRef, U2OpStatus &os);
    /**
     * Creates a feature having @name and @parentId without location
     */
    static U2Feature                exportAnnotationGroupToFeature(const QString &name, const U2DataId &rootFeatureId,
                                        const U2DataId &parentFeatureId, const U2DbiRef &dbiRef, U2OpStatus &os);
    /**
     * Removes feature, its children from db
     */
    static void                     removeFeature(const U2DataId &featureId, const U2DbiRef &dbiRef, U2OpStatus &os);

    static void                     removeFeatures(const QList<U2DataId> &featureIds, const U2DbiRef &dbiRef, U2OpStatus &os);
    /**
     * Replaces parent feature reference with @newParentId for the feature having @featureId
     */
    static void                     updateFeatureParent(const U2DataId &featureId, const U2DataId &newParentId, const U2DbiRef &dbiRef, U2OpStatus &os);
    /**
     * For the feature having @featureId the method replaces its type with @newType
     */
    static void                     updateFeatureType(const U2DataId &featureId, U2FeatureType newType, const U2DbiRef &dbiRef, U2OpStatus &os);
    /**
     * For the feature having @featureId the method replaces its name with @newName
     */
    static void                     updateFeatureName(const U2DataId &featureId, const QString &newName, const U2DbiRef &dbiRef, U2OpStatus &os);
    /**
     * Sets @location for a feature having @featureId.
     * The feature is to represent an annotation
     */
    static void                     updateFeatureLocation(const U2DataId &featureId, const U2DataId &rootFeatureId, const U2Location &location,
                                        const U2DbiRef &dbiRef, U2OpStatus &os);
    /**
     * Adds the @key to DB for the feature having @featureId
     */
    static void                     addFeatureKey(const U2DataId &featureId, const U2FeatureKey& key, const U2DbiRef &dbiRef, U2OpStatus &os);
    /**
     * Removes the @key of the feature having @featureId from DB
     */
    static void                     removeFeatureKey(const U2DataId &featureId, const U2FeatureKey& key, const U2DbiRef &dbiRef, U2OpStatus &os);
    /**
     * Fetches all annotations according to passed @rootId, builds annotation group tree and returns the rooot group
     */
    static AnnotationGroup *        loadAnnotationTable(const U2DataId &rootFeatureId, const U2DbiRef &dbiRef,
                                        AnnotationTableObject *parentObj, U2OpStatus &os);

    //////////////////////////////////////////////////////////////////////////
    // Functions that are used for testing
    //////////////////////////////////////////////////////////////////////////
    /**
     * Imports feature to annotation. If feature's location region is empty then child features are
     * treated as subregions
     */
    static SharedAnnotationData     getAnnotationDataFromFeature(const U2DataId &featureId, const U2DbiRef &dbiRef, U2OpStatus &op);
    /**
     * Returns feature based on the DB content corresponding to the given @id
     */
    static U2Feature                getFeatureById(const U2DataId &id, const U2DbiRef &dbiRef, U2OpStatus &op);
    /**
     * Returns list of all child features representing annotations for parent feature with given id.
     * Parent is to represent annotation group.
     *
     * If @scope == Recursive the list contains all the children of child features
     * (representing annotation groups) and so on
     */
    static QList<U2Feature>         getSubAnnotations(const U2DataId &parentFeatureId, const U2DbiRef &dbiRef, U2OpStatus &os,
                                        OperationScope scope = Recursive, ParentFeatureStatus parentIs = Root);
    /**
     * Returns list of all child annotation groups for parent feature with given id.
     * Parent is to represent annotation group.
     * If @scope == Recursive the list contains all the children of child features
     * (representing annotation groups) and so on
     */
    static QList<U2Feature>         getSubGroups(const U2DataId &parentFeatureId, const U2DbiRef &dbiRef, U2OpStatus &os,
                                        OperationScope scope = Recursive, ParentFeatureStatus parent = Nonroot);

    static QList<FeatureAndKey>     getSortedSubgroups(QList<FeatureAndKey> &fkList, const U2DataId &parentId);

private:
    static QList<U2Feature>         getFeaturesByParent(const U2DataId &parentFeatureId, const U2DbiRef &dbiRef, U2OpStatus &os,
                                        OperationScope scope = Recursive, const FeatureFlags &featureClass = U2Feature::Annotation,
                                        SubfeatureSelectionMode mode = NotSelectParentFeature);
    static QList<U2Feature>         getFeaturesByRoot(const U2DataId &rootFeatureId, const U2DbiRef &dbiRef, U2OpStatus &os,
                                        OperationScope scope = Recursive, const FeatureFlags &featureClass = U2Feature::Annotation);
    /**
     * Adds a set of subfeatures to the feature having @parentFeatureId
     * and representing an annotation. Each new subfeature corresponds an item from @regions.
     * The parent feature has to represent an annotation.
     */
    static void                    addSubFeatures(const QVector<U2Region> &regions, const U2Strand &strand, const U2DataId &parentFeatureId,
                                        const U2DataId &rootFeatureId, const U2DbiRef &dbiRef, U2OpStatus &os);

    static void                     createFeatureEntityFromAnnotationData(const SharedAnnotationData &annotation, const U2DataId &rootFeatureId,
                                        const U2DataId &parentFeatureId, U2Feature &resFeature, QList<U2FeatureKey> &resFeatureKeys);
    static U2FeatureKey             createFeatureKeyLocationOperator(U2LocationOperator value);
};

} // namespace U2


#endif //_U2_FEATURE_UTILS_H_
