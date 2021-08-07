#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

#include "qscore.h"

#ifdef _MSC_VER
#pragma warning(disable: 4800) // int->bool
#endif

void SeqsDiffer(MSA_QScore &msaTest, unsigned uTestSeqIndex1, MSA_QScore &/*msaRef*/,
  unsigned /*uRefSeqIndex1*/, unsigned /*uRefColIndex*/)
	{
	Quit_Qscore("Test & ref sequences differ, label=%s", msaTest.GetSeqName(uTestSeqIndex1));
	}

bool IsAlignedColWithGaps(const MSA_QScore &msa, unsigned uColIndex)
	{
	const unsigned uSeqCount = msa.GetSeqCount();
	bool AnyGaps = false;
	bool AnyUpper = false;
	for (unsigned uSeqIndex = 0; uSeqIndex < uSeqCount; ++uSeqIndex)
		{
		if (msa.IsGap(uSeqIndex, uColIndex))
			{
			AnyGaps = true;
			continue;
			}
		char c = msa.GetChar(uSeqIndex, uColIndex);
		bool Upper = isupper(c);
		if (Upper)
			AnyUpper = true;
		else
			return false;
		if (Upper && AnyGaps)
			return true;
		}
	return AnyUpper && AnyGaps;
	}

int IsAlignedCol(const MSA_QScore &msa, unsigned uColIndex)
	{
	const unsigned uSeqCount = msa.GetSeqCount();
	for (unsigned uSeqIndex = 0; uSeqIndex < uSeqCount; ++uSeqIndex)
		{
		if (msa.IsGap(uSeqIndex, uColIndex))
			continue;
		return isupper(msa.GetChar(uSeqIndex, uColIndex));
		}
	return false;
	}

double ComputeTC(MSA_QScore &msaTest, MSA_QScore &msaRef)
	{
	const unsigned uRefSeqCount = msaRef.GetSeqCount();
	if (0 == uRefSeqCount)
		Quit_Qscore("No sequences in ref alignment");

	unsigned *RefSeqIndexToTestSeqIndex = new unsigned[uRefSeqCount];
	for (unsigned uRefSeqIndex = 0; uRefSeqIndex < uRefSeqCount; ++uRefSeqIndex)
		{
		const char *ptrName = msaRef.GetSeqName(uRefSeqIndex);
		unsigned uTestSeqIndex;
		bool bFound = msaTest.GetSeqIndex(ptrName, &uTestSeqIndex);
		if (bFound)
			RefSeqIndexToTestSeqIndex[uRefSeqIndex] = uTestSeqIndex;
		else
			RefSeqIndexToTestSeqIndex[uRefSeqIndex] = uInsane;
		}

	unsigned uRefAlignedColCount = 0;
	unsigned uCorrectlyAlignedColCount = 0;
	unsigned uRefColCount = msaRef.GetColCount();
	for (unsigned uRefColIndex = 0; uRefColIndex < uRefColCount; ++uRefColIndex)
		{
		if (!IsAlignedCol(msaRef, uRefColIndex))
			continue;

		bool bAllAlignedCorrectly = true;
		bool bAllGaps = true;
	// Iterate over all pairs
		for (unsigned uRefSeqIndex1 = 0; uRefSeqIndex1 < uRefSeqCount;
		  ++uRefSeqIndex1)
			{
			unsigned uTestSeqIndex1 = RefSeqIndexToTestSeqIndex[uRefSeqIndex1];
			if (uTestSeqIndex1 == uInsane)
				continue;

			char cRef1 = msaRef.GetChar(uRefSeqIndex1, uRefColIndex);
			if (IsGap(cRef1))
				continue;
			if (!isupper(cRef1))
				Quit_Qscore("Ref alignment col %d has both upper and lower-case letters", uRefColIndex);
			unsigned uRefUngappedColIndex1 =
			  msaRef.GetUngappedColIndex(uRefSeqIndex1, uRefColIndex);
			unsigned uTestGappedColIndex1 = msaTest.
			  GetGappedColIndex(uTestSeqIndex1, uRefUngappedColIndex1);
			char cTest1 = msaTest.GetChar(uTestSeqIndex1, uTestGappedColIndex1);
			if (cRef1 != toupper(cTest1))
				SeqsDiffer(msaTest, uTestSeqIndex1, msaRef, uRefSeqIndex1, uRefColIndex);
			for (unsigned uRefSeqIndex2 = uRefSeqIndex1 + 1; uRefSeqIndex2 < uRefSeqCount;
			  ++uRefSeqIndex2)
				{
				unsigned uTestSeqIndex2 = RefSeqIndexToTestSeqIndex[uRefSeqIndex2];
				if (uTestSeqIndex2 == uInsane)
					continue;

				char cRef2 = msaRef.GetChar(uRefSeqIndex2, uRefColIndex);
				if (IsGap(cRef2))
					continue;
				bAllGaps = false;
				assert(isupper(cRef2));

				unsigned uRefUngappedColIndex2 =
				  msaRef.GetUngappedColIndex(uRefSeqIndex2, uRefColIndex);
				unsigned uTestGappedColIndex2 = msaTest.
				  GetGappedColIndex(uTestSeqIndex2, uRefUngappedColIndex2);

				char cTest2 = msaTest.GetChar(uTestSeqIndex2, uTestGappedColIndex2);
				if (!(isupper(cTest1) && isupper(cTest2) &&
				  uTestGappedColIndex1 == uTestGappedColIndex2))
					{
					bAllAlignedCorrectly = false;
					goto NextCol;
					}
				}
			}

	NextCol:
		if (!bAllGaps)
			{
			++uRefAlignedColCount;
			if (bAllAlignedCorrectly)
				++uCorrectlyAlignedColCount;
			}
		}
	delete[] RefSeqIndexToTestSeqIndex;

	if (0 == uRefAlignedColCount)
		Quit_Qscore("No aligned columns (upper case) in ref alignment");

	return (double) uCorrectlyAlignedColCount / (double) uRefAlignedColCount;
	}
