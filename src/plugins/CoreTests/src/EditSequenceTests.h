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
#include <QObject>

#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObject.h>
#include <U2Core/ModifySequenceObjectTask.h>
#include <U2Core/U2Region.h>
#include <U2Core/global.h>

#include <U2Test/XMLTestUtils.h>

namespace U2 {

class GTest_RemovePartFromSequenceTask : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_RemovePartFromSequenceTask, "Remove_Part_From_Sequence_Task", TaskFlags_NR_FOSCOE);

    ~GTest_RemovePartFromSequenceTask();

    void prepare() override;
    Task::ReportResult report() override;

private:
    U1AnnotationUtils::AnnotationStrategyForResize strat;
    QString docName;
    QString seqName;
    int startPos;
    int length;
    QString annotationName;
    QString expectedSequence;
    QVector<U2Region> expectedRegions;
    U2SequenceObject* dnaso;
};

class GTest_AddPartToSequenceTask : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_AddPartToSequenceTask, "Add_Part_To_Sequence_Task", TaskFlags_NR_FOSCOE);

    ~GTest_AddPartToSequenceTask();
    void prepare() override;
    Task::ReportResult report() override;

private:
    U1AnnotationUtils::AnnotationStrategyForResize strat;
    QString docName;
    QString seqName;
    int startPos;
    QString insertedSequence;
    QString annotationName;
    QString expectedSequence;
    QVector<U2Region> expectedRegions;
    U2SequenceObject* dnaso;
};

class GTest_ReplacePartOfSequenceTask : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_ReplacePartOfSequenceTask, "replace-part-of-sequence", TaskFlags_NR_FOSCOE);

    ~GTest_ReplacePartOfSequenceTask() {};
    void prepare() override;
    Task::ReportResult report() override;

private:
    U1AnnotationUtils::AnnotationStrategyForResize strat;
    QString docName;
    QString seqName;
    int startPos;
    int length;
    QString insertedSequence;
    QString annotationName;
    QString expectedSequence;
    QVector<U2Region> expectedRegions;
    U2SequenceObject* dnaso;
};

class EditSequenceTests {
public:
    static QList<XMLTestFactory*> createTestFactories();
};
}  // namespace U2
