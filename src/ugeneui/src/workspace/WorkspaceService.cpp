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
#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/L10n.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/MainWindow.h>

#include "KeycloakAuthenticator.h"
#include "WebSocketClientService.h"

namespace U2 {

WorkspaceService::WorkspaceService()
    : Service(Service_Workspace, "Workspace", "Remove workspace service for UGENE") {
    loginAction = new QAction(QIcon(":ugene/images/login_icon.svg"), tr("Login to Workspace"));
    loginAction->setObjectName("loginToWorkspaceAction");
    connect(loginAction, &QAction::triggered, this, &WorkspaceService::login);

    logoutAction = new QAction(QIcon(":ugene/images/logout_icon.svg"), tr("Logout from Workspace"));
    logoutAction->setObjectName("logoutFromWorkspaceAction");
    connect(logoutAction, &QAction::triggered, this, &WorkspaceService::logout);

    connect(this, &WorkspaceService::si_authenticationEvent, this, &WorkspaceService::updateMainMenuActions);

    webSocketService = new WebSocketClientService(this);
}

Task* WorkspaceService::createServiceEnablingTask() {
    return new EnableWorkspaceTask(this);
}

bool WorkspaceService::isLoggedIn() const {
    return !accessToken.isEmpty();
}

void WorkspaceService::enable() {
    updateMainMenuActions();
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

void WorkspaceService::login() {
    QString authUrl = "https://auth.ugene.net/realms/ugene-prod/protocol/openid-connect/auth";
    QString tokenUrl = "https://auth.ugene.net/realms/ugene-prod/protocol/openid-connect/token";
    QString clientId = "workspace-client-prod";

    auto authenticator = new KeycloakAuthenticator(authUrl, tokenUrl, clientId);
    connect(authenticator, &KeycloakAuthenticator::si_authenticationGranted, this, [this](const QString& accessToken) {
        this->accessToken = accessToken;
        updateMainMenuActions();
        si_authenticationEvent(true);
        webSocketService->refreshWebSocketConnection(this->accessToken);
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
