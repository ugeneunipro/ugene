// qscore.h

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <errno.h>

#include <algorithm>
#include <vector>
#include <string>

#ifndef UINT_MAX
#define UINT_MAX 0xffffffff
#endif

#ifdef _MSC_VER
#  if _MSC_VER < 1900
#include <hash_map>
typedef stdext::hash_map<std::string, unsigned> StrToInt;
#  else
#include <unordered_map>
typedef std::unordered_map<std::string, unsigned> StrToInt;
#  endif
#else
#undef __DEPRECATED // disable warning message about deprecated dependency.
#include <ext/hash_map>

struct HashStringToUnsigned
	{
	size_t operator()(const std::string &Key)  const
		{
		size_t h = 0;
		size_t Bytes = Key.size();
		for (size_t i = 0; i < Bytes; ++i)
			{
			unsigned char c = (unsigned char) Key[i];
			h = c + (h << 6) + (h << 16) - h;
			}
		return h;
		}
	};

typedef __gnu_cxx::hash_map<std::string, unsigned, HashStringToUnsigned> StrToInt;
#endif

using namespace std;

// Allow different conventions: DEBUG or _DEBUG for debug mode,
// NDEBUG for not debug mode.
#ifdef	_DEBUG
#undef DEBUG
#define	DEBUG	1
#endif

#ifdef	DEBUG
#undef _DEBUG
#define	_DEBUG	1
#endif

#ifdef NDEBUG
#undef	DEBUG
#undef	_DEBUG
#endif

typedef vector<unsigned> IntVec;
typedef vector<bool> BoolVec;

#define	all(t, n)		(t *) allocmem((n)*sizeof(t))
#define	reall(p, t, n)		p = (t *) reallocmem(p, (n)*sizeof(t))
#define zero(p,	t, n)	memset(p, 0, (n)*sizeof(t))
void *allocmem(int bytes);
void freemem(void *p);
void *reallocmem(void *p, int bytes);

static inline bool IsGap(char c)
	{
	return '-' == c || '~' == c || '.' == c || '+' == c || '#' == c;
	}

static inline int iabs(int i)
	{
	return i >= 0 ? i : -i;
	}

class MSA_QScore;

const double dInsane = double(0xffffffff);
const unsigned uInsane = 987654321;

unsigned CharToLetter(char c);
char LetterToChar(unsigned Letter);

void ComparePair(const MSA_QScore &msaTest, unsigned uTestSeqIndexA,
  unsigned uTestSeqIndexB, const MSA_QScore &msaRef, unsigned uRefSeqIndexA,
  unsigned uRefSeqIndexB, double *ptrdSP, double *ptrdPS, double *ptrdCS);

double ComparePairSP(const MSA_QScore &msaTest, unsigned uTestSeqIndexA,
  unsigned uTestSeqIndexB, const MSA_QScore &msaRef, unsigned uRefSeqIndexA,
  unsigned uRefSeqIndexB);

void ComparePairMap(const int iTestMapA[], const int iTestMapB[],
  const int iRefMapA[], const int iRefMapB[], int iLengthA, int iLengthB,
  double *ptrdSP, double *ptrdPS, double *ptrdCS);

double ComparePairMapSP(const int iTestMapA[], const int iTestMapB[],
  const int iRefMapA[], const int iRefMapB[], int iLengthA, int iLengthB);

double SumPairs(const int iMapRef[], const int iMapTest[], unsigned uLength);

double ClineShift(const int iTestMapA[], const int iRefMapA[], unsigned uLengthA,
  const int iTestMapB[], const int iRefMapB[], unsigned uLengthB, double dEpsilon = 0.2);

void MakePairMaps(const MSA_QScore &msaTest, unsigned uTestSeqIndexA, unsigned uTestSeqIndexB,
  const MSA_QScore &msaRef, unsigned uRefSeqIndexA, unsigned uRefSeqIndexB, int **ptriTestMapAr,
  int **ptriTestMapBr, int **ptriRefMapAr, int **ptriRefMapBr);

void Quit_Qscore(const char *Format, ...);
//void Warning_Qscore(const char *Format, ...);

FILE *OpenStdioFile(const char *FileName);
int GetFileSize(FILE *f);

//void ParseOptions(int argc, char *argv[]);
bool FlagOpt_QScore(const char *Name);
const char *ValueOpt_QScore(const char *Name);
const char *RequiredValueOpt(const char *Name);

void CompareMSA(const MSA_QScore &msaTest, const MSA_QScore &msaRef, double *ptrdSP,
  double *ptrdPS, double *ptrdCS);
double ComputeTC(MSA_QScore &msaTest, MSA_QScore &msaRef);
void FastQ(const MSA_QScore &msaTest, const MSA_QScore &msaRef, double &Q, double &TC,
  bool WarnIfNoRefAligned = true);
void ComputeGapScoreMSA(MSA_QScore &msaTest, MSA_QScore &msaRef, double &GC, double &TC);
//void Log(const char *Format, ...);
double PerSeq(const MSA_QScore &msaTest, const MSA_QScore &msaRef);

double QScore(MSA_QScore* _msaTest, MSA_QScore* _msaRef);
void SAB();

#include "qscore/msa.h"
#include "qscore/seq.h"
#include "qscore/qscore_context.h"
