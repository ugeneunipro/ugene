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

#include "WebSocketClientService.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QUuid>

#include <U2Core/U2SafePoints.h>

namespace U2 {

static QString getWebSocketRequestTypeAsString(const WebSocketRequestType& type) {
    switch (type) {
        case WebSocketRequestType::Subscribe:
            return "subscribe";
        case WebSocketRequestType::Unsubscribe:
            return "unsubscribe";
        case WebSocketRequestType::UpdateAccessToken:
            return "updateAccessToken";
        case WebSocketRequestType::Acknowledge:
            return "acknowledge";
    }
    FAIL("Unexpected request type", "");
}

static QString getSubscriptionTypeAsString(const WebSocketSubscriptionType& type) {
    switch (type) {
        case WebSocketSubscriptionType::StorageState:
            return "STORAGE_STATE";
    }
    FAIL("Unexpected subscription type", "");
}

WebSocketClientService::WebSocketClientService(QObject* parent)
    : QObject(parent),
      socket(new QWebSocket()),
      clientId(QUuid::createUuid().toString()) {
    qDebug() << "WebSocketClientService is created";

    connect(socket, &QWebSocket::connected, this, &WebSocketClientService::onConnected);
    connect(socket, &QWebSocket::disconnected, this, &WebSocketClientService::onDisconnected);
    connect(socket, &QWebSocket::textMessageReceived, this, &WebSocketClientService::onTextMessageReceived);
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &WebSocketClientService::onError);
}

WebSocketClientService::~WebSocketClientService() {
    delete socket;
}

void WebSocketClientService::onConnected() {
    qDebug() << "Connected to websocket";
    connectionReady = true;
    emit si_connectionStateChanged(true);
}

void WebSocketClientService::onDisconnected() {
    qDebug() << "Disconnected from websocket";
    connectionReady = false;
    emit si_connectionStateChanged(false);
}

void WebSocketClientService::onTextMessageReceived(const QString& message) {
    qDebug() << "WebSocketClientService: received message" << message;
    if (message == "heartbeat") {
        return;
    }
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject incomingMessage = doc.object();
    auto messageId = incomingMessage["messageId"].toString();
    receivedMessageIds.insert(messageId);
    sendMessage(WebSocketRequestType::Acknowledge, {{"messageId", messageId}});
    emit si_messageReceived(incomingMessage);
}

void WebSocketClientService::onError(QAbstractSocket::SocketError error) {
    qDebug() << "WebSocket error:" << error;
    if (isRetrying) {
        return;
    }
    isRetrying = true;

    QTimer::singleShot(5000, [this] {
        if (isRetrying && !socket->isValid()) {
            qDebug() << "Retrying websocket connection...";
            refreshWebSocketConnection(accessToken);
        }
        isRetrying = false;
    });
}

void WebSocketClientService::sendMessage(const WebSocketRequestType& type, const QJsonObject& request) const {
    CHECK(connectionReady, );
    auto typeString = getWebSocketRequestTypeAsString(type);
    qDebug() << "WebSocketClientService: sending message to the server:" << typeString << request;
    QJsonObject message = request;
    message["type"] = typeString;
    message["clientId"] = clientId;
    if (!accessToken.isEmpty()) {
        message["accessToken"] = accessToken;
    }
    socket->sendTextMessage(QJsonDocument(message).toJson(QJsonDocument::Compact));
}

void WebSocketClientService::refreshWebSocketConnection(const QString& newAccessToken) {
    accessToken = newAccessToken;
    if (socket->isValid()) {
        updateAccessToken();
        return;
    }

    qDebug() << "Connecting to backend socket";
    QString wsBackendUrl = "wss://workspace.ugene.net/api/?accessToken=" + accessToken + "&clientId=" + clientId;
    socket->open(QUrl(wsBackendUrl));
}

void WebSocketClientService::disconnect(bool clearSubscriptions) {
    qDebug() << "Disconnecting from websocket, clearSubscriptions:" << clearSubscriptions;
    if (socket->isValid()) {
        socket->close();
    }
    connectionReady = false;

    if (clearSubscriptions) {
        clientId = QUuid::createUuid().toString();
        this->clearSubscriptions();
    }
}

void WebSocketClientService::clearSubscriptions() {
    subscriptions.clear();
}

void WebSocketClientService::updateAccessToken() const {
    if (!socket->isValid()) {
        qWarning() << "Not connected to WebSocket during access token update.";
        return;
    }
    qDebug() << "Updating access token";

    sendMessage(WebSocketRequestType::UpdateAccessToken, {});  // Will add the token to the message.
}

static QString getClientSubscriptionKey(const WebSocketSubscription& subscription) {
    return getSubscriptionTypeAsString(subscription.type) + (subscription.entityId.isEmpty() ? " " + subscription.entityId : "");
}

static bool hasSubscriptionWithKey(const QString& key, const QList<WebSocketSubscription>& subscriptions) {
    return std::any_of(subscriptions.cbegin(), subscriptions.cend(), [&key](const auto& subscription) {
        return getClientSubscriptionKey(subscription) == key;
    });
}

void WebSocketClientService::subscribe(const WebSocketSubscription& subscription) {
    auto subscriptionKey = getClientSubscriptionKey(subscription);
    if (!hasSubscriptionWithKey(subscriptionKey, subscriptions)) {
        QJsonObject subscriptionAsJsonObject;
        subscriptionAsJsonObject["type"] = getSubscriptionTypeAsString(subscription.type);
        if (!subscription.entityId.isEmpty()) {
            subscriptionAsJsonObject["entityId"] = subscription.entityId;
        };
        sendMessage(WebSocketRequestType::Subscribe, {{"subscriptions", QJsonArray {subscriptionAsJsonObject}}});
    }
    subscriptions << subscription;
}

void WebSocketClientService::unsubscribe(const WebSocketSubscription& subscription) {
    const auto subscriptionKey = getClientSubscriptionKey(subscription);
    const auto it = std::find_if(subscriptions.begin(), subscriptions.end(), [&](const auto& s) {
        return getClientSubscriptionKey(s) == subscriptionKey;
    });
    CHECK(it != subscriptions.cend(), );

    subscriptions.erase(it);

    if (!hasSubscriptionWithKey(subscriptionKey, subscriptions)) {
        QJsonObject subscriptionAsJsonObject;
        subscriptionAsJsonObject["type"] = getSubscriptionTypeAsString(subscription.type);
        if (!subscription.entityId.isEmpty()) {
            subscriptionAsJsonObject["entityId"] = subscription.entityId;
        };
        sendMessage(WebSocketRequestType::Unsubscribe, {{"subscriptions", QJsonArray {subscriptionAsJsonObject}}});
    }
}

bool WebSocketClientService::isConnected() const {
    return connectionReady;
}

}  // namespace U2