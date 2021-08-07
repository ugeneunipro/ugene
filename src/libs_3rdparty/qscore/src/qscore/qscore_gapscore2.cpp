#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

#include "qscore.h"

using namespace std;

static bool IsGapChar(char c)
	{
	return c == '-' || c == '.' || c == '+' || c == '#';
	}

void MakeAlnColToSeqPosVec(const string &Seq, vector<unsigned> &PosVec)
	{
	const unsigned ColCount = (unsigned) Seq.size();
	PosVec.resize(ColCount);
	unsigned Pos = 0;
	for (unsigned ColIndex = 0; ColIndex < ColCount; ++ColIndex)
		{
		char c = Seq[ColIndex];
		PosVec[ColIndex] = Pos;
		if (!IsGapChar(c))
			PosVec[ColIndex] = Pos++;
		}
	}

void MakeSeqPosToAlnColVec(const string &Seq, vector<unsigned> &PosVec)
	{
	const unsigned ColCount = (unsigned) Seq.size();
	PosVec.resize(ColCount);
	unsigned Pos = 0;
	for (unsigned ColIndex = 0; ColIndex < ColCount; ++ColIndex)
		{
		char c = Seq[ColIndex];
		if (IsGapChar(c))
			continue;
		PosVec[Pos++] = ColIndex;
		}
	}

void MakeAlnColToSeqPosVecs(const vector<string> &Seqs,
  vector<vector<unsigned> > &PosVecs)
	{
	PosVecs.clear();
	const unsigned SeqCount = (unsigned) Seqs.size();
	PosVecs.resize(SeqCount);
	for (unsigned SeqIndex = 0; SeqIndex < SeqCount; ++SeqIndex)
		MakeAlnColToSeqPosVec(Seqs[SeqIndex], PosVecs[SeqIndex]);
	}

void MakeSeqPosToAlnColVecs(const vector<string> &Seqs,
  vector<vector<unsigned> > &PosVecs)
	{
	PosVecs.clear();
	const unsigned SeqCount = (unsigned) Seqs.size();
	PosVecs.resize(SeqCount);
	for (unsigned SeqIndex = 0; SeqIndex < SeqCount; ++SeqIndex)
		MakeSeqPosToAlnColVec(Seqs[SeqIndex], PosVecs[SeqIndex]);
	}

