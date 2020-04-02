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

#ifndef GTWEBKITVIEW_H
#define GTWEBKITVIEW_H

#include "GTGlobals.h"
#include "GTWebView.h"

#include <QWebView>
#include <QWebElement>

namespace HI {

class HI_EXPORT GTWebkitView {
public:
    static HIWebElement findElement(GUITestOpStatus &os, QWebView *view, const QString &text, const QString &tag = "*", bool exactMatch = false);
    static HIWebElement findElementById(GUITestOpStatus &os, QWebView *view, const QString &id, const QString &tag = "*");
    static QList<HIWebElement> findElementsById(GUITestOpStatus &os, QWebView *view, const QString &id, const QString &tag = "*", const HIWebElement &parentElement = HIWebElement());

    // Selectors: https://www.w3.org/TR/selectors/
    static HIWebElement findElementBySelector(GUITestOpStatus &os, QWebView *view, const QString &selector, const GTGlobals::FindOptions &options = GTGlobals::FindOptions());
    static QList<HIWebElement> findElementsBySelector(GUITestOpStatus &os, QWebView *view, const QString &selector, const GTGlobals::FindOptions &options = GTGlobals::FindOptions());

    static void checkElement(GUITestOpStatus &os, QWebView *view, QString text, QString tag = "*", bool exists = true, bool exactMatch = false);
    static bool doesElementExist(GUITestOpStatus &os, QWebView *view, const QString &text, const QString &tag = "*", bool exactMatch = false);

    static HIWebElement findTreeElement(GUITestOpStatus &os, QWebView *view, QString text);
    static HIWebElement findContextMenuElement(GUITestOpStatus &os, QWebView *view, QString text);

    static void traceAllWebElements(GUITestOpStatus &os, QWebView *view);

private:
    static HIWebElement toHiWebEleent(const QWebElement &element);
};

typedef class GTWebkitView GTWebViewPrivate;


}

#endif // GTWEBKITVIEW_H
