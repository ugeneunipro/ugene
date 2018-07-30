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

#include "DiamondSupport.h"

namespace U2 {

const QString DiamondSupport::TOOL_NAME = "DIAMOND";

DiamondSupport::DiamondSupport(const QString &name)
    : ExternalTool(name)
{
    validationArguments << "--version";
    validMessage = "diamond version ";
    versionRegExp = QRegExp("diamond version (\\d+\\.\\d+\\.\\d+)");
    executableFileName = "diamond";
    description = tr("In general, DIAMOND is a sequence aligner for protein and translated DNA searches similar to the NCBI BLAST software tools. "
                     "In UGENE it is integrated as one of the taxonomic classification tool.");
}

}   // namesapce U2
