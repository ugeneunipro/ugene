/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include "WorkspaceConnectionStatusIcon.h"

#include <U2Core/AppContext.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>

#include "WebSocketClientService.h"
#include "WorkspaceService.h"

namespace U2 {

WorkspaceConnectionStatusIcon::WorkspaceConnectionStatusIcon() {
    auto serviceRegistry = AppContext::getServiceRegistry();
    SAFE_POINT(serviceRegistry, "serviceRegistry is nullptr", );
    connect(serviceRegistry, &ServiceRegistry::si_serviceStateChanged, this, [this](const Service* service) {
        if (service->getType() == Service_Workspace) {
            reconnectToWorkspace();
        }
    });
    reconnectToWorkspace();
}

void WorkspaceConnectionStatusIcon::reconnectToWorkspace() {
    const auto& services = AppContext::getServiceRegistry()->getServices();
    if (workspaceService != nullptr) {
        disconnect(workspaceService);
        disconnect(workspaceService->getWebSocketService());
        workspaceService = nullptr;
    }
    for (const auto& service : qAsConst(services)) {
        if (service->getType() == Service_Workspace) {
            workspaceService = qobject_cast<WorkspaceService*>(service);
            break;
        }
    }
    if (workspaceService && workspaceService->isEnabled()) {
        connect(workspaceService, &WorkspaceService::si_authenticationEvent, this, [this] {
            updateIcon();
            auto websocketService = workspaceService->getWebSocketService();
            if (websocketService != nullptr) {
                connect(workspaceService->getWebSocketService(), &WebSocketClientService::si_connectionStateChanged, this, [this] { updateIcon(); }, Qt::UniqueConnection);
            }
        });
    }
    updateIcon();
}

void WorkspaceConnectionStatusIcon::updateIcon() {
    QString iconName = ":/ugene/images/cloud_disabled.svg";
    QString tooltip = tr("Workspace is disconnected.\nUse 'File->Login to Workspace' to login.");
    if (workspaceService != nullptr && workspaceService->isLoggedIn()) {
        auto webSocketService = workspaceService->getWebSocketService();
        auto isConnected = webSocketService != nullptr && webSocketService->isConnected();
        iconName = isConnected ? GUIUtils::getThemedPath(":/ugene/images/cloud_connected.svg") : GUIUtils::getThemedPath(":/ugene/images/cloud_connecting.svg");
        tooltip = isConnected ? tr("Connected to Workspace") : tr("Connecting to workspace...");
    }
    setPixmap(QIcon(iconName).pixmap(20, 20));
    setToolTip(tooltip);
}

}  // namespace U2
