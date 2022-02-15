#ifndef __HMMSEARCH_SSE_H__
#define __HMMSEARCH_SSE_H__

QList<float> sseScoring(unsigned char* dsq, int seqlen, plan7_s* hmm, HMMSeqGranulation* gr, U2::TaskStateInfo& ti);

#endif  // __HMMSEARCH_SSE_H__
