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

#include <U2Core/Counter.h>

#include "WeightMatrixSearchTask.h"

namespace U2 {
//Weight matrix multiple search
WeightMatrixSearchTask::WeightMatrixSearchTask(const QList<QPair<PWMatrix,WeightMatrixSearchCfg> > &m, const QByteArray& seq, int ro)
: Task(tr("Weight matrix multiple search"), TaskFlags_NR_FOSCOE), models(m), resultsOffset(ro)
{
    for (int i = 0, n = m.size(); i < n; i++) {
        addSubTask(new WeightMatrixSingleSearchTask(m[i].first, seq, m[i].second, ro));
    }
}

void WeightMatrixSearchTask::addResult(const WeightMatrixSearchResult& r) {
    lock.lock();
    results.append(r);
    lock.unlock();
}

QList<WeightMatrixSearchResult> WeightMatrixSearchTask::takeResults() {
    lock.lock();
    QList<WeightMatrixSearchResult> res;
    foreach (const QPointer<Task> &task, getSubtasks()) {
        WeightMatrixSingleSearchTask* curr = static_cast<WeightMatrixSingleSearchTask*>(task.data());
        res.append(curr->takeResults());
    }
    lock.unlock();
    return res;
}

//Weight matrix single search
WeightMatrixSingleSearchTask::WeightMatrixSingleSearchTask(const PWMatrix& m, const QByteArray& _seq, const WeightMatrixSearchCfg& cfg, int ro)
: Task(tr("Weight matrix search"), TaskFlags_NR_FOSCOE), model(m), cfg(cfg), resultsOffset(ro), seq(_seq)
{
    GCOUNTER( cvar, tvar, "WeightMatrixSingleSearchTask" );
    SequenceWalkerConfig c;
    c.walkCircular = false;
    c.seq = seq.constData();
    c.seqSize = seq.length();
    c.complTrans  = cfg.complTT;
    c.strandToWalk = cfg.complTT == NULL ? StrandOption_DirectOnly : StrandOption_Both;
    c.aminoTrans = NULL;

    c.chunkSize = seq.length();
    c.overlapSize = 0;

    SequenceWalkerTask* t = new SequenceWalkerTask(c, this, tr("Weight matrix search parallel"));
    addSubTask(t);
}

void WeightMatrixSingleSearchTask::onRegion(SequenceWalkerSubtask* t, TaskStateInfo& ti) {
//TODO: process border case as if there are 'N' chars before 0 and after seqlen
    if (cfg.complOnly && !t->isDNAComplemented()) {
        return;
    }
    U2Region globalRegion = t->getGlobalRegion();
    int seqLen = globalRegion.length;
    const char* seq = t->getGlobalConfig().seq + globalRegion.startPos;
    int modelSize = model.getLength();
    ti.progress =0;
    int lenPerPercent = seqLen / 100;
    int pLeft = lenPerPercent;
    DNATranslation* complTT = t->isDNAComplemented() ? t->getGlobalConfig().complTrans : NULL;
    for (int i = 0, n = seqLen - modelSize; i <= n && !ti.cancelFlag; i++, --pLeft) {
        float psum = WeightMatrixAlgorithm::getScore(seq + i, modelSize, model, complTT);
        if (psum < -1e-6 || psum > 1 + 1e-6) {
            ti.setError(  tr("Internal error invalid psum: %1").arg(psum) );
            return;
        }
        WeightMatrixSearchResult r;
        r.score = 100*psum;
        if (r.score >= cfg.minPSUM) {//report result
            r.region.startPos = globalRegion.startPos +  i + resultsOffset;
            if(t->isDNAComplemented()){
                r.strand = U2Strand::Complementary;
                r.region.startPos += 1;
            }else{
                r.strand = U2Strand::Direct;
            }
            r.region.length = modelSize;
            r.qual = model.getProperties();
            r.modelInfo = cfg.modelName.split("/").last();
            addResult(r);
        }
        if (pLeft == 0) {
            ti.progress++;
            pLeft = lenPerPercent;
        }
    }
}


void WeightMatrixSingleSearchTask::addResult(const WeightMatrixSearchResult& r) {
    lock.lock();
    results.append(r);
    lock.unlock();
}

QList<WeightMatrixSearchResult> WeightMatrixSingleSearchTask::takeResults() {
    lock.lock();
    QList<WeightMatrixSearchResult> res = results;
    results.clear();
    lock.unlock();
    return res;
}

}//namespace
