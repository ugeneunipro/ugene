/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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

#include <QUrl>
#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>
#include "NetworkConfiguration.h"

namespace U2
{

static const char * SETTINGS_HTTP_PROXY_HOST = "network_settings/http_proxy/host";
static const char * SETTINGS_HTTP_PROXY_PORT = "network_settings/http_proxy/port";
static const char * SETTINGS_HTTP_PROXY_USER = "network_settings/http_proxy/user";
static const char * SETTINGS_HTTP_PROXY_PASSWORD = "network_settings/http_proxy/password";
static const char * SETTINGS_HTTP_PROXY_ENABLED = "network_settings/http_proxy/enabled";
static const char * SETTINGS_PROXY_EXCEPTED_URLS = "network_settings/proxy_exc_urls";
static const char * SETTINGS_PROXY_EXCEPTED_URLS_ENABLED = "network_settings/proxy_exc_urls_enabled";
static const char * SETTINGS_SSL_PROTOCOL = "network_settings/ssl_protocol";
static const char * SETTINGS_REMOTE_REQUEST_TIMEOUT = "network_settings/remote_request/timeout";

const QString SslConfig::TLSV1 = "TlsV1";
const QString SslConfig::SSLV2 = "SslV2";
const QString SslConfig::SSLV3 = "SslV3";

const int RemoteRequestConfig::DEFAULT_REMOTE_REQUEST_TIMEOUT_SECONDS = 60;
const QString RemoteRequestConfig::HTTP_BODY_SEPARATOR("HTTP_BODY_SEPARATOR");

NetworkConfiguration::NetworkConfiguration() {
    Settings * s = AppContext::getSettings();
    pc.excepted_addr_enabled = s->getValue( SETTINGS_PROXY_EXCEPTED_URLS_ENABLED ).toBool();
    pc.excepted_addr = s->getValue( SETTINGS_PROXY_EXCEPTED_URLS ).toStringList();

    QString httpProxyHost = s->getValue( SETTINGS_HTTP_PROXY_HOST ).toString();
    int httpProxyPort = s->getValue( SETTINGS_HTTP_PROXY_PORT ).toInt();

    if( !httpProxyHost.isEmpty() && httpProxyPort ) {
        QNetworkProxy httpProxy( QNetworkProxy::HttpProxy, httpProxyHost, httpProxyPort );

        QString user = s->getValue( SETTINGS_HTTP_PROXY_USER ).toString();
        if (!user.isEmpty())  {
            QByteArray passwordEncoded = s->getValue( SETTINGS_HTTP_PROXY_PASSWORD ).toByteArray();
            QString passwordDecoded = QByteArray::fromBase64(passwordEncoded);
            httpProxy.setUser(user);
            httpProxy.setPassword(passwordDecoded);
        }

        pc.proxyz[QNetworkProxy::HttpProxy] = httpProxy;
        pc.proxyz_usage[QNetworkProxy::HttpProxy] = s->getValue( SETTINGS_HTTP_PROXY_ENABLED ).toBool();
    }

    sslConfig.currentProtocol = s->getValue(SETTINGS_SSL_PROTOCOL, SslConfig::SSLV3).toString();

    rrConfig.remoteRequestTimeout = s->getValue(SETTINGS_REMOTE_REQUEST_TIMEOUT, RemoteRequestConfig::DEFAULT_REMOTE_REQUEST_TIMEOUT_SECONDS).toInt();

}

NetworkConfiguration::~NetworkConfiguration() {
    Settings * s = AppContext::getSettings();
    s->setValue( SETTINGS_PROXY_EXCEPTED_URLS_ENABLED, pc.excepted_addr_enabled );
    s->setValue( SETTINGS_PROXY_EXCEPTED_URLS, pc.excepted_addr );
    s->setValue( SETTINGS_SSL_PROTOCOL, sslConfig.currentProtocol );
    s->setValue( SETTINGS_REMOTE_REQUEST_TIMEOUT, rrConfig.remoteRequestTimeout );

    QNetworkProxy httpP = getProxy( QNetworkProxy::HttpProxy );

    if( !httpP.hostName().isEmpty() ) {
        s->setValue( SETTINGS_HTTP_PROXY_HOST, httpP.hostName() );
        s->setValue( SETTINGS_HTTP_PROXY_PORT, httpP.port() );
        s->setValue( SETTINGS_HTTP_PROXY_USER, httpP.user());
        s->setValue( SETTINGS_HTTP_PROXY_PASSWORD, httpP.password().toLatin1().toBase64());
        s->setValue( SETTINGS_HTTP_PROXY_ENABLED, isProxyUsed(QNetworkProxy::HttpProxy) );

    }
}

int NetworkConfiguration::addProxy( const QNetworkProxy & p )
{
    int ret = !pc.proxyz.contains( p.type() );
    pc.proxyz.insert( p.type(), p );
    pc.proxyz_usage.insert( p.type(), false ); //needs explicit enabling
    return ret;
}

QNetworkProxy NetworkConfiguration::getProxyByUrl( const QUrl & url ) const
{
    Proxy_t prtype = url2type( url );
    if( pc.proxyz.contains( prtype ) ) {
        assert( pc.proxyz_usage.contains(prtype) );
        if( pc.proxyz_usage[prtype] ) {
            return ( pc.excepted_addr_enabled && pc.excepted_addr.contains( url.toString() ) ?
                QNetworkProxy() : pc.proxyz[prtype] );
        }
    }
    return QNetworkProxy();
}

QNetworkProxy NetworkConfiguration::getProxy( Proxy_t prtype ) const {
    return (pc.proxyz.contains( prtype ) ? pc.proxyz[prtype] : QNetworkProxy() );
}

void NetworkConfiguration::removeProxy( Proxy_t prtype ) {
    pc.proxyz.remove( prtype );
}

bool NetworkConfiguration::isProxyUsed( Proxy_t prtype ) const {
    return pc.proxyz_usage.contains( prtype ) ? pc.proxyz_usage[prtype] : false;
}

void NetworkConfiguration::setExceptionsList( const QStringList & exc_addr ) {
    pc.excepted_addr = exc_addr;
}

void NetworkConfiguration::setProxyUsed( Proxy_t prtype, bool flag ) {
    if( pc.proxyz_usage.contains(prtype) ) {
        pc.proxyz_usage[prtype] = flag;
    }
}

Proxy_t NetworkConfiguration::url2type( const QUrl & url ) {
    if( "http" == url.scheme() || "https" == url.scheme() ) {
        return QNetworkProxy::HttpProxy;
    }
    if( "ftp" == url.scheme() ) {
        return QNetworkProxy::FtpCachingProxy;
    }
    assert( false );
    return QNetworkProxy::NoProxy;
}

void NetworkConfiguration::copyFrom(const NetworkConfiguration& image) {
    pc = image.pc;
    sslConfig = image.sslConfig;
    rrConfig = image.rrConfig;
}

#ifndef QT_NO_OPENSSL

QSsl::SslProtocol NetworkConfiguration::getSslProtocol() const
{
    if (sslConfig.currentProtocol == SslConfig::SSLV2) {
        return QSsl::SslV2;
    } else if (sslConfig.currentProtocol == SslConfig::SSLV3) {
        return QSsl::SslV3;
    } else if (sslConfig.currentProtocol == SslConfig::TLSV1) {
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
        return QSsl::TlsV1_0;
#else
        return QSsl::TlsV1;
#endif
    } else {
        return QSsl::SslV3;
    }
}

#endif

QString NetworkConfiguration::getSslProtocolName() const
{
    if (sslConfig.currentProtocol.isEmpty()) {
        return SslConfig::SSLV3;
    } else {
        return sslConfig.currentProtocol;
    }
}

void NetworkConfiguration::setSslProtocol( const QString& name )
{
    sslConfig.currentProtocol = name;
}

void NetworkConfiguration::setRequestTimeout( const int timeout )
{
    rrConfig.remoteRequestTimeout = timeout;
}


} //namespace
