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

#ifndef _U2_PCR_PRIMER_DESIGN_TASK_REPORT_UTILS_H_
#define _U2_PCR_PRIMER_DESIGN_TASK_REPORT_UTILS_H_

#include <QString>
#include <QStringList>

#include <U2Core/U2Region.h>

namespace U2 {

class PCRPrimerDesignForDNAAssemblyTask;
struct PCRPrimerDesignForDNAAssemblyTaskSettings;

struct PCRPrimerDesignTaskReportUtils {
    //Stores reports of unwanted connections for one user primer.
    struct UserPrimerReports {
        QString     selfdimer;
        QString     fileSeq;
        QString     fileRevComplSeq;
        QStringList other;
    };

    //No validation of columns number when adding rows.
    class UserPrimersTable {
        const QString name;
        const QStringList headers;
        QList<bool> forwardRow;
        QList<bool> reverseRow;

        QString getHeader() const;
        QString getRow(const QList<bool>& row) const;

    public:
        UserPrimersTable(const QString& name, const QStringList& headers);

        void addForwardRow(const QList<bool>& row);
        void addReverseRow(const QList<bool>& row);
        QString getTable() const;
    };

    static QString errMsg();
    static QString forwardUserPrimerStr();
    static QString reverseUserPrimerStr();
    static QString sequenceStr();
    static QString revComplSeqStr();
    static QString otherSequencesStr();

    static bool areUserPrimers(const PCRPrimerDesignForDNAAssemblyTaskSettings& settings);
    static bool hasHeterodimer(const UserPrimerReports& reports);
    static bool hasUserPrimersUnwantedConnections(const UserPrimerReports& forward, const UserPrimerReports& reverse,
                                                  const QString& userHeterodimer);

    static QString primerHeader(const QString& primerName);
    static QString primerToHtml(QString primer);
    static QString checkPrimerAndGetInfo(const U2Region region, const QString& primerName, const bool isForward,
                                         const QByteArray& sequence, const QString& backbone);
    //All found primers + backbone in html format. User primers are good if both are specified and both have no unwanted
    //connections.
    static QString primersInfo(const PCRPrimerDesignForDNAAssemblyTask& task, const QByteArray& sequence,
                               const bool areUserPrimersGood);

    static QString selfdimerTable(const QString& forwardSelfdimer, const QString& reverseSelfdimer);
    static QString returnReportIfAny(const QString& header, const QString& report);
    static QString userSelfdimersInfo(const QString& forwardSelfdimer, const QString& reverseSelfdimer);
    static QString heterodimersTable(const UserPrimerReports& forward, const UserPrimerReports& reverse,
                                     const QString& userHeterodimer);
    static QString concatenatePrimerNames(const QString& primer, const QString& sequence);
    static QString heterodimerInfoForOnePrimer(const QString& primerName, const UserPrimerReports& reports);
    static QString userHeterodimersInfo(const UserPrimerReports& forward, const UserPrimerReports& reverse,
                                        const QString& userHeterodimer);
    //Unwanted connections of user primers in html format.
    static QString userPrimersUnwantedConnectionsInfo(const PCRPrimerDesignForDNAAssemblyTaskSettings& settings,
                                                      const UserPrimerReports& forward,
                                                      const UserPrimerReports& reverse,
                                                      const QString& userHeterodimer);
};

}

#endif  // _U2_PCR_PRIMER_DESIGN_TASK_REPORT_UTILS_H_
