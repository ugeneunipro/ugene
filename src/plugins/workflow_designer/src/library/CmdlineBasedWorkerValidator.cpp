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

#include <U2Lang/ActorPrototype.h>
#include <U2Lang/ExternalToolCfg.h>
#include <U2Lang/WorkflowEnv.h>

#include "CmdlineBasedWorkerValidator.h"
#include "util/CustomWorkerUtils.h"

namespace U2 {
namespace Workflow {

bool CmdlineBasedWorkerValidator::validate(const Actor* actor, NotificationsList& notificationList, const QMap<QString, QString>& options) const {
    ExternalProcessConfig* config = WorkflowEnv::getExternalCfgRegistry()->getConfigById(actor->getProto()->getId());
    if (CustomWorkerUtils::commandContainsVarName(config->cmdLine, CustomWorkerUtils::TOOL_PATH_VAR_NAME)) {
        CHECK_EXT(QFile(config->customToolPath).exists(),
                  notificationList << WorkflowNotification(tr("The element specifies a nonexistent path to an external tool executable."), actor->getId()),
                  false);
        CHECK_EXT(QFileInfo(config->customToolPath).isFile(),
                  notificationList << WorkflowNotification(tr("The element should specify an executable file."), actor->getId()),
                  false);
    }

    return true;
}

}
}