#include "muscle.h"
#include "pwpath.h"
#include "timing.h"
#include "textfile.h"
#include "msa.h"
#include "profile.h"
#include "muscle_context.h"

#if	!VER_3_52

#define COMPARE_SIMPLE	0

#if	TIMING
TICKS g_ticksDP = 0;
#endif

// NWSmall() (see nwsmall.cpp) keeps its DP score rows in O(length) memory, but
// still allocates a full uLengthA x uLengthB traceback byte cache up front.
// For very long profiles that cache alone can reach tens of GB. Once the
// estimated cache size crosses this budget, fall back to the diagonal/banded
// GlobalAlignDiags() (glbaligndiag.cpp) instead, which bounds DP to narrow
// regions around matching diagonals.
static const unsigned long long MUSCLE_MAX_FULL_DP_CELLS = 2ULL * 1024 * 1024 * 1024; // ~2GB traceback cache

#if	1

SCORE NWSmall(const ProfPos *PA, unsigned uLengthA, const ProfPos *PB,
  unsigned uLengthB, PWPath &Path);
SCORE NWDASmall(const ProfPos *PA, unsigned uLengthA, const ProfPos *PB,
  unsigned uLengthB, PWPath &Path);
SCORE NWDASimple(const ProfPos *PA, unsigned uLengthA, const ProfPos *PB,
  unsigned uLengthB, PWPath &Path);
SCORE NWDASimple2(const ProfPos *PA, unsigned uLengthA, const ProfPos *PB,
  unsigned uLengthB, PWPath &Path);
SCORE GlobalAlignSimple(const ProfPos *PA, unsigned uLengthA, const ProfPos *PB,
  unsigned uLengthB, PWPath &Path);

SCORE GlobalAlignNoDiags(const ProfPos *PA, unsigned uLengthA, const ProfPos *PB,
  unsigned uLengthB, PWPath &Path)
	{
	// Must call NWSmall() directly, NOT GlobalAlign(): GlobalAlignDiags()
	// (glbaligndiag.cpp) calls this function for its leftover rectangular
	// regions and expects an unconditional plain-DP fallback. Routing back
	// through the size-checking GlobalAlign() below can re-trigger the
	// diags path for a region the diagonal finder failed to shrink,
	// recursing without bound.
	//
	// This is also the single choke point every full (non-diagonal) DP call
	// funnels through, including GlobalAlignDiags()'s own leftover regions.
	// For genuinely dissimilar inputs, diagonal-finding can fail to shrink
	// the problem at all, so this hard cap is what actually prevents an
	// uncontrolled multi-GB allocation: refuse cleanly instead of letting
	// the process grow until the OS kills it.
	const unsigned long long uCells =
	  (unsigned long long) (uLengthA + 1) * (unsigned long long) (uLengthB + 1);
	if (uCells > MUSCLE_MAX_FULL_DP_CELLS)
		throw MuscleException("Alignment requires too much memory for a single "
		  "pairwise DP step, even after diagonal-based reduction (the input "
		  "sequences are likely too long and too dissimilar to align safely). "
		  "Refusing to continue to avoid exhausting system memory.");
	return NWSmall(PA, uLengthA, PB, uLengthB, Path);
	}

#if	COMPARE_SIMPLE

SCORE GlobalAlign(const ProfPos *PA, unsigned uLengthA, const ProfPos *PB,
  unsigned uLengthB, PWPath &Path)
	{
#if	TIMING
	TICKS t1 = GetClockTicks();
#endif
	g_bKeepSimpleDP = true;
	PWPath SimplePath;
	GlobalAlignSimple(PA, uLengthA, PB, uLengthB, SimplePath);

	SCORE Score = NWSmall(PA, uLengthA, PB, uLengthB, Path);

	if (!Path.Equal(SimplePath))
		{
		Log("Simple:\n");
		SimplePath.LogMe();
		Log("Small:\n");
		Path.LogMe();
		Quit("Paths differ");
		}

#if	TIMING
	TICKS t2 = GetClockTicks();
	g_ticksDP += (t2 - t1);
#endif
	return Score;
	}

#else // COMPARE_SIMPLE

SCORE GlobalAlign(const ProfPos *PA, unsigned uLengthA, const ProfPos *PB,
  unsigned uLengthB, PWPath &Path)
	{
#if	TIMING
	TICKS t1 = GetClockTicks();
#endif
	const unsigned long long uCells =
	  (unsigned long long) (uLengthA + 1) * (unsigned long long) (uLengthB + 1);
	SCORE Score = (uCells > MUSCLE_MAX_FULL_DP_CELLS)
	  ? GlobalAlignDiags(PA, uLengthA, PB, uLengthB, Path)
	  : NWSmall(PA, uLengthA, PB, uLengthB, Path);
#if	TIMING
	TICKS t2 = GetClockTicks();
	g_ticksDP += (t2 - t1);
#endif
	return Score;
	}

#endif

#else // 1

static void AllInserts(PWPath &Path, unsigned uLengthB)
	{
	Path.Clear();
	PWEdge Edge;
	Edge.cType = 'I';
	Edge.uPrefixLengthA = 0;
	for (unsigned uPrefixLengthB = 1; uPrefixLengthB <= uLengthB; ++uPrefixLengthB)
		{
		Edge.uPrefixLengthB = uPrefixLengthB;
		Path.AppendEdge(Edge);
		}
	}

static void AllDeletes(PWPath &Path, unsigned uLengthA)
	{
	Path.Clear();
	PWEdge Edge;
	Edge.cType = 'D';
	Edge.uPrefixLengthB = 0;
	for (unsigned uPrefixLengthA = 1; uPrefixLengthA <= uLengthA; ++uPrefixLengthA)
		{
		Edge.uPrefixLengthA = uPrefixLengthA;
		Path.AppendEdge(Edge);
		}
	}

SCORE GlobalAlign(const ProfPos *PA, unsigned uLengthA, const ProfPos *PB,
  unsigned uLengthB, PWPath &Path)
	{
#if	TIMING
	TICKS t1 = GetClockTicks();
#endif
	if (0 == uLengthA)
		{
		AllInserts(Path, uLengthB);
		return 0;
		}
	else if (0 == uLengthB)
		{
		AllDeletes(Path, uLengthA);
		return 0;
		}

	SCORE Score = 0;
	if (g_bDiags)
		Score = GlobalAlignDiags(PA, uLengthA, PB, uLengthB, Path);
	else
		Score = GlobalAlignNoDiags(PA, uLengthA, PB, uLengthB, Path);
#if	TIMING
	TICKS t2 = GetClockTicks();
	g_ticksDP += (t2 - t1);
#endif
	return Score;
	}

SCORE GlobalAlignNoDiags(const ProfPos *PA, unsigned uLengthA, const ProfPos *PB,
  unsigned uLengthB, PWPath &Path)
	{
	if (g_bDimer)
		return GlobalAlignDimer(PA, uLengthA, PB, uLengthB, Path);

	switch (g_PPScore)
		{
	case PPSCORE_LE:
		return GlobalAlignLE(PA, uLengthA, PB, uLengthB, Path);

	case PPSCORE_SP:
	case PPSCORE_SV:
		return GlobalAlignSP(PA, uLengthA, PB, uLengthB, Path);

	case PPSCORE_SPN:
		return GlobalAlignSPN(PA, uLengthA, PB, uLengthB, Path);
		}

	Quit("Invalid PP score (GlobalAlignNoDiags)");
	return 0;
	}

#endif

#endif	// !VER_3_52
