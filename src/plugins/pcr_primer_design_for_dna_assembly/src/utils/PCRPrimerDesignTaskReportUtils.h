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

#include <QStringList>

namespace U2 {

class PCRPrimerDesignForDNAAssemblyTask;
struct PCRPrimerDesignForDNAAssemblyTaskSettings;

struct PCRPrimerDesignTaskReportUtils {
    // Saves all unwanted connection reports of both user primers.
    struct UserPrimersReports;

    // Returns PCRPrimerDesignForDNAAssemblyTask report. fileSequence must not be empty.
    static QString generateReport(const PCRPrimerDesignForDNAAssemblyTask &task, const QByteArray &fileSequence,
                                  const QByteArray &revComplSeq, const UserPrimersReports &reports);

private:
    /**
     * To build a user primer unwanted connection table. Includes a header row, a forward user primer row, and a reverse
     * user primer row. Number of columns varies.
     * No validation of columns number when adding rows.
     */
    class UserPrimersTable;

    /**
     * Checks for unwanted user primer connections.
     * If there are no connections, an empty string is returned.
     * If so, information about them is returned:
     *   selfdimer table and list of selfdimer reports,
     *   heterodimer table and list of heterodimer reports.
     */
    static QString userPrimersUnwantedConnectionsInfo(const PCRPrimerDesignForDNAAssemblyTaskSettings &settings,
                                                      const UserPrimersReports &reports);
};

struct PCRPrimerDesignTaskReportUtils::UserPrimersReports {
    /**
     * Stores reports of unwanted connections of a single user primer. Includes selfdimer report and heterodimer
     * reports with sequence from file, reverse complementary sequence from file, and other sequences in PCR reaction.
     */
    struct PrimerReports {
        QString selfdimer,
            fileSeq,
            fileRevComplSeq;
        QStringList other;
    };

    PrimerReports forward,  // Reports of unwanted connections of forward user primer.
        reverse;  // Reports of unwanted connections of reverse user primer.
    QString heterodimer;  // User primers heterodimer report.

    // Are there selfdimers?
    bool hasSelfdimers() const;
    // Are there heterodimers?
    bool hasHeterodimers() const;
    // Are there unwanted user primer connections?
    bool hasUnwantedConnections() const;
};

class PCRPrimerDesignTaskReportUtils::UserPrimersTable {
public:
    // Parameters must not be empty.
    UserPrimersTable(const QString &name, const QStringList &headers);

    // Adds cell values to forward user primer row. List size must be equal to the number of columns (headers size).
    void addForwardRow(const QList<bool> &row);
    // Adds cell values to reverse user primer row. List size must be equal to the number of columns (headers size).
    void addReverseRow(const QList<bool> &row);
    // Builds table and returns non-empty html string.
    QString getTable() const;

private:
    const QString name;  // Name of table.
    const QStringList headers;  // Table column headers.
    QList<bool> forwardRow,  // Forward user primer row cell values.
        reverseRow;  // Reverse user primer row cell values.

    // Converts bool to string, wraps it in <td> tag, concatenates and returns cells of row.
    QString getRow(const QList<bool> &row) const;
};

}  // namespace U2

#endif  // _U2_PCR_PRIMER_DESIGN_TASK_REPORT_UTILS_H_
