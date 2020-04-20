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

#include "MuscleWorkPool.h"
#include <U2Core/Log.h>

namespace U2 {

    MuscleWorkPool::MuscleWorkPool(MuscleContext *_ctx, const MuscleTaskSettings  &_config, TaskStateInfo& _ti, int _nThreads, const MultipleSequenceAlignment& _ma, MultipleSequenceAlignment& _res, bool _mhack)
        :ctx(_ctx), config(_config), ma(_ma->getCopy()), res(_res), mhack(_mhack), Weights(NULL), ProgNodes(NULL), ph(NULL), ti(_ti),
        treeNodeStatus(NULL), treeNodeIndexes(NULL), nThreads(_nThreads), uJoin(0), ptrbOscillating(NULL), bAnyAccepted(false), InternalNodeIndexes(NULL), uInternalNodeCount(0),
        bReversed(false), bRight(false), History(NULL), bLockLeft(NULL), bLockRight(false), msaIn(NULL)
    {
        refineConstructor();
    }
    MuscleWorkPool::~MuscleWorkPool() {
        delete[] Weights;
        delete[] ProgNodes;
        delete[] treeNodeStatus;
        delete[] treeNodeIndexes;
        Weights = NULL;
        ProgNodes = NULL;
        refineClear();
    }

    unsigned MuscleWorkPool::getJob() {
        QMutexLocker lock(&jobMgrMutex);
        unsigned uNodeCount = GuideTree.GetNodeCount();
        for(unsigned k=0; k<uNodeCount; k++) {
            unsigned uNodeIndex = treeNodeIndexes[k];
            if(treeNodeStatus[uNodeIndex] == TreeNodeStatus_Available) {
                treeNodeStatus[uNodeIndex] = TreeNodeStatus_Processing;
                return uNodeIndex;
            }
        }
        return NULL_NEIGHBOR;
    }
    unsigned MuscleWorkPool::getNextJob(unsigned uNodeIndex) {
        QMutexLocker lock(&jobMgrMutex);
        assert(treeNodeStatus[uNodeIndex] == TreeNodeStatus_Processing);
        treeNodeStatus[uNodeIndex] = TreeNodeStatus_Done;
        if(!GuideTree.IsRoot(uNodeIndex)) {
            unsigned uParentNodeIndex = GuideTree.GetParent(uNodeIndex);
            unsigned u_NeighborIndex = NULL_NEIGHBOR;
            if(GuideTree.GetRight(uParentNodeIndex) == uNodeIndex) 
                u_NeighborIndex = GuideTree.GetLeft(uParentNodeIndex);
            else 
                u_NeighborIndex = GuideTree.GetRight(uParentNodeIndex);
            if(treeNodeStatus[u_NeighborIndex] == TreeNodeStatus_Done) {
                assert(treeNodeStatus[uParentNodeIndex] == TreeNodeStatus_WaitForChild);
                treeNodeStatus[uParentNodeIndex] = TreeNodeStatus_Processing;
                return uParentNodeIndex;
            }
            else if(treeNodeStatus[u_NeighborIndex] == TreeNodeStatus_Available) {
                treeNodeStatus[u_NeighborIndex] = TreeNodeStatus_Processing;
                return u_NeighborIndex;
            }

            unsigned uNodeCount = GuideTree.GetNodeCount();
            for(unsigned k=0; k<uNodeCount; k++) {
                unsigned uNodeIndex = treeNodeIndexes[k];
                if(treeNodeStatus[uNodeIndex] == TreeNodeStatus_Available) {
                    treeNodeStatus[uNodeIndex] = TreeNodeStatus_Processing;
                    return uNodeIndex;
                }
            }
        }
        return NULL_NEIGHBOR;

    }

    ////////////////////////////
    // Refine
    ////////////////////////////
    void MuscleWorkPool::refineConstructor() {
        refineTI = NULL;
        uRangeCount = 1;
        uRangeIndex = 0;
        uIter = 0;
        uIters = 1;
        needRestart = new bool[nThreads];
        workerStartPos = new unsigned[nThreads];
        currentNodeIndex = new unsigned[nThreads];
        refineNodeStatuses = NULL;
        lastAcceptedIndex = 0;
        refineDone = true;
        oscillatingIter = NULL_NEIGHBOR;
        for(int i=0;i<nThreads;i++) {
            workerStartPos[i]=0;
            currentNodeIndex[i]=0;
            needRestart[i] = false;
        }
    }
    void MuscleWorkPool::refineClear() {
        
        delete[] workerStartPos;
        delete[] currentNodeIndex;
        delete[] needRestart;
        refineTI = NULL;
        workerStartPos = NULL;
        currentNodeIndex = NULL;
        needRestart = NULL;
        refineNodeStatuses = NULL;
    }
    void MuscleWorkPool::reset() {
        for (unsigned n = 0; n < uInternalNodeCount; ++n) {
            refineNodeStatuses[n] = RefineTreeNodeStatus_Available;
        }
        lastAcceptedIndex = 0;
        oscillatingIter = NULL_NEIGHBOR;
        for(int i=0;i<nThreads;i++) {
            workerStartPos[i]=0;
            currentNodeIndex[i]=NULL_NEIGHBOR;
            needRestart[i] = false;
        }
    }

