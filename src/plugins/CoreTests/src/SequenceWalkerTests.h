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

#ifndef _U2_SEQUENCEWALKER_TESTS_H_
#define _U2_SEQUENCEWALKER_TESTS_H_

#include <U2Test/XMLTestUtils.h>
#include <U2Core/GObject.h>
#include <QDomElement>
#include <U2Core/SequenceWalkerTask.h>

namespace U2 {

class GTest_SW_CheckRegion : public XmlTest, public SequenceWalkerCallback {
    Q_OBJECT
public:

    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_SW_CheckRegion, "sw-check-region");

    ReportResult report();

    void onRegion(SequenceWalkerSubtask* t,TaskStateInfo& ti) {Q_UNUSED(t); Q_UNUSED(ti);}

private:
    int chunkSize;
    int overlap;
    int extraLen;
    U2Region region;
    bool reventNorm;
    QVector<U2Region> result;

protected:
};

class SequenceWalkerTests {
public:
    static QList<XMLTestFactory*> createTestFactories();
};
}//namespace
#endif
