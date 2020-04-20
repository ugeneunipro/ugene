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

#ifndef _U2_FEATURE_TABLE_OBJECT_TESTS_H_
#define _U2_FEATURE_TABLE_OBJECT_TESTS_H_

#include <core/dbi/DbiTest.h>

#include <U2Core/AnnotationTableObject.h>

#include <unittest.h>

namespace U2 {

class FeaturesTableObjectTestData {
public:
    static void init();
    static void shutdown();

    static U2FeatureDbi * getFeatureDbi();

protected:
    static TestDbiProvider dbiProvider;

private:
    static U2FeatureDbi * featureDbi;
};

DECLARE_TEST(FeatureTableObjectUnitTest, createEmptyFeaturesTableObject);
// annotation interface tests
DECLARE_TEST(FeatureTableObjectUnitTest, addAnnotationSingleRegion);
DECLARE_TEST(FeatureTableObjectUnitTest, addAnnotationMultipleRegion);
DECLARE_TEST(FeatureTableObjectUnitTest, getAnnotations);
DECLARE_TEST(FeatureTableObjectUnitTest, getRootGroup);
DECLARE_TEST(FeatureTableObjectUnitTest, addAnnotationsToRootGroup);
DECLARE_TEST(FeatureTableObjectUnitTest, addAnnotationsToSubgroup);
DECLARE_TEST(FeatureTableObjectUnitTest, removeAnnotation);
DECLARE_TEST(FeatureTableObjectUnitTest, removeAnnotations);
DECLARE_TEST(FeatureTableObjectUnitTest, clone);
DECLARE_TEST(FeatureTableObjectUnitTest, getAnnotationsByName);
DECLARE_TEST(FeatureTableObjectUnitTest, getAnnotationsByRegion);
DECLARE_TEST(FeatureTableObjectUnitTest, checkConstraints);

}//namespace

DECLARE_METATYPE(FeatureTableObjectUnitTest, createEmptyFeaturesTableObject)
DECLARE_METATYPE(FeatureTableObjectUnitTest, addAnnotationSingleRegion)
DECLARE_METATYPE(FeatureTableObjectUnitTest, addAnnotationMultipleRegion)
DECLARE_METATYPE(FeatureTableObjectUnitTest, getAnnotations)
DECLARE_METATYPE(FeatureTableObjectUnitTest, getRootGroup)
DECLARE_METATYPE(FeatureTableObjectUnitTest, addAnnotationsToRootGroup)
DECLARE_METATYPE(FeatureTableObjectUnitTest, addAnnotationsToSubgroup)
DECLARE_METATYPE(FeatureTableObjectUnitTest, removeAnnotation)
DECLARE_METATYPE(FeatureTableObjectUnitTest, removeAnnotations)
DECLARE_METATYPE(FeatureTableObjectUnitTest, clone)
DECLARE_METATYPE(FeatureTableObjectUnitTest, getAnnotationsByName)
DECLARE_METATYPE(FeatureTableObjectUnitTest, getAnnotationsByRegion)
DECLARE_METATYPE(FeatureTableObjectUnitTest, checkConstraints)

#endif //_U2_FEATURE_TABLE_OBJECT_TESTS_H_
