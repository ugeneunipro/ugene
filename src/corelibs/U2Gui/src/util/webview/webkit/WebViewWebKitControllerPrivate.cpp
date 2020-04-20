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

#include <QWebFrame>

#include "WebViewWebKitControllerPrivate.h"
#include "util/webview/JavaScriptAgent.h"

namespace U2 {

WebViewWebKitControllerPrivate::WebViewWebKitControllerPrivate(U2WebView *webView)
    : WebViewControllerPrivate(webView)
{

}

void WebViewWebKitControllerPrivate::loadPage(const QString &pageUrl) {
    webView->load(pageUrl);
}

void WebViewWebKitControllerPrivate::savePage(const QString &pageUrl) {
    saveContent(pageUrl, webView->page()->mainFrame()->toHtml());
}

void WebViewWebKitControllerPrivate::registerJavaScriptAgent(JavaScriptAgent *agent) {
    webView->page()->mainFrame()->addToJavaScriptWindowObject(agent->getId(), agent);
}

void WebViewWebKitControllerPrivate::runJavaScript(const QString &script) {
    webView->page()->mainFrame()->evaluateJavaScript(script);
}

void WebViewWebKitControllerPrivate::runJavaScript(const QString &script, WebViewCallback callback) {
    const QVariant result = webView->page()->mainFrame()->evaluateJavaScript(script);
    callback(result);
}

void WebViewWebKitControllerPrivate::init() {
    webView->page()->setLinkDelegationPolicy(QWebPage::DelegateExternalLinks);
    connect(webView->page(), SIGNAL(linkClicked(const QUrl &)), SLOT(sl_linkClicked(const QUrl &)));
    runJavaScript("initializeWebkitPage();");
    webView->setContextMenuPolicy(Qt::NoContextMenu);
}

}   // namespace U2
