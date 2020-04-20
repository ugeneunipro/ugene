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

#ifdef SW2_BUILD_WITH_CUDA
#include <cuda_runtime.h>
#endif

#include "SWAlgorithmTask.h"

#include "SmithWatermanAlgorithmCUDA.h"
#include "SmithWatermanAlgorithmSSE2.h"
#include "SmithWatermanAlgorithmOPENCL.h"
#include "sw_cuda_cpp.h"

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/AppResources.h>
#include <U2Core/Log.h>
#include <U2Core/Counter.h>
#include <U2Core/Timer.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SequenceDbi.h>
#include <U2Core/U2SafePoints.h>

#include <U2Algorithm/CudaGpuRegistry.h>
#include <U2Algorithm/SmithWatermanResult.h>
#include <U2Algorithm/SubstMatrixRegistry.h>

#include <QMutexLocker>
#include <QMap>
#include <QVariant>

using namespace std;

const double B_TO_MB_FACTOR = 1048576.0;

namespace U2 {

const QString PairwiseAlignmentSmithWatermanTaskSettings::PA_SW_GAP_OPEN("SW_gapOpen");
const QString PairwiseAlignmentSmithWatermanTaskSettings::PA_SW_GAP_EXTD("SW_gapExtd");
const QString PairwiseAlignmentSmithWatermanTaskSettings::PA_SW_SCORING_MATRIX_NAME("SW_scoringMatrix");
const QString PairwiseAlignmentSmithWatermanTaskSettings::PA_SW_REALIZATION_NAME("SW_realizationName");
const QString PairwiseAlignmentSmithWatermanTaskSettings::PA_SW_DEFAULT_RESULT_FILTER("filter-intersections");

SWAlgorithmTask::SWAlgorithmTask(const SmithWatermanSettings& s,
                                 const QString& taskName, SW_AlgType _algType):Task (taskName, TaskFlag_NoRun),
                                 sWatermanConfig(s)
{
    GCOUNTER( cvar, tvar, "SWAlgorithmTask" );

    algType = _algType;
    if (algType == SW_sse2) {
        if (sWatermanConfig.ptrn.length() < 8) {
            algType = SW_classic;
        }
    }

    int maxScore = calculateMaxScore(s.ptrn, s.pSm);

    minScore = (maxScore * s.percentOfScore) / 100;
    if ( (maxScore * (int)s.percentOfScore) % 100 != 0) minScore += 1;

    //acquiring resources for GPU computations
    if( SW_cuda == algType ) {
        addTaskResource(TaskResourceUsage( RESOURCE_CUDA_GPU, 1, true /*prepareStage*/));
    } else if( SW_opencl == algType ) {
        addTaskResource(TaskResourceUsage( RESOURCE_OPENCL_GPU, 1, true /*prepareStage*/));
    }

    setupTask(maxScore);
}

SWAlgorithmTask::~SWAlgorithmTask() {
    delete sWatermanConfig.resultListener;
    delete sWatermanConfig.resultCallback;
    // we do not delete resultFilter here, because filters are stored in special registry
}

void SWAlgorithmTask::setupTask(int maxScore) {

    SequenceWalkerConfig c;
    c.seq = sWatermanConfig.sqnc.constData();
    c.seqSize = sWatermanConfig.sqnc.size();
    c.range = sWatermanConfig.globalRegion;
    c.complTrans = sWatermanConfig.complTT;
    c.aminoTrans = sWatermanConfig.aminoTT;
    c.strandToWalk = sWatermanConfig.strand;
    algoLog.details(QString("Strand: %1 ").arg(c.strandToWalk));

    quint64 overlapSize = calculateMatrixLength(sWatermanConfig.sqnc.length(),
        sWatermanConfig.ptrn.length() * (sWatermanConfig.aminoTT == NULL ? 1 : 3),
        sWatermanConfig.gapModel.scoreGapOpen,
        sWatermanConfig.gapModel.scoreGapExtd,
        maxScore,
        minScore);

    // divide sequence by PARTS_NUMBER parts
    int idealThreadCount = AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount();

    qint64 partsNumber = 0;
    double computationMatrixSquare = 0.0;

    switch(algType) {
        case SW_sse2:
            computationMatrixSquare = 1619582300.0; //this constant is considered to be optimal computation matrix square (square = localSequence.length * pattern.length) for given algorithm realization and the least minimum score value
            c.nThreads = idealThreadCount * 2.5;
            break;
        case SW_classic:
            computationMatrixSquare = 751948900.29; //the same as previous
            c.nThreads = idealThreadCount;
            break;
        case SW_cuda:
        case SW_opencl:
            computationMatrixSquare = 58484916.67; //the same as previous
            c.nThreads = 1;
            break;
        default:
            assert(0);
    }

    c.walkCircular = sWatermanConfig.searchCircular;
    c.walkCircularDistance = c.walkCircular ? sWatermanConfig.ptrn.size() - 1 : 0;

    partsNumber = static_cast<qint64>((sWatermanConfig.sqnc.size() + c.walkCircularDistance) / (computationMatrixSquare / sWatermanConfig.ptrn.size()) + 1.0);
    if(partsNumber < c.nThreads) {
        c.nThreads = partsNumber;
    }

    c.chunkSize = (c.seqSize + c.walkCircularDistance + overlapSize * (partsNumber - 1)) / partsNumber;
    if (c.chunkSize <= overlapSize) {
        c.chunkSize = overlapSize + 1;
    }
    if (c.chunkSize < (quint64)sWatermanConfig.ptrn.length() * (sWatermanConfig.aminoTT == NULL ? 1 : 3)) {
        c.chunkSize = sWatermanConfig.ptrn.length() * (sWatermanConfig.aminoTT == NULL ? 1 : 3);
    }

    c.overlapSize = overlapSize;

    c.lastChunkExtraLen = partsNumber - 1;

    //acquiring memory resources for computations
    switch(algType) {
        case SW_cuda:
#ifdef SW2_BUILD_WITH_CUDA
            addTaskResource(TaskResourceUsage( RESOURCE_MEMORY,
                SmithWatermanAlgorithmCUDA::estimateNeededRamAmount(sWatermanConfig.pSm, sWatermanConfig.ptrn,
                    sWatermanConfig.sqnc.left(c.chunkSize * c.nThreads), sWatermanConfig.resultView),
                true));
#endif
            break;
        case SW_opencl:
#ifdef SW2_BUILD_WITH_OPENCL
            addTaskResource(TaskResourceUsage( RESOURCE_MEMORY,
                SmithWatermanAlgorithmOPENCL::estimateNeededRamAmount(sWatermanConfig.pSm, sWatermanConfig.ptrn,
                    sWatermanConfig.sqnc.left(c.chunkSize * c.nThreads), sWatermanConfig.resultView),
                true));
#endif
            break;
        case SW_classic:
            addTaskResource(TaskResourceUsage(RESOURCE_MEMORY,
                SmithWatermanAlgorithm::estimateNeededRamAmount(sWatermanConfig.gapModel.scoreGapOpen,
                    sWatermanConfig.gapModel.scoreGapExtd, minScore, maxScore, sWatermanConfig.ptrn,
                    sWatermanConfig.sqnc.left(c.chunkSize * c.nThreads), sWatermanConfig.resultView),
                true));
            break;
        case SW_sse2:
#ifdef SW2_BUILD_WITH_SSE2
            addTaskResource(TaskResourceUsage(RESOURCE_MEMORY,
                SmithWatermanAlgorithmSSE2::estimateNeededRamAmount(sWatermanConfig.ptrn,
                    sWatermanConfig.sqnc.left(c.chunkSize * c.nThreads), sWatermanConfig.gapModel.scoreGapOpen,
                    sWatermanConfig.gapModel.scoreGapExtd, minScore, maxScore, sWatermanConfig.resultView),
                true));
#endif
            break;
        default:
            assert(0);
    }

    t = new SequenceWalkerTask(c, this, tr("Smith Waterman2 SequenceWalker"));
    addSubTask(t);
}

void SWAlgorithmTask::prepare() {
    if( SW_cuda == algType ) {
        cudaGpu = AppContext::getCudaGpuRegistry()->acquireAnyReadyGpu();
        assert( cudaGpu );
#ifdef SW2_BUILD_WITH_CUDA
        const SequenceWalkerConfig & config = t->getConfig();
        quint64 needMemBytes = SmithWatermanAlgorithmCUDA::estimateNeededGpuMemory(
                sWatermanConfig.pSm, sWatermanConfig.ptrn, sWatermanConfig.sqnc.left(config.chunkSize * config.nThreads), sWatermanConfig.resultView);
        quint64 gpuMemBytes = cudaGpu->getGlobalMemorySizeBytes();
        if( gpuMemBytes < needMemBytes ) {
            stateInfo.setError( tr("Not enough memory on CUDA-enabled device. "
                "The space required is %1 bytes, but only %2 bytes are available. Device id: %3, device name: %4").
                                arg(QString::number(needMemBytes), QString::number(gpuMemBytes), QString::number(cudaGpu->getId()), QString(cudaGpu->getName()))
                              );
            return;
        } else {
            algoLog.details( tr("The Smith-Waterman search allocates ~%1 bytes (%2 Mb) on CUDA device").
                arg(QString::number(needMemBytes), QString::number(needMemBytes / B_TO_MB_FACTOR)) );
        }

        coreLog.details(QString("GPU model: %1").arg(cudaGpu->getName()));

        cudaSetDevice( cudaGpu->getId() );
#else
        assert(false);
#endif
    }
    else if(SW_opencl == algType) {
#ifdef SW2_BUILD_WITH_OPENCL
        openClGpu = AppContext::getOpenCLGpuRegistry()->acquireEnabledGpuIfReady();
        SAFE_POINT(nullptr != openClGpu, "GPU isn't ready, abort.", );

        const SequenceWalkerConfig & config = t->getConfig();
        const quint64 needMemBytes = SmithWatermanAlgorithmOPENCL::estimateNeededGpuMemory(
            sWatermanConfig.pSm, sWatermanConfig.ptrn, sWatermanConfig.sqnc.left(config.chunkSize * config.nThreads));
        const quint64 gpuMemBytes = openClGpu->getGlobalMemorySizeBytes();

        if(gpuMemBytes < needMemBytes) {
            stateInfo.setError(QString("Not enough memory on OpenCL-enabled device. "
                "The space required is %1 bytes, but only %2 bytes are available. Device id: %3, device name: %4").
                arg(QString::number(needMemBytes), QString::number(gpuMemBytes), QString::number((qlonglong)openClGpu->getId()), QString(openClGpu->getName())));
            return;
        } else {
            algoLog.details(QString("The Smith-Waterman search allocates ~%1 bytes (%2 Mb) on OpenCL device").
                arg(QString::number(needMemBytes), QString::number(needMemBytes / B_TO_MB_FACTOR)));
        }

        coreLog.details(QString("GPU model: %1").arg(openClGpu->getName()));
#else
        assert(0);
#endif
    }
}

QList<PairAlignSequences> &  SWAlgorithmTask::getResult() {
    removeResultFromOverlap(pairAlignSequences);
    SmithWatermanAlgorithm::sortByScore(pairAlignSequences);

    return pairAlignSequences;
}

void SWAlgorithmTask::onRegion(SequenceWalkerSubtask* t, TaskStateInfo& ti) {
    Q_UNUSED(ti);

    int regionLen = t->getRegionSequenceLen();
    QByteArray localSeq(t->getRegionSequence(), regionLen);

    SmithWatermanAlgorithm * sw = NULL;
    if (algType == SW_sse2) {
#ifdef SW2_BUILD_WITH_SSE2
        sw = new SmithWatermanAlgorithmSSE2;
#else
        coreLog.error( "SSE2 was not enabled in this build" );
        return;
#endif //SW2_BUILD_WITH_SSE2
    } else if (algType == SW_cuda) {
#ifdef SW2_BUILD_WITH_CUDA
        sw = new SmithWatermanAlgorithmCUDA;
#else
        coreLog.error( "CUDA was not enabled in this build" );
        return;
#endif //SW2_BUILD_WITH_CUDA
    } else if (algType == SW_opencl) {
#ifdef SW2_BUILD_WITH_OPENCL
        sw = new SmithWatermanAlgorithmOPENCL;
#else
        coreLog.error( "OPENCL was not enabled in this build" );
        return;
#endif //SW2_BUILD_WITH_OPENCL
    } else {
        assert(algType == SW_classic);
        sw = new SmithWatermanAlgorithm;
    }

    // this substitution is needed for the case when annotation are required as result
    // as well as pattern subsequence
    const SmithWatermanSettings::SWResultView resultView =
        ( SmithWatermanSettings::ANNOTATIONS == sWatermanConfig.resultView
        && sWatermanConfig.includePatternContent )
        ? SmithWatermanSettings::MULTIPLE_ALIGNMENT : sWatermanConfig.resultView;

    quint64 t1 = GTimer::currentTimeMicros();
    sw->launch(sWatermanConfig.pSm, sWatermanConfig.ptrn, localSeq,
        sWatermanConfig.gapModel.scoreGapOpen + sWatermanConfig.gapModel.scoreGapExtd,
        sWatermanConfig.gapModel.scoreGapExtd, minScore, resultView);
    QString algName;
    if (algType == SW_cuda) {
        algName = "CUDA";
    } else {
        algName = "Classic";
    }
    QString testName;
    if (getParentTask() != NULL) {
        testName = getParentTask()->getTaskName();
    } else {
        testName = "SW alg";
    }
    perfLog.details(QString("\n%1 %2 run time is %3\n").arg(testName).arg(algName).arg(GTimer::secsBetween(t1, GTimer::currentTimeMicros())));

    QList<PairAlignSequences> res = sw->getResults();

    for (int i = 0; i < res.size(); i++) {
        res[i].isDNAComplemented = t->isDNAComplemented();
        res[i].isAminoTranslated = t->isAminoTranslated();

        if (t->isAminoTranslated()) {
            res[i].refSubseqInterval.startPos *= 3;
            res[i].refSubseqInterval.length *= 3;
        }


        if (t->isDNAComplemented()) {
            const U2Region& wr = t->getGlobalRegion();
            res[i].refSubseqInterval.startPos =
                wr.endPos() - res[i].refSubseqInterval.endPos() - sWatermanConfig.globalRegion.startPos;
        }
        else {
            res[i].refSubseqInterval.startPos +=
                (t->getGlobalRegion().startPos - sWatermanConfig.globalRegion.startPos);
        }
    }

    addResult(res);

/////////////////////
    delete sw;
}

void SWAlgorithmTask::removeResultFromOverlap(QList<PairAlignSequences> & res) {
    for (int i = 0; i < res.size() - 1; i++) {
        for (int j = i + 1; j < res.size(); j++) {
            if (res.at(i).refSubseqInterval == res.at(j).refSubseqInterval && res.at(i).isDNAComplemented == res.at(j).isDNAComplemented) {
                if (res.at(i).score > res.at(j).score) {
                    res.removeAt(j);
                    j--;
                } else {
                    res.removeAt(i);
                    i--;
                    j=res.size();
                }

            }
        }
    }
}


void SWAlgorithmTask::addResult(QList<PairAlignSequences> & res) {
    QMutexLocker ml(&lock);
    pairAlignSequences += res;
    pairAlignSequences += res;
}

int SWAlgorithmTask::calculateMatrixLength(int searchSeqLen, int patternSeqLen, int gapOpen, int gapExtension, int maxScore, int minScore) {

    int matrixLength = 0;

    int gap = gapOpen;
    if (gapOpen < gapExtension) gap = gapExtension;

    matrixLength = patternSeqLen + (maxScore - minScore) / gap * (-1) + 1;

    if (searchSeqLen + 1 < matrixLength) matrixLength = searchSeqLen + 1;

    matrixLength += 1;

    return matrixLength;
}

int SWAlgorithmTask::calculateMaxScore(const QByteArray & seq, const SMatrix& substitutionMatrix) {
    int maxScore = 0;
    int max;
    int substValue = 0;

    QByteArray alphaChars = substitutionMatrix.getAlphabet()->getAlphabetChars();
    for (int i = 0; i < seq.length(); i++) {
        max = 0;
        for (int j = 0; j < alphaChars.size(); j++) {
            //TODO: use raw pointers!
            char c1 = seq.at(i);
            char c2 = alphaChars.at(j);
            substValue = substitutionMatrix.getScore(c1, c2);
            if (max < substValue) max = substValue;
        }
        maxScore += max;
    }
    return maxScore;
}

Task::ReportResult SWAlgorithmTask::report() {

    if( SW_cuda == algType ) {
        cudaGpu->setAcquired(false);
    } else if(SW_opencl == algType) {
#ifdef SW2_BUILD_WITH_OPENCL
        openClGpu->setAcquired(false);
#endif
    }

    SmithWatermanResultListener* rl = sWatermanConfig.resultListener;
    QList<SmithWatermanResult> resultList = rl->getResults();

    int resultsNum = resultList.size();
    algoLog.details(tr("%1 results found").arg(resultsNum));

    if (0 != sWatermanConfig.resultCallback) {
        SmithWatermanReportCallback* rcb = sWatermanConfig.resultCallback;
        QString res = rcb->report(resultList);
        if (!res.isEmpty()) {
            stateInfo.setError(res);
        }
    }

    return ReportResult_Finished;
}

QList<Task*> SWAlgorithmTask::onSubTaskFinished( Task* subTask ){
    QList<Task*> res;
    if (hasError() || isCanceled()) {
        return res;
    }

    if (subTask == t){
        res.append(new SWResultsPostprocessingTask(sWatermanConfig, resultList, getResult()));
    }
    return res;
}


SWResultsPostprocessingTask::SWResultsPostprocessingTask( SmithWatermanSettings &_sWatermanConfig,
                                                         QList<SmithWatermanResult> &_resultList,
                                                         QList<PairAlignSequences> &_resPAS )
:Task ("SWResultsPostprocessing", TaskFlag_None), sWatermanConfig(_sWatermanConfig), resultList(_resultList), resPAS(_resPAS){
}

void SWResultsPostprocessingTask::prepare(){

}

void SWResultsPostprocessingTask::run(){
    SmithWatermanResult r;
    for(QList<PairAlignSequences>::const_iterator i = resPAS.constBegin(); i != resPAS.constEnd(); ++i) {
        r.strand = (*i).isDNAComplemented ? U2Strand::Complementary : U2Strand::Direct;
        r.trans = (*i).isAminoTranslated;

        r.refSubseq = (*i).refSubseqInterval;
        r.refSubseq.startPos += sWatermanConfig.globalRegion.startPos;
        r.isJoined = false;
        if ((*i).refSubseqInterval.endPos() > sWatermanConfig.sqnc.size() && sWatermanConfig.searchCircular) {
            int t = (*i).refSubseqInterval.endPos() - sWatermanConfig.sqnc.size();
            r.refSubseq.length -= t;
            r.isJoined = true;
            r.refJoinedSubseq = U2Region(0, t);
        }
        r.ptrnSubseq = (*i).ptrnSubseqInterval;
        r.score = (*i).score;
        r.pairAlignment = (*i).pairAlignment;

        resultList.append(r);
    }

    if (0 != sWatermanConfig.resultFilter) {
        SmithWatermanResultFilter* rf = sWatermanConfig.resultFilter;
        rf->applyFilter(&resultList);
    }
    foreach( const SmithWatermanResult & r, resultList ) { /* push results after filters */
        sWatermanConfig.resultListener->pushResult(r);
    }
}

PairwiseAlignmentSmithWatermanTaskSettings::PairwiseAlignmentSmithWatermanTaskSettings(const PairwiseAlignmentTaskSettings &s) :
    PairwiseAlignmentTaskSettings(s),
    reportCallback(NULL),
    resultListener(NULL),
    resultFilter(NULL),
    gapOpen(0),
    gapExtd(0),
    percentOfScore(0) {}

PairwiseAlignmentSmithWatermanTaskSettings::~PairwiseAlignmentSmithWatermanTaskSettings() {
    //all dynamic objects in the world will be destroyed by the task
}

bool PairwiseAlignmentSmithWatermanTaskSettings::convertCustomSettings() {
    if ((customSettings.contains(PA_SW_GAP_OPEN) == false) ||
            (customSettings.contains(PA_SW_GAP_EXTD) == false) ||
            (customSettings.contains(PA_SW_SCORING_MATRIX_NAME) == false)) {
        return false;
    }
    gapOpen = customSettings.value(PA_SW_GAP_OPEN).toInt();
    gapExtd = customSettings.value(PA_SW_GAP_EXTD).toInt();
    sMatrixName = customSettings.value(PA_SW_SCORING_MATRIX_NAME).toString();
    sMatrix = AppContext::getSubstMatrixRegistry()->getMatrix(sMatrixName);
    SAFE_POINT(!sMatrix.isEmpty(), "No matrix found", false);

    PairwiseAlignmentTaskSettings::convertCustomSettings();
    return true;
}

PairwiseAlignmentSmithWatermanTask::PairwiseAlignmentSmithWatermanTask(PairwiseAlignmentSmithWatermanTaskSettings* _settings, SW_AlgType _algType) :
    PairwiseAlignmentTask(TaskFlag_NoRun), settings(_settings){
    GCOUNTER( cvar, tvar, "SWAlgorithmTask" );

    assert(settings != NULL);
    bool isValid = settings->convertCustomSettings();
    assert(isValid == true);
    Q_UNUSED(isValid);

    U2OpStatus2Log os;
    DbiConnection con(settings->msaRef.dbiRef, os);
    CHECK_OP(os, );
    U2Sequence sequence = con.dbi->getSequenceDbi()->getSequenceObject(settings->firstSequenceRef.entityId, os);
    CHECK_OP(os, );
    first = con.dbi->getSequenceDbi()->getSequenceData(sequence.id, U2Region(0, sequence.length), os);
    CHECK_OP(os, );

    sequence = con.dbi->getSequenceDbi()->getSequenceObject(settings->secondSequenceRef.entityId, os);
    CHECK_OP(os, );
    second = con.dbi->getSequenceDbi()->getSequenceData(sequence.id, U2Region(0, sequence.length), os);
    CHECK_OP(os, );
    con.close(os);

    sqnc = NULL;
    ptrn = NULL;
    if (first.length() < second.length()) {
        sqnc = &second;
        ptrn = &first;
    } else {
        sqnc = &first;
        ptrn = &second;
    }

    algType = _algType;
    if (algType == SW_sse2) {
        if (ptrn->length() < 8) {
            algType = SW_classic;
            settings->setCustomValue("realizationName", "SW_classic");
            settings->realizationName = SW_classic;
        }
    }
    SAFE_POINT(!settings->sMatrix.isEmpty(), tr("Substitution matrix is empty"), );
    maxScore = calculateMaxScore(*ptrn, settings->sMatrix);

    minScore = (maxScore * settings->percentOfScore) / 100;
    if ( (maxScore * settings->percentOfScore) % 100 != 0) {
        minScore += 1;
    }

    //acquiring resources for GPU computations
    if( SW_cuda == algType ) {
        addTaskResource(TaskResourceUsage( RESOURCE_CUDA_GPU, 1, true /*prepareStage*/));
    } else if( SW_opencl == algType ) {
        addTaskResource(TaskResourceUsage( RESOURCE_OPENCL_GPU, 1, true /*prepareStage*/));
    }

    setupTask();
}

PairwiseAlignmentSmithWatermanTask::~PairwiseAlignmentSmithWatermanTask() {
    delete settings->reportCallback;
    delete settings->resultListener;
    //result filter stored in registry, don`t delete it here
    delete settings;
}

void PairwiseAlignmentSmithWatermanTask::onRegion(SequenceWalkerSubtask *t, TaskStateInfo &ti) {
    Q_UNUSED(ti);

    int regionLen = t->getRegionSequenceLen();
    QByteArray localSeq(t->getRegionSequence(), regionLen);

    SmithWatermanAlgorithm * sw = NULL;
    if (algType == SW_sse2) {
#ifdef SW2_BUILD_WITH_SSE2
        sw = new SmithWatermanAlgorithmSSE2;
#else
        coreLog.error( "SSE2 was not enabled in this build" );
        return;
#endif //SW2_BUILD_WITH_SSE2
    } else if (algType == SW_cuda) {
#ifdef SW2_BUILD_WITH_CUDA
        sw = new SmithWatermanAlgorithmCUDA;
#else
        coreLog.error( "CUDA was not enabled in this build" );
        return;
#endif //SW2_BUILD_WITH_CUDA
    } else if (algType == SW_opencl) {
#ifdef SW2_BUILD_WITH_OPENCL
        sw = new SmithWatermanAlgorithmOPENCL;
#else
        coreLog.error( "OPENCL was not enabled in this build" );
        return;
#endif //SW2_BUILD_WITH_OPENCL
    } else {
        assert(algType == SW_classic);
        sw = new SmithWatermanAlgorithm;
    }

    quint64 t1 = GTimer::currentTimeMicros();
    sw->launch(settings->sMatrix, *ptrn, localSeq, settings->gapOpen + settings->gapExtd,
        settings->gapExtd, minScore, SmithWatermanSettings::MULTIPLE_ALIGNMENT);
    QString algName;
    if (algType == SW_cuda) {
        algName = "CUDA";
    } else {
        algName = "Classic";
    }
    QString testName;
    if (getParentTask() != NULL) {
        testName = getParentTask()->getTaskName();
    } else {
        testName = "SW alg";
    }
    perfLog.details(QString("\n%1 %2 run time is %3\n").arg(testName).arg(algName).arg(GTimer::secsBetween(t1, GTimer::currentTimeMicros())));

    QList<PairAlignSequences> res = sw->getResults();
    res = expandResults(res);

    for (int i = 0; i < res.size(); i++) {
        res[i].isDNAComplemented = t->isDNAComplemented();
        res[i].isAminoTranslated = t->isAminoTranslated();

        if (t->isAminoTranslated()) {
            res[i].refSubseqInterval.startPos *= 3;
            res[i].refSubseqInterval.length *= 3;
        }


        if (t->isDNAComplemented()) {
            const U2Region& wr = t->getGlobalRegion();
            res[i].refSubseqInterval.startPos =
                wr.endPos() - res[i].refSubseqInterval.endPos();
        }
        else {
            res[i].refSubseqInterval.startPos +=
                (t->getGlobalRegion().startPos);
        }
    }

    addResult(res);

/////////////////////
    delete sw;
}

int PairwiseAlignmentSmithWatermanTask::calculateMaxScore(const QByteArray &seq, const SMatrix& substitutionMatrix) {
    int maxScore = 0;
    int max;
    int substValue = 0;

    QByteArray alphaChars = substitutionMatrix.getAlphabet()->getAlphabetChars();
    for (int i = 0; i < seq.length(); i++) {
        max = 0;
        for (int j = 0; j < alphaChars.size(); j++) {
            //TODO: use raw pointers!
            char c1 = seq.at(i);
            char c2 = alphaChars.at(j);
            substValue = substitutionMatrix.getScore(c1, c2);
            if (max < substValue) max = substValue;
        }
        maxScore += max;
    }
    return maxScore;
}

void PairwiseAlignmentSmithWatermanTask::setupTask() {
    SequenceWalkerConfig c;
    c.seq = *sqnc;
    c.seqSize = sqnc->size();
    c.range = U2Region(0, sqnc->size());
    c.complTrans = NULL;
    c.aminoTrans = NULL;
    c.strandToWalk = StrandOption_DirectOnly;

    quint64 overlapSize = calculateMatrixLength(*sqnc, *ptrn, settings->gapOpen,
                                                settings->gapExtd, maxScore, minScore);

    // divide sequence by PARTS_NUMBER parts
    int idealThreadCount = AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount();

    qint64 partsNumber = 0;
    double computationMatrixSquare = 0.0;

    switch(algType) {
        case SW_sse2:
            computationMatrixSquare = 16195823.0; //this constant is considered to be optimal computation matrix square (square = localSequence.length * pattern.length) for given algorithm realization and the least minimum score value
            c.nThreads = idealThreadCount * 2.5;
            break;
        case SW_classic:
            computationMatrixSquare = 7519489.29; //the same as previous
            c.nThreads = idealThreadCount;
            break;
        case SW_cuda:
        case SW_opencl:
            computationMatrixSquare = 58484916.67; //the same as previous
            c.nThreads = 1;
            break;
        default:
            assert(0);
    }

    partsNumber = static_cast<qint64>(sqnc->size() / (computationMatrixSquare / ptrn->size()) + 1.0);
    if(partsNumber < c.nThreads) {
        c.nThreads = partsNumber;
    }

    c.chunkSize = (c.seqSize + overlapSize * (partsNumber - 1)) / partsNumber;
    if (c.chunkSize <= overlapSize) {
        c.chunkSize = overlapSize + 1;
    }
    c.overlapSize = overlapSize;

    c.lastChunkExtraLen = partsNumber - 1;

    //acquiring memory resources for computations
    switch(algType) {
        case SW_cuda:
#ifdef SW2_BUILD_WITH_CUDA
            addTaskResource(TaskResourceUsage( RESOURCE_MEMORY,
                SmithWatermanAlgorithmCUDA::estimateNeededRamAmount(settings->sMatrix, *ptrn,
                    sqnc->left(c.chunkSize * c.nThreads), SmithWatermanSettings::MULTIPLE_ALIGNMENT),
                true));
#endif
            break;
        case SW_opencl:
#ifdef SW2_BUILD_WITH_OPENCL
            addTaskResource(TaskResourceUsage( RESOURCE_MEMORY,
                SmithWatermanAlgorithmOPENCL::estimateNeededRamAmount(settings->sMatrix, *ptrn,
                    sqnc->left(c.chunkSize * c.nThreads), SmithWatermanSettings::MULTIPLE_ALIGNMENT),
                true));
#endif
            break;
        case SW_classic:
            addTaskResource(TaskResourceUsage(RESOURCE_MEMORY,
                SmithWatermanAlgorithm::estimateNeededRamAmount(settings->gapOpen, settings->gapExtd,
                                                                minScore, maxScore, *ptrn,
                                                                sqnc->left(c.chunkSize * c.nThreads),
                                                                SmithWatermanSettings::MULTIPLE_ALIGNMENT),
                                                                true));
            break;
        case SW_sse2:
#ifdef SW2_BUILD_WITH_SSE2
            addTaskResource(TaskResourceUsage(RESOURCE_MEMORY,
                SmithWatermanAlgorithmSSE2::estimateNeededRamAmount(*ptrn,
                    sqnc->left(c.chunkSize * c.nThreads), settings->gapOpen, settings->gapExtd,
                    minScore, maxScore, SmithWatermanSettings::MULTIPLE_ALIGNMENT),
                true));
#endif
            break;
        default:
            assert(0);
    }

    t = new SequenceWalkerTask(c, this, tr("Smith Waterman2 SequenceWalker"));
    addSubTask(t);
}

int PairwiseAlignmentSmithWatermanTask::calculateMatrixLength(const QByteArray & searchSeq, const QByteArray & patternSeq, int gapOpen, int gapExtension, int maxScore, int minScore) {
    int matrixLength = 0;

    int gap = gapOpen;
    if (gapOpen < gapExtension) gap = gapExtension;

    matrixLength = patternSeq.length() + (maxScore - minScore)/gap * (-1) + 1;

    if (searchSeq.length() + 1 < matrixLength) matrixLength = searchSeq.length() + 1;

    matrixLength += 1;

    return matrixLength;
}

void PairwiseAlignmentSmithWatermanTask::prepare() {
    if( SW_cuda == algType ) {
        cudaGpu = AppContext::getCudaGpuRegistry()->acquireAnyReadyGpu();
        assert( cudaGpu );
#ifdef SW2_BUILD_WITH_CUDA
        const SequenceWalkerConfig & config = t->getConfig();
        quint64 needMemBytes = SmithWatermanAlgorithmCUDA::estimateNeededGpuMemory(
                settings->sMatrix, *ptrn, sqnc->left(config.chunkSize * config.nThreads), SmithWatermanSettings::MULTIPLE_ALIGNMENT);
        quint64 gpuMemBytes = cudaGpu->getGlobalMemorySizeBytes();
        if( gpuMemBytes < needMemBytes ) {
            stateInfo.setError( tr("Not enough memory on CUDA-enabled device. "
                "The space required is %1 bytes, but only %2 bytes are available. Device id: %3, device name: %4").
                                arg(QString::number(needMemBytes), QString::number(gpuMemBytes), QString::number(cudaGpu->getId()), QString(cudaGpu->getName()))
                              );
            return;
        } else {
            algoLog.details( tr("The Smith-Waterman search allocates ~%1 bytes (%2 Mb) on CUDA device").
                arg(QString::number(needMemBytes), QString::number(needMemBytes / B_TO_MB_FACTOR)) );
        }

        coreLog.details(QString("GPU model: %1").arg(cudaGpu->getName()));

        cudaSetDevice( cudaGpu->getId() );
#else
        assert(false);
#endif
    }
    else if(SW_opencl == algType) {
#ifdef SW2_BUILD_WITH_OPENCL
        openClGpu = AppContext::getOpenCLGpuRegistry()->acquireEnabledGpuIfReady();
        SAFE_POINT(nullptr != openClGpu, "GPU isn't ready, abort.", );

        const SequenceWalkerConfig & config = t->getConfig();
        const quint64 needMemBytes = SmithWatermanAlgorithmOPENCL::estimateNeededGpuMemory(
            settings->sMatrix, *ptrn, sqnc->left(config.chunkSize * config.nThreads));
        const quint64 gpuMemBytes = openClGpu->getGlobalMemorySizeBytes();

        if(gpuMemBytes < needMemBytes) {
            stateInfo.setError(QString("Not enough memory on OpenCL-enabled device. "
                "The space required is %1 bytes, but only %2 bytes are available. Device id: %3, device name: %4").
                arg(QString::number(needMemBytes), QString::number(gpuMemBytes), QString::number((qlonglong)openClGpu->getId()), QString(openClGpu->getName())));
            return;
        } else {
            algoLog.details(QString("The Smith-Waterman search allocates ~%1 bytes (%2 Mb) on OpenCL device").
                arg(QString::number(needMemBytes), QString::number(needMemBytes / B_TO_MB_FACTOR)));
        }

        coreLog.details(QString("GPU model: %1").arg(openClGpu->getName()));
#else
        assert(0);
#endif
    }
}

QList<PairAlignSequences> & PairwiseAlignmentSmithWatermanTask::getResult() {
    removeResultFromOverlap(pairAlignSequences);
    SmithWatermanAlgorithm::sortByScore(pairAlignSequences);

    return pairAlignSequences;
}

Task::ReportResult PairwiseAlignmentSmithWatermanTask::report() {
    if( SW_cuda == algType ) {
        cudaGpu->setAcquired(false);
    } else if(SW_opencl == algType) {
#ifdef SW2_BUILD_WITH_OPENCL
        openClGpu->setAcquired(false);
#endif
    }

    assert(settings->resultListener != NULL);
    QList<SmithWatermanResult> resultList = settings->resultListener->getResults();

    int resultsNum = resultList.size();
    algoLog.details(tr("%1 results found").arg(resultsNum));

    if (0 != settings->reportCallback) {
        QString res = settings->reportCallback->report(resultList);
        if (!res.isEmpty()) {
            stateInfo.setError(res);
        }
    }

    return ReportResult_Finished;
}

void PairwiseAlignmentSmithWatermanTask::addResult(QList<PairAlignSequences> & res) {
    QMutexLocker ml(&lock);
    pairAlignSequences += res;
}

QList<Task*> PairwiseAlignmentSmithWatermanTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    if (hasError() || isCanceled()) {
        return res;
    }

