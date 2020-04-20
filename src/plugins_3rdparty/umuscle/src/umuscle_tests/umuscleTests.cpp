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
#include <U2Core/Log.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "umuscleTests.h"
#include "MuscleConstants.h"
#include "MuscleTask.h"
#include "MuscleParallel.h"

/* TRANSLATOR U2::GTest*/

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
#define ENV_MUSCLE_N_THREADS "MUSCLE_N_THREADS"
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

void GTest_uMuscle::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    ctxAdded = false;
    ma_result = NULL;
    refineOnly = false;
    maxIters = -1;
    alignRegion = false;
    stable = false;

    inputDocCtxName = el.attribute(IN_FILE_NAME_ATTR);
    if (inputDocCtxName.isEmpty()) {
        failMissingValue(IN_FILE_NAME_ATTR);
        return;
    }

    QString refineOnly_str = el.attribute(REFINE_ONLY_ATTR);
    if (!refineOnly_str.isEmpty()) {
        bool ok = false;
        refineOnly = refineOnly_str.toInt(&ok);
        if (!ok) {
            failMissingValue(REFINE_ONLY_ATTR);
            return;
        }
    }

    QString maxIters_str = el.attribute(MAX_ITERS_ATTR);
    if(!maxIters_str.isEmpty()) {
        bool ok = false;
        maxIters = maxIters_str.toInt(&ok);
        if (!ok) {
            failMissingValue(MAX_ITERS_ATTR);
            return;
        }
    }

    QString region_str = el.attribute(REGION_ATTR);
    if(!region_str.isEmpty()) {
        QRegExp rx("([0123456789]+)..([0123456789]+)");
        if(rx.indexIn(region_str, 0) != -1) {
            bool ok1,ok2;
            int start = rx.cap(1).toInt(&ok1) - 1;
            int end = rx.cap(2).toInt(&ok2);
            if (ok1 && ok2) {
                region = U2Region(start, end - start);
                alignRegion = true;
            } else {
                failMissingValue(REGION_ATTR);
            }
        } else {
            failMissingValue(REGION_ATTR);
        }
    }

    GET_BOOL_PROP(STABLE_ATTR,stable)

    resultCtxName = el.attribute(INDEX_ATTR);
}

void GTest_uMuscle::prepare() {
    mTask = NULL;
    ma_result = NULL;

    doc = getContext<Document>(this, inputDocCtxName);
    if (doc == NULL) {
        stateInfo.setError(  QString("context not found %1").arg(inputDocCtxName) );
        return;
    }

    QList<GObject*> list = doc->findGObjectByType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);
    if (list.size() == 0) {
        stateInfo.setError(  QString("container of object with type \"%1\" is empty").arg(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT) );
        return;
    }

    GObject *obj = list.first();
    if(obj==NULL){
        stateInfo.setError(  QString("object with type \"%1\" not found").arg(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT) );
        return;
    }
    assert(obj!=NULL);
    MultipleSequenceAlignmentObject* ma = qobject_cast<MultipleSequenceAlignmentObject*>(obj);
    if(ma==NULL){
        stateInfo.setError(  QString("error can't cast to multiple alignment from GObject") );
        return;
    }

    MuscleTaskSettings s;
    bool ok = false;
    s.nThreads = env->getVar(ENV_MUSCLE_N_THREADS).toInt(&ok);
    if(!ok) {
        stateInfo.setError(QString("Invalid test suite environment variable \"%1\"").arg(ENV_MUSCLE_N_THREADS));
        return;
    }
    if(maxIters != -1) {
        s.maxIterations = maxIters;
    }
    if(refineOnly) {
        s.op = MuscleTaskOp_Refine;
    }
    s.stableMode = stable; //default mode is 'group' like in MUSCLE
    s.alignRegion = alignRegion;
    if(alignRegion) {
        s.regionToAlign = region;
    }
    ma_result = ma;
    mTask = new MuscleGObjectTask(ma_result,s);
    addSubTask(mTask);
}

Task::ReportResult GTest_uMuscle::report() {
    if (!hasError()) {
        if(mTask->hasError()) {
            stateInfo.setError(  mTask->getError() );
            return ReportResult_Finished;
        }
        if(!resultCtxName.isEmpty()) {
            ctxAdded = true;
            addContext(resultCtxName, ma_result);
        }
    }
    return ReportResult_Finished;
}

