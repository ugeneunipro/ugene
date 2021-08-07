#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

#include "qscore.h"

#define TRACE	0

using namespace std;

void ComputeGapScore(const vector<string> &TestSeqs, const vector<string> &TestLabels,
  const vector<string> &RefSeqs, const vector<string> &RefLabels, double &GC, double &TC);

void MSAToVecs(const MSA_QScore &msa, vector<string> &Seqs,
  vector<string> &Labels)
	{
	Seqs.clear();
	Labels.clear();

	const unsigned SeqCount = msa.GetSeqCount();
	const unsigned ColCount = msa.GetColCount();

	Seqs.resize(SeqCount);
	Labels.resize(SeqCount);
	for (unsigned SeqIndex = 0; SeqIndex < SeqCount; ++SeqIndex)
		{
		const char *Label = msa.GetSeqName(SeqIndex);
		Labels[SeqIndex] = Label;

		string &Seq = Seqs[SeqIndex];
		Seq.resize(ColCount);

		for (unsigned ColIndex = 0; ColIndex < ColCount; ++ColIndex)
			{
			char c = msa.GetChar(SeqIndex, ColIndex);
			Seq[ColIndex] = c;
			}
		}
	}

void ComputeGapScoreMSA(MSA_QScore &msaTest, MSA_QScore &msaRef, double &GC, double &TC)
	{
	vector<string> TestSeqs;
	vector<string> RefSeqs;
	vector<string> TestLabels;
	vector<string> RefLabels;
	MSAToVecs(msaTest, TestSeqs, TestLabels);
	MSAToVecs(msaRef, RefSeqs, RefLabels);
	ComputeGapScore(TestSeqs, TestLabels, RefSeqs, RefLabels, GC, TC);
	}
