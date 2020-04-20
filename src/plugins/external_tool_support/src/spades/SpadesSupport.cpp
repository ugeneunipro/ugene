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

#include "SpadesSupport.h"

#include <python/PythonSupport.h>

namespace U2 {

// SpadesSupport
const QString SpadesSupport::ET_SPADES = "SPAdes";
const QString SpadesSupport::ET_SPADES_ID = "USUPP_SPADES";

SpadesSupport::SpadesSupport(const QString& id, const QString &name, const QString &path):
    ExternalTool(id, name, path)
{
    if (AppContext::getMainWindow()) {
        icon = QIcon(":external_tool_support/images/cmdline.png");
        grayIcon = QIcon(":external_tool_support/images/cmdline_gray.png");
        warnIcon = QIcon(":external_tool_support/images/cmdline_warn.png");
    }

    executableFileName = "spades.py";
    validMessage = "SPAdes";
    description = tr("<i>SPAdes</i> - St. Petersburg genome assembler - is intended for both standard isolates and single-cell MDA bacteria assemblies. Official site: http://bioinf.spbau.ru/spades");
    validationArguments << "--version";
    versionRegExp = QRegExp("SPAdes v(\\d+.\\d+.\\d+)");
    toolKitName = "SPAdes";

    toolRunnerProgram = PythonSupport::ET_PYTHON_ID;
    dependencies << PythonSupport::ET_PYTHON_ID;
}

} // namespace U2
