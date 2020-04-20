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

#include "WevoteSupport.h"

namespace U2 {

const QString WevoteSupport::TOOL_NAME = "WEVOTE";
const QString WevoteSupport::TOOL_ID = "USUPP_WEVOTE";

WevoteSupport::WevoteSupport()
    : ExternalTool(TOOL_ID, TOOL_NAME, "")
{
    validMessage = "less than the required minimum number of options";
    executableFileName = "WEVOTE";
    description = tr("WEVOTE (WEighted VOting Taxonomic idEntification) is a metagenome shortgun sequencing DNA reads classifier "
                     "based on an ensemble of other classification methods. In UGENE one can use the following methods: Kraken, CLARK, DIAMOND.");
}

}   // namespace U2
