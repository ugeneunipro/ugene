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

#include "MuscleParallel.h"
#include "MuscleWorkPool.h"
#include "MuscleConstants.h"
#include "muscle/objscore.h"

#include <U2Core/Log.h>

#define DIFFOBJSCORE	0

namespace U2 {

    
    bool TryRealign(MSA &msaIn, const Tree &tree, const unsigned Leaves1[],
        unsigned uCount1, const unsigned Leaves2[], unsigned uCount2,
        SCORE *ptrscoreBefore, SCORE *ptrscoreAfter,
        bool bLockLeft, bool bLockRight)
    {
#if	TRACE
        Log("TryRealign, msaIn=\n");
#endif

        MuscleContext *ctx = getMuscleContext();

        const unsigned uSeqCount = msaIn.GetSeqCount();

        unsigned *Ids1 = new unsigned[uSeqCount];
        unsigned *Ids2 = new unsigned[uSeqCount];

        LeafIndexesToIds(tree, Leaves1, uCount1, Ids1);
        LeafIndexesToIds(tree, Leaves2, uCount2, Ids2);

        MSA msa1;
        MSA msa2;

        MSASubsetByIds(msaIn, Ids1, uCount1, msa1);
        MSASubsetByIds(msaIn, Ids2, uCount2, msa2);

#if	DEBUG
        ValidateMuscleIds(msa1);
        ValidateMuscleIds(msa2);
#endif

        // Computing the objective score may be expensive for
        // large numbers of sequences. As a speed optimization,
        // we check whether the alignment changes. If it does
        // not change, there is no need to compute the objective
        // score. We test for the alignment changing by comparing
        // the Viterbi paths before and after re-aligning.
        PWPath pathBefore;
        pathBefore.FromMSAPair(msa1, msa2);

        DeleteGappedCols(msa1);
        DeleteGappedCols(msa2);

        if (0 == msa1.GetColCount() || 0 == msa2.GetColCount()) {
            delete[] Ids1;
            delete[] Ids2;
            return false;
        }

        MSA msaRealigned;
        PWPath pathAfter;

        AlignTwoMSAs(msa1, msa2, msaRealigned, pathAfter, bLockLeft, bLockRight);

        bool bAnyChanges = !pathAfter.Equal(pathBefore);
        unsigned uDiffCount1;
        unsigned uDiffCount2;
        unsigned* Edges1 = ctx->refinehoriz.Edges1;
        unsigned* Edges2 = ctx->refinehoriz.Edges2;
        DiffPaths(pathBefore, pathAfter, Edges1, &uDiffCount1, Edges2, &uDiffCount2);

#if	TRACE
        Log("TryRealign, msa1=\n");
        Log("\nmsa2=\n");
        Log("\nRealigned (changes %s)=\n", bAnyChanges ? "TRUE" : "FALSE");
#endif

        if (!bAnyChanges)
        {
            *ptrscoreBefore = 0;
            *ptrscoreAfter = 0;
            delete[] Ids1;
            delete[] Ids2;
            return false;
        }

        SetMSAWeightsMuscle(msaIn);
        SetMSAWeightsMuscle(msaRealigned);

#if	DIFFOBJSCORE
        const SCORE scoreDiff = DiffObjScore(msaIn, pathBefore, Edges1, uDiffCount1,
            msaRealigned, pathAfter, Edges2, uDiffCount2);
        bool bAccept = (scoreDiff > 0);
        *ptrscoreBefore = 0;
        *ptrscoreAfter = scoreDiff;
        //const SCORE scoreBefore = ObjScoreIds(msaIn, Ids1, uCount1, Ids2, uCount2);
        //const SCORE scoreAfter = ObjScoreIds(msaRealigned, Ids1, uCount1, Ids2, uCount2);
        //Log("Diff = %.3g %.3g\n", scoreDiff, scoreAfter - scoreBefore);
#else
        const SCORE scoreBefore = ObjScoreIds(msaIn, Ids1, uCount1, Ids2, uCount2);
        const SCORE scoreAfter = ObjScoreIds(msaRealigned, Ids1, uCount1, Ids2, uCount2);

        bool bAccept = (scoreAfter > scoreBefore);

#if	TRACE
        Log("Score %g -> %g Accept %s\n", scoreBefore, scoreAfter, bAccept ? "TRUE" : "FALSE");
#endif

        *ptrscoreBefore = scoreBefore;
        *ptrscoreAfter = scoreAfter;
#endif

        if (bAccept)
            msaIn.Copy(msaRealigned);
        delete[] Ids1;
        delete[] Ids2;
        return bAccept;
    }

