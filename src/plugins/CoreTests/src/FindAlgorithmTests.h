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

#ifndef _U2_FINDALGORITHM_TESTS_H_
#define _U2_FINDALGORITHM_TESTS_H_

#include <U2Algorithm/FindAlgorithmTask.h>
#include <U2Test/XMLTestUtils.h>
#include <U2Core/DNASequenceObject.h>

namespace U2 {

class GTest_FindAlgorithmTest : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_FindAlgorithmTest, "find-algorithm");

    void prepare();
    Task::ReportResult report();

private:
    FindAlgorithmTaskSettings settings;
    bool translatetoAmino;
    QString sequenceName;
    QString docName;
    U2SequenceObject *se;
    QList<U2Region> expectedResults;
    FindAlgorithmTask *t;
};

class FindAlgorithmTests {
public:
    static QList<XMLTestFactory*> createTestFactories();
};

}//ns

#endif
