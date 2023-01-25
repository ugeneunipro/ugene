/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "util/SyncHttp.h"

#include <QNetworkRequest>
#include <QTimer>

#include <U2Core/U2SafePoints.h>

namespace U2 {

SyncHttp::SyncHttp(U2OpStatus& os, QObject* parent)
    : QNetworkAccessManager(parent), loop(nullptr), errString(""),
      os(os) {
    connect(this, SIGNAL(finished(QNetworkReply*)), SLOT(finished(QNetworkReply*)));
}

QString SyncHttp::syncGet(const QUrl& url, int timeOutMillis) {
    connect(this, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)), this, SLOT(onProxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)));
    QNetworkRequest request(url);
    QNetworkReply* reply = get(request);
    SAFE_POINT(reply != nullptr, "SyncHttp::syncGet no reply is created", "");
    ReplyTimeout::set(reply, timeOutMillis);
    runStateCheckTimer();
    if (loop == nullptr) {
        loop = new QEventLoop();
    }
    CHECK_OP(os, QString());
    loop->exec();
    err = reply->error();
    errString = reply->errorString();
    return QString(reply->readAll());
}

void SyncHttp::finished(QNetworkReply*) {
    SAFE_POINT(loop != nullptr, "SyncHttp::finished no event loop", );
    loop->exit();
}

void SyncHttp::onProxyAuthenticationRequired(const QNetworkProxy& proxy, QAuthenticator* auth) {
    auth->setUser(proxy.user());
    auth->setPassword(proxy.password());
    disconnect(this, SLOT(onProxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)));
}

SyncHttp::~SyncHttp() {
    delete loop;
    loop = nullptr;
}
void SyncHttp::runStateCheckTimer() {
    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(sl_taskCancellingCheck()));
    timer->start(500);
}

void SyncHttp::sl_taskCancellingCheck() {
    if (loop != nullptr && os.isCanceled()) {
        loop->exit();
    }
}

ReplyTimeout::ReplyTimeout(QNetworkReply* reply, const int timeoutMillis)
    : QObject(reply) {
    if (reply != nullptr && reply->isRunning()) {
        timer.start(timeoutMillis, this);
    }
}
void ReplyTimeout::set(QNetworkReply* reply, const int timeoutMillis) {
    new ReplyTimeout(reply, timeoutMillis);
}

void ReplyTimeout::timerEvent(QTimerEvent* timerEvent) {
    if (!timer.isActive() || timerEvent->timerId() != timer.timerId()) {
        return;
    }
    auto reply = static_cast<QNetworkReply*>(parent());
    if (reply->isRunning()) {
        reply->close();
    }
    timer.stop();
}

}  // namespace U2
