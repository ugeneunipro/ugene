/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#pragma once

#include <QDomElement>

#include <U2Core/DNAAlphabet.h>

#include <U2Test/XMLTestUtils.h>

namespace U2 {

class Document;

//---------------------------------------------------------------------
class GTest_DNATranslation3to1Test : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_DNATranslation3to1Test, "check-translation");

    ReportResult report() override;

    QString objContextName;

    int strFrom;
    int strTo;
    QString stringValue;
};
//---------------------------------------------------------------------
class DNATranslationImplTests {
public:
    static QList<XMLTestFactory*> createTestFactories();
};

}  // namespace U2
