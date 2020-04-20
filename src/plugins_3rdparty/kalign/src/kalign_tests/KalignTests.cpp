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

#include <QDir>
#include <QtMath>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/U2SafePoints.h>

#include "KalignConstants.h"
#include "KalignTask.h"
#include "KalignTests.h"

namespace U2 {

extern double QScore(const MultipleSequenceAlignment& maTest, const MultipleSequenceAlignment& maRef, TaskStateInfo& ti);

#define OUT_FILE_NAME_ATTR "out"
#define IN_FILE_NAME_ATTR "in"
#define QSCORE_ATTR "qscr"
#define QSCORE_DELTA_ATTR "dqscr"
#define INDEX_ATTR "index"
#define DOC1_ATTR "doc1"
#define DOC2_ATTR "doc2"
#define IN_DIR_ATTR "indir"
#define PAT_DIR_ATTR "refdir"
#define PARALLEL_FLAG_ATTR "parallel"
#define MAX_ITERS_ATTR "maxiters"
#define REFINE_ONLY_ATTR "refine"
#define REGION_ATTR "region"
#define STABLE_ATTR "stable"
#define MACHINE_PATH "MACHINE"

struct GTestBoolProperty {
    static bool get(QString attr, bool &value, const QDomElement &el) {
        QString value_str = el.attribute(attr);
        if(!value_str.isEmpty()) {
            bool ok = false;
            value = value_str.toInt(&ok);
            return ok;
        }
        return true;
    }
};

#define GET_BOOL_PROP(ATTR,VAL) if(GTestBoolProperty::get((ATTR),(VAL),(el))==false) {\
failMissingValue((ATTR));\
return;}

Kalign_Load_Align_Compare_Task::Kalign_Load_Align_Compare_Task( QString inFileURL, QString patFileURL,
    KalignTaskSettings& _config, QString _name)
    : Task(_name, TaskFlags_FOSCOE), str_inFileURL(inFileURL), str_patFileURL(patFileURL),kalignTask(NULL), config(_config)
{
    //QFileInfo fInf(inFileURL);
    //setTaskName("Kalign_Load_Align_Compare_Task: " + fInf.fileName());
    setUseDescriptionFromSubtask(true);
    stateInfo.progress = 0;
    loadTask1 = NULL;
    loadTask2 = NULL;
    kalignTask = NULL;
    ma1 = NULL;
    ma2 = NULL;
}

void Kalign_Load_Align_Compare_Task::prepare() {
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(str_inFileURL));
    loadTask1 = new LoadDocumentTask(BaseDocumentFormats::FASTA,str_inFileURL,iof);
    loadTask1->setSubtaskProgressWeight(0);
    addSubTask(loadTask1);
    iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(str_patFileURL));
    loadTask2 = new LoadDocumentTask(BaseDocumentFormats::FASTA,str_patFileURL,iof);
    addSubTask(loadTask2);
    loadTask1->setSubtaskProgressWeight(0);
}

MultipleSequenceAlignment Kalign_Load_Align_Compare_Task::dna_to_ma(QList<GObject*> dnaSeqs) {

    int seqCount = dnaSeqs.count();
    U2SequenceObject *seq = qobject_cast<U2SequenceObject *>(dnaSeqs[0]);
    MultipleSequenceAlignment ma("Alignment", seq->getAlphabet());
    for(int i=0; i<seqCount; i++) {
        seq = qobject_cast<U2SequenceObject *>(dnaSeqs[i]);
        if(seq == NULL) {
            stateInfo.setError(  QString("Can't cast GObject to U2SequenceObject") );
            return ma;
        }
        QByteArray seqData = seq->getWholeSequenceData(stateInfo);
        SAFE_POINT_OP(stateInfo, MultipleSequenceAlignment());
        ma->addRow(seq->getSequenceName(), seqData);
    }
    return ma;
}

