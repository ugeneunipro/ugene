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


#define TRACE 0

namespace U2 {

    struct Range
    {
        Range() : m_uBestColLeft(0), m_uBestColRight(0) {}

        unsigned m_uBestColLeft;
        unsigned m_uBestColRight;
    };

    static void ListVertSavings(unsigned uColCount, unsigned uAnchorColCount,
        const Range *Ranges, unsigned uRangeCount)
    {
        MuscleContext *ctx = getMuscleContext();
        if (!ctx->params.g_bVerbose || !ctx->params.g_bAnchors)
            return;
        double dTotalArea = uColCount*uColCount;
        double dArea = 0.0;
        for (unsigned i = 0; i < uRangeCount; ++i)
        {
            unsigned uLength = Ranges[i].m_uBestColRight - Ranges[i].m_uBestColLeft;
            dArea += uLength*uLength;
        }
        double dPct = (dTotalArea - dArea)*100.0/dTotalArea;
        Log("Anchor columns found       %u\n", uAnchorColCount);
        Log("DP area saved by anchors   %-4.1f%%\n", dPct);
    }

    static void ColsToRanges(const unsigned BestCols[], unsigned uBestColCount,
        unsigned uColCount, Range Ranges[])
    {
        // N best columns produces N+1 vertical blocks.
        const unsigned uRangeCount = uBestColCount + 1;
        for (unsigned uIndex = 0; uIndex < uRangeCount ; ++uIndex)
        {
            unsigned uBestColLeft = 0;
            if (uIndex > 0)
                uBestColLeft = BestCols[uIndex-1];

            unsigned uBestColRight = uColCount;
            if (uIndex < uBestColCount)
                uBestColRight = BestCols[uIndex];

            Ranges[uIndex].m_uBestColLeft = uBestColLeft;
            Ranges[uIndex].m_uBestColRight = uBestColRight;
        }
    }

    // Return true if any changes made
    bool RefineTask::RefineVertP(MSA* msaIn, unsigned uIters) 
    {
        bool bAnyChanges = false;
        const unsigned uColCountIn = msaIn->GetColCount();
        const unsigned uSeqCountIn = msaIn->GetSeqCount();

        if (uColCountIn < 3 || uSeqCountIn < 3)
            return false;

        unsigned *AnchorCols = new unsigned[uColCountIn];
        unsigned uAnchorColCount;
        SetMSAWeightsMuscle(*msaIn);
        FindAnchorCols(*msaIn, AnchorCols, &uAnchorColCount);

        const unsigned uRangeCount = uAnchorColCount + 1;
        Range *Ranges = new Range[uRangeCount];

        ColsToRanges(AnchorCols, uAnchorColCount, uColCountIn, Ranges);
        ListVertSavings(uColCountIn, uAnchorColCount, Ranges, uRangeCount);

        delete[] AnchorCols;

        MSA msaOut;
        msaOut.SetSize(uSeqCountIn, 0);

        for (unsigned uSeqIndex = 0; uSeqIndex < uSeqCountIn ; ++uSeqIndex)
        {
            const char *ptrName = msaIn->GetSeqName(uSeqIndex);
            unsigned uId = msaIn->GetSeqId(uSeqIndex);
            msaOut.SetSeqName(uSeqIndex, ptrName);
            msaOut.SetSeqId(uSeqIndex, uId);
        }


        MuscleContext* ctx = getMuscleContext();
        workpool->uRangeCount = uRangeCount;

        for (unsigned uRangeIndex = 0; uRangeIndex < uRangeCount && !ctx->isCanceled(); ++uRangeIndex)
        {
            workpool->uRangeIndex = uRangeIndex;
            MSA msaRange;
            const Range &r = Ranges[uRangeIndex];

            const unsigned uFromColIndex = r.m_uBestColLeft;
            const unsigned uRangeColCount = r.m_uBestColRight - uFromColIndex;

            if (0 == uRangeColCount)
                continue;
            else if (1 == uRangeColCount)
            {
                MSAFromColRange(*msaIn, uFromColIndex, 1, msaRange);
                MSAAppend(msaOut, msaRange);
                continue;
            }
            MSAFromColRange(*msaIn, uFromColIndex, uRangeColCount, msaRange);
            bool &bLockLeft = workpool->bLockLeft;
            bool &bLockRight = workpool->bLockRight;
            bLockLeft = (0 != uRangeIndex);
            bLockRight = (uRangeCount - 1 != uRangeIndex);
            bool bAnyChangesThisBlock = RefineHorizP(&msaRange, uIters, bLockLeft, bLockRight);
            bAnyChanges = (bAnyChanges || bAnyChangesThisBlock);

            MSAAppend(msaOut, msaRange);

        }

        delete[] Ranges;
        if (ctx->isCanceled()) {
            throw MuscleException("Canceled");
        }

#if	DEBUG
        // Sanity check
        AssertMSAEqIgnoreCaseAndGaps(*msaIn, msaOut);
#endif

        if (bAnyChanges) {
            msaIn->Copy(msaOut);
        }
        return bAnyChanges;
    }

} //namespace
