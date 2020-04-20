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

#include "RFSArray.h"
#include "RFConstants.h"

#include <U2Core/Timer.h>
#include <U2Core/Log.h>

#include <QMutexLocker>
#include <U2Core/DNAAlphabet.h>

namespace U2 {

RFSArrayWAlgorithm::RFSArrayWAlgorithm(RFResultsListener* rl, const char* seqX, int sizeX,
                                       const char* seqY, int sizeY,  const DNAAlphabet *al, int w)
: RFAlgorithmBase(rl, seqX, sizeX, seqY, sizeY, al->getType(), w, w, TaskFlags_FOSCOE)
{
    indexTask = NULL;
    nThreads = 1;
    int maxSize = qMax(SIZE_X, SIZE_Y);
    int minSize = qMin(SIZE_X, SIZE_Y);

    bool arrayIsMax = true;
    int gap = w - getWGap(w);
    int maxWithGapSize = maxSize/(gap+1);
    int minWithGapSize = minSize/(gap+1);
    if (maxSize > 1000*1000) {
        if (maxWithGapSize > minSize) {
            arrayIsMax=false;
        } else if (maxWithGapSize * 8 > 128*1000*1000) { //too many mem to use max -> using min will reduce mem usage
            arrayIsMax=false;
        } else {
            double searchK = 1.3;
            int tMinIndexed = minWithGapSize + searchK*maxSize;
            int tMaxIndexed = maxWithGapSize + searchK*minSize;
            if (tMinIndexed < tMaxIndexed) {
                arrayIsMax=false;
            } else if /*practical*/ ((gap < 16 && maxSize > 50*1000*1000) ||
                (gap < 20 && maxSize > 100*1000*1000) ||
                (gap < 25 && maxSize > 200*1000*1000) ||
                (gap < 32 && maxSize > 400*1000*1000) ||
                (gap < 37 && maxSize > 500*1000*1000)) {
                    arrayIsMax = false;
            }
        }
    }
    arrayIsX = arrayIsMax && SIZE_X >=SIZE_Y;
    arraySeq = arrayIsX ? seqX : seqY;
    searchSeq = arrayIsX ? seqY : seqX;
    ARRAY_SIZE = arrayIsX ? SIZE_X: SIZE_Y;
    SEARCH_SIZE = arrayIsX ? SIZE_Y: SIZE_X;

    bitMaskCharBitsNum = bt.getBitMaskCharBitsNum(al->getType());
    if ((al->getId() == BaseDNAAlphabetIds::NUCL_DNA_DEFAULT()) || (al->getId() == BaseDNAAlphabetIds::NUCL_RNA_DEFAULT())) {
        bitMask = bt.getBitMaskCharBits(al->getType());
    }
    else {
        bitMask = NULL;
    }
    //single thread approximation (re-estimated in some algorithms)
    arrayPercent = int((ARRAY_SIZE / double(ARRAY_SIZE + SEARCH_SIZE)) * 100 / 5); //array creation time ~5 times faster than search
}

void RFSArrayWAlgorithm::prepare() {
    RFAlgorithmBase::prepare();
    if (hasError()) {
        return;
    }

    int sSize = SEARCH_SIZE;

    nThreads = qBound(1, getNumParallelSubtasks(), SEARCH_SIZE / (20*1000));

    //create index task that must be executed before all other tasks
    int matchSize = getWGap(WINDOW_SIZE);
    int gap = WINDOW_SIZE - matchSize;

    indexTask = new CreateSArrayIndexTask(arraySeq, ARRAY_SIZE, matchSize, unknownChar, bitMask, bitMaskCharBitsNum, gap);
    indexTask->setSubtaskProgressWeight(arrayPercent/100.0F);
    addSubTask(indexTask);
    setMaxParallelSubtasks(1);

    // create all other tasks that will use index
    int start = 0;
    int len = sSize / nThreads;
    for (int i = 0; i < nThreads; i++) {
        int sStart = start ? start - WINDOW_SIZE+1:0;
        int sEnd = i < nThreads - 1 ? start + len : sSize;
        RFSArrayWSubtask* t = new RFSArrayWSubtask(this, sStart, sEnd, i);
        t->setSubtaskProgressWeight((100 - arrayPercent) / (100.0F*nThreads));
        addSubTask(t);
        start+=len;
    }
}

QList<Task*> RFSArrayWAlgorithm::onSubTaskFinished(Task* subTask) {
    if (subTask == indexTask) {
        setMaxParallelSubtasks(nThreads);
    }
    return QList<Task*>();
}


void RFSArrayWAlgorithm::run(RFSArrayWSubtask* t) {
    calculate(t);
}

void RFSArrayWAlgorithm::run() {
    try {
        processBoundaryResults();
    }
    catch(...) {
        setError("Not enough memory");
    }
}

void RFSArrayWAlgorithm::calculate(RFSArrayWSubtask* t) {
    quint64 t0 = GTimer::currentTimeMicros();

    int W = WINDOW_SIZE;
    int W_GAP = getWGap(W);
    int GAP = W - W_GAP;
    int aSize = ARRAY_SIZE;
    int sSize = t->sEnd - t->sStart;
    const char* dataA = arraySeq;
    const char* dataS = searchSeq + t->sStart;
    const char* dataAEnd = dataA + aSize;
    const char* dataSEnd = dataS + sSize;
    const char* dataSCheckEnd= dataSEnd-W_GAP+1;
    int reportLen = sSize / (100 - arrayPercent);
    const char* reportPos = dataS + reportLen;

    EdgePool edgePool(3000);
    QVector<CheckEdge*> chains(GAP+1);
    for (int i = 0 ;i < chains.size(); i++) {
        chains[i] = new CheckEdge();
    }
    bool _useBitMask = bitMask!=NULL;
    SArrayIndex* index = indexTask->index;
    assert(index!=NULL);

    quint32 bitValue = 0xFFFFFFFF;
    quint32 charBitsNum = bitMaskCharBitsNum;
    int wCharsInMask = index->getCharsInMask();
    const quint32* bm = bitMask;
    quint32 bitFilter = index->getBitFilter();
    int nNew = 0;
    int nMatches = 0;
    int chainIdx = 0;
    int& progress = t->stateInfo.progress;
    for (const char* posS = dataS; posS < dataSCheckEnd && !stateInfo.cancelFlag; posS++, chainIdx = chainIdx == GAP ? 0 : chainIdx + 1) {
        if (posS >= reportPos) {
            progress++;
            reportPos += reportLen;
        }

        //validate edges from prev steps
        CheckEdge* chain = chains[chainIdx];
        for (CheckEdge *edge = chain->next, *next = NULL; edge->lastS < posS && edge!=chain; edge = next) {
            next = edge->next;
            edge->fromChain();
            // now extend result forward
            const char* lastS = edge->lastS;
            const char* lastA = dataA + (lastS-dataS) + edge->diag;
            for (;lastS < dataSEnd && lastA < dataAEnd && PCHAR_MATCHES(lastS, lastA); lastS++, lastA++){}

            int len = lastS - edge->posS;
            if (len >= W) {
                int s = edge->posS - dataS;
                int a = edge->diag + s;
                addResult(a, s, len, 0, t); // 0 mismatches
            }
            edgePool.returnEdge(edge);
        }

        //setup iterator
        if (_useBitMask ) {
            uchar c = *(posS + wCharsInMask - 1);
            bool bitValueCreated = true;

            if (c != unknownChar && posS!= dataS) {
                bitValue = ((bitValue<<charBitsNum) | bm[c]) & bitFilter;
            } else { //if there are unknown chars -> rebuild mask starting from the first W without unknown chars
                bitValue = 0;
                if (posS!=dataS) {
                    posS+=wCharsInMask;
                }
                int cleanChars;
                for (cleanChars = 0; cleanChars < wCharsInMask && posS < dataSEnd; posS++) {
                    if (*posS==unknownChar) {
                        cleanChars = 0;
                        bitValue = 0;
                    } else {
                        bitValue = (bitValue<<charBitsNum) | bm[uchar(*posS)];
                        cleanChars++;
                    }
                }

                bitValueCreated = cleanChars > 0;

                posS-=wCharsInMask;
                if(posS >= dataSCheckEnd) {
                    break;
                }
                chainIdx=(posS-dataS)%(GAP+1);
                chain = chains[chainIdx];
            }
            if (bitValueCreated && !index->findBit(t, bitValue, posS)) { // if there were not unknown chars, bitValue is initialized
                continue;
            }
        } else {
            if (!index->find(t, posS)) {
                continue;
            }
        }
        //iterate over sarray hits
        int s = posS - dataS;
        int a = 0;
        while ((a = index->nextArrSeqPos(t))!=-1) {
            nMatches++;
            if (reflective && s + t->sStart >= a) {
                continue;
            }
            //check if this hit can be merged with older one
            int diag = a-s;
            bool merged = false;
            CheckEdge* edge = chain->next;
            for (; edge!=chain; edge=edge->next) {
                if (edge->diag != diag) {
                    continue;
                }
                if (edge->lastS < posS) { // gap while passing unknownChar
                    break;
                }
                merged = true;
                edge->lastS = posS + W_GAP;
                edge->fromChain();
                edge->toChain(chain);//making edge last
                break;
            }
            if (!merged) {
                //extend backward
                const char* edgeS = posS - 1;
                const char* edgeA = dataA + a - 1;
                for (;edgeS >= dataS && edgeA >= dataA && PCHAR_MATCHES(edgeS, edgeA); edgeS--, edgeA--){};
                edgeS++;
                edge = edgePool.allocEdge(edgeS, posS + W_GAP, diag);
                edge->toChain(chain);
            }
        }
    }
    for (chainIdx=0; chainIdx < chains.size(); chainIdx++) {
        CheckEdge* chain = chains[chainIdx];
        for (CheckEdge *edge = chain->next, *next = NULL; edge!=chain; edge = next) { //expand forward
            next = edge->next;
            const char* lastS = edge->lastS;
            const char* lastA = dataA + (lastS-dataS) + edge->diag;
            for (;lastS < dataSEnd && lastA < dataAEnd && PCHAR_MATCHES(lastS, lastA); lastS++, lastA++){}
            edge->lastS = lastS;

            int len = edge->lastS - edge->posS;
            int s = edge->posS - dataS;
            int a = edge->diag + s;
            if (len >= W) {
                addResult(a, s, len, 0, t); // 0 mismatches
            }
            delete edge;
        }
        delete chains[chainIdx];
    }

    quint64 t1 = GTimer::currentTimeMicros();
    algoLog.trace(QString("Done, nNew %1, nMatches %2").arg(nNew).arg(nMatches));
    algoLog.trace(QString("Done. Thread %1, Search time: %2 sec").arg(t->tid).arg(double(t1-t0)/(1000*1000)));
}

static bool resultsIntrersectR1R2(const RFResult &r1, const RFResult &r2) {
    if (r1.x <= r2.x) {
        if (r1.x + r1.l >= r2.x) {
            return true;
        } else {
            return false;
        }
    }
    return false;
}

static bool resultsIntrersectR2R1(const RFResult &r1, const RFResult &r2) {
    if (r2.x <= r1.x) {
        if (r2.x + r2.l >= r1.x) {
            return true;
        } else {
            return false;
        }
    }
    return false;
}

static bool resultsIntrersect(const RFResult &r1, const RFResult &r2) {
    bool r1r2 = resultsIntrersectR1R2(r1, r2);
    bool r2r1 = resultsIntrersectR2R1(r1, r2);
    return (r1r2 || r2r1);
}

void RFSArrayWAlgorithm::processBoundaryResults() {
    //called after all subtasks finished -> merge boundary results
    RFResult* rs = bresults.data();
    for (int j=0, n = bresults.size(); j < n; j++) {
        RFResult& rj = rs[j];
        if (rj.l == -1) { //was merged
            continue;
        }
        int dj = rj.x - rj.y;
        for (int i = 0; i < n; i++) {
            if (i == j) {
                continue;
            }
            RFResult& ri = rs[i];
            if (ri.l == -1) { //was merged
                continue;
            }
            int di = ri.x - ri.y;
            if (dj != di) {
                continue;
            }
            if (!resultsIntrersect(rj, ri)) {
                continue;
            }
            bool rjri = resultsIntrersectR1R2(rj, ri);
            bool rirj = resultsIntrersectR2R1(rj, ri);

            if (rirj) {
                ri.l = rj.x + rj.l - ri.x;
                ri.c = ri.l;
                assert(ri.l >=0);

                rj.l = -1;
                break;
            } else if (rjri) {
                rj.l = ri.x + ri.l - rj.x;
                rj.c = rj.l;
                assert(rj.l >=0);

                ri.l = -1;
            }
        }
    }

    QVector<RFResult> mergedResults;
    for (int j=0, n = bresults.size(); j < n; j++) {
        const RFResult& rj = rs[j];
        if (rj.l != -1) {
            mergedResults.append(rj);
        }
    }
    //assert(checkResults(mergedResults));
    addToResults(mergedResults);
}

// ast: add one hit in sequence
void RFSArrayWAlgorithm::addResult( int a, int s, int l, int c, RFSArrayWSubtask* t )
{
    bool boundary = nThreads > 1 && (s == 0 || s + l == t->sEnd - t->sStart);
    s+=t->sStart;
    RFResult r((arrayIsX ? a : s), (arrayIsX ? s : a), l, c);
    if (boundary) {
        QMutexLocker ml(&boundaryMutex);
        bresults.append(r);
    } else {
        assert(checkResult(r));
        addToResults(r);
    }
}

int RFSArrayWAlgorithm::getWGap(int W) {
    return W < 8? W : W < 10? W - 1: W < 12 ? W-2: W < 16 ? W-3: W < 20 ? W-4: W < 30 ? 16 : W/2+1;
}

//////////////////////////////////////////////////////////////////////////
//Worker
RFSArrayWSubtask::RFSArrayWSubtask(RFSArrayWAlgorithm* _owner, int _sStart, int _sEnd, int _tid)
: Task(tr("Find repeats subtask (suffix)"), TaskFlag_None), owner(_owner), sStart(_sStart), sEnd(_sEnd), tid(_tid)
{
    tpm = Task::Progress_Manual;
}

void RFSArrayWSubtask::run() {
    owner->run(this);
}

} //namespace
