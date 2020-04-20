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

#ifndef _U2_PTOOLS_TESTS_H_
#define _U2_PTOOLS_TESTS_H_

#include "PToolsAligner.h"

#include <U2Test/XMLTestUtils.h>

namespace U2 {

class Gtest_PToolsAlignerTask : public XmlTest {
    Q_OBJECT

public:
    static const QString REFO_ATTR;
    static const QString MOBO_ATTR;
    static const QString RMSD_ATTR;
    static const QString ACCURACY_ATTR;
    static const QString TRANSFORM_ATTR;

public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(Gtest_PToolsAlignerTask, "ptools-align-and-compare", TaskFlags_FOSCOE)

    void run();
    Task::ReportResult report();

private:
    QString refName, mobName;
    StructuralAlignment result, expected;
    double accuracy;
};  // class Gtest_PToolsAlignerTask

}   // namespace U2

#endif  // #ifndef _U2_PTOOLS_TESTS_H_

