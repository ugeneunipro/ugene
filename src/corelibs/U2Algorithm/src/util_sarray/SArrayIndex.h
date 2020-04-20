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

#ifndef _U2_SARRAY_INDEX_H_
#define _U2_SARRAY_INDEX_H_

#include <U2Core/Task.h>
#include <U2Algorithm/BitsTable.h>

#include <QtGlobal>

namespace U2 {

class SArrayIndex;
class U2SequenceObject;
class BitsTable;

/// A task to create SArrayIndex
class U2ALGORITHM_EXPORT CreateSArrayIndexTask : public Task {
    Q_OBJECT
public:
    CreateSArrayIndexTask(const U2SequenceObject* obj, int windowSize, bool useBitMask = false,
                        bool prebuiltIdx = false, const QString &fileName = "", const QString &refFileName = "");
    CreateSArrayIndexTask(const char* seq, quint32 size,  quint32 w, char unknownChar=0,
                        const quint32* bitTable = NULL, quint32 bitCharLen = 0,
                        quint32 skipGap = 0, quint32 _gapOffset=0);
    ~CreateSArrayIndexTask();

    void run();
    void cleanup();

    SArrayIndex*    index;

    inline quint32 getPrefixSize()const{ return w;}
    char getUnknownChar() const {return unknownChar; }
    const quint32* getBitTable() const {return bitTable; }

private:
    QByteArray      seqArray;
    const char*     seq;
    quint32         size;
    quint32         w;
    char            unknownChar;
    const quint32*  bitTable;
    quint32         bitCharLen;
    quint32         skipGap;
    quint32         gapOffset;
    bool            prebuiltIdx;
    QString         indexFileName;
    QString         refFileName;
    BitsTable       bt;
};

/// Main SArrayIndex structure
class U2ALGORITHM_EXPORT SArrayIndex {

    friend class SArrayIndexSerializer;
public:
    class SAISearchContext {
    public:
        SAISearchContext() : currSample(NULL), currPos(-1), bitValue(0xFFFFFFFF) {};
        virtual ~SAISearchContext(){}
        const char* currSample;
        int         currPos;
        quint32     bitValue;
    };

    //qlt - quick lookup table, size = 0 disable it's usage
    SArrayIndex(const char *serStart, const quint32* bitTable, int bitCharLen);
    SArrayIndex(const char* seq, quint32 size,  quint32 w, TaskStateInfo& ti,
        char unknownChar=0, const quint32* bitTable = NULL, int bitCharLen = 0, int skipGap = 0, int gapOffset=0);

    virtual ~SArrayIndex();

    quint32 getBitValue(const char *seq) const;
    bool find(SAISearchContext* c, const char* seq);
    bool findBit(SAISearchContext* c, quint32 bitValue, const char* seq);
    int  nextArrSeqPos(SAISearchContext* c); //-1 -> end

    /** compares subsequence with offsets x1 and x2, returns -1;0;1 */
    inline int compare(quint32 x1, quint32 x2) const ;
    int compare(const char* seq1,  const char* seq2) const;

    /** x1 and x2 is bitMask*/
    int compareBit(const quint32* x1, const quint32* x2) const;
    /** x1 and x2 is sArray*/
    int compareBitByPos(const quint32* x1, const quint32* x2) const;
    /**compares only after bits chars*/
    int compareAfterBits(quint32 bitMaskPos, const char* afterBitsSeq) const;

    inline quint32 seq2val(const char* seq) const;
    inline const char* val2seq(quint32 offs) const;         // value of sarray element -> seq
    inline const char* sarr2seq(const quint32* sarr) const; // pointer to sarray element -> seq
    inline const char* idx2seq(quint32 idx) const;  // index in sarray element -> seq

    int         getCharsInMask() const {return wCharsInMask;}
    quint32     getBitFilter() const {return bitFilter;}

    const quint32* getMaskedSequence() const {return bitMask;}
    const quint32* getSArray() const {return sArray;}

    quint32 getSArraySize() const {return arrLen;}
    int getPrefixSize() const { return w; }
    int getSequenceLength() const { return seqLen; }
    const char* getIndexedSequence() const {return seqStart; }

private:
    int             w, w4, wRest, skipGap, gapOffset;

    int             arrLen;     // size of the array == sequence len with unknown chars removed
    quint32*        sArray;     // sorted offsets (or direct addr) to the sequence
    quint32*        bitMask;    // for all suffixes in sArray this mask contains bit-masked prefix (32-bit)
    quint32         bitFilter;
    int             wCharsInMask;
    int             wAfterBits;
    const quint32   *bitTable;
    int             bitCharLen;


