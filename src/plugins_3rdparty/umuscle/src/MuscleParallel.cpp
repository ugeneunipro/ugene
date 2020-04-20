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

#include "MuscleAdapter.h"
#include "MuscleParallel.h"
#include "MuscleUtils.h"
#include "MuscleConstants.h"
#include "MuscleWorkPool.h"
#include "TaskLocalStorage.h"

#include <U2Core/AppResources.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/Log.h>
#include <U2Core/AppResources.h>

#include <QMutex>
#include <QMutexLocker>
#include <QSemaphore>

#include <limits>
#include "muscle/scorehistory.h"

namespace U2 {

/////////////////////////////////////////////////////////////////////

MuscleParallelTask::MuscleParallelTask(const MultipleSequenceAlignment& ma, MultipleSequenceAlignment& res, const MuscleTaskSettings& _config, MuscleContext* ctx)
    : Task(tr("MuscleParallelTask"), TaskFlags_NR_FOSCOE), progAlignTask(NULL), refineTreeTask(NULL), refineTask(NULL)
{
    //assert(ma->isNormalized()); //not required to be normalized    assert(_config.op == MuscleTaskOp_Align || _config.op == MuscleTaskOp_Refine);    workpool = NULL;
    setMaxParallelSubtasks(1);
    workpool = new MuscleWorkPool(ctx, _config, stateInfo, _config.nThreads, ma, res, _config.regionToAlign.startPos == 0);
    prepareTask = new MusclePrepareTask(workpool);
    prepareTask->setSubtaskProgressWeight(0);
    addSubTask(prepareTask);

    TaskResourceUsage resourseUsage(RESOURCE_MEMORY, estimateMemoryUsageInMb(ma), true);
    resourseUsage.errorMessage = tr("There is not enough memory to align these sequences with MUSCLE.");
    addTaskResource(resourseUsage);
}

int MuscleParallelTask::estimateMemoryUsageInMb(const MultipleSequenceAlignment& ma) {
    QList<int> rowsLengths;
    foreach(const MultipleSequenceAlignmentRow& row, ma->getMsaRows()) {
        rowsLengths.append(row->getCoreLength());
    }
    qSort(rowsLengths.begin(), rowsLengths.end(), qGreater<int>());

    qint64 usedBytes = 0;
    int availableThreads = workpool->nThreads;
    for (int i = 0; i < rowsLengths.size() && availableThreads > 0; i++) {
        for (int j = 0; j < rowsLengths.size() && availableThreads > 0; j++, availableThreads--) {
            usedBytes += qint64((rowsLengths[i] + 1025)) * (rowsLengths[j] + 1025);
        }
    }
    const int maxInt = std::numeric_limits<int>::max();
    usedBytes = usedBytes >= 0 ? usedBytes : maxInt;
    return qMin(usedBytes / 1024 / 1024, qint64(maxInt));
}

QList<Task*> MuscleParallelTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    if (isCanceled()) {
        return res;
    }

    if (subTask == prepareTask && workpool->res->isEmpty()) {
        foreach(Task* task, prepareTask->res)
            res << task;
    }
    return res;
}

