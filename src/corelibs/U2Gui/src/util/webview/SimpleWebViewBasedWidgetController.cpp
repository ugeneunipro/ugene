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

#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/WebViewController.h>

#include "SimpleWebViewBasedWidgetController.h"

namespace U2 {

SimpleWebViewBasedWidgetController::SimpleWebViewBasedWidgetController(U2WebView *webView, JavaScriptAgent *_agent)
    : QObject(webView),
      agent(_agent),
      webViewController(new WebViewController(webView, agent)),
      pageReady(false)
{
    connect(webViewController, SIGNAL(si_pageIsAboutToBeInitialized()), SLOT(sl_pageIsAboutToBeInitialized()));
    connect(webViewController, SIGNAL(si_pageReady()), SLOT(sl_pageInitialized()));
}

void SimpleWebViewBasedWidgetController::loadPage(const QString &pageUrl) {
    webViewController->loadPage(pageUrl);
}

void SimpleWebViewBasedWidgetController::savePage(const QString &pageUrl) {
    webViewController->savePage(pageUrl);
}

bool SimpleWebViewBasedWidgetController::isPageReady() const {
    return pageReady;
}

void SimpleWebViewBasedWidgetController::runJavaScript(const QString &script) {
    webViewController->runJavaScript(script);
}

void SimpleWebViewBasedWidgetController::runJavaScript(const QString &script, WebViewCallback callback) {
    webViewController->runJavaScript(script, callback);
}

void SimpleWebViewBasedWidgetController::sl_pageIsAboutToBeInitialized() {
    // Do nothing by default
}

void SimpleWebViewBasedWidgetController::sl_pageInitialized() {
    pageReady = true;
    emit si_pageReady();
}

}   // namespace U2
