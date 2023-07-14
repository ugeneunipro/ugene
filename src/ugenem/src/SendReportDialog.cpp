/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "SendReportDialog.h"

#include <QBuffer>
#include <QDir>
#include <QFile>
#include <QHostInfo>
#include <QHttpPart>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QOperatingSystemVersion>
#include <QProcess>
#include <QThread>
#include <QUrl>
#include <qglobal.h>

#include "Utils.h"
#include "getMemorySize.c"

#define HOST_URL "http://ugene.net"
#ifdef Q_OS_LINUX
#    define DESTINATION_URL_KEEPER_PAGE "/crash_reports_dest_breakpad_lin.html"
#elif defined(Q_OS_DARWIN)
#    define DESTINATION_URL_KEEPER_PAGE "/crash_reports_dest_breakpad_mac.html"
#elif defined(Q_OS_UNIX)
#    define DESTINATION_URL_KEEPER_PAGE "/crash_reports_dest_breakpad_unx.html"
#elif defined(Q_OS_WIN)
#    define DESTINATION_URL_KEEPER_PAGE "/crash_reports_dest_breakpad_win.html"
#else
#    define DESTINATION_URL_KEEPER_PAGE "/crash_reports_dest_breakpad.html"
#endif

ReportSender::ReportSender(bool addGuiTestInfo)
    : report(""), addGuiTestInfo(addGuiTestInfo), failedTest("UNKNOWN TEST") {
}

void ReportSender::parse(const QString& htmlReport, const QString& dumpUrl) {
    report = "Exception with code ";

    QStringList list = htmlReport.split("|");
    if (list.size() == 9) {
        report += list.takeFirst() + " - ";
        report += list.takeFirst() + "\n\n";

        if (!dumpUrl.isEmpty()) {
            report += "Stack dump: " + QDir::toNativeSeparators(dumpUrl) + "\n\n";
        }

        report += "Operation system: ";
        report += getOSVersion() + "\n\n";
        report += "CPU Info: ";
        report += getCPUInfo() + "\n\n";

        report += "Memory Info: ";
        report += QString::number(getTotalPhysicalMemory()) + "Mb\n\n";

        if (addGuiTestInfo) {
            report += "User email: ugene@unipro.ru\n\n";
            report += "Local Host name: ";
            report += QHostInfo::localHostName() + "\n\n";

            report += "Failed test: ";
            report += failedTest + "\n\n";
        }

        report += "UGENE version: ";
#ifdef UGENE_VERSION_SUFFIX
        // Example of usage on linux: DEFINES+='UGENE_VERSION_SUFFIX=\\\"-ppa\\\"'
        report += list.takeFirst() + QString(UGENE_VERSION_SUFFIX) + getArchSuffix() + "\n\n";
#else
        report += list.takeFirst() + getArchSuffix() + "\n\n";
#endif
        report += "UUID: ";
        report += list.takeFirst() + "\n\n";

        report += "ActiveWindow: ";
        report += list.takeFirst() + "\n\n";

        const QString crashHandlerInfo = list.takeFirst();
        if ("None" != crashHandlerInfo) {
            report += "Crash handler additional info:\n";
            report += crashHandlerInfo + "\n";
        }

        report += "Log:\n";
        report += list.takeFirst() + "\n";

        report += "Task tree:\n";
        report += list.takeFirst() + "\n";

#if defined(Q_OS_WIN)
        report += list.takeLast();
#endif
    } else {
        if (addGuiTestInfo) {
            report += "\n\n";
            report += "User email: ugene@unipro.ru\n\n";
            report += "Local Host name: ";
            report += QHostInfo::localHostName() + "\n\n";

            report += "Failed test: ";
            report += failedTest + "\n\n";
        }
        foreach (const QString& str, list) {
            report += str + "\n";
        }
    }

    QFile fp(QDir::tempPath() + "/ugene_crashes/UGENEstacktrace.txt");
    if (fp.open(QIODevice::ReadOnly)) {
        QByteArray stacktrace = fp.readAll();
        report += "Stack trace:\n";
        report += stacktrace.data();
        fp.close();
    }
}

