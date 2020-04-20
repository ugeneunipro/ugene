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

#include <QDebug>
#include <QWebFrame>

#include "GTWebkitView.h"
#include "core/MainThreadRunnable.h"
#include "utils/GTThread.h"

namespace HI {

#define GT_CLASS_NAME "GTWebkitView"

#define GT_METHOD_NAME "findElementsBySelector"
QList<HIWebElement> GTWebkitView::findElementsBySelector(GUITestOpStatus &os, QWebView *view, const QString &selector, const GTGlobals::FindOptions &options) {
    class Scenario : public CustomScenario {
    public:
        Scenario(QWebView *view, const QString &selector, const GTGlobals::FindOptions &options, QList<HIWebElement> &webElements) :
            view(view),
            selector(selector),
            options(options),
            webElements(webElements) {}

        void run(GUITestOpStatus &os) {
            Q_UNUSED(os);
            QWebFrame* frame = view->page()->mainFrame();
            foreach (const QWebElement &el, frame->findAllElements(selector)) {
                webElements << toHiWebElement(el);
            }
        }

    private:
        QWebView *view;
        const QString selector;
        const GTGlobals::FindOptions options;
        QList<HIWebElement> &webElements;
    };

    QList<HIWebElement> webElements;
    MainThreadRunnable mainThreadRunnable(os, new Scenario(view, selector, options, webElements));
    mainThreadRunnable.doRequest();

    QList<HIWebElement> filteredElements;
    if (options.searchInHidden) {
        filteredElements = webElements;
    } else{
        foreach(const HIWebElement & element, webElements) {
            if (element.isVisible()) {
                filteredElements << element;
            }
        }
    }

    GT_CHECK_RESULT(!options.failIfNotFound || !filteredElements.isEmpty(), QString("There are no elements that match selector '%1'").arg(selector), QList<HIWebElement>());
    return filteredElements;
}
#undef GT_METHOD_NAME

HIWebElement GTWebkitView::toHiWebElement(const QWebElement &element) {
    QMap<QString, QString> attributesMap;
    foreach (const QString &name, element.attributeNames()) {
        attributesMap.insert(name, element.attribute(name));
    }
    return HIWebElement(element.geometry(),
                        element.toPlainText(),
                        element.toInnerXml(),
                        element.tagName(),
                        element.attribute("id"),
                        attributesMap,
                        element.geometry().isValid());
}

#undef GT_CLASS_NAME

}
