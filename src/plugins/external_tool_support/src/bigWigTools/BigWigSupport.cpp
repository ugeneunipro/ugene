/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "BigWigSupport.h"

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/DataPathRegistry.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/ConvertFileTask.h>

namespace U2 {

const QString BigWigSupport::ET_BIGWIG_ID = "USUPP_BED_GRAPH_TO_BIG_WIG";
const QString BigWigSupport::GENOMES_DATA_NAME = "Genome files";
const QString BigWigSupport::GENOMES_DIR_NAME = "genome_lengths";

BigWigSupport::BigWigSupport(const QString& path)
    : ExternalTool(ET_BIGWIG_ID, "bigwig", "bigwig", path) {
    if (AppContext::getMainWindow() != nullptr) {
        icon = QIcon(":external_tool_support/images/cmdline.png");
        grayIcon = QIcon(":external_tool_support/images/cmdline_gray.png");
        warnIcon = QIcon(":external_tool_support/images/cmdline_warn.png");
    }
#ifdef Q_OS_WIN
    executableFileName = "bedGraphToBigWig.exe";
#else
    executableFileName = "bedGraphToBigWig";
#endif
    validationMessageRegExp = "bedGraphToBigWig";
    description = tr("<i>bedGraphToBigWig</i>: convert a bedGraph file to bigWig format.");

    versionRegExp = QRegExp("bedGraphToBigWig v (\\d+)");
    validationArguments << "";
    toolKitName = "bedGraphToBigWig";

    U2DataPathRegistry* dpr = AppContext::getDataPathRegistry();
    if (dpr != nullptr) {
        auto dp = new U2DataPath(GENOMES_DATA_NAME, QString(PATH_PREFIX_DATA) + ":" + GENOMES_DIR_NAME, "", U2DataPath::CutFileExtension);
        dpr->registerEntry(dp);
    }
}

}  // namespace U2
