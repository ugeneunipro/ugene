#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

#include "qscore.h"

void CompareMSA(const MSA_QScore &msaTest, const MSA_QScore &msaRef, double *ptrdSP,
  double *ptrdPS, double *ptrdCS)
	{
	const unsigned uRefSeqCount = msaRef.GetSeqCount();

	double dTotalSP = 0.0;
	double dTotalPS = 0.0;
	double dTotalCS = 0.0;
	unsigned uPairCount = 0;
	//const QScoreContext *ctx = getQScoreContext();

	const unsigned uTotalPairCount = (uRefSeqCount*(uRefSeqCount - 1))/2;
	for (unsigned uRefSeqIndexA = 0; uRefSeqIndexA < uRefSeqCount; ++uRefSeqIndexA)
		{
		const char *pstrSeqNameA = msaRef.GetSeqName(uRefSeqIndexA);
		unsigned uTestSeqIndexA;
		bool bFound = msaTest.GetSeqIndex(pstrSeqNameA, &uTestSeqIndexA);
		if (!bFound)
			{
			Quit_Qscore("Sequence '%s' not found in test alignment", pstrSeqNameA);
			continue;
			}

		for (unsigned uRefSeqIndexB = uRefSeqIndexA + 1; uRefSeqIndexB < uRefSeqCount;
		  ++uRefSeqIndexB)
			{
			/*if (!ctx->g_Quiet && uPairCount%100 == 0)
				{
				double Pct = uPairCount*100.0/uTotalPairCount;
				k_printf( "%u seqs; %u of %u pairs (%.1f%%)\r",
				  uRefSeqCount, uPairCount, uTotalPairCount, Pct);
				}*/
			const char *pstrSeqNameB = msaRef.GetSeqName(uRefSeqIndexB);
			unsigned uTestSeqIndexB;
			bool bFound = msaTest.GetSeqIndex(pstrSeqNameB, &uTestSeqIndexB);
			if (!bFound)
				{
				Quit_Qscore("Sequence '%s' not found in test alignment", pstrSeqNameA);
				continue;
				}

			double dSP = dInsane;
			double dPS = dInsane;
			double dCS = dInsane;
			
			ComparePair(msaTest, uTestSeqIndexA, uTestSeqIndexB, msaRef, uRefSeqIndexA,
			  uRefSeqIndexB, &dSP, &dPS, &dCS);

			dTotalSP += dSP;
			dTotalPS += dPS;
			dTotalCS += dCS;
			++uPairCount;
			}
		}
	if (0 == uPairCount)
		{
		Quit_Qscore("No sequence pairs in common between test and reference alignment");
		*ptrdSP = 0;
		*ptrdPS = 0;
		*ptrdCS = 0;
		return;
		}
	/*if (!ctx->g_Quiet)
		k_printf( "%u seqs; %u of %u pairs (100%%)\n",
		  uRefSeqCount, uTotalPairCount, uTotalPairCount);*/

	*ptrdSP = dTotalSP / uPairCount;
	*ptrdPS = dTotalPS / uPairCount;
	*ptrdCS = dTotalCS / uPairCount;
	}
