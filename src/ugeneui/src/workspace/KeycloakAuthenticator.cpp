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
#include <QMessageBox>
#include <QNetworkReply>
#include <QOAuthHttpServerReplyHandler>
#include <QUrl>

KeycloakAuthenticator::KeycloakAuthenticator(const QString& _authUrl, const QString& _tokenUrl, const QString& _clientId, QObject* parent)
    : QObject(parent), authUrl(_authUrl), tokenUrl(_tokenUrl), clientId(_clientId) {
    oauth2.setAuthorizationUrl(QUrl(authUrl));
    oauth2.setAccessTokenUrl(QUrl(tokenUrl));
    oauth2.setClientIdentifier(clientId);
    oauth2.setScope("openid email profile");

    auto replyHandler = new QOAuthHttpServerReplyHandler(8848, this);
    oauth2.setReplyHandler(replyHandler);

    connect(&oauth2, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, &QDesktopServices::openUrl);
    connect(&oauth2, &QOAuth2AuthorizationCodeFlow::granted, this, &KeycloakAuthenticator::handleAuthorizationGranted);
    connect(&oauth2, &QOAuth2AuthorizationCodeFlow::error, this, &KeycloakAuthenticator::handleErrorOccurred);
}

void KeycloakAuthenticator::startAuthentication() {
    oauth2.grant();
}

void KeycloakAuthenticator::handleAuthorizationGranted() {
    QString accessToken = oauth2.token();
    emit si_authenticationGranted(accessToken);
    deleteLater();
}

void KeycloakAuthenticator::handleErrorOccurred(const QString& error) {
    emit si_authenticationFailed(error);
    deleteLater();
}
