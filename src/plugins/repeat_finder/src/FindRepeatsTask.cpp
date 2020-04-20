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

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/CreateAnnotationTask.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/Log.h>
#include <U2Core/TextUtils.h>
#include <U2Core/Timer.h>
#include <U2Core/U1AnnotationUtils.h>

#include "FindRepeatsTask.h"
#include "RFBase.h"
#include "RFConstants.h"
#include "RFDiagonal.h"
#include "RF_SArray_TandemFinder.h"

namespace U2 {

RevComplSequenceTask::RevComplSequenceTask(const DNASequence& s, const U2Region& reg)
: Task(tr("Reverse complement sequence"), TaskFlag_None), sequence(s), region(reg)
{
}

void RevComplSequenceTask::run() {
    DNATranslation* complT = AppContext::getDNATranslationRegistry()->lookupComplementTranslation(sequence.alphabet);
    if (complT == NULL) {
        stateInfo.setError(tr("Can't find complement translation for alphabet: %1").arg(sequence.alphabet->getId()));
        return;
    }
    complementSequence.alphabet = complT->getDstAlphabet();
    complementSequence.seq.resize(region.length);
    const char* src = sequence.constData();
    char* dst = complementSequence.seq.data();

    complT->translate(src + region.startPos, region.length, dst, region.length);
    TextUtils::reverse(dst, region.length);
}

void RevComplSequenceTask::cleanup() {
    sequence.seq.clear();
    complementSequence.seq.clear();
}

FindRepeatsTask::FindRepeatsTask(const FindRepeatsTaskSettings& s, const DNASequence& seq, const DNASequence& seq2)
: Task(tr("Find repeats in a single sequence"), TaskFlags_FOSCOE), settings(s),
seq1(seq), seq2(seq2), tandemTask1(NULL), tandemTask2(NULL)
{
    GCOUNTER(cvar, tvar, "FindRepeatsTask");
    if (settings.seqRegion.length == 0) {
        settings.seqRegion= U2Region(0, seq1.length());
    }
    oneSequence = (seq1.seq == seq2.seq);
    if (oneSequence) {
        settings.seq2Region = settings.seqRegion;
    }else {
        settings.seq2Region = U2Region(0, seq2.length());
    }

    revComplTask = NULL;
    rfTask = NULL;
    startTime = GTimer::currentTimeMicros();
}

void FindRepeatsTask::prepare() {
    if (settings.excludeTandems) {
        FindTandemsTaskSettings s;
        s.minPeriod = 2;
        s.minRepeatCount = 3;
        s.seqRegion = U2Region(0, seq1.length());
        s.nThreads = settings.nThreads;

        tandemTask1 = new FindTandemsToAnnotationsTask(s, seq1);
        addSubTask(tandemTask1);
        if (!oneSequence) {
            tandemTask2 = new FindTandemsToAnnotationsTask(s, seq2);
            addSubTask(tandemTask2);
        }
    } else {
        Task *subTask = createRepeatFinderTask();
        addSubTask(subTask);
    }
}

Task *FindRepeatsTask::createRepeatFinderTask() {
    if (settings.inverted) {
        stateInfo.setDescription(tr("Rev-complementing sequence"));
        assert(seq1.alphabet && seq1.alphabet->isNucleic());
        revComplTask = new RevComplSequenceTask(seq1, settings.seqRegion);
        revComplTask->setSubtaskProgressWeight(0);
        return revComplTask;
    } else {
        rfTask = createRFTask();
        return rfTask;
    }
}

void FindRepeatsTask::filterTandems(const QList<SharedAnnotationData> &tandems, DNASequence &seq) {
    char unknownChar = RFAlgorithmBase::getUnknownChar(seq.alphabet->getType());
    QByteArray gap;

    foreach (const SharedAnnotationData &d, tandems) {
        const QVector<U2Region> &regs = d->getRegions();
        foreach (const U2Region &r, regs) {
            gap.fill(unknownChar, r.length);
            seq.seq.replace(r.startPos, r.length, gap);
        }
    }
}

QList<Task*> FindRepeatsTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    if (hasError() || isCanceled()) {
        return res;
    }
    FindTandemsToAnnotationsTask *t = qobject_cast<FindTandemsToAnnotationsTask*>(subTask);
    if (NULL != t) {
        if (t == tandemTask1) {
            filterTandems(t->getResult(), seq1);
        } else {
            filterTandems(t->getResult(), seq2);
        }
        res << createRepeatFinderTask();
    } else if (subTask == revComplTask) {
        startTime = GTimer::currentTimeMicros();
        rfTask = createRFTask();
        res.append(rfTask);
    }
    return res;
}

