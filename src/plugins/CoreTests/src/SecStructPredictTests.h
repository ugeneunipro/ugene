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

#ifndef _U2_SEC_STRUCT_PREDICT_TESTS_H_
#define _U2_SEC_STRUCT_PREDICT_TESTS_H_

#include <U2Test/XMLTestUtils.h>


namespace U2 {

class SecStructPredictTask;
class AnnotationTableObject;

class GTest_SecStructPredictAlgorithm : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_SecStructPredictAlgorithm, "test-sec-struct-predict-algorithm");
    void prepare();
    Task::ReportResult report();
private:
    SecStructPredictTask* task;
    QString algName;
    QString inputSeq;
    QString outputSeq;
};


class GTest_SecStructPredictTask : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_SecStructPredictTask, "predict-sec-structure-and-save-results");
    void prepare();
    void cleanup();
    Task::ReportResult report();
private:
    bool contextAdded;
    SecStructPredictTask* task;
    AnnotationTableObject* aObj;
    QString seqName;
    QString algName;
    QString resultsTableContextName;
};


class SecStructPredictTests {
public:
    static QList<XMLTestFactory*> createTestFactories();
};

} //namespace
#endif