QList<Task*> Kalign_Load_Align_Compare_Task::onSubTaskFinished(Task* subTask) {
    Q_UNUSED(subTask);
    QList<Task*> res;
    if (hasError() || isCanceled()) {
        return res;
    }

    if (subTask == loadTask1) {
        Document *doc = loadTask1->getDocument();
        if(loadTask1->hasError()) {
            return res;
        }
        assert(doc!=NULL);

        QList<GObject*> list = doc->findGObjectByType(GObjectTypes::SEQUENCE);

        if (list.size() == 0) {
            stateInfo.setError(  QString("container of object with type \"%1\" is empty").arg(GObjectTypes::SEQUENCE) );
            return res;
        }

        MultipleSequenceAlignment malign = dna_to_ma(list);
        if(hasError()) {
            return res;
        }

        ma1 = MultipleSequenceAlignmentImporter::createAlignment(doc->getDbiRef(), malign, stateInfo);
        CHECK_OP(stateInfo, res);

        if(ma1 == NULL){
            stateInfo.setError(  QString("can't convert dna sequences to MultipleSequenceAlignment") );
            return res;
        }


        res << kalignTask;
        this->connect(kalignTask,SIGNAL(si_progressChanged()),SLOT(sl_kalignProgressChg()));
    }
    else if (subTask == kalignTask) {
        if(kalignTask->hasError()) {
            setError( kalignTask->getError() );
            return res;
        }
        KalignTask * localKalign = qobject_cast<KalignTask*>( subTask );
        assert( NULL != localKalign );
        ma1->updateGapModel(localKalign->resultMA->getMsaRows());
    }
    else if (subTask == loadTask2) {
        if (loadTask2->hasError()) {
            return res;
        }
        Document *doc = loadTask2->getDocument();
        if(loadTask2->hasError()) {
            return res;
        }
        assert(doc!=NULL);

        QList<GObject*> list = doc->findGObjectByType(GObjectTypes::SEQUENCE);

        if (list.size() == 0) {
            stateInfo.setError(  QString("container of object with type \"%1\" is empty").arg(GObjectTypes::SEQUENCE) );
            return res;
        }

        MultipleSequenceAlignment malign = dna_to_ma(list);
        if(hasError()) {
            return res;
        }

        ma2 = MultipleSequenceAlignmentImporter::createAlignment(doc->getDbiRef(), malign, stateInfo);
        CHECK_OP(stateInfo, res);

        if(ma2 == NULL){
            stateInfo.setError(  QString("can't convert dna sequences to MultipleSequenceAlignment") );
            return res;
        }
    }
    return res;
}

void Kalign_Load_Align_Compare_Task::run() {

    const QList<MultipleSequenceAlignmentRow> alignedSeqs1 = ma1->getMsa()->getMsaRows();
    const QList<MultipleSequenceAlignmentRow> alignedSeqs2 = ma2->getMsa()->getMsaRows();

    foreach(const MultipleSequenceAlignmentRow &maItem1, alignedSeqs1) {
        bool nameFound = false;
        foreach(const MultipleSequenceAlignmentRow &maItem2, alignedSeqs2) {
            if (maItem1->getName() == maItem2->getName()) {
                nameFound = true;
                if(maItem2->getCoreEnd() != maItem1->getCoreEnd()) {
                    stateInfo.setError(  QString("Aligned sequences \"%1\" length not matched \"%2\", expected \"%3\"").arg(maItem1->getName()).arg(maItem1->getCoreEnd()).arg(maItem2->getCoreEnd()) );
                    return;
                }
                if (maItem1 != maItem2) {
                    stateInfo.setError(  QString("Aligned sequences \"%1\" not matched \"%2\", expected \"%3\"").arg(maItem1->getName()).arg(QString(maItem1->getCore())).arg(QString(maItem2->getCore())) );
                    return;
                }
            }
        }
        if (!nameFound) {
            stateInfo.setError(  QString("aligned sequence not found \"%1\"").arg(maItem1->getName()) );
        }
    }
}

void Kalign_Load_Align_Compare_Task::cleanup() {
    delete ma1;
    delete ma2;
    ma1 = NULL;
    ma2 = NULL;
}

Task::ReportResult Kalign_Load_Align_Compare_Task::report() {
    propagateSubtaskError();
    if(hasError()) {
        stateInfo.setError(  QString("input file \"%1\", pattern file \"%2\":\n").arg(str_inFileURL).arg(str_patFileURL) + stateInfo.getError() );
    }
    return ReportResult_Finished;
}

void GTest_Kalign_Load_Align_Compare::init(XMLTestFormat*, const QDomElement& el) {
    inFileURL = el.attribute(IN_FILE_NAME_ATTR);
    if (inFileURL.isEmpty()) {
        failMissingValue(IN_FILE_NAME_ATTR);
        return;
    }
    patFileURL = el.attribute(OUT_FILE_NAME_ATTR);
    if (inFileURL.isEmpty()) {
        failMissingValue(OUT_FILE_NAME_ATTR);
        return;
    }
}

