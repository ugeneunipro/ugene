#ifndef _MUSCLE_CONTEXT_H_
#define _MUSCLE_CONTEXT_H_

#include "profile.h"
#include <limits.h>
#include <time.h>
#include <QVector>

#define EDGE_ARRAY_SIZE 10000

class MuscleException {
public:
    MuscleException();
    MuscleException(const char* str);
    char str[4096];
};

typedef int (*progress_printf)(FILE *str, const char *format, ...);

class MuscleContext {
public:
    MuscleContext(int nTHreads ); 
    ~MuscleContext();

    void fillUidsVectors(int rowsCount);

    int nThreads;
    
    int *cancelFlag;
    int cancelStub;
    bool isCanceled() const {return *cancelFlag;}

    static const int MAX_HEADINGS = 32;
    static const unsigned NULL_NEIGHBOR = UINT_MAX;
    unsigned m_uIdCount;

    QVector<unsigned> input_uIds;
    QVector<unsigned> tmp_uIds;
    QVector<unsigned> output_uIds;

    struct intmath_struct {
        bool bInit;
    } intmath;

    struct params_struct {
        SCORE g_scoreGapOpen;
        SCORE g_scoreCenter;
        SCORE g_scoreGapExtend;
        SCORE g_scoreGapAmbig;

        PPSCORE g_PPScore;
        OBJSCORE g_ObjScore;

        DISTANCE g_Distance1;
        CLUSTER g_Cluster1;
        ROOT g_Root1;
        SEQWEIGHT g_SeqWeight1;

        DISTANCE g_Distance2;
        CLUSTER g_Cluster2;
        ROOT g_Root2;
        SEQWEIGHT g_SeqWeight2;


        const char *g_pstrInFileName;
        const char *g_pstrOutFileName;
        const char *g_pstrFASTAOutFileName;
        const char *g_pstrMSFOutFileName;
        const char *g_pstrClwOutFileName;
        const char *g_pstrClwStrictOutFileName;
        const char *g_pstrHTMLOutFileName;
        const char *g_pstrPHYIOutFileName;
        const char *g_pstrPHYSOutFileName;
        const char *g_pstrDistMxFileName1;
        const char *g_pstrDistMxFileName2;

        const char *g_pstrFileName1;
        const char *g_pstrFileName2;

        const char *g_pstrSPFileName;
        const char *g_pstrMatrixFileName;

        const char *g_pstrUseTreeFileName;

        bool g_bUseTreeNoWarn;

        const char *g_pstrComputeWeightsFileName;
        const char *g_pstrScoreFileName;

        const char *g_pstrProf1FileName;
        const char *g_pstrProf2FileName;


        
        //#if	DOUBLE_AFFINE
        SCORE g_scoreGapOpen2;
        SCORE g_scoreGapExtend2;
        //#endif

        unsigned g_uSmoothWindowLength;
        unsigned g_uAnchorSpacing;
        unsigned g_uMaxTreeRefineIters;

        unsigned g_uMinDiagLength;
        unsigned g_uMaxDiagBreak;
        unsigned g_uDiagMargin;

        unsigned g_uRefineWindow;
        unsigned g_uWindowFrom;
        unsigned g_uWindowTo;
        unsigned g_uSaveWindow;
        unsigned g_uWindowOffset;

        unsigned g_uMaxSubFamCount;

        unsigned g_uHydrophobicRunLength;
        float g_dHydroFactor;

        float g_dSmoothScoreCeil;
        float g_dMinBestColScore;
        float g_dMinSmoothScore;
        float g_dSUEFF;

        bool g_bPrecompiledCenter;
        bool g_bNormalizeCounts;
        bool g_bDiags1;
        bool g_bDiags2;
        bool g_bDiags;
        bool g_bAnchors;
        bool g_bCatchExceptions;

        bool g_bMSF;
        bool g_bAln;
        bool g_bClwStrict;
        bool g_bHTML;
        bool g_bPHYI;
        bool g_bPHYS;

