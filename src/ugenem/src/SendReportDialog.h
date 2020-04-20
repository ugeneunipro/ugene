/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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

#ifndef _SEND_REPORT_DIALOG_H_
#define _SEND_REPORT_DIALOG_H_


#include <ui_SendReportDialog.h>

class QEventLoop;
class QNetworkReply;

class ReportSender :public QObject {
    Q_OBJECT
public:
    ReportSender(bool addGuiTestInfo = false);
    void parse(const QString &str, const QString &dumpUrl);
    bool send(const QString &additionalInfo, const QString &dumpUrl);
    QString getOSVersion();
    QString getReport() const { return report; }
    int getTotalPhysicalMemory();
    QString getCPUInfo();
    QString getUgeneBitCount() const;
    void setFailedTest(const QString &failedTestStr);

private slots:
    void sl_replyFinished(QNetworkReply*);

private:
    QString report;
    QEventLoop loop;
    bool addGuiTestInfo;
    QString failedTest;
};

class SendReportDialog :public QDialog, public Ui_Dialog {
    Q_OBJECT
public:
    SendReportDialog(const QString &report, const QString &dumpUrl, QDialog *d = NULL);

private:
    void openUgene() const;
    QString getUgeneExecutablePath() const;
    QStringList getParameters() const;

private slots:
    void sl_onOkClicked();
    void sl_onMaximumMessageSizeReached();
    void sl_onCancelClicked();

private:
    ReportSender sender;
    const QString dumpUrl;
};

#endif
