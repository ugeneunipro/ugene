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

#include <QEventLoop>
#include <QNetworkReply>
#include <QNetworkRequest>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/Log.h>
#include <U2Core/NetworkConfiguration.h>

#include <U2Lang/WorkflowScriptEngine.h>

#include "LinkDataScriptLibrary.h"

namespace U2 {

static QString LINKDATA_API_URL = "http://linkdata.org/api/1/%1/%2_rdf.json";
static QString LINKDATA_OBJ_NAME = "LinkData";
static QString LINKDATA_USERDATA_OBJ_NAME = "userdata";

LinkDataRequestHandler::LinkDataRequestHandler(QEventLoop* _eventLoop) : eventLoop(_eventLoop), error("") {
    assert(eventLoop != NULL);
}

bool LinkDataRequestHandler::hasError() const {
    return !error.isEmpty();
}

const QString LinkDataRequestHandler::errorString() const {
    return error;
}

QByteArray LinkDataRequestHandler::getResult() const {
    return result;
}

void LinkDataRequestHandler::sl_onReplyFinished(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
        result.append(reply->readAll());
    } else {
        error = reply->errorString();
    }
    eventLoop->exit();
}

void LinkDataRequestHandler::onProxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *auth){
    auth->setUser(proxy.user());
    auth->setPassword(proxy.password());
    disconnect(this, SLOT(onProxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)));
}


static QString readScript (const QString& filename)
{
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        return stream.readAll();
    }
    return "";
}

void LinkDataScriptLibrary::init(WorkflowScriptEngine* engine) {
    QScriptValue global = engine->globalObject();
    QScriptValue linkData = engine->newObject();
    QScriptValue userdata = engine->newObject();
    userdata.setProperty("fetchFile", engine->newFunction(fetchFile));
    linkData.setProperty(LINKDATA_USERDATA_OBJ_NAME, userdata);
    global.setProperty(LINKDATA_OBJ_NAME, linkData);
    QString script = readScript(":/linkdata_support/src/linkdata.js");
    assert(!script.isEmpty());
    engine->evaluate(script);
    scriptLog.trace("LinkData script registered");
}

QScriptValue LinkDataScriptLibrary::fetchFile(QScriptContext *ctx, QScriptEngine *engine) {
    if(ctx->argumentCount() < 2 && ctx->argumentCount() >3) {
        return ctx->throwError(LinkDataRequestHandler::tr("Incorrect number of arguments"));
    }

    QString workId = ctx->argument(0).toString();
    if(workId.isNull() || workId.isEmpty()) {
        return ctx->throwError(QObject::tr("Invalid argument"));
    }
    QString filename = ctx->argument(1).toString();
    if(filename.isNull() || filename.isEmpty()) {
        return ctx->throwError(QObject::tr("Invalid argument"));
    }

    QNetworkRequest request(LINKDATA_API_URL.arg(workId).arg(filename));
    QNetworkAccessManager networkManager;
    NetworkConfiguration* nc = AppContext::getAppSettings()->getNetworkConfiguration();
    QNetworkProxy proxy = nc->getProxyByUrl(LINKDATA_API_URL.arg(workId).arg(filename));
    networkManager.setProxy(proxy);

    QEventLoop eventLoop;
    LinkDataRequestHandler handler(&eventLoop);
    handler.connect(&networkManager, SIGNAL(finished(QNetworkReply*)), SLOT(sl_onReplyFinished(QNetworkReply*)));
    handler.connect(&networkManager, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)), SLOT(onProxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)));
    networkManager.get(request);
    eventLoop.exec();

    if(handler.hasError()) {
        return ctx->throwError(handler.errorString());
    }

    QScriptValue result = engine->evaluate("("+handler.getResult()+")");
    if(result.isObject()) {
        return result;
    } else {
        return ctx->throwError(LinkDataRequestHandler::tr("Invalid response"));
    }

    return result;
}


} //U2

