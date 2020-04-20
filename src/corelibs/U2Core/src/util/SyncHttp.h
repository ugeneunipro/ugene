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

#ifndef __SYNC_HTTP_H__
#define __SYNC_HTTP_H__

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QAuthenticator>
#include <QEventLoop>

#include <U2Core/U2OpStatus.h>

namespace U2 {

class U2CORE_EXPORT SyncHttp : public QNetworkAccessManager {
    Q_OBJECT
public:
    SyncHttp(U2OpStatus &os, QObject* parent = 0);
    ~SyncHttp();
    QString syncGet(const QUrl& url);
    QString syncPost(const QUrl & url, QIODevice * data);
    QNetworkReply::NetworkError error() { return err; }
    QString errorString() { return errString; }
    protected slots:
    virtual void finished(QNetworkReply*);
    virtual void onProxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*);
    void sl_taskCancellingCheck();

private:
    void    runTimer();

    QEventLoop* loop;
    QNetworkReply::NetworkError err;
    QString errString;
    U2OpStatus &os;
};

} // U2

#endif