bool ReportSender::send(const QString& additionalInfo, const QString& dumpUrl) {
    report += additionalInfo;

    QNetworkAccessManager* netManager = new QNetworkAccessManager(this);
    QNetworkProxy proxy = QNetworkProxy::applicationProxy();
    netManager->setProxy(proxy);

    connect(netManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(sl_replyFinished(QNetworkReply*)));
    // check destination availability

    QNetworkReply* reply = netManager->get(QNetworkRequest(QString(HOST_URL) + QString(DESTINATION_URL_KEEPER_PAGE)));
    loop.exec();
    QString reportsPath = QString(reply->readAll());
    if (reportsPath.isEmpty()) {
        return false;
    }
    if (reply->error() != QNetworkReply::NoError) {
        return false;
    }

    // send report
    QString data = QUrl::toPercentEncoding(report);
    QNetworkRequest request(reportsPath);

    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart logPart;
    logPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"log\""));
    logPart.setBody(data.toUtf8());
    multiPart->append(logPart);

    if (QFile::exists(dumpUrl)) {
        QHttpPart dumpPart;

        QFile* file = new QFile(dumpUrl);
        file->setParent(multiPart);
        file->open(QIODevice::ReadOnly);
        dumpPart.setBodyDevice(file);

        dumpPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
        dumpPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"dump\"; filename=\"" + file->fileName() + "\""));
        dumpPart.setHeader(QNetworkRequest::UserAgentHeader, QVariant("Unipro UGENE Crash Reporter"));

        multiPart->append(dumpPart);
    }

    reply = netManager->post(request, multiPart);
    multiPart->setParent(reply);

    loop.exec();
    if (reply->error() != QNetworkReply::NoError) {
        return false;
    }

    return true;
}
void ReportSender::sl_replyFinished(QNetworkReply*) {
    loop.exit();
}

SendReportDialog::SendReportDialog(const QString& report, const QString& dumpUrl, QDialog* d)
    : QDialog(d),
      dumpUrl(dumpUrl) {
    setupUi(this);
    sender.parse(report, dumpUrl);
    errorEdit->setText(sender.getReport());

    connect(additionalInfoTextEdit, SIGNAL(textChanged()), SLOT(sl_onMaximumMessageSizeReached()));
    connect(sendButton, SIGNAL(clicked()), SLOT(sl_onOkClicked()));
    connect(cancelButton, SIGNAL(clicked()), SLOT(sl_onCancelClicked()));

    QFile file(getUgeneExecutablePath());
    if (!file.exists()) {
        checkBox->hide();
        checkBox->setChecked(false);
    }
}

void SendReportDialog::sl_onCancelClicked() {
    if (checkBox->isChecked()) {
        openUgene();
    }
    this->reject();
}

