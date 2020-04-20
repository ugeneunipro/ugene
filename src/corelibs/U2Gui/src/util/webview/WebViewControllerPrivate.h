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

#ifndef _U2_WEB_VIEW_CONTROLLER_PRIVATE_H_
#define _U2_WEB_VIEW_CONTROLLER_PRIVATE_H_

#include <QPointer>

#include "util/webview/U2WebView.h"

namespace U2 {

class JavaScriptAgent;

class WebViewControllerPrivate : public QObject {
    Q_OBJECT
public:
    WebViewControllerPrivate(U2WebView *webView);

    virtual void init() = 0;

    virtual void loadPage(const QString &pageUrl) = 0;
    virtual void savePage(const QString &pageUrl) = 0;

    virtual void registerJavaScriptAgent(JavaScriptAgent *agent) = 0;
    virtual void runJavaScript(const QString &script) = 0;
    virtual void runJavaScript(const QString &script, WebViewCallback callback) = 0;

    static void saveContent(const QString &url, const QString &data);

private slots:
    void sl_linkClicked(const QUrl &url);

protected:
    QPointer<U2WebView> webView;
};

}   // namespace U2

#endif // _U2_WEB_VIEW_CONTROLLER_PRIVATE_H_
