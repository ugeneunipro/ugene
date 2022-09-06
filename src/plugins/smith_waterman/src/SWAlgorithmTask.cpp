/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "SWAlgorithmTask.h"

#include <QMap>
#include <QMutexLocker>

#include <U2Algorithm/SmithWatermanResult.h>
#include <U2Algorithm/SubstMatrixRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/Counter.h>
#include <U2Core/Log.h>
#include <U2Core/Timer.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceDbi.h>

#include "SmithWatermanAlgorithmOPENCL.h"
#include "SmithWatermanAlgorithmSSE2.h"

using namespace std;

const double B_TO_MB_FACTOR = 1048576.0;

namespace U2 {

const QString PairwiseAlignmentSmithWatermanTaskSettings::PA_SW_GAP_OPEN("SW_gapOpen");
const QString PairwiseAlignmentSmithWatermanTaskSettings::PA_SW_GAP_EXTD("SW_gapExtd");
const QString PairwiseAlignmentSmithWatermanTaskSettings::PA_SW_SCORING_MATRIX_NAME("SW_scoringMatrix");
const QString PairwiseAlignmentSmithWatermanTaskSettings::PA_SW_REALIZATION_NAME("SW_realizationName");
const QString PairwiseAlignmentSmithWatermanTaskSettings::PA_SW_DEFAULT_RESULT_FILTER("filter-intersections");

SWAlgorithmTask::SWAlgorithmTask(const SmithWatermanSettings& s,
                                 const QString& taskName,
                                 SW_AlgType _algType)
    : Task(taskName, TaskFlag_NoRun),
      sWatermanConfig(s) {
    GCOUNTER(cvar, "SWAlgorithmTask");

    algType = _algType;
    if (algType == SW_sse2) {
        if (sWatermanConfig.ptrn.length() < 8) {
            algType = SW_classic;
        }
    }

    int maxScore = calculateMaxScore(s.ptrn, s.pSm);

    minScore = (maxScore * s.percentOfScore) / 100;
    if ((maxScore * (int)s.percentOfScore) % 100 != 0)
        minScore += 1;

    // acquiring resources for GPU computations
    if (algType == SW_opencl) {
        addTaskResource(TaskResourceUsage(RESOURCE_OPENCL_GPU, 1, true /*prepareStage*/));
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
                                                sWatermanConfig.ptrn.length() * (sWatermanConfig.aminoTT == nullptr ? 1 : 3),
                                                sWatermanConfig.gapModel.scoreGapOpen,
                                                sWatermanConfig.gapModel.scoreGapExtd,
                                                maxScore,
                                                minScore);

    // divide sequence by PARTS_NUMBER parts
    int idealThreadCount = AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount();

    double computationMatrixSquare = 0.0;

    switch (algType) {
        case SW_sse2:
            computationMatrixSquare = 1619582300.0;  // this constant is considered to be optimal computation matrix square (square = localSequence.length * pattern.length) for given algorithm realization and the least minimum score value
            c.nThreads = idealThreadCount * 2.5;
            break;
        case SW_classic:
            computationMatrixSquare = 751948900.29;  // the same as previous
            c.nThreads = idealThreadCount;
            break;
        case SW_opencl:
            computationMatrixSquare = 58484916.67;  // the same as previous
            c.nThreads = 1;
            break;
        default:
            assert(0);
    }

    c.walkCircular = sWatermanConfig.searchCircular;
    c.walkCircularDistance = c.walkCircular ? sWatermanConfig.ptrn.size() - 1 : 0;

    int partsNumber = static_cast<qint64>((sWatermanConfig.sqnc.size() + c.walkCircularDistance) / (computationMatrixSquare / sWatermanConfig.ptrn.size()) + 1.0);
    if (partsNumber < c.nThreads) {
        c.nThreads = partsNumber;
    }

    c.chunkSize = (c.seqSize + c.walkCircularDistance + overlapSize * (partsNumber - 1)) / partsNumber;
    if (c.chunkSize <= overlapSize) {
        c.chunkSize = overlapSize + 1;
    }
    if (c.chunkSize < (quint64)sWatermanConfig.ptrn.length() * (sWatermanConfig.aminoTT == nullptr ? 1 : 3)) {
        c.chunkSize = sWatermanConfig.ptrn.length() * (sWatermanConfig.aminoTT == nullptr ? 1 : 3);
    }

    c.overlapSize = overlapSize;

    c.lastChunkExtraLen = partsNumber - 1;

    // acquiring memory resources for computations
    quint64 neededRam = 0;
    switch (algType) {
        case SW_opencl:
#ifdef SW2_BUILD_WITH_OPENCL
            neededRam = SmithWatermanAlgorithmOPENCL::estimateNeededRamAmount(sWatermanConfig.pSm, sWatermanConfig.ptrn, sWatermanConfig.sqnc.left(c.chunkSize * c.nThreads), sWatermanConfig.resultView);
#endif
            break;
        case SW_classic:
            neededRam = SmithWatermanAlgorithm::estimateNeededRamAmount(sWatermanConfig.gapModel.scoreGapOpen,
                                                                        sWatermanConfig.gapModel.scoreGapExtd,
                                                                        minScore,
                                                                        maxScore,
                                                                        sWatermanConfig.ptrn,
                                                                        sWatermanConfig.sqnc.left(c.chunkSize * c.nThreads),
                                                                        sWatermanConfig.resultView);
            break;
        case SW_sse2:
            neededRam = SmithWatermanAlgorithmSSE2::estimateNeededRamAmount(sWatermanConfig.ptrn,
                                                                            sWatermanConfig.sqnc.left(c.chunkSize * c.nThreads),
                                                                            sWatermanConfig.gapModel.scoreGapOpen,
                                                                            sWatermanConfig.gapModel.scoreGapExtd,
                                                                            minScore,
                                                                            maxScore,
                                                                            sWatermanConfig.resultView);
            break;
        default:
            assert(0);
    }
    if (neededRam > SmithWatermanAlgorithm::MEMORY_SIZE_LIMIT_MB && algType != SW_opencl) {
        stateInfo.setError(tr("Needed amount of memory for this task is %1 MB, but it limited to %2 MB.").arg(QString::number(neededRam)).arg(QString::number(SmithWatermanAlgorithm::MEMORY_SIZE_LIMIT_MB)));
    } else {
        addTaskResource(TaskResourceUsage(RESOURCE_MEMORY, neededRam, true));
        t = new SequenceWalkerTask(c, this, tr("Smith Waterman2 SequenceWalker"));
        addSubTask(t);
    }
}

void SWAlgorithmTask::prepare() {
    if (SW_opencl == algType) {
#ifdef SW2_BUILD_WITH_OPENCL
        openClGpu = AppContext::getOpenCLGpuRegistry()->acquireEnabledGpuIfReady();
        SAFE_POINT(nullptr != openClGpu, "GPU isn't ready, abort.", );

        const SequenceWalkerConfig& config = t->getConfig();
        const quint64 needMemBytes = SmithWatermanAlgorithmOPENCL::estimateNeededGpuMemory(
            sWatermanConfig.pSm, sWatermanConfig.ptrn, sWatermanConfig.sqnc.left(config.chunkSize * config.nThreads));
        const quint64 gpuMemBytes = openClGpu->getGlobalMemorySizeBytes();

        if (gpuMemBytes < needMemBytes) {
            stateInfo.setError(QString("Not enough memory on OpenCL-enabled device. "
                                       "The space required is %1 bytes, but only %2 bytes are available. Device id: %3, device name: %4")
                                   .arg(QString::number(needMemBytes), QString::number(gpuMemBytes), QString::number((qlonglong)openClGpu->getId()), QString(openClGpu->getName())));
            return;
        } else {
            algoLog.details(QString("The Smith-Waterman search allocates ~%1 bytes (%2 Mb) on OpenCL device").arg(QString::number(needMemBytes), QString::number(needMemBytes / B_TO_MB_FACTOR)));
        }

        coreLog.details(QString("GPU model: %1").arg(openClGpu->getName()));
#else
        assert(0);
#endif
    }
}

QList<PairAlignSequences>& SWAlgorithmTask::getResult() {
    removeResultFromOverlap(pairAlignSequences);
    SmithWatermanAlgorithm::sortByScore(pairAlignSequences);

    return pairAlignSequences;
}

void SWAlgorithmTask::onRegion(SequenceWalkerSubtask* t, TaskStateInfo&) {
    int regionLen = t->getRegionSequenceLen();
    QByteArray localSeq(t->getRegionSequence(), regionLen);

    SmithWatermanAlgorithm* sw;
    if (algType == SW_sse2) {
        sw = new SmithWatermanAlgorithmSSE2;
    } else if (algType == SW_opencl) {
#ifdef SW2_BUILD_WITH_OPENCL
        sw = new SmithWatermanAlgorithmOPENCL;
#else
        coreLog.error("OPENCL was not enabled in this build");
        return;
#endif  // SW2_BUILD_WITH_OPENCL
    } else {
        assert(algType == SW_classic);
        sw = new SmithWatermanAlgorithm;
    }

    // this substitution is needed for the case when annotation are required as result
    // as well as pattern subsequence
    const SmithWatermanSettings::SWResultView resultView =
        (SmithWatermanSettings::ANNOTATIONS == sWatermanConfig.resultView && sWatermanConfig.includePatternContent) ? SmithWatermanSettings::MULTIPLE_ALIGNMENT : sWatermanConfig.resultView;

    quint64 t1 = GTimer::currentTimeMicros();
    sw->launch(sWatermanConfig.pSm, sWatermanConfig.ptrn, localSeq, sWatermanConfig.gapModel.scoreGapOpen + sWatermanConfig.gapModel.scoreGapExtd, sWatermanConfig.gapModel.scoreGapExtd, minScore, resultView);
    QString algName = "Classic";
    QString testName;
    if (getParentTask() != nullptr) {
        testName = getParentTask()->getTaskName();
    } else {
        testName = "SW alg";
    }
    perfLog.details(QString("\n%1 %2 run time is %3\n").arg(testName).arg(algName).arg(GTimer::secsBetween(t1, GTimer::currentTimeMicros())));
    if (sw->getCalculationError().isEmpty()) {
        QList<PairAlignSequences> results = sw->getResults();

        for (auto result : qAsConst(results)) {
            result.isDNAComplemented = t->isDNAComplemented();
            result.isAminoTranslated = t->isAminoTranslated();

            if (t->isAminoTranslated()) {
                result.refSubseqInterval.startPos *= 3;
                result.refSubseqInterval.length *= 3;
            }

            if (t->isDNAComplemented()) {
                const U2Region& wr = t->getGlobalRegion();
                result.refSubseqInterval.startPos =
                    wr.endPos() - result.refSubseqInterval.endPos() - sWatermanConfig.globalRegion.startPos;
            } else {
                result.refSubseqInterval.startPos +=
                    (t->getGlobalRegion().startPos - sWatermanConfig.globalRegion.startPos);
            }
        }

        addResult(results);
    } else {
        stateInfo.setError(sw->getCalculationError());
    }

    /////////////////////
    delete sw;
}

void SWAlgorithmTask::removeResultFromOverlap(QList<PairAlignSequences>& res) {
    for (int i = 0; i < res.size() - 1; i++) {
        for (int j = i + 1; j < res.size(); j++) {
            if (res.at(i).refSubseqInterval == res.at(j).refSubseqInterval && res.at(i).isDNAComplemented == res.at(j).isDNAComplemented) {
                if (res.at(i).score > res.at(j).score) {
                    res.removeAt(j);
                    j--;
                } else {
                    res.removeAt(i);
                    i--;
                    j = res.size();
                }
            }
        }
    }
}

void SWAlgorithmTask::addResult(QList<PairAlignSequences>& res) {
    QMutexLocker ml(&lock);
    pairAlignSequences += res;
    pairAlignSequences += res;
}

int SWAlgorithmTask::calculateMatrixLength(int searchSeqLen, int patternSeqLen, int gapOpen, int gapExtension, int maxScore, int minScore) {
    int gap = gapOpen;
    if (gapOpen < gapExtension) {
        gap = gapExtension;
    }

    int matrixLength = patternSeqLen + (maxScore - minScore) / gap * (-1) + 1;

    if (searchSeqLen + 1 < matrixLength)
        matrixLength = searchSeqLen + 1;

    matrixLength += 1;

    return matrixLength;
}

int SWAlgorithmTask::calculateMaxScore(const QByteArray& sequence, const SMatrix& substitutionMatrix) {
    int maxScore = 0;
    QByteArray alphaChars = substitutionMatrix.getAlphabet()->getAlphabetChars();
    for (char charInSequence : qAsConst(sequence)) {
        int max = 0;
        for (char charInAlphabet : qAsConst(alphaChars)) {
            // TODO: use raw pointers!
            int substValue = substitutionMatrix.getScore(charInSequence, charInAlphabet);
            if (max < substValue)
                max = substValue;
        }
        maxScore += max;
    }
    return maxScore;
}

Task::ReportResult SWAlgorithmTask::report() {
    if (algType == SW_opencl) {
#ifdef SW2_BUILD_WITH_OPENCL
        openClGpu->setAcquired(false);
#endif
    }

    SmithWatermanResultListener* rl = sWatermanConfig.resultListener;
    QList<SmithWatermanResult> resultList = rl->getResults();

    int resultsNum = resultList.size();
    algoLog.details(tr("%1 results found").arg(resultsNum));

    if (sWatermanConfig.resultCallback != nullptr) {
        SmithWatermanReportCallback* rcb = sWatermanConfig.resultCallback;
        QString res = rcb->report(resultList);
        if (!res.isEmpty()) {
            stateInfo.setError(res);
        }
    }

    return ReportResult_Finished;
}

QList<Task*> SWAlgorithmTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    if (hasError() || isCanceled()) {
        return res;
    }

