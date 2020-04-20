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

#include "KrakenTranslateLogParser.h"

namespace U2 {

const QStringList KrakenTranslateLogParser::wellKnownErrors = KrakenTranslateLogParser::initWellKnownErrors();

KrakenTranslateLogParser::KrakenTranslateLogParser()
    : ExternalToolLogParser()
{

}

bool KrakenTranslateLogParser::isError(const QString &line) const {
    foreach (const QString &wellKnownError, wellKnownErrors) {
        if (line.contains(wellKnownError)) {
            return true;
        }
    }
    return false;
}

QStringList KrakenTranslateLogParser::initWellKnownErrors() {
    QStringList result;
    result << "Must specify DB";
    result << "unable to find";
    result << "does not contain necessary file database.kdb";
    result << "No such file or directory";
    result << "Use of uninitialized value $taxid";

    return result;
}

}   // namespace U2