RFAlgorithmBase* FindRepeatsTask::createRFTask() {
    stateInfo.setDescription(tr("Searching repeats ..."));

    const char* seqX = seq1.constData() + settings.seqRegion.startPos;
    const char* seqY = revComplTask == NULL ? seqX : revComplTask->complementSequence.constData();
    int seqXLen = settings.seqRegion.length;
    int seqYLen = settings.seqRegion.length;

    if (!oneSequence) {
        seqY = seq2.constData();
        seqYLen = seq2.length();
    }
    RFAlgorithmBase* t = RFAlgorithmBase::createTask(this, seqX, seqXLen, seqY, seqYLen,
        seq1.alphabet, settings.minLen, settings.mismatches, settings.algo, settings.nThreads);

    t->setReportReflected(settings.reportReflected);
    return t;
}

void FindRepeatsTask::run()
{
    if (settings.filter != NoFiltering)
    {
        if (settings.filter == UniqueRepeats)
        {
            stateInfo.setDescription(tr("Filtering unique results"));
            filterUniqueRepeats();
        }
        if (settings.filter == DisjointRepeats)
        {
            stateInfo.setDescription(tr("Filtering nested results"));
            filterNestedRepeats();
        }
    }
}

Task::ReportResult FindRepeatsTask::report() {
    stateInfo.setDescription("");
    if (hasError()) {
        return ReportResult_Finished;
    }
    quint64 endTime = GTimer::currentTimeMicros();
    perfLog.details(tr("Repeat search time %1 sec").arg((endTime-startTime)/(1000*1000.0)));
    return ReportResult_Finished;
}

bool CompareResultLen(RFResult r1, RFResult r2)
{
    return r1.l < r2.l;
}

void FindRepeatsTask::filterUniqueRepeats()
{
    quint64 t1 = GTimer::currentTimeMicros();

    qSort(results.begin(), results.end(), CompareResultLen);

    bool changed = false;
    for (int i=0, n = results.size(); i < n; i++)
    {
        RFResult& ri = results[i];

        for (int j = i+1; j < results.size(); j++)
        {
            int Index = results[j].fragment.indexOf(ri.fragment);
            if (Index != -1)
            {
                changed = true;
                ri.l = -1;
                break;
            }
        }
    }
    int nBefore = results.size();
    if (changed)
    {
        QVector<RFResult> prev = results;
        results.clear();
        foreach(const RFResult& r, prev)
        {
            if (r.l!=-1)
            {
                results.append(r);
            }
        }
    }
    int nAfter = results.size();
    quint64 t2 = GTimer::currentTimeMicros();
    perfLog.details(tr("Unique repeats filtering time %1 sec, results before: %2, filtered: %3, after %4")
        .arg(double((t2-t1))/(1000*1000)).arg(nBefore).arg(nBefore - nAfter).arg(nAfter));
}

