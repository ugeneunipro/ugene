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

#ifndef _U2_PRIMER3_TESTS_H_
#define _U2_PRIMER3_TESTS_H_

#include <U2Core/global.h>
#include <U2Test/XMLTestUtils.h>
#include <U2Core/GObject.h>
#include <QDomElement>
#include "Primer3Task.h"

namespace U2 {

class GTest_Primer3 : public XmlTest {
Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_Primer3, "plugin_primer_3", TaskFlags(TaskFlag_FailOnSubtaskCancel) | TaskFlag_NoRun);

    ~GTest_Primer3();

    void prepare();
    Task::ReportResult report();

private:
    bool checkPrimerPair(const PrimerPair &primerPair, const PrimerPair &expectedPrimerPair, QString suffix);
    bool checkPrimer(const Primer *primer, const Primer *expectedPrimer, QString prefix, bool internalOligo);
    bool checkIntProperty(int value, int expectedValue, QString name);
    bool checkDoubleProperty(double value, double expectedValue, QString name);
    bool checkAlignProperty(short value, short expectedValue, QString name);
    Primer3SWTask *task;
    Primer3TaskSettings settings;
    QList<PrimerPair> currentBestPairs;
    QList<PrimerPair> expectedBestPairs;
};

}//ns
#endif
