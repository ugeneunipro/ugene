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

#include <U2Core/AnnotationData.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2Region.h>
#include <U2Core/U1AnnotationUtils.h>

#include <U2Formats/GenbankLocationParser.h>

#include "LocationParserUnitTests.h"

namespace U2 {

IMPLEMENT_TEST(LocationParserTestData, locationParser) {
    QString regionStr = "0..0";
    U2Location location;
    Genbank::LocationParser::parseLocation(qPrintable(regionStr),regionStr.length(), location);
    QVector<U2Region> regions = location->regions;
    CHECK_EQUAL(regions.size(), 1, "regions size should be 1");

    regionStr = "0..10,10..12";
    Genbank::LocationParser::parseLocation(qPrintable(regionStr),regionStr.length(), location);
    regions = location->regions;

    CHECK_EQUAL(regions.size(), 2, "regions size should be 2");
}
IMPLEMENT_TEST(LocationParserTestData, locationParserEmpty) {
    QString regionStr;
    U2Location location;
    Genbank::LocationParser::parseLocation(qPrintable(regionStr),regionStr.length(), location);
    QVector<U2Region> regions = location->regions;
    CHECK_EQUAL(regions.size(), 0, "regions size should be 1");
}


IMPLEMENT_TEST(LocationParserTestData, locationParserCompare) {
    QString original = "0..10";
    U2Location location;
    Genbank::LocationParser::parseLocation(qPrintable(original),original.length(), location);
    QVector<U2Region> regions = location->regions;
    CHECK_EQUAL(regions.size(), 1, "regions size should be 1");
    QString expected = U1AnnotationUtils::buildLocationString(regions);
    CHECK_TRUE(original == expected, "regions should be the same");
}

IMPLEMENT_TEST(LocationParserTestData, locationParserDuplicate) {
    QString regionStr = "0..10,0..10,0..10";
    U2Location location;
    Genbank::LocationParser::parseLocation(qPrintable(regionStr),regionStr.length(), location);
    QVector<U2Region> regions = location->regions;
    CHECK_EQUAL(3, regions.size(), "regions size should be 1");
}


IMPLEMENT_TEST(LocationParserTestData, locationParserInvalid) {
    QString regionStr = "join(10..9,-22..30)";
    U2Location location;
    Genbank::LocationParser::parseLocation(qPrintable(regionStr),regionStr.length(), location);
    QVector<U2Region> regions = location->regions;
    CHECK_EQUAL(0, regions.size(), "regions size should be 0");

    regionStr = "0.10,15-20,30..0xFF";
    Genbank::LocationParser::parseLocation(qPrintable(regionStr),regionStr.length(), location);
    regions = location->regions;
    CHECK_EQUAL(regions.size(), 0, "regions size should be 0");
}

IMPLEMENT_TEST(LocationParserTestData, hugeLocationParser) {
    U2Location location;
    QString regionStr = "";
    qint64 i;
    for (i = 0; i < 10000000; i ++) {
        regionStr += QString::number(i*10) + ".." + QString::number(10*(i+1));
        if ( i < 10000000 - 1) {
            regionStr += + ",";
        }
    }

    Genbank::LocationParser::parseLocation(qPrintable(regionStr),regionStr.length(), location);
    QVector<U2Region> regions = location->regions;
    CHECK_EQUAL(regions.size(), i, "regions size should be " + QString::number(i));

    SharedAnnotationData ad;
    ad->location->regions = regions;
    QString expectedStr = U1AnnotationUtils::buildLocationString(ad);
    CHECK_TRUE(expectedStr.length() > 0, "regions string should not be empty");
    QStringList expected = expectedStr.split(",");
    CHECK_EQUAL(expected.size(), ad->location->regions.size(), "incorrect expected regions size");
}

IMPLEMENT_TEST(LocationParserTestData, locationParserComplement) {
    QString regionStr = "complement(0..0)";
    U2Location location;
    Genbank::LocationParser::parseLocation(qPrintable(regionStr),regionStr.length(), location);
    QVector<U2Region> regions = location->regions;
    CHECK_EQUAL(regions.size(), 1, "regions size should be 1");

    regionStr = "complement(0..10,10..12)";
    Genbank::LocationParser::parseLocation(qPrintable(regionStr),regionStr.length(), location);
    regions = location->regions;

    CHECK_EQUAL(regions.size(), 2, "regions size should be 2");
}

IMPLEMENT_TEST(LocationParserTestData, locationParserComplementInvalid) {
    QString regionStr = "complement{0..0)";
    U2Location location;
    Genbank::LocationParser::parseLocation(qPrintable(regionStr),regionStr.length(), location);
    QVector<U2Region> regions = location->regions;
    CHECK_EQUAL(0, regions.size(), "regions size should be 1");

    regionStr = "complement(0..10,10..12}";
    Genbank::LocationParser::parseLocation(qPrintable(regionStr),regionStr.length(), location);
    regions = location->regions;

    CHECK_EQUAL(0, regions.size(), "regions size should be 2");
}

IMPLEMENT_TEST(LocationParserTestData, buildLocationString) {
    SharedAnnotationData ad(new AnnotationData);
    qint64 region_length = 100;
    for (int i = 0; i < 1000; i++){
        ad->location->regions << U2Region((region_length - 1) * i, region_length);
    }
    QString regionStr = U1AnnotationUtils::buildLocationString(ad);
    CHECK_TRUE(regionStr.length() > 0, "regions string should not be empty");

    QStringList regions = regionStr.split(",");
    CHECK_EQUAL(regions.size(), ad->location->regions.size(), "incorrect expected regions size");

    U2Location location;
    Genbank::LocationParser::parseLocation(qPrintable(regionStr),regionStr.length(), location);
    QVector<U2Region> expected = location->regions;
    CHECK_EQUAL(regions.size(), expected.size(), "incorrect expected regions size");
}

IMPLEMENT_TEST(LocationParserTestData, buildLocationStringDuplicate) {
    SharedAnnotationData ad(new AnnotationData);
    qint64 region_length = 100;
    for (int i = 0; i < 10; i++){
        ad->location->regions << U2Region(1, region_length);
    }
    QString regionStr = U1AnnotationUtils::buildLocationString(ad);
    CHECK_TRUE(regionStr.length() > 0, "regions string should not be empty");

    QStringList regions = regionStr.split(",");
    CHECK_EQUAL(regions.size(), ad->location->regions.size(), "incorrect expected regions size");

    U2Location location;
    Genbank::LocationParser::parseLocation(qPrintable(regionStr),regionStr.length(), location);
    QVector<U2Region> expected = location->regions;
    CHECK_EQUAL(regions.size(), expected.size(), "incorrect expected regions size");
}

IMPLEMENT_TEST(LocationParserTestData, buildLocationStringInvalid) {
    SharedAnnotationData ad(new AnnotationData);
    qint64 region_length = 10;
    for (int i = 0; i < 10; i++){
        ad->location->regions << U2Region(-region_length * i, -region_length);
    }
    QString regionStr = U1AnnotationUtils::buildLocationString(ad);
    QStringList regions = regionStr.split(",");

    U2Location location;
    Genbank::LocationParser::parseLocation(qPrintable(regionStr),regionStr.length(), location);
    QVector<U2Region> expected = location->regions;
    CHECK_EQUAL(expected.size(), regions.size(), "incorrect expected regions size");
}

IMPLEMENT_TEST(LocationParserTestData, locationOperatorJoin) {
    SharedAnnotationData ad(new AnnotationData);
    ad->setLocationOperator(U2LocationOperator_Join);

    qint64 region_length = 10;
    for (int i = 0; i < 10; i++){
        ad->location->regions << U2Region((region_length -1) * i, region_length);
    }
    QString regionStr = U1AnnotationUtils::buildLocationString(ad);
    CHECK_TRUE(regionStr.length() > 0, "regions string should not be empty");
    CHECK_TRUE(regionStr.startsWith("join"), "regions join string must start with <join>");
}


IMPLEMENT_TEST(LocationParserTestData, locationOperatorJoinInvalid) {
    QString str = "join 1..10,11..10)";
    U2Location location;
    Genbank::LocationParser::parseLocation(qPrintable(str),str.length(), location);
    QVector<U2Region> regions = location->regions;
    CHECK_EQUAL(0, regions.size(), "incorrect expected regions size");
}

IMPLEMENT_TEST(LocationParserTestData, locationOperatorOrder) {
    SharedAnnotationData ad(new AnnotationData);
    ad->setLocationOperator(U2LocationOperator_Order);

    qint64 region_length = 100;
    for (int i = 0; i < 10; i++){
        ad->location->regions << U2Region((region_length -1) * i, region_length);
    }
    QString regionStr = U1AnnotationUtils::buildLocationString(ad);
    CHECK_TRUE(regionStr.length() > 0, "regions string should not be empty");
    CHECK_TRUE(regionStr.startsWith("order"), "regions join string must start with order");
}

IMPLEMENT_TEST(LocationParserTestData, locationOperatorOrderInvalid) {
    QString str = "order(1..10,11..10";
    U2Location location;
    Genbank::LocationParser::parseLocation(qPrintable(str),str.length(), location);
    QVector<U2Region> regions = location->regions;
    CHECK_EQUAL(0, regions.size(), "incorrect expected regions size");
}

IMPLEMENT_TEST(LocationParserTestData, locationParserParenthesis) {
    QString str = "join(1..10,11..10)";
    U2Location location;
    Genbank::LocationParser::parseLocation(qPrintable(str),str.length(), location);
    QVector<U2Region> regions = location->regions;
    CHECK_EQUAL(2, regions.size(), "incorrect expected regions size");
}

IMPLEMENT_TEST(LocationParserTestData, locationParserParenthesisInvalid) {
    QString str = "join((1..10,11..10),(31..10))";
    U2Location location;
    Genbank::LocationParser::parseLocation(qPrintable(str),str.length(), location);
    QVector<U2Region> regions = location->regions;
    CHECK_EQUAL(0, regions.size(), "incorrect expected regions size");
}

IMPLEMENT_TEST(LocationParserTestData, locationParserLeftParenthesisMissed) {
    QString str = "join 1..10)";
    U2Location location;
    Genbank::LocationParser::parseLocation(qPrintable(str),str.length(), location);
    QVector<U2Region> regions = location->regions;
    CHECK_EQUAL(0, regions.size(), "incorrect expected regions size");
}

IMPLEMENT_TEST(LocationParserTestData, locationParserRightParenthesisMissed) {
    QString str = "oreder(1..10";
    U2Location location;
    Genbank::LocationParser::parseLocation(qPrintable(str),str.length(), location);
    QVector<U2Region> regions = location->regions;
    CHECK_EQUAL(0, regions.size(), "incorrect expected regions size");
}

IMPLEMENT_TEST(LocationParserTestData, locationParserName) {
    QString str = "test.1:(3.4)..(5.6)";
    U2Location location;
    Genbank::LocationParser::parseLocation(qPrintable(str),str.length(), location);
    CHECK_EQUAL(0, location->regions.size(), "incorrect expected regions size");
}


IMPLEMENT_TEST(LocationParserTestData, locationParserNameInvalid) {
    QString str = "test.1.2:(3.4)..(5.6),(7.8)..(9.10)";
    U2Location location;
    Genbank::LocationParser::parseLocation(qPrintable(str),str.length(), location);
    CHECK_EQUAL(0, location->regions.size(), "incorrect expected regions size");
}

IMPLEMENT_TEST(LocationParserTestData, locationParserPeriod) {
    QString str = "(3.4)..(5.6)";
    U2Location location;
    Genbank::LocationParser::parseLocation(qPrintable(str),str.length(), location);
    CHECK_EQUAL(1, location->regions.size(), "incorrect expected regions size");

    str = "(0.0)..(1.3),(20.8)..(45.74)";
    Genbank::LocationParser::parseLocation(qPrintable(str),str.length(), location);
    CHECK_EQUAL(2, location->regions.size(), "incorrect expected regions size");
}

IMPLEMENT_TEST(LocationParserTestData, locationParserPeriodInvalid) {
    QString str = "(1.11).(13.0)";
    U2Location location;
    Genbank::LocationParser::parseLocation(qPrintable(str),str.length(), location);
    QVector<U2Region> regions = location->regions;
    CHECK_EQUAL(0, regions.size(), "incorrect expected regions size");
}

IMPLEMENT_TEST(LocationParserTestData, locationParserDoublePeriodInvalid) {
    QString str = "join(..10,12..,)";
    U2Location location;
    Genbank::LocationParser::parseLocation(qPrintable(str),str.length(), location);
    QVector<U2Region> regions = location->regions;
    CHECK_EQUAL(0, regions.size(), "incorrect expected regions size");
}

IMPLEMENT_TEST(LocationParserTestData, locationParserCommaInvalid) {
    QString str = "order(1,3,5,)";
    U2Location location;
    Genbank::LocationParser::parseLocation(qPrintable(str),str.length(), location);
    QVector<U2Region> regions = location->regions;
    CHECK_EQUAL(0, regions.size(), "incorrect expected regions size");
}

IMPLEMENT_TEST(LocationParserTestData, locationParserNumberInvalid) {
    QString str = "9223372036854775809..9223372036854775899";
    U2Location location;
    Genbank::LocationParser::parseLocation(qPrintable(str),str.length(), location);
    QVector<U2Region> regions = location->regions;
    SharedAnnotationData ad(new AnnotationData);
    ad->location->regions << location->regions;
    QString regionStr = U1AnnotationUtils::buildLocationString(ad);
    U2Location newLocation;
    Genbank::LocationParser::parseLocation(qPrintable(regionStr),regionStr.length(), newLocation);

    CHECK_EQUAL(location->regions.size(), newLocation->regions.size(), "incorrect expected regions size");
}

IMPLEMENT_TEST(LocationParserTestData, locationBuildStringNumberInvalid) {
    SharedAnnotationData ad(new AnnotationData);
    ad->location->regions << U2Region(Q_INT64_C(9223372036854775807), 90);
    QString regionStr = U1AnnotationUtils::buildLocationString(ad);
    U2Location location;
    Genbank::LocationParser::parseLocation(qPrintable(regionStr),regionStr.length(), location);
    CHECK_EQUAL(ad->location->regions.size(), location->regions.size(), "incorrect expected regions size");
}

IMPLEMENT_TEST(LocationParserTestData, locationParserLessInvalid) {
    QString str = "<<1..13";
    U2Location location;
    Genbank::LocationParser::parseLocation(qPrintable(str),str.length(), location);
    CHECK_EQUAL(0, location->regions.size(), "incorrect expected regions size");
}

IMPLEMENT_TEST(LocationParserTestData, locationParserGreaterInvalid) {
    QString str = ">>>>10..19";
    U2Location location;
    Genbank::LocationParser::parseLocation(qPrintable(str),str.length(), location);
    CHECK_EQUAL(0, location->regions.size(), "incorrect expected regions size");

}

/*IMPLEMENT_TEST(LocationParserTestData, locationParserNameInvald) {
    QString str = "test10..19";
    U2Location location;
    Genbank::LocationParser::parseLocation(qPrintable(str, str.length(), location);
    CHECK_EQUAL(1, location->regions.size(), "incorrect expected regions size");
}*/

} //namespace
