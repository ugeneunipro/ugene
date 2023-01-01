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

#ifndef __SYNC_HTTP_H__
#define __SYNC_HTTP_H__

#include <QAuthenticator>
#include <QBasicTimer>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QTimerEvent>

#include <U2Core/U2OpStatus.h>

namespace U2 {

class U2CORE_EXPORT SyncHttp : public QNetworkAccessManager {
    Q_OBJECT
public:
    SyncHttp(U2OpStatus& os, QObject* parent = nullptr);
    ~SyncHttp();

    /**
     * WARNING:
     * The method creates new event loop that will block the current one until request is finished.
     * Consider a better approach before start using this method.
     */
    QString syncGet(const QUrl& url, int timeoutMillis);

    QNetworkReply::NetworkError error() const {
        return err;
    }
    QString errorString() const {
        return errString;
    }
protected slots:
    virtual void finished(QNetworkReply*);
    virtual void onProxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*);
    void sl_taskCancellingCheck();

private:
    void runStateCheckTimer();

    QEventLoop* loop;
    QNetworkReply::NetworkError err;
    QString errString;
    U2OpStatus& os;
};

class ReplyTimeout : public QObject {
    Q_OBJECT
public:
    ReplyTimeout(QNetworkReply* reply, int timeoutMillis);

    static void set(QNetworkReply* reply, int timeoutMillis);

protected:
    void timerEvent(QTimerEvent* timerEvent);

private:
    QBasicTimer timer;
};

}  // namespace U2

#endif
