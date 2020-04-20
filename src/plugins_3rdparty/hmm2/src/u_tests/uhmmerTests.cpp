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
#include <QFileInfo>

#include <U2Core/DNASequence.h>
#include <U2Core/TextUtils.h>
#include <U2Core/AppContext.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/GObject.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/TextObject.h>

#include <U2Test/GTestFrameworkComponents.h>

#include "uhmmerTests.h"
#include "hmmer2/funcs.h"
#include "u_build/HMMBuildDialogController.h"
#include "u_calibrate/HMMCalibrateTask.h"
#include "u_search/HMMSearchDialogController.h"

namespace U2 {

#define HMM_FILE_ATTR "hmmfile"
#define SEQ_DB_DOC "seqdoc"
#define OUT_DOC_NAME_ATTR "outdoc"
#define EXPERT_OPT_FLAG_ATTR "expert_opt"
#define E_VALUE_CUTOFF_ATTR "evalue_cutoff"
#define DOM_E_VALUE_CUTOFF_ATTR "domE_cutoff"
#define MIN_SCORE_CUTOFF_ATTR "min_score_cutoff"
#define NUMBER_OF_SEQ_ATTR "seq_in_db"
#define PARALLEL_FLAG_ATTR "parallel"
#define IN_FILE_NAME_ATTR "infile"
#define OUT_FILE_NAME_ATTR "outfile"
#define EXP_OPT_ATTR "expert"
#define HMM_NAME_ATTR "hmmname"
#define IN_FILE1_NAME_ATTR "file1"
#define IN_FILE2_NAME_ATTR "file2"
#define DEL_TEMP_FILE_ATTR "deltemp"
#define HMMSEARCH_CHUNK_ATTR "chunksize"
#define NUMBER_OF_THREADS_ATTR "nthreads"
#define RUN_N_CALIBRATES "ncalibrates"
#define MU_ATTR "mu"
#define LAMBDA_ATTR "lambda"
#define SEED_ATTR "seed"

#define ENV_HMMSEARCH_ALGORITHM_NAME "HMMSEARCH_ALGORITHM"
#define ENV_HMMSEARCH_ALGORITHM_SSE "sse"
#define ENV_HMMSEARCH_ALGORITHM_CELL "cell"

class GTest_LoadDocument;
class Document;
class GObject;

//**********uHMMER Search********************

/* TRANSLATOR U2::GTest */

void GTest_uHMMERSearch::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    evalueCutoff = 10;
    number_of_seq = 1;
    domEvalueCutoff = 0.9999999;
    minScoreCutoff = -FLT_MAX;

    hmmFileName = el.attribute(HMM_FILE_ATTR);
    if (hmmFileName.isEmpty()) {
        failMissingValue(HMM_FILE_ATTR);
        return;
    }

    resultDocContextName = el.attribute("index");

    seqDocCtxName = el.attribute(SEQ_DB_DOC);
    if (seqDocCtxName.isEmpty()) {
        failMissingValue(SEQ_DB_DOC);
        return;
    }
    resultDocName = el.attribute(OUT_DOC_NAME_ATTR);

    QString exp_opt_str = el.attribute(EXPERT_OPT_FLAG_ATTR);
    if (exp_opt_str.isEmpty()) {
        failMissingValue(EXPERT_OPT_FLAG_ATTR);
        return;
    }
    bool ok=false;
    expertOptions = exp_opt_str.toInt(&ok);
    if(!ok) {
        failMissingValue(EXPERT_OPT_FLAG_ATTR);
        return;
    }
    if(expertOptions) {
        QString eval_catoff_str = el.attribute(E_VALUE_CUTOFF_ATTR);
        if (!eval_catoff_str.isEmpty()) {
            ok=false;
            evalueCutoff = eval_catoff_str.toFloat(&ok);
            if(!ok) {
                failMissingValue(E_VALUE_CUTOFF_ATTR);
                return;
            }
        }

        QString num_of_seq_str = el.attribute(NUMBER_OF_SEQ_ATTR);
        if (!num_of_seq_str.isEmpty()) {
            ok=false;
            number_of_seq = num_of_seq_str.toFloat(&ok);
            if(!ok) {
                failMissingValue(NUMBER_OF_SEQ_ATTR);
                return;
            }
        }

        QString domEvalueCutoff_str = el.attribute(DOM_E_VALUE_CUTOFF_ATTR);
        if (!domEvalueCutoff_str.isEmpty()) {
            ok=false;
            domEvalueCutoff = domEvalueCutoff_str .toFloat(&ok);
            if(!ok) {
                failMissingValue(DOM_E_VALUE_CUTOFF_ATTR);
                return;
            }
        }

        QString minScoreCutoff_str = el.attribute(MIN_SCORE_CUTOFF_ATTR);
        if (!minScoreCutoff_str.isEmpty()) {

            ok=false;
            minScoreCutoff = minScoreCutoff_str.toFloat(&ok);
            if(!ok) {
                failMissingValue(MIN_SCORE_CUTOFF_ATTR);
                return;
            }
        }
    }

