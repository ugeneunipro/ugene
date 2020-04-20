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

#ifndef _U2_CUSTOM_TOOL_CONFIG_PARSER_H_
#define _U2_CUSTOM_TOOL_CONFIG_PARSER_H_

#include <QCoreApplication>
#include <QDomElement>

class QDomDocument;

namespace U2 {

class CustomExternalTool;
class U2OpStatus;

class CustomToolConfigParser {
    Q_DECLARE_TR_FUNCTIONS(CustomToolConfigParser)
public:

    static CustomExternalTool *parse(U2OpStatus &os, const QString& url);
    static QDomDocument serialize(CustomExternalTool *tool);

private:
    static bool validate(U2OpStatus  &os, CustomExternalTool *tool);
    static QDomElement addChildElement(QDomDocument &doc, const QString &elementName, const QString &elementData);

    static const QString ELEMENT_CONFIG;
    static const QString ATTRIBUTE_VERSION;
    static const QString HARDCODED_EXPECTED_VERSION;

    static const QString ID;
    static const QString NAME;
    static const QString PATH;
    static const QString DESCRIPTION;
    static const QString TOOLKIT_NAME;
    static const QString TOOL_VERSION;
    static const QString LAUNCHER_ID;
    static const QString DEPENDENCIES;
    static const QString DEPENDENCY_ID;
    static const QString BINARY_NAME;
};

}   // namespace U2

#endif // _U2_CUSTOM_TOOL_CONFIG_PARSER_H_
