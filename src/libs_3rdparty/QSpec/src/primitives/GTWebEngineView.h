/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2017 UniPro <ugene@unipro.ru>
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

#ifndef GTWEBENGINEVIEW_H
#define GTWEBENGINEVIEW_H

#include "GTGlobals.h"
#include "GTWebView.h"

#include <QEventLoop>
#include <QWebEngineView>

namespace HI {

class SynchronizedCall : public QObject {
    Q_OBJECT
public:
    static void run(QWebEngineView *view, const QString &scriptSource, const std::function<void(QVariant)> &resultCallback);

signals:
    void callbackFinished();

private:
    QEventLoop eventLoop;
};

class HI_EXPORT GTWebEngineView {
public:
    static QList<HIWebElement> findElementsBySelector(GUITestOpStatus &os, QWebEngineView *view, const QString &selector, const GTGlobals::FindOptions &options = GTGlobals::FindOptions());

private:
    static HIWebElement toHiWebElement(const QMap<QString, QVariant> &map);

    static const QString X;
    static const QString Y;
    static const QString WIDTH;
    static const QString HEIGHT;
    static const QString TEXT;
    static const QString XML;
    static const QString TAG;
    static const QString ID;
    static const QString ATTRIBUTES_MAP;
    static const QString VISIBLE;
};

typedef class GTWebEngineView GTWebViewPrivate;

}

#endif // GTWEBENGINEVIEW_H
