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
#include <QDesktopServices>
#include <QHttpMultiPart>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMessageBox>
#include <QNetworkReply>

#include <U2Core/AppContext.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/MainWindow.h>

#include "CloudStorageDockWidget.h"
#include "CloudStorageService.h"
#include "KeycloakAuthenticator.h"
#include "WebSocketClientService.h"

namespace U2 {

static const QString TEST_REFRESH_TOKEN = "value-is-not-used";

/** Returns a non-empty access token in case if UGENE is run in test mode. */
static QByteArray getTestModeAccessToken() {
    return qgetenv("UGENE_TEST_WORKSPACE_USER_TOKEN");
}

static bool isTestMode() {
    return !getTestModeAccessToken().isEmpty();
}

static QDateTime getTokenExpirationTime(const QString& accessToken) {
    CHECK(!isTestMode(), QDateTime(QDate(2100, 1, 1)))
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
    QString webDomainAndPort = stage == "dev"     ? "workspace-dev.ugene.net"
                               : stage == "local" ? "localhost:4200"
                                                  : "workspace.ugene.net";
    QString apiDomainAndPort = stage == "dev"     ? "workspace-dev.ugene.net"
                               : stage == "local" ? "localhost:4201"
                                                  : "workspace.ugene.net";
    QString workspaceHttpProtocolPrefix = (stage == "local" ? "http://" : "https://");
    webWorkspaceUrl = workspaceHttpProtocolPrefix + webDomainAndPort;
    apiUrl = workspaceHttpProtocolPrefix + apiDomainAndPort;
    webSocketUrl = stage == "local" ? "ws://localhost:4201" : "wss://" + apiDomainAndPort;
    clientId = "workspace-client-" + stage;
    authUrl = "https://auth.ugene.net/realms/ugene-" + stage + "/protocol/openid-connect/auth";
    logoutUrl = workspaceHttpProtocolPrefix + webDomainAndPort + "/logout";
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
    coreLog.trace("WorkspaceService: Renewing access token");
    QDateTime refreshTokenExpirationTime = getTokenExpirationTime(refreshToken);
    QDateTime now = QDateTime::currentDateTimeUtc();
    CHECK(refreshTokenExpirationTime.isValid() && refreshTokenExpirationTime > now, );

    if (!accessToken.isEmpty()) {
        QDateTime accessTokenExpirationTime = getTokenExpirationTime(accessToken);
        if (accessTokenExpirationTime.isValid()) {
            QDateTime minRenewTime = accessTokenExpirationTime.addSecs(-ACCESS_TOKEN_RENEW_BEFORE_EXPIRE_SECONDS);
            coreLog.trace("WorkspaceService: accessTokenExpirationTime is " +
                          accessTokenExpirationTime.toLocalTime().toString() +
                          ", minRenewTime: " + minRenewTime.toLocalTime().toString() +
                          ", now: " + now.toLocalTime().toString());
            if (minRenewTime > now) {
                coreLog.trace("WorkspaceService: Skip access token renew: too early");
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
            coreLog.trace("WorkspaceService: Got unretriable error. Stopping auto token refresh");
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
    coreLog.trace("WorkspaceService:setTokens is called");
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
    if (isTestMode()) {
        setTokens(getTestModeAccessToken(), TEST_REFRESH_TOKEN, false);
        return;
    }
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
    bool isLogoutFromBrowser = QMessageBox::question(nullptr, tr("Logout"), tr("Do you also want to close the browser session?")) == QMessageBox::Yes;
    if (isLogoutFromBrowser) {
        QDesktopServices::openUrl(logoutUrl);
    }
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

static void fillErrorStateInJson(const QString& errorMessage, QJsonObject& jsonResponse) {
    jsonResponse["isError"] = true;
    jsonResponse["errorMessage"] = errorMessage;
}

static void fillErrorStateInJson(QNetworkReply* reply, QJsonObject& jsonResponse) {
    SAFE_POINT(reply->error() != QNetworkReply::NoError, "QNetworkReply has no error", );

    QString serverError = reply->errorString();
    QString serverReplyPrefixToken = "server replied: ";
    int index = serverError.indexOf(serverReplyPrefixToken);
    if (index > 0) {
        serverError = serverError.mid(index + serverReplyPrefixToken.length()).trimmed();
        if (serverError == "Request Entity Too Large") {  // Frontend error from Nginx.
            serverError = "Error uploading file: the file is too large";  // TODO: check this before starting the upload process? Deal with individual quotas correctly.
        }
    }

    QByteArray responseData = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    if (!jsonDoc.isNull()) {
        QJsonObject jsonObj = jsonDoc.object();
        QString errorMessage = jsonObj["message"].toString();
        if (!errorMessage.isEmpty()) {
            serverError = errorMessage;
        }
    }

    fillErrorStateInJson(serverError, jsonResponse);
    jsonResponse["errorCode"] = reply->error();
}

QString WorkspaceService::getErrorMessageFromResponse(const QJsonObject& response) {
    if (response["isError"].toBool()) {
        return response["errorMessage"].toString();
    }
    return {};
}

void WorkspaceService::executeApiRequest(const QString& apiPath,
                                         const QJsonObject& payload,
                                         QObject* context,
                                         std::function<void(const QJsonObject&)> callback) {
    coreLog.trace("WorkspaceService::sendApiRequest: " + apiPath);
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

    connect(reply, &QNetworkReply::finished, this, [reply, networkManager, callback, contextLifeRangeTracker] {
        ioLog.trace("WorkspaceService::sendApiRequest: reply finished");
        QJsonObject jsonResponse;
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            ioLog.trace("WorkspaceService::sendApiRequest: Got response: " + responseData);
            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            if (doc.isObject()) {
                jsonResponse = doc.object();
            }
        } else {
            ioLog.trace("WorkspaceService::executeApiRequest: Got error response: " + reply->errorString());
            fillErrorStateInJson(reply, jsonResponse);
        }
        if (callback && !contextLifeRangeTracker.isNull()) {
            callback(jsonResponse);
        }
        reply->deleteLater();
        networkManager->deleteLater();
    });
}

void WorkspaceService::executeDownloadFileRequest(const QList<QString>& cloudPath,
                                                  const QString& localFilePath,
                                                  QObject* context,
                                                  std::function<void(const QJsonObject&)> callback) {
    coreLog.trace("WorkspaceService::executeDownloadFileRequest: " + cloudPath.join("/") + " to " + localFilePath);
    SAFE_POINT(!cloudPath.isEmpty(), "Cloud path is empty", );
    SAFE_POINT(!localFilePath.isEmpty(), "Local file path is empty", );
    SAFE_POINT(callback == nullptr || context != nullptr, "A callback requires non-null life-range context", );

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
    QPointer<QObject> contextLifeRangeTracker(context);

    connect(reply, &QNetworkReply::downloadProgress, [](qint64 bytesReceived, qint64 bytesTotal) {
        ioLog.trace("WorkspaceService::downloadFile: progress:" + QString::number(bytesReceived) + "/" + QString::number(bytesTotal));
    });

    connect(reply, &QNetworkReply::finished, [reply, localFilePath, networkManager, callback, contextLifeRangeTracker] {
        QJsonObject jsonResponse;
        if (reply->error() == QNetworkReply::NoError) {
            QString downloadDirPath = QFileInfo(localFilePath).absolutePath();
            if (!FileAndDirectoryUtils::createWritableDirIfNotExists(downloadDirPath)) {
                fillErrorStateInJson(tr("Failed to create download directory: %1").arg(downloadDirPath), jsonResponse);
            } else {
                QByteArray data = reply->readAll();
                QFile file(localFilePath);

                if (file.open(QIODevice::WriteOnly)) {
                    file.write(data);
                    file.close();
                    ioLog.trace("WorkspaceService::executeDownloadFileRequest: Download finished and saved to" + localFilePath);
                    // Open this file or file folder.
                    DocumentFormatId formatId;
                    FormatDetectionConfig detectionConfig;
                    QList<FormatDetectionResult> detectionResults = DocumentUtils::detectFormat(localFilePath, detectionConfig);
                    if (!detectionResults.isEmpty()) {
                        auto loadDocumentTask = AppContext::getProjectLoader()->openWithProjectTask(localFilePath);
                        if (loadDocumentTask != nullptr) {
                            AppContext::getTaskScheduler()->registerTopLevelTask(loadDocumentTask);
                        }
                    } else {
                        QDesktopServices::openUrl(QUrl::fromLocalFile(downloadDirPath));
                    }
                } else {
                    ioLog.trace("WorkspaceService::executeDownloadFileRequest: Could not open file for writing.");
                }
            }
        } else {
            ioLog.trace("WorkspaceService::executeDownloadFileRequest ERROR:" + reply->errorString());
            fillErrorStateInJson(reply, jsonResponse);
        }
        if (callback && !contextLifeRangeTracker.isNull()) {
            callback(jsonResponse);
        }
        reply->deleteLater();
        networkManager->deleteLater();
    });
}

void WorkspaceService::executeUploadFileRequest(const QList<QString>& cloudDirPath,
                                                const QString& localFilePath,
                                                QObject* context,
                                                std::function<void(const QJsonObject&)> callback) {
    coreLog.trace("WorkspaceService::executeUploadFileRequest: " + localFilePath + "to " + cloudDirPath.join("/"));
    SAFE_POINT(callback == nullptr || context != nullptr, "A callback requires non-null life-range context", );
    QUrl downloadUrl(apiUrl + "/api/storage/upload");

    QNetworkRequest request(downloadUrl);
    if (!accessToken.isEmpty()) {
        request.setRawHeader("Authorization", "Bearer " + accessToken.toUtf8());
    }

    // Disable cache usage.
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
    request.setAttribute(QNetworkRequest::CacheSaveControlAttribute, false);

    request.setRawHeader("Authorization", "Bearer " + accessToken.toUtf8());

    // Prepare the multipart form data.
    auto multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    // Path part
    QHttpPart pathPart;
    QJsonArray jsonArray = QJsonArray::fromStringList(cloudDirPath);
    QByteArray jsonData = QJsonDocument(jsonArray).toJson(QJsonDocument::Compact);
    pathPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"path\""));
    pathPart.setBody(jsonData);

    // File part.
    QString cloudFileName = QFileInfo(localFilePath).fileName();
    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(R"(form-data; name="files"; filename=")" + cloudFileName + "\""));