    void RefineTask::RefineHeightPartsP(bool *ptrbAnyChanges) 
    {
        *(workpool->ptrbOscillating) = false;
        workpool->bAnyAccepted = false;
        //run workers
#if TRACE
        algoLog.trace(QString("Refine: Start workers. Wait..."));
#endif
        workpool->mainSem.release(workpool->nThreads);
        workpool->childSem.acquire(workpool->nThreads);
#if TRACE
        algoLog.trace(QString("Refine: Iteration done. Workers waits."));
#endif
        *ptrbAnyChanges = workpool->bAnyAccepted;
#ifdef DEBUG
        //Validate node statuses
        if(!getMuscleContext()->isCanceled())
            for(unsigned i=0;i<workpool->uInternalNodeCount;i++) {
                assert(workpool->refineNodeStatuses[i]==RefineTreeNodeStatus_Done);
            }
#endif
    }

    void RefineWorker::_run() {

#if TRACE
        algoLog.trace(QString("Worker %1 start. Wait...").arg(QString::number(workerID)));
#endif
        workpool->mainSem.acquire();
#if TRACE
        algoLog.trace(QString("Worker %1: Stop wait. Start (mainSem %2, childSem %3)").arg(QString::number(workerID)).
            arg(QString::number(workpool->mainSem.available())).arg(QString::number(workpool->mainSem.available())));
#endif
        while(!workpool->isRefineDone())
        {
            MSA msaIn;
            
            unsigned i = workpool->refineGetJob(&msaIn, workerID);

            MuscleContext *ctx = workpool->ctx;
//            unsigned &g_uTreeSplitNode1 = ctx->muscle.g_uTreeSplitNode1;
//            unsigned &g_uTreeSplitNode2 = ctx->muscle.g_uTreeSplitNode2;
//            unsigned &g_uRefineHeightSubtree = ctx->refinehoriz.g_uRefineHeightSubtree;
//            unsigned &g_uRefineHeightSubtreeTotal = ctx->refinehoriz.g_uRefineHeightSubtreeTotal;
            Tree &tree = workpool->GuideTree;
            const unsigned uSeqCount = msaIn.GetSeqCount();
//            const unsigned uInternalNodeCount = uSeqCount - 1;
            unsigned *Leaves1 = new unsigned[uSeqCount];
            unsigned *Leaves2 = new unsigned[uSeqCount];
            const unsigned uRootNodeIndex = tree.GetRootNodeIndex();

            while (i != NULL_NEIGHBOR) {

                const unsigned uInternalNodeIndex = workpool->InternalNodeIndexes[i];
                unsigned uNeighborNodeIndex;
                if (tree.IsRoot(uInternalNodeIndex) && !workpool->bRight) {
                    i = workpool->refineGetNextJob(&msaIn, false, -1, i, workerID);
                    continue;
                }
                else if (workpool->bRight)
                    uNeighborNodeIndex = tree.GetRight(uInternalNodeIndex);
                else
                    uNeighborNodeIndex = tree.GetLeft(uInternalNodeIndex);

                //            g_uTreeSplitNode1 = uInternalNodeIndex;
                //            g_uTreeSplitNode2 = uNeighborNodeIndex;

                unsigned uCount1;
                unsigned uCount2;

                GetLeaves(tree, uNeighborNodeIndex, Leaves1, &uCount1);
                GetLeavesExcluding(tree, uRootNodeIndex, uNeighborNodeIndex,
                    Leaves2, &uCount2);

                SCORE scoreBefore;
                SCORE scoreAfter;
                bool bAccepted = TryRealign(msaIn, tree, Leaves1, uCount1, Leaves2, uCount2,
                    &scoreBefore, &scoreAfter, workpool->bLockLeft, workpool->bLockRight);

                SCORE scoreMax = scoreAfter > scoreBefore? scoreAfter : scoreBefore;
                //bool bRepeated = workpool->History->SetScore(workpool->uIter, uInternalNodeIndex, workpool->bRight, scoreMax);
                i = workpool->refineGetNextJob(&msaIn, bAccepted, scoreMax, i, workerID);
            }

            delete[] Leaves1;
            delete[] Leaves2;
#if TRACE
            algoLog.trace(QString("Worker %1: no job available. Wait... (mainSem %2, childSem %3)").arg(QString::number(workerID)).
                arg(QString::number(workpool->mainSem.available())).arg(QString::number(workpool->mainSem.available())));
#endif         
            workpool->childSem.release();
            workpool->mainSem.acquire();
#if TRACE      
            algoLog.trace(QString("Worker %1: Stop wait. Start (mainSem %2, childSem %3)").arg(QString::number(workerID)).
                arg(QString::number(workpool->mainSem.available())).arg(QString::number(workpool->mainSem.available())));
#endif         
        }
#if TRACE
        algoLog.trace(QString("Worker %1: Refine done. Exit").arg(QString::number(workerID)));
#endif
    }

