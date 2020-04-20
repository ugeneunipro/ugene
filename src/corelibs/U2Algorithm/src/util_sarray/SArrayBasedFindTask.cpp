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
#include "SArrayBasedFindTask.h"

namespace U2 {

#define PCHAR_MATCH(x, y) (*(x) == *(y) && *(x) != unknownChar)


SArrayBasedFindTask::SArrayBasedFindTask(  SArrayIndex* i, const SArrayBasedSearchSettings& s, bool _onlyFirstMatch )
: Task("SArrayBasedFindTask", TaskFlag_None), index(i), config(new SArrayBasedSearchSettings(s)),
onlyFirstMatch(_onlyFirstMatch)
{
    assert(index);
}

void SArrayBasedFindTask::run()
{
     runSearchWithMismatches();
}

void SArrayBasedFindTask::runSearch()
{
    bool haveResults = true;
    SArrayIndex::SAISearchContext context;
    const char* querySeq = config->query.constData();
    if (config->useBitMask) {
        const quint32* bm = config->bitMask;
        int charBitsNum = config->bitMaskCharBitsNum;
        quint32 bitValue = 0;
        int wCharsInMask = index->getCharsInMask();
        const char* posS = querySeq;
        char unknownChar = config->unknownChar;
        for (int cleanChars = 0; cleanChars < wCharsInMask; posS++) {
            if (*posS == unknownChar) {
                cleanChars = 0;
                bitValue = 0;
            } else {
                bitValue = (bitValue << charBitsNum) | bm[uchar(*posS)];
                cleanChars++;
            }
        }
        haveResults = index->findBit(&context,bitValue,querySeq);
    } else {
        haveResults = index->find(&context,querySeq);
    }

    if (haveResults) {
        int pos = -1;
        while( ( pos = index->nextArrSeqPos(&context) ) != -1 ) {
            results.append(pos + 1);
        }
    }

}

void SArrayBasedFindTask::runSearchWithMismatches()
{
    const char* querySeq = config->query.constData();
    const char* arraySeq = index->getIndexedSequence();
    char unknownChar = config->unknownChar;
    int CMAX = 0;
    if (config->absMismatches) {
        CMAX = config->nMismatches;
    } else {
        CMAX = (config->query.length() * config->ptMismatches) / MAX_PERCENTAGE;
    }
    int W = config->query.size();
    int q = W / (CMAX + 1);
    int prefixSize = index->getPrefixSize();

    assert(prefixSize <=  q);
    if (prefixSize > q) {
        setError( QString("Too large SArrayIndex window (%1) for %2-mismatch search").arg(prefixSize).arg(CMAX) );
        return;
    }

    for (int i = 0; i < W - prefixSize + 1; ++i) {
        const char* seq = querySeq + i;
        SArrayIndex::SAISearchContext context;
        bool haveResults = false;
        if (config->useBitMask) {
            const quint32* bm = config->bitMask;
            int charBitsNum = config->bitMaskCharBitsNum;
            quint32 bitValue = 0;
            int wCharsInMask = index->getCharsInMask();
            const char* posS = querySeq;
            char unknownChar = config->unknownChar;
            for (int cleanChars = 0; cleanChars < wCharsInMask; posS++) {
                if (*posS == unknownChar) {
                    cleanChars = 0;
                    bitValue = 0;
                } else {
                    bitValue = (bitValue << charBitsNum) | bm[uchar(*posS)];
                    cleanChars++;
                }
            }
            haveResults = index->findBit(&context,bitValue,seq);
        } else {
            haveResults = index->find(&context,seq);
        }
        if (!haveResults) {
            continue;
        }
        int pos = -1;
        const char* endS = querySeq + W;
        const char* endA = arraySeq + index->getSequenceLength();
        while( ( pos = index->nextArrSeqPos(&context) ) != -1 ) {
            int c = 0;
            // forward collect
            const char* posS = seq + prefixSize;
            const char* posA = arraySeq + pos + prefixSize;
            for ( ; (posS < endS) && (c <= CMAX); posS++, posA++) {
                if (posA >= endA) {
                    // out of arrraySeq boundaries - > do not need to continue
                    c = CMAX + 1;
                    break;
                }
                c += PCHAR_MATCH(posS, posA) ? 0 : 1;
            }

            // backward collect
            posS = seq - 1;
            posA = arraySeq + pos - 1;

            for ( ; (posS >= querySeq) && (c <= CMAX); posS--, posA--) {
                if (posA < arraySeq) {
                    // out of arrraySeq boundaries - > do not need to continue
                    c = CMAX + 1;
                    break;
                }
                c += PCHAR_MATCH(posS, posA) ? 0 : 1;
            }
            int result = pos - i + 1;
            if ( (c <= CMAX) && (!results.contains(result))) {
                results.append(result);
                if (onlyFirstMatch) {
                    break;
                }
            }
        }
    }
}

void SArrayBasedFindTask::cleanup()
{
    delete config;
    config = NULL;
}



} // U2
