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

void SynchronizedCall::run(QWebEngineView *view, const QString &scriptSource, const std::function<void(QVariant)> &resultCallback) {
    SynchronizedCall instance;
    connect(&instance, SIGNAL(callbackFinished()), &instance.eventLoop, SLOT(quit()));
    auto innerCallback = [&](const QVariant &v) {
        resultCallback(v);
        emit instance.callbackFinished();
    };
    view->page()->runJavaScript(scriptSource, innerCallback);
    instance.eventLoop.exec();
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
    } else {
        foreach(const HIWebElement &element, elements) {
            if (element.isVisible()) {
                filteredElements << element;
            }
        }
    }

    GT_CHECK_RESULT(!options.failIfNotFound || !filteredElements.isEmpty(), QString("There are no elements that match selector '%1'").arg(selector), QList<HIWebElement>());
    return filteredElements;
}
#undef GT_METHOD_NAME

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
