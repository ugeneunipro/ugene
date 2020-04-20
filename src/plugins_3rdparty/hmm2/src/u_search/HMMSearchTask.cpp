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

#include <hmmer2/funcs.h>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/SequenceWalkerTask.h>

#include "HMMIO.h"
#include "HMMSearchTask.h"
#include "TaskLocalStorage.h"

namespace U2 {

HMMSearchTask::HMMSearchTask(plan7_s* _hmm, const DNASequence& _seq, const UHMMSearchSettings& s)
: Task("", TaskFlag_NoRun),
  hmm(_hmm), seq(_seq), settings(s), complTrans(NULL), aminoTrans(NULL), fName(""), swTask(NULL), readHMMTask(NULL)
{
    setTaskName(tr("HMM search with '%1'").arg(hmm->name));
    GCOUNTER(cvar, tvar, "HMM2 Search");
}

HMMSearchTask::HMMSearchTask(const QString& hFile, const DNASequence& _seq, const UHMMSearchSettings& s)
:Task("", TaskFlag_NoRun),
hmm(NULL), seq(_seq), settings(s), complTrans(NULL), aminoTrans(NULL), fName(hFile), swTask(NULL), readHMMTask(NULL)
{
    setTaskName(tr("HMM Search"));
    GCOUNTER(cvar, tvar, "HMM2 Search");
}

void HMMSearchTask::prepare() {

    if(hasError()) {
        return;
    }

    if(NULL != hmm) {
        swTask = getSWSubtask();
        if(NULL == swTask) {
            assert(hasError());
            return;
        }
        addSubTask(swTask);
    } else {
        readHMMTask = new HMMReadTask(fName);
        addSubTask(readHMMTask);
    }
//     if (!checkAlphabets(hmm->atype, seq.alphabet, complTrans, aminoTrans)) {
//         return;
//     }
//     SequenceWalkerConfig config;
//     config.seq = seq.seq.data();
//     config.seqSize = seq.seq.size();
//     config.complTrans = complTrans;
//     config.strandToWalk = complTrans == NULL ? StrandOption_DirectOnly : StrandOption_Both;
//     config.aminoTrans = aminoTrans;
//     config.overlapSize = 2 * hmm->M;
//     config.chunkSize = qMax(6 * hmm->M, settings.searchChunkSize);
//     if (settings.extraLen == -1) {
//         config.lastChunkExtraLen = config.chunkSize / 2;
//     } else {
//         config.lastChunkExtraLen = settings.extraLen;
//     }
//
//     config.nThreads = MAX_PARALLEL_SUBTASKS_AUTO;
//
//     addSubTask(new SequenceWalkerTask(config, this, tr("parallel_hmm_search_task")));
}


void HMMSearchTask::onRegion(SequenceWalkerSubtask* t, TaskStateInfo& si)
{
    const char* localSeq = t->getRegionSequence();
    int localSeqSize = t->getRegionSequenceLen();
    bool wasCompl = t->isDNAComplemented();
    bool wasAmino = t->isAminoTranslated();
    U2Region globalReg = t->getGlobalRegion();

    //set TLS data
    TaskLocalData::createHMMContext(t->getTaskId(), true);

    QList<UHMMSearchResult> sresults;
    try {
        sresults = UHMMSearch::search(hmm, localSeq, localSeqSize, settings, si);
    } catch (const HMMException &e) {
        stateInfo.setError(e.error);
    }
    if (si.hasError()) {
        stateInfo.setError(si.getError());
    }
    if (sresults.isEmpty()  || stateInfo.cancelFlag || stateInfo.hasError()) {
        TaskLocalData::freeHMMContext(t->getTaskId());
        return;
    }

    //convert all UHMMSearchResults into HMMSearchTaskResult
    QMutexLocker locker(&lock);
    int halfOverlap = hmm->M;
    foreach(const UHMMSearchResult& sr, sresults) {
        HMMSearchTaskResult r;
        r.evalue = sr.evalue;
        r.score = sr.score;
        r.onCompl = wasCompl;
        r.onAmino = wasAmino;
        int resLen   = wasAmino ? sr.r.length * 3 : sr.r.length;
        int resStart = wasAmino ? sr.r.startPos * 3 : sr.r.startPos;
        if (wasCompl) {
            resStart = globalReg.length - resStart - resLen;
        }
        r.r.startPos = globalReg.startPos + resStart;
        r.r.length = resLen;
        if (t->intersectsWithOverlaps(r.r)) {
            //don't add to overlaps if it must be found in 2 regions
            bool add = true;
            if (!r.onCompl && t->hasRightOverlap()) { //check if will be found in a next chunk
                U2Region nextChunkRegion(globalReg.endPos() - halfOverlap, halfOverlap);
                add = !nextChunkRegion.contains(r.r);
            } else if (r.onCompl && t->hasLeftOverlap()) { //check if will found in a prev chunk
                U2Region prevChunkRegion(globalReg.startPos, halfOverlap);
                add = !prevChunkRegion.contains(r.r);
            }
            if (add) {
                r.borderResult = (t->hasLeftOverlap() && r.r.startPos == globalReg.startPos)
                    || (t->hasRightOverlap() && r.r.endPos() == globalReg.endPos());
                overlaps.append(r);
            }
        } else {
            results.append(r);
        }
    }

    TaskLocalData::freeHMMContext(t->getTaskId());
}

static bool HMMSearchResult_LessThan(const HMMSearchTaskResult& r1, const HMMSearchTaskResult& r2) {
    if (r1.evalue == r2.evalue) {
        if (r1.r == r2.r) {
            if (r1.onCompl == r2.onCompl) {
                return &r1 < &r2;
            }
            return r2.onCompl;
        }
        return r1.r < r2.r;
    }
    return r1.evalue < r2.evalue;
}

Task::ReportResult HMMSearchTask::report() {
    if (hasError()) {
        return ReportResult_Finished;
    }

    //postprocess overlaps
    int maxCommonLen = hmm->M / 2; //if 2 results have common part of 'maxCommonLen' or greater -> select best one
    for(int i=0; i < overlaps.count(); i++){
        HMMSearchTaskResult& r1 = overlaps[i];
        if (r1.filtered) {
            continue;
        }
        for(int j=i+1; j < overlaps.count(); j++){
            HMMSearchTaskResult& r2 = overlaps[j];
            if (r2.filtered) {
                continue;
            }
            if (r1.onCompl != r2.onCompl) { //check both regions are on the same strand
                continue;
            }
            if (r1.onAmino) { //check both regions have the same amino frame
                int s1 = r1.onCompl ? r1.r.endPos() % 3 : r1.r.startPos % 3;
                int s2 = r2.onCompl ? r2.r.endPos() % 3 : r2.r.startPos % 3;
                if (s1 != s2) {
                    continue;
                }
            }
            if (r1.r.contains(r2.r) && r1.r != r2.r) {
                r2.filtered = true;
            } else if (r2.r.contains(r1.r) && r1.r != r2.r) {
                r1.filtered = true;
                break;
            } else if (r1.r.intersect(r2.r).length >= maxCommonLen) {
                bool useR1 = r2.score <= r1.score;
                if (r1.score == r2.score && r1.evalue == r2.evalue && r1.borderResult && !r2.borderResult) {
                    useR1 = false;
                }
                if (useR1) {
                    r2.filtered = true;
                } else {
                    r1.filtered = true;
                    break;
                }
            }
        }
    }

    foreach(const HMMSearchTaskResult& r, overlaps) {
        if (!r.filtered) {
            results.append(r);
        }
    }

    // sort results by E-value
    qSort(results.begin(), results.end(), HMMSearchResult_LessThan);
    return ReportResult_Finished;
}

QList<SharedAnnotationData> HMMSearchTask::getResultsAsAnnotations(U2FeatureType type, const QString& name) const {
    QList<SharedAnnotationData>  annotations;
    foreach (const HMMSearchTaskResult &hmmRes, results) {
        SharedAnnotationData a(new AnnotationData);
        a->type = type;
        a->name = name;
        a->setStrand(hmmRes.onCompl ? U2Strand::Complementary : U2Strand::Direct);
        a->location->regions << hmmRes.r;

        QString str; /*add zeros at begin of evalue exponent part, so exponent part must contains 3 numbers*/
        str.sprintf("%.2g", ((double) hmmRes.evalue));
        QRegExp rx("\\+|\\-.+");
        int pos = rx.indexIn(str,0);
        if(pos!=-1){
            str.insert(pos+1,"0");
        }
        QString info = hmm->name;
        if (hmm->flags & PLAN7_ACC) {
            info += QString().sprintf("\nAccession number in PFAM : %s", hmm->acc);
        }
        if (hmm->flags & PLAN7_DESC) {
            info += QString().sprintf("\n%s", hmm->desc);
        }
        if (!info.isEmpty()) {
            a->qualifiers.append(U2Qualifier("HMM-model", info));
        }
        //a->qualifiers.append(U2Qualifier("E-value", QString().sprintf("%.2lg", ((double) hmmRes.evalue))));
        a->qualifiers.append(U2Qualifier("E-value", str));
        a->qualifiers.append(U2Qualifier("Score", QString().sprintf("%.1f", hmmRes.score)));
        annotations.append(a);
    }
    return annotations;
}

bool HMMSearchTask::checkAlphabets(int hmmAlType, const DNAAlphabet* seqAl, DNATranslation*& complTrans, DNATranslation*& aminoTrans)
{
    assert(stateInfo.getError().isEmpty());
    DNAAlphabetType hmmAl = HMMIO::convertHMMAlphabet(hmmAlType);
    if (hmmAl == DNAAlphabet_RAW) {
        stateInfo.setError(tr("Invalid HMM alphabet!"));
        return false;
    }
    if (seqAl->isRaw()) {
        stateInfo.setError(tr("Invalid sequence alphabet!"));
        return false;
    }

    complTrans = NULL;
    aminoTrans = NULL;
    if (seqAl->isNucleic()) {
        DNATranslationRegistry* tr = AppContext::getDNATranslationRegistry();
        DNATranslation* complT = tr->lookupComplementTranslation(seqAl);
        if (complT != NULL) {
            complTrans = complT;
        }
        if (hmmAl == DNAAlphabet_AMINO) {
            QList<DNATranslation*> aminoTs = tr->lookupTranslation(seqAl, DNATranslationType_NUCL_2_AMINO);
            if (!aminoTs.empty()) {
                aminoTrans = tr->getStandardGeneticCodeTranslation(seqAl);
            }
        }
    } else {
        assert(seqAl->isAmino());
    }

    // check the result;
    if (hmmAl == DNAAlphabet_AMINO) {
        if (seqAl->isAmino()) {
            assert(complTrans == NULL && aminoTrans == NULL);
        } else {
            if (aminoTrans == NULL) {
                stateInfo.setError(tr("Amino translation is not available for the sequence alphabet!"));
                return false;
            }
        }
    }

    return true;
}

SequenceWalkerTask* HMMSearchTask::getSWSubtask()
{
    assert(!hasError());
    assert(NULL != hmm);

    if (!checkAlphabets(hmm->atype, seq.alphabet, complTrans, aminoTrans)) {
        return NULL;
    }
    SequenceWalkerConfig config;
    config.seq = seq.seq.data();
    config.seqSize = seq.seq.size();
    config.complTrans = complTrans;
    config.strandToWalk = complTrans == NULL ? StrandOption_DirectOnly : StrandOption_Both;
    config.aminoTrans = aminoTrans;
    config.overlapSize = 2 * hmm->M;
    config.chunkSize = qMax(6 * hmm->M, settings.searchChunkSize);
    if (settings.extraLen == -1) {
        config.lastChunkExtraLen = config.chunkSize / 2;
    } else {
        config.lastChunkExtraLen = settings.extraLen;
    }
    config.walkCircular = false;

    config.nThreads = MAX_PARALLEL_SUBTASKS_AUTO;

    return new SequenceWalkerTask(config, this, tr("Parallel HMM search"));
}

QList< Task* > HMMSearchTask::onSubTaskFinished(Task* subTask)
{
    assert(NULL != subTask);
    QList< Task* > res;
    if(subTask->hasError()) {
        stateInfo.setError(subTask->getError());
        return res;
    }

    if(readHMMTask == subTask) {
        hmm = readHMMTask->getHMM();
        swTask = getSWSubtask();
        if(NULL == swTask) {
            assert(hasError());
            return res;
        }
        res << swTask;
    } else {
        if(swTask != subTask) {
            assert(0 && "undefined_subtask_finished");
        }
    }

    return res;

}

}//endif
