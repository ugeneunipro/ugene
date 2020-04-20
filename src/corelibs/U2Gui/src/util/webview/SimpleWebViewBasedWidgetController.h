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

#ifndef _U2_SIMPLE_WEB_VIEW_BASED_WIDGET_CONTROLLER_H_
#define _U2_SIMPLE_WEB_VIEW_BASED_WIDGET_CONTROLLER_H_

#include "U2WebView.h"

namespace U2 {

class JavaScriptAgent;
class WebViewController;

class U2GUI_EXPORT SimpleWebViewBasedWidgetController : public QObject {
    Q_OBJECT
public:
    SimpleWebViewBasedWidgetController(U2WebView *webView, JavaScriptAgent *agent = Q_NULLPTR);

    void loadPage(const QString &pageUrl);
    void savePage(const QString &pageUrl);
    bool isPageReady() const;

    void runJavaScript(const QString &script);
    void runJavaScript(const QString &script, WebViewCallback callback);

signals:
    void si_pageReady();

protected slots:
    virtual void sl_pageIsAboutToBeInitialized();
    virtual void sl_pageInitialized();

protected:
    JavaScriptAgent *agent;
    WebViewController *webViewController;

private:
    bool pageReady;
};

}   // namespace U2

#endif // _U2_SIMPLE_WEB_VIEW_BASED_WIDGET_CONTROLLER_H_
