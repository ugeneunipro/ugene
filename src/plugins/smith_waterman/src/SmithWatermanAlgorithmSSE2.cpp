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

#ifdef SW2_BUILD_WITH_SSE2

#include "SmithWatermanAlgorithmSSE2.h"

#include <emmintrin.h>
#include <iostream>

#ifdef _MSC_VER
#pragma warning( disable : 4799 )
#endif

extern "C" {

// http://sourceforge.net/p/predef/wiki/Compilers/

#ifdef _MSC_VER
#if UGENE_X86_64
#define LAME_MSC
#endif
#endif

#if defined (LAME_MSC) || defined (Q_OS_MAC)
inline void xmm_store8(__m64* m, __m128i x) {
    *(int*)m = _mm_cvtsi128_si32(x);
    *((int*)m + 1) = _mm_cvtsi128_si32(_mm_srli_epi64(x, 32));
}

inline __m128i xmm_load8(__m64* m) {
    return _mm_or_si128(_mm_cvtsi32_si128(*(int*)m), _mm_slli_si128(_mm_cvtsi32_si128(*((int*)m + 1)), 4));
}
#else
inline void xmm_store8(__m64* m, __m128i x) { *m = _mm_movepi64_pi64(x); }
inline __m128i xmm_load8(__m64* m) { return _mm_movpi64_epi64(*m); }
#endif

}

using namespace std;

