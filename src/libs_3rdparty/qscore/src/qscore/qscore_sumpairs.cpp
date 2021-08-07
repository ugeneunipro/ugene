#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

#include "qscore.h"

// Compute the sum of pairs score from two pair maps.
// It is the simplicity of this code that motivates the use of pair maps.
double SumPairs(const int iMapRef[], const int iMapTest[], unsigned uLength)
	{
	unsigned uPairCount = 0;
	unsigned uCorrectPairCount = 0;
	for (unsigned uPos = 0; uPos < uLength; ++uPos)
		{
		int iPosRef = iMapRef[uPos];
		if (-1 == iPosRef)
			continue;

		++uPairCount;

		int iPosTest = iMapTest[uPos];
		if (-1 == iPosTest)
			continue;

		if (iPosRef == iPosTest)
			++uCorrectPairCount;
		}

	if (0 == uPairCount)
		return 0.0;

	return (double) uCorrectPairCount / (double) uPairCount;
	}