    customHmmSearchChunk = false;
    QString hmmSearchChunk_str = el.attribute(HMMSEARCH_CHUNK_ATTR);
    if (!hmmSearchChunk_str.isEmpty()) {
        ok=false;
        hmmSearchChunk = hmmSearchChunk_str.toInt(&ok);
        if(!ok) {
            failMissingValue(HMMSEARCH_CHUNK_ATTR);
            return;
        }
        customHmmSearchChunk = true;
    }
    parallel_flag = false;
    QString parallel_flag_str = el.attribute(PARALLEL_FLAG_ATTR);
    if (!parallel_flag_str.isEmpty()) {
        if(parallel_flag_str == "true")
            parallel_flag = true;
        else if(parallel_flag_str == "false")
            parallel_flag = false;
        else {
            failMissingValue(PARALLEL_FLAG_ATTR);
            return;
        }
    }

    searchTask = NULL;
    saveTask = NULL;
    aDoc = NULL;
}
void GTest_uHMMERSearch::prepare() {
    Document* doc = getContext<Document>(this, seqDocCtxName);
    if (doc == NULL) {
        stateInfo.setError(  QString("context not found %1").arg(seqDocCtxName) );
        return;
    }


    QList<GObject*> list = doc->findGObjectByType(GObjectTypes::SEQUENCE);
    if (list.size() == 0) {
        stateInfo.setError(  QString("container of object with type \"%1\" is empty").arg(GObjectTypes::SEQUENCE) );
        return;
    }

    GObject *obj = list.first();
    if(obj==NULL){
        stateInfo.setError(  QString("object with type \"%1\" not found").arg(GObjectTypes::SEQUENCE) );
        return;
    }
    assert(obj!=NULL);
    U2SequenceObject * mySequence = qobject_cast<U2SequenceObject*>(obj);
    if(mySequence==NULL){
        stateInfo.setError(  QString("error can't cast to sequence from GObject") );
        return;
    }

    UHMMSearchSettings s;
    if (expertOptions){
        s.globE = evalueCutoff;
        s.eValueNSeqs = number_of_seq;
        s.domE = domEvalueCutoff;
        s.domT = minScoreCutoff;
    }
    QString env_algo = env->getVar(ENV_HMMSEARCH_ALGORITHM_NAME);
    if( !env_algo.isEmpty() ) {
        if( ENV_HMMSEARCH_ALGORITHM_SSE == env_algo ) {
#if !defined(HMMER_BUILD_WITH_SSE2)
            stateInfo.setError( QString("SSE2 was not enabled in this build") );
            return;
#endif
            s.alg = HMMSearchAlgo_SSEOptimized;
        } else if( ENV_HMMSEARCH_ALGORITHM_CELL == env_algo ) {
#if !defined UGENE_CELL
            stateInfo.setError( QString("HMMER-Cell was not enabled in this build") );
            return;
#endif
            s.alg = HMMSearchAlgo_CellOptimized;
        } else {
            stateInfo.setError( QString("unknown hmmsearch algorithm is selected") );
            return;
        }
    }
    if(customHmmSearchChunk) {
        s.searchChunkSize = hmmSearchChunk;
    }
    QString annotationName = "hmm_signal";
    QString url = env->getVar("TEMP_DATA_DIR")+"/uhmmsearch/"+resultDocName;
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url));
    DocumentFormat* df = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::PLAIN_GENBANK);
    assert(aDoc == NULL);
    aDoc = df->createNewLoadedDocument(iof, url, stateInfo);
    CHECK_OP(stateInfo, );
    AnnotationTableObject *ao = new AnnotationTableObject( "Annotations", aDoc->getDbiRef( ) );
    aDoc->addObject(ao);
    DNASequence dnaSequence = mySequence->getWholeSequence(stateInfo);
    CHECK_OP(stateInfo, );
    searchTask = new HMMSearchToAnnotationsTask(env->getVar("COMMON_DATA_DIR")+"/"+hmmFileName, dnaSequence, ao, annotationName, "", U2FeatureTypes::MiscSignal, annotationName, s);
    addSubTask(searchTask);
}

