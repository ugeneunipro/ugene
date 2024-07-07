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
#include <QJsonDocument>
#include <QUuid>

namespace U2 {
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
    receivedMessageIds.insert(incomingMessage["messageId"].toString());
    // Handle incoming message
}

void WebSocketClientService::onError(QAbstractSocket::SocketError error) {
    qWarning() << "WebSocket error:" << error;
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

void WebSocketClientService::sendMessage(const QString& type, const QJsonObject& request) {
    if (connectionReady) {
        qDebug() << "WebSocketClientService: sending message to the server:" << type << request;
        QJsonObject message = request;
        message["type"] = type;
        socket->sendTextMessage(QJsonDocument(message).toJson(QJsonDocument::Compact));
    }
}

void WebSocketClientService::refreshWebSocketConnection(const QString& accessToken) {
    this->accessToken = accessToken;
    if (socket->isValid()) {
        updateAccessToken(accessToken);
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

void WebSocketClientService::updateAccessToken(const QString& accessToken) {
    if (!socket->isValid()) {
        qWarning() << "Not connected to WebSocket during access token update.";
        return;
    }
    qDebug() << "Updating access token";

    QJsonObject message;
    message["accessToken"] = accessToken;
    message["clientId"] = clientId;
    sendMessage("updateAccessToken", message);
}

void WebSocketClientService::subscribe(const QJsonObject& subscription) {
    Q_UNUSED(subscription);
    // Implement subscription logic
}

void WebSocketClientService::unsubscribe(const QJsonObject& subscription) {
    Q_UNUSED(subscription);
    // Implement unsubscription logic
}

bool WebSocketClientService::isConnected() const {
    return connectionReady;
}

}  // namespace U2