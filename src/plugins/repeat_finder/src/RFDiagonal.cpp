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

#include "RFDiagonal.h"

#include <U2Core/DNATranslation.h>

#include <QMutex>

namespace U2 {

RFDiagonalAlgorithmWK::RFDiagonalAlgorithmWK(RFResultsListener* rl, const char* seqX, int sizeX, const char* seqY, int sizeY,
                                            DNAAlphabetType seqType,  int w, int k)
: RFAlgorithmBase(rl, seqX, sizeX, seqY, sizeY, seqType, w, k)
{
    setMaxParallelSubtasks(MAX_PARALLEL_SUBTASKS_AUTO);

    START_DIAG = SIZE_X - WINDOW_SIZE;
    END_DIAG = reflective ? 1 : -int(SIZE_Y - WINDOW_SIZE);
}

void RFDiagonalAlgorithmWK::prepare(){
    RFAlgorithmBase::prepare();
    if (hasError()) {
        return;
    }
    int nThreads = getNumParallelSubtasks();
    for (int i = 0; i < nThreads; i++) {
        addSubTask(new RFDiagonalWKSubtask(this, i, nThreads));
    }
}

//////////////////////////////////////////////////////////////////////////
/// RFDiagonalWKSubtask methods

RFDiagonalWKSubtask::RFDiagonalWKSubtask(RFDiagonalAlgorithmWK* owner, int tn, int totalThreads)
: Task(tr("Find repeats subtask (diagonal)"), TaskFlag_None),
owner(owner), threadNum(tn), nThreads(totalThreads), dataX(owner->seqX), dataY(owner->seqY)
{
    assert(totalThreads >= 1);

    //progress is approximated by area processed
    if ((owner->END_DIAG <= 0 && owner->START_DIAG <= 0) || (owner->END_DIAG >= 0 && owner->START_DIAG >= 0)) {
        int diagRange = owner->START_DIAG - owner->END_DIAG + 1;
        int medDiag = (owner->START_DIAG + owner->END_DIAG) / 2;
        areaS = quint64(getDiagLen(medDiag)) * diagRange / totalThreads;
    } else {
        int medDiag1 = owner->START_DIAG / 2;
        int medDiag2 = owner->END_DIAG / 2;
        quint64 areaS1 = quint64(getDiagLen(medDiag1)) * qAbs(owner->START_DIAG) / totalThreads;
        quint64 areaS2 = quint64(getDiagLen(medDiag2)) * qAbs(owner->END_DIAG) / totalThreads;
        areaS = areaS1 + areaS2;
    }
    areaS = areaS == 0 ? 1 : areaS;
    currentS = 0;
    tpm = Task::Progress_Manual;
}

int RFDiagonalWKSubtask::getDiagLen(int d) const {
    return (d > 0) ? qMin(owner->SIZE_X - d, owner->SIZE_Y) : qMin(owner->SIZE_Y + d, owner->SIZE_X);
}

void RFDiagonalWKSubtask::run(){
    int start_diag = owner->START_DIAG;
    int end_diag = owner->END_DIAG;
    int d = start_diag - threadNum;

    while (d >= end_diag && !isCanceled()) {
        int x = d > 0 ? d : 0;
        int y = d > 0 ? 0 : -d;
        processDiagonal(x, y);
        d -= nThreads;

        currentS+= getDiagLen(d);
        stateInfo.progress = qMin(100, int(100 * currentS / areaS));
    }
}


void RFDiagonalWKSubtask::processDiagonal(int x, int y){
    //make stack local copies
    int W = owner->WINDOW_SIZE;
    int C = owner->C;
    char unknownChar = owner->unknownChar;

    const char* xseq = dataX + x + W - 1; //point to the last pos in window -> will be checked first
    const char* yseq = dataY + y + W - 1;
    const char* xseqMax = dataX + owner->SIZE_X;
    const char* yseqMax = dataY + owner->SIZE_Y;

    assert(xseqMax - xseq >= 0 && yseqMax - yseq >= 0);
    while (xseq < xseqMax && yseq < yseqMax) {
        int c = 0; //number of mismatches (temporary)
        for (const char* s = xseq - W; xseq > s && (c += (PCHAR_MATCHES(xseq, yseq) ? 0 : 1)) <= C; xseq--, yseq--){}
        if (c > C) {
            xseq += W;
            yseq += W;
            continue;
        }
        assert(xseq < dataX || yseq < dataY || !PCHAR_MATCHES(xseq, yseq));
        xseq++;
        yseq++;
        int len = processMatch(xseq, yseq, xseqMax, yseqMax, c);
        assert(len >= W);
        while (len > W && !PCHAR_MATCHES(xseq, yseq)){len--; xseq++; yseq++;} //ensure that match with len > W starts with hit
        int step = len;
        while (len > W && !PCHAR_MATCHES(xseq + len - 1, yseq + len - 1)){len--;} //ensure that match with len > W ends with hit
        int allMatches = 0;
        const char* xsqS = xseq;
        const char* ysqS = yseq;
        for(int i = len-1; i>=0; --i, xsqS++, ysqS++){allMatches += PCHAR_MATCHES(xsqS, ysqS)? 1 : 0;} //matches
        RFResult r(xseq - dataX, yseq - dataY, len, allMatches);
        assert(owner->checkResult(r));
        diagResults.append(r);
        xseq += step + 1;
        yseq += step + 1;
    }
    if (!diagResults.isEmpty()) {
        owner->addToResults(diagResults);
        diagResults.clear();
    }
}

int RFDiagonalWKSubtask::processMatch(const char* x, const char* y, const char* xEnd, const char* yEnd, int c) {
    int W = owner->WINDOW_SIZE;
    int K = owner->K;
    int k = W - c;
    char unknownChar = owner->unknownChar;
    const char *xr = x + W, *yr = y + W;
    for (; xr < xEnd && yr < yEnd; ++xr, ++yr) {
        int pushV = PCHAR_MATCHES(xr, yr) ? 1 : 0;
        int popV = PCHAR_MATCHES(xr-W, yr-W) ? 1 : 0;
        k += pushV - popV;
        if (k < K) { //end of the match
            break;
        }
    }
    return xr - x;
}

} //namespace
