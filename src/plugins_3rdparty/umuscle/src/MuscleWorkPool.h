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

#ifndef _MUSCLEWORKPOOL_H_
#define _MUSCLEWORKPOOL_H_

#include "MuscleParallel.h"
#include "MuscleUtils.h"

#include <QMutex>
#include <QMutexLocker>
#include <QSemaphore>

#include "muscle/scorehistory.h"

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
        RefineTreeNodeStatus_DoneFinaly
    };

    struct MuscleWorkPool {
        MuscleWorkPool(MuscleContext *_ctx, const MuscleTaskSettings  &_config, TaskStateInfo& _ti, int _nThreads, const MultipleSequenceAlignment& _ma, MultipleSequenceAlignment& _res, bool _mhack);
            
        ~MuscleWorkPool() ;

        unsigned getJob();
        unsigned getNextJob(unsigned uNodeIndex);

        MuscleContext       *ctx;
        const MuscleTaskSettings  &config;
        MultipleSequenceAlignment          ma;
        MultipleSequenceAlignment&         res;
        bool                mhack;
        SeqVect             v;
        Tree                GuideTree;
        MSA                 a;
        WEIGHT              *Weights;
        ProgNode            *ProgNodes;
        MuscleParamsHelper  *ph;
        TaskStateInfo&      ti;
        TreeNodeStatus*     treeNodeStatus;
        unsigned*           treeNodeIndexes;
        int                 nThreads;
        unsigned            uJoin;
        QMutex              jobMgrMutex;
        QMutex              proAligMutex;
        ////////////////////////////
        // Refine
        ////////////////////////////
        void refineConstructor();
        void refineClear();
        void reset();

        unsigned refineGetJob(MSA *_msaIn, int workerID);

        unsigned refineGetNextJob(MSA *_msaIn, bool accepted, SCORE scoreMax, unsigned index, int workerID);

        unsigned isRefineDone() {
            return refineDone; /*|| ctx->isCanceled();*/
        }

        bool                *ptrbOscillating;
        unsigned            oscillatingIter;
        bool                bAnyAccepted;
        unsigned            *InternalNodeIndexes;
        unsigned            uInternalNodeCount;
        bool                bReversed;
        bool                bRight;
        unsigned            uIter;
        ScoreHistory        *History;
        bool                bLockLeft;
        bool                bLockRight;

        bool                refineDone;
        QSemaphore          mainSem;
        QSemaphore          childSem;
        QMutex              mut;
        RefineTreeNodeStatus* refineNodeStatuses;
        bool*               needRestart;
        unsigned            lastAcceptedIndex;
        unsigned*           currentNodeIndex;
        unsigned*           workerStartPos; 
        MSA*                msaIn;

        unsigned            uIters;
        unsigned            uRangeIndex;
        unsigned            uRangeCount;
        TaskStateInfo       *refineTI;

    };

#endif //_MUSCLEWORKPOOL_H_
} //namespace
