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

#include "SmithWatermanTests.h"

#include <U2Core/DNASequenceObject.h>

#include <U2Algorithm/SmithWatermanTaskFactoryRegistry.h>
#include <U2Core/AppContext.h>
#include <U2Algorithm/SubstMatrixRegistry.h>

#include <U2Algorithm/SmithWatermanSettings.h>
#include <U2Core/SequenceWalkerTask.h>
#include <U2Core/SMatrix.h>
#include <U2Core/U2SafePoints.h>


#define FILE_SUBSTITUTION_MATRIX_ATTR "subst_f"
#define FILE_FASTA_CONTAIN_SEQUENCE_ATTR "seq_f"
#define FILE_FASTA_CONTAIN_PATTERN_ATTR "pattern_f"
#define GAP_OPEN_ATTR "g_o"
#define GAP_EXT_ATTR "g_e"
#define PERCENT_OF_SCORE_ATTR "percent_of_score"
#define EXPECTED_RESULT_ATTR "expected_res"
#define ENV_IMPL_ATTR "IMPL"
#define IMPL_ATTR "impl"

using namespace std;
namespace U2 {

void GTest_SmithWatermnan::sortByScore(QList<SmithWatermanResult> & resultsForSort) {
    for (int i = 0; i < resultsForSort.size(); i++) {
        for (int j = i + 1; j < resultsForSort.size(); j++) {
            if (resultsForSort.at(i).score < resultsForSort.at(j).score) {
                SmithWatermanResult buf = resultsForSort.at(i);
                resultsForSort[i] = resultsForSort.at(j);
                resultsForSort[j] = buf;
            }
            if (resultsForSort.at(i).score == resultsForSort.at(j).score &&
                resultsForSort.at(i).refSubseq.startPos > resultsForSort.at(j).refSubseq.startPos) {
                    SmithWatermanResult buf = resultsForSort.at(i);
                    resultsForSort[i] = resultsForSort.at(j);
                    resultsForSort[j] = buf;
            }
            if (resultsForSort.at(i).score == resultsForSort.at(j).score &&
                resultsForSort.at(i).refSubseq.startPos == resultsForSort.at(j).refSubseq.startPos &&
                resultsForSort.at(i).refSubseq.length > resultsForSort.at(j).refSubseq.length) {
                    SmithWatermanResult buf = resultsForSort.at(i);
                    resultsForSort[i] = resultsForSort.at(j);
                    resultsForSort[j] = buf;
            }
        }
    }
}


void GTest_SmithWatermnan::init(XMLTestFormat *, const QDomElement& el) {
    searchSeqDocName = el.attribute(FILE_FASTA_CONTAIN_SEQUENCE_ATTR);
    if (searchSeqDocName.isEmpty()) {
        failMissingValue(FILE_FASTA_CONTAIN_SEQUENCE_ATTR);
        return;
    }

    patternSeqDocName = el.attribute(FILE_FASTA_CONTAIN_PATTERN_ATTR);
    if (patternSeqDocName.isEmpty()) {
        failMissingValue(FILE_FASTA_CONTAIN_PATTERN_ATTR);
        return;
    }

    pathToSubst = el.attribute(FILE_SUBSTITUTION_MATRIX_ATTR);
    if (pathToSubst.isEmpty()) {
        failMissingValue(FILE_SUBSTITUTION_MATRIX_ATTR);
        return;
    }

    QString buffer = el.attribute(GAP_OPEN_ATTR);
    bool ok = false;

    if (!buffer.isEmpty()) {
        ok=false;
        gapOpen = buffer.toInt(&ok);
        if(!ok) {
            failMissingValue(GAP_OPEN_ATTR);
            return;
        }
    }

    buffer = el.attribute(GAP_EXT_ATTR);
    if (!buffer.isEmpty()) {
        ok=false;
        gapExtension = buffer.toInt(&ok);
        if(!ok) {
            failMissingValue(GAP_EXT_ATTR);
            return;
        }
    }

    buffer = el.attribute(PERCENT_OF_SCORE_ATTR);
    if (!buffer.isEmpty()) {
        ok=false;
        percentOfScore = buffer.toFloat(&ok);
        if(!ok) {
            failMissingValue(PERCENT_OF_SCORE_ATTR);
            return;
        }
    }

    expected_res = el.attribute(EXPECTED_RESULT_ATTR);
    if (expected_res.isEmpty()) {
        failMissingValue(EXPECTED_RESULT_ATTR);
        return;
    }

    if (!parseExpected_res()) {
        stateInfo.setError(QString("value not correct %1").arg(EXPECTED_RESULT_ATTR));
        return;
    }

    impl = env->getVar(ENV_IMPL_ATTR);
    if (impl.isEmpty()) {
        failMissingValue(ENV_IMPL_ATTR);
        return;
    }
}


void GTest_SmithWatermnan::prepare() {

    //get search sequence
    U2SequenceObject * searchSeqObj = getContext<U2SequenceObject>(this, searchSeqDocName);
    if(searchSeqObj==NULL){
        stateInfo.setError(QString("error can't cast to sequence from GObject"));
        return;
    }
    searchSeq = searchSeqObj->getWholeSequenceData(stateInfo);
    CHECK_OP(stateInfo, );

    //get pattern sequence
    U2SequenceObject * patternSeqObj = getContext<U2SequenceObject>(this, patternSeqDocName);
    if(patternSeqObj==NULL){
        stateInfo.setError(QString("error can't cast to sequence from GObject"));
        return;
    }
    patternSeq = patternSeqObj->getWholeSequenceData(stateInfo);
    CHECK_OP(stateInfo, );

    //set subst matrix

    QString pathToCommonData = getEnv()->getVar("COMMON_DATA_DIR");
    if(patternSeqObj==NULL){
        stateInfo.setError(QString("error can't get path to common_data dir"));
        return;
    }
    QString fullPathToSubst = pathToCommonData + "/" + pathToSubst;

    QString error;
    SMatrix mtx = SubstMatrixRegistry::readMatrixFromFile(fullPathToSubst, error);
    if (mtx.isEmpty()) {
        stateInfo.setError(QString("value not set %1").arg(FILE_SUBSTITUTION_MATRIX_ATTR));
        return;
    }

    s.pSm = mtx;
    s.sqnc = searchSeq;
    s.ptrn = patternSeq;
    s.globalRegion.startPos = 0;
    s.globalRegion.length = searchSeq.length();
    s.gapModel.scoreGapOpen = gapOpen;
    s.gapModel.scoreGapExtd = gapExtension;
    s.percentOfScore = percentOfScore;
    s.aminoTT = NULL;
    s.complTT = NULL;
    s.strand = StrandOption_DirectOnly;
    s.resultCallback = NULL;
    s.resultListener = NULL;
    s.resultFilter = 0;

    if( !machinePath.isEmpty() ) { /* run smith-waterman on remote machine */
    //TODO: BUG-001870
        assert(0);
        //         SmithWatermanLocalTaskSettings localTaskSettings( s );
//         RemoteMachine * machine = NULL;
//         if( !SerializeUtils::deserializeRemoteMachineSettingsFromFile( machinePath, &machine ) ) {
//             setError( QString( "Cannot create remote machine from '%1'" ).arg( machinePath ) );
//             return;
//         }
//         assert( NULL != machine );
//         swAlgorithmTask = new RemoteTask( SmithWatermanLocalTaskFactory::ID, localTaskSettings, machine );
    } else { /* run on local machine */
        s.resultListener = new SmithWatermanResultListener();
        if (0 != AppContext::getSmithWatermanTaskFactoryRegistry()->getFactory(impl)) {
            swAlgorithmTask = (Task *) AppContext::getSmithWatermanTaskFactoryRegistry()->getFactory(impl)->getTaskInstance(s, "tests SmithWaterman");
        } else {
            stateInfo.setError(QString("Not known impl of Smith-Waterman: %1").arg(impl));
            return;
        }
    }
    addSubTask(swAlgorithmTask);

}

bool GTest_SmithWatermnan::parseExpected_res() {

    SWresult swRes;
    QStringList expectedList = expected_res.split(tr("**"));

    foreach(QString res, expectedList) {
        QStringList resValues = res.split(tr(","));
        if (resValues.size() != 2) {
            stateInfo.setError(QString("wrong count values in expected result: %1").arg(resValues.size()));
            return false;
        }

        //////// first enterval
        QStringList bounds = resValues.at(1).split(tr(".."));
        if (bounds.size() != 2) {
            stateInfo.setError(  QString("wrong region in expected result %1").arg(resValues.at(1)) );
            return false;
        }
        bool startOk, finishOk;
        int start = bounds.first().toInt(&startOk);
        int finish = bounds.last().toInt(&finishOk);
        if (startOk && finishOk != true) {
            stateInfo.setError(  QString("wrong region in expected result %1").arg(resValues.at(1)) );
            return false;
        }
        swRes.sInterval.startPos = start;
        swRes.sInterval.length = finish - start;

        start = resValues.at(0).toInt(&startOk);
        if (startOk != true) {
            stateInfo.setError(  QString("wrong scorein expected result %1").arg(resValues.at(0)) );
            return false;
        }
        swRes.score= start;

        expectedRes.append(swRes);
    }
    return true;
}

bool GTest_SmithWatermnan::toInt(QString & str, int & num) {
    bool ok = false;
    if (!str.isEmpty()) {
        num = str.toInt(&ok);
    }
    return ok;
}


Task::ReportResult GTest_SmithWatermnan::report() {

    propagateSubtaskError();
    if( hasError() ) {
        return ReportResult_Finished;
    }

    QList<SmithWatermanResult> resultList;
    if( !machinePath.isEmpty() ) { /* remote task used */
        //TODO: BUG-0001870
//         RemoteTask * remoteSW = qobject_cast<RemoteTask*>( swAlgorithmTask );
//         assert( NULL != remoteSW );
//         SmithWatermanLocalTaskResult * result = dynamic_cast<SmithWatermanLocalTaskResult*>( remoteSW->getResult() );
//         assert( NULL != result );
//         resultList = result->getResult();
    } else { /* task on local machine */
        resultList = s.resultListener->popResults();
    }
    sortByScore(resultList);


    if (expectedRes.size() != resultList.size()) {
        stateInfo.setError(QString("Not expected result: count result not coincide, expected: %1, current: %2")
                                   .arg(expectedRes.size())
                                   .arg(resultList.size()));
        return ReportResult_Finished;
    }

    for (int i = 0; i < resultList.size(); i++) {
        if (expectedRes.at(i).score != resultList.at(i).score ||
            expectedRes.at(i).sInterval != resultList.at(i).refSubseq) {
                stateInfo.setError(QString("Not expected result"));
                return ReportResult_Finished;
        }
    }


    return ReportResult_Finished;
}


void GTest_SmithWatermnanPerf::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);


    searchSeqDocName = el.attribute(FILE_FASTA_CONTAIN_SEQUENCE_ATTR);
    if (searchSeqDocName.isEmpty()) {
        failMissingValue(FILE_FASTA_CONTAIN_SEQUENCE_ATTR);
        return;
    }

    patternSeqDocName = el.attribute(FILE_FASTA_CONTAIN_PATTERN_ATTR);
    if (patternSeqDocName.isEmpty()) {
        failMissingValue(FILE_FASTA_CONTAIN_PATTERN_ATTR);
        return;
    }

    impl = el.attribute(IMPL_ATTR);
    if (patternSeqDocName.isEmpty()) {
        failMissingValue(IMPL_ATTR);
        return;
    }

    pathToSubst = "smith_waterman2/blosum62.txt";
    gapOpen = -1;
    gapExtension = -1;
    percentOfScore = 100;
}

