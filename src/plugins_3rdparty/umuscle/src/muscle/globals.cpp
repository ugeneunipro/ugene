#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

#if	WIN32
#include <windows.h>
#include <share.h>
#endif

#include "muscle.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <errno.h>
#include "muscle_context.h"

void SetSeqWeightMethod(SEQWEIGHT Method)
	{
	getMuscleContext()->globals.g_SeqWeight = Method;
	}

SEQWEIGHT GetSeqWeightMethod()
	{
	return getMuscleContext()->globals.g_SeqWeight;
	}

void SetListFileName(const char *ptrListFileName, bool bAppend)
	{
    MuscleContext *ctx = getMuscleContext();

	assert(strlen(ptrListFileName) < MAX_PATH);
	strcpy(ctx->globals.g_strListFileName, ptrListFileName);
	ctx->globals.g_bListFileAppend = bAppend;
	}

void Log(const char szFormat[], ...) {
}

const char *GetTimeAsStr() {
    return NULL;
}

/*void Log(const char szFormat[], ...)
        {
    MuscleContext *ctx = getMuscleContext();
    char* g_strListFileName = ctx->globals.g_strListFileName;
    bool &g_bListFileAppend = ctx->globals.g_bListFileAppend;

        if (0 == g_strListFileName[0])
                return;

        static FILE *f = NULL;
    char *mode;
        if (g_bListFileAppend)
                mode = "a";
        else
                mode = "w";
        if (NULL == f)
                f = _fsopen(g_strListFileName, mode, _SH_DENYNO);
        if (NULL == f)
                {
                perror(g_strListFileName);
                exit(EXIT_NotStarted);
                }

        char szStr[4096];
        va_list ArgList;
        va_start(ArgList, szFormat);
        vsprintf(szStr, szFormat, ArgList);
        fprintf(f, "%s", szStr);
        fflush(f);
        }

const char *GetTimeAsStr()
        {
        static char szStr[32];
        time_t t;
        time(&t);
        struct tm *ptmCurrentTime = localtime(&t);
        strcpy(szStr, asctime(ptmCurrentTime));
        assert('\n' == szStr[24]);
        szStr[24] = 0;
        return szStr;
        }*/

// Exit immediately with error message, printf-style.
void Quit(const char szFormat[], ...)
	{
    MuscleException e;
    
    va_list ArgList;
	
	va_start(ArgList, szFormat);
	vsprintf(e.str, szFormat, ArgList);

	fprintf(stderr, "\n*** ERROR ***  %s\n", e.str);

	Log("\n*** FATAL ERROR ***  ");
	Log("%s\n", e.str);
	Log("Stopped %s\n", GetTimeAsStr());

    throw e;
//	exit(EXIT_FatalError);
	}

void Warning(const char szFormat[], ...)
	{
	va_list ArgList;
	char szStr[4096];

	va_start(ArgList, szFormat);
	vsprintf(szStr, szFormat, ArgList);

	fprintf(stderr, "\n*** WARNING *** %s\n", szStr);
	Log("\n*** WARNING ***  %s\n", szStr);
	}

// Remove leading and trailing blanks from string
void TrimBlanks(char szStr[])
	{
	TrimLeadingBlanks(szStr);
	TrimTrailingBlanks(szStr);
	}

void TrimLeadingBlanks(char szStr[])
	{
	size_t n = strlen(szStr);
	while (szStr[0] == ' ')
		{
		memmove(szStr, szStr+1, n);
		szStr[--n] = 0;
		}
	}

void TrimTrailingBlanks(char szStr[])
	{
	size_t n = strlen(szStr);
	while (n > 0 && szStr[n-1] == ' ')
		szStr[--n] = 0;
	}

bool Verbose()
	{
	return true;
	}

SCORE StrToScore(const char *pszStr)
	{
	return (SCORE) atof(pszStr);
	}

void StripWhitespace(char szStr[])
	{
	unsigned uOutPos = 0;
	unsigned uInPos = 0;
	while (char c = szStr[uInPos++])
		if (' ' != c && '\t' != c && '\n' != c && '\r' != c)
			szStr[uOutPos++] = c;
	szStr[uOutPos] = 0;
	}

