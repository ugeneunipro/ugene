#include <assert.h>
#include <emmintrin.h>
#include <hmmer2/funcs.h>

#include <U2Core/SequenceWalkerTask.h>
#include <U2Core/Timer.h>
#include <U2Core/U2Region.h>

#include "uhmmsearch_opt.h"

using namespace U2;

namespace {

#define CHUNK_SIZE (10 * 1024)
const int ALIGNMENT_MEM = 128;

#define ALIGNED(ptr, base) ((unsigned char*)((((quintptr)(ptr)) + ((base)-1)) & ~((base - 1))))

struct hmm_opt {
    static const int ALIGNMENT_MEM = 128;  // alignment of mem
    static const int ALIGNMENT_SCORES = 16;  // alignment of tsc, msc, etc.
    int M;  // Length of this HMM in states
    int* mem;  // honestly allocated memory. All pointers below are mem + some offset.

    int* tsc[7];  // transition scores [0..6][1..M-1]
    int* msc[MAXCODE];  // M state emission probs. [0..MAXCODE-1][1..M]
    int* isc[MAXCODE];  // I state emission probs. [0..MAXCODE-1][1..M-1]
    int* xsc[4];  // N,E,C,J transition scores [0..3][0..1]
    int* bsc;  // B state transition probs. [1..M]
    int* esc;  // E state transition probs. [1..M]

    hmm_opt(plan7_s* hmm) {
        assert(NULL != hmm);
        M = hmm->M;

        qint64 tsc_sz = 7 * (M + 16);
        qint64 msc_sz = MAXCODE * (M + 1 + 16);
        qint64 isc_sz = MAXCODE * (M + 16);
        qint64 xsc_sz = 50;
        qint64 bsc_sz = M + 1 + 12;
        qint64 esc_sz = M + 1 + 12;

        qint64 bsc_real_sz = M + 1;
        qint64 esc_real_sz = M + 1;

        qint64 total_sz = (tsc_sz + msc_sz + isc_sz + xsc_sz + bsc_sz + esc_sz) * sizeof(int) +
                          ALIGNMENT_MEM + 0x10 * (15 + 2 * MAXCODE) * sizeof(int);

        mem = (int*)malloc(total_sz);
        memset(mem, 0, total_sz);
        int* mem_aligned = (int*)ALIGNED(mem, ALIGNMENT_MEM);

        bsc = mem_aligned;
        bsc += 3;
        memcpy(bsc, hmm->bsc_mem, bsc_real_sz * sizeof(int));

        esc = (int*)ALIGNED(bsc + 4 + bsc_sz, ALIGNMENT_SCORES) + 0x10;
        esc += 3;
        memcpy(esc, hmm->esc_mem, esc_real_sz * sizeof(int));
        for (int i = 1; i < 8; i++) {
            esc[M + i] = -INFTY;
        }

        int* tsc_tmp = (int*)ALIGNED(esc + esc_sz, ALIGNMENT_SCORES) + 0x10;
        for (int i = 0; i < 7; ++i) {
            tsc[i] = tsc_tmp;
            tsc_tmp = (int*)ALIGNED(tsc_tmp + M, ALIGNMENT_SCORES) + 0x10;
        }
        tsc[1] += 3;
        tsc[4] += 3;
        for (int i = 0; i < 7; ++i) {
            memcpy(tsc[i], hmm->tsc[i], (M) * sizeof(int));
        }

        int* msc_tmp = (int*)ALIGNED(tsc[0] + tsc_sz, ALIGNMENT_SCORES);
        for (int i = 0; i < MAXCODE; ++i) {
            msc[i] = msc_tmp;
            msc[i] += 3;
            msc_tmp = (int*)ALIGNED(msc_tmp + M + 1, ALIGNMENT_SCORES) + 0x10;
            memcpy(msc[i], hmm->msc[i], (M + 1) * sizeof(int));
        }

        int* isc_tmp = (int*)ALIGNED(msc[0] + msc_sz, ALIGNMENT_SCORES) + 0x10;
        for (int i = 0; i < MAXCODE; ++i) {
            isc[i] = isc_tmp;
            isc[i] += 3;
            isc_tmp = (int*)ALIGNED(isc_tmp + M, ALIGNMENT_SCORES) + 0x10;
            memcpy(isc[i], hmm->isc[i], (M) * sizeof(int));
        }

        int* xsc_tmp = (int*)ALIGNED(isc[0] + isc_sz, ALIGNMENT_SCORES) + 0x10;
        for (int i = 0; i < 4; ++i) {
            xsc[i] = xsc_tmp;
            xsc_tmp = (int*)ALIGNED(xsc_tmp + 2, ALIGNMENT_SCORES) + 0x10;
            for (int j = 0; j < 2; ++j) {
                xsc[i][j] = hmm->xsc[i][j];
            }
        }
    }

