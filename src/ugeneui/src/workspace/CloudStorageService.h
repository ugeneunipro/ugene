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
enum class WebSocketSubscriptionType;
class WebSocketSubscription;
class WorkspaceService;
}  // namespace U2
namespace U2 {

class CloudStorageService : public QObject {
    Q_OBJECT

public:
    CloudStorageService(WorkspaceService* ws);

signals:
    void si_storageStateChanged(const QString& state);

private:
    void onWebSocketMessageReceived(const WebSocketSubscriptionType& subscriptionType, const QString& entityId, const QJsonObject& payload);

    WorkspaceService* workspaceService = nullptr;
};

class StorageEntry {
public:
    QString name;
    QList<StorageEntry> children;
    qint64 size = 0;
    QDateTime modificationTime;

    StorageEntry(const QString& name, qint64 size, const QDateTime& modificationTime);

    bool isFolder() const;
};
}  // namespace U2