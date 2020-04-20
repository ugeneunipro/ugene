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

#ifndef _U2_FEATURES_DBI_UNIT_TESTS_H_
#define _U2_FEATURES_DBI_UNIT_TESTS_H_

#include <unittest.h>

#include <U2Core/U2Feature.h>
#include <U2Core/U2Sequence.h>

#include "core/dbi/DbiTest.h"

namespace U2 {

class U2FeatureDbi;
class U2SequenceDbi;

class FeatureTestData {
public:
    static U2FeatureDbi * getFeatureDbi( );
    static U2SequenceDbi * getSequenceDbi( );
    static U2FeatureDbi *getSubgroupDbi();

    static U2Feature createTestFeature1( const U2Sequence &seq, U2OpStatus &os,
        const U2Feature &parentFeature = U2Feature( ) );
    static U2Feature createTestFeature2( const U2Sequence &seq, U2OpStatus &os,
        const U2Feature &parentFeature = U2Feature( ) );
    static U2Feature createTestFeature3( const U2Sequence &seq, U2OpStatus &os,
        const U2Feature &parentFeature = U2Feature( ) );
    static U2Feature createTestFeatureWithoutKeys( const U2Sequence &seq, U2OpStatus &os,
        const U2Feature &parentFeature = U2Feature( ) );

    static void shutdown( );

private:
    static void init( );

    static TestDbiProvider dbiProvider;
    static TestDbiProvider subgroupsDbiProvider;
    static const QString featureDbiUrl;
    static const QString subgroupDbiUrl;
    static U2FeatureDbi *featureDbi;
    static U2FeatureDbi *subgroupDbi;
    static U2SequenceDbi *sequenceDbi;
};

/** Creates new feature in DB */
DECLARE_TEST( FeatureDbiUnitTests, createFeature );
/** Gets feature from DB by ID */
DECLARE_TEST( FeatureDbiUnitTests, getFeature );
/** Counts features that matched the query */
DECLARE_TEST( FeatureDbiUnitTests, countFeatures );
/** Get features that matched the query */
DECLARE_TEST( FeatureDbiUnitTests, getFeatures );
/** Get all keys of a specified feature */
DECLARE_TEST( FeatureDbiUnitTests, getFeatureKeys );
/** Add key to feature */
DECLARE_TEST( FeatureDbiUnitTests, addKey );
/** Remove all feature keys with a specified name */
DECLARE_TEST( FeatureDbiUnitTests, removeAllKeysByName );
/** Remove all feature keys with a specified name and value */
DECLARE_TEST( FeatureDbiUnitTests, removeAllKeys );
/** Update feature key */
DECLARE_TEST( FeatureDbiUnitTests, updateKeyValue );
/** Updates feature location */
DECLARE_TEST( FeatureDbiUnitTests, updateLocation );
/** Updates feature name */
DECLARE_TEST( FeatureDbiUnitTests, updateName );
/** Update feature parent */
DECLARE_TEST( FeatureDbiUnitTests, updateParentId );
/** Remove the feature from database */
DECLARE_TEST( FeatureDbiUnitTests, removeFeature );
/** Return features that matched the query */
DECLARE_TEST( FeatureDbiUnitTests, getFeaturesByRegion );
DECLARE_TEST( FeatureDbiUnitTests, getSubFeatures );
DECLARE_TEST( FeatureDbiUnitTests, getFeaturesBySequence );
/** Testing properly sorting of annotation subgroups */
DECLARE_TEST( FeatureDbiUnitTests, sortingSubgroups );

} // namespace U2

DECLARE_METATYPE( FeatureDbiUnitTests, createFeature );
DECLARE_METATYPE( FeatureDbiUnitTests, getFeature );
DECLARE_METATYPE( FeatureDbiUnitTests, countFeatures );
DECLARE_METATYPE( FeatureDbiUnitTests, getFeatures );
DECLARE_METATYPE( FeatureDbiUnitTests, getFeatureKeys );
DECLARE_METATYPE( FeatureDbiUnitTests, addKey );
DECLARE_METATYPE( FeatureDbiUnitTests, removeAllKeysByName );
DECLARE_METATYPE( FeatureDbiUnitTests, removeAllKeys );
DECLARE_METATYPE( FeatureDbiUnitTests, updateKeyValue );
DECLARE_METATYPE( FeatureDbiUnitTests, updateLocation );
DECLARE_METATYPE( FeatureDbiUnitTests, updateName );
DECLARE_METATYPE( FeatureDbiUnitTests, updateParentId );
DECLARE_METATYPE( FeatureDbiUnitTests, removeFeature );
DECLARE_METATYPE( FeatureDbiUnitTests, getFeaturesByRegion );
DECLARE_METATYPE( FeatureDbiUnitTests, getSubFeatures );
DECLARE_METATYPE( FeatureDbiUnitTests, getFeaturesBySequence );
DECLARE_METATYPE( FeatureDbiUnitTests, sortingSubgroups );

#endif // _U2_FEATURES_DBI_UNIT_TESTS_H_
