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

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>

#include <U2Core/U2SafePoints.h>

#include "WebSocketClientService.h"
#include "WorkspaceService.h"

namespace U2 {

CloudStorageService::CloudStorageService(WorkspaceService* ws)
    : QObject(ws), workspaceService(ws), rootEntry("", 0, QDateTime()) {
    connect(workspaceService, &WorkspaceService::si_authenticationEvent, this, [this] {
        auto webSocketService = workspaceService->getWebSocketService();
        if (webSocketService != nullptr) {
            webSocketService->subscribe(WebSocketSubscription(WebSocketSubscriptionType::StorageState, "", this));
            connect(webSocketService, &WebSocketClientService::si_messageReceived, this, &CloudStorageService::onWebSocketMessageReceived, Qt::UniqueConnection);
        }
    });
}

const CloudStorageEntry& CloudStorageService::getRootEntry() const {
    return rootEntry;
}

void CloudStorageService::onWebSocketMessageReceived(const WebSocketSubscriptionType& type, const QString&, const QJsonObject& payload) {
    qDebug() << "CloudStorageService::onWebSocketMessageReceived";
    CHECK(type == WebSocketSubscriptionType::StorageState, );
    rootEntry = CloudStorageEntry::fromJson(payload);
    emit si_storageStateChanged(rootEntry);
}

CloudStorageEntry::CloudStorageEntry(const QString& _name, qint64 _size, const QDateTime& _modificationTime)
    : name(_name), size(_size), modificationTime(_modificationTime) {
}

bool CloudStorageEntry::isFolder() const {
    return !children.isEmpty();
}

CloudStorageEntry CloudStorageEntry::fromJson(const QJsonObject& json) {
    QString name = json["name"].toString();
    qint64 size = json["size"].toVariant().toLongLong();
    QDateTime modificationTime = QDateTime::fromMSecsSinceEpoch(json["modificationTime"].toVariant().toLongLong());

    CloudStorageEntry entry(name, size, modificationTime);

    if (json.contains("children") && json["children"].isArray()) {
        QJsonArray childrenArray = json["children"].toArray();
        for (const QJsonValue& value : childrenArray) {
            if (value.isObject()) {
                entry.children.append(fromJson(value.toObject()));
            }
        }
    }
    return entry;
}

}  // namespace U2