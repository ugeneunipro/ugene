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

#include <QMutexLocker>

#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Lang/HRSchemaSerializer.h>

#include "SchemaActorsRegistry.h"

namespace U2 {
namespace Workflow {

bool SchemaActorsRegistry::registerSchema(const QString &protoId, Schema *schema) {
    QMutexLocker lock(&mutex);
    if (schemas.keys().contains(protoId)) {
        return false;
    }

    schemas.insert(protoId, schema);
    return true;
}

Schema *SchemaActorsRegistry::unregisterSchema(const QString &protoId) {
    QMutexLocker lock(&mutex);
    Schema *res = schemas.value(protoId, NULL);
    schemas.remove(protoId);
    return res;
}

Schema *SchemaActorsRegistry::getSchema(const QString &protoId) {
    QMutexLocker lock(&mutex);
    Schema *schema = schemas.value(protoId, NULL);
    if (NULL == schema) {
        return NULL;
    }
    Schema *copy = new Schema();
    U2OpStatusImpl os;
    HRSchemaSerializer::deepCopy(*schema, copy, os);
    SAFE_POINT_EXT(!os.isCoR(), copy->reset(), NULL);
    return copy;
}

} // Workflow
} // U2
