/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "Shtirlitz.h"

#include <QBuffer>
#include <QDate>
#include <QMainWindow>
#include <QMessageBox>
#include <QNetworkReply>
#include <QOperatingSystemVersion>
#include <QProcess>
#include <QSysInfo>
#include <QUrl>
#include <QtCore/QFile>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/Counter.h>
#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/NetworkConfiguration.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Settings.h>
#include <U2Core/SyncHttp.h>
#include <U2Core/UserApplicationsSettings.h>
#include <U2Core/Version.h>

#include <U2Gui/MainWindow.h>

#include "StatisticalReportController.h"

const static char* SETTINGS_NOT_FIRST_LAUNCH = "shtirlitz/not_first_launch";
const static char* SETTINGS_PREVIOUS_REPORT_DATE = "shtirlitz/previous_report_date";
const static char* SETTINGS_COUNTERS = "shtirlitz/counters";
const static char* SETTINGS_UGENE_UID = "shtirlitz/uid";

const static int DAYS_BETWEEN_REPORTS = 7;

// This file stores the actual location of reports-receiver script.
const static char* DESTINATION_URL_KEEPER_SRV = "http://ugene.net";
const static char* DESTINATION_URL_KEEPER_PAGE = "/reports_dest.html";

namespace U2 {

//////////////////////////////////////////////////////////////////////////
// Shtirlitz itself

const QString Shtirlitz::SEPARATOR = "|";

QUuid Shtirlitz::getUniqueUgeneId() {
    static QUuid uniqueUgeneId;
    if (uniqueUgeneId.isNull()) {
        // try to load it from settings
        QVariant uuidQvar = AppContext::getSettings()->getValue(SETTINGS_UGENE_UID);
        if (uuidQvar.isNull()) {
            uniqueUgeneId = QUuid::createUuid();
            AppContext::getSettings()->setValue(SETTINGS_UGENE_UID, uniqueUgeneId.toString());
        } else {
            uniqueUgeneId = QUuid(uuidQvar.toString());
        }
    }
    return uniqueUgeneId;
}

static QString getWhatsNewHtml() {
    QString activeLanguage = L10N::getActiveLanguageCode();
    if (activeLanguage != "ru") {  // We have only Russian & English versions of "version_news_" files.
        activeLanguage = "en";
    }
    QFile htmlFile(":/ugene/html/version_news_" + activeLanguage + ".html");
    if (!htmlFile.open(QFile::ReadOnly | QFile::Text)) {
        return "";
    }
    QTextStream in(&htmlFile);
    in.setCodec("UTF-8");
    return in.readAll();
}

// Report about system is sent on the first launch of UGENE.
// Statistical reports are sent once per DAYS_BETWEEN_REPORTS.
QList<Task*> Shtirlitz::wakeup() {
    QList<Task*> result;
    Settings* s = AppContext::getSettings();
    bool allVersionsFirstLaunch = true;
    bool minorVersionFirstLaunch = true;
    bool bSentSystemReport = false;
    getFirstLaunchInfo(allVersionsFirstLaunch, minorVersionFirstLaunch);

    QString allVersionsKey = SETTINGS_NOT_FIRST_LAUNCH;
    QString minorVersionFirstLaunchKey = s->toMinorVersionKey(SETTINGS_NOT_FIRST_LAUNCH);
    if (allVersionsFirstLaunch) {
        s->setValue(allVersionsKey, QVariant(true));
    }
    if (minorVersionFirstLaunch) {
        s->setValue(minorVersionFirstLaunchKey, QVariant(true));
    }
    getUniqueUgeneId();

    // Do nothing if Shtirlitz was disabled
    if (QProcess::systemEnvironment().contains(ENV_UGENE_DEV)) {
        return result;
    }

    // Check if this version of UGENE is launched for the first time
    // and user did not enabled stats before -> ask to enable
    // Do not ask to enable it twice for different versions!
    UserAppsSettings* userAppSettings = AppContext::getAppSettings()->getUserAppsSettings();
    if (minorVersionFirstLaunch) {
        showWhatsNewDialog();
        if (!userAppSettings->isStatisticsCollectionEnabled()) {
            return result;
        }
        coreLog.details(ShtirlitzTask::tr("Shtirlitz is sending the first-time report"));
        result << sendSystemReport();
        bSentSystemReport = true;
        // Leave a mark that the first-time report was sent
    }

    // Check if previous report was sent more than a week ago
    if (!allVersionsFirstLaunch && userAppSettings->isStatisticsCollectionEnabled()) {
        QVariant prevDateQvar = AppContext::getSettings()->getValue(SETTINGS_PREVIOUS_REPORT_DATE);
        QDate prevDate = prevDateQvar.toDate();
        int daysPassed = prevDate.isValid() ? prevDate.daysTo(QDate::currentDate()) : 0;

        if (!prevDate.isValid() || daysPassed > DAYS_BETWEEN_REPORTS) {
            coreLog.details(ShtirlitzTask::tr("%1 days passed passed since previous Shtirlitz's report. Shtirlitz is sending the new one."));
            if (!bSentSystemReport) {
                result << sendSystemReport();
            }
            result << sendCountersReport();
            // and save the new date
            s->setValue(SETTINGS_PREVIOUS_REPORT_DATE, QDate::currentDate());
        }
    }
    return result;
}

Task* Shtirlitz::sendCustomReport(const QString& customReport) {
    return new ShtirlitzTask(customReport);
}

void Shtirlitz::saveGatheredInfo() {
    if (!enabled()) {
        return;
    }
    // 1. Save counters
    Settings* s = AppContext::getSettings();
    QList<GCounter*> appCounters = GCounter::getAllCounters();
    for (const GCounter* counter : qAsConst(appCounters)) {
        if (counter->isReportable) {
            QString ctrKey = counter->name + SEPARATOR + counter->suffix;
            double ctrVal = counter->getScaledValue();

            QString curKey = QString(SETTINGS_COUNTERS) + "/" + ctrKey;
            QVariant lastValQvar = s->getValue(curKey, QVariant());
            double lastVal = lastValQvar.canConvert<double>() ? lastValQvar.toDouble() : 0.;
            double newVal = (lastVal + ctrVal > lastVal) ? lastVal + ctrVal : lastVal;  // overflow detection
            s->setValue(curKey, newVal);
        }
    }
}

bool Shtirlitz::enabled() {
    // Check environment variable for developers
    if (QProcess::systemEnvironment().contains(ENV_UGENE_DEV)) {
        return false;
    }
    return AppContext::getAppSettings()->getUserAppsSettings()->isStatisticsCollectionEnabled();
}

Task* Shtirlitz::sendCountersReport() {
    QString countersReport = formCountersReport();
    return sendCustomReport(countersReport);
}

Task* Shtirlitz::sendSystemReport() {
    QString systemReport = formSystemReport();
    return sendCustomReport(systemReport);
}

// Tries to load saved counters from settings.
// Adds loaded counters to report, sets saved values to zero
QString Shtirlitz::formCountersReport() {
    Settings* s = AppContext::getSettings();
    QString countersReport;
    countersReport += "COUNTERS REPORT:\n";
    countersReport += "ID: " + getUniqueUgeneId().toString() + "\n";
    QStringList savedCounters = s->getAllKeys(SETTINGS_COUNTERS);
    if (savedCounters.empty()) {
        countersReport += "NO INFO\n";
    } else {
        foreach (QString savedCtrName, savedCounters) {
            QVariant savedCtrQvar = s->getValue(QString(SETTINGS_COUNTERS) + "/" + savedCtrName);
            double savedCtrVal = savedCtrQvar.canConvert<double>() ? savedCtrQvar.toDouble() : 0.;
            countersReport += savedCtrName + " : " + QString::number(savedCtrVal) + "\n";
            s->setValue(QString(SETTINGS_COUNTERS) + "/" + savedCtrName, 0.);
        }
    }
    countersReport += "ENDOF COUNTERS REPORT.\n";
    return countersReport;
}

QString Shtirlitz::formSystemReport() {
    QString dateAndTime = QDateTime::currentDateTime().toString(Qt::ISODate);
    QString ugeneVer = Version::appVersion().toString();
    QString qtVersion = qVersion();
    QString osName;
    QString osVersion;
    QString kernelType;
    QString kernelVersion;
    QString productVersion;
    QString productType;
    QString prettyProductName;
    QString cpuArchitecture;
    getSysInfo(osName, osVersion, kernelType, kernelVersion, productVersion, productType, prettyProductName, cpuArchitecture);

    QString systemReport;
    systemReport += "SYSTEM REPORT:\n";
    systemReport += "ID: " + getUniqueUgeneId().toString() + "\n";
    systemReport += "Date and time: " + dateAndTime + "\n";
    systemReport += "Qt Version: " + qtVersion + "\n";
    systemReport += "UGENE version: " + ugeneVer + "\n";
    systemReport += "Word size: " + QString::number(QSysInfo::WordSize) + "\n";
    systemReport += "OS name: " + osName + "\n";
    systemReport += "OS version: " + osVersion + "\n";
    systemReport += "kernelType: " + kernelType + "\n";
    systemReport += "kernelVersion: " + kernelVersion + "\n";
    systemReport += "productVersion: " + productVersion + "\n";
    systemReport += "productType: " + productType + "\n";
    systemReport += "prettyProductName: " + prettyProductName + "\n";
    systemReport += "cpuArchitecture: " + cpuArchitecture + "\n";
    systemReport += "ENDOF SYSTEM REPORT.\n";

    return systemReport;
}

void Shtirlitz::getSysInfo(QString& name,
                           QString& version,
                           QString& kernelType,
                           QString& kernelVersion,
                           QString& productVersion,
                           QString& productType,
                           QString& prettyProductName,
                           QString& cpuArchitecture) {
    QOperatingSystemVersion osVersion = QOperatingSystemVersion::current();
    version = QString("%1.%2.%3").arg(osVersion.majorVersion()).arg(osVersion.minorVersion()).arg(osVersion.microVersion());
#if defined(Q_OS_WIN)
    name = "Windows";
#elif defined(Q_OS_DARWIN)
    name = "Mac";
#elif defined(Q_OS_LINUX)
    name = "Linux";
    version = "unknown";  // no version is available :(
#elif defined(Q_OS_FREEBSD)
    name = "FreeBSD";
#elif defined(Q_OS_UNIX)
    name = "Unix";
#else
    name = "Other";
#endif
    kernelType = QSysInfo::kernelType();
    kernelVersion = QSysInfo::kernelVersion();
    productVersion = QSysInfo::productVersion();
    productType = QSysInfo::productType();
    prettyProductName = QSysInfo::prettyProductName();
    cpuArchitecture = QSysInfo::currentCpuArchitecture();
}

void Shtirlitz::getFirstLaunchInfo(bool& allVersions, bool& minorVersions) {
    Settings* settings = AppContext::getSettings();

    QString allVersionsKey = SETTINGS_NOT_FIRST_LAUNCH;
    QString minorVersionsKey = settings->toMinorVersionKey(SETTINGS_NOT_FIRST_LAUNCH);

    QVariant launchedAllQvar = settings->getValue(allVersionsKey);
    QVariant launchedThisMajorQvar = settings->getValue(minorVersionsKey);

    allVersions = (!launchedAllQvar.isValid()) || launchedAllQvar.isNull();
    minorVersions = (!launchedThisMajorQvar.isValid()) || launchedThisMajorQvar.isNull();
}

QString Shtirlitz::tr(const char* str) {
    return ShtirlitzTask::tr(str);
}

void Shtirlitz::showWhatsNewDialog() {
    // Show a non-blocking what's new dialog.
    auto startupWhatsNewDialog = new StatisticalReportController(getWhatsNewHtml());
    startupWhatsNewDialog->setModal(false);
    QSharedPointer<StatisticalReportController> dialog(startupWhatsNewDialog);
    QObject::connect(dialog.data(), &QDialog::accepted, [dialog] {
        auto userSettings = AppContext::getAppSettings()->getUserAppsSettings();
        userSettings->setEnableCollectingStatistics(dialog->isInfoSharingAccepted());
        userSettings->setExperimentalFeaturesModeEnabled(dialog->isExperimentalFeaturesEnabled());
    });
    dialog->open();
}

//////////////////////////////////////////////////////////////////////////
// Shtirlitz Task

ShtirlitzTask::ShtirlitzTask(const QString& _report)
    : Task("Shtirlitz task", TaskFlag_None), report(_report) {
}

void ShtirlitzTask::run() {
    stateInfo.setDescription(tr("Connecting to remote server"));

    // Creating SyncHttp object and enabling proxy if needed.
    SyncHttp http(stateInfo);
    NetworkConfiguration* nc = AppContext::getAppSettings()->getNetworkConfiguration();
    bool isProxy = nc->isProxyUsed(QNetworkProxy::HttpProxy);
    bool isException = nc->getExceptionsList().contains(QUrl(DESTINATION_URL_KEEPER_SRV).host());

    if (isProxy && !isException) {
        http.setProxy(nc->getProxy(QNetworkProxy::HttpProxy));
    }

    QByteArray preparedReport("data=");
    preparedReport += QUrl::toPercentEncoding(report);

    // Get actual location of the reports receiver
    // FIXME: error handling
    QString reportsPath = http.syncGet(QUrl(QString(DESTINATION_URL_KEEPER_SRV) +
                                            QString(DESTINATION_URL_KEEPER_PAGE)),
                                       10000);
    if (reportsPath.isEmpty()) {
        stateInfo.setError(tr("Cannot resolve destination path for statistical reports"));
        return;
    }
    if (http.error() != QNetworkReply::NoError) {
        stateInfo.setError(tr("Network error while resolving destination URL: ") + http.errorString());
        return;
    }

    // Checking proxy again, for the new url
    SyncHttp http2(stateInfo);
    isException = nc->getExceptionsList().contains(QUrl(reportsPath).host());
    if (isProxy && !isException) {
        http2.setProxy(nc->getProxy(QNetworkProxy::HttpProxy));
    }
    QString fullPath = reportsPath + "?" + preparedReport;
    http2.syncGet(fullPath, 20000);  // TODO: consider using POST method?
    if (http.error() != QNetworkReply::NoError) {
        stateInfo.setError(tr("Network error while sending report: ") + http2.errorString());
        return;
    }
}

ShtirlitzStartupTask::ShtirlitzStartupTask()
    : Task(tr("Shtirlitz Startup Task"), TaskFlag_NoRun) {
}

void ShtirlitzStartupTask::prepare() {
    foreach (Task* t, Shtirlitz::wakeup()) {
        addSubTask(t);
    }
}

}  // namespace U2
