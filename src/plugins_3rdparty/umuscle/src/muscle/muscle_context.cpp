#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

#include "muscle.h"
#include "muscle_context.h"

extern SCOREMATRIX VTML_LA;
extern SCOREMATRIX PAM200;
extern SCOREMATRIX PAM200NoCenter;
extern SCOREMATRIX VTML_SP;
extern SCOREMATRIX VTML_SPNoCenter;
extern SCOREMATRIX NUC_SP;

bool MuscleContext::glbalndimer_stuct::InitializePPTerm()
{
    PPTerm.m_bAllGaps = false;
    PPTerm.m_LL = 1;
    PPTerm.m_LG = 0;
    PPTerm.m_GL = 0;
    PPTerm.m_GG = 0;
    PPTerm.m_fOcc = 1;
    return true;
}

MuscleContext::MuscleContext(int _nThreads) 
{
    memset(&intmath, 0, sizeof(intmath));
    memset(&params, 0, sizeof(params));
    memset(&alpha, 0, sizeof(alpha));
    memset(&muscle, 0, sizeof(muscle));
    memset(&nwdasimple, 0, sizeof(nwdasimple));
    memset(&ppscore, 0, sizeof(ppscore));
    memset(&scoredist, 0, sizeof(scoredist));
    memset(&objscore2, 0, sizeof(objscore2));
    memset(&nwsmalls, 0, sizeof(nwsmalls));
    memset(&fastdistnuc, 0, sizeof(fastdistnuc));
    memset(&fastdistmafft, 0, sizeof(fastdistmafft));
    memset(&glbalignsp, 0, sizeof(glbalignsp));
    memset(&glbalignspn, 0, sizeof(glbalignspn));
    memset(&glbalignss, 0, sizeof(glbalignss));
    memset(&glbalignle, 0, sizeof(glbalignle));
    memset(&setnewhandler, 0, sizeof(setnewhandler));
    memset(&options, 0, sizeof(options));
    memset(&globals, 0, sizeof(globals));
    memset(&progress, 0, sizeof(progress));
    memset(&scoregaps, 0, sizeof(scoregaps));
    memset(&glbaligndiag, 0, sizeof(glbaligndiag));
    memset(&upgma2, 0, sizeof(upgma2));
    memset(&msa2, 0, sizeof(msa2));
    memset(&validateids, 0, sizeof(validateids));
    memset(&refinehoriz, 0, sizeof(refinehoriz));
    memset(&spfast, 0, sizeof(spfast));
    memset(&readmx, 0, sizeof(readmx));
    memset(&mhack, 0, sizeof(mhack));
    memset(&glbalndimer, 0, sizeof(glbalndimer));
    memset(&subfams, 0, sizeof(subfams));
    memset(&savebest, 0, sizeof(savebest));
    memset(&enumtostr, 0, sizeof(enumtostr));
    memset(&finddiags, 0, sizeof(finddiags));
    memset(&finddiagsn, 0, sizeof(finddiagsn));
    memset(&globalswin32, 0, sizeof(globalswin32));

    nThreads = _nThreads;
    progressStub = 0;
    progressPercent = &progressStub;
    cancelStub = 0;
    cancelFlag = &cancelStub;

    m_uIdCount = 0;

    //intmath.cpp
    intmath.bInit = false;

    //params
    params.g_scoreCenter = 0;
    params.g_scoreGapExtend = 0;
    params.g_scoreGapOpen2 = MINUS_INFINITY;
    params.g_scoreGapExtend2 = MINUS_INFINITY;
    params.g_scoreGapAmbig = 0;
    params.g_scoreAmbigFactor = 0;
    
    params.g_pstrInFileName = "-";
    params.g_pstrOutFileName = "-";
    params.g_pstrFASTAOutFileName = 0;
    params.g_pstrMSFOutFileName = 0;
    params.g_pstrClwOutFileName = 0;
    params.g_pstrClwStrictOutFileName = 0;
    params.g_pstrHTMLOutFileName = 0;
    params.g_pstrPHYIOutFileName = 0;
    params.g_pstrPHYSOutFileName = 0;
    params.g_pstrDistMxFileName1 = 0;
    params.g_pstrDistMxFileName2 = 0;

    params.g_pstrFileName1 = 0;
    params.g_pstrFileName2 = 0;

    params.g_pstrSPFileName = 0;
    params.g_pstrMatrixFileName = 0;

    params.g_pstrUseTreeFileName = 0;

    ////
    params.g_pstrInFileName = "-";
    params.g_pstrOutFileName = "-";
    params.g_pstrFASTAOutFileName = 0;
    params.g_pstrMSFOutFileName = 0;
    params.g_pstrClwOutFileName = 0;
    params.g_pstrClwStrictOutFileName = 0;
    params.g_pstrHTMLOutFileName = 0;
    params.g_pstrPHYIOutFileName = 0;
    params.g_pstrPHYSOutFileName = 0;
    params.g_pstrDistMxFileName1 = 0;
    params.g_pstrDistMxFileName2 = 0;

    params.g_pstrComputeWeightsFileName = 0;
    params.g_pstrFileName1 = 0;
    params.g_pstrFileName2 = 0;

    params.g_pstrSPFileName = 0;
    params.g_pstrMatrixFileName = 0;

    params.g_pstrUseTreeFileName = 0;

    params.g_pstrComputeWeightsFileName = NULL;

    params.g_pstrProf1FileName = 0;
    params.g_pstrProf2FileName = 0;

    params.g_bUseTreeNoWarn = false;

    params.g_uSmoothWindowLength = 7;
    params.g_uAnchorSpacing = 32;
    params.g_uMaxTreeRefineIters = 1;

    params.g_uRefineWindow = 200;
    params.g_uWindowFrom = 0;
    params.g_uWindowTo = 0;
    params.g_uSaveWindow = uInsane;
    params.g_uWindowOffset = 0;

    params.g_uMaxSubFamCount = 5;

    params.g_uHydrophobicRunLength = 5;
    params.g_dHydroFactor = (float) 1.2;

    params.g_uMinDiagLength = 24;	// TODO alpha -- should depend on alphabet?
    params.g_uMaxDiagBreak = 1;
    params.g_uDiagMargin = 5;

    params.g_dSUEFF = (float) 0.1;

    params.g_bPrecompiledCenter = true;
    params.g_bNormalizeCounts = false;
    params.g_bDiags1 = false;
    params.g_bDiags2 = false;
    params.g_bAnchors = true;
    params.g_bQuiet = false;
    params.g_bVerbose = false;
    params.g_bRefine = false;
    params.g_bRefineW = false;
    params.g_bProfDB = false;
    params.g_bLow = false;
    params.g_bSW = false;
    params.g_bClusterOnly = false;
    params.g_bProfile = false;
    params.g_bPPScore = false;
    params.g_bBrenner = false;
    params.g_bDimer = false;
    params.g_bVersion = false;
    params.g_bStable = false;
    params.g_bFASTA = false;
    params.g_bPAS = false;
    params.g_bTomHydro = false;
    params.g_bMakeTree = false;

#if	DEBUG
    params.g_bCatchExceptions = false;
#else
    params.g_bCatchExceptions = true;
#endif

    params.g_bMSF = false;
    params.g_bAln = false;
    params.g_bClwStrict = false;
    params.g_bHTML = false;
    params.g_bPHYI = false;
    params.g_bPHYS = false;

    params.g_uMaxIters = 8;
    params.g_ulMaxSecs = 0;
    params.g_uMaxMB = 500;

    params.g_PPScore = PPSCORE_LE;
    params.g_ObjScore = OBJSCORE_SPM;

    params.g_SeqWeight1 = SEQWEIGHT_ClustalW;
    params.g_SeqWeight2 = SEQWEIGHT_ClustalW;

    params.g_Distance1 = DISTANCE_Kmer6_6;
    params.g_Distance2 = DISTANCE_PctIdKimura;

    params.g_Cluster1 = CLUSTER_UPGMB;
    params.g_Cluster2 = CLUSTER_UPGMB;

    params.g_Root1 = ROOT_Pseudo;
    params.g_Root2 = ROOT_Pseudo;

    // params.g_bDiags;

    params.g_SeqType = SEQTYPE_Auto;

    params.g_TermGaps = TERMGAPS_Half;

    //------------------------------------------------------
    // These parameters depending on the chosen prof-prof
    //  (params.g_PP), initialized to "Undefined".
    params.g_dSmoothScoreCeil = fInsane;
    params.g_dMinBestColScore = fInsane;
    params.g_dMinSmoothScore = fInsane;
    params.g_scoreGapOpen = fInsane;

    //alpha.cpp
    alpha.g_Alpha = ALPHA_Undefined;
    alpha.g_AlphaSize = 0;
    alpha.InvalidLetterCount = 0;

    //ppscore.cpp
    ppscore.g_bTracePPScore = false;
    ppscore.g_ptrPPScoreMSA1 = 0;
    ppscore.g_ptrPPScoreMSA2 = 0;

    //setnewhandler.cpp
    setnewhandler.EmergencyReserve = 0;

    //options.cpp
    static options_struct::VALUE_OPT ValueOptsSample[] =
    {
        {"in",			0},
        {"in1",			0},
        {"in2",			0},
        {"out",			0},
        {"MaxIters",		0},
        {"MaxHours",		0},
        {"GapOpen",		0},
        {"GapOpen2",		0},
        {"GapExtend",           0},
        {"GapExtend2",          0},
        {"GapAmbig",		0},
        {"Center",		0},
        {"SmoothScoreCeil",	0},
        {"MinBestColScore",	0},
        {"MinSmoothScore",	0},
        {"ObjScore",		0},
        {"SmoothWindow",	0},
        {"RefineWindow",	0},
        {"FromWindow",		0},
        {"ToWindow",		0},
        {"SaveWindow",		0},
        {"WindowOffset",	0},
        {"FirstWindow",		0},
        {"AnchorSpacing",	0},
        {"Log",			0},
        {"LogA",		0},
        {"MaxTrees",		0},
        {"SUEFF",		0},
        {"Distance",		0},
        {"Distance1",		0},
        {"Distance2",		0},
        {"Weight",		0},
        {"Weight1",		0},
        {"Weight2",		0},
        {"Cluster",		0},
        {"Cluster1",		0},
        {"Cluster2",		0},
        {"Root1",		0},
        {"Root2",		0},
        {"Tree1",		0},
        {"Tree2",		0},
        {"UseTree",		0},
        {"UseTree_NoWarn",	0},
        {"DiagLength",		0},
        {"DiagMargin",		0},
        {"DiagBreak",		0},
        {"Hydro",		0},
        {"HydroFactor",		0},
        {"SPScore",		0},
        {"SeqType",		0},
        {"MaxMB",		0},
        {"ComputeWeights",	0},
        {"MaxSubFam",		0},
        {"ScoreFile",		0},
        {"TermGaps",		0},
        {"FASTAOut",		0},
        {"CLWOut",		0},
        {"CLWStrictOut",	0},
        {"HTMLOut",		0},
        {"MSFOut",		0},
        {"PHYIOut",		0},
        {"PHYSOut",		0},
        {"Matrix",		0},
        {"DistMx1",		0},
        {"DistMx2",		0},
        {"Weight",		0},
    };
    options.ValueOptCount = sizeof(ValueOptsSample)/sizeof(ValueOptsSample[0]);
    options.ValueOpts = new options_struct::VALUE_OPT[options.ValueOptCount];
    memcpy(options.ValueOpts,ValueOptsSample,sizeof(ValueOptsSample));

    static options_struct::FLAG_OPT FlagOptsSample[] =
    {
        {"LE",		false},
        {"SP",		false},
        {"SV",		false},
        {"SPN",		false},
        {"Core",	false},
        {"NoCore",	false},
        {"Diags1",	false},
        {"Diags2",	false},
        {"Diags",	false},
        {"Quiet",	false},
        {"MSF",		false},
        {"Verbose",	false},
        {"Anchors",	false},
        {"NoAnchors",	false},
        {"Refine",	false},
        {"RefineW",	false},
        {"SW",		false},
        {"Profile",	false},
        {"PPScore",	false},
        {"ClusterOnly",	false},
        {"Brenner",	false},
        {"Dimer",	false},
        {"clw",		false},
        {"clwstrict",	false},
        {"HTML",	false},
        {"Version",	false},
        {"Stable",	false},
        {"Group",	false},
        {"FASTA",	false},
        {"ProfDB",	false},
        {"PAS",		false},
        {"PHYI",	false},
        {"PHYS",	false},
        {"TomHydro",	false},
        {"MakeTree",	false},
    };
    options.FlagOptCount = sizeof(FlagOptsSample)/sizeof(FlagOptsSample[0]);
    options.FlagOpts = new options_struct::FLAG_OPT[options.FlagOptCount];
    memcpy(options.FlagOpts,FlagOptsSample,sizeof(FlagOptsSample));

    //globals.cpp
    globals.g_bListFileAppend = false;
    globals.g_SeqWeight = SEQWEIGHT_Undefined;

    //progress.cpp
    progress.g_uIter = 0;       // Main MUSCLE iteration 1, 2..
    progress.g_uLocalMaxIters = 0;  // Max iters
    progress.g_fProgress = stderr;  // Default to standard error
    progress.pr_printf = fprintf;
    progress.g_bWipeDesc = false;

    //glbaligndiag.cpp
    glbaligndiag.g_dDPAreaWithoutDiags = 0.0;
    glbaligndiag.g_dDPAreaWithDiags = 0.0;

    //glbalndimer.cpp
    glbalndimer.PPTermInitialized = glbalndimer.InitializePPTerm();

    //validateids.cpp
#if _DEBUG
    validateids.g_ptrMuscleSeqVect = 0;
#endif
    validateids.g_uTreeSplitNode1 = NULL_NEIGHBOR;
    validateids.g_uTreeSplitNode2 = NULL_NEIGHBOR;

    //msa2.cpp
    msa2.g_ptrMuscleTree = 0;
    msa2.g_uTreeSplitNode1 = NULL_NEIGHBOR;
    msa2.g_uTreeSplitNode2 = NULL_NEIGHBOR;
    msa2.g_MuscleWeights = NULL;

    //mhack.cpp
    mhack.M = NULL;

    //nwsmall->cpp
    nwsmalls = new nwsmall_struct[nThreads];
    for(int i=0;i<nThreads;i++) {
        nwsmall_struct* nwsmall = &nwsmalls[i];
        nwsmall->uCachePrefixCountA = 0;
        nwsmall->uCachePrefixCountB = 0;
        nwsmall->CacheTB = NULL;
        nwsmall->CacheDRow = NULL;
        nwsmall->CacheMCurr = NULL;
        nwsmall->CacheMNext = NULL;
        nwsmall->CacheMPrev = NULL;
    }

    //readmx.cpp
    readmx.HeadingCount = 0;

    //spfast.cpp
    spfast.bGapScoreMatrixInit = false;
} 

