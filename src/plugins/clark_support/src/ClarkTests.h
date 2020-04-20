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

#ifndef _U2_CLARK_TESTS_H_
#define _U2_CLARK_TESTS_H_

#include <U2Test/XMLTestUtils.h>

namespace U2 {

/**
 * @brief The GTest_CompareClarkDatabaseMetafiles class
 * This test compares metafiles of two CLARK databases defined by @database1 and @database2 attributes.
 * The following metafiles are compared: ".custom", ".custom.fileToAccssnTaxID", ".custom.fileToTaxIDs", ".custom_rejected", "files_excluded.txt", "targets.txt".
 * The metafiles ".setting" are not compared.
 * @database1-prefixes and @database2-prefixes attributes contain lists of paths to folders with
 * the source reference sequences that were used to build the databases separated with ';'.
 * These paths are removed from every line of metafiles to make them independent from the test folder path.
 */
class GTest_CompareClarkDatabaseMetafiles : public XmlTest {
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_CompareClarkDatabaseMetafiles, "clark-compare-database-metafiles")

private:
    ReportResult report() override;

    QString database1;
    QString database2;
    QStringList database1Prefixes;
    QStringList database2Prefixes;

    // attributes
    static const QString DATABASE1;         // this attribute is a path to the first CLARK database
    static const QString DATABASE2;         // this attribute is a path to the second CLARK database
    static const QString DATABASE1_PREFIXES;  // this attribute contains prefixes separated with ';' for database 1 that will be removed from every database 1 metafile line before lines comparison
    static const QString DATABASE2_PREFIXES;  // this attribute contains prefixes separated with ';' for database 2 that will be removed from every database 2 metafile line before lines comparison

    // inner constants
    static const QString DATABASE_PREFIX_PLACEHOLDER;
    static const QStringList DATABASE_METAFILES;
    static const QString PREFIXES_SEPARATOR;
};

class ClarkTests {
public:
    static QList<XMLTestFactory *> createTestFactories();
};

}   // namespace U2

#endif // _U2_CLARK_TESTS_H_
