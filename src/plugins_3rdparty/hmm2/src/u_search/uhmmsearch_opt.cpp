#include "uhmmsearch_opt.h"
#include "uhmmsearch.h"

#include <U2Core/Task.h>
#include <U2Core/Log.h>

#include <hmmer2/funcs.h>
#include <hmmer2/structs.h>
#include <HMMIO.h>

#include <limits.h>
#include <algorithm>

#define SEQ_ALIGN_BASE 16

#define ALIGNED(ptr, base) ( (unsigned char*) ( (((quintptr)(ptr))+((base)-1)) &~((base-1)) ) )
#define ALIGNED_16(ptr)    ( ALIGNED(ptr, 16) )

namespace {
//returns aligned pointer.
unsigned char * DigitizeSequenceHP( const unsigned char * seq, int L, const unsigned char * mem, int align_base )
{
    //Digitized sequences should be aligned for easier DMA transfer
    unsigned char * dsq = ALIGNED( mem, align_base );
    int i;

    HMMERTaskLocalData *tld = getHMMERTaskLocalData();
    alphabet_s *al = &tld->al;

    dsq[0] = dsq[L+1] = (unsigned char) al->Alphabet_iupac;
    for (i = 1; i <= L+1; i++) {
        dsq[i] = SymbolIndex(seq[i-1]);
    }
    return dsq;
}

} //anonymous namespace

