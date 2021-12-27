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

#include "PCRPrimerDesignTaskReportUtils.h"

#include <U2Core/U2SafePoints.h>

#include "../PCRPrimerDesignForDNAAssemblyPlugin.h"
#include "../tasks/PCRPrimerDesignForDNAAssemblyTask.h"

namespace U2 {

// Returns error message in html.
static QString errMsg() {
    return PCRPrimerDesignForDNAAssemblyPlugin::tr("<p>Error, see log.</p>");
}

// Returns primerName as html header. Wraps primerName in <h2> tag. primerName must not be empty.
static QString primerHeader(const QString &primerName) {
    return "<h2>" + primerName + ":</h2>";
}

// Returns primer in bold. Wraps primer in <b> tag. primer must not be empty.
static QString primerToHtml(const QString &primer) {
    return "<b>" + primer + "</b>";
}

// Functions below return row and column headers.
static QString forwardUserPrimerStr() {
    return PCRPrimerDesignForDNAAssemblyPlugin::tr("Forward user primer");
}
static QString reverseUserPrimerStr() {
    return PCRPrimerDesignForDNAAssemblyPlugin::tr("Reverse user primer");
}
static QString sequenceStr() {
    return PCRPrimerDesignForDNAAssemblyPlugin::tr("Sequence");
}
static QString revComplSeqStr() {
    return PCRPrimerDesignForDNAAssemblyPlugin::tr("Reverse complementary sequence");
}
static QString otherSequencesStr() {
    return PCRPrimerDesignForDNAAssemblyPlugin::tr("Other sequences in PCR reaction");
}

// Returns "name1 and name2". name1 and name2 must not be empty.
static QString concatenateNames(const QString &name1, const QString &name2) {
    return name1 + PCRPrimerDesignForDNAAssemblyPlugin::tr(" and ") + name2;
}

// Are user primers set?
static bool hasUserPrimers(const PCRPrimerDesignForDNAAssemblyTaskSettings &settings) {
    return !settings.forwardUserPrimer.isEmpty() && !settings.reverseUserPrimer.isEmpty();
}

/**
 * Checks correctness of found primer.
 * If no region is specified, empty string is returned.
 * If region is incorrect, error is displayed.
 * If region is ok, returns primerName as header and primer with backbone.
 * primerName and sequence must not be empty.
 */
static QString checkPrimerAndGetInfo(const U2Region region, const QString &primerName, const bool isForward,
                                     const QByteArray &sequence, const QString &backbone) {
    QString ans;
    if (!region.isEmpty()) {
        ans += primerHeader(primerName);
        SAFE_POINT(region.startPos > 0 && region.startPos < sequence.length() && region.length > 0,
                   QString("Invalid region %1 for sequence length %2 (%3 primer)")
                       .arg(region.toString())
                       .arg(sequence.length())
                       .arg(primerName),
                   ans += errMsg())

        QString res = "<p>%1%2</p>";
        QString primer = primerToHtml(QString(sequence.mid(region.startPos, region.length)));
        if (isForward) {
            ans += res.arg(backbone, primer);
        } else {
            ans += res.arg(primer, backbone);
        }
    }
    return ans;
}

/**
 * Returns non-empty html report of found primers A, B1, B2, B3 in sequence and user primers.
 * sequence must not be empty.
 */
static QString primersInfo(const PCRPrimerDesignForDNAAssemblyTask &task,
                           const QByteArray &sequence,
                           const QByteArray &revComplSeq,
                           const PCRPrimerDesignTaskReportUtils::UserPrimersReports &reports) {
    SAFE_POINT(!sequence.isEmpty(), "Empty sequence", errMsg())

    const QStringList &primersNames = PCRPrimerDesignForDNAAssemblyTask::FRAGMENT_INDEX_TO_NAME;
    QList<U2Region> regions = task.getResults();
    SAFE_POINT(regions.size() == primersNames.size(),
               QString("The number of resulting primers (%1) isn't equal to the number of primer names (%2)")
                   .arg(regions.size())
                   .arg(primersNames.size()),
               errMsg())

    bool primersNotFound = std::all_of(regions.begin(), regions.end(), [](U2Region r) { return r.isEmpty(); });
    bool areUserPrimersBad = !hasUserPrimers(task.getSettings()) || reports.hasUnwantedConnections();
    if (primersNotFound && areUserPrimersBad) {
        return PCRPrimerDesignForDNAAssemblyPlugin::tr("<p>There are no primers that meet the specified parameters."
                                                       "</p>");
    }

    QString ans;
    // Desciption.
    ans += PCRPrimerDesignForDNAAssemblyPlugin::tr("<h3>Details:</h3>"
                                                   "<p>"
                                                   "<u>Underlined</u>&#8211;backbone sequence&#59;<br>"
                                                   "<b>Bold</b>&#8211;primer sequence."
                                                   "</p>");

    QString backbone = task.getBackboneSequence().isEmpty() ? task.getBackboneSequence() : "<u>" +
        task.getBackboneSequence() + "</u>";

    // Result primers.
    for (int i = 0; i + 1 < regions.size(); i += 2) {
        QVector<U2Region> reverseRegion = {regions[i + 1]};
        U2Region::mirror(sequence.size(), reverseRegion);
        ans += checkPrimerAndGetInfo(regions[i], primersNames[i], true, sequence, backbone);
        ans += checkPrimerAndGetInfo(reverseRegion.first(), primersNames[i + 1], false, revComplSeq, backbone);
    }
    // User primers.
    if (!areUserPrimersBad) {
        ans += primerHeader("C Forward");
        ans += primerToHtml(task.getSettings().forwardUserPrimer);
        ans += primerHeader("C Reverse");
        ans += primerToHtml(task.getSettings().reverseUserPrimer);
    }
    return ans;
}

/**
 * If report is empty, empty string is returned.
 * Otherwise, returns concatenated header and report of unwanted user primer connections.
 * header must not be empty.
 */
static QString returnReportIfAny(const QString &header, const QString &report) {
    return report.isEmpty() ? QString() : QString("<p><u>%1:</u></p><p>%2</p>").arg(header, report);
}

// Returns html string containing list of heterodimer reports of single user primer. primerName must not be empty.
static QString heterodimerInfoForOnePrimer(const QString &primerName,
        const PCRPrimerDesignTaskReportUtils::UserPrimersReports::PrimerReports &reports) {
    return returnReportIfAny(concatenateNames(primerName, sequenceStr()), reports.fileSeq) +
           returnReportIfAny(concatenateNames(primerName, revComplSeqStr()), reports.fileRevComplSeq) +
           returnReportIfAny(concatenateNames(primerName, otherSequencesStr()), reports.other.join("<br>"));
}

/////////////////////////////////////////////PCRPrimerDesignTaskReportUtils/////////////////////////////////////////////
QString PCRPrimerDesignTaskReportUtils::generateReport(const PCRPrimerDesignForDNAAssemblyTask &task,
                                                       const QByteArray &fileSequence,
                                                       const QByteArray &revComplSeq,
                                                       const UserPrimersReports &reports) {
    return "<br><br>" + primersInfo(task, fileSequence, revComplSeq, reports) +
           userPrimersUnwantedConnectionsInfo(task.getSettings(), reports);
}

QString PCRPrimerDesignTaskReportUtils::userPrimersUnwantedConnectionsInfo(
    const PCRPrimerDesignForDNAAssemblyTaskSettings &settings,
    const PCRPrimerDesignTaskReportUtils::UserPrimersReports &reports) {
    QString ans;
    if (hasUserPrimers(settings) && reports.hasUnwantedConnections()) {
        ans += PCRPrimerDesignForDNAAssemblyPlugin::tr("<h2>Unwanted connections of user primers</h2>");

        if (reports.hasSelfdimers()) {
            QString selfdimerStr = PCRPrimerDesignForDNAAssemblyPlugin::tr("Self-dimers");
            PCRPrimerDesignTaskReportUtils::UserPrimersTable table(selfdimerStr, {selfdimerStr});
            table.addForwardRow({!reports.forward.selfdimer.isEmpty()});
            table.addReverseRow({!reports.reverse.selfdimer.isEmpty()});

            ans += table.getTable();
            ans += returnReportIfAny(forwardUserPrimerStr(), reports.forward.selfdimer);
            ans += returnReportIfAny(reverseUserPrimerStr(), reports.reverse.selfdimer);
        }

        if (reports.hasHeterodimers()) {
            const auto getRow = [&reports](const UserPrimersReports::PrimerReports &r) -> QList<bool> {
                return {!reports.heterodimer.isEmpty(), !r.fileSeq.isEmpty(), !r.fileRevComplSeq.isEmpty(),
                        !r.other.isEmpty()};
            };
            UserPrimersTable table(PCRPrimerDesignForDNAAssemblyPlugin::tr("Hetero-dimers"),
                                   {PCRPrimerDesignForDNAAssemblyPlugin::tr("Another user primer"),
                                    sequenceStr(),
                                    revComplSeqStr(),
                                    otherSequencesStr()});
            table.addForwardRow(getRow(reports.forward));
            table.addReverseRow(getRow(reports.reverse));

            ans += table.getTable();
            ans += heterodimerInfoForOnePrimer(forwardUserPrimerStr(), reports.forward);
            ans += heterodimerInfoForOnePrimer(reverseUserPrimerStr(), reports.reverse);
            ans += returnReportIfAny(concatenateNames(forwardUserPrimerStr(), reverseUserPrimerStr()),
                                     reports.heterodimer);
        }
    }
    return ans;
}

///////////////////////////////////////////////////UserPrimersReports///////////////////////////////////////////////////
bool PCRPrimerDesignTaskReportUtils::UserPrimersReports::hasSelfdimers() const {
    return !forward.selfdimer.isEmpty() || !reverse.selfdimer.isEmpty();
}

bool PCRPrimerDesignTaskReportUtils::UserPrimersReports::hasHeterodimers() const {
    return !forward.selfdimer.isEmpty() || !reverse.selfdimer.isEmpty() ||
           !forward.fileSeq.isEmpty() || !reverse.fileSeq.isEmpty() ||
           !forward.fileRevComplSeq.isEmpty() || !reverse.fileRevComplSeq.isEmpty() ||
           !forward.other.isEmpty() || !reverse.other.isEmpty() ||
           !heterodimer.isEmpty();
}

bool PCRPrimerDesignTaskReportUtils::UserPrimersReports::hasUnwantedConnections() const {
    return hasSelfdimers() || hasHeterodimers();
}

////////////////////////////////////////////////////UserPrimersTable////////////////////////////////////////////////////
QString PCRPrimerDesignTaskReportUtils::UserPrimersTable::getRow(const QList<bool> &row) const {
    QString ans;
    for (bool cell : qAsConst(row)) {
        ans += "<td align='center'>" +
               (cell ? PCRPrimerDesignForDNAAssemblyPlugin::tr("Yes") : PCRPrimerDesignForDNAAssemblyPlugin::tr("No")) +
               "</td>";
    }
    return ans;
}

PCRPrimerDesignTaskReportUtils::UserPrimersTable::UserPrimersTable(const QString &name, const QStringList &headers)
    : name(name), headers(headers) {
}

void PCRPrimerDesignTaskReportUtils::UserPrimersTable::addForwardRow(const QList<bool> &row) {
    forwardRow = row;
}

void PCRPrimerDesignTaskReportUtils::UserPrimersTable::addReverseRow(const QList<bool> &row) {
    reverseRow = row;
}

QString PCRPrimerDesignTaskReportUtils::UserPrimersTable::getTable() const {
    QString header;
    for (const QString &h : qAsConst(headers)) {
        header += "<th>" + h + "</th>";
    }

    return QString("<h3>%1:</h3>"  // Table name
                   "<table border='1' cellpadding='4'>"
                   "<tr>"
                   "<td></td>"
                   "%2"  // Column headers.
                   "</tr>"
                   "<tr>"
                   "<th>%3</th>"  // Forward user primer str.
                   "%4"  // Unwanted connections of forward user primer.
                   "</tr>"
                   "<tr>"
                   "<th>%5</th>"  // Reverse user primer str.
                   "%6"  // Unwanted connections of reverse user primer.
                   "</tr>"
                   "</table>")
        .arg(name)
        .arg(header)
        .arg(forwardUserPrimerStr())
        .arg(getRow(forwardRow))
        .arg(reverseUserPrimerStr())
        .arg(getRow(reverseRow));
}

}  // namespace U2
