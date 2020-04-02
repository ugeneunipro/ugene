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
namespace {

bool compare(QString s1, QString s2, bool exactMatch){
    if(exactMatch){
        return s1==s2;
    }else{
        return s1.contains(s2);
    }
}
}

#define GT_CLASS_NAME "GTWebkitView"

#define GT_METHOD_NAME "findElement"
HIWebElement GTWebkitView::findElement(GUITestOpStatus &os, QWebView *view, const QString &text, const QString &tag, bool exactMatch) {
    foreach (const HIWebElement &element, findElementsBySelector(os, view, tag, GTGlobals::FindOptions())) {
        if (compare(element.toPlainText(), text, exactMatch)) {
            return toHiWebElement(element);
        }
    }

    GT_CHECK_RESULT(false, QString("element with text '%1' and tag '%2' not found").arg(text).arg(tag), HIWebElement());
    return HIWebElement();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findElementById"
HIWebElement GTWebkitView::findElementById(GUITestOpStatus &os, QWebView *view, const QString &id, const QString &tag) {
    const QString selector = id.isEmpty() ? tag : QString("%1 [id='%2']").arg(tag).arg(id);
    return findElementBySelector(os, view, selector, GTGlobals::FindOptions());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findElementsById"
QList<HIWebElement> GTWebkitView::findElementsById(GUITestOpStatus &os, QWebView *view, const QString &id, const QString &tag, const HIWebElement &parentElement) {
    const QString parentQuery = parentElement.tagName().isEmpty() ? "" : parentElement.tagName() + (id.isEmpty() ? ""  : "[id=" + parentElement.id() + "]") + " ";
    const QString elementQuery = tag + (id.isEmpty() ? ""  : "[id=" + id + "]");
    return findElementsBySelector(os, view, parentQuery + elementQuery, GTGlobals::FindOptions());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findElementBySelector"
HIWebElement GTWebkitView::findElementBySelector(GUITestOpStatus &os, QWebView *view, const QString &selector, const GTGlobals::FindOptions &options) {
    class Scenario : public CustomScenario {
    public:
        Scenario(QWebView *view, const QString &selector, const GTGlobals::FindOptions &options, HIWebElement &webElement) :
            view(view),
            selector(selector),
            options(options),
            webElement(webElement) {}

        void run(GUITestOpStatus &os) {
            Q_UNUSED(os)
            QWebFrame* frame = view->page()->mainFrame();
            foreach (const QWebElement &el, frame->findAllElements(selector)) {
                const int width = el.geometry().width();

                if (options.searchInHidden || width != 0) {
                    webElement = toHiWebElement(el);
                    return;
                }
            }

            if (options.failIfNotFound) {
                GT_CHECK(false, QString("There are no elements that match selector '%1'").arg(selector));
            }
        }

    private:
        QWebView *view;
        const QString selector;
        const GTGlobals::FindOptions options;
        HIWebElement &webElement;
    };

    HIWebElement webElement;
    MainThreadRunnable mainThreadRunnable(os, new Scenario(view, selector, options, webElement));
    mainThreadRunnable.doRequest();
    return webElement;
}
#undef GT_METHOD_NAME

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
                const int width = el.geometry().width();

                if (options.searchInHidden || width != 0) {
                    webElements << toHiWebElement(el);
                }
            }

            if (webElements.isEmpty() && options.failIfNotFound) {
                GT_CHECK(!webElements.isEmpty(), QString("There are no elements that match selector '%1'").arg(selector));
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
    return webElements;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkElement"
void GTWebkitView::checkElement(GUITestOpStatus &os, QWebView *view, QString text, QString tag, bool exists, bool exactMatch){
    const bool found = doesElementExist(os, view, text, tag, exactMatch);
    if (exists) {
        GT_CHECK(found, "element with text " + text + " and tag " + tag + " not found");
    } else {
        GT_CHECK(!found, "element with text " + text + " and tag " + tag + " unexpectedly found");
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "doesElementExist"
bool GTWebkitView::doesElementExist(GUITestOpStatus &os, QWebView *view, const QString &text, const QString &tag, bool exactMatch) {
    class Scenario : public CustomScenario {
    public:
        Scenario(QWebView *view, const QString &text, const QString &tag, bool exactMatch, bool &webElementExists) :
            view(view),
            text(text),
            tag(tag),
            exactMatch(exactMatch),
            webElementExists(webElementExists) {}

        void run(GUITestOpStatus &os) {
            Q_UNUSED(os);
            webElementExists = false;
            QWebFrame* frame = view->page()->mainFrame();
            foreach (QWebElement el, frame->findAllElements(tag)) {
                QString s = el.toPlainText();
                int width = el.geometry().width();

                if (compare(s, text, exactMatch) && width != 0) {
                    webElementExists = true;
                    return;
                }
            }
        }

    private:
        QWebView *view;
        const QString text;
        const QString tag;
        bool exactMatch;
        bool &webElementExists;
    };

    bool webElementExists;
    MainThreadRunnable mainThreadRunnable(os, new Scenario(view, text, tag, exactMatch, webElementExists));
    mainThreadRunnable.doRequest();
    return webElementExists;
}
#undef GT_METHOD_NAME

HIWebElement GTWebkitView::findTreeElement(GUITestOpStatus &os, QWebView *view, QString text){
    return findElement(os, view, text, "SPAN");
}

HIWebElement GTWebkitView::findContextMenuElement(GUITestOpStatus &os, QWebView *view, QString text){
    return findElement(os, view, text, "LI");
}

void GTWebkitView::traceAllWebElements(GUITestOpStatus &os, QWebView *view){
    class Scenario : public CustomScenario {
    public:
        Scenario(QWebView *view) :
            view(view) {}

        void run(GUITestOpStatus &os) {
            Q_UNUSED(os);
            QWebFrame* frame = view->page()->mainFrame();
            foreach (QWebElement el, frame->findAllElements("*")) {
                QString s = el.toPlainText();
                QString tagName = el.tagName();
                QString localName = el.localName();

                if (el.geometry().width() != 0){
                    qDebug("GT_DEBUG_MESSAGE tag: %s name: %s text: %s width: %d", tagName.toLocal8Bit().constData(), localName.toLocal8Bit().constData(), s.toLocal8Bit().constData(), el.geometry().width());
                }
            }
        }

    private:
        QWebView *view;
    };

    MainThreadRunnable mainThreadRunnable(os, new Scenario(view));
    mainThreadRunnable.doRequest();
}

HIWebElement GTWebkitView::toHiWebEleent(const QWebElement &element) {
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
