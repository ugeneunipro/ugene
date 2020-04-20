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

#include <QRegularExpression>

#include <U2Core/AppContext.h>
#include <U2Core/Log.h>
#include <U2Core/U2SafePoints.h>

#include "CustomWorkerUtils.h"

namespace U2 {
namespace Workflow {

const QString CustomWorkerUtils::TOOL_PATH_VAR_NAME = "TOOL_PATH";

QString CustomWorkerUtils::getVarName(const ExternalTool *tool) {
    SAFE_POINT(!tool->isModule(),
               "Bad external tool type",
               "__UGENE_BAD_EXTERNAL_TOOL_TYPE__");
    QString id = tool->getId();
    SAFE_POINT((id.indexOf(QRegularExpression("[^A-Za-z0-9_-]")) < 0),
               "Bad external tool id",
               "__UGENE_BAD_EXTERNAL_TOOL_ID__");

    QString virtId = (tool->isCustom() ? "UCUST_" + id : id).toUpper();

    return virtId;
}

bool CustomWorkerUtils::commandContainsSpecialTool(const QString &cmd, const QString toolId) {
    ExternalTool *tool = AppContext::getExternalToolRegistry()->getById(toolId);
    if (tool) {
        return commandContainsSpecialTool(cmd, tool);
    }
    return false;
}

bool CustomWorkerUtils::commandContainsSpecialTool(const QString &cmd, const ExternalTool *tool) {
    QString varName = getVarName(tool);
    return commandContainsVarName(cmd, varName);
}

bool CustomWorkerUtils::commandContainsVarName(const QString& cmd, const QString& varName) {
    QRegularExpression regex1 = QRegularExpression(CMDTOOL_SPECIAL_REGEX + ("%" + varName + "%"));
    return cmd.indexOf(regex1) >= 0;
}

QStringList CustomWorkerUtils::getToolIdsFromCommand(const QString& cmd) {
    QRegularExpression regex1 = QRegularExpression(CMDTOOL_SPECIAL_REGEX + QString("%([A-Za-z0-9_-]+)%"));
    QRegularExpressionMatchIterator match = regex1.globalMatch(cmd);
    QStringList result;
    while (match.hasNext()) {
        QString id = match.next().captured().remove(QRegularExpression("([^A-Za-z0-9_-]*)"));
        CHECK_CONTINUE(id.startsWith("USUPP_") || id.startsWith("UCUST_"));

        if (id.startsWith("UCUST_")) {
            id = id.right(id.size() - 6);
        }
        result << id;
    }
    return result;
}

bool CustomWorkerUtils::commandReplaceSpecialByUgenePath(QString &cmd, const QString varName, const QString path) {
    SAFE_POINT(!(varName.isNull() || varName.isEmpty()),
               "Bad varName",
               false);
    bool result = false;
    QRegularExpression regex1 = QRegularExpression(CMDTOOL_SPECIAL_REGEX + ("%" + varName + "%"));
    while (cmd.indexOf(regex1) >= 0) {
        cmd.replace(regex1, "\\1\"" + path + "\"");
        result |= true;
    }

    return result;
}

bool CustomWorkerUtils::commandReplaceSpecialByUgenePath(QString &cmd, const ExternalTool *tool) {
    QString varName = getVarName(tool);
    return commandReplaceSpecialByUgenePath(cmd, varName, tool->getPath());
}

void CustomWorkerUtils::commandReplaceAllSpecialByUgenePath(QString &cmd, ExternalProcessConfig *cfg) {
    commandReplaceSpecialByUgenePath(cmd, TOOL_PATH_VAR_NAME, cfg->customToolPath);

    QList<ExternalTool *> all = AppContext::getExternalToolRegistry()->getAllEntries();
    for (auto tool : all) {
        if (!tool->isModule()) {
            commandReplaceSpecialByUgenePath(cmd, tool);
        }
    }
}


} // Workflow
} // U2