void GTest_uMuscle::cleanup() {
    //if(ma_result!=NULL)
    //    delete ma_result;
    if(ctxAdded) {
        removeContext(resultCtxName);
    }

    XmlTest::cleanup();
}

void GTest_CompareMAlignment::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    doc1CtxName = el.attribute(DOC1_ATTR);
    if (doc1CtxName.isEmpty()) {
        failMissingValue(DOC1_ATTR);
        return;
    }
    doc2CtxName = el.attribute(DOC2_ATTR);
    if (doc2CtxName.isEmpty()) {
        failMissingValue(DOC2_ATTR);
        return;
    }
}

Task::ReportResult GTest_CompareMAlignment::report() {
    Document* doc1 = getContext<Document>(this, doc1CtxName);
    if (doc1 == NULL) {
        stateInfo.setError(  QString("document not found %1").arg(doc1CtxName) );
        return ReportResult_Finished;
    }
    Document* doc2 = getContext<Document>(this, doc2CtxName);
    if (doc2 == NULL) {
        stateInfo.setError(  QString("document not found %1").arg(doc2CtxName) );
        return ReportResult_Finished;
    }

    QList<GObject*> objs1 = doc1->findGObjectByType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);
    QList<GObject*> objs2 = doc2->findGObjectByType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);

    if(objs1.size()!=objs2.size()) {
        stateInfo.setError(  QString("MAlignmentObjects count not matched %1, expected %2").arg(objs1.size()).arg(objs2.size()) );
        return ReportResult_Finished;
    }

    int listSize = objs1.size();
    for (int i=0;i<listSize;i++) {
        MultipleSequenceAlignmentObject* ma1 = qobject_cast<MultipleSequenceAlignmentObject*>(objs1.at(i));
        MultipleSequenceAlignmentObject* ma2 = qobject_cast<MultipleSequenceAlignmentObject*>(objs2.at(i));
        const QList<MultipleSequenceAlignmentRow> alignedSeqs1 = ma1->getMsa()->getMsaRows();
        const QList<MultipleSequenceAlignmentRow> alignedSeqs2 = ma2->getMsa()->getMsaRows();
        if(ma1->objectName()!=ma2->objectName()) {
            stateInfo.setError(  QString("MAlignmentObjects name not matched \"%1\", expected \"%2\"").arg(ma1->objectName()).arg(ma2->objectName()) );
            return ReportResult_Finished;
        }
        foreach(const MultipleSequenceAlignmentRow &maItem1, alignedSeqs1) {
            bool nameFound = false;
            foreach(const MultipleSequenceAlignmentRow &maItem2, alignedSeqs2) {
                if (maItem1->getName() == maItem2->getName()) {
                    nameFound = true;
                    int l1 = maItem1->getCoreEnd();
                    int l2 = maItem2->getCoreEnd();
                    if(l1!=l2) {
                        stateInfo.setError(  QString("Aligned sequences \"%1\" length not matched \"%2\", expected \"%3\"").arg(maItem1->getName()).arg(l1).arg(l2) );
                        return ReportResult_Finished;
                    }
                    if (*maItem1 != *maItem2) {
                        stateInfo.setError(  QString("Aligned sequences \"%1\" not matched \"%2\", expected \"%3\"").arg(maItem1->getName()).arg(QString(maItem1->getCore())).arg(QString(maItem2->getCore())) );
                        return ReportResult_Finished;
                    }
                }
            }
            if (!nameFound) {
                stateInfo.setError(  QString("aligned sequence not found \"%1\"").arg(maItem1->getName()) );
            }

        }

    }
    return ReportResult_Finished;
}