void SendReportDialog::sl_onMaximumMessageSizeReached() {
    if (additionalInfoTextEdit->toPlainText().length() > 500) {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("The \"Additional information\" message is too long."));
        msgBox.setInformativeText(tr("You can also send the description of the problem to UGENE team "
                                     "by e-mail <a href=\"mailto:ugene@unipro.ru\">ugene@unipro.ru</a>."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        additionalInfoTextEdit->undo();
    }
}

QString SendReportDialog::getUgeneExecutablePath() const {
    QString name;
    bool isWin = false;
    Q_UNUSED(isWin);
#ifdef Q_OS_UNIX
    name = "ugene";
#endif
#ifdef Q_OS_WIN32
    isWin = true;
    name = "ugeneui.exe";
#endif
#ifdef Q_OS_DARWIN
    name = "ugeneui";
#endif
#ifdef QT_DEBUG
    if (isWin) {
        name = "ugeneuid.exe";
    } else {
        name.append("d");
    }
#endif
    return QCoreApplication::applicationDirPath() + "/" + name;
}

QStringList SendReportDialog::getParameters() const {
    QStringList parameters;
#ifdef Q_OS_UNIX
    parameters << "-ui";
#endif
    if (Utils::hasDatabaseUrl()) {
        parameters << Utils::SESSION_DB_UGENE_ARG + "\"" + Utils::getDatabaseUrl() + "\"";
    }
    return parameters;
}

void SendReportDialog::openUgene() const {
    QString command = getUgeneExecutablePath();
    QStringList parameters = getParameters();
    QProcess::startDetached(command, parameters);
}

void SendReportDialog::sl_onOkClicked() {
    sendButton->setEnabled(false);
    cancelButton->setEnabled(false);
    checkBox->setEnabled(false);
    QString htmlReport = "";
    if (!emailLineEdit->text().isEmpty()) {
        htmlReport += "\nUser email: ";
        htmlReport += emailLineEdit->text() + "\n";
    }

    if (!additionalInfoTextEdit->toPlainText().isEmpty()) {
        htmlReport += "\nAdditional info: \n";
        htmlReport += additionalInfoTextEdit->toPlainText() + "\n";
    }

    if (checkBox->isChecked()) {
        openUgene();
    }

    sender.send(htmlReport, dumpUrl);
    accept();
}

QString ReportSender::getOSVersion() {
#ifdef Q_OS_DARWIN
    QString result = "Mac";
#elif defined(Q_OS_WIN)
    QString result = "Windows";
#elif defined(Q_OS_LINUX)
    QString result = "Linux";
#else
    QString result = "Unknown";
#endif

    QOperatingSystemVersion osVersion = QOperatingSystemVersion::current();
    result += QString(" %1.%2.%3").arg(osVersion.majorVersion()).arg(osVersion.minorVersion()).arg(osVersion.microVersion());
    return result;
}

int ReportSender::getTotalPhysicalMemory() {
    return (int)(getMemorySize() / (1024 * 1024));
}

#if !defined(Q_OS_DARWIN) && defined Q_PROCESSOR_X86
#    define UGENE_HAS_CPU_ID
#endif

#ifdef UGENE_HAS_CPU_ID
static void cpuID(unsigned i, unsigned regs[4]) {
#    ifdef _WIN32
    __cpuid((int*)regs, (int)i);

#    else
    asm volatile("cpuid"
                 : "=a"(regs[0]), "=b"(regs[1]), "=c"(regs[2]), "=d"(regs[3])
                 : "a"(i), "c"(0));
    // ECX is set to zero for CPUID function 4
#    endif
}
#endif

QString ReportSender::getCPUInfo() {
    QString result;
#ifdef UGENE_HAS_CPU_ID
    unsigned regs[4];

    // Get vendor
    char vendor[12];
    cpuID(0, regs);
    ((unsigned*)vendor)[0] = regs[1];  // EBX
    ((unsigned*)vendor)[1] = regs[3];  // EDX
    ((unsigned*)vendor)[2] = regs[2];  // ECX
    QString cpuVendor = QString(vendor);
    result += "\n  Vendor :" + cpuVendor;

    // Get CPU features
    cpuID(1, regs);
    unsigned cpuFeatures = regs[3];  // EDX

    // Logical core count per CPU
    cpuID(1, regs);
    unsigned logical = (regs[1] >> 16) & 0xff;  // EBX[23:16]

    result += "\n  logical cpus: " + QString::number(logical);
    unsigned cores = 0;

    if (cpuVendor.contains("GenuineIntel")) {
        // Get DCP cache info
        cpuID(4, regs);
        cores = ((regs[0] >> 26) & 0x3f) + 1;  // EAX[31:26] + 1

    } else if (cpuVendor.contains("AuthenticAMD")) {
        // Get NC: Number of CPU cores - 1
        cpuID(0x80000008, regs);
        cores = ((unsigned)(regs[2] & 0xff)) + 1;  // ECX[7:0] + 1
    }

    result += "\n  cpu cores: " + QString::number(cores);

    // Detect hyper-threads..
    bool hyperThreads = cpuFeatures & (1 << 28) && cores < logical;

    result += "\n  hyper-threads: " + QString(hyperThreads ? "true" : "false");
#else
    result = "unknown";
#endif
    return result;
}

void ReportSender::setFailedTest(const QString& failedTestStr) {
    failedTest = failedTestStr;
}

QString ReportSender::getArchSuffix() const {
#ifdef Q_PROCESSOR_X86_64
    return " x64";
#elif defined(Q_PROCESSOR_X86_32)
    return " x86";
#elif defined(Q_PROCESSOR_ARM_64)
    return " arm-64";
#elif defined(Q_PROCESSOR_ARM_32)
    return " arm-32";
#else
    return " unknown-arch";
#endif
}
