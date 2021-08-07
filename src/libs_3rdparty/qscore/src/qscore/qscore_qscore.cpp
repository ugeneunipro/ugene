#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

#include "qscore.h"

//const char *g_TestFileName;
//const char *g_RefFileName;

//bool g_Quiet = false;
//bool g_Slow = false;
//bool g_Cline = false;
//bool g_Modeler = false;
//bool g_GC = false;
//bool g_SeqDiffWarn = false;
//bool g_Verbose = false;
//bool g_StripX = false;
//bool g_StripZ = false;
//bool g_StripB = false;

static void ToUpper(MSA_QScore &msa)
	{
	const int SeqCount = msa.GetSeqCount();
	const int ColCount = msa.GetColCount();

	for (int SeqIndex = 0; SeqIndex < SeqCount; ++SeqIndex)
		for (int ColIndex = 0; ColIndex < ColCount; ++ColIndex)
			{
			char c = msa.GetChar(SeqIndex, ColIndex);
			if (isalpha(c))
				{
				c = toupper(c);
				msa.SetChar(SeqIndex, ColIndex, c);
				}
			}
	}

double QScore(MSA_QScore* _msaTest, MSA_QScore* _msaRef)
	{
	/*g_TestFileName = RequiredValueOpt("test");
	g_RefFileName = RequiredValueOpt("ref");

	g_Slow = FlagOpt("slow");
	g_Cline = FlagOpt("cline");
	g_Modeler = FlagOpt("modeler");
	g_Quiet = FlagOpt("quiet");
	g_GC = FlagOpt("gapscore");
	g_SeqDiffWarn = FlagOpt("seqdiffwarn");
	g_Verbose = FlagOpt("verbose");

	g_StripX = FlagOpt("stripx");
	g_StripB = FlagOpt("stripb");
	g_StripZ = FlagOpt("stripz");*/

	
	MSA_QScore &msaTest = *_msaTest;
	MSA_QScore &msaRef = *_msaRef;


	/*FILE *fTest = OpenStdioFile(g_TestFileName);
	FILE *fRef = OpenStdioFile(g_RefFileName);*/

	//msaTest.FromFASTAFile(fTest);
	//msaRef.FromFASTAFile(fRef);

	//fclose(fTest);
	//fclose(fRef);

	//if (FlagOpt("ignoretestcase"))
		ToUpper(msaTest);

	//if (FlagOpt("ignorerefcase"))
		ToUpper(msaRef);

	if (0 == msaTest.GetSeqCount())
		Quit_Qscore("No seqs in test alignment");

	if (0 == msaRef.GetSeqCount())
		Quit_Qscore("No seqs in ref alignment");

	//if (FlagOpt("perseq"))
	//	PerSeq(msaTest, msaRef);

	double Q = dInsane;
	double TC = dInsane;
	double Modeler = dInsane;
	double SlowQ = dInsane;
	double SlowTC = dInsane;
	double SlowModeler = dInsane;
	double SlowCline = dInsane;
	double GC = dInsane;
	double CG = dInsane;
	double TGC = dInsane;
	double TCG = dInsane;

	FastQ(msaTest, msaRef, Q, TC);

	/*if (g_Modeler)
		{
		double NotUsedTC = dInsane;
		FastQ(msaRef, msaTest, Modeler, NotUsedTC, false);
		}

	if (g_Slow || g_Cline || g_Modeler)
		{
		CompareMSA(msaTest, msaRef, &SlowQ, &SlowModeler, &SlowCline);
		SlowTC = ComputeTC(msaTest, msaRef);
		}

	if (g_GC)
		{
		ComputeGapScoreMSA(msaTest, msaRef, GC, TGC);
		ComputeGapScoreMSA(msaRef, msaTest, CG, TCG);
		}*/

	/*printf("Test=%s;Ref=%s;Q=%.3g;TC=%.3g",
	  g_TestFileName, g_RefFileName, Q, TC);*/
	return Q;

	//if (g_Cline)
	//	printf(";Cline=%.3g", SlowCline);

	//if (g_Modeler)
	//	printf(";Modeler=%.3g", Modeler);

	//if (g_Slow)
	//	printf(";SlowQ=%.3g;SlowTC=%.3g;SlowModeler=%.3g", SlowQ, SlowTC, SlowModeler);

	//if (g_GC)
	//	printf(";GC=%.3g;CG=%.3g;TGC=%.3g;TCG=%.3g", GC, CG, TGC, TCG);

	//printf("\n");
	}