    if (subTask == t) {
        res.append(new PairwiseAlignmentSWResultsPostprocessingTask(settings->resultFilter, settings->resultListener, resultList, getResult()));
    }
    return res;
}

void PairwiseAlignmentSmithWatermanTask::removeResultFromOverlap(QList<PairAlignSequences> & res) {
    for (int i = 0; i < res.size() - 1; i++) {
        for (int j = i + 1; j < res.size(); j++) {
            if (res.at(i).refSubseqInterval == res.at(j).refSubseqInterval && res.at(i).isDNAComplemented == res.at(j).isDNAComplemented /*&&
                    res.at(i).pairAlignment == res.at(j).pairAlignment*/) {
                if (res.at(i).score > res.at(j).score) {
                    res.removeAt(j);
                    j--;
                } else {
                    res.removeAt(i);
                    i--;
                    j=res.size();
                }

            }
        }
    }
}

QList<PairAlignSequences> PairwiseAlignmentSmithWatermanTask::expandResults(QList<PairAlignSequences>& res) {
    bool resIsEmpty = false;
    if (res.size() == 0) {
        PairAlignSequences p;
        p.refSubseqInterval.startPos = sqnc->length();
        p.refSubseqInterval.length = 0;
        p.ptrnSubseqInterval.startPos = ptrn->length();
        p.ptrnSubseqInterval.length = 0;
        res += p;
        resIsEmpty = true;
    }
        for (int i = 0; i < res.size(); i++) {
        if (res[i].ptrnSubseqInterval.length < ptrn->length() && res[i].refSubseqInterval.length < sqnc->length() &&
                resIsEmpty == false) {
            for (int j = 0; j < res[i].ptrnSubseqInterval.startPos && j < res[i].refSubseqInterval.startPos;) {
                res[i].pairAlignment.append(PairAlignSequences::DIAG);
                res[i].ptrnSubseqInterval.startPos--;
                res[i].ptrnSubseqInterval.length++;
                res[i].refSubseqInterval.startPos--;
                res[i].refSubseqInterval.length++;
            }
            for (int j = ptrn->length(); j > res[i].ptrnSubseqInterval.endPos() && j > res[i].refSubseqInterval.endPos();) {
                res[i].pairAlignment.prepend(PairAlignSequences::DIAG);
                res[i].ptrnSubseqInterval.length++;
                res[i].refSubseqInterval.length++;
            }
        }
        if (res[i].ptrnSubseqInterval.length < ptrn->length()) {
            for (int j = 0; j < res[i].ptrnSubseqInterval.startPos;) {
                res[i].pairAlignment.append(PairAlignSequences::LEFT);
                res[i].ptrnSubseqInterval.startPos--;
                res[i].ptrnSubseqInterval.length++;
            }
            for (int j = ptrn->length(); j > res[i].ptrnSubseqInterval.endPos();) {
                res[i].pairAlignment.prepend(PairAlignSequences::LEFT);
                res[i].ptrnSubseqInterval.length++;
            }
        }
        if (res[i].refSubseqInterval.length < sqnc->length()) {
            for (int j = 0; j < res[i].refSubseqInterval.startPos;) {
                res[i].pairAlignment.append(PairAlignSequences::UP);
                res[i].refSubseqInterval.startPos--;
                res[i].refSubseqInterval.length++;
            }
            for (int j = sqnc->length(); j > res[i].refSubseqInterval.endPos();) {
                res[i].pairAlignment.prepend(PairAlignSequences::UP);
                res[i].refSubseqInterval.length++;
            }
        }
    }
    return res;
}

