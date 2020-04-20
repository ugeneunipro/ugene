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

#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QXmlInputSource>
#include <QXmlSimpleReader>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/Counter.h>
#include <U2Core/CustomExternalTool.h>
#include <U2Core/GUrl.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include "CustomToolConfigParser.h"
#include "ImportCustomToolsTask.h"
#include "RegisterCustomToolTask.h"

namespace U2 {

const QString ImportCustomToolsTask::SETTINGS_PATH = "external_tools/custom_tool_configs";

ImportCustomToolsTask::ImportCustomToolsTask(const QString &_url)
    : Task(tr("Import custom external tools configuration"), TaskFlags_FOSE_COSC|TaskFlag_CollectChildrenWarnings),
      url(_url),
      registerTask(nullptr)
{
    GCOUNTER(cvar, tvar, "ImportCustomToolsTask");
}

void ImportCustomToolsTask::prepare() {
    registerTask = new RegisterCustomToolTask(url);
    addSubTask(registerTask);
}

void ImportCustomToolsTask::run() {
    CustomExternalTool *tool = registerTask->getTool();
    CHECK(nullptr != tool, );
    saveToolConfig(tool);
}

void ImportCustomToolsTask::saveToolConfig(CustomExternalTool *tool) {
    QDomDocument doc = CustomToolConfigParser::serialize(tool);

    const QString storagePath = AppContext::getAppSettings()->getUserAppsSettings()->getCustomToolsConfigsDirPath();
    QDir().mkpath(storagePath);

    const QString url = GUrlUtils::rollFileName(storagePath + "/" + GUrlUtils::fixFileName(tool->getId()) + ".xml", "_");
    QFile configFile(url);
    configFile.open(QIODevice::WriteOnly);
    QTextStream stream(&configFile);
    stream << doc.toString(4);
    configFile.close();

    tool->setConfigFilePath(url);
}

}   // namespace U2
