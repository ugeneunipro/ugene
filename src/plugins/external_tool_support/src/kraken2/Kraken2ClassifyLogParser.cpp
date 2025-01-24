/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "Kraken2ClassifyLogParser.h"

namespace U2 {

static const QStringList wellKnownErrors = {"Must specify DB", 
                                            "does not contain necessary file database.kdb", 
                                            "--paired requires exactly two filenames", 
                                            "mismatched mate pair names", 
                                            "can't determine what format",
                                            "can't open database.idx: No such file or directory",
                                            "mismatched sequence counts", 
                                            "Need to specify input filenames"};

Kraken2ClassifyLogParser::Kraken2ClassifyLogParser()
    : ExternalToolLogParser() {
}

bool Kraken2ClassifyLogParser::isError(const QString &line) const {
    for (const QString &wellKnownError : wellKnownErrors) {
        if (line.contains(wellKnownError)) {
            return true;
        }
    }
    return false;
}

}  // namespace U2