void GTest_uMuscleAddUnalignedSequenceToProfile::init(XMLTestFormat *tf, const QDomElement& el) {
    origAliSeqs = 0;
    aliObj = NULL;
    resultAliSeqs = 0;
    aliDocName = el.attribute("ali-doc");
    if (aliDocName.isEmpty()) {
        stateInfo.setError(  QString("value not set %1").arg("ali-doc") );
        return;
    }
    seqDocName = el.attribute("seq-doc");
    if (seqDocName.isEmpty()) {
        stateInfo.setError(  QString("value not set %1").arg("seq-doc") );
        return;
    }
    QString gaps = el.attribute("gap-map");
    QStringList gapsPerSeq = gaps.split('|');
    //gapsPerSeq.removeAll(QString());
    foreach (const QString& s, gapsPerSeq) {
        QList<int> seqGaps;
        QStringList nums = s.split(',');
        foreach (const QString& n, nums) {
            if (n.isEmpty()) {
                continue;
            }
            bool ok = false;
            int gapPos = n.toInt(&ok);
            if (!ok) {
                stateInfo.setError(  QString("error parsing gap value '%1', line %2").arg(n).arg(s) );
                return;
            }
            seqGaps.append(gapPos);
        }
        gapPositionsForSeqs.append(seqGaps);
    }
    QString resultLen = el.attribute("result-ali-len");
    bool ok = false;
    resultAliLen = resultLen.toInt(&ok);
    if (!ok) {
        stateInfo.setError(  QString("error result-ali-len '%1'").arg(resultLen) );
        return;
    }
}

void GTest_uMuscleAddUnalignedSequenceToProfile::prepare() {
    if (hasError()) {
        return;
    }
    Document* aliDoc = getContext<Document>(this, aliDocName);
    if (aliDoc == NULL) {
        stateInfo.setError(  QString("alignment document not found in context: %1").arg(aliDocName) );
        return;
    }
    Document* seqDoc = getContext<Document>(this, seqDocName);
    if (seqDoc == NULL) {
        stateInfo.setError(  QString("sequence document not found in context: %1").arg(seqDocName) );
        return;
    }
    QList<GObject*> aliObjs = aliDoc->findGObjectByType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);
    if (aliObjs.isEmpty()) {
        stateInfo.setError(  QString("no alignment object found in doc: %1").arg(aliDoc->getURLString()) );
        return;
    }
    aliObj = qobject_cast<MultipleSequenceAlignmentObject*>(aliObjs[0]);
    origAliSeqs = aliObj->getNumRows();

    QList<GObject*> seqObjs = seqDoc->findGObjectByType(GObjectTypes::SEQUENCE);
    if (seqObjs.isEmpty()) {
        stateInfo.setError(  QString("no sequence objects found in doc: %1").arg(seqDoc->getURLString()) );
        return;
    }
    MultipleSequenceAlignment unalignedMA;
    unalignedMA->setAlphabet(aliObj->getAlphabet());
    foreach (GObject* obj, seqObjs) {
        U2SequenceObject* dnaObj = qobject_cast<U2SequenceObject*>(obj);
        QByteArray seqData = dnaObj->getWholeSequenceData(stateInfo);
        CHECK_OP(stateInfo, );
        unalignedMA->addRow(dnaObj->getSequenceName(), seqData);
    }
    if (unalignedMA->getNumRows()!=gapPositionsForSeqs.size()) {
        stateInfo.setError( QString("number of sequences not matches number of gaps in test: %1 sequences and %2 gap lines")
            .arg(unalignedMA->getNumRows()).arg(gapPositionsForSeqs.size()) );
        return;
    }
    resultAliSeqs = origAliSeqs + unalignedMA->getNumRows();

    MuscleTaskSettings s;
    s.op = MuscleTaskOp_AddUnalignedToProfile;
    s.profile = unalignedMA;
    bool ok = false;
    s.nThreads = env->getVar(ENV_MUSCLE_N_THREADS).toInt(&ok);
    if(!ok) {
        stateInfo.setError(QString("Invalid test suite environment variable \"%1\"").arg(ENV_MUSCLE_N_THREADS));
        return;
    }
    addSubTask(new MuscleGObjectTask(aliObj, s));
}

