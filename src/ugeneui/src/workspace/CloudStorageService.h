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

class CloudStorageEntryData : public QSharedData {
public:
    CloudStorageEntryData(const QList<QString>& path, qint64 size, const QDateTime& modificationTime, qint64 sessionLocalId);

    const QString& getName() const;

    QList<QString> path;

    QList<CloudStorageEntry> children;

    qint64 size;

    QDateTime modificationTime;

    qint64 sessionLocalId;

    bool isFolder = false;
};

class CloudStorageEntry {
public:
    CloudStorageEntry(const QList<QString>& path, qint64 size, const QDateTime& modificationTimel, qint64 sessionLocalId);

    static CloudStorageEntry fromJson(const QJsonObject& json, const QList<QString>& parentPath);

    CloudStorageEntryData* operator->();

    const CloudStorageEntryData* operator->() const;

private:
    QSharedDataPointer<CloudStorageEntryData> data;
};

class CloudStorageService : public QObject {
    Q_OBJECT

public:
    CloudStorageService(WorkspaceService* ws);

    const CloudStorageEntry& getRootEntry() const;

    void createDir(const QList<QString>& path);

    void deleteItem(const QList<QString>& path);

    static bool checkCloudStorageFolderName(const QString& folderName);

signals:
    void si_storageStateChanged(const CloudStorageEntry& rootEntry);

private:
    void onWebSocketMessageReceived(const WebSocketSubscriptionType& subscriptionType, const QString& entityId, const QJsonObject& payload);

    WorkspaceService* workspaceService = nullptr;
    CloudStorageEntry rootEntry;
};

}  // namespace U2
