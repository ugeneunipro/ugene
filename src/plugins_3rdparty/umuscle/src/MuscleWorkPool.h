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

#ifndef _MUSCLEWORKPOOL_H_
#    define _MUSCLEWORKPOOL_H_

#    include <QMutex>
#    include <QMutexLocker>
#    include <QSemaphore>

#    include "MuscleParallel.h"
#    include "MuscleUtils.h"
#    include "muscle/scorehistory.h"

namespace U2 {

enum TreeNodeStatus {
    TreeNodeStatus_WaitForChild,
    TreeNodeStatus_Available,
    TreeNodeStatus_Processing,
    TreeNodeStatus_Done
};

enum RefineTreeNodeStatus {
    RefineTreeNodeStatus_Available,
    RefineTreeNodeStatus_Processing,
    RefineTreeNodeStatus_Done,
};

struct MuscleWorkPool {
    MuscleWorkPool(MuscleContext* _ctx, const MuscleTaskSettings& _config, TaskStateInfo& _ti, int _nThreads, const MultipleAlignment& _ma, MultipleAlignment& _res, bool _mhack);

    ~MuscleWorkPool();

    unsigned getJob();
    unsigned getNextJob(unsigned uNodeIndex);

    MuscleContext* ctx = nullptr;
    const MuscleTaskSettings& config;
    MultipleAlignment ma;
    MultipleAlignment& res;
    bool mhack;
    SeqVect v;
    Tree GuideTree;
    MSA a;
    WEIGHT* Weights = nullptr;
    ProgNode* ProgNodes = nullptr;
    MuscleParamsHelper* ph = nullptr;
    TaskStateInfo& ti;
    TreeNodeStatus* treeNodeStatus = nullptr;
    unsigned* treeNodeIndexes = nullptr;
    int nThreads = 1;
    unsigned uJoin = 0;
    QMutex jobMgrMutex;
    QMutex proAligMutex;
    ////////////////////////////
    // Refine
    ////////////////////////////
    void refineConstructor();
    void refineClear();
    void reset();

    unsigned refineGetJob(MSA* _msaIn, int workerID);

    unsigned refineGetNextJob(MSA* _msaIn, bool accepted, SCORE scoreMax, unsigned index, int workerID);

    unsigned isRefineDone() const {
        return refineDone; /*|| ctx->isCanceled();*/
    }

    bool* ptrbOscillating = nullptr;
    unsigned oscillatingIter = NULL_NEIGHBOR;
    bool bAnyAccepted = false;
    unsigned* InternalNodeIndexes = nullptr;
    unsigned uInternalNodeCount = 0;
    bool bReversed = false;
    bool bRight = false;
    unsigned uIter = 0;
    ScoreHistory* History = nullptr;
    bool bLockLeft = false;
    bool bLockRight = false;

    bool refineDone = true; // TODO: check if we can set default to false. Historically it was true.
    QSemaphore mainSem;
    QSemaphore childSem;
    QMutex mut;
    RefineTreeNodeStatus* refineNodeStatuses;
    bool* needRestart;
    unsigned lastAcceptedIndex = 0;
    unsigned* currentNodeIndex;
    unsigned* workerStartPos;
    MSA* msaIn = nullptr;

    unsigned uIters = 1;
    unsigned uRangeIndex = 0;
    unsigned uRangeCount = 1;
    TaskStateInfo* refineTI = nullptr;
};

#endif  //_MUSCLEWORKPOOL_H_
}  // namespace
