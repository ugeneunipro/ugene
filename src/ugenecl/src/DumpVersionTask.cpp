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

#include <cstdio>

#include <U2Core/AppContext.h>
#include <U2Core/CMDLineHelpProvider.h>
#include <U2Core/CMDLineRegistry.h>

#include "DumpHelpTask.h"
#include "DumpVersionTask.h"

namespace U2 {

const QString DumpVersionTask::VERSION_CMDLINE_OPTION       = "version";
const QString DumpVersionTask::VERSION_CMDLINE_OPTION_SHORT = "v";

void DumpVersionTask::initHelp() {
    CMDLineHelpProvider * versionSection = new CMDLineHelpProvider(
        VERSION_CMDLINE_OPTION,
        tr( "Shows version information." ),
        "", // No full description
        "", // No arguments
        VERSION_CMDLINE_OPTION_SHORT);

    AppContext::getCMDLineRegistry()->registerCMDLineHelpProvider( versionSection );
}

DumpVersionTask::DumpVersionTask() : Task(tr("Dump version information task"), TaskFlag_None) {
}

void DumpVersionTask::run() {
    fprintf(stdout, "%s", DumpHelpTask::VERSION_INFO.toLatin1().constData());
    fprintf(stdout, "Using Qt %s\n", qVersion());
}

} // U2
