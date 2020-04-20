/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
 * http://ugene.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include "MuscleAdapter.h"
#include "MuscleUtils.h"
#include "MuscleConstants.h"

#include <U2Core/DNAAlphabet.h>
#include <U2Core/Task.h>
#include <U2Core/Log.h>
#include <U2Core/GAutoDeleteList.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2OpStatusUtils.h>

#include <algorithm>
#include <QVector>


namespace U2 {

//////////////////////////////////////////////////////////////////////////

void MuscleAdapter::align(const MultipleSequenceAlignment& ma, MultipleSequenceAlignment& res, TaskStateInfo& ti, bool mhack) {
    if(ti.cancelFlag)  {
        return;
    }
    try {
        alignUnsafe(ma, res, ti, mhack);
    } catch (const MuscleException &e) {
        if (!ti.cancelFlag) {
            ti.setError(  tr("Internal MUSCLE error: %1").arg(e.str) );
        }
    } catch (const std::bad_alloc &e) {
        ti.setError(getBadAllocError());
    } catch (...) {
        if (!ti.cancelFlag) {
            ti.setError(  tr("Undefined internal MUSCLE error") );
        }
    }
}

void MuscleAdapter::alignUnsafe(const MultipleSequenceAlignment& ma, MultipleSequenceAlignment& res, TaskStateInfo& ti, bool mhack) {
    ti.progress = 0;
    MuscleContext* ctx = getMuscleContext();

    MuscleParamsHelper ph(ti,ctx);

    SetSeqWeightMethod(ctx->params.g_SeqWeight1);

    setupAlphaAndScore(ma->getAlphabet(), ti);
    if (ti.hasError()) {
        return;
    }

    SeqVect v;
    convertMAlignment2SecVect(v, ma, true);
    const unsigned uSeqCount = v.Length();
    if (0 == uSeqCount) {
        ti.setError(  tr("No sequences in input file") );
        return;
    }

    unsigned uMinL = 0;
    unsigned uMaxL = 0;
    unsigned uTotL = 0;
    for (unsigned uSeqIndex = 0; uSeqIndex < uSeqCount; ++uSeqIndex) {
        unsigned L = v.GetSeq(uSeqIndex).Length();
        uTotL += L;
        uMinL = qMin(uMinL, L);
        uMaxL = qMax(uMaxL, L);
    }

    SetIter(1);
    ctx->params.g_bDiags = ctx->params.g_bDiags1;
    SetSeqStats(uSeqCount, uMinL, uMaxL, uTotL/uSeqCount);

    SetMuscleSeqVect(v);

    MSA::SetIdCount(uSeqCount);

    // Initialize sequence ids.
    // From this point on, ids must somehow propagate from here.
    for (unsigned uSeqIndex = 0; uSeqIndex < uSeqCount; ++uSeqIndex) {
        v.SetSeqId(uSeqIndex, uSeqIndex);
    }

    if (0 == uSeqCount) {
        ti.setError(  tr("Alignment is empty") );
        return;
    }

    if (1 == uSeqCount) {
        res = ma;
        return;
    }

    if (uSeqCount > 1 && mhack) {
        MHackStart(v);
    }
    Tree GuideTree;
    TreeFromSeqVect(v, GuideTree, ctx->params.g_Cluster1, ctx->params.g_Distance1, ctx->params.g_Root1, ctx->params.g_pstrDistMxFileName1);

    SetMuscleTree(GuideTree);
    ValidateMuscleIds(GuideTree);

    MSA msa;
    gauto_array<ProgNode> ProgNodes;

    if (ctx->params.g_bLow) {
        ProgNodes.data = ProgressiveAlignE(v, GuideTree, msa);
    } else {
        ProgressiveAlign(v, GuideTree, msa);
    }
    if (ti.cancelFlag) {
        return;
    }

    SetCurrentAlignment(msa);

    ValidateMuscleIds(msa);

    if (1 == ctx->params.g_uMaxIters || 2 == uSeqCount) {
        assert(int(msa.GetSeqCount()) == ma->getNumRows());
        prepareAlignResults(msa, ma->getAlphabet(), res, mhack);
        return;
    }

    ti.progress = 25;

    if(ti.cancelFlag) {
        return;
    }

    if (0 == ctx->params.g_pstrUseTreeFileName)  {
        ctx->params.g_bDiags = ctx->params.g_bDiags2;
        SetIter(2);

        if (ctx->params.g_bLow) {
            if (0 != ctx->params.g_uMaxTreeRefineIters)
                RefineTreeE(msa, v, GuideTree, ProgNodes.get());
        } else {
            RefineTree(msa, GuideTree);
        }
    }
    if (ti.cancelFlag) {
        return;
    }

    SetSeqWeightMethod(ctx->params.g_SeqWeight2);
    SetMuscleTree(GuideTree);

    ti.progress = 45;
    QTime timer;
    timer.start();
    if (ctx->params.g_bAnchors) {
        RefineVert(msa, GuideTree, ctx->params.g_uMaxIters - 2);
    } else {
        RefineHoriz(msa, GuideTree, ctx->params.g_uMaxIters - 2, false, false);
    }
    perfLog.trace(QString("Serial refine stage complete. Elapsed %1 ms").arg(timer.elapsed()));
    if (ti.cancelFlag) {
        return;
    }

    ValidateMuscleIds(msa);
    ValidateMuscleIds(GuideTree);

    //assert(int(msa.GetSeqCount()) == ma->getNumSequences());

    prepareAlignResults(msa, ma->getAlphabet(), res, mhack);
}

//////////////////////////////////////////////////////////////////////////
// refine single MSA

void MuscleAdapter::refine(const MultipleSequenceAlignment& ma, MultipleSequenceAlignment& res, TaskStateInfo& ti) {
    if(ti.cancelFlag)  {
        return;
    }
    try {
        QTime timer;
        timer.start();
        refineUnsafe(ma, res, ti);
        algoLog.trace(QString("Serial refine stage complete. Elapsed %1 ms").arg(timer.elapsed()));
    } catch (const MuscleException &e) {
        if (!ti.cancelFlag) {
            ti.setError(  tr("Internal MUSCLE error: %1").arg(e.str) );
        }
    } catch (const std::bad_alloc &e) {
        ti.setError(getBadAllocError());
    } catch (...) {
        if (!ti.cancelFlag) {
            ti.setError(  tr("Undefined internal MUSCLE error") );
        }
    }
}

void MuscleAdapter::refineUnsafe(const MultipleSequenceAlignment& ma, MultipleSequenceAlignment& res, TaskStateInfo& ti) {
    ti.progress = 0;

    MuscleContext *ctx = getMuscleContext();

    MuscleParamsHelper ph(ti,ctx);

    SetSeqWeightMethod(ctx->params.g_SeqWeight1);

    setupAlphaAndScore(ma->getAlphabet(), ti);
    if (ti.hasError()) {
        return;
    }
    MSA msa;
    convertMAlignment2MSA(msa, ma, true);
    unsigned uSeqCount = msa.GetSeqCount();
    MSA::SetIdCount(uSeqCount);

    // Initialize sequence ids.
    // From this point on, ids must somehow propogate from here.
    for (unsigned uSeqIndex = 0; uSeqIndex < uSeqCount; ++uSeqIndex) {
        msa.SetSeqId(uSeqIndex, uSeqIndex);
    }
    SetMuscleInputMSA(msa);

    Tree GuideTree;
    TreeFromMSA(msa, GuideTree, ctx->params.g_Cluster2, ctx->params.g_Distance2, ctx->params.g_Root2);
    SetMuscleTree(GuideTree);

    if (ctx->params.g_bAnchors) {
        RefineVert(msa, GuideTree, ctx->params.g_uMaxIters);
    } else {
        RefineHoriz(msa, GuideTree, ctx->params.g_uMaxIters, false, false);
    }

    ValidateMuscleIds(msa);
    ValidateMuscleIds(GuideTree);

    prepareAlignResults(msa, ma->getAlphabet(), res, false);
}

//////////////////////////////////////////////////////////////////////////
/// align 2 profiles

//from profile.cpp
static bool TreeNeededForWeighting(SEQWEIGHT s) {
    switch (s) {
        case SEQWEIGHT_ClustalW:
        case SEQWEIGHT_ThreeWay:
            return true;
        default:
            return false;
    }
}

static ProfPos *ProfileFromMSALocal_ProfileCPP(MSA &msa, Tree &tree) {
    MuscleContext *ctx = getMuscleContext();
    const unsigned uSeqCount = msa.GetSeqCount();
    for (unsigned uSeqIndex = 0; uSeqIndex < uSeqCount; ++uSeqIndex)
        msa.SetSeqId(uSeqIndex, uSeqIndex);

    if (TreeNeededForWeighting(ctx->params.g_SeqWeight2)) {
        TreeFromMSA(msa, tree, ctx->params.g_Cluster2, ctx->params.g_Distance2, ctx->params.g_Root1);
        SetMuscleTree(tree);
    }
    return ProfileFromMSA(msa);
}

void MuscleAdapter::align2Profiles(const MultipleSequenceAlignment& ma1, const MultipleSequenceAlignment& ma2, MultipleSequenceAlignment& res, TaskStateInfo& ti) {
    if(ti.cancelFlag)  {
        return;
    }
    if (ma1->isEmpty() || ma2->isEmpty()) {
        ti.setError(  tr("Invalid input alignment") );
        return;
    }
    try {
        align2ProfilesUnsafe(ma1, ma2, res, ti);
    } catch (const MuscleException &e) {
        if (!ti.cancelFlag) {
            ti.setError(  tr("Internal MUSCLE error: %1").arg(e.str) );
        }
    } catch (const std::bad_alloc &e) {
        ti.setError(getBadAllocError());
    } catch (...) {
        if (!ti.cancelFlag) {
            ti.setError(  tr("Undefined internal MUSCLE error") );
        }
    }
}

void MuscleAdapter::align2ProfilesUnsafe(const MultipleSequenceAlignment& ma1, const MultipleSequenceAlignment& ma2, MultipleSequenceAlignment& res, TaskStateInfo& ti) {
    assert(!ma1->isEmpty() && !ma2->isEmpty());
    const DNAAlphabet* al = U2AlphabetUtils::deriveCommonAlphabet(ma1->getAlphabet(), ma2->getAlphabet());
    CHECK_EXT(al != NULL, ti.setError(  tr("Incompatible alphabets") ), );

    MuscleContext *ctx = getMuscleContext();

    MuscleParamsHelper ph(ti,ctx);
    SetSeqWeightMethod(ctx->params.g_SeqWeight1);

    setupAlphaAndScore(al, ti);
    if (ti.hasError()) {
        return;
    }

    MSA msa1;
    convertMAlignment2MSA(msa1, ma1, true);
    MSA msa2;
    convertMAlignment2MSA(msa2, ma2, true);

    MSA::SetIdCount(ma1->getNumRows() + ma2->getNumRows());

    unsigned uLength1 = msa1.GetColCount();
    unsigned uLength2 = msa2.GetColCount();

    Tree tree1, tree2;
    gauto_array<ProfPos> Prof1(ProfileFromMSALocal_ProfileCPP(msa1, tree1));
    gauto_array<ProfPos> Prof2(ProfileFromMSALocal_ProfileCPP(msa2, tree2));
    gauto_array<ProfPos> ProfOut;

    PWPath Path; unsigned uLengthOut;
    ti.setDescription(tr("Aligning profiles"));
    AlignTwoProfs(Prof1.data, uLength1, 1.0, Prof2.data, uLength2, 1.0, Path, &ProfOut.data, &uLengthOut);

    ti.setDescription(tr("Building output"));
    MSA msaOut;
    AlignTwoMSAsGivenPath(Path, msa1, msa2, msaOut);
    msa1.Clear();//save memory
    msa2.Clear();

    //todo: stablize? -> original muscle fails if stablize is called
    convertMSA2MAlignment(msaOut, al, res);
}



//////////////////////////////////////////////////////////////////////////
// add unaligned sequences to profile

class AlignedSeq {
public:
    QString     name;
    QByteArray  seq;
    QByteArray  pathToMSA;
};


static QByteArray path2Str(const PWPath& path) {
    QByteArray res(path.GetEdgeCount(), '\0');
    for (int i = 0, n = path.GetEdgeCount(); i<n; i++) {
        const PWEdge &edge = path.GetEdge(i);
        res[i] = edge.cType;
    }
    return res;
}

//alters origPath according insertions made in msa (adjPath)
static void originalMSAToCurrent(const QByteArray& adjPath, const QByteArray& origPath, QByteArray& resPath) {
    assert(resPath.isEmpty());
    int aLen = adjPath.length();
    int oLen = origPath.length();
    int oPos = 0;
    int aPos = 0;
    for (;oPos < oLen || aPos < aLen; aPos++) {
        char oc = oPos < oLen ? origPath[oPos] : 'D';
        char ac = aPos < aLen ? adjPath[aPos]  : 'M';
        assert(ac != 'D'); //TODO: report error or check before the algorithm that MSA has no gap-cols
        if (ac == 'I' && oc == 'I') {
            resPath.append('M');
            oPos++;
            continue;
        }
        if (ac == 'I') {
            resPath.append('D');
            continue;
        }
        resPath.append(oc);
        oPos++;
    }
}

static void addSequenceToMSA(MultipleSequenceAlignment& ma, const QByteArray& path, QByteArray& msaPathChanges, const QByteArray& seq, const QString& name) {
    assert(msaPathChanges.length() == ma->getLength());

    QVector<int> insCoords; //coords of gaps to be added to model
    QByteArray alignedSeq; //a sequence to be added to model
    int pathLen = path.size();
    int seqPos = 0;
    int seqLen = seq.length(); Q_UNUSED(seqLen);
    for(int pathPos = 0; pathPos < pathLen; pathPos++) {
        char c = path[pathPos];
        if (c == 'D') { //gap in seq
            alignedSeq.append((char)U2Msa::GAP_CHAR);
            continue;
        }
        //for 'M' or 'I' insert original char to seq
        char sc = seq[seqPos];
        alignedSeq.append(sc);
        seqPos++;
        if (c =='I') { //insert gap to MSA
            insCoords.append(pathPos);
        }
    }
    assert(seqPos == seqLen); //all seq symbols used
    int aseqLen = alignedSeq.length(); Q_UNUSED(aseqLen);
    int numIns = insCoords.size();
    int ma1Len = ma->getLength(); Q_UNUSED(ma1Len);
    assert(aseqLen == ma1Len + numIns);
    if (numIns != 0) {
        int prevLen = ma->getLength();
        int newLen = prevLen + numIns;
        QByteArray msaPathChangesNew;
        for (int i = 0, n = ma->getNumRows(); i < n; i++) {
            const MultipleSequenceAlignmentRow row = ma->getMsaRow(i);
            QByteArray newSeq;
            newSeq.reserve(newLen);
            int insCoordsPos = insCoords[0];
            int prevInsCoordsPos = -1;
            int insCoordsIdx = 0;
            for (int seqPos = 0; seqPos < prevLen; seqPos++) {
                if (seqPos == insCoordsPos) {
                    do { //add all sequential insertions
                        insCoordsIdx++;
                        prevInsCoordsPos = insCoordsPos;
                        insCoordsPos = insCoordsIdx < numIns ? insCoords[insCoordsIdx] : -1;
                        newSeq.append((char)U2Msa::GAP_CHAR);
                        if (i == 0) {
                            msaPathChangesNew.append('I');
                        }
                    } while (insCoordsPos == prevInsCoordsPos+1);
                }
                newSeq.append(row->charAt(seqPos));
                if (i == 0) {
                    msaPathChangesNew.append(msaPathChanges[seqPos]);
                }
            }
            while (insCoordsIdx!=numIns) {
                insCoordsIdx++;
                newSeq.append((char)U2Msa::GAP_CHAR);
                if (i == 0) {
                    msaPathChangesNew.append('I');
                }
            }
            assert(newSeq.length() == newLen);
            ma->setRowContent(i, newSeq);
        }
        msaPathChanges.clear();
        msaPathChanges+=msaPathChangesNew;
        assert(msaPathChanges.length() == ma->getLength());
    }

    int ma2Len = ma->getLength(); Q_UNUSED(ma2Len);
    assert(aseqLen == ma2Len);  // gapped sequence has the same length as alignment

    U2OpStatus2Log os;
    ma->addRow(name, alignedSeq);
}

void MuscleAdapter::addUnalignedSequencesToProfile(const MultipleSequenceAlignment& ma, const MultipleSequenceAlignment& unalignedSeqs, MultipleSequenceAlignment& res, TaskStateInfo& ti) {
    if(ti.cancelFlag)  {
        return;
    }
    try {
        addUnalignedSequencesToProfileUnsafe(ma, unalignedSeqs, res, ti);
    } catch (const MuscleException &e) {
        if (!ti.cancelFlag) {
            ti.setError(  tr("Internal MUSCLE error: %1").arg(e.str) );
        }
    } catch (const std::bad_alloc &e) {
        ti.setError(getBadAllocError());
    } catch (...) {
        if (!ti.cancelFlag) {
            ti.setError(  tr("Undefined internal MUSCLE error") );
        }
    }
}


void MuscleAdapter::addUnalignedSequencesToProfileUnsafe(const MultipleSequenceAlignment& ma, const MultipleSequenceAlignment& unalignedSeqs, MultipleSequenceAlignment& res, TaskStateInfo& ti) {
    const DNAAlphabet* al = U2AlphabetUtils::deriveCommonAlphabet(ma->getAlphabet(), unalignedSeqs->getAlphabet());
    CHECK_EXT(al != NULL, ti.setError(tr("Incompatible alphabets")), );

    // init muscle
    MuscleContext *ctx = getMuscleContext();

    MuscleParamsHelper ph(ti,ctx);
    SetSeqWeightMethod(ctx->params.g_SeqWeight1);

    setupAlphaAndScore(al, ti);
    CHECK_OP(ti, );

    MSA::SetIdCount(ma->getNumRows() + 1);

    //prepare original MSA
    MSA profileMSA;
    convertMAlignment2MSA(profileMSA, ma, true);

    res = ma->getExplicitCopy();

    //align with input sequences one by one
    Tree tree1;
    gauto_array<ProfPos> prof1(ProfileFromMSALocal_ProfileCPP(profileMSA, tree1));
    QVector<AlignedSeq> alignedSeqs;
    int dp = ti.progress;
    for (int i=0, n = unalignedSeqs->getNumRows(); i < n; i++) {
        ti.setDescription(tr("Aligning sequence %1 of %2").arg(QString::number(i+1)).arg(QString::number(n)));
        ti.progress = dp + i*(95-dp)/n;
        const MultipleSequenceAlignmentRow useq = unalignedSeqs->getMsaRow(i);
        Seq seq;
        seq.FromString(useq->getCore().constData(), useq->getName().toLocal8Bit().constData());
        seq.SetId(0);
        seq.StripGaps();
        seq.FixAlpha();
        MSA  tmpMSA; tmpMSA.FromSeq(seq);

        Tree tree2;
        gauto_array<ProfPos> prof2(ProfileFromMSALocal_ProfileCPP(tmpMSA, tree2));
        gauto_array<ProfPos> profOut;

        PWPath path; unsigned uLengthOut;
        AlignTwoProfs(prof1.data, profileMSA.GetColCount(), 1.0, prof2.get(), tmpMSA.GetColCount(), 1.0, path, &profOut.data, &uLengthOut);

        AlignedSeq aseq;
        aseq.name = useq->getName();
        aseq.seq = QByteArray((const char*)&seq.front(), seq.Length());//without gaps
        aseq.pathToMSA = path2Str(path);
        alignedSeqs.append(aseq);

        if (ti.cancelFlag) {
            return;
        }
    }

    //now add all sequences to the original MSA
    QByteArray changesToOriginalMSA(res->getLength(), 'M');  //path from original MSA to MSA with i seqs aligned
    QByteArray seqPath2CurrentMSA;
    for (int i=0, n = alignedSeqs.size(); i < n; i++) {
        if (i%10 == 9) {
            ti.setDescription(tr("Merging results: %1 of %2").arg(QString::number(i+1)).arg(QString::number(n)));
        }
        seqPath2CurrentMSA.clear();
        const AlignedSeq& aseq = alignedSeqs[i];
        originalMSAToCurrent(changesToOriginalMSA, aseq.pathToMSA, seqPath2CurrentMSA);
        addSequenceToMSA(res, seqPath2CurrentMSA, changesToOriginalMSA, aseq.seq, aseq.name);
        if (ti.cancelFlag) {
            res->clear();
            return;
        }
    }
    res->setAlphabet(al);
}

QString MuscleAdapter::getBadAllocError() {
    static const QString errorX86 = tr("Not enough memory to do this alignment. You can try the 64-bit version of UGENE. In this case, more available memory will be used for aligning.");
    static const QString errorX64 = tr("Not enough memory to do this alignment.");
#ifdef UGENE_X86
    return errorX86;
#else
    return errorX64;
#endif
}

} //namespace
