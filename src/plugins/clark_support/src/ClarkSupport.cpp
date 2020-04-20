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

#include "ClarkSupport.h"

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/ScriptingToolRegistry.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Gui/MainWindow.h>
#include <QMainWindow>

namespace U2 {

const QString ClarkSupport::CLARK_GROUP = "CLARK";
const QString ClarkSupport::ET_CLARK = "CLARK";
const QString ClarkSupport::ET_CLARK_ID = "USUPP_CLARK";
const QString ClarkSupport::ET_CLARK_L = "CLARK-l";
const QString ClarkSupport::ET_CLARK_L_ID = "USUPP_CLARK_L";
const QString ClarkSupport::ET_CLARK_BUILD_SCRIPT = "builddb.sh";
const QString ClarkSupport::ET_CLARK_BUILD_SCRIPT_ID = "USUPP_CLARK_BUILD_DB";
const QString ClarkSupport::ET_CLARK_GET_ACCSSN_TAX_ID = "getAccssnTaxID";
const QString ClarkSupport::ET_CLARK_GET_ACCSSN_TAX_ID_ID = "USUPP_CLARK_GET_ACCSSN_TAX_ID";
const QString ClarkSupport::ET_CLARK_GET_TARGETS_DEF = "getTargetsDef";
const QString ClarkSupport::ET_CLARK_GET_TARGETS_DEF_ID = "USUPP_CLARK_GET_TARGETS_DEF";
const QString ClarkSupport::ET_CLARK_GET_FILES_TO_TAX_NODES = "getfilesToTaxNodes";
const QString ClarkSupport::ET_CLARK_GET_FILES_TO_TAX_NODES_ID = "USUPP_CLARK_GET_FILES_TO_TAX_NODES";

ClarkSupport::ClarkSupport(const QString& id, const QString& name, const QString& path) : ExternalTool(id, name, path)
{
    if (AppContext::getMainWindow()) {
        icon = QIcon(":external_tool_support/images/cmdline.png");
        grayIcon = QIcon(":external_tool_support/images/cmdline_gray.png");
        warnIcon = QIcon(":external_tool_support/images/cmdline_warn.png");
    }
#ifdef Q_OS_WIN
    executableFileName = name + ".exe";
#else
//    #if defined(Q_OS_UNIX)
    executableFileName = name;
//    #endif
#endif
    toolKitName = CLARK_GROUP;
    muted = true;
    validMessage = "UGENE-customized .*" + name;

    if (name == ET_CLARK) {
        description += tr("One of the classifiers from the CLARK framework. This tool is created for powerful workstations and can require a significant amount of RAM.<br><br>"
                          "Note that a UGENE-customized version of the tool is required.");
        versionRegExp = QRegExp("Version: (\\d+\\.\\d+\\.?\\d*\\.?\\d*)");
    } else if (name == ET_CLARK_L) {
        description += tr("One of the classifiers from the CLARK framework. This tool is created for workstations with limited memory (i.e., “l” for light), it provides precise classification on small metagenomes.<br><br>"
                          "Note that a UGENE-customized version of the tool is required.");
        versionRegExp = QRegExp("Version: (\\d+\\.\\d+\\.?\\d*\\.?\\d*)");
        validMessage = "UGENE-customized CLARK";
    } else {
        description += tr("Used to set up metagenomic database for CLARK.<br><br>"
                          "Note that a UGENE-customized version of the tool is required.");
    }

    if (name == ET_CLARK_BUILD_SCRIPT) {
        validMessage = name;
    }
}

void ClarkSupport::registerTools(ExternalToolRegistry *etRegistry)
{
    etRegistry->registerEntry(new ClarkSupport(ET_CLARK_ID, ET_CLARK, ""));
    etRegistry->registerEntry(new ClarkSupport(ET_CLARK_L_ID, ET_CLARK_L, ""));
    etRegistry->registerEntry(new ClarkSupport(ET_CLARK_GET_ACCSSN_TAX_ID_ID, ET_CLARK_GET_ACCSSN_TAX_ID, ""));
    etRegistry->registerEntry(new ClarkSupport(ET_CLARK_GET_FILES_TO_TAX_NODES_ID, ET_CLARK_GET_FILES_TO_TAX_NODES, ""));
    etRegistry->registerEntry(new ClarkSupport(ET_CLARK_GET_TARGETS_DEF_ID, ET_CLARK_GET_TARGETS_DEF, ""));
    etRegistry->registerEntry(new ClarkSupport(ET_CLARK_BUILD_SCRIPT_ID, ET_CLARK_BUILD_SCRIPT, ""));
    etRegistry->setToolkitDescription(CLARK_GROUP, tr("CLARK (CLAssifier based on Reduced K-mers) is a tool for supervised sequence classification "
        "based on discriminative k-mers. UGENE provides the GUI for CLARK and CLARK-l variants of the CLARK framework "
                                                      "for solving the problem of the assignment of metagenomic reads to known genomes."));
}

void ClarkSupport::unregisterTools(ExternalToolRegistry *etRegistry) {
    etRegistry->unregisterEntry(ET_CLARK_ID);
    etRegistry->unregisterEntry(ET_CLARK_L_ID);
    etRegistry->unregisterEntry(ET_CLARK_GET_ACCSSN_TAX_ID_ID);
    etRegistry->unregisterEntry(ET_CLARK_GET_FILES_TO_TAX_NODES_ID);
    etRegistry->unregisterEntry(ET_CLARK_GET_TARGETS_DEF_ID);
    etRegistry->unregisterEntry(ET_CLARK_BUILD_SCRIPT_ID);
}


}//namespace
