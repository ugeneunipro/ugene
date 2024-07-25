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

#include "KeycloakAuthenticator.h"

#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkReply>
#include <QOAuthHttpServerReplyHandler>
#include <QUrl>
#include <QUrlQuery>

KeycloakAuthenticator::KeycloakAuthenticator(const QString& _authUrl, const QString& _tokenUrl, const QString& _clientId, QObject* parent)
    : QObject(parent), authUrl(_authUrl), tokenUrl(_tokenUrl), clientId(_clientId) {
    oauth2.setAuthorizationUrl(QUrl(authUrl));
    oauth2.setAccessTokenUrl(QUrl(tokenUrl));
    oauth2.setClientIdentifier(clientId);
    oauth2.setScope("openid email profile");

    auto replyHandler = new QOAuthHttpServerReplyHandler(8848, this);
    oauth2.setReplyHandler(replyHandler);

    connect(&oauth2, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, &QDesktopServices::openUrl);

    connect(&oauth2, &QOAuth2AuthorizationCodeFlow::granted, this, [this] {
        qDebug() << "KeycloakAuthenticator: authentication granted";
        QString accessToken = oauth2.token();
        QString refreshToken = oauth2.refreshToken();
        emit si_authenticationGranted(accessToken, refreshToken);
        deleteLater();
    });

    connect(&oauth2, &QOAuth2AuthorizationCodeFlow::error, this, [this](const QString& error, const QString& errorDescription, const QUrl& uri) {
        qDebug() << "KeycloakAuthenticator: authentication failed: " << error << ", description: " << errorDescription << ", uri: " << uri.toString();
        bool isRetriable = false;
        emit si_authenticationFailed(error, isRetriable);
        deleteLater();
    });
}

void KeycloakAuthenticator::startAuthentication() {
    oauth2.grant();
}

void KeycloakAuthenticator::refreshAccessToken(const QString& refreshToken) {
    qDebug() << "KeycloakAuthenticator: Renew access token, refresh: " << refreshToken;
    QUrl tokenUrl(this->tokenUrl);
    QNetworkRequest request(tokenUrl);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QUrlQuery params;
    params.addQueryItem("client_id", clientId);
    params.addQueryItem("grant_type", "refresh_token");
    params.addQueryItem("refresh_token", refreshToken);

    const auto networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished, this, [this](QNetworkReply* reply) {
        if (reply->error() == QNetworkReply::NoError) {
            qDebug() << "KeycloakAuthenticator: Request finished with no errors";

            QByteArray response = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
            QJsonObject jsonObj = jsonDoc.object();

            QString newAccessToken = jsonObj["access_token"].toString();
            QString newRefreshToken = jsonObj["refresh_token"].toString();

            oauth2.setToken(newAccessToken);
            emit si_authenticationGranted(newAccessToken, newRefreshToken);
        } else {
            qDebug() << "KeycloakAuthenticator: Request finished with error: " << reply->errorString() << ", code: " << reply->error();
            static QSet<QNetworkReply::NetworkError> nonRetriableErrors = {
                QNetworkReply::ContentAccessDenied,
                QNetworkReply::ContentOperationNotPermittedError,
                QNetworkReply::ContentNotFoundError,
                QNetworkReply::AuthenticationRequiredError,
                QNetworkReply::ProtocolUnknownError,
                QNetworkReply::ProtocolFailure,
                QNetworkReply::ProtocolInvalidOperationError,
            };
            auto error = reply->error();
            bool isRetriable = !nonRetriableErrors.contains(error);
            emit si_authenticationFailed(reply->errorString(), isRetriable);
        }
        reply->deleteLater();
        deleteLater();
    });
    networkManager->post(request, params.toString(QUrl::FullyEncoded).toUtf8());
}
