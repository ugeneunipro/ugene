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
#ifndef _U2_LINKDATA_SCRIPT_LIBRARY_H_
#define _U2_LINKDATA_SCRIPT_LIBRARY_H_

#include <QAuthenticator>
#include <QNetworkAccessManager>
#include <QScriptValue>

class QEventLoop;
class QScriptContext;

namespace U2 {

class WorkflowScriptEngine;

class LinkDataScriptLibrary {
public:
    static void init(WorkflowScriptEngine* engine);

    static QScriptValue fetchFile(QScriptContext *ctx, QScriptEngine *engine);

};

class LinkDataRequestHandler : public QObject {
    Q_OBJECT
public:
    LinkDataRequestHandler(QEventLoop* eventLoop);
    bool hasError() const;
    const QString errorString() const;
    QByteArray getResult() const;
private slots:
    void sl_onReplyFinished(QNetworkReply* reply);
    void onProxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*);
private:
    QEventLoop* eventLoop;
    QByteArray result;
    QString error;
};


} //U2
#endif //_U2_LINKDATA_SCRIPT_LIBRARY_H_
