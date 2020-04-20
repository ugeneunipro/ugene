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

#ifndef _U2_CUSTOM_WORKER_UTILS_H_
#define _U2_CUSTOM_WORKER_UTILS_H_

#include <QObject>

#include <U2Lang/ExternalToolCfg.h>
#include "U2Core/ExternalToolRegistry.h"

namespace U2 {
namespace Workflow {

#define CMDTOOL_SPECIAL_REGEX "((?<!(\\\\))(\\\\\\\\)*|^)"

class CustomWorkerUtils {

public:
    static const QString TOOL_PATH_VAR_NAME;

    static QString getVarName(const ExternalTool *tool);

    static bool commandContainsSpecialTool(const QString &cmd, const ExternalTool *tool);
    static bool commandContainsSpecialTool(const QString &cmd, const QString toolId);
    static bool commandContainsVarName(const QString& cmd, const QString& varName);
    static QStringList getToolIdsFromCommand(const QString& cmd);

    static bool commandReplaceSpecialByUgenePath(QString &cmd, const ExternalTool *tool);
    static bool commandReplaceSpecialByUgenePath(QString &cmd, const QString varName, const QString path);
    static void commandReplaceAllSpecialByUgenePath(QString &cmd, ExternalProcessConfig *cfg);

};

} // Workflow
} // U2

#endif // _U2_CUSTOM_WORKER_UTILS_H_
