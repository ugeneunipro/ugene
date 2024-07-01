/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#pragma once

#include <QQueue>

#include <U2Core/DocumentProviderTask.h>
#include <U2Core/U2VariantUtils.h>
#include <U2Core/VariantTrackObject.h>

#include "AssemblyConsensusTask.h"

namespace U2 {

struct ExportConsensusVariationsTaskSettings : public AssemblyConsensusTaskSettings {
    DocumentFormatId formatId;
    QString fileName;
    QString seqObjName;
    bool addToProject;
    bool keepGaps;
    CallVariationsMode mode;
    U2EntityRef refSeq;
};

class ExportConsensusVariationsTask : public DocumentProviderTask, ConsensusSettingsQueue {
    Q_OBJECT
public:
    ExportConsensusVariationsTask(const ExportConsensusVariationsTaskSettings& settings_);

    void prepare() override;
    QList<Task*> onSubTaskFinished(Task* subTask) override;

    // implement ConsensusSettingsQueue interface
    int count() override {
        return consensusRegions.count();
    }
    bool hasNext() override {
        return !consensusRegions.isEmpty();
    }
    AssemblyConsensusTaskSettings getNextSettings() override;
    void reportResult(const ConsensusInfo& result) override;

private:
    ExportConsensusVariationsTaskSettings settings;
    AssemblyConsensusWorker* consensusTask;
    VariantTrackObject* varTrackObject;

    // A region to analyze at a time
    static const qint64 REGION_TO_ANALAYZE = 1000000;

    // implement ConsensusSettingsQueue:
    QQueue<U2Region> consensusRegions;
};

}  // namespace U2