    // Return true if any changes made
    bool RefineTask::RefineHorizP(MSA* msaIn, unsigned uIters, bool bLockLeft, bool bLockRight) 
    {
        Q_UNUSED(bLockLeft); Q_UNUSED(bLockRight);
        
        MuscleContext *ctx = workpool->ctx;
        unsigned &g_uRefineHeightSubtree = ctx->refinehoriz.g_uRefineHeightSubtree;
        unsigned &g_uRefineHeightSubtreeTotal = ctx->refinehoriz.g_uRefineHeightSubtreeTotal;
        Tree &tree = workpool->GuideTree;
        workpool->msaIn = msaIn;
        workpool->uIters = uIters;

        if (!tree.IsRooted())
            Quit("RefineHeight: requires rooted tree");

        const unsigned uSeqCount = msaIn->GetSeqCount();
        if (uSeqCount < 3)
            return false;

        const unsigned uInternalNodeCount = uSeqCount - 1;
        unsigned *InternalNodeIndexes = new unsigned[uInternalNodeCount];
        unsigned *InternalNodeIndexesR = new unsigned[uInternalNodeCount];
        

        GetInternalNodesInHeightOrder(tree, InternalNodeIndexes);

        ScoreHistory History(uIters, 2*uSeqCount - 1);
        workpool->History = &History;
        workpool->uInternalNodeCount = uInternalNodeCount;
        bool bAnyChangesAnyIter = false;
        workpool->refineNodeStatuses = new RefineTreeNodeStatus[uInternalNodeCount];
        for (unsigned n = 0; n < uInternalNodeCount; ++n) {
            InternalNodeIndexesR[uInternalNodeCount - 1 - n] = InternalNodeIndexes[n];
            workpool->refineNodeStatuses[n] = RefineTreeNodeStatus_Available;
        }

        for (unsigned uIter = 0; uIter < uIters && !ctx->isCanceled(); ++uIter)
        {
            workpool->uIter = uIter;

            bool bAnyChangesThisIter = false;
            IncIter();
            SetProgressDesc("Refine biparts");
            g_uRefineHeightSubtree = 0;
            g_uRefineHeightSubtreeTotal = uInternalNodeCount*2 - 1;

            bool &bReverse = workpool->bReversed = (uIter%2 != 0);
            if (bReverse)
                workpool->InternalNodeIndexes = InternalNodeIndexesR;
            else
                workpool->InternalNodeIndexes = InternalNodeIndexes;

            bool bOscillating;
            workpool->ptrbOscillating = &bOscillating;
            for (unsigned i = 0; i < 2 && !ctx->isCanceled(); ++i)
            {
                bool bAnyChanges = false;
                bool &bRight = workpool->bRight;
                switch (i)
                {
                case 0:
                    bRight = true;
                    break;
                case 1:
                    bRight = false;
                    break;
                default:
                    delete[] InternalNodeIndexes;
                    delete[] InternalNodeIndexesR;
                    Quit("RefineHeight default case");
                }
                workpool->reset();
                RefineHeightPartsP(&bAnyChanges);
                if (bOscillating)
                {
                    ProgressStepsDone();
                    goto Osc;
                }
                if (bAnyChanges)
                {
                    bAnyChangesThisIter = true;
                    bAnyChangesAnyIter = true;
                }
            }

            ProgressStepsDone();
            if (bOscillating)
                break;

            if (!bAnyChangesThisIter)
                break;
        }

Osc:
        delete[] InternalNodeIndexes;
        delete[] InternalNodeIndexesR;
        delete[] workpool->refineNodeStatuses;

        return bAnyChangesAnyIter;
    }
}//namespace
