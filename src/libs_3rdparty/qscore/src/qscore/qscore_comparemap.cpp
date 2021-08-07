#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

#include "qscore.h"

#if	_DEBUG
static bool LocalEq(double x, double y)
	{
	double diff = fabs(x-y);
	return diff < 1e-3;
	}

double ComparePairMapSP(const int iTestMapA[], const int iTestMapB[],
  const int iRefMapA[], const int iRefMapB[], int iLengthA, int iLengthB)
#else
double ComparePairMapSP(const int iTestMapA[], const int /*iTestMapB[]*/,
  const int iRefMapA[], const int /*iRefMapB[]*/, int iLengthA, int /*iLengthB*/)
#endif
	{
	double dSP = SumPairs(iRefMapA, iTestMapA, iLengthA);

#if	_DEBUG
	{
// Verify symmetry as a correctness check
	double dSP2 = SumPairs(iRefMapB, iTestMapB, iLengthB);
	if (!LocalEq(dSP, dSP2))
		Quit_Qscore("ComparePairMapSP: dSP=%g dSP2=%g diff=%g", dSP, dSP2, fabs(dSP-dSP2));
	}
#endif

	return dSP;
	}

void ComparePairMap(const int iTestMapA[], const int iTestMapB[],
  const int iRefMapA[], const int iRefMapB[], int iLengthA, int iLengthB,
  double *ptrdSP, double *ptrdPS, double *ptrdCS)
	{
	double dSP = SumPairs(iRefMapA, iTestMapA, iLengthA);
	double dPS = SumPairs(iTestMapA, iRefMapA, iLengthA);

	double dCS = -999.0;
	/*if (ctx->g_Cline)
		dCS = ClineShift(iTestMapA, iRefMapA, iLengthA, iTestMapB, iRefMapB, iLengthB);*/

#if	_DEBUG
	{
// Verify symmetries as a correctness check
	double dSP2 = SumPairs(iRefMapB, iTestMapB, iLengthB);
	double dPS2 = SumPairs(iTestMapB, iRefMapB, iLengthB);
	if (!LocalEq(dSP, dSP2))
		Quit_Qscore("CompareSeqs: dSP=%g dSP2=%g diff=%g", dSP, dSP2, fabs(dSP-dSP2));
	if (!LocalEq(dPS, dPS2))
		Quit_Qscore("CompareSeqs: dPS=%g dPS2=%g diff=%g", dPS, dPS2, fabs(dPS-dPS2));

	/*if (ctx->g_Cline)
		{
		double dCS2 = ClineShift(iTestMapB, iRefMapB, iLengthB, iTestMapA,
		  iRefMapA, iLengthA);
		double dCS3 = ClineShift(iRefMapA, iTestMapA, iLengthA, iRefMapB,
		  iTestMapB, iLengthB);
		double dCS4 = ClineShift(iRefMapB, iTestMapB, iLengthB, iRefMapA,
		  iTestMapA, iLengthA);
		if (!LocalEq(dCS, dCS2))
			Quit_Qscore("CompareSeqs: dCS=%g dCS2=%g diff=%g", dCS, dCS2, fabs(dCS-dCS2));
		if (!LocalEq(dCS, dCS3))
			Quit_Qscore("CompareSeqs: dCS=%g dCS3=%g diff=%g", dCS, dCS3, fabs(dCS-dCS3));
		if (!LocalEq(dCS, dCS4))
			Quit_Qscore("CompareSeqs: dCS=%g dCS4=%g diff=%g", dCS, dCS4, fabs(dCS-dCS4));
		}*/
	}
#endif

	*ptrdSP = dSP;
	*ptrdPS = dPS;
	*ptrdCS = dCS;
	}