    const char*     seqStart;
    int             seqLen;
    int             l1Step;
    int             L1_SIZE;

    quint32*        l1bitMask; // compressed bitMask. Used to localize range before accessing to the real bitMask

    void sort(quint32* x, int off, int len);
    void sortBit(quint32* x, int off, int len);

    void sortBitClassic(quint32* x, int off, int len);
    int  partition(quint32* x, int p, int r);

    //swaps bit mask values. Swap corresponding sArray values too
    inline void swapBit(quint32* x1, quint32* x2) const;

    /** Swaps x[a .. (a+n-1)] with x[b .. (b+n-1)]. */
    inline void vecswap(quint32* x, quint32 a, quint32 b, quint32 n);
    inline int vecswapBit(quint32* x1, quint32* x2,  quint32 n);

    /** Returns index of the median of the three indexed x[] values.*/
    inline quint32 med3(quint32* x, quint32 a, quint32 b, quint32 c);
    inline quint32 med3Bit(quint32* x, quint32 a, quint32 b, quint32 c);

    void debugCheck(char c);
};


void SArrayIndex::swapBit(quint32* x1, quint32* x2) const {
    //swaps bit mask values. Swap corresponding sArray values too
    assert(x1 - bitMask >= 0 && x1 - bitMask < arrLen);
    assert(x2 - bitMask >= 0 && x2 - bitMask < arrLen);

    quint32* a1 = sArray+(x1-bitMask);
    quint32* a2 = sArray+(x2-bitMask);
    qSwap(*x1, *x2);
    qSwap(*a1, *a2);
}


/** Swaps x[a .. (a+n-1)] with x[b .. (b+n-1)]. */
void SArrayIndex::vecswap(quint32* x, quint32 a, quint32 b, quint32 n) {
    for (quint32 i=0; i<n; i++, a++, b++) {
        qSwap(x[a], x[b]);
    }
}

int SArrayIndex::vecswapBit(quint32* x1, quint32* x2, quint32 n) {
    for (quint32 i=0; i<n; i++) {
        swapBit(x1+i, x2+i);
    }
    return 0;
}

/** Returns index of the median of the three indexed x[] values.*/
quint32 SArrayIndex::med3(quint32* x, quint32 a, quint32 b, quint32 c) {
    int bc = compare(x[b], x[c]);
    int ac = compare(x[a], x[c]);
    return compare(x[a], x[b]) < 0 ?
        (bc < 0 ? b : ac < 0 ? c : a) :
        (bc > 0 ? b : ac > 0 ? c : a);
}


quint32 SArrayIndex::med3Bit(quint32* x, quint32 a, quint32 b, quint32 c) {
    int bc = compareBit(x+b, x+c);
    int ac = compareBit(x+a, x+c);
    return compareBit(x+a, x+b) < 0 ?
        (bc < 0 ? b : ac < 0 ? c : a) :
        (bc > 0 ? b : ac > 0 ? c : a);
}

quint32 SArrayIndex::seq2val(const char* seq) const {
//#ifndef _LP64 //assuming 32bit platform -> address is used as value
//    //TODO: check if there is any benefit in storing direct addresses
//    return (quint32)seq;
//#else
    assert(seq >=seqStart && seq<=seqStart+seqLen);
    return seq - seqStart; // on 64-bit platform index stores sequence offsets
//#endif

}

const char*  SArrayIndex::val2seq(quint32 val) const {
//#ifndef _LP64 //assuming 32bit platform -> index stores direct addresses
//    //TODO: check if there is any benefit in storing direct addresses
//    const char* res = (const char*)val;
//    assert(res >= seqStart && res < seqStart + seqLen);
//    return res;
//#else
    assert(val < quint32(seqLen)); // on 64-bit platform index stores sequence offsets
    return seqStart + val;
//#endif
}

const char* SArrayIndex::sarr2seq(const quint32* sarr) const {
    assert(sarr >= sArray && sarr < sArray + arrLen);
    return val2seq(*sarr);
}

const char* SArrayIndex::idx2seq(quint32 idx) const {
    return sarr2seq(sArray + idx);
}

int SArrayIndex::compare(quint32 x1, quint32 x2) const {
    return compare(val2seq(x1), val2seq(x2));
}

} //namespace

#endif
