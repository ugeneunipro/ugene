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

#include "CloudStorageService.h"

#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>

#include <U2Core/GUrlUtils.h>
#include <U2Core/U2SafePoints.h>

#include "WebSocketClientService.h"
#include "WorkspaceService.h"

namespace U2 {

CloudStorageService::CloudStorageService(WorkspaceService* ws)
    : QObject(ws), workspaceService(ws), rootEntry({""}, 0, QDateTime(), 0, {}) {
    connect(workspaceService, &WorkspaceService::si_authenticationEvent, this, [this] {
        auto webSocketService = workspaceService->getWebSocketService();
        if (webSocketService != nullptr) {
            QString subscriptionId = WebSocketSubscription::generateSubscriptionId();
            webSocketService->subscribe(WebSocketSubscription(subscriptionId, WebSocketSubscriptionType::StorageState, "", this));
            connect(webSocketService, &WebSocketClientService::si_messageReceived, this, &CloudStorageService::onWebSocketMessageReceived, Qt::UniqueConnection);
        }
    });
}

const CloudStorageEntry& CloudStorageService::getRootEntry() const {
    return rootEntry;
}

void CloudStorageService::createDir(const QList<QString>& path,
                                    QObject* context,
                                    std::function<void(const QJsonObject&)> callback) const {
    SAFE_POINT(checkCloudStoragePath(path), "Invalid cloud file path: " + path.join("/"), );
    ioLog.trace("CloudStorageService::createDir: " + path.join("/"));
    QJsonObject payload;
    payload["path"] = QJsonArray::fromStringList(path);
    workspaceService->executeApiRequest("/storage/createDir", payload, context, callback);
}

void CloudStorageService::deleteEntry(const QList<QString>& path,
                                      QObject* context,
                                      std::function<void(const QJsonObject&)> callback) const {
    ioLog.trace("CloudStorageService::deleteEntry: " + path.join("/"));
    SAFE_POINT(checkCloudStoragePath(path), "Invalid cloud file path: " + path.join("/"), );
    QJsonObject payload;
    payload["path"] = QJsonArray::fromStringList(path);
    workspaceService->executeApiRequest("/storage/delete", payload, context, callback);
}

void CloudStorageService::renameEntry(const QList<QString>& oldPath,
                                      const QList<QString>& newPath,
                                      QObject* context,
                                      std::function<void(const QJsonObject&)> callback) const {
    ioLog.trace("CloudStorageService::renameEntry: " + oldPath.join("/") + " -> " + newPath.join("/"));
    SAFE_POINT(checkCloudStoragePath(oldPath), "Invalid old file path: " + oldPath.join("/"), );
    SAFE_POINT(checkCloudStoragePath(newPath), "Invalid new file path: " + newPath.join("/"), );
    QJsonObject payload;
    payload["path"] = QJsonArray::fromStringList(oldPath);
    payload["newPath"] = QJsonArray::fromStringList(newPath);
    workspaceService->executeApiRequest("/storage/move", payload, context, callback);
}

void CloudStorageService::shareEntry(const QList<QString>& path,
                                     const QString& email,
                                     QObject* context,
                                     std::function<void(const QJsonObject&)> callback) const {
    ioLog.trace("CloudStorageService::shareEntry: " + path.join("/") + " -> " + email);
    SAFE_POINT(checkCloudStoragePath(path), "Invalid file path: " + path.join("/"), );
    SAFE_POINT(checkEmail(email), "Invalid email: " + email, );
    QJsonObject payload;
    payload["path"] = QJsonArray::fromStringList(path);
    payload["email"] = email.toLower();
    workspaceService->executeApiRequest("/storage/share", payload, context, callback);
}

void CloudStorageService::unshareEntry(const QList<QString>& path,
                                       const QString& email,
                                       QObject* context,
                                       std::function<void(const QJsonObject&)> callback) const {
    ioLog.trace("CloudStorageService::unshareEntry: " + path.join("/") + " -> " + email);
    SAFE_POINT(checkCloudStoragePath(path), "Invalid file path: " + path.join("/"), );
    SAFE_POINT(checkEmail(email), "Invalid email: " + email, );
    QJsonObject payload;
    payload["path"] = QJsonArray::fromStringList(path);
    payload["email"] = email.toLower();
    workspaceService->executeApiRequest("/storage/unshare", payload, context, callback);
}

void CloudStorageService::downloadFile(const QList<QString>& path,
                                       const QString& localDirPath,
                                       QObject* context,
                                       std::function<void(const QJsonObject&)> callback) const {
    ioLog.trace("CloudStorageService::downloadFile: " + path.join("/") + " -> " + localDirPath);
    SAFE_POINT(checkCloudStoragePath(path), "Invalid cloud file path: " + path.join("/"), );
    QFileInfo dir(localDirPath);
    const QString& fileName = path.last();
    QString localFilePath = GUrlUtils::rollFileName(dir.absoluteFilePath() + "/" + fileName, "_");
    workspaceService->executeDownloadFileRequest(path, localFilePath, context, callback);
}

void CloudStorageService::uploadFile(const QList<QString>& cloudDirPath,
                                     const QString& localFilePath,
                                     QObject* context,
                                     std::function<void(const QJsonObject&)> callback) const {
    ioLog.trace("CloudStorageService::uploadFile: " + localFilePath + "->" + cloudDirPath.join("/"));
    SAFE_POINT(checkCloudStoragePath(cloudDirPath, true), "Invalid cloud file path: " + cloudDirPath.join("/"), );
    workspaceService->executeUploadFileRequest(cloudDirPath, localFilePath, context, callback);
}

static QRegularExpression forbiddenChars(R"([<>:"/\\|?*\x00])");

