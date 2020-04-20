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

#include "WebViewController.h"
#include "WebViewControllerPrivate.h"

#ifdef UGENE_WEB_KIT
#include "webkit/WebViewWebKitControllerPrivate.h"
# else
#include "qtwebengine/WebViewQtWebEngineControllerPrivate.h"
#endif

namespace U2 {

WebViewController::WebViewController(U2WebView *_webView, JavaScriptAgent *_agent)
    : QObject(_webView),
      agent(_agent),
      pageLoaded(false),
      pageInitialized(false)
{
#ifdef UGENE_WEB_KIT
    controllerPrivate = new WebViewWebKitControllerPrivate(_webView);
# else
    controllerPrivate = new WebViewQtWebEngineControllerPrivate(_webView);
#endif
    connect(_webView, SIGNAL(loadFinished(bool)), SLOT(sl_pageLoaded(bool)));

    if (Q_NULLPTR == agent) {
        agent = new JavaScriptAgent(this);
    }
    connect(agent, SIGNAL(si_pageInitialized()), SLOT(sl_pageInitialized()));
}

WebViewController::~WebViewController() {
    delete controllerPrivate;
}

void WebViewController::loadPage(const QString &_pageUrl) {
    pageUrl = _pageUrl;
    controllerPrivate->loadPage(pageUrl);
    controllerPrivate->registerJavaScriptAgent(agent);
}

void WebViewController::savePage(const QString &_pageUrl) {
    pageUrl = _pageUrl;
    controllerPrivate->savePage(pageUrl);
}

void WebViewController::setPageUrl(const QString &newPageUrl) {
    pageUrl = newPageUrl;
}

void WebViewController::runJavaScript(const QString &script) {
    controllerPrivate->runJavaScript(script);
}

void WebViewController::runJavaScript(const QString &script, WebViewCallback callback) {
    controllerPrivate->runJavaScript(script, callback);
}

void WebViewController::sl_pageLoaded(bool ok) {
    SAFE_POINT(!pageLoaded, "Page was loaded twice", );
    disconnect(parent(), NULL, this, SLOT(sl_pageLoaded(bool)));
    CHECK(ok, );
    pageLoaded = true;
    SAFE_POINT(!pageInitialized, "Page was initialized before it was loaded", );
    emit si_pageIsAboutToBeInitialized();
    controllerPrivate->init();
}

void WebViewController::sl_pageInitialized() {
    pageInitialized = true;
    emit si_pageReady();
}

}   // namespace U2
