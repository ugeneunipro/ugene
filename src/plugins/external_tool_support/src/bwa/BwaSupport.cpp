/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "BwaSupport.h"

#include <U2Core/AppContext.h>
#include <U2Core/ExternalToolRunTask.h>

namespace U2 {

const QString BwaSupport::ET_BWA = "BWA";
const QString BwaSupport::ET_BWA_ID = "USUPP_BWA";

BwaSupport::BwaSupport()
    : ExternalTool(BwaSupport::ET_BWA_ID, "bwa", BwaSupport::ET_BWA) {
    if (AppContext::getMainWindow() != nullptr) {
        icon = QIcon(":external_tool_support/images/cmdline.png");
        grayIcon = QIcon(":external_tool_support/images/cmdline_gray.png");
        warnIcon = QIcon(":external_tool_support/images/cmdline_warn.png");
    }
#ifdef Q_OS_WIN
    executableFileName = "bwa.exe";
#else
#    if defined(Q_OS_UNIX)
    executableFileName = "bwa";
#    endif
#endif
    validMessage = "Program: bwa \\(alignment via Burrows-Wheeler transformation\\)";
    description = tr("<i>Burrows-Wheeler Aligner (BWA)</i> is an efficient program "
                     "that aligns relatively short nucleotide sequences "
                     "against a long reference sequence such as the human genome.");
    versionRegExp = QRegExp("Version: (\\d+\\.\\d+\\.\\d+-r\\d+)");
    toolKitName = "BWA";
    if (isOsWindows()) {
        pathChecks << ExternalTool::PathChecks::NonLatinArguments
                   << ExternalTool::PathChecks::NonLatinTemporaryDirPath
                   << ExternalTool::PathChecks::NonLatinIndexPath;
    }
}

}  // namespace U2
