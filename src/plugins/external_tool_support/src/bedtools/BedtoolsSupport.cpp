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

#include "BedtoolsSupport.h"

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/DataPathRegistry.h>
#include <U2Core/ExternalToolRunTask.h>

#include <U2Formats/ConvertFileTask.h>

#include "BedtoolsSupportTask.h"

namespace U2 {

const QString BedtoolsSupport::ET_BEDTOOLS_ID = "USUPP_BEDTOOLS";
const QString BedtoolsSupport::GENOMES_DIR_NAME = "genome_lengths";
const QString BedtoolsSupport::GENOMES_DATA_NAME = "Genome files";

BedtoolsSupport::BedtoolsSupport(const QString& path)
    : ExternalTool(ET_BEDTOOLS_ID, "bedtools", "bedtools", path) {
    if (AppContext::getMainWindow() != nullptr) {
        icon = QIcon(":external_tool_support/images/cmdline.png");
        grayIcon = QIcon(":external_tool_support/images/cmdline_gray.png");
        warnIcon = QIcon(":external_tool_support/images/cmdline_warn.png");
    }
#ifdef Q_OS_WIN
    executableFileName = "bedtools.exe";
#else
    executableFileName = "bedtools";
#endif
    validationMessageRegExp = "bedtools v";
    description = tr("<i>Bedtools</i>: flexible tools for genome arithmetic and DNA sequence analysis.");

    versionRegExp = QRegExp("bedtools v(\\d+.\\d+.\\d+)");
    validationArguments << "--version";
    toolKitName = "bedtools";

    connect(this, SIGNAL(si_toolValidationStatusChanged(bool)), SLOT(sl_validationStatusChanged(bool)));

    U2DataPathRegistry* dpr = AppContext::getDataPathRegistry();
    if (dpr != nullptr) {
        auto dp = new U2DataPath(GENOMES_DATA_NAME, QString(PATH_PREFIX_DATA) + ":" + GENOMES_DIR_NAME, "", U2DataPath::CutFileExtension);
        dpr->registerEntry(dp);
    }
    if (isOsWindows()) {
        pathChecks << ExternalTool::PathChecks::NonLatinArguments;
    }
}

void BedtoolsSupport::sl_validationStatusChanged(bool /*newStatus*/) {
    ConvertFactoryRegistry* registry = AppContext::getConvertFactoryRegistry();
    if (isValid()) {
        registry->registerConvertFactory(new BAMBEDConvertFactory());
    }
}

}  // namespace U2
