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

#if (QT_VERSION < 0x050500)
#include <QWebChannelAbstractTransport>
#include <QWebSocketServer>

#include <U2Gui/WebSocketClientWrapper.h>
#include <U2Gui/WebSocketTransport.h>
#endif

#include <QWebEnginePage>

#include "U2WebChannel.h"

namespace U2 {

const int U2WebChannel::INVALID_PORT = -1;

U2WebChannel::U2WebChannel(QWebEnginePage *page)
    : QObject(page),
      channel(new QWebChannel(this)),
      port(INVALID_PORT)
{
#if (QT_VERSION < 0x050500)
    QWebSocketServer *server = new QWebSocketServer(QStringLiteral("UGENE Standalone Server"), QWebSocketServer::NonSecureMode, this);
    port = 12346;
    while (!server->listen(QHostAddress::LocalHost, port)) { //TODO: need more useful solution
        port++;
    }

    WebSocketClientWrapper *clientWrapper = new WebSocketClientWrapper(server, this);
    connect(clientWrapper, &WebSocketClientWrapper::clientConnected, channel, &QWebChannel::connectTo);
#else
    page->setWebChannel(channel);
#endif
}

void U2WebChannel::registerObject(const QString &id, QObject *object) {
    channel->registerObject(id, object);
}

int U2WebChannel::getPort() const {
    return port;
}

}   // namespace U2