    ~hmm_opt() {
        free(mem);
    }

    void dump() {
        int i = 0;
        int j = 0;
        ;
        printf("--START HMM OPT DUMP (M=%d)\n", M);
        for (i = 0; i < M; i++) {
            printf("HMM->ESC[%d]=%d\n", i, esc[i]);
        }
        for (i = 0; i < M; i++) {
            printf("HMM->BSC[%d]=%d\n", i, bsc[i]);
        }
        for (i = 0; i < MAXCODE; i++) {
            for (j = 0; j <= M; j++) {
                printf("HMM->MSC[%d][%d]=%d\n", i, j, msc[i][j]);
            }
        }
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 2; j++) {
                printf("HMM->XSC[%d][%d]=%d\n", i, j, xsc[i][j]);
            }
        }
        for (i = 0; i < 7; i++) {
            for (j = 0; j < M; j++) {
                printf("HMM->TSC[%d][%d]=%d\n", i, j, tsc[i][j]);
            }
        }
        for (i = 0; i < MAXCODE; i++) {
            for (j = 0; j < M; j++) {
                printf("HMM->ISC[%d][%d]=%d\n", i, j, isc[i][j]);
            }
        }
        printf("--END HMM OPT DUMP (M=%d)\n", M);
    }
};

inline int at0(__m128i from) {
    int a = _mm_extract_epi16(from, 0) & 0xFFFF;
    int b = _mm_extract_epi16(from, 1) & 0xFFFF;
    return (a | (b << 16));
}

inline int at1(__m128i from) {
    int a = _mm_extract_epi16(from, 2) & 0xFFFF;
    int b = _mm_extract_epi16(from, 3) & 0xFFFF;
    return (a | (b << 16));
}

inline int at2(__m128i from) {
    int a = _mm_extract_epi16(from, 4) & 0xFFFF;
    int b = _mm_extract_epi16(from, 5) & 0xFFFF;
    return (a | (b << 16));
}

inline int at3(__m128i from) {
    int a = _mm_extract_epi16(from, 6) & 0xFFFF;
    int b = _mm_extract_epi16(from, 7) & 0xFFFF;
    return (a | (b << 16));
}

inline __m128i shufflePMask(__m128i a, __m128i b) {
    return _mm_set_epi32(at2(a), at1(a), at0(a), at3(b));
}

inline __m128i shuffleD0Mask(const __m128i& a, const __m128i& b) {
    return _mm_set_epi32(at3(b), at2(b), at1(a), at0(a));
}

inline __m128i shuffleD1Mask(const __m128i& a, const __m128i& b) {
    return _mm_set_epi32(at0(b), at0(b), at0(b), at0(a));
}

inline __m128i shuffleD2Mask(const __m128i& a, const __m128i& b) {
    return _mm_set_epi32(at1(b), at1(b), at1(a), at0(a));
}

inline __m128i shuffleD3Mask(const __m128i& a, const __m128i& b) {
    return _mm_set_epi32(at2(b), at2(a), at1(a), at0(a));
}

inline __m128i insert0I32(const __m128i& a, int b) {
    __m128i mask_a = _mm_set_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0);
    __m128i mask_b = _mm_set_epi32(0, 0, 0, b);
    __m128i res = _mm_and_si128(a, mask_a);
    res = _mm_or_si128(res, mask_b);
    return res;
}

