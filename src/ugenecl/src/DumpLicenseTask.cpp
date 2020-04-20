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

#include <QFile>

#include <U2Core/AppContext.h>
#include <U2Core/CMDLineRegistry.h>
#include <U2Core/CMDLineHelpProvider.h>

#include "DumpLicenseTask.h"

namespace U2 {

const QString DumpLicenseTask::LICENSE_CMDLINE_OPTION = "license";
const QString DumpLicenseTask::LICENCE_FILENAME = "license";

void DumpLicenseTask::initHelp() {
    CMDLineRegistry * cmdlineRegistry = AppContext::getCMDLineRegistry();

    CMDLineHelpProvider * helpSection = new CMDLineHelpProvider(
        LICENSE_CMDLINE_OPTION,
        tr( "Shows license information." ));

    cmdlineRegistry->registerCMDLineHelpProvider(helpSection);
}

DumpLicenseTask::DumpLicenseTask() : Task(tr("Dump license information task"), TaskFlag_None) {
}

void DumpLicenseTask::run() {
    QFile file(QString( PATH_PREFIX_DATA ) + ":" + LICENCE_FILENAME );
    if(!file.open(QIODevice::ReadOnly)) {
        setError("Cannot find license file");
        return;
    }
    QByteArray licenseText = file.readAll();
    fprintf(stdout, "%s", licenseText.constData());
}

} // U2