        bool g_bQuiet;
        bool g_bVerbose;
        bool g_bRefine;
        bool g_bRefineW;
        bool g_bRefineX;
        bool g_bLow;
        bool g_bSW;
        bool g_bClusterOnly;
        bool g_bProfile;
        bool g_bProfDB;
        bool g_bPPScore;
        bool g_bBrenner;
        bool g_bDimer;
        bool g_bVersion;
        bool g_bStable;
        bool g_bFASTA;
        bool g_bPAS;
        bool g_bTomHydro;
        bool g_bMakeTree;

        unsigned g_uMaxIters;
        unsigned long g_ulMaxSecs;
        unsigned g_uMaxMB;

        SEQTYPE g_SeqType;
        TERMGAPS g_TermGaps;

        PTR_SCOREMATRIX g_ptrScoreMatrix;
        SCORE g_scoreAmbigFactor;
    } params;


    //alpha.cpp
    struct alpha_struct {
        unsigned g_CharToLetter[MAX_CHAR];
        unsigned g_CharToLetterEx[MAX_CHAR];
        char g_LetterToChar[MAX_ALPHA];
        char g_LetterExToChar[MAX_ALPHA_EX];
        char g_UnalignChar[MAX_CHAR];
        char g_AlignChar[MAX_CHAR];
        bool g_IsWildcardChar[MAX_CHAR];
        bool g_IsResidueChar[MAX_CHAR];
        ALPHA g_Alpha;
        unsigned g_AlphaSize;
        char InvalidLetters[256];
        int InvalidLetterCount;
    } alpha;

    //muscle.h
    struct muscle_struct {
        int g_argc;
        char **g_argv;
        unsigned g_uTreeSplitNode1;
        unsigned g_uTreeSplitNode2;
        //double g_dNAN;
        unsigned long g_tStart;
    } muscle;

    /*
    //profile.h
    struct profile_struct {
    unsigned *ResidueGroup;
    } profile;
    */
    //nwdasimple.cpp
    struct nwdasimple_struct {
        bool g_bKeepSimpleDP;
        SCORE *g_DPM;
        SCORE *g_DPD;
        SCORE *g_DPE;
        SCORE *g_DPI;
        SCORE *g_DPJ;
        char *g_TBM;
        char *g_TBD;
        char *g_TBE;
        char *g_TBI;
        char *g_TBJ;
    } nwdasimple;

    //ppscore.cpp
    struct ppscore_struct {
        bool g_bTracePPScore;
        MSA *g_ptrPPScoreMSA1;
        MSA *g_ptrPPScoreMSA2;
    } ppscore;

    //scoredist.cpp
    struct scoredist_struct {
        int BLOSUM62[20][20];
        double BLOSUM62_Expected;
    } scoredist;

    //objscore2.cpp
    struct objscore2_struct {
        SCORE g_SPScoreLetters;
        SCORE g_SPScoreGaps;
    } objscore2;

    //nwsmalls[workerID].cpp
    struct nwsmall_struct {
        unsigned uCachePrefixCountB;
        unsigned uCachePrefixCountA;
        SCORE *CacheMCurr;
        SCORE *CacheMNext;
        SCORE *CacheMPrev;
        SCORE *CacheDRow;
        char **CacheTB;
    } *nwsmalls;

    //fastdistnuc.cpp
    struct fastdistnuc_struct {
        static const unsigned TUPLE_COUNT = 6*6*6*6*6*6;
        unsigned char Count1[TUPLE_COUNT];
        unsigned char Count2[TUPLE_COUNT];
    } fastdistnuc;

    //fastdistmafft.cpp
    struct fastdistmafft_struct {
        static const unsigned TUPLE_COUNT = 6*6*6*6*6*6;
        unsigned char Count1[TUPLE_COUNT];
        unsigned char Count2[TUPLE_COUNT];
    } fastdistmafft;