inline __m128i insert3I32(const __m128i& a, int b) {
    __m128i mask_a = _mm_set_epi32(0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
    __m128i mask_b = _mm_set_epi32(b, 0, 0, 0);
    __m128i res = _mm_and_si128(a, mask_a);
    res = _mm_or_si128(res, mask_b);
    return res;
}

inline __m128i selectBits(const __m128i& a, const __m128i& b, const __m128i& pattern) {
    __m128i tmpB = _mm_and_si128(pattern, b);
    __m128i tmpA = _mm_andnot_si128(pattern, a);
    return _mm_or_si128(tmpA, tmpB);
}

inline int maxComponent(const __m128i& a) {
    int max = at0(a);
    int e1 = at1(a);
    int e2 = at2(a);
    int e3 = at3(a);

    max = max > e1 ? max : e1;
    max = max > e2 ? max : e2;
    max = max > e3 ? max : e3;
    return max;
}

// void p ( const char * name, __m128i arg ) {
//     printf( "%s: {%d, %d, %d, %d}\n", name,
//         at0(arg),
//         at1(arg),
//         at2(arg),
//         at3(arg) );
// }

float viterbiSSE(hmm_opt* hmm, const unsigned char* seq, int seqLength, int* mscoreC, int* mscoreP, int* iscoreC, int* iscoreP, int* dscoreC, int* dscoreP) {
    assert(NULL != hmm);
    assert(NULL != seq && 0 < seqLength);
    assert(NULL != mscoreC && NULL != mscoreP);
    assert(NULL != iscoreC && NULL != iscoreP);
    assert(NULL != dscoreC && NULL != dscoreP);

    __m128i itV1, itV2, itV3, itV4, itV5, itV6, itV7, itV8;
    __m128i ipV, ipV1, dpV, dpV1, mpV, mpV1;
    __m128i dt1, dt2, dt3, dt4, dt, mt1, mt2, mt3, mt4;
    __m128i xscXTN = _mm_set1_epi32(hmm->xsc[XTN][MOVE]);
    __m128i xscPXMB = _mm_set1_epi32(hmm->xsc[XTN][MOVE]);
    __m128i hmmxscXTNLOOP = _mm_set1_epi32(hmm->xsc[XTN][LOOP]);
    __m128i hmmxscXTCMOVE = _mm_set1_epi32(hmm->xsc[XTC][MOVE]);
    __m128i hmmxscXTCLOOP = _mm_set1_epi32(hmm->xsc[XTC][LOOP]);
    __m128i hmmxscXTJMOVE = _mm_set1_epi32(hmm->xsc[XTJ][MOVE]);
    __m128i hmmxscXTJLOOP = _mm_set1_epi32(hmm->xsc[XTJ][LOOP]);
    __m128i hmmxscXTEMOVE = _mm_set1_epi32(hmm->xsc[XTE][MOVE]);
    __m128i hmmxscXTELOOP = _mm_set1_epi32(hmm->xsc[XTE][LOOP]);
    __m128i zeroVEC = _mm_set1_epi32(0);
    __m128i INFTY_VEC = _mm_set1_epi32(-INFTY);
    __m128i xscCXME = _mm_set1_epi32(-INFTY);
    __m128i xscCXMC = _mm_set1_epi32(-INFTY);
    __m128i xscCXMJ = _mm_set1_epi32(-INFTY);

    __m128i* hmmtscTII = (__m128i*)(hmm->tsc[TII] + 1);
    __m128i* hmmtscTMM = (__m128i*)hmm->tsc[TMM];
    __m128i* hmmtscTIM = (__m128i*)hmm->tsc[TIM];
    __m128i* hmmtscTDM = (__m128i*)hmm->tsc[TDM];
    __m128i* hmmtscTMI = (__m128i*)(hmm->tsc[TMI] + 1);
    __m128i* hmmbsc = (__m128i*)(hmm->bsc + 1);
    __m128i* hmmesc = (__m128i*)(hmm->esc + 1);

    __m128i* mscCV = (__m128i*)(mscoreC + 1);
    __m128i* iscCV = (__m128i*)(iscoreC + 1);
    __m128i* dscCV = (__m128i*)(dscoreC + 1);
    __m128i* mscPV = (__m128i*)(mscoreP + 1);
    __m128i* iscPV = (__m128i*)(iscoreP + 1);
    __m128i* dscPV = (__m128i*)(dscoreP + 1);
    __m128i* Mdt = (__m128i*)hmm->tsc[TMD] - 1;

    __m128i *mscDSQ, *iscDSQ, *tmpMV, *tmpIV, *tmpDV;
    __m128i cmp1, cmp2, cmp3;

    float result = 0;
    int i = 0, j = 0, k = 0, sc = 0, M4th = 0;
    int *tmpM = NULL, *tmpD = NULL, *hmmtscTDDip = hmm->tsc[TDD];

    M4th = (hmm->M + 3) >> 2;
    for (k = 0; k <= M4th; ++k) {
        mscPV[k] = INFTY_VEC;
        iscPV[k] = INFTY_VEC;
        dscPV[k] = INFTY_VEC;
    }

    for (i = 1; i <= seqLength; ++i) {
        xscCXME = INFTY_VEC;

        mscDSQ = (__m128i*)(hmm->msc[(int)seq[i]] + 1);
        iscDSQ = (__m128i*)(hmm->isc[(int)seq[i]] + 1);
        for (k = 1, j = 1; k <= M4th; j += 8, k += 2) {
            ipV = shufflePMask(iscPV[k - 1], iscPV[k - 2]);
            ipV1 = shufflePMask(iscPV[k], iscPV[k - 1]);
            mpV = shufflePMask(mscPV[k - 1], mscPV[k - 2]);
            mpV1 = shufflePMask(mscPV[k], mscPV[k - 1]);
            dpV = shufflePMask(dscPV[k - 1], dscPV[k - 2]);
            dpV1 = shufflePMask(dscPV[k], dscPV[k - 1]);

            itV2 = _mm_add_epi32(ipV, hmmtscTIM[k - 1]);
            itV6 = _mm_add_epi32(ipV1, hmmtscTIM[k]);
            itV1 = _mm_add_epi32(mpV, hmmtscTMM[k - 1]);
            itV5 = _mm_add_epi32(mpV1, hmmtscTMM[k]);
            itV4 = _mm_add_epi32(dpV, hmmtscTDM[k - 1]);
            itV8 = _mm_add_epi32(dpV1, hmmtscTDM[k]);
            itV3 = _mm_add_epi32(xscPXMB, hmmbsc[k - 1]);
            itV7 = _mm_add_epi32(xscPXMB, hmmbsc[k]);

            cmp1 = _mm_cmpgt_epi32(itV1, itV2);
            itV1 = selectBits(itV2, itV1, cmp1);

            cmp2 = _mm_cmpgt_epi32(itV3, itV4);
            dt4 = insert3I32(zeroVEC, hmmtscTDDip[j + 2]);
            itV2 = selectBits(itV4, itV3, cmp2);
            dt3 = _mm_set1_epi32(hmmtscTDDip[j + 1]);
            cmp1 = _mm_cmpgt_epi32(itV1, itV2);
            dt3 = _mm_add_epi32(dt3, dt4);
            itV3 = selectBits(itV2, itV1, cmp1);
            cmp2 = _mm_cmpgt_epi32(itV5, itV6);
            dt3 = shuffleD0Mask(zeroVEC, dt3);
            itV1 = selectBits(itV6, itV5, cmp2);

            cmp1 = _mm_cmpgt_epi32(itV7, itV8);
            dt2 = _mm_set1_epi32(hmmtscTDDip[j]);
            itV2 = selectBits(itV8, itV7, cmp1);
            dt2 = _mm_add_epi32(dt2, dt3);
            cmp2 = _mm_cmpgt_epi32(itV1, itV2);
            mscCV[k - 1] = _mm_add_epi32(itV3, mscDSQ[k - 1]);
            itV4 = selectBits(itV2, itV1, cmp2);
            dt2 = insert0I32(dt2, 0);
            mscCV[k] = _mm_add_epi32(itV4, mscDSQ[k]);
            dt1 = _mm_set1_epi32(hmmtscTDDip[j - 1]);
            mpV = shufflePMask(mscCV[k - 1], mscCV[k - 2]);
            dt1 = _mm_add_epi32(dt1, dt2);
            mt1 = _mm_add_epi32(mpV, Mdt[k]);
            dt = _mm_set1_epi32(dscoreC[j - 1]);
            mt2 = shuffleD1Mask(INFTY_VEC, mt1);
            dt1 = _mm_add_epi32(dt, dt1);
            mt3 = shuffleD2Mask(INFTY_VEC, mt1);
            dt2 = _mm_add_epi32(mt2, dt2);
            mt4 = shuffleD3Mask(INFTY_VEC, mt1);
            dt3 = _mm_add_epi32(mt3, dt3);
            cmp1 = _mm_cmpgt_epi32(dt1, mt1);
            dt4 = _mm_add_epi32(mt4, dt4);
            cmp2 = _mm_cmpgt_epi32(dt2, dt3);
            dt1 = selectBits(mt1, dt1, cmp1);
            cmp3 = _mm_cmpgt_epi32(dt1, dt4);
            dt2 = selectBits(dt3, dt2, cmp2);
            dt1 = selectBits(dt4, dt1, cmp3);
            cmp2 = _mm_cmpgt_epi32(dt1, dt2);
            dt4 = insert3I32(zeroVEC, hmmtscTDDip[j + 6]);
            dscCV[k - 1] = selectBits(dt2, dt1, cmp2);
            dt3 = _mm_set1_epi32(hmmtscTDDip[j + 5]);
            itV1 = _mm_add_epi32(mscPV[k - 1], hmmtscTMI[k - 1]);
            dt3 = _mm_add_epi32(dt3, dt4);
            dt3 = shuffleD0Mask(zeroVEC, dt3);
            dt2 = _mm_set1_epi32(hmmtscTDDip[j + 4]);
            itV2 = _mm_add_epi32(iscPV[k - 1], hmmtscTII[k - 1]);
            dt2 = _mm_add_epi32(dt2, dt3);
            dt1 = _mm_set1_epi32(hmmtscTDDip[j + 3]);
            dt2 = insert0I32(dt2, 0);
            cmp1 = _mm_cmpgt_epi32(itV1, itV2);
            dt1 = _mm_add_epi32(dt1, dt2);
            mpV = shufflePMask(mscCV[k], mscCV[k - 1]);
            mt1 = _mm_add_epi32(mpV, Mdt[k + 1]);
            dt = _mm_set1_epi32(dscoreC[j + 3]);
            itV3 = selectBits(itV2, itV1, cmp1);
            dt1 = _mm_add_epi32(dt, dt1);
            mt2 = shuffleD1Mask(INFTY_VEC, mt1);
            itV1 = _mm_add_epi32(mscPV[k], hmmtscTMI[k]);
            dt2 = _mm_add_epi32(mt2, dt2);
            mt3 = shuffleD2Mask(INFTY_VEC, mt1);
            itV2 = _mm_add_epi32(iscPV[k], hmmtscTII[k]);
            dt3 = _mm_add_epi32(mt3, dt3);
            mt4 = shuffleD3Mask(INFTY_VEC, mt1);
            cmp3 = _mm_cmpgt_epi32(itV1, itV2);
            dt4 = _mm_add_epi32(mt4, dt4);
            cmp1 = _mm_cmpgt_epi32(dt1, mt1);
            itV4 = selectBits(itV2, itV1, cmp3);
            cmp2 = _mm_cmpgt_epi32(dt2, dt3);
            dt1 = selectBits(mt1, dt1, cmp1);
            dt2 = selectBits(dt3, dt2, cmp2);
            cmp1 = _mm_cmpgt_epi32(dt1, dt4);
            dt1 = selectBits(dt4, dt1, cmp1);
            cmp2 = _mm_cmpgt_epi32(dt1, dt2);
            iscCV[k - 1] = _mm_add_epi32(itV3, iscDSQ[k - 1]);
            dscCV[k] = selectBits(dt2, dt1, cmp2);
            itV1 = _mm_add_epi32(mscCV[k - 1], hmmesc[k - 1]);
            itV2 = _mm_add_epi32(mscCV[k], hmmesc[k]);
            cmp1 = _mm_cmpgt_epi32(itV1, xscCXME);
            cmp2 = _mm_cmpgt_epi32(itV2, xscCXME);
            xscCXME = selectBits(xscCXME, itV1, cmp1);
            iscCV[k] = _mm_add_epi32(itV4, iscDSQ[k]);
            xscCXME = selectBits(xscCXME, itV2, cmp2);
        }
        xscCXME = _mm_set1_epi32(maxComponent(xscCXME));
        itV2 = _mm_add_epi32(xscCXMJ, hmmxscXTJLOOP);
        itV3 = _mm_add_epi32(xscCXME, hmmxscXTELOOP);
        itV1 = _mm_add_epi32(xscCXMJ, hmmxscXTJMOVE);

        cmp1 = _mm_cmpgt_epi32(itV2, itV3);
        xscXTN = _mm_add_epi32(xscXTN, hmmxscXTNLOOP);
        xscCXMJ = selectBits(itV3, itV2, cmp1);
        itV2 = _mm_add_epi32(xscCXMC, hmmxscXTCLOOP);
        cmp1 = _mm_cmpgt_epi32(xscXTN, itV1);
        itV3 = _mm_add_epi32(xscCXME, hmmxscXTEMOVE);
        xscPXMB = selectBits(itV1, xscXTN, cmp1);
        cmp2 = _mm_cmpgt_epi32(itV2, itV3);
        xscCXMC = selectBits(itV3, itV2, cmp2);
        tmpMV = mscCV;
        tmpIV = iscCV;
        tmpDV = dscCV;
        mscCV = mscPV;
        iscCV = iscPV;
        dscCV = dscPV;
        mscPV = tmpMV;
        iscPV = tmpIV;
        dscPV = tmpDV;
        tmpM = mscoreC;
        tmpD = dscoreC;
        mscoreC = mscoreP;
        dscoreC = dscoreP;
        mscoreP = tmpM;
        dscoreP = tmpD;
    }
    itV1 = _mm_add_epi32(xscCXMC, hmmxscXTCMOVE);
    sc = at0(itV1);
    result = (float)sc / INTSCALE;
    return result;
}

}  // anonymous namespace

