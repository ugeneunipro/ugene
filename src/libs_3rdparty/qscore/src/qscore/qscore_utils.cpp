#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

#include "qscore.h"

void Quit_Qscore(const char *szFormat, ...)
	{
		QScoreException e;
		
		va_list ArgList;

		va_start(ArgList, szFormat);
		vsprintf(e.str, szFormat, ArgList);
		va_end(ArgList);
		
		throw e;
		//	exit(EXIT_FatalError);
	}

FILE *OpenStdioFile(const char *FileName)
	{
	FILE *f = fopen(FileName, "r");
	if (0 == f)
		Quit_Qscore("Cannot open %s, %s [errno=%d]", FileName, strerror(errno), errno);
	return f;
	}

int GetFileSize(FILE *f)
	{
	long CurrPos = ftell(f);
	if (CurrPos < 0)
		Quit_Qscore("FileSize: ftell<0 (CurrPos), errno=%d", errno);

	int Ok = fseek(f, 0, SEEK_END);
	if (Ok != 0)
		Quit_Qscore("FileSize fseek(END) != 0 errno=%d", errno);

	long Size = ftell(f);
	if (Size < 0)
		Quit_Qscore("FileSize: ftell<0 (size), errno=%d", errno);

	Ok = fseek(f, CurrPos, SEEK_SET);
	if (Ok != 0)
		Quit_Qscore("FileSize fseek(restore curr pos) != 0 errno=%d", errno);

	long NewPos = ftell(f);
	if (CurrPos < 0)
		Quit_Qscore("FileSize: ftell=%ld != CurrPos=%ld", CurrPos, NewPos);

	return (int) Size;
	}

void *allocmem(int bytes)
	{
	char *p = (char *) malloc((size_t) (bytes));
	if (0 == p)
		Quit_Qscore("Out of memory (%d)", bytes);
	return p;
	}

void freemem(void *p)
	{
	if (0 == p)
		return;
	free(((char *) p));
	}
