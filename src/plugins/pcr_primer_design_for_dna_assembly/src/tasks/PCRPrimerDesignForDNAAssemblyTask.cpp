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

#include <QApplication>
#include <QMessageBox>

#include <algorithm>

#include "utils/UnwantedConnectionsUtils.h"
#include "FindPresenceOfUnwantedParametersTask.h"
#include "FindUnwantedIslandsTask.h"
#include "UnwantedStructuresInBackboneDialog.h"

#include <U2Core/PrimerStatistics.h>

namespace U2 {

//To generate a PCRPrimerDesignForDNAAssemblyTask report.
namespace ReportUtils {
using PcrTask = PCRPrimerDesignForDNAAssemblyTask;
using Reports = PcrTask::UserPrimerReports;

static QString forwardUserPrimerStr();
static QString reverseUserPrimerStr();

//No validation of columns number when adding rows.
class UserPrimersTable {
    const QString name;
    const QStringList headers;
    QList<bool> forwardRow;
    QList<bool> reverseRow;

    QString getHeader() const {
        QString ans;
        for (const QString& h : headers) {
            ans += "<th>" + h + "</th>";
        }
        return ans;
    }
    QString getRow(const QList<bool>& row) const {
        QString ans;
        for (const bool cell : row) {
            ans += "<td align='center'>" + (cell ? QObject::tr("Yes") : QObject::tr("No")) + "</td>";
        }
        return ans;
    }

public:
    UserPrimersTable(const QString& name, const QStringList& headers) : name(name), headers(headers) {}

