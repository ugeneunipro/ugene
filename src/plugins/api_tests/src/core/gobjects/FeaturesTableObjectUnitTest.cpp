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
#include <QDir>
#include <QScopedPointer>

#include <U2Core/AnnotationTableObjectConstraints.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>

#include <U2Test/TestRunnerSettings.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2FeatureDbi.h>
#include <U2Core/U2FeatureUtils.h>
#include <U2Core/U2FeatureKeys.h>

#include "FeaturesTableObjectUnitTest.h"

namespace U2 {

template<>
QString toString<U2FeatureLocation>(const U2FeatureLocation &loc) {
    QString strand = loc.strand == U2Strand::Direct ? "direct" :
                     loc.strand == U2Strand::Complementary ? "complement" :
                                                             "nostrand";
    return QString("%1-%2").arg(loc.region.toString()).arg(strand);
}

namespace {
    const QString FEATURE_DB_URL("feature-dbi.ugenedb");
}

U2FeatureDbi * FeaturesTableObjectTestData::featureDbi = NULL;
TestDbiProvider FeaturesTableObjectTestData::dbiProvider;

void FeaturesTableObjectTestData::init() {
    bool ok = dbiProvider.init(FEATURE_DB_URL, true);
    SAFE_POINT(ok, "dbi provider failed to initialize",);

    featureDbi = dbiProvider.getDbi()->getFeatureDbi();
    SAFE_POINT(NULL != featureDbi, "feature database not loaded",);
}

U2FeatureDbi *FeaturesTableObjectTestData::getFeatureDbi() {
    if (NULL == featureDbi) {
        init();
    }
    return featureDbi;
}

static U2DbiRef getDbiRef() {
    return FeaturesTableObjectTestData::getFeatureDbi()->getRootDbi()->getDbiRef();
}

void FeaturesTableObjectTestData::shutdown() {
    if (NULL != featureDbi) {
        U2OpStatusImpl opStatus;
        dbiProvider.close();
        featureDbi = NULL;
        SAFE_POINT_OP(opStatus,);
    }
}

IMPLEMENT_TEST(FeatureTableObjectUnitTest, createEmptyFeaturesTableObject) {
    U2FeatureDbi *featureDbi = FeaturesTableObjectTestData::getFeatureDbi();

    AnnotationTableObject ft("empty", getDbiRef());
    const U2DataId &objRootFeatureId = ft.getRootFeatureId();
    CHECK_TRUE(!objRootFeatureId.isEmpty(), "invalid root feature id");

    U2OpStatusImpl os;
    const U2Feature &actual = featureDbi->getFeature(objRootFeatureId, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(objRootFeatureId, actual.id, "root feature id");
}

IMPLEMENT_TEST(FeatureTableObjectUnitTest, addAnnotationSingleRegion) {
    U2FeatureDbi *featureDbi = FeaturesTableObjectTestData::getFeatureDbi();

    const QString aname = "aname_single";
    const QString grname = "agroupename_single";
    const QString qualname = "aqualname_single";
    const QString qualval = "aqualvalue_single";
    const U2Region areg(7, 2000);
    const U2DbiRef dbiRef(getDbiRef());

    SharedAnnotationData anData(new AnnotationData);
    anData->location->regions.append(areg);
    anData->name = aname;
    anData->qualifiers.append(U2Qualifier(qualname, qualval));

    AnnotationTableObject ft("aname_table_single", dbiRef);
    ft.addAnnotations(QList<SharedAnnotationData>() << anData, grname);

    const U2DataId &objRootFeatureId = ft.getRootFeatureId();
    CHECK_TRUE(!objRootFeatureId.isEmpty(), "invalid root feature id");

    U2OpStatusImpl os;
    const QList<U2Feature> annSubfeatures = U2FeatureUtils::getSubAnnotations(objRootFeatureId, dbiRef, os, Recursive, Nonroot);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(1, annSubfeatures.size(), "annotation subfeatures of root feature");

    const QList<U2Feature> groupSubfeatures = U2FeatureUtils::getSubGroups(objRootFeatureId, dbiRef, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(1, groupSubfeatures.size(), "group subfeatures of root feature");

    const U2Feature subAnnotation = annSubfeatures.first();
    const U2Feature subGroup = groupSubfeatures.first();

    CHECK_EQUAL(aname, subAnnotation.name, "feature name");
    CHECK_EQUAL(areg, subAnnotation.location.region, "feature region");
    CHECK_EQUAL(subGroup.id, subAnnotation.parentFeatureId, "annotation feature parent id");

    //check groups and qualifiers
    const QList<U2FeatureKey> fkeys = featureDbi->getFeatureKeys(subAnnotation.id, os);
    bool hasQual = false;
    foreach (const U2FeatureKey &fkey, fkeys) {
        if (fkey.name == qualname && !hasQual) {
            hasQual = fkey.value == qualval;
        }
    }
    CHECK_TRUE(hasQual, "qualifier not found in feature keys");
}

IMPLEMENT_TEST(FeatureTableObjectUnitTest, addAnnotationMultipleRegion) {
    U2FeatureDbi *featureDbi = FeaturesTableObjectTestData::getFeatureDbi();

    const QString fname = "aname_table_multy";
    const QString aname = "aname_multy";
    const QString grname = "agroupename_multy";
    const QString qualname = "aqualname_multy";
    const QString qualval = "aqualvalue_multy";
    const U2Region areg1(1, 2);
    const U2Region areg2(400, 10);
    const U2Region areg3(666, 666);
    const U2DbiRef dbiRef(getDbiRef());

    SharedAnnotationData anData(new AnnotationData);
    anData->location->regions << areg1 << areg2 << areg3;
    anData->name = aname;
    anData->qualifiers.append(U2Qualifier(qualname, qualval));

    AnnotationTableObject ft(fname, dbiRef);
    ft.addAnnotations(QList<SharedAnnotationData>() << anData, grname);

    const U2DataId &objRootFeatureId = ft.getRootFeatureId();
    CHECK_TRUE(!objRootFeatureId.isEmpty(), "invalid root feature id");

    U2OpStatusImpl os;
    const QList<U2Feature> annSubfeatures = U2FeatureUtils::getSubAnnotations(objRootFeatureId, dbiRef, os, Recursive, Root);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(1, annSubfeatures.size(), "annotating subfeatures of root feature");

    const QList<U2Feature> groupSubfeatures = U2FeatureUtils::getSubGroups(objRootFeatureId, dbiRef, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(1, groupSubfeatures.size(), "group subfeatures of root feature");

    const U2Feature subAnnotation = annSubfeatures.first();
    const U2Feature subGroup = groupSubfeatures.first();

    CHECK_EQUAL(aname, subAnnotation.name, "feature name");
    CHECK_EQUAL(U2Region(), subAnnotation.location.region, "feature region");
    CHECK_EQUAL(subGroup.id, subAnnotation.parentFeatureId, "feature parent id");

    //check groups and qualifiers
    const QList<U2FeatureKey> fkeys = featureDbi->getFeatureKeys(subAnnotation.id, os);
    bool hasQual = false;
    foreach (const U2FeatureKey &fkey, fkeys) {
        if (fkey.name == qualname && !hasQual) {
            hasQual = fkey.value == qualval;
        }
    }
    CHECK_TRUE(hasQual, "qualifier not found in feature keys");

    // test subfeatures
    const SharedAnnotationData processedData = U2FeatureUtils::getAnnotationDataFromFeature(subAnnotation.id, dbiRef, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(3, processedData->location->regions.size(), "subfeatures of multi-region feature");

    QBitArray regs(3, false);
    foreach (const U2Region &reg, processedData->location->regions) {
        if (reg == areg1) {
            regs.setBit(0, true);
        } else if(reg == areg2) {
            regs.setBit(1, true);
        } else if(reg == areg3) {
            regs.setBit(2, true);
        }
    }
    CHECK_EQUAL(3, regs.count(true), "matching regions");
}

IMPLEMENT_TEST(FeatureTableObjectUnitTest, getAnnotations) {
    const QString aname1 = "aname1";
    const QString aname2 = "aname2";
    const QString aname3 = "aname3";
    const QString grname = "agroupename_single";
    const U2Region areg(7, 2000);
    const U2DbiRef dbiRef(getDbiRef());

    SharedAnnotationData anData1(new AnnotationData);
    anData1->location->regions.append(areg);
    anData1->name = aname1;

    SharedAnnotationData anData2(new AnnotationData);
    anData2->location->regions.append(areg);
    anData2->name = aname2;

    SharedAnnotationData anData3(new AnnotationData);
    anData3->location->regions.append(areg);
    anData3->name = aname3;

    AnnotationTableObject ft("ftable_name", dbiRef);
    ft.addAnnotations(QList<SharedAnnotationData>() << anData1 << anData2 << anData3, grname);

    const QList<Annotation *> annotations = ft.getAnnotations();
    CHECK_EQUAL(3, annotations.size(), "annotation count");

    QBitArray annotationMatches(3, false);
    foreach (Annotation *annotation, annotations) {
        if (annotation->getName() == aname1) {
            annotationMatches.setBit(0, true);
        } else if(annotation->getName() == aname2) {
            annotationMatches.setBit(1, true);
        } else if(annotation->getName() == aname3) {
            annotationMatches.setBit(2, true);
        }
    }
    CHECK_EQUAL(3, annotationMatches.count(true), "matching annotations");
}

IMPLEMENT_TEST(FeatureTableObjectUnitTest, getRootGroup) {
    const QString aname = "aname_single";
    const QString grname = "agroupename_single";
    const U2Region areg(7, 2000);
    const U2DbiRef dbiRef(getDbiRef());

    SharedAnnotationData anData(new AnnotationData);
    anData->location->regions.append(areg);
    anData->name = aname;

    AnnotationTableObject ft("ftable_name", dbiRef);
    ft.addAnnotations(QList<SharedAnnotationData>() << anData, grname);

    AnnotationGroup *rootGroup = ft.getRootGroup();
    CHECK_TRUE(rootGroup->hasValidId(), "root group ID");

    const QList<AnnotationGroup *> subgroups = rootGroup->getSubgroups();
    CHECK_EQUAL(1, subgroups.size(), "count of annotation groups");
    CHECK_EQUAL(grname, subgroups.first()->getName(), "group's name");
}

IMPLEMENT_TEST(FeatureTableObjectUnitTest, addAnnotationsToRootGroup) {
    const QString aname1 = "aname1";
    const QString aname2 = "aname2";
    const U2Region areg1(7, 100);
    const U2Region areg2(1000, 200);
    const U2DbiRef dbiRef(getDbiRef());

    SharedAnnotationData anData1(new AnnotationData);
    anData1->location->regions << areg1 << areg2;
    anData1->name = aname1;

    SharedAnnotationData anData2(new AnnotationData);
    anData2->location->regions << areg1;
    anData2->name = aname2;

    SharedAnnotationData anData3(new AnnotationData);
    anData3->location->regions << areg2;
    anData3->name = aname2;

    QList<SharedAnnotationData> annotations;
    annotations << anData1 << anData2 << anData3;

    AnnotationTableObject ft("ftable_name", dbiRef);
    ft.addAnnotations(annotations);

    AnnotationGroup *rootGroup = ft.getRootGroup();
    CHECK_TRUE(rootGroup->hasValidId(), "root group ID");

    const QList<AnnotationGroup *> subgroups = rootGroup->getSubgroups();
    CHECK_EQUAL(2, subgroups.size(), "root annotation group has subgroups");

    QBitArray groupMatches(2, false);
    foreach (AnnotationGroup *group, subgroups) {
        if (group->getName() == aname1) {
            groupMatches.setBit(0, true);

            const QList<Annotation *> anns = group->getAnnotations();
            CHECK_EQUAL(1, anns.size(), "count of annotations");

            CHECK_EQUAL(2, anns.first()->getLocation()->regions.size(), "count of annotation regions");
        } else if (group->getName() == aname2) {
            groupMatches.setBit(1, true);

            const QList<Annotation *> anns = group->getAnnotations();
            CHECK_EQUAL(2, anns.size(), "count of annotations");
        }
    }
    CHECK_EQUAL(2, groupMatches.count(true), "matching groups");
}

IMPLEMENT_TEST(FeatureTableObjectUnitTest, addAnnotationsToSubgroup) {
    const QString aname1 = "aname1";
    const QString aname2 = "aname2";
    const QString grname = "subgroup1/subgroup2";
    const U2Region areg1(7, 100);
    const U2Region areg2(1000, 200);
    const U2DbiRef dbiRef(getDbiRef());

    SharedAnnotationData anData1(new AnnotationData);
    anData1->location->regions << areg1 << areg2;
    anData1->name = aname1;

    SharedAnnotationData anData2(new AnnotationData);
    anData2->location->regions << areg1;
    anData2->name = aname2;

    SharedAnnotationData anData3(new AnnotationData);
    anData3->location->regions << areg2;
    anData3->name = aname2;

    QList<SharedAnnotationData> annotations;
    annotations << anData1 << anData2 << anData3;

    U2OpStatusImpl os;
    AnnotationTableObject ft("ftable_name", dbiRef);
    ft.addAnnotations(annotations, grname);

    AnnotationGroup *rootGroup = ft.getRootGroup();
    CHECK_TRUE(rootGroup->hasValidId(), "root group ID");

    const QList<AnnotationGroup *> subgroup1 = rootGroup->getSubgroups();
    CHECK_EQUAL(1, subgroup1.size(), "root group's subgroups");

    const QList<Annotation *> anns1 = subgroup1.first()->getAnnotations();
    CHECK_EQUAL(0, anns1.size(), "annotation count");

    const QList<AnnotationGroup *> subgroup2 = subgroup1.first()->getSubgroups();
    CHECK_EQUAL(1, subgroup2.size(), "subgroup count");

    const QList<Annotation *> anns2 = subgroup2.first()->getAnnotations();
    CHECK_EQUAL(3, anns2.size(), "annotation count");
}

IMPLEMENT_TEST(FeatureTableObjectUnitTest, removeAnnotation) {
    const QString aname1 = "aname1";
    const QString aname2 = "aname2";
    const QString grname = "subgroup";
    const U2Region areg1(7, 100);
    const U2Region areg2(1000, 200);
    const U2DbiRef dbiRef(getDbiRef());

    SharedAnnotationData anData1(new AnnotationData);
    anData1->location->regions << areg1 << areg2;
    anData1->name = aname1;

    SharedAnnotationData anData2(new AnnotationData);
    anData2->location->regions << areg1;
    anData2->name = aname2;

    SharedAnnotationData anData3(new AnnotationData);
    anData3->location->regions << areg2;
    anData3->name = aname2;

    QList<SharedAnnotationData> annotations;
    annotations << anData1 << anData2 << anData3;

    AnnotationTableObject ft("ftable_name", dbiRef);
    U2OpStatusImpl os;
    ft.addAnnotations(annotations, grname);

    AnnotationGroup *rootGroup = ft.getRootGroup();
    CHECK_TRUE(rootGroup->hasValidId(), "root group ID");

    const QList<AnnotationGroup *> subgroup = rootGroup->getSubgroups();
    CHECK_EQUAL(1, subgroup.size(), "root group's subgroups");

    const QList<Annotation *> annsBefore = subgroup.first()->getAnnotations();
    CHECK_EQUAL(3, annsBefore.size(), "annotation count");

    QList<Annotation *> annsToRemove;
    foreach (Annotation *ann, annsBefore) {
        if (aname1 == ann->getName()) {
            annsToRemove.append(ann);
        }
    }
    subgroup.first()->removeAnnotations(annsToRemove);

    const QList<U2Feature> featuresAfter = U2FeatureUtils::getSubAnnotations(subgroup.first()->id, dbiRef, os, Recursive, Nonroot);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(2, featuresAfter.size(), "annotation count");
}

IMPLEMENT_TEST(FeatureTableObjectUnitTest, removeAnnotations) {
    const QString aname1 = "aname1";
    const QString aname2 = "aname2";
    const QString grname = "subgroup";
    const U2Region areg1(7, 100);
    const U2Region areg2(1000, 200);
    const U2DbiRef dbiRef(getDbiRef());

    SharedAnnotationData anData1(new AnnotationData);
    anData1->location->regions << areg1 << areg2;
    anData1->name = aname1;

    SharedAnnotationData anData2(new AnnotationData);
    anData2->location->regions << areg1;
    anData2->name = aname2;

    SharedAnnotationData anData3(new AnnotationData);
    anData3->location->regions << areg2;
    anData3->name = aname2;

    QList<SharedAnnotationData> annotations;
    annotations << anData1 << anData2 << anData3;

    AnnotationTableObject ft("ftable_name", dbiRef);
    ft.addAnnotations(annotations, grname);

    AnnotationGroup *rootGroup = ft.getRootGroup();
    CHECK_TRUE(rootGroup->hasValidId(), "root group ID");

    const QList<AnnotationGroup *> subgroup = rootGroup->getSubgroups();
    CHECK_EQUAL(1, subgroup.size(), "root group's subgroups");

    const QList<Annotation *> annsBefore = subgroup.first()->getAnnotations();
    CHECK_EQUAL(3, annsBefore.size(), "annotation count");

    ft.removeAnnotations(annsBefore);

    U2OpStatusImpl os;
    const QList<U2Feature> featuresAfter = U2FeatureUtils::getSubAnnotations(subgroup.first()->id, dbiRef, os, Recursive, Nonroot);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(0, featuresAfter.size(), "annotation count");
}

IMPLEMENT_TEST(FeatureTableObjectUnitTest, clone) {
    const QString aname1 = "aname1";
    const QString aname2 = "aname2";
    const QString aname3 = "aname3";
    const U2Region areg1(7, 100);
    const U2Region areg2(1000, 200);
    const U2DbiRef dbiRef(getDbiRef());

    SharedAnnotationData anData1(new AnnotationData);
    anData1->location->regions << areg1 << areg2;
    anData1->name = aname1;

    SharedAnnotationData anData2(new AnnotationData);
    anData2->location->regions << areg1;
    anData2->name = aname2;

    SharedAnnotationData anData3(new AnnotationData);
    anData3->location->regions << areg2;
    anData3->name = aname2;

    QList<SharedAnnotationData> annotations;
    annotations << anData1 << anData2 << anData3;

    AnnotationTableObject ft("ftable_name", dbiRef);
    ft.addAnnotations(annotations);

    AnnotationGroup *sourceRootGroup = ft.getRootGroup();

    U2OpStatusImpl os;
    QScopedPointer<AnnotationTableObject> clonedTable(dynamic_cast<AnnotationTableObject *>(ft.clone(dbiRef, os)));
    CHECK_NO_ERROR(os);

    AnnotationGroup *clonedRootGroup = clonedTable->getRootGroup();
    CHECK_TRUE(clonedRootGroup->hasValidId(), "cloned root group ID");
    CHECK_NOT_EQUAL(clonedRootGroup->id, sourceRootGroup->id, "root group");

    const QList<AnnotationGroup *> clonedSubgroups = clonedRootGroup->getSubgroups();
    QList<AnnotationGroup *> sourceSubgroups = sourceRootGroup->getSubgroups();
    CHECK_EQUAL(sourceSubgroups.size(), clonedSubgroups.size(), "root group's subgroups");

    foreach (AnnotationGroup *clonedSubgroup, clonedSubgroups) {
        const QList<Annotation *> clonedAnns = clonedSubgroup->getAnnotations();

        bool groupMatched = false;
        foreach (AnnotationGroup *sourceSubgroup, sourceSubgroups) {
            if (sourceSubgroup->getName() == clonedSubgroup->getName()) {
                groupMatched = true;
                const QList<Annotation *> sourceAnns = sourceSubgroup->getAnnotations();
                CHECK_EQUAL(sourceAnns.size(), clonedAnns.size(), "annotation count");
                break;
            }
        }
        CHECK_TRUE(groupMatched, "cloned group not found");
    }
}

IMPLEMENT_TEST(FeatureTableObjectUnitTest, getAnnotationsByName) {
    const QString aname1 = "aname1";
    const QString aname2 = "aname2";
    const QString grname = "subgroup";
    const U2Region areg1(7, 100);
    const U2Region areg2(1000, 200);
    const U2DbiRef dbiRef(getDbiRef());

    SharedAnnotationData anData1(new AnnotationData);
    anData1->location->regions << areg1 << areg2;
    anData1->name = aname1;

    SharedAnnotationData anData2(new AnnotationData);
    anData2->location->regions << areg1;
    anData2->name = aname2;

    SharedAnnotationData anData3(new AnnotationData);
    anData3->location->regions << areg2;
    anData3->name = aname2;

    QList<SharedAnnotationData> annotations;
    annotations << anData1 << anData2 << anData3;

    AnnotationTableObject ft("ftable_name", dbiRef);
    ft.addAnnotations(annotations);

    const QList<Annotation *> anns1 = ft.getAnnotationsByName(aname2);
    CHECK_EQUAL(2, anns1.size(), "annotation count");

    ft.addAnnotations(annotations, grname);

    const QList<Annotation *> anns2 = ft.getAnnotationsByName(aname2);
    CHECK_EQUAL(4, anns2.size(), "annotation count");
}

IMPLEMENT_TEST(FeatureTableObjectUnitTest, getAnnotationsByRegion) {
    const QString aname1 = "aname1";
    const QString aname2 = "aname2";
    const U2Region areg1(7, 100);
    const U2Region areg2(1000, 200);
    const U2DbiRef dbiRef(getDbiRef());

    SharedAnnotationData anData1(new AnnotationData);
    anData1->location->regions << areg1 << areg2;
    anData1->name = aname1;

    SharedAnnotationData anData2(new AnnotationData);
    anData2->location->regions << areg1;
    anData2->name = aname2;

    SharedAnnotationData anData3(new AnnotationData);
    anData3->location->regions << areg2;
    anData3->name = aname2;

    QList<SharedAnnotationData> annotations;
    annotations << anData1 << anData2 << anData3;

    AnnotationTableObject ft("ftable_name", dbiRef);
    ft.addAnnotations(annotations);

    const QList<Annotation *> anns1 = ft.getAnnotationsByRegion(U2Region(500, 500), false);
    CHECK_EQUAL(0, anns1.size(), "annotation count");

    const QList<Annotation *> anns2 = ft.getAnnotationsByRegion(U2Region(500, 500), true);
    CHECK_EQUAL(0, anns2.size(), "annotation count");

    const QList<Annotation *> anns3 = ft.getAnnotationsByRegion(U2Region(0, 500), false);
    CHECK_EQUAL(2, anns3.size(), "annotation count");
    foreach (Annotation *ann, anns3) {
        CHECK_TRUE(ann->getRegions().contains(areg1), "count of annotation regions");
    }
}

IMPLEMENT_TEST(FeatureTableObjectUnitTest, checkConstraints) {
    const QString aname1 = "aname1";
    const QString aname2 = "aname2";
    const U2Region areg1(7, 100);
    const U2Region areg2(1000, 200);
    const U2DbiRef dbiRef(getDbiRef());

    SharedAnnotationData anData1(new AnnotationData);
    anData1->location->regions << areg1 << areg2;
    anData1->name = aname1;

    SharedAnnotationData anData2(new AnnotationData);
    anData2->location->regions << areg1;
    anData2->name = aname2;

    SharedAnnotationData anData3(new AnnotationData);
    anData3->location->regions << areg2;
    anData3->name = aname2;

    QList<SharedAnnotationData> annotations;
    annotations << anData1 << anData2 << anData3;

    AnnotationTableObject ft("ftable_name", dbiRef);
    ft.addAnnotations(annotations);

    AnnotationTableObjectConstraints constraints;

    constraints.sequenceSizeToFit = 1000;
    CHECK_FALSE(ft.checkConstraints(&constraints), "unexpected constraint test result");

    constraints.sequenceSizeToFit = 100;
    CHECK_FALSE(ft.checkConstraints(&constraints), "unexpected constraint test result");

    constraints.sequenceSizeToFit = 2000;
    CHECK_TRUE(ft.checkConstraints(&constraints), "unexpected constraint test result");
}

} // namespace
