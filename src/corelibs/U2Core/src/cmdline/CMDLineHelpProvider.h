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

#ifndef _U2_CMD_LINE_HELP_PROVIDER_
#define _U2_CMD_LINE_HELP_PROVIDER_

#include "U2Core/global.h"

namespace U2 {

class U2CORE_EXPORT CMDLineHelpProvider {
public:
    CMDLineHelpProvider(
        const QString & _fullName,                    // Name of the option (e.g. "version")
        const QString & _shortDescription,            // Description shown in the options list
        const QString & _fullDescription = QString(), // Description shown in the option's help
        const QString & _argsDescription = QString(), // Description of the option arguments
        const QString & _shortName = QString())       // Short name of the option (e.g. "v")
        : fullName(_fullName),
          shortDescription(_shortDescription),
          fullDescription(_fullDescription),
          argsDescription(_argsDescription),
          shortName(_shortName)
    {
        assert((_shortDescription.length() < 49) && ("The length of the short"
            " description must fit into the options list. If you need to create"
            " a short description longer than 49 characters, then register two"
            " or more CMDLineHelpProvider with the same fullName. They will be"
            " placed on different lines in the options list."));
    }

    QString getHelpSectionFullName() const {return fullName;}
    QString getHelpSectionShortDescription() const { return shortDescription; }
    QString getHelpSectionFullDescription() const { return fullDescription; }
    QString getHelpSectionArgsDescription() const { return argsDescription; }
    QString getHelpSectionShortName() const {return shortName;}
    QString getHelpSectionNames() const {
        QString ret = fullName;
        if (!shortName.isEmpty()) { ret += " | -" + shortName; }
        return ret;
    }

private:
    QString fullName;
    QString shortDescription;
    QString fullDescription;
    QString argsDescription;
    QString shortName;

}; // CMDLineHelpProvider

}//namespace

#endif
