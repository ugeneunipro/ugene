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

#ifndef _RF_SUFFIX_ARRAY_H_
#define _RF_SUFFIX_ARRAY_H_

#include "RF_BitMask.h"
#include <ctime>

// Attention! Currently constructs suffix array sorted by prefix up to 32 symbols in length

namespace U2 {

class SuffixArray{
    const unsigned prefixLen;
    const unsigned usablePrefixLen;
    const quint32 seqSize;
    const char* sequence;
    const unsigned prefixNum;
    quint32 maxSuffixesPerPrefix;
    quint64* qSortBuffer;

    clock_t startt;
    quint32* suffixes;
    quint32* prefixes;
    BitMask* bitMask;

//    static quint64 getAvailableMemory(){return 128*1024*1024;}

public:
    SuffixArray(const char* _sequence, int size, int _prefixLen);
private:
    void sort();
    void sortDeeper(const quint32 begin, const quint32 end);
    void sortUndefinedDeeper(const quint32 begin, const quint32 end);
    void qsort(const quint32 first, const quint32 last);
    inline bool less(const quint32 li, const quint32 ri);
public:
    const BitMask& getBitMask()const {return *bitMask;}
//    inline quint32& SuffixArray::operator[](const quint32 index)            {return suffixes[index];}
//    inline const quint32& SuffixArray::operator[](const quint32 index)const    {return suffixes[index];}
    inline quint32* getArray()                {return suffixes;}

    ~SuffixArray();
};

} //U2

#endif
