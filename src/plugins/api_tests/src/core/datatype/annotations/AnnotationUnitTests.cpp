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

#include <U2Core/U2FeatureUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2OpStatusUtils.h>

#include "../../gobjects/FeaturesTableObjectUnitTest.h"
#include "AnnotationUnitTests.h"

#define CHECK_REGIONS_MATCH(first, second) \
CHECK_TRUE(first.size() == second.size(), "Invalid annotation's region count"); \
foreach (const U2Region &region, first) { \
    CHECK_TRUE(second.contains(region), "Invalid annotation's region"); \
}

namespace U2 {

static U2DbiRef getDbiRef() {
    return FeaturesTableObjectTestData::getFeatureDbi()->getRootDbi()->getDbiRef();
}

static SharedAnnotationData createTestAnnotationData() {
    const U2Region areg1(1, 2);
    const U2Region areg2(400, 10);
    const U2Region areg3(666, 666);

    SharedAnnotationData anData(new AnnotationData);
    anData->location->regions << areg1 << areg2 << areg3;
    anData->name = "aname_multy";

    anData->qualifiers.append(U2Qualifier("aqualname_multy", "aqualvalue_multy"));
    anData->qualifiers.append(U2Qualifier("1", "A"));
    anData->qualifiers.append(U2Qualifier("2", "C"));
    anData->qualifiers.append(U2Qualifier("key", "value1"));
    anData->qualifiers.append(U2Qualifier("key", "value2"));
    anData->qualifiers.append(U2Qualifier("key", "value3"));
    anData->qualifiers.append(U2Qualifier("2", "B"));

    return anData;
}

IMPLEMENT_TEST(AnnotationUnitTest, get_IdObjectData) {
    const U2DbiRef dbiRef(getDbiRef());
    SharedAnnotationData anData = createTestAnnotationData();

    AnnotationTableObject ft("aname_table_multy", dbiRef);
    ft.addAnnotations(QList<SharedAnnotationData>() << anData);

    const QList<Annotation *> annotations = ft.getAnnotations();
    CHECK_EQUAL(1, annotations.size(), "count of annotations");
    Annotation *annotation = annotations.first();

    CHECK_TRUE(&ft == annotation->getGObject(), "Unexpected value of annotation's parent object");

    U2OpStatusImpl os;
    const U2Feature feature = U2FeatureUtils::getFeatureById(annotation->id, dbiRef, os);
    CHECK_EQUAL(U2Region(), feature.location.region, "Annotation's region");
    CHECK_TRUE(feature.location.strand.isDirect(), "Annotation has to belong to direct strand");

    CHECK_TRUE(*anData == *(annotation->getData()), "Unexpected value of annotation's data");
}

IMPLEMENT_TEST(AnnotationUnitTest, getSet_Name) {
    const U2DbiRef dbiRef(getDbiRef());
    SharedAnnotationData anData = createTestAnnotationData();

    AnnotationTableObject ft("aname_table_multy", dbiRef);
    ft.addAnnotations(QList<SharedAnnotationData>() << anData);

    QList<Annotation *> annotations = ft.getAnnotations();
    CHECK_EQUAL(1, annotations.size(), "count of annotations");
    Annotation *annotation = annotations.first();

    CHECK_EQUAL(annotation->getName(), anData->name, "Annotation's name");

    const QString newName = "newName";
    annotation->setName(newName);

    CHECK_EQUAL(annotation->getName(), newName, "Annotation's name");
}

IMPLEMENT_TEST(AnnotationUnitTest, getSet_LocationOperator) {
    const U2DbiRef dbiRef(getDbiRef());
    SharedAnnotationData anData = createTestAnnotationData();

    AnnotationTableObject ft("aname_table_multy", dbiRef);
    ft.addAnnotations(QList<SharedAnnotationData>() << anData);

    QList<Annotation *> annotations = ft.getAnnotations();
    CHECK_EQUAL(1, annotations.size(), "count of annotations");
    Annotation *annotation = annotations.first();

    CHECK_TRUE(annotation->isJoin(), "Unexpected location op");
    CHECK_EQUAL(annotation->getLocationOperator(), anData->getLocationOperator(), "Annotation's location op");

    const U2LocationOperator newOp = U2LocationOperator_Order;
    annotation->setLocationOperator(newOp);

    CHECK_EQUAL(newOp, annotation->getLocationOperator(), "Annotation's location op");
    CHECK_TRUE(annotation->isOrder(), "Unexpected location op");
}

IMPLEMENT_TEST(AnnotationUnitTest, getSet_Strand) {
    const U2DbiRef dbiRef(getDbiRef());
    SharedAnnotationData anData = createTestAnnotationData();

    AnnotationTableObject ft("aname_table_multy", dbiRef);
    ft.addAnnotations(QList<SharedAnnotationData>() << anData);

    QList<Annotation *> annotations = ft.getAnnotations();
    CHECK_EQUAL(1, annotations.size(), "count of annotations");
    Annotation *annotation = annotations.first();

    CHECK_TRUE(annotation->getStrand().isDirect(), "Unexpected annotation's direction");
    CHECK_EQUAL(annotation->getStrand().getDirectionValue(), anData->getStrand().getDirectionValue(), "Annotation's direction");

    const U2Strand newStrand(U2Strand::Complementary);
    annotation->setStrand(newStrand);

    CHECK_EQUAL(newStrand.getDirectionValue(), annotation->getStrand().getDirectionValue(), "Annotation's location op");
    CHECK_TRUE(annotation->getStrand().isCompementary(), "Unexpected location op");
}

IMPLEMENT_TEST(AnnotationUnitTest, getSet_Location) {
    const U2DbiRef dbiRef(getDbiRef());
    SharedAnnotationData anData = createTestAnnotationData();

    AnnotationTableObject ft("aname_table_multy", dbiRef);
    ft.addAnnotations(QList<SharedAnnotationData>() << anData);

    QList<Annotation *> annotations = ft.getAnnotations();
    CHECK_EQUAL(1, annotations.size(), "count of annotations");
    Annotation *annotation = annotations.first();

    CHECK_TRUE(*(anData->location) == *(annotation->getLocation()), "Unexpected annotation's location");

    U2Location newLocation(anData->location);
    newLocation->regions.remove(0);
    newLocation->regions << U2Region(23, 56);
    newLocation->op = U2LocationOperator_Order;
    newLocation->strand = U2Strand::Complementary;
    annotation->setLocation(newLocation);

    const U2Location resultLocation = annotation->getLocation();

    CHECK_REGIONS_MATCH(newLocation->regions, resultLocation->regions);

    CHECK_TRUE(resultLocation->op == newLocation->op, "Illegal annotation's location operator!");
    CHECK_TRUE(resultLocation->regionType == newLocation->regionType, "Illegal annotation's region type!");
    CHECK_TRUE(resultLocation->strand == newLocation->strand, "Illegal annotation's strand!");
}

IMPLEMENT_TEST(AnnotationUnitTest, getSet_Regions) {
    const U2DbiRef dbiRef(getDbiRef());
    SharedAnnotationData anData = createTestAnnotationData();

    AnnotationTableObject ft("aname_table_multy", dbiRef);
    ft.addAnnotations(QList<SharedAnnotationData>() << anData);

    QList<Annotation *> annotations = ft.getAnnotations();
    CHECK_EQUAL(1, annotations.size(), "count of annotations");
    Annotation *annotation = annotations.first();

    const QVector<U2Region> initRegions = anData->getRegions();

    CHECK_REGIONS_MATCH(annotation->getRegions(), initRegions);

    const U2Region supplementedRegion(45, 434);
    annotation->addLocationRegion(supplementedRegion);

    CHECK_REGIONS_MATCH(annotation->getRegions(), (QVector<U2Region>(initRegions) << supplementedRegion));

    QVector<U2Region> newRegions(initRegions);
    newRegions.remove(0, 2);
    newRegions << U2Region(45, 543) << U2Region(434, 432);
    annotation->updateRegions(newRegions);

    CHECK_REGIONS_MATCH(annotation->getRegions(), newRegions);
}

IMPLEMENT_TEST(AnnotationUnitTest, getSet_Qualifiers) {
    const U2DbiRef dbiRef(getDbiRef());
    SharedAnnotationData anData = createTestAnnotationData();

    AnnotationTableObject ft("aname_table_multy", dbiRef);
    ft.addAnnotations(QList<SharedAnnotationData>() << anData);

    QList<Annotation *> annotations = ft.getAnnotations();
    CHECK_EQUAL(1, annotations.size(), "count of annotations");
    Annotation *annotation = annotations.first();

    const QVector<U2Qualifier> initQuals = anData->qualifiers;

    CHECK_TRUE(annotation->getQualifiers() == initQuals, "Unexpected annotation's qualifiers");

    U2Qualifier supplementedQual("New_Key", "New Value");
    annotation->addQualifier(supplementedQual);

    CHECK_TRUE(annotation->getQualifiers() == (QVector<U2Qualifier>(initQuals) << supplementedQual), "Unexpected annotation's qualifiers");

    QVector<U2Qualifier> changedQuals(annotation->getQualifiers());
    U2Qualifier removedQual = changedQuals[1];
    changedQuals.remove(1);

    annotation->removeQualifier(removedQual);
    CHECK_TRUE(annotation->getQualifiers() == changedQuals, "Unexpected annotation's qualifiers");
}

IMPLEMENT_TEST(AnnotationUnitTest, find_Qualifiers) {
    const U2DbiRef dbiRef(getDbiRef());
    SharedAnnotationData anData = createTestAnnotationData();

    AnnotationTableObject ft("aname_table_multy", dbiRef);
    ft.addAnnotations(QList<SharedAnnotationData>() << anData);

    QList<Annotation *> annotations = ft.getAnnotations();
    CHECK_EQUAL(1, annotations.size(), "count of annotations");
    Annotation *annotation = annotations.first();

    QList<U2Qualifier> qualifiers1;
    annotation->findQualifiers("key", qualifiers1);
    CHECK_EQUAL(qualifiers1.size(), 3, "Qualifier count");

    QList<U2Qualifier> qualifiers2;
    annotation->findQualifiers("2", qualifiers2);
    CHECK_EQUAL(qualifiers2.size(), 2, "Qualifier count");

    QList<U2Qualifier> qualifiers3;
    annotation->findQualifiers("1", qualifiers3);
    CHECK_EQUAL(qualifiers3.size(), 1, "Qualifier count");

    QList<U2Qualifier> qualifiers4;
    annotation->findQualifiers("gene", qualifiers4);
    CHECK_TRUE(qualifiers4.isEmpty(), "Unexpected qualifier count");

    CHECK_TRUE(!annotation->findFirstQualifierValue("1").isEmpty(), "Unexpected qualifier value");
    CHECK_TRUE(!annotation->findFirstQualifierValue("2").isEmpty(), "Unexpected qualifier value");
    CHECK_TRUE(!annotation->findFirstQualifierValue("key").isEmpty(), "Unexpected qualifier value");
    CHECK_TRUE(annotation->findFirstQualifierValue("gene").isEmpty(), "Unexpected qualifier value");
}

IMPLEMENT_TEST(AnnotationUnitTest, get_Group) {
    const U2DbiRef dbiRef(getDbiRef());
    SharedAnnotationData anData = createTestAnnotationData();
    const QString firstGroupName = "group1/subgroup";
    const QString secondGroupName = "group2";

    AnnotationTableObject ft("aname_table_multy", dbiRef);
    ft.addAnnotations(QList<SharedAnnotationData>() << anData, firstGroupName);
    ft.addAnnotations(QList<SharedAnnotationData>() << anData, secondGroupName);

    QList<Annotation *> annotations = ft.getAnnotations();
    CHECK_EQUAL(2, annotations.size(), "count of annotations");

    QBitArray groupMatches(2, false);
    Annotation *ann1 = annotations.first();
    Annotation *ann2 = annotations.first();

    foreach (Annotation *ann, annotations) {
        if (ann->getGroup()->getName() == "subgroup") {
            groupMatches.setBit(0, true);
            ann2 = ann;
        } else if (ann->getGroup()->getName() == "group2") {
            groupMatches.setBit(1, true);
            ann1 = ann;
        }

        const QList<Annotation *> anns = ann->getGroup()->getAnnotations();
        CHECK_EQUAL(1, anns.size(), "count of annotations");
    }
    CHECK_EQUAL(2, groupMatches.count(true), "matching groups");

    CHECK_TRUE(Annotation::annotationLessThan(ann1, ann2), "Unexpected annotations comparison result");
}

IMPLEMENT_TEST(AnnotationUnitTest, getSet_Case) {
    const U2DbiRef dbiRef(getDbiRef());
    SharedAnnotationData anData = createTestAnnotationData();

    AnnotationTableObject ft("ftable", dbiRef);
    ft.addAnnotations(QList<SharedAnnotationData>() << anData);

    QList<Annotation *> annotations = ft.getAnnotations();
    CHECK_EQUAL(1, annotations.size(), "count of annotations");

    Annotation *annotation = annotations.first();
    CHECK_FALSE(annotation->isCaseAnnotation(), "Unexpected annotation case!");
    annotation->setCaseAnnotation(false);
    CHECK_FALSE(annotation->isCaseAnnotation(), "Unexpected annotation case!");
    annotation->setCaseAnnotation(true);
    CHECK_TRUE(annotation->isCaseAnnotation(), "Unexpected annotation case!");
    annotation->setCaseAnnotation(true);
    CHECK_TRUE(annotation->isCaseAnnotation(), "Unexpected annotation case!");
    annotation->setCaseAnnotation(false);
    CHECK_FALSE(annotation->isCaseAnnotation(), "Unexpected annotation case!");
}

} // namespace U2
