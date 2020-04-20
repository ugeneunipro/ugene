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

#include <QScopedPointer>

#include <U2Core/AppContext.h>
#include <U2Core/CustomExternalTool.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/U2SafePoints.h>

#include "CustomToolConfigParser.h"
#include "RegisterCustomToolTask.h"

namespace U2 {

RegisterCustomToolTask::RegisterCustomToolTask(const QString &_url)
    : Task(tr("Register custom external tool"), TaskFlag_None),
      url(_url),
      registeredTool(nullptr)
{

}

CustomExternalTool *RegisterCustomToolTask::getTool() const {
    return registeredTool;
}

void RegisterCustomToolTask::run() {
    QScopedPointer<CustomExternalTool> tool(CustomToolConfigParser::parse(stateInfo, url));
    CHECK_OP(stateInfo, );

    tool->setConfigFilePath(url);

    const bool registered = AppContext::getExternalToolRegistry()->registerEntry(tool.data());
    if (registered) {
        registeredTool = tool.take();
    } else {
        setError(tr("Can't register a custom external tool '%1'").arg(tool->getName()));
    }
}

}   // namespace U2
