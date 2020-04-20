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

#include "RF_SuffixArray.h"
#include "RFConstants.h"
#include <QThread>
#include <U2Core/Log.h>

namespace U2 {
/*
void memset(void* _arr, const char _val, const size_t _size){
    char* arr = (char*)_arr;
    for (size_t i=0;i<_size;i++){
        arr[i]=_val;
    }
}
void memmove(void* _src, void* _dst, const size_t _size){
    char* src = (char*)_src;
    char* dst = (char*)_dst;
    if (_src<_dst){
        const char* srcend = src+_size;
        for (;src<srcend;){
            *src++=*dst++;
        }
    }else{
        char* srclast = src+_size-1;
        char* dstlast = dst+_size-1;
        for (;srclast>=src;){
            *srclast--=*dstlast--;
        }
    }
}
*/

#define time ((clock()-startt)*1./CLOCKS_PER_SEC)

using namespace std;

class exc : public exception{
    const char* msg;
public:
    exc(const char* _msg):
    msg(_msg){}

    const char* what() const throw(){
        return msg;
    }
};

SuffixArray::SuffixArray(const char* _sequence, int size, int _prefixLen):
prefixLen(_prefixLen),usablePrefixLen(qMin(_prefixLen,12)),seqSize(size),sequence(_sequence),prefixNum(1<<(2*usablePrefixLen))
{
    startt = clock();

    bitMask = new BitMask(sequence, seqSize);
    bitMask->setPrefixLen(prefixLen);
    perfLog.trace(QString("Construct finished: %1").arg(time));

    prefixes = new quint32[prefixNum];
    U2::memset(prefixes, 0, prefixNum*sizeof(quint32));
    //init prefixes
    const BitMask& mask = *bitMask;
    for (quint32 i=0;i<=seqSize-prefixLen;i++){
        const quint32 ind = ( mask[i]>>(64-usablePrefixLen*2) );
#ifdef _DEBUG
        if (ind>=prefixNum){
            algoLog.error(QString("Error init! charIndex: %1, prefInd: %2, prefixLen: %3").arg(i).arg(ind).arg(prefixLen));
        }
#endif
        prefixes[ ind ]++;
    }
/*    int usedPrefixes = 0;
    for (unsigned i=0;i<prefixNum;i++){
        if (prefixes[i]>0){
            usedPrefixes++;
            if (prefixLen==14) cout << "Prefix "<< i << " x " << prefixes[i] << endl;
        }
    }
    cout << "Used prefixes: " << usedPrefixes  << " of total " << (1<<(2*prefixLen)) << endl;
*/
    perfLog.trace(QString("Init prefixes finished: %1").arg(time));
    quint32 oldV = prefixes[0];
    maxSuffixesPerPrefix = oldV;
    prefixes[0]=0;
    for (unsigned i=1;i<prefixNum;i++){
        quint32 t = prefixes[i];
        if (maxSuffixesPerPrefix<t) maxSuffixesPerPrefix = t;
        prefixes[i] = prefixes[i-1] + oldV;
        oldV = t;
    }
    qSortBuffer = new quint64[maxSuffixesPerPrefix];
//    U2::memset(qSortBuffer, 'q', maxSuffixesPerPrefix*sizeof(quint64));
    perfLog.trace(QString("Set prefixes finished: %1").arg(time));
    sort();
}

void SuffixArray::sort(){
    suffixes = new quint32[seqSize-prefixLen+1];
    //sorting
    for (quint32 i=0;i<=seqSize-prefixLen;i++){
        const quint32 ind = (*bitMask)[i]>>(64-usablePrefixLen*2);
#ifdef _DEBUG
        if (ind>=prefixNum){
            algoLog.error(QString("Error sorting! charIndex: %1, prefInd: %2, prefixLen: %3").arg(i).arg(ind).arg(prefixLen));
        }
#endif
        suffixes[prefixes[ind]++] = i;
    }
/*    for (unsigned i=1;i<prefixNum;i++){
        if (prefixLen!=14) break;
        if (prefixes[i] > prefixes[i-1]){
            cout<<"Prefix "<<i<< " at pos "<<prefixes[i]<< " to suffix pos "<<suffixes[prefixes[i]]<<'\t'<< string(sequence+suffixes[prefixes[i]],14)<< endl;
        }
    }
*/
//    U2::memmove(prefixes+1, prefixes, (prefixNum-1)*sizeof(quint32));
//    prefixes[0] = 0;
    if (prefixLen>usablePrefixLen){
        const quint32 secondSuffixIndex = prefixes[0];
        if (0<secondSuffixIndex){
            sortDeeper(0, secondSuffixIndex);
        }
        const unsigned undefinedPrefix = (prefixNum-1)/3;
        for (unsigned prefix=0; prefix<undefinedPrefix-1; prefix++){
            const quint32 curSuffixIndex = prefixes[prefix];
            const quint32 nextSuffixIndex = prefixes[prefix+1];
            if (curSuffixIndex<nextSuffixIndex){
                sortDeeper(curSuffixIndex, nextSuffixIndex);
            }
        }
        // skip undefined area
        for (unsigned prefix=undefinedPrefix; prefix<prefixNum-1; prefix++){
            const quint32 curSuffixIndex = prefixes[prefix];
            const quint32 nextSuffixIndex = prefixes[prefix+1];
            if (curSuffixIndex<nextSuffixIndex){
                sortDeeper(curSuffixIndex, nextSuffixIndex);
            }
        }
    }
    perfLog.trace(QString("Sort finished: %1").arg(time));
}

void SuffixArray::sortDeeper(const quint32 begin, const quint32 end){
#ifdef _DEBUG
    Q_ASSERT(begin<end && (end-begin)<=maxSuffixesPerPrefix);
#endif
    const BitMask& bits = *bitMask;
    const static quint64 HI_DWORD_MASK = 0xffffffff00000000LL;
    const static quint64 LO_DWORD_MASK = ~HI_DWORD_MASK;
    for (quint32 suffRunner=begin; suffRunner<end; suffRunner++){
        const quint32 suffix = suffixes[suffRunner];
        qSortBuffer[suffRunner-begin] = suffix | ((bits[suffix]<<usablePrefixLen*2)&HI_DWORD_MASK);
    }
    //qsort(begin, end-1);
    qSort(qSortBuffer, qSortBuffer+end-begin);
    for (quint32 suffRunner=begin; suffRunner<end; suffRunner++){
        suffixes[suffRunner] = qSortBuffer[suffRunner-begin]&LO_DWORD_MASK;
    }
}
void SuffixArray::sortUndefinedDeeper(const quint32 begin, const quint32 end){
#ifdef _DEBUG
    Q_ASSERT(begin<end && (end-begin)<=maxSuffixesPerPrefix);
#endif
    const BitMask& bits = *bitMask;
    const static quint64 HI_DWORD_MASK = 0xffffffff00000000LL;
    const static quint64 LO_DWORD_MASK = ~HI_DWORD_MASK;
    for (quint32 suffRunner=begin; suffRunner<end; suffRunner++){
        const quint32 suffix = suffixes[suffRunner];
        qSortBuffer[suffRunner-begin] = suffix | (bits[suffix]&HI_DWORD_MASK);
    }
    //qsort(begin, end-1);
    const quint32 minDetectSize = 100;
    for (quint32 buffRunner=0; buffRunner<end-begin-minDetectSize; buffRunner++){
        quint32 areaOffs=buffRunner;
        quint32 areaSize=minDetectSize;
        if (qSortBuffer[buffRunner+minDetectSize]-qSortBuffer[buffRunner]==minDetectSize){
            while(buffRunner<end-begin-minDetectSize && qSortBuffer[buffRunner+minDetectSize]-qSortBuffer[buffRunner]==minDetectSize){
                areaSize++;
            }
            U2::memmove(qSortBuffer+areaOffs, qSortBuffer+areaOffs+areaSize,areaSize);
            qSort(qSortBuffer, qSortBuffer+end-begin-areaSize);
            U2::memmove(qSortBuffer+areaOffs+areaSize, qSortBuffer+areaOffs,areaSize);
            for(quint32 fillRunner=0;fillRunner<areaSize;fillRunner++){
                //fill with garbage
                (qSortBuffer+areaOffs)[fillRunner] = areaSize-fillRunner;
            }
            break;
        }
    }
    for (quint32 suffRunner=begin; suffRunner<end; suffRunner++){
        suffixes[suffRunner] = qSortBuffer[suffRunner-begin]&LO_DWORD_MASK;
    }
}
void SuffixArray::qsort(const quint32 first, const quint32 last){
    if (first>=last) return;
    if (last-first==1){
        const quint32 t = suffixes[first];
        suffixes[first] = suffixes[last];
        suffixes[last] = t;
        return;
    }
    const quint64* alignedSortBuf = qSortBuffer-first;
    quint32 lrunner = first;
    quint32 rrunner = last;
    const quint64 midv = alignedSortBuf[(first+last)/2];
    do{
        while (alignedSortBuf[lrunner]<midv) ++lrunner;
        while (midv<alignedSortBuf[rrunner]) --rrunner;
        if (lrunner>=rrunner){ //lrunner == rrunner == middle
            break;
        }else{
            const quint32 t = suffixes[lrunner];
            suffixes[lrunner] = suffixes[rrunner];
            suffixes[rrunner] = t;
            ++lrunner;
            --rrunner;
        }
    }while(lrunner<rrunner);
    qsort(first,lrunner-1);
    qsort(rrunner+1,last);
}

//prefixes cannot be equal! either left less than right or right less than left
inline bool    SuffixArray::less(const quint32 li, const quint32 ri){
#ifdef _DEBUG
    Q_ASSERT(li!=ri);
#endif
    const quint64 left    = (*bitMask)[li];
    const quint64 right    = (*bitMask)[ri];
    return (left<right) || (left==right && li<ri);
}
SuffixArray::~SuffixArray(){
    delete bitMask;
    delete [] suffixes;
    delete [] prefixes;
    delete [] qSortBuffer;
}

}//GB namespace
