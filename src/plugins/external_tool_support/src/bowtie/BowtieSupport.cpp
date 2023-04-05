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

#include "BowtieSupport.h"

#include <U2Core/AppContext.h>
#include <U2Core/ExternalToolRunTask.h>

namespace U2 {

// BowtieSupport
const QString BowtieSupport::ET_BOWTIE_ID = "USUPP_BOWTIE";
const QString BowtieSupport::ET_BOWTIE_BUILD_ID = "USUPP_BOWTIE_BUILD";

static QString getBowtieToolNameById(const QString& id) {
    return id == BowtieSupport::ET_BOWTIE_ID ? "Bowtie aligner" : "Bowtie build indexer";
}

BowtieSupport::BowtieSupport(const QString& id)
    : ExternalTool(id, "bowtie1", getBowtieToolNameById(id)) {
    if (AppContext::getMainWindow() != nullptr) {
        icon = QIcon(":external_tool_support/images/cmdline.png");
        grayIcon = QIcon(":external_tool_support/images/cmdline_gray.png");
        warnIcon = QIcon(":external_tool_support/images/cmdline_warn.png");
    }
    if (id == ET_BOWTIE_ID) {
#ifdef Q_OS_WIN
        executableFileName = "bowtie-align-s.exe";
#else
        executableFileName = "bowtie-align-s";
#endif
    } else {
#ifdef Q_OS_WIN
        executableFileName = "bowtie-build-s.exe";
#else
        executableFileName = "bowtie-build-s";
#endif
    }
    validationArguments.append("--version");
    validMessage = "version";
    description = tr("<i>Bowtie</i> is an ultrafast, memory-efficient short read aligner. "
                     "It aligns short DNA sequences (reads) to the human genome at "
                     "a rate of over 25 million 35-bp reads per hour. "
                     "Bowtie indexes the genome with a Burrows-Wheeler index to keep "
                     "its memory footprint small: typically about 2.2 GB for the human "
                     "genome (2.9 GB for paired-end).");
    versionRegExp = QRegExp("version (\\d+\\.\\d+\\.\\d+)");
    toolKitName = "Bowtie";

    if (isOsWindows()) {
        pathChecks << ExternalTool::PathChecks::NonLatinArguments
                   << ExternalTool::PathChecks::NonLatinTemporaryDirPath
                   << ExternalTool::PathChecks::NonLatinToolPath
                   << ExternalTool::PathChecks::NonLatinIndexPath;
    }
}

}  // namespace U2
