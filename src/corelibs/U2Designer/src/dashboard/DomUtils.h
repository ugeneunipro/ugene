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

#ifndef _U2_DOM_UTILS_H_
#define _U2_DOM_UTILS_H_

#include <QDomDocument>
#include <QDomElement>
#include <QList>

namespace U2 {

class DomUtils {
public:
    static QDomDocument fromString(const QString &string, QString &parseError);

    static QDomElement findElementById(const QDomElement &element, const QString &id);

    static QDomElement findParentByTag(const QDomElement &element, const QString &tagName);

    static bool hasClass(const QDomElement &element, const QString &className);

    static QList<QDomElement> findChildElementsByClass(const QDomNode& node, const QString &className, int maxDepth = -1);

    static void findChildElementsByClass(const QDomNode &node, const QString &className, QList<QDomElement> &result, int maxDepth = -1);

    static QDomElement findChildElementByClass(const QDomNode &element, const QString &className, int maxDepth = -1);

    static QString toString(const QDomElement &element, bool includeElement);
};

}    // namespace U2
#endif    // _U2_DOM_UTILS_H_