void FindRepeatsTask::filterNestedRepeats() {
    //if one repeats fits into another repeat -> filter it
    quint64 t1 = GTimer::currentTimeMicros();

    qSort(results);

    bool changed = false;
    int extraLen = settings.mismatches; //extra len added to repeat region to search for duplicates
    for (int i=0, n = results.size(); i < n; i++) {
        RFResult& ri = results[i];
        if (ri.l == -1) { //this result was filtered
            continue;
        }
        for (int j=i+1; j < n; j++) {
            RFResult& rj = results[j];
            assert(rj.x >= ri.x);
            if (rj.l == -1) {//was filtered
                continue;
            }
            if (rj.x > ri.x + ri.l) { //no more intersection will found with later repeats in first region
                break;
            }

            U2Region ri1(ri.x, ri.l), ri2(ri.y, ri.l), rj1(rj.x, rj.l), rj2(rj.y, rj.l);
            bool filteri = false, filterj = false;
            if (rj.l > ri.l) {
                rj1.startPos-=extraLen; rj1.length+=2*extraLen;
                rj2.startPos-=extraLen; rj2.length+=2*extraLen;
                filteri = rj1.contains(ri1) && rj2.contains(ri2);
            } else {
                ri1.startPos-=extraLen; ri1.length+=2*extraLen;
                ri2.startPos-=extraLen; ri2.length+=2*extraLen;
                filterj = ri1.contains(rj1) && ri2.contains(rj2);
            }
            if (filteri || filterj) {
                changed = true;
                if (filteri) {
                    ri.l = -1;
                    break;
                } else {
                    rj.l = -1;
                }
            }
        }
    }
    int nBefore = results.size();
    if (changed) {
        QVector<RFResult> prev = results;
        results.clear();
        foreach(const RFResult& r, prev) {
            if (r.l!=-1) {
                results.append(r);
            }
        }
    }
    int nAfter = results.size();
    quint64 t2 = GTimer::currentTimeMicros();
    perfLog.details(tr("Nested repeats filtering time %1 sec, results before: %2, filtered: %3, after %4")
        .arg(double((t2-t1))/(1000*1000)).arg(nBefore).arg(nBefore - nAfter).arg(nAfter));
}

void FindRepeatsTask::cleanup() {
    seq1.seq.clear();
    results.clear();
}

void FindRepeatsTask::addResult(const RFResult& r)
{
    int x = r.x + settings.seqRegion.startPos;
    int y = settings.inverted ? settings.seqRegion.endPos() - r.y - r.l : r.y + settings.seq2Region.startPos;
    int l = r.l;
    int c = r.c;
    assert(x >= settings.seqRegion.startPos && x + r.l <= settings.seqRegion.endPos());
    assert(y >= settings.seq2Region.startPos && y + r.l <= settings.seq2Region.endPos());

    int dist = qAbs(x - y) - l;
    if (dist < settings.minDist || dist > settings.maxDist)
    {
        if (dist < 0)
        {
            if (settings.filter == DisjointRepeats)
            {
                // dist < 0 -> overlapping repeat. Try to reduce its length to fit min/max constraints if possible

                // match if prefixes fits dist
                int plen = qAbs(x - y) - settings.minDist;
                if (plen  >= settings.minLen)
                {
                    _addResult(x, y, plen, plen);
                }
                // match if suffixes fits dist
                int dlen = settings.minDist - dist;
                if (l - dlen >= settings.minLen)
                {
                    _addResult(x + dlen, y + dlen, l - dlen, l - dlen);
                }
            }
            else
            {
                _addResult(x, y, l, c);
            }
        }
        return;
    }
    _addResult(x, y, l, c);
}

void FindRepeatsTask::_addResult(int x, int y, int l, int c)
{
    const QByteArray& locDNA = seq1.constSequence();

    if (settings.reportReflected || x <= y)
    {
        QString locFragment = QString(locDNA.mid(x,l));
        results.append(RFResult(x, y, l, c, locFragment));
    }
    else
    {
        QString locFragment = QString(locDNA.mid(y,l));
        results.append(RFResult(y, x, l, c, locFragment));
    }
}

void FindRepeatsTask::onResult(const RFResult& r) {
    if (settings.hasRegionFilters() && isFilteredByRegions(r)) {
        return;
    }
    QMutexLocker ml(&resultsLock);
    addResult(r);
}

void FindRepeatsTask::onResults(const QVector<RFResult>& results) {
    QVector<RFResult> filteredResults = results;
    if (settings.hasRegionFilters()) {
        filteredResults.clear();
        foreach(const RFResult& r, results) {
            if (!isFilteredByRegions(r)) {
                filteredResults.append(r);
            }
        }
    }
    QMutexLocker ml(&resultsLock);
    foreach(const RFResult& r, filteredResults) {
        addResult(r);
    }
}

