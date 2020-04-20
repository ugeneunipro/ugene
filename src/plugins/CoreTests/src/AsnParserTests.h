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

#ifndef _U2_ASN_PARSER_TESTS_H_
#define _U2_ASN_PARSER_TESTS_H_

#include <U2Test/XMLTestUtils.h>

#include <U2Core/IOAdapter.h>

#include <QDomElement>

namespace U2 {

class AsnNode;

class GTest_LoadAsnTree : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_LoadAsnTree, "load-asn-tree");

    ReportResult report();

    virtual void cleanup();

private:
    QString         asnTreeContextName;
    bool            contextAdded;
    AsnNode*        rootElem;
};

class GTest_FindFirstNodeByName : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_FindFirstNodeByName, "find-first-node-by-name");

    ReportResult report();
    void cleanup();
private:
    QString rootContextName;
    QString nodeContextName;
    QString nodeName;
    bool contextAdded;

};

class GTest_CheckNodeType : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_CheckNodeType, "check-node-type");

    ReportResult report();
private:
    QString nodeContextName;
    QString nodeTypeName;
};


class GTest_CheckNodeValue : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_CheckNodeValue, "check-node-value");

    ReportResult report();
private:
    QString nodeContextName;
    QString nodeValue;
};


class GTest_CheckNodeChildrenCount : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_CheckNodeChildrenCount, "check-node-children-count");
    ReportResult report();
private:
    QString nodeContextName;
    int numChildren;
};


class AsnParserTests {
public:
    static QList<XMLTestFactory*> createTestFactories();
};

}//namespace

#endif // _U2_ASN_PARSER_TESTS_H_
