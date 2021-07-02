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

private:
    QList<QByteArray> extractLoadedSequences(LoadDocumentTask* task);
    bool areMetlingTempAndDeltaGood(const QByteArray& primer) const;
    bool hasUnwantedConnections(const QByteArray& primer) const;
    void updatePrimerRegion(int& primerEnd, int& primerLength) const;
    void findCandidatePrimers(const QList<U2Region>& regionsBetweenIslands, int amplifiedFragmentEdge,
                              bool findFirstOnly, bool isComplement,
                              U2Region& b1, U2Region& b2, U2Region& b3) const;

    template<class WhileCondition>
    U2Region findCandidatePrimer(int primerEnd, int amplifiedFragmentEdge, bool isComplement, WhileCondition cond) const {
        int primerLength = settings.overlapLength.minValue;
        U2Region foundPrimerRegion;
        while (cond(primerEnd, primerLength)) { //While we are in the region between islands
            const U2Region candidatePrimerRegion(primerEnd - primerLength, primerLength);
            if ((amplifiedFragmentEdge/*settings.leftArea.endPos() - 1*/) < candidatePrimerRegion.startPos) {
                primerEnd--;
                continue;
            }
            QByteArray candidatePrimerSequence;
            if (!isComplement) {
                candidatePrimerSequence = sequence.mid(candidatePrimerRegion.startPos, candidatePrimerRegion.length);
            } else {
                candidatePrimerSequence = reverseComplementSequence.mid(candidatePrimerRegion.startPos, candidatePrimerRegion.length);
            }

            //Check if candidate primer melting temperature and deltaG fit to settings
            bool areSettingsGood = areMetlingTempAndDeltaGood(candidatePrimerSequence);
            if (!areSettingsGood) {
                updatePrimerRegion(primerEnd, primerLength);
                continue;
            } else {
                //If melt temp and delta G are good - add backbone and check unwanted connections
                QString candidatePrimerRegionString = regionToString(candidatePrimerRegion, isComplement);
                taskLog.details(tr("The candidate primer region \"%1\" fits to \"Parameters of priming sequences\" values, check for unwanted connections")
                                .arg(candidatePrimerRegionString));
                candidatePrimerSequence = backboneSequence + candidatePrimerSequence;
                bool hasUnwanted = hasUnwantedConnections(candidatePrimerSequence);
                if (!hasUnwanted) {
                    //If there are no unwanted connections - we are found primer region
                    foundPrimerRegion = candidatePrimerRegion;
                    break;
                } else {
                    taskLog.details(tr("The candidate primer region \"%1\" contains unwanted connections")
                        .arg(candidatePrimerRegionString));
                    updatePrimerRegion(primerEnd, primerLength);
                    continue;
                }
            }
        }

        return foundPrimerRegion;
    }

    QString PCRPrimerDesignForDNAAssemblyTask::regionToString(const U2Region& region, bool isComplement) const;


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