QList<Task*> GTest_uHMMERSearch::onSubTaskFinished(Task* subTask) {
    Q_UNUSED(subTask);
    QList<Task*> res;
    if (hasError() || isCanceled()) {
        return res;
    }


    if(saveTask && saveTask->isFinished()) {
        if (saveTask->hasError()) {
            stateInfo.setError(  "SaveDocumentTask: "+saveTask->getError() );
        }
        return res;
    } else if(searchTask!=NULL && searchTask->isFinished()) {

        if (searchTask->hasError()) {
            stateInfo.setError(  searchTask->getError() );
            return res;
        }

        if(aDoc == NULL) {
            stateInfo.setError(  QString("documet creating error") );
            return res;
        }
        if(!resultDocName.isEmpty()) {
            QFileInfo fi(aDoc->getURLString());
            fi.absoluteDir().mkpath(fi.absoluteDir().absolutePath());
            saveTask = new SaveDocumentTask(aDoc);
            res.append(saveTask);
        }
        return res;
    }
    return res;
}

Task::ReportResult GTest_uHMMERSearch::report() {
    if (!resultDocContextName.isEmpty()) {
        addContext(resultDocContextName, aDoc);
    }
    return ReportResult_Finished;
}

GTest_uHMMERSearch::~GTest_uHMMERSearch() {
    //cleanup();
}

void GTest_uHMMERSearch::cleanup() {
    if (aDoc!=NULL) {
        delete aDoc;
        aDoc = NULL;
    }

    XmlTest::cleanup();
}


//*****************************************************************************
//**********uHMMER Build*******************************************************
//*****************************************************************************


void GTest_uHMMERBuild::init(XMLTestFormat* tf, const QDomElement& el) {
    Q_UNUSED(tf);

    QString inFile = el.attribute(IN_FILE_NAME_ATTR);
    if (inFile.isEmpty()) {
        failMissingValue(IN_FILE_NAME_ATTR);
        return;
    }
    outFile = el.attribute(OUT_FILE_NAME_ATTR);
    if (outFile.isEmpty()) {
        failMissingValue(OUT_FILE_NAME_ATTR);
        return;
    }
    QString expOpt = el.attribute(EXP_OPT_ATTR);
    if (expOpt.isEmpty()) {
        failMissingValue(EXP_OPT_ATTR);
        return;
    }
    QString hmmName = el.attribute(HMM_NAME_ATTR);

    QString delTempStr = el.attribute(DEL_TEMP_FILE_ATTR);
    if (delTempStr .isEmpty()) {
        failMissingValue(DEL_TEMP_FILE_ATTR);
        return;
    }
    if(delTempStr=="yes")
        deleteTempFile = true;
    else if(delTempStr=="no") deleteTempFile =false;
    else {
        failMissingValue(DEL_TEMP_FILE_ATTR);
        return;
    }

    UHMMBuildSettings s;
    s.name = hmmName;
    if(expOpt=="LS") s.strategy = P7_LS_CONFIG;
    else if(expOpt=="FS")  s.strategy = P7_FS_CONFIG;
    else if(expOpt=="BASE")  s.strategy = P7_BASE_CONFIG;
    else if(expOpt=="SW")  s.strategy = P7_SW_CONFIG;
    else {
        stateInfo.setError(  QString("invalid value %1, available values: LS, FS, BASE, SW").arg(EXP_OPT_ATTR) );
        return;
    }
    QFileInfo fi(env->getVar("TEMP_DATA_DIR")+"/"+outFile);
    fi.absoluteDir().mkpath(fi.absoluteDir().absolutePath());
    QFile createFile(fi.absoluteFilePath());
    createFile.open(QIODevice::WriteOnly);
    if(!createFile.isOpen()){
        stateInfo.setError(  QString("File opening error \"%1\", description: ").arg(createFile.fileName())+createFile.errorString() );
        return;
    }
    else createFile.close();
    buildTask = new HMMBuildToFileTask(env->getVar("COMMON_DATA_DIR")+"/"+inFile, createFile.fileName(), s);
    outFile = createFile.fileName();
    addSubTask(buildTask);
}

Task::ReportResult GTest_uHMMERBuild::report() {
    propagateSubtaskError();
    if(buildTask->hasError())  {
        stateInfo.setError(  buildTask->getError() );
    }
    return ReportResult_Finished;
}

void GTest_uHMMERBuild::cleanup(){
    if (!hasError() && deleteTempFile){
        QFile::remove(outFile);
    }

    XmlTest::cleanup();
}

