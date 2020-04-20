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

#include "RFSArrayWK.h"

namespace U2 {

RFSArrayWKAlgorithm::RFSArrayWKAlgorithm(RFResultsListener* rl, const char* seqX, int sizeX, const char* seqY, int sizeY,
                                         DNAAlphabetType seqType, int w, int k)
: RFAlgorithmBase(rl, seqX, sizeX, seqY, sizeY, seqType, w, k, TaskFlags_NR_FOSCOE)
{
    q = WINDOW_SIZE/(C+1);
    indexTask = NULL;
    if (sizeX > sizeY) {
        arrayIsX = false;
        SEARCH_SIZE = sizeX;
        ARRAY_SIZE = sizeY;
        searchSeq = seqX;
        arraySeq = seqY;
    } else {
        arrayIsX = true;
        SEARCH_SIZE = sizeY;
        ARRAY_SIZE = sizeX;
        searchSeq = seqY;
        arraySeq = seqX;

    }
}

void RFSArrayWKAlgorithm::prepare() {
    RFAlgorithmBase::prepare();
    if (hasError()) {
        return;
    }
    // ARRAY_SIZE is smaller than SEARCH_SIZE. Allocate diags only for ARRAY_SIZE elements ->
    // when ARRAY_SIZE diags are processed, 0 one becomes unused and ARRAY_SIZE+1 one is placed to 0 cell
    diagOffsets.reserve(ARRAY_SIZE);
    diagOffsets.resize(ARRAY_SIZE);

    if (diagOffsets.isEmpty()) {
        stateInfo.setError(tr("Memory allocation error, size: %1").arg(ARRAY_SIZE));
        return;
    }
    diagOffsets.fill(-1);

    nThreads = qBound(1, getNumParallelSubtasks(), SEARCH_SIZE / (20*1000));

    indexTask = new CreateSArrayIndexTask(arraySeq, ARRAY_SIZE, q, unknownChar);
    int arrayPercent = 10;
    indexTask->setSubtaskProgressWeight(arrayPercent/100.0F);
    addSubTask(indexTask);
    setMaxParallelSubtasks(1);

    for (int i = 0; i < nThreads; i++) {
        RFSArrayWKSubtask* t = new RFSArrayWKSubtask(this, i);
        t->setSubtaskProgressWeight((100 - arrayPercent) / (100.0F*nThreads));
        addSubTask(t);
    }
}

void RFSArrayWKAlgorithm::cleanup() {
    diagOffsets.resize(0);
}

QList<Task*> RFSArrayWKAlgorithm::onSubTaskFinished(Task* subTask) {
    if (subTask == indexTask) {
        setMaxParallelSubtasks(nThreads);
    }
    return QList<Task*>();
}

#define IS_THREAD_DIAG(diagNum) \
    ((nThreads == 1) || (diagNum % nThreads == t->tid))

void RFSArrayWKAlgorithm::calculate(RFSArrayWKSubtask* t) {
    SArrayIndex* _index =  indexTask->index;

    int W = WINDOW_SIZE;
    int CMAX = C;

    const char* dataA = arraySeq;
    const char* dataAEnd = dataA + ARRAY_SIZE;
    int aSize = ARRAY_SIZE;

    const char* dataS = searchSeq;
    const char* dataSEnd = searchSeq + SEARCH_SIZE;
    int sSize = SEARCH_SIZE;

    int percentLen = (dataSEnd - dataS) / 100;
    int reportS = percentLen;

    int* diags = diagOffsets.data();

    for (int s = 0, maxS = sSize - W + 1, s0diag = 0; s < maxS && !t->isCanceled(); s++, s0diag = (s0diag == aSize - 1) ? 0 : s0diag + 1) {
        if (IS_THREAD_DIAG(s0diag)) {
            // s0diag is a border position for rolling (over 2 sequences) diagOffsets.
            // Remember: diagOffset size is not sizeS+sizeA, but sizeA (that is < sizeS) only size
            // and it's zero point 'rolls' (or shifts) as scope of possible active diagonals shifts
            diags[s0diag] = -1;
        }
        if (s == reportS) {
            t->stateInfo.progress++;
            reportS+=percentLen;
        }
        if (!_index->find(t, dataS + s)) {
            continue;
        }
        //here we have iterator set to the first match in A, guaranteed match length for all values returned by iterator = Q
        int a;
        while ((a = _index->nextArrSeqPos(t))!=-1) {
            if (reflective && s >= a) {
                continue;
            }
            int diag = a > s0diag ? aSize + s0diag - a : s0diag - a ;
            assert(diag >=0 && diag < diagOffsets.size());
            if (!IS_THREAD_DIAG(diag)) { //this diagonal is assigned to another thread -> skip it
                continue;
            }
            int checkedS = diags[diag];
            if (checkedS >= s || a + W > aSize || s + W > sSize) {
                continue;

            }

            assert(s == 0 || a == 0 || !PCHAR_MATCHES(dataS + s - 1, dataA + a - 1)); //this result must be processed on prev s step!

            // construct initial window, startS/startA - start positions for potential hit
            const char* startS = dataS + s;
            const char* startA = dataA + a;
            const char* posS = startS + q;
            const char* posA = startA + q;
            assert(PCHAR_MATCHES(startS, startA));
            const char* endS = startS + W;
            //find first mismatch -> need it to update diags
            for (; PCHAR_MATCHES(posS, posA) && posS < endS; posS++, posA++){}
            int c = 0;
            if (posS != endS) { //there was mismatch on W range -> save its pos and find all other mismatches on W range
                const char* firstMismatchPosS = posS;
                for (c = 1; (c += PCHAR_MATCHES(posS, posA) ? 0 : 1) <= CMAX && posS < endS; posS++, posA++) {}
                assert(posS - startS <= W);
                if (c > CMAX) {// to many mismatches on forward strand
                    assert(!PCHAR_MATCHES(firstMismatchPosS, posA - (posS - firstMismatchPosS)));
                    diags[diag] = firstMismatchPosS - dataS;
                    continue;
                }
            }
            // have a hit! -> expand window forward as much as possible
            // posS/A points to the first unchecked base here
            assert(posS - startS == W);
            int allMismatches = 0;
            for (; c <= CMAX && posA < dataAEnd && posS < dataSEnd; posA++, posS++) {
                int popVal = PCHAR_MATCHES(posA - W, posS - W) ? 0 : 1;
                int pushVal = PCHAR_MATCHES(posA, posS) ? 0 : 1;
                //allMismatches += pushVal - popVal;
                assert(c>=0);
                c += pushVal - popVal;
            }

            int len = posS - startS;
            int lastCheckedS = s + len - (W - q + 1); // the first point of the last window checked

            const char *posAA = startA, *posSS = startS;
            for (int ppppos = 0; ppppos < len ; posAA++, posSS++, ppppos++) {
                allMismatches += PCHAR_MATCHES(posAA, posSS) ? 0 : 1;
            }

            //now shift to the first mismatch pos and save value in diags
            const char* firstMismatchPosS = dataS + lastCheckedS - 1;
            const char* firstMismatchPosA = dataA + a + (lastCheckedS - 1 - s);
            while (++firstMismatchPosS < dataSEnd && ++firstMismatchPosA < dataAEnd && PCHAR_MATCHES(firstMismatchPosS, firstMismatchPosA)) {}
            assert(firstMismatchPosS<=posS);
            diags[diag] = firstMismatchPosS - dataS;

            //ensure that match with len > W ends with hit
            while (len > W && !PCHAR_MATCHES(startS + len - 1, startA + len - 1)){len--; allMismatches--;}

            //save result
            addResult(a, s, len, len-allMismatches);
            assert(len >= W);//a place for a break-point
        }
    }
}

void RFSArrayWKAlgorithm::addResult(int a, int s, int l, int c) {
    RFResult res((arrayIsX ? a : s), (arrayIsX ? s : a), l, c);
    assert(checkResult(res));
    addToResults(res);
}


RFSArrayWKSubtask::RFSArrayWKSubtask(RFSArrayWKAlgorithm* _owner, int _tid)
: Task(tr("Find repeats subtask (suffixWK)"), TaskFlag_None), owner(_owner),  tid(_tid)
{
    tpm = Task::Progress_Manual;
}

void RFSArrayWKSubtask::run() {
    stateInfo.progress = 0;
    owner->calculate(this);
}

} //namespace
