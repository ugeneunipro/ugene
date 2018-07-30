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

#include "TrimmomaticLogParser.h"

namespace U2 {

const QStringList TrimmomaticLogParser::wellKnownErrors = QStringList()
        << "Exception"
        << "Unable to determine input files"
        << "Unable to determine output files"
        << "Unknown option";

TrimmomaticLogParser::TrimmomaticLogParser()
    : ExternalToolLogParser()
{

}

bool TrimmomaticLogParser::isError(const QString &line) const {
    foreach (const QString &wellKnownErrors, wellKnownErrors) {
        if (line.contains(wellKnownErrors)) {
            return true;
        }
    }
    return false;
}

} // namespace U2