void GTest_Kalign_Load_Align_Compare::prepare() {

    KalignTaskSettings mSettings;
    bool ok = false;
    QFileInfo inFile(env->getVar("COMMON_DATA_DIR")+"/"+inFileURL);
    if(!inFile.exists()) {
        stateInfo.setError(  QString("file not exist %1").arg(inFile.absoluteFilePath()) );
        return;
    }
    QFileInfo patFile(env->getVar("COMMON_DATA_DIR")+"/"+patFileURL);
    if(!patFile.exists()) {
        stateInfo.setError(  QString("file not exist %1").arg(patFile.absoluteFilePath()) );
        return;
    }

    worker = new Kalign_Load_Align_Compare_Task(inFile.absoluteFilePath(),patFile.absoluteFilePath(),mSettings,inFile.fileName());
    addSubTask(worker);
}

Task::ReportResult GTest_Kalign_Load_Align_Compare::report() {
    propagateSubtaskError();
    return ReportResult_Finished;
}

GTest_Kalign_Load_Align_Compare::~GTest_Kalign_Load_Align_Compare() {
}

/************************************************************************/
/* Kalign load align qscore                                             */
/************************************************************************/

void GTest_Kalign_Load_Align_QScore::init(XMLTestFormat*, const QDomElement& el) {
    inFileURL = el.attribute(IN_FILE_NAME_ATTR);
    if (inFileURL.isEmpty()) {
        failMissingValue(IN_FILE_NAME_ATTR);
        return;
    }
    patFileURL = el.attribute(OUT_FILE_NAME_ATTR);
    if (patFileURL.isEmpty()) {
        failMissingValue(OUT_FILE_NAME_ATTR);
        return;
    }

    QString str_qscore = el.attribute(QSCORE_ATTR);
    if(str_qscore.isEmpty()) {
        failMissingValue(QSCORE_ATTR);
        return;
    }
    bool ok = false;
    qscore = str_qscore.toFloat(&ok);
    if(!ok) {
        failMissingValue(QSCORE_ATTR);
        return;
    }

    this->dqscore = 0.01;

    QString str_dqscore = el.attribute(QSCORE_DELTA_ATTR);
    if(!str_dqscore.isEmpty()) {
        bool ok = false;
        dqscore = str_dqscore.toFloat(&ok);
        if(!ok) {
            failMissingValue(QSCORE_DELTA_ATTR);
            return;
        }
    }
    setUseDescriptionFromSubtask(true);
    stateInfo.progress = 0;
    loadTask1 = NULL;
    loadTask2 = NULL;
    kalignTask = NULL;
    ma1 = NULL;
    ma2 = NULL;
}

void GTest_Kalign_Load_Align_QScore::prepare() {

    bool ok = false;
    QFileInfo inFile(env->getVar("COMMON_DATA_DIR")+"/"+inFileURL);
    if(!inFile.exists()) {
        stateInfo.setError(  QString("file not exist %1").arg(inFile.absoluteFilePath()) );
        return;
    }
    QFileInfo patFile(env->getVar("COMMON_DATA_DIR")+"/"+patFileURL);
    if(!patFile.exists()) {
        stateInfo.setError(  QString("file not exist %1").arg(patFile.absoluteFilePath()) );
        return;
    }

    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(env->getVar("COMMON_DATA_DIR")+"/"+inFileURL));
    loadTask1 = new LoadDocumentTask(BaseDocumentFormats::FASTA,env->getVar("COMMON_DATA_DIR")+"/"+inFileURL,iof);
    loadTask1->setSubtaskProgressWeight(0);
    addSubTask(loadTask1);
    iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(env->getVar("COMMON_DATA_DIR")+"/"+patFileURL));

    loadTask2 = new LoadDocumentTask(BaseDocumentFormats::FASTA,env->getVar("COMMON_DATA_DIR")+"/"+patFileURL,iof);

    addSubTask(loadTask2);
    loadTask1->setSubtaskProgressWeight(0);
}

