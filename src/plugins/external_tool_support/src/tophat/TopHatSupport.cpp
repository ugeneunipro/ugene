/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "TopHatSupport.h"

#include <U2Core/AppContext.h>

#include <U2Gui/MainWindow.h>

#include "python/Python3Support.h"

namespace U2 {

const QString TopHatSupport::ET_TOPHAT = "TopHat";
const QString TopHatSupport::ET_TOPHAT_ID = "USUPP_TOPHAT";
const QString TopHatSupport::TOPHAT_TMP_DIR = "tophat";

TopHatSupport::TopHatSupport()
    : ExternalTool(TopHatSupport::ET_TOPHAT_ID, "tophat2", TopHatSupport::ET_TOPHAT) {
    if (AppContext::getMainWindow() != nullptr) {
        icon = QIcon(":external_tool_support/images/cmdline.png");
        grayIcon = QIcon(":external_tool_support/images/cmdline_gray.png");
        warnIcon = QIcon(":external_tool_support/images/cmdline_warn.png");
    }

#ifdef Q_OS_WIN
    executableFileName = "tophat.exe";
#else
#    if defined(Q_OS_UNIX)
    executableFileName = "tophat";
#    endif
#endif
    validationArguments << "--version";

    dependencies << Python3Support::ET_PYTHON_ID;

    validationMessageRegExp = "TopHat ";
    description = "<i>TopHat</i> is a program that aligns RNA-Seq reads to a genome"
                  " in order to identify exon-exon splice junctions. It is built on"
                  " the ultrafast short read mapping program Bowtie."
                  "<br><br><b>Note:</b>"
                  "<br>TopHat is not officially supported today and is not compatible with the latest Bowtie versions."
                  "<br>The old and compatible versions of Bowtie1 and Bowtie2 must be placed "
                  "into the 'bowtie1' and 'bowtie2' sub-folders of the TopHat to be used by default.";
    versionRegExp = QRegExp("(\\d+.\\d+.\\d+\\w?)");
    toolKitName = "TopHat";

    muted = true;
}

}  // namespace U2