Task::ReportResult GTest_uMuscleAddUnalignedSequenceToProfile::report() {
    propagateSubtaskError();
    if (hasError()) {
        return ReportResult_Finished;
    }
    const MultipleSequenceAlignment msa = aliObj->getMultipleAlignment();
    if (msa->getLength()!=resultAliLen) {
        stateInfo.setError(  QString("result alignment length notmatches: %1, expected: %2").arg(msa->getLength()).arg(resultAliLen) );
        return ReportResult_Finished;
    }

    if (resultAliSeqs!=msa->getNumRows()) {
        stateInfo.setError(  QString("unexpected number of sequences in result: %1, expected: %2").arg(msa->getNumRows()).arg(resultAliSeqs) );
        return ReportResult_Finished;
    }

    U2OpStatus2Log os;
    for (int i = origAliSeqs, j = 0; i < msa->getNumRows(); i++, j++) {
        const MultipleSequenceAlignmentRow row = msa->getMsaRow(i);
        QByteArray seq = row->toByteArray(os, msa->getLength());
        QList<int> seqGaps = gapPositionsForSeqs[j];
        for (int pos = 0; pos < seq.size(); pos++) {
            char c = seq[pos];
            if (c == U2Msa::GAP_CHAR) {
                bool found = seqGaps.contains(pos);
                if (!found) {
                    stateInfo.setError(  QString("illegal gap found! pos: %1, sequence: %2").arg(pos).arg(row->getName()) );
                    return ReportResult_Finished;
                }
            }
        }
        for (int gap = 0; gap < seqGaps.size(); gap++) {
            int pos  = seqGaps[gap];
            char c = seq[pos];
            if (c != U2Msa::GAP_CHAR) {
                stateInfo.setError(  QString("gap not found! pos: %1, sequence: %2").arg(pos).arg(row->getName()) );
                return ReportResult_Finished;
            }
        }
    }
    return ReportResult_Finished;
}

void GTest_Muscle_Load_Align_QScore::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);
    inFileURL = el.attribute(IN_FILE_NAME_ATTR);
    stateInfo.progress = 0;
    loadTask1 = NULL;
    loadTask2 = NULL;
    muscleTask = NULL;
    ma1 = NULL;
    ma2 = NULL;

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
    if (str_qscore.isEmpty()) {
        failMissingValue(QSCORE_ATTR);
        return;
    }
    bool ok = false;
    qscore = str_qscore.toFloat(&ok);
    if (!ok) {
        failMissingValue(QSCORE_ATTR);
        return;
    }

    this->dqscore = 0.01;
    QString str_dqscore = el.attribute(QSCORE_DELTA_ATTR);
    if (!str_dqscore.isEmpty()) {
        bool ok = false;
        dqscore = str_dqscore.toFloat(&ok);
        if (!ok) {
            failMissingValue(QSCORE_DELTA_ATTR);
            return;
        }
    }
    setUseDescriptionFromSubtask(true);
}

