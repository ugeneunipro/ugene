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

#ifndef _U2_BIOSTRUCT3D_OBJECT_TESTS_H_
#define _U2_BIOSTRUCT3D_OBJECT_TESTS_H_

#include <U2Test/XMLTestUtils.h>
#include <QDomElement>
#include <QHash>

namespace U2 {

class Document;

class GTest_BioStruct3DNumberOfAtoms : public XmlTest {
    Q_OBJECT
        SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_BioStruct3DNumberOfAtoms, "check-biostruct3d-number-of-atoms");

    ReportResult report();

    QString objContextName;
    int numAtoms;

};

class GTest_BioStruct3DNumberOfChains : public XmlTest {
    Q_OBJECT
        SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_BioStruct3DNumberOfChains, "check-biostruct3d-number-of-chains");

    ReportResult report();

    QString objContextName;
    int     numChains;
};


class GTest_BioStruct3DAtomCoordinates : public XmlTest {
    Q_OBJECT
        SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_BioStruct3DAtomCoordinates, "check-biostruct3d-atom-coords");

    ReportResult report();

    QString objContextName;
    double  x,y,z;
    int     atomId;
    int     modelId;

};

class GTest_BioStruct3DAtomChainIndex : public XmlTest {
    Q_OBJECT
        SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_BioStruct3DAtomChainIndex, "check-biostruct3d-atom-chain-id");

    ReportResult report();

    QString objContextName;
    int     chainId;
    int     atomId;
    int     modelId;

};

class GTest_BioStruct3DAtomResidueName : public XmlTest {
    Q_OBJECT
        SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_BioStruct3DAtomResidueName, "check-biostruct3d-atom-residue-name");

    ReportResult report();

    QString objContextName;
    QString residueName;
    int     atomId;
    int     modelId;

};


class GTest_PDBFormatStressTest : public XmlTest {
    Q_OBJECT
        SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_PDBFormatStressTest, "try-load-all-pdb-documents");

    ReportResult report();
    QList<Task*> onSubTaskFinished(Task* subTask);
    QHash<Task*, QString>   fileNames;
};


class GTest_ASNFormatStressTest : public XmlTest {
    Q_OBJECT
        SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_ASNFormatStressTest, "try-load-all-mmdb-documents");

    ReportResult report();
    QList<Task*> onSubTaskFinished(Task* subTask);
    QHash<Task*, QString>   fileNames;
};


class BioStruct3DObjectTests {
public:
    static QList<XMLTestFactory*> createTestFactories();
};



}//namespace

#endif //_U2_BIOSTRUCT3D_OBJECT_TESTS_H_
