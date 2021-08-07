#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

#include "qscore.h"

//unsigned g_TestSeqIndexA = UINT_MAX;
//unsigned g_TestSeqIndexB = UINT_MAX;
//unsigned g_RefSeqIndexA = UINT_MAX;
//unsigned g_RefSeqIndexB = UINT_MAX;

void ComparePair(const MSA_QScore &msaTest, unsigned uTestSeqIndexA,
  unsigned uTestSeqIndexB, const MSA_QScore &msaRef, unsigned uRefSeqIndexA,
  unsigned uRefSeqIndexB, double *ptrdSP, double *ptrdPS, double *ptrdCS)
	{
	const int iLengthA = (int) msaTest.GetSeqLength(uTestSeqIndexA);
	const int iLengthB = (int) msaTest.GetSeqLength(uTestSeqIndexB);
	const int iLengthAr = (int) msaRef.GetSeqLength(uRefSeqIndexA);
	const int iLengthBr = (int) msaRef.GetSeqLength(uRefSeqIndexB);

	if (iLengthA != iLengthAr)
		Quit_Qscore("Seq %s has %u letters in test, %u letters in ref",
		  msaTest.GetSeqName(uTestSeqIndexA), iLengthA, iLengthAr);
	if (iLengthB != iLengthBr)
		Quit_Qscore("Seq %s has %u letters in test, %u letters in ref",
		  msaTest.GetSeqName(uTestSeqIndexB), iLengthB, iLengthBr);

	int *iRefMapA = new int[iLengthA];
	int *iRefMapB = new int[iLengthB];
	int *iTestMapA = new int[iLengthA];
	int *iTestMapB = new int [iLengthB];

	msaTest.GetPairMap(uTestSeqIndexA, uTestSeqIndexB, iTestMapA, iTestMapB);
	msaRef.GetPairMap(uRefSeqIndexA, uRefSeqIndexB, iRefMapA, iRefMapB);

	ComparePairMap(iTestMapA, iTestMapB, iRefMapA, iRefMapB, iLengthA, iLengthB,
	  ptrdSP, ptrdPS, ptrdCS);

	delete[] iRefMapA;
	delete[] iRefMapB;
	delete[] iTestMapA;
	delete[] iTestMapB;
	}
