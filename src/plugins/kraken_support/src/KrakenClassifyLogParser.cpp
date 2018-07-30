/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include "KrakenClassifyLogParser.h"

namespace U2 {

const QStringList KrakenClassifyLogParser::wellKnownErrors = KrakenClassifyLogParser::initWellKnownErrors();

KrakenClassifyLogParser::KrakenClassifyLogParser()
    : ExternalToolLogParser()
{

}

bool KrakenClassifyLogParser::isError(const QString &line) const {
    foreach (const QString &wellKnownError, wellKnownErrors) {
        if (line.contains(wellKnownError)) {
            return true;
        }
    }
    return false;
}

QStringList KrakenClassifyLogParser::initWellKnownErrors() {
    QStringList result;
    result << "Must specify DB";
    result << "does not contain necessary file database.kdb";
    result << "--paired requires exactly two filenames";
    result << "mismatched mate pair names";
    result << "can't determine what format";
    result << "can't open database.idx: No such file or directory";
    result << "mismatched sequence counts";
    result << "Need to specify input filenames";
    return result;
}

}   // namespace U2
