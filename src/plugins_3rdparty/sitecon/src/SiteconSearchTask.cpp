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
#include "SiteconSearchTask.h"

namespace U2 {

SiteconSearchTask::SiteconSearchTask(const SiteconModel& m, const QByteArray& seq, const SiteconSearchCfg& cfg, int ro)
: Task(tr("SITECON search"), TaskFlags_NR_FOSCOE), model(new SiteconModel(m)), cfg(new SiteconSearchCfg(cfg)), resultsOffset(ro), wholeSeq(seq)
{
    lock = new QMutex();
    GCOUNTER( cvar, tvar, "SiteconSearchTask" );
    model->checkState(true);
    model->matrix = SiteconAlgorithm::normalize(model->matrix, model->settings);
    SequenceWalkerConfig c;
    c.seq = wholeSeq.constData();
    c.seqSize = wholeSeq.length();
    c.complTrans  = cfg.complTT;
    c.strandToWalk = cfg.complTT == NULL ? StrandOption_DirectOnly : StrandOption_Both;
    c.aminoTrans = NULL;
    c.walkCircular = false;

    c.chunkSize = seq.length();
    c.overlapSize = 0;

    SequenceWalkerTask* t = new SequenceWalkerTask(c, this, tr("SITECON search parallel subtask"));
    addSubTask(t);
}

void SiteconSearchTask::onRegion(SequenceWalkerSubtask* t, TaskStateInfo& ti) {
//TODO: process border case as if there are 'N' chars before 0 and after seqlen
    if (cfg->complOnly && !t->isDNAComplemented()) {
        return;
    }
    U2Region globalRegion = t->getGlobalRegion();
    qint64 seqLen = globalRegion.length;
    const char* seq = t->getGlobalConfig().seq + globalRegion.startPos;
    int modelSize = model->settings.windowSize;
    ti.progress =0;
    qint64 lenPerPercent = seqLen / 100;
    qint64 pLeft = lenPerPercent;
    DNATranslation* complTT = t->isDNAComplemented() ? t->getGlobalConfig().complTrans : NULL;
    for (int i = 0, n = seqLen - modelSize; i <= n && !ti.cancelFlag; i++, --pLeft) {
        float psum = SiteconAlgorithm::calculatePSum(seq+i, modelSize, model->matrix, model->settings, model->deviationThresh, complTT);
        if (psum < 0 || psum >=1) {
            ti.setError(  tr("Internal error: invalid PSUM: %1").arg(psum) );
            return;
        }
        SiteconSearchResult r;
        r.psum = 100*psum;
        r.err1 = model->err1[r.psum];
        r.err2 = model->err2[r.psum];
        if (r.psum >= cfg->minPSUM && r.err1 >= cfg->minE1 && r.err2 <= cfg->maxE2) {//report result

            r.modelInfo = model->modelName;
            r.strand = t->isDNAComplemented() ? U2Strand::Complementary : U2Strand::Direct;
            r.region.startPos = globalRegion.startPos +  i + resultsOffset;
            r.region.length = modelSize;
            addResult(r);
        }
        if (pLeft == 0) {
            ti.progress++;
            pLeft = lenPerPercent;
        }
    }
}


void SiteconSearchTask::addResult(const SiteconSearchResult& r) {
    lock->lock();
    results.append(r);
    lock->unlock();
}

QList<SiteconSearchResult> SiteconSearchTask::takeResults() {
    lock->lock();
    QList<SiteconSearchResult> res = results;
    results.clear();
    lock->unlock();
    return res;
}

SiteconSearchTask::~SiteconSearchTask() {
    delete cfg;
    delete model;
    delete lock;
}

void SiteconSearchTask::cleanup() {
    results.clear();
    wholeSeq.clear();
    delete cfg;
    delete model;
    delete lock;

    cfg = NULL;
    model = NULL;
    lock = NULL;
}

}//namespace
