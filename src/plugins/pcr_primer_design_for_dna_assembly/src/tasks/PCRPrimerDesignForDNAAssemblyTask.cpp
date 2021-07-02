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
#include <U2Core/IOAdapter.h>
#include <U2Core/Counter.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/L10n.h>

#include <QApplication>
#include <QMessageBox>

#include "FindPresenceOfUnwantedParametersTask.h"
#include "FindUnwantedIslandsTask.h"

#include <U2Core/PrimerStatistics.h>

namespace U2 {

PCRPrimerDesignForDNAAssemblyTask::PCRPrimerDesignForDNAAssemblyTask(const PCRPrimerDesignForDNAAssemblyTaskSettings& _settings, const QByteArray& _sequence)
    : Task("PCR Primer Design For DNA Assembly Task", TaskFlags_FOSCOE | TaskFlag_ReportingIsSupported | TaskFlag_ReportingIsEnabled),
      settings(_settings),
      sequence(_sequence) {
    GCOUNTER(cvar, "PCRPrimerDesignForDNAAssemblyTask");
}

void PCRPrimerDesignForDNAAssemblyTask::prepare() {
    auto prepareLoadDocumentTask = [this](const QString& url) -> LoadDocumentTask* {
        QList<FormatDetectionResult> formats = DocumentUtils::detectFormat(url);
        CHECK_EXT(!formats.isEmpty(), setError(tr("Unknown file format!")), nullptr);

        auto ioRegistry = AppContext::getIOAdapterRegistry();
        SAFE_POINT_EXT(ioRegistry != nullptr, setError(L10N::nullPointerError("IOAdapterRegistry")), nullptr);

        IOAdapterFactory* iow = ioRegistry->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
        return new LoadDocumentTask(formats.first().format->getFormatId(), GUrl(url), iow);
    };

    if (!settings.backboneSequenceUrl.isEmpty()) {
        loadBackboneSequence = prepareLoadDocumentTask(settings.backboneSequenceUrl);
        CHECK_OP(stateInfo, );

        addSubTask(loadBackboneSequence);
    }

    if (!settings.otherSequencesInPcrUrl.isEmpty()) {
        loadOtherSequencesInPcr = prepareLoadDocumentTask(settings.otherSequencesInPcrUrl);
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
    int amplifiedFragmentLeftEdge = settings.leftArea.endPos() - 1;
    taskLog.details(tr("Looking for candidate primers B1, B2 and B3 in the left area"));
    findCandidatePrimers(regionsBetweenIslandsForward,
                         amplifiedFragmentLeftEdge,
                         false,
                         false,
                         b1Forward, b2Forward, b3Forward);

    int amplifiedFragmentRightEdgeReverseComplement = reverseComplementSequence.size() - settings.rightArea.startPos;
    taskLog.details(tr("Looking for candidate primers B1, B2 and B3 in the right area"));
    findCandidatePrimers(regionsBetweenIslandsReverse,
                         amplifiedFragmentRightEdgeReverseComplement,
                         false,
                         true,
                         b1Reverse, b2Reverse, b3Reverse);
    if (!b1Reverse.isEmpty()) {
        int sequenceSize = reverseComplementSequence.size();
        b1Reverse = DNASequenceUtils::reverseComplementRegion(b1Reverse, sequenceSize);
        if (!b2Reverse.isEmpty()) {
            b2Reverse = DNASequenceUtils::reverseComplementRegion(b2Reverse, sequenceSize);
        }
        if (!b3Reverse.isEmpty()) {
            b3Reverse = DNASequenceUtils::reverseComplementRegion(b3Reverse, sequenceSize);
        }
    }

    U2Region fake;
    taskLog.details(tr("Looking for the candidate primer A in the left area"));
    findCandidatePrimers(regionsBetweenIslandsForward, amplifiedFragmentLeftEdge, true, false, aForward, fake, fake);

    taskLog.details(tr("Looking for the candidate primer A in the right area"));
    findCandidatePrimers(regionsBetweenIslandsReverse, amplifiedFragmentRightEdgeReverseComplement, true, true, aReverse, fake, fake);
    if (!aReverse.isEmpty()) {
        int sequenceSize = reverseComplementSequence.size();
        aReverse = DNASequenceUtils::reverseComplementRegion(aReverse, sequenceSize);
    }
}

QList<Task*> PCRPrimerDesignForDNAAssemblyTask::onSubTaskFinished(Task* subTask) {
    CHECK_OP(stateInfo, {});

    if (subTask == loadBackboneSequence || subTask == checkBackboneSequence) {
        if (subTask == loadBackboneSequence) {
            backboneSequencesCandidates = extractLoadedSequences(loadBackboneSequence);
            CHECK_OP(stateInfo, {});
        } else if (subTask == checkBackboneSequence) {
            if (!checkBackboneSequence->hasUnwantedParameters()) {
                backboneSequence = checkBackboneSequence->getSequence();
                taskLog.details(tr("The backbone sequence without unwanted hairpins, self- and hetero-dimers has ben found: %1").arg(QString(backboneSequence)));
                return {};
            } else {
                taskLog.details(tr("The following backbone sequence candidate contains parameters: %1").arg(QString(backboneSequence)));
            }
        }

        if (!backboneSequencesCandidates.isEmpty()) {
            checkBackboneSequence = new FindPresenceOfUnwantedParametersTask(backboneSequencesCandidates.takeFirst(), settings);
            return { checkBackboneSequence };
        } else {
            taskLog.error(tr("The file \"%1\" doesn't contain the backbone sequence, which matchs the parameters. "
                "Skip the backbone sequence parameter.").arg(settings.backboneSequenceUrl));
        }
    } else if (subTask == loadOtherSequencesInPcr) {
        otherSequencesInPcr = extractLoadedSequences(loadOtherSequencesInPcr);
        CHECK_OP(stateInfo, {});
    } else if (subTask == findUnwantedIslands) {
        candidatePrimerRegions = findUnwantedIslands->getRegionBetweenIslands();
    }

    return {};
}

QString PCRPrimerDesignForDNAAssemblyTask::generateReport() const {
    //TODO - report
    return QString();
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

    //TODO: hairpins and heterodimers
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

void PCRPrimerDesignForDNAAssemblyTask::findCandidatePrimers(const QList<U2Region>& regionsBetweenIslands,
                                                             int amplifiedFragmentEdge,
                                                             bool findFirstOnly,
                                                             bool isComplement,
                                                             U2Region& first,
                                                             U2Region& second,
                                                             U2Region& third) const {
    first = U2Region();
    second = U2Region();
    third = U2Region();
    for (const auto& regionBetweenIslands : regionsBetweenIslands) {
        if (regionBetweenIslands.length < MINIMUM_LENGTH_BETWEEN_ISLANDS) {
            continue;
        }

        auto firstCandidatePrimerWhileCondition = [&](int primerEnd, int primerLength) {
            return primerEnd - primerLength > regionBetweenIslands.startPos;
        };
        int firstCandidatePrimerEnd = regionBetweenIslands.endPos();
        first = findCandidatePrimer(firstCandidatePrimerEnd, amplifiedFragmentEdge, isComplement, firstCandidatePrimerWhileCondition);

        //If we didn't find primer - check another region between islands
        if (first.isEmpty()) {
            continue;
        } else if (findFirstOnly) {
            taskLog.details(tr("A %1 primer has been found").arg(isComplement ? "reverse" : "forvard"));
            break;
        }
        taskLog.details(tr("B1 %1 primer has been found").arg(isComplement ? "reverse" : "forvard"));

        auto secondCandidatePrimerWhileCondition = [&](int primerEnd, int) {
            return primerEnd == first.startPos + SECOND_PRIMER_OFFSET;
        };
        int secondCandidatePrimerEnd = first.startPos + SECOND_PRIMER_OFFSET;
        second = findCandidatePrimer(secondCandidatePrimerEnd, amplifiedFragmentEdge, isComplement, secondCandidatePrimerWhileCondition);
        if (!second.isEmpty()) {
            taskLog.details(tr("B2 %1 primer has been found").arg(isComplement ? "reverse" : "forvard"));
        }

        auto thirdCandidatePrimerWhileCondition = [&](int primerEnd, int) {
            return primerEnd == first.startPos;
        };
        int thirdCandidatePrimerEnd = first.startPos;
        third = findCandidatePrimer(thirdCandidatePrimerEnd, amplifiedFragmentEdge, isComplement, thirdCandidatePrimerWhileCondition);
        if (!third.isEmpty()) {
            taskLog.details(tr("B3 %1 primer has been found").arg(isComplement ? "reverse" : "forvard"));
        }

        // If we didn't find at least one additional primer - clear reasults and try again
        if (second.isEmpty() && third.isEmpty()) {
            taskLog.details(tr("B2 and B3 %1 primers haven't been found, search again").arg(isComplement ? "reverse" : "forvard"));
            first = U2Region();
        } else {
            break;
        }
    }
}

QString PCRPrimerDesignForDNAAssemblyTask::regionToString(const U2Region& region, bool isComplement) const {
    U2Region regionToLog = isComplement ? DNASequenceUtils::reverseComplementRegion(region, sequence.size()) : region;
    return QString("%1..%2").arg(regionToLog.startPos + 1).arg(regionToLog.endPos());
}

}