bool FindRepeatsTask::isFilteredByRegions(const RFResult& r) {
    int x1 = r.x + settings.seqRegion.startPos;
    int y1 = settings.inverted ? settings.seqRegion.endPos() - r.y - 1 : r.y + settings.seqRegion.startPos;

    x1 += settings.reportSeqShift;
    y1 += settings.reportSeq2Shift;

    if (x1 > y1) {
        qSwap(x1, y1);
    }
    int x2 = x1 + r.l;
    int y2 = y1 + r.l;

    //check mid range includes
    if (!settings.midRegionsToInclude.isEmpty()) {
        bool checkOk = false;
        foreach(const U2Region& r, settings.midRegionsToInclude) {
            if (r.startPos >= x2 && r.endPos() <= y1) {
                checkOk = true;
                break;
            }
        }
        if (!checkOk) {
            return true;
        }
    }

    //check mid range excludes
    if (!settings.midRegionsToExclude.isEmpty()) {
        foreach(const U2Region& r, settings.midRegionsToExclude) {
            if (r.intersects(U2Region(x1, y2-x1))) {
                return true;
            }
        }
    }

    //check allowed regions
    if (!settings.allowedRegions.isEmpty()) {
        bool checkOk = false;
        foreach(const U2Region& r, settings.allowedRegions) {
            if (r.startPos <= x1 && r.endPos() >= y2) {
                checkOk = true;
                break;
            }
        }
        if (!checkOk) {
            return true;
        }
    }
    return false;
}

FindRepeatsToAnnotationsTask::FindRepeatsToAnnotationsTask(const FindRepeatsTaskSettings& s, const DNASequence& seq,
                             const QString& _an, const QString& _gn, const QString &annDescription, const GObjectReference& _aor)
: Task(tr("Find repeats to annotations"), TaskFlags_NR_FOSCOE), annName(_an), annGroup(_gn), annDescription(annDescription), annObjRef(_aor), findTask(NULL), settings(s)
{
    setVerboseLogMode(true);
    if (annObjRef.isValid()) {
        LoadUnloadedDocumentTask::addLoadingSubtask(this,
            LoadDocumentTaskConfig(true, annObjRef, new LDTObjectFactory(this)));
    }
    findTask = new FindRepeatsTask(s, seq, seq);
    addSubTask(findTask);
}

QList<Task*> FindRepeatsToAnnotationsTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    if (hasError() || isCanceled()) {
        return res;
    }

    if (subTask == findTask && annObjRef.isValid()) {
        QList<SharedAnnotationData> annotations = importAnnotations();
        if (!annotations.isEmpty()) {
            algoLog.info(tr("Found %1 repeat regions").arg(annotations.size()));
            Task* createTask = new CreateAnnotationsTask(annObjRef, annotations, annGroup);
            createTask->setSubtaskProgressWeight(0);
            res.append(createTask);
        }
    }
    return res;
}

QList<SharedAnnotationData> FindRepeatsToAnnotationsTask::importAnnotations() {
    QList<SharedAnnotationData> res;
    foreach (const RFResult &r, findTask->getResults()) {
        SharedAnnotationData ad(new AnnotationData);
        ad->type = U2FeatureTypes::RepeatRegion;
        ad->name = annName;
        U2Region l1(r.x + settings.reportSeqShift, r.l);
        U2Region l2(r.y + settings.reportSeq2Shift, r.l);
        if (l1.startPos <= l2.startPos) {
            ad->location->regions << l1 << l2;
        } else {
            ad->location->regions << l2 << l1;
        }
        int dist = qAbs(r.x - r.y) - r.l;
        if (findTask->getSettings().inverted) {
            ad->qualifiers.append(U2Qualifier("rpt_type", "inverted"));
        }
        ad->qualifiers.append(U2Qualifier("repeat_len", QString::number(r.l)));
        ad->qualifiers.append(U2Qualifier("repeat_dist", QString::number(dist)));
        ad->qualifiers.append(U2Qualifier("repeat_identity", QString::number(settings.getIdentity(r.l - r.c, r.l))));
        U1AnnotationUtils::addDescriptionQualifier(ad, annDescription);

        res.append(ad);
    }
    return res;
}

}//namespace

