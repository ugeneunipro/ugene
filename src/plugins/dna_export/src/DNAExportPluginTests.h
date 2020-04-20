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

#ifndef _U2_DNA_EXPORT_PLUGIN_TESTS_H_
#define _U2_DNA_EXPORT_PLUGIN_TESTS_H_


#include <U2Test/XMLTestUtils.h>
#include <QDomElement>
#include <U2Core/U2Region.h>
#include "ExportTasks.h"


namespace U2 {

class U2SequenceObject;
class MultipleSequenceAlignmentObject;

class GTest_ImportPhredQualityScoresTask : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_ImportPhredQualityScoresTask, "import-phred-qualities", TaskFlags_NR_FOSCOE);

    void prepare();

    QList<U2SequenceObject*> seqList;
    QStringList seqNameList;
    QString     fileName;

};

class GTest_ExportNucleicToAminoAlignmentTask : public XmlTest {
    Q_OBJECT


public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_ExportNucleicToAminoAlignmentTask, "export-nucleic-alignment", TaskFlags_NR_FOSCOE);

    void prepare();
    ReportResult report();
    QList<Task*> onSubTaskFinished(Task* subTask);

    int                 transTable;
    QString             inputFile;
    QString             outputFileName;
    QString             expectedOutputFile;
    U2Region             selectedRows;
    ExportMSA2MSATask*  exportTask;
    LoadDocumentTask*   resultLoadTask;
    MultipleSequenceAlignment          srcAl;
    MultipleSequenceAlignment          resAl;
};

class DNAExportPluginTests {
public:
    static QList<XMLTestFactory*> createTestFactories();
};

} // namespace U2

#endif