    unsigned MuscleWorkPool::refineGetJob(MSA *_msaIn, int workerID) {
        QMutexLocker lock(&mut);
        bool availible = false;
        if(ctx->isCanceled())
            return NULL_NEIGHBOR;
        for(unsigned i=0;i<uInternalNodeCount;i++) {
            if(refineNodeStatuses[i] == RefineTreeNodeStatus_Available) {
                currentNodeIndex[workerID] = i;
                workerStartPos[workerID] = i;
                availible = true;
                _msaIn->Copy(*msaIn);
                refineNodeStatuses[i] = RefineTreeNodeStatus_Processing;
                break;
            }
        }
        if(!availible) {
            workerStartPos[workerID] = uInternalNodeCount - 1;
            currentNodeIndex[workerID] = NULL_NEIGHBOR;
        }
#if TRACE 
        log.trace(QString("uRangeIndex %1, uIter %2, workerID %3: start at index %4").arg(uRangeIndex).arg(uIter).arg(workerID).arg(currentNodeIndex[workerID]));
#endif
        return currentNodeIndex[workerID];
    }

    unsigned MuscleWorkPool::refineGetNextJob(MSA *_msaIn,bool accepted, SCORE scoreMax, const unsigned index, int workerID) {
        QMutexLocker lock(&mut);
        if(ctx->isCanceled()) {
#if TRACE 
            log.trace(QString("uRangeIndex %1, uIter %2, workerID %3: task canceled, lock worker").arg(uRangeIndex).arg(uIter).arg(workerID));
#endif
            return NULL_NEIGHBOR;
        }
        if(*ptrbOscillating) {
            if(oscillatingIter < index) {
#if TRACE
            log.trace(QString("uRangeIndex %1, uIter %2, workerID %3: Oscillating detected on prev iter").arg(uRangeIndex).arg(uIter).arg(workerID));    
#endif  
            return NULL_NEIGHBOR;
            }
        }
        if(needRestart[workerID]) {
            needRestart[workerID] = false;
            bool available = false;
            for(unsigned i=lastAcceptedIndex;i<uInternalNodeCount;i++) {
                if(refineNodeStatuses[i] == RefineTreeNodeStatus_Available) {
                    currentNodeIndex[workerID] = i;
                    available = true;
                    _msaIn->Copy(*msaIn);
                    refineNodeStatuses[i] = RefineTreeNodeStatus_Processing;
                    break;
                }
            }
            if(!available) {
#if TRACE 
                log.trace(QString("uRangeIndex %1, uIter %2, workerID %3: no job available, lock worker").arg(uRangeIndex).arg(uIter).arg(workerID));
#endif
                currentNodeIndex[workerID] = NULL_NEIGHBOR;
                return NULL_NEIGHBOR;
            }
#if TRACE 
            log.trace(QString("uRangeIndex %1, uIter %2, workerID %3: restarting worker with index %4").arg(uRangeIndex).arg(uIter).arg(workerID).arg(currentNodeIndex[workerID]));
#endif
            workerStartPos[workerID] = currentNodeIndex[workerID];
        }
        else {
			bool repeated = false;
			if(scoreMax!=-1)
				 repeated = History->SetScore(uIter, InternalNodeIndexes[index], bRight, scoreMax);
            refineNodeStatuses[index] = RefineTreeNodeStatus_Done;
            ctx->refinehoriz.g_uRefineHeightSubtree++;
            SetCurrentAlignment(*msaIn);
            Progress(ctx->refinehoriz.g_uRefineHeightSubtree, ctx->refinehoriz.g_uRefineHeightSubtreeTotal);
            refineTI->progress = (100.0*(uRangeIndex+((uIter+1.0)/uIters*(ctx->refinehoriz.g_uRefineHeightSubtree+1.0)/ctx->refinehoriz.g_uRefineHeightSubtreeTotal)))/uRangeCount;
            if (repeated)
            {
#if TRACE
                log.trace(QString("uRangeIndex %1, uIter %2, workerID %3: Oscillating").arg(uRangeIndex).arg(uIter).arg(workerID));    
#endif  
                msaIn->Copy(*_msaIn);
                oscillatingIter = index;
                *ptrbOscillating = true;
                for(unsigned j = index+1;j<uInternalNodeCount;j++) 
                    refineNodeStatuses[j] = RefineTreeNodeStatus_Done;
                currentNodeIndex[workerID] = NULL_NEIGHBOR;
                workerStartPos[workerID] = uInternalNodeCount - 1;
                return NULL_NEIGHBOR;
            }
#ifdef DEBUG
            //Validate node statuses
            int processingCount = 0;
            for(unsigned i=0;i<index;i++) {
                if(refineNodeStatuses[i]==RefineTreeNodeStatus_Processing)
                    processingCount++;
                else
                    assert(refineNodeStatuses[i]==RefineTreeNodeStatus_Done);
            }
            assert(processingCount < nThreads);
#endif

            if(accepted) {
#if TRACE 
                log.trace(QString("uRangeIndex %1, uIter %2, workerID %3: index %4 accepted").arg(uRangeIndex).arg(uIter).arg(workerID).arg(index));
#endif
                bAnyAccepted = true;
                
                if(*ptrbOscillating && oscillatingIter > index) 
                    *ptrbOscillating = false;

                for(int i=0;i<nThreads;i++) {
                    if(workerID!=i && workerStartPos[i] > index) {
                        needRestart[i] = true;
                        workerStartPos[i] = uInternalNodeCount - 1;    
#if TRACE 
                        log.trace(QString("\tu|Need for restart worker %1").arg(i));
#endif
                    }
                }
                
                for(unsigned j = index+2;j<uInternalNodeCount;j++) {
                    if(refineNodeStatuses[j] != RefineTreeNodeStatus_Available)
                        ctx->refinehoriz.g_uRefineHeightSubtree --; 
                    refineNodeStatuses[j] = RefineTreeNodeStatus_Available;
                    //TODO: start waiting workers 
                    /*if(waitWorkers > 0) {
                        waitWorkers --;
                        mainSem.release();
                    }*/
                }
                
                lastAcceptedIndex = index;
                msaIn->Copy(*_msaIn);
                currentNodeIndex[workerID]++;
                if(currentNodeIndex[workerID] >= uInternalNodeCount) {
#if TRACE 
                    log.trace(QString("uRangeIndex %1, uIter %2, workerID %3: no job available, lock worker").arg(uRangeIndex).arg(uIter).arg(workerID));
#endif
                    currentNodeIndex[workerID] = NULL_NEIGHBOR;
                    return NULL_NEIGHBOR;
                }

                workerStartPos[workerID] = currentNodeIndex[workerID];
                refineNodeStatuses[currentNodeIndex[workerID]] = RefineTreeNodeStatus_Processing;
            }
            else {
#if TRACE 
                log.trace(QString("uRangeIndex %1, uIter %2, workerID %3: index %4 done").arg(uRangeIndex).arg(uIter).arg(workerID).arg(index));
#endif
                bool available = false;
                for(unsigned i=index+1;i<uInternalNodeCount;i++) {
                    if(refineNodeStatuses[i] == RefineTreeNodeStatus_Available) {
                        currentNodeIndex[workerID] = i;
                        available = true;
                        refineNodeStatuses[i] = RefineTreeNodeStatus_Processing;
                        break;
                    }
                }   
                if(!available) {
                    currentNodeIndex[workerID] = NULL_NEIGHBOR;
#if TRACE 
                    log.trace(QString("uRangeIndex %1, uIter %2, workerID %3: no job available, lock worker").arg(uRangeIndex).arg(uIter).arg(workerID));
#endif
                    return NULL_NEIGHBOR;
                }
                if(currentNodeIndex[workerID] != index+1) {
                    workerStartPos[workerID] = currentNodeIndex[workerID];
                    _msaIn->Copy(*msaIn);
                }
            }
        }
#if TRACE 
        log.trace(QString("uRangeIndex %1, uIter %2, workerID %3: continue with index %4").arg(uRangeIndex).arg(uIter).arg(workerID).arg(currentNodeIndex[workerID]));
#endif
        return currentNodeIndex[workerID];
    }

} //namespace
