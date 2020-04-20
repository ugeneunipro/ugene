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

#include "AssemblyConsensusTask.h"

#include <U2Core/Log.h>
#include <U2Core/Timer.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2OpStatusUtils.h>

namespace U2 {

AssemblyConsensusTask::AssemblyConsensusTask(const AssemblyConsensusTaskSettings &settings_)
    : BackgroundTask<ConsensusInfo>(tr("Calculate assembly consensus"), TaskFlag_None), settings(settings_)
{
    tpm = Progress_Manual;
}

static void doCalculation(const AssemblyConsensusTaskSettings & settings, U2OpStatus & os, ConsensusInfo & result) {
    CHECK_EXT(!settings.consensusAlgorithm.isNull(), os.setError(AssemblyConsensusTask::tr("No consensus algorithm given")),);

    QScopedPointer< U2DbiIterator<U2AssemblyRead> > reads(settings.model->getReads(settings.region, os));
    QByteArray referenceFragment;
    if(settings.model->hasReference()) {
        referenceFragment = settings.model->getReferenceRegion(settings.region, os);
    }
    CHECK_OP(os,);

    result.region = settings.region;
    result.algorithmId = settings.consensusAlgorithm->getId();
    result.consensus = settings.consensusAlgorithm->getConsensusRegion(settings.region, reads.data(), referenceFragment, os);

    os.setProgress(100);
}

void AssemblyConsensusTask::run() {
    GTIMER(c2, t2, "AssemblyConsensusTask::run");
    quint64 t0 = GTimer::currentTimeMicros();

    doCalculation(settings, stateInfo, result);

    CHECK_OP(stateInfo,);
    perfLog.trace(QString("Assembly: '%1' consensus calculation time: %2 seconds")
                  .arg(settings.consensusAlgorithm->getName())
                  .arg((GTimer::currentTimeMicros() - t0) / float(1000*1000)));
}

AssemblyConsensusWorker::AssemblyConsensusWorker(ConsensusSettingsQueue *settingsQueue_)
    : Task(tr("Assembly consensus worker"), TaskFlag_None), settingsQueue(settingsQueue_)
{
    tpm = Progress_Manual;
}

void AssemblyConsensusWorker::run() {
    GTIMER(c2, t2, "AssemblyConsensusTask::run");
    quint64 t0 = GTimer::currentTimeMicros();

    int count = settingsQueue->count();
    int mappingLength = 100/count;
    ConsensusInfo result;

    int completed = 0;
    while(settingsQueue->hasNext()) {
        AssemblyConsensusTaskSettings settings = settingsQueue->getNextSettings();

        U2OpStatusChildImpl os(&stateInfo, U2OpStatusMapping(completed*100/count, mappingLength));
        doCalculation(settings, os, result);
        CHECK_OP(stateInfo,);
        settingsQueue->reportResult(result);

        ++completed;
    }
    stateInfo.setProgress(100);

    perfLog.trace(QString("Assembly: '%1' consensus export time: %2 seconds")
                  .arg(result.algorithmId)
                  .arg((GTimer::currentTimeMicros() - t0) / float(1000*1000)));
}

} //namespace
