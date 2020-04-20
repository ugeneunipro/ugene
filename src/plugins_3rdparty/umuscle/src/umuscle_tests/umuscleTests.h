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

#ifndef _U2_MUSCLE_TESTS_H_
#define _U2_MUSCLE_TESTS_H_

#include <QDomElement>
#include <QFileInfo>

#include <U2Core/GObject.h>

#include <U2Test/XMLTestUtils.h>

#include "MuscleTask.h"

namespace U2 {

class MultipleSequenceAlignmentObject;
class MuscleGObjectTask;
class LoadDocumentTask;

class GTest_uMuscle : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_uMuscle, "umuscle");
    
    void prepare();
    Task::ReportResult report();
    void cleanup();

private:    
    QString inputDocCtxName;
    QString resultCtxName;
    Document* doc;
    MuscleGObjectTask* mTask;
    MultipleSequenceAlignmentObject *ma_result;
    bool ctxAdded;
    bool refineOnly;
    int maxIters;
    bool alignRegion;
    U2Region region;
    bool stable;
};

class GTest_CompareMAlignment : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_CompareMAlignment, "compare-malignment");

    Task::ReportResult report();

private:    
    QString doc1CtxName;
    QString doc2CtxName;
};

class GTest_uMuscleAddUnalignedSequenceToProfile: public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_uMuscleAddUnalignedSequenceToProfile, "umuscle-add-unaligned-seq-to-profile");

    void prepare();
    Task::ReportResult report();

private:    
    QString                 aliDocName;
    QString                 seqDocName;
    int                     resultAliLen;
    QList< QList<int> >     gapPositionsForSeqs;
    MultipleSequenceAlignmentObject*       aliObj;
    int                     origAliSeqs;
    int                     resultAliSeqs;
};

class GTest_Muscle_Load_Align_QScore: public XmlTest {
    Q_OBJECT
public:
	SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_Muscle_Load_Align_QScore, "umuscle-load-align-qscore", TaskFlags_FOSCOE)
    //SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_Muscle_Load_Align_QScore, "umuscle-load-align-qscore");
    ~GTest_Muscle_Load_Align_QScore();
    void prepare();
    Task::ReportResult report();
	void run();
	QList<Task*> onSubTaskFinished(Task* subTask);
	MultipleSequenceAlignment dna_to_ma(QList<GObject*> dnaSeqs);

public slots:
		void sl_muscleProgressChg() {stateInfo.progress = muscleTask->getProgress();}

private:
    QString inFileURL;
    QString patFileURL;
	double qscore;
	double dqscore;
	LoadDocumentTask*           loadTask1;
	LoadDocumentTask*           loadTask2;
	Task*                       muscleTask; 
	MuscleTaskSettings          config;
	MultipleSequenceAlignmentObject*           ma1;
	MultipleSequenceAlignmentObject*           ma2;
};

class  Muscle_Load_Align_Compare_Task : public Task {
    Q_OBJECT
public:
    Muscle_Load_Align_Compare_Task(QString inFileURL, QString patFileURL, MuscleTaskSettings& config, 
        QString _name = QString("Muscle_Load_Align_Compare_Task"));
    ~Muscle_Load_Align_Compare_Task() {cleanup();}
    void prepare();
    void run();
    QList<Task*> onSubTaskFinished(Task* subTask);
    ReportResult report();
    void cleanup();

public slots:
    void sl_muscleProgressChg() {stateInfo.progress = muscleTask->getProgress();}

private:
    MultipleSequenceAlignment dna_to_ma(QList<GObject*> dnaSeqs);
    QString str_inFileURL;
    QString str_patFileURL;
    LoadDocumentTask*           loadTask1;
    LoadDocumentTask*           loadTask2;
    Task*                       muscleTask; 
    MuscleTaskSettings          config;
    MultipleSequenceAlignmentObject*           ma1;
    MultipleSequenceAlignmentObject*           ma2;
};

class GTest_Muscle_Load_Align_Compare: public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_Muscle_Load_Align_Compare, "umuscle-load-align-compare");
    ~GTest_Muscle_Load_Align_Compare();
    void prepare();
    Task::ReportResult report();
private:
    QString inFileURL;
    QString patFileURL;
    Muscle_Load_Align_Compare_Task *worker;
};

class GTest_uMusclePacketTest: public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_uMusclePacketTest, "umuscle-packet-test");

    void prepare();
    Task::ReportResult report();

private:    
    QString                 inDirName;
    QString                 patDirName;
    QTime                   timer;
};

class UMUSCLETests {
public:
    static QList<XMLTestFactory*> createTestFactories();
};
}//namespace
#endif
