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

#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2FeatureUtils.h>
#include <U2Core/U2SequenceDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "FeatureDbiUnitTests.h"

namespace U2 {

static const U2Feature INVALID_FEATURE = U2Feature();

TestDbiProvider FeatureTestData::dbiProvider = TestDbiProvider();
TestDbiProvider FeatureTestData::subgroupsDbiProvider = TestDbiProvider();
const QString FeatureTestData::featureDbiUrl("features-dbi.ugenedb");
const QString FeatureTestData::subgroupDbiUrl("featureSubgroupsSorting.ugenedb");
U2FeatureDbi *FeatureTestData::featureDbi = NULL;
U2FeatureDbi *FeatureTestData::subgroupDbi = NULL;
U2SequenceDbi *FeatureTestData::sequenceDbi = NULL;

void FeatureTestData::init() {
    SAFE_POINT(NULL == featureDbi, "featuresDbi has been already initialized!",);

    bool ok = dbiProvider.init(featureDbiUrl, false);
    SAFE_POINT(ok, "Dbi provider failed to initialize in FeaturesTestData::init()!",);

    U2Dbi *dbi = dbiProvider.getDbi();
    featureDbi = dbi->getFeatureDbi();
    SAFE_POINT(NULL != featureDbi, "Failed to get featureDbi!",);

    sequenceDbi = dbi->getSequenceDbi();
    SAFE_POINT(NULL != sequenceDbi, "Failed to get sequenceDbi!",);

    SAFE_POINT(NULL == subgroupDbi, "subgroupDbi has been already initialized!", );

    ok = subgroupsDbiProvider.init(subgroupDbiUrl, false);
    SAFE_POINT(ok, "Dbi provider failed to initialize in FeaturesTestData::init()!", );

    dbi = subgroupsDbiProvider.getDbi();
    subgroupDbi = dbi->getFeatureDbi();
    SAFE_POINT(NULL != featureDbi, "Failed to get subgroupDbi!", );
}

void FeatureTestData::shutdown() {
    U2OpStatusImpl os;
    if (NULL != featureDbi) {
        SAFE_POINT(NULL != sequenceDbi, "sequenceDbi must also be not NULL on this step!",);

        dbiProvider.close();
        featureDbi = NULL;
        sequenceDbi = NULL;
        SAFE_POINT_OP(os,);
    }
    if (NULL != subgroupDbi) {
        SAFE_POINT(NULL != subgroupDbi, "subgroupDbi must also be not NULL on this step!", );

        subgroupsDbiProvider.close();
        subgroupDbi = NULL;
        SAFE_POINT_OP(os, );
    }
}

U2FeatureDbi *FeatureTestData::getFeatureDbi() {
    if (NULL == featureDbi) {
        init();
    }
    return featureDbi;
}

U2FeatureDbi *FeatureTestData::getSubgroupDbi() {
    if (NULL == subgroupDbi) {
        init();
    }
    return subgroupDbi;
}

U2SequenceDbi* FeatureTestData::getSequenceDbi() {
    if (NULL == sequenceDbi) {
        init();
    }
    return sequenceDbi;
}

U2Feature FeatureTestData::createTestFeature1(const U2Sequence &seq, U2OpStatus &os,
    const U2Feature &parentFeature)
{
    U2Feature feature;
    feature.sequenceId = seq.id;
    feature.location = U2FeatureLocation(U2Strand::Complementary, U2Region(1000, 100));
    feature.name = "misc_feature";
    QList<U2FeatureKey> keys;
    keys << U2FeatureKey("1", "A") << U2FeatureKey("2", "B") << U2FeatureKey("3", "C");
    featureDbi->createFeature(feature, keys, os);
    CHECK_OP(os, INVALID_FEATURE);
    CHECK_EXT(feature.hasValidId(), os.setError("Invalid feature ID!"), INVALID_FEATURE);
    if (parentFeature.hasValidId()) {
        featureDbi->updateParentId(feature.id, parentFeature.id, os);
        CHECK_OP(os, INVALID_FEATURE);
    }
    return feature;
}

U2Feature FeatureTestData::createTestFeature2(const U2Sequence &seq, U2OpStatus &os,
    const U2Feature &parentFeature)
{
    U2Feature feature;
    feature.sequenceId = seq.id;
    feature.location = U2FeatureLocation(U2Strand::Direct, U2Region(500, 600));
    feature.name = "other_feature";
    QList<U2FeatureKey> keys;
    keys << U2FeatureKey("qwe", "123");
    featureDbi->createFeature(feature, keys, os);
    CHECK_OP(os, INVALID_FEATURE);
    CHECK_EXT(feature.hasValidId(), os.setError("Invalid feature ID!"), INVALID_FEATURE);
    if (parentFeature.hasValidId()) {
        featureDbi->updateParentId(feature.id, parentFeature.id, os);
        CHECK_OP(os, INVALID_FEATURE);
    }
    return feature;
}

U2Feature FeatureTestData::createTestFeature3(const U2Sequence &seq, U2OpStatus &os,
    const U2Feature &parentFeature)
{
    U2Feature feature;
    feature.sequenceId = seq.id;
    feature.location = U2FeatureLocation(U2Strand::Direct, U2Region(800, 10));
    feature.name = "misc_feature";
    QList<U2FeatureKey> keys;
    keys << U2FeatureKey("qwe", "A") << U2FeatureKey("123", "asd");
    featureDbi->createFeature(feature, keys, os);
    CHECK_OP(os, INVALID_FEATURE);
    CHECK_EXT(feature.hasValidId(), os.setError("Invalid feature ID!"), INVALID_FEATURE);
    if (parentFeature.hasValidId()) {
        featureDbi->updateParentId(feature.id, parentFeature.id, os);
        CHECK_OP(os, INVALID_FEATURE);
    }
    return feature;
}

U2Feature FeatureTestData::createTestFeatureWithoutKeys(const U2Sequence &seq, U2OpStatus &os,
    const U2Feature &parentFeature)
{
    U2Feature feature;
    feature.sequenceId = seq.id;
    feature.location = U2FeatureLocation(U2Strand::Direct, U2Region(800, 10));
    feature.name = "misc_feature";
    featureDbi->createFeature(feature, QList<U2FeatureKey>(), os);
    CHECK_OP(os, INVALID_FEATURE);
    CHECK_EXT(feature.hasValidId(), os.setError("Invalid feature ID!"), INVALID_FEATURE);
    if (parentFeature.hasValidId()) {
        featureDbi->updateParentId(feature.id, parentFeature.id, os);
        CHECK_OP(os, INVALID_FEATURE);
    }
    return feature;
}

IMPLEMENT_TEST(FeatureDbiUnitTests, createFeature) {
    U2FeatureDbi *featureDbi = FeatureTestData::getFeatureDbi();
    U2SequenceDbi *sequenceDbi = FeatureTestData::getSequenceDbi();

    U2OpStatusImpl os;
    U2Sequence seq;
    sequenceDbi->createSequenceObject(seq, "", os);
    CHECK_NO_ERROR(os);

    U2Feature newFeature = FeatureTestData::createTestFeatureWithoutKeys(seq, os);
    const U2Feature featureBackup(newFeature);

    featureDbi->createFeature(newFeature, QList<U2FeatureKey>(), os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(newFeature.hasValidId(), "Invalid feature ID!");

    CHECK_EQUAL(featureBackup.sequenceId, newFeature.sequenceId, "sequence ID");
    CHECK_EQUAL(featureBackup.location.strand.getDirectionValue(),
        newFeature.location.strand.getDirectionValue(), "location.strand");
    CHECK_EQUAL(featureBackup.location.region.toString(),
        newFeature.location.region.toString(), "location.strand");
    CHECK_EQUAL(featureBackup.name, newFeature.name, "name");
}

IMPLEMENT_TEST(FeatureDbiUnitTests, getFeature) {
    U2FeatureDbi *featureDbi = FeatureTestData::getFeatureDbi();
    U2SequenceDbi *sequenceDbi = FeatureTestData::getSequenceDbi();

    U2OpStatusImpl os;
    U2Sequence seq;
    sequenceDbi->createSequenceObject(seq, "", os);
    CHECK_NO_ERROR(os);

    U2Feature newFeature = FeatureTestData::createTestFeatureWithoutKeys(seq, os);
    const U2Feature featureBackup(newFeature);

    featureDbi->createFeature(newFeature, QList<U2FeatureKey>(), os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(newFeature.hasValidId(), "Invalid feature ID!");

    newFeature = featureDbi->getFeature(newFeature.id, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(featureBackup.sequenceId, newFeature.sequenceId, "sequence ID");
    CHECK_EQUAL(featureBackup.location.strand.getDirectionValue(),
        newFeature.location.strand.getDirectionValue(), "location.strand");
    CHECK_EQUAL(featureBackup.location.region.toString(),
        newFeature.location.region.toString(), "location.strand");
    CHECK_EQUAL(featureBackup.name, newFeature.name, "name");
}

IMPLEMENT_TEST(FeatureDbiUnitTests, countFeatures) {
    U2FeatureDbi *featureDbi = FeatureTestData::getFeatureDbi();
    U2SequenceDbi *sequenceDbi = FeatureTestData::getSequenceDbi();

    U2OpStatusImpl os;
    U2Sequence seq1;
    sequenceDbi->createSequenceObject(seq1, "", os);
    CHECK_NO_ERROR(os);
    U2Sequence seq2;
    sequenceDbi->createSequenceObject(seq2, "", os);
    CHECK_NO_ERROR(os);

    U2Feature feature1 = FeatureTestData::createTestFeature1(seq1, os);
    CHECK_NO_ERROR(os);
    U2Feature feature2 = FeatureTestData::createTestFeature2(seq2, os);
    CHECK_NO_ERROR(os);
    U2Feature feature3 = FeatureTestData::createTestFeature3(seq2, os, feature2);
    CHECK_NO_ERROR(os);

    FeatureQuery query;
    query.featureName = "misc_feature";
    query.intersectRegion = U2Region(700, 1500);
    query.sequenceId = seq2.id;
    query.topLevelOnly = true;
    int queryResult = featureDbi->countFeatures(query, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(0, queryResult, "first query count");

    query.featureName = "other_feature";
    query.strandQuery = Strand_Direct;
    query.intersectRegion = U2Region(-1, 0);
    query.sequenceId = seq2.id;
    query.topLevelOnly = false;
    queryResult = featureDbi->countFeatures(query, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(1, queryResult, "second query count");

    query.featureName = "";
    query.sequenceId = seq2.id;
    query.strandQuery = Strand_Both;
    queryResult = featureDbi->countFeatures(query, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(2, queryResult, "third query count");

    query.sequenceId = seq2.id;
    query.keyName = "qwe";
    queryResult = featureDbi->countFeatures(query, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(2, queryResult, "fourth query count");

    query.keyName = "1";
    query.keyValue = "A";
    query.sequenceId = seq1.id;
    query.keyValueCompareOp = ComparisonOp_EQ;
    queryResult = featureDbi->countFeatures(query, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(1, queryResult, "fifth query count");

    query.keyValue = "";
    query.keyName = "";
    query.sequenceId = seq2.id;
    query.intersectRegion = U2Region(800, 100);
    query.closestFeature = ComparisonOp_EQ;
    queryResult = featureDbi->countFeatures(query, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(1, queryResult, "sixth query count");
}

IMPLEMENT_TEST(FeatureDbiUnitTests, getFeatures) {
    U2FeatureDbi *featureDbi = FeatureTestData::getFeatureDbi();
    U2SequenceDbi *sequenceDbi = FeatureTestData::getSequenceDbi();

    U2OpStatusImpl os;
    U2Sequence seq1;
    sequenceDbi->createSequenceObject(seq1, "", os);
    CHECK_NO_ERROR(os);
    U2Sequence seq2;
    sequenceDbi->createSequenceObject(seq2, "", os);
    CHECK_NO_ERROR(os);

    U2Feature feature1 = FeatureTestData::createTestFeature1(seq1, os);
    CHECK_NO_ERROR(os);
    U2Feature feature2 = FeatureTestData::createTestFeature2(seq2, os);
    CHECK_NO_ERROR(os);
    U2Feature feature3 = FeatureTestData::createTestFeature3(seq2, os, feature2);
    CHECK_NO_ERROR(os);

    FeatureQuery query;
    query.featureName = "misc_feature";
    query.intersectRegion = U2Region(700, 1500);
    query.sequenceId = seq2.id;
    query.topLevelOnly = true;
    U2DbiIterator<U2Feature> *iter = featureDbi->getFeatures(query, os);
    CHECK_NO_ERROR(os);
    CHECK_EQUAL(false, iter->hasNext(), "first feature query");

    query.featureName = "other_feature";
    query.strandQuery = Strand_Direct;
    query.intersectRegion = U2Region(-1, 0);
    query.sequenceId = seq2.id;
    query.topLevelOnly = false;
    iter = featureDbi->getFeatures(query, os);
    CHECK_NO_ERROR(os);
    while (iter->hasNext()) {
        CHECK_EQUAL(feature2.id, iter->next().id, "second feature query");
    }

    query.featureName = "";
    query.sequenceId = seq2.id;
    query.strandQuery = Strand_Both;
    iter = featureDbi->getFeatures(query, os);
    CHECK_NO_ERROR(os);
    while (iter->hasNext()) {
        const U2Feature current = iter->next();
        CHECK_TRUE((feature2.id == current.id) || (feature3.id == current.id),
            "third feature query");
    }

    query.sequenceId = seq2.id;
    query.keyName = "qwe";
    iter = featureDbi->getFeatures(query, os);
    CHECK_NO_ERROR(os);
    while (iter->hasNext()) {
        const U2Feature current = iter->next();
        CHECK_TRUE((feature2.id == current.id) || (feature3.id == current.id),
            "fourth feature query");
    }

    query.keyName = "1";
    query.keyValue = "A";
    query.sequenceId = seq1.id;
    query.keyValueCompareOp = ComparisonOp_EQ;
    iter = featureDbi->getFeatures(query, os);
    CHECK_NO_ERROR(os);
    while (iter->hasNext()) {
        CHECK_EQUAL(feature1.id, iter->next().id, "fifth feature query");
    }

    query.keyValue = "";
    query.keyName = "";
    query.sequenceId = seq2.id;
    query.intersectRegion = U2Region(800, 100);
    query.closestFeature = ComparisonOp_EQ;
    iter = featureDbi->getFeatures(query, os);
    CHECK_NO_ERROR(os);
    while (iter->hasNext()) {
        CHECK_EQUAL(feature3.id, iter->next().id, "sixth feature query");
    }
}

IMPLEMENT_TEST(FeatureDbiUnitTests, getFeatureKeys) {
    U2FeatureDbi *featureDbi = FeatureTestData::getFeatureDbi();
    U2SequenceDbi *sequenceDbi = FeatureTestData::getSequenceDbi();

    U2OpStatusImpl os;
    U2Sequence seq;
    sequenceDbi->createSequenceObject(seq, "", os);
    CHECK_NO_ERROR(os);

    U2Feature feature1 = FeatureTestData::createTestFeature1(seq, os);
    CHECK_NO_ERROR(os);

    QList<U2FeatureKey> keys = featureDbi->getFeatureKeys(feature1.id, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(3, keys.size(), "first feature key count");

    CHECK_EQUAL(keys.at(0).name, "1", "first feature key's name");
    CHECK_EQUAL(keys.at(0).value, "A", "first feature key's value");

    CHECK_EQUAL(keys.at(1).name, "2", "second feature key's name");
    CHECK_EQUAL(keys.at(1).value, "B", "second feature key's value");

    CHECK_EQUAL(keys.at(2).name, "3", "third feature key's name");
    CHECK_EQUAL(keys.at(2).value, "C", "third feature key's value");

    U2Feature feature2 = FeatureTestData::createTestFeatureWithoutKeys(seq, os);

    featureDbi->createFeature(feature2, QList<U2FeatureKey>(), os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(feature2.hasValidId(), "Invalid feature ID!");
    CHECK_EQUAL(0, featureDbi->getFeatureKeys(feature2.id, os).size(),
        "feature key count");
}

IMPLEMENT_TEST(FeatureDbiUnitTests, addKey) {
    U2FeatureDbi *featureDbi = FeatureTestData::getFeatureDbi();
    U2SequenceDbi *sequenceDbi = FeatureTestData::getSequenceDbi();

    U2OpStatusImpl os;
    U2Sequence seq;
    sequenceDbi->createSequenceObject(seq, "", os);
    CHECK_NO_ERROR(os);

    U2Feature feature = FeatureTestData::createTestFeature3(seq, os);
    CHECK_NO_ERROR(os);
    featureDbi->addKey(feature.id, U2FeatureKey("name", "value"), os);
    CHECK_NO_ERROR(os);
    QList<U2FeatureKey> keys = featureDbi->getFeatureKeys(feature.id, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(3, keys.size(), "first feature key count");

    CHECK_EQUAL(keys.at(0).name, "qwe", "first feature key's name");
    CHECK_EQUAL(keys.at(0).value, "A", "first feature key's value");

    CHECK_EQUAL(keys.at(1).name, "123", "second feature key's name");
    CHECK_EQUAL(keys.at(1).value, "asd", "second feature key's value");

    CHECK_EQUAL(keys.at(2).name, "name", "third feature key's name");
    CHECK_EQUAL(keys.at(2).value, "value", "third feature key's value");
}

IMPLEMENT_TEST(FeatureDbiUnitTests, removeAllKeysByName) {
    U2FeatureDbi *featureDbi = FeatureTestData::getFeatureDbi();
    U2SequenceDbi *sequenceDbi = FeatureTestData::getSequenceDbi();

    U2OpStatusImpl os;
    U2Sequence seq;
    sequenceDbi->createSequenceObject(seq, "", os);
    CHECK_NO_ERROR(os);

    U2Feature feature1 = FeatureTestData::createTestFeature3(seq, os);
    CHECK_NO_ERROR(os);
    // add key with existing name
    featureDbi->addKey(feature1.id, U2FeatureKey("qwe", "lalka"), os);
    CHECK_NO_ERROR(os);
    // remove all the keys with this name
    featureDbi->removeAllKeys(feature1.id, "qwe", os);
    CHECK_NO_ERROR(os);
    // the feature is supposed to have one key
    QList<U2FeatureKey> keys = featureDbi->getFeatureKeys(feature1.id, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(1, keys.size(), "first feature key count");
    CHECK_EQUAL(keys.at(0).name, "123", "first feature key's name");
    CHECK_EQUAL(keys.at(0).value, "asd", "first feature key's value");

    // repeat the same scenario for a feature without keys
    U2Feature feature2 = FeatureTestData::createTestFeatureWithoutKeys(seq, os);
    CHECK_NO_ERROR(os);
    featureDbi->removeAllKeys(feature2.id, "qwe", os);
    CHECK_NO_ERROR(os);
    // no keys should be here
    keys = featureDbi->getFeatureKeys(feature2.id, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(0, keys.size(), "second feature key count");
}

IMPLEMENT_TEST(FeatureDbiUnitTests, removeAllKeys) {
    U2FeatureDbi *featureDbi = FeatureTestData::getFeatureDbi();
    U2SequenceDbi *sequenceDbi = FeatureTestData::getSequenceDbi();

    U2OpStatusImpl os;
    U2Sequence seq;
    sequenceDbi->createSequenceObject(seq, "", os);
    CHECK_NO_ERROR(os);

    U2Feature feature1 = FeatureTestData::createTestFeature3(seq, os);
    CHECK_NO_ERROR(os);
    // add key with existing name
    featureDbi->addKey(feature1.id, U2FeatureKey("qwe", "lalka"), os);
    CHECK_NO_ERROR(os);
    // remove all keys with that name
    featureDbi->removeKey(feature1.id, U2FeatureKey("qwe", "A"), os);
    CHECK_NO_ERROR(os);
    // the feature is supposed to have 2 keys
    QList<U2FeatureKey> keys = featureDbi->getFeatureKeys(feature1.id, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(2, keys.size(), "first feature key count");
    CHECK_EQUAL(keys.at(0).name, "123", "first feature 0th key's name");
    CHECK_EQUAL(keys.at(0).value, "asd", "first feature 0th key's value");
    CHECK_EQUAL(keys.at(1).name, "qwe", "first feature 1st key's name");
    CHECK_EQUAL(keys.at(1).value, "lalka", "first feature 1st key's value");

    // try to remove the same key again
    featureDbi->removeKey(feature1.id, U2FeatureKey("qwe", "A"), os);
    CHECK_NO_ERROR(os);
    // result must be the same as previous
    keys = featureDbi->getFeatureKeys(feature1.id, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(2, keys.size(), "first feature key count");
    CHECK_EQUAL(keys.at(0).name, "123", "first feature 0th key's name");
    CHECK_EQUAL(keys.at(0).value, "asd", "first feature 0th key's value");
    CHECK_EQUAL(keys.at(1).name, "qwe", "first feature 1st key's name");
    CHECK_EQUAL(keys.at(1).value, "lalka", "first feature 1st key's value");

    // create an empty feature
    U2Feature feature2 = FeatureTestData::createTestFeatureWithoutKeys(seq, os);
    CHECK_NO_ERROR(os);
    // remove some key
    featureDbi->removeKey(feature2.id, U2FeatureKey("qwe", "A"), os);
    CHECK_NO_ERROR(os);
    // the feature is supposed to have no keys
    keys = featureDbi->getFeatureKeys(feature2.id, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(0, keys.size(), "first feature key count");
}

IMPLEMENT_TEST(FeatureDbiUnitTests, updateKeyValue) {
    U2FeatureDbi *featureDbi = FeatureTestData::getFeatureDbi();
    U2SequenceDbi *sequenceDbi = FeatureTestData::getSequenceDbi();

    U2OpStatusImpl os;
    U2Sequence seq;
    sequenceDbi->createSequenceObject(seq, "", os);
    CHECK_NO_ERROR(os);

    U2Feature feature1 = FeatureTestData::createTestFeature3(seq, os);
    CHECK_NO_ERROR(os);
    // add key with existing name
    featureDbi->addKey(feature1.id, U2FeatureKey("qwe", "lalka"), os);
    CHECK_NO_ERROR(os);
    // update values for the key
    featureDbi->updateKeyValue(feature1.id, U2FeatureKey("qwe", "newValue"), os);
    CHECK_NO_ERROR(os);
    // all the keys with given name should change value
    QList<U2FeatureKey> keys = featureDbi->getFeatureKeys(feature1.id, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(3, keys.size(), "first feature key count");
    CHECK_EQUAL(keys.at(0).name, "qwe", "first feature 0th key's name");
    CHECK_EQUAL(keys.at(0).value, "newValue", "first feature 0th key's value");
    CHECK_EQUAL(keys.at(1).name, "123", "first feature 1st key's name");
    CHECK_EQUAL(keys.at(1).value, "asd", "first feature 1st key's value");
    CHECK_EQUAL(keys.at(2).name, "qwe", "first feature 2nd key's name");
    CHECK_EQUAL(keys.at(2).value, "newValue", "first feature 2nd key's value");

    // repeat the same scenario for a key with nonexistent name
    U2Feature feature2 = FeatureTestData::createTestFeature1(seq, os);
    CHECK_NO_ERROR(os);
    featureDbi->updateKeyValue(feature2.id, U2FeatureKey("qwe", "newValue"), os);
    CHECK_NO_ERROR(os);

    // all the feature's keys should stay unchanged
    keys = featureDbi->getFeatureKeys(feature2.id, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(3, keys.size(), "second feature key count");
    CHECK_EQUAL(keys.at(0).name, "1", "second feature 0th key's name");
    CHECK_EQUAL(keys.at(0).value, "A", "second feature 0th key's value");
    CHECK_EQUAL(keys.at(1).name, "2", "second feature 1st key's name");
    CHECK_EQUAL(keys.at(1).value, "B", "second feature 1st key's value");
    CHECK_EQUAL(keys.at(2).name, "3", "second feature 2nd key's name");
    CHECK_EQUAL(keys.at(2).value, "C", "second feature 2nd key's value");
}

IMPLEMENT_TEST(FeatureDbiUnitTests, updateLocation) {
    U2FeatureDbi *featureDbi = FeatureTestData::getFeatureDbi();
    U2SequenceDbi *sequenceDbi = FeatureTestData::getSequenceDbi();

    U2OpStatusImpl os;
    U2Sequence seq;
    sequenceDbi->createSequenceObject(seq, "", os);
    CHECK_NO_ERROR(os);

    U2Feature feature = FeatureTestData::createTestFeature1(seq, os);
    CHECK_NO_ERROR(os);
    const U2FeatureLocation location(U2Strand::Direct, U2Region(500, 50));
    featureDbi->updateLocation(feature.id, location, os);
    CHECK_NO_ERROR(os);
    const U2Feature changedFeature = featureDbi->getFeature(feature.id, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(location.region.startPos, changedFeature.location.region.startPos,
        "feature region start");
    CHECK_EQUAL(location.region.length, changedFeature.location.region.length,
        "feature region length");
    CHECK_EQUAL(location.strand.getDirectionValue(),
        changedFeature.location.strand.getDirectionValue(), "feature region strand");
}

IMPLEMENT_TEST(FeatureDbiUnitTests, updateName) {
    U2FeatureDbi *featureDbi = FeatureTestData::getFeatureDbi();
    U2SequenceDbi *sequenceDbi = FeatureTestData::getSequenceDbi();

    U2OpStatusImpl os;
    U2Sequence seq;
    sequenceDbi->createSequenceObject(seq, "", os);
    CHECK_NO_ERROR(os);

    U2Feature feature = FeatureTestData::createTestFeature2(seq, os);
    CHECK_NO_ERROR(os);
    const QString newName("new_feature");
    featureDbi->updateName(feature.id, newName, os);
    CHECK_NO_ERROR(os);
    const U2Feature changedFeature = featureDbi->getFeature(feature.id, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(newName, changedFeature.name, "feature name");
}

IMPLEMENT_TEST(FeatureDbiUnitTests, updateParentId) {
    U2FeatureDbi *featureDbi = FeatureTestData::getFeatureDbi();
    U2SequenceDbi *sequenceDbi = FeatureTestData::getSequenceDbi();

    U2OpStatusImpl os;
    U2Sequence seq;
    sequenceDbi->createSequenceObject(seq, "", os);
    CHECK_NO_ERROR(os);

    U2Feature firstParentFeature = FeatureTestData::createTestFeature1(seq, os);
    CHECK_NO_ERROR(os);
    U2Feature feature = FeatureTestData::createTestFeature2(seq, os, firstParentFeature);
    CHECK_NO_ERROR(os);
    U2Feature secondParentFeature = FeatureTestData::createTestFeature3(seq, os);
    featureDbi->updateParentId(feature.id, secondParentFeature.id, os);
    CHECK_NO_ERROR(os);
    const U2Feature changedFeature = featureDbi->getFeature(feature.id, os);
    CHECK_NO_ERROR(os);

    CHECK_EQUAL(secondParentFeature.id, changedFeature.parentFeatureId, "feature parent id");
}

IMPLEMENT_TEST(FeatureDbiUnitTests, removeFeature) {
    U2FeatureDbi *featureDbi = FeatureTestData::getFeatureDbi();
    U2SequenceDbi *sequenceDbi = FeatureTestData::getSequenceDbi();

    U2OpStatusImpl os;
    U2Sequence seq;
    sequenceDbi->createSequenceObject(seq, "", os);
    CHECK_NO_ERROR(os);

    U2Feature feature = FeatureTestData::createTestFeature1(seq, os);
    CHECK_NO_ERROR(os);
    featureDbi->removeFeature(feature.id, os);
    CHECK_NO_ERROR(os);
    const U2Feature changedFeature = featureDbi->getFeature(feature.id, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(changedFeature.id.isEmpty(), "Unexpected value of feature ID");
}

IMPLEMENT_TEST(FeatureDbiUnitTests, getFeaturesByRegion) {
    U2FeatureDbi *featureDbi = FeatureTestData::getFeatureDbi();
    U2SequenceDbi *sequenceDbi = FeatureTestData::getSequenceDbi();

    U2OpStatusImpl os;
    U2Sequence seq;
    sequenceDbi->createSequenceObject(seq, "", os);
    CHECK_NO_ERROR(os);

    U2Feature feature1 = FeatureTestData::createTestFeature1(seq, os);
    CHECK_NO_ERROR(os);
    U2Feature feature2 = FeatureTestData::createTestFeature2(seq, os);
    CHECK_NO_ERROR(os);
    U2Feature feature3 = FeatureTestData::createTestFeature3(seq, os);
    CHECK_NO_ERROR(os);
    U2DbiIterator<U2Feature> *iter = featureDbi->getFeaturesByRegion(U2Region(900, 200),
        U2DataId(), "misc_feature", seq.id, os);
    CHECK_NO_ERROR(os);

    while (iter->hasNext()) {
        U2Feature current = iter->next();
        CHECK_TRUE((current.id == feature1.id) || (current.id == feature3.id),
            "Unexpected feature ID");
    }
}

IMPLEMENT_TEST(FeatureDbiUnitTests, getSubFeatures) {
    U2FeatureDbi *featureDbi = FeatureTestData::getFeatureDbi();
    U2SequenceDbi *sequenceDbi = FeatureTestData::getSequenceDbi();

    U2OpStatusImpl os;
    U2Sequence seq;
    sequenceDbi->createSequenceObject(seq, "", os);
    CHECK_NO_ERROR(os);

    U2Feature feature2 = FeatureTestData::createTestFeature2(seq, os);
    CHECK_NO_ERROR(os);
    U2Feature feature1 = FeatureTestData::createTestFeature1(seq, os, feature2);
    CHECK_NO_ERROR(os);
    U2Feature feature3 = FeatureTestData::createTestFeature3(seq, os, feature2);
    CHECK_NO_ERROR(os);
    U2DbiIterator<U2Feature> *iter = featureDbi->getFeaturesByParent(feature2.id, QString(),
        seq.id, os);
    CHECK_NO_ERROR(os);

    while (iter->hasNext()) {
        U2Feature current = iter->next();
        CHECK_TRUE((current.id == feature1.id) || (current.id == feature3.id),
            "Unexpected feature ID");
    }
}

IMPLEMENT_TEST(FeatureDbiUnitTests, getFeaturesBySequence) {
    U2FeatureDbi *featureDbi = FeatureTestData::getFeatureDbi();
    U2SequenceDbi *sequenceDbi = FeatureTestData::getSequenceDbi();

    U2OpStatusImpl os;
    U2Sequence seq1;
    sequenceDbi->createSequenceObject(seq1, "", os);
    CHECK_NO_ERROR(os);
    U2Sequence seq2;
    sequenceDbi->createSequenceObject(seq2, "", os);
    CHECK_NO_ERROR(os);

    U2Feature feature1 = FeatureTestData::createTestFeature1(seq1, os);
    CHECK_NO_ERROR(os);
    U2Feature feature2 = FeatureTestData::createTestFeature2(seq2, os);
    CHECK_NO_ERROR(os);
    U2Feature feature3 = FeatureTestData::createTestFeature3(seq1, os, feature2);
    CHECK_NO_ERROR(os);
    U2DbiIterator<U2Feature> *iter = featureDbi->getFeaturesBySequence("misc_feature", seq1.id,
        os);
    CHECK_NO_ERROR(os);

    while (iter->hasNext()) {
        U2Feature current = iter->next();
        CHECK_TRUE((current.id == feature1.id) || (current.id == feature3.id),
            "Unexpected feature ID");
    }
}

IMPLEMENT_TEST(FeatureDbiUnitTests, sortingSubgroups) {
    U2OpStatusImpl os;
    U2FeatureDbi *subgroupDbi = FeatureTestData::getSubgroupDbi();

    U2DataId rootFeatureId = U2DbiUtils::toU2DataId(2, U2Type::AnnotationTable);
    QList<FeatureAndKey> rawData = subgroupDbi->getFeatureTable(rootFeatureId, os);
    QList<FeatureAndKey> groups = U2FeatureUtils::getSortedSubgroups(rawData, rootFeatureId);

    QList<U2DataId> passedFeaturesList;
    for (int i = 0; i < groups.size(); i++) {
        U2DataId id = groups.at(i).feature.id;
        U2DataId parentId = groups.at(i).feature.parentFeatureId;
        passedFeaturesList.append(id);
        if (i == 0) {
            U2DataId rootId = groups.at(i).feature.rootFeatureId;
            CHECK_TRUE(parentId == rootId, "Wrong groups sorting");
        } else {
            CHECK_TRUE(passedFeaturesList.contains(parentId), "Wrong groups sorting");
        }
    }
}

} // namespace U2
