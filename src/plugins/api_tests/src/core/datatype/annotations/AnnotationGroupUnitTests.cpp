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

#include <QBitArray>

#include <U2Core/AnnotationData.h>
#include <U2Core/U2FeatureUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "../../gobjects/FeaturesTableObjectUnitTest.h"
#include "AnnotationGroupUnitTests.h"

namespace U2 {

static U2DbiRef getDbiRef() {
    return FeaturesTableObjectTestData::getFeatureDbi()->getRootDbi()->getDbiRef();
}

static SharedAnnotationData createTestAnnotationData() {
    const U2Region areg1(1, 2);
    const U2Region areg2(100, 200);

    SharedAnnotationData anData(new AnnotationData);
    anData->location->regions << areg1;
    anData->name = "aname";

    return anData;
}

IMPLEMENT_TEST(AnnotationGroupUnitTest, get_IdObject) {
    const U2DbiRef dbiRef(getDbiRef());
    SharedAnnotationData anData = createTestAnnotationData();
    const QString groupName1 = "subgroup1";
    const QString groupName2 = "subgroup1/subgroup11";
    const QString groupName3 = "subgroup2/subgroup21";

    AnnotationTableObject ft("aname_table", dbiRef);
    ft.addAnnotations(QList<SharedAnnotationData>() << anData, groupName1);
    ft.addAnnotations(QList<SharedAnnotationData>() << anData, groupName2);
    ft.addAnnotations(QList<SharedAnnotationData>() << anData, groupName3);

    AnnotationGroup *rootGroup = ft.getRootGroup();
    CHECK_TRUE(rootGroup->hasValidId(), "Invalid root group ID");
    const QList<AnnotationGroup *> subgroups = rootGroup->getSubgroups();
    CHECK_EQUAL(2, subgroups.size(), "Count of subgroups");

    QBitArray groupMatches(2, false);
    foreach (AnnotationGroup *subgroup, subgroups) {
        CHECK_TRUE(&ft == subgroup->getGObject(), "Unexpected parent object");
        U2OpStatusImpl os;
        const U2Feature f = U2FeatureUtils::getFeatureById(subgroup->id, dbiRef, os);
        CHECK_NO_ERROR(os);
        if ("subgroup1" == f.name) {
            groupMatches.setBit(0, true);

            const QList<AnnotationGroup *> secondLevelSubgroups = subgroup->getSubgroups();
            CHECK_EQUAL(1, secondLevelSubgroups.size(), "Count of 2nd level subgroups");

            AnnotationGroup *secondLevelSubgroup = secondLevelSubgroups.first();
            CHECK_TRUE(&ft == secondLevelSubgroup->getGObject(), "Unexpected parent object");
            CHECK_EQUAL(secondLevelSubgroup->getName(), "subgroup11", "2nd level subgroup's name");
        } else if ("subgroup2" == f.name) {
            groupMatches.setBit(1, true);

            const QList<AnnotationGroup *> secondLevelSubgroups = subgroup->getSubgroups();
            CHECK_EQUAL(1, secondLevelSubgroups.size(), "Count of 2nd level subgroups");

            AnnotationGroup *secondLevelSubgroup = secondLevelSubgroups.first();
            CHECK_TRUE(&ft == secondLevelSubgroup->getGObject(), "Unexpected parent object");
            CHECK_EQUAL(secondLevelSubgroup->getName(), "subgroup21", "2nd level subgroup's name");
        }
    }
    CHECK_EQUAL(2, groupMatches.count(true), "Count of subgroups");
}

IMPLEMENT_TEST(AnnotationGroupUnitTest, getSet_Name) {
    const U2DbiRef dbiRef(getDbiRef());
    SharedAnnotationData anData = createTestAnnotationData();
    const QString groupName = "subgroup";

    AnnotationTableObject ft("aname_table", dbiRef);
    ft.addAnnotations(QList<SharedAnnotationData>() << anData, groupName);

    AnnotationGroup *rootGroup = ft.getRootGroup();
    CHECK_TRUE(rootGroup->hasValidId(), "Invalid root group ID");
    const QList<AnnotationGroup *> subgroups = rootGroup->getSubgroups();
    CHECK_EQUAL(1, subgroups.size(), "Count of subgroups");

    AnnotationGroup *group = subgroups.first();
    CHECK_EQUAL(groupName, group->getName(), "annotation group name");

    const QString newName = "misc_feature";
    group->setName(newName);
    CHECK_EQUAL(newName, group->getName(), "annotation group name");
}

IMPLEMENT_TEST(AnnotationGroupUnitTest, groupHierarchy) {
    const U2DbiRef dbiRef(getDbiRef());
    SharedAnnotationData anData = createTestAnnotationData();
    const QString groupName1 = "subgroup1";
    const QString groupName2 = "subgroup1/subgroup11";
    const QString groupName3 = "subgroup2/subgroup21";

    AnnotationTableObject ft("aname_table", dbiRef);
    ft.addAnnotations(QList<SharedAnnotationData>() << anData, groupName1);
    ft.addAnnotations(QList<SharedAnnotationData>() << anData, groupName2);
    ft.addAnnotations(QList<SharedAnnotationData>() << anData, groupName3);

    AnnotationGroup *rootGroup = ft.getRootGroup();
    CHECK_FALSE(rootGroup->isTopLevelGroup(), "Unexpected top level group");
    CHECK_EQUAL(1, rootGroup->getGroupDepth(), "Root group's depth");
    CHECK_EQUAL(QString(), rootGroup->getGroupPath(), "Root group's path");

    const QList<AnnotationGroup *> subgroups = rootGroup->getSubgroups();
    CHECK_EQUAL(2, subgroups.size(), "Count of subgroups");

    QBitArray groupMatches(2, false);
    foreach (AnnotationGroup *subgroup, subgroups) {
        CHECK_TRUE(subgroup->isTopLevelGroup(), "Unexpected top level group");
        CHECK_TRUE(rootGroup->isParentOf(subgroup), "Unexpected parent group");
        CHECK_EQUAL(2, subgroup->getGroupDepth(), "Subgroup's depth");

        U2OpStatusImpl os;
        const U2Feature f = U2FeatureUtils::getFeatureById(subgroup->id, dbiRef, os);
        CHECK_NO_ERROR(os);
        AnnotationGroup *secondLevelSubgroup = subgroup;
        if ("subgroup1" == f.name) {
            groupMatches.setBit(0, true);
            CHECK_EQUAL("subgroup1", subgroup->getGroupPath(), "Subgroup's path");

            const QList<AnnotationGroup *> secondLevelSubgroups = subgroup->getSubgroups();
            CHECK_EQUAL(1, secondLevelSubgroups.size(), "Count of 2nd level subgroups");

            secondLevelSubgroup = secondLevelSubgroups.first();
            CHECK_EQUAL("subgroup1/subgroup11", secondLevelSubgroup->getGroupPath(), "Subgroup's path");
        } else if ("subgroup2" == f.name) {
            groupMatches.setBit(1, true);
            CHECK_EQUAL("subgroup2", subgroup->getGroupPath(), "Subgroup's path");

            const QList<AnnotationGroup *> secondLevelSubgroups = subgroup->getSubgroups();
            CHECK_EQUAL(1, secondLevelSubgroups.size(), "Count of 2nd level subgroups");

            secondLevelSubgroup = secondLevelSubgroups.first();
            CHECK_EQUAL("subgroup2/subgroup21", secondLevelSubgroup->getGroupPath(), "Subgroup's path");
        }
        CHECK_FALSE(secondLevelSubgroup->isTopLevelGroup(), "Unexpected top level group");
        CHECK_TRUE(subgroup->isParentOf(secondLevelSubgroup), "Unexpected parent group");
        CHECK_EQUAL(3, secondLevelSubgroup->getGroupDepth(), "Subgroup's depth");
    }
    CHECK_EQUAL(2, groupMatches.count(true), "Count of subgroups");
}

IMPLEMENT_TEST(AnnotationGroupUnitTest, getSet_Annotations) {
    const U2DbiRef dbiRef(getDbiRef());
    SharedAnnotationData anData1 = createTestAnnotationData();
    SharedAnnotationData anData2(new AnnotationData(*anData1));
    anData2->name = "aname2";
    SharedAnnotationData anData3(new AnnotationData(*anData1));
    anData3->name = "aname3";
    SharedAnnotationData anData4(new AnnotationData(*anData1));
    anData4->name = "aname4";
    const QString groupName1 = "subgroup1";
    const QString groupName2 = "subgroup2";

    AnnotationTableObject ft("aname_table", dbiRef);
    ft.addAnnotations(QList<SharedAnnotationData>() << anData1 << anData2 << anData3, groupName1);
    ft.addAnnotations(QList<SharedAnnotationData>() << anData4, groupName2);

    AnnotationGroup *rootGroup = ft.getRootGroup();
    const QList<AnnotationGroup *> subgroups = rootGroup->getSubgroups();
    CHECK_EQUAL(2, subgroups.size(), "Count of subgroups");

    AnnotationGroup *subgroup1 = rootGroup;
    AnnotationGroup *subgroup2 = rootGroup;
    if (subgroups.first()->getName() == groupName1) {
        subgroup1 = subgroups.first();
        subgroup2 = subgroups.last();
    } else {
        subgroup1 = subgroups.last();
        subgroup2 = subgroups.first();
    }

    QList<Annotation *> annotations = subgroup1->getAnnotations();
    CHECK_EQUAL(3, annotations.size(), "Count of annotations");

    QBitArray annotationMatch(3, false);
    foreach (Annotation *annotation, annotations) {
        if ("aname" == annotation->getName()) {
            annotationMatch.setBit(0, true);
        } else if ("aname2" == annotation->getName()) {
            annotationMatch.setBit(1, true);
        } else if ("aname3" == annotation->getName()) {
            annotationMatch.setBit(2, true);
        }
    }
    CHECK_EQUAL(3, annotationMatch.count(true), "Count of annotations");

    const QList<Annotation *> newAnnotations = subgroup2->getAnnotations();
    CHECK_EQUAL(1, newAnnotations.size(), "Count of annotations");

    subgroup1->addAnnotations(QList<SharedAnnotationData>() << newAnnotations.first()->getData());
    annotations = subgroup1->getAnnotations();
    CHECK_EQUAL(4, annotations.size(), "Count of annotations");

    subgroup1->removeAnnotations(QList<Annotation *>() << annotations.first());
    annotations = subgroup1->getAnnotations();
    CHECK_EQUAL(3, annotations.size(), "Count of annotations");

    subgroup1->removeAnnotations(annotations);
    CHECK_TRUE(subgroup1->getAnnotations().isEmpty(), "Count of annotations");
}

IMPLEMENT_TEST(AnnotationGroupUnitTest, findAnnotationsInSubtree) {
    const U2DbiRef dbiRef(getDbiRef());
    SharedAnnotationData anData1 = createTestAnnotationData();
    SharedAnnotationData anData2(new AnnotationData(*anData1));
    anData2->name = "aname2";
    SharedAnnotationData anData3(new AnnotationData(*anData1));
    anData3->name = "aname3";
    SharedAnnotationData anData4(new AnnotationData(*anData1));
    anData4->name = "aname4";
    const QString groupName1 = "subgroup1";
    const QString groupName2 = "subgroup1/subgroup2";
    const QString groupName3 = "subgroup3";

    AnnotationTableObject ft("aname_table", dbiRef);
    ft.addAnnotations(QList<SharedAnnotationData>() << anData1, groupName1);
    ft.addAnnotations(QList<SharedAnnotationData>() << anData2 << anData3, groupName2);
    ft.addAnnotations(QList<SharedAnnotationData>() << anData4, groupName3);

    AnnotationGroup *rootGroup = ft.getRootGroup();
    const QList<AnnotationGroup *> subgroups = rootGroup->getSubgroups();
    CHECK_EQUAL(2, subgroups.size(), "Count of subgroups");

    AnnotationGroup *subgroup1 = rootGroup;
    AnnotationGroup *subgroup3 = rootGroup;
    if (subgroups.first()->getName() == groupName1) {
        subgroup1 = subgroups.first();
        subgroup3 = subgroups.last();
    } else {
        subgroup1 = subgroups.last();
        subgroup3 = subgroups.first();
    }

    QList<Annotation *> annotations = subgroup1->getAnnotations();
    CHECK_EQUAL(1, annotations.size(), "Count of annotations");

    Annotation *singleAnnotation = annotations.first();
    QList<Annotation *> set;
    set << singleAnnotation;

    subgroup1->findAllAnnotationsInGroupSubTree(set);
    CHECK_EQUAL(3, set.size(), "Count of annotations");

    QBitArray annotationMatch(3, false);
    foreach (Annotation *annotation, set) {
        if ("aname" == annotation->getName()) {
            annotationMatch.setBit(0, true);
        } else if ("aname2" == annotation->getName()) {
            annotationMatch.setBit(1, true);
        } else if ("aname3" == annotation->getName()) {
            annotationMatch.setBit(2, true);
        }
    }
    CHECK_EQUAL(3, annotationMatch.count(true), "Count of annotations");

    subgroup3->findAllAnnotationsInGroupSubTree(set);
    CHECK_EQUAL(4, set.size(), "Count of annotations");
}

IMPLEMENT_TEST(AnnotationGroupUnitTest, getSet_Subgroups) {
    const U2DbiRef dbiRef(getDbiRef());
    SharedAnnotationData anData1 = createTestAnnotationData();
    SharedAnnotationData anData2(new AnnotationData(*anData1));
    anData2->name = "aname2";
    SharedAnnotationData anData3(new AnnotationData(*anData1));
    anData3->name = "aname3";
    SharedAnnotationData anData4(new AnnotationData(*anData1));
    anData4->name = "aname4";
    const QString groupName1 = "subgroup1";
    const QString groupName2 = "subgroup1/subgroup2";
    const QString groupName3 = "subgroup3";

    AnnotationTableObject ft("aname_table", dbiRef);
    ft.addAnnotations(QList<SharedAnnotationData>() << anData1, groupName1);
    ft.addAnnotations(QList<SharedAnnotationData>() << anData2 << anData3, groupName2);
    ft.addAnnotations(QList<SharedAnnotationData>() << anData4, groupName3);

    AnnotationGroup *rootGroup = ft.getRootGroup();
    const QList<AnnotationGroup *> subgroups = rootGroup->getSubgroups();
    CHECK_EQUAL(2, subgroups.size(), "Count of subgroups");

    AnnotationGroup *subgroup1 = rootGroup;
    AnnotationGroup *subgroup3 = rootGroup;
    if (subgroups.first()->getName() == groupName1) {
        subgroup1 = subgroups.first();
        subgroup3 = subgroups.last();
    } else {
        subgroup1 = subgroups.last();
        subgroup3 = subgroups.first();
    }

    QList<AnnotationGroup *> subSubgroups = subgroup1->getSubgroups();
    CHECK_EQUAL(1, subSubgroups.size(), "Count of subgroups");
    CHECK_TRUE(subgroup1->getParentGroup() == rootGroup, "Unexpected parent group");

    AnnotationGroup *removedSubgroup = subSubgroups.first();
    CHECK_TRUE(removedSubgroup->getParentGroup() == subgroup1, "Unexpected parent group");
    subgroup1->removeSubgroup(subSubgroups.first());
    subSubgroups = subgroup1->getSubgroups();
    CHECK_TRUE(subSubgroups.isEmpty(), "Unexpected subgroups");

    QList<AnnotationGroup *> subSubgroups3 = subgroup3->getSubgroups();
    CHECK_TRUE(subgroup1->getParentGroup() == rootGroup, "Unexpected parent group");
    CHECK_TRUE(subSubgroups3.isEmpty(), "Unexpected subgroups");

    AnnotationGroup *nonExistentGroup = subgroup3->getSubgroup("123/456", false);
    CHECK_TRUE(nonExistentGroup == NULL, "Unexpected subgroup ID");
    subSubgroups3 = subgroup3->getSubgroups();
    CHECK_TRUE(subSubgroups3.isEmpty(), "Unexpected subgroups");

    AnnotationGroup *newSubGroup = subgroup3->getSubgroup("123/456", true);
    CHECK_EQUAL("456", newSubGroup->getName(), "Subgroup's name");
    CHECK_FALSE(newSubGroup->getParentGroup() == subgroup3, "Unexpected parent group");
    subSubgroups3 = subgroup3->getSubgroups();
    CHECK_EQUAL(1, subSubgroups3.size(), "Count of subgroups");
}

} // namespace U2
