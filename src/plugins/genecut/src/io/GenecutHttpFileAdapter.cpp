/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#include "GenecutHttpFileAdapter.h"
#include "GenecutHttpFileAdapterFactory.h"

#include <U2Core/U2SafePoints.h>
#include <U2Core/Log.h>

#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace U2 {

GenecutHttpFileAdapter::GenecutHttpFileAdapter(GenecutHttpFileAdapterFactory* factory, QObject* parent)
    : HttpFileAdapter(factory, parent) {

}

bool GenecutHttpFileAdapter::open(const GUrl& urlString, IOAdapterMode) {
    SAFE_POINT(!isOpen(), "Adapter is already opened!", false);

    if (reply) {
        close();
    }

    const QUrl url(urlString.getURLString());
    QNetworkRequest request(url);
    {
        auto headerKeys = headers.keys();
        for (auto header : qAsConst(headerKeys)) {
            request.setHeader(header, headers.value(header));
        }
    }
    {
        auto rawHeaderKeys = rawHeaders.keys();
        for (auto rawHeader : qAsConst(rawHeaderKeys)) {
            request.setRawHeader(rawHeader, rawHeaders.value(rawHeader));
        }
    }

    QJsonObject obj;
    {
        auto replyDataKeys = replyData.keys();
        for (auto replyDataKey : qAsConst(replyDataKeys)) {
            obj[replyDataKey] = replyData.value(replyDataKey);
        }
    }

    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();
    switch (requestType) {
    case RequestType::Post:
        reply = netManager->post(request, data);
        break;
    case RequestType::Get:
        reply = netManager->sendCustomRequest(request, "GET", data);
        break;
    default:
        FAIL("Define request type", false);
        break;
    }

    coreLog.details(tr("Getting data from %1").arg(reply->url().toString()));
    connect(reply, SIGNAL(readyRead()), this, SLOT(add_data()), Qt::DirectConnection);
    connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(progress(qint64, qint64)), Qt::DirectConnection);
    connect(reply, SIGNAL(finished()), this, SLOT(sl_finished()), Qt::DirectConnection);

    return true;
}

void GenecutHttpFileAdapter::addHeader(QNetworkRequest::KnownHeaders key, const QVariant& value) {
    headers.insert(key, value);
}

void GenecutHttpFileAdapter::addRawHeader(const QByteArray& key, const QByteArray& value) {
    rawHeaders.insert(key, value);
}

void GenecutHttpFileAdapter::addDataValue(const QString& key, const QString& value) {
    replyData.insert(key, value);
}

void GenecutHttpFileAdapter::setRequestType(RequestType type) {
    requestType = type;
}

void GenecutHttpFileAdapter::sl_finished() {
    is_downloaded = true;
    loop.exit();
    requestType = RequestType::Undefined;
    headers.clear();
    rawHeaders.clear();
    replyData.clear();

    emit si_done();

    close();
    Empty();
}

}