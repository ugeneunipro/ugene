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

#ifndef _U2_WEB_PAGE_CONTROLLER_H_
#define _U2_WEB_PAGE_CONTROLLER_H_

#include "JavaScriptAgent.h"
#include "U2WebView.h"

namespace U2 {

class WebViewControllerPrivate;

class U2GUI_EXPORT WebViewController : public QObject {
    Q_OBJECT
public:
    WebViewController(U2WebView *webView, JavaScriptAgent *agent = Q_NULLPTR);
    ~WebViewController();

    void loadPage(const QString &pageUrl);
    void savePage(const QString &_pageUrl);

    void setPageUrl(const QString &pageUrl);

    void runJavaScript(const QString &script);
    void runJavaScript(const QString &script, WebViewCallback callback);

signals:
    void si_pageIsAboutToBeInitialized();
    void si_pageReady();

private slots:
    void sl_pageLoaded(bool ok);
    void sl_pageInitialized();

protected:
    JavaScriptAgent *agent;

    QString pageUrl;

    bool pageLoaded;
    bool pageInitialized;

    WebViewControllerPrivate *controllerPrivate;
};

}   // namespace U2

#endif // _U2_WEB_PAGE_CONTROLLER_H_