PairwiseAlignmentSWResultsPostprocessingTask::PairwiseAlignmentSWResultsPostprocessingTask( SmithWatermanResultFilter* _rf,
                                                                                            SmithWatermanResultListener* _rl,
                                                                                            QList<SmithWatermanResult> &_resultList,
                                                                                            QList<PairAlignSequences> &_resPAS )
    :Task ("PairwiseAlignmentSWResultsPostprocessing", TaskFlag_None), rf(_rf), rl(_rl), resultList(_resultList), resPAS(_resPAS) {
}

void PairwiseAlignmentSWResultsPostprocessingTask::run(){
    SmithWatermanResult r;
    for(QList<PairAlignSequences>::const_iterator i = resPAS.constBegin(); i != resPAS.constEnd(); ++i) {
        r.strand = (*i).isDNAComplemented ? U2Strand::Complementary : U2Strand::Direct;
        r.trans = (*i).isAminoTranslated;
        r.refSubseq = (*i).refSubseqInterval;
        r.refSubseq.startPos = 0;
        r.ptrnSubseq = (*i).ptrnSubseqInterval;
        r.score = (*i).score;
        r.pairAlignment = (*i).pairAlignment;

        resultList.append(r);
    }

    if (0 != rf) {
        rf->applyFilter(&resultList);
    }
    foreach( const SmithWatermanResult & r, resultList ) { /* push results after filters */
        rl->pushResult(r);
    }
}

void PairwiseAlignmentSWResultsPostprocessingTask::prepare(){

}

} //namespace
