/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "SmithWatermanAlgorithmSSE2.h"

using namespace std;

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

    qint64 partsNumber = 0;
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
        default:
            assert(0);
    }

    c.walkCircular = sWatermanConfig.searchCircular;
    c.walkCircularDistance = c.walkCircular ? sWatermanConfig.ptrn.size() - 1 : 0;

    partsNumber = static_cast<qint64>((sWatermanConfig.sqnc.size() + c.walkCircularDistance) / (computationMatrixSquare / sWatermanConfig.ptrn.size()) + 1.0);
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
    if (neededRam > SmithWatermanAlgorithm::MEMORY_SIZE_LIMIT_MB) {
        stateInfo.setError(tr("Needed amount of memory for this task is %1 MB, but it limited to %2 MB.").arg(QString::number(neededRam)).arg(QString::number(SmithWatermanAlgorithm::MEMORY_SIZE_LIMIT_MB)));
    } else {
        addTaskResource(TaskResourceUsage(RESOURCE_MEMORY, neededRam, true));
        t = new SequenceWalkerTask(c, this, tr("Smith Waterman2 SequenceWalker"));
        addSubTask(t);
    }
}

QList<PairAlignSequences>& SWAlgorithmTask::getResult() {
    removeResultFromOverlap(pairAlignSequences);
    SmithWatermanAlgorithm::sortByScore(pairAlignSequences);

    return pairAlignSequences;
}

void SWAlgorithmTask::onRegion(SequenceWalkerSubtask* t, TaskStateInfo& ti) {
    Q_UNUSED(ti);

    int regionLen = t->getRegionSequenceLen();
    QByteArray localSeq(t->getRegionSequence(), regionLen);

    SmithWatermanAlgorithm* sw = nullptr;
    if (algType == SW_sse2) {
        sw = new SmithWatermanAlgorithmSSE2;
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
            } else {
                res[i].refSubseqInterval.startPos +=
                    (t->getGlobalRegion().startPos - sWatermanConfig.globalRegion.startPos);
            }
        }

        addResult(res);
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
    int matrixLength = 0;

    int gap = gapOpen;
    if (gapOpen < gapExtension)
        gap = gapExtension;

    matrixLength = patternSeqLen + (maxScore - minScore) / gap * (-1) + 1;

    if (searchSeqLen + 1 < matrixLength)
        matrixLength = searchSeqLen + 1;

    matrixLength += 1;

    return matrixLength;
}

int SWAlgorithmTask::calculateMaxScore(const QByteArray& seq, const SMatrix& substitutionMatrix) {
    int maxScore = 0;
    int max;
    int substValue = 0;

    QByteArray alphaChars = substitutionMatrix.getAlphabet()->getAlphabetChars();
    for (int i = 0; i < seq.length(); i++) {
        max = 0;
        for (int j = 0; j < alphaChars.size(); j++) {
            // TODO: use raw pointers!
            char c1 = seq.at(i);
            char c2 = alphaChars.at(j);
            substValue = substitutionMatrix.getScore(c1, c2);
            if (max < substValue)
                max = substValue;
        }
        maxScore += max;
    }
    return maxScore;
}

Task::ReportResult SWAlgorithmTask::report() {
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

    if (sWatermanConfig.resultFilter != 0) {
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

    SmithWatermanAlgorithm* sw = nullptr;
    if (algType == SW_sse2) {
        sw = new SmithWatermanAlgorithmSSE2;
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
            } else {
                res[i].refSubseqInterval.startPos +=
                    (t->getGlobalRegion().startPos);
            }
        }

        addResult(res);
    } else {
        stateInfo.setError(sw->getCalculationError());
    }

    /////////////////////
    delete sw;
}

int PairwiseAlignmentSmithWatermanTask::calculateMaxScore(const QByteArray& seq, const SMatrix& substitutionMatrix) {
    int maxScore = 0;
    int max;
    int substValue = 0;

    QByteArray alphaChars = substitutionMatrix.getAlphabet()->getAlphabetChars();
    for (int i = 0; i < seq.length(); i++) {
        max = 0;
        for (int j = 0; j < alphaChars.size(); j++) {
            // TODO: use raw pointers!
            char c1 = seq.at(i);
            char c2 = alphaChars.at(j);
            substValue = substitutionMatrix.getScore(c1, c2);
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

    qint64 partsNumber = 0;
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
        default:
            assert(0);
    }

    partsNumber = static_cast<qint64>(sqnc->size() / (computationMatrixSquare / ptrn->size()) + 1.0);
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
    if (neededRam > SmithWatermanAlgorithm::MEMORY_SIZE_LIMIT_MB) {
        stateInfo.setError(tr("Needed amount of memory for this task is %1 MB, but it limited to %2 MB.").arg(QString::number(neededRam)).arg(QString::number(SmithWatermanAlgorithm::MEMORY_SIZE_LIMIT_MB)));
    } else {
        addTaskResource(TaskResourceUsage(RESOURCE_MEMORY, neededRam, true));
        t = new SequenceWalkerTask(c, this, tr("Smith Waterman2 SequenceWalker"));
        addSubTask(t);
    }
}

int PairwiseAlignmentSmithWatermanTask::calculateMatrixLength(const QByteArray& searchSeq, const QByteArray& patternSeq, int gapOpen, int gapExtension, int maxScore, int minScore) {
    int matrixLength = 0;

    int gap = gapOpen;
    if (gapOpen < gapExtension)
        gap = gapExtension;

    matrixLength = patternSeq.length() + (maxScore - minScore) / gap * (-1) + 1;

    if (searchSeq.length() + 1 < matrixLength)
        matrixLength = searchSeq.length() + 1;

    matrixLength += 1;

    return matrixLength;
}

void PairwiseAlignmentSmithWatermanTask::prepare() {
}

QList<PairAlignSequences>& PairwiseAlignmentSmithWatermanTask::getResult() {
    removeResultFromOverlap(pairAlignSequences);
    SmithWatermanAlgorithm::sortByScore(pairAlignSequences);

    return pairAlignSequences;
}

Task::ReportResult PairwiseAlignmentSmithWatermanTask::report() {
    assert(settings->resultListener != nullptr);
    QList<SmithWatermanResult> resultList = settings->resultListener->getResults();

    int resultsNum = resultList.size();
    algoLog.details(tr("%1 results found").arg(resultsNum));

    if (0 != settings->reportCallback) {
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

    if (rf != 0) {
        rf->applyFilter(&resultList);
    }
    foreach (const SmithWatermanResult& r, resultList) { /* push results after filters */
        rl->pushResult(r);
    }
}

}  // namespace U2
