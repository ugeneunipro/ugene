#include "uhmmcalibrate.h"
#include <assert.h>
#include <float.h>
#include <hmmer2/funcs.h>
#include <stdlib.h>
#include <time.h>

#include <QMutexLocker>

#include <U2Core/Task.h>

namespace U2 {
static void main_loop_serial(struct plan7_s* hmm, int seed, int nsample, float lenmean, float lensd, int fixedlen, struct histogram_s** ret_hist, float* ret_max, int& cancelFlag, int& progress);

UHMMCalibrateSettings::UHMMCalibrateSettings() {
    nsample = 5000;
    fixedlen = 0;
    lenmean = 325.;
    lensd = 200.;
    seed = (int)time((time_t*)NULL);
    nThreads = 1;
}

void UHMMCalibrate::calibrate(plan7_s* hmm, const UHMMCalibrateSettings& s, TaskStateInfo& si) {
    struct histogram_s* hist;  // a resulting histogram

    float max = 0;
    main_loop_serial(hmm, s.seed, s.nsample, s.lenmean, s.lensd, s.fixedlen, &hist, &max, si.cancelFlag, si.progress);
    if (!si.cancelFlag) {
        // Fit an EVD to the observed histogram.
        // The TRUE left-censors and fits only the right slope of the histogram.
        // The 9999. is an arbitrary high number that means we won't trim
        // outliers on the right.

        if (!ExtremeValueFitHistogram(hist, TRUE, 9999.)) {
            si.setError("fit failed; num sequences may be set too small?\n");
        } else {
            hmm->flags |= PLAN7_STATS;
            hmm->mu = hist->param[EVD_MU];
            hmm->lambda = hist->param[EVD_LAMBDA];
        }
    }
    FreeHistogram(hist);
}

// Function: main_loop_serial()
// Date:     SRE, Tue Aug 18 16:18:28 1998 [St. Louis]
//
// Purpose:  Given an HMM and parameters for synthesizing random
//           sequences; return a histogram of scores.
//           (Serial version)
//
// Args:     hmm      - an HMM to calibrate.
//           seed     - random number seed
//           nsample  - number of seqs to synthesize
//           lenmean  - mean length of random sequence
//           lensd    - std dev of random seq length
//           fixedlen - if nonzero, override lenmean, always this len
//           ret_hist - RETURN: the score histogram
//           ret_max  - RETURN: highest score seen in simulation
//
// Returns:  (void)
//           hist is alloc'ed here, and must be free'd by caller.

static void main_loop_serial(struct plan7_s* hmm, int seed, int nsample, float lenmean, float lensd, int fixedlen, struct histogram_s** ret_hist, float* ret_max, int& cancelFlag, int& progress) {
    struct histogram_s* hist;
    struct dpmatrix_s* mx;
    float randomseq[MAXABET];
    float p1;
    float max;
    char* seq;
    unsigned char* dsq;
    float score;
    int sqlen;
    int idx;

    // Initialize.
    // We assume we've already set the alphabet (safe, because
    // HMM input sets the alphabet).

    sre_srandom(seed);

    // get HMMERTaskLocalData
    HMMERTaskLocalData* tls = getHMMERTaskLocalData();
    alphabet_s& al = tls->al;

    SetAlphabet(hmm->atype);

    P7Logoddsify(hmm, TRUE);
    P7DefaultNullModel(randomseq, &p1);
    hist = AllocHistogram(-200, 200, 100);
    mx = CreatePlan7Matrix(1, hmm->M, 25, 0);
    max = -FLT_MAX;

    progress = 0;
    int pStub;

    for (idx = 0; idx < nsample && !cancelFlag; idx++) {
        // choose length of random sequence
        if (fixedlen) {
            sqlen = fixedlen;
        } else {
            do
                sqlen = (int)Gaussrandom(lenmean, lensd);
            while (sqlen < 1);
        }
        // generate it
        seq = RandomSequence(al.Alphabet, randomseq, al.Alphabet_size, sqlen);
        dsq = DigitizeSequence(seq, sqlen);

        if (P7ViterbiSpaceOK(sqlen, hmm->M, mx)) {
            score = P7Viterbi(dsq, sqlen, hmm, mx, NULL);
        } else {
            score = P7SmallViterbi(dsq, sqlen, hmm, mx, NULL, pStub);
        }

        AddToHistogram(hist, score);
        max = qMax(score, max);

        progress = int(100 * idx / float(nsample));

        free(dsq);
        free(seq);
    }

    FreePlan7Matrix(mx);
    *ret_hist = hist;
    *ret_max = max;
}

// parallel calibrate

void UHMMCalibrate::calibrateParallel(WorkPool_s* wpool, TaskStateInfo& si) {
    HMMERTaskLocalData* tls = getHMMERTaskLocalData();
    struct alphabet_s* al = &tls->al;
    struct plan7_s* hmm = wpool->hmm;
    struct dpmatrix_s* mx = CreatePlan7Matrix(1, hmm->M, 25, 0);

    int len;
    float sc;
    char* seq;
    unsigned char* dsq;

    while (true) {
        /* generate a sequence */
        {
            QMutexLocker locker(&wpool->lockInput);
            wpool->nseq++;
            if (wpool->nseq > wpool->nsample) { /* we're done; release input lock, break loop */
                break;
            }
            if (wpool->fixedlen) {
                len = wpool->fixedlen;
            } else {
                do {
                    len = (int)Gaussrandom(wpool->lenmean, wpool->lensd);
                } while (len < 1);
            }
            seq = RandomSequence(al->Alphabet, wpool->randomseq.data(), al->Alphabet_size, len);
        }

        /* compute score */
        dsq = DigitizeSequence(seq, len);

        if (P7ViterbiSpaceOK(len, hmm->M, mx)) {
            sc = P7Viterbi(dsq, len, hmm, mx, NULL);
        } else {
            int pStub;
            sc = P7SmallViterbi(dsq, len, hmm, mx, NULL, pStub);
        }
        free(dsq);
        free(seq);

        /* save output */
        QMutexLocker locker(&wpool->lockOutput);
        AddToHistogram(wpool->hist, sc);
        wpool->max_score = qMax(wpool->max_score, sc);
        si.progress = int(100 * wpool->nseq / float(wpool->nsample));  // TODO: update progress for all tasks?
        if (wpool->progress != NULL) {
            *wpool->progress = si.progress;
        }
    }
    FreePlan7Matrix(mx);
}

}  // namespace U2
