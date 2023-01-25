/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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
    : Task(name, tf), config(c), callback(cb) {
    assert(config.chunkSize > static_cast<uint>(config.overlapSize));  // if chunk == overlap -> infinite loop occurs
    assert(cb != nullptr);
    assert(config.strandToWalk == StrandOption_DirectOnly || config.complTrans != nullptr);

    maxParallelSubtasks = config.nThreads;
    QList<SequenceDbiWalkerSubtask*> subs = prepareSubtasks();
    foreach (SequenceDbiWalkerSubtask* sub, subs) {
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
            SAFE_POINT_EXT(wholeSeqReg.contains(config.range), stateInfo.setError(QString("Target region out of sequence range")), res);
        }
    }

    if (config.walkCircular && config.range.length == sequenceObject.getSequenceLength()) {
        config.range.length += config.walkCircularDistance * (config.aminoTrans == nullptr ? 1 : 3);
    }

    if (config.aminoTrans == nullptr) {
        // try walk direct and complement strands
        QVector<U2Region> chunks = splitRange(config.range, config.chunkSize, config.overlapSize, config.lastChunkExtraLen, false);

        if (config.strandToWalk == StrandOption_Both || config.strandToWalk == StrandOption_DirectOnly) {
            QList<SequenceDbiWalkerSubtask*> directTasks = createSubs(chunks, false, false);
            res += directTasks;
        }
        if (config.strandToWalk == StrandOption_Both || config.strandToWalk == StrandOption_ComplementOnly) {
            assert(config.complTrans != nullptr);
            QList<SequenceDbiWalkerSubtask*> complTasks = createSubs(chunks, true, false);
            res += complTasks;
        }
    } else {
        // try walk up to 3 direct and 3 complement translation frames.
        int nFrames = config.translateOnlyFirstFrame ? 1 : 3;
        if (config.strandToWalk == StrandOption_Both || config.strandToWalk == StrandOption_DirectOnly) {
            for (int i = 0; i < nFrames; i++) {
                U2Region strandRange(config.range.startPos + i, config.range.length - i);
                QVector<U2Region> chunks = splitRange(strandRange, config.chunkSize, config.overlapSize, config.lastChunkExtraLen, false);
                QList<SequenceDbiWalkerSubtask*> directTasks = createSubs(chunks, false, true);
                res += directTasks;
            }
        }
        if (config.strandToWalk == StrandOption_Both || config.strandToWalk == StrandOption_ComplementOnly) {
            assert(config.complTrans != nullptr);
            for (int i = 0; i < nFrames; i++) {
                U2Region strandRange(config.range.startPos, config.range.length - i);
                QVector<U2Region> chunks = splitRange(strandRange, config.chunkSize, config.overlapSize, config.lastChunkExtraLen, true);
                QList<SequenceDbiWalkerSubtask*> complTasks = createSubs(chunks, true, true);
                res += complTasks;
            }
        }
    }
    return res;
}

QList<SequenceDbiWalkerSubtask*> SequenceDbiWalkerTask::createSubs(const QVector<U2Region>& chunks, bool doCompl, bool doAmino) {
    QList<SequenceDbiWalkerSubtask*> res;
    for (int i = 0, n = chunks.size(); i < n; i++) {
        const U2Region& chunk = chunks[i];
        bool lo = config.overlapSize > 0 && i > 0;
        bool ro = config.overlapSize > 0 && i + 1 < n;
        SequenceDbiWalkerSubtask* t = new SequenceDbiWalkerSubtask(this, chunk, lo, ro, config.seqRef, doCompl, doAmino);
        res.append(t);
    }
    return res;
}

QVector<U2Region> SequenceDbiWalkerTask::splitRange(const U2Region& range, int chunkSize, int overlapSize, int lastChunkExtraLen, bool reverseMode) {
    assert(chunkSize > overlapSize);
    int stepSize = chunkSize - overlapSize;

    QVector<U2Region> res;
    for (int pos = range.startPos, end = range.endPos(), lastPos = range.startPos; lastPos < end; pos += stepSize) {
        int chunkLen = qMin(pos + chunkSize, end) - pos;
        if (end - chunkLen - pos <= lastChunkExtraLen) {
            chunkLen = end - pos;
        }
        lastPos = pos + chunkLen;
        res.append(U2Region(pos, chunkLen));
    }

    if (reverseMode) {
        QVector<U2Region> revertedRegions;
        foreach (const U2Region& r, res) {
            U2Region rr(range.startPos + (range.endPos() - r.endPos()), r.length);
            revertedRegions.prepend(rr);
        }
        res = revertedRegions;
    }
    return res;
}

//////////////////////////////////////////////////////////////////////////
// subtask
SequenceDbiWalkerSubtask::SequenceDbiWalkerSubtask(SequenceDbiWalkerTask* _t, const U2Region& glob, bool lo, bool ro, const U2EntityRef& seqRef, bool _doCompl, bool _doAmino)
    : Task(tr("Sequence walker subtask"), TaskFlag_None),
      t(_t), globalRegion(glob), seqRef(seqRef),
      doCompl(_doCompl), doAmino(_doAmino),
      leftOverlap(lo), rightOverlap(ro) {
    tpm = Task::Progress_Manual;

    // get resources
    QList<TaskResourceUsage> resources = t->getCallback()->getResources(this);
    foreach (const TaskResourceUsage& resource, resources) {
        addTaskResource(resource);
    }
}

const QByteArray& SequenceDbiWalkerSubtask::getRegionSequence() {
    prepareRegionSequence();
    return regionSequence;
}

void SequenceDbiWalkerSubtask::prepareRegionSequence() {
    CHECK(!isRegionSequencePrepared, )
    U2SequenceObject sequenceObject("sequence", seqRef);
    DNASequence dnaSequence = sequenceObject.getSequence(globalRegion, stateInfo);
    CHECK_OP(stateInfo, );

    QByteArray res = dnaSequence.seq;
    if (doCompl) {
        // do complement;
        SAFE_POINT_EXT(t->getConfig().complTrans != nullptr, stateInfo.setError("No complement translation found!"), );
        const QByteArray& complementMap = t->getConfig().complTrans->getOne2OneMapper();
        TextUtils::translate(complementMap, res.data(), res.length());
        TextUtils::reverse(res.data(), res.length());
    }
    if (doAmino) {
        SAFE_POINT_EXT(t->getConfig().complTrans != nullptr, stateInfo.setError("No amino translation found!"), );
        t->getConfig().aminoTrans->translate(res.data(), res.length(), res.data(), res.length());
        int newLen = res.length() / 3;
        res.resize(newLen);
    }
    regionSequence = res;
    isRegionSequencePrepared = true;
}

void SequenceDbiWalkerSubtask::run() {
    assert(!t->hasError());
    t->getCallback()->onRegion(this, stateInfo);
}

const U2Region& SequenceDbiWalkerSubtask::getGlobalRegion() const {
    return globalRegion;
}

SequenceDbiWalkerTask* SequenceDbiWalkerSubtask::getSequenceDbiWalkerTask() const {
    return t;
}

}  // namespace U2
