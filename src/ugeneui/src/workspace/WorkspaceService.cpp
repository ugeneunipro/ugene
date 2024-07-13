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

#include "WorkspaceService.h"

#include <QAction>
#include <QJsonDocument>
#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/L10n.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/MainWindow.h>

#include "CloudStorageDockWidget.h"
#include "KeycloakAuthenticator.h"
#include "WebSocketClientService.h"

namespace U2 {

static QDateTime getTokenExpirationTime(const QString& accessToken) {
    QStringList parts = accessToken.split('.');
    CHECK(parts.size() == 3, {});
    QByteArray payloadString = QByteArray::fromBase64(parts[1].toUtf8());
    QJsonDocument doc = QJsonDocument::fromJson(payloadString);
    QJsonObject payloadJson = doc.object();
    CHECK(payloadJson.contains("exp"), {});
    qint64 exp = payloadJson["exp"].toVariant().toLongLong();
    return QDateTime::fromSecsSinceEpoch(exp);
}

static const int ACCESS_TOKEN_RENEW_BEFORE_EXPIRE_SECONDS = 30;
static const QString WORKSPACE_SETTINGS_FOLDER = "workspace";
static const QString WORKSPACE_SETTINGS_REFRESH_TOKEN = "rt";

WorkspaceService::WorkspaceService()
    : Service(Service_Workspace, "Workspace", "Remove workspace service for UGENE") {
    loginAction = new QAction(QIcon(":ugene/images/login.svg"), tr("Login to Workspace"));
    loginAction->setObjectName("loginToWorkspaceAction");
    connect(loginAction, &QAction::triggered, this, &WorkspaceService::login);

    logoutAction = new QAction(QIcon(":ugene/images/logout.svg"), tr("Logout from Workspace"));
    logoutAction->setObjectName("logoutFromWorkspaceAction");
    connect(logoutAction, &QAction::triggered, this, &WorkspaceService::logout);

    connect(this, &WorkspaceService::si_authenticationEvent, this, &WorkspaceService::updateMainMenuActions);

    refreshToken = AppContext::getSettings()->getValue(WORKSPACE_SETTINGS_FOLDER + "/" + WORKSPACE_SETTINGS_REFRESH_TOKEN).toString();

    webSocketService = new WebSocketClientService(this);

    auto refreshTokenTimer = new QTimer(this);
    connect(refreshTokenTimer, &QTimer::timeout, this, [this] { renewAccessToken(); });
    int accessTokenRefreshTimerIntervalMillis = (ACCESS_TOKEN_RENEW_BEFORE_EXPIRE_SECONDS / 2) * 1000;
    refreshTokenTimer->start(accessTokenRefreshTimerIntervalMillis);
    renewAccessToken();
}

void WorkspaceService::renewAccessToken() {
    CHECK(!refreshToken.isEmpty(), );
    QDateTime refreshTokenExpirationTime = getTokenExpirationTime(refreshToken);
    QDateTime now = QDateTime::currentDateTimeUtc();
    CHECK(refreshTokenExpirationTime.isValid() && refreshTokenExpirationTime > now, );

    if (!accessToken.isEmpty()) {
        QDateTime accessTokenExpirationTime = getTokenExpirationTime(accessToken);
        if (accessTokenExpirationTime.isValid()) {
            QDateTime minRenewTime = accessTokenExpirationTime.addSecs(-ACCESS_TOKEN_RENEW_BEFORE_EXPIRE_SECONDS);
            CHECK(minRenewTime >= now, );  // Do not renew - Access Token will be valid long enough.
        }
    }

    auto authenticator = new KeycloakAuthenticator(authUrl, tokenUrl, clientId);
    connect(authenticator, &KeycloakAuthenticator::si_authenticationGranted, this, [this](const QString& accessToken, const QString& refreshToken) {
        setTokens(accessToken, refreshToken, true);
    });
    authenticator->refreshAccessToken(refreshToken);  // Self destroys upon completion.
}

Task* WorkspaceService::createServiceEnablingTask() {
    return new EnableWorkspaceTask(this);
}

bool WorkspaceService::isLoggedIn() const {
    return !accessToken.isEmpty();
}

void WorkspaceService::enable() {
    updateMainMenuActions();
    AppContext::getMainWindow()->getDockManager()->registerDock(MWDockArea_Left, new CloudStorageDockWidget(), QKeySequence(Qt::ALT | Qt::Key_4));
}

void WorkspaceService::updateMainMenuActions() {
    MainWindow* mw = AppContext::getMainWindow();
    QMenu* fileMenu = mw->getTopLevelMenu(MWMENU_FILE);
    if (!separatorAction) {
        separatorAction = new QAction("", this);
        separatorAction->setSeparator(true);
        auto beforeAction = GUIUtils::findAction(fileMenu->actions(), ACTION_PROJECTSUPPORT__ACCESS_REMOTE_DB);
        fileMenu->insertAction(beforeAction, separatorAction);
    }
    if (isLoggedIn()) {
        fileMenu->insertAction(separatorAction, logoutAction);
        fileMenu->removeAction(loginAction);
    } else {
        fileMenu->insertAction(separatorAction, loginAction);
        fileMenu->removeAction(logoutAction);
    }
}

WebSocketClientService* WorkspaceService::getWebSocketService() const {
    return webSocketService;
}

void WorkspaceService::setTokens(const QString& newAccessToken, const QString& newRefreshToken, bool saveToSettings) {
    accessToken = newAccessToken;
    refreshToken = newRefreshToken;
    updateMainMenuActions();
    if (saveToSettings) {
        AppContext::getSettings()->setValue(WORKSPACE_SETTINGS_FOLDER + "/" + WORKSPACE_SETTINGS_REFRESH_TOKEN, refreshToken);
    }
    si_authenticationEvent(true);
    webSocketService->refreshWebSocketConnection(this->accessToken);
}

void WorkspaceService::login() {
    auto authenticator = new KeycloakAuthenticator(authUrl, tokenUrl, clientId);
    connect(authenticator, &KeycloakAuthenticator::si_authenticationGranted, this, [this](const QString& accessToken, const QString& refreshToken) {
        this->refreshToken = refreshToken;
        setTokens(accessToken, refreshToken, true);
    });
    connect(authenticator, &KeycloakAuthenticator::si_authenticationFailed, this, [this](const QString& error) {
        this->accessToken.clear();
        this->updateMainMenuActions();
        QMessageBox::critical(nullptr, L10N::errorTitle(), tr("Failed to authenticate: %1").arg(error));
    });
    authenticator->startAuthentication();  // Authenticator self-destroys after authentication is finished.
}

void WorkspaceService::logout() {
    accessToken.clear();
    updateMainMenuActions();
    delete webSocketService;
    webSocketService = nullptr;
    emit si_authenticationEvent(false);
}

EnableWorkspaceTask::EnableWorkspaceTask(WorkspaceService* _ws)
    : Task(tr("Enable Workspace"), TaskFlag_NoRun), ws(_ws) {
}

Task::ReportResult EnableWorkspaceTask::report() {
    ws->enable();
    return ReportResult_Finished;
}

}  // namespace U2
