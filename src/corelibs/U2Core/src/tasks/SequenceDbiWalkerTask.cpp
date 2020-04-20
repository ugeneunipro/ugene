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

#include "SequenceDbiWalkerTask.h"

#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

SequenceDbiWalkerTask::SequenceDbiWalkerTask(const SequenceDbiWalkerConfig& c, SequenceDbiWalkerCallback* cb, const QString& name, TaskFlags tf)
: Task(name, tf), config(c), callback(cb)
{
    assert(config.chunkSize > static_cast<uint>(config.overlapSize)); // if chunk == overlap -> infinite loop occurs
    assert(cb != NULL);
    assert(config.strandToWalk == StrandOption_DirectOnly || config.complTrans!=NULL);

    maxParallelSubtasks = config.nThreads;
    QList<SequenceDbiWalkerSubtask*> subs = prepareSubtasks();
    foreach(SequenceDbiWalkerSubtask* sub, subs) {
        addSubTask(sub);
    }
}

QList<SequenceDbiWalkerSubtask*> SequenceDbiWalkerTask::prepareSubtasks() {
    QList<SequenceDbiWalkerSubtask*> res;

    U2SequenceObject sequenceObject("sequence", config.seqRef);

    if (config.range.isEmpty()) {
        config.range = U2Region(0, sequenceObject.getSequenceLength());
    } else {
        U2Region wholeSeqReg(0, sequenceObject.getSequenceLength());
        if (!config.walkCircular) {
            SAFE_POINT_EXT(wholeSeqReg.contains(config.range), stateInfo.setError(tr("Target region out of sequence range")), res);
        }
    }

    if (config.walkCircular && config.range.length == sequenceObject.getSequenceLength()) {
        config.range.length += config.walkCircularDistance * (config.aminoTrans == NULL ? 1 : 3);
    }

    if (config.aminoTrans == NULL ) {
        //try walk direct and complement strands
        QVector<U2Region> chunks = splitRange(config.range, config.chunkSize, config.overlapSize, config.lastChunkExtraLen, false);

        if (config.strandToWalk == StrandOption_Both || config.strandToWalk == StrandOption_DirectOnly) {
            QList<SequenceDbiWalkerSubtask*> directTasks  = createSubs(chunks, false, false);
            res+=directTasks;
        }
        if (config.strandToWalk == StrandOption_Both || config.strandToWalk == StrandOption_ComplementOnly) {
            assert(config.complTrans!=NULL);
            QList<SequenceDbiWalkerSubtask*> complTasks = createSubs(chunks, true, false);
            res+=complTasks;
        }
    } else {
        // try walk 3 direct and 3 complement translations
        if (config.strandToWalk == StrandOption_Both || config.strandToWalk == StrandOption_DirectOnly) {
            for (int i=0; i<3; i++) {
                U2Region strandRange(config.range.startPos + i, config.range.length - i);
                QVector<U2Region> chunks = splitRange(strandRange, config.chunkSize, config.overlapSize, config.lastChunkExtraLen, false);
                QList<SequenceDbiWalkerSubtask*> directTasks = createSubs(chunks, false, true);
                res+=directTasks;
            }
        }
        if (config.strandToWalk == StrandOption_Both || config.strandToWalk == StrandOption_ComplementOnly) {
            assert(config.complTrans!=NULL);
            for (int i=0; i<3; i++) {
                U2Region strandRange(config.range.startPos, config.range.length - i);
                QVector<U2Region> chunks = splitRange(strandRange, config.chunkSize, config.overlapSize, config.lastChunkExtraLen, true);
                QList<SequenceDbiWalkerSubtask*> complTasks = createSubs(chunks, true, true);
                res+=complTasks;
            }
        }
    }
    return res;
}

QList<SequenceDbiWalkerSubtask*> SequenceDbiWalkerTask::createSubs(const QVector<U2Region>& chunks, bool doCompl, bool doAmino) {
    QList<SequenceDbiWalkerSubtask*> res;
    for (int i=0, n = chunks.size(); i<n; i++) {
        const U2Region& chunk = chunks[i];
        bool lo = config.overlapSize > 0 && i > 0;
        bool ro = config.overlapSize > 0 && i+1 < n;
        SequenceDbiWalkerSubtask* t = new SequenceDbiWalkerSubtask(this, chunk, lo, ro, config.seqRef, chunk.length, doCompl, doAmino);
        res.append(t);
    }
    return res;
}

QVector<U2Region> SequenceDbiWalkerTask::splitRange(const U2Region& range, int chunkSize, int overlapSize,
                                              int lastChunkExtraLen, bool reverseMode)
{
    assert(chunkSize > overlapSize);
    int stepSize = chunkSize - overlapSize;

    QVector<U2Region> res;
    for (int pos = range.startPos, end = range.endPos(), lastPos = range.startPos; lastPos < end; pos+=stepSize) {
        int chunkLen = qMin(pos + chunkSize, end) - pos;
        if (end - chunkLen - pos <= lastChunkExtraLen) {
            chunkLen = end-pos;
        }
        lastPos = pos+chunkLen;
        res.append(U2Region(pos, chunkLen));
    }

    if (reverseMode) {
        QVector<U2Region> revertedRegions;
        foreach(const U2Region& r, res) {
            U2Region rr(range.startPos + (range.endPos() - r.endPos()), r.length);
            revertedRegions.prepend(rr);
        }
        res = revertedRegions;
    }
    return res;
}

//////////////////////////////////////////////////////////////////////////
// subtask
SequenceDbiWalkerSubtask::SequenceDbiWalkerSubtask(SequenceDbiWalkerTask* _t, const U2Region& glob, bool lo, bool ro, const U2EntityRef& seqRef, int _len, bool _doCompl, bool _doAmino)
: Task(tr("Sequence walker subtask"), TaskFlag_None),
t(_t), globalRegion(glob), seqRef(seqRef), localSeq(NULL), originalLocalSeq(NULL),
localLen(_len), originalLocalLen(_len), doCompl(_doCompl), doAmino(_doAmino),
leftOverlap(lo), rightOverlap(ro)
{
    tpm = Task::Progress_Manual;

    // get resources
    QList< TaskResourceUsage > resources = t->getCallback()->getResources( this );
    foreach( const TaskResourceUsage & resource, resources ) {
        addTaskResource(resource);
    }
}

const char* SequenceDbiWalkerSubtask::getRegionSequence() {
    if (needLocalRegionProcessing()) {
        prepareLocalRegion();
    }
    return localSeq;
}

int SequenceDbiWalkerSubtask::getRegionSequenceLen() {
    if (needLocalRegionProcessing()) {
        prepareLocalRegion();
    }
    return localLen;
}

void SequenceDbiWalkerSubtask::prepareLocalRegion() {
    assert(doAmino || doCompl);

    QByteArray res(localSeq, localLen);
    if (doCompl) {
        //do complement;
        assert(t->getConfig().complTrans!=NULL);
        const QByteArray& complementMap = t->getConfig().complTrans->getOne2OneMapper();
        TextUtils::translate(complementMap, res.data(), res.length());
        TextUtils::reverse(res.data(), res.length());
    }
    if (doAmino) {
        assert(t->getConfig().aminoTrans!=NULL && t->getConfig().aminoTrans->isThree2One());
        t->getConfig().aminoTrans->translate(res.data(), res.length(), res.data(), res.length());
        int newLen = res.length()/3;
        res.resize(newLen);
    }
    processedSeqImage = res;
    localLen = processedSeqImage.size();
    localSeq = processedSeqImage.constData();
}

void SequenceDbiWalkerSubtask::run() {
    assert(!t->hasError());
    t->getCallback()->onRegion(this, stateInfo);
}

bool SequenceDbiWalkerSubtask::intersectsWithOverlaps(const U2Region& reg) const {
    int overlap = getGlobalConfig().overlapSize;
    if (overlap == 0) {
        return false;
    }
    bool intersects = false;
    if (leftOverlap) {
        intersects = reg.intersects(U2Region(globalRegion.startPos, overlap));
    }
    if (!intersects && rightOverlap) {
        intersects = reg.intersects(U2Region(globalRegion.endPos() - overlap, overlap));
    }
    return intersects;
}

} //namespace
