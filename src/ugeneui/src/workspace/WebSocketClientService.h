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

#include <QJsonObject>
#include <QObject>
#include <QSet>
#include <QTimer>
#include <QWebSocket>

namespace U2 {

enum class WebSocketRequestType {
    Subscribe,
    Unsubscribe,
    UpdateAccessToken,
    Acknowledge,
};

enum class WebSocketSubscriptionType {
    StorageState,
    Invalid,
};

class WebSocketOutgoingMessage {
public:
    WebSocketOutgoingMessage(const WebSocketRequestType& type, const QJsonObject& request);
    WebSocketRequestType type;
    QJsonObject request;
};

class WebSocketSubscription {
public:
    WebSocketSubscription(const WebSocketSubscriptionType& type, const QString& entityId, QObject* subscriber = nullptr);

    WebSocketSubscriptionType type;
    QString entityId;
    /** If set the subscription will be automatically unsubscribed when this object is deleted. */
    QObject* subscriber;
};

class WebSocketClientService : public QObject {
    Q_OBJECT

public:
    explicit WebSocketClientService(const QString& domain, QObject* parent = nullptr);
    ~WebSocketClientService() override;

    void setAccessToken(const QString& accessToken);
    void reconnectIfNotConnected();

    bool isConnected() const;

    void subscribe(const WebSocketSubscription& subscription);
    void unsubscribe(const WebSocketSubscription& subscription);

signals:
    void si_connectionStateChanged(bool isConnected);
    void si_messageReceived(const WebSocketSubscriptionType& type, const QString& entityId, const QJsonObject& payload);

private slots:
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString& message);
    void onError(QAbstractSocket::SocketError error);

private:
    void sendMessage(const WebSocketOutgoingMessage& message);
    void sendPendingMessages();
    void disconnect(bool clearSubscriptions = true);
    void clearSubscriptions();
    void sendAccessTokenToServer();

    QString domainAndPort;
    QWebSocket* socket;
    /** ID if the client. Never changes. */
    const QString clientId;
    bool connectionReady = false;
    QSet<QString> receivedMessageIds;
    bool isRetrying = false;
    QString accessToken;
    QList<WebSocketSubscription> subscriptions;
    QList<WebSocketOutgoingMessage> pendingMessages;
};

}  // namespace U2