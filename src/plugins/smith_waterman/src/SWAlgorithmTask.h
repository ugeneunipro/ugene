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

#ifndef _U2_SW_ALGORITHM_TASK_H_
#define _U2_SW_ALGORITHM_TASK_H_

#include <U2Core/Task.h>
#include <U2Core/DNASequence.h>
#include <U2Core/SequenceWalkerTask.h>
#include <U2Algorithm/SmithWatermanSettings.h>
#include <U2Core/SMatrix.h>
#include <U2Algorithm/PairwiseAlignmentTask.h>

#include "SmithWatermanAlgorithm.h"
#include "PairAlignSequences.h"

#include <QVector>
#include <QMutex>

namespace U2 {

enum SW_AlgType {SW_classic, SW_sse2, SW_cuda, SW_opencl};

class CudaGpuModel;
class OpenCLGpuModel;

class SWAlgorithmTask : public Task, public SequenceWalkerCallback {
    Q_OBJECT
public:

    SWAlgorithmTask(const SmithWatermanSettings& s,
        const QString& taskName, SW_AlgType algType);
    ~SWAlgorithmTask();

    void prepare();

    virtual void onRegion(SequenceWalkerSubtask* t, TaskStateInfo& ti);

    QList<PairAlignSequences> & getResult();
    ReportResult report();

    QList<Task*> onSubTaskFinished(Task* subTask);

private:

    void addResult(QList<PairAlignSequences> & res);
    int calculateMatrixLength(int searchSeqLen, int patternLen, int gapOpen, int gapExtension, int maxScore, int minScore);
    void removeResultFromOverlap(QList<PairAlignSequences> & res);
    int calculateMaxScore(const QByteArray & seq, const SMatrix& substitutionMatrix);

    void setupTask(int maxScore);

    QList<PairAlignSequences> pairAlignSequences;
    int minScore;

    QMutex lock;

    SW_AlgType algType;

    QList<SmithWatermanResult> resultList;
    SmithWatermanSettings sWatermanConfig;
    SequenceWalkerTask* t;

    CudaGpuModel * cudaGpu;
    OpenCLGpuModel * openClGpu;
};

class SWResultsPostprocessingTask : public Task{
    Q_OBJECT
public:
    SWResultsPostprocessingTask(SmithWatermanSettings &_sWatermanConfig, QList<SmithWatermanResult> &_resultList, QList<PairAlignSequences> &_resPAS);
    ~SWResultsPostprocessingTask(){}

    void prepare();
    void run();
    ReportResult report(){return ReportResult_Finished;}
private:
    SmithWatermanSettings sWatermanConfig;
    QList<SmithWatermanResult> resultList;
    QList<PairAlignSequences> resPAS;
};

class PairwiseAlignmentSmithWatermanTaskSettings : public PairwiseAlignmentTaskSettings {
public:
    PairwiseAlignmentSmithWatermanTaskSettings(const PairwiseAlignmentTaskSettings &s);
    virtual ~PairwiseAlignmentSmithWatermanTaskSettings();

    virtual bool convertCustomSettings();

    //all settings except sMatrix and pointers must be set up through customSettings and then must be converted by convertCustomSettings().
    SmithWatermanReportCallbackMAImpl* reportCallback;
    SmithWatermanResultListener* resultListener;
    SmithWatermanResultFilter* resultFilter;

    int gapOpen;
    int gapExtd;
    int percentOfScore;
    QString sMatrixName;
    SMatrix sMatrix;        //initialized by convertCustomSettings()

    static const QString PA_SW_GAP_OPEN;
    static const QString PA_SW_GAP_EXTD;
    static const QString PA_SW_SCORING_MATRIX_NAME;
    static const QString PA_SW_REALIZATION_NAME;
    static const qint64 PA_SW_DEFAULT_PERCENT_OF_SCORE = 0;
    static const QString PA_SW_DEFAULT_RESULT_FILTER;
};

class PairwiseAlignmentSmithWatermanTask : public PairwiseAlignmentTask, public SequenceWalkerCallback {
    Q_OBJECT
public:
    PairwiseAlignmentSmithWatermanTask(PairwiseAlignmentSmithWatermanTaskSettings* _settings, SW_AlgType algType);
    ~PairwiseAlignmentSmithWatermanTask();
    virtual void onRegion(SequenceWalkerSubtask *t, TaskStateInfo &ti);
    void prepare();
    QList<PairAlignSequences> & getResult();
    ReportResult report();

    QList<Task*> onSubTaskFinished(Task* subTask);

protected:
    void addResult(QList<PairAlignSequences> & res);
    int calculateMaxScore(const QByteArray &seq, const SMatrix& substitutionMatrix);
    void setupTask();
    int calculateMatrixLength(const QByteArray & searchSeq, const QByteArray & patternSeq, int gapOpen, int gapExtension, int maxScore, int minScore);
    void removeResultFromOverlap(QList<PairAlignSequences> & res);
    QList<PairAlignSequences> expandResults(QList<PairAlignSequences>& res);

protected:
    QMutex lock;
    PairwiseAlignmentSmithWatermanTaskSettings* settings;
    SW_AlgType algType;
    QList<PairAlignSequences> pairAlignSequences;
    QList<SmithWatermanResult> resultList;
    int minScore;
    int maxScore;
    QByteArray* sqnc;
    QByteArray* ptrn;
    SequenceWalkerTask* t;

    CudaGpuModel * cudaGpu;
    OpenCLGpuModel * openClGpu;
};

class PairwiseAlignmentSWResultsPostprocessingTask : public Task {
    Q_OBJECT

public:
    PairwiseAlignmentSWResultsPostprocessingTask(SmithWatermanResultFilter* rf, SmithWatermanResultListener* rl, QList<SmithWatermanResult> &_resultList, QList<PairAlignSequences> &_resPAS);
    ~PairwiseAlignmentSWResultsPostprocessingTask(){}

    void run();
    void prepare();
    ReportResult report(){ return ReportResult_Finished; }

private:
    SmithWatermanResultFilter* rf;
    SmithWatermanResultListener* rl;
    QList<SmithWatermanResult> resultList;
    QList<PairAlignSequences> resPAS;
};

}   //namespace

#endif  //_U2_SW_ALGORITHM_TASK_H_