    //glbalignsp.cpp
    struct glbalignsp_struct {
        struct DP_MEMORY
        {
            unsigned uLength;
            SCORE *GapOpenA;
            SCORE *GapOpenB;
            SCORE *GapCloseA;
            SCORE *GapCloseB;
            SCORE *MPrev;
            SCORE *MCurr;
            SCORE *MWork;
            SCORE *DPrev;
            SCORE *DCurr;
            SCORE *DWork;
            SCORE **ScoreMxB;
            unsigned **SortOrderA;
            unsigned *uDeletePos;
            FCOUNT **FreqsA;
            int **TraceBack;
        } DPM;
    } glbalignsp;

    //glbalignspn.cpp
    struct glbalignspn_struct {
        struct DP_MEMORY
        {
            unsigned uLength;
            SCORE *GapOpenA;
            SCORE *GapOpenB;
            SCORE *GapCloseA;
            SCORE *GapCloseB;
            SCORE *MPrev;
            SCORE *MCurr;
            SCORE *MWork;
            SCORE *DPrev;
            SCORE *DCurr;
            SCORE *DWork;
            SCORE **ScoreMxB;
            unsigned **SortOrderA;
            unsigned *uDeletePos;
            FCOUNT **FreqsA;
            int **TraceBack;
        } DPM;
    } glbalignspn;

    //glbalihnss.cpp
    struct glbalignss_struct {
        struct DP_MEMORY
        {
            unsigned uLength;
            SCORE *MPrev;
            SCORE *MCurr;
            SCORE *MWork;
            SCORE *DPrev;
            SCORE *DCurr;
            SCORE *DWork;
            SCORE **MxRowA;
            unsigned *LettersB;
            unsigned *uDeletePos;
            int **TraceBack;
        } DPM;
    } glbalignss;

    //glbalignle.cpp
    struct glbalignle_struct {
#define	OCC	1

        struct DP_MEMORY
        {
            unsigned uLength;
            SCORE *GapOpenA;
            SCORE *GapOpenB;
            SCORE *GapCloseA;
            SCORE *GapCloseB;
            SCORE *MPrev;
            SCORE *MCurr;
            SCORE *MWork;
            SCORE *DPrev;
            SCORE *DCurr;
            SCORE *DWork;
            SCORE **ScoreMxB;
#if	OCC
            FCOUNT *OccA;
            FCOUNT *OccB;
#endif
            unsigned **SortOrderA;
            unsigned *uDeletePos;
            FCOUNT **FreqsA;
            int **TraceBack;
        } DPM;
    } glbalignle;

    //setnewhandler.cpp
    struct setnewhandler_struct {
        void *EmergencyReserve;
    } setnewhandler;

    //options.cpp
    struct options_struct {
        struct VALUE_OPT
        {
            const char *m_pstrName;
            const char *m_pstrValue;
        };


        struct FLAG_OPT
        {
            const char *m_pstrName;
            bool m_bSet;
        };
        VALUE_OPT *ValueOpts;
        int ValueOptCount;

        FLAG_OPT *FlagOpts;
        int FlagOptCount;
    } options;
    //globals.cpp
    struct globals_struct {
#ifndef	MAX_PATH
#define	MAX_PATH	260
#endif
        char g_strListFileName[MAX_PATH];
        bool g_bListFileAppend;
        SEQWEIGHT g_SeqWeight;
        size_t g_MemTotal;
    } globals;

    //progress.cpp
    struct progress_struct {
        unsigned g_uIter;		// Main MUSCLE iteration 1, 2..
        unsigned g_uLocalMaxIters;	// Max iters
        FILE *g_fProgress;	// Default to standard error
        progress_printf pr_printf;
        char g_strFileName[32];		// File name
        time_t g_tLocalStart;				// Start time
        char g_strDesc[32];			// Description
        bool g_bWipeDesc;
        int g_nPrevDescLength;
        unsigned g_uTotalSteps;
        
    } progress;

    //scoregaps.cpp
    struct scoregaps_struct {
        struct GAPINFO
        {
            GAPINFO *Next;
            unsigned Start;
            unsigned End;
        };

