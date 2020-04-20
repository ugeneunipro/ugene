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

#include "RSupport.h"
#include "ExternalToolSupportSettingsController.h"
#include "ExternalToolSupportSettings.h"
#include "conduct_go/ConductGOSupport.h"
#include "seqpos/SeqPosSupport.h"

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/ScriptingToolRegistry.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Gui/MainWindow.h>
#include <QMainWindow>

namespace U2 {

const QString RSupport::ET_R = "Rscript";
const QString RSupport::ET_R_ID = "USUPP_RSCRIPT";
const QString RModuleGostatsSupport::ET_R_GOSTATS = "GOstats";
const QString RModuleGostatsSupport::ET_R_GOSTATS_ID = "USUPP_GOSTATS";
const QString RModuleGodbSupport::ET_R_GO_DB = "GO.db";
const QString RModuleGodbSupport::ET_R_GO_DB_ID = "USUPP_GO_DB";
const QString RModuleHgu133adbSupport::ET_R_HGU133A_DB = "hgu133a.db";
const QString RModuleHgu133adbSupport::ET_R_HGU133A_DB_ID = "USUPP_HGU133A_DB";
const QString RModuleHgu133bdbSupport::ET_R_HGU133B_DB = "hgu133b.db";
const QString RModuleHgu133bdbSupport::ET_R_HGU133B_DB_ID = "USUPP_HGU133B_DB";
const QString RModuleHgu133plus2dbSupport::ET_R_HGU1333PLUS2_DB = "hgu133plus2.db";
const QString RModuleHgu133plus2dbSupport::ET_R_HGU1333PLUS2_DB_ID = "USUPP_HGU133PLUS2_DB";
const QString RModuleHgu95av2dbSupport::ET_R_HGU95AV2_DB = "hgu95av2.db";
const QString RModuleHgu95av2dbSupport::ET_R_HGU95AV2_DB_ID = "USUPP_HGU95AV2_DB";
const QString RModuleMouse430a2dbSupport::ET_R_MOUSE430A2_DB = "mouse430a2.db";
const QString RModuleMouse430a2dbSupport::ET_R_MOUSE430A2_DB_ID = "USUPP_MOUSE430A2_DB";
const QString RModuleCelegansdbSupport::ET_R_CELEGANS_DB = "celegans.db";
const QString RModuleCelegansdbSupport::ET_R_CELEGANS_DB_ID = "USUPP_CELEGANS_DB";
const QString RModuleDrosophila2dbSupport::ET_R_DROSOPHILA2_DB = "drosophila2.db";
const QString RModuleDrosophila2dbSupport::ET_R_DROSOPHILA2_DB_ID = "USUPP_DROSOPHILA2_DB";
const QString RModuleOrghsegdbSupport::ET_R_ORG_HS_EG_DB = "org.Hs.eg.db";
const QString RModuleOrghsegdbSupport::ET_R_ORG_HS_EG_DB_ID = "USUPP_ORG_HS_EG_DB";
const QString RModuleOrgmmegdbSupport::ET_R_ORG_MM_EG_DB = "org.Mm.eg.db";
const QString RModuleOrgmmegdbSupport::ET_R_ORG_MM_EG_DB_ID = "USUPP_ORG_MM_EG_DB";
const QString RModuleOrgceegdbSupport::ET_R_ORG_CE_EG_DB = "org.Ce.eg.db";
const QString RModuleOrgceegdbSupport::ET_R_ORG_CE_EG_DB_ID = "USUPP_ORG_CE_EG_DB";
const QString RModuleOrgdmegdbSupport::ET_R_ORG_DM_EG_DB = "org.Dm.eg.db";
const QString RModuleOrgdmegdbSupport::ET_R_ORG_DM_EG_DB_ID = "USUPP_ORG_DM_EG_DB";
const QString RModuleSeqlogoSupport::ET_R_SEQLOGO = "seqLogo";
const QString RModuleSeqlogoSupport::ET_R_SEQLOGO_ID = "USUPP_SEQLOGO";

RSupport::RSupport(const QString& id, const QString& name, const QString& path)
    : RunnerTool(QStringList(), id, name, path)
{
    if (AppContext::getMainWindow()) {
        icon = QIcon(":external_tool_support/images/R.png");
        grayIcon = QIcon(":external_tool_support/images/R_gray.png");
        warnIcon = QIcon(":external_tool_support/images/R_warn.png");
    }

#ifdef Q_OS_WIN
    executableFileName = "Rscript.exe";
#else
#if defined(Q_OS_UNIX)
    executableFileName = "Rscript";
#endif
#endif
    validMessage = "R scripting front-end";
    validationArguments << "--version";

    description+=tr("Rscript interpreter");
    versionRegExp = QRegExp("(\\d+.\\d+.\\d+)");
    toolKitName = "R";

    muted = true;
}

RModuleSupport::RModuleSupport(const QString& id, const QString& name) : ExternalToolModule(id, name) {
    if (AppContext::getMainWindow()) {
        icon = QIcon(":external_tool_support/images/R.png");
        grayIcon = QIcon(":external_tool_support/images/R_gray.png");
        warnIcon = QIcon(":external_tool_support/images/R_warn.png");
    }

#ifdef Q_OS_WIN
    executableFileName = "Rscript.exe";
#else
    #if defined(Q_OS_UNIX)
    executableFileName = "Rscript";
    #endif
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

RModuleGostatsSupport::RModuleGostatsSupport(const QString &id, const QString &name) : RModuleSupport(id, name) {
    description += ET_R_GOSTATS + tr(": Rscript module for the %1 tool").arg(ConductGOSupport::ET_GO_ANALYSIS);
    validationArguments << getScript().arg(ET_R_GOSTATS);
    validMessage = QString("\"%1\"").arg(ET_R_GOSTATS);
}

RModuleGodbSupport::RModuleGodbSupport(const QString &id, const QString &name) : RModuleSupport(id, name) {
    description += ET_R_GO_DB + tr(": Rscript module for the %1 tool").arg(ConductGOSupport::ET_GO_ANALYSIS);
    validationArguments << getScript().arg(ET_R_GO_DB);
    validMessage = QString("\"%1\"").arg(ET_R_GO_DB);
}

RModuleHgu133adbSupport::RModuleHgu133adbSupport(const QString &id, const QString &name) : RModuleSupport(id, name) {
    description += ET_R_HGU133A_DB + tr(": Rscript module for the %1 tool").arg(ConductGOSupport::ET_GO_ANALYSIS);
    validationArguments << getScript().arg(ET_R_HGU133A_DB);
    validMessage = QString("\"%1\"").arg(ET_R_HGU133A_DB);
}

RModuleHgu133bdbSupport::RModuleHgu133bdbSupport(const QString &id, const QString &name) : RModuleSupport(id, name) {
    description += ET_R_HGU133B_DB + tr(": Rscript module for the %1 tool").arg(ConductGOSupport::ET_GO_ANALYSIS);
    validationArguments << getScript().arg(ET_R_HGU133B_DB);
    validMessage = QString("\"%1\"").arg(ET_R_HGU133B_DB);
}

RModuleHgu133plus2dbSupport::RModuleHgu133plus2dbSupport(const QString &id, const QString &name) : RModuleSupport(id, name) {
    description += ET_R_HGU1333PLUS2_DB + tr(": Rscript module for the %1 tool").arg(ConductGOSupport::ET_GO_ANALYSIS);
    validationArguments << getScript().arg(ET_R_HGU1333PLUS2_DB);
    validMessage = QString("\"%1\"").arg(ET_R_HGU1333PLUS2_DB);
}

RModuleHgu95av2dbSupport::RModuleHgu95av2dbSupport(const QString &id, const QString &name) : RModuleSupport(id, name) {
    description += ET_R_HGU95AV2_DB + tr(": Rscript module for the %1 tool").arg(ConductGOSupport::ET_GO_ANALYSIS);
    validationArguments << getScript().arg(ET_R_HGU95AV2_DB);
    validMessage = QString("\"%1\"").arg(ET_R_HGU95AV2_DB);
}

RModuleMouse430a2dbSupport::RModuleMouse430a2dbSupport(const QString &id, const QString &name) : RModuleSupport(id, name) {
    description += ET_R_MOUSE430A2_DB + tr(": Rscript module for the %1 tool").arg(ConductGOSupport::ET_GO_ANALYSIS);
    validationArguments << getScript().arg(ET_R_MOUSE430A2_DB);
    validMessage = QString("\"%1\"").arg(ET_R_MOUSE430A2_DB);
}

RModuleCelegansdbSupport::RModuleCelegansdbSupport(const QString &id, const QString &name) : RModuleSupport(id, name) {
    description += ET_R_CELEGANS_DB + tr(": Rscript module for the %1 tool").arg(ConductGOSupport::ET_GO_ANALYSIS);
    validationArguments << getScript().arg(ET_R_CELEGANS_DB);
    validMessage = QString("\"%1\"").arg(ET_R_CELEGANS_DB);
}

RModuleDrosophila2dbSupport::RModuleDrosophila2dbSupport(const QString &id, const QString &name) : RModuleSupport(id, name) {
    description += ET_R_DROSOPHILA2_DB + tr(": Rscript module for the %1 tool").arg(ConductGOSupport::ET_GO_ANALYSIS);
    validationArguments << getScript().arg(ET_R_DROSOPHILA2_DB);
    validMessage = QString("\"%1\"").arg(ET_R_DROSOPHILA2_DB);
}

RModuleOrghsegdbSupport::RModuleOrghsegdbSupport(const QString &id, const QString &name) : RModuleSupport(id, name) {
    description += ET_R_ORG_HS_EG_DB + tr(": Rscript module for the %1 tool").arg(ConductGOSupport::ET_GO_ANALYSIS);
    validationArguments << getScript().arg(ET_R_ORG_HS_EG_DB);
    validMessage = QString("\"%1\"").arg(ET_R_ORG_HS_EG_DB);
}

RModuleOrgmmegdbSupport::RModuleOrgmmegdbSupport(const QString &id, const QString &name) : RModuleSupport(id, name) {
    description += ET_R_ORG_MM_EG_DB + tr(": Rscript module for the %1 tool").arg(ConductGOSupport::ET_GO_ANALYSIS);
    validationArguments << getScript().arg(ET_R_ORG_MM_EG_DB);
    validMessage = QString("\"%1\"").arg(ET_R_ORG_MM_EG_DB);
}

RModuleOrgceegdbSupport::RModuleOrgceegdbSupport(const QString &id, const QString &name) : RModuleSupport(id, name) {
    description += ET_R_ORG_CE_EG_DB + tr(": Rscript module for the %1 tool").arg(ConductGOSupport::ET_GO_ANALYSIS);
    validationArguments << getScript().arg(ET_R_ORG_CE_EG_DB);
    validMessage = QString("\"%1\"").arg(ET_R_ORG_CE_EG_DB);
}

RModuleOrgdmegdbSupport::RModuleOrgdmegdbSupport(const QString &id, const QString &name) : RModuleSupport(id, name) {
    description += ET_R_ORG_DM_EG_DB + tr(": Rscript module for the %1 tool").arg(ConductGOSupport::ET_GO_ANALYSIS);
    validationArguments << getScript().arg(ET_R_ORG_DM_EG_DB);
    validMessage = QString("\"%1\"").arg(ET_R_ORG_DM_EG_DB);
}

RModuleSeqlogoSupport::RModuleSeqlogoSupport(const QString &id, const QString &name) : RModuleSupport(id, name) {
    description += ET_R_SEQLOGO + tr(": Rscript module for the %1 tool").arg(SeqPosSupport::ET_SEQPOS);
    validationArguments << getScript().arg(ET_R_SEQLOGO);
    validMessage = QString("\"%1\"").arg(ET_R_SEQLOGO);
}

}   // namespace
