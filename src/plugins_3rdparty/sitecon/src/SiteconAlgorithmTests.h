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

#ifndef _U2_SITECON_ALG_TEST_H_
#define _U2_SITECON_ALG_TEST_H_

#include "SiteconAlgorithm.h"
#include "SiteconSearchTask.h"
#include <U2Test/XMLTestUtils.h>
#include <U2Core/GObject.h>
#include <U2Core/MultipleSequenceAlignment.h>


namespace U2 {

class GTest_CalculateACGTContent : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_CalculateACGTContent, "sitecon-calculateACGTContent", TaskFlags_FOSCOE);

    void prepare();
    void run();
    Task::ReportResult report();
private:
    QString docName;
    SiteconBuildSettings s;
    MultipleSequenceAlignment ma;
    int expectedACGT[4];
};

class GTest_CalculateDispersionAndAverage : public XmlTest {
    Q_OBJECT
    typedef QVector<int> ResultVector;
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_CalculateDispersionAndAverage, "sitecon-calculateDispersionAndAverage", TaskFlags_FOSCOE);

    void prepare();
    void run();
    Task::ReportResult report();
private:
    QString docName;
    SiteconBuildSettings s;
    MultipleSequenceAlignment ma;
    QVector<PositionStats> result;
    QVector<ResultVector> expectedResults;    
};

class GTest_CalculateFirstTypeError : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_CalculateFirstTypeError, "sitecon-calculateFirstTypeError", TaskFlags_FOSCOE);

    void prepare();
    void run();
    Task::ReportResult report();
private:
    QString docName;
    int offset;
    SiteconBuildSettings s;
    MultipleSequenceAlignment ma;
    QVector<float> result;
    QVector<int> expectedResult;    
};

class GTest_CalculateSecondTypeError : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_CalculateSecondTypeError, "sitecon-calculateSecondTypeError", TaskFlags_FOSCOE);

    void prepare();
    void run();
    Task::ReportResult report();
private:
    QString docName;
    int offset;
    SiteconBuildSettings s;
    MultipleSequenceAlignment ma;
    QVector<float> result;
    QVector<int> expectedResult;    
};

class GTest_SiteconSearchTask : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_SiteconSearchTask, "sitecon-search_task");

    void init();
    void prepare();
    Task::ReportResult report();
private:
    
    SiteconSearchTask *task;
    QString seqName;
    int tresh;
    SiteconModel model;
    bool isNeedCompliment;
    bool complOnly;
    QByteArray seqData;
    QList<SiteconSearchResult>  results;
    QList<SiteconSearchResult>  expectedResults;
};


class GTest_CompareSiteconModels: public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_CompareSiteconModels, "compare-sitecon-models");
    Task::ReportResult report();

public:
    QString             doc1ContextName;
    QString             doc2ContextName;

};

}//namespace
#endif