void StripGaps(char szStr[])
	{
	unsigned uOutPos = 0;
	unsigned uInPos = 0;
	while (char c = szStr[uInPos++])
		if ('-' != c)
			szStr[uOutPos++] = c;
	szStr[uOutPos] = 0;
	}

bool IsValidSignedInteger(const char *Str)
	{
	if (0 == strlen(Str))
		return false;
	if ('+' == *Str || '-' == *Str)
		++Str;
	while (char c = *Str++)
		if (!isdigit(c))
			return false;
	return true;
	}

bool IsValidInteger(const char *Str)
	{
	if (0 == strlen(Str))
		return false;
	while (char c = *Str++)
		if (!isdigit(c))
			return false;
	return true;
	}

// Is c valid as first character in an identifier?
bool isidentf(char c)
	{
	return isalpha(c) || '_' == c;
	}

// Is c valid character in an identifier?
bool isident(char c)
	{
	return isalpha(c) || isdigit(c) || '_' == c;
	}

bool IsValidIdentifier(const char *Str)
	{
	if (!isidentf(Str[0]))
		return false;
	while (char c = *Str++)
		if (!isident(c))
			return false;
	return true;
	}

void SetLogFile()
	{
    MuscleContext *ctx = getMuscleContext();
    char* g_strListFileName = ctx->globals.g_strListFileName;
    bool &g_bListFileAppend = ctx->globals.g_bListFileAppend;

	const char *strFileName = ValueOpt("loga");
	if (0 != strFileName)
		g_bListFileAppend = true;
	else
		strFileName = ValueOpt("log");
	if (0 == strFileName)
		return;
	strcpy(g_strListFileName, strFileName);
	}

// Get filename, stripping any extension and folder parts.
void NameFromPath(const char szPath[], char szName[], unsigned uBytes)
	{
	if (0 == uBytes)
		return;
	const char *pstrLastSlash = strrchr(szPath, '/');
	const char *pstrLastBackslash = strrchr(szPath, '\\');
	const char *pstrLastDot = strrchr(szPath, '.');
	const char *pstrLastSep = pstrLastSlash > pstrLastBackslash ?
	  pstrLastSlash : pstrLastBackslash;
	const char *pstrBegin = pstrLastSep ? pstrLastSep + 1 : szPath;
	const char *pstrEnd = pstrLastDot ? pstrLastDot - 1 : szPath + strlen(szPath);
	unsigned uNameLength = (unsigned) (pstrEnd - pstrBegin + 1);
	if (uNameLength > uBytes - 1)
		uNameLength = uBytes - 1;
	memcpy(szName, pstrBegin, uNameLength);
	szName[uNameLength] = 0;
	}

char *strsave(const char *s)
	{
	char *ptrCopy = strdup(s);
	if (0 == ptrCopy)
		Quit("Out of memory");
	return ptrCopy;
	}

bool IsValidFloatChar(char c)
	{
	return isdigit(c) || '.' == c || 'e' == c || 'E' == c || 'd' == c ||
	  'D' == c || '.' == c || '+' == c || '-' == c;
	}

void MemPlus(size_t Bytes, char *Where)
	{
    MuscleContext *ctx = getMuscleContext();
    size_t &g_MemTotal = ctx->globals.g_MemTotal;
        
	g_MemTotal += Bytes;
//	Log("+%10u  %6u  %6u  %s\n",
//	  (unsigned) Bytes,
//	  (unsigned) GetMemUseMB(),
//	  (unsigned) (g_MemTotal/1000000),
//	  Where);
	}

void MemMinus(size_t Bytes, char *Where)
	{
    MuscleContext *ctx = getMuscleContext();
    size_t &g_MemTotal = ctx->globals.g_MemTotal;

	g_MemTotal -= Bytes;
//	Log("-%10u  %6u  %6u  %s\n",
//	  (unsigned) Bytes,
//	  (unsigned) GetMemUseMB(),
//	  (unsigned) (g_MemTotal/1000000),
//	  Where);
	}
