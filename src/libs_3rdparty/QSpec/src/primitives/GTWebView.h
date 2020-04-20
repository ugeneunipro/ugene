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

#ifndef GTWEBVIEW_H
#define GTWEBVIEW_H

#ifdef UGENE_WEB_KIT
#include <QWebPage>
#include <QWebView>
#else
#include <QWebEnginePage>
#include <QWebEngineView>
#endif

#include "GTGlobals.h"

#ifdef UGENE_WEB_KIT
typedef class QWebView WebView;
typedef class QWebPage WebPage;
#else
typedef class QWebEngineView WebView;
typedef class QWebEnginePage WebPage;
#endif

namespace HI {

class HI_EXPORT HIWebElement{
public:
    HIWebElement();
    HIWebElement(const QRect &rect,
                 const QString &text,
                 const QString &xml,
                 const QString &tag,
                 const QString &idAttribute,
                 const QMap<QString, QString> &attributesMap,
                 const bool isVisible);

    const QRect &geometry() const;
    const QString &toInnerXml() const;
    const QString &toPlainText() const;
    const QString &tagName() const;
    const QString &id() const;
    const QMap<QString, QString> &attributes() const;
    QString attribute(const QString &name, const QString &defaultValue = QString()) const;
    bool isVisible() const;

private:
    QRect rect;
    QString text;
    QString xml;
    QString tag;
    QString idAttribute;
    QMap<QString, QString> attributesMap;
    bool visible;
};

class HI_EXPORT GTWebView {
public:
    static HIWebElement findElement(GUITestOpStatus &os, WebView *view, const QString &text, const QString &tag = "*", bool exactMatch = false);
    static HIWebElement findElementById(GUITestOpStatus &os, WebView *view, const QString &id, const QString &tag = "*");
    static QList<HIWebElement> findElementsById(GUITestOpStatus &os, WebView *view, const QString &id, const QString &tag = "*", const HIWebElement &parentElement = HIWebElement());

    // Selectors: https://www.w3.org/TR/selectors/
    static HIWebElement findElementBySelector(GUITestOpStatus &os, WebView *view, const QString &selector, const GTGlobals::FindOptions &options = GTGlobals::FindOptions());
    static QList<HIWebElement> findElementsBySelector(GUITestOpStatus &os, WebView *view, const QString &selector, const GTGlobals::FindOptions &options = GTGlobals::FindOptions());

    static void checkElement(GUITestOpStatus &os, WebView *view, QString text, QString tag = "*", bool exists = true, bool exactMatch = false);
    static bool doesElementExist(GUITestOpStatus &os, WebView *view, const QString &text, const QString &tag = "*", bool exactMatch = false);

    static void click(GUITestOpStatus &os, WebView *view, HIWebElement el, Qt::MouseButton button = Qt::LeftButton);
    static void selectElementText(GUITestOpStatus &os, WebView *view, HIWebElement el);
    static HIWebElement findTreeElement(GUITestOpStatus &os, WebView *view, QString text);
    static HIWebElement findContextMenuElement(GUITestOpStatus &os, WebView *view, QString text);

    static void traceAllWebElements(GUITestOpStatus &os, WebView *view);
};

}

#endif // GTWEBVIEW_H