void MuscleParallelTask::cleanup() {
    //delete workpool->ph;
    delete workpool;
    workpool = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////
MusclePrepareTask::MusclePrepareTask(MuscleWorkPool *wp) 
:Task("MusclePrepareTask", TaskFlags_FOSCOE), workpool(wp)
{
    //do nothing
}

void MusclePrepareTask::run() {
    algoLog.details(tr("Preparing MUSCLE alignment..."));
    TaskLocalData::bindToMuscleTLSContext(workpool->ctx);
    try {
        workpool->ph = new MuscleParamsHelper(workpool->ti, workpool->ctx);
        _run(); 
    }
    catch (const MuscleException& e) {
        if (!isCanceled()) {
            workpool->ti.setError(  tr("Internal parallel MUSCLE error: %1").arg(e.str) );
        }
    } 
    catch (std::bad_alloc) {
        if (!isCanceled()) {
            workpool->ti.setError(MuscleAdapter::getBadAllocError());
        }
    }
    TaskLocalData::detachMuscleTLSContext();
    algoLog.details(tr("MUSCLE prepared successfully"));
}

void MusclePrepareTask::_run() {
    switch(workpool->config.op) {
        case MuscleTaskOp_Align:
            alignPrepareUnsafe();
            break;
        case MuscleTaskOp_Refine: 
            refinePrepareUnsafe();
            break;
        case MuscleTaskOp_AddUnalignedToProfile: 
            assert(0);
            //doAddUnalignedToProfile();
            break;
        case MuscleTaskOp_ProfileToProfile: 
            assert(0);
            //doProfile2Profile();
            break;
    }
}

void MusclePrepareTask::alignPrepareUnsafe()
{
    MuscleContext* ctx = workpool->ctx;
    SetSeqWeightMethod(ctx->params.g_SeqWeight1);

    setupAlphaAndScore(workpool->ma->getAlphabet(), stateInfo);
    if (stateInfo.hasError()) {
        return;
    }

    SeqVect &v = workpool->v; 

    convertMAlignment2SecVect(v, workpool->ma, true);
    const unsigned uSeqCount = v.Length();
    if (0 == uSeqCount) {
        stateInfo.setError(  tr("No sequences in input file") );
        return;
    }

    unsigned uMinL = 0;
    unsigned uMaxL = 0;
    unsigned uTotL = 0;
    for (unsigned uSeqIndex = 0; uSeqIndex < uSeqCount; ++uSeqIndex) {
        unsigned L = v.GetSeq(uSeqIndex).Length();
        uTotL += L;
        uMinL = qMin(uMinL, L);
        uMaxL = qMax(uMaxL, L);
    }

    SetIter(1);
    ctx->params.g_bDiags = ctx->params.g_bDiags1;
    SetSeqStats(uSeqCount, uMinL, uMaxL, uTotL/uSeqCount);

    SetMuscleSeqVect(v);

    MSA::SetIdCount(uSeqCount);

    // Initialize sequence ids.
    // From this point on, ids must somehow propagate from here.
    for (unsigned uSeqIndex = 0; uSeqIndex < uSeqCount; ++uSeqIndex) {
        v.SetSeqId(uSeqIndex, uSeqIndex);
    }

    if (0 == uSeqCount) {
        stateInfo.setError(  tr("Alignment is empty") );
        return;
    }

    if (1 == uSeqCount) {
        workpool->res = workpool->ma;
        return;
    }

    if (uSeqCount > 1 && workpool->mhack) {
        MHackStart(v);
    }
    Tree &GuideTree = workpool->GuideTree;

    TreeFromSeqVect(v, GuideTree, ctx->params.g_Cluster1, ctx->params.g_Distance1, ctx->params.g_Root1, ctx->params.g_pstrDistMxFileName1);

    SetMuscleTree(GuideTree);
    ValidateMuscleIds(GuideTree);

    //ProgressiveAlignment
    assert(workpool->GuideTree.IsRooted());

    //const unsigned uIterCount = uSeqCount - 1;
    const unsigned uNodeCount = 2*uSeqCount - 1;

    if(ctx->params.g_bLow) {
        workpool->Weights = new WEIGHT[uSeqCount];
        CalcClustalWWeights(workpool->GuideTree, workpool->Weights);
    }

    workpool->ProgNodes = new ProgNode[uNodeCount];
    SetProgressDesc("Align node");

    //////////////////////////////////////////////////////////////////////////
    workpool->treeNodeStatus = new TreeNodeStatus[GuideTree.GetNodeCount()];
    workpool->treeNodeIndexes = new unsigned[GuideTree.GetNodeCount()];
#ifdef _DEBUG
    //set initial values
    for(unsigned i=0;i<GuideTree.GetNodeCount();i++) {
        workpool->treeNodeStatus[i] = TreeNodeStatus_Processing;
        workpool->treeNodeIndexes[i] = NULL_NEIGHBOR;
    }
#endif
    for(unsigned k=0, i=GuideTree.FirstDepthFirstNode();i!=NULL_NEIGHBOR;i=GuideTree.NextDepthFirstNode(i),k++) {
        workpool->treeNodeIndexes[k] = i;
        if(GuideTree.IsLeaf(i)) {
            workpool->treeNodeStatus[i] = TreeNodeStatus_Available;
        } else {
            workpool->treeNodeStatus[i] = TreeNodeStatus_WaitForChild;
        }
    }
#ifdef _DEBUG
    //validate
    for(unsigned i=0;i<GuideTree.GetNodeCount();i++) {
        assert(workpool->treeNodeStatus[i] != TreeNodeStatus_Processing);
        assert(workpool->treeNodeIndexes[i] != NULL_NEIGHBOR);
    }
#endif

    Task *progAlignTask = new ProgressiveAlignTask(workpool);
    res << progAlignTask;
    if (workpool->ctx->params.g_uMaxIters == 1) {
        progAlignTask->setSubtaskProgressWeight(0.9f);
    } else {
        Task *refineTreeTask = new RefineTreeTask(workpool);
        Task *refineTask = new RefineTask(workpool);
        progAlignTask->setSubtaskProgressWeight(0.3f);
        refineTreeTask->setSubtaskProgressWeight(0.1f);
        refineTask->setSubtaskProgressWeight(0.5f);
        res << refineTreeTask << refineTask;
    }

}

void MusclePrepareTask::refinePrepareUnsafe() {
    workpool->ti.progress = 0;

    MuscleContext *ctx = getMuscleContext();

    SetSeqWeightMethod(ctx->params.g_SeqWeight1);

    setupAlphaAndScore(workpool->ma->getAlphabet(), workpool->ti);
    if (workpool->ti.hasError()) {
        return;
    }
    MSA &msa = workpool->a;
    convertMAlignment2MSA(msa, workpool->ma, true);
    unsigned uSeqCount = msa.GetSeqCount();
    MSA::SetIdCount(uSeqCount);

    // Initialize sequence ids.
    // From this point on, ids must somehow propogate from here.
    for (unsigned uSeqIndex = 0; uSeqIndex < uSeqCount; ++uSeqIndex) {
        msa.SetSeqId(uSeqIndex, uSeqIndex);
    }
    SetMuscleInputMSA(msa);

    TreeFromMSA(msa, workpool->GuideTree, ctx->params.g_Cluster2, ctx->params.g_Distance2, ctx->params.g_Root2);
    SetMuscleTree(workpool->GuideTree);

    Task* refineTask;
    refineTask = new RefineTask(workpool);
    res << refineTask;
}

void MusclePrepareTask::cleanup() {}

///////////////////////////////////////////////////////////////////////////////////////

ProgressiveAlignTask::ProgressiveAlignTask(MuscleWorkPool *_wp)
:Task(tr("ProgressiveAlignTask"), TaskFlags_FOSCOE), workpool(_wp)
{
    assert(_wp!=NULL);
}

void ProgressiveAlignTask::prepare() {
    assert(workpool->nThreads >= 0);
    setMaxParallelSubtasks(workpool->nThreads);
    for(int i=0; i < workpool->nThreads; i++) {
        ProgressiveAlignWorker* sub = new ProgressiveAlignWorker(workpool, i);
        addSubTask(sub);
    }
    timer.start();
}

void ProgressiveAlignTask::run() {
    TaskLocalData::bindToMuscleTLSContext(workpool->ctx);
    try {
        _run(); 
    }
    catch (const MuscleException& e) {
        if (!isCanceled()) {
            workpool->ti.setError(  tr("Internal parallel MUSCLE error: %1").arg(e.str) );
        }
    }
    catch (std::bad_alloc) {
        if (!isCanceled()) {
            workpool->ti.setError(MuscleAdapter::getBadAllocError());
    }
}
    TaskLocalData::detachMuscleTLSContext();
#ifdef TRACE    
    log.info(tr("alignment \"%1\" Parallel MUSCLE Iter 1 accomplished. Time elapsed %2 ms").arg(workpool->ma->name).arg(timer.elapsed()));
#endif
}

void ProgressiveAlignTask::_run() {
    if (!workpool->res->isEmpty())  {
        return;// no more need in align
    }
	if (workpool->ti.hasError())  {
		return; // cancel on error
	}
    MuscleContext* ctx = workpool->ctx;

    const unsigned uSeqCount = workpool->v.Length();

    if(ctx->params.g_bLow) {
        
        ProgressStepsDone();

        if (getMuscleContext()->params.g_bBrenner)
            MakeRootMSABrenner((SeqVect &) workpool->v, workpool->GuideTree, workpool->ProgNodes, workpool->a);
        else
            MakeRootMSA(workpool->v, workpool->GuideTree, workpool->ProgNodes, workpool->a);

    } else {
        if(ctx->isCanceled()) {
            throw MuscleException("Canceled");
        }

        ProgressStepsDone();

        unsigned uRootNodeIndex = workpool->GuideTree.GetRootNodeIndex();
        const ProgNode &RootProgNode = workpool->ProgNodes[uRootNodeIndex];
        workpool->a.Copy(RootProgNode.m_MSA);
    }

    SetCurrentAlignment(workpool->a);

    ValidateMuscleIds(workpool->a);

    if (1 == ctx->params.g_uMaxIters || 2 == uSeqCount) {
        assert(int(workpool->a.GetSeqCount()) == workpool->ma->getNumRows());
        prepareAlignResults(workpool->a, workpool->ma->getAlphabet(), workpool->res, workpool->mhack);
    }
}

void ProgressiveAlignTask::cleanup() {}

////////////////////////////////////////////////////////////////////////////////

ProgressiveAlignWorker::ProgressiveAlignWorker(MuscleWorkPool *_wp, int _workerID)
:Task(tr("ProgressiveAlignWorker"), TaskFlags_FOSCOE), workpool(_wp), workerID(_workerID)
{
    assert(_wp!=NULL);
    assert(workerID>=0);
    uTreeNodeIndex = NULL_NEIGHBOR;
}

void ProgressiveAlignWorker::run() {
    TaskLocalData::bindToMuscleTLSContext(workpool->ctx, workerID);
    try {
        _run();
    }
    catch (const MuscleException& e) {
        if (!isCanceled()) {
            workpool->ti.setError(  tr("Internal parallel MUSCLE error: %1").arg(e.str) );
        }
    }
    catch (std::bad_alloc) {
        if (!isCanceled()) {
            workpool->ti.setError(MuscleAdapter::getBadAllocError());
        }
    }
    TaskLocalData::detachMuscleTLSContext();
}

void ProgressiveAlignWorker::_run() {

    MuscleContext* ctx = workpool->ctx;
    SeqVect &v = workpool->v;
    Tree &GuideTree = workpool->GuideTree;

    const unsigned uSeqCount = v.Length();
    const unsigned uNodeCount = 2*uSeqCount - 1;
    //
    uTreeNodeIndex = workpool->getJob();
    if(uTreeNodeIndex == NULL_NEIGHBOR)
        return;

    do {
        if (GuideTree.IsLeaf(uTreeNodeIndex))
        {
            if(ctx->params.g_bLow) {
                if (uTreeNodeIndex >= uNodeCount)
                    Quit("TreeNodeIndex=%u NodeCount=%u\n", uTreeNodeIndex, uNodeCount);
                ProgNode &Node = workpool->ProgNodes[uTreeNodeIndex];
                unsigned uId = GuideTree.GetLeafId(uTreeNodeIndex);
                if (uId >= uSeqCount)
                    Quit("Seq index out of range");
                const Seq &s = *(v[uId]);
                Node.m_MSA.FromSeq(s);
                Node.m_MSA.SetSeqId(0, uId);
                Node.m_uLength = Node.m_MSA.GetColCount();
                Node.m_Weight = workpool->Weights[uId];
                // TODO: Term gaps settable
                Node.m_Prof = ProfileFromMSA(Node.m_MSA);
                Node.m_EstringL = 0;
                Node.m_EstringR = 0;
            }
            else {
                if (uTreeNodeIndex >= uNodeCount)
                    Quit("TreeNodeIndex=%u NodeCount=%u\n", uTreeNodeIndex, uNodeCount);
                ProgNode &Node = workpool->ProgNodes[uTreeNodeIndex];
                unsigned uId = GuideTree.GetLeafId(uTreeNodeIndex);
                if (uId >= uSeqCount)
                    Quit("Seq index out of range");
                const Seq &s = *(v[uId]);
                Node.m_MSA.FromSeq(s);
                Node.m_MSA.SetSeqId(0, uId);
                Node.m_uLength = Node.m_MSA.GetColCount();
            }
        }
        else
        {
            {
                QMutexLocker(&workpool->proAligMutex);
                Progress(workpool->uJoin, uSeqCount - 1);
                ++workpool->uJoin;
            }
            if(ctx->params.g_bLow) {

                const unsigned uMergeNodeIndex = uTreeNodeIndex;
                ProgNode &Parent = workpool->ProgNodes[uMergeNodeIndex];

                const unsigned uLeft = GuideTree.GetLeft(uTreeNodeIndex);
                const unsigned uRight = GuideTree.GetRight(uTreeNodeIndex);

                ProgNode &Node1 = workpool->ProgNodes[uLeft];
                ProgNode &Node2 = workpool->ProgNodes[uRight];

                AlignTwoProfs(
                    Node1.m_Prof, Node1.m_uLength, Node1.m_Weight,
                    Node2.m_Prof, Node2.m_uLength, Node2.m_Weight,
                    Parent.m_Path,
                    &Parent.m_Prof, &Parent.m_uLength);

                PathToEstrings(Parent.m_Path, &Parent.m_EstringL, &Parent.m_EstringR);

                Parent.m_Weight = Node1.m_Weight + Node2.m_Weight;

                Node1.m_MSA.Clear();
                Node2.m_MSA.Clear();

            } else {

                const unsigned uMergeNodeIndex = uTreeNodeIndex;
                ProgNode &Parent = workpool->ProgNodes[uMergeNodeIndex];

                const unsigned uLeft = GuideTree.GetLeft(uTreeNodeIndex);
                const unsigned uRight = GuideTree.GetRight(uTreeNodeIndex);

                ProgNode &Node1 = workpool->ProgNodes[uLeft];
                ProgNode &Node2 = workpool->ProgNodes[uRight];

                PWPath Path;
                AlignTwoMSAs(Node1.m_MSA, Node2.m_MSA, Parent.m_MSA, Path);
                Parent.m_uLength = Parent.m_MSA.GetColCount();

                Node1.m_MSA.Clear();
                Node2.m_MSA.Clear();
            }

        }
        uTreeNodeIndex = workpool->getNextJob(uTreeNodeIndex);
    }
    while (!isCanceled() && uTreeNodeIndex != NULL_NEIGHBOR);
}

///////////////////////////////////////////////////////////////////
extern bool TryRealign(MSA &msaIn, const Tree &tree, const unsigned Leaves1[],
                unsigned uCount1, const unsigned Leaves2[], unsigned uCount2,
                SCORE *ptrscoreBefore, SCORE *ptrscoreAfter,
                bool bLockLeft, bool bLockRight);

RefineTreeTask::RefineTreeTask(MuscleWorkPool *_workpool)
:Task(tr("RefineTreeTask"),TaskFlags_FOSCOE), workpool(_workpool)
{
    assert(workpool!=NULL);
}

void RefineTreeTask::run() {
    TaskLocalData::bindToMuscleTLSContext(workpool->ctx);
    try {
        _run(); 
    }
    catch (const MuscleException& e) {
        if (!isCanceled()) {
            workpool->ti.setError(  tr("Internal parallel MUSCLE error: %1").arg(e.str) );
        }
    }
    catch (std::bad_alloc) {
        if (!isCanceled()) {
            workpool->ti.setError(MuscleAdapter::getBadAllocError());
        }
    }
    TaskLocalData::detachMuscleTLSContext();
}

void RefineTreeTask::_run() {
    if (!workpool->res->isEmpty())  {
        return;// no more need in align
    }
    MuscleContext* ctx = workpool->ctx;

    if (0 == ctx->params.g_pstrUseTreeFileName)  {
        ctx->params.g_bDiags = ctx->params.g_bDiags2;
        SetIter(2);

        if (ctx->params.g_bLow) {
            if (0 != ctx->params.g_uMaxTreeRefineIters)
                RefineTreeE(workpool->a, workpool->v, workpool->GuideTree, workpool->ProgNodes);
        } else {
            RefineTree(workpool->a, workpool->GuideTree);
        }
    }
    if (stateInfo.cancelFlag) {
        return;
    }

    SetSeqWeightMethod(ctx->params.g_SeqWeight2);
    SetMuscleTree(workpool->GuideTree);
}
///////////////////////////////////////////////////////////////////

RefineTask::RefineTask(MuscleWorkPool *_workpool)
:Task(tr("RefineTask"),TaskFlags_RBSF_FOSCOE), workpool(_workpool)
{
    assert(workpool!=NULL);
    tpm = Progress_Manual;
    workpool->refineTI = &stateInfo;
    setMaxParallelSubtasks(workpool->nThreads);
    for(int i=0;i<workpool->nThreads;i++) {
        addSubTask(new RefineWorker(workpool,i));
    }
}

void RefineTask::run() {
    TaskLocalData::bindToMuscleTLSContext(workpool->ctx);
    try {
        QTime timer;
        timer.start();
        _run(); 
        workpool->refineDone = true;
        workpool->mainSem.release(workpool->nThreads);
        perfLog.trace(QString("Parallel muscle refine stage complete. Elapsed %1 ms").arg(timer.elapsed()));
    }
    catch (const MuscleException& e) {
        if (!isCanceled()) {
            workpool->ti.setError(  tr("Internal parallel MUSCLE error: %1").arg(e.str) );
        }
        workpool->refineDone = true;
        workpool->mainSem.release(workpool->nThreads);
    }
    catch (std::bad_alloc) {
        if (!isCanceled()) {
            workpool->ti.setError(tr("Can't allocate enough memory to perform aligning, try to use 64bit UGENE version"));
        }
    }
    TaskLocalData::detachMuscleTLSContext();
}

void RefineTask::_run() {
    if (!workpool->res->isEmpty())  {
        return;// no more need in align
    }
    MuscleContext* ctx = workpool->ctx;
    MSA &msa = workpool->a;

    workpool->refineDone = false;
    int d = workpool->config.op == MuscleTaskOp_Refine?0:2;
    if (ctx->params.g_bAnchors) {
        RefineVertP(&msa, ctx->params.g_uMaxIters - d);
    } else {
        RefineHorizP(&msa, ctx->params.g_uMaxIters - d, false, false);
    }
    
    if (stateInfo.cancelFlag) {
        return;
    }

    ValidateMuscleIds(msa);
    ValidateMuscleIds(workpool->GuideTree);
    
    if(workpool->config.op != MuscleTaskOp_Refine) {
        //assert(int(msa.GetSeqCount()) == workpool->ma->getNumSequences());
        prepareAlignResults(msa, workpool->ma->getAlphabet(), workpool->res, workpool->mhack);
    } else {
        prepareAlignResults(msa, workpool->ma->getAlphabet(), workpool->res, false);
    }
}
////////////////////////////////////////////////////////////////////////////

RefineWorker::RefineWorker(MuscleWorkPool *_workpool, int _workerID) 
    :Task(QString("RefineWorker"), TaskFlags_FOSCOE), workpool(_workpool), workerID(_workerID), Leaves1(NULL), Leaves2(NULL)
{
    assert(workerID>=0);
    assert(workpool!=NULL);
}

void RefineWorker::run() {
    TaskLocalData::bindToMuscleTLSContext(workpool->ctx, workerID);
    try {
        _run();
    }
    catch (const MuscleException& e) {
        if (!isCanceled()) {
            workpool->ti.setError(  tr("Internal parallel MUSCLE error: %1").arg(e.str) );
            workpool->childSem.release();
        }
    }
    catch (std::bad_alloc) {
        if (!isCanceled()) {
            workpool->ti.setError(tr("Can't allocate enough memory to perform aligning, try to use 64bit UGENE version"));
        }
    }
    TaskLocalData::detachMuscleTLSContext();
}



} //namespace

