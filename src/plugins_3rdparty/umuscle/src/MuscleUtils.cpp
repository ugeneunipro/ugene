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

/// various helper methods and structures
#include "MuscleUtils.h"

#include <U2Core/DNAAlphabet.h>
#include <U2Core/U2OpStatusUtils.h>

#include "MuscleAdapter.h"

#include <algorithm>
#include <QVector>

namespace U2 {

MuscleParamsHelper::MuscleParamsHelper(TaskStateInfo& ti, MuscleContext *_ctx)
: ctx(_ctx), ugeneFileStub(ti)
{
    SetParams();

    //override some params. TODO: recheck possible conflict with SetPPScore() !
    ctx->progressPercent = &ti.progress;
    ctx->cancelFlag = &ti.cancelFlag;
    ctx->progress.g_fProgress = &ugeneFileStub; //log context
    ctx->progress.pr_printf = ugene_printf; //overriding logging
    ctx->params.g_uMaxMB = 0; //unlimited memory

    SetMaxIters(ctx->params.g_uMaxIters);
    SetStartTime();
}

MuscleParamsHelper::~MuscleParamsHelper() {
    ctx->cancelFlag = &ctx->cancelStub;
    ctx->progressPercent = &ctx->progressStub;
    ctx->progress.pr_printf = fprintf;
    ctx->progress.g_fProgress = NULL;
}

int ugene_printf(FILE *f, const char *format, ...) {
    if (format[0] <= 31 || strlen(format) == 1) {
        return 0;
    }
    char str[1024];
    va_list ArgList;
    va_start(ArgList, format);
    int n = vsprintf(str, format, ArgList);
    assert(n>=0 && n < 1024);
    Q_UNUSED(n);
    va_end(ArgList);

    FILEStub* s = static_cast<FILEStub*>(f);
    s->tsi.setDescription(QString::fromLatin1(str));
    return 0;
}

ALPHA convertAlpha(const DNAAlphabet* al) {
    if (al->isAmino()) {
        return ALPHA_Amino;
    }
    const QString& id = al->getId();
    if (id == BaseDNAAlphabetIds::NUCL_DNA_DEFAULT() || id == BaseDNAAlphabetIds::NUCL_DNA_EXTENDED()) {
        return ALPHA_DNA;
    }
    if (id == BaseDNAAlphabetIds::NUCL_RNA_DEFAULT() || id == BaseDNAAlphabetIds::NUCL_RNA_EXTENDED()) {
        return ALPHA_RNA;
    }
    return ALPHA_Undefined;
}

void setupAlphaAndScore(const DNAAlphabet* al, TaskStateInfo& ti) {
    ALPHA Alpha = convertAlpha(al);
    if (Alpha == ALPHA_Undefined) {
        ti.setError(  U2::MuscleAdapter::tr("Unsupported alphabet: %1").arg(al->getName()) );
        return;
    }
    SetAlpha(Alpha);
    SetPPScore();
    if (ALPHA_DNA == Alpha || ALPHA_RNA == Alpha) {
        SetPPScore(PPSCORE_SPN);
    }
}

void convertMAlignment2MSA(MSA& muscleMSA, const MultipleSequenceAlignment& ma, bool fixAlpha) {    
    MuscleContext *ctx = getMuscleContext();
    ctx->fillUidsVectors(ma->getNumRows());
    for (int i=0, n = ma->getNumRows(); i<n; i++) {
        const MultipleSequenceAlignmentRow row = ma->getMsaRow(i);
        
        int coreLen = row->getCoreLength();
        int maLen = ma->getLength();
        char* seq  = new char[maLen + 1];
        memcpy(seq, row->getCore().constData(), coreLen);
        memset(seq + coreLen, '-', maLen - coreLen + 1);
        seq[maLen] = 0;

        char* name = new char[row->getName().length() + 1];
        memcpy(name, row->getName().toLocal8Bit().constData(), row->getName().length());
        name[row->getName().length()] = '\0';
        
        muscleMSA.AppendSeq(seq, maLen, name);
        ctx->tmp_uIds[i] = ctx->input_uIds[i];
    }
    if (fixAlpha) {
        muscleMSA.FixAlpha();
    }
}

void convertMAlignment2SecVect(SeqVect& sv, const MultipleSequenceAlignment& ma, bool fixAlpha) {
    sv.Clear();

    MuscleContext *ctx = getMuscleContext();
    ctx->fillUidsVectors(ma->getNumRows());

    unsigned i=0;
    unsigned seq_count = 0;
    foreach(const MultipleSequenceAlignmentRow& row, ma->getMsaRows()) {
        Seq *ptrSeq = new Seq();
        QByteArray name =  row->getName().toLocal8Bit();
        ptrSeq->FromString(row->getCore().constData(), name.constData());
        //stripping gaps, original Seq::StripGaps fails on MSVC9
        Seq::iterator newEnd = std::remove(ptrSeq->begin(), ptrSeq->end(), U2Msa::GAP_CHAR);
        ptrSeq->erase(newEnd, ptrSeq->end());
        if (ptrSeq->Length()!=0) {
            ctx->tmp_uIds[seq_count] = ctx->input_uIds[i];
            sv.push_back(ptrSeq);
            seq_count++; 
        }
        i++;
    }
    if (fixAlpha) {
        sv.FixAlpha();
    }
}

void convertMSA2MAlignment(MSA& msa, const DNAAlphabet* al, MultipleSequenceAlignment& res) {
    assert(res->isEmpty());
    MuscleContext *ctx = getMuscleContext();
    res->setAlphabet(al);
    ctx->output_uIds.clear();

    for(int i=0, n = msa.GetSeqCount(); i < n; i++) {
        QString name = msa.GetSeqName(i);
        QByteArray seq;
        seq.reserve(msa.GetColCount());
        for (int j = 0, m = msa.GetColCount(); j < m ; j++) {
            char c = msa.GetChar(i, j);
            seq.append(c);
        }
        ctx->output_uIds.append(ctx->tmp_uIds[msa.GetSeqId(i)]);
        res->addRow(name, seq);
    }
}

void prepareAlignResults(MSA& msa, const DNAAlphabet* al, MultipleSequenceAlignment& ma, bool mhack) {
    if (mhack) {
        MHackEnd(msa);
    }
    MuscleContext* ctx = getMuscleContext();
    if (ctx->params.g_bStable) {
        MSA msaStable;
        Stabilize(msa, msaStable);
        msa.Clear();
        convertMSA2MAlignment(msaStable, al, ma);
    } else {
        convertMSA2MAlignment(msa, al, ma);
    }
}

} //namespace
