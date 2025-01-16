/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include <QNetworkAccessManager>
#include <QOAuth2AuthorizationCodeFlow>
#include <QObject>

namespace U2 {
/** Authentication support for workspace cloud service. */
class KeycloakAuthenticator : public QObject {
    Q_OBJECT

public:
    KeycloakAuthenticator(const QString& authUrl,
                          const QString& tokenUrl,
                          const QString& clientId,
                          QObject* parent = nullptr);

    /**
     * Triggers authentication UI. A user will be asked about login & password.
     * The result will be reported using signals.
     */
    void startAuthentication();

    /** Refreshes access token using the given refresh token. */
    void refreshAccessToken(const QString& refreshToken);

signals:
    /** Emitted when authentication succeed. */
    void si_authenticationGranted(const QString& accessToken, const QString& refreshToken);

    /**
     * Emitted when authentication failed.
     * If the error is retriable the authentication can be retried with the same credentials later.
     */
    void si_authenticationFailed(const QString& error, bool isRetriable);

private:
    QNetworkAccessManager networkAccessManager;
    QOAuth2AuthorizationCodeFlow oauth2;
    QString authUrl;
    QString tokenUrl;
    QString clientId;
};

}  // namespace U2