// assuming dsq is aligned
QList<float> sseScoring(unsigned char* dsq, int seqlen, plan7_s* hmm, HMMSeqGranulation* gr, TaskStateInfo& ti) {
    assert(dsq);
    assert(gr);
    assert(seqlen > 0);

    const int M4th = (hmm->M + 3) >> 2;
    const int memSz = 4 + (M4th + 1) * 16 + 12 + ALIGNMENT_MEM;
    char* mscCurMem = (char*)malloc(memSz);
    char* iscCurMem = (char*)malloc(memSz);
    char* dscCurMem = (char*)malloc(memSz);
    char* mscPrevMem = (char*)malloc(memSz);
    char* iscPrevMem = (char*)malloc(memSz);
    char* dscPrevMem = (char*)malloc(memSz);

    memset(mscCurMem, 0, memSz);
    memset(iscCurMem, 0, memSz);
    memset(dscCurMem, 0, memSz);
    memset(mscPrevMem, 0, memSz);
    memset(iscPrevMem, 0, memSz);
    memset(dscPrevMem, 0, memSz);

    int* mscCur = (int*)(ALIGNED(mscCurMem, ALIGNMENT_MEM) + 0xC);
    int* iscCur = (int*)(ALIGNED(iscCurMem, ALIGNMENT_MEM) + 0xC);
    int* dscCur = (int*)(ALIGNED(dscCurMem, ALIGNMENT_MEM) + 0xC);
    int* mscPrev = (int*)(ALIGNED(mscPrevMem, ALIGNMENT_MEM) + 0xC);
    int* iscPrev = (int*)(ALIGNED(iscPrevMem, ALIGNMENT_MEM) + 0xC);
    int* dscPrev = (int*)(ALIGNED(dscPrevMem, ALIGNMENT_MEM) + 0xC);

    QList<float> results;
    U2Region range(0, seqlen);
    gr->overlap = 2 * hmm->M;
    gr->exOverlap = 0;
    gr->chunksize = qBound(gr->overlap + 1, CHUNK_SIZE, seqlen);
    gr->regions = SequenceWalkerTask::splitRange(range, gr->chunksize, gr->overlap, gr->exOverlap, false);
    const QVector<U2Region>& regions = gr->regions;

    int regionsSz = regions.size();
    int regionsPassed = 0;

    hmm_opt hmmOpt(hmm);
    foreach (U2Region chunk, regions) {
        unsigned char* curSeqAddr = dsq + chunk.startPos;
        int curSeqLen = chunk.length;
        float sc = viterbiSSE(&hmmOpt, curSeqAddr, curSeqLen, mscCur, mscPrev, iscCur, iscPrev, dscCur, dscPrev);
        results.push_back(sc);

        ti.progress = (int)((100.0 * (regionsPassed++)) / regionsSz);
        if (ti.cancelFlag) {
            break;
        }
    }

    free(mscCurMem);
    free(iscCurMem);
    free(dscCurMem);
    free(mscPrevMem);
    free(iscPrevMem);
    free(dscPrevMem);

    return results;
}