void ComputeGapScore(const vector<string> &TestSeqs, const vector<string> &TestLabels,
  const vector<string> &RefSeqs, const vector<string> &RefLabels, double &GC, double &TC)
	{
	const unsigned TestSeqCount = (unsigned) TestSeqs.size();
	const unsigned RefSeqCount = (unsigned) RefSeqs.size();
	if (RefSeqCount == 0)
		Quit_Qscore("No seqs in reference alignment");
	if (TestSeqCount == 0)
		Quit_Qscore("No seqs in test alignment");
	
	const unsigned RefColCount = (unsigned) RefSeqs[0].size();

	if ((unsigned) TestLabels.size() != TestSeqCount)
		Quit_Qscore("ComputeGapScore, test label count");
	if ((unsigned) RefLabels.size() != RefSeqCount)
		Quit_Qscore("ComputeGapScore, ref label count");

	vector<vector<unsigned> > TestSeqPosToAlnColVecs;
	vector<vector<unsigned> > TestAlnColToSeqPosVecs;
	vector<vector<unsigned> > RefAlnColToSeqPosVecs;
	MakeSeqPosToAlnColVecs(TestSeqs, TestSeqPosToAlnColVecs);
	MakeAlnColToSeqPosVecs(TestSeqs, TestAlnColToSeqPosVecs);
	MakeAlnColToSeqPosVecs(RefSeqs, RefAlnColToSeqPosVecs);

	vector<unsigned> RefSeqIndexToTestSeqIndex(RefSeqCount, UINT_MAX);
	for (unsigned RefSeqIndex = 0; RefSeqIndex < RefSeqCount; ++RefSeqIndex)
		{
		const string &RefLabel = RefLabels[RefSeqIndex];
		bool Found = false;
		for (unsigned TestSeqIndex = 0; TestSeqIndex < TestSeqCount; ++TestSeqIndex)
			{
			const string &TestLabel = TestLabels[TestSeqIndex];
			if (TestLabel == RefLabel)
				{
				Found = true;
				RefSeqIndexToTestSeqIndex[RefSeqIndex] = TestSeqIndex;
				break;
				}
			}
		if (!Found)
			Quit_Qscore("Ref seq '%s' not found in test alignment", RefLabel.c_str());
		}

	unsigned AlignedColCount = 0;
	unsigned AlignedGapColCount = 0;
	unsigned CorrectColCount = 0;
	unsigned CorrectGapColCount = 0;
	for (unsigned RefColIndex = 0; RefColIndex < RefColCount; ++RefColIndex)
		{
		bool AnyGaps = false;
		bool AnyLetters = false;
		bool AllCorrect = true;
		bool AnyUpper = false;
		for (unsigned RefSeqIndex = 0; RefSeqIndex < RefSeqCount; ++RefSeqIndex)
			{
			const string &RefSeq = RefSeqs[RefSeqIndex];
			char c = RefSeq[RefColIndex];
			if (IsGapChar(c))
				{
				AnyGaps = true;
				continue;
				}
			AnyLetters = true;
			if (!isupper(c))
				{
				if (AnyUpper)
					Quit_Qscore("Mixed upper and lower case in ref alignment column");
				continue;
				}
			AnyUpper = true;

		// We have a letter (X) in the reference sequence (S).
		//		RefSeqPos is the (ungapped) position of X in the ref sequence.
		//		TestSeqIndex is the sequence index in the test alignment of S.
		//		TestColIndex is the (gapped) column in the test alignment of X.
			unsigned RefSeqPos = RefAlnColToSeqPosVecs[RefSeqIndex][RefColIndex];
			unsigned TestSeqIndex = RefSeqIndexToTestSeqIndex[RefSeqIndex];
			unsigned TestColIndex = TestSeqPosToAlnColVecs[TestSeqIndex][RefSeqPos];

		// For every other sequence (S2)
			for (unsigned RefSeqIndex2 = 0; RefSeqIndex2 < RefSeqCount; ++RefSeqIndex2)
				{
				if (RefSeqIndex2 == RefSeqIndex)
					continue;
				const string &RefSeq2 = RefSeqs[RefSeqIndex2];

			// TestSeqIndex2 is the sequence index in the test alignment of S2.
				unsigned TestSeqIndex2 = RefSeqIndexToTestSeqIndex[RefSeqIndex2];
				char Refc2 = RefSeq2[RefColIndex];
				if (IsGapChar(Refc2))
					AnyGaps = true;
				char Testc2 = TestSeqs[TestSeqIndex2][TestColIndex];

			// Must be the same letter or must both be gap.
				if (Refc2 != Testc2)
					{
					AllCorrect = false;
					break;
					}

			// If independent insertions, ambiguous where gap should be so skip next test.
			// if gap-gap.
				if (IsGapChar(Refc2) && IsGapChar(Testc2))
					continue;

				unsigned RefSeqPos2 = RefAlnColToSeqPosVecs[RefSeqIndex2][RefColIndex];
				unsigned TestSeqPos2 = TestAlnColToSeqPosVecs[TestSeqIndex2][TestColIndex];

				if (RefSeqPos2 != TestSeqPos2)
					{
					AllCorrect = false;
					break;
					}
				}
			if (!AllCorrect)
				break;
			}

		if (AnyLetters && AnyUpper)
			{
			++AlignedColCount;
			if (AllCorrect)
				++CorrectColCount;
			if (AnyGaps)
				{
				++AlignedGapColCount;
				if (AllCorrect)
					++CorrectGapColCount;
				}
			}
		}

	if (AlignedColCount == 0)
		Quit_Qscore("No aligned cols in ref alignment");
	else
		TC = (double) CorrectColCount / (double) AlignedColCount;

	if (AlignedGapColCount == 0)
		GC = 1.0;
	else
		GC = (double) CorrectGapColCount / (double) AlignedGapColCount;

//	Log("GC=%.3g;TC=%.3g\n", GC, TC);
	}
