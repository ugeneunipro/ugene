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

#pragma once

#include <QAction>

#include <U2Core/ServiceTypes.h>
#include <U2Core/Task.h>

namespace U2 {

class ProjectViewImpl;
class WebSocketClientService;
class EnableWorkspaceTask;

class WorkspaceService : public Service {
    Q_OBJECT
    friend class EnableWorkspaceTask;

public:
    WorkspaceService();

    bool isLoggedIn() const;

    /** Opens login dialog. Does nothing if user is already logged in. */
    void login();

    /** Logs out user. Does nothing if user is already logged out. */
    void logout();

    WebSocketClientService* getWebSocketService() const;

signals:
    void si_authenticationEvent(bool isLoggedIn);

protected:
    Task* createServiceEnablingTask() override;

private:
    void enable();
    void updateMainMenuActions();
    void renewAccessToken();
    void setTokens(const QString& newAccessToken, const QString& newRefreshToken, bool saveToSettings);

    QString accessToken;
    QString refreshToken;
    QAction* loginAction;
    QAction* logoutAction;
    WebSocketClientService* webSocketService = nullptr;
    QAction* separatorAction = nullptr;
    QString authUrl = "https://auth.ugene.net/realms/ugene-prod/protocol/openid-connect/auth";
    QString tokenUrl = "https://auth.ugene.net/realms/ugene-prod/protocol/openid-connect/token";
    QString clientId = "workspace-client-prod";
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