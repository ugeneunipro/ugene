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

#include <QRegExp>

#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "UdrSchemaRegistry.h"

namespace U2 {

UdrSchemaRegistry::UdrSchemaRegistry()
: mutex(QMutex::Recursive)
{

}

UdrSchemaRegistry::~UdrSchemaRegistry() {
    qDeleteAll(schemas.values());
}

void UdrSchemaRegistry::registerSchema(const UdrSchema *schema, U2OpStatus &os) {
    QMutexLocker lock(&mutex);
    CHECK_EXT(NULL != schema, os.setError("NULL schema"), );
    CHECK_EXT(isCorrectName(schema->getId()), os.setError("Incorrect schema id"), );
    CHECK_EXT(!schemas.contains(schema->getId()), os.setError("Duplicate schema id"), );

    schemas[schema->getId()] = schema;
}

QList<UdrSchemaId> UdrSchemaRegistry::getRegisteredSchemas() const {
    QMutexLocker lock(&mutex);
    return schemas.keys();
}

const UdrSchema * UdrSchemaRegistry::getSchemaById(const UdrSchemaId &id) const {
    QMutexLocker lock(&mutex);
    return schemas.value(id, NULL);
}

bool UdrSchemaRegistry::isCorrectName(const QByteArray &name) {
    QRegExp regExp("([A-z]|_)([A-z]|_|\\d)*");
    return regExp.exactMatch(name);
}

} // U2
