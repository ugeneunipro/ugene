#ifndef _U2_UHMM_CALIBRATE_H_
#define _U2_UHMM_CALIBRATE_H_

struct plan7_s;
struct alphabet_s;
struct histogram_s;

#include <float.h>

#include <QMutex>
#include <QVector>

namespace U2 {

class TaskStateInfo;

class UHMMCalibrateSettings {
public:
    UHMMCalibrateSettings();

    int nsample;  // number of random seqs to sample
    int seed;  // random number seed
    int fixedlen;  // fixed length, or 0 if unused
    float lenmean;  // mean of length distribution
    float lensd;  // std dev of length distribution
    int nThreads;
};
struct WorkPool_s {
    WorkPool_s() {
        hmm = NULL;
        fixedlen = 0;
        lenmean = 0;
        lensd = 0;
        nsample = 0;
        nseq = 0;
        hist = NULL;
        max_score = -FLT_MAX;
        progress = NULL;
    }

    /* Static configuration */
    plan7_s* hmm; /* ptr to single HMM to search with    */
    int fixedlen; /* if >0, fix random seq len to this   */
    float lenmean; /* mean of Gaussian for random seq len */
    float lensd; /* s.d. of Gaussian for random seq len */
    QVector<float> randomseq; /* 0..Alphabet_size-1 i.i.d. probs     */
    int nsample; /* number of random seqs to do         */

    /* Shared (mutex-protected) input */
    int nseq; /* current number of seqs searched     */

    /* Shared (mutex-protected) output */
    QMutex lockInput;
    QMutex lockOutput;

    histogram_s* hist; /* histogram          */
    float max_score; /* maximum score seen */

    int* progress;
};

class UHMMCalibrate {
public:
    static void calibrate(plan7_s* hmm, const UHMMCalibrateSettings& s, TaskStateInfo& si);
    static void calibrateParallel(WorkPool_s* wpool, TaskStateInfo& si);
};

}  // namespace U2

#endif
