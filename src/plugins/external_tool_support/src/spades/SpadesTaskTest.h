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

#ifndef _U2_SPADES_TEST_TASK_
#define _U2_SPADES_TEST_TASK_

#include <U2Algorithm/GenomeAssemblyRegistry.h>

#include <U2Test/GTest.h>
#include <U2Test/XMLTestFormat.h>
#include <U2Test/XMLTestUtils.h>

#include "SpadesTask.h"

namespace U2 {

class OutputCollector;

class GTest_SpadesTaskTest : public XmlTest {
    Q_OBJECT
public:
    static const QString SEQUENCING_PLATFORM;

    static const QString PAIRED_END_READS;
    static const QString PAIRED_END_READS_ORIENTATION;
    static const QString PAIRED_END_READS_TYPE;

    static const QString HIGH_QUALITY_MATE_PAIRS;
    static const QString HIGH_QUALITY_MATE_PAIRS_ORIENTATION;
    static const QString HIGH_QUALITY_MATE_PAIRS_TYPE;

    static const QString UNPAIRED_READS;
    static const QString PACBIO_CCS_READS;

    static const QString MATE_PAIRS;
    static const QString MATE_PAIRS_ORIENTATION;
    static const QString MATE_PAIRS_TYPE;

    static const QString PACBIO_CLR_READS;
    static const QString OXFORD_NANOPORE_READS;
    static const QString SANGER_READS;
    static const QString TRUSTED_CONTIGS;
    static const QString UNTRUSTED_CONTIGS;

    static const QString DESIRED_PARAMETERS;
    static const QString OUTPUT_DIR;
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_SpadesTaskTest, "spades-task-input-type", TaskFlags(TaskFlag_NoRun) | TaskFlag_FailOnSubtaskCancel);
    //SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_SpadesTaskTest, "spades-task-input-type");

    void prepare();
    QList<Task*> onSubTaskFinished(Task* subTask);

private:
    void setAndCheckArgs();

    GenomeAssemblyTaskSettings taskSettings;
    SpadesTask *spadesTask;
    OutputCollector *collector;
    QStringList desiredParameters;
};

class GTest_CheckYAMLFile : public XmlTest {
    Q_OBJECT
public:
    static const QString STRINGS_TO_CHECK;
    static const QString INPUT_DIR;

    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_CheckYAMLFile, "check-yaml-file");

    void prepare();
private:
    QStringList desiredStrings;
    QString fileToCheck;
};

class SpadesTaskTest {
public:
    static QList<XMLTestFactory*> createTestFactories();
};

}
#endif
