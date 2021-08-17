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

namespace {
// Return html title for resulting sequence report.
QString getPrimerReportTitle(const QString& primerName, bool isForward) {
    return QString("<h2>%1 %2:</h2>").arg(primerName).arg(isForward ? QObject::tr("Forward") : QObject::tr("Reverse"));
}
QString getPrimerForReport(const QString& primer) {
    return QString("<b>%1</b>").arg(primer);
}
QString getBackboneForReport(const QByteArray& backbone) {
    return QString("<u>%1</u>").arg(QString(backbone));
}
// Return html concatenated backbone and forward primer.
QString getForwardPrimerReportResult(const QByteArray& backbone, const QString& primer) {
    return QString("<div>%1%2</div>").arg(getBackboneForReport(backbone)).arg(getPrimerForReport(primer));
}
QString getForwardPrimerReportResult(const QByteArray& backbone, const QByteArray& primer) {
    return getForwardPrimerReportResult(backbone, QString(primer));
}
// Return html concatenated reverse primer and backbone.
QString getReversePrimerReportResult(const QByteArray& backbone, const QString& primer) {
    return QString("<div>%1%2</div>").arg(getPrimerForReport(primer)).arg(getBackboneForReport(backbone));
}
QString getReversePrimerReportResult(const QByteArray& backbone, const QByteArray& primer) {
    return getReversePrimerReportResult(backbone, QString(primer));
}
}    // namespace

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
    saveUnwantedConnectionsReports();
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
    QString report("<br>"
                   "<br>"
                   "<h3>%1</h3>");
    SAFE_POINT(!sequence.isEmpty(), tr("Empty sequence"), report.arg(tr("Error, see log.")))

    const bool primersNotFound = aForward.isEmpty()  && aReverse.isEmpty()  &&
                                 b1Forward.isEmpty() && b1Reverse.isEmpty() &&
                                 b2Forward.isEmpty() && b2Reverse.isEmpty() &&
                                 b3Forward.isEmpty() && b3Reverse.isEmpty();
    const bool noUserPrimers = settings.forwardUserPrimer.isEmpty() || settings.reverseUserPrimer.isEmpty();
    if (primersNotFound && noUserPrimers) {
        return report.arg(tr("There are no primers that meet the specified parameters."));
    }

    report = report.arg(tr("Details:"));
    report += tr("<div>"
                 "<p><u>Underlined</u>&#8211;backbone sequence<br><b>Bold</b>&#8211;primer sequence</p>"
                 "</div>");
    report += getPairReport(aForward, aReverse, "A");
    report += getPairReport(b1Forward, b1Reverse, "B1");
    report += getPairReport(b2Forward, b2Reverse, "B2");
    report += getPairReport(b3Forward, b3Reverse, "B3");

    if (userPrimersUnwantedConnections.isEmpty()) {
        report += getPairReportForUserPrimers();
    } else {
        report += getUserPrimersUnwantedConnectionsReport();
    }
    return report;
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
    for (const auto& regionBetweenIslandsReverse : regionsBetweenIslandsReverse) {
        if (regionBetweenIslandsReverse.length < MINIMUM_LENGTH_BETWEEN_ISLANDS) {
            continue;
        }

        int amplifiedFragmentEdgeReverse = reverseComplementSequence.size() - settings.rightArea.startPos;
        int b1ReverseCandidatePrimerEnd = regionBetweenIslandsReverse.endPos();
        int b1ReversePrimerLength = settings.overlapLength.minValue;

        while (b1ReverseCandidatePrimerEnd - b1ReversePrimerLength > regionBetweenIslandsReverse.startPos) { //While we are in the region between islands
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
                taskLog.details(tr("The \"B1 Reverse\" candidate primer region \"%1\" fits to \"Parameters of priming sequences\" values, check for unwanted connections").arg(b1ReverseCandidatePrimerRegionString));
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
            taskLog.details(tr("The \"%1\" candidate primer region \"%2\" fits to \"Parameters of priming sequences\" values, check for unwanted connections").arg(forwardPrimerName).arg(forwardCandidatePrimerRegionString));
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
        const U2Region reverseCandidatePrimerRegion(reverseCandidatePrimerEnd - reversePrimerLength, reversePrimerLength);
        QByteArray reverseCandidatePrimerSequence = reverseComplementSequence.mid(reverseCandidatePrimerRegion.startPos, reverseCandidatePrimerRegion.length);

        //Check if candidate primer melting temperature and deltaG fit to settings
        bool areSettingsGood = areMetlingTempAndDeltaGood(reverseCandidatePrimerSequence);
        if (!areSettingsGood) {
            updatePrimerRegion(reverseCandidatePrimerEnd, reversePrimerLength);
            continue;
        } else {
            QString reverseCandidatePrimerRegionString = regionToString(reverseCandidatePrimerRegion, true);
            taskLog.details(tr("The \"%1\" candidate primer region \"%2\" fits to \"Parameters of priming sequences\" values, check for unwanted connections").arg(reversePrimerName).arg(reverseCandidatePrimerRegionString));
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

void PCRPrimerDesignForDNAAssemblyTask::saveUnwantedConnectionsReports() {
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
    const QByteArray forward = settings.forwardUserPrimer.toLocal8Bit();
    const QByteArray reverse = settings.reverseUserPrimer.toLocal8Bit();
    if (hasUnwantedConnections(forward)) {
        saveUnwantedConnections(forward, UserPrimer::Forward);
    }
    if (hasUnwantedConnections(reverse)) {
        saveUnwantedConnections(reverse, UserPrimer::Reverse);
    }
    QString heteroReport;
    if (UnwantedConnectionsUtils::isUnwantedHeteroDimer(forward, reverse, settings.gibbsFreeEnergyExclude,
        settings.meltingPointExclude, settings.complementLengthExclude, heteroReport)) {
        userPrimersUnwantedConnections[{UserPrimer::Forward, SeqToSearchInThem::ReverseUser}] << heteroReport;
        userPrimersUnwantedConnections[{UserPrimer::Reverse, SeqToSearchInThem::ForwardUser}] << "";
    }
}

void PCRPrimerDesignForDNAAssemblyTask::saveUnwantedConnections(const QByteArray& primer, UserPrimer primerType) {
    // Synonyms for readability.
    using Sequence = SeqToSearchInThem;
    const int deltaG   = settings.gibbsFreeEnergyExclude,
              meltingT = settings.meltingPointExclude,
              dimerLen = settings.complementLengthExclude;

    QString report_;
    if (UnwantedConnectionsUtils::isUnwantedSelfDimer(primer, deltaG, meltingT, dimerLen, report_)) {
        Sequence s = (primerType == UserPrimer::Forward ? Sequence::ForwardUser : Sequence::ReverseUser);
        userPrimersUnwantedConnections[{primerType, s}] << report_;
    }
    if (UnwantedConnectionsUtils::isUnwantedHeteroDimer(primer, sequence, deltaG, meltingT, dimerLen, report_)) {
        userPrimersUnwantedConnections[{primerType, Sequence::Sequence}] << report_;
    }
    if (UnwantedConnectionsUtils::isUnwantedHeteroDimer(primer, reverseComplementSequence, deltaG, meltingT, dimerLen,
                                                        report_)) {
        userPrimersUnwantedConnections[{primerType, Sequence::RevComplSeq}] << report_;
    }
    for (const QByteArray& otherSeqInPcr : qAsConst(otherSequencesInPcr)) {
        if (UnwantedConnectionsUtils::isUnwantedHeteroDimer(primer, otherSeqInPcr, deltaG, meltingT, dimerLen,
                                                            report_)) {
            userPrimersUnwantedConnections[{primerType, Sequence::OtherSeq}] << report_;
        }
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

QString PCRPrimerDesignForDNAAssemblyTask::regionToString(const U2Region& region, bool isComplement) const {
    U2Region regionToLog = isComplement ? DNASequenceUtils::reverseComplementRegion(region, sequence.size()) : region;
    return QString("%1..%2").arg(regionToLog.startPos + 1).arg(regionToLog.endPos());
}

QString PCRPrimerDesignForDNAAssemblyTask::getPairReport(U2Region forward,
                                                         U2Region reverse,
                                                         const QString &primerName) const {
    QString report;
    if (!forward.isEmpty()) {
        report += getPrimerReportTitle(primerName, true);

        auto firstCandidatePrimerWhileCondition = [&](int primerEnd, int primerLength) {
            return primerEnd - primerLength > regionBetweenIslands.startPos;
        };
        int firstCandidatePrimerEnd = regionBetweenIslands.endPos();
        first = findCandidatePrimer(firstCandidatePrimerEnd, amplifiedFragmentEdge, isComplement, firstCandidatePrimerWhileCondition);

        report += getForwardPrimerReportResult(backboneSequence, sequence.mid(forward.startPos, forward.length));
    }
    if (!reverse.isEmpty()) {
        report += getPrimerReportTitle(primerName, false);

        auto thirdCandidatePrimerWhileCondition = [&](int primerEnd, int) {
            return primerEnd == first.startPos;
        };
        int thirdCandidatePrimerEnd = first.startPos;
        third = findCandidatePrimer(thirdCandidatePrimerEnd, amplifiedFragmentEdge, isComplement, thirdCandidatePrimerWhileCondition);
        if (!third.isEmpty()) {
            taskLog.details(tr("B3 %1 primer has been found").arg(isComplement ? "reverse" : "forvard"));
        }

        report += getReversePrimerReportResult(backboneSequence, sequence.mid(reverse.startPos, reverse.length));
    }
}

QString PCRPrimerDesignForDNAAssemblyTask::getPairReportForUserPrimers() const {
    QString report_;
    if (!settings.forwardUserPrimer.isEmpty() && !settings.reverseUserPrimer.isEmpty()) {
        report_ += getPrimerReportTitle("C", true);
        report_ += QString("<div>%1</div>").arg(getPrimerForReport(settings.forwardUserPrimer));
        report_ += getPrimerReportTitle("C", false);
        report_ += QString("<div>%1</div>").arg(getPrimerForReport(settings.reverseUserPrimer));
    }
    return report_;
}

QString PCRPrimerDesignForDNAAssemblyTask::getUserPrimersUnwantedConnectionsReport() const {
    using Seq = SeqToSearchInThem;
    const QList<Seq> allTypes = { Seq::ForwardUser, Seq::ReverseUser, Seq::Sequence, Seq::RevComplSeq, Seq::OtherSeq };
    const QMap<Seq, QString> toString = { {Seq::ForwardUser, tr("Forward user primer")},
                                          {Seq::ReverseUser, tr("Reverse user primer")},
                                          {Seq::Sequence, tr("Sequence")},
                                          {Seq::RevComplSeq, tr("Reverse complement sequence")},
                                          {Seq::OtherSeq, tr("Other sequences in PCR reaction")} };

    // Summary table for unwanted connections. Rows -- user primers, columns -- sequences with which unwanted
    // connections have been formed.
    QString report_ = tr("<br><h3>Unwanted connections of user primers</h3>"
                         "<div>"
                           "<p>+ means there are unwanted connections,<br>- means there are no unwanted connections.</p>"
                           "<p>Diagonal elements are homodimers, off-diagonal elements are heterodimers.<br></p>"
                         "</div>");
    QString table   = "<table border=\"1\" cellpadding=\"4\">"
                        "<tr>%1</tr>"
                        "<tr>%2</tr>"
                        "<tr>%3</tr>"
                      "</table>";

    QString headerRow = "<td></td>";
    for (auto it = toString.constBegin(); it != toString.constEnd(); ++it) {
        headerRow += QString("<th>%1</th>").arg(it.value());
    }

    QString forwardRow = tr("<th>Forward user primer</th>");
    for (int i = 0; i < allTypes.size(); i++) {
        char ch = '-';    // In cell '-' means that no unwanted connections were found.
        if (userPrimersUnwantedConnections.contains({ UserPrimer::Forward, allTypes[i] })) {
            ch = '+';
        }
        forwardRow += "<td align=\"center\">";
        forwardRow += ch;
        forwardRow += "</td>";
    }

    QString reverseRow = tr("<th>Reverse user primer</th>");
    for (int i = 0; i < allTypes.size(); i++) {
        char ch = '-';
        if (userPrimersUnwantedConnections.contains({ UserPrimer::Reverse, allTypes[i] })) {
            ch = '+';
        }
        reverseRow += "<td align=\"center\">";
        reverseRow += ch;
        reverseRow += "</td>";
    }

    report_ += table.arg(headerRow).arg(forwardRow).arg(reverseRow);
    report_ += "<br>";

    // Add reports about unwanted connections.
    for (auto it = userPrimersUnwantedConnections.constBegin(); it != userPrimersUnwantedConnections.constEnd(); ++it) {
        const UserPrimer userPrimer = it.key().first;
        const Seq s = it.key().second;
        if (userPrimer == UserPrimer::Reverse && s == Seq::ForwardUser) {
            // (forward, reverse) report same as (reverse, forward).
            continue;
        }

        const QString userPrimerStr =
            (userPrimer == UserPrimer::Forward ? toString[Seq::ForwardUser]: toString[Seq::ReverseUser]);
        const bool isHomodimer = (userPrimer == UserPrimer::Forward && s == Seq::ForwardUser) ||
                                 (userPrimer == UserPrimer::Reverse && s == Seq::ReverseUser);

        if (isHomodimer) {
            report_ += tr("<br><br><u>%1:</u><br><br>").arg(userPrimerStr);
        } else {
            report_ += tr("<br><br><u>Connections between %1 and %2:</u><br><br>").arg(userPrimerStr).arg(toString[s]);
        }
        report_ += it.value().join("<br>");
    }
    return report_;
}



}
