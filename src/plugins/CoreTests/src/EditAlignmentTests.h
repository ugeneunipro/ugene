/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#pragma once

#include <QDomElement>

#include <U2Algorithm/CreateSubalignmentTask.h>

#include <U2Core/GUrl.h>
#include <U2Core/Msa.h>

#include <U2Test/GTest.h>
#include <U2Test/XMLTestUtils.h>

namespace U2 {

class U2SequenceObject;

class GTest_CreateSubalignimentTask : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_CreateSubalignimentTask, "plugin_create-subalignment-task", TaskFlags_NR_FOSCOE);
    void prepare() override;
    Task::ReportResult report() override;

private:
    MsaObject* maobj;
    MsaObject* expectedMaobj;
    QString docName;
    bool negativeTest;
    Task* t;
    QString expectedDocName;
    QStringList seqNames;
    U2Region window;
};

class GTest_RemoveAlignmentRegion : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_RemoveAlignmentRegion, "test-remove-alignment-region", TaskFlags_NR_FOSCOE);
    void prepare() override;
    Task::ReportResult report() override;

private:
    MsaObject* maobj;
    MsaObject* expectedMaobj;
    QString docName;
    QString expectedDocName;
    int startBase, startSeq, width, height;
};

class GTest_AddSequenceToAlignment : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_AddSequenceToAlignment, "test-add-seq-to-alignment", TaskFlags_NR_FOSCOE);
    void prepare() override;
    Task::ReportResult report() override;

private:
    MsaObject* maobj;
    MsaObject* expectedMaobj;
    QString docName;
    QString expectedDocName;
    QString seqFileName;
};

class GTest_RemoveColumnsOfGaps : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_RemoveColumnsOfGaps, "remove-columns-of-gaps");
    void prepare() override;

private:
    QString inputDocCtxName;
};

class CreateSubalignimentTests {
public:
    static QList<XMLTestFactory*> createTestFactories();
};

}  // namespace U2
