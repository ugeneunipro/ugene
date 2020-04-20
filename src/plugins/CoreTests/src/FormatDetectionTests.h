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

#ifndef _U2_FORMAT_DETECTION_TESTS_H_
#define _U2_FORMAT_DETECTION_TESTS_H_

#include <U2Core/DocumentUtils.h>
#include <U2Test/XMLTestUtils.h>

namespace U2 {

// NOTE: "fd" stays for Format Detection

/** Check that given file will get specified score */
class GTest_CheckScore : public XmlTest {
    Q_OBJECT

public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_CheckScore, "fd-check-score", TaskFlags_FOSCOE);

    virtual void run();
    virtual ReportResult report();

private:
    QString fileURL;
    DocumentFormatId expectedFormat;
    QList<FormatDetectionResult> matchedFormats;

    // check constraint
    QString cmpFunStr;
    int value;
};

/** Check that format detected exatly for specified file */
class GTest_PerfectMatch : public XmlTest {
    Q_OBJECT

public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_PerfectMatch, "fd-perfect-match", TaskFlags_FOSCOE);

    virtual void run();
    virtual ReportResult report();

private:
    QString fileURL;
    DocumentFormatId expectedFormat;
    QList<FormatDetectionResult> matchedFormats;
};

class FormatDetectionTests {
public:
    static QList<XMLTestFactory*> createTestFactories();
};

}   // namespace U2

#endif  // #ifndef _U2_FORMAT_DETECTION_TESTS_H_
