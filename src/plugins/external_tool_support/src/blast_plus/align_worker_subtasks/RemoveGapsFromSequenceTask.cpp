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
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/MsaDbiUtils.h>
#include <U2Core/UserApplicationsSettings.h>

#include "RemoveGapsFromSequenceTask.h"

namespace U2 {

FindGapsInSequenceCallback::FindGapsInSequenceCallback(U2SequenceObject * const sequenceObject)
    : sequenceObject(sequenceObject)
{
    SAFE_POINT(NULL != sequenceObject, "Sequence object is NULL", );
}

void FindGapsInSequenceCallback::onRegion(SequenceDbiWalkerSubtask *subtask, TaskStateInfo &stateInfo) {
    CHECK_OP(stateInfo, );

    const QByteArray sequenceData = sequenceObject->getSequenceData(subtask->getGlobalRegion(), stateInfo);
    CHECK_OP(stateInfo, );

    QByteArray ungappedSequenceData;
    QList<U2MsaGap> gaps;
    MaDbiUtils::splitBytesToCharsAndGaps(sequenceData, ungappedSequenceData, gaps);
    MsaRowUtils::shiftGapModel(gaps, subtask->getGlobalRegion().startPos);

    addGaps(gaps);
}

const QList<U2Region> &FindGapsInSequenceCallback::getGappedRegions() const {
    return gappedRegions;
}

void FindGapsInSequenceCallback::addGaps(const QList<U2MsaGap> &gaps) {
    QMutexLocker mutexLocker(&mutex);
    Q_UNUSED(mutex);
    foreach (const U2MsaGap &gap, gaps) {
        gappedRegions << gap;
    }
}

RemoveGapsFromSequenceTask::RemoveGapsFromSequenceTask(U2SequenceObject * const sequenceObject)
    : Task(tr("Remove gaps from the sequence"), TaskFlags_FOSE_COSC),
      sequenceObject(sequenceObject),
      callback(sequenceObject),
      findGapsTask(NULL)
{
    SAFE_POINT_EXT(NULL != sequenceObject, setError("Sequence object is NULL"), );
}

void RemoveGapsFromSequenceTask::prepare() {
    SequenceDbiWalkerConfig config;
    config.seqRef = sequenceObject->getEntityRef();
    config.chunkSize = CHUNK_SIZE;
    config.overlapSize = 0;
    config.nThreads = AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount();
    config.strandToWalk = StrandOption_DirectOnly;
    config.walkCircular = false;

    findGapsTask = new SequenceDbiWalkerTask(config, &callback, tr("Find gaps in the sequence"));
    addSubTask(findGapsTask);
}

void RemoveGapsFromSequenceTask::run() {
    QList<U2Region> gappedRegions = callback.getGappedRegions();
    for (int i = gappedRegions.size() - 1; i >= 0; i--) {
        sequenceObject->removeRegion(stateInfo, gappedRegions[i]);
        CHECK_OP(stateInfo, );
    }
}

}   // namespace U2