        GAPINFO **g_Gaps;
        GAPINFO *g_FreeList;
        unsigned g_MaxSeqCount;
        unsigned g_MaxColCount;
        unsigned g_ColCount;
        bool *g_ColDiff;
    } scoregaps;

    //glbaligndiag.cpp
    struct glbaligndiag_struct {
        double g_dDPAreaWithoutDiags;
        double g_dDPAreaWithDiags;
    } glbaligndiag;

    //upgma2.cpp
    struct upgma2_struct {
        unsigned g_uLeafCount;
        unsigned g_uTriangleSize;
        unsigned g_uInternalNodeCount;
        unsigned g_uInternalNodeIndex;
        typedef float dist_t;
        dist_t *g_Dist;
        dist_t *g_MinDist;
        unsigned *g_uNearestNeighbor;
        unsigned *g_uNodeIndex;
        unsigned *g_uLeft;
        unsigned *g_uRight;
        dist_t *g_Height;
        dist_t *g_LeftLength;
        dist_t *g_RightLength;
    } upgma2;

    //msa2.cpp
    struct msa2_struct {
        const Tree *g_ptrMuscleTree;
        WEIGHT *g_MuscleWeights;
        unsigned g_uMuscleIdCount;
        unsigned g_uTreeSplitNode1;
        unsigned g_uTreeSplitNode2;
    } msa2;

    //validateids.cpp
    struct validateids_struct {
#if	DEBUG
        SeqVect *g_ptrMuscleSeqVect;
        MSA MuscleInputMSA;
#endif
        unsigned g_uTreeSplitNode1;
        unsigned g_uTreeSplitNode2;
    } validateids;

    //refinehoriz.cpp
    struct refinehoriz_struct {
        unsigned g_uRefineHeightSubtree;
        unsigned g_uRefineHeightSubtreeTotal;
        unsigned Edges1[EDGE_ARRAY_SIZE];
        unsigned Edges2[EDGE_ARRAY_SIZE];
    } refinehoriz;

    //spfast.cpp
    struct spfast_struct {
        SCORE GapScoreMatrix[4][4];
        typedef SCORE SCORE44[4][4];
        typedef SCORE44 *ptrGapScoreMatrix;
        bool bGapScoreMatrixInit;
    } spfast;

    //readmx.cpp
    struct readmx_struct {
        char Heading[MAX_HEADINGS];
        unsigned HeadingCount;
        float Mx[32][32];
    } readmx;

    //mhack.cpp
    struct mhack_struct {
        bool *M;
    } mhack;

    //glbalndimer.cpp
    struct glbalndimer_stuct {
        ProfPos PPTerm;
        bool PPTermInitialized;
        bool InitializePPTerm();
    } glbalndimer;

    //subfams.cpp
    struct subfams_struct {
        float *ShortestPathEstimate;
        unsigned *Predecessor;
    } subfams;

    //savebest.cpp
    struct savebest_struct {
        MSA *ptrBestMSA;
        const char *pstrOutputFileName;
    } savebest;

    //enumtostr.cpp
    struct enumtostr_struct {
        char szMsg[64];
    } enumtostr;

    //finddiags.cpp
    struct finddiags_struct {
        static const unsigned KTUP = 5;
        static const unsigned KTUPS = 6*6*6*6*6;
        unsigned TuplePos[KTUPS];
    } finddiags;

    //finddiagsn.cpp
    struct finddiagsn_struct {
        #define pow4(i)	(1 << (2*i))	// 4^i = 2^(2*i)

        static const unsigned K = 7;
        static const unsigned KTUPS = pow4(K);
        unsigned TuplePos[KTUPS];
    } finddiagsn;

    //globalswin32.cpp
    struct globalswin32_struct {
        unsigned uPeakMemUseBytes;
    } globalswin32;
};

extern MuscleContext *getMuscleContext();
extern int getMuscleWorkerID();
#endif //tasklocaldata_h