bool CloudStorageService::checkCloudStorageEntryName(const QString& entryName) {
    CHECK_EXT(!entryName.isEmpty(), ioLog.trace("File name is empty"), false);
    CHECK_EXT(!forbiddenChars.match(entryName).hasMatch(), ioLog.trace("File name contains forbidden characters: " + entryName), false);
    CHECK_EXT(entryName.trimmed() == entryName, ioLog.trace("File name has leading or trailing spaces: " + entryName), false);
    for (const QChar& c : qAsConst(entryName)) {
        CHECK_EXT(c.category() != QChar::Other_Control, ioLog.trace("File name contains control characters: " + entryName), false);
    }
    CHECK_EXT(entryName.length() <= 255, ioLog.trace("File name exceeds maximum length: " + entryName), false);
    return true;
}

static const QRegularExpression emailRegex(R"((^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$))");

bool CloudStorageService::checkEmail(const QString& email) {
    return emailRegex.match(email).hasMatch();
}

bool CloudStorageService::checkCloudStoragePath(const QList<QString>& path, bool isDir) {
    CHECK(!path.isEmpty() || isDir, false);
    return std::all_of(path.cbegin(), path.cend(), [](const auto& pathEntry) {
        return checkCloudStorageEntryName(pathEntry);
    });
}

void CloudStorageService::onWebSocketMessageReceived(const WebSocketSubscriptionType& type, const QString&, const QJsonObject& payload) {
    ioLog.trace("CloudStorageService::onWebSocketMessageReceived");
    CHECK(type == WebSocketSubscriptionType::StorageState, );
    rootEntry = CloudStorageEntry::fromJson(payload, {});
    emit si_storageStateChanged(rootEntry);
}

static constexpr int ROOT_ENTRY_SESSION_LOCAL_ID = 0;

CloudStorageEntryData::CloudStorageEntryData(const QList<QString>& _path,
                                             qint64 _size,
                                             const QDateTime& _modificationTime,
                                             qint64 _sessionLocalId,
                                             const QList<QString>& _sharedWithEmails)
    : path(_path), size(_size), modificationTime(_modificationTime), sessionLocalId(_sessionLocalId), sharedWithEmails(_sharedWithEmails) {
    SAFE_POINT(!path.isEmpty() || sessionLocalId == ROOT_ENTRY_SESSION_LOCAL_ID, "Item path must not be empty", );  // Only root path can be empty.
}

const QString& CloudStorageEntryData::getName() const {
    return path.last();
}

CloudStorageEntry::CloudStorageEntry(const QList<QString>& path,
                                     qint64 size,
                                     const QDateTime& modificationTime,
                                     qint64 sessionLocalId,
                                     const QList<QString>& sharedWithEmails)
    : data(new CloudStorageEntryData(path, size, modificationTime, sessionLocalId, sharedWithEmails)) {
    coreLog.info("Path: " + path.join("/") + " shared: " + sharedWithEmails.join(","));
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
    QList<QString> sharedWithEmails;
    auto sharingStateJson = json["sharingState"];
    if (!sharingStateJson.isUndefined()) {
        auto sharedWithRecipientsJsonArray = sharingStateJson["sharedWith"].toArray();
        for (const QJsonValue& recipient : qAsConst(sharedWithRecipientsJsonArray)) {
            auto email = recipient["email"].toString();
            if (!email.isEmpty()) {
                sharedWithEmails.append(email);
            }
        }
    }

    QList<QString> path = parentPath;
    bool isRoot = parentPath.isEmpty() && name.isEmpty();
    if (!isRoot) {  // Root path is an empty list.
        path.append(name);
    }
    CloudStorageEntry entry(path, size, modificationTime, sessionLocalId, sharedWithEmails);
    entry->isFolder = json["isFolder"].toBool();

    QList<CloudStorageEntry> children;
    QJsonArray childrenArray = json["children"].toArray();
    for (const QJsonValue& childValue : qAsConst(childrenArray)) {
        children.append(fromJson(childValue.toObject(), path));
    }
    entry->children = children;

    return entry;
}

}  // namespace U2
