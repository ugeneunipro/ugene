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

#ifndef _U2_PHYLTREE_OBJECT_TESTS_H_
#define _U2_PHYLTREE_OBJECT_TESTS_H_

#include <U2Test/XMLTestUtils.h>
#include <QDomElement>
#include <QHash>

namespace U2 {

class Document;
class PhyTreeGeneratorLauncherTask;

class GTest_CalculateTreeFromAligment : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_CalculateTreeFromAligment, "calc-tree-from-alignment");

    ReportResult report();
    void prepare();

    PhyTreeGeneratorLauncherTask* task;
    QString objContextName;
    QString treeObjContextName;
    QString algId;
};

class GTest_CheckPhyNodeHasSibling : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_CheckPhyNodeHasSibling, "check-phynode-has-sibling");

    ReportResult report();

    QString treeContextName;
    QString nodeName;
    QString siblingName;

};

class GTest_CheckPhyNodeBranchDistance : public XmlTest {
    Q_OBJECT
        SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_CheckPhyNodeBranchDistance, "check-phynode-branch-distance");

    ReportResult report();

    QString treeContextName;
    QString nodeName;
    double distance;

};

class GTest_CompareTreesInTwoObjects : public XmlTest {
    Q_OBJECT
        SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_CompareTreesInTwoObjects, "compare-trees-in-two-objects");

    ReportResult report();
    QString docContextName;
    QString secondDocContextName;
};

class PhyTreeObjectTests {
public:
    static QList<XMLTestFactory*> createTestFactories();
};



}//namespace

#endif //_U2_BIOSTRUCT3D_OBJECT_TESTS_H_
