#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

#include "muscle.h"
#include "msa.h"
#include "tree.h"
#include "profile.h"
#include "pwpath.h"
#include "seqvect.h"
#include "estring.h"
#include "muscle_context.h"

#define TRACE		0

void DeleteProgNode(ProgNode &Node)
	{
	delete[] Node.m_Prof;
	delete[] Node.m_EstringL;
	delete[] Node.m_EstringR;

	Node.m_Prof = 0;
	Node.m_EstringL = 0;
	Node.m_EstringR = 0;
	}

static void MakeNode(ProgNode &OldNode, ProgNode &NewNode, bool bSwapLR)
	{
	if (bSwapLR)
		{
		NewNode.m_EstringL = OldNode.m_EstringR;
		NewNode.m_EstringR = OldNode.m_EstringL;
		}
	else
		{
		NewNode.m_EstringL = OldNode.m_EstringL;
		NewNode.m_EstringR = OldNode.m_EstringR;
		}
	NewNode.m_Prof = OldNode.m_Prof;
	NewNode.m_uLength = OldNode.m_uLength;
	NewNode.m_Weight = OldNode.m_Weight;

	OldNode.m_Prof = 0;
	OldNode.m_EstringL = 0;
	OldNode.m_EstringR = 0;
	}

void RealignDiffsE(const MSA &msaIn, const SeqVect &v,
  const Tree &NewTree, const Tree &OldTree, 
  const unsigned uNewNodeIndexToOldNodeIndex[],
  MSA &msaOut, ProgNode *OldProgNodes)
	{
	assert(OldProgNodes != 0);

	const unsigned uNodeCount = NewTree.GetNodeCount();
	if (uNodeCount%2 == 0)
		Quit("RealignDiffs: Expected odd number of nodes");

	const unsigned uMergeCount = (uNodeCount - 1)/2;
	ProgNode *NewProgNodes = new ProgNode[uNodeCount];
    MuscleContext* ctx = getMuscleContext();

	for (unsigned uNewNodeIndex = 0; uNewNodeIndex < uNodeCount; ++uNewNodeIndex)
		{
		if (NODE_CHANGED == uNewNodeIndexToOldNodeIndex[uNewNodeIndex])
			continue;

		unsigned uOldNodeIndex = uNewNodeIndexToOldNodeIndex[uNewNodeIndex];
		assert(uNewNodeIndex < uNodeCount);
		assert(uOldNodeIndex < uNodeCount);

		ProgNode &NewNode = NewProgNodes[uNewNodeIndex];
		ProgNode &OldNode = OldProgNodes[uOldNodeIndex];
		bool bSwapLR = false;
		if (!NewTree.IsLeaf(uNewNodeIndex))
			{
			unsigned uNewLeft = NewTree.GetLeft(uNewNodeIndex);
			unsigned uNewRight = NewTree.GetRight(uNewNodeIndex);
			unsigned uOld = uNewNodeIndexToOldNodeIndex[uNewNodeIndex];
			unsigned uOldLeft = OldTree.GetLeft(uOld);
			unsigned uOldRight = OldTree.GetRight(uOld);
			assert(uOldLeft < uNodeCount && uOldRight < uNodeCount);
			if (uOldLeft != uNewNodeIndexToOldNodeIndex[uNewLeft])
				{
				assert(uOldLeft == uNewNodeIndexToOldNodeIndex[uNewRight]);
				bSwapLR = true;
				}
			}
		MakeNode(OldNode, NewNode, bSwapLR);
#if	TRACE
		Log("MakeNode old=%u new=%u swap=%d length=%u weight=%.3g\n",
		  uOldNodeIndex, uNewNodeIndex, bSwapLR, NewNode.m_uLength, NewNode.m_Weight);
#endif
		}

	unsigned uJoin = 0;
	SetProgressDesc("Refine tree");
	for (unsigned uNewNodeIndex = NewTree.FirstDepthFirstNode();
	  NULL_NEIGHBOR != uNewNodeIndex && !ctx->isCanceled();
	  uNewNodeIndex = NewTree.NextDepthFirstNode(uNewNodeIndex))
		{
		if (NODE_CHANGED != uNewNodeIndexToOldNodeIndex[uNewNodeIndex])
			continue;

		Progress(uJoin, uMergeCount - 1);
		++uJoin;

		const unsigned uMergeNodeIndex = uNewNodeIndex;
		ProgNode &Parent = NewProgNodes[uMergeNodeIndex];

		const unsigned uLeft = NewTree.GetLeft(uNewNodeIndex);
		const unsigned uRight = NewTree.GetRight(uNewNodeIndex);

		ProgNode &Node1 = NewProgNodes[uLeft];
		ProgNode &Node2 = NewProgNodes[uRight];

		AlignTwoProfs(
			Node1.m_Prof, Node1.m_uLength, Node1.m_Weight,
			Node2.m_Prof, Node2.m_uLength, Node2.m_Weight,
			Parent.m_Path,
			&Parent.m_Prof, &Parent.m_uLength);
        
        PathToEstrings(Parent.m_Path, &Parent.m_EstringL, &Parent.m_EstringR);

		Parent.m_Weight = Node1.m_Weight + Node2.m_Weight;

		delete[] Node1.m_Prof;
		delete[] Node2.m_Prof;

		Node1.m_Prof = 0;
		Node2.m_Prof = 0;
		}
    
    if (!ctx->isCanceled()) {

	    ProgressStepsDone();

	    if (getMuscleContext()->params.g_bBrenner)
		    MakeRootMSABrenner((SeqVect &) v, NewTree, NewProgNodes, msaOut);
	    else
		    MakeRootMSA(v, NewTree, NewProgNodes, msaOut);

#if	DEBUG
	    AssertMSAEqIgnoreCaseAndGaps(msaIn, msaOut);
#endif
    }

	for (unsigned uNodeIndex = 0; uNodeIndex < uNodeCount; ++uNodeIndex)
		DeleteProgNode(NewProgNodes[uNodeIndex]);

	delete[] NewProgNodes;
	}