namespace U2 {

quint64 SmithWatermanAlgorithmSSE2::estimateNeededRamAmount(const QByteArray
    & _patternSeq, const QByteArray & _searchSeq, const qint32 gapOpen,
    const qint32 gapExtension, const quint32 minScore, const quint32 maxScore,
    const SmithWatermanSettings::SWResultView resultView)
{
    const double b_to_mb_factor = 1048576.0;

    const quint64 queryLength = _patternSeq.length();
    const quint64 searchLength = _searchSeq.length();

    const quint32 iter = (queryLength + 7) >> 3;

    quint64 memNeeded = 0;
    if(SmithWatermanSettings::MULTIPLE_ALIGNMENT == resultView) {
        qint32 maxGapPenalty = (gapOpen > gapExtension) ? gapOpen : gapExtension;
        assert(0 > maxGapPenalty);

        quint64 matrixLength = queryLength - (maxScore - minScore) / maxGapPenalty + 1;
        if (searchLength + 1 < matrixLength) {
            matrixLength = searchLength + 1;
        }

        memNeeded = iter * (131 + matrixLength) * sizeof(__m128i);
    } else if(SmithWatermanSettings::ANNOTATIONS == resultView) {
        memNeeded = (5 + iter * 133) * sizeof(__m128i);
    } else {
        assert(false);
    }

    return memNeeded / b_to_mb_factor;
}

void SmithWatermanAlgorithmSSE2::launch(const SMatrix& _substitutionMatrix, const QByteArray & _patternSeq,
    const QByteArray & _searchSeq, int _gapOpen, int _gapExtension, int _minScore, SmithWatermanSettings::SWResultView _resultView) {
    setValues(_substitutionMatrix, _patternSeq, _searchSeq, _gapOpen, _gapExtension, _minScore, _resultView);
    int maxScore = 0;
    if (isValidParams() && calculateMatrixLength()) {
        maxScore = calculateMatrixSSE2(patternSeq.length(), (unsigned char *)searchSeq.data(),
            searchSeq.length(), (-1)*(gapOpen + gapExtension), (-1)*(gapExtension));

        if (minScore <= maxScore) {
            if (maxScore >= 0x8000 || matrixLength >= 0x10000) {
                switch(resultView) {
                case SmithWatermanSettings::MULTIPLE_ALIGNMENT:
                    calculateMatrixForMultipleAlignmentResultWithInt();
                    break;
                case SmithWatermanSettings::ANNOTATIONS:
                    calculateMatrixForAnnotationsResultWithInt();
                    break;
                default:
                    assert(false);
                }
            } else {
                switch(resultView) {
                case SmithWatermanSettings::MULTIPLE_ALIGNMENT:
                    calculateMatrixForMultipleAlignmentResultWithShort();
                    break;
                case SmithWatermanSettings::ANNOTATIONS:
                    calculateMatrixForAnnotationsResultWithShort();
                    break;
                default:
                    assert(false);
                }
            }
        }
    }
}

void SmithWatermanAlgorithmSSE2::calculateMatrixForMultipleAlignmentResultWithShort() {

    int i, j, n, k, max1;
    __m128i f1 = _mm_setzero_si128(), f2 = _mm_setzero_si128(), f3 = _mm_setzero_si128(), f4 = _mm_setzero_si128(), e1 = _mm_setzero_si128();
    unsigned int src_n = searchSeq.length(), pat_n = patternSeq.length();
    unsigned char *src = (unsigned char*)searchSeq.data(), *pat = (unsigned char*)patternSeq.data();
    unsigned int iter = (pat_n + 7) >> 3;

    n = iter * 2;
    __m128i *buf, *matrix = (__m128i*)_mm_malloc((n + iter * 0x80) * 16 + iter * 8 * 4 + matrixLength * iter * 8, 16);
    short *score, *score1 = (short*)(matrix + n);
    int *map = (int*)(score1 + iter * 0x80 * 8);
    char *dir, *dir2, *dir1 = (char*)(map + iter * 8);
    memset(matrix, 0, n * sizeof(__m128i));
    memset(dir1, 0, iter * 8);
    dir = dir1 + iter * 8;
    dir2 = dir1 + matrixLength * iter * 8;

    for(i = 0, j = 0; j < static_cast<int>(iter); j++) {
        for(k = j, n = 0; n < 8; n++, k += iter) {
            map[k] = i++;
        }
    }

    QByteArray alphaChars = substitutionMatrix.getAlphabet()->getAlphabetChars();
    char *alphaCharsData = alphaChars.data(); n = alphaChars.size();
    for(i = 0; i < n; i++) {
        int n;
        unsigned char ch = alphaCharsData[i];
        score = score1 + ch * iter * 8;
        for(j = 0; j < static_cast<int>(iter); j++) {
            for(k = j, n = 0; n < 8; n++, k += iter) {
                int a = -0x8000;
                if(k < static_cast<int>(pat_n)) {
                    a = substitutionMatrix.getScore(ch, pat[k]);
                }
                *score++ = a;
            }
        }
    }

    __m128i xMax = _mm_setzero_si128(), xPos = _mm_setzero_si128();
    __m128i xOpen = _mm_setzero_si128();
    xOpen = _mm_insert_epi16(xOpen, gapOpen, 0);
    __m128i xExt = _mm_setzero_si128();
    xExt = _mm_insert_epi16(xExt, gapExtension, 0);
    xOpen = _mm_shufflelo_epi16(xOpen, 0);
    xExt = _mm_shufflelo_epi16(xExt, 0);
    xOpen = _mm_unpacklo_epi32(xOpen, xOpen);
    xExt = _mm_unpacklo_epi32(xExt, xExt);

    PairAlignSequences p;

    p.refSubseqInterval.startPos = 0;
    p.score = 0;

    i = 1;
    do {
        buf = matrix;
        score = score1 + src[i - 1] * iter * 8;
        xMax = _mm_xor_si128(xMax, xMax);
        f1 = _mm_slli_si128(_mm_load_si128(buf + (iter - 1) * 2), 2);
        e1 = _mm_xor_si128(e1, e1);
        if(dir == dir2) dir = dir1;
        j = iter;
        do {
            f2 = _mm_adds_epi16(f1, *((__m128i*)score)); score += 8; /* subst */

            f3 = _mm_xor_si128(f3, f3);
            f2 = _mm_max_epi16(f2, f3);
            f3 = _mm_cmpgt_epi16(f2, f3);
            f3 = _mm_slli_epi16(f3, 2);
            /* f2 f3 */
            f4 = _mm_insert_epi16(f4, j, 0);
            f4 = _mm_shufflelo_epi16(f4, 0);
            f4 = _mm_unpacklo_epi32(f4, f4);
            xMax = _mm_max_epi16(xMax, f2);
            f1 = _mm_cmpeq_epi16(f2, xMax);
            xPos = _mm_or_si128(_mm_and_si128(f4, f1), _mm_andnot_si128(f1, xPos));

            f1 = _mm_load_si128(buf + 1);
            f1 = _mm_max_epi16(f1, f2);
            f2 = _mm_cmpgt_epi16(f1, f2);
            f2 = _mm_slli_epi16(f2, 1);
            f3 = _mm_or_si128(f3, f2);
            /* f1 f3 */
            f2 = _mm_max_epi16(e1, f1);
            f1 = _mm_cmpgt_epi16(e1, f1);
            f3 = _mm_or_si128(f3, f1);
            /* f2 f3 */
            f1 = _mm_load_si128(buf);
            _mm_store_si128(buf, f2);
            f3 = _mm_packs_epi16(f3, f3);
            // *(__m64*)dir = _mm_movepi64_pi64(f3); dir += 8;
            xmm_store8((__m64*)dir, f3);
            dir += 8;

            f2 = _mm_adds_epi16(f2, xOpen);
            e1 = _mm_max_epi16(_mm_adds_epi16(e1, xExt), f2);
            f3 = _mm_load_si128(buf + 1);
            f3 = _mm_max_epi16(_mm_adds_epi16(f3, xExt), f2);
            _mm_store_si128(buf + 1, f3);
            buf += 2;
        } while(--j);

        buf = matrix; j = (-1) * iter;
        e1 = _mm_slli_si128(e1, 2);
        f1 = _mm_load_si128(buf);
        f3 = _mm_max_epi16(_mm_xor_si128(f3, f3), _mm_adds_epi16(f1, xOpen));
        k = _mm_movemask_epi8(_mm_cmpgt_epi16(e1, f3));
        if(k) {
            do {
                f2 = _mm_max_epi16(e1, f1);
                _mm_store_si128(buf, f2);

                f1 = _mm_cmpgt_epi16(f2, f1);
                f2 = _mm_adds_epi16(f2, xOpen);
                f1 = _mm_packs_epi16(f1, f1);
                // f1 = _mm_or_si128(f1, _mm_movpi64_epi64(*((__m64*)dir + j)));
                f1 = _mm_or_si128(f1, xmm_load8((__m64*)dir + j));
                f2 = _mm_max_epi16(f2, *(buf + 1));
                // *((__m64*)dir + j) = _mm_movepi64_pi64(f1);
                xmm_store8((__m64*)dir + j, f1);
                _mm_store_si128(buf + 1, f2);

                e1 = _mm_adds_epi16(e1, xExt);
                buf += 2;
                if(!(++j)) {
                    buf = matrix;
                    j = (-1) * iter;
                    e1 = _mm_slli_si128(e1, 2);
                }
                f1 = _mm_load_si128(buf);
                f3 = _mm_max_epi16(_mm_xor_si128(f3, f3), _mm_adds_epi16(f1, xOpen));
                k = _mm_movemask_epi8(_mm_cmpgt_epi16(e1, f3));
            } while(k);
        }
    /*
        for(j = 0; j < pat_n; j++)
        printf(" %02X", *((unsigned short*)(matrix + (j % iter) * 2) + (j / iter)));
        printf("\n");
    */
        max1 = *((short*)(&xMax)); n = 0;
        k = 1;
        do {
            j = ((short*)(&xMax))[k];
            if(j >= max1) {
                max1 = j;
                n = k;
            }
        } while(++k < 8);

        if(max1 >= minScore) {
            QByteArray pairAlign;
            int xpos = 1 + n * iter + iter - ((unsigned short*)(&xPos))[n]; j = i;
            int xend = xpos;
            char *xdir = dir - iter * 8;
            for(;;) {
                if(!xpos)
                    break;
                k = xdir[map[xpos - 1]];
                if(!k)
                    break;
                if(k == -1) {
                    pairAlign.append(PairAlignSequences::LEFT);
                    xpos--;
                    continue;
                }
                if(k == -2) {
                    pairAlign.append(PairAlignSequences::UP);
                } else if(k == -4) {
                    pairAlign.append(PairAlignSequences::DIAG);
                    xpos--;
                }
                if(xdir == dir1) {
                    xdir = dir2;
                }
                if(xdir == dir) {
                    /* printf("#error\n"); */ break;
                }
                xdir -= iter * 8; j--;
            }

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

    _mm_free(matrix);
}

void SmithWatermanAlgorithmSSE2::calculateMatrixForAnnotationsResultWithShort() {
    int i, j, n, k, max1;
    __m128i f1 = _mm_setzero_si128(), f2 = _mm_setzero_si128(), f3 = _mm_setzero_si128(), f4 = _mm_setzero_si128(), e1 = _mm_setzero_si128();
    unsigned int src_n = searchSeq.length(), pat_n = patternSeq.length();
    unsigned char *src = (unsigned char*)searchSeq.data(), *pat = (unsigned char*)patternSeq.data();
    unsigned int iter = (pat_n + 7) >> 3;

    n = (iter + 1) * 5;
    __m128i *buf, *matrix = (__m128i*)_mm_malloc((n + iter * 0x80) * sizeof(__m128i), 16);
    short *score, *score1 = (short*)(matrix + n);
    memset(matrix, 0, n * sizeof(__m128i));

    QByteArray alphaChars = substitutionMatrix.getAlphabet()->getAlphabetChars();
    char *alphaCharsData = alphaChars.data(); n = alphaChars.size();
    for(i = 0; i < n; i++) {
        int n;
        unsigned char ch = alphaCharsData[i];
        score = score1 + ch * iter * 8;
        for(j = 0; j < static_cast<int>(iter); j++) {
            for(k = j, n = 0; n < 8; n++, k += iter) {
                int a = -0x8000;
                if(k < static_cast<int>(pat_n)) {
                    a = substitutionMatrix.getScore(ch, pat[k]);
                }
                *score++ = a;
            }
        }
    }

    __m128i xMax = _mm_setzero_si128(), xPos = _mm_setzero_si128();
    __m128i xOpen = _mm_setzero_si128();
    xOpen = _mm_insert_epi16(xOpen, gapOpen, 0);
    __m128i xExt = _mm_setzero_si128();
    xExt = _mm_insert_epi16(xExt, gapExtension, 0);
    xOpen = _mm_shufflelo_epi16(xOpen, 0);
    xExt = _mm_shufflelo_epi16(xExt, 0);
    xOpen = _mm_unpacklo_epi32(xOpen, xOpen);
    xExt = _mm_unpacklo_epi32(xExt, xExt);

    PairAlignSequences p;

    p.refSubseqInterval.startPos = 0;
    p.score = 0;

#define SW_LOOP(SWA, SWB) \
      buf = matrix + 5; \
      score = score1 + src[i - 1] * iter * 8; \
      xMax = _mm_xor_si128(xMax, xMax); \
      f4 = _mm_insert_epi16(f4, i, 0); \
      f4 = _mm_shufflelo_epi16(f4, 0); \
      f4 = _mm_unpacklo_epi32(f4, f4); \
      f2 = _mm_slli_si128(_mm_load_si128(SWB + (iter - 1) * 5), 2); \
      f1 = _mm_slli_si128(_mm_load_si128(SWB + 1 + (iter - 1) * 5), 2); \
      f1 = _mm_insert_epi16(f1, i - 1, 0); \
      e1 = _mm_xor_si128(e1, e1); \
      j = iter; do { \
        f2 = _mm_adds_epi16(f2, *((__m128i*)score)); score += 8; /* subst */ \
        /* f2 f1 */ \
        f3 = _mm_xor_si128(f3, f3); \
        f2 = _mm_max_epi16(f2, f3); \
        f3 = _mm_cmpeq_epi16(f3, f2); \
        f3 = _mm_or_si128(_mm_and_si128(f3, f4), _mm_andnot_si128(f3, f1)); \
        /* f2 f3 */ \
        xMax = _mm_max_epi16(xMax, f2); \
        f1 = _mm_cmpeq_epi16(f2, xMax); \
        xPos = _mm_or_si128(_mm_and_si128(f1, f3), _mm_andnot_si128(f1, xPos)); \
        \
        f1 = _mm_load_si128(buf + 4); \
        f1 = _mm_max_epi16(f1, f2); \
        f2 = _mm_cmpeq_epi16(f2, f1); \
        f3 = _mm_or_si128(_mm_and_si128(f3, f2), _mm_andnot_si128(f2, _mm_load_si128(SWB + 1))); \
        /* f1 f3 */ \
        f2 = _mm_max_epi16(e1, f1); \
        f1 = _mm_cmpeq_epi16(f1, f2); \
        f3 = _mm_or_si128(_mm_and_si128(f3, f1), _mm_andnot_si128(f1, _mm_load_si128(SWA - 5 + 1))); \
        /* f2 f3 */ \
        _mm_store_si128(SWA, f2); \
        _mm_store_si128(SWA + 1, f3); \
        f2 = _mm_adds_epi16(f2, xOpen); \
        e1 = _mm_max_epi16(_mm_adds_epi16(e1, xExt), f2); \
        f1 = _mm_load_si128(buf + 4); \
        f1 = _mm_max_epi16(_mm_adds_epi16(f1, xExt), f2); \
        _mm_store_si128(buf + 4, f1); \
        \
        f2 = _mm_load_si128(SWB); \
        f1 = _mm_load_si128(SWB + 1); \
        buf += 5; \
      } while(--j); \
      \
      f4 = _mm_slli_si128(_mm_load_si128(SWA - 5 + 1), 2); \
      buf = matrix + 5; j = 0; \
      e1 = _mm_slli_si128(e1, 2); \
      f2 = _mm_load_si128(SWA); \
      f3 = _mm_max_epi16(_mm_xor_si128(f3, f3), _mm_adds_epi16(f2, xOpen)); \
      k = _mm_movemask_epi8(_mm_cmpgt_epi16(e1, f3)); \
      if(k) do { \
        f1 = _mm_max_epi16(e1, f2); \
        f2 = _mm_cmpeq_epi16(f2, f1); \
        f2 = _mm_or_si128(_mm_and_si128(f2, *(SWA + 1)), _mm_andnot_si128(f2, f4)); \
        _mm_store_si128(SWA, f1); \
        _mm_store_si128(SWA + 1, f2); \
        \
        f1 = _mm_adds_epi16(f1, xOpen); \
        f1 = _mm_max_epi16(f1, *(buf + 4)); \
        _mm_store_si128(buf + 4, f1); \
        \
        e1 = _mm_adds_epi16(e1, xExt); \
        buf += 5; \
        if(++j >= static_cast<int>(iter)) {\
            buf = matrix + 5;\
            j = 0;\
            e1 = _mm_slli_si128(e1, 2);\
            f4 = _mm_slli_si128(f4, 2);\
        } \
        f2 = _mm_load_si128(SWA); \
        f3 = _mm_max_epi16(_mm_xor_si128(f3, f3), _mm_adds_epi16(f2, xOpen)); \
        k = _mm_movemask_epi8(_mm_cmpgt_epi16(e1, f3)); \
      } while(k); \
      \
      max1 = *((short*)(&xMax)); n = 0; \
      k = 1; do { \
        j = ((short*)(&xMax))[k]; \
        if(j >= max1) { max1 = j; n = k; } \
      } while(++k < 8); \
      \
      if(max1 >= minScore) { \
        j = ((((short*)(&xPos))[n] - i - 1) | -0x10000) + i + 1; \
        SW_FILT_MACRO; \
      }

// #define SW_FILT

#ifdef SW_FILT
#define SW_FILT_MACRO \
        if(p.refSubseqInterval.startPos != j) { \
          if(p.score) { \
            pairAlignmentStrings.append(p); \
            /* printf("#%i-%i %i\n", (int)p.refSubseqInterval.startPos, (int)p.refSubseqInterval.length, (int)p.score); */ \
          } \
          p.refSubseqInterval.startPos = j; \
          p.refSubseqInterval.length = i - j; \
          p.score = max1; \
        } else if(p.score < max1) { \
          p.refSubseqInterval.length = i - j; \
          p.score = max1; \
        }
#else
#define SW_FILT_MACRO \
        p.refSubseqInterval.startPos = j; \
        p.refSubseqInterval.length = i - j; \
        p.score = max1; \
        pairAlignmentStrings.append(p); \
        /* printf("#%i-%i %i\n", (int)p.refSubseqInterval.startPos, (int)p.refSubseqInterval.length, (int)p.score); */
#endif

    i = 1;
    do {
        SW_LOOP(buf, buf + 2);
        if(++i > static_cast<int>(src_n)) {
            break;
        }
        SW_LOOP(buf + 2, buf);
    } while(++i <= static_cast<int>(src_n));

#undef SW_LOOP
#undef SW_FILT_MACRO

#ifdef SW_FILT
    if(p.score) {
      pairAlignmentStrings.append(p);
      // printf("#%i-%i %i\n", (int)p.refSubseqInterval.startPos, (int)p.refSubseqInterval.length, (int)p.score);
    }
#endif
    _mm_free(matrix);
}

void SmithWatermanAlgorithmSSE2::calculateMatrixForMultipleAlignmentResultWithInt() {
    int i, j, n, k, max1;
    __m128i f1 = _mm_setzero_si128(), f2 = _mm_setzero_si128(), f3 = _mm_setzero_si128(), f4 = _mm_setzero_si128(), e1 = _mm_setzero_si128();
    unsigned int src_n = searchSeq.length(), pat_n = patternSeq.length();
    unsigned char *src = (unsigned char*)searchSeq.data(), *pat = (unsigned char*)patternSeq.data();
    unsigned int iter = (pat_n + 3) >> 2;

    n = iter * 2;
    __m128i *buf, *matrix = (__m128i*)_mm_malloc((n + iter * 0x80 + iter) * 16 + matrixLength * iter * 4, 16);
    int *score, *score1 = (int*)(matrix + n);
    int *map = score1 + iter * 0x80 * 4;
    char *dir, *dir2, *dir1 = (char*)(map + iter * 4);
    memset(matrix, 0, n * sizeof(__m128i));
    memset(dir1, 0, iter * 4);
    dir = dir1 + iter * 4;
    dir2 = dir1 + matrixLength * iter * 4;

    for(i = 0, j = 0; j < static_cast<int>(iter); j++) {
        for(k = j, n = 0; n < 4; n++, k += iter) {
            map[k] = i++;
        }
    }

    QByteArray alphaChars = substitutionMatrix.getAlphabet()->getAlphabetChars();
    char *alphaCharsData = alphaChars.data(); n = alphaChars.size();
    for(i = 0; i < n; i++) {
        int n;
        unsigned char ch = alphaCharsData[i];
        score = score1 + ch * iter * 4;
        for(j = 0; j < static_cast<int>(iter); j++) {
            for(k = j, n = 0; n < 4; n++, k += iter) {
                int a = -0x8000;
                if(k < static_cast<int>(pat_n)) {
                    a = substitutionMatrix.getScore(ch, pat[k]);
                }
                *score++ = a;
            }
        }
    }

    __m128i xMax = _mm_setzero_si128(), xPos = _mm_setzero_si128();
    __m128i xOpen = _mm_cvtsi32_si128(gapOpen);
    __m128i xExt = _mm_cvtsi32_si128(gapExtension);
    xOpen = _mm_shuffle_epi32(xOpen, 0);
    xExt = _mm_shuffle_epi32(xExt, 0);

    PairAlignSequences p;

    p.refSubseqInterval.startPos = 0;
    p.score = 0;

    i = 1;
    do {
        buf = matrix;
        score = score1 + src[i - 1] * iter * 4;
        xMax = _mm_xor_si128(xMax, xMax);
        f1 = _mm_slli_si128(_mm_load_si128(buf + (iter - 1) * 2), 4);
        e1 = _mm_xor_si128(e1, e1);
        if(dir == dir2) dir = dir1;
        j = iter;
        do {
            f2 = _mm_add_epi32(f1, *((__m128i*)score)); score += 4; /* subst */

            QByteArray alphaChars = substitutionMatrix.getAlphabet()->getAlphabetChars();
            char *alphaCharsData = alphaChars.data(); n = alphaChars.size();
            for(i = 0; i < n; i++) {
                int n;
                unsigned char ch = alphaCharsData[i];
                score = score1 + ch * iter * 4;
                for(j = 0; j < static_cast<int>(iter); j++) {
                    for(k = j, n = 0; n < 4; n++, k += iter) {
                        int a = -0x8000;
                        if(k < static_cast<int>(pat_n)) {
                            a = substitutionMatrix.getScore(ch, pat[k]);
                        }
                        *score++ = a;
                    }
                }
            }

            f3 = _mm_xor_si128(f3, f3);
            f3 = _mm_cmpgt_epi32(f2, f3);
            f2 = _mm_and_si128(f2, f3);
            f3 = _mm_slli_epi32(f3, 2);
            /* f2 f3 */
            f4 = _mm_cvtsi32_si128(j);
            f4 = _mm_shuffle_epi32(f4, 0);
            f1 = _mm_cmpgt_epi32(xMax, f2);
            xMax = _mm_xor_si128(xMax, f2);
            xMax = _mm_and_si128(xMax, f1);
            xMax = _mm_xor_si128(xMax, f2);
            xPos = _mm_or_si128(_mm_and_si128(xPos, f1), _mm_andnot_si128(f1, f4));

            f1 = _mm_load_si128(buf + 1);
            f4 = _mm_cmpgt_epi32(f1, f2);
            f1 = _mm_xor_si128(f1, f2);
            f1 = _mm_and_si128(f1, f4);
            f1 = _mm_xor_si128(f1, f2);
            f4 = _mm_slli_epi32(f4, 1);
            f3 = _mm_or_si128(f3, f4);
            /* f1 f3 */
            f4 = _mm_cmpgt_epi32(e1, f1);
            f2 = _mm_xor_si128(e1, f1);
            f2 = _mm_and_si128(f2, f4);
            f2 = _mm_xor_si128(f2, f1);
            f3 = _mm_or_si128(f3, f4);
            /* f2 f3 */
            f1 = _mm_load_si128(buf);
            _mm_store_si128(buf, f2);
            f3 = _mm_packs_epi32(f3, f3);
            f3 = _mm_packs_epi16(f3, f3);
            *(int*)dir = _mm_cvtsi128_si32(f3); dir += 4;
            f2 = _mm_add_epi32(f2, xOpen);

            e1 = _mm_add_epi32(e1, xExt);
            f4 = _mm_cmpgt_epi32(e1, f2);
            e1 = _mm_xor_si128(e1, f2);
            e1 = _mm_and_si128(e1, f4);
            e1 = _mm_xor_si128(e1, f2);

            f3 = _mm_load_si128(buf + 1);
            f3 = _mm_add_epi32(f3, xExt);
            f4 = _mm_cmpgt_epi32(f3, f2);
            f3 = _mm_xor_si128(f3, f2);
            f3 = _mm_and_si128(f3, f4);
            f3 = _mm_xor_si128(f3, f2);
            _mm_store_si128(buf + 1, f3);
            buf += 2;
        } while(--j);

        buf = matrix;
        j = (-1) * iter;
        e1 = _mm_slli_si128(e1, 4);
        f1 = _mm_load_si128(buf);

        f2 = _mm_add_epi32(f1, xOpen);
        f3 = _mm_xor_si128(f3, f3);
        f3 = _mm_cmpgt_epi32(f2, f3);
        f2 = _mm_and_si128(f2, f3);
        k = _mm_movemask_epi8(_mm_cmpgt_epi32(e1, f2));
        if(k) {
            do {
                f4 = _mm_cmpgt_epi32(e1, f1);
                f2 = _mm_xor_si128(e1, f1);
                f2 = _mm_and_si128(f2, f4);
                f2 = _mm_xor_si128(f2, f1);
                _mm_store_si128(buf, f2);

                f2 = _mm_add_epi32(f2, xOpen);
                f4 = _mm_packs_epi32(f4, f4);
                f4 = _mm_packs_epi16(f4, f4);
                f4 = _mm_or_si128(f4, _mm_cvtsi32_si128(*((int*)dir + j)));
                f1 = _mm_load_si128(buf + 1);
                *((int*)dir + j) = _mm_cvtsi128_si32(f4);
                f3 = _mm_cmpgt_epi32(f1, f2);
                f1 = _mm_xor_si128(f1, f2);
                f1 = _mm_and_si128(f1, f3);
                f1 = _mm_xor_si128(f1, f2);
                _mm_store_si128(buf + 1, f1);

                e1 = _mm_add_epi32(e1, xExt);
                buf += 2;
                if(!(++j)) {
                    buf = matrix;
                    j = (-1) * iter;
                    e1 = _mm_slli_si128(e1, 4);
                }
                f1 = _mm_load_si128(buf);
                f2 = _mm_add_epi32(f1, xOpen);
                f3 = _mm_xor_si128(f3, f3);
                f3 = _mm_cmpgt_epi32(f2, f3);
                f2 = _mm_and_si128(f2, f3);
                k = _mm_movemask_epi8(_mm_cmpgt_epi32(e1, f2));
            } while(k);
        }
    /*
        for(j = 0; j < pat_n; j++)
        printf(" %02X", *((int*)(matrix + (j % iter) * 2) + (j / iter)));
        printf("\n");
    */
        max1 = *((int*)(&xMax)); n = 0;
        k = 1; do {
        j = ((int*)(&xMax))[k];
        if(j >= max1) { max1 = j; n = k; }
        } while(++k < 4);

        if(max1 >= minScore) {
        QByteArray pairAlign;
        int xpos = 1 + n * iter + iter - ((int*)(&xPos))[n]; j = i;
        int xend = xpos;
        char *xdir = dir - iter * 4;
        for(;;) {
            if(!xpos) break;
            k = xdir[map[xpos - 1]];
            if(!k) break;
            if(k == -1) {
            pairAlign.append(PairAlignSequences::LEFT);
            xpos--;
            continue;
            }
            if(k == -2) {
            pairAlign.append(PairAlignSequences::UP);
            } else if(k == -4) {
            pairAlign.append(PairAlignSequences::DIAG);
            xpos--;
            }
            if(xdir == dir1) xdir = dir2;
            if(xdir == dir) { /* printf("#error\n"); */ break; }
            xdir -= iter * 4; j--;
        }

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

    _mm_free(matrix);
}

void SmithWatermanAlgorithmSSE2::calculateMatrixForAnnotationsResultWithInt() {

    int i, j, n, k, max1;
    __m128i f1 = _mm_setzero_si128(), f2 = _mm_setzero_si128(), f3 = _mm_setzero_si128(), f4 = _mm_setzero_si128(), e1 = _mm_setzero_si128();
    unsigned int src_n = searchSeq.length(), pat_n = patternSeq.length();
    unsigned char *src = (unsigned char*)searchSeq.data(), *pat = (unsigned char*)patternSeq.data();
    unsigned int iter = (pat_n + 3) >> 2;

    n = (iter + 1) * 5;
    __m128i *buf, *matrix = (__m128i*)_mm_malloc((n + iter * 0x80) * sizeof(__m128i), 16);
    int *score, *score1 = (int*)(matrix + n);
    memset(matrix, 0, n * sizeof(__m128i));

    QByteArray alphaChars = substitutionMatrix.getAlphabet()->getAlphabetChars();
    char *alphaCharsData = alphaChars.data();
    n = alphaChars.size();
    for(i = 0; i < n; i++) {
        int n;
        unsigned char ch = alphaCharsData[i];
        score = score1 + ch * iter * 4;
        for(j = 0; j < static_cast<int>(iter); j++) {
            for(k = j, n = 0; n < 4; n++, k += iter) {
                int a = -0x8000;
                if(k < static_cast<int>(pat_n)) {
                    a = substitutionMatrix.getScore(ch, pat[k]);
                }
                *score++ = a;
            }
        }
    }

    __m128i xMax = _mm_setzero_si128(), xPos = _mm_setzero_si128();
    __m128i xOpen  = _mm_setzero_si128();
    xOpen = _mm_cvtsi32_si128(gapOpen);
    __m128i xExt  = _mm_setzero_si128();
    xExt = _mm_cvtsi32_si128(gapExtension);
    xOpen = _mm_shuffle_epi32(xOpen, 0);
    xExt = _mm_shuffle_epi32(xExt, 0);

    PairAlignSequences p;

    p.refSubseqInterval.startPos = 0;
    p.score = 0;

#define SW_LOOP(SWA, SWB) \
      buf = matrix + 5; \
      score = score1 + src[i - 1] * iter * 4; \
      xMax = _mm_xor_si128(xMax, xMax); \
      f2 = _mm_slli_si128(_mm_load_si128(SWB + (iter - 1) * 5), 4); \
      f1 = _mm_slli_si128(_mm_load_si128(SWB + 1 + (iter - 1) * 5), 4); \
      f1 = _mm_or_si128(f1, _mm_cvtsi32_si128(i - 1)); \
      e1 = _mm_xor_si128(e1, e1); \
      j = iter; do { \
        f2 = _mm_add_epi32(f2, *((__m128i*)score)); score += 4; /* subst */ \
        /* f2 f1 */ \
        f4 = _mm_cvtsi32_si128(i); \
        f4 = _mm_shuffle_epi32(f4, 0); \
        f3 = _mm_xor_si128(f3, f3); \
        f3 = _mm_cmpgt_epi32(f2, f3); \
        f2 = _mm_and_si128(f2, f3); \
        f3 = _mm_or_si128(_mm_and_si128(f1, f3), _mm_andnot_si128(f3, f4)); \
        /* f2 f3 */ \
        f1 = _mm_cmpgt_epi32(xMax, f2); \
        xMax = _mm_xor_si128(xMax, f2); \
        xMax = _mm_and_si128(xMax, f1); \
        xMax = _mm_xor_si128(xMax, f2); \
        xPos = _mm_or_si128(_mm_and_si128(xPos, f1), _mm_andnot_si128(f1, f3)); \
        \
        f1 = _mm_load_si128(buf + 4); \
        f4 = _mm_cmpgt_epi32(f1, f2); \
        f1 = _mm_xor_si128(f1, f2); \
        f1 = _mm_and_si128(f1, f4); \
        f1 = _mm_xor_si128(f1, f2); \
        f3 = _mm_or_si128(_mm_and_si128(f4, *(SWB + 1)), _mm_andnot_si128(f4, f3)); \
        /* f1 f3 */ \
        f4 = _mm_cmpgt_epi32(e1, f1); \
        f2 = _mm_xor_si128(e1, f1); \
        f2 = _mm_and_si128(f2, f4); \
        f2 = _mm_xor_si128(f2, f1); \
        f3 = _mm_or_si128(_mm_and_si128(f4, *(SWA - 5 + 1)), _mm_andnot_si128(f4, f3)); \
        /* f2 f3 */ \
        _mm_store_si128(SWA, f2); \
        _mm_store_si128(SWA + 1, f3); \
        f2 = _mm_add_epi32(f2, xOpen); \
        e1 = _mm_add_epi32(e1, xExt); \
        f1 = _mm_cmpgt_epi32(e1, f2); \
        e1 = _mm_xor_si128(e1, f2); \
        e1 = _mm_and_si128(e1, f1); \
        e1 = _mm_xor_si128(e1, f2); \
        f3 = _mm_load_si128(buf + 4); \
        f3 = _mm_add_epi32(f3, xExt); \
        f1 = _mm_cmpgt_epi32(f3, f2); \
        f3 = _mm_xor_si128(f3, f2); \
        f3 = _mm_and_si128(f3, f1); \
        f3 = _mm_xor_si128(f3, f2); \
        _mm_store_si128(buf + 4, f3); \
        \
        f2 = _mm_load_si128(SWB); \
        f1 = _mm_load_si128(SWB + 1); \
        buf += 5; \
      } while(--j); \
      \
      f4 = _mm_slli_si128(_mm_load_si128(SWA - 5 + 1), 4); \
      buf = matrix + 5; j = 0; \
      e1 = _mm_slli_si128(e1, 4); \
      f2 = _mm_load_si128(SWA); \
      f1 = _mm_add_epi32(f2, xOpen); \
      f3 = _mm_xor_si128(f3, f3); \
      f3 = _mm_cmpgt_epi32(f1, f3); \
      f1 = _mm_and_si128(f1, f3); \
      k = _mm_movemask_epi8(_mm_cmpgt_epi32(e1, f1)); \
      if(k) do { \
        f3 = _mm_cmpgt_epi32(e1, f2); \
        f1 = _mm_xor_si128(e1, f2); \
        f1 = _mm_and_si128(f1, f3); \
        f1 = _mm_xor_si128(f1, f2); \
        f2 = _mm_or_si128(_mm_and_si128(f3, f4), _mm_andnot_si128(f3, *(SWA + 1))); \
        _mm_store_si128(SWA, f1); \
        _mm_store_si128(SWA + 1, f2); \
        \
        f1 = _mm_add_epi32(f1, xOpen); \
        f2 = _mm_load_si128(buf + 4); \
        f3 = _mm_cmpgt_epi32(f1, f2); \
        f1 = _mm_xor_si128(f1, f2); \
        f1 = _mm_and_si128(f1, f3); \
        f1 = _mm_xor_si128(f1, f2); \
        _mm_store_si128(buf + 4, f2); \
        \
        e1 = _mm_add_epi32(e1, xExt); \
        buf += 5; \
        if(++j >= static_cast<int>(iter)) {\
            buf = matrix + 5;\
            j = 0;\
            e1 = _mm_slli_si128(e1, 4);\
            f4 = _mm_slli_si128(f4, 4);\
        } \
        f2 = _mm_load_si128(SWA); \
        f1 = _mm_add_epi32(f2, xOpen); \
        f3 = _mm_xor_si128(f3, f3); \
        f3 = _mm_cmpgt_epi32(f1, f3); \
        f1 = _mm_and_si128(f1, f3); \
        k = _mm_movemask_epi8(_mm_cmpgt_epi32(e1, f1)); \
      } while(k); \
      \
      max1 = *((int*)(&xMax)); n = 0; \
      k = 1; do { \
        j = ((int*)(&xMax))[k]; \
        if(j >= max1) { max1 = j; n = k; } \
      } while(++k < 4); \
      \
      if(max1 >= minScore) { \
        j = ((int*)(&xPos))[n]; \
        SW_FILT_MACRO; \
      }

// #define SW_FILT

#ifdef SW_FILT
#define SW_FILT_MACRO \
        if(p.refSubseqInterval.startPos != j) { \
          if(p.score) { \
            pairAlignmentStrings.append(p); \
            /* printf("#%i-%i %i\n", (int)p.refSubseqInterval.startPos, (int)p.refSubseqInterval.length, (int)p.score); */ \
          } \
          p.refSubseqInterval.startPos = j; \
          p.refSubseqInterval.length = i - j; \
          p.score = max1; \
        } else if(p.score < max1) { \
          p.refSubseqInterval.length = i - j; \
          p.score = max1; \
        }
#else
#define SW_FILT_MACRO \
        p.refSubseqInterval.startPos = j; \
        p.refSubseqInterval.length = i - j; \
        p.score = max1; \
        pairAlignmentStrings.append(p); \
        /* printf("#%i-%i %i\n", (int)p.refSubseqInterval.startPos, (int)p.refSubseqInterval.length, (int)p.score); */
#endif

    i = 1; do {
        SW_LOOP(buf, buf + 2);
        if(++i > static_cast<int>(src_n)) {
            break;
        }
        SW_LOOP(buf + 2, buf);
    } while(++i <= static_cast<int>(src_n));

#undef SW_LOOP
#undef SW_FILT_MACRO

#ifdef SW_FILT
    if(p.score) {
      pairAlignmentStrings.append(p);
      // printf("#%i-%i %i\n", (int)p.refSubseqInterval.startPos, (int)p.refSubseqInterval.length, (int)p.score);
    }
#endif
    _mm_free(matrix);
}


inline void SmithWatermanAlgorithmSSE2::printVector(__m128i &toprint, int add) {

    ScoreType * tmpArray = (ScoreType*)_mm_malloc(nElementsInVec*sizeof(ScoreType),16);

    _mm_store_si128((__m128i*) &tmpArray[0], toprint);
    cout <<"printVector" <<endl;
    for (int i = 0; i < nElementsInVec; i++) cout <<tmpArray[i] + add <<" ";

    cout <<endl;
}

int SmithWatermanAlgorithmSSE2::calculateMatrixSSE2(unsigned queryLength, unsigned char *dbSeq, unsigned dbLength, unsigned short gapOpenOrig, unsigned short gapExtend) {

    unsigned iter = (queryLength + nElementsInVec - 1) / nElementsInVec;

    int ALPHA_SIZE = substitutionMatrix.getAlphabet()->getNumAlphabetChars();

    __m128i* pvQueryProf = (__m128i*)_mm_malloc
        ('Z'*ALPHA_SIZE*iter*sizeof(__m128i), 16);

    int weight = 0;
    unsigned short * queryProfile = (unsigned short *) pvQueryProf;
    int segSize = (queryLength + 7) / 8; //iter
    int nCount = segSize * 8;
    char curChar = ' ';

    QByteArray alphaChars = substitutionMatrix.getAlphabet()->getAlphabetChars();
    for (int i = 0; i < ALPHA_SIZE; i++) {
        curChar = alphaChars.at(i);
        int h = 0;
        for (int j = 0; j < segSize; j++) {
            unsigned int k = j;
            for (unsigned int kk = 0; kk < 8; kk++) {
                if (k >= queryLength) {
                    weight = 0;
                } else {
                    weight = substitutionMatrix.getScore(curChar, patternSeq.at(k));
                }
                queryProfile[curChar * nCount + h] = (unsigned short) weight;
                k += segSize;
                h++;
            }
        }
    }


    __m128i *pvHLoad = (__m128i*)_mm_malloc(iter*sizeof(__m128i), 16);
    __m128i *pvHStore = (__m128i*)_mm_malloc(iter*sizeof(__m128i), 16);
    __m128i *pvE = (__m128i*)_mm_malloc(iter*sizeof(__m128i), 16);


    unsigned i = 0, j = 0;
    int     score = 0;

    int cmp = 0;

    unsigned short gapOpenFarrar = gapOpenOrig - gapExtend;

    __m128i *pv = 0;

    __m128i vE = _mm_set1_epi32(0), vF = _mm_set1_epi32(0), vH = _mm_set1_epi32(0);

    __m128i vMaxScore = _mm_set1_epi32(0);
    __m128i vGapOpen = _mm_set1_epi32(0);
    __m128i vGapExtend = _mm_set1_epi32(0);

    __m128i vMin = _mm_set1_epi32(0);
    __m128i vMinimums = _mm_set1_epi32(0);
    __m128i vTemp = _mm_set1_epi32(0);

    __m128i *pvScore = 0;


    /* Load gap opening penalty to all elements of a constant */
    vGapOpen = _mm_insert_epi16 (vGapOpen, gapOpenFarrar, 0);
    vGapOpen = _mm_shufflelo_epi16 (vGapOpen, 0);
    vGapOpen = _mm_shuffle_epi32 (vGapOpen, 0);

    /* Load gap extension penalty to all elements of a constant */
    vGapExtend = _mm_insert_epi16 (vGapExtend, gapExtend, 0);
    vGapExtend = _mm_shufflelo_epi16 (vGapExtend, 0);
    vGapExtend = _mm_shuffle_epi32 (vGapExtend, 0);

    /*  load vMaxScore with the zeros.  since we are using signed */
    /*  math, we will bias the maxscore to -32768 so we have the */
    /*  full range of the short. */
    vMaxScore = _mm_cmpeq_epi16 (vMaxScore, vMaxScore);
    vMaxScore = _mm_slli_epi16 (vMaxScore, 15);

    vMinimums = _mm_shuffle_epi32 (vMaxScore, 0);
    Q_UNUSED(vMinimums);

    vMin = _mm_shuffle_epi32 (vMaxScore, 0);
    vMin = _mm_srli_si128 (vMin, 14);

    /* Zero out the storage vector */
    for (i = 0; i < iter; ++i) {
        _mm_store_si128 (pvE + i, vMaxScore);
        _mm_store_si128 (pvHStore + i, vMaxScore);
    }

    PairAlignSequences p;
    U2Region sReg;

    for (i = 0; i < dbLength; ++i) {


        /* fetch first data asap. */
        pvScore = pvQueryProf + dbSeq[i] * iter;

        /* zero out F. */
        vF = _mm_cmpeq_epi16 (vF, vF);
        vF = _mm_slli_epi16 (vF, 15);

        /* load the next h value */
        vH = _mm_load_si128 (pvHStore + iter - 1);
        vH = _mm_slli_si128 (vH, 2);
        vH = _mm_or_si128 (vH, vMin);

        pv = pvHLoad;
        pvHLoad = pvHStore;
        pvHStore = pv;

        for (j = 0; j < iter; ++j) {

            /* load values of vF and vH from previous row (one unit up) */
            vE = _mm_load_si128 (pvE + j);

            /* add score to vH */
            vH = _mm_adds_epi16 (vH, *pvScore++);

            /* Update highest score encountered this far */
            vMaxScore = _mm_max_epi16 (vMaxScore, vH);

            /* get max from vH, vE and vF */
            vH = _mm_max_epi16 (vH, vE);
            vH = _mm_max_epi16 (vH, vF);

            /* save vH values */
            _mm_store_si128 (pvHStore + j, vH);

            /* update vE value */
            vH = _mm_subs_epi16 (vH, vGapOpen);
            vE = _mm_subs_epi16 (vE, vGapExtend);
            vE = _mm_max_epi16 (vE, vH);

            /* update vF value */
            vF = _mm_subs_epi16 (vF, vGapExtend);
            vF = _mm_max_epi16 (vF, vH);

            /* save vE values */
            _mm_store_si128 (pvE + j, vE);

            /* load the next h value */
            vH = _mm_load_si128 (pvHLoad + j);

        }

        /* reset pointers to the start of the saved data */
        j = 0;
        vH = _mm_load_si128 (pvHStore + j);

        /*  the computed vF value is for the given column.  since */
        /*  we are at the end, we need to shift the vF value over */
        /*  to the next column. */

        vF = _mm_slli_si128 (vF, 2);
        vF = _mm_or_si128 (vF, vMin);
        vTemp = _mm_subs_epi16 (vH, vGapOpen);

        vTemp = _mm_cmpgt_epi16 (vF, vTemp);

        cmp  = _mm_movemask_epi8 (vTemp);

        while (cmp != 0x0000)
            //for (unsigned cnt=0; cnt<iter; ++cnt)
        {
            vE = _mm_load_si128 (pvE + j);

            vH = _mm_max_epi16 (vH, vF);

            /* save vH values */
            _mm_store_si128 (pvHStore + j, vH);

            /*  update vE incase the new vH value would change it */
            vH = _mm_subs_epi16 (vH, vGapOpen);
            vE = _mm_max_epi16 (vE, vH);


            /* update vF value */
            vF = _mm_subs_epi16 (vF, vGapExtend);


            j++;
            if (j >= iter) {
                j = 0;
                vF = _mm_slli_si128 (vF, 2);
                vF = _mm_or_si128 (vF, vMin);
            }

            vH = _mm_load_si128 (pvHStore + j);

            vTemp = _mm_subs_epi16 (vH, vGapOpen);
            vTemp = _mm_cmpgt_epi16 (vF, vTemp);
            cmp  = _mm_movemask_epi8 (vTemp);
        }
    }

    /* find largest score in the vMaxScore vector */
    vTemp = _mm_srli_si128 (vMaxScore, 8);
    vMaxScore = _mm_max_epi16 (vMaxScore, vTemp);
    vTemp = _mm_srli_si128 (vMaxScore, 4);
    vMaxScore = _mm_max_epi16 (vMaxScore, vTemp);
    vTemp = _mm_srli_si128 (vMaxScore, 2);
    vMaxScore = _mm_max_epi16 (vMaxScore, vTemp);

    /* store in temporary variable */
    score = (short) _mm_extract_epi16 (vMaxScore, 0);

    _mm_free(pvHLoad);
    _mm_free(pvHStore);
    _mm_free(pvE);
    _mm_free(pvQueryProf);

    /* return largest score */
    return score + 32768;
}

} //namespase
#endif //SW2_BUILD_WITH_SSE2
