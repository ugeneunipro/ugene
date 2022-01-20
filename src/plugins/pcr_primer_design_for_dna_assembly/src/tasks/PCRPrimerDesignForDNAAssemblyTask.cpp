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

#include "PCRPrimerDesignForDNAAssemblyTask.h"

#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/Counter.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/L10n.h>
#include <U2Core/PrimerStatistics.h>

#include <U2Gui/MainWindow.h>

#include <QApplication>
#include <QMainWindow>
#include <QMessageBox>

#include "utils/UnwantedConnectionsUtils.h"
#include "FindPresenceOfUnwantedParametersTask.h"
#include "FindUnwantedIslandsTask.h"
#include "UnwantedStructuresInBackboneDialog.h"

namespace U2 {

const QStringList PCRPrimerDesignForDNAAssemblyTask::FRAGMENT_INDEX_TO_NAME = {
    "A Forward",
    "A Reverse",
    "B1 Forward",
    "B1 Reverse",
    "B2 Forward",
    "B2 Reverse",
    "B3 Forward",
    "B3 Reverse"
};

PCRPrimerDesignForDNAAssemblyTask::PCRPrimerDesignForDNAAssemblyTask(const PCRPrimerDesignForDNAAssemblyTaskSettings& _settings, const QByteArray& _sequence)
    : Task(tr("PCR Primer Design For DNA Assembly Task"), TaskFlags_FOSCOE | TaskFlag_ReportingIsSupported | TaskFlag_ReportingIsEnabled),
      settings(_settings),
      sequence(_sequence),
      reverseComplementSequence(DNASequenceUtils::reverseComplement(sequence)) {
    GCOUNTER(cvar, "PCRPrimerDesignForDNAAssemblyTask");

    tpm = ProgressManagement::Progress_Manual;
}

void PCRPrimerDesignForDNAAssemblyTask::prepare() {
    if (!settings.backboneSequenceUrl.isEmpty()) {
        loadBackboneSequence = LoadDocumentTask::getDefaultLoadDocTask(stateInfo, GUrl(settings.backboneSequenceUrl));
        CHECK_OP(stateInfo, );

        addSubTask(loadBackboneSequence);
    }

    if (!settings.otherSequencesInPcrUrl.isEmpty()) {
        loadOtherSequencesInPcr = LoadDocumentTask::getDefaultLoadDocTask(stateInfo, GUrl(settings.otherSequencesInPcrUrl));;
        CHECK_OP(stateInfo, );

        addSubTask(loadOtherSequencesInPcr);
    }

    findUnwantedIslands = new FindUnwantedIslandsTask(settings.leftArea, settings.overlapLength.maxValue, sequence, false);
    addSubTask(findUnwantedIslands);

    U2Region reverseComplementArea = DNASequenceUtils::reverseComplementRegion(settings.rightArea, reverseComplementSequence.size());
    findUnwantedIslandsReverseComplement = new FindUnwantedIslandsTask(reverseComplementArea, settings.overlapLength.maxValue, reverseComplementSequence, true);
    addSubTask(findUnwantedIslandsReverseComplement);


}

void PCRPrimerDesignForDNAAssemblyTask::run() {
    // We have A and B1, B2, B3 pairs of primers
    // A and B1 are pretty the same, except the fact that B1 has some depencedcies on B2 and B3
    // So, the first time we find good B1 pair they are also the A pair. But if B1 doesn't fit to B2 or B3, we need to continue searching B1, which will be fit to B2 and B3
    bool aWasNotFoundYet = true;
    int progressStage = 100 / regionsBetweenIslandsForward.size();
    for (int i = 0; i < regionsBetweenIslandsForward.size(); i++) {
        const auto& regionBetweenIslandsForward = regionsBetweenIslandsForward[i];
        stateInfo.setProgress(((double)i / (double)regionsBetweenIslandsForward.size()) * 100);
        //The task could be really time-consuming, so it's better to check sometimes it it's been canceled
        CHECK_OP(stateInfo, );

        if (regionBetweenIslandsForward.length < MINIMUM_LENGTH_BETWEEN_ISLANDS) {
            continue;
        }
        const int amplifiedFragmentLeftEdge = settings.leftArea.endPos() - 1;
        int b1ForwardCandidatePrimerEnd = regionBetweenIslandsForward.endPos();
        int b1ForwardPrimerLength = settings.overlapLength.minValue;

        // For progress state
        int iterationsNumber = (settings.overlapLength.maxValue - settings.overlapLength.minValue) * regionBetweenIslandsForward.length;
        int currentIteration = 0;
        int stateProgress = getProgress();

        //While the primer start position is in the region between islands
        while (b1ForwardCandidatePrimerEnd - b1ForwardPrimerLength > regionBetweenIslandsForward.startPos) {
            //The task could be really time-consuming, so it's better to check sometimes it it's been canceled
            stateInfo.setProgress(stateProgress + (((double)currentIteration / (double)iterationsNumber)) * progressStage);
            currentIteration++;
            CHECK_OP(stateInfo, );

            const U2Region b1ForwardCandidatePrimerRegion(b1ForwardCandidatePrimerEnd - b1ForwardPrimerLength, b1ForwardPrimerLength);
            // The amplified fragment shouldn't contain the all primer
            // So move the primer untill at least one character out of the amplified fragment
            if (amplifiedFragmentLeftEdge < b1ForwardCandidatePrimerRegion.startPos) {
                b1ForwardCandidatePrimerEnd--;
                // For progress state
                iterationsNumber -= (settings.overlapLength.maxValue - settings.overlapLength.minValue);
                continue;
            }

            // The nucleotide sequence of the corresponding region
            QByteArray b1ForwardCandidatePrimerSequence = sequence.mid(b1ForwardCandidatePrimerRegion.startPos, b1ForwardCandidatePrimerRegion.length);
            //Check if candidate primer melting temperature and deltaG fit to settings
            bool areSettingsGood = areMetlingTempAndDeltaGood(b1ForwardCandidatePrimerSequence);
            // If they are not - increase primer size (if it's not too big)
            // If it's too big, move to the left and reset length to minimum...
            if (!areSettingsGood) {
                updatePrimerRegion(b1ForwardCandidatePrimerEnd, b1ForwardPrimerLength);
                // ... and check the primer again
                continue;
            } else {
                // Region string representation
                QString b1ForwardCandidatePrimerRegionString = regionToString(b1ForwardCandidatePrimerRegion, false);
                taskLog.details(tr("The \"B1 Forward\" candidate primer region \"%1\" fits to \"Parameters of priming sequences\" values, checking for unwanted connections").arg(QString(b1ForwardCandidatePrimerRegionString)));
                //If melting temperature and delta G are good - add backbone and check unwanted connections
                b1ForwardCandidatePrimerSequence = backboneSequence + b1ForwardCandidatePrimerSequence;
                bool hasUnwanted = hasUnwantedConnections(b1ForwardCandidatePrimerSequence);
                if (!hasUnwanted) {
                    // If there are no unwanted connections - we are found b1 forward candidate primer
                    // Now we need to check b1 reverse primer
                    // b1 forwards and b1 reverse shouldn't have unwanted hetero-dimers - if they do we need to continue searching of b1 forward
                    findB1ReversePrimer(b1ForwardCandidatePrimerSequence);

                    //If b1 reverse wasn't found - continue searching
                    if (b1Reverse.isEmpty()) {
                        taskLog.details(tr("The \"B1 Forward\" primer with the region \"%1\" doesn't fit because there are no corresponding \"B1 Reverse\" primers").arg(QString(b1ForwardCandidatePrimerRegionString)));
                        updatePrimerRegion(b1ForwardCandidatePrimerEnd, b1ForwardPrimerLength);
                        continue;
                    }

                    //If it was found - we found b1 forward
                    b1Forward = b1ForwardCandidatePrimerRegion;

                    //The first time we found the pair of B1 primers - they are also A primers
                    if (aWasNotFoundYet) {
                        taskLog.details(tr("The \"A Forward\" and the \"A Reverse\" primers have been found"));
                        aForward = b1Forward;
                        aReverse = b1Reverse;
                        aWasNotFoundYet = false;
                    }

                    //Now we need to find B2 and B3
                    findSecondaryForwardReversePrimers(SecondaryPrimer::B2);
                    findSecondaryForwardReversePrimers(SecondaryPrimer::B3);

                    // We need to find at leas one pair - B2 or B3 - for B1 to be valid
                    // If we didn't - continue searching
                    if (b2Forward.isEmpty() && b3Forward.isEmpty()) {
                        taskLog.details(tr("B2 and B3 primer pairs haven't been found, search again"));
                        b1Forward = U2Region();
                        b1Reverse = U2Region();
                        updatePrimerRegion(b1ForwardCandidatePrimerEnd, b1ForwardPrimerLength);
                        continue;
                    }

                    break;
                } else {
                    taskLog.details(tr("The candidate primer region \"%1\" contains unwanted connections").arg(QString(b1ForwardCandidatePrimerRegionString)));
                    //If they are unwanted connections - update primer region and check all parameters again
                    updatePrimerRegion(b1ForwardCandidatePrimerEnd, b1ForwardPrimerLength);
                    continue;
                }
            }
        }
        if (!b1Forward.isEmpty()) {
            break;
        }
    }
    generateUserPrimersReports();
}

QList<Task*> PCRPrimerDesignForDNAAssemblyTask::onSubTaskFinished(Task* subTask) {
    CHECK_OP(stateInfo, {});

    if (subTask == loadBackboneSequence || subTask == checkBackboneSequence) {
        if (subTask == loadBackboneSequence) {
            backboneSequencesCandidates = extractLoadedSequences(loadBackboneSequence);
            CHECK_OP(stateInfo, {});
        } else if (subTask == checkBackboneSequence) {
            const QByteArray &consideredBackboneSequence = checkBackboneSequence->getSequence();
            // No unwanted structures -> Set as backbone, finish search for backbone.
            if (!checkBackboneSequence->hasUnwantedParameters()) {
                backboneSequence = consideredBackboneSequence;
                taskLog.details(tr("The backbone sequence without unwanted hairpins, self- and hetero-dimers has been "
                                   "found: %1")
                                    .arg(QString(backboneSequence)));
                return {};
            }
            // There are unwanted structures -> Asking the user if this sequence with unwanted structures should be used
            // as backbone?
            taskLog.details(tr("The following backbone sequence candidate contains parameters: %1")
                                .arg(QString(consideredBackboneSequence)));
            int userResponse = UnwantedStructuresInBackboneDialog(consideredBackboneSequence,
                                                                  checkBackboneSequence->getUnwantedStructures(),
                                                                  backboneSequencesCandidates.length(),
                                                                  AppContext::getMainWindow()->getQMainWindow())
                                   .exec();
            // User accepted -> Set as backbone, finish search for backbone.
            if (userResponse == QDialog::Accepted) {
                backboneSequence = consideredBackboneSequence;
                return {};
            }
            // User rejected -> Proceed to next candidate sequence (next if).
        }

        if (!backboneSequencesCandidates.isEmpty()) {
            checkBackboneSequence = new FindPresenceOfUnwantedParametersTask(backboneSequencesCandidates.takeFirst(), settings);
            return { checkBackboneSequence };
        } else {
            backboneSequence = QByteArray();
            taskLog.error(tr("The file \"%1\" doesn't contain the backbone sequence, which matches the parameters. "
                "Skip the backbone sequence parameter.").arg(settings.backboneSequenceUrl));
        }
    } else if (subTask == loadOtherSequencesInPcr) {
        otherSequencesInPcr = extractLoadedSequences(loadOtherSequencesInPcr);
        CHECK_OP(stateInfo, {});
    } else if (subTask == findUnwantedIslands) {
        regionsBetweenIslandsForward = findUnwantedIslands->getRegionBetweenIslands();
    } else if (subTask == findUnwantedIslandsReverseComplement) {
        regionsBetweenIslandsReverse = findUnwantedIslandsReverseComplement->getRegionBetweenIslands();
    }

    return {};
}

QString PCRPrimerDesignForDNAAssemblyTask::generateReport() const {
    CHECK_OP(stateInfo, stateInfo.getError());

    return PCRPrimerDesignTaskReportUtils::generateReport(*this, sequence, reverseComplementSequence,
                                                          userPrimersReports);
}

QList<U2Region> PCRPrimerDesignForDNAAssemblyTask::getResults() const {
    QList<U2Region> results;
    results << aForward << aReverse << b1Forward << b1Reverse << b2Forward << b2Reverse << b3Forward << b3Reverse;
    return results;
}

QByteArray PCRPrimerDesignForDNAAssemblyTask::getBackboneSequence() const {
    return backboneSequence;
}

const PCRPrimerDesignForDNAAssemblyTaskSettings& PCRPrimerDesignForDNAAssemblyTask::getSettings() const {
    return settings;
}

QList<QByteArray> PCRPrimerDesignForDNAAssemblyTask::extractLoadedSequences(LoadDocumentTask* task) {
    auto doc = task->getDocument();
    CHECK_EXT(doc != nullptr, setError(tr("The file \"%1\" wasn't loaded").arg(task->getURL().getURLString())), { {} });

    auto gObjects = doc->getObjects();
    CHECK_EXT(!gObjects.isEmpty(), setError(tr("No objects in the file \"%1\"").arg(task->getURL().getURLString())), { {} });

    QList<QByteArray> loadedSequences;
    for (auto gObj : qAsConst(gObjects)) {
        auto sequenceObject = qobject_cast<U2SequenceObject*>(gObj);
        CHECK_CONTINUE(sequenceObject != nullptr);

        auto wholeSeq = sequenceObject->getWholeSequence(stateInfo);
        CHECK_OP(stateInfo, { {} });

        loadedSequences << wholeSeq.seq;
    }
    CHECK_EXT(!loadedSequences.isEmpty(), setError(tr("No sequences in the file \"%1\"").arg(task->getURL().getURLString())), { {} });

    return loadedSequences;
}

void PCRPrimerDesignForDNAAssemblyTask::findB1ReversePrimer(const QByteArray& b1ForwardCandidatePrimerSequence) {
    // Very the same algorithm, but for reverse regions etween islands
    for (const auto& regionBetweenIslandsReverse : qAsConst(regionsBetweenIslandsReverse)) {
        //The task could be really time-consuming, so it's better to check sometimes it it's been canceled
        CHECK_OP(stateInfo, );

        if (regionBetweenIslandsReverse.length < MINIMUM_LENGTH_BETWEEN_ISLANDS) {
            continue;
        }

        int amplifiedFragmentEdgeReverse = reverseComplementSequence.size() - settings.rightArea.startPos;
        int b1ReverseCandidatePrimerEnd = regionBetweenIslandsReverse.endPos();
        int b1ReversePrimerLength = settings.overlapLength.minValue;

        while (b1ReverseCandidatePrimerEnd - b1ReversePrimerLength > regionBetweenIslandsReverse.startPos) { //While we are in the region between islands
            //The task could be really time-consuming, so it's better to check sometimes it it's been canceled
            CHECK_OP(stateInfo, );

            const U2Region b1ReverseCandidatePrimerRegion(b1ReverseCandidatePrimerEnd - b1ReversePrimerLength, b1ReversePrimerLength);
            // The amplified fragment shouldn't contain the all primer
            // So move the primer untill at least one character out of the amplified fragment
            if (amplifiedFragmentEdgeReverse < b1ReverseCandidatePrimerRegion.startPos) {
                b1ReverseCandidatePrimerEnd--;
                continue;
            }
            QByteArray b1ReverseCandidatePrimerSequence = reverseComplementSequence.mid(b1ReverseCandidatePrimerRegion.startPos, b1ReverseCandidatePrimerRegion.length);
            //Check if candidate primer melting temperature and deltaG fit to settings
            bool areB1ReverseSettingsGood = areMetlingTempAndDeltaGood(b1ReverseCandidatePrimerSequence);
            if (!areB1ReverseSettingsGood) {
                updatePrimerRegion(b1ReverseCandidatePrimerEnd, b1ReversePrimerLength);
                continue;
            } else {
                QString b1ReverseCandidatePrimerRegionString = regionToString(b1ReverseCandidatePrimerRegion, true);
                taskLog.details(tr("The \"B1 Reverse\" candidate primer region \"%1\" fits to \"Parameters of priming sequences\" values, checking for unwanted connections").arg(b1ReverseCandidatePrimerRegionString));
                //If melt temp and delta G are good - add backbone and check unwanted connections
                b1ReverseCandidatePrimerSequence = backboneSequence + b1ReverseCandidatePrimerSequence;
                bool hasUnwanted = hasUnwantedConnections(b1ReverseCandidatePrimerSequence);
                if (!hasUnwanted) {
                    //If there are no unwanted connections - check hetero-dimers between B1 Forward and B1 Reverse
                    bool hasB1ForwardReverseHeteroDimer =
                        UnwantedConnectionsUtils::isUnwantedHeteroDimer(b1ForwardCandidatePrimerSequence, b1ReverseCandidatePrimerSequence,
                            settings.gibbsFreeEnergyExclude, settings.meltingPointExclude, settings.complementLengthExclude);

                    if (hasB1ForwardReverseHeteroDimer) {
                        taskLog.details(tr("\"B1 Forward\" and \"B1 Reverse\" have unwanted hetero-dimers, move on"));
                        updatePrimerRegion(b1ReverseCandidatePrimerEnd, b1ReversePrimerLength);
                        continue;
                    }

                    b1Reverse = DNASequenceUtils::reverseComplementRegion(b1ReverseCandidatePrimerRegion, reverseComplementSequence.size());
                    break;
                } else {
                    taskLog.details(tr("The \"B1 Reverse\" candidate primer region \"%1\" contains unwanted connections").arg(b1ReverseCandidatePrimerRegionString));
                    updatePrimerRegion(b1ReverseCandidatePrimerEnd, b1ReversePrimerLength);
                    continue;
                }
            }
        }
        if (!b1Reverse.isEmpty()) {
            break;
        }
    }
}

void PCRPrimerDesignForDNAAssemblyTask::findSecondaryForwardReversePrimers(SecondaryPrimer type) {
    QString forwardPrimerName;
    QString reversePrimerName;
    int defaultForwardCandidatePrimerEnd = 0;
    switch (type) {
    case SecondaryPrimer::B2:
        forwardPrimerName = "B2 Forward";
        reversePrimerName = "B2 Reverse";
        defaultForwardCandidatePrimerEnd = b1Forward.startPos + SECOND_PRIMER_OFFSET;
        break;
    case SecondaryPrimer::B3:
        forwardPrimerName = "B3 Forward";
        reversePrimerName = "B3 Reverse";
        defaultForwardCandidatePrimerEnd = b1Forward.startPos;
        break;
    }

    int forwardCandidatePrimerEnd = defaultForwardCandidatePrimerEnd;
    int forwardPrimerLength = settings.overlapLength.minValue;
    while (forwardCandidatePrimerEnd == defaultForwardCandidatePrimerEnd) { //While we are in the region between islands
        //The task could be really time-consuming, so it's better to check sometimes it it's been canceled
        CHECK_OP(stateInfo, );

        const U2Region forwardCandidatePrimerRegion(forwardCandidatePrimerEnd - forwardPrimerLength, forwardPrimerLength);
        QByteArray forwardCandidatePrimerSequence = sequence.mid(forwardCandidatePrimerRegion.startPos, forwardCandidatePrimerRegion.length);

        //Check if candidate primer melting temperature and deltaG fit to settings
        bool areSettingsGood = areMetlingTempAndDeltaGood(forwardCandidatePrimerSequence);
        if (!areSettingsGood) {
            updatePrimerRegion(forwardCandidatePrimerEnd, forwardPrimerLength);
            continue;
        } else {
            //If melt temp and delta G are good - add backbone and check unwanted connections
            QString forwardCandidatePrimerRegionString = regionToString(forwardCandidatePrimerRegion, false);
            taskLog.details(tr("The \"%1\" candidate primer region \"%2\" fits to \"Parameters of priming sequences\" values, checking for unwanted connections").arg(forwardPrimerName).arg(forwardCandidatePrimerRegionString));
            forwardCandidatePrimerSequence = backboneSequence + forwardCandidatePrimerSequence;
            bool hasUnwanted = hasUnwantedConnections(forwardCandidatePrimerSequence);
            if (!hasUnwanted) {
                // Find Reverse primer
                findSecondaryReversePrimer(type, forwardCandidatePrimerSequence);

                bool isEmpty = true;
                switch (type) {
                case SecondaryPrimer::B2:
                    isEmpty = b2Reverse.isEmpty();
                    break;
                case SecondaryPrimer::B3:
                    isEmpty = b3Reverse.isEmpty();
                    break;
                }

                if (isEmpty) {
                    taskLog.details(tr("The \"%1\" primer with the region \"%2\" doesn't fit because there are no corresponding \"%3\" primers")
                        .arg(forwardPrimerName).arg(QString(forwardCandidatePrimerRegionString)).arg(reversePrimerName));
                    updatePrimerRegion(forwardCandidatePrimerEnd, forwardPrimerLength);
                    continue;
                }

                //If there are no unwanted connections - we are found primer region
                switch (type) {
                case SecondaryPrimer::B2:
                    b2Forward = forwardCandidatePrimerRegion;
                    break;
                case SecondaryPrimer::B3:
                    b3Forward = forwardCandidatePrimerRegion;
                    break;
                }

                break;
            } else {
                taskLog.details(tr("The \"%1\" candidate primer region \"%2\" contains unwanted connections").arg(forwardPrimerName).arg(forwardCandidatePrimerRegionString));
                updatePrimerRegion(forwardCandidatePrimerEnd, forwardPrimerLength);
                continue;
            }
        }
    }

}

void PCRPrimerDesignForDNAAssemblyTask::findSecondaryReversePrimer(SecondaryPrimer type, const QByteArray& forwardCandidatePrimerSequence) {
    QString forwardPrimerName;
    QString reversePrimerName;
    int defaultReverseCandidatePrimerEnd = 0;
    switch (type) {
    case SecondaryPrimer::B2:
        forwardPrimerName = "B2 Forward";
        reversePrimerName = "B2 Reverse";
        defaultReverseCandidatePrimerEnd = DNASequenceUtils::reverseComplementRegion(b1Reverse, reverseComplementSequence.size()).startPos + SECOND_PRIMER_OFFSET;
        break;
    case SecondaryPrimer::B3:
        forwardPrimerName = "B3 Forward";
        reversePrimerName = "B3 Reverse";
        defaultReverseCandidatePrimerEnd = DNASequenceUtils::reverseComplementRegion(b1Reverse, reverseComplementSequence.size()).startPos;
        break;
    }

    int reverseCandidatePrimerEnd = defaultReverseCandidatePrimerEnd;
    int reversePrimerLength = settings.overlapLength.minValue;
    while (reverseCandidatePrimerEnd == defaultReverseCandidatePrimerEnd) { //While we are in the region between islands
        //The task could be really time-consuming, so it's better to check sometimes it it's been canceled
        CHECK_OP(stateInfo, );

        const U2Region reverseCandidatePrimerRegion(reverseCandidatePrimerEnd - reversePrimerLength, reversePrimerLength);
        QByteArray reverseCandidatePrimerSequence = reverseComplementSequence.mid(reverseCandidatePrimerRegion.startPos, reverseCandidatePrimerRegion.length);

        //Check if candidate primer melting temperature and deltaG fit to settings
        bool areSettingsGood = areMetlingTempAndDeltaGood(reverseCandidatePrimerSequence);
        if (!areSettingsGood) {
            updatePrimerRegion(reverseCandidatePrimerEnd, reversePrimerLength);
            continue;
        } else {
            QString reverseCandidatePrimerRegionString = regionToString(reverseCandidatePrimerRegion, true);
            taskLog.details(tr("The \"%1\" candidate primer region \"%2\" fits to \"Parameters of priming sequences\" values, checking for unwanted connections").arg(reversePrimerName).arg(reverseCandidatePrimerRegionString));
            //If melt temp and delta G are good - add backbone and check unwanted connections
            reverseCandidatePrimerSequence = backboneSequence + reverseCandidatePrimerSequence;
            bool hasUnwanted = hasUnwantedConnections(reverseCandidatePrimerSequence);
            if (!hasUnwanted) {
                //If there are no unwanted connections - check hetero-dimers between Forward and Reverse
                bool hasForwardReverseHeteroDimer =
                    UnwantedConnectionsUtils::isUnwantedHeteroDimer(forwardCandidatePrimerSequence, reverseCandidatePrimerSequence,
                        settings.gibbsFreeEnergyExclude, settings.meltingPointExclude, settings.complementLengthExclude);
                if (hasForwardReverseHeteroDimer) {
                    taskLog.details(tr("\"%1\" and \"%2\" have unwanted hetero-dimers, move on").arg(forwardPrimerName).arg(reversePrimerName));
                    updatePrimerRegion(reverseCandidatePrimerEnd, reversePrimerLength);
                    continue;
                }

                //If there are no unwanted connections - we are found primer region
                switch (type) {
                case SecondaryPrimer::B2:
                    b2Reverse = DNASequenceUtils::reverseComplementRegion(reverseCandidatePrimerRegion, reverseComplementSequence.size());
                    break;
                case SecondaryPrimer::B3:
                    b3Reverse = DNASequenceUtils::reverseComplementRegion(reverseCandidatePrimerRegion, reverseComplementSequence.size());
                    break;
                }
                break;
            } else {
                taskLog.details(tr("The \"B2 Reverse\" candidate primer region \"%1\" contains unwanted connections").arg(reverseCandidatePrimerRegionString));
                updatePrimerRegion(reverseCandidatePrimerEnd, reversePrimerLength);
                continue;
            }
        }
    }
}

void PCRPrimerDesignForDNAAssemblyTask::generateUserPrimersReports() {
    if (settings.forwardUserPrimer.isEmpty() || settings.reverseUserPrimer.isEmpty()) {
        if (settings.forwardUserPrimer.isEmpty() && settings.reverseUserPrimer.isEmpty()) {
            taskLog.details(tr("No user primers"));
        } else if (settings.forwardUserPrimer.isEmpty()) {
            taskLog.error(tr("No forward user primer. Reverse user primer ignored"));
        } else if (settings.reverseUserPrimer.isEmpty()) {
            taskLog.error(tr("No reverse user primer. Forward user primer ignored"));
        }
        return;
    }

    int deltaG = settings.gibbsFreeEnergyExclude,
        meltingT = settings.meltingPointExclude,
        dimerLen = settings.complementLengthExclude;
    const auto saveOnePrimerReports = [deltaG, meltingT, dimerLen, this](const QByteArray &primer,
            PCRPrimerDesignTaskReportUtils::UserPrimersReports::PrimerReports &saveTo) {
        QString report_;
        if (UnwantedConnectionsUtils::isUnwantedSelfDimer(primer, deltaG, meltingT, dimerLen, report_)) {
            saveTo.selfdimer = report_;
        }
        if (UnwantedConnectionsUtils::isUnwantedHeteroDimer(primer, sequence, deltaG, meltingT, dimerLen, report_)) {
            saveTo.fileSeq = report_;
        }
        if (UnwantedConnectionsUtils::isUnwantedHeteroDimer(primer, reverseComplementSequence, deltaG, meltingT,
                                                            dimerLen, report_)) {
            saveTo.fileRevComplSeq = report_;
        }
        for (const QByteArray &otherSeqInPcr : qAsConst(otherSequencesInPcr)) {
            if (UnwantedConnectionsUtils::isUnwantedHeteroDimer(primer, otherSeqInPcr, deltaG, meltingT, dimerLen,
                                                                report_)) {
                saveTo.other << report_;
            }
        }
    };

    QByteArray forward = settings.forwardUserPrimer.toLocal8Bit();
    QByteArray reverse = settings.reverseUserPrimer.toLocal8Bit();
    saveOnePrimerReports(forward, userPrimersReports.forward);
    saveOnePrimerReports(reverse, userPrimersReports.reverse);

    QString report_;
    if (UnwantedConnectionsUtils::isUnwantedHeteroDimer(forward, reverse, deltaG, meltingT, dimerLen, report_)) {
        userPrimersReports.heterodimer = report_;
    }
}

bool PCRPrimerDesignForDNAAssemblyTask::areMetlingTempAndDeltaGood(const QByteArray& primer) const {
    auto candidatePrimerDeltaG = PrimerStatistics::getDeltaG(primer);
    auto candidatePrimerMeltingTemp = PrimerStatistics::getMeltingTemperature(primer);
    bool goodDeltaG = settings.gibbsFreeEnergy.minValue <= candidatePrimerDeltaG &&
                      candidatePrimerDeltaG <= settings.gibbsFreeEnergy.maxValue;
    bool goodMeltTemp = settings.meltingPoint.minValue <= candidatePrimerMeltingTemp &&
                      candidatePrimerMeltingTemp <= settings.meltingPoint.maxValue;

    return goodDeltaG && goodMeltTemp;
}

bool PCRPrimerDesignForDNAAssemblyTask::hasUnwantedConnections(const QByteArray& primer) const {
    bool isUnwantedSelfDimer = UnwantedConnectionsUtils::isUnwantedSelfDimer(primer, settings.gibbsFreeEnergyExclude, settings.meltingPointExclude, settings.complementLengthExclude);
    bool hasUnwantedHeteroDimer = false;
    hasUnwantedHeteroDimer |= UnwantedConnectionsUtils::isUnwantedHeteroDimer(primer, sequence,
        settings.gibbsFreeEnergyExclude, settings.meltingPointExclude, settings.complementLengthExclude);
    hasUnwantedHeteroDimer |= UnwantedConnectionsUtils::isUnwantedHeteroDimer(primer, reverseComplementSequence,
        settings.gibbsFreeEnergyExclude, settings.meltingPointExclude, settings.complementLengthExclude);
    for (const QByteArray& otherSeqInPcr : qAsConst(otherSequencesInPcr)) {
        hasUnwantedHeteroDimer |= UnwantedConnectionsUtils::isUnwantedHeteroDimer(primer,
            otherSeqInPcr, settings.gibbsFreeEnergyExclude, settings.meltingPointExclude, settings.complementLengthExclude);
    }

    //TODO: hairpins
    return isUnwantedSelfDimer || hasUnwantedHeteroDimer;
}

void PCRPrimerDesignForDNAAssemblyTask::updatePrimerRegion(int& primerEnd, int& primerLength) const {
    //Increase candidate primer length
    //If primer length is too big, reset it and decrease primer end
    primerLength++;
    if (primerLength > settings.overlapLength.maxValue) {
        primerLength = settings.overlapLength.minValue;
        primerEnd--;
    }
}

QString PCRPrimerDesignForDNAAssemblyTask::regionToString(const U2Region& region, bool isComplement) const {
    U2Region regionToLog = isComplement ? DNASequenceUtils::reverseComplementRegion(region, sequence.size()) : region;
    return QString("%1..%2").arg(regionToLog.startPos + 1).arg(regionToLog.endPos());
}



}
