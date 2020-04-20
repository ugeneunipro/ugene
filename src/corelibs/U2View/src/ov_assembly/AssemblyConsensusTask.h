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

#ifndef _U2_ASSEMBLY_CONSENSUS_TASK_H_
#define _U2_ASSEMBLY_CONSENSUS_TASK_H_

#include "AssemblyModel.h"

#include <U2Core/BackgroundTaskRunner.h>
#include <U2Core/U2Assembly.h>
#include <U2Core/U2Type.h>

#include <U2Algorithm/AssemblyConsensusAlgorithm.h>

namespace U2 {

struct ConsensusInfo {
    QByteArray consensus;
    U2Region region;
    QString algorithmId;
};

struct U2VIEW_EXPORT AssemblyConsensusTaskSettings {
    QSharedPointer<AssemblyConsensusAlgorithm> consensusAlgorithm;
    QSharedPointer<AssemblyModel> model;
    U2Region region;
};

/**
    A background task for ConsensusArea: finds consensus for given settings
*/
class AssemblyConsensusTask : public BackgroundTask<ConsensusInfo> {
    Q_OBJECT
public:
    AssemblyConsensusTask(const AssemblyConsensusTaskSettings & settings);
    virtual void run();
private:
    AssemblyConsensusTaskSettings settings;
};

/**
    An interface of task queue for AssemblyConsensusWorker.
*/
class ConsensusSettingsQueue {
public:
    virtual bool hasNext() = 0;
    virtual AssemblyConsensusTaskSettings getNextSettings() = 0;
    virtual int count() = 0;
    virtual void reportResult(const ConsensusInfo & result) = 0;
};

/**
    Subsequently gets tasks settings from ConsensusSettingsQueue::getNextSettings(),
    finds consensus and reports it to ConsensusSettingsQueue::reportResult()
*/
class AssemblyConsensusWorker : public Task {
    Q_OBJECT
public:
    AssemblyConsensusWorker(ConsensusSettingsQueue * settingsQueue);
    virtual void run();

private:
    ConsensusSettingsQueue * settingsQueue;
};

} // namespace U2

#endif // _U2_ASSEMBLY_CONSENSUS_H_