    if (subTask == t) {
        res.append(new SWResultsPostprocessingTask(sWatermanConfig, resultList, getResult()));
    }
    return res;
}

SWResultsPostprocessingTask::SWResultsPostprocessingTask(SmithWatermanSettings& _sWatermanConfig,
                                                         QList<SmithWatermanResult>& _resultList,
                                                         QList<PairAlignSequences>& _resPAS)
    : Task("SWResultsPostprocessing", TaskFlag_None), sWatermanConfig(_sWatermanConfig), resultList(_resultList), resPAS(_resPAS) {
}

void SWResultsPostprocessingTask::prepare() {
}

void SWResultsPostprocessingTask::run() {
    for (QList<PairAlignSequences>::const_iterator i = resPAS.constBegin(); i != resPAS.constEnd(); ++i) {
        SmithWatermanResult r;
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

    if (sWatermanConfig.resultFilter != nullptr) {
        SmithWatermanResultFilter* rf = sWatermanConfig.resultFilter;
        rf->applyFilter(&resultList);
    }
    for (const SmithWatermanResult& r : qAsConst(resultList)) { /* push results after filters */
        sWatermanConfig.resultListener->pushResult(r);
    }
}

PairwiseAlignmentSmithWatermanTaskSettings::PairwiseAlignmentSmithWatermanTaskSettings(const PairwiseAlignmentTaskSettings& s)
    : PairwiseAlignmentTaskSettings(s),
      reportCallback(nullptr),
      resultListener(nullptr),
      resultFilter(nullptr),
      gapOpen(0),
      gapExtd(0),
      percentOfScore(0) {
}

bool PairwiseAlignmentSmithWatermanTaskSettings::convertCustomSettings() {
    if (!customSettings.contains(PA_SW_GAP_OPEN) ||
        !customSettings.contains(PA_SW_GAP_EXTD) ||
        !customSettings.contains(PA_SW_SCORING_MATRIX_NAME)) {
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

PairwiseAlignmentSmithWatermanTask::PairwiseAlignmentSmithWatermanTask(PairwiseAlignmentSmithWatermanTaskSettings* _settings, SW_AlgType _algType)
    : PairwiseAlignmentTask(TaskFlags_NR_FOSE_COSC), settings(_settings) {
    GCOUNTER(cvar, "SWAlgorithmTask");

    assert(settings != nullptr);
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

    sqnc = nullptr;
    ptrn = nullptr;
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
    if ((maxScore * settings->percentOfScore) % 100 != 0) {
        minScore += 1;
    }

    // acquiring resources for GPU computations
    if (algType == SW_opencl) {
        addTaskResource(TaskResourceUsage(RESOURCE_OPENCL_GPU, 1, true /*prepareStage*/));
    }

    setupTask();
}

PairwiseAlignmentSmithWatermanTask::~PairwiseAlignmentSmithWatermanTask() {
    delete settings->reportCallback;
    delete settings->resultListener;
    // result filter stored in registry, don`t delete it here
    delete settings;
}

void PairwiseAlignmentSmithWatermanTask::onRegion(SequenceWalkerSubtask* t, TaskStateInfo& ti) {
    Q_UNUSED(ti);

    int regionLen = t->getRegionSequenceLen();
    QByteArray localSeq(t->getRegionSequence(), regionLen);

    SmithWatermanAlgorithm* sw;
    if (algType == SW_sse2) {
        sw = new SmithWatermanAlgorithmSSE2;
    } else if (algType == SW_opencl) {
#ifdef SW2_BUILD_WITH_OPENCL
        sw = new SmithWatermanAlgorithmOPENCL;
#else
        coreLog.error("OPENCL was not enabled in this build");
        return;
#endif  // SW2_BUILD_WITH_OPENCL
    } else {
        assert(algType == SW_classic);
        sw = new SmithWatermanAlgorithm;
    }

    quint64 t1 = GTimer::currentTimeMicros();
    sw->launch(settings->sMatrix, *ptrn, localSeq, settings->gapOpen + settings->gapExtd, settings->gapExtd, minScore, SmithWatermanSettings::MULTIPLE_ALIGNMENT);
    QString algName = "Classic";
    QString testName;
    if (getParentTask() != nullptr) {
        testName = getParentTask()->getTaskName();
    } else {
        testName = "SW alg";
    }
    perfLog.details(QString("\n%1 %2 run time is %3\n").arg(testName).arg(algName).arg(GTimer::secsBetween(t1, GTimer::currentTimeMicros())));
    if (sw->getCalculationError().isEmpty()) {
        QList<PairAlignSequences> results = sw->getResults();
        results = expandResults(results);

        for (auto result : qAsConst(results)) {
            result.isDNAComplemented = t->isDNAComplemented();
            result.isAminoTranslated = t->isAminoTranslated();

            if (t->isAminoTranslated()) {
                result.refSubseqInterval.startPos *= 3;
                result.refSubseqInterval.length *= 3;
            }

            if (t->isDNAComplemented()) {
                const U2Region& wr = t->getGlobalRegion();
                result.refSubseqInterval.startPos =
                    wr.endPos() - result.refSubseqInterval.endPos();
            } else {
                result.refSubseqInterval.startPos +=
                    (t->getGlobalRegion().startPos);
            }
        }

        addResult(results);
    } else {
        stateInfo.setError(sw->getCalculationError());
    }

    /////////////////////
    delete sw;
}

int PairwiseAlignmentSmithWatermanTask::calculateMaxScore(const QByteArray& sequence, const SMatrix& substitutionMatrix) {
    int maxScore = 0;
    QByteArray alphaChars = substitutionMatrix.getAlphabet()->getAlphabetChars();
    for (char charInSequence : qAsConst(sequence)) {
        int max = 0;
        for (char charInAlphabet : qAsConst(alphaChars)) {
            // TODO: use raw pointers!
            int substValue = substitutionMatrix.getScore(charInSequence, charInAlphabet);
            if (max < substValue)
                max = substValue;
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
    c.complTrans = nullptr;
    c.aminoTrans = nullptr;
    c.strandToWalk = StrandOption_DirectOnly;

    quint64 overlapSize = calculateMatrixLength(*sqnc, *ptrn, settings->gapOpen, settings->gapExtd, maxScore, minScore);

    // divide sequence by PARTS_NUMBER parts
    int idealThreadCount = AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount();

    double computationMatrixSquare = 0.0;

    switch (algType) {
        case SW_sse2:
            computationMatrixSquare = 16195823.0;  // this constant is considered to be optimal computation matrix square (square = localSequence.length * pattern.length) for given algorithm realization and the least minimum score value
            c.nThreads = idealThreadCount * 2.5;
            break;
        case SW_classic:
            computationMatrixSquare = 7519489.29;  // the same as previous
            c.nThreads = idealThreadCount;
            break;
        case SW_opencl:
            computationMatrixSquare = 58484916.67;  // the same as previous
            c.nThreads = 1;
            break;
        default:
            assert(0);
    }

    int partsNumber = static_cast<qint64>(sqnc->size() / (computationMatrixSquare / ptrn->size()) + 1.0);
    if (partsNumber < c.nThreads) {
        c.nThreads = partsNumber;
    }

    c.chunkSize = (c.seqSize + overlapSize * (partsNumber - 1)) / partsNumber;
    if (c.chunkSize <= overlapSize) {
        c.chunkSize = overlapSize + 1;
    }
    c.overlapSize = overlapSize;

    c.lastChunkExtraLen = partsNumber - 1;

    // acquiring memory resources for computations
    quint64 neededRam = 0;
    switch (algType) {
        case SW_opencl:
#ifdef SW2_BUILD_WITH_OPENCL
            neededRam = SmithWatermanAlgorithmOPENCL::estimateNeededRamAmount(settings->sMatrix,
                                                                              *ptrn,
                                                                              sqnc->left(c.chunkSize * c.nThreads),
                                                                              SmithWatermanSettings::MULTIPLE_ALIGNMENT);
#endif
            break;
        case SW_classic:
            neededRam = SmithWatermanAlgorithm::estimateNeededRamAmount(settings->gapOpen,
                                                                        settings->gapExtd,
                                                                        minScore,
                                                                        maxScore,
                                                                        *ptrn,
                                                                        sqnc->left(c.chunkSize * c.nThreads),
                                                                        SmithWatermanSettings::MULTIPLE_ALIGNMENT);
            break;
        case SW_sse2:
            neededRam = SmithWatermanAlgorithmSSE2::estimateNeededRamAmount(*ptrn,
                                                                            sqnc->left(c.chunkSize * c.nThreads),
                                                                            settings->gapOpen,
                                                                            settings->gapExtd,
                                                                            minScore,
                                                                            maxScore,
                                                                            SmithWatermanSettings::MULTIPLE_ALIGNMENT);
            break;
        default:
            assert(0);
    }
    if (neededRam > SmithWatermanAlgorithm::MEMORY_SIZE_LIMIT_MB && algType != SW_opencl) {
        stateInfo.setError(tr("Needed amount of memory for this task is %1 MB, but it limited to %2 MB.").arg(QString::number(neededRam)).arg(QString::number(SmithWatermanAlgorithm::MEMORY_SIZE_LIMIT_MB)));
    } else {
        addTaskResource(TaskResourceUsage(RESOURCE_MEMORY, neededRam, true));
        t = new SequenceWalkerTask(c, this, tr("Smith Waterman2 SequenceWalker"));
        addSubTask(t);
    }
}

int PairwiseAlignmentSmithWatermanTask::calculateMatrixLength(const QByteArray& searchSeq, const QByteArray& patternSeq, int gapOpen, int gapExtension, int maxScore, int minScore) {
    int gap = gapOpen;
    if (gapOpen < gapExtension) {
        gap = gapExtension;
    }

    int matrixLength = patternSeq.length() + (maxScore - minScore) / gap * (-1) + 1;

    if (searchSeq.length() + 1 < matrixLength) {
        matrixLength = searchSeq.length() + 1;
    }

    matrixLength += 1;

    return matrixLength;
}

void PairwiseAlignmentSmithWatermanTask::prepare() {
    if (algType == SW_opencl) {
#ifdef SW2_BUILD_WITH_OPENCL
        openClGpu = AppContext::getOpenCLGpuRegistry()->acquireEnabledGpuIfReady();
        SAFE_POINT(nullptr != openClGpu, "GPU isn't ready, abort.", );

        const SequenceWalkerConfig& config = t->getConfig();
        const quint64 needMemBytes = SmithWatermanAlgorithmOPENCL::estimateNeededGpuMemory(
            settings->sMatrix, *ptrn, sqnc->left(config.chunkSize * config.nThreads));
        const quint64 gpuMemBytes = openClGpu->getGlobalMemorySizeBytes();

        if (gpuMemBytes < needMemBytes) {
            stateInfo.setError(QString("Not enough memory on OpenCL-enabled device. "
                                       "The space required is %1 bytes, but only %2 bytes are available. Device id: %3, device name: %4")
                                   .arg(QString::number(needMemBytes), QString::number(gpuMemBytes), QString::number((qlonglong)openClGpu->getId()), QString(openClGpu->getName())));
            return;
        } else {
            algoLog.details(QString("The Smith-Waterman search allocates ~%1 bytes (%2 Mb) on OpenCL device").arg(QString::number(needMemBytes), QString::number(needMemBytes / B_TO_MB_FACTOR)));
        }

        coreLog.details(QString("GPU model: %1").arg(openClGpu->getName()));
#else
        assert(0);
#endif
    }
}

QList<PairAlignSequences>& PairwiseAlignmentSmithWatermanTask::getResult() {
    removeResultFromOverlap(pairAlignSequences);
    SmithWatermanAlgorithm::sortByScore(pairAlignSequences);

    return pairAlignSequences;
}

Task::ReportResult PairwiseAlignmentSmithWatermanTask::report() {
    if (algType == SW_opencl) {
#ifdef SW2_BUILD_WITH_OPENCL
        openClGpu->setAcquired(false);
#endif
    }

    assert(settings->resultListener != nullptr);
    QList<SmithWatermanResult> resultList = settings->resultListener->getResults();

    int resultsNum = resultList.size();
    algoLog.details(tr("%1 results found").arg(resultsNum));

    if (settings->reportCallback != nullptr) {
        QString res = settings->reportCallback->report(resultList);
        if (!res.isEmpty() && !stateInfo.hasError()) {
            stateInfo.setError(res);
        }
    }

    return ReportResult_Finished;
}

void PairwiseAlignmentSmithWatermanTask::addResult(QList<PairAlignSequences>& res) {
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

void PairwiseAlignmentSmithWatermanTask::removeResultFromOverlap(QList<PairAlignSequences>& res) {
    for (int i = 0; i < res.size() - 1; i++) {
        for (int j = i + 1; j < res.size(); j++) {
            if (res.at(i).refSubseqInterval == res.at(j).refSubseqInterval && res.at(i).isDNAComplemented == res.at(j).isDNAComplemented /*&&
                    res.at(i).pairAlignment == res.at(j).pairAlignment*/
            ) {
                if (res.at(i).score > res.at(j).score) {
                    res.removeAt(j);
                    j--;
                } else {
                    res.removeAt(i);
                    i--;
                    j = res.size();
                }
            }
        }
    }
}

QList<PairAlignSequences> PairwiseAlignmentSmithWatermanTask::expandResults(QList<PairAlignSequences>& results) {
    bool resIsEmpty = false;
    if (results.empty()) {
        PairAlignSequences p;
        p.refSubseqInterval.startPos = sqnc->length();
        p.refSubseqInterval.length = 0;
        p.ptrnSubseqInterval.startPos = ptrn->length();
        p.ptrnSubseqInterval.length = 0;
        results += p;
        resIsEmpty = true;
    }
    for (auto& result : results) {
        if (result.ptrnSubseqInterval.length < ptrn->length() && result.refSubseqInterval.length < sqnc->length() && !resIsEmpty) {
            for (int j = 0; j < result.ptrnSubseqInterval.startPos && j < result.refSubseqInterval.startPos;) {
                result.pairAlignment.append(PairAlignSequences::DIAG);
                result.ptrnSubseqInterval.startPos--;
                result.ptrnSubseqInterval.length++;
                result.refSubseqInterval.startPos--;
                result.refSubseqInterval.length++;
            }
            for (int j = ptrn->length(); j > result.ptrnSubseqInterval.endPos() && j > result.refSubseqInterval.endPos();) {
                result.pairAlignment.prepend(PairAlignSequences::DIAG);
                result.ptrnSubseqInterval.length++;
                result.refSubseqInterval.length++;
            }
        }
        if (result.ptrnSubseqInterval.length < ptrn->length()) {
            for (int j = 0; j < result.ptrnSubseqInterval.startPos;) {
                result.pairAlignment.append(PairAlignSequences::LEFT);
                result.ptrnSubseqInterval.startPos--;
                result.ptrnSubseqInterval.length++;
            }
            for (int j = ptrn->length(); j > result.ptrnSubseqInterval.endPos();) {
                result.pairAlignment.prepend(PairAlignSequences::LEFT);
                result.ptrnSubseqInterval.length++;
            }
        }
        if (result.refSubseqInterval.length < sqnc->length()) {
            for (int j = 0; j < result.refSubseqInterval.startPos;) {
                result.pairAlignment.append(PairAlignSequences::UP);
                result.refSubseqInterval.startPos--;
                result.refSubseqInterval.length++;
            }
            for (int j = sqnc->length(); j > result.refSubseqInterval.endPos();) {
                result.pairAlignment.prepend(PairAlignSequences::UP);
                result.refSubseqInterval.length++;
            }
        }
    }
    return results;
}

PairwiseAlignmentSWResultsPostprocessingTask::PairwiseAlignmentSWResultsPostprocessingTask(SmithWatermanResultFilter* _rf,
                                                                                           SmithWatermanResultListener* _rl,
                                                                                           QList<SmithWatermanResult>& _resultList,
                                                                                           QList<PairAlignSequences>& _resPAS)
    : Task("PairwiseAlignmentSWResultsPostprocessing", TaskFlag_None), rf(_rf), rl(_rl), resultList(_resultList), resPAS(_resPAS) {
}

void PairwiseAlignmentSWResultsPostprocessingTask::run() {
    for (QList<PairAlignSequences>::const_iterator i = resPAS.constBegin(); i != resPAS.constEnd(); ++i) {
        SmithWatermanResult r;
        r.strand = (*i).isDNAComplemented ? U2Strand::Complementary : U2Strand::Direct;
        r.trans = (*i).isAminoTranslated;
        r.refSubseq = (*i).refSubseqInterval;
        r.refSubseq.startPos = 0;
        r.ptrnSubseq = (*i).ptrnSubseqInterval;
        r.score = (*i).score;
        r.pairAlignment = (*i).pairAlignment;

        resultList.append(r);
    }

    if (rf != nullptr) {
        rf->applyFilter(&resultList);
    }
    foreach (const SmithWatermanResult& r, resultList) { /* push results after filters */
        rl->pushResult(r);
    }
}

void PairwiseAlignmentSWResultsPostprocessingTask::prepare() {
}

}  // namespace U2
