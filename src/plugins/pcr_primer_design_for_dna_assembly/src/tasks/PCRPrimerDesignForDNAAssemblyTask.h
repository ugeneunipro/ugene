/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#ifndef _U2_PCR_PRIMER_DESIGN_FOR_DNA_ASSEMBLY_TASK_H_
#define _U2_PCR_PRIMER_DESIGN_FOR_DNA_ASSEMBLY_TASK_H_

#include <U2Core/Task.h>

#include "PCRPrimerDesignForDNAAssemblyTaskSettings.h"

namespace U2 {

class LoadDocumentTask;
class FindPresenceOfUnwantedParametersTask;
class FindUnwantedIslandsTask;

class PCRPrimerDesignForDNAAssemblyTask : public Task {
public:
    PCRPrimerDesignForDNAAssemblyTask(const PCRPrimerDesignForDNAAssemblyTaskSettings& settings, const QByteArray& sequence);

    void prepare() override;

    void run() override;

    QList<Task*> onSubTaskFinished(Task* subTask) override;

    QString generateReport() const override;

    QList<U2Region> getResults() const;

    static const QStringList FRAGMENT_INDEX_TO_NAME;

private:
    QList<QByteArray> extractLoadedSequences(LoadDocumentTask* task);

    PCRPrimerDesignForDNAAssemblyTaskSettings settings;
    QByteArray sequence;

    LoadDocumentTask* loadBackboneSequence = nullptr;
    LoadDocumentTask* loadOtherSequencesInPcr = nullptr;
    FindPresenceOfUnwantedParametersTask* checkBackboneSequence = nullptr;
    FindUnwantedIslandsTask* findUnwantedIslands = nullptr;

    QList<QByteArray> backboneSequencesCandidates;
    QList<QByteArray> otherSequencesInPcr;
    QByteArray backboneSequence;
    QList<U2Region> candidatePrimerRegions;

    //Results
    U2Region aForward = U2Region(54, 77 - 54);
    U2Region aReverse = U2Region(327, 353 - 328);
    U2Region b1Forward = U2Region(39, 57 - 40);
    U2Region b1Reverse = U2Region(337, 367 - 338);
    U2Region b2Forward = U2Region(26, 43 - 27);
    U2Region b2Reverse = U2Region(362, 379 - 363);
    U2Region b3Forward = U2Region(22, 43 - 23);
    U2Region b3Reverse = U2Region(367, 384 - 368);
};

}

#endif // _U2_PCR_PRIMER_DESIGN_FOR_DNA_ASSEMBLY_TASK_H_
