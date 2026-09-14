/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

AssemblyConsensusTask::AssemblyConsensusTask(const AssemblyConsensusTaskSettings& settings_)
    : BackgroundTask<ConsensusInfo>(tr("Calculate assembly consensus"), TaskFlag_None), settings(settings_) {
    tpm = Progress_Manual;
}

/**
 * Reserves a column for every read insertion of the region and finds the consensus of those
 * columns. The reads are walked twice, because the statistics of a column can only be counted
 * once it is known which columns are there, and they are streamed rather than listed: a region of
 * the export can easily hold more reads than fit into memory at a time.
 *
 * One base of overlap is added on the left. An insertion belongs to the reference position it
 * precedes, and U2AssemblyReadIterator only reports it after the position before it has been
 * walked through: without the overlap a read that starts earlier would keep the insertion of the
 * very first position of the region to itself. The export splits a long assembly into regions of
 * its own, so the insertions in front of those splits would be lost with no reason the caller
 * could see. The extra position is only scanned, never reported: the sites of the region itself
 * are what the caller asks the map about.
 */
static void calculateInsertions(const AssemblyConsensusTaskSettings& settings, U2OpStatus& os, ConsensusInfo& result) {
    qint64 scanStart = qMax<qint64>(0, settings.region.startPos - 1);
    U2Region regionToScan(scanStart, settings.region.endPos() - scanStart);
    result.insertions.startBuild(regionToScan);

    QScopedPointer<U2DbiIterator<U2AssemblyRead>> siteReads(settings.model->getReads(regionToScan, os));
    CHECK_OP(os, );
    while (siteReads->hasNext() && !os.isCoR()) {
        result.insertions.addSitesOfRead(siteReads->next());
    }
    result.insertions.finishSites();
    CHECK_OP(os, );
    CHECK(!result.insertions.isEmpty(), );

    QScopedPointer<U2DbiIterator<U2AssemblyRead>> statisticsReads(settings.model->getReads(regionToScan, os));
    CHECK_OP(os, );
    while (statisticsReads->hasNext() && !os.isCoR()) {
        result.insertions.addStatisticsOfRead(statisticsReads->next());
    }
    result.insertions.finishStatistics();
}

static void doCalculation(const AssemblyConsensusTaskSettings& settings, U2OpStatus& os, ConsensusInfo& result) {
    CHECK_EXT(!settings.consensusAlgorithm.isNull(), os.setError(AssemblyConsensusTask::tr("No consensus algorithm given")), );

    QScopedPointer<U2DbiIterator<U2AssemblyRead>> reads(settings.model->getReads(settings.region, os));
    QByteArray referenceFragment;
    if (settings.model->hasReference()) {
        referenceFragment = settings.model->getReferenceRegion(settings.region, os);
    }
    CHECK_OP(os, );

    result.region = settings.region;
    result.algorithmId = settings.consensusAlgorithm->getId();
    result.consensus = settings.consensusAlgorithm->getConsensusRegion(settings.region, reads.data(), referenceFragment, os);
    CHECK_OP(os, );

    result.insertions.clear();
    if (settings.calculateInsertions) {
        calculateInsertions(settings, os, result);
        CHECK_OP(os, );
    }

    os.setProgress(100);
}

void AssemblyConsensusTask::run() {
    GTIMER(c2, t2, "AssemblyConsensusTask::run");
    quint64 t0 = GTimer::currentTimeMicros();

    doCalculation(settings, stateInfo, result);

    CHECK_OP(stateInfo, );
    perfLog.trace(QString("Assembly: '%1' consensus calculation time: %2 seconds")
                      .arg(settings.consensusAlgorithm->getName())
                      .arg((GTimer::currentTimeMicros() - t0) / float(1000 * 1000)));
}

AssemblyConsensusWorker::AssemblyConsensusWorker(ConsensusSettingsQueue* settingsQueue_)
    : Task(tr("Assembly consensus worker"), TaskFlag_None), settingsQueue(settingsQueue_) {
    tpm = Progress_Manual;
}

void AssemblyConsensusWorker::run() {
    GTIMER(c2, t2, "AssemblyConsensusTask::run");
    quint64 t0 = GTimer::currentTimeMicros();

    int count = settingsQueue->count();
    int mappingLength = 100 / count;
    ConsensusInfo result;

    int completed = 0;
    while (settingsQueue->hasNext()) {
        AssemblyConsensusTaskSettings settings = settingsQueue->getNextSettings();

        U2OpStatusChildImpl os(&stateInfo, U2OpStatusMapping(completed * 100 / count, mappingLength));
        doCalculation(settings, os, result);
        CHECK_OP(stateInfo, );
        settingsQueue->reportResult(result);

        ++completed;
    }
    stateInfo.setProgress(100);

    perfLog.trace(QString("Assembly: '%1' consensus export time: %2 seconds")
                      .arg(result.algorithmId)
                      .arg((GTimer::currentTimeMicros() - t0) / float(1000 * 1000)));
}

}  // namespace U2
