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
#include <QNetworkReply>
#include <QScreen>

#include <U2Core/AppContext.h>
#include <U2Core/L10n.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/MainWindow.h>

#include "CloudStorageDockWidget.h"
#include "CloudStorageService.h"
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
    stage = qgetenv("UGENE_WORKSPACE_STAGE");
    if (stage != "dev" && stage != "local") {
        stage = "prod";
    }
    QString apiDomainAndPort = stage == "dev"     ? "workspace-dev.ugene.net"
                               : stage == "local" ? "localhost:4201"
                                                  : "workspace.ugene.net";
    apiUrl = (stage == "local" ? "http://" : "https://") + apiDomainAndPort;
    webSocketUrl = stage == "local" ? "ws://localhost:4201" : "wss://" + apiDomainAndPort;
    clientId = "workspace-client-" + stage;
    authUrl = "https://auth.ugene.net/realms/ugene-" + stage + "/protocol/openid-connect/auth";
    tokenUrl = "https://auth.ugene.net/realms/ugene-" + stage + "/protocol/openid-connect/token";

    loginAction = new QAction(QIcon(":ugene/images/login.svg"), tr("Login to Workspace"));
    loginAction->setObjectName("loginToWorkspaceAction");
    connect(loginAction, &QAction::triggered, this, &WorkspaceService::login);

    logoutAction = new QAction(QIcon(":ugene/images/logout.svg"), tr("Logout from Workspace"));
    logoutAction->setObjectName("logoutFromWorkspaceAction");
    connect(logoutAction, &QAction::triggered, this, &WorkspaceService::logout);

    connect(this, &WorkspaceService::si_authenticationEvent, this, &WorkspaceService::updateMainMenuActions);

    refreshToken = AppContext::getSettings()->getValue(WORKSPACE_SETTINGS_FOLDER + "/" + WORKSPACE_SETTINGS_REFRESH_TOKEN).toString();

    auto refreshTokenTimer = new QTimer(this);
    connect(refreshTokenTimer, &QTimer::timeout, this, [this] { renewAccessTokenIfCloseToExpire(); });
    int accessTokenRefreshTimerIntervalMillis = (ACCESS_TOKEN_RENEW_BEFORE_EXPIRE_SECONDS / 2) * 1000;

    refreshTokenTimer->start(accessTokenRefreshTimerIntervalMillis);
    renewAccessTokenIfCloseToExpire();
}

