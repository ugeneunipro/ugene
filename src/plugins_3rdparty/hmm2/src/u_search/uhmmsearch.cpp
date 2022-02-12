#include "uhmmsearch.h"
#include <HMMIO.h>
#include <hmmer2/funcs.h>

#include <U2Core/Task.h>

#include "uhmmsearch_opt.h"
#include "uhmmsearch_sse.h"

namespace U2 {

static void main_loop_serial(struct plan7_s* hmm, const char* seq, int seqLen, struct threshold_s* thresh, int do_forward, int do_null2, int do_xnu, struct histogram_s* histogram, struct tophit_s* ghit, struct tophit_s* dhit, int* ret_nseq, TaskStateInfo& ti);

QList<UHMMSearchResult> UHMMSearch::search(plan7_s* _hmm, const char* seq, int seqLen, const UHMMSearchSettings& s, TaskStateInfo& si) {
    plan7_s* hmm = HMMIO::cloneHMM(_hmm);
    // Set up optional Pfam score thresholds.
    threshold_s thresh;  // contains all threshold (cutoff) info
    thresh.globE = s.globE;  // use a reasonable Eval threshold
    thresh.globT = -FLT_MAX;  // but no bit threshold
    thresh.domT = s.domT;  // no domain bit threshold
    thresh.domE = s.domE;  // and no domain Eval threshold
    thresh.autocut = CUT_NONE;  // and no Pfam cutoffs used
    thresh.Z = s.eValueNSeqs;  // Z not preset; use actual # of seqs

    int do_null2 = TRUE;  // TRUE to adjust scores with null model #2
    int do_forward = FALSE;  // TRUE to use Forward() not Viterbi()
    int do_xnu = FALSE;  // TRUE to filter sequences thru XNU
    QList<UHMMSearchResult> res;  // the results of the method

    // get HMMERTaskLocalData
    HMMERTaskLocalData* tld = getHMMERTaskLocalData();
    alphabet_s* al = &tld->al;

    SetAlphabet(hmm->atype);

    P7Logoddsify(hmm, !do_forward);  // TODO: clone model to avoid changes in it or make it thread safe??

    if (do_xnu && al->Alphabet_type == hmmNUCLEIC) {
        si.setError("The HMM is a DNA model, and you can't use the --xnu filter on DNA data");
        return res;
    }

    /*****************************************************************
     * Set up optional Pfam score thresholds.
     * Can do this before starting any searches, since we'll only use 1 HMM.
     *****************************************************************/

    if (!SetAutocuts(&thresh, hmm)) {
        si.setError("HMM did not contain the GA, TC, or NC cutoffs you needed");
        return res;
    }

    // set up structures for storing output
    histogram_s* histogram = AllocHistogram(-200, 200, 100);  // keeps full histogram of all scores
    tophit_s* ghit = AllocTophits(200);  // per-seq hits: 200=lumpsize
    tophit_s* dhit = AllocTophits(200);  // domain hits:  200=lumpsize

    int nseq = 0;  // number of sequences searched
    if (s.alg == HMMSearchAlgo_SSEOptimized) {
        main_loop_opt(hmm, seq, seqLen, &thresh, do_forward, do_null2, do_xnu, histogram, ghit, dhit, &nseq, si, sseScoring);
    } else if (s.alg == HMMSearchAlgo_Conservative) {
        main_loop_serial(hmm, seq, seqLen, &thresh, do_forward, do_null2, do_xnu, histogram, ghit, dhit, &nseq, si);
    } else {
        assert(false && "bad hmmsearch algorithm selected");
    }
    // Process hit lists, produce text output

    // Set the theoretical EVD curve in our histogram using calibration in the HMM, if available.
    if (hmm->flags & PLAN7_STATS) {
        ExtremeValueSetHistogram(histogram, hmm->mu, hmm->lambda, histogram->lowscore, histogram->highscore, 0);
    }
    if (!thresh.Z) {
        thresh.Z = nseq;  // set Z for good now that we're done
    }

    // report our output

    FullSortTophits(dhit);

    // int namewidth = MAX(8, TophitsMaxName(ghit)); // max width of sequence name

    // Report domain hits (sorted on E-value)
    for (int i = 0; i < dhit->num && !si.cancelFlag; i++) {
        float sc;  // score of an HMM search
        double pvalue;  // pvalue of an HMM score
        double evalue;  // evalue of an HMM score
        char *name, *desc;  // hit sequence name and description
        double motherp;  // pvalue of a whole seq HMM score
        float mothersc;  // score of a whole seq parent of domain
        int sqfrom, sqto;  // coordinates in sequence
        int sqlen;  // length of seq that was hit
        int hmmfrom, hmmto;  // coordinate in HMM
        int ndom;  // total # of domains in this seq
        int domidx;  // number of this domain

        GetRankedHit(dhit, i, &pvalue, &sc, &motherp, &mothersc, &name, NULL, &desc, &sqfrom, &sqto, &sqlen,  // seq position info
                     &hmmfrom,
                     &hmmto,
                     NULL,  // HMM position info
                     &domidx,
                     &ndom,  // domain info
                     NULL);  // alignment info

        evalue = pvalue * (double)thresh.Z;

        if (motherp * (double)thresh.Z > thresh.globE || mothersc < thresh.globT) {
            continue;
        } else if (evalue <= thresh.domE && sc >= thresh.domT) {
            // hmm reports results in range [1...N] -> translate it to [0..N)
            res.append(UHMMSearchResult(U2Region(sqfrom - 1, sqto - sqfrom + 1), sc, evalue));
        }
    }

    // Clean-up and exit.
    FreeHistogram(histogram);
    FreeTophits(ghit);
    FreeTophits(dhit);
    FreePlan7(hmm);

    return res;
}

// Function: main_loop_serial()
// Date:     SRE, Wed Sep 23 10:20:49 1998 [St. Louis]
//
// Purpose:  Search an HMM against a sequence database.
//           main loop for the serial (non-PVM, non-threads)
//           version.
//
//           In:   HMM and open sqfile, plus options
//           Out:  histogram, global hits list, domain hits list, nseq.
//
// Args:     hmm        - the HMM to search with.
//           seq,
//           seqLen
//           thresh     - score/evalue threshold info
//           do_forward - TRUE to score using Forward()
//           do_null2   - TRUE to use ad hoc null2 score correction
//           do_xnu     - TRUE to apply XNU mask
//           histogram  - RETURN: score histogram
//           ghit       - RETURN: ranked global scores
//           dhit       - RETURN: ranked domain scores
//           ret_nseq   - RETURN: actual number of seqs searched
//
// Returns:  (void)

static void
    main_loop_serial(struct plan7_s* hmm, const char* seq, int seqLen, struct threshold_s* thresh, int do_forward, int do_null2, int do_xnu, struct histogram_s* histogram, struct tophit_s* ghit, struct tophit_s* dhit, int* /*ret_nseq*/, TaskStateInfo& ti) {
    // get HMMERTaskLocalData
    HMMERTaskLocalData* tld = getHMMERTaskLocalData();
    alphabet_s* al = &tld->al;

    struct dpmatrix_s* mx;  // DP matrix, growable
    struct p7trace_s* tr;  // traceback
    unsigned char* dsq;  // digitized target sequence
    float sc;  // score of an HMM search
    double pvalue;  // pvalue of an HMM score
    double evalue;  // evalue of an HMM score

    // Create a DP matrix; initially only two rows big, but growable;
    // we overalloc by 25 rows (L dimension) when we grow; not growable
    // in model dimension, since we know the hmm size
    mx = CreatePlan7Matrix(1, hmm->M, 25, 0);

    assert(seqLen > 0);

    dsq = DigitizeSequence(seq, seqLen);

    if (do_xnu && al->Alphabet_type == hmmAMINO) {
        XNU(dsq, seqLen);
    }

    // 1. Recover a trace by Viterbi.
    //    In extreme cases, the alignment may be literally impossible;
    //    in which case, the score comes out ridiculously small (but not
    //    necessarily <= -INFTY, because we're not terribly careful
    //    about underflow issues), and tr will be returned as NULL.
    if (P7ViterbiSpaceOK(seqLen, hmm->M, mx)) {
        sc = P7Viterbi(dsq, seqLen, hmm, mx, &tr);
    } else {
        sc = P7SmallViterbi(dsq, seqLen, hmm, mx, &tr, ti.progress);
    }

    // 2. If we're using Forward scores, calculate the
    //    whole sequence score; this overrides anything
    //    PostprocessSignificantHit() is going to do to the per-seq score.
    if (do_forward) {
        sc = P7Forward(dsq, seqLen, hmm, NULL);
        if (do_null2)
            sc -= TraceScoreCorrection(hmm, tr, dsq);
    }

    // 2. Store score/pvalue for global alignment; will sort on score,
    //    which in hmmsearch is monotonic with E-value.
    //    Keep all domains in a significant sequence hit.
    //    We can only make a lower bound estimate of E-value since
    //    we don't know the final value of nseq yet, so the list
    //    of hits we keep in memory is >= the list we actually
    //    output.
    //
    pvalue = PValue(hmm, sc);
    evalue = thresh->Z ? (double)thresh->Z * pvalue : (double)pvalue;
    if (sc >= thresh->globT && evalue <= thresh->globE) {
        sc = PostprocessSignificantHit(ghit, dhit, tr, hmm, dsq, seqLen,
                                       (char*)"sequence",  // todo: sqinfo.name,
                                       NULL,
                                       NULL,
                                       do_forward,
                                       sc,
                                       do_null2,
                                       thresh,
                                       FALSE);  // FALSE-> not hmmpfam mode, hmmsearch mode
    }
    AddToHistogram(histogram, sc);
    P7FreeTrace(tr);
    free(dsq);

    FreePlan7Matrix(mx);
    return;
}

}  // namespace U2
