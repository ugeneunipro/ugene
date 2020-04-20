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

#ifndef __BINARY_FIND_OPENCL_TESTS_H__
#define __BINARY_FIND_OPENCL_TESTS_H__

#ifdef OPENCL_SUPPORT

#include <QVector>

#include <U2Algorithm/BinaryFindOpenCL.h>

#include <U2Test/XMLTestUtils.h>

namespace U2{

class GTest_BinaryFindOpenCL : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_BinaryFindOpenCL, "check-binary-find-opencl", TaskFlags_NR_FOSCOE);

    void prepare();
    Task::ReportResult report();

private:
    QVector<NumberType> numbers;
    QVector<NumberType> findNumbers;
    QVector<int> windowSizes;
    QVector<NumberType> expectedResults;
    NumberType* results;
};

class BinaryFindOpenCLTests {
public:
    static QList< XMLTestFactory* > createTestFactories();
};

} //namespace

#endif /*OPENCL_SUPPORT*/

#endif // __BINARY_FIND_OPENCL_TESTS_H__
