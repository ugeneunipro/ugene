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

#include <U2Core/AppContext.h>

#include "perl/PerlSupport.h"

#include "VcfutilsSupport.h"

namespace U2 {

const QString VcfutilsSupport::VCF_UTILS("vcfutils");
const QString VcfutilsSupport::VCF_UTILS_ID("USUPP_VCFUTILS");

VcfutilsSupport::VcfutilsSupport(const QString& id, const QString &name)
: ExternalTool(id, name, "")
{
    if (AppContext::getMainWindow()) {
        icon = QIcon(":external_tool_support/images/cmdline.png");
        grayIcon = QIcon(":external_tool_support/images/cmdline_gray.png");
        warnIcon = QIcon(":external_tool_support/images/cmdline_warn.png");
    }

    executableFileName = "vcfutils.pl";
    description = "The set of utilities for VCF format operations";

    toolRunnerProgram = PerlSupport::ET_PERL_ID;
    dependencies << PerlSupport::ET_PERL_ID;

    validMessage = "varFilter";
    toolKitName = "SAMtools";

    muted = true;
}

} // U2
