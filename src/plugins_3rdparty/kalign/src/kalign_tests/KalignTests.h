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

#ifndef _U2_KALIGN_TESTS_H_
#define _U2_KALIGN_TESTS_H_

#include <QDomElement>
#include <QFileInfo>

#include <U2Core/GObject.h>

#include <U2Test/XMLTestUtils.h>

#include "KalignTask.h"

namespace U2 {

class KalignGObjectTask;
class LoadDocumentTask;
class MultipleSequenceAlignmentObject;

class  Kalign_Load_Align_Compare_Task : public Task {
	Q_OBJECT
public:
	Kalign_Load_Align_Compare_Task(QString inFileURL, QString patFileURL, KalignTaskSettings& config, 
		QString _name = QString("Kalign_Load_Align_Compare_Task"));
	~Kalign_Load_Align_Compare_Task() {cleanup();}
	void prepare();
	void run();
	QList<Task*> onSubTaskFinished(Task* subTask);
	ReportResult report();
	void cleanup();

	public slots:
		void sl_kalignProgressChg() {stateInfo.progress = kalignTask->getProgress();}

private:
	MultipleSequenceAlignment dna_to_ma(QList<GObject*> dnaSeqs);
	QString str_inFileURL;
	QString str_patFileURL;
	LoadDocumentTask*           loadTask1;
	LoadDocumentTask*           loadTask2;
	Task*                       kalignTask; 
	KalignTaskSettings          config;
	MultipleSequenceAlignmentObject*           ma1;
	MultipleSequenceAlignmentObject*           ma2;
};

class GTest_Kalign_Load_Align_Compare: public XmlTest {
	Q_OBJECT
public:
	SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_Kalign_Load_Align_Compare, "kalign-load-align-compare");
	~GTest_Kalign_Load_Align_Compare();
	void prepare();
	Task::ReportResult report();
private:
	QString inFileURL;
	QString patFileURL;
	Kalign_Load_Align_Compare_Task *worker;
};

class GTest_Kalign_Load_Align_QScore: public XmlTest {
	Q_OBJECT
public:
	SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_Kalign_Load_Align_QScore, "kalign-load-align-qscore", TaskFlags_FOSCOE)
		//SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_Kalign_Load_Align_QScore, "kalign-load-align-qscore");
		~GTest_Kalign_Load_Align_QScore();
	void prepare();
	Task::ReportResult report();
	void run();
	QList<Task*> onSubTaskFinished(Task* subTask);
	MultipleSequenceAlignment dna_to_ma(QList<GObject*> dnaSeqs);

	public slots:
		void sl_kalignProgressChg() {stateInfo.progress = kalignTask->getProgress();}

private:
	QString inFileURL;
	QString patFileURL;
	double qscore;
	double dqscore;
	LoadDocumentTask*           loadTask1;
	LoadDocumentTask*           loadTask2;
	Task*                       kalignTask; 
	KalignTaskSettings          config;
	MultipleSequenceAlignmentObject*           ma1;
	MultipleSequenceAlignmentObject*           ma2;
};

class KalignTests {
public:
	static QList<XMLTestFactory*> createTestFactories();
};
}//namespace
#endif

