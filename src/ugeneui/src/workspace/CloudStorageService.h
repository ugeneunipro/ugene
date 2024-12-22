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

#include <QDateTime>
#include <QObject>

namespace U2 {

class CloudStorageEntry;
enum class WebSocketSubscriptionType;
class WebSocketSubscription;
class WorkspaceService;

/** A single file/folder in the cloud storage. */
class CloudStorageEntryData : public QSharedData {
public:
    CloudStorageEntryData(const QList<QString>& path,
                          qint64 size,
                          const QDateTime& modificationTime,
                          qint64 sessionLocalId,
                          const QList<QString>& sharedWithEmails);

    const QString& getName() const;

    QList<QString> path;

    QList<CloudStorageEntry> children;

    qint64 size;

    QDateTime modificationTime;

    qint64 sessionLocalId;

    QList<QString> sharedWithEmails;

    bool isFolder = false;
};

class CloudStorageEntry {
public:
    CloudStorageEntry(const QList<QString>& path,
                      qint64 size,
                      const QDateTime& modificationTime,
                      qint64 sessionLocalId,
                      const QList<QString>& sharedWithEmails);

    static CloudStorageEntry fromJson(const QJsonObject& json, const QList<QString>& parentPath);

    CloudStorageEntryData* operator->();

    const CloudStorageEntryData* operator->() const;

private:
    QSharedDataPointer<CloudStorageEntryData> data;
};

/** Cloud storage service supports commands to access & manipulate cloud storage content. */
class CloudStorageService : public QObject {
    Q_OBJECT

public:
    CloudStorageService(WorkspaceService* ws);

    const CloudStorageEntry& getRootEntry() const;

    void createDir(const QList<QString>& path,
                   QObject* context = nullptr,
                   std::function<void(const QJsonObject&)> callback = nullptr) const;

    void deleteEntry(const QList<QString>& path,
                     QObject* context = nullptr,
                     std::function<void(const QJsonObject&)> callback = nullptr) const;

    void renameEntry(const QList<QString>& oldPath,
                     const QList<QString>& newPath,
                     QObject* context = nullptr,
                     std::function<void(const QJsonObject&)> callback = nullptr) const;

    void shareEntry(const QList<QString>& path,
                    const QString& email,
                    QObject* context = nullptr,
                    std::function<void(const QJsonObject&)> callback = nullptr) const;

    void unshareEntry(const QList<QString>& path,
                      const QString& email,
                      QObject* context = nullptr,
                      std::function<void(const QJsonObject&)> callback = nullptr) const;

    void downloadFile(const QList<QString>& path,
                      const QString& localDirPath,
                      QObject* context = nullptr,
                      std::function<void(const QJsonObject&)> callback = nullptr) const;

    void uploadFile(const QList<QString>& cloudDirPath,
                    const QString& localFilePath,
                    QObject* context = nullptr,
                    std::function<void(const QJsonObject&)> callback = nullptr) const;

    /** Checks that the 'entryName' is an acceptable cloud storage file/folder name. */
    static bool checkCloudStorageEntryName(const QString& entryName);

    /** Checks that the 'email' is a valid email address. */
    static bool checkEmail(const QString& email);

    /** Checks that the 'path' is a valid cloud storage path: is not empty (unless is dir), does not contain empty or invalid items. */
    static bool checkCloudStoragePath(const QList<QString>& path, bool isDir = false);

signals:
    /** Emitted every time cloud storage state is changed. */
    void si_storageStateChanged(const CloudStorageEntry& rootEntry);

private:
    void onWebSocketMessageReceived(const WebSocketSubscriptionType& subscriptionType, const QString& entityId, const QJsonObject& payload);

    WorkspaceService* workspaceService = nullptr;
    CloudStorageEntry rootEntry;
};

}  // namespace U2
