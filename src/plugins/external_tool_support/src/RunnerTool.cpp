/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "RunnerTool.h"

#include <U2Core/ScriptingToolRegistry.h>

namespace U2 {

RunnerTool::RunnerTool(const QStringList& _runParameters, const QString& id, const QString& dirName, const QString& name, const QString& path)
    : ExternalTool(id, dirName, name, path), runParameters(_runParameters) {
    isRunnerTool = true;
    connect(this, SIGNAL(si_toolValidationStatusChanged(bool)), SLOT(sl_toolValidationStatusChanged(bool)));
}

QStringList RunnerTool::getRunParameters() const {
    return runParameters;
}

void RunnerTool::sl_toolValidationStatusChanged(bool isValid) {
    Q_UNUSED(isValid);
    ScriptingTool::onPathChanged(this, runParameters);
}

}  // namespace U2