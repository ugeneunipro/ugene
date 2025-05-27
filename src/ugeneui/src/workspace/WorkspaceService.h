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

#pragma once

#include <QAction>
#include <QJsonObject>
#include <QNetworkAccessManager>

#include <U2Core/ServiceTypes.h>
#include <U2Core/Task.h>

namespace U2 {
class CloudStorageService;

class ProjectViewImpl;
class WebSocketClientService;
class EnableWorkspaceTask;

/** Workspace service enables support of all kinds of operations with UGENE Cloud Workspace. */
class WorkspaceService : public Service {
    Q_OBJECT
    friend class EnableWorkspaceTask;

public:
    WorkspaceService();

    /** Returns true if the workspace is currently in the logged in state. */
    bool isLoggedIn() const;

    QString getCurrentUserEmail() const;

    /** Opens login dialog. Does nothing if user is already logged in. */
    void login();

    /** Logs out user. Does nothing if user is already logged out. */
    void logout();

    CloudStorageService* getCloudStorageService() const;

    WebSocketClientService* getWebSocketService() const;

    /** Executes an API request to Workspace. */
    void executeApiRequest(const QString& apiPath,
                           const QJsonObject& payload,
                           QObject* context = nullptr,
                           std::function<void(const QJsonObject&)> callback = nullptr);

    /** Downloads a file from the workspace. */
    void executeDownloadFileRequest(const QList<QString>& cloudPath,
                                    const QString& localFilePath,
                                    QObject* context = nullptr,
                                    std::function<void(const QJsonObject&)> callback = nullptr);

    /** Uploads a file from the workspace. */
    void executeUploadFileRequest(const QList<QString>& cloudDirPath,
                                  const QString& localFilePath,
                                  QObject* context = nullptr,
                                  std::function<void(const QJsonObject&)> callback = nullptr);

    /**
     * Returns error message from the response in case if the response is error response.
     * Returns empty string otherwise.
     */
    static QString getErrorMessageFromResponse(const QJsonObject& response);

    QString getWebWorkspaceUrl() const;

signals:
    /** Emitted every time authentication state is changed. */
    void si_authenticationEvent(bool isLoggedIn);

protected:
    Task* createServiceEnablingTask() override;

private slots:
    void sl_colorModeSwitched();

private:
    void enable();
    void updateMainMenuActions();
    void renewAccessTokenIfCloseToExpire();
    void setTokens(const QString& newAccessToken, const QString& newRefreshToken, bool saveToSettings);
    void setTokenFields(const QString& newAccessToken, const QString& newRefreshToken, bool saveToSettings);

    QString accessToken = nullptr;
    QString refreshToken = nullptr;
    QAction* loginAction = nullptr;
    QAction* logoutAction = nullptr;
    /**
     * Websocket service is created for every session.
     * When user is logged out webSocketService is null.
     */
    WebSocketClientService* webSocketService = nullptr;
    CloudStorageService* cloudStorageService = nullptr;
    QAction* separatorAction = nullptr;
    QString webWorkspaceUrl;
    QString authUrl;
    QString logoutUrl;
    QString tokenUrl;
    QString clientId;
    QString stage;
    QString apiUrl;
    QString webSocketUrl;
};

class EnableWorkspaceTask : public Task {
    Q_OBJECT

public:
    explicit EnableWorkspaceTask(WorkspaceService* ws);

    ReportResult report() override;

private:
    WorkspaceService* ws;
};
}  // namespace U2
