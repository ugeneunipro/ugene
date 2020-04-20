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

#include <cstdlib>
#include <cstring>
#include <QString>
#include <U2Core/Log.h>

// Attention! Works correctly only on little-endian systems!

namespace U2{
inline void memset(void* _arr, const char _val, const size_t _size){
    char* arr = (char*)_arr;
    for (size_t i=0;i<_size;i++){
        arr[i]=_val;
    }
}
inline void memmove(void* _src, void* _dst, const size_t _size){
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
}



static const quint64 c01 = 0x0101010101010101LL;
static const quint64 c1f = 0x1f1f1f1f1f1f1f1fLL;
static const quint64 c03 = 0x0303030303030303LL;

#define word1(a) w1(decode(a))
#define word2(a) w2(decode(a))
#define word3(a) w3(decode(a))
#define word4(a) w4(decode(a))

class BitMask{
    BitMask():origSequence(NULL),origSequenceSize(0){}
    const char* origSequence;
    const quint32 origSequenceSize;
    quint64* bitSequence;
    quint32 bitSequenceSize;
    const static int SYMB_PER_INTEGER = sizeof(quint64)*8/2; //32 symbols per one request

    int maskSize;
    quint64 mask;

    quint32 currentPosition;
    int currBitPosition;
    quint64 currentPrefix;
    quint64 nextPrefix;

public:
    BitMask(const char* sequence, const quint32 size):origSequence(sequence),origSequenceSize(size){
        bitSequenceSize = origSequenceSize / SYMB_PER_INTEGER + 2;
        bitSequence = new quint64[bitSequenceSize];

        quint64* bitSeqRunner = bitSequence;
        quint64* charSeqRunner = (quint64*)origSequence;
        //number of bytes multiple to 32
        const quint64* charSeqRunnerStop = (quint64*)(origSequence + (origSequenceSize&(~0x1f)));
        //construct bitSequence
        while(charSeqRunner<charSeqRunnerStop){
            quint64 bitSeqBit = 0;
            bitSeqBit |= word4(*charSeqRunner++);
            bitSeqBit |= word3(*charSeqRunner++);
            bitSeqBit |= word2(*charSeqRunner++);
            bitSeqBit |= word1(*charSeqRunner++);
            *bitSeqRunner = bitSeqBit;
            bitSeqRunner++;
        }
        //construct rest of bitMask
        char restStr[32];
        U2::memset(restStr, 'A', 32);    //TODO: what is the best fill for tail?
        U2::memmove(restStr, charSeqRunner, origSequenceSize&0x1f);
        charSeqRunner = (quint64*)restStr;
        *bitSeqRunner = word4(*charSeqRunner++);
        *bitSeqRunner |= word3(*charSeqRunner++);
        *bitSeqRunner |= word2(*charSeqRunner++);
        *bitSeqRunner |= word1(*charSeqRunner++);

//        setPrefixLen(16);    //default prefix length
//        currentPrefix = (*this)[0];
//        nextPrefix = (*this)[1];
        currentPosition=0;
        currBitPosition=32;

        //check bitSequence
#ifdef _DEBUG
/*
        const char* decAlph="AGTC";
        const BitMask& bits = *this;
        for (quint32 i=0; i<origSequenceSize-maskSize; i++){
            if(origSequence[i] != decAlph[(bits[i]>>62)&3]){
                char numstr[10];
                char c = decAlph[(bits[i]>>62)&3];
                //throw std::exception((_itoa(i,numstr,10),numstr));
            }
        }
*/
#endif
    }
    ~BitMask(){
        delete[] bitSequence;
        bitSequence=NULL;
    }

    void setPrefixLen(const int prefLen){
        Q_ASSERT(prefLen<=SYMB_PER_INTEGER);
        maskSize = 2*prefLen;//number of bits in prefix
        mask = ~((quint64)~0>>maskSize);
    }
    inline quint64 operator[](const quint64 index)const{
        const quint64* localCell = bitSequence + index/SYMB_PER_INTEGER;
        const int bitPosition = index%SYMB_PER_INTEGER*2;
        const int& q0 = bitPosition;
        //const int q1 = (q0+maskSize)%(SYMB_PER_INTEGER*2);
        return q0==0 ? (mask & localCell[0]) : mask & ( (localCell[0]<<q0) | (localCell[1]>>(64-q0)) );
    }
    inline quint64 next(){
        // not implemented
        Q_ASSERT(0);
        quint64 res = currentPrefix & mask;
        currentPrefix = (currentPrefix<<2) | (nextPrefix>>62);
        nextPrefix <<= 2;
        --currBitPosition;
        if (currBitPosition==0){
            currBitPosition = 32;
            currentPosition++;
            currentPrefix = bitSequence[currentPosition];
            nextPrefix = bitSequence[currentPosition+1];
        }
        return res;
    }
    inline quint64 next()const{
        return const_cast<BitMask*>(this)->next();
    }

    inline quint64 w1(const quint64& v) const {
        return ((v>>16)|(v>>56))&0x0000ffffLL;
    }
    inline quint64 w2(const quint64& v) const {
        return ((v>>0)|(v>>40))&0xffff0000;
    }
    inline quint64 w3(const quint64& v) const {
        return ((v<<16)|(v>>24))&0x0000ffff00000000LL;
    }
    inline quint64 w4(const quint64& v) const {
        return ((v<<32)|(v>>8))&0xffff000000000000LL;
    }
    inline quint64 decode(const quint64& str){
        //decode symbols
        const quint64 r = (str & c1f) - c01;
        const quint64 s = r+(r<<1);
        const quint64 t = s^((s&c01)<<2);
        const quint64 u = (t>>1)&c03;
        //compress
        const quint64 v =  u | (u<<10);
        return (v | (v<<20)) & 0xff000000ff000000LL;
    }
};
