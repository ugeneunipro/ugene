#include "CloudStorageService.h"
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

#include <QJsonDocument>

#include <U2Core/U2SafePoints.h>;

#include "WebSocketClientService.h"
#include "WorkspaceService.h"
namespace U2 {

CloudStorageService::CloudStorageService(WorkspaceService* ws)
    : QObject(ws), workspaceService(ws) {
    connect(workspaceService, &WorkspaceService::si_authenticationEvent, this, [this] {
        auto webSocketService = workspaceService->getWebSocketService();
        if (webSocketService != nullptr) {
            webSocketService->subscribe(WebSocketSubscription(WebSocketSubscriptionType::StorageState, "", this));
            connect(webSocketService, &WebSocketClientService::si_messageReceived, this, &CloudStorageService::onWebSocketMessageReceived, Qt::UniqueConnection);
        }
    });
}

void CloudStorageService::onWebSocketMessageReceived(const QJsonObject& message) {
    QJsonDocument jsonDoc(message);
    QString jsonString = jsonDoc.toJson(QJsonDocument::Indented);
    emit si_storageStateChanged(jsonString);
}

}  // namespace U2