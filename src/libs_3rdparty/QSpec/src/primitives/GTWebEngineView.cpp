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

#include "GTWebEngineView.h"
#include "core/MainThreadRunnable.h"
#include "utils/GTThread.h"

namespace HI {
namespace {

bool compare(const QString &s1, const QString &s2, bool exactMatch) {
    if (exactMatch) {
        return s1 == s2;
    } else {
        return s1.contains(s2);
    }
}

}

void SynchronizedCall::run(QWebEngineView *view, const QString &scriptSource, const std::function<void(QVariant)> &resultCallback) {
    SynchronizedCall instance;
    connect(&instance, SIGNAL(callbackFinished()), &instance.eventLoop, SLOT(quit()));
    auto innerCallback = [&](const QVariant &v) {
        resultCallback(v);
        emit instance.callbackFinished();
    };
    view->page()->runJavaScript(scriptSource, innerCallback);
    instance.eventLoop.exec();
    int ggg = 0;
}

#define GT_CLASS_NAME "GTWebEngineView"

const QString GTWebEngineView::X = "x";
const QString GTWebEngineView::Y = "y";
const QString GTWebEngineView::WIDTH = "width";
const QString GTWebEngineView::HEIGHT = "height";
const QString GTWebEngineView::TEXT = "text";
const QString GTWebEngineView::XML = "xml";
const QString GTWebEngineView::TAG = "tag";
const QString GTWebEngineView::ID = "id";
const QString GTWebEngineView::ATTRIBUTES_MAP = "attributes_map";
const QString GTWebEngineView::VISIBLE = "visible";

#define GT_METHOD_NAME "findElement"
HIWebElement GTWebEngineView::findElement(GUITestOpStatus &os, QWebEngineView *view, const QString &text, const QString &tag, bool exactMatch) {
    foreach (const HIWebElement &element, findElementsBySelector(os, view, tag, GTGlobals::FindOptions())) {
        if (compare(element.toPlainText(), text, exactMatch)) {
            return element;
        }
    }

    GT_CHECK_RESULT(false, QString("element with text '%1' and tag '%2' not found").arg(text).arg(tag), HIWebElement());
    return HIWebElement();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findElementById"
HIWebElement GTWebEngineView::findElementById(GUITestOpStatus &os, QWebEngineView *view, const QString &id, const QString &tag) {
    const QString selector = id.isEmpty() ? tag : QString("%1 [id='%2']").arg(tag).arg(id);
    return findElementBySelector(os, view, selector, GTGlobals::FindOptions());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findElementsById"
QList<HIWebElement> GTWebEngineView::findElementsById(GUITestOpStatus &os, QWebEngineView *view, const QString &id, const QString &tag, const HIWebElement &parentElement) {
    const QString parentQuery = parentElement.tagName().isEmpty() ? "" : parentElement.tagName() + (id.isEmpty() ? ""  : "[id=" + parentElement.id() + "]") + " ";
    const QString elementQuery = tag + (id.isEmpty() ? ""  : "[id=" + id + "]");
    return findElementsBySelector(os, view, parentQuery + elementQuery, GTGlobals::FindOptions());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findElementBySelector"
HIWebElement GTWebEngineView::findElementBySelector(GUITestOpStatus &os, QWebEngineView *view, const QString &selector, const GTGlobals::FindOptions &options) {
    const QList<HIWebElement> elements = findElementsBySelector(os, view, selector, options);
    GT_CHECK_RESULT(!options.failIfNotFound || !elements.isEmpty(), QString("There are no elements that match selector '%1'").arg(selector), HIWebElement());
    return elements.first();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findElementsBySelector"
QList<HIWebElement> GTWebEngineView::findElementsBySelector(GUITestOpStatus &os, QWebEngineView *view, const QString &selector, const GTGlobals::FindOptions &options) {
    class Scenario : public CustomScenario {
    public:
        Scenario(QWebEngineView *view, const QString &selector, const GTGlobals::FindOptions &options, QList<HIWebElement> &elements)
            : view(view),
              selector(selector),
              options(options),
              elements(elements) {
        }

        void run(GUITestOpStatus& os) {
            Q_UNUSED(os);

            const QString code = QString("var elements = document.querySelectorAll('%1');"
                                         "var result = [];"
                                         "for (var i = 0; i < elements.length; i++) {"
                                         "    result[i] = {};"
                                         "    var rect = elements[i].getBoundingClientRect();"
                                         "    result[i]['%2'] = rect.x;"
                                         "    result[i]['%3'] = rect.y;"
                                         "    result[i]['%4'] = rect.width;"
                                         "    result[i]['%5'] = rect.height;"
                                         "    result[i]['%6'] = elements[i].innerText;"
                                         "    result[i]['%7'] = elements[i].innerHTML;"
                                         "    result[i]['%8'] = elements[i].tagName;"
                                         "    result[i]['%9'] = elements[i].id;"
                                         "    result[i]['%10'] = {};"
                                         "    for (var j = 0; j < elements[i].attributes.length; j++) {"
                                         "        var attribute = elements[i].attributes[j];"
                                         "        result[i]['%10'][attribute.name] = attribute.value;"
                                         "    }"
                                         "    result[i]['%11'] = (elements[i].offsetParent !== null);"
                                         "}" 
                                         "result;")
                                     .arg(selector)
                                     .arg(X)
                                     .arg(Y)
                                     .arg(WIDTH)
                                     .arg(HEIGHT)
                                     .arg(TEXT)
                                     .arg(XML)
                                     .arg(TAG)
                                     .arg(ID)
                                     .arg(ATTRIBUTES_MAP)
                                     .arg(VISIBLE);
            auto callback = [&](const QVariant &v) {
                const QVariantList elementsMaps = v.toList();
                foreach (const QVariant &map, elementsMaps) {
                    elements << toHiWebElement(map.toMap());
                }
            };

            SynchronizedCall::run(view, code, callback);
        }

    private:
        QWebEngineView *view;
        const QString selector;
        const GTGlobals::FindOptions options;
        QList<HIWebElement> &elements;
    };

    QList<HIWebElement> elements;
    MainThreadRunnable mainThreadRunnable(os, new Scenario(view, selector, options, elements));
    mainThreadRunnable.doRequest();

    QList<HIWebElement> filteredElements;
    if (options.searchInHidden) {
        filteredElements = elements;
    } else{
        foreach(const HIWebElement & element, elements) {
            if (0 < element.geometry().width()) {
                filteredElements << element;
            }
        }
    }

    GT_CHECK_RESULT(!options.failIfNotFound || !filteredElements.isEmpty(), QString("There are no elements that match selector '%1'").arg(selector), QList<HIWebElement>());
    return filteredElements;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkElement"
void GTWebEngineView::checkElement(GUITestOpStatus &os, QWebEngineView *view, QString text, QString tag, bool exists, bool exactMatch){
    const bool found = doesElementExist(os, view, text, tag, exactMatch);
    if (exists) {
        GT_CHECK(found, "element with text " + text + " and tag " + tag + " not found");
    } else {
        GT_CHECK(!found, "element with text " + text + " and tag " + tag + " unexpectedly found");
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "doesElementExist"
bool GTWebEngineView::doesElementExist(GUITestOpStatus &os, QWebEngineView *view, const QString &text, const QString &tag, bool exactMatch) {
    GTGlobals::FindOptions options;
    options.failIfNotFound = false;
    foreach (const HIWebElement &element, findElementsBySelector(os, view, tag, options)) {
        if (compare(element.toPlainText(), text, exactMatch)) {
            return true;
        }
    }
    return false;
}
#undef GT_METHOD_NAME

HIWebElement GTWebEngineView::findTreeElement(GUITestOpStatus &os, QWebEngineView *view, QString text) {
    return findElement(os, view, text, "SPAN");
}

HIWebElement GTWebEngineView::findContextMenuElement(GUITestOpStatus &os, QWebEngineView *view, QString text ){
    return findElement(os, view, text, "LI");
}

void GTWebEngineView::traceAllWebElements(GUITestOpStatus &os, QWebEngineView *view) {
    GTGlobals::FindOptions options;
    options.failIfNotFound = false;
    foreach (const HIWebElement &element, findElementsBySelector(os, view, "*", options)) {
        qDebug("GT_DEBUG_MESSAGE tag: '%s'; text: '%s'; width: %d", qPrintable(element.tagName()), qPrintable(element.toPlainText()), element.geometry().width());
    }
}

HIWebElement GTWebEngineView::toHiWebElement(const QMap<QString, QVariant> &map) {
    QMap<QString, QString> attributesMap;
    QMap<QString, QVariant> rawAttributesMap = map[ATTRIBUTES_MAP].toMap();
    foreach (const QString &key, rawAttributesMap.keys()) {
        attributesMap.insert(key, rawAttributesMap[key].toString());
    }
    return HIWebElement(QRect(map.value(X, -1).toInt(),
                              map.value(Y, -1).toInt(),
                              map.value(WIDTH, -1).toInt(),
                              map.value(HEIGHT, -1).toInt()),
                        map[TEXT].toString(),
                        map[XML].toString(),
                        map[TAG].toString(),
                        map[ID].toString(),
                        attributesMap,
                        map[VISIBLE].toBool());
}

#undef GT_CLASS_NAME

}
