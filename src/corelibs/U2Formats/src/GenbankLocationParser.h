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

#ifndef _U2_GENBANK_LOCATION_PARSER_H
#define _U2_GENBANK_LOCATION_PARSER_H

#include <QObject>

#include <U2Core/global.h>

namespace U2 {

class U2Location;

namespace Genbank {

class U2FORMATS_EXPORT LocationParser : public QObject {
    Q_OBJECT
public:
    enum ParsingResult {
        Success,
        ParsedWithWarnings,
        Failure
    };

    static const QString REMOTE_ENTRY_WARNING;
    static const QString JOIN_COMPLEMENT_WARNING;

    static ParsingResult parseLocation(const char* str, int len, U2Location& location, qint64 seqlenForCircular = -1);
    static ParsingResult parseLocation(const char* str, int len, U2Location& location, QStringList &messages, qint64 seqlenForCircular = -1);
};


}

}//namespace

#endif
