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

#include "DNATranslationImpl.h"

#include <U2Core/TextUtils.h>
#include <U2Core/DNAAlphabet.h>
#include <QDateTime>

namespace U2 {


DNATranslation1to1Impl::DNATranslation1to1Impl(const QString& id, const QString& _name, const DNAAlphabet* src, const DNAAlphabet* dst, QByteArray mapper121)
: DNATranslation(id, _name, src, dst)
{
    map = mapper121;
    assert(map.size() == 256);
}

qint64 DNATranslation1to1Impl::translate(const char* src, qint64 src_len, char* dst, qint64 dst_capacity) const {
    qint64 len = qMin(src_len, dst_capacity);
    TextUtils::translate(map, src, len, dst);
    return len;
}

int DNATranslation1to1Impl::translate(char* src_and_dst, int len) const {
    TextUtils::translate(map, src_and_dst, len);
    return len;
}

//////////////////////////////////////////////////////////////////////////
/// 3->1

DNATranslation3to1Impl::DNATranslation3to1Impl(const QString& _id, const QString& _name,
                                               const DNAAlphabet* src, const DNAAlphabet* dst,
                                               const QList<Mapping3To1<char> >& rm, char dc,
                                               const QMap<DNATranslationRole,QList<Triplet> >& _roles)
:  DNATranslation(_id, _name, src, dst), index(rm, dc), roles(_roles)
{
    codons = new char*[DNATranslationRole_Num];
    cod_lens = new int[DNATranslationRole_Num];
    for (int j=0;j<DNATranslationRole_Num; j++ )
    {
        cod_lens[j] = 0;
    }

    QMapIterator<DNATranslationRole,QList<Triplet> > i(roles);
    while (i.hasNext()) {
        i.next();
        QList<Triplet> l = i.value();
        DNATranslationRole r = i.key();
        cod_lens[r] = l.size()*3;
        codons[r] = new char[l.size()*3];
        for (int j = 0; j<l.size(); j++)
        {
            char* codon = codons[r] + j*3;
            *codon++ = l.at(j).c[0];
            *codon++ = l.at(j).c[1];
            *codon++ = l.at(j).c[2];
        }
    }
}

DNATranslation3to1Impl::~DNATranslation3to1Impl() {
    for (int j=0;j<DNATranslationRole_Num; j++ ) {
        if (cod_lens[j] != 0) {
            delete[] codons[j];
        }
    }
    delete[] codons;
    delete[] cod_lens;
}

qint64 DNATranslation3to1Impl::translate(const char* src, qint64 src_len, char* dst, qint64 dst_capacity) const {
    bool caseSensitive = srcAlphabet->isCaseSensitive();
    qint64 resLen = qMin(src_len/3, dst_capacity);
    if (caseSensitive) {
        for(int dstIdx=0, srcIdx=0; dstIdx < resLen; dstIdx++, srcIdx+=3) {
            char c = index.map(src + srcIdx);
            dst[dstIdx] = c;
        }
    } else {
        char uc[3];
        const QByteArray& ucMap = TextUtils::UPPER_CASE_MAP;
        for(int dstIdx=0, srcIdx=0; dstIdx < resLen; dstIdx++, srcIdx+=3) {
            TextUtils::translate(ucMap, src+srcIdx, 3, uc);
            char c = index.map(src + srcIdx);
            dst[dstIdx] = c;
        }
    }
    return resLen;
}

int DNATranslation3to1Impl::translate(char* src_and_dst, int len) const {
    return translate(src_and_dst, len, src_and_dst, len);
}

//////////////////////////////////////////////////////////////////////////
// 3-1 index

Index3To1::Index3To1() {
    indexSize = 0;
    bitsPerChar = 0;
    bitsPerCharX2 = 0;
    qFill(maskByChar, maskByChar+256, 0);
}

void Index3To1::init(const QList<Triplet>& ts) {
    assert(indexSize == 0);
    // count number of different input chars in map
    QBitArray usedChars(256, false);
    foreach(const Triplet& t, ts) {
        usedChars[(quint8)t.c[0]] = true;
        usedChars[(quint8)t.c[1]] = true;
        usedChars[(quint8)t.c[2]] = true;
    }
    int nChars = usedChars.count(true);
    assert(nChars > 0);

    // count number of bit enough to encode all input chars
    bitsPerChar = 0;
    while ((1 << bitsPerChar) <= nChars) bitsPerChar++;
#ifdef _DEBUG
    int prevPow = 1 << (bitsPerChar-1);
    int nextPow = 1 << bitsPerChar;
    assert(nChars < nextPow && nChars >= prevPow);
    assert(bitsPerChar<=8); //validity check
    assert(bitsPerChar<=6);//TODO: optimize large tables!!
#endif
    bitsPerCharX2 = bitsPerChar * 2; //cache this value -> used for optimization

    // assign bit mask for every char
    char mask = 1;
    for (int i = 0; i < 256; i++) {
        if (usedChars[i]) {
            maskByChar[i] = mask;
            mask++;
        } else {
            maskByChar[i] = 0;
        }
    }
    mask--; //last value was not used

    indexSize = 1 + (mask << bitsPerCharX2) + (mask<<bitsPerChar) + mask;

    assert(indexSize > 0);
}

void Index3To1::init(const QByteArray& alphabetChars) {
    int nChars = alphabetChars.size();
    QList<Triplet> ts;
    for (int i1=0; i1 < nChars; i1++) {
        char c1 = alphabetChars[i1];
        for (int i2=0; i2 < nChars; i2++) {
            char c2 = alphabetChars[i1];
            for (int i3=0; i3 < nChars; i3++) {
                char c3 = alphabetChars[i1];
                Triplet m(c1, c2, c3);
                ts.append(m);
            }
        }
    }
    init(ts);
}

//////////////////////////////////////////////////////////////////////////
/// 1->3
DNATranslation1to3Impl::DNATranslation1to3Impl(const QString& _id, const QString& _name,
                                               const DNAAlphabet* src, const DNAAlphabet* dst,
                                               const BackTranslationRules& rules)
:  DNATranslation(_id, _name, src, dst), index(rules)
{
    qsrand(QDateTime::currentDateTime().toTime_t());
}

DNATranslation1to3Impl::~DNATranslation1to3Impl() {
}

qint64 DNATranslation1to3Impl::translate (const char* src, qint64 src_len, char* dst, qint64 dst_capacity, BackTranslationMode mode) const {
    bool caseSensitive = srcAlphabet->isCaseSensitive();
    qint64 resLen = qMin(src_len * 3, dst_capacity);
    switch (mode) {
        case USE_MOST_PROBABLE_CODONS :
        {
            if (caseSensitive) {
                for(int dstIdx=0, srcIdx=0; dstIdx < resLen; srcIdx++) {
                    TripletP curr = index.map[index.index[(int)*(src + srcIdx)]];
                    dst[dstIdx++] = curr.c[0];
                    dst[dstIdx++] = curr.c[1];
                    dst[dstIdx++] = curr.c[2];
                }
            } else {
                char uc;
                const QByteArray& ucMap = TextUtils::UPPER_CASE_MAP;
                for(int dstIdx=0, srcIdx=0; dstIdx < resLen; srcIdx++) {
                    TextUtils::translate(ucMap, src+srcIdx, 1, &uc);
                    TripletP curr = index.map[index.index[(int)*(src + srcIdx)]];
                    dst[dstIdx++] = curr.c[0];
                    dst[dstIdx++] = curr.c[1];
                    dst[dstIdx++] = curr.c[2];
                }
            }
            break;
        }
        case USE_FREQUENCE_DISTRIBUTION :
        {
            if (caseSensitive) {
                for(int dstIdx=0, srcIdx=0; dstIdx < resLen; srcIdx++) {
                    int p = qrand() % 100;
                    int pos = index.index[(int)*(src + srcIdx)];
                    while (index.map[pos].p <= p) {
                        p -= index.map[pos++].p;
                    }
                    TripletP curr = index.map[pos];
                    dst[dstIdx++] = curr.c[0];
                    dst[dstIdx++] = curr.c[1];
                    dst[dstIdx++] = curr.c[2];
                }
            } else {
                char uc;
                const QByteArray& ucMap = TextUtils::UPPER_CASE_MAP;
                for(int dstIdx=0, srcIdx=0; dstIdx < resLen; srcIdx++) {
                    TextUtils::translate(ucMap, src+srcIdx, 1, &uc);
                    int p = qrand() % 100;
                    int pos = index.index[(int)*(src + srcIdx)];
                    while (index.map[pos].p <= p) {
                        p -= index.map[pos++].p;
                    }
                    TripletP curr = index.map[pos];
                    dst[dstIdx++] = curr.c[0];
                    dst[dstIdx++] = curr.c[1];
                    dst[dstIdx++] = curr.c[2];
                }
            }
            break;
        }
        default: break;
    }
    return resLen;
}

qint64 DNATranslation1to3Impl::translate(const char* src, qint64 src_len, char* dst, qint64 dst_capacity) const {
    return translate(src, src_len, dst, dst_capacity, USE_MOST_PROBABLE_CODONS);
}

int DNATranslation1to3Impl::translate(char* src_and_dst, int len) const {
    return translate(src_and_dst, len, src_and_dst, len, USE_MOST_PROBABLE_CODONS);
}

}//namespace