    void addForwardRow(const QList<bool>& row) {
        forwardRow = row;
    }
    void addReverseRow(const QList<bool>& row) {
        reverseRow = row;
    }
    QString getTable() const {
        return QString("<h3>%1:</h3>"             //Table name
            "<table border='1' cellpadding='4'>"
              "<tr>"
                "<td></td>"
                "%2"                              //Column headers.
              "</tr>"
              "<tr>"
                "<th>%3</th>"                     //Forward user primer str.
                "%4"                              //Unwanted connections of forward user primer.
              "</tr>"
              "<tr>"
                "<th>%5</th>"                     //Reverse user primer str.
                "%6"                              //Unwanted connections of reverse user primer.
              "</tr>"
            "</table>").arg(name).arg(getHeader()).arg(forwardUserPrimerStr()).arg(getRow(forwardRow))
                       .arg(reverseUserPrimerStr()).arg(getRow(reverseRow));
    }
};

static QString errMsg() {
    return QObject::tr("<p>Error, see log.</p>");
}

static QString forwardUserPrimerStr() {
    return QObject::tr("Forward user primer");
}

static QString reverseUserPrimerStr() {
    return QObject::tr("Reverse user primer");
}

static QString sequenceStr() {
    return QObject::tr("File sequence");
}

static QString revComplSeqStr() {
    return QObject::tr("Reverse complementary file sequence");
}

static QString otherSequencesStr() {
    return QObject::tr("Other sequences in PCR reaction");
}

static QString primerHeader(const QString& primerName) {
    return "<h2>" + primerName + ":</h2>";
}

static QString primerToHtml(QString primer) {
    return "<b>" + primer + "</b>";
}

static bool areUserPrimers(const PCRPrimerDesignForDNAAssemblyTaskSettings& settings) {
    return !settings.forwardUserPrimer.isEmpty() && !settings.reverseUserPrimer.isEmpty();
}

static bool hasHeterodimer(const Reports& reports) {
    return !reports.fileSeq.isEmpty() || !reports.fileRevComplSeq.isEmpty() || !reports.other.isEmpty();
}

static bool hasUserPrimersUnwantedConnections(const Reports& forward, const Reports& reverse,
                                              const QString& userHeterodimer) {
    const auto hasConnections = [](const Reports& reports) {
        return !reports.selfdimer.isEmpty() || hasHeterodimer(reports);
    };
    return hasConnections(forward) || hasConnections(reverse) || !userHeterodimer.isEmpty();
}

static QString checkPrimerAndGetInfo(const U2Region region, const QString& primerName, const bool isForward,
                                     const QByteArray& sequence, const QString& backbone) {
    QString ans;
    if (!region.isEmpty()) {
        ans += primerHeader(primerName);
        SAFE_POINT(region.startPos > 0 && region.startPos < sequence.length() && region.length > 0,
                   QObject::tr("Invalid region %1 for sequence length %2 (%3 primer)").arg(region.toString()).
                               arg(sequence.length()).arg(primerName),
                   ans += errMsg())

        const QString res = "<p>%1%2</p>";
        const QString primer = primerToHtml(QString(sequence.mid(region.startPos, region.length)));
        if (isForward) {
            ans += res.arg(backbone, primer);
        } else {
            ans += res.arg(primer, backbone);
        }
    }
    return ans;
}

//All found primers + backbone in html format. User primers are good if both are specified and both have no unwanted
//connections.
static QString primersInfo(const PcrTask& task, const QByteArray& sequence, const bool areUserPrimersGood) {
    const PCRPrimerDesignForDNAAssemblyTaskSettings& settings = task.getSettings();
    const QList<U2Region> regions = task.getResults();
    SAFE_POINT(regions.size() == PcrTask::FRAGMENT_INDEX_TO_NAME.size(),
               QObject::tr("The number of resulting primers (%1) isn't equal to the number of primer names (%2)").
                           arg(regions.size()).arg(PcrTask::FRAGMENT_INDEX_TO_NAME.size()),
               errMsg())

    const bool primersNotFound = std::all_of(regions.begin(), regions.end(), [](U2Region r) { return r.isEmpty(); });
    if (primersNotFound && !areUserPrimersGood) {
        return QObject::tr("<p>There are no primers that meet the specified parameters.</p>");
    }

    QString ans;
    //Desciption.
    ans += QObject::tr("<h3>Details:</h3>"
                       "<p>"
                         "<u>Underlined</u>&#8211;backbone sequence&#59;<br>"
                         "<b>Bold</b>&#8211;primer sequence."
                       "</p>");

    QString backbone = task.getBackboneSequence();
    backbone = backbone.isEmpty() ? backbone : "<u>" + backbone + "</u>";

    //Result primers.
    for (int i = 0; i + 1 < regions.size(); i += 2) {
        ans += checkPrimerAndGetInfo(regions[i],     PcrTask::FRAGMENT_INDEX_TO_NAME[i],     true,  sequence, backbone);
        ans += checkPrimerAndGetInfo(regions[i + 1], PcrTask::FRAGMENT_INDEX_TO_NAME[i + 1], false, sequence, backbone);
    }
    //User primers.
    if (areUserPrimersGood) {
        ans += primerHeader("C Forward");
        ans += primerToHtml(settings.forwardUserPrimer);
        ans += primerHeader("C Reverse");
        ans += primerToHtml(settings.reverseUserPrimer);
    }
    return ans;
}

static QString selfdimerTable(const QString& forwardSelfdimer, const QString& reverseSelfdimer) {
    const QString selfdimerStr = QObject::tr("Selfdimers");
    UserPrimersTable table(selfdimerStr, { selfdimerStr });
    table.addForwardRow({ !forwardSelfdimer.isEmpty() });
    table.addReverseRow({ !reverseSelfdimer.isEmpty() });
    return table.getTable();
}

QString returnReportIfAny(const QString& header, const QString& report) {
    if (report.isEmpty()) {
        return {};
    }
    return QString("<p><u>%1:</u></p><p>%2</p>").arg(header, report);
}

static QString userSelfdimersInfo(const QString& forwardSelfdimer, const QString& reverseSelfdimer) {
    QString ans = returnReportIfAny(forwardUserPrimerStr(), forwardSelfdimer);
    return ans += returnReportIfAny(reverseUserPrimerStr(), reverseSelfdimer);
}

static QString heterodimersTable(const Reports& forward, const Reports& reverse, const QString& userHeterodimer) {
    const auto getRow = [&userHeterodimer](const Reports& reports) -> QList<bool> {
        return { !userHeterodimer.isEmpty(), !reports.fileSeq.isEmpty(), !reports.fileRevComplSeq.isEmpty(),
                 !reports.other.isEmpty() };
    };
    UserPrimersTable table(QObject::tr("Heterodimers"), {QObject::tr("Another user primer"), sequenceStr(),
                                                         revComplSeqStr(), otherSequencesStr()});
    table.addForwardRow(getRow(forward));
    table.addReverseRow(getRow(reverse));
    return table.getTable();
}

static QString concatenatePrimerNames(const QString& primer, const QString& sequence) {
    return primer + QObject::tr(" and ") + sequence;
}

static QString heterodimerInfoForOnePrimer(const QString& primerName, const Reports& reports) {
    QString ans = returnReportIfAny(concatenatePrimerNames(primerName, sequenceStr()), reports.fileSeq);
    ans += returnReportIfAny(concatenatePrimerNames(primerName, revComplSeqStr()), reports.fileRevComplSeq);
    return ans += returnReportIfAny(concatenatePrimerNames(primerName, otherSequencesStr()), reports.other.join("<br>"));
}

static QString userHeterodimersInfo(const Reports& forward, const Reports& reverse, const QString& userHeterodimer) {
    QString ans = heterodimerInfoForOnePrimer(forwardUserPrimerStr(), forward) +
                  heterodimerInfoForOnePrimer(reverseUserPrimerStr(), reverse);
    return ans += returnReportIfAny(concatenatePrimerNames(forwardUserPrimerStr(), reverseUserPrimerStr()),
                                    userHeterodimer);
}

//Unwanted connections of user primers in html format.
static QString userPrimersUnwantedConnectionsInfo(const PCRPrimerDesignForDNAAssemblyTaskSettings& settings,
                                                  const Reports& forward, const Reports& reverse,
                                                  const QString& userHeterodimer) {
    QString ans;
    if (areUserPrimers(settings) && hasUserPrimersUnwantedConnections(forward, reverse, userHeterodimer)) {
        ans += QObject::tr("<h2>Unwanted connections of user primers</h2>");
        if (!forward.selfdimer.isEmpty() || !reverse.selfdimer.isEmpty()) {
            ans += selfdimerTable(forward.selfdimer, reverse.selfdimer);
            ans += userSelfdimersInfo(forward.selfdimer, reverse.selfdimer);
        }
        if (hasHeterodimer(forward) || hasHeterodimer(reverse) || !userHeterodimer.isEmpty()) {
            ans += heterodimersTable(forward, reverse, userHeterodimer);
            ans += userHeterodimersInfo(forward, reverse, userHeterodimer);
        }
    }
    return ans;
}
}

const QStringList PCRPrimerDesignForDNAAssemblyTask::FRAGMENT_INDEX_TO_NAME = {
    QObject::tr("A Forward"),
    QObject::tr("A Reverse"),
    QObject::tr("B1 Forward"),
    QObject::tr("B1 Reverse"),
    QObject::tr("B2 Forward"),
    QObject::tr("B2 Reverse"),
    QObject::tr("B3 Forward"),
    QObject::tr("B3 Reverse")
};

PCRPrimerDesignForDNAAssemblyTask::PCRPrimerDesignForDNAAssemblyTask(const PCRPrimerDesignForDNAAssemblyTaskSettings& _settings, const QByteArray& _sequence)
    : Task("PCR Primer Design For DNA Assembly Task", TaskFlags_FOSCOE | TaskFlag_ReportingIsSupported | TaskFlag_ReportingIsEnabled),
      settings(_settings),
      sequence(_sequence),
      reverseComplementSequence(DNASequenceUtils::reverseComplement(sequence)) {
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
    // We have A and B1, B2, B3 pairs of primers
    // A and B1 are pretty the same, except the fact that B1 has some depencedcies on B2 and B3
    // So, the first time we find good B1 pair they are also the A pair. But if B1 doesn't fit to B2 or B3, we need to continue searching B1, which will be fit to B2 and B3
    bool aWasNotFoundYet = true;
    for (const auto& regionBetweenIslandsForward : regionsBetweenIslandsForward) {
        if (regionBetweenIslandsForward.length < MINIMUM_LENGTH_BETWEEN_ISLANDS) {
            continue;
        }
        const int amplifiedFragmentLeftEdge = settings.leftArea.endPos() - 1;
        int b1ForwardCandidatePrimerEnd = regionBetweenIslandsForward.endPos();
        int b1ForwardPrimerLength = settings.overlapLength.minValue;

        //While the primer start position is in the region between islands
        while (b1ForwardCandidatePrimerEnd - b1ForwardPrimerLength > regionBetweenIslandsForward.startPos) {
            const U2Region b1ForwardCandidatePrimerRegion(b1ForwardCandidatePrimerEnd - b1ForwardPrimerLength, b1ForwardPrimerLength);
            // The amplified fragment shouldn't contain the all primer
            // So move the primer untill at least one character out of the amplified fragment
            if (amplifiedFragmentLeftEdge < b1ForwardCandidatePrimerRegion.startPos) {
                b1ForwardCandidatePrimerEnd--;
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
                taskLog.details(tr("The \"B1 Forward\" candidate primer region \"%1\" fits to \"Parameters of priming sequences\" values, check for unwanted connections").arg(QString(b1ForwardCandidatePrimerRegionString)));
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
    saveUserPrimersReports();
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
                taskLog.details(tr("The backbone sequence without unwanted hairpins, self- and hetero-dimers has been found: %1").arg(QString(backboneSequence)));
                return {};
            }
            // There are unwanted structures -> Asking the user if this sequence with unwanted structures should be used
            // as backbone?
            taskLog.details(tr("The following backbone sequence candidate contains parameters: %1")
                                .arg(QString(consideredBackboneSequence)));
            int userResponse = UnwantedStructuresInBackboneDialog(consideredBackboneSequence,
                                                                  checkBackboneSequence->getUnwantedStructures(),
                                                                  backboneSequencesCandidates.length())
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
            taskLog.error(tr("The file \"%1\" doesn't contain the backbone sequence, which matchs the parameters. "
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
    using namespace ReportUtils;
    SAFE_POINT(!sequence.isEmpty(), tr("Empty sequence"), errMsg())

    const bool areUserPrimersGood = areUserPrimers(settings) &&
                                    !hasUserPrimersUnwantedConnections(forwardReports, reverseReports, userHeterodimer);
    return "<br><br>" + primersInfo(*this, sequence, areUserPrimersGood) +
           userPrimersUnwantedConnectionsInfo(settings, forwardReports, reverseReports, userHeterodimer);
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

void PCRPrimerDesignForDNAAssemblyTask::saveUserPrimersReports() {
    if (settings.forwardUserPrimer.isEmpty() || settings.reverseUserPrimer.isEmpty()) {
        if (settings.forwardUserPrimer.isEmpty() && settings.reverseUserPrimer.isEmpty()) {
            taskLog.details(tr("No user primers"));
        }
        else if (settings.forwardUserPrimer.isEmpty()) {
            taskLog.error(tr("No forward user primer. Reverse user primer ignored"));
        }
        else if (settings.reverseUserPrimer.isEmpty()) {
            taskLog.error(tr("No reverse user primer. Forward user primer ignored"));
        }
        return;
    }

    const int deltaG   = settings.gibbsFreeEnergyExclude,
              meltingT = settings.meltingPointExclude,
              dimerLen = settings.complementLengthExclude;
    const auto saveOnePrimerReports = [deltaG, meltingT, dimerLen, this](const QByteArray& primer, UserPrimerReports& reports) {
        QString report_;
        if (UnwantedConnectionsUtils::isUnwantedSelfDimer(primer, deltaG, meltingT, dimerLen, report_)) {
            reports.selfdimer = report_;
        }
        if (UnwantedConnectionsUtils::isUnwantedHeteroDimer(primer, sequence, deltaG, meltingT, dimerLen, report_)) {
            reports.fileSeq = report_;
        }
        if (UnwantedConnectionsUtils::isUnwantedHeteroDimer(primer, reverseComplementSequence, deltaG, meltingT,
                                                            dimerLen, report_)) {
            reports.fileRevComplSeq = report_;
        }
        for (const QByteArray& otherSeqInPcr : qAsConst(otherSequencesInPcr)) {
            if (UnwantedConnectionsUtils::isUnwantedHeteroDimer(primer, otherSeqInPcr, deltaG, meltingT, dimerLen,
                                                                report_)) {
                reports.other << report_;
            }
        }

    };

    const QByteArray forward = settings.forwardUserPrimer.toLocal8Bit();
    const QByteArray reverse = settings.reverseUserPrimer.toLocal8Bit();
    saveOnePrimerReports(forward, forwardReports);
    saveOnePrimerReports(reverse, reverseReports);

    QString report_;
    if (UnwantedConnectionsUtils::isUnwantedHeteroDimer(forward, reverse, deltaG, meltingT, dimerLen, report_)) {
        userHeterodimer = report_;
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
