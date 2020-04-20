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

#include "SArrayIndex.h"
#include "SArrayIndexSerializer.h"

#include <U2Core/Log.h>
#include <U2Core/Timer.h>
#include <U2Core/U2SafePoints.h>

#include <U2Core/DNASequenceObject.h>

#include <QFile>

namespace U2 {


CreateSArrayIndexTask::CreateSArrayIndexTask(const char* _seq, quint32 _size, quint32 _w, char _unknownChar,
                                            const quint32* _bitTable, quint32 _bitCharLen,
                                            quint32 _skipGap, quint32 _gapOffset)
:Task("Create SArray index", TaskFlag_None),
index(NULL), seq(_seq), size(_size), w(_w),
unknownChar(_unknownChar), bitTable(_bitTable), bitCharLen(_bitCharLen),
skipGap(_skipGap), gapOffset(_gapOffset), indexFileName(""), refFileName("")
{
    prebuiltIdx = false;
}

CreateSArrayIndexTask::CreateSArrayIndexTask( const U2SequenceObject* obj, int windowSize, bool useBitMask, bool _prebuiltIdx, const QString &idxFN,const QString &refFN )
    :Task("Create SArray index", TaskFlag_None),
      index(NULL),
      w(windowSize),
      unknownChar('\0'),
      skipGap(0),
      gapOffset(0),
      prebuiltIdx(_prebuiltIdx),
      indexFileName(idxFN),
      refFileName(refFN)
{
    seqArray = obj->getWholeSequenceData(stateInfo);
    CHECK_OP(stateInfo, );
    size = seqArray.length();
    seq = seqArray.constData();
    DNAAlphabetType seqType = obj->getAlphabet()->getType();
    unknownChar = seqType == DNAAlphabet_AMINO ? 'X' : seqType==DNAAlphabet_NUCL ? 'N' : '\0';
    if (useBitMask) {
        bitTable = bt.getBitMaskCharBits(seqType);
        bitCharLen = bt.getBitMaskCharBitsNum(seqType);
    } else {
        bitTable = NULL;
        bitCharLen = 0;
    }
}



CreateSArrayIndexTask::~CreateSArrayIndexTask() {
    if (index!=NULL){
        cleanup();
    }
}

void CreateSArrayIndexTask::run() {
    if (prebuiltIdx) {
        index = new SArrayIndex(seq, bitTable, bitCharLen);
        SArrayIndexSerializer::deserialize(index, indexFileName, stateInfo);
    } else {
        index = new SArrayIndex(seq, size, w, stateInfo, unknownChar, bitTable, bitCharLen, skipGap, gapOffset);
        SArrayIndexSerializer::serialize(index, indexFileName, refFileName);
    }
}

void CreateSArrayIndexTask::cleanup() {
    delete index;
    index = 0;
}

//////////////////////////////////////////////////////////////////////////
// index
SArrayIndex::SArrayIndex(const char *_seqStart, const quint32* _bitTable, int _bitCharLen)
:  w(0), w4(0), wRest(0), skipGap(0), gapOffset(0),
arrLen(0), sArray(NULL), bitMask(NULL), bitFilter(0),
wCharsInMask(0), wAfterBits(0),
bitTable(_bitTable), bitCharLen(_bitCharLen),
seqStart(_seqStart), seqLen(0),
l1Step(0), L1_SIZE(0), l1bitMask(NULL)
{

}


SArrayIndex::SArrayIndex(const char* seq, quint32 seqSize,  quint32 _len, TaskStateInfo& ti,
                         char unknownChar, const quint32* _bitTable,  int _bitCharLen, int _gap, int _gapOffset)
                         : w(_len), w4(_len/4), wRest(_len%4), skipGap(_gap), gapOffset(_gapOffset),
                         bitTable(_bitTable), bitCharLen(_bitCharLen),
                         l1Step(0), L1_SIZE(0), l1bitMask(NULL)
{
    quint64 t1 = GTimer::currentTimeMicros();
    seqLen = seqSize;
    arrLen = seqLen - w + 1;
    if (skipGap > 0) {
        arrLen = (arrLen / skipGap) + 1;
    }
    sArray = new quint32[arrLen];
    if (bitTable!=NULL && bitCharLen>0 && bitCharLen<=5) {
        wCharsInMask = qMin(30 / bitCharLen, w);//30 to avoid +- overflow
        wAfterBits =  qMax(0, w - wCharsInMask);
        if (wCharsInMask * bitCharLen == 32) {
            bitFilter = 0xFFFFFFFF;
        } else {
            bitFilter = (1<<(bitCharLen * wCharsInMask))-1;
        }
    } else {
        bitMask = NULL;
        bitFilter = wAfterBits  = wCharsInMask  = 0;
    }

    quint32* arunner = sArray;
    seqStart = seq;
    const char* seqEnd= seqStart+seqSize - w + 1;
    if (unknownChar == 0) {
        quint32 step = 1 + skipGap;
        for (const char* crunner = seqStart+gapOffset; crunner < seqEnd; arunner++, crunner+=step) {
            *arunner=seq2val(crunner);
        }
    } else { //filter suffixes with unknown char from result
        int oldLen = arrLen;
        const char* crunner = seqStart;
        int lastErrDist = 0;
        for (; crunner < seqEnd && lastErrDist < w - 1; crunner++) {
            if (*crunner != unknownChar) {
                lastErrDist++;
                continue;
            }
            lastErrDist = 0;
        }
        const char* cpos = crunner - w;
        int gapLeft = _gapOffset;
        quint32 w1 = w - 1;
        if (arrLen != 0) {
            while ( ++cpos < seqEnd ) {
                if (*(cpos + w1) != unknownChar) {
                    lastErrDist++;
                    if (lastErrDist >= w && gapLeft-- == 0) {
                        *arunner = seq2val(cpos);
                        arunner++;
                        gapLeft = skipGap;
                    }
                    continue;
                }
                lastErrDist = 0;
                gapLeft = _gapOffset;
            }
        }
        arrLen = arunner - sArray;
        algoLog.trace(QString("filtered len %1, percent %2\n").arg(oldLen - arrLen).arg((arrLen/(float)(oldLen!=0?oldLen:1))));
    }
    // here sArray is initialized with default values and is not sorted
    arrLen = arunner - sArray;

    if (bitTable != NULL) {
        //mask all prefixes in sArray with 32-bit values
        bitMask = new quint32[arrLen];
        quint32 bitValue = 0;
        quint32* arunner = sArray;
        quint32* mrunner = bitMask;

        // Used for optimization - do not recompute whole bit mask if only 1 symbol changes
        // Note 1: at this moment arrays is not sorted and points to sequential regions
        // Note 2: expectedNext is not matched if some region was excluded from sarray
        quint32 expectedNext = 0;

        quint32 wCharsInMask1 = wCharsInMask - 1;
        for (quint32* end = mrunner + arrLen; mrunner < end; arunner++, mrunner++) {
            const char* seq = sarr2seq(arunner);
            if (*arunner == expectedNext && expectedNext != 0) { //pop first bit, push wCharsInMask1 char to the mask
                bitValue = ((bitValue << bitCharLen) | bitTable[uchar(*(seq + wCharsInMask1))]) & bitFilter;
#ifdef _DEBUG
                // double check that optimization doesn't break anything
                quint32 bitValue2 = getBitValue(seq);
                assert(bitValue == bitValue2);
#endif
            } else {
                //recompute the mask if we have some symbols skipped
                bitValue = getBitValue(seq);
            }
            expectedNext = seq2val(seq + 1);
            *mrunner = bitValue;
        }
    }

    if (ti.cancelFlag) {
        return;
    }

    //now sort sArray. Use bit-mask if available
    if (bitMask!=NULL)  {
        sortBit(bitMask, 0, arrLen);
        //sortBitClassic(bitMask, 0, arrLen-1);

        //create L1 cache for bitMask
        if (arrLen < 200*1000) {
            L1_SIZE = arrLen;
            l1Step = 1;
            l1bitMask = bitMask;
        } else {
            L1_SIZE = 8192;
            l1bitMask = new quint32[L1_SIZE];
            l1Step = arrLen / L1_SIZE;
            for (int i=0; i < L1_SIZE; i++) {
                l1bitMask[i] = bitMask[i*l1Step];
            }
            l1bitMask[L1_SIZE-1] = bitMask[arrLen-1];
        }
    } else {
        sort(sArray, 0, arrLen);
    }

    quint64 t2 = GTimer::currentTimeMicros();
    perfLog.details(QString("SArray index creation time: %1").arg(double(t2-t1)/(1000*1000)));

#ifdef _DEBUG
    debugCheck(unknownChar);
#endif
}

SArrayIndex::~SArrayIndex() {
    delete[] sArray;
    if (bitMask!=l1bitMask) {
        delete[] l1bitMask;
    }
    delete bitMask;
}

quint32 SArrayIndex::getBitValue(const char *seq) const {
    quint32 bitValue = 0;
    for (int i = 0; i < wCharsInMask; i++) {
        bitValue = (bitValue << bitCharLen) | bitTable[uchar(*(seq+i))];
    }

    return bitValue;
}

//Stable sort of sequences
void SArrayIndex::sort(quint32* x, int off, int len) {
    // Insertion sort on smallest arrays
    if (len < 7) {
        for (int i=off; i<len+off; i++){
            for (int j=i; j > off && compare(x[j-1],x[j])>0; j--) {
                qSwap(x[j], x[j-1]);
            }
        }
        return;
    }

    // Choose a partition element, v
    quint32 m = off + len / 2;       // Small arrays, middle element
    if (len > 7) {
        quint32 l = off;
        quint32 n = off + len - 1;
        if (len > 40) {        // Big arrays, pseudo median of 9
            quint32 s = len / 8;
            l = med3(x, l,     l+s, l+2*s);
            m = med3(x, m-s,   m,   m+s);
            n = med3(x, n-2*s, n-s, n);
        }
        m = med3(x, l, m, n); // Mid-size, med of 3
    }
    quint32 v = x[m];

    // Establish Invariant: v* (<v)* (>v)* v*
    int a = off, b = off, c = off + len - 1, d = c;
    while(true) {
        int cr;
        while (b <= c && (cr = compare(v, x[b])) >= 0) {
            if (cr == 0) {
                qSwap(x[a],x[b]);
                a++;
            }
            b++;
        }
        while (c >= b && (cr = compare(x[c], v)) >=0 ) {
            if (cr == 0) {
                qSwap(x[c],x[d]);
                d--;
            }
            c--;
        }
        if (b > c) {
            break;
        }
        qSwap(x[b], x[c]);
        b++; c--;
    }

    // Swap partition elements back to middle
    int s, n = off + len;
    s = qMin(a-off, b-a  );  vecswap(x, off, b-s, s);
    s = qMin(d-c,   n-d-1);  vecswap(x, b,   n-s, s);

    // Recursively sort non-partition-elements
    if ((s = b-a) > 1) {
        sort(x, off, s);
    }
    if ((s = d-c) > 1) {
        sort(x, n-s, s);
    }
}

//Stable sort of sequences
void SArrayIndex::sortBit(quint32* x, int off, int len) {
    // Insertion sort on smallest arrays
    if (len < 7) {
        for (int i=off; i<len+off; i++){
            for (int j=i; j > off && compareBit(x+j-1,x+j)>0; j--) {
                swapBit(x+j, x+j-1);
            }
        }
        return;
    }

    // Choose a partition element, v
    quint32 m = off + len / 2;       // Small arrays, middle element
    if (len > 7) {
        quint32 l = off;
        quint32 n = off + len - 1;
        if (len > 40) {        // Big arrays, pseudo median of 9
            quint32 s = len / 8;
            l = med3Bit(x, l,     l+s, l+2*s);
            m = med3Bit(x, m-s,   m,   m+s);
            n = med3Bit(x, n-2*s, n-s, n);
        }
        m = med3Bit(x, l, m, n); // Mid-size, med of 3
    }
    quint32* v = x + m;

    // Establish Invariant: v* (<v)* (>v)* v*
    int a = off, b = a, c = off + len - 1, d = c;
    while(true) {
        int cr;
        while (b <= c && (cr = compareBit(v, x+b)) >=0 ) {
            if (cr == 0) {
                (x+b==v) && (v=x+a);//save middle pos value
                swapBit(x+a++,x+b);
            }
            b++;
        }
        while (c >= b && (cr = compareBit(x+c, v)) >=0 ) {
            if (cr == 0) {
                (x+c==v) && (v=x+d);//save middle pos value
                swapBit(x+c, x+d--);
            }
            c--;
        }
        if (b > c) {
            break;
        }
        swapBit(x+b++, x+c--);
    }

    // Swap partition elements back to middle
    int s, n = off + len;
    s = qMin(a-off, b-a  ); vecswapBit(x+off, x+b-s, s);
    s = qMin(d-c,   n-d-1); vecswapBit(x+b,   x+n-s, s);

    // Recursively sort non-partition-elements
    if ((s = b-a) > 1) {
        sortBit(x, off, s);
    }
    if ((s = d-c) > 1) {
        sortBit(x, n-s, s);
    }
}

int SArrayIndex::partition(quint32* x, int l, int r) {
    quint32* xv = x + l;
    int i = l - 1, j = r + 1;
    while (true) {
        while (compareBit(x + (--j), xv) > 0){}
        while (compareBit(x + (++i), xv) < 0){}
        if (i >= j) {
            return j;
        }
        swapBit(x+i, x+j);
    }
}

void SArrayIndex::sortBitClassic(quint32* x, int l, int r) {
    if (l < r) {
        int q = partition(x, l, r);
        sortBitClassic(x, l, q);
        sortBitClassic(x, q + 1, r);
    }
}

// Compare by char sequences, then by their position in main sequence
int SArrayIndex::compare(const char* seq1, const char* seq2) const {
    //TODO: use memcmp instead?
    int res = qstrncmp(seq1, seq2, w);
    return res; //==0 ? seq1-seq2 : res;

//     const quint32* a1 = (const quint32*)seq1;
//     const quint32* a2 = (const quint32*)seq2;
//     int rc = 0;
//     for (const quint32* aend1 = a1 + w4; a1 < aend1; a1++, a2++) {
//         rc = *a1-*a2;
//         if (rc != 0) {
//             return rc;
//         }
//     }
//     if (wRest > 0) {
//         const char* b1 = (const char*)a1;
//         const char* b2 = (const char*)a2;
//         rc = *b1 - *b2;
//         if (rc!=0) {
//             return rc;
//         }
//         if (wRest > 1) {
//             b1++; b2++;
//             rc = *b1-*b2;
//             if (rc != 0) {
//                 return rc;
//             }
//             return wRest > 2 ? *++b1-*++b2: 0;
//         }
//     }
//     return seq1-seq2;
}

// Compare by char sequences, then by their position in main sequence
int SArrayIndex::compareBit(const quint32* x1, const quint32* x2) const {
    int rc = *x1-*x2;
    if ( rc != 0 || wAfterBits == 0) {
        return rc;
    }
    const char* b1 = sarr2seq(sArray+(x1-bitMask))+wCharsInMask;
    const char* b2 = sarr2seq(sArray+(x2-bitMask))+wCharsInMask;
    for (const char* end = b1+wAfterBits; b1 < end; b1++, b2++) {
        rc=*b1-*b2;
        if ( rc != 0 ) {
            return rc;
        }
    }
    return 0;
}

int SArrayIndex::compareAfterBits(quint32 bitMaskPos, const char* seq) const {
    const char* b1 = sarr2seq(sArray+bitMaskPos)+wCharsInMask;
    const char* b2 = seq;
    int rc = 0;
    for (const char* end = b1+wAfterBits; b1 < end; b1++, b2++) {
        rc=*b1-*b2;
        if (rc != 0) {
            return rc;
        }
    }
    return 0;
}


int SArrayIndex::compareBitByPos(const quint32* x1, const quint32* x2) const {
    int rc = bitMask[x1-sArray]-bitMask[x2-sArray];
    if ( rc != 0 ) {
        return rc;
    }
    const char* b1 = sarr2seq(x1)+wCharsInMask;
    const char* b2 = sarr2seq(x2)+wCharsInMask;
    for (const char* end = b1+wAfterBits; b1<end; b1++, b2++) {
        rc=*b1-*b2;
        if ( rc != 0 ) {
            return rc;
        }
    }
    return 0;
}

bool SArrayIndex::find(SArrayIndex::SAISearchContext* t, const char* seq)  {
    int low = 0;
    int high = arrLen - 1;
    quint32* a = sArray;
    while (low <= high) {
        int mid = (low + high) / 2;
        const char* midSeq = sarr2seq(a + mid);
        int rc = compare(midSeq, seq);
        if (rc < 0) {
            low = mid + 1;
        } else if (rc > 0) {
            high = mid - 1;
        } else { // match!
            t->currSample = seq;
            int i = mid;
            // Find first match position
            while(--i >= 0) {
                 int res = compare(sarr2seq(a+i), seq);
                 if (res != 0) {
                     break;
                 }
            }
            t->currPos = i + 1;
            return true;
        }
    }
    return false;
}

bool SArrayIndex::findBit(SArrayIndex::SAISearchContext* t, quint32 bitValue, const char* seq) {
    int low = 0;
    int high = L1_SIZE - 1;
    quint32* a = l1bitMask;
    while (low <= high) {
        int mid = (low + high) / 2;
        int rc = a[mid] - bitValue;
        if (rc < 0) {
            low = mid + 1;
        } else if (rc > 0) {
            high = mid - 1;
        } else {
            for(low=mid+1; low < arrLen && a[low]==bitValue; low++){};
            for(high=mid-1;high >  0 && a[high]==bitValue; high--){};
            break;
        }
    }
    int newLow = high > 0 ? high * l1Step : 0;
    high = low < L1_SIZE - 1 ? low * l1Step : arrLen - 1;
    low = newLow;
    a = bitMask;
    while (low <= high) {
        int mid = (low + high) >> 1;
        int rc = a[mid] - bitValue;
        if (rc < 0) {
            low = mid + 1;
        } else if (rc > 0) {
            high = mid - 1;
        } else {//found bitMask
            if (wAfterBits == 0) {
                quint32* maskPos = bitMask+mid;
                for (; maskPos > bitMask && compareBit(maskPos, maskPos-1) == 0; maskPos--){};
                t->currPos = maskPos - bitMask;
            } else {
                quint32 midVal = a[mid];
                const char* afterBitsSeq = seq + wCharsInMask;
                bool found = false;
                rc = compareAfterBits(mid, afterBitsSeq);
                if (rc == 0) {
                    found = true;// moving to the start of the search item
                    while (mid > 0 && a[mid-1] == midVal && compareAfterBits(mid-1, afterBitsSeq) == 0) {
                        mid--;
                    }
                } else if (rc > 0) { // search item have lower index
                    for (mid = mid-1; int(mid) >= 0 && a[mid] == midVal; mid--) {
                        if (compareAfterBits(mid, afterBitsSeq) == 0) {
                            found = true;
                            while (mid > 0 && a[mid-1] == midVal && compareAfterBits(mid-1, afterBitsSeq) == 0) {
                                mid--;
                            }
                            break;
                        }
                    }
                } else { //if (rc < 0) { // search item have higher index
                    for (mid = mid+1; mid < arrLen && a[mid] == midVal; mid++) {
                        if (compareAfterBits(mid, afterBitsSeq) == 0) {
                            found = true;
                            break;
                        }
                    }
                }
                if (!found) {
                    return false;
                }
                t->currPos = mid;
            }
            t->bitValue = bitValue;
            t->currSample = seq;
            return true;
        }
    }
    return false;
}

int SArrayIndex::nextArrSeqPos(SArrayIndex::SAISearchContext* t) {
    if (t->currPos==-1) {
        return -1;
    }
    int seqPos = sarr2seq(sArray + t->currPos) - seqStart;
    t->currPos++;
    if (t->currPos == arrLen
        || (bitMask!=NULL && compareBit(bitMask + t->currPos-1, bitMask + t->currPos))
        || (bitMask==NULL && compare(sarr2seq(sArray + t->currPos), t->currSample)))
    {
        t->currPos=-1;
    }
    return seqPos;
}

void SArrayIndex::debugCheck(char unknownChar){
    for (int i=1; i < arrLen; i++) {
        bool fail = bitMask == NULL ? compare(sArray[i-1], sArray[i]) > 0 : bitMask[i-1] > bitMask[i];
        if (fail) {
            algoLog.error("SArray index internal check error 1");
            assert(0);
        }
    }
    if (bitMask!=NULL) {
        quint32* prev = bitMask;
        for (int i=1; i < arrLen; i++) {
            quint32* next = bitMask+i;
            if (compareBit(prev, next) > 0) {
                algoLog.error("SArray index internal check error 2");
                assert(0);
            }
            prev = next;
        }
        quint32 prevMask = 0;
        for (int i=1; i < arrLen; i++) {
            quint32 newMask = bitMask[i];
            if (prevMask == newMask) {
                const char* prevC = sarr2seq(sArray + i - 1);
                const char* newC = sarr2seq(sArray + i);
                if (memcmp(prevC, newC, wCharsInMask)) {
                    algoLog.error("SArray index internal check error 3");
                    assert(0);
                }
            }
            prevMask = newMask;
        }
        for (int i=1; i < L1_SIZE; i++) {
            if (l1bitMask[i-1] > l1bitMask[i]) {
                algoLog.error("SArray index internal check error 4");
            }
        }
    }
    if (unknownChar!=0) {
        for (int i=0; i < arrLen; i++) {
            const char* prefix = sarr2seq(sArray + i);
            for (int j=0; j < w; j++) {
                if (prefix[j] == unknownChar) {
                    algoLog.error("SArray index internal check error 5");
                    assert(0);
                }
            }
        }
    }
}

template<typename T>
inline void writeNum(T n, QFile &file) {
    n = qToBigEndian(n);
    char *num = (char*)&n;
    file.write(num, 4);
}

template<typename T>
inline T readNum(char *num, QFile &file) {
    file.read(num, 4);
    T n = *((T*)num);
    n = qFromBigEndian(n);

    return n;
}
}//namespace