void GTest_SmithWatermnanPerf::prepare() {

    //get search sequence
    U2SequenceObject * searchSeqObj = getContext<U2SequenceObject>(this, searchSeqDocName);
    if(searchSeqObj==NULL){
        stateInfo.setError(QString("error can't cast to sequence from GObject"));
        return;
    }
    searchSeq = searchSeqObj->getWholeSequenceData(stateInfo);
    CHECK_OP(stateInfo, );

    //get pattern sequence
    U2SequenceObject * patternSeqObj = getContext<U2SequenceObject>(this, patternSeqDocName);
    if(patternSeqObj==NULL){
        stateInfo.setError(QString("error can't cast to sequence from GObject"));
        return;
    }
    patternSeq = patternSeqObj->getWholeSequenceData(stateInfo);
    CHECK_OP(stateInfo, );

    setTaskName(QString("Test seq size %1").arg(patternSeq.size()));

    //set subst matrix

    QString pathToCommonData = getEnv()->getVar("COMMON_DATA_DIR");
    if(patternSeqObj==NULL){
        stateInfo.setError(QString("error can't get path to common_data dir"));
        return;
    }
    QString fullPathToSubst = pathToCommonData + "/" + pathToSubst;

    QString error;
    SMatrix mtx = SubstMatrixRegistry::readMatrixFromFile(fullPathToSubst, error);
    if (mtx.isEmpty()) {
        stateInfo.setError(QString("value not set %1").arg(FILE_SUBSTITUTION_MATRIX_ATTR));
        return;
    }

    s.pSm = mtx;
    s.sqnc = searchSeq;
    s.ptrn = patternSeq;
    s.globalRegion.startPos = 0;
    s.globalRegion.length = searchSeq.length();
    s.gapModel.scoreGapOpen = gapOpen;
    s.gapModel.scoreGapExtd = gapExtension;
    s.percentOfScore = percentOfScore;
    s.aminoTT = NULL;
    s.complTT = NULL;
    s.strand = StrandOption_DirectOnly;
    s.resultCallback = NULL;
    s.resultListener = NULL;
    s.resultFilter = 0;
    s.resultListener = new SmithWatermanResultListener();
    if (0 != AppContext::getSmithWatermanTaskFactoryRegistry()->getFactory(impl)) {
        swAlgorithmTask = (Task *) AppContext::getSmithWatermanTaskFactoryRegistry()->getFactory(impl)->getTaskInstance(s, "test SW performance");
    } else {
         stateInfo.setError(QString("Not known impl of Smith-Waterman: %1").arg(impl));
           return;
    }
    addSubTask(swAlgorithmTask);
}

Task::ReportResult GTest_SmithWatermnanPerf::report() {
    propagateSubtaskError();
    if( hasError() ) {
        return ReportResult_Finished;
    }

    return ReportResult_Finished;
}

}
