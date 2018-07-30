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

#include "StringTieSupport.h"

namespace U2 {

StringTieSupport::StringTieSupport(const QString &name, const QString &path)
    : ExternalTool(name, path) {

    executableFileName = "stringtie";

    validMessage = "StringTie";
    description = tr("<i>StringTie</i> is a fast and highly efficient assembler"
                     " of RNA-Seq alignments into potential transcripts. "
                     "It uses a novel network flow algorithm as well as "
                     "an optional de novo assembly step to assemble and "
                     "quantitate full-length transcripts representing multiple"
                     " splice variants for each gene locus. "
                     "Its input can include not only the alignments of raw reads "
                     "used by other transcript assemblers, "
                     "but also alignments longer sequences that have been assembled "
                     "from those reads.");

    versionRegExp = QRegExp("StringTie v(\\d+.\\d+.\\d+[a-zA-Z]?)");
    validationArguments << "-h";
    toolKitName = "StringTie";
}

} // namespace