void MuscleContext::fillUidsVectors(int rowsCount) {
    const int initialRowsCount = input_uIds.length();
    tmp_uIds.reserve(initialRowsCount + rowsCount);
    input_uIds.reserve(initialRowsCount + rowsCount);
    for (unsigned i = initialRowsCount; i < initialRowsCount + rowsCount; i++) {
        input_uIds.append(i);
        tmp_uIds.append(i);
    }
}

extern void FreeDPMemSPN();  

MuscleContext::~MuscleContext() {
    //params.g_ptrScoreMatrix - do not delete -> inited from static structures

    delete ppscore.g_ptrPPScoreMSA1;
    delete ppscore.g_ptrPPScoreMSA2;
    //glbalignsp.cpp
    if (glbalignsp.DPM.uLength > 0) {
        for (unsigned i = 0; i < glbalignsp.DPM.uLength; ++i) {
            delete[] glbalignsp.DPM.TraceBack[i];
            delete[] glbalignsp.DPM.FreqsA[i];
            delete[] glbalignsp.DPM.SortOrderA[i];
        }
        for (unsigned n = 0; n < 20; ++n) {
            delete[] glbalignsp.DPM.ScoreMxB[n];
        }

        delete[] glbalignsp.DPM.MPrev;
        delete[] glbalignsp.DPM.MCurr;
        delete[] glbalignsp.DPM.MWork;
        delete[] glbalignsp.DPM.DPrev;
        delete[] glbalignsp.DPM.DCurr;
        delete[] glbalignsp.DPM.DWork;
        delete[] glbalignsp.DPM.uDeletePos;
        delete[] glbalignsp.DPM.GapOpenA;
        delete[] glbalignsp.DPM.GapOpenB;
        delete[] glbalignsp.DPM.GapCloseA;
        delete[] glbalignsp.DPM.GapCloseB;
        delete[] glbalignsp.DPM.SortOrderA;
        delete[] glbalignsp.DPM.FreqsA;
        delete[] glbalignsp.DPM.ScoreMxB;
        delete[] glbalignsp.DPM.TraceBack;
    }


    //glbalignspn.cpp
    //FreeDPMemSPN();
    const unsigned uOldLength = glbalignspn.DPM.uLength;
    if (0 != uOldLength)
    {
        for (unsigned i = 0; i < uOldLength; ++i)
        {
            delete[] glbalignspn.DPM.TraceBack[i];
            delete[] glbalignspn.DPM.FreqsA[i];
            delete[] glbalignspn.DPM.SortOrderA[i];
        }
        for (unsigned n = 0; n < 4; ++n)
            delete[] glbalignspn.DPM.ScoreMxB[n];

        delete[] glbalignspn.DPM.MPrev;
        delete[] glbalignspn.DPM.MCurr;
        delete[] glbalignspn.DPM.MWork;
        delete[] glbalignspn.DPM.DPrev;
        delete[] glbalignspn.DPM.DCurr;
        delete[] glbalignspn.DPM.DWork;
        delete[] glbalignspn.DPM.uDeletePos;
        delete[] glbalignspn.DPM.GapOpenA;
        delete[] glbalignspn.DPM.GapOpenB;
        delete[] glbalignspn.DPM.GapCloseA;
        delete[] glbalignspn.DPM.GapCloseB;
        delete[] glbalignspn.DPM.SortOrderA;
        delete[] glbalignspn.DPM.FreqsA;
        delete[] glbalignspn.DPM.ScoreMxB;
        delete[] glbalignspn.DPM.TraceBack;
    }
    //glbalignle.cpp
    if(glbalignle.DPM.uLength > 0) {
        for (unsigned i = 0; i < glbalignle.DPM.uLength; ++i) {
            delete[] glbalignle.DPM.TraceBack[i];
            delete[] glbalignle.DPM.FreqsA[i];
            delete[] glbalignle.DPM.SortOrderA[i];
        }
        for (unsigned n = 0; n < 20; ++n) {
            delete[] glbalignle.DPM.ScoreMxB[n];
        }

        delete[] glbalignle.DPM.MPrev;
        delete[] glbalignle.DPM.MCurr;
        delete[] glbalignle.DPM.MWork;
        delete[] glbalignle.DPM.DPrev;
        delete[] glbalignle.DPM.DCurr;
        delete[] glbalignle.DPM.DWork;
        delete[] glbalignle.DPM.uDeletePos;
        delete[] glbalignle.DPM.GapOpenA;
        delete[] glbalignle.DPM.GapOpenB;
        delete[] glbalignle.DPM.GapCloseA;
        delete[] glbalignle.DPM.GapCloseB;
        delete[] glbalignle.DPM.SortOrderA;
        delete[] glbalignle.DPM.FreqsA;
        delete[] glbalignle.DPM.ScoreMxB;
        delete[] glbalignle.DPM.TraceBack;
#if OCC
        delete[] glbalignle.DPM.OccA;
        delete[] glbalignle.DPM.OccB;
#endif
    }

    //glbalignss.cpp
    if (glbalignss.DPM.uLength > 0) {
        for (unsigned i = 0; i < glbalignss.DPM.uLength; ++i) {
            delete[] glbalignss.DPM.TraceBack[i];
        }

        delete[] glbalignss.DPM.MPrev;
        delete[] glbalignss.DPM.MCurr;
        delete[] glbalignss.DPM.MWork;
        delete[] glbalignss.DPM.DPrev;
        delete[] glbalignss.DPM.DCurr;
        delete[] glbalignss.DPM.DWork;
        delete[] glbalignss.DPM.MxRowA;
        delete[] glbalignss.DPM.LettersB;
        delete[] glbalignss.DPM.uDeletePos;
        delete[] glbalignss.DPM.TraceBack;
    }
    
    //setnewhandler.cpp
    free(setnewhandler.EmergencyReserve);

    //scoregaps.cpp
    scoregaps_struct::GAPINFO *GI = scoregaps.g_FreeList;
    while(GI!=NULL) {
        scoregaps_struct::GAPINFO *GI_del = GI;
        GI=GI->Next;
        delete GI_del;
    }
    delete[] scoregaps.g_Gaps;
    delete[] scoregaps.g_ColDiff;
    
    //msa2.cpp
    delete[] msa2.g_MuscleWeights;
    //created on stack only: delete msa2.g_ptrMuscleTree;
    
    //mhack.cpp
    delete[] mhack.M;

    //nwsmall->cpp
    for(int iThread=0;iThread<nThreads;iThread++) {
        nwsmall_struct* nwsmall = &nwsmalls[iThread];
        for (unsigned i = 0; i < nwsmall->uCachePrefixCountA; ++i) {
            delete[] nwsmall->CacheTB[i];
        }
        delete[] nwsmall->CacheTB;
        delete[] nwsmall->CacheDRow;
        delete[] nwsmall->CacheMCurr;
        delete[] nwsmall->CacheMNext;
        delete[] nwsmall->CacheMPrev;
    }
    delete[] nwsmalls;
    
    //options.cpp
    for (int i=0; i<options.ValueOptCount; i++) {
        delete options.ValueOpts[i].m_pstrValue;
    }
    delete[] options.ValueOpts;
    delete[] options.FlagOpts;

    //subfams.cpp
    delete[] subfams.ShortestPathEstimate;
    delete[] subfams.Predecessor;

    //savebest.cpp
    /* created on stack only
    if(savebest.ptrBestMSA!=NULL) {
        savebest.ptrBestMSA->Clear();
        delete savebest.ptrBestMSA;
    }*/

    //validateids.cpp
#if	_DEBUG
    //created on stack only delete validateids.g_ptrMuscleSeqVect;
#endif

}

MuscleException::MuscleException(const char* _str) {
    int len = strlen(_str);
    assert(len < 4096);
    memcpy(str, _str, len);
    str[len] = '\0';
}

MuscleException::MuscleException() {
    memset(str, '\0', 4096);
}
