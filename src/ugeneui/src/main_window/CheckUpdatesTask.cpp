/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "CheckUpdatesTask.h"

#include <QMainWindow>
#include <QMessageBox>
#include <QNetworkReply>
#include <QPushButton>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/NetworkConfiguration.h>
#include <U2Core/Settings.h>
#include <U2Core/SyncHttp.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/MainWindow.h>

#include "update/UgeneUpdater.h"

namespace U2 {

CheckUpdatesTask::CheckUpdatesTask(bool startUp)
    : Task(tr("Check for updates"), TaskFlag_SilentCancelOnShutdown) {
    runOnStartup = startUp;
    setVerboseLogMode(true);
    startError = false;
}

#define SITE_URL QString("ugene.net")
#ifdef Q_OS_WIN
#    define PAGE_NAME QString("/current_version_win.html")
#elif defined(Q_OS_DARWIN)
#    define PAGE_NAME QString("/current_version_mac.html")
#elif defined(Q_OS_LINUX)
#    define PAGE_NAME QString("/current_version_linux.html")
#else
#    define PAGE_NAME QString("/current_version.html")
#endif

void CheckUpdatesTask::run() {
    stateInfo.setDescription(tr("Connecting to updates server"));
    NetworkConfiguration* nc = AppContext::getAppSettings()->getNetworkConfiguration();
    SAFE_POINT(nc != nullptr, "Network configuration is null", );

    bool isProxy = nc->isProxyUsed(QNetworkProxy::HttpProxy);
    bool isException = nc->getExceptionsList().contains(SITE_URL);
    SyncHttp http(stateInfo);
    if (isProxy && !isException) {
        http.setProxy(nc->getProxy(QNetworkProxy::HttpProxy));
    }
    // The following call blocks some UI tasks (Close project), so don't make it run long.
    QString siteVersionText = http.syncGet(QUrl("http://" + SITE_URL + PAGE_NAME), 5000);
    if (siteVersionText.isEmpty()) {
        if (!runOnStartup) {
            stateInfo.setError(tr("Cannot load the current version."));
        } else {
            uiLog.error(tr("Cannot load the current version."));
            startError = true;
        }
    }
    siteVersion = Version::parseVersion(siteVersionText);
    stateInfo.setDescription(QString());

    if (http.error() != QNetworkReply::NoError) {
        if (!runOnStartup) {
            stateInfo.setError(tr("Connection error while checking for updates: %1").arg(http.errorString()));
        } else {
            uiLog.error(tr("Connection error while checking for updates: %1").arg(http.errorString()));
            startError = true;
        }
        return;
    }
}

Task::ReportResult CheckUpdatesTask::report() {
    if (hasError() || startError) {
        return ReportResult_Finished;
    }

    Version currentUgeneVersion = Version::appVersion();

    Answer answer = DoNothing;
    if (runOnStartup) {
        if (siteVersion > currentUgeneVersion && !UgeneUpdater::isUpdateSkipped(siteVersion)) {
            UpdateMessage message(siteVersion.toString());
            answer = message.getAnswer();
        }
    } else {
        VersionMessage message(siteVersion);
        answer = message.getAnswer();
    }

    switch (answer) {
        case Update:
            UgeneUpdater::getInstance()->update();
            break;
        case Skip:
            UgeneUpdater::skipUpdate(siteVersion);
            break;
        case DoNothing:
        default:
            break;
    }
    return ReportResult_Finished;
}

/************************************************************************/
/* UpdateMessage */
/************************************************************************/
UpdateMessage::UpdateMessage(const QString& newVersion) {
    QWidget* parent = AppContext::getMainWindow()->getQMainWindow();
    const QString message = tr("UGENE %1 is available for downloading.\nWould you like to download and install it?").arg(newVersion);
    const QString title = tr("New Updates");

    dialog = new QMessageBox(QMessageBox::Question, title, message, QMessageBox::NoButton, parent);

    updateButton = dialog->addButton(tr("Update now"), QMessageBox::ActionRole);
    postponeButton = dialog->addButton(tr("Ask later"), QMessageBox::ActionRole);
    dialog->addButton(tr("Skip this update"), QMessageBox::ActionRole);
}

CheckUpdatesTask::Answer UpdateMessage::getAnswer() const {
    dialog->exec();
    CHECK(!dialog.isNull(), CheckUpdatesTask::DoNothing);

    if (updateButton == dialog->clickedButton()) {
        return CheckUpdatesTask::Update;
    }
    if (postponeButton == dialog->clickedButton()) {
        return CheckUpdatesTask::DoNothing;
    }
    return CheckUpdatesTask::Skip;
}

/************************************************************************/
/* VersionMessage */
/************************************************************************/
VersionMessage::VersionMessage(const Version& newVersion) {
    QWidget* parent = AppContext::getMainWindow()->getQMainWindow();
    const QString message = getMessageText(Version::appVersion(), newVersion);
    const QString title = tr("Version Information");

    dialog = new QMessageBox(QMessageBox::Information, title, message, QMessageBox::NoButton, parent);
    dialog->addButton(QMessageBox::Ok);
    updateButton = nullptr;
    if (Version::appVersion() < newVersion) {
        updateButton = dialog->addButton(tr("Update Now"), QMessageBox::ActionRole);
    }
}

CheckUpdatesTask::Answer VersionMessage::getAnswer() const {
    dialog->exec();
    CHECK(!dialog.isNull(), CheckUpdatesTask::DoNothing);

    if (dialog->clickedButton() == updateButton) {
        return CheckUpdatesTask::Update;
    }
    return CheckUpdatesTask::DoNothing;
}

QString VersionMessage::getMessageText(const Version& thisVersion, const Version& newVersion) const {
    QString message = QString("<table>"
                              " <tr><td>%1</td><td><b>&nbsp;%2</b></td></tr>"
                              " <tr><td>%3</td><td><b>&nbsp;%4</b></td></tr>"
                              "</table>")
                          .arg(tr("Your version:"), thisVersion.toString(), tr("Latest version:"), newVersion.toString());

    if (thisVersion >= newVersion) {
        message += "<p>" + tr("You have the latest version") + "</p>";
    }
    return message;
}

}  // namespace U2
