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

#include <U2Core/U2SafePoints.h>

#include <U2Lang/HRSchemaSerializer.h>

#include "ActorScriptValidator.h"
#include "ActorValidator.h"

namespace U2 {
using namespace WorkflowSerialize;
namespace Workflow {

bool ActorValidator::validate(const Configuration *cfg, NotificationsList &notificationList) const {
    const Actor *actor = static_cast<const Actor*>(cfg);
    SAFE_POINT(NULL != actor, "NULL actor", false);
    QMap<QString, QString> options;
    return validate(actor, notificationList, options);
}

ActorValidatorRegistry::~ActorValidatorRegistry() {
    qDeleteAll(validators);
}

ActorValidatorRegistry::ActorValidatorRegistry() {
    addValidator(Constants::V_SCRIPT, new ActorScriptValidator());
}

bool ActorValidatorRegistry::addValidator(const QString &id, ActorValidator *validator) {
    QMutexLocker lock(&mutex);
    if (validators.contains(id)) {
        return false;
    }
    validators[id] = validator;
    return true;
}

ActorValidator * ActorValidatorRegistry::findValidator(const QString &id) {
    QMutexLocker lock(&mutex);
    return validators.value(id, NULL);
}

} // Workflow
} // U2
