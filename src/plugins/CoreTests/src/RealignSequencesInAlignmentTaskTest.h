/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#ifndef _U2_REALIGN_SEQUENCES_IN_ALIGNMENT_TASK_TEST_
#define _U2_REALIGN_SEQUENCES_IN_ALIGNMENT_TASK_TEST_

#include <QDomElement>
#include <QFileInfo>

#include <U2Core/GObject.h>

#include <U2Test/XMLTestUtils.h>

namespace U2 {

class RealignSequencesInAlignmentTask;
class MultipleSequenceAlignmentObject;

class GTest_Realign : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_Realign, "realign");

    void prepare() override;
    Task::ReportResult report() override;
    void cleanup() override;

private:
    QString inputObjectName;
    MultipleSequenceAlignmentObject* msaObj = nullptr;
    QList<int> rowsIndexesToAlign;
    Document* doc = nullptr;
    RealignSequencesInAlignmentTask* realignTask = nullptr;
};

class RealignTests {
public:
    static QList<XMLTestFactory*> createTestFactories();
};

}  // namespace U2

#endif