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

#ifndef _U2_WEB_VIEW_H_
#define _U2_WEB_VIEW_H_

#include <functional>

#if UGENE_WEB_KIT
#include <QWebView>
#else
#include <QWebEngineView>
#endif

#include <U2Core/global.h>

namespace U2 {

typedef std::function<void(const QVariant &)> WebViewCallback;

#if UGENE_WEB_KIT
typedef class QWebView U2WebView;
typedef class QWebPage U2WebPage;
#else

class U2GUI_EXPORT U2WebPage : public QWebEnginePage {
    Q_OBJECT
public:
    U2WebPage(QObject *parent = Q_NULLPTR) : QWebEnginePage(parent) {

    }

signals:
    void si_linkClicked(const QUrl &url);

private:
#if (QT_VERSION >= 0x050500)
    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool /*isMainFrame*/) {
        if (type == NavigationTypeLinkClicked) {
            emit si_linkClicked(url);
            return false;
        }
        return true;
    }
#else
    bool javaScriptConfirm(const QUrl & /*securityOrigin*/, const QString &msg) { // hack for Qt5.4 only
        emit si_linkClicked(msg);
        return false;
    }
#endif
};

typedef class QWebEngineView U2WebView;
#endif

}   // namespace U2

#endif // _U2_WEB_VIEW_H_
