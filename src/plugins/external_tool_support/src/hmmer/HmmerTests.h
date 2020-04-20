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

#ifndef _U2_HMMER_TESTS_H_
#define _U2_HMMER_TESTS_H_

#include <U2Test/XMLTestFormat.h>

#include "HmmerSearchTaskTest.h"
#include "HmmerBuildTaskTest.h"
#include "PhmmerSearchTaskTest.h"

namespace U2 {

class HmmerTests {
public:
    static QList<XMLTestFactory*> createTestFactories() {
        QList<XMLTestFactory*> res;
        res.append(GTest_UHMM3Search::createFactory());
        res.append(GTest_UHMM3SearchCompare::createFactory());

        res.append(GTest_UHMMER3Build::createFactory());
        res.append(GTest_CompareHmmFiles::createFactory());

        res.append(GTest_UHMM3Phmmer::createFactory());
        res.append(GTest_UHMM3PhmmerCompare::createFactory());
        return res;
    }
};

}

#endif
