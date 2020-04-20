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

#include <U2Core/U2SafePoints.h>

#include "WebViewQtWebEngineControllerPrivate.h"
#include "util/webview/JavaScriptAgent.h"
#include "util/webview/qtwebengine/webchannel/U2WebChannel.h"

namespace U2 {

WebViewWriter::WebViewWriter(const QString &_pageUrl)
    : pageUrl(_pageUrl)
{

}

void WebViewWriter::write(const QString &data) {
    CHECK(!data.isEmpty(), );
    WebViewControllerPrivate::saveContent(pageUrl, data);
    deleteLater();
}

WebViewQtWebEngineControllerPrivate::WebViewQtWebEngineControllerPrivate(U2WebView *webView)
    : WebViewControllerPrivate(webView),
      channel(NULL)
{

}

void WebViewQtWebEngineControllerPrivate::loadPage(const QString &pageUrl) {
    U2WebPage *page = new U2WebPage(webView.data());
    connect(page, SIGNAL(si_linkClicked(const QUrl &)), SLOT(sl_linkClicked(const QUrl &)));
    webView->setPage(page);
    page->load(pageUrl);

    channel = new U2WebChannel(page);
}

void WebViewQtWebEngineControllerPrivate::savePage(const QString &pageUrl) {
    WebViewWriter *writer = new WebViewWriter(pageUrl);
    webView->page()->toHtml([writer](const QString &result) mutable {writer->write(result);});
}

void WebViewQtWebEngineControllerPrivate::registerJavaScriptAgent(JavaScriptAgent *agent) {
    channel->registerObject(agent->getId(), agent);
}

void WebViewQtWebEngineControllerPrivate::runJavaScript(const QString &script) {
    webView->page()->runJavaScript(script);
}

void WebViewQtWebEngineControllerPrivate::runJavaScript(const QString &script, WebViewCallback callback) {
    webView->page()->runJavaScript(script, callback);
}

void WebViewQtWebEngineControllerPrivate::init() {
    SAFE_POINT(NULL != channel, "U2WebChannel is NULL", );
    const int port = channel->getPort();
    const QString onSocketsArgument = (U2WebChannel::INVALID_PORT == port ? "false" : "true");
    const QString portArgument = (U2WebChannel::INVALID_PORT == port ? "" : "," + QString::number(port));
    runJavaScript("installWebChannel(" + onSocketsArgument + portArgument + ")");
    webView->setContextMenuPolicy(Qt::NoContextMenu);
}

}   // namespace U2
