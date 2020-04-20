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

#ifndef _U2_PHYML_TESTS_H
#define _U2_PHYML_TESTS_H

#include <U2Test/XMLTestUtils.h>
#include <U2Core/GObject.h>
#include <U2Algorithm/PhyTreeGeneratorTask.h>

#include <QDomElement>
#include <QFileInfo>
#include <QList>

namespace U2{

class PhyTreeObject;
class MultipleSequenceAlignmentObject;

class GTest_PhyML : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_PhyML, "test-PHYML");

    void prepare();
    Task::ReportResult report();

private:
    QString inputDocCtxName;
    QString resultCtxName;
    QString negative;

    Document* maDoc;
    Document* treeDoc;
    PhyTreeGeneratorLauncherTask* task;
    MultipleSequenceAlignmentObject* input;
    PhyTreeObject* treeObjFromDoc;
    CreatePhyTreeSettings settings;
};

class  PhyMLToolTests {
public:
    static QList<XMLTestFactory*> createTestFactories();
};

}//namespace

#endif // _U2_PHYML_TESTS_H
