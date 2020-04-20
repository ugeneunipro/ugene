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

#include "ConductGOSupport.h"

#include <python/PythonSupport.h>
#include <R/RSupport.h>

#include <U2Core/AppContext.h>

namespace U2 {

const QString ConductGOSupport::ET_GO_ANALYSIS = "go_analysis";
const QString ConductGOSupport::ET_GO_ANALYSIS_ID = "USUPP_CONDUCT_GO_ANALYSIS";

ConductGOSupport::ConductGOSupport(const QString& id, const QString &name)
: ExternalTool(id, name, "")
{
    initialize();
}

void ConductGOSupport::initialize() {
    if (AppContext::getMainWindow()) {
        icon = QIcon(":external_tool_support/images/cmdline.png");
        grayIcon = QIcon(":external_tool_support/images/cmdline_gray.png");
        warnIcon = QIcon(":external_tool_support/images/cmdline_warn.png");
    }

    toolKitName = "Cistrome";
    description = ConductGOSupport::tr("<i>Conduct GO</i> - For a list of input genes, this tool uses R/BioC packages (GO, GOstats) to identify over represented GO terms.");

    executableFileName = "go_analysis.py";

    toolRunnerProgram = PythonSupport::ET_PYTHON_ID;
    dependencies << PythonSupport::ET_PYTHON_ID
                 << RSupport::ET_R_ID
                 << RModuleGostatsSupport::ET_R_GOSTATS_ID
                 << RModuleGodbSupport::ET_R_GO_DB_ID
                 << RModuleHgu133adbSupport::ET_R_HGU133A_DB_ID
                 << RModuleHgu133bdbSupport::ET_R_HGU133B_DB_ID
                 << RModuleHgu133plus2dbSupport::ET_R_HGU1333PLUS2_DB_ID
                 << RModuleHgu95av2dbSupport::ET_R_HGU95AV2_DB_ID
                 << RModuleMouse430a2dbSupport::ET_R_MOUSE430A2_DB_ID
                 << RModuleCelegansdbSupport::ET_R_CELEGANS_DB_ID
                 << RModuleDrosophila2dbSupport::ET_R_DROSOPHILA2_DB_ID
                 << RModuleOrghsegdbSupport::ET_R_ORG_HS_EG_DB_ID
                 << RModuleOrgmmegdbSupport::ET_R_ORG_MM_EG_DB_ID
                 << RModuleOrgceegdbSupport::ET_R_ORG_CE_EG_DB_ID
                 << RModuleOrgdmegdbSupport::ET_R_ORG_DM_EG_DB_ID;

    validMessage = "Conduct GO";
    validationArguments << "--version";

    versionRegExp = QRegExp("Conduct GO (\\d+\\.\\d+(\\.\\d+)?)");

    muted = true;
}

} // U2