    auto file = new QFile(localFilePath);
    if (!file->open(QIODevice::ReadOnly)) {
        ioLog.trace("Could not open file for reading:" + localFilePath);
        delete file;
        delete multiPart;
        return;
    }
    filePart.setBodyDevice(file);
    file->setParent(multiPart);  // The multiPart takes ownership of the file.

    multiPart->append(pathPart);
    multiPart->append(filePart);

    auto manager = new QNetworkAccessManager();
    QNetworkReply* reply = manager->post(request, multiPart);
    multiPart->setParent(reply);  // the multiPart object will be deleted along with the reply

    QPointer<QObject> contextLifeRangeTracker(context);
    connect(reply, &QNetworkReply::uploadProgress, [](qint64 bytesSent, qint64 bytesTotal) {
        ioLog.trace("WorkspaceService::executeUploadFileRequest: Progress:" + QString::number(bytesSent) + "/" + QString::number(bytesTotal));
    });

    connect(reply, &QNetworkReply::finished, [reply, manager, file, callback, contextLifeRangeTracker] {
        QJsonObject jsonResponse;
        if (reply->error() == QNetworkReply::NoError) {
            ioLog.trace("WorkspaceService::executeUploadFileRequest: Upload finished");
        } else {
            ioLog.trace("WorkspaceService::executeUploadFileRequest: ERROR:" + reply->errorString());
            fillErrorStateInJson(reply, jsonResponse);
        }
        file->close();
        if (callback && !contextLifeRangeTracker.isNull()) {
            callback(jsonResponse);
        }
        reply->deleteLater();
        manager->deleteLater();
    });
}

QString WorkspaceService::getWebWorkspaceUrl() const {
    return webWorkspaceUrl;
}

EnableWorkspaceTask::EnableWorkspaceTask(WorkspaceService* _ws)
    : Task(tr("Enable Workspace"), TaskFlag_NoRun), ws(_ws) {
}

Task::ReportResult EnableWorkspaceTask::report() {
    ws->enable();
    return ReportResult_Finished;
}

}  // namespace U2
