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

#include "GTWebView.h"
#include "drivers/GTMouseDriver.h"
#include "utils/GTThread.h"

#ifdef UGENE_WEB_KIT
#include "GTWebkitView.h"
#else
#include "GTWebEngineView.h"
#endif

namespace HI {

HIWebElement::HIWebElement()
    : visible(false) {

}

HIWebElement::HIWebElement(const QRect &rect,
                           const QString &text,
                           const QString &xml,
                           const QString &tag,
                           const QString &idAttribute,
                           const QMap<QString, QString> &attributesMap,
                           const bool isVisible)
    : rect(rect),
      text(text),
      xml(xml),
      tag(tag),
      idAttribute(idAttribute),
      attributesMap(attributesMap),
      visible(isVisible)
{

}

const QRect &HIWebElement::geometry() const {
    return rect;
}

const QString &HIWebElement::toInnerXml() const {
    return xml;
}

const QString &HIWebElement::toPlainText() const {
    return text;
}

const QString &HIWebElement::tagName() const {
    return tag;
}

const QString &HIWebElement::id() const {
    return idAttribute;
}

const QMap<QString, QString> &HIWebElement::attributes() const {
    return attributesMap;
}

QString HIWebElement::attribute(const QString &name, const QString &defaultValue) const {
    return attributesMap.value(name, defaultValue);
}

bool HIWebElement::isVisible() const {
    return visible;
}

namespace {

bool compare(const QString &s1, const QString &s2, bool exactMatch) {
    if (exactMatch) {
        return s1 == s2;
    } else {
        return s1.contains(s2);
    }
}

}

#define GT_CLASS_NAME "GTWebView"

#define GT_METHOD_NAME "findElement"
HIWebElement GTWebView::findElement(GUITestOpStatus &os, WebView *view, const QString &text, const QString &tag, bool exactMatch) {
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
HIWebElement GTWebView::findElementById(GUITestOpStatus &os, WebView *view, const QString &id, const QString &tag) {
    const QString selector = id.isEmpty() ? tag : QString("%1 [id='%2']").arg(tag).arg(id);
    return findElementBySelector(os, view, selector, GTGlobals::FindOptions());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findElementsById"
QList<HIWebElement> GTWebView::findElementsById(GUITestOpStatus &os, WebView *view, const QString &id, const QString &tag, const HIWebElement &parentElement) {
    const QString parentQuery = parentElement.tagName().isEmpty() ? "" : parentElement.tagName() + (id.isEmpty() ? ""  : "[id=" + parentElement.id() + "]") + " ";
    const QString elementQuery = tag + (id.isEmpty() ? ""  : "[id=" + id + "]");
    return findElementsBySelector(os, view, parentQuery + elementQuery, GTGlobals::FindOptions());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findElementBySelector"
HIWebElement GTWebView::findElementBySelector(GUITestOpStatus &os, WebView *view, const QString &selector, const GTGlobals::FindOptions &options) {
    const QList<HIWebElement> elements = findElementsBySelector(os, view, selector, options);
    GT_CHECK_RESULT(!options.failIfNotFound || !elements.isEmpty(), QString("There are no elements that match selector '%1'").arg(selector), HIWebElement());
    return elements.first();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findElementsBySelector"
QList<HIWebElement> GTWebView::findElementsBySelector(GUITestOpStatus &os, WebView *view, const QString &selector, const GTGlobals::FindOptions &options) {
    return GTWebViewPrivate::findElementsBySelector(os, view, selector, options);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkElement"
void GTWebView::checkElement(GUITestOpStatus &os, WebView *view, QString text, QString tag, bool exists, bool exactMatch){
    const bool found = doesElementExist(os, view, text, tag, exactMatch);
    if (exists) {
        GT_CHECK(found, "element with text " + text + " and tag " + tag + " not found");
    } else {
        GT_CHECK(!found, "element with text " + text + " and tag " + tag + " unexpectedly found");
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "doesElementExist"
bool GTWebView::doesElementExist(GUITestOpStatus &os, WebView *view, const QString &text, const QString &tag, bool exactMatch) {
    GTGlobals::FindOptions options;
    options.failIfNotFound = false;
    options.searchInHidden = false;
    foreach (const HIWebElement &element, findElementsBySelector(os, view, tag, options)) {
        if (compare(element.toPlainText(), text, exactMatch)) {
            return true;
        }
    }
    return false;
}
#undef GT_METHOD_NAME

HIWebElement GTWebView::findTreeElement(GUITestOpStatus &os, WebView *view, QString text){
    return findElement(os, view, text, "SPAN");
}

HIWebElement GTWebView::findContextMenuElement(GUITestOpStatus &os, WebView *view, QString text){
    return findElement(os, view, text, "LI");
}

void GTWebView::click(GUITestOpStatus & /*os*/, WebView *view, HIWebElement el, Qt::MouseButton button) {
    GTMouseDriver::moveTo(view->mapToGlobal(el.geometry().center()));
    GTMouseDriver::click(button);
    GTThread::waitForMainThread();
}

void GTWebView::selectElementText(GUITestOpStatus & /*os*/, WebView *view, HIWebElement el){
    GTMouseDriver::moveTo(view->mapToGlobal(el.geometry().topLeft()) + QPoint(5,5));
    GTMouseDriver::press();
    GTMouseDriver::moveTo(view->mapToGlobal(el.geometry().bottomRight()) - QPoint(5,5));
    GTMouseDriver::release();
}

void GTWebView::traceAllWebElements(GUITestOpStatus &os, WebView *view) {
    GTGlobals::FindOptions options;
    options.failIfNotFound = false;
    foreach (const HIWebElement &element, findElementsBySelector(os, view, "*", options)) {
        qDebug("GT_DEBUG_MESSAGE tag: '%s'; text: '%s'; width: %d", qPrintable(element.tagName()), qPrintable(element.toPlainText()), element.geometry().width());
    }
}

#undef GT_CLASS_NAME

} // namespace HI
