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

#include "TabixSupport.h"

#include <U2Core/AppContext.h>
#include <U2Gui/MainWindow.h>

namespace U2 {

const QString TabixSupport::ET_TABIX = "Tabix";
const QString TabixSupport::ET_TABIX_ID = "USUPP_TABIX";

TabixSupport::TabixSupport(const QString& id, const QString &name, const QString &path)
    : ExternalTool(id, name, path)
{
    if (AppContext::getMainWindow() != NULL) {
        icon = QIcon(":external_tool_support/images/cmdline.png");
        grayIcon = QIcon(":external_tool_support/images/cmdline_gray.png");
        warnIcon = QIcon(":external_tool_support/images/cmdline_warn.png");
    }

#ifdef Q_OS_WIN
    executableFileName="tabix.exe";
#else
    #if defined(Q_OS_UNIX)
    executableFileName = "tabix";
    #endif
#endif

    muted = true;
    validationArguments << "-help";
    validMessage = "tabix";
    description = tr("<i>Tabix</i> is a generic indexer for TAB-delimited genome position files");
    versionRegExp = QRegExp("Version: (\\d+.\\d+.\\d+)");
    toolKitName = "SAMtools";
}

} // namespace
