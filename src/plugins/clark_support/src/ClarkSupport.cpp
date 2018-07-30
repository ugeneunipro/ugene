/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

ClarkSupport::ClarkSupport(const QString& name, const QString& path) : ExternalTool(name, path)
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
    validMessage = name;

    if (name == ET_CLARK) {
        description += tr("One of the classifiers from the CLARK framework. This tool is created for powerful workstations and can require a significant amount of RAM.");
        versionRegExp = QRegExp("Version: (\\d+\\.\\d+\\.?\\d*\\.?\\d*)");
    } else if (name == ET_CLARK_L) {
        description += tr("One of the classifiers from the CLARK framework. This tool is created for workstations with limited memory (i.e., “l” for light), it provides precise classification on small metagenomes.");
        versionRegExp = QRegExp("Version: (\\d+\\.\\d+\\.?\\d*\\.?\\d*)");
        validMessage = "CLARK";
    } else {
        description += tr("Used to set up metagenomic database for CLARK. ");
    }

    connect(this, SIGNAL(si_toolValidationStatusChanged(bool)), SLOT(sl_toolValidationStatusChanged(bool)));
}

void ClarkSupport::registerTools(ExternalToolRegistry *etRegistry)
{
    etRegistry->registerEntry(new ClarkSupport(ET_CLARK));
    etRegistry->registerEntry(new ClarkSupport(ET_CLARK_L));
    etRegistry->registerEntry(new ClarkSupport(ET_CLARK_getAccssnTaxID));
    etRegistry->registerEntry(new ClarkSupport(ET_CLARK_getfilesToTaxNodes));
    etRegistry->registerEntry(new ClarkSupport(ET_CLARK_getTargetsDef));
    etRegistry->registerEntry(new ClarkSupport(ET_CLARK_buildScript));
    etRegistry->setToolkitDescription(CLARK_GROUP, tr("CLARK (CLAssifier based on Reduced K-mers) is a tool for supervised sequence classification "
        "based on discriminative k-mers. UGENE provides the GUI for CLARK and CLARK-l variants of the CLARK framework "
                                                      "for solving the problem of the assignment of metagenomic reads to known genomes."));
}

void ClarkSupport::unregisterTools(ExternalToolRegistry *etRegistry) {
    etRegistry->unregisterEntry(ET_CLARK);
    etRegistry->unregisterEntry(ET_CLARK_L);
    etRegistry->unregisterEntry(ET_CLARK_getAccssnTaxID);
    etRegistry->unregisterEntry(ET_CLARK_getfilesToTaxNodes);
    etRegistry->unregisterEntry(ET_CLARK_getTargetsDef);
    etRegistry->unregisterEntry(ET_CLARK_buildScript);
}

void ClarkSupport::sl_toolValidationStatusChanged(bool) {
    ScriptingTool::onPathChanged(this);
}


}//namespace
