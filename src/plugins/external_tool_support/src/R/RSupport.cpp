/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#include "RSupport.h"

#include <QMainWindow>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/ScriptingToolRegistry.h>
#include <U2Core/U2SafePoints.h>

#include "ExternalToolSupportSettings.h"
#include "ExternalToolSupportSettingsController.h"
#include "conduct_go/ConductGOSupport.h"
#include "seqpos/SeqPosSupport.h"

namespace U2 {

const QString RSupport::ET_R_ID = "USUPP_RSCRIPT";
const QString RModuleGostatsSupport::ET_R_GOSTATS_ID = "USUPP_GOSTATS";
const QString RModuleGodbSupport::ET_R_GO_DB_ID = "USUPP_GO_DB";
const QString RModuleHgu133adbSupport::ET_R_HGU133A_DB_ID = "USUPP_HGU133A_DB";
const QString RModuleHgu133bdbSupport::ET_R_HGU133B_DB_ID = "USUPP_HGU133B_DB";
const QString RModuleHgu133plus2dbSupport::ET_R_HGU1333PLUS2_DB_ID = "USUPP_HGU133PLUS2_DB";
const QString RModuleHgu95av2dbSupport::ET_R_HGU95AV2_DB_ID = "USUPP_HGU95AV2_DB";
const QString RModuleMouse430a2dbSupport::ET_R_MOUSE430A2_DB_ID = "USUPP_MOUSE430A2_DB";
const QString RModuleCelegansdbSupport::ET_R_CELEGANS_DB_ID = "USUPP_CELEGANS_DB";
const QString RModuleDrosophila2dbSupport::ET_R_DROSOPHILA2_DB_ID = "USUPP_DROSOPHILA2_DB";
const QString RModuleOrghsegdbSupport::ET_R_ORG_HS_EG_DB_ID = "USUPP_ORG_HS_EG_DB";
const QString RModuleOrgmmegdbSupport::ET_R_ORG_MM_EG_DB_ID = "USUPP_ORG_MM_EG_DB";
const QString RModuleOrgceegdbSupport::ET_R_ORG_CE_EG_DB_ID = "USUPP_ORG_CE_EG_DB";
const QString RModuleOrgdmegdbSupport::ET_R_ORG_DM_EG_DB_ID = "USUPP_ORG_DM_EG_DB";
const QString RModuleSeqlogoSupport::ET_R_SEQLOGO_ID = "USUPP_SEQLOGO";

RSupport::RSupport()
    : RunnerTool(QStringList(), RSupport::ET_R_ID, "rscript", "Rscript") {
    if (AppContext::getMainWindow() != nullptr) {
        icon = QIcon(":external_tool_support/images/R.png");
        grayIcon = QIcon(":external_tool_support/images/R_gray.png");
        warnIcon = QIcon(":external_tool_support/images/R_warn.png");
    }

#ifdef Q_OS_WIN
    executableFileName = "Rscript.exe";
#else
#    if defined(Q_OS_UNIX)
    executableFileName = "Rscript";
#    endif
#endif
    validMessage = "R scripting front-end";
    validationArguments << "--version";

    description += tr("Rscript interpreter");
    versionRegExp = QRegExp("(\\d+.\\d+.\\d+)");
    toolKitName = "R";

    muted = true;
}

RModuleSupport::RModuleSupport(const QString &id, const QString &name)
    : ExternalToolModule(id, "rscript", name) {
    if (AppContext::getMainWindow() != nullptr) {
        icon = QIcon(":external_tool_support/images/R.png");
        grayIcon = QIcon(":external_tool_support/images/R_gray.png");
        warnIcon = QIcon(":external_tool_support/images/R_warn.png");
    }

#ifdef Q_OS_WIN
    executableFileName = "Rscript.exe";
#else
#    if defined(Q_OS_UNIX)
    executableFileName = "Rscript";
#    endif
#endif

    versionRegExp = QRegExp("(\\d+.\\d+.\\d+)");

    validationArguments << "-e";

    toolKitName = "R";
    dependencies << RSupport::ET_R_ID;

    errorDescriptions.insert("character(0)", tr("R module is not installed. "
                                                "Install module or set path "
                                                "to another R scripts interpreter "
                                                "with installed module in "
                                                "the External Tools settings"));

    muted = true;
}

QString RModuleSupport::getScript() const {
    return QString("list <- installed.packages();list[grep('%1',rownames(list))];list['%1','Version'];");
}

RModuleGostatsSupport::RModuleGostatsSupport()
    : RModuleSupport(RModuleGostatsSupport::ET_R_GOSTATS_ID, "GOstats") {
    description += name + tr(": Rscript module for the %1 tool").arg(ConductGOSupport::ET_GO_ANALYSIS);
    validationArguments << getScript().arg(name);
    validMessage = QString("\"%1\"").arg(name);
}

RModuleGodbSupport::RModuleGodbSupport()
    : RModuleSupport(RModuleGodbSupport::ET_R_GO_DB_ID, "GO.db") {
    description += name + tr(": Rscript module for the %1 tool").arg(ConductGOSupport::ET_GO_ANALYSIS);
    validationArguments << getScript().arg(name);
    validMessage = QString("\"%1\"").arg(name);
}

RModuleHgu133adbSupport::RModuleHgu133adbSupport()
    : RModuleSupport(RModuleHgu133adbSupport::ET_R_HGU133A_DB_ID, "hgu133a.db") {
    description += name + tr(": Rscript module for the %1 tool").arg(ConductGOSupport::ET_GO_ANALYSIS);
    validationArguments << getScript().arg(name);
    validMessage = QString("\"%1\"").arg(name);
}

RModuleHgu133bdbSupport::RModuleHgu133bdbSupport()
    : RModuleSupport(RModuleHgu133bdbSupport::ET_R_HGU133B_DB_ID, "hgu133b.db") {
    description += name + tr(": Rscript module for the %1 tool").arg(ConductGOSupport::ET_GO_ANALYSIS);
    validationArguments << getScript().arg(name);
    validMessage = QString("\"%1\"").arg(name);
}

RModuleHgu133plus2dbSupport::RModuleHgu133plus2dbSupport()
    : RModuleSupport(RModuleHgu133plus2dbSupport::ET_R_HGU1333PLUS2_DB_ID, "hgu133plus2.db") {
    description += name + tr(": Rscript module for the %1 tool").arg(ConductGOSupport::ET_GO_ANALYSIS);
    validationArguments << getScript().arg(name);
    validMessage = QString("\"%1\"").arg(name);
}

RModuleHgu95av2dbSupport::RModuleHgu95av2dbSupport()
    : RModuleSupport(RModuleHgu95av2dbSupport::ET_R_HGU95AV2_DB_ID, "hgu95av2.db") {
    description += name + tr(": Rscript module for the %1 tool").arg(ConductGOSupport::ET_GO_ANALYSIS);
    validationArguments << getScript().arg(name);
    validMessage = QString("\"%1\"").arg(name);
}

RModuleMouse430a2dbSupport::RModuleMouse430a2dbSupport()
    : RModuleSupport(RModuleMouse430a2dbSupport::ET_R_MOUSE430A2_DB_ID, "mouse430a2.db") {
    description += name + tr(": Rscript module for the %1 tool").arg(ConductGOSupport::ET_GO_ANALYSIS);
    validationArguments << getScript().arg(name);
    validMessage = QString("\"%1\"").arg(name);
}

RModuleCelegansdbSupport::RModuleCelegansdbSupport()
    : RModuleSupport(RModuleCelegansdbSupport::ET_R_CELEGANS_DB_ID, "celegans.db") {
    description += name + tr(": Rscript module for the %1 tool").arg(ConductGOSupport::ET_GO_ANALYSIS);
    validationArguments << getScript().arg(name);
    validMessage = QString("\"%1\"").arg(name);
}

RModuleDrosophila2dbSupport::RModuleDrosophila2dbSupport()
    : RModuleSupport(RModuleDrosophila2dbSupport::ET_R_DROSOPHILA2_DB_ID, "drosophila2.db") {
    description += name + tr(": Rscript module for the %1 tool").arg(ConductGOSupport::ET_GO_ANALYSIS);
    validationArguments << getScript().arg(name);
    validMessage = QString("\"%1\"").arg(name);
}

RModuleOrghsegdbSupport::RModuleOrghsegdbSupport()
    : RModuleSupport(RModuleOrghsegdbSupport::ET_R_ORG_HS_EG_DB_ID, "org.Hs.eg.db") {
    description += name + tr(": Rscript module for the %1 tool").arg(ConductGOSupport::ET_GO_ANALYSIS);
    validationArguments << getScript().arg(name);
    validMessage = QString("\"%1\"").arg(name);
}

RModuleOrgmmegdbSupport::RModuleOrgmmegdbSupport()
    : RModuleSupport(RModuleOrgmmegdbSupport::ET_R_ORG_MM_EG_DB_ID, "org.Mm.eg.db") {
    description += name + tr(": Rscript module for the %1 tool").arg(ConductGOSupport::ET_GO_ANALYSIS);
    validationArguments << getScript().arg(name);
    validMessage = QString("\"%1\"").arg(name);
}

RModuleOrgceegdbSupport::RModuleOrgceegdbSupport()
    : RModuleSupport(RModuleOrgceegdbSupport::ET_R_ORG_CE_EG_DB_ID, "org.Ce.eg.db") {
    description += name + tr(": Rscript module for the %1 tool").arg(ConductGOSupport::ET_GO_ANALYSIS);
    validationArguments << getScript().arg(name);
    validMessage = QString("\"%1\"").arg(name);
}

RModuleOrgdmegdbSupport::RModuleOrgdmegdbSupport()
    : RModuleSupport(RModuleOrgdmegdbSupport::ET_R_ORG_DM_EG_DB_ID, "org.Dm.eg.db") {
    description += name + tr(": Rscript module for the %1 tool").arg(ConductGOSupport::ET_GO_ANALYSIS);
    validationArguments << getScript().arg(name);
    validMessage = QString("\"%1\"").arg(name);
}

RModuleSeqlogoSupport::RModuleSeqlogoSupport()
    : RModuleSupport(RModuleSeqlogoSupport::ET_R_SEQLOGO_ID, "seqLogo") {
    description += name + tr(": Rscript module for the %1 tool").arg(SeqPosSupport::ET_SEQPOS);
    validationArguments << getScript().arg(name);
    validMessage = QString("\"%1\"").arg(name);
}

}    // namespace U2
