#include "uhmmbuild.h"
#include <hmmer2/funcs.h>

#include <QByteArray>
#include <QVector>

#include <U2Core/Task.h>

namespace U2 {

static void maximum_entropy(struct alphabet_s *al, struct plan7_s *hmm, unsigned char **dsq, MSA *msa, float eff_nseq, struct p7prior_s *prior, struct p7trace_s **tr);

// weighting strategy
enum p7_weight {
    WGT_NONE,
    WGT_GSC,
    WGT_BLOSUM,
    WGT_PB,
    WGT_VORONOI,
    WGT_ME
};

plan7_s *UHMMBuild::build(msa_struct *msa, int atype, const UHMMBuildSettings &s, TaskStateInfo &si) {
    p7_construction c_strategy = P7_MAP_CONSTRUCTION;    // construction strategy choice
    p7_weight w_strategy = WGT_GSC;    // weighting strategy
    float blosumlevel = 0.62;    // BLOSUM frac id filtering level [0.62]
    float gapmax = 0.5;    // max frac gaps in mat col for -k
    float archpri = 0.85;    // "architecture" prior on model size
    float swentry = 0.5;    // S/W aggregate entry probability
    float swexit = 0.5;    // S/W aggregate exit probability
    int do_eff = TRUE;    // TRUE to set an effective seq number
    int pbswitch = 1000;    // nseq >= this, switchover to PB weights
    p7trace_s **trace = nullptr;    // fake tracebacks for aseq's
    plan7_s *hmm = nullptr;    //result

    //get HMMERTaskLocalData
    HMMERTaskLocalData *tld = getHMMERTaskLocalData();
    alphabet_s &al = tld->al;

    assert(atype == hmmAMINO || atype == hmmNUCLEIC);
    SetAlphabet(atype);

    // Get alignment(s), build HMMs one at a time

    // Do some initialization the first time through.
    // This code must be delayed until after we've seen the
    // first alignment, because we have to see the alphabet type first

    //Set up Dirichlet priors
    p7prior_s *pri = P7DefaultPrior();

    // Set up the null/random seq model
    float randomseq[MAXABET];    // null sequence model
    float p1;    // null sequence model p1 transition
    P7DefaultNullModel(randomseq, &p1);

    // Prepare unaligned digitized sequences for internal use
    unsigned char **dsq;    // digitized unaligned aseq's
    DigitizeAlignment(msa, &dsq);

    // In some respects we treat DNA more crudely for now;
    // for example, we can't do eff seq #, because it's
    // calibrated for protein.

    if (al.Alphabet_type == hmmNUCLEIC) {
        do_eff = FALSE;
    }

    // Determine "effective sequence number".
    // The BlosumWeights() routine is now an efficient O(N)
    // memory clustering algorithm that doesn't blow up on,
    // say, Pfam's GP120 alignment (13000+ sequences)

    float eff_nseq = (float)msa->nseq;
    if (do_eff) {
        QVector<float> wgt(msa->nseq, 0);
        BlosumWeights(msa->aseq, msa->nseq, msa->alen, blosumlevel, wgt.data());
        eff_nseq = FSum(wgt.data(), msa->nseq);
    }

    // Weight the sequences (optional),

    if (w_strategy == WGT_GSC || w_strategy == WGT_BLOSUM || w_strategy == WGT_VORONOI || w_strategy == WGT_PB) {
        if (w_strategy != WGT_PB && msa->nseq >= pbswitch) {
            PositionBasedWeights(msa->aseq, msa->nseq, msa->alen, msa->wgt);
        } else if (w_strategy == WGT_GSC) {
            GSCWeights(msa->aseq, msa->nseq, msa->alen, msa->wgt);
        } else if (w_strategy == WGT_BLOSUM) {
            BlosumWeights(msa->aseq, msa->nseq, msa->alen, blosumlevel, msa->wgt);
        } else if (w_strategy == WGT_PB) {
            PositionBasedWeights(msa->aseq, msa->nseq, msa->alen, msa->wgt);
        } else if (w_strategy == WGT_VORONOI) {
            VoronoiWeights(msa->aseq, msa->nseq, msa->alen, msa->wgt);
        }
    }

    //Set the effective sequence number (if do_eff is FALSE, eff_nseq was set to nseq).

    FNorm(msa->wgt, msa->nseq);
    FScale(msa->wgt, msa->nseq, eff_nseq);

    // Build a model architecture.
    // If we're not doing MD or ME, that's all we need to do.
    // We get an allocated, counts-based HMM back.

    // Because the architecture algorithms are allowed to change
    // gap characters in the alignment, we have to calculate the
    // alignment checksum before we enter the algorithms.

    int checksum = GCGMultchecksum(msa->aseq, msa->nseq);    // checksum of the alignment
    if (c_strategy == P7_FAST_CONSTRUCTION) {
        P7Fastmodelmaker(msa, dsq, gapmax, &hmm, &trace);
    } else if (c_strategy == P7_HAND_CONSTRUCTION) {
        P7Handmodelmaker(msa, dsq, &hmm, &trace);
    } else {
        P7Maxmodelmaker(msa, dsq, gapmax, pri, randomseq, p1, archpri, &hmm, &trace);
    }
    hmm->checksum = checksum;
    hmm->atype = atype;

    // Record the null model in the HMM;
    // add prior contributions in pseudocounts and renormalize.

    Plan7SetNullModel(hmm, randomseq, p1);
    P7PriorifyHMM(hmm, pri);

    // Model configuration, temporary.
    // hmmbuild assumes that it's given an alignment of single domains,
    // and the alignment may contain fragments. So, for the purpose of
    // scoring the sequences (or, optionally, MD/ME weighting),
    // configure the model into hmmsw mode. Later we'll
    // configure the model according to how the user wants to
    // use it.

    Plan7SWConfig(hmm, 0.5, 0.5);

    // Do model-dependent "weighting" strategies.

    if (w_strategy == WGT_ME) {
        maximum_entropy(&al, hmm, dsq, msa, eff_nseq, pri, trace);
    }

    // Give the model a name.
    QString name = s.name;
    if (name.isEmpty()) {
        name = msa->name;
    }
    QByteArray nameArr = name.toLatin1();
    Plan7SetName(hmm, nameArr.constData());

    // Transfer other information from the alignment to
    // the HMM. This typically only works for Stockholm or SELEX format
    //  alignments, so these things are conditional/optional.

    if (msa->acc != nullptr)
        Plan7SetAccession(hmm, msa->acc);
    if (msa->desc != nullptr)
        Plan7SetDescription(hmm, msa->desc);

    if (msa->cutoff_is_set[MSA_CUTOFF_GA1] && msa->cutoff_is_set[MSA_CUTOFF_GA2]) {
        hmm->flags |= PLAN7_GA;
        hmm->ga1 = msa->cutoff[MSA_CUTOFF_GA1];
        hmm->ga2 = msa->cutoff[MSA_CUTOFF_GA2];
    }
    if (msa->cutoff_is_set[MSA_CUTOFF_TC1] && msa->cutoff_is_set[MSA_CUTOFF_TC2]) {
        hmm->flags |= PLAN7_TC;
        hmm->tc1 = msa->cutoff[MSA_CUTOFF_TC1];
        hmm->tc2 = msa->cutoff[MSA_CUTOFF_TC2];
    }
    if (msa->cutoff_is_set[MSA_CUTOFF_NC1] && msa->cutoff_is_set[MSA_CUTOFF_NC2]) {
        hmm->flags |= PLAN7_NC;
        hmm->nc1 = msa->cutoff[MSA_CUTOFF_NC1];
        hmm->nc2 = msa->cutoff[MSA_CUTOFF_NC2];
    }

    // Record some other miscellaneous information in the HMM,
    // like how/when we built it.
    Plan7SetCtime(hmm);
    hmm->nseq = msa->nseq;

    // Configure the model for chosen algorithm
    switch (s.strategy) {
        case P7_BASE_CONFIG:
            Plan7GlobalConfig(hmm);
            break;
        case P7_SW_CONFIG:
            Plan7SWConfig(hmm, swentry, swexit);
            break;
        case P7_LS_CONFIG:
            Plan7LSConfig(hmm);
            break;
        case P7_FS_CONFIG:
            Plan7FSConfig(hmm, swentry, swexit);
            break;
        default:
            si.setError(tr("bogus configuration choice"));
            break;
    }

    // Clean up trace
    for (int idx = 0; idx < msa->nseq; idx++) {
        P7FreeTrace(trace[idx]);
    }
    free(trace);
    Free2DArray((void **)dsq, msa->nseq);

    P7FreePrior(pri);
    return hmm;
}

// Function: position_average_score()
// Date:     Wed Dec 31 09:36:35 1997 [StL]
//
// Purpose:  Calculate scores from tracebacks, keeping them
//           in a position specific array. The final array
//           is normalized position-specifically too, according
//           to how many sequences contributed data to this
//           position. Used for compensating for sequence
//           fragments in ME and MD score optimization.
//           Very much ad hoc.
//
//           Code related to (derived from) TraceScore().
//
// Args:     hmm       - HMM structure, scores valid
//           dsq       - digitized unaligned sequences
//           wgt       - weights on the sequences
//           nseq      - number of sequences
//           tr        - array of nseq tracebacks that aligns each dsq to hmm
//           pernode   - RETURN: [0]1..M array of position-specific avg scores
//           ret_avg   - RETURN: overall average full-length, one-domain score
//
// Return:   1 on success, 0 on failure.
//           pernode is MallocOrDie'ed [0]1..M by CALLER and filled here.

static void position_average_score(struct plan7_s *hmm, unsigned char **dsq, float *wgt, int nseq, struct p7trace_s **tr, float *pernode, float *ret_avg) {
    unsigned char sym;
//    int pos;    //position in seq
    int tpos;    // position in trace/state sequence
    float *counts;    //counts at each position
    float avg;    // RETURN: average overall
    int k;    // counter for model position
    int idx;    // counter for sequence number

    // Allocations
    counts = (float *)MallocOrDie((hmm->M + 1) * sizeof(float));
    FSet(pernode, hmm->M + 1, 0.);
    FSet(counts, hmm->M + 1, 0.);

    // Loop over traces, accumulate weighted scores per position
    for (idx = 0; idx < nseq; idx++) {
        for (tpos = 0; tpos < tr[idx]->tlen; tpos++) {
//            pos = tr[idx]->pos[tpos];
            sym = dsq[idx][tr[idx]->pos[tpos]];
            k = tr[idx]->nodeidx[tpos];

            // Counts: how many times did we use this model position 1..M?(weighted)
            if (tr[idx]->statetype[tpos] == STM || tr[idx]->statetype[tpos] == STD) {
                counts[k] += wgt[idx];
            }

            // Emission scores.
            if (tr[idx]->statetype[tpos] == STM) {
                pernode[k] += wgt[idx] * Scorify(hmm->msc[sym][k]);
            } else if (tr[idx]->statetype[tpos] == STI) {
                pernode[k] += wgt[idx] * Scorify(hmm->isc[sym][k]);
            }

            // Transition scores.
            if (tr[idx]->statetype[tpos] == STM ||
                tr[idx]->statetype[tpos] == STD ||
                tr[idx]->statetype[tpos] == STI) {
                pernode[k] += wgt[idx] *
                              Scorify(TransitionScoreLookup(hmm, tr[idx]->statetype[tpos], tr[idx]->nodeidx[tpos], tr[idx]->statetype[tpos + 1], tr[idx]->nodeidx[tpos + 1]));
            }
        }
    }

    // Divide accumulated scores by accumulated weighted counts
    avg = 0.;
    for (k = 1; k <= hmm->M; k++) {
        pernode[k] /= counts[k];
        avg += pernode[k];
    }

    free(counts);
    *ret_avg = avg;
}

// Function: frag_trace_score()
// Date:     SRE, Wed Dec 31 10:03:47 1997 [StL]
//
// Purpose:  Allow MD/ME optimization to be used for alignments
//           that include fragments and multihits -- estimate a full-length
//           per-domain score.
//
//
//
// Return:   "corrected" score.

static float frag_trace_score(struct plan7_s *hmm, unsigned char *dsq, struct p7trace_s *tr, float *pernode, float expected) {
    float sc;    // corrected score
    float fragexp;    // expected score for a trace like this
    int tpos;    // position in trace */

    // get uncorrected score
    sc = P7TraceScore(hmm, dsq, tr);

    // calc expected score for trace like this
    fragexp = 0.;
    for (tpos = 0; tpos < tr->tlen; tpos++) {
        if (tr->statetype[tpos] == STM || tr->statetype[tpos] == STD) {
            fragexp += pernode[tr->nodeidx[tpos]];
        }
    }

    // correct for multihits
    fragexp /= (float)TraceDomainNumber(tr);

    // extrapolate to full-length, one-hit score
    sc = sc * expected / fragexp;
    return sc;
}

// Function: maximum_entropy()
// Date:     SRE, Fri Jan  2 10:56:00 1998 [StL]
//
// Purpose:  Optimizes a model according to maximum entropy weighting.
//           See Krogh and Mitchison (1995).
//
//           [Actually, we do minimum relative entropy, rather than
//           maximum entropy. Same thing, though we refer to "ME"
//           weights and models. The optimization is a steepest
//           descents minimization of the relative entropy.]
//
//           Expects to be called shortly after a Maxmodelmaker()
//           or Handmodelmaker(), so that both a new model architecture
//           (with MAP parameters) and fake tracebacks are available.
//
//           Prints a summary of optimization progress to stdout.
//
// Args:     hmm     - model. allocated, set with initial MAP parameters.
//           dsq     - dealigned digitized seqs the model is based on
//           ainfo   - extra info for aseqs
//           nseq    - number of aseqs
//           eff_nseq- effective sequence number; weights normalize up to this.
//           prior   - prior distributions for parameterizing model
//           tr      - array of fake traces for each sequence
//
// Return:   (void)
//           hmm changed to an ME HMM
//           ainfo changed, contains ME weights

static void maximum_entropy(struct alphabet_s * /*al*/, struct plan7_s *hmm, unsigned char **dsq, MSA *msa, float eff_nseq, struct p7prior_s *prior, struct p7trace_s **tr) {
    float *wgt;    // current best set of ME weights
    float *new_wgt;    // new set of ME weights to try
    float *sc;    // log-odds score of each sequence
    float *grad;    // gradient
    float epsilon;    // steepness of descent
    float relative_entropy;    // current best relative entropy
    float new_entropy;    // relative entropy at new weights
    float last_new_entropy;    // last new_entropy we calc'ed
    float use_epsilon;    // current epsilon value in use
    int idx;    // counter over sequences
    int i1, i2;    // counters for iterations

    float converge_criterion;
    float minw, maxw;    // min, max weight
    int posw, highw;    // number of positive weights
    float mins, maxs, avgs;    // min, max, avg score
    float *pernode;    // expected score per node of HMM
    float expscore;    // expected score of complete HMM
    int max_iter;    // bulletproof against infinite loop bugs

    epsilon = 0.2;    // works fine
    max_iter = 666;

    // Allocations
    sc = (float *)MallocOrDie(sizeof(float) * msa->nseq);
    wgt = (float *)MallocOrDie(sizeof(float) * msa->nseq);
    new_wgt = (float *)MallocOrDie(sizeof(float) * msa->nseq);
    grad = (float *)MallocOrDie(sizeof(float) * msa->nseq);
    pernode = (float *)MallocOrDie(sizeof(float) * (hmm->M + 1));

    // Initialization. Start with all weights == 1.0.
    // Find relative entropy and gradient.
    Plan7SWConfig(hmm, 0.5, 0.5);
    P7Logoddsify(hmm, TRUE);

    FSet(wgt, msa->nseq, 1.0);
    position_average_score(hmm, dsq, wgt, msa->nseq, tr, pernode, &expscore);
    for (idx = 0; idx < msa->nseq; idx++)
        sc[idx] = frag_trace_score(hmm, dsq[idx], tr[idx], pernode, expscore);
    relative_entropy = FSum(sc, msa->nseq) / (float)msa->nseq;
    for (idx = 0; idx < msa->nseq; idx++)
        grad[idx] = relative_entropy - sc[idx];

    // Steepest descents optimization;
    // iterate until relative entropy converges.
    i1 = 0;
    while (++i1 < max_iter) {
        // Gradient gives us a line of steepest descents.
        // (Roughly speaking, anyway. We actually have a constraint
        // that weights are nonnegative and normalized, and the
        // gradient doesn't take these into account.)
        // Look along this line, a distance of epsilon * gradient:
        // if new point is better, accept; if new point is worse,
        // move back along the line by half the distance and re-evaluate.

        use_epsilon = epsilon;
        new_entropy = relative_entropy + 1.0; /* just ensure new > old */

        i2 = 0;
        while (new_entropy > relative_entropy && ++i2 < max_iter) {
            last_new_entropy = new_entropy;

            // find a new point in weight space */
            for (idx = 0; idx < msa->nseq; idx++) {
                new_wgt[idx] = wgt[idx] + use_epsilon * grad[idx];
                if (new_wgt[idx] < 0.)
                    new_wgt[idx] = 0.0;
            }
            FNorm(new_wgt, msa->nseq);
            FScale(new_wgt, msa->nseq, (float)msa->nseq);

            // Make new HMM using these weights
            ZeroPlan7(hmm);
            for (idx = 0; idx < msa->nseq; idx++) {
                P7TraceCount(hmm, dsq[idx], new_wgt[idx], tr[idx]);
            }
            P7PriorifyHMM(hmm, prior);

            // Evaluate new point
            Plan7SWConfig(hmm, 0.5, 0.5);
            P7Logoddsify(hmm, TRUE);
            position_average_score(hmm, dsq, new_wgt, msa->nseq, tr, pernode, &expscore);
            for (idx = 0; idx < msa->nseq; idx++) {
                sc[idx] = frag_trace_score(hmm, dsq[idx], tr[idx], pernode, expscore);
            }
            new_entropy = FDot(sc, new_wgt, msa->nseq) / (float)msa->nseq;

            use_epsilon /= 2.0;
            // Failsafe: we're not converging. Set epsilon to zero,
            // do one more round.
            if (use_epsilon < 1e-6)
                use_epsilon = 0.0;
            if (use_epsilon == 0.0)
                break;

            // Failsafe: avoid infinite loops. Sometimes the
            // new entropy converges without ever being better
            // than the previous point, probably as a result
            // of minor roundoff error
            if (last_new_entropy == new_entropy)
                break;
        }

        // Evaluate convergence before accepting the new weights;
        // then, accept the new point and evaluate the gradient there.
        converge_criterion = qAbs((relative_entropy - new_entropy) / relative_entropy);
        relative_entropy = new_entropy;
        FCopy(wgt, new_wgt, msa->nseq);
        for (idx = 0; idx < msa->nseq; idx++) {
            grad[idx] = relative_entropy - sc[idx];
        }

        // Print some statistics about this iteration
        mins = maxs = avgs = sc[0];
        minw = maxw = wgt[0];
        posw = (wgt[0] > 0.0) ? 1 : 0;
        highw = (wgt[0] > 1.0) ? 1 : 0;
        for (idx = 1; idx < msa->nseq; idx++) {
            if (sc[idx] < mins)
                mins = sc[idx];
            if (sc[idx] > maxs)
                maxs = sc[idx];
            if (wgt[idx] < minw)
                minw = wgt[idx];
            if (wgt[idx] > maxw)
                maxw = wgt[idx];
            if (wgt[idx] > 0.0)
                posw++;
            if (wgt[idx] > 1.0)
                highw++;
            avgs += sc[idx];
        }
        if (converge_criterion < 1e-5)
            break;
    }

    // Renormalize weights to sum to eff_nseq, and save.
    FNorm(wgt, msa->nseq);
    FScale(wgt, msa->nseq, (float)eff_nseq);
    FCopy(msa->wgt, wgt, msa->nseq);
    // Make final HMM using these adjusted weights */
    ZeroPlan7(hmm);
    for (idx = 0; idx < msa->nseq; idx++) {
        P7TraceCount(hmm, dsq[idx], wgt[idx], tr[idx]);
    }
    P7PriorifyHMM(hmm, prior);

    // Cleanup and return
    free(pernode);
    free(new_wgt);
    free(grad);
    free(wgt);
    free(sc);
}

}    // namespace U2