void GTest_hmmCompare::init(XMLTestFormat* tf, const QDomElement& el) {
    Q_UNUSED(tf);

    file1Name = el.attribute(IN_FILE1_NAME_ATTR);
    if (file1Name.isEmpty()) {
        failMissingValue(IN_FILE1_NAME_ATTR);
        return;
    }
    file2Name= el.attribute(IN_FILE2_NAME_ATTR);
    if (file2Name.isEmpty()) {
        failMissingValue(IN_FILE2_NAME_ATTR);
        return;
    }
}

Task::ReportResult GTest_hmmCompare::report() {

    QFileInfo fi1(env->getVar("COMMON_DATA_DIR")+"/"+file1Name);
    QString url1 = fi1.absoluteFilePath();
    IOAdapterFactory* iof1 = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url1));
    QScopedPointer<IOAdapter> io1(iof1->createIOAdapter());
    //QFile file1(fi1.absoluteFilePath());
    QFileInfo fi2(env->getVar("TEMP_DATA_DIR")+"/"+file2Name);
    QString url2 = fi2.absoluteFilePath();
    IOAdapterFactory* iof2 = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url2));
    QScopedPointer<IOAdapter> io2(iof2->createIOAdapter());
    fi2.absoluteDir().mkdir(fi2.absoluteDir().absolutePath()); // ???
    //QFile file2(fi2.absoluteFilePath());

    if(!io1->open(url1, IOAdapterMode_Read)){
        stateInfo.setError(  QString("File opening error \"%1\", description: ").arg(url1) );//+file1.errorString() );
        return ReportResult_Finished;
    }
    //file2.open(QIODevice::ReadOnly|QIODevice::Text);
    if(!io2->open(url2, IOAdapterMode_Read)){
        stateInfo.setError(  QString("File opening error \"%1\", description: ").arg(url2) );//+file2.errorString() );
        return ReportResult_Finished;
    }

    static int READ_BUFF_SIZE = 4096;

    qint64 len1, len2, line1 = 0, line2 = 0;
    QByteArray readBuffer1(READ_BUFF_SIZE, '\0'), readBuffer2(READ_BUFF_SIZE, '\0');
    char* cbuff1 = readBuffer1.data();
    char* cbuff2 = readBuffer2.data();
    QRegExp rx("CKSUM ");

    bool ok = false;
    while ( (len1 = io1->readUntil(cbuff1, READ_BUFF_SIZE, TextUtils::LINE_BREAKS, IOAdapter::Term_Include)) > 0 ) {
        line1++;
        if ((ok = rx.indexIn(QString(QByteArray(cbuff1, len1))) !=-1)) {
            break;
        }
    }
    if(!ok){
        stateInfo.setError(  QString("can't find CKSUM in file \"%1\"").arg(url1) );
        return ReportResult_Finished;
    }

    ok = false;
    while ( (len2 = io2->readUntil(cbuff2, READ_BUFF_SIZE, TextUtils::LINE_BREAKS, IOAdapter::Term_Include)) > 0 ) {
        line2++;
        if ((ok = rx.indexIn(QString(QByteArray(cbuff2, len2))) !=-1)) {
            break;
        }
    }
    if(!ok){
        stateInfo.setError(  QString("can't find CKSUM in file \"%1\"").arg(url2) );
        return ReportResult_Finished;
    }

    do{
        len1 = io1->readUntil(cbuff1, READ_BUFF_SIZE, TextUtils::LINE_BREAKS, IOAdapter::Term_Include);
        len2 = io2->readUntil(cbuff2, READ_BUFF_SIZE, TextUtils::LINE_BREAKS, IOAdapter::Term_Include);
        if(len1 == 0 && 0 != len2){
            stateInfo.setError(  QString("hmm-compare: files not equal, desc: files length mismatch") );
            return ReportResult_Finished;
        }
        line1++;line2++;
        QString s1 = QString::fromLatin1(cbuff1, len1).trimmed();
        QString s2 = QString::fromLatin1(cbuff2, len2).trimmed();

        if(s1 != s2)
        {
            stateInfo.setError( QString("hmm-compare: files not equal, desc: file1, line %1 \"%2\", expected file2, line %3 \"%4\"")
                .arg(line1).arg(s1).arg(line2).arg(s2) );
            return ReportResult_Finished;
        }

    } while(len1 > 0);

    return ReportResult_Finished;
}



//*****************************************************************************
//**********uHMMER Calibrate***************************************************
//*****************************************************************************