void main_loop_opt( struct plan7_s * hmm_, const char * seq_, int seqlen, struct threshold_s *thresh, int do_forward,
                    int do_null2, int do_xnu, struct histogram_s * histogram, struct tophit_s * ghit, struct tophit_s * dhit, 
                    int * ret_nseq, U2::TaskStateInfo & ti, hmmScoringFunction scoring_f )
{
    using namespace U2;
    Q_UNUSED( ret_nseq );
    
    HMMERTaskLocalData *tld = getHMMERTaskLocalData();
    alphabet_s *al = &tld->al;
    plan7_s * hmm = HMMIO::cloneHMM( hmm_ );
    
    unsigned char * seq = (unsigned char *)seq_;
    unsigned char * dsq_mem = new unsigned char[seqlen + SEQ_ALIGN_BASE];
    unsigned char * dsq = DigitizeSequenceHP( seq, seqlen, dsq_mem, SEQ_ALIGN_BASE );
    
    if (do_xnu && al->Alphabet_type == hmmAMINO) {
        XNU(dsq, seqlen);
    }
    
    struct dpmatrix_s * mx;               
    struct p7trace_s * tr;                
    
    HMMSeqGranulation gr;
    //Scoring function splits the sequence and computes a score for each chunk.
    //Resulting granulation is returned in 'gr' parameter
    QList<float> results = scoring_f( dsq, seqlen, hmm, &gr, ti );
    
    mx = CreatePlan7Matrix( 1, hmm->M, 25, 0 );
    
    const QVector<U2Region> & regions = gr.regions;
    QList<int> resultsCache; //stores indexes in dhit->unsrt
    QList<int> borderResults; //stores indexes of 'border results'
    int last_hit_index = 0; //index in dhit->unsrt
     
    for( int i = 0, sz = results.size(); i < sz; ++i ){
        unsigned char * curSeqAddr = dsq + regions.at(i).startPos;
        int curSeqLen = regions.at(i).length;
        int overlap = gr.overlap;

        float sc = results.at(i);
        double pvalue = PValue(hmm, sc);
        double evalue = thresh->Z ? (double) thresh->Z * pvalue : (double) pvalue;

        if ( sc >= thresh->domT && evalue <= thresh->domE )  {
            float conservative_sc = 0.0f;

            // This sequence needs traceback computation.
            if (P7ViterbiSpaceOK(curSeqLen, hmm->M, mx)){
                conservative_sc = P7Viterbi( curSeqAddr, curSeqLen, hmm, mx, &tr);
            } else {
                int progress; //stub
                conservative_sc = P7SmallViterbi( curSeqAddr, curSeqLen, hmm, mx, &tr, progress);
            }
            if (do_forward) {
                conservative_sc  = P7Forward( curSeqAddr, curSeqLen, hmm, NULL);
                if (do_null2) {
                    conservative_sc -= TraceScoreCorrection( hmm, tr, curSeqAddr );
                }
            }
//             if( sc != conservative_sc ) {
//                 hmm_log.trace( QString("Optimized HMMER reports error: sc: %1, ppe_sc: %2, diff: %3").
//                     arg( QString().setNum(sc), QString().setNum(conservative_sc), QString().setNum(sc - conservative_sc) ) );
//             } else {
//                 hmm_log.trace( QString("Optimized HMMER reports OK: sc: %1").arg(QString().setNum(sc)) );
//             }

            sc = conservative_sc; //using more precise score
            pvalue = PValue(hmm, sc);
            evalue = thresh->Z ? (double) thresh->Z * pvalue : pvalue;

            if ( sc >= thresh->domT && evalue <= thresh->domE ) {
                // This function adds new results to dhit->unsrt. They are added right after 'last_hit_index'
                sc = PostprocessSignificantHit( ghit, dhit, tr, hmm, curSeqAddr, curSeqLen,  (char *)"seq", 0, 0, do_forward, sc, do_null2, thresh, FALSE );

                //shifting results 
                for( int hi = last_hit_index; hi < dhit->num; ++hi ) {
                    const U2Region & curReg = regions.at(i);
                    int & from = dhit->unsrt[hi].sqfrom;
                    int & to   = dhit->unsrt[hi].sqto; 

                    bool leftBorderResult = (i && !from);
                    bool rightBorderResult = ((i != sz-1) && (to == curReg.length));
                    bool borderResult =  leftBorderResult || rightBorderResult;
                    //preparing to correct merging 
                    if( (i && from < overlap) || ((i != sz-1) && to > curReg.length - overlap) ) {
                        resultsCache.push_back(hi);
                        //'border result'
                        if( borderResult )  {
                            borderResults.push_back(hi);
                        }
                    }
                    //shifting results to correct position in global sequence
                    from += curReg.startPos;
                    to   += curReg.startPos;
                }
                last_hit_index = dhit->num;
            }
            P7FreeTrace(tr);
        }
        AddToHistogram(histogram, sc);
    }

    //merging cached results
    //adopted from HMMSearchTask::report
    //NOTE: results are shifted
    int maxCommonLen = hmm->M / 2; //hust heuristic
    QList<int> resultsValid;
    QList<int> resultsToSkip;
    
    //Search for results which will be skipped
    for( int i = 0, endi = resultsCache.size(); i < endi; ++i ) {
        //NOTE: i, j are indexes in resulsCache, but res_i and res_j are indexes in dhit->unsrt
        int res_i = resultsCache.at(i);
        if( resultsToSkip.contains(res_i) ) {
            continue;
        }
        hit_s & hit1 = dhit->unsrt[ res_i ];
        U2Region r1( hit1.sqfrom, hit1.sqto - hit1.sqfrom );

        for( int j = i+1, endj = resultsCache.size(); j < endj; ++j ) {
            int res_j = resultsCache.at(j);
            if( resultsToSkip.contains(res_j) ) {
                continue;
            }
            hit_s & hit2 = dhit->unsrt[ res_j ];
            U2Region r2( hit2.sqfrom, hit2.sqto - hit2.sqfrom );

            if( r1.contains(r2) ) {
                resultsToSkip.push_back(res_j);
            } 
            else if( r2.contains(r1) ) {
                resultsToSkip.push_back(res_i);
                break;
            }
            else if( r1.intersect(r2).length >= maxCommonLen ) {
                 bool useR1 = hit2.score <= hit1.score;

                 if (hit1.score == hit2.score && hit1.pvalue == hit2.pvalue && borderResults.contains(res_i) && !borderResults.contains(res_j) ) {
                     useR1 = false;
                 }
                 if (useR1) {
                     resultsToSkip.push_back(res_i);
                 } else {
                     resultsToSkip.push_back(res_j);
                 }
            }
        }
    }
    //Skipping unneeded results
    foreach( int cached, resultsCache ) {
        if( !resultsToSkip.contains(cached) ) {
            resultsValid.push_back( cached );
        }
    }
    //Freeing resources of unneeded results
    foreach( int i, resultsToSkip ) {
        assert( !resultsValid.contains(i) );
        hit_s & hit = dhit->unsrt[i];
        FreeFancyAli( hit.ali );
        hit.ali = NULL;
        free(hit.name);
        free(hit.acc);
        free(hit.desc);
    }

    //ugly hack: directly modifying dhit members
    if( resultsValid.size() != dhit->num ) {
        //copying (a)non-cached results, (b)cached and filtered results
        assert( resultsCache.size() >= resultsValid.size() );
        int total = dhit->num - resultsCache.size() + resultsValid.size();
        hit_s * newHits = (hit_s *)MallocOrDie( total * sizeof(hit_s) );
        //non-cached:
        int count = 0;
        for( int i = 0; i < dhit->num; ++i ) {
            if( !resultsCache.contains(i) ) {
                newHits[count++] = dhit->unsrt[i];
            }
        }
        //cached:
        foreach( int vali, resultsValid ) {
            newHits[count++] = dhit->unsrt[vali];
        }
        assert( count == total );
        dhit->num = total;
        dhit->alloc = dhit->num;

        free( dhit->unsrt );
        dhit->unsrt = newHits;
        assert( 0 == dhit->hit );
    }

    FreePlan7Matrix(mx);
    FreePlan7( hmm );
    delete[] dsq_mem;
}

