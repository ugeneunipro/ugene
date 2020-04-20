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

#include "SamToolsExtToolSupport.h"

#include <U2Core/AppContext.h>

#include <U2Gui/MainWindow.h>


namespace U2 {

const QString SamToolsExtToolSupport::ET_SAMTOOLS_EXT = "SAMtools";
const QString SamToolsExtToolSupport::ET_SAMTOOLS_EXT_ID = "USUPP_SAMTOOLS";

SamToolsExtToolSupport::SamToolsExtToolSupport(const QString& id, const QString& name, const QString& path)
    : ExternalTool(id, name, path)
{
    if (AppContext::getMainWindow()) {
        icon = QIcon(":external_tool_support/images/cmdline.png");
        grayIcon = QIcon(":external_tool_support/images/cmdline_gray.png");
        warnIcon = QIcon(":external_tool_support/images/cmdline_warn.png");
    }

#ifdef Q_OS_WIN
    executableFileName = "samtools.exe";
#else
#if defined(Q_OS_UNIX)
    executableFileName = "samtools";
#endif
#endif

    validMessage = "samtools \\(Tools for alignments in the SAM format\\)";
    description = "<i>SAMtools</i> is a set of utilities for interacting"
        " with and post-processing short DNA sequence read alignments."
        " This external tool is required to run <i>TopHat</i> external tool.";
    versionRegExp = QRegExp("Version: (\\d+.\\d+.\\d+)");

    toolKitName = "SAMtools";

    muted = true;
}

} // namespace