void GTest_uHMMERCalibrate::init(XMLTestFormat* tf, const QDomElement& el) {
    Q_UNUSED(tf);

    calibrateTask = NULL;

    QString hmmFile = el.attribute(HMM_FILE_ATTR);
    if (hmmFile.isEmpty()) {
        failMissingValue(HMM_FILE_ATTR);
        return;
    }
    QString mu_str = el.attribute(MU_ATTR);
    if (mu_str.isEmpty()) {
        failMissingValue(MU_ATTR);
        return;
    }
    bool ok=false;
    mu = mu_str.toFloat(&ok);
    if(!ok) {
        failMissingValue(MU_ATTR);
        return;
    }
    QString lambda_str = el.attribute(LAMBDA_ATTR);
    if (lambda_str.isEmpty()) {
        failMissingValue(LAMBDA_ATTR);
        return;
    }
    ok=false;
    lambda = lambda_str.toFloat(&ok);
    if(!ok) {
        failMissingValue(LAMBDA_ATTR);
        return;
    }
    QString nThreads_str = el.attribute(NUMBER_OF_THREADS_ATTR);
    if (nThreads_str.isEmpty()) {
        failMissingValue(NUMBER_OF_THREADS_ATTR);
        return;
    }
    ok=false;
    int nThreads = nThreads_str.toFloat(&ok);
    if(!ok) {
        failMissingValue(NUMBER_OF_THREADS_ATTR);
        return;
    }
    nCalibrates = 1;
    QString nCalibrates_str = el.attribute(RUN_N_CALIBRATES);
    if (!nCalibrates_str.isEmpty()) {
        bool ok=false;
        nCalibrates = nCalibrates_str.toInt(&ok);
        if(!ok) {
            failMissingValue(RUN_N_CALIBRATES);
            return;
        }
    }
    UHMMCalibrateSettings s;

    QString seed_str = el.attribute(SEED_ATTR);
    if (!seed_str.isEmpty()) {
        bool ok=false;
        int seed = seed_str.toInt(&ok);
        if(!ok) {
            failMissingValue(SEED_ATTR);
            return;
        }
        s.seed = seed;
    }

    calibrateTask = new HMMCalibrateToFileTask*[nCalibrates];

    s.nThreads = nThreads;

    //Run nCalibrates HMMCalibrate tasks simultaneously
    for(int i=0;i<nCalibrates;i++){
        calibrateTask[i] = new HMMCalibrateToFileTask(env->getVar("COMMON_DATA_DIR")+"/"+hmmFile,env->getVar("TEMP_DATA_DIR")+"/temp111",s);
    }
    addSubTask(new GTest_uHMMERCalibrateSubtask(calibrateTask, nCalibrates));
}

Task::ReportResult GTest_uHMMERCalibrate::report() {
    propagateSubtaskError();
    if (isCanceled() || hasError()) {
        return ReportResult_Finished;
    }
    for(int i=0; i < nCalibrates; i++){
        float new_mu = ((::plan7_s*)calibrateTask[i]->getHMM())->mu;
        if (qAbs(new_mu - mu) > 0.1) {
            stateInfo.setError(  QString("mu value %1, expected %2").arg(new_mu).arg(mu) );
            break;
        }
        float new_lambda = ((::plan7_s*)calibrateTask[i]->getHMM())->lambda;
        if (qAbs(new_lambda - lambda) > 0.1){
            stateInfo.setError(  QString("lambda value %1, expected %2").arg(new_lambda).arg(lambda) );
            break;
        }
    }
    return ReportResult_Finished;
}

GTest_uHMMERCalibrate::GTest_uHMMERCalibrateSubtask::GTest_uHMMERCalibrateSubtask(HMMCalibrateToFileTask **calibrateTask, int n)
:Task(tr("uhmmer-calibrate-subtask"),TaskFlags_NR_FOSCOE)
{
    assert(calibrateTask!=NULL);
    for(int i=0;i<n;i++){
        assert(calibrateTask[i]!=NULL);
        addSubTask(calibrateTask[i]);
    }
}
void GTest_uHMMERCalibrate::cleanup(){
    if (!hasError()) {
        QFile::remove(env->getVar("TEMP_DATA_DIR")+"/temp111");
    }
    delete[] calibrateTask;

    XmlTest::cleanup();
}

QList<XMLTestFactory*> UHMMERTests::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(GTest_uHMMERSearch::createFactory());
    res.append(GTest_uHMMERBuild::createFactory());
    res.append(GTest_hmmCompare::createFactory());
    res.append(GTest_uHMMERCalibrate::createFactory());
    return res;
}

}//namespace
