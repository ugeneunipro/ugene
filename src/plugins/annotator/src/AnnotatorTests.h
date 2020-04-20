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

#ifndef _U2_ANNOTATOR_TESTS_H_
#define _U2_ANNOTATOR_TESTS_H_

#include <QDomElement>

#include <U2Core/U2Region.h>

#include <U2Test/XMLTestUtils.h>

#include <U2View/AnnotatedDNAView.h>

#include "CollocationsDialogController.h"
#include "GeneByGeneReportTask.h"
#include "CustomPatternAnnotationTask.h"

namespace U2 {

class GTest_AnnotatorSearch : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_AnnotatorSearch, "plugin_dna-annotator-search");

    void prepare();
    Task::ReportResult report();
private:
    QString seqName;
    QString docName;
    QSet<QString> groupsToSearch;
    int regionSize;
    CollocationsAlgorithm::SearchType st;
    CollocationSearchTask *searchTask;
    QVector<U2Region> expectedResults;
};

class GTest_CustomAutoAnnotation : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_CustomAutoAnnotation, "custom-auto-annotation-search");

    void prepare();
    Task::ReportResult report();
private:
    QString seqName;
    QString docName;
    QString resultDocContextName;
    bool isCircular;
    CustomPatternAnnotationTask* searchTask;
};


class GTest_GeneByGeneApproach : public XmlTest{
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_GeneByGeneApproach, "plugin_dna-annotator-gene-by-gene");

    void prepare();
    Task::ReportResult report();
private:
    QString seqName;
    QString annName;
    QString docName;
    bool expected;
    float identity;
    GeneByGeneCompareResult result;
};

} //namespace U2

#endif
