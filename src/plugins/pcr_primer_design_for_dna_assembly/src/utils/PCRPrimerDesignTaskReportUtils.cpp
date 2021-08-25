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

#include "../PCRPrimerDesignForDNAAssemblyTaskSettings.h"
#include "../tasks/PCRPrimerDesignForDNAAssemblyTask.h"

namespace U2 {

using PcrTask = PCRPrimerDesignForDNAAssemblyTask;

static QString forwardUserPrimerStr();
static QString reverseUserPrimerStr();

////////////////////////////////////////////////////UserPrimersTable////////////////////////////////////////////////////
QString PCRPrimerDesignTaskReportUtils::UserPrimersTable::getHeader() const {
    QString ans;
    for (const QString& h : headers) {
        ans += "<th>" + h + "</th>";
    }
    return ans;
}

QString PCRPrimerDesignTaskReportUtils::UserPrimersTable::getRow(const QList<bool>& row) const {
    QString ans;
    for (const bool cell : row) {
        ans += "<td align='center'>" + (cell ? QObject::tr("Yes") : QObject::tr("No")) + "</td>";
    }
    return ans;
}

PCRPrimerDesignTaskReportUtils::UserPrimersTable::UserPrimersTable(const QString& name, const QStringList& headers) :
    name(name), headers(headers) {}

void PCRPrimerDesignTaskReportUtils::UserPrimersTable::addForwardRow(const QList<bool>& row) {
    forwardRow = row;
}

void PCRPrimerDesignTaskReportUtils::UserPrimersTable::addReverseRow(const QList<bool>& row) {
    reverseRow = row;
}

QString PCRPrimerDesignTaskReportUtils::UserPrimersTable::getTable() const {
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

/////////////////////////////////////////////PCRPrimerDesignTaskReportUtils/////////////////////////////////////////////
QString PCRPrimerDesignTaskReportUtils::errMsg() {
    return QObject::tr("<p>Error, see log.</p>");
}

QString PCRPrimerDesignTaskReportUtils::forwardUserPrimerStr() {
    return QObject::tr("Forward user primer");
}

QString PCRPrimerDesignTaskReportUtils::reverseUserPrimerStr() {
    return QObject::tr("Reverse user primer");
}

QString PCRPrimerDesignTaskReportUtils::sequenceStr() {
    return QObject::tr("File sequence");
}

QString PCRPrimerDesignTaskReportUtils::revComplSeqStr() {
    return QObject::tr("Reverse complementary file sequence");
}

QString PCRPrimerDesignTaskReportUtils::otherSequencesStr() {
    return QObject::tr("Other sequences in PCR reaction");
}

QString PCRPrimerDesignTaskReportUtils::primerHeader(const QString& primerName) {
    return "<h2>" + primerName + ":</h2>";
}

QString PCRPrimerDesignTaskReportUtils::primerToHtml(QString primer) {
    return "<b>" + primer + "</b>";
}

bool PCRPrimerDesignTaskReportUtils::areUserPrimers(const PCRPrimerDesignForDNAAssemblyTaskSettings& settings) {
    return !settings.forwardUserPrimer.isEmpty() && !settings.reverseUserPrimer.isEmpty();
}

bool PCRPrimerDesignTaskReportUtils::hasHeterodimer(const UserPrimerReports& reports) {
    return !reports.fileSeq.isEmpty() || !reports.fileRevComplSeq.isEmpty() || !reports.other.isEmpty();
}

bool PCRPrimerDesignTaskReportUtils::hasUserPrimersUnwantedConnections(const UserPrimerReports& forward,
                                                                       const UserPrimerReports& reverse,
                                                                       const QString& userHeterodimer) {
    const auto hasConnections = [](const UserPrimerReports& reports) {
        return !reports.selfdimer.isEmpty() || hasHeterodimer(reports);
    };
    return hasConnections(forward) || hasConnections(reverse) || !userHeterodimer.isEmpty();
}

QString PCRPrimerDesignTaskReportUtils::checkPrimerAndGetInfo(const U2Region region, const QString& primerName,
                                                              const bool isForward, const QByteArray& sequence,
                                                              const QString& backbone) {
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

QString PCRPrimerDesignTaskReportUtils::primersInfo(const PcrTask& task, const QByteArray& sequence,
                                                   const bool areUserPrimersGood) {
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

QString PCRPrimerDesignTaskReportUtils::selfdimerTable(const QString& forwardSelfdimer,
                                                       const QString& reverseSelfdimer) {
    const QString selfdimerStr = QObject::tr("Selfdimers");
    UserPrimersTable table(selfdimerStr, { selfdimerStr });
    table.addForwardRow({ !forwardSelfdimer.isEmpty() });
    table.addReverseRow({ !reverseSelfdimer.isEmpty() });
    return table.getTable();
}

QString PCRPrimerDesignTaskReportUtils::returnReportIfAny(const QString& header, const QString& report) {
    if (report.isEmpty()) {
        return {};
    }
    return QString("<p><u>%1:</u></p><p>%2</p>").arg(header, report);
}

QString PCRPrimerDesignTaskReportUtils::userSelfdimersInfo(const QString& forwardSelfdimer,
                                                           const QString& reverseSelfdimer) {
    QString ans = returnReportIfAny(forwardUserPrimerStr(), forwardSelfdimer);
    return ans += returnReportIfAny(reverseUserPrimerStr(), reverseSelfdimer);
}

QString PCRPrimerDesignTaskReportUtils::heterodimersTable(const UserPrimerReports& forward,
                                                          const UserPrimerReports& reverse,
                                                          const QString& userHeterodimer) {
    const auto getRow = [&userHeterodimer](const UserPrimerReports& reports) -> QList<bool> {
        return { !userHeterodimer.isEmpty(), !reports.fileSeq.isEmpty(), !reports.fileRevComplSeq.isEmpty(),
                 !reports.other.isEmpty() };
    };
    UserPrimersTable table(QObject::tr("Heterodimers"), {QObject::tr("Another user primer"), sequenceStr(),
                                                         revComplSeqStr(), otherSequencesStr()});
    table.addForwardRow(getRow(forward));
    table.addReverseRow(getRow(reverse));
    return table.getTable();
}

QString PCRPrimerDesignTaskReportUtils::concatenatePrimerNames(const QString& primer, const QString& sequence) {
    return primer + QObject::tr(" and ") + sequence;
}

QString PCRPrimerDesignTaskReportUtils::heterodimerInfoForOnePrimer(const QString& primerName,
                                                                    const UserPrimerReports& reports) {
    QString ans = returnReportIfAny(concatenatePrimerNames(primerName, sequenceStr()), reports.fileSeq);
    ans += returnReportIfAny(concatenatePrimerNames(primerName, revComplSeqStr()), reports.fileRevComplSeq);
    return ans += returnReportIfAny(concatenatePrimerNames(primerName, otherSequencesStr()), reports.other.join("<br>"));
}

QString PCRPrimerDesignTaskReportUtils::userHeterodimersInfo(const UserPrimerReports& forward,
                                                             const UserPrimerReports& reverse,
                                                             const QString& userHeterodimer) {
    QString ans = heterodimerInfoForOnePrimer(forwardUserPrimerStr(), forward) +
                  heterodimerInfoForOnePrimer(reverseUserPrimerStr(), reverse);
    return ans += returnReportIfAny(concatenatePrimerNames(forwardUserPrimerStr(), reverseUserPrimerStr()),
                                    userHeterodimer);
}

QString PCRPrimerDesignTaskReportUtils::userPrimersUnwantedConnectionsInfo(
                                                              const PCRPrimerDesignForDNAAssemblyTaskSettings& settings,
                                                              const UserPrimerReports& forward,
                                                              const UserPrimerReports& reverse,
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

}  // namespace U2
