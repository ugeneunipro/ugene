#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

#include "qscore.h"

double QPair(const MSA_QScore &msaTest, unsigned uTestSeqIndexA, unsigned uTestSeqIndexB,
  const MSA_QScore &msaRef, unsigned uRefSeqIndexA, unsigned uRefSeqIndexB)
	{
	const int iLengthA = (int) msaTest.GetSeqLength(uTestSeqIndexA);
	const int iLengthB = (int) msaTest.GetSeqLength(uTestSeqIndexB);
	const int iLengthAr = (int) msaRef.GetSeqLength(uRefSeqIndexA);
	const int iLengthBr = (int) msaRef.GetSeqLength(uRefSeqIndexB);

	int *iRefMapA = new int[iLengthA];
	int *iRefMapB = new int[iLengthB];
	int *iTestMapA = new int[iLengthA];
	int *iTestMapB = new int [iLengthB];

	msaTest.GetPairMap(uTestSeqIndexA, uTestSeqIndexB, iTestMapA, iTestMapB);
	msaRef.GetPairMap(uRefSeqIndexA, uRefSeqIndexB, iRefMapA, iRefMapB);

	double Q = SumPairs(iRefMapA, iTestMapA, iLengthA);

	delete[] iRefMapA;
	delete[] iRefMapB;
	delete[] iTestMapA;
	delete[] iTestMapB;

	return Q;
	}

double PerSeq(const MSA_QScore &msaTest, const MSA_QScore &msaRef)
	{
	const unsigned uRefSeqCount = msaRef.GetSeqCount();
	const unsigned uTotalPairCount = (uRefSeqCount*(uRefSeqCount - 1))/2;
	unsigned uPairCount = 0;

	vector<vector<double> > QMx(uRefSeqCount);
	for (unsigned uRefSeqIndexA = 0; uRefSeqIndexA < uRefSeqCount; ++uRefSeqIndexA)
		QMx[uRefSeqIndexA].resize(uRefSeqCount);

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

			double Q = QPair(msaTest, uTestSeqIndexA, uTestSeqIndexB, msaRef,
			  uRefSeqIndexA, uRefSeqIndexB);

			QMx[uRefSeqIndexA][uRefSeqIndexB] = Q;
			QMx[uRefSeqIndexB][uRefSeqIndexA] = Q;
			}
		}

	double GrandSumQ = 0;
	for (unsigned uRefSeqIndexA = 0; uRefSeqIndexA < uRefSeqCount; ++uRefSeqIndexA)
		{
		double SumQ = 0;
		for (unsigned uRefSeqIndexB = 0; uRefSeqIndexB < uRefSeqCount; ++uRefSeqIndexB)
			{
			if (uRefSeqIndexB == uRefSeqIndexA)
				continue;
			SumQ += QMx[uRefSeqIndexA][uRefSeqIndexB];
			}
		double Q = SumQ/(uRefSeqCount - 1);
		const char *pstrSeqName = msaRef.GetSeqName(uRefSeqIndexA);
		/*printf("Test=%s;Ref=%s;Seq=%s;Q=%.4f\n",
		  g_TestFileName, g_RefFileName, pstrSeqName, Q);*/
		GrandSumQ += Q;
		}

	double Q = GrandSumQ/uRefSeqCount;
	/*	printf("Test=%s;Ref=%s;AvgQ=%.4f\n",
		  g_TestFileName, g_RefFileName, Q);*/
	return Q;
	}
