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

#include "Python3ModuleCutadaptSupport.h"
#include "Python3Support.h"

#include <U2Core/AppContext.h>
#include <U2Core/DataPathRegistry.h>

namespace U2 {

const QString Python3ModuleCutadaptSupport::ET_PYTHON_CUTADAPT_ID = "USUPP_PYTHON_CUTADAPT";
const QString Python3ModuleCutadaptSupport::ADAPTERS_DIR_NAME = "adapters";
const QString Python3ModuleCutadaptSupport::ADAPTERS_DATA_NAME = "Adapters file";

Python3ModuleCutadaptSupport::Python3ModuleCutadaptSupport()
    : ExternalToolModule(Python3ModuleCutadaptSupport::ET_PYTHON_CUTADAPT_ID, "python3", "Cutadapt") {
    if (AppContext::getMainWindow()) {
        icon = IconParameters("external_tool_support", "python_cutadapt.png");
        grayIcon = IconParameters("external_tool_support", "python_cutadapt_gray.png");
        warnIcon = IconParameters("external_tool_support", "python_cutadapt_warn.png");
    }
    description += "Cutadapt" + tr(" finds and removes adapter sequences, primers, poly-A tails and other types of unwanted sequence from your high-throughput sequencing reads.");

    validationArguments.append({"-m", "cutadapt", "--version"});
    toolKitName = "python3";
    dependencies << Python3Support::ET_PYTHON_ID;
    validationMessageRegExp = "(\\d+.\\d+)";
    versionRegExp = QRegExp(validationMessageRegExp);
    executableFileName = "cutadapt";
    muted = true;

    errorDescriptions.insert("No module named", tr("The \"Cutadapt\" module is not installed."
                                                   "Install module or set path "
                                                   "to another Python scripts interpreter "
                                                   "with installed module in "
                                                   "the External Tools settings"));

    U2DataPathRegistry* dpr = AppContext::getDataPathRegistry();
    if (dpr != nullptr) {
        auto dp = new U2DataPath(ADAPTERS_DATA_NAME, QString(PATH_PREFIX_DATA) + ":" + ADAPTERS_DIR_NAME, "", U2DataPath::CutFileExtension);
        dpr->registerEntry(dp);
    }

    if (isOsWindows()) {
        pathChecks << ExternalTool::PathChecks::NonLatinArguments
                   << ExternalTool::PathChecks::NonLatinToolPath;
    }
}

}  // namespace U2
