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
    : QObject(ws), workspaceService(ws), rootEntry({""}, 0, QDateTime(), 0) {
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

void CloudStorageService::createDir(const QList<QString>& path) {
    qDebug() << "CloudStorageService::createDir: " + path.join(",");
    QJsonObject payload;
    payload["path"] = QJsonArray::fromStringList(path);
    workspaceService->executeApiRequest("/storage/createDir", payload);
}

void CloudStorageService::deleteItem(const QList<QString>& path) {
    qDebug() << "CloudStorageService::deleteItem: " + path.join(",");
    QJsonObject payload;
    payload["path"] = QJsonArray::fromStringList(path);
    workspaceService->executeApiRequest("/storage/delete", payload);
}

void CloudStorageService::onWebSocketMessageReceived(const WebSocketSubscriptionType& type, const QString&, const QJsonObject& payload) {
    qDebug() << "CloudStorageService::onWebSocketMessageReceived";
    CHECK(type == WebSocketSubscriptionType::StorageState, );
    rootEntry = CloudStorageEntry::fromJson(payload, {});
    emit si_storageStateChanged(rootEntry);
}

CloudStorageEntryData::CloudStorageEntryData(const QList<QString>& _path, qint64 _size, const QDateTime& _modificationTime, qint64 _sessionLocalId)
    : path(_path), size(_size), modificationTime(_modificationTime), sessionLocalId(_sessionLocalId) {
    SAFE_POINT(path.length() >= 1, "Item path must not be empty", );
}

const QString& CloudStorageEntryData::getName() const {
    return path.last();
}

CloudStorageEntry::CloudStorageEntry(const QList<QString>& path, qint64 size, const QDateTime& modificationTime, qint64 sessionLocalId)
    : data(new CloudStorageEntryData(path, size, modificationTime, sessionLocalId)) {
}

CloudStorageEntryData* CloudStorageEntry::operator->() {
    return data.data();
}
const CloudStorageEntryData* CloudStorageEntry::operator->() const {
    return data.data();
}

CloudStorageEntry CloudStorageEntry::fromJson(const QJsonObject& json, const QList<QString>& parentPath) {
    QString name = json["name"].toString();
    qint64 size = json["size"].toVariant().toLongLong();
    QDateTime modificationTime = QDateTime::fromString(json["modificationTime"].toString(), Qt::ISODate);
    qint64 sessionLocalId = json["sessionLocalId"].toVariant().toLongLong();

    QList<QString> path = parentPath;
    path.append(name);
    CloudStorageEntry entry(path, size, modificationTime, sessionLocalId);
    entry->isFolder = json["isFolder"].toBool();

    QList<CloudStorageEntry> children;
    QJsonArray childrenArray = json["children"].toArray();
    for (const QJsonValue& childValue : childrenArray) {
        children.append(fromJson(childValue.toObject(), path));
    }
    entry->children = children;

    return entry;
}

}  // namespace U2