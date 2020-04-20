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

#include "SequenceWalkerTask.h"

#include <U2Core/DNATranslation.h>
#include <U2Core/TextUtils.h>

namespace U2 {

SequenceWalkerConfig::SequenceWalkerConfig()
: seq(NULL), seqSize(0), complTrans(NULL), aminoTrans(NULL),
        chunkSize(0), lastChunkExtraLen(0), overlapSize(0), nThreads(MAX_PARALLEL_SUBTASKS_SERIAL),
        walkCircular(false), walkCircularDistance(0)
{
    strandToWalk = (complTrans!=NULL) ? StrandOption_Both : StrandOption_DirectOnly;
}


SequenceWalkerTask::SequenceWalkerTask(const SequenceWalkerConfig& c, SequenceWalkerCallback* cb,
                                       const QString& name, TaskFlags tf)
    : Task(name, tf),
      config(c),
      callback(cb),
      tempBuffer(NULL)
{
    assert(config.chunkSize > static_cast<uint>(config.overlapSize)); // if chunk == overlap -> infinite loop occurs
    assert(cb != NULL);
    assert(config.strandToWalk == StrandOption_DirectOnly || config.complTrans!=NULL);

    maxParallelSubtasks = config.nThreads;
    QList<SequenceWalkerSubtask*> subs = prepareSubtasks();
    foreach(SequenceWalkerSubtask* sub, subs) {
        addSubTask(sub);
    }
}

QList<SequenceWalkerSubtask*> SequenceWalkerTask::prepareSubtasks() {
    QList<SequenceWalkerSubtask*> res;

    if (config.range.isEmpty()) {
        config.range.startPos = 0;
        config.range.length = config.seqSize;
    } else {
        U2Region wholeSeqReg(0, config.seqSize);
        assert(wholeSeqReg.contains(config.range));
        config.range = wholeSeqReg.intersect(config.range);
    }

    if (config.walkCircular && static_cast<quint64>(config.range.length) == config.seqSize) {
        tempBuffer.clear();
        tempBuffer.append(QByteArray(config.seq, config.seqSize));
        tempBuffer.append(QByteArray(config.seq).left(config.walkCircularDistance * (config.aminoTrans == NULL ? 1 : 3)));

        config.seq = tempBuffer.constData();
        config.range.length += config.walkCircularDistance * (config.aminoTrans == NULL ? 1 : 3);
    }

    if (config.aminoTrans == NULL ) {
        //try walk direct and complement strands
        QVector<U2Region> chunks = splitRange(config.range, config.chunkSize, config.overlapSize, config.lastChunkExtraLen, false);

        if (config.strandToWalk == StrandOption_Both || config.strandToWalk == StrandOption_DirectOnly) {
            QList<SequenceWalkerSubtask*> directTasks  = createSubs(chunks, false, false);
            res+=directTasks;
        }
        if (config.strandToWalk == StrandOption_Both || config.strandToWalk == StrandOption_ComplementOnly) {
            assert(config.complTrans!=NULL);
            QList<SequenceWalkerSubtask*> complTasks = createSubs(chunks, true, false);
            res+=complTasks;
        }
    } else {
        // in case of amino walk (chunk - overlap) should be devisible by 3
        if ( (config.chunkSize - config.overlapSize) % 3 != 0 && config.overlapSize != 0) {
            config.chunkSize += 3 - (config.chunkSize - config.overlapSize) % 3;
        }

        // try walk 3 direct and 3 complement translations
        if (config.strandToWalk == StrandOption_Both || config.strandToWalk == StrandOption_DirectOnly) {
            for (int i=0; i<3; i++) {
                U2Region strandRange(config.range.startPos + i, config.range.length - i);
                QVector<U2Region> chunks = splitRange(strandRange, config.chunkSize, config.overlapSize, config.lastChunkExtraLen, false);
                QList<SequenceWalkerSubtask*> directTasks = createSubs(chunks, false, true);
                res+=directTasks;
            }
        }
        if (config.strandToWalk == StrandOption_Both || config.strandToWalk == StrandOption_ComplementOnly) {
            assert(config.complTrans!=NULL);
            for (int i=0; i<3; i++) {
                U2Region strandRange(config.range.startPos, config.range.length - i);
                QVector<U2Region> chunks = splitRange(strandRange, config.chunkSize, config.overlapSize, config.lastChunkExtraLen, true);
                QList<SequenceWalkerSubtask*> complTasks = createSubs(chunks, true, true);
                res+=complTasks;
            }
        }
    }
    return res;
}

QList<SequenceWalkerSubtask*> SequenceWalkerTask::createSubs(const QVector<U2Region>& chunks, bool doCompl, bool doAmino) {
    QList<SequenceWalkerSubtask*> res;
    for (int i=0, n = chunks.size(); i<n; i++) {
        const U2Region& chunk = chunks[i];
        bool lo = config.overlapSize > 0 && i > 0;
        bool ro = config.overlapSize > 0 && i+1 < n;
        SequenceWalkerSubtask* t = new SequenceWalkerSubtask(this, chunk, lo, ro, config.seq + chunk.startPos, chunk.length, doCompl, doAmino);
        res.append(t);
    }
    return res;
}

QVector<U2Region> SequenceWalkerTask::splitRange(const U2Region& range, int chunkSize, int overlapSize,
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
SequenceWalkerSubtask::SequenceWalkerSubtask(SequenceWalkerTask* _t, const U2Region& glob, bool lo, bool ro, const char* _seq, int _len, bool _doCompl, bool _doAmino)
: Task(tr("Sequence walker subtask"), TaskFlag_None),
t(_t), globalRegion(glob), localSeq(_seq), originalLocalSeq(_seq),
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

const char* SequenceWalkerSubtask::getRegionSequence() {
    if (needLocalRegionProcessing()) {
        prepareLocalRegion();
    }
    return localSeq;
}

int SequenceWalkerSubtask::getRegionSequenceLen() {
    if (needLocalRegionProcessing()) {
        prepareLocalRegion();
    }
    return localLen;
}

void SequenceWalkerSubtask::prepareLocalRegion() {
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

void SequenceWalkerSubtask::run() {
    assert(!t->hasError());
    t->getCallback()->onRegion(this, stateInfo);
}

bool SequenceWalkerSubtask::intersectsWithOverlaps(const U2Region& reg) const {
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
