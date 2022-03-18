#ifndef __UHMMSEARCH_OPT_H__
#define __UHMMSEARCH_OPT_H__

#include <hmmer2/config.h>
#include <hmmer2/structs.h>

#include <QVector>

namespace U2 {
class U2Region;
class TaskStateInfo;
}  // namespace U2

struct HMMSeqGranulation {
    HMMSeqGranulation()
        : overlap(0), chunksize(0), exOverlap(0) {};
    int overlap;
    int chunksize;
    int exOverlap;
    QVector<U2::U2Region> regions;
};

typedef QList<float> (*hmmScoringFunction)(unsigned char* dsq, int seqlen, plan7_s* hmm, HMMSeqGranulation* gr, U2::TaskStateInfo& ti);

void main_loop_opt(struct plan7_s* hmm, const char* seq_, int seqlen, struct threshold_s* thresh, int do_forward, int do_null2, int do_xnu, struct histogram_s* histogram, struct tophit_s* ghit, struct tophit_s* dhit, int* ret_nseq, U2::TaskStateInfo& ti, hmmScoringFunction scoring_f);

#endif
