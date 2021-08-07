#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

#include "muscle.h"
#include <stdio.h>
#include <time.h>
#include "muscle_context.h"

// Functions that provide visible feedback to the user
// that progress is being made.

//const char *ElapsedTimeAsStr()
//	{
//    time_t &g_tLocalStart = getMuscleContext()->progress.g_tLocalStart;
//	time_t Now = time(0);
//	unsigned long ElapsedSecs = (unsigned long) (Now - g_tLocalStart);
//	return SecsToStr(ElapsedSecs);
//	}
//warning: unsafe
//const char *MemToStr(double MB)
//	{
//	if (MB < 0)
//		return "";
//
//	static char Str[256];
//	static double MaxMB = 0;
//	static double RAMMB = 0;
//
//	if (RAMMB == 0)
//		RAMMB = GetRAMSizeMB();
//
//	if (MB > MaxMB)
//		MaxMB = MB;
//	double Pct = (MaxMB*100.0)/RAMMB;
//	if (Pct > 100)
//		Pct = 100;
//	sprintf(Str, "%.0f MB(%.0f%%)", MaxMB, Pct);
//	return Str;
//	}

void SetInputFileName(const char *pstrFileName)
	{
    char* g_strFileName = getMuscleContext()->progress.g_strFileName;
	NameFromPath(pstrFileName, g_strFileName, sizeof(g_strFileName));
	}

void SetSeqStats(unsigned uSeqCount, unsigned uMinL, unsigned uMaxL, unsigned uAvgL)
	{
    MuscleContext *ctx = getMuscleContext();
    FILE* &g_fProgress = ctx->progress.g_fProgress;
    char* g_strFileName = ctx->progress.g_strFileName;
    bool &g_bQuiet = ctx->params.g_bQuiet;
    bool &g_bVerbose = ctx->params.g_bVerbose;
	if (g_bQuiet)
		return;

    ctx->progress.pr_printf(g_fProgress, "%s %u seqs, lengths min %u, max %u, avg  length %u\n",
      g_strFileName, uSeqCount, uMinL, uMaxL, uAvgL);
	if (g_bVerbose)
		Log("%u seqs, max length %u, avg  length %u\n",
		  uSeqCount, uMaxL, uAvgL);
	}

void SetStartTime()
	{
	time(&getMuscleContext()->progress.g_tLocalStart);
	}

unsigned long GetStartTime()
	{
	return (unsigned long) getMuscleContext()->progress.g_tLocalStart;
	}

void SetIter(unsigned uIter)
	{
	getMuscleContext()->progress.g_uIter = uIter;
	}

void IncIter()
	{
	++getMuscleContext()->progress.g_uIter;
	}

void SetMaxIters(unsigned uMaxIters)
	{
	getMuscleContext()->progress.g_uLocalMaxIters = uMaxIters;
	}

void SetProgressDesc(const char szDesc[])
	{
    char* g_strDesc = getMuscleContext()->progress.g_strDesc;
	strncpy(g_strDesc, szDesc, sizeof(g_strDesc));
	g_strDesc[sizeof(g_strDesc) - 1] = 0;
	}

static void Wipe(int n)
	{
    MuscleContext* ctx = getMuscleContext();
    FILE* &g_fProgress = ctx->progress.g_fProgress;
	for (int i = 0; i < n; ++i)
		ctx->progress.pr_printf(g_fProgress, " ");
	}

void Progress(const char *szFormat, ...)
	{
    MuscleContext *ctx = getMuscleContext();
    FILE* &g_fProgress = ctx->progress.g_fProgress;
    bool &g_bQuiet = ctx->params.g_bQuiet;

	CheckMaxTime();

	if (g_bQuiet)
		return;

    //double MB = GetMemUseMB();

	char szStr[4096];
	va_list ArgList;
	va_start(ArgList, szFormat);
	vsprintf(szStr, szFormat, ArgList);

	ctx->progress.pr_printf(g_fProgress, /*"%8.8s  %12s  */"%s",
	  //ElapsedTimeAsStr(),
	  //MemToStr(MB),
	  szStr);

	ctx->progress.pr_printf(g_fProgress, "\n");
	fflush(g_fProgress);
	}

void Progress(unsigned uStep, unsigned uTotalSteps)
	{
    MuscleContext *ctx = getMuscleContext();
    FILE* &g_fProgress = ctx->progress.g_fProgress;
    bool &g_bQuiet = ctx->params.g_bQuiet;
    char* g_strDesc = ctx->progress.g_strDesc;
    unsigned &g_uIter = ctx->progress.g_uIter;
    bool &g_bWipeDesc = ctx->progress.g_bWipeDesc;
    int &g_nPrevDescLength = ctx->progress.g_nPrevDescLength;
    unsigned &g_uTotalSteps = ctx->progress.g_uTotalSteps;

	CheckMaxTime();

	if (g_bQuiet)
		return;

	double dPct = ((uStep + 1)*100.0)/uTotalSteps;
    //double MB = GetMemUseMB();
	ctx->progress.pr_printf(g_fProgress, /*"%8.8s  %12s  */"Iter %3u  %6.2f%%  %s",
	  //ElapsedTimeAsStr(),
	  //MemToStr(MB),
	  g_uIter,
	  dPct,
	  g_strDesc);

	if (g_bWipeDesc)
		{
		int n = g_nPrevDescLength - (int) strlen(g_strDesc);
		Wipe(n);
		g_bWipeDesc = false;
		}

	ctx->progress.pr_printf(g_fProgress, "\r");

	g_uTotalSteps = uTotalSteps;
	}

void ProgressStepsDone()
	{
	CheckMaxTime();
    MuscleContext *ctx = getMuscleContext();
    FILE* &g_fProgress = ctx->progress.g_fProgress;
    bool &g_bQuiet = ctx->params.g_bQuiet;
    char* g_strDesc = ctx->progress.g_strDesc;
    unsigned &g_uIter = ctx->progress.g_uIter;
    bool &g_bWipeDesc = ctx->progress.g_bWipeDesc;
    int &g_nPrevDescLength = ctx->progress.g_nPrevDescLength;
    unsigned &g_uTotalSteps = ctx->progress.g_uTotalSteps;
    bool &g_bVerbose = ctx->params.g_bVerbose;

	if (g_bVerbose)
		{
        //double MB = GetMemUseMB();
		//Log("Elapsed time %8.8s  Peak memory use %12s  Iteration %3u %s\n",
		// ElapsedTimeAsStr(),
		// MemToStr(MB),
		// g_uIter,
		// g_strDesc);
		}

	if (g_bQuiet)
		return;

    Progress(g_uTotalSteps - 1, g_uTotalSteps);
	ctx->progress.pr_printf(g_fProgress, "\n");
	g_bWipeDesc = true;
	g_nPrevDescLength = (int) strlen(g_strDesc);
	}
