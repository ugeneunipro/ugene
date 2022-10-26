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

#ifndef _U2_GENECUT_HTTP_FILE_ADAPTER_H_
#define _U2_GENECUT_HTTP_FILE_ADAPTER_H_

#include <U2Core/HttpFileAdapter.h>
#include <U2Core/U2IdTypes.h>

#include <QNetworkRequest>

namespace U2 {

class GenecutHttpFileAdapterFactory;

/**
* @class GenecutHttpFileAdapter
* @brief An object to controll HTTP requests from the GeneCut plugin
*/
class GenecutHttpFileAdapter : public HttpFileAdapter {
    Q_OBJECT
public:
    GenecutHttpFileAdapter(GenecutHttpFileAdapterFactory* factory, QObject* parent = nullptr);

    bool open(const GUrl& urlString, IOAdapterMode m = IOAdapterMode::IOAdapterMode_Read) override;

    void addHeader(QNetworkRequest::KnownHeaders key, const QVariant& value);
    void addRawHeader(const QByteArray& key, const QByteArray& value);
    void addDataValue(const QString& key, const QString& value);

    enum class RequestType {
        Undefined,
        Post,
        Get
    };
    void setRequestType(RequestType type);

signals:
    void si_done();

private:
    RequestType requestType = RequestType::Undefined;
    QMap<QNetworkRequest::KnownHeaders, QVariant> headers;
    QMap<QByteArray, QByteArray> rawHeaders;
    QMap<QString, QString> replyData;

private slots:
    void sl_finished();

};

}

#endif
