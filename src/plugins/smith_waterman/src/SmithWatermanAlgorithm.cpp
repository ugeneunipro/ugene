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

#include <stdio.h>
#include "SmithWatermanAlgorithm.h"
#include <U2Core/U2Region.h>

namespace U2 {

const char SmithWatermanAlgorithm::STOP = 's';
const char SmithWatermanAlgorithm::UP = 'u';
const char SmithWatermanAlgorithm::LEFT = 'l';
const char SmithWatermanAlgorithm::DIAG = 'd';

SmithWatermanAlgorithm::SmithWatermanAlgorithm() {
    gapOpen = 0;
    gapExtension = 0;
    minScore = 0;
    matrixLength = 0;
}

quint64 SmithWatermanAlgorithm::estimateNeededRamAmount(const qint32 gapOpen,
    const qint32 gapExtension, const quint32 minScore, const quint32 maxScore,
    const QByteArray & patternSeq, const QByteArray & searchSeq,
    const SmithWatermanSettings::SWResultView resultView)
{
    const double b_to_mb_factor = 1048576.0;

    const quint64 queryLength = patternSeq.length();
    const quint64 searchLength = searchSeq.length();

    quint64 memToAllocInBytes = 0;
    if(SmithWatermanSettings::MULTIPLE_ALIGNMENT == resultView) {
        const qint32 maxGapPenalty = (gapOpen > gapExtension) ? gapOpen : gapExtension;
        assert(0 > maxGapPenalty);
        quint64 matrixLength = queryLength - (maxScore - minScore) / maxGapPenalty + 1;
        if (searchLength + 1 < matrixLength) {
            matrixLength = searchLength + 1;
        }
        memToAllocInBytes = queryLength * (2 * sizeof(int) + 0x80) + matrixLength
            * ((4 + queryLength + 3) >> 2);
    } else if(SmithWatermanSettings::ANNOTATIONS == resultView) {
        memToAllocInBytes = queryLength * (3 * sizeof(int) + 0x80);
    } else {
        assert(false);
    }

    return memToAllocInBytes / b_to_mb_factor;
}

bool SmithWatermanAlgorithm::calculateMatrixLength() {
    int maxScore = 0;

    QByteArray alphaChars = substitutionMatrix.getAlphabet()->getAlphabetChars();
    int nCharsInAlphabet = alphaChars.size();
    for (int i = 0; i < patternSeq.length(); i++) {
        int max = 0;
        for (int j = 0; j < nCharsInAlphabet; j++) {
            //TODO: cache pattern seq raw pointer and alphaChars row pointer out of the loop
            int substValue = substitutionMatrix.getScore(patternSeq.at(i), alphaChars.at(j));
            max = qMax(max, substValue);
        }
        maxScore += max;
    }

    if (minScore > maxScore) return 0;
    int gap = gapOpen > gapExtension ? gapOpen : gapExtension;
    matrixLength = patternSeq.length() + (maxScore - minScore)/gap * (-1) + 1;
    if(searchSeq.length() + 1 < matrixLength) matrixLength = searchSeq.length() + 1;
    return 1;
}

void SmithWatermanAlgorithm::setValues(const SMatrix& _substitutionMatrix,
        const QByteArray & _patternSeq, const QByteArray & _searchSeq,
        int _gapOpen, int _gapExtension, int _minScore, SmithWatermanSettings::SWResultView _resultView)
{
    substitutionMatrix = _substitutionMatrix;
    patternSeq = _patternSeq;
    searchSeq = _searchSeq;
    gapOpen = _gapOpen;
    gapExtension = _gapExtension;
    minScore = _minScore;
    resultView = _resultView;
}

void SmithWatermanAlgorithm::launch(const SMatrix& _substitutionMatrix,
        const QByteArray & _patternSeq, const QByteArray & _searchSeq,
        int _gapOpen, int _gapExtension, int _minScore, SmithWatermanSettings::SWResultView _resultView)
{
    setValues(_substitutionMatrix, _patternSeq, _searchSeq, _gapOpen, _gapExtension, _minScore,
        _resultView);
    if(isValidParams() && calculateMatrixLength()) {
        switch(resultView) {
        case SmithWatermanSettings::MULTIPLE_ALIGNMENT:
            calculateMatrixForMultipleAlignmentResult();
            break;
        case SmithWatermanSettings::ANNOTATIONS:
            calculateMatrixForAnnotationsResult();
            break;
        default:
            assert(false);
        }
    }
}

bool SmithWatermanAlgorithm::isValidParams() {
    if(searchSeq.length() <= 0 || patternSeq.length() <= 0) return false;
    if(searchSeq.length() < patternSeq.length()) return false;
    if(gapOpen >= 0 || gapExtension >= 0) return false;
    return true;
}

//Get results
//countResults - count of results will be return
QList<PairAlignSequences> SmithWatermanAlgorithm::getResults() {
    return pairAlignmentStrings;
}

void SmithWatermanAlgorithm::sortByScore( QList<PairAlignSequences> & res) {
    QList<PairAlignSequences> buf;
    QVector<int> pos;
    QVector<KeyOfPairAlignSeq> sortedScores;

    for (int i = 0; i < res.size(); i++)
        for (int j = i + 1; j < res.size(); j++) {

            if (res.at(i).score < res.at(j).score) {
                KeyOfPairAlignSeq::exchange(res[i],res[j]);
            }
            else if (res.at(i).score == res.at(j).score
                && res.at(i).refSubseqInterval.startPos > res.at(j).refSubseqInterval.startPos) {
                    KeyOfPairAlignSeq::exchange(res[i], res[j]);
            }
            else if (res.at(i).score == res.at(j).score
                && res.at(i).refSubseqInterval.startPos == res.at(j).refSubseqInterval.startPos
                && res.at(i).refSubseqInterval.length > res.at(j).refSubseqInterval.length) {
                    KeyOfPairAlignSeq::exchange(res[i], res[j]);
            }
        }
}

void SmithWatermanAlgorithm::calculateMatrixForMultipleAlignmentResult() {
    int subst, max, max1;
    int i, j, n, e1, f1, x; unsigned int xpos = 0;
    unsigned int src_n = searchSeq.length(), pat_n = patternSeq.length();
    unsigned char *src = (unsigned char*)searchSeq.data(), *pat = (unsigned char*)patternSeq.data();

    n = pat_n * 2; int dirn = (4 + pat_n + 3) >> 2;
    int *buf, *matrix = (int*)malloc(n * sizeof(int) + pat_n * 0x80 + matrixLength * dirn);
    char *score, *score1 = (char*)(matrix + n);
    unsigned char *dir, *dir2, *dir1 = (unsigned char*)score1 + pat_n * 0x80;
    memset(matrix, 0, n * sizeof(int));
    memset(dir1, 0, dirn);
    dir = dir1 + dirn;
    dir2 = dir1 + matrixLength * dirn;

    QByteArray alphaChars = substitutionMatrix.getAlphabet()->getAlphabetChars();
    char *alphaCharsData = alphaChars.data(); n = alphaChars.size();
    for(i = 0; i < n; i++) {
        unsigned char ch = alphaCharsData[i];
        score = score1 + ch * pat_n;
        j = 0;
        do {
            score[j] = substitutionMatrix.getScore(ch, pat[j]);
        } while(++j < static_cast<int>(pat_n));
    }

    PairAlignSequences p;
    p.refSubseqInterval.startPos = 0;
    p.score = 0;

#define SW_LOOP(SWX, N) \
        max = subst + *score++; x = 3 << N; \
        f1 = buf[1]; \
        if(max <= 0) { max = 0; x = 0; } \
        if(max >= max1) { max1 = max; xpos = j; } \
        if(max < f1) { max = f1; x = 2 << N; } \
        if(max < e1) { max = e1; x = 1 << N; } \
        subst = buf[0]; \
        buf[0] = max; SWX; \
        \
        e1 += gapExtension; \
        max += gapOpen; \
        f1 = buf[1] + gapExtension; \
        e1 = e1 > max ? e1 : max; \
        f1 = f1 > max ? f1 : max; \
        buf[1] = f1; \
        buf += 2; \

    i = 1;
    do {
        buf = matrix;
        score = score1 + src[i - 1] * pat_n;
        e1 = 0; max1 = 0;
        subst = 0;

        if(dir == dir2) {
            dir = dir1;
        }
        *dir++ = 0;
        j = pat_n;
        do {
            SW_LOOP(*dir++ = x, 0);
            if(!(--j)) break;
            SW_LOOP(dir[-1] |= x, 2);
            if(!(--j)) break;
            SW_LOOP(dir[-1] |= x, 4);
            if(!(--j)) break;
            SW_LOOP(dir[-1] |= x, 6);
        } while(--j);

        #undef SW_LOOP

    /*
        for(j = 0; j < pat_n; j++)
        printf(" %02X", matrix[j * 2]);
        printf("\n");
    */
        if(max1 >= minScore) {
        QByteArray pairAlign;
        xpos = pat_n - xpos + 4; j = i;
        int xend = xpos - 3;
        unsigned char *xdir = (unsigned char*)dir - dirn;
        for(;;) {
            x = (xdir[xpos >> 2] >> ((xpos & 3) * 2)) & 3;
            if(!x) break;
            if(x == 1) {
                pairAlign.append(PairAlignSequences::LEFT);
                xpos--;
                continue;
            }
            if(x == 2) {
                pairAlign.append(PairAlignSequences::UP);
            } else if(x == 3) {
                pairAlign.append(PairAlignSequences::DIAG);
                xpos--;
            }
            if(xdir == dir1) {
                xdir = dir2;
            }
            if(xdir == dir) {
                /* printf("#error\n"); */ break;
            }
            xdir -= dirn; j--;
        }
        xpos -= 3;

        p.score = max1;
        p.refSubseqInterval.startPos = j;
        p.refSubseqInterval.length = i - j;
        p.ptrnSubseqInterval.startPos = xpos;
        p.ptrnSubseqInterval.length = xend - xpos;
        p.pairAlignment = pairAlign;
        pairAlignmentStrings.append(p);

        // printf("#%i-%i %i\n", (int)p.refSubseqInterval.startPos, (int)p.refSubseqInterval.length, (int)p.score);
        // printf("#%i-%i %s\n", xpos, xend - xpos, pairAlign.data());
        }
    } while(++i <= static_cast<int>(src_n));

    free(matrix);
}

void SmithWatermanAlgorithm::calculateMatrixForAnnotationsResult() {

    int subst, max, pos = 0, max1;
    int i, j, n, e1, f1, fpos;
    unsigned int src_n = searchSeq.length(), pat_n = patternSeq.length();
    unsigned char *src = (unsigned char*)searchSeq.data(), *pat = (unsigned char*)patternSeq.data();

    n = pat_n * 3;
    int *buf, *matrix = (int*)malloc(n * sizeof(int) + pat_n * 0x80);
    char *score, *score1 = (char*)(matrix + n);
    memset(matrix, 0, n * sizeof(int));

        QByteArray alphaChars = substitutionMatrix.getAlphabet()->getAlphabetChars();
    char *alphaCharsData = alphaChars.data(); n = alphaChars.size();
    for(i = 0; i < n; i++) {
        unsigned char ch = alphaCharsData[i];
        score = score1 + ch * pat_n;
        j = 0;
        do {
            score[j] = substitutionMatrix.getScore(ch, pat[j]);
        } while(++j < static_cast<int>(pat_n));
    }

    PairAlignSequences p;

    p.refSubseqInterval.startPos = 0;
    p.score = 0;

    i = 1;
    do {
        buf = matrix;
        score = score1 + src[i - 1] * pat_n;
        e1 = 0; max1 = 0;
        subst = 0; fpos = i - 1;
        j = pat_n;
        do {
            max = subst + *score++; n = fpos; \
            f1 = buf[2];
            if(max <= 0) { max = 0; n = i; }
            if(max >= max1) { max1 = max; pos = n; } \
            if(max < f1) { max = f1; n = buf[1]; }
            if(max < e1) { max = e1; n = buf[1 - 3]; }
            subst = buf[0]; fpos = buf[1];
            buf[0] = max; buf[1] = n;

            e1 += gapExtension;
            max += gapOpen;
            f1 = buf[2] + gapExtension;
            e1 = e1 > max ? e1 : max;
            f1 = f1 > max ? f1 : max; \
            buf[2] = f1; \
            buf += 3;
        } while(--j);

    // #define SW_FILT

        if(max1 >= minScore) {
        #ifdef SW_FILT
            if(p.refSubseqInterval.startPos != pos) {
                if(p.score) {
                pairAlignmentStrings.append(p);
                // printf("#%i-%i %i\n", (int)p.refSubseqInterval.startPos, (int)p.refSubseqInterval.length, (int)p.score);
                }
                p.refSubseqInterval.startPos = pos;
                p.refSubseqInterval.length = i - pos;
                p.score = max1;
            } else if(p.score < max1) {
                p.refSubseqInterval.length = i - pos;
                p.score = max1;
            }
        #else
            p.refSubseqInterval.startPos = pos;
            p.refSubseqInterval.length = i - pos;
            p.score = max1;
            pairAlignmentStrings.append(p);
            // printf("#%i-%i %i\n", (int)p.refSubseqInterval.startPos, (int)p.refSubseqInterval.length, (int)p.score);
        #endif
        }
    } while(++i <= static_cast<int>(src_n));

#ifdef SW_FILT
    if(p.score) {
      pairAlignmentStrings.append(p);
      // printf("#%i-%i %i\n", (int)p.refSubseqInterval.startPos, (int)p.refSubseqInterval.length, (int)p.score);
    }
#endif

    free(matrix);
}

} // namespace U2