void GTest_Muscle_Load_Align_QScore::prepare() {

    config.stableMode = false; //default mode is 'group' like in MUSCLE
    bool ok = false;
    config.nThreads = env->getVar(ENV_MUSCLE_N_THREADS).toInt(&ok);
    if(!ok) {
        stateInfo.setError(QString("Invalid test suite environment variable \"%1\"").arg(ENV_MUSCLE_N_THREADS));
        return;
    }
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

MultipleSequenceAlignment GTest_Muscle_Load_Align_QScore::dna_to_ma(QList<GObject*> dnaSeqs) {

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

QList<Task*> GTest_Muscle_Load_Align_QScore::onSubTaskFinished(Task* subTask) {
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

        muscleTask = new MuscleTask(ma1->getMultipleAlignment(), config);
        res << muscleTask;
        this->connect(muscleTask,SIGNAL(si_progressChanged()),SLOT(sl_muscleProgressChg()));
    }
    else if (subTask == muscleTask) {
        if(muscleTask->hasError()) {
            setError( muscleTask->getError() );
            return res;
        }

        MuscleTask * localMuscle = qobject_cast<MuscleTask*>( subTask );
        assert( NULL != localMuscle );
        ma1->setMultipleAlignment( localMuscle->resultMA );

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

void GTest_Muscle_Load_Align_QScore::run() {
    double qscore = QScore(ma1->getMultipleAlignment(), ma2->getMultipleAlignment(), stateInfo);
    if(stateInfo.hasError()) {
        return;
    }

    bool match = fabsl(this->qscore - qscore) < dqscore;

    if(!match) stateInfo.setError(  QString("qscore not matched: %1, expected %2").arg(qscore).arg(this->qscore));
}

Task::ReportResult GTest_Muscle_Load_Align_QScore::report() {
    propagateSubtaskError();
    return ReportResult_Finished;
}

GTest_Muscle_Load_Align_QScore::~GTest_Muscle_Load_Align_QScore() {
}

Muscle_Load_Align_Compare_Task::Muscle_Load_Align_Compare_Task( QString inFileURL, QString patFileURL,
                                                                MuscleTaskSettings& _config, QString _name)
: Task(_name, TaskFlags_FOSCOE), str_inFileURL(inFileURL), str_patFileURL(patFileURL),muscleTask(NULL), config(_config)
{
    //QFileInfo fInf(inFileURL);
    //setTaskName("Muscle_Load_Align_Compare_Task: " + fInf.fileName());
    setUseDescriptionFromSubtask(true);
    stateInfo.progress = 0;
    loadTask1 = NULL;
    loadTask2 = NULL;
    muscleTask = NULL;
    ma1 = NULL;
    ma2 = NULL;
}

void Muscle_Load_Align_Compare_Task::prepare() {
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(str_inFileURL));
    loadTask1 = new LoadDocumentTask(BaseDocumentFormats::FASTA,str_inFileURL,iof);
    loadTask1->setSubtaskProgressWeight(0);
    addSubTask(loadTask1);
    iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(str_patFileURL));
    loadTask2 = new LoadDocumentTask(BaseDocumentFormats::FASTA,str_patFileURL,iof);
    addSubTask(loadTask2);
    loadTask1->setSubtaskProgressWeight(0);
}

MultipleSequenceAlignment Muscle_Load_Align_Compare_Task::dna_to_ma(QList<GObject*> dnaSeqs) {

    int seqCount = dnaSeqs.count();
    U2SequenceObject *seq = qobject_cast<U2SequenceObject *>(dnaSeqs[0]);
    MultipleSequenceAlignment ma("Alignment",seq->getAlphabet());
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

QList<Task*> Muscle_Load_Align_Compare_Task::onSubTaskFinished(Task* subTask) {
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

        muscleTask = new MuscleTask(ma1->getMultipleAlignment(), config);

        res << muscleTask;
        this->connect(muscleTask,SIGNAL(si_progressChanged()),SLOT(sl_muscleProgressChg()));
    }
    else if (subTask == muscleTask) {
        if(muscleTask->hasError()) {
            setError( muscleTask->getError() );
            return res;
        }
        MuscleTask * localMuscle = qobject_cast<MuscleTask*>( subTask );
        assert( NULL != localMuscle );
        ma1->setMultipleAlignment( localMuscle->resultMA );

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

void Muscle_Load_Align_Compare_Task::run() {

    const QList<MultipleSequenceAlignmentRow> alignedSeqs1 = ma1->getMsa()->getMsaRows();
    const QList<MultipleSequenceAlignmentRow> alignedSeqs2 = ma2->getMsa()->getMsaRows();

    foreach(const MultipleSequenceAlignmentRow &maItem1, alignedSeqs1) {
        bool nameFound = false;
        foreach(const MultipleSequenceAlignmentRow&maItem2, alignedSeqs2) {
            if (maItem1->getName()== maItem2->getName()) {
                nameFound = true;
                int l1 = maItem1->getCoreLength();
                int l2 = maItem2->getCoreLength();
                if (l1!= l2) {
                    stateInfo.setError(  QString("Aligned sequences \"%1\" length not matched \"%2\", expected \"%3\"").arg(maItem1->getName()).arg(l1).arg(l2) );
                    return;
                }
                if (*maItem1 != *maItem2) {
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

void Muscle_Load_Align_Compare_Task::cleanup() {
    delete ma1;
    delete ma2;
    ma1 = NULL;
    ma2 = NULL;
}

Task::ReportResult Muscle_Load_Align_Compare_Task::report() {
    propagateSubtaskError();
    if(hasError()) {
        stateInfo.setError(  QString("input file \"%1\", pattern file \"%2\":\n").arg(str_inFileURL).arg(str_patFileURL) + stateInfo.getError() );
    }
    return ReportResult_Finished;
}

void GTest_Muscle_Load_Align_Compare::init(XMLTestFormat *, const QDomElement& el) {
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

void GTest_Muscle_Load_Align_Compare::prepare() {

    MuscleTaskSettings mSettings;
    mSettings.stableMode = false; //default mode is 'group' like in MUSCLE
    bool ok = false;
    mSettings.nThreads = env->getVar(ENV_MUSCLE_N_THREADS).toInt(&ok);
    if(!ok) {
        stateInfo.setError(QString("Invalid test suite environment variable \"%1\"").arg(ENV_MUSCLE_N_THREADS));
        return;
    }
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

    worker = new Muscle_Load_Align_Compare_Task(inFile.absoluteFilePath(),patFile.absoluteFilePath(),mSettings,inFile.fileName());
    addSubTask(worker);
}

Task::ReportResult GTest_Muscle_Load_Align_Compare::report() {
    propagateSubtaskError();
    return ReportResult_Finished;
}

GTest_Muscle_Load_Align_Compare::~GTest_Muscle_Load_Align_Compare() {
}

void GTest_uMusclePacketTest::init(U2::XMLTestFormat *tf, const QDomElement &el) {
    Q_UNUSED(tf);
    int nThread = qMax(0, getEnv()->getVar("NUM_THREADS").toInt());
    setMaxParallelSubtasks(nThread);

    tpm = Task::Progress_SubTasksBased;

    inDirName = el.attribute(IN_DIR_ATTR);
    if (inDirName.isEmpty()) {
        failMissingValue(IN_DIR_ATTR);
        return;
    }
    patDirName = el.attribute(PAT_DIR_ATTR);
    if (patDirName.isEmpty()) {
        failMissingValue(PAT_DIR_ATTR);
        return;
    }
}
void GTest_uMusclePacketTest::prepare() {
    QDir inDir(env->getVar("COMMON_DATA_DIR")+"/"+inDirName);
    QDir refDir(env->getVar("COMMON_DATA_DIR")+"/"+patDirName);
    QFileInfoList allFilesInfoList = inDir.entryInfoList();
    QFileInfoList inFileInfoList;
    QFileInfoList patFileInfoList;
    if (allFilesInfoList.isEmpty()) {
        stateInfo.setError(  QString("no files in dir %1").arg(inDir.absolutePath()) );
        return;
    }

    foreach(const QFileInfo &infoLisItem, allFilesInfoList) {
        if(infoLisItem.isFile()) {
            if(!infoLisItem.exists()) {
                stateInfo.setError(  QString("no not exist %1").arg(infoLisItem.absoluteFilePath()) );
            }
            QDir patternDir(env->getVar("COMMON_DATA_DIR")+"/"+patDirName);
            QFileInfo patFile(patternDir.absolutePath() + "/" + infoLisItem.fileName());
            if(!patFile.exists()) {
                stateInfo.setError(  QString("file not exist %1").arg(patFile.absoluteFilePath()) );
                return;
            }
            inFileInfoList << infoLisItem;
            patFileInfoList << patFile;
        }
    }

    MuscleTaskSettings mSettings;
    mSettings.stableMode = false; //default mode is 'group' like in MUSCLE
    bool ok = false;
    mSettings.nThreads = env->getVar(ENV_MUSCLE_N_THREADS).toInt(&ok);
    if(!ok) {
        stateInfo.setError(QString("Invalid test suite environment variable \"%1\"").arg(ENV_MUSCLE_N_THREADS));
        return;
    }

    for (int i = 0; i < inFileInfoList.count(); i++) {
        Task *task = new Muscle_Load_Align_Compare_Task(inFileInfoList[i].absoluteFilePath(),patFileInfoList[i].absoluteFilePath(),mSettings, "MusclePacketTestSubtask: "+inFileInfoList[i].fileName());
        addSubTask(task);
    }
    timer.start();
}

Task::ReportResult GTest_uMusclePacketTest::report() {
    propagateSubtaskError();
    if (!hasError()) {
        algoLog.trace(QString("uMusclePacketTest: \"%1\" accomplished. Time elapsed: %2 ms").arg(inDirName).arg(timer.elapsed()));
    }
    return ReportResult_Finished;
}

QList<XMLTestFactory*> UMUSCLETests::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(GTest_uMuscle::createFactory());
    res.append(GTest_CompareMAlignment::createFactory());
    res.append(GTest_uMuscleAddUnalignedSequenceToProfile::createFactory());
    res.append(GTest_uMusclePacketTest::createFactory());
    res.append(GTest_Muscle_Load_Align_Compare::createFactory());
    res.append(GTest_Muscle_Load_Align_QScore::createFactory());
    return res;
}

}//namespace