MultipleSequenceAlignment GTest_Kalign_Load_Align_QScore::dna_to_ma(QList<GObject*> dnaSeqs) {

    int seqCount = dnaSeqs.count();
    U2SequenceObject *seq = qobject_cast<U2SequenceObject *>(dnaSeqs[0]);
    MultipleSequenceAlignment ma("Alignment", seq->getAlphabet());
    for(int i=0; i<seqCount; i++) {
        seq = qobject_cast<U2SequenceObject *>(dnaSeqs[i]);
        if(seq == NULL) {
            stateInfo.setError(  QString("Can't cast GObject to U2SequenceObject") );
            return ma;
        }
        QByteArray seqData = seq->getWholeSequenceData(stateInfo);
        SAFE_POINT_OP(stateInfo, MultipleSequenceAlignment());
        ma->addRow(seq->getSequenceName(), seqData);
    }
    return ma;
}

QList<Task*> GTest_Kalign_Load_Align_QScore::onSubTaskFinished(Task* subTask) {
    Q_UNUSED(subTask);
    QList<Task*> res;
    if (hasError() || isCanceled()) {
        return res;
    }

    if (subTask == loadTask1) {
        Document *doc = loadTask1->getDocument();
        if(loadTask1->hasError()) {
            return res;
        }
        assert(doc!=NULL);

        QList<GObject*> list = doc->findGObjectByType(GObjectTypes::SEQUENCE);

        if (list.size() == 0) {
            stateInfo.setError(  QString("container of object with type \"%1\" is empty").arg(GObjectTypes::SEQUENCE) );
            return res;
        }

        MultipleSequenceAlignment malign = dna_to_ma(list);
        if(hasError()) {
            return res;
        }

        ma1 = MultipleSequenceAlignmentImporter::createAlignment(doc->getDbiRef(), malign, stateInfo);
        CHECK_OP(stateInfo, res);

        if(ma1 == NULL){
            stateInfo.setError(  QString("can't convert dna sequences to MultipleSequenceAlignment") );
            return res;
        }

        kalignTask = new KalignTask(ma1->getMultipleAlignment(), config);
        res << kalignTask;
        this->connect(kalignTask,SIGNAL(si_progressChanged()),SLOT(sl_kalignProgressChg()));
    }
    else if (subTask == kalignTask) {
        if(kalignTask->hasError()) {
            setError( kalignTask->getError() );
            return res;
        }
        KalignTask * localKalign = qobject_cast<KalignTask*>( subTask );
        assert( NULL != localKalign );
        ma1->updateGapModel(localKalign->resultMA->getMsaRows());
    }
    else if (subTask == loadTask2) {
        if (loadTask2->hasError()) {
            return res;
        }
        Document *doc = loadTask2->getDocument();
        if(loadTask2->hasError()) {
            return res;
        }
        assert(doc!=NULL);

        QList<GObject*> list = doc->findGObjectByType(GObjectTypes::SEQUENCE);

        if (list.size() == 0) {
            stateInfo.setError(  QString("container of object with type \"%1\" is empty").arg(GObjectTypes::SEQUENCE) );
            return res;
        }

        MultipleSequenceAlignment malign = dna_to_ma(list);
        if(hasError()) {
            return res;
        }

        ma2 = MultipleSequenceAlignmentImporter::createAlignment(doc->getDbiRef(), malign, stateInfo);
        CHECK_OP(stateInfo, res);

        if(ma2 == NULL){
            stateInfo.setError(  QString("can't convert dna sequences to MultipleSequenceAlignment") );
            return res;
        }
    }
    return res;
}

void GTest_Kalign_Load_Align_QScore::run() {
    double qscore = QScore(ma1->getMultipleAlignment(), ma2->getMultipleAlignment(), stateInfo);
    if (stateInfo.hasError()) {
        return;
    }

    bool match = fabsl (this->qscore - qscore) < dqscore;

    if (!match) {
        stateInfo.setError(  QString("qscore not matched: %1, expected %2").arg(qscore).arg(this->qscore));
    }
}

Task::ReportResult GTest_Kalign_Load_Align_QScore::report() {
    propagateSubtaskError();
    return ReportResult_Finished;
}

GTest_Kalign_Load_Align_QScore::~GTest_Kalign_Load_Align_QScore() {
}


QList<XMLTestFactory*> KalignTests::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(GTest_Kalign_Load_Align_Compare::createFactory());
    res.append(GTest_Kalign_Load_Align_QScore::createFactory());
    return res;
}

}//namespace
