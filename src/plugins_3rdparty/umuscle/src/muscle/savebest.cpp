#include "muscle.h"
#include "msa.h"
#include "textfile.h"
#include <time.h>
#include "muscle_context.h"


void SetOutputFileName(const char *out)
	{
	getMuscleContext()->savebest.pstrOutputFileName = out;
	}

void SetCurrentAlignment(MSA &msa)
	{
	getMuscleContext()->savebest.ptrBestMSA = &msa;
	}

void SaveCurrentAlignment()
	{
    MuscleContext *ctx = getMuscleContext();
	static bool bCalled = false;
	if (bCalled)
		{
		fprintf(stderr,
		  "\nRecursive call to SaveCurrentAlignment, giving up attempt to save.\n");
        //exit(EXIT_FatalError);
        throw MuscleException("Recursive call to SaveCurrentAlignment, giving up attempt to save.");
		}

	if (0 == ctx->savebest.ptrBestMSA)
		{
		fprintf(stderr, "\nAlignment not completed, cannot save.\n");
		Log("Alignment not completed, cannot save.\n");
		//exit(EXIT_FatalError);
        throw MuscleException("Alignment not completed, cannot save");
		}

	if (0 == ctx->savebest.pstrOutputFileName)
		{
		fprintf(stderr, "\nOutput file name not specified, cannot save.\n");
		//exit(EXIT_FatalError);
        throw MuscleException("Output file name not specified, cannot save.");
		}

	fprintf(stderr, "\nSaving current alignment ...\n");

	TextFile fileOut(ctx->savebest.pstrOutputFileName, true);
	ctx->savebest.ptrBestMSA->ToFASTAFile(fileOut);

	fprintf(stderr, "Current alignment saved to \"%s\".\n", ctx->savebest.pstrOutputFileName);
	Log("Current alignment saved to \"%s\".\n", ctx->savebest.pstrOutputFileName);
	}

void CheckMaxTime()
	{
    MuscleContext *ctx = getMuscleContext();
	if (ctx->params.g_ulMaxSecs == 0)
		return;

	time_t Now = time(0);
	time_t ElapsedSecs = Now - GetStartTime();
	if (ElapsedSecs <= (time_t) ctx->params.g_ulMaxSecs)
		return;

	//Log("Max time %s exceeded, elapsed seconds = %ul\n",
	//  MaxSecsToStr(), ElapsedSecs);

	SaveCurrentAlignment();
	//exit(EXIT_Success);
    throw MuscleException("Max time exceeded");
	}
