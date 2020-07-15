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

#include "DomUtils.h"

#include <QTextStream>

namespace U2 {

QDomDocument DomUtils::fromString(const QString &string, QString &parseError) {
    parseError.clear();
    QDomDocument doc;
    int line = 0, col = 0;
    doc.setContent(string, &parseError, &line, &col);
    return doc;
}

QDomElement DomUtils::findElementById(const QDomElement &element, const QString &id) {
    QString elId = element.attribute("id");
    if (elId == id) {
        return element;
    }
    for (auto childElement = element.firstChildElement(); !childElement.isNull(); childElement = childElement.nextSiblingElement()) {
        QDomElement result = findElementById(childElement, id);
        if (result.attribute("id") == id) {
            return result;
        }
    }
    return QDomElement();
}

QDomElement DomUtils::findParentByTag(const QDomElement &element, const QString &tagName) {
    for (QDomNode parent = element.parentNode(); !parent.isNull(); parent = parent.parentNode()) {
        if (parent.toElement().tagName() == tagName) {
            return parent.toElement();
        }
    }
    return QDomElement();
}

bool DomUtils::hasClass(const QDomElement &element, const QString &className) {
    QString elementClass = element.attribute("class");
    if (elementClass.isEmpty()) {
        return false;
    }
    return elementClass == className ||
           elementClass.startsWith(className + " ") ||
           elementClass.endsWith(" " + className) ||
           elementClass.contains(" " + className + " ");
}

QList<QDomElement> DomUtils::findChildElementsByClass(const QDomNode &node, const QString &className, int maxDepth) {
    QList<QDomElement> result;
    findChildElementsByClass(node, className, result, maxDepth);
    return result;
}

void DomUtils::findChildElementsByClass(const QDomNode &node, const QString &className, QList<QDomElement> &result, int maxDepth) {
    if (maxDepth == 0) {
        return;
    }
    for (auto childElement = node.firstChildElement(); !childElement.isNull(); childElement = childElement.nextSiblingElement()) {
        if (hasClass(childElement, className)) {
            result << childElement;
        }
        findChildElementsByClass(childElement, className, result, maxDepth - 1);
    }
}

QDomElement DomUtils::findChildElementByClass(const QDomNode &node, const QString &className, int maxDepth) {
    if (maxDepth == 0) {
        return QDomElement();
    }
    QList<QDomElement> list;
    findChildElementsByClass(node, className, list, maxDepth - 1);
    return list.size() > 0 ? list.first() : QDomElement();
}

QString DomUtils::toString(const QDomElement &element, bool includeElement) {
    if (element.isNull()) {
        return "";
    }
    QString result;
    QTextStream stream(&result);
    element.save(stream, 0);
    stream.flush();
    if (!includeElement) {
        int innerContentStart = result.indexOf(">") + 1;
        int innerContentEnd = result.lastIndexOf("<");
        return result.mid(innerContentStart, innerContentEnd - innerContentStart);
    }
    return result;
}

}    // namespace U2
