#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

#include "qscore.h"

// Compute the Cline shift score [1] from two pair maps.
// It is the relative simplicity of this code that motivates the use of pair maps.
// [1] Cline, Hughey & Karplus (2002), Bioinformatics 18(2) p.306.
double ClineShift(const int iTestMapA[], const int iRefMapA[], unsigned uLengthA,
  const int iTestMapB[], const int iRefMapB[], unsigned uLengthB, double dEpsilon)
	{
	unsigned uRefPairCount = 0;
	unsigned uTestPairCount = 0;
	double dTotal = 0.0;

	for (unsigned uPosA = 0; uPosA < uLengthA; ++uPosA)
		{
		int iRefPosB = iRefMapA[uPosA];
		if (-1 == iRefPosB)
			continue;

		++uRefPairCount;

		int iTestPosB = iTestMapA[uPosA];
		if (-1 == iTestPosB)
			continue;

		int iShift = iabs(iRefPosB - iTestPosB);
		double dScore = (1 + dEpsilon)/(1 + iShift) - dEpsilon;
		assert(dScore >= -dEpsilon);
		assert(dScore <= 1.0);

		dTotal += dScore;
		}

	for (unsigned uPosB = 0; uPosB < uLengthB; ++uPosB)
		{
		int iTestPosA = iTestMapB[uPosB];
		if (-1 == iTestPosA)
			continue;

		++uTestPairCount;

		int iRefPosA = iRefMapB[uPosB];
		if (-1 == iRefPosA)
			continue;

		int iShift = iabs(iRefPosA - iTestPosA);
		double dScore = (1 + dEpsilon)/(1 + iShift) - dEpsilon;
		assert(dScore >= -dEpsilon);
		assert(dScore <= 1.0);

		dTotal += dScore;
		}

	if (0 == uRefPairCount)
		{
		//DumpRefPair();
		//Quit_Qscore("ClineShift: No aligned pair in ref alignment");
		return 0.0;
		}

	return dTotal / (double) (uTestPairCount + uRefPairCount);
	}