void WorkspaceService::renewAccessTokenIfCloseToExpire() {
    CHECK(!refreshToken.isEmpty(), );
    qDebug() << "WorkspaceService: Renewing access token";
    QDateTime refreshTokenExpirationTime = getTokenExpirationTime(refreshToken);
    QDateTime now = QDateTime::currentDateTimeUtc();
    CHECK(refreshTokenExpirationTime.isValid() && refreshTokenExpirationTime > now, );

    if (!accessToken.isEmpty()) {
        QDateTime accessTokenExpirationTime = getTokenExpirationTime(accessToken);
        if (accessTokenExpirationTime.isValid()) {
            QDateTime minRenewTime = accessTokenExpirationTime.addSecs(-ACCESS_TOKEN_RENEW_BEFORE_EXPIRE_SECONDS);
            qDebug() << "WorkspaceService: accessTokenExpirationTime is "
                     << accessTokenExpirationTime.toLocalTime().toString()
                     << ", minRenewTime: " << minRenewTime.toLocalTime().toString()
                     << ", now: " + now.toLocalTime().toString();
            if (minRenewTime > now) {
                qDebug() << "WorkspaceService: Skip access token renew: too early";
                return;  // Do not renew - Access Token will be valid long enough.
            }
        }
    }

    setTokens("", refreshToken, true);  // Forget expired access token.
    auto authenticator = new KeycloakAuthenticator(authUrl, tokenUrl, clientId);
    connect(authenticator, &KeycloakAuthenticator::si_authenticationGranted, this, [this](const QString& accessToken, const QString& refreshToken) {
        setTokens(accessToken, refreshToken, true);
    });
    connect(authenticator, &KeycloakAuthenticator::si_authenticationFailed, this, [this](const QString&, bool isRetriable) {
        if (!isRetriable) {
            qDebug() << "WorkspaceService: Got unretriable error. Stopping auto token refresh";
            setTokens("", "", true);  // Forget refresh token.
        }
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
    cloudStorageService = new CloudStorageService(this);
    auto dockWidget = new CloudStorageDockWidget(this);
    AppContext::getMainWindow()->getDockManager()->registerDock(MWDockArea_Left, dockWidget, QKeySequence(Qt::ALT | Qt::Key_4));
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
    qDebug() << "WorkspaceService:setTokens is called";
    setTokenFields(newAccessToken, newRefreshToken, saveToSettings);
    CHECK(!newAccessToken.isEmpty() && !newRefreshToken.isEmpty(), );
    if (webSocketService == nullptr) {
        webSocketService = new WebSocketClientService(webSocketUrl, this);
    }
    updateMainMenuActions();
    emit si_authenticationEvent(true);
    webSocketService->setAccessToken(this->accessToken);
}

void WorkspaceService::setTokenFields(const QString& newAccessToken, const QString& newRefreshToken, bool saveToSettings) {
    accessToken = newAccessToken;
    refreshToken = newRefreshToken;
    if (saveToSettings) {
        AppContext::getSettings()->setValue(WORKSPACE_SETTINGS_FOLDER + "/" + WORKSPACE_SETTINGS_REFRESH_TOKEN, refreshToken);
    }
}

void WorkspaceService::login() {
    setTokens("", "", true);  // Reset tokens on start.
    auto authenticator = new KeycloakAuthenticator(authUrl, tokenUrl, clientId);
    connect(authenticator, &KeycloakAuthenticator::si_authenticationGranted, this, [this](const QString& accessToken, const QString& refreshToken) {
        setTokens(accessToken, refreshToken, true);
    });
    connect(authenticator, &KeycloakAuthenticator::si_authenticationFailed, this, [this](const QString& error) {
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
    setTokenFields("", "", true);
    emit si_authenticationEvent(false);
}

CloudStorageService* WorkspaceService::getCloudStorageService() const {
    return cloudStorageService;
}

void WorkspaceService::executeApiRequest(const QString& apiPath,
                                         const QJsonObject& payload,
                                         QObject* context,
                                         std::function<void(const QJsonObject&)>* callback) {
    qDebug() << "WorkspaceService::sendApiRequest: " << apiPath;
    SAFE_POINT(callback == nullptr || context != nullptr, "A callback requires non-null life-range context", );
    SAFE_POINT(apiPath.startsWith("/"), "API path must start with /", );

    QNetworkRequest request(apiUrl + "/api" + apiPath);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    if (!accessToken.isEmpty()) {
        request.setRawHeader("Authorization", "Bearer " + accessToken.toUtf8());
    }

    // Disable cache usage.
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
    request.setAttribute(QNetworkRequest::CacheSaveControlAttribute, false);

    // Ensure connection is closed after each request
    request.setRawHeader("Connection", "close");

    auto networkManager = new QNetworkAccessManager();
    networkManager->setCache(nullptr);

    QNetworkReply* reply = networkManager->post(request, QJsonDocument(payload).toJson());
    QPointer<QObject> contextLifeRangeTracker(context);

    connect(reply, &QNetworkReply::errorOccurred, this, [](QNetworkReply::NetworkError code) {
        qDebug() << "WorkspaceService::sendApiRequest error occured: " + QString::number(code);
    });

    connect(reply, &QNetworkReply::readyRead, this, [] {
        qDebug() << "WorkspaceService::sendApiRequest: readyRead";
    });

    connect(networkManager, &QNetworkAccessManager::finished, this, [this] {
        qDebug() << "WorkspaceService::sendApiRequest: NM FINISHED!!!";
    });

    connect(reply, &QNetworkReply::finished, this, [reply, networkManager, callback, contextLifeRangeTracker] {
        qDebug() << "WorkspaceService::sendApiRequest: REPLY FINISHED!!!";
        QJsonObject jsonResponse;
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            qDebug() << "WorkspaceService::sendApiRequest: Got response: " << responseData;
            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            if (doc.isObject()) {
                jsonResponse = doc.object();
            }
        } else {
            jsonResponse["isError"] = true;
            jsonResponse["errorMessage"] = reply->errorString();
            jsonResponse["errorCode"] = reply->error();
            qDebug() << "WorkspaceService::sendApiRequest: Got error response: " << reply->errorString();
        }
        if (callback && !contextLifeRangeTracker.isNull()) {
            (*callback)(jsonResponse);
        }
        reply->deleteLater();
        networkManager->deleteLater();
    });
}

void WorkspaceService::downloadFile(const QList<QString>& cloudPath, const QString& localFilePath) {
    qDebug() << "WorkspaceService::downloadFile: " << cloudPath << " to " << localFilePath;
    SAFE_POINT(cloudPath.length() > 0, "Cloud path is empty", );
    SAFE_POINT(localFilePath.length() > 0, "Local file path is empty", );

    auto networkManager = new QNetworkAccessManager();
    networkManager->setCache(nullptr);

    QUrl downloadUrl(apiUrl + "/api/storage/download/" + QUrl::toPercentEncoding(cloudPath.join("/")));

    QNetworkRequest request(downloadUrl);
    if (!accessToken.isEmpty()) {
        request.setRawHeader("Authorization", "Bearer " + accessToken.toUtf8());
    }

    // Disable cache usage.
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
    request.setAttribute(QNetworkRequest::CacheSaveControlAttribute, false);

    QNetworkReply* reply = networkManager->get(request);

    connect(reply, &QNetworkReply::downloadProgress, [](qint64 bytesReceived, qint64 bytesTotal) {
        qDebug() << "WorkspaceService::downloadFile: progress:" << bytesReceived << "/" << bytesTotal;
    });

    connect(reply, &QNetworkReply::finished, [reply, localFilePath, networkManager]() {
        if (reply->error()) {
            qDebug() << "WorkspaceService::downloadFile ERROR:" << reply->errorString();
        } else {
            QByteArray data = reply->readAll();  // TODO: download using streaming. Avoid memory overflow.
            QFile file(localFilePath);
            if (file.open(QIODevice::WriteOnly)) {
                file.write(data);
                file.close();
                qDebug() << "WorkspaceService::downloadFile: Download finished and saved to" << localFilePath;
            } else {
                qDebug() << "WorkspaceService::downloadFile: Could not open file for writing.";
            }
        }
        reply->deleteLater();
        networkManager->deleteLater();
    });
}

EnableWorkspaceTask::EnableWorkspaceTask(WorkspaceService* _ws)
    : Task(tr("Enable Workspace"), TaskFlag_NoRun), ws(_ws) {
}

Task::ReportResult EnableWorkspaceTask::report() {
    ws->enable();
    return ReportResult_Finished;
}

}  // namespace U2
